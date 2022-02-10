// KernelPub.cpp: implementation of the CKernelPub class.
//
//////////////////////////////////////////////////////////////////////

#include "KernelPub.h"
#include "BF_Date_Time.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CKernelPub::CKernelPub()
{

}

CKernelPub::~CKernelPub()
{

}

bool CKernelPub::ProcFundFlow(S_MDB_TXACC_LOCK *acclock,std::vector<S_MDB_FUND_FLOW *> &flowlist)
{
	int i;
	char fundflag;
	char sFundCode[4];
	if (flowlist.size()<1)
	{
		return true;
	}
	double amt;
	for (i=0; i< flowlist.size(); i++)
	{
		amt = flowlist[i]->dAmt;
		S_MDB_CUST_FUND *fund = m_pMemDb->GetFund(acclock,flowlist[i]->sCurrency);
		if (fund == NULL)
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"资金表无此资金账号[%s %s]",acclock->sTxAcc,flowlist[i]->sCurrency);
			return false;
		}
		//判断出入金方向
		if (amt > MAXDOUBLECOMPARE)//入金
		{
			fundflag = FUND_TYPE_INOUT;
			strcpy(sFundCode,FUND_CODE_IN);//入金
			if (!UpdateFund(acclock,fund,fundflag,sFundCode,amt,true))
			{
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"资金账号[%s] 更新资金出错",acclock->sTxAcc);
				return false;
			}
		}
		else if (amt<MINDOUBLECOMPARE) 
		{
			fundflag = FUND_TYPE_INOUT;
			strcpy(sFundCode,FUND_CODE_OUT);//出金
			if (!UpdateFund(acclock,fund,fundflag,sFundCode,amt,true))
			{
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"资金账号[%s] 更新资金出错 ",acclock->sTxAcc);
				return false;
			}
		}
	}
	return true;
}

bool CKernelPub::ProcOrder(S_MDB_TXACC_LOCK *acclock,std::vector<S_MDB_ORDER_INFO *> &orderlist)
{
	int i;
	if (orderlist.size()<1)
	{
		return true;
	}
	S_MDB_CUST_FUND *fund = NULL;
	S_MDB_CONTRACT_INFO *pc= NULL;
	for (i=0; i<orderlist.size(); i++)
	{
		pc = m_pMemDb->m_pDbBaseinfo.m_pContract.Select(orderlist[i]->nContractNo);
		if (pc == NULL)
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"资金账号[%s] 报单[%s] 的合约编号[%d]在合约表不存在",orderlist[i]->sTxAcc,orderlist[i]->sLocalSerial,orderlist[i]->nContractNo);
			return false;
		}
		fund = m_pMemDb->GetFund(acclock,pc->sCurrency);
		if (fund == NULL)
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"资金表无此资金账号[%s %s]",acclock->sTxAcc,pc->sCurrency);
			return false;
		}
		if (!ComputeOneOrder(acclock,fund,orderlist[i],pc))
		{
			//sprintf(errmsg,"计算报单[%s %d %s ] 出错 %s",donelist[i]->sTxAcc,orderlist[i]->nContractNo,orderlist[i]->sDoneNo,errmsg);
			return false;
		}
	}
	return true;
}

bool CKernelPub::ProcDone(S_MDB_TXACC_LOCK *acclock,std::vector<S_MDB_DONE_INFO *> &donelist)
{
	//要一笔笔成交来，计算均价，盈亏，更新持仓表，资金表
	int i;
	if (donelist.size()<1)
	{
		return true;
	}
	S_MDB_CUST_FUND *fund = NULL;
	S_MDB_CONTRACT_INFO *pc= NULL;
	for (i=0; i<donelist.size(); i++)
	{
		pc = m_pMemDb->m_pDbBaseinfo.m_pContract.Select(donelist[i]->nContractNo);
		if (pc == NULL)
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"资金账号[%s] 成交报单号[%s] 成交号[%s] 的合约编号[%d]在合约表不存在",\
				donelist[i]->sTxAcc,donelist[i]->sLocalNo,donelist[i]->sDoneNo,donelist[i]->nContractNo);
			return false;
		}
		fund = m_pMemDb->GetFund(acclock,pc->sCurrency);
		if (fund == NULL)
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"资金表无此资金账号[%s %s]",acclock->sTxAcc,pc->sCurrency);
			return false;
		}
		if (!ComputeOneDone(acclock,fund,donelist[i],pc))
		{
			//sprintf(errmsg,"计算成交[%s %d %s ] 出错 %s",donelist[i]->sTxAcc,donelist[i]->nContractNo,donelist[i]->sDoneNo,errmsg);
			return false;
		}
	}
	return true;
}
bool CKernelPub::ProcPosition(S_MDB_TXACC_LOCK *acclock,std::vector<S_MDB_CUST_POSITION *> &posilist)
{
	//首先判断持仓是否是大边
	int ntmp=0;
	int rid;
	S_MDB_BIG_MARGIN info;
	bzero(&info,sizeof(S_MDB_BIG_MARGIN));
	strcpy(info.sTxAcc,acclock->sTxAcc);

	
	//如果是柜台导入进来的，没有资金明细，这里也不用更新资金明细，可能显示资金明细时，为负数
	S_MDB_CUST_FUND_DETAIL sfd;
	bzero(&sfd,sizeof(S_MDB_CUST_FUND_DETAIL));
	S_MDB_CUST_FUND_DETAIL *psfd = NULL;

	S_MDB_CUST_FUNDACC_INFO *accinfo = m_pMemDb->m_pDbCust.m_pCustFundAcc.At(acclock->nFundAccRowid);
	if (accinfo == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"资金账户锁表的资金账户[%s] rowid[%d]在资金账户表找不到",info.sTxAcc,acclock->nFundAccRowid);
		return false;
	}
	bool isWriteDetail=true;
	S_MDB_BROKERAGE_INFO *pbk = m_pMemDb->m_pDbSet.m_pBrokerage.Select(accinfo->nSysId);
	if (pbk == NULL)
	{
		isWriteDetail=false;//找不到对应的配置，先不写
	}
	else
	{
		if (pbk->nClearFlag != 0)
		{
			//说明不是导入的，不用写明细
			isWriteDetail = false;
			
		}
	}
	
	int nMarginMode = 0;
	S_MDB_BIG_MARGIN *pbm=NULL;
	S_MDB_VARIETY_INFO *pv=NULL;
	S_MDB_CUST_POSITION *pPosi=NULL;
	for (int i=0 ; i< posilist.size(); i++)
	{
		pPosi = posilist[i];
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"持仓 [%d %s] [%d %d] 品种[%d]",pPosi->nContractNo,pPosi->sContractCode,pPosi->nCurPB,pPosi->nCurPS,pPosi->nVarietyId);
		//判断是否是期货
		S_MDB_CONTRACT_INFO *pc = m_pMemDb->m_pDbBaseinfo.m_pContract.Select(posilist[i]->nContractNo);
		if (pc == NULL)
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"合约[%d %s]在合约表不存在",posilist[i]->nContractNo,posilist[i]->sContractCode);
			return false;
		}
		if (pc->sProd_type[0] != PROD_TYPE_FUTURES )
		{
			//不是期货
			continue;
		}
		nMarginMode = GetMarginMode(pc);
		if (nMarginMode<0)
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"合约[%d %s] 取保证金模式出错 ",pc->nContractId,pc->sContractCode);
			return false;
		}
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"合约[%d %s] 保证金模式[%d] ",pc->nContractId,pc->sContractCode,nMarginMode);
		if (nMarginMode != 1 && nMarginMode !=2) 
		{
			if (!isWriteDetail)
			{
				continue;
			}
			//写不参与大边的明细
			if (posilist[i]->nCurPB>0)
			{
				psfd = m_pMemDb->m_pDbTrader.m_pFundDetail.Select((char *)info.sTxAcc,pc->sCurrency,FUND_TYPE_MARGIN,(char *)FUND_CODE_MARGIN_POSI_BUY);
				if (psfd == NULL)
				{
					strcpy(sfd.sTxAcc,info.sTxAcc);
					sfd.sFundType[0] = FUND_TYPE_MARGIN;
					strcpy(sfd.sFundCode,FUND_CODE_MARGIN_POSI_BUY);
					sfd.dFundValue = posilist[i]->dPB_Margin;
					strcpy(sfd.sCurrency,pc->sCurrency);
					if (m_pMemDb->m_pDbTrader.m_pFundDetail.Insert(sfd)<0)
					{
						m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"写资金详细表失败 [%s %s %s %s]",sfd.sTxAcc,sfd.sCurrency,sfd.sFundType,sfd.sFundCode);
						return false;
					}
				}
				else
				{
					psfd->dFundValue+=posilist[i]->dPB_Margin;
				}
			}
			else
			{
				psfd = m_pMemDb->m_pDbTrader.m_pFundDetail.Select((char *)info.sTxAcc,pc->sCurrency,FUND_TYPE_MARGIN,(char *)FUND_CODE_MARGIN_POSI_SELL);
				if (psfd == NULL)
				{
					strcpy(sfd.sTxAcc,info.sTxAcc);
					sfd.sFundType[0] = FUND_TYPE_MARGIN;
					strcpy(sfd.sFundCode,FUND_CODE_MARGIN_POSI_SELL);
					sfd.dFundValue = posilist[i]->dPS_Margin;
					strcpy(sfd.sCurrency,pc->sCurrency);
					if (m_pMemDb->m_pDbTrader.m_pFundDetail.Insert(sfd)<0)
					{
						m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"写资金详细表失败 [%s %s %s %s]",sfd.sTxAcc,sfd.sCurrency,sfd.sFundType,sfd.sFundCode);
						return false;
					}
				}
				else
				{
					psfd->dFundValue+=posilist[i]->dPS_Margin;
				}
			}
			continue;
		}
		//是大边或净边
		pbm = m_pMemDb->m_pDbTrader.m_pBigNetMargin.Select(posilist[i]->sTxAcc,posilist[i]->nVarietyId);
		if (pbm == NULL)
		{
			//新增大边净边
			bzero(&info,sizeof(S_MDB_BIG_MARGIN));
			strcpy(info.sTxAcc,acclock->sTxAcc);
			info.dBuyMargin = posilist[i]->dPB_Margin;
			info.dSellMargin = posilist[i]->dPS_Margin;
			info.nBuyPosi = posilist[i]->nCurPB;
			info.nSellPosi = posilist[i]->nCurPS;
			info.nVarietyId = posilist[i]->nVarietyId;
			info.sBailMode = 48+nMarginMode;
			info.nFrozeFlag = 1;
			if (nMarginMode == 1) //净边
			{
				if (info.nBuyPosi >= info.nSellPosi)
				{
					info.nMarginFlag = 1;
					if (isWriteDetail)
					{
						psfd = m_pMemDb->m_pDbTrader.m_pFundDetail.Select((char *)info.sTxAcc,pc->sCurrency,FUND_TYPE_MARGIN,(char *)FUND_CODE_MARGIN_POSI_BUY);
						if (psfd == NULL)
						{
							strcpy(sfd.sTxAcc,info.sTxAcc);
							sfd.sFundType[0] = FUND_TYPE_MARGIN;
							strcpy(sfd.sFundCode,FUND_CODE_MARGIN_POSI_BUY);
							sfd.dFundValue = posilist[i]->dPB_Margin*(info.nBuyPosi-info.nSellPosi)/info.nBuyPosi;
							strcpy(sfd.sCurrency,pc->sCurrency);
							if (m_pMemDb->m_pDbTrader.m_pFundDetail.Insert(sfd)<0)
							{
								m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"写资金详细表失败 [%s %s %s %s]",sfd.sTxAcc,sfd.sCurrency,sfd.sFundType,sfd.sFundCode);
								return false;
							}
						}
						else
						{
							psfd->dFundValue = psfd->dFundValue+ posilist[i]->dPB_Margin*(info.nBuyPosi-info.nSellPosi)/info.nBuyPosi;
						}
					}
					
				}
				else
				{
					info.nMarginFlag = 2;
					if (isWriteDetail)
					{
						psfd = m_pMemDb->m_pDbTrader.m_pFundDetail.Select((char *)info.sTxAcc,pc->sCurrency,FUND_TYPE_MARGIN, (char*)FUND_CODE_MARGIN_POSI_SELL);
						if (psfd == NULL)
						{
							strcpy(sfd.sTxAcc,info.sTxAcc);
							sfd.sFundType[0] = FUND_TYPE_MARGIN;
							strcpy(sfd.sFundCode,FUND_CODE_MARGIN_POSI_SELL);
							sfd.dFundValue = posilist[i]->dPS_Margin*(info.nSellPosi-info.nBuyPosi)/info.nSellPosi;
							strcpy(sfd.sCurrency,pc->sCurrency);
							if (m_pMemDb->m_pDbTrader.m_pFundDetail.Insert(sfd)<0)
							{
								m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"写资金详细表失败 [%s %s %s %s]",sfd.sTxAcc,sfd.sCurrency,sfd.sFundType,sfd.sFundCode);
								return false;
							}
						}
						else
						{
							psfd->dFundValue = psfd->dFundValue+ posilist[i]->dPS_Margin*(info.nSellPosi-info.nBuyPosi)/info.nSellPosi;
						}
					}
				}
				
			}
			else //大边
			{
				if (info.dBuyMargin - info.dSellMargin>MINDOUBLECOMPARE)
				{
					info.nMarginFlag = 1;
					if (isWriteDetail)
					{
						psfd = m_pMemDb->m_pDbTrader.m_pFundDetail.Select((char *)info.sTxAcc,pc->sCurrency,FUND_TYPE_MARGIN, (char*)FUND_CODE_MARGIN_POSI_BUY);
						if (psfd == NULL)
						{
							strcpy(sfd.sTxAcc,info.sTxAcc);
							sfd.sFundType[0] = FUND_TYPE_MARGIN;
							strcpy(sfd.sFundCode,FUND_CODE_MARGIN_POSI_BUY);
							sfd.dFundValue = posilist[i]->dPB_Margin;
							strcpy(sfd.sCurrency,pc->sCurrency);
							if (m_pMemDb->m_pDbTrader.m_pFundDetail.Insert(sfd)<0)
							{
								m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"写资金详细表失败 [%s %s %s %s]",sfd.sTxAcc,sfd.sCurrency,sfd.sFundType,sfd.sFundCode);
								return false;
							}
						}
						else
						{
							psfd->dFundValue+=posilist[i]->dPB_Margin;
						}
					}
				}
				else
				{
					info.nMarginFlag = 2;
					if (isWriteDetail)
					{
						psfd = m_pMemDb->m_pDbTrader.m_pFundDetail.Select((char *)info.sTxAcc,pc->sCurrency,FUND_TYPE_MARGIN, (char*)FUND_CODE_MARGIN_POSI_SELL);
						if (psfd == NULL)
						{
							strcpy(sfd.sTxAcc,info.sTxAcc);
							sfd.sFundType[0] = FUND_TYPE_MARGIN;
							strcpy(sfd.sFundCode,FUND_CODE_MARGIN_POSI_SELL);
							sfd.dFundValue = posilist[i]->dPS_Margin;
							strcpy(sfd.sCurrency,pc->sCurrency);
							if (m_pMemDb->m_pDbTrader.m_pFundDetail.Insert(sfd)<0)
							{
								m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"写资金详细表失败 [%s %s %s %s]",sfd.sTxAcc,sfd.sCurrency,sfd.sFundType,sfd.sFundCode);
								return false;
							}
						}
						else
						{
							psfd->dFundValue+=posilist[i]->dPS_Margin;
						}
					}
				}
				
			}
			rid = m_pMemDb->m_pDbTrader.m_pBigNetMargin.Insert(info);
			if (rid<0)
			{
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"写大边净边记录表失败 [%s %d]",info.sTxAcc,info.nVarietyId);
				return false;
			}
			pbm = m_pMemDb->m_pDbTrader.m_pBigNetMargin.At(rid);
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"资金账号[%s]品种[%d] 大边净边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 冻结[%lf]",\
				pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,\
				pbm->dSellFroze,pbm->dFrozeAmt);
		}
		else //更新大边净边
		{
			//要更新资金详细
			if (isWriteDetail)
			{
				//先解冻
				if (nMarginMode == 1) //净边
				{

				}

			}
			pbm->dBuyMargin = posilist[i]->dPB_Margin;
			pbm->dSellMargin = posilist[i]->dPS_Margin;
			pbm->nBuyPosi = posilist[i]->nCurPB;
			pbm->nSellPosi = posilist[i]->nCurPS;
			if (nMarginMode == 1) //净边
			{
				if (pbm->nBuyPosi >= pbm->nSellPosi)
				{
					pbm->nMarginFlag = 1;
				}
				else
				{
					pbm->nMarginFlag = 2;
				}
			}
			else
			{
				if (pbm->dBuyMargin - pbm->dSellMargin>MINDOUBLECOMPARE)
				{
					pbm->nMarginFlag = 1;
				}
				else
				{
					pbm->nMarginFlag = 2;
				}
			}
		}
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"资金账号[%s]品种[%d] 大边净边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 冻结[%lf]",\
			pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,\
			pbm->dSellFroze,pbm->dFrozeAmt);
		
	}
	return true;
}

bool CKernelPub::ComputeOneDone(S_MDB_TXACC_LOCK *lockinfo,S_MDB_CUST_FUND *fundinfo,S_MDB_DONE_INFO *info,S_MDB_CONTRACT_INFO *pc)
{
	int nBailMode=0;
	int rid;
	S_MDB_CUST_POSITION *pPosi=NULL;
	S_MDB_BIG_MARGIN *pbm=NULL;
	S_MDB_BIG_MARGIN bm;
	double dDiffMargin=0.0;
	bzero(&bm,sizeof(S_MDB_BIG_MARGIN));
	//不用去解冻结(报单冻结的) 要判断是否大边
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"处理成交  账户[%s]合约编号[%d %s]  开平[%s] 买卖[%s] 投保[%s] 本地报单号[%s] 解冻结[%lf %lf] 成交量[%d]",\
		info->sTxAcc,info->nContractNo,info->sContractCode,info->sEoFlag,info->sBsFlag,info->sShFlag,info->sLocalNo,info->dUnfrozeAmt,info->dUnfrozeFee,info->nDoneNum);
	switch (pc->sProd_type[0] )
	{
		case PROD_TYPE_FUTURES:
			//取保证金模式
			nBailMode = GetMarginMode(pc);
			if (nBailMode <0)
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"保证金模式不符");
				return false;
			}
			if (nBailMode  == 2) //大边
			{
				if (info->sEoFlag[0] == EO_FLAG_OPEN) //开仓
				{
					if (info->sBsFlag[0] == BS_FLAG_BUY) //开多
					{
						//查找表
						pbm = m_pMemDb->m_pDbTrader.m_pBigNetMargin.Select(info->sTxAcc,pc->nProdId);
						if (pbm == NULL)//没有新增
						{
							strcpy(bm.sTxAcc,info->sTxAcc);
							bm.nBuyPosi = info->nDoneNum;
							bm.dBuyMargin = info->dExMargin+info->dMemberMargin;
							bm.nMarginFlag = 1;
							bm.sBailMode= BAIL_MODE_LARGE;
							bm.nVarietyId = pc->nProdId;
							rid =  m_pMemDb->m_pDbTrader.m_pBigNetMargin.Insert(bm);
							if (rid<0)
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"写品种大边净边表出错 资金账号[%s]品种[%d]",info->sTxAcc,pc->nProdId);
								return false;
							}
							pbm  = m_pMemDb->m_pDbTrader.m_pBigNetMargin.At(rid);
							m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"开多仓 资金账号[%s]品种[%d] 大边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 冻结[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
							//更新资金
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_BUY,info->dExMargin+info->dMemberMargin,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
								return false;
							}
						}
						else
						{
							m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"开多仓 资金账号[%s]品种[%d] 大边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 冻结[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
							pbm->dBuyMargin = pbm->dBuyMargin+info->dExMargin+info->dMemberMargin;
							pbm->nBuyPosi+=info->nDoneNum;//买持量增加
							if (pbm->nMarginFlag == 1) //大边在多 方向相同
							{
								
								//更新资金
								if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_BUY,info->dExMargin+info->dMemberMargin,true))
								{
									m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
									return false;
								}
							}
							else //大边在空 方向相反
							{
								if (pbm->dBuyMargin >pbm->dSellMargin+MAXDOUBLECOMPARE ) //大边在多了
								{
									pbm->nMarginFlag = 1;
									//换了个方向，大边在多 先去掉原方向的值
									if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_SELL,0.00-pbm->dSellMargin,true))
									{
										m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
										return false;
									}
									//再增加买方的值
									if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_BUY,pbm->dBuyMargin,true))
									{
										m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
										return false;
									}
								}
								else
								{
									//方向不变，不用动
								}
							}
							
						}
						//更新手续费
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FEE,FUND_CODE_FEE_EXCNANGE,info->dExFee+info->dMemberFee,true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
							return false;
						}
						m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"开多仓 资金账号[%s]品种[%d] 大边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 冻结[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
					}
					else //开空
					{
						//查找表
						pbm = m_pMemDb->m_pDbTrader.m_pBigNetMargin.Select(info->sTxAcc,pc->nProdId);
						if (pbm == NULL)//没有新增
						{
							//大边在空
							strcpy(bm.sTxAcc,info->sTxAcc);
							bm.nSellPosi = info->nDoneNum;
							bm.dSellMargin = info->dExMargin+info->dMemberMargin;
							bm.nMarginFlag = 2;
							bm.sBailMode= BAIL_MODE_LARGE;
							bm.nVarietyId = pc->nProdId;
							rid =  m_pMemDb->m_pDbTrader.m_pBigNetMargin.Insert(bm);
							if (rid<0)
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"写品种大边净边表出错 资金账号[%s]品种[%d]",info->sTxAcc,pc->nProdId);
								return false;
							}
							pbm  = m_pMemDb->m_pDbTrader.m_pBigNetMargin.At(rid);
							m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"开空仓 资金账号[%s]品种[%d] 大边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 冻结[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
							//更新资金
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_SELL,info->dExMargin+info->dMemberMargin,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
								return false;
							}
						}
						else
						{
							m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"开空仓 资金账号[%s]品种[%d] 大边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 冻结[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
							pbm->dSellMargin = pbm->dSellMargin+info->dExMargin+info->dMemberMargin;
							pbm->nSellPosi+=info->nDoneNum;//卖持量增加
							if (pbm->nMarginFlag == 2) //大边在空 方向相同
							{
								//更新资金
								if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_SELL,info->dExMargin+info->dMemberMargin,true))
								{
									m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
									return false;
								}
							}
							else //大边在多 方向相反
							{
								if (pbm->dBuyMargin >pbm->dSellMargin+MAXDOUBLECOMPARE ) //大边还在多
								{
									//不用动
								}
								else
								{
									pbm->nMarginFlag = 2;
									//换了个方向，大边在空 先去掉原方向的值
									if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_BUY,0.00-pbm->dBuyMargin,true))
									{
										m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
										return false;
									}
									//再增加买方的值
									if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_SELL,pbm->dSellMargin,true))
									{
										m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
										return false;
									}
								}
							}

						}
						m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"开空仓 资金账号[%s]品种[%d] 大边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 冻结[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FEE,FUND_CODE_FEE_EXCNANGE,info->dExFee+info->dMemberFee,true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
							return false;
						}

					}
				}
				else //平仓  
				{
					//取持仓信息
					pPosi = m_pMemDb->m_pDbTrader.m_pPosition.Select(info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo);
					if (pPosi == NULL)
					{
						m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"无持仓 资金账号[%s] 席位[%s] 投保标志[%s] 合约[%d %s] 数量[%d] 平仓  跳过此成交",\
							info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo,info->sContractCode,info->nDoneNum);
						return true;
					}
					double dTmp=0.0;
					//计算持仓
					if (info->sBsFlag[0] == BS_FLAG_BUY) //买平  平空
					{
						//查找表
						pbm = m_pMemDb->m_pDbTrader.m_pBigNetMargin.Select(info->sTxAcc,pc->nProdId);
						if (pbm == NULL)//没有肯定是出错了
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"大边平仓时找不到大边净边信息 账户[%s] 品种[%d]",info->sTxAcc,pc->nProdId);
							return false;
						}
						m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"平空仓 资金账号[%s]品种[%d] 大边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 冻结[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
						//释放原来的
						if (pbm->nMarginFlag == 1) //大边在多
						{
							//释放资金
							dDiffMargin = pbm->dBuyMargin;
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_BUY,0.0-pbm->dBuyMargin,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
								return false;
							}
						}
						else //大边在空 
						{
							dDiffMargin = pbm->dSellMargin;
							//释放资金
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_SELL,0.0-pbm->dSellMargin,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
								return false;
							}
						}
						//空仓减少 空仓保证金减少
						pbm->nSellPosi = pbm->nSellPosi-info->nDoneNum;
						pbm->dSellMargin = pbm->dSellMargin - pPosi->dPS_Margin*(1.00*info->nDoneNum/pPosi->nCurPS);
						if (pbm->dBuyMargin >pbm->dSellMargin+MAXDOUBLECOMPARE ) //大边在多
						{
							pbm->nMarginFlag = 1;
							//更新买方的值
							dDiffMargin = pbm->dBuyMargin-dDiffMargin;
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_BUY,pbm->dBuyMargin,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
								return false;
							}
						}
						else
						{
							pbm->nMarginFlag = 2;
							dDiffMargin = pbm->dSellMargin-dDiffMargin;
							//更新买方的值
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_SELL,pbm->dSellMargin,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
								return false;
							}
						}
						m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"平空仓 资金账号[%s]品种[%d] 大边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 冻结[%lf] 占用保证金差额[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt,dDiffMargin);
						//重新计算冻结	 20210422  平仓买卖冻结不变化，但因为平掉后保证金减少，方向可能会发生变化
						//释放原来的
						if (pbm->nFrozeFlag == 1) //大边在多
						{
							//释放资金
							if (!UpdateFund(lockinfo, fundinfo, FUND_TYPE_FROZE, FUND_CODE_BUYFROZEMARGIN, 0.0 - pbm->dFrozeAmt, true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "UpdateFund出错");
								return false;
							}
						}
						else //大边在空 
						{
							//释放资金
							if (!UpdateFund(lockinfo, fundinfo, FUND_TYPE_FROZE, FUND_CODE_SELLFROZEMARGIN, 0.0 - pbm->dFrozeAmt, true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "UpdateFund出错");
								return false;
							}
						}
					    dTmp = pbm->dBuyMargin + pbm->dBuyFroze - pbm->dSellMargin - pbm->dSellFroze;
						if (dTmp > MINDOUBLECOMPARE) //大于等于0
						{
							pbm->nFrozeFlag = 1; //冻结的在多
							if (pbm->nMarginFlag == 1) //持有的在多
							{
								//pbm->dFrozeAmt  = pbm->dBuyMargin+pbm->dBuyFroze-pbm->dBuyMargin;
								pbm->dFrozeAmt = pbm->dBuyFroze;//同上面一样，少了运算
							}
							else //持有的在空
							{
								//现有(冻结+持有) 减去占有，即为该再去冻结的
								pbm->dFrozeAmt = pbm->dBuyMargin + pbm->dBuyFroze - pbm->dSellMargin;
							}
							//进行新的冻结
							if (!UpdateFund(lockinfo, fundinfo, FUND_TYPE_FROZE, FUND_CODE_BUYFROZEMARGIN, pbm->dFrozeAmt, true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "UpdateFund出错");
								return false;
							}
						}
						else
						{
							pbm->nFrozeFlag = 2;
							if (pbm->nMarginFlag == 1) //持有的在多
							{
								pbm->dFrozeAmt = pbm->dSellMargin + pbm->dSellFroze - pbm->dBuyMargin;
							}
							else //持有的在空
							{
								//现有(冻结+持有) 减去占有，即为该再去冻结的
								//pbm->dFrozeAmt  = pbm->dSellMargin+pbm->dSellFroze-pbm->dSellMargin;
								pbm->dFrozeAmt = pbm->dSellFroze;//同上面一样，少了运算
							}
							//进行新的冻结
							if (!UpdateFund(lockinfo, fundinfo, FUND_TYPE_FROZE, FUND_CODE_SELLFROZEMARGIN, pbm->dFrozeAmt, true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "UpdateFund出错");
								return false;
							}
						}
						m_pLog->LogMp(LOG_DEBUG, __FILE__, __LINE__, "平空仓 资金账号[%s]品种[%d] 大边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 冻结[%lf] 保证金差额[%lf]", pbm->sTxAcc, pbm->nVarietyId, pbm->nMarginFlag, pbm->dBuyMargin, pbm->dSellMargin, pbm->nFrozeFlag, pbm->dBuyFroze, pbm->dSellFroze, pbm->dFrozeAmt, dTmp);
						//重新计算冻结	 20210422 END
						//费用
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FEE,FUND_CODE_FEE_EXCNANGE,info->dExFee+info->dMemberFee,true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
							return false;
						}
					}
					else //卖平 平多
					{
						//查找表
						pbm = m_pMemDb->m_pDbTrader.m_pBigNetMargin.Select(info->sTxAcc,pc->nProdId);
						if (pbm == NULL)//没有肯定是出错了
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"大边平仓时找不到大边净边信息 账户[%s] 品种[%d]",info->sTxAcc,pc->nProdId);
							return false;
						}
						m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"平多仓 资金账号[%s]品种[%d] 大边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 冻结[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
						
						//先释放原来的
						if (pbm->nMarginFlag == 1) //大边在多 方向相同
						{
							dDiffMargin = pbm->dBuyMargin;
							//释放资金
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_BUY,0.00-pbm->dBuyMargin,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
								return false;
							}
						}
						else //大边在空 方向相反
						{
							dDiffMargin = pbm->dSellMargin;
							//释放资金
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_SELL,0.00-pbm->dSellMargin,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
								return false;
							}
						}
						//重新计算持有保证金 //多仓减少 多仓保证金减少
						pbm->nBuyPosi = pbm->nBuyPosi-info->nDoneNum;
						pbm->dBuyMargin = pbm->dBuyMargin - pPosi->dPB_Margin*(1.00*info->nDoneNum/pPosi->nCurPB);
						if (pbm->dBuyMargin >pbm->dSellMargin+MAXDOUBLECOMPARE ) //大边在多
						{
							pbm->nMarginFlag = 1;
							dDiffMargin = pbm->dBuyMargin-dDiffMargin;
							//更新买方的值
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_BUY,pbm->dBuyMargin,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
								return false;
							}
						}
						else
						{
							pbm->nMarginFlag = 2;
							dDiffMargin = pbm->dSellMargin-dDiffMargin;
							//更新买方的值
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_SELL,pbm->dSellMargin,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
								return false;
							}
						}
						//重新计算冻结	 20210422  平仓买卖冻结不变化，但因为平掉后保证金减少，方向可能会发生变化
						//释放原来的
						if (pbm->nFrozeFlag == 1) //大边在多
						{
							//释放资金
							if (!UpdateFund(lockinfo, fundinfo, FUND_TYPE_FROZE, FUND_CODE_BUYFROZEMARGIN, 0.0 - pbm->dFrozeAmt, true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "UpdateFund出错");
								return false;
							}
						}
						else //大边在空 
						{
							//释放资金
							if (!UpdateFund(lockinfo, fundinfo, FUND_TYPE_FROZE, FUND_CODE_SELLFROZEMARGIN, 0.0 - pbm->dFrozeAmt, true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "UpdateFund出错");
								return false;
							}
						}
						dTmp = pbm->dBuyMargin + pbm->dBuyFroze - pbm->dSellMargin - pbm->dSellFroze;
						if (dTmp > MINDOUBLECOMPARE) //大于等于0
						{
							pbm->nFrozeFlag = 1; //冻结的在多
							if (pbm->nMarginFlag == 1) //持有的在多
							{
								//pbm->dFrozeAmt  = pbm->dBuyMargin+pbm->dBuyFroze-pbm->dBuyMargin;
								pbm->dFrozeAmt = pbm->dBuyFroze;//同上面一样，少了运算
							}
							else //持有的在空
							{
								//现有(冻结+持有) 减去占有，即为该再去冻结的
								pbm->dFrozeAmt = pbm->dBuyMargin + pbm->dBuyFroze - pbm->dSellMargin;
							}
							//进行新的冻结
							if (!UpdateFund(lockinfo, fundinfo, FUND_TYPE_FROZE, FUND_CODE_BUYFROZEMARGIN, pbm->dFrozeAmt, true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "UpdateFund出错");
								return false;
							}
						}
						else
						{
							pbm->nFrozeFlag = 2;
							if (pbm->nMarginFlag == 1) //持有的在多
							{
								pbm->dFrozeAmt = pbm->dSellMargin + pbm->dSellFroze - pbm->dBuyMargin;
							}
							else //持有的在空
							{
								//现有(冻结+持有) 减去占有，即为该再去冻结的
								//pbm->dFrozeAmt  = pbm->dSellMargin+pbm->dSellFroze-pbm->dSellMargin;
								pbm->dFrozeAmt = pbm->dSellFroze;//同上面一样，少了运算
							}
							//进行新的冻结
							if (!UpdateFund(lockinfo, fundinfo, FUND_TYPE_FROZE, FUND_CODE_SELLFROZEMARGIN, pbm->dFrozeAmt, true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "UpdateFund出错");
								return false;
							}
						}
						m_pLog->LogMp(LOG_DEBUG, __FILE__, __LINE__, "平空仓 资金账号[%s]品种[%d] 大边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 冻结[%lf] 保证金差额[%lf]", pbm->sTxAcc, pbm->nVarietyId, pbm->nMarginFlag, pbm->dBuyMargin, pbm->dSellMargin, pbm->nFrozeFlag, pbm->dBuyFroze, pbm->dSellFroze, pbm->dFrozeAmt, dTmp);
						//重新计算冻结	 20210422 END
						m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"平多仓 资金账号[%s]品种[%d] 大边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 冻结[%lf] 占用保证金差额[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt,dDiffMargin);
						//费用
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FEE,FUND_CODE_FEE_EXCNANGE,info->dExFee+info->dMemberFee,true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
							return false;
						}
					}
				}
			}
			else if (nBailMode == 1) //净边
			{
				if (ProcDoneNetMargin(lockinfo,fundinfo,info,NULL,pc,true))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"处理净边初始成交出错");
					return false;
				}
			}
			else  //双边
			{
				if (info->sEoFlag[0] == EO_FLAG_OPEN) //开仓
				{
					if (info->sBsFlag[0] == BS_FLAG_BUY)
					{
						//更新资金
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_BUY,info->dExMargin+info->dMemberMargin,true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
							return false;
						}
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FEE,FUND_CODE_FEE_EXCNANGE,info->dExFee+info->dMemberFee,true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
							return false;
						}

					}
					else
					{
						//更新资金	
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_SELL,info->dExMargin+info->dMemberMargin,true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
							return false;
						}
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FEE,FUND_CODE_FEE_EXCNANGE,info->dExFee+info->dMemberFee,true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
							return false;
						}
					}
				}
				else //平仓
				{
					//取持仓信息
					pPosi = m_pMemDb->m_pDbTrader.m_pPosition.Select(info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo);
					if (pPosi == NULL)
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"无持仓 资金账号[%s] 席位[%s] 投保标志[%s] 合约[%d %s] 数量[%d] 平仓",\
							info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo,info->sContractCode,info->nDoneNum);
						return false;

					}
					//计算持仓
					if (info->sBsFlag[0] == BS_FLAG_BUY) //买平  平空
					{
						//均价不变 保证金按数量比例减小
						//更新保证金
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_SELL,0.00-pPosi->dPS_Margin*(1.00*info->nDoneNum/pPosi->nCurPS),true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
							return false;
						}
						//费用
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FEE,FUND_CODE_FEE_EXCNANGE,info->dExFee+info->dMemberFee,true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
							return false;
						}
					}
					else //卖平 平多
					{
						//均价不变 保证金按数量比例减小
						//更新保证金
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_BUY,0.00-pPosi->dPB_Margin*(1.00*info->nDoneNum/pPosi->nCurPB),true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
							return false;
						}
						//费用
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FEE,FUND_CODE_FEE_EXCNANGE,info->dExFee+info->dMemberFee,true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
							return false;
						}
					}
				}
			}
			if (!UpdatePosition(lockinfo,fundinfo,info,pPosi,pc,true))
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdatePosition 出错 ");
			}
			break;
		case PROD_TYPE_SPOT://现货
		case PROD_TYPE_SEC://股票
			//更新持仓
			pPosi = m_pMemDb->m_pDbTrader.m_pPosition.Select(info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo);
			if (info->sBsFlag[0] == BS_FLAG_BUY)
			{
				//更新资金
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_INOUT,FUND_CODE_BUY,info->dAmt,true))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
					return false;
				}
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FEE,FUND_CODE_FEE_EXCNANGE,info->dExFee+info->dMemberFee,true))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
					return false;
				}
				if (pPosi == NULL)
				{
					S_MDB_CUST_POSITION posi;
					bzero(&posi,sizeof(S_MDB_CUST_POSITION));
					strcpy(posi.sTxAcc,info->sTxAcc);
					strcpy(posi.sSeatNo,info->sSeatNo);
					strcpy(posi.sShFlag,info->sShFlag);
					strcpy(posi.sTradeCode,info->sTxCode);
					posi.nContractNo = info->nContractNo;
					strcpy(posi.sContractCode,info->sContractCode);
					posi.dPB_Margin = 0.00-info->dAmt-info->dExFee-info->dMemberFee;
					posi.dPB_Avp = posi.dPB_Margin/info->nDoneNum;//成本均价/每手
					posi.nDayPB = info->nDoneNum;
					posi.nCurCanUsePB = info->nDoneNum;
					posi.nCurPB = info->nDoneNum;
					posi.dPB_OAvp = posi.dPB_Avp;//开仓成本均价 
					if (m_pMemDb->m_pDbTrader.m_pPosition.Insert(posi)<0)
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"写入持仓表失败 资金账号[%s] 席位[%s] 投保标志[%s] 合约[%d %s] 数量[%d] 开多",\
							info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo,info->sContractCode,info->nDoneNum);
						return false;
					}
				}
				else
				{
					//持有均价
					pPosi->dPB_Avp = (pPosi->dPB_Avp*pPosi->nCurPB-info->dAmt-info->dExFee-info->dMemberFee)/(pPosi->nCurPB+info->nDoneNum);
					pPosi->dPB_Margin = pPosi->dPB_Margin - info->dAmt-info->dExFee-info->dMemberFee;
					pPosi->nDayPB = pPosi->nDayPB+info->nDoneNum;
					pPosi->nCurCanUsePB = pPosi->nCurCanUsePB+info->nDoneNum;
					pPosi->nCurPB = pPosi->nCurPB+info->nDoneNum;
					//开仓均价
					pPosi->dPB_OAvp = (pPosi->dPB_OAvp*pPosi->nDayPB+info->dAmt+info->dExFee+info->dMemberFee)/(pPosi->nDayPB+info->nDoneNum);

				}
			}
			else  //卖出
			{
				//更新资金	
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_INOUT,FUND_CODE_SELL,info->dAmt,true))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
					return false;
				}
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FEE,FUND_CODE_FEE_EXCNANGE,info->dExFee+info->dMemberFee,true))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
					return false;
				}
				if (pPosi == NULL)
				{

					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"写入持仓表失败 资金账号[%s] 席位[%s] 投保标志[%s] 合约[%d %s] 数量[%d] 开多",\
						info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo,info->sContractCode,info->nDoneNum);
					return false;
				}
				//计算持仓均价
				pPosi->dPB_Margin = pPosi->dPB_Margin*(pPosi->nCurPB-info->nDoneNum)/pPosi->nCurPB;
				pPosi->nCurCanUsePB = pPosi->nCurCanUsePB+info->nDoneNum;
				pPosi->nCurPB = pPosi->nCurPB+info->nDoneNum;
				//开仓均价不变
				//将盈亏重新到均价上
				pPosi->dPB_Avp = (pPosi->dPB_OAvp*pPosi->nCurPB+info->dCovSurplus)/pPosi->nCurPB;
				//计算卖出盈亏
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_SURPLUS,FUND_CODE_SURPLUS_CLOSE,(info->dDonePrice-pPosi->dPB_Avp)*info->nDoneNum*pc->dTxUnit,true))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
					return false;
				}
			}
			break;
		case PROD_TYPE_OPTION:
			if (info->sEoFlag[0] == EO_FLAG_OPEN) //开仓
			{
				if (info->sBsFlag[0] == BS_FLAG_BUY)
				{
					//更新资金
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_BUY,info->dExMargin+info->dMemberMargin,true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
						return false;
					}
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FEE,FUND_CODE_FEE_EXCNANGE,info->dExFee+info->dMemberFee,true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
						return false;
					}

				}
				else
				{
					//更新资金	
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_SELL,info->dExMargin+info->dMemberMargin,true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
						return false;
					}
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FEE,FUND_CODE_FEE_EXCNANGE,info->dExFee+info->dMemberFee,true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
						return false;
					}
				}
			}
			else //平仓
			{
				//取持仓信息
				pPosi = m_pMemDb->m_pDbTrader.m_pPosition.Select(info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo);
				if (pPosi == NULL)
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"无持仓 资金账号[%s] 席位[%s] 投保标志[%s] 合约[%d %s] 数量[%d] 平仓",\
						info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo,info->sContractCode,info->nDoneNum);
					return false;

				}
				//计算持仓
				if (info->sBsFlag[0] == BS_FLAG_BUY) //买平  平空
				{
					//均价不变 保证金按数量比例减小
					//更新保证金
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_SELL,pPosi->dPS_Margin*(1.00*info->nDoneNum/pPosi->nCurPS),true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
						return false;
					}
					//费用
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FEE,FUND_CODE_FEE_EXCNANGE,info->dExFee+info->dMemberFee,true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
						return false;
					}
				}
				else //卖平 平多
				{
					//均价不变 保证金按数量比例减小
					//更新保证金
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_BUY,pPosi->dPB_Margin*(1.00*info->nDoneNum/pPosi->nCurPB),true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
						return false;
					}
					//费用
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FEE,FUND_CODE_FEE_EXCNANGE,info->dExFee+info->dMemberFee,true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
						return false;
					}
				}
			}
			if (!UpdatePosition(lockinfo,fundinfo,info,pPosi,pc,true))
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdatePosition 出错 ");
			}
			break;
		default:
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"合约类型[%s] 不符 ",pc->sProd_type);
			return false;
	}
	
	return true;
}
bool CKernelPub::UpdateFund(S_MDB_TXACC_LOCK *info,S_MDB_CUST_FUND *fund,const char fundflag,const  char *fundcode,double amt,bool isadd,bool islock)
{
	// #define  FUND_TYPE_INOUT            '1'   //收支
	// #define  FUND_TYPE_FROZE            '2'   //冻结
	// #define  FUND_TYPE_MARGIN           '3'   //保证金
	// #define  FUND_TYPE_FEE              '4'   //费用
	// #define  FUND_TYPE_RESERVE          '5'   //备付金
	// #define  FUND_TYPE_SURPLUS          '6'   //盈亏
	// #define  FUND_TYPE_FLOATSURPLUS     '7'   //浮动盈亏
	// #define  FUND_TYPE_ARAP             '8'   //应收应付
	// #define  FUND_TYPE_INCR_INTE        '9'   //利息
	
	if (amt > MINDOUBLECOMPARE && amt <MAXDOUBLECOMPARE) //金额为0
	{
		return true;
	}
	if (islock)
	{
		CBF_PMutex pp(info->mutex);
	}
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"资金账户[%s] UpdateFund前资金 结存[%lf] 保证金[%lf] 冻结[%lf] 费用[%lf] 浮动盈亏[%lf] 资金类型[%c] 资金代码[%s] 金额[%lf]",info->sTxAcc,fund->dBalance,fund->dMargin,fund->dFrozeFund,fund->dFee,fund->dFloatSurplus,fundflag,fundcode,amt);
	S_MDB_CUST_FUND_DETAIL sfd;
	sfd.sFundType[0] = fundflag;
	sfd.sFundType[1] = 0;
	double dTmp;
	S_MDB_CUST_FUND_DETAIL *psfd= m_pMemDb->m_pDbTrader.m_pFundDetail.Select((char *)info->sTxAcc,fund->sCurrency,sfd.sFundType,(char *)fundcode);
	switch (fundflag)
	{
		case FUND_TYPE_INOUT://收支款项   正为增加负为减小
			if (isadd)
			{
				fund->dIncomeExpense= fund->dIncomeExpense+amt;
				//可用可提当日结存
				fund->dBalance= fund->dBalance+amt;
				fund->dAvailFund= fund->dAvailFund+amt;
				fund->dGetFund= fund->dGetFund+amt;
			}
			else //用新值替换
			{
				dTmp = amt - fund->dIncomeExpense;
				fund->dBalance= fund->dBalance+dTmp;
				fund->dAvailFund= fund->dAvailFund+dTmp;
				fund->dGetFund= fund->dGetFund+dTmp;
				fund->dIncomeExpense = amt;
			}
			break;
		case FUND_TYPE_FROZE: //冻结  不影响结存   资产类为正数
			if (isadd)
			{
				fund->dFrozeFund = fund->dFrozeFund+amt;
				fund->dAvailFund= fund->dAvailFund-amt;
				fund->dGetFund= fund->dGetFund-amt;
			}
			else
			{
				dTmp = amt - fund->dFrozeFund;
				fund->dAvailFund= fund->dAvailFund+dTmp;
				fund->dGetFund= fund->dGetFund+dTmp;
				fund->dFrozeFund = amt;
			}
			break;
		case FUND_TYPE_MARGIN: //保证金 不影响结存  资产类为正数
			if (isadd)
			{
				fund->dMargin = fund->dMargin+amt;
				fund->dAvailFund= fund->dAvailFund-amt;
				fund->dGetFund= fund->dGetFund-amt;
			}
			else
			{
				dTmp = amt - fund->dMargin;
				fund->dAvailFund= fund->dAvailFund+dTmp;
				fund->dGetFund= fund->dGetFund+dTmp;
				fund->dMargin = amt;
			}
			break;
		case FUND_TYPE_FEE: //费用   正为增加负为减小
			if (isadd)
			{
				fund->dFee = fund->dFee+amt;
				//可用可提当日结存
				fund->dBalance= fund->dBalance+amt;
				fund->dAvailFund= fund->dAvailFund+amt;
				fund->dGetFund= fund->dGetFund+amt;
			}
			else
			{
				dTmp = amt - fund->dFee;

				//可用可提当日结存
				fund->dBalance= fund->dBalance+dTmp;
				fund->dAvailFund= fund->dAvailFund+dTmp;
				fund->dGetFund= fund->dGetFund+dTmp;
				fund->dFee = amt;
			}
			break;
		case FUND_TYPE_RESERVE: //备付金 不影响结存  资产类为正数
			if (isadd)
			{
				fund->dReserve = fund->dReserve+amt;
				fund->dAvailFund= fund->dAvailFund-amt;
				fund->dGetFund= fund->dGetFund-amt;
			}
			else
			{
				dTmp = amt - fund->dReserve;

				fund->dAvailFund= fund->dAvailFund+dTmp;
				fund->dGetFund= fund->dGetFund+dTmp;
				fund->dReserve = amt;
			}

			break;
		case FUND_TYPE_SURPLUS: //盈亏   正为增加负为减小
			if (isadd)
			{
				fund->dSurplus = fund->dSurplus+amt;
				//可用可提当日结存
				fund->dBalance= fund->dBalance+amt;
				fund->dAvailFund= fund->dAvailFund+amt;
				fund->dGetFund= fund->dGetFund+amt;
			}
			else
			{
				dTmp = amt - fund->dSurplus;

				//可用可提当日结存
				fund->dBalance= fund->dBalance+dTmp;
				fund->dAvailFund= fund->dAvailFund+dTmp;
				fund->dGetFund= fund->dGetFund+dTmp;
				fund->dSurplus = amt;
			}
			break;
		case FUND_TYPE_FLOATSURPLUS: //浮动盈亏  正为增加负为减小
			if (isadd)
			{
				if (fund->dFloatSurplus < MINDOUBLECOMPARE) //原来的浮盈为负
				{
					//恢复原样，减去就是加上
					fund->dAvailFund= fund->dAvailFund -fund->dFloatSurplus;
					fund->dGetFund= fund->dGetFund-fund->dFloatSurplus;
				}
				//新的浮动盈亏
				fund->dFloatSurplus = fund->dFloatSurplus+amt;
				//当日结存要加上浮动盈亏
				fund->dBalance= fund->dBalance+amt;
				//可用可提
				if (fund->dFloatSurplus <MINDOUBLECOMPARE)
				{
					//浮动盈亏为负数，要减去
					fund->dAvailFund= fund->dAvailFund+fund->dFloatSurplus;
					fund->dGetFund= fund->dGetFund+fund->dFloatSurplus;
				}
				 //浮盈为正，不加到可用可提里面

			}
			else
			{
				if (fund->dFloatSurplus < MINDOUBLECOMPARE) //原来的浮盈为负
				{
					//恢复原样，减去就是加上
					fund->dAvailFund= fund->dAvailFund -fund->dFloatSurplus;
					fund->dGetFund= fund->dGetFund-fund->dFloatSurplus;
				}
				//当日结存要加上浮动盈亏
				fund->dBalance = fund->dBalance -fund->dFloatSurplus;
				fund->dBalance= fund->dBalance+amt;
				if (amt <MINDOUBLECOMPARE) //浮盈为正，不加到可用可提结存里面
				{
					//可用可提
					fund->dAvailFund= fund->dAvailFund+amt;
					fund->dGetFund= fund->dGetFund+amt;
				}
				fund->dFloatSurplus = amt;
			}
			break;
		case FUND_TYPE_ARAP: //应收应付   正为增加负为减小
			if (isadd)
			{
				fund->dArap = fund->dArap+amt;
				//可用可提当日结存
				fund->dBalance= fund->dBalance+amt;
				fund->dAvailFund= fund->dAvailFund+amt;
				fund->dGetFund= fund->dGetFund+amt;
			}
			else
			{
				dTmp = amt - fund->dArap;

				//可用可提当日结存
				fund->dBalance= fund->dBalance+dTmp;
				fund->dAvailFund= fund->dAvailFund+dTmp;
				fund->dGetFund= fund->dGetFund+dTmp;
				fund->dArap = amt;
			}
			break;
		case FUND_TYPE_INCR_INTE: //利息  正为增加负为减小
			if (isadd)
			{
				fund->dIncrInte = fund->dIncrInte+amt;
				//可用可提当日结存
				fund->dBalance= fund->dBalance+amt;
				fund->dAvailFund= fund->dAvailFund+amt;
				fund->dGetFund= fund->dGetFund+amt;
			}
			else
			{
				dTmp = amt - fund->dIncrInte;
				fund->dBalance= fund->dBalance+dTmp;
				fund->dAvailFund= fund->dAvailFund+dTmp;
				fund->dGetFund= fund->dGetFund+dTmp;
				fund->dIncrInte = amt;
			}
			break;
		default:
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"资金类型[%c]不符",fundflag);
			return false;
	}
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"资金账户[%s] UpdateFund后资金 结存[%lf] 保证金[%lf] 冻结[%lf] 费用[%lf] 浮动盈亏[%lf] 资金类型[%c] 资金代码[%s]",info->sTxAcc,fund->dBalance,fund->dMargin,fund->dFrozeFund,fund->dFee,fund->dFloatSurplus,fundflag,fundcode);
	//增加明细
	if (psfd == NULL) //新增
	{
		strcpy(sfd.sTxAcc,info->sTxAcc);
		strcpy(sfd.sFundCode,fundcode);
		sfd.dFundValue = amt;
		strcpy(sfd.sCurrency,fund->sCurrency);
		if (m_pMemDb->m_pDbTrader.m_pFundDetail.Insert(sfd)<0)
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"增加资金详细表失败[%s %s %s %s]",sfd.sTxAcc,sfd.sCurrency,sfd.sFundType,sfd.sFundCode);
			return false;
		}
	}
	else
	{
		if (isadd)
		{
			psfd->dFundValue = psfd->dFundValue+amt;
		}
		else
		{
			psfd->dFundValue = amt;
		}
	}

	return true;
}


int CKernelPub::GetLastTradeDate(const char *market,const char *excode,const char *curdate,int lastn)
{
	if (strlen(curdate)!=8)
	{
		return -1;
	}
	CBF_Date_Time pdate;
	pdate.Set(curdate);
	S_MDB_SPECDATE *pd = NULL;
	for (int i=0; i<lastn ;)
	{
		if (pdate.GetWeek() >0 &&  pdate.GetWeek()<6) //工作日
		{
			pd = m_pMemDb->m_pDbBaseinfo.m_pSpecDate.Select((char *)market,(char *)excode,(char *)pdate.ToStringD8().c_str());
			if (pd == NULL) //不是节假日
			{
				if (excode[0] != '$') //按市场再取一次
				{
					pd = m_pMemDb->m_pDbBaseinfo.m_pSpecDate.Select((char *)market,(char *)excode,(char *)pdate.ToStringD8().c_str());
					if (pd == NULL) //不是节假日
					{
						i++;
						pdate--;
						continue;
					}
					//按市场取到是节假日
					pdate--;
					continue;
				}
				else  //按市场取的，没有就是交易日
				{
					i++;
					pdate--;
					continue;
				}
			}
			else
			{
				pdate--;
			}
		}
		else
		{
			pdate--;
		}
	}
	return atoi(pdate.ToStringD8().c_str());
}


int CKernelPub::GetMarginMode(S_MDB_CONTRACT_INFO *pc)
{
	if (pc->sProd_type[0] != PROD_TYPE_FUTURES)
	{
		return 3;
	}
	S_MDB_VARIETY_INFO *pv = m_pMemDb->m_pDbBaseinfo.m_pVariety.Select(pc->nProdId);
	if (pv == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"合约[%d %s]的品种[%d]在品种表中不存在",pc->nContractId,pc->sContractCode,pc->nProdId);
		return -1;
	}
	if (pv->sBailMode[0] != BAIL_MODE_LARGE)
	{
		return atoi(pv->sBailMode);
	}
	//是大边模式，检查到期日
	S_MDB_EXCH_INFO *pe = m_pMemDb->m_pDbBaseinfo.m_pExchange.Select(pc->sExcode);
	if (pe == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"交易所[%s]在交易所表不存在",pc->sExcode);
		return -1;
	}
	//中金所是到期月，无大边优惠
	if (pc->sExcode[0] == '4') //中金所
	{
		CBF_Date_Time pdate;
		char curym[5];
		memcpy(curym,pdate.ToStringD8().c_str(),4);
		curym[4]=0;
		if (strncmp(curym,pc->sDueDate,4) == 0)
		{
			return 0;
		}
		return 2;
	}

	//倒数第6个交易日，盘中仍算大边
	int lastdate = GetLastTradeDate(pe->sMarket,pc->sExcode,pc->sDueDate,6);
	if (lastdate < atoi(pe->sTradeDate)) 
	{
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"合约[%d %s]到期日[%s]当前交易日[%s] 最后大边交易日[%d] 进入最后交易日前5个交易日内，按双边保证金计算",pc->nContractId,pc->sContractCode,pc->sDueDate,pe->sTradeDate,lastdate);
		return 0;
	}
	return 2;
}

bool CKernelPub::ComputeOneOrder(S_MDB_TXACC_LOCK *lockinfo,S_MDB_CUST_FUND *fundinfo,S_MDB_ORDER_INFO *info,S_MDB_CONTRACT_INFO *pc)
{
	
	S_MDB_CUST_POSITION *pPosi = NULL;
	if (info->sCancelFlag[0] == BOOL_TRUE   || info->nCancelNum !=0 )
	{ 
		//已撤单 不用处理
		return true;
	}
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"处理报单  账户[%s]合约编号[%d %s]  开平[%s] 买卖[%s] 投保[%s] 本地报单号[%s] 冻结[%lf] 委托量[%d] 剩余量[%d]",\
		info->sTxAcc,info->nContractNo,info->sContractCode,info->sEoFlag,info->sBsFlag,info->sShFlag,info->sLocalSerial,info->dFrozAmt,info->nOrderNum,info->nRemainNum);
	S_MDB_BIG_MARGIN *pbm=NULL;
	S_MDB_BIG_MARGIN bm;
	int nMarginMode=0;
	double dTmp=0.0;
	int rid;
	bzero(&bm,sizeof(S_MDB_BIG_MARGIN));
	switch (pc->sProd_type[0])
	{
		case PROD_TYPE_FUTURES://期货
			nMarginMode = GetMarginMode(pc);
			if (nMarginMode<0)
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"合约[%d %s] 取保证金模式出错 ",pc->nContractId,pc->sContractCode);
				return false;
			}
			if (nMarginMode == 2) //大边保证金
			{
				if (info->sEoFlag[0] == EO_FLAG_OPEN) //开仓
				{
					//冻资金
					if (info->sBsFlag[0] == BS_FLAG_BUY) //买开
					{
						//查找表
						pbm = m_pMemDb->m_pDbTrader.m_pBigNetMargin.Select(info->sTxAcc,pc->nProdId);
						if (pbm == NULL)//没有新增
						{
							strcpy(bm.sTxAcc,info->sTxAcc);
							bm.nMarginFlag = 1;
							bm.sBailMode= BAIL_MODE_LARGE;
							bm.nVarietyId = pc->nProdId;
							bm.dFrozeAmt = info->dFrozAmt*info->nRemainNum/(1.00*info->nOrderNum);
							bm.dBuyFroze = info->dFrozAmt;
							bm.nBuyQty = info->nRemainNum;
							bm.nFrozeFlag = 1;
							rid = m_pMemDb->m_pDbTrader.m_pBigNetMargin.Insert(bm);
							if (rid<0)
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"写品种大边净边表出错 资金账号[%s]品种[%d]",info->sTxAcc,pc->nProdId);
								return false;
							}
							pbm = m_pMemDb->m_pDbTrader.m_pBigNetMargin.At(rid);
							m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"资金账号[%s]品种[%d] 大边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 冻结[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
							//按剩余数量冻买保证金
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,pbm->dFrozeAmt,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
								return false;
							}
						}
						else //计算该冻结多少
						{
							m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"资金账号[%s]品种[%d] 大边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 冻结[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
							pbm->dBuyFroze = pbm->dBuyFroze+info->dFrozAmt*info->nRemainNum/(1.00*info->nOrderNum);
							pbm->nBuyQty = pbm->nBuyQty+info->nRemainNum;
							dTmp = pbm->dBuyMargin+pbm->dBuyFroze-pbm->dSellMargin-pbm->dSellFroze;
							if (pbm->nFrozeFlag == 1)
							{
								//将原来的释放
								if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,0.00-pbm->dFrozeAmt,true))
								{
									m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
									return false;
								}
							}
							else
							{
								//将原来的释放
								if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,0.00-pbm->dFrozeAmt,true))
								{
									m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
									return false;
								}
							}
							if (dTmp > MINDOUBLECOMPARE) //大于等于0
							{
								pbm->nFrozeFlag = 1; //冻结的在多
								if (pbm->nMarginFlag == 1) //持有的在多
								{
									//pbm->dFrozeAmt  = pbm->dBuyMargin+pbm->dBuyFroze-pbm->dBuyMargin;
									pbm->dFrozeAmt =pbm->dBuyFroze;//同上面一样，少了运算
								}
								else //持有的在空
								{
									//现有(冻结+持有) 减去占有，即为该再去冻结的
									pbm->dFrozeAmt  = pbm->dBuyMargin+pbm->dBuyFroze-pbm->dSellMargin;
								}
								//进行新的冻结
								if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,pbm->dFrozeAmt,true))
								{
									m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
									return false;
								}
							}
							else
							{
								pbm->nFrozeFlag = 2;
								if (pbm->nMarginFlag == 1) //持有的在多
								{
									pbm->dFrozeAmt  = pbm->dSellMargin+pbm->dSellFroze-pbm->dBuyMargin;
								}
								else //持有的在空
								{
									//现有(冻结+持有) 减去占有，即为该再去冻结的
									//pbm->dFrozeAmt  = pbm->dSellMargin+pbm->dSellFroze-pbm->dSellMargin;
									pbm->dFrozeAmt = pbm->dSellFroze;//同上面一样，少了运算
								}
								//进行新的冻结
								if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,pbm->dFrozeAmt,true))
								{
									m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
									return false;
								}
							}
							
						}
						m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"资金账号[%s]品种[%d] 大边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 冻结[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
						//按剩余数量冻手续费
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dExFee*info->nRemainNum/(1.00*info->nOrderNum),true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
							return false;
						}
					}
					else  //卖开
					{
						//查找表
						pbm = m_pMemDb->m_pDbTrader.m_pBigNetMargin.Select(info->sTxAcc,pc->nProdId);
						if (pbm == NULL)//没有新增
						{
							strcpy(bm.sTxAcc,info->sTxAcc);
							bm.nMarginFlag = 1;
							bm.sBailMode= BAIL_MODE_LARGE;
							bm.nVarietyId = pc->nProdId;
							bm.dFrozeAmt = info->dFrozAmt*info->nRemainNum/(1.00*info->nOrderNum);
							bm.dSellFroze = info->dFrozAmt;
							bm.nSellQty = info->nRemainNum;
							bm.nFrozeFlag = 2;
							rid = m_pMemDb->m_pDbTrader.m_pBigNetMargin.Insert(bm);
							if (rid<0)
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"写品种大边净边表出错 资金账号[%s]品种[%d]",info->sTxAcc,pc->nProdId);
								return false;
							}
							pbm = m_pMemDb->m_pDbTrader.m_pBigNetMargin.At(rid);
							m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"资金账号[%s]品种[%d] 大边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 冻结[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
							//按剩余数量冻买保证金
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,pbm->dFrozeAmt,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
								return false;
							}
						}
						else //计算该冻结多少
						{
							m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"资金账号[%s]品种[%d] 大边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 冻结[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
							pbm->dSellFroze = pbm->dSellFroze+info->dFrozAmt*info->nRemainNum/(1.00*info->nOrderNum);
							pbm->nSellQty = pbm->nSellQty+info->nRemainNum;
							dTmp = pbm->dBuyMargin+pbm->dBuyFroze-pbm->dSellMargin-pbm->dSellFroze;

							if (pbm->nFrozeFlag == 1)
							{
								//将原来的释放
								if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,0.00-pbm->dFrozeAmt,true))
								{
									m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
									return false;
								}
							}
							else
							{
								//将原来的释放
								if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,0.00-pbm->dFrozeAmt,true))
								{
									m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
									return false;
								}
							}
							if (dTmp > MINDOUBLECOMPARE) //大于等于0
							{
								pbm->nFrozeFlag = 1; //冻结的在多
								if (pbm->nMarginFlag == 1) //持有的在多
								{
									//pbm->dFrozeAmt  = pbm->dBuyMargin+pbm->dBuyFroze-pbm->dBuyMargin;
									pbm->dFrozeAmt =pbm->dBuyFroze;//同上面一样，少了运算
								}
								else //持有的在空
								{
									//现有(冻结+持有) 减去占有，即为该再去冻结的
									pbm->dFrozeAmt  = pbm->dBuyMargin+pbm->dBuyFroze-pbm->dSellMargin;
								}
								//进行新的冻结
								if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,pbm->dFrozeAmt,true))
								{
									m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
									return false;
								}
							}
							else
							{
								pbm->nFrozeFlag = 2;
								if (pbm->nMarginFlag == 1) //持有的在多
								{
									pbm->dFrozeAmt  = pbm->dSellMargin+pbm->dSellFroze-pbm->dBuyMargin;
								}
								else //持有的在空
								{
									//现有(冻结+持有) 减去占有，即为该再去冻结的
									//pbm->dFrozeAmt  = pbm->dSellMargin+pbm->dSellFroze-pbm->dSellMargin;
									pbm->dFrozeAmt = pbm->dSellFroze;//同上面一样，少了运算
								}
								//进行新的冻结
								if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,pbm->dFrozeAmt,true))
								{
									m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
									return false;
								}
							}

						}
						m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"资金账号[%s]品种[%d] 大边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 冻结[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
						//按剩余数量冻手续费
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dExFee*info->nRemainNum/(1.00*info->nOrderNum),true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
							return false;
						}
					}
				}
				else //平
				{
					//冻持仓
					pPosi = m_pMemDb->m_pDbTrader.m_pPosition.Select(info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo);
					if (pPosi == NULL)
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"资金账号[%s] 平仓时 席位[%s] 投保标志[%s] 合约[%d %s]无持仓",info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo,info->sContractCode);
						return false;
					}
					//只冻剩余数量
					if (info->sBsFlag[0] == BS_FLAG_BUY) //买平 平空仓
					{
						//可用减少
						pPosi->nCurCanUsePS = pPosi->nCurCanUsePS - info->nRemainNum;
						//冻结增加
						pPosi->nDAY_COV_PS_Froze = pPosi->nDAY_COV_PS_Froze + info->nRemainNum;
					}
					else //卖平 平多仓
					{
						//可用减少
						pPosi->nCurCanUsePB = pPosi->nCurCanUsePB - info->nRemainNum;
						//冻结增加
						pPosi->nDAY_COV_PB_Froze = pPosi->nDAY_COV_PB_Froze - info->nRemainNum;
					}
					//冻费用
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dExFee*info->nRemainNum/(1.00*info->nOrderNum),true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
						return false;
					}
				}
			}
			else if (nMarginMode == 1) //净边保证金
			{
				if (ProcOrderNetMargin(lockinfo,fundinfo,info,pc,true,false) != 0)
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"处理净边初始报单出错");
					return false;
				}
			}
			else
			{
				if (info->sEoFlag[0] == EO_FLAG_OPEN) //开仓
				{
					//冻资金
					if (info->sBsFlag[0] == BS_FLAG_BUY)
					{
						//按剩余数量冻买保证金
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,info->dFrozAmt*info->nRemainNum/(1.00*info->nOrderNum),true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
							return false;
						}
						//按剩余数量冻手续费
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dExFee*info->nRemainNum/(1.00*info->nOrderNum),true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
							return false;
						}
					}
					else
					{
						//按剩余数量冻卖保证金
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,info->dFrozAmt*info->nRemainNum/(1.00*info->nOrderNum),true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
							return false;
						}
						//按剩余数量冻手续费
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dExFee*info->nRemainNum/(1.00*info->nOrderNum),true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
							return false;
						}
					}
				}
				else //平
				{
					//冻持仓
					pPosi = m_pMemDb->m_pDbTrader.m_pPosition.Select(info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo);
					if (pPosi == NULL)
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"资金账号[%s] 平仓时 席位[%s] 投保标志[%s] 合约[%d %s]无持仓",info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo,info->sContractCode);
						return false;
					}
					//只冻剩余数量
					if (info->sBsFlag[0] == BS_FLAG_BUY) //买平 平空仓
					{
						//可用减少
						pPosi->nCurCanUsePS = pPosi->nCurCanUsePS - info->nRemainNum;
						//冻结增加
						pPosi->nDAY_COV_PS_Froze = pPosi->nDAY_COV_PS_Froze + info->nRemainNum;
					}
					else //卖平 平多仓
					{
						//可用减少
						pPosi->nCurCanUsePB = pPosi->nCurCanUsePB - info->nRemainNum;
						//冻结增加
						pPosi->nDAY_COV_PB_Froze = pPosi->nDAY_COV_PB_Froze - info->nRemainNum;
					}
					//冻费用
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dExFee*info->nRemainNum/(1.00*info->nOrderNum),true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
						return false;
					}
					
				}
			}
			
			break;
		case PROD_TYPE_SPOT:
		case PROD_TYPE_SEC:
			if (info->sBsFlag[0] == BS_FLAG_BUY) //买 
			{
				//按剩余数量冻买冻结
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZE,info->dAmt*info->nRemainNum/(1.00*info->nOrderNum),true))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
					return false;
				}
				//按剩余数量冻手续费
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dExFee*info->nRemainNum/(1.00*info->nOrderNum),true))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
					return false;
				}
			}
			else
			{
				//冻持仓
				pPosi = m_pMemDb->m_pDbTrader.m_pPosition.Select(info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo);
				if (pPosi == NULL)
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"资金账号[%s] 平仓时 席位[%s] 投保标志[%s] 合约[%d %s]无持仓",info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo,info->sContractCode);
					return false;
				}
				//只冻剩余数量  可用多仓减少，冻结多仓增加
				pPosi->nCurCanUsePB = pPosi->nCurCanUsePB - info->nRemainNum;
				pPosi->nDAY_COV_PB_Froze = pPosi->nDAY_COV_PB_Froze + info->nRemainNum;

				//冻费用
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dExFee*info->nRemainNum/(1.00*info->nOrderNum),true))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
					return false;
				}
			}
			break;
		case PROD_TYPE_OPTION:
			if (info->sEoFlag[0] == EO_FLAG_OPEN) //开仓
			{
				//冻资金
				if (info->sBsFlag[0] == BS_FLAG_BUY)
				{
					//冻结买开资金
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZE,info->dAmt*info->nRemainNum/(1.00*info->nOrderNum),true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
						return false;
					}
					//冻结买手续费
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dExFee*info->nRemainNum/(1.00*info->nOrderNum),true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
						return false;
					}
				}
				else
				{
					//冻结卖开保证金
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,info->dFrozAmt*info->nRemainNum/(1.00*info->nOrderNum),true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
						return false;
					}
					//冻结手续费
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dExFee*info->nRemainNum/(1.00*info->nOrderNum),true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
						return false;
					}
				}
			}
			else //平
			{
				//冻持仓
				pPosi = m_pMemDb->m_pDbTrader.m_pPosition.Select(info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo);
				if (pPosi == NULL)
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"资金账号[%s] 平仓时 席位[%s] 投保标志[%s] 合约[%d %s]无持仓",info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo,info->sContractCode);
					return false;
				}
				//只冻剩余数量
				if (info->sBsFlag[0] == BS_FLAG_BUY) //买平 平空仓
				{
					//冻权利金
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZE,info->dAmt*info->nRemainNum/(1.00*info->nOrderNum),true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
						return false;
					}
					//可用减少冻结增加
					pPosi->nCurCanUsePS = pPosi->nCurCanUsePS - info->nRemainNum;
					pPosi->nDAY_COV_PS_Froze = pPosi->nDAY_COV_PS_Froze + info->nRemainNum;
				}
				else //卖平 平多仓
				{
					//可用减少冻结增加
					pPosi->nCurCanUsePB = pPosi->nCurCanUsePB - info->nRemainNum;
					pPosi->nDAY_COV_PB_Froze = pPosi->nDAY_COV_PB_Froze + info->nRemainNum;
				}
				//冻费用
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dExFee*info->nRemainNum/(1.00*info->nOrderNum),true))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
					return false;
				}

			}
			break;
		default:
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"合约类型[%c] 不符 ",pc->sProd_type[0]);
			return false;
	}
	return true;
}

int CKernelPub::ComputeNewOrder(S_MDB_TXACC_LOCK *lockinfo,S_MDB_CUST_FUND *fundinfo,S_MDB_ORDER_INFO *info,S_MDB_CONTRACT_INFO *pc,bool checkfund)
{
	//加锁
	CBF_PMutex pp(lockinfo->mutex);
	S_MDB_CUST_POSITION *pPosi = NULL;
	S_MDB_BIG_MARGIN *pbm=NULL;
	S_MDB_BIG_MARGIN bm;
	int nMarginMode=0;
	double dTmp=0.0;
	bzero(&bm,sizeof(S_MDB_BIG_MARGIN));
	int rid;
	int nRet;
	double dLastFroze=0.0;
	int    nLastFlag=1;

	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"处理新报单  账户[%s]合约编号[%d %s]  开平[%s] 买卖[%s] 投保[%s] 本地报单号[%s] 冻结[%lf] 委托量[%d] 剩余量[%d]",\
		info->sTxAcc,info->nContractNo,info->sContractCode,info->sEoFlag,info->sBsFlag,info->sShFlag,info->sLocalSerial,info->dFrozAmt,info->nOrderNum,info->nRemainNum);
	switch (pc->sProd_type[0])
	{
		case PROD_TYPE_FUTURES://期货
			nMarginMode = GetMarginMode(pc);
			if (nMarginMode<0)
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"合约[%d %s] 取保证金模式出错 ",pc->nContractId,pc->sContractCode);
				info->sOrderStatus[0] = ORDER_STATUS_INVALID;
				strcpy(info->sOrderRsp,"取保证金模式出错");
				return RET_ERR_SYSTEM;
			}
			if (nMarginMode == 2) //大边保证金
			{
				if (info->sEoFlag[0] == EO_FLAG_OPEN) //开仓
				{
					//冻资金
					if (info->sBsFlag[0] == BS_FLAG_BUY) //买开
					{
						//查找表
						pbm = m_pMemDb->m_pDbTrader.m_pBigNetMargin.Select(info->sTxAcc,pc->nProdId);
						if (pbm == NULL)//没有新增
						{
							strcpy(bm.sTxAcc,info->sTxAcc);
							bm.nMarginFlag = 1;
							bm.sBailMode= BAIL_MODE_LARGE;
							bm.nVarietyId = pc->nProdId;
							bm.dFrozeAmt = info->dFrozAmt;
							bm.dBuyFroze = info->dFrozAmt;
							bm.nBuyQty = info->nOrderNum;
							bm.nFrozeFlag = 1;
							rid = m_pMemDb->m_pDbTrader.m_pBigNetMargin.Insert(bm);
							if (rid<0)
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"写品种大边净边表出错 资金账号[%s]品种[%d]",info->sTxAcc,pc->nProdId);
								info->sOrderStatus[0] = ORDER_STATUS_INVALID;
								strcpy(info->sOrderRsp,"写品种大边净边表出错");
								return RET_ERR_SYSTEM;
							}
							pbm =  m_pMemDb->m_pDbTrader.m_pBigNetMargin.At(rid);
							m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"资金账号[%s]品种[%d] 大边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 冻结[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
							//冻买保证金
							if (checkfund)
							{
								if (fundinfo->dAvailFund < pbm->dFrozeAmt-info->dExFee)
								{
									m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"可用资金不足   客户[%s %s]可用[%.4lf]，要冻结[%.4lf]",fundinfo->sTxAcc,fundinfo->sCurrency,fundinfo->dAvailFund,pbm->dFrozeAmt-info->dExFee);
									info->sOrderStatus[0] = ORDER_STATUS_INVALID;
									strcpy(info->sOrderRsp,"可用资金不足");
									return RET_ERR_AVAILFUND;
								}
							}
							
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,bm.dFrozeAmt,false))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
								info->sOrderStatus[0] = ORDER_STATUS_INVALID;
								strcpy(info->sOrderRsp,"更新资金出错");
								return RET_ERR_SYSTEM;
							}
						}
						else //计算该冻结多少
						{
							m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"资金账号[%s]品种[%d] 大边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 冻结[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
							pbm->dBuyFroze = pbm->dBuyFroze+info->dFrozAmt;
							pbm->nBuyQty = pbm->nBuyQty+info->nOrderNum;
							dTmp = pbm->dBuyMargin+pbm->dBuyFroze-pbm->dSellMargin-pbm->dSellFroze;

							nLastFlag = pbm->nFrozeFlag;
							dLastFroze = pbm->dFrozeAmt;

							
							if (dTmp > MINDOUBLECOMPARE) //大于等于0
							{
								pbm->nFrozeFlag = 1; //冻结的在多
								if (pbm->nMarginFlag == 1) //持有的在多
								{
									//pbm->dFrozeAmt  = pbm->dBuyMargin+pbm->dBuyFroze-pbm->dBuyMargin;
									pbm->dFrozeAmt =pbm->dBuyFroze;//同上面一样，少了运算
								}
								else //持有的在空
								{
									//现有(冻结+持有) 减去占有，即为该再去冻结的
									pbm->dFrozeAmt  = pbm->dBuyMargin+pbm->dBuyFroze-pbm->dSellMargin;
								}

							}
							else
							{
								pbm->nFrozeFlag = 2;
								if (pbm->nMarginFlag == 1) //持有的在多
								{
									pbm->dFrozeAmt  = pbm->dSellMargin+pbm->dSellFroze-pbm->dBuyMargin;
								}
								else //持有的在空
								{
									//现有(冻结+持有) 减去占有，即为该再去冻结的
									//pbm->dFrozeAmt  = pbm->dSellMargin+pbm->dSellFroze-pbm->dSellMargin;
									pbm->dFrozeAmt = pbm->dSellFroze;//同上面一样，少了运算
								}
								
								
							}
						}
						//判断资金是否足够
						if (checkfund)
						{
							dTmp = pbm->dFrozeAmt-dLastFroze;
							if (dTmp >MAXDOUBLECOMPARE) //需要冻结
							{
								if (fundinfo->dAvailFund < dTmp-info->dExFee+MAXDOUBLECOMPARE )
								{
									m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"可用资金不足   客户[%s %s]可用[%.4lf]，要冻结[%.4lf]",fundinfo->sTxAcc,fundinfo->sCurrency,fundinfo->dAvailFund,dTmp-info->dExFee);
									info->sOrderStatus[0] = ORDER_STATUS_INVALID;
									strcpy(info->sOrderRsp,"可用资金不足");
									return RET_ERR_AVAILFUND;
								}
							}
							else
							{
								if (fundinfo->dAvailFund < 0.0-info->dExFee+MAXDOUBLECOMPARE)
								{
									m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"可用资金不足   客户[%s %s]可用[%.4lf]，要冻结[%.4lf]",fundinfo->sTxAcc,fundinfo->sCurrency,fundinfo->dAvailFund,0.00-info->dExFee);
									info->sOrderStatus[0] = ORDER_STATUS_INVALID;
									strcpy(info->sOrderRsp,"可用资金不足");
									return RET_ERR_AVAILFUND;
								}
							}
						}
						//先解冻
						if (nLastFlag == 1)
						{
							//将原来的释放
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,0.00-dLastFroze,false))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
								info->sOrderStatus[0] = ORDER_STATUS_INVALID;
								strcpy(info->sOrderRsp,"更新资金出错");
								return RET_ERR_SYSTEM;
							}
						}
						else
						{
							//将原来的释放
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,0.00-dLastFroze,false))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
								info->sOrderStatus[0] = ORDER_STATUS_INVALID;
								strcpy(info->sOrderRsp,"更新资金出错");
								return RET_ERR_SYSTEM;
							}
						}
						//重新冻结
						if (pbm->nFrozeFlag == 1)
						{
							//进行新的冻结
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,pbm->dFrozeAmt,false))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
								info->sOrderStatus[0] = ORDER_STATUS_INVALID;
								strcpy(info->sOrderRsp,"更新资金出错");
								return RET_ERR_SYSTEM;
							}
						}
						else
						{
							//进行新的冻结
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,pbm->dFrozeAmt,false))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
								info->sOrderStatus[0] = ORDER_STATUS_INVALID;
								strcpy(info->sOrderRsp,"更新资金出错");
								return RET_ERR_SYSTEM;
							}
						}
						m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"资金账号[%s]品种[%d] 大边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 冻结[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
						//冻手续费
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dExFee,false))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
							info->sOrderStatus[0] = ORDER_STATUS_INVALID;
							strcpy(info->sOrderRsp,"更新资金出错");
							return RET_ERR_SYSTEM;
						}
					}
					else  //卖开
					{
						//查找表
						pbm = m_pMemDb->m_pDbTrader.m_pBigNetMargin.Select(info->sTxAcc,pc->nProdId);
						if (pbm == NULL)//没有新增
						{
							strcpy(bm.sTxAcc,info->sTxAcc);
							bm.nMarginFlag = 1;
							bm.sBailMode= BAIL_MODE_LARGE;
							bm.nVarietyId = pc->nProdId;
							bm.dFrozeAmt = info->dFrozAmt;
							bm.dSellFroze = info->dFrozAmt;
							bm.nSellQty = info->nOrderNum;
							bm.nFrozeFlag = 2;
							rid = m_pMemDb->m_pDbTrader.m_pBigNetMargin.Insert(bm);
							if (rid<0)
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"写品种大边净边表出错 资金账号[%s]品种[%d]",info->sTxAcc,pc->nProdId);
								info->sOrderStatus[0] = ORDER_STATUS_INVALID;
								strcpy(info->sOrderRsp,"写品种大边净边表出错");
								return RET_ERR_SYSTEM;
							}
							pbm =  m_pMemDb->m_pDbTrader.m_pBigNetMargin.At(rid);
							m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"资金账号[%s]品种[%d] 大边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 冻结[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
							if (checkfund)
							{
								if (fundinfo->dAvailFund < pbm->dFrozeAmt-info->dExFee)
								{
									m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"可用资金不足   客户[%s %s]可用[%.4lf]，要冻结[%.4lf]",fundinfo->sTxAcc,fundinfo->sCurrency,fundinfo->dAvailFund,pbm->dFrozeAmt-info->dExFee);
									info->sOrderStatus[0] = ORDER_STATUS_INVALID;
									strcpy(info->sOrderRsp,"可用资金不足");
									return RET_ERR_AVAILFUND;
								}
							}
							//冻卖保证金
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,pbm->dFrozeAmt,false))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
								info->sOrderStatus[0] = ORDER_STATUS_INVALID;
								strcpy(info->sOrderRsp,"更新资金出错");
								return RET_ERR_SYSTEM;
							}
						}
						else //计算该冻结多少
						{
							m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"资金账号[%s]品种[%d] 大边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 冻结[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
							pbm->dSellFroze = pbm->dSellFroze+info->dFrozAmt;
							pbm->nSellQty = pbm->nSellQty+info->nOrderNum;
							dTmp = pbm->dBuyMargin+pbm->dBuyFroze-pbm->dSellMargin-pbm->dSellFroze;

							if (dTmp > MINDOUBLECOMPARE) //大于等于0
							{
								pbm->nFrozeFlag = 1; //冻结的在多
								if (pbm->nMarginFlag == 1) //持有的在多
								{
									//pbm->dFrozeAmt  = pbm->dBuyMargin+pbm->dBuyFroze-pbm->dBuyMargin;
									pbm->dFrozeAmt = pbm->dBuyFroze;//同上面一样，少了运算
								}
								else //持有的在空
								{
									//现有(冻结+持有) 减去占有，即为该再去冻结的
									pbm->dFrozeAmt  = pbm->dBuyMargin+pbm->dBuyFroze-pbm->dSellMargin;
								}
							}
							else
							{
								pbm->nFrozeFlag = 2;
								if (pbm->nMarginFlag == 1) //持有的在多
								{
									pbm->dFrozeAmt  = pbm->dSellMargin+pbm->dSellFroze-pbm->dBuyMargin;
								}
								else //持有的在空
								{
									//现有(冻结+持有) 减去占有，即为该再去冻结的
									//pbm->dFrozeAmt  = pbm->dSellMargin+pbm->dSellFroze-pbm->dSellMargin;
									pbm->dFrozeAmt = pbm->dSellFroze;//同上面一样，少了运算
								}
							}

						}
						//判断资金是否足够
						if (checkfund)
						{
							dTmp = pbm->dFrozeAmt-dLastFroze;
							if (dTmp >MAXDOUBLECOMPARE) //需要冻结
							{
								if (fundinfo->dAvailFund < dTmp-info->dExFee+MAXDOUBLECOMPARE )
								{
									m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"可用资金不足   客户[%s %s]可用[%.4lf]，要冻结[%.4lf]",fundinfo->sTxAcc,fundinfo->sCurrency,fundinfo->dAvailFund,dTmp-info->dExFee);
									info->sOrderStatus[0] = ORDER_STATUS_INVALID;
									strcpy(info->sOrderRsp,"可用资金不足");
									return RET_ERR_AVAILFUND;
								}
							}
							else
							{
								if (fundinfo->dAvailFund < 0.0-info->dExFee+MAXDOUBLECOMPARE)
								{
									m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"可用资金不足   客户[%s %s]可用[%.4lf]，要冻结[%.4lf]",fundinfo->sTxAcc,fundinfo->sCurrency,fundinfo->dAvailFund,0.00-info->dExFee);
									info->sOrderStatus[0] = ORDER_STATUS_INVALID;
									strcpy(info->sOrderRsp,"可用资金不足");
									return RET_ERR_AVAILFUND;
								}
							}
						}
						//先解冻
						if (nLastFlag == 1)
						{
							//将原来的释放
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,0.00-dLastFroze,false))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
								info->sOrderStatus[0] = ORDER_STATUS_INVALID;
								strcpy(info->sOrderRsp,"更新资金出错");
								return RET_ERR_SYSTEM;
							}
						}
						else
						{
							//将原来的释放
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,0.00-dLastFroze,false))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
								info->sOrderStatus[0] = ORDER_STATUS_INVALID;
								strcpy(info->sOrderRsp,"更新资金出错");
								return RET_ERR_SYSTEM;
							}
						}
						//重新冻结
						if (pbm->nFrozeFlag == 1)
						{
							//进行新的冻结
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,pbm->dFrozeAmt,false))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
								info->sOrderStatus[0] = ORDER_STATUS_INVALID;
								strcpy(info->sOrderRsp,"更新资金出错");
								return RET_ERR_SYSTEM;
							}
						}
						else
						{
							//进行新的冻结
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,pbm->dFrozeAmt,false))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
								info->sOrderStatus[0] = ORDER_STATUS_INVALID;
								strcpy(info->sOrderRsp,"更新资金出错");
								return RET_ERR_SYSTEM;
							}
						}
						m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"资金账号[%s]品种[%d] 大边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 冻结[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
						//冻手续费
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dExFee,false))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
							info->sOrderStatus[0] = ORDER_STATUS_INVALID;
							strcpy(info->sOrderRsp,"更新资金出错");
							return RET_ERR_SYSTEM;
						}
					}
				}
				else //平
				{
					//冻持仓
					pPosi = m_pMemDb->m_pDbTrader.m_pPosition.Select(info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo);
					if (pPosi == NULL)
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"资金账号[%s] 平仓时 席位[%s] 投保标志[%s] 合约[%d %s]无持仓",info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo,info->sContractCode);
						info->sOrderStatus[0] = ORDER_STATUS_INVALID;
						strcpy(info->sOrderRsp,"持仓不足");
						return RET_ERR_POSITION_NOTFOUND;
					}
					//只冻剩余数量
					if (info->sBsFlag[0] == BS_FLAG_BUY) //买平 平空仓
					{
						//可用减少
						if (pPosi->nCurCanUsePS < info->nOrderNum)
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"资金账号[%s] 平仓时 席位[%s] 投保标志[%s] 合约[%d %s] 可用持仓不足",info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo,info->sContractCode);
							info->sOrderStatus[0] = ORDER_STATUS_INVALID;
							strcpy(info->sOrderRsp,"持仓不足");
							return RET_ERR_POSITION_NOTFOUND;
						}
						pPosi->nCurCanUsePS = pPosi->nCurCanUsePS - info->nOrderNum;
						//冻结增加
						pPosi->nDAY_COV_PS_Froze = pPosi->nDAY_COV_PS_Froze + info->nOrderNum;
					}
					else //卖平 平多仓
					{
						//可用减少
						if (pPosi->nCurCanUsePB < info->nOrderNum)
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"资金账号[%s] 平仓时 席位[%s] 投保标志[%s] 合约[%d %s] 可用持仓不足",info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo,info->sContractCode);
							info->sOrderStatus[0] = ORDER_STATUS_INVALID;
							strcpy(info->sOrderRsp,"持仓不足");
							return RET_ERR_POSITION_NOTFOUND;
						}
						pPosi->nCurCanUsePB = pPosi->nCurCanUsePB - info->nOrderNum;
						//冻结增加
						pPosi->nDAY_COV_PB_Froze = pPosi->nDAY_COV_PB_Froze - info->nOrderNum;
					}
					//平仓不用验手续费是否足够
					//if (checkfund)
					//{
					//	if (fundinfo->dAvailFund <  0.00-info->dExFee+MINDOUBLECOMPARE)
					//	{
					//		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"可用资金不足   客户[%s %s]可用[%.4lf]，要冻结[%.4lf]",fundinfo->sTxAcc,fundinfo->sCurrency,fundinfo->dAvailFund,0.00-info->dExFee);
					//		return RET_ERR_AVAILFUND;
					//	}
					//}
					//冻费用
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dExFee,false))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
						info->sOrderStatus[0] = ORDER_STATUS_INVALID;
						strcpy(info->sOrderRsp,"更新资金出错");
						return RET_ERR_SYSTEM;
					}
				}
			}
			else if (nMarginMode == 1) //净边保证金
			{
				nRet = ProcOrderNetMargin(lockinfo,fundinfo,info,pc,false,checkfund);
				if (nRet != 0)
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"处理净边报单出错");
					return nRet;
				}
			}
			else //双边保证金
			{
				if (info->sEoFlag[0] == EO_FLAG_OPEN) //开仓
				{
					if (checkfund)
					{
						if (fundinfo->dAvailFund <  info->dFrozAmt-info->dExFee+MINDOUBLECOMPARE)
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"可用资金不足   客户[%s %s]可用[%.4lf]，要冻结[%.4lf]",fundinfo->sTxAcc,fundinfo->sCurrency,fundinfo->dAvailFund, info->dFrozAmt-info->dExFee);
							info->sOrderStatus[0] = ORDER_STATUS_INVALID;
							strcpy(info->sOrderRsp,"可用资金不足");
							return RET_ERR_AVAILFUND;
						}
					}
					//冻资金
					if (info->sBsFlag[0] == BS_FLAG_BUY)
					{
						//冻结买保证金
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,info->dFrozAmt,false))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund 出错");
							info->sOrderStatus[0] = ORDER_STATUS_INVALID;
							strcpy(info->sOrderRsp,"更新资金出错");
							return RET_ERR_SYSTEM;
						}
						//冻结手续费
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dExFee,false))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund 出错");
							info->sOrderStatus[0] = ORDER_STATUS_INVALID;
							strcpy(info->sOrderRsp,"更新资金出错");
							return RET_ERR_SYSTEM;
						}
					}
					else
					{
						//冻结卖保证金
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,info->dFrozAmt,false))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund 出错");
							info->sOrderStatus[0] = ORDER_STATUS_INVALID;
							strcpy(info->sOrderRsp,"更新资金出错");
							return RET_ERR_SYSTEM;
						}
						//冻结手续费
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dExFee,false))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund 出错");
							info->sOrderStatus[0] = ORDER_STATUS_INVALID;
							strcpy(info->sOrderRsp,"更新资金出错");
							return RET_ERR_SYSTEM;
						}
					}
				}
				else //平
				{
					//冻持仓
					pPosi = m_pMemDb->m_pDbTrader.m_pPosition.Select(info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo);
					if (pPosi == NULL)
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"资金账号[%s] 平仓时 席位[%s] 投保标志[%s] 合约[%d %s]无持仓",info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo,info->sContractCode);
						info->sOrderStatus[0] = ORDER_STATUS_INVALID;
						strcpy(info->sOrderRsp,"持仓不足");
						return RET_ERR_POSITION_NOTFOUND;
					}
					//冻结报单数量
					if (info->sBsFlag[0] == BS_FLAG_BUY) //买平 平空仓
					{
						//可用空仓减少
						if (pPosi->nCurCanUsePS < info->nOrderNum)
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"资金账号[%s] 平仓时 席位[%s] 投保标志[%s] 合约[%d %s] 可用持仓不足",info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo,info->sContractCode);
							info->sOrderStatus[0] = ORDER_STATUS_INVALID;
							strcpy(info->sOrderRsp,"持仓不足");
							return RET_ERR_POSITION_NOTFOUND;
						}
						pPosi->nCurCanUsePS = pPosi->nCurCanUsePS - info->nOrderNum;
						//冻结空仓增加
						pPosi->nDAY_COV_PS_Froze = pPosi->nDAY_COV_PS_Froze + info->nOrderNum;
					}
					else //卖平 平多仓
					{
						//可用多仓减少
						if (pPosi->nCurCanUsePB < info->nOrderNum)
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"资金账号[%s] 平仓时 席位[%s] 投保标志[%s] 合约[%d %s] 可用持仓不足",info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo,info->sContractCode);
							info->sOrderStatus[0] = ORDER_STATUS_INVALID;
							strcpy(info->sOrderRsp,"持仓不足");
							return RET_ERR_POSITION_NOTFOUND;
						}
						pPosi->nCurCanUsePB = pPosi->nCurCanUsePB - info->nOrderNum;
						//冻结多仓增加
						pPosi->nDAY_COV_PB_Froze = pPosi->nDAY_COV_PB_Froze + info->nOrderNum;
					}
					//平仓不用判断手续费是否足够
					//冻费用
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dExFee,false))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund 出错");
						info->sOrderStatus[0] = ORDER_STATUS_INVALID;
						strcpy(info->sOrderRsp,"更新资金出错");
						return RET_ERR_SYSTEM;
					}

				}
			}
			break;
		case PROD_TYPE_SPOT:
		case PROD_TYPE_SEC:
			if (info->sBsFlag[0] == BS_FLAG_BUY) //买 
			{
				if (checkfund)
				{
					if (fundinfo->dAvailFund <  info->dAmt-info->dExFee+MINDOUBLECOMPARE)
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"可用资金不足   客户[%s %s]可用[%.4lf]，要冻结[%.4lf]",fundinfo->sTxAcc,fundinfo->sCurrency,fundinfo->dAvailFund, info->dAmt-info->dExFee);
						info->sOrderStatus[0] = ORDER_STATUS_INVALID;
						strcpy(info->sOrderRsp,"可用资金不足");
						return RET_ERR_AVAILFUND;
					}
				}
				//买冻结
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZE,info->dAmt,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund 出错");
					info->sOrderStatus[0] = ORDER_STATUS_INVALID;
					strcpy(info->sOrderRsp,"更新资金出错");
					return RET_ERR_SYSTEM;
				}
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dExFee,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund 出错");
					info->sOrderStatus[0] = ORDER_STATUS_INVALID;
					strcpy(info->sOrderRsp,"更新资金出错");
					return RET_ERR_SYSTEM;
				}
			}
			else
			{
				//冻持仓库存
				pPosi = m_pMemDb->m_pDbTrader.m_pPosition.Select(info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo);
				if (pPosi == NULL)
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"资金账号[%s] 平仓时 席位[%s] 投保标志[%s] 合约[%d %s]无持仓",info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo,info->sContractCode);
					info->sOrderStatus[0] = ORDER_STATUS_INVALID;
					strcpy(info->sOrderRsp,"持有不足");
					return RET_ERR_POSITION_NOTFOUND;
				}
				if (pPosi->nCurCanUsePB < info->nOrderNum)
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"资金账号[%s] 平仓时 席位[%s] 投保标志[%s] 合约[%d %s] 可用持仓不足",info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo,info->sContractCode);
					info->sOrderStatus[0] = ORDER_STATUS_INVALID;
					strcpy(info->sOrderRsp,"持有不足");
					return RET_ERR_POSITION_NOTFOUND;
				}
				//可用多仓减少
				pPosi->nCurCanUsePB = pPosi->nCurCanUsePB - info->nOrderNum;
				//冻结多仓增加
				pPosi->nDAY_COV_PB_Froze = pPosi->nDAY_COV_PB_Froze + info->nOrderNum;
				//冻费用
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dExFee,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund 出错");
					info->sOrderStatus[0] = ORDER_STATUS_INVALID;
					strcpy(info->sOrderRsp,"更新资金出错");
					return RET_ERR_SYSTEM;
				}
			}
			break;
		case PROD_TYPE_OPTION:
			if (info->sEoFlag[0] == EO_FLAG_OPEN) //开仓
			{
				//冻资金
				if (info->sBsFlag[0] == BS_FLAG_BUY) //买开
				{
					if (checkfund)
					{
						if (fundinfo->dAvailFund <  info->dAmt-info->dExFee+MINDOUBLECOMPARE)
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"可用资金不足   客户[%s %s]可用[%.4lf]，要冻结[%.4lf]",fundinfo->sTxAcc,fundinfo->sCurrency,fundinfo->dAvailFund, info->dAmt-info->dExFee);
							info->sOrderStatus[0] = ORDER_STATUS_INVALID;
							strcpy(info->sOrderRsp,"可用资金不足");
							return RET_ERR_AVAILFUND;
						}
					}
					//冻权利金
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZE,info->dAmt,false))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund 出错");
						info->sOrderStatus[0] = ORDER_STATUS_INVALID;
						strcpy(info->sOrderRsp,"更新资金出错");
						return RET_ERR_SYSTEM;
					}
					//冻手续费
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dExFee,false))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund 出错");
						info->sOrderStatus[0] = ORDER_STATUS_INVALID;
						strcpy(info->sOrderRsp,"更新资金出错");
						return RET_ERR_SYSTEM;
					}
				}
				else  //卖开
				{
					if (checkfund)
					{
						if (fundinfo->dAvailFund <  info->dFrozAmt-info->dExFee+MINDOUBLECOMPARE)
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"可用资金不足   客户[%s %s]可用[%.4lf]，要冻结[%.4lf]",fundinfo->sTxAcc,fundinfo->sCurrency,fundinfo->dAvailFund, info->dFrozAmt -info->dExFee);
							info->sOrderStatus[0] = ORDER_STATUS_INVALID;
							strcpy(info->sOrderRsp,"可用资金不足");
							return RET_ERR_AVAILFUND;
						}
					}
					//冻结卖出保证金
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,info->dFrozAmt,false))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund 出错");
						info->sOrderStatus[0] = ORDER_STATUS_INVALID;
						strcpy(info->sOrderRsp,"更新资金出错");
						return RET_ERR_SYSTEM;
					}
					//冻结手续费
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dExFee,false))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund 出错");
						info->sOrderStatus[0] = ORDER_STATUS_INVALID;
						strcpy(info->sOrderRsp,"更新资金出错");
						return RET_ERR_SYSTEM;
					}
				}
			}
			else //平
			{
				//冻持仓
				pPosi = m_pMemDb->m_pDbTrader.m_pPosition.Select(info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo);
				if (pPosi == NULL)
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"资金账号[%s] 平仓时 席位[%s] 投保标志[%s] 合约[%d %s]无持仓",info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo,info->sContractCode);
					info->sOrderStatus[0] = ORDER_STATUS_INVALID;
					strcpy(info->sOrderRsp,"持仓不足");
					return RET_ERR_POSITION_NOTFOUND;
				}
				//只冻剩余数量
				if (info->sBsFlag[0] == BS_FLAG_BUY) //买平 平空仓
				{
					if (pPosi->nCurCanUsePS < info->nOrderNum)
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"资金账号[%s] 平仓时 席位[%s] 投保标志[%s] 合约[%d %s] 可用持仓不足",info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo,info->sContractCode);
						info->sOrderStatus[0] = ORDER_STATUS_INVALID;
						strcpy(info->sOrderRsp,"持仓不足");
						return RET_ERR_POSITION_NOTFOUND;
					}
					if (checkfund)
					{
						if (fundinfo->dAvailFund <  info->dAmt-info->dExFee+MINDOUBLECOMPARE)
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"可用资金不足   客户[%s %s]可用[%.4lf]，要冻结[%.4lf]",fundinfo->sTxAcc,fundinfo->sCurrency,fundinfo->dAvailFund, info->dAmt -info->dExFee);
							info->sOrderStatus[0] = ORDER_STATUS_INVALID;
							strcpy(info->sOrderRsp,"可用资金不足");
							return RET_ERR_AVAILFUND;
						}
					}
					//冻权利金
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZE,info->dAmt,false))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund 出错");
						info->sOrderStatus[0] = ORDER_STATUS_INVALID;
						strcpy(info->sOrderRsp,"更新资金出错");
						return RET_ERR_SYSTEM;
					}
					
					//空仓可用减少
					pPosi->nCurCanUsePS = pPosi->nCurCanUsePS - info->nOrderNum;
					//冻结增加
					pPosi->nDAY_COV_PS_Froze = pPosi->nDAY_COV_PS_Froze + info->nOrderNum;
				}
				else //卖平 平多仓
				{
					//多仓可用减少
					if (pPosi->nCurCanUsePB < info->nOrderNum)
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"资金账号[%s] 平仓时 席位[%s] 投保标志[%s] 合约[%d %s] 可用持仓不足",info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo,info->sContractCode);
						info->sOrderStatus[0] = ORDER_STATUS_INVALID;
						strcpy(info->sOrderRsp,"持仓不足");
						return RET_ERR_POSITION_NOTFOUND;
					}
					pPosi->nCurCanUsePB = pPosi->nCurCanUsePB - info->nOrderNum;
					//冻结增加
					pPosi->nDAY_COV_PB_Froze = pPosi->nDAY_COV_PB_Froze + info->nOrderNum;
				}
				//冻费用
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dExFee,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund 出错");
					info->sOrderStatus[0] = ORDER_STATUS_INVALID;
					strcpy(info->sOrderRsp,"更新资金出错");
					return RET_ERR_SYSTEM;
				}

			}
			break;
		default:
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"合约类型[%c] 不符 ",pc->sProd_type[0]);
			info->sOrderStatus[0] = ORDER_STATUS_INVALID;
			strcpy(info->sOrderRsp,"合约类型不符");
			return RET_ERR_SYSTEM;
	}
	return SUCCESS;
}

bool CKernelPub::ComputeNewDone(S_MDB_TXACC_LOCK *lockinfo,S_MDB_CUST_FUND *fundinfo,S_MDB_DONE_INFO *info,S_MDB_ORDER_INFO *po,S_MDB_CONTRACT_INFO *pc)
{
	CBF_PMutex pp(lockinfo->mutex);
	//更新成交均价
	po->dDoneAvp = (po->dDoneAvp*(po->nOrderNum-po->nRemainNum) + info->dDonePrice*info->nDoneNum)/(po->nOrderNum-po->nRemainNum+info->nDoneNum);
	//更新报单剩余数量
	if (po->nRemainNum == 0 || po->sOrderStatus[0] == ORDER_STATUS_CANCEL || po->sOrderStatus[0] == ORDER_STATUS_PARTCANCEL)
	{
		//报单已为撤单
	}
	else
	{
		po->nRemainNum = po->nRemainNum - info->nDoneNum;
		//更新报单状态
		if (po->nRemainNum <=0)
		{
			po->sOrderStatus[0] = ORDER_STATUS_COMPLETE;
			po->sOrderStatus[1] =0;
		}
		else
		{
			po->sOrderStatus[0] = ORDER_STATUS_PART;
			po->sOrderStatus[1] =0;
		}
	}
	
	S_MDB_CUST_POSITION *pPosi=NULL;
	S_MDB_BIG_MARGIN *pbm=NULL;
	int nMarginMode=0;
	double dTmp=0.0;
	

	switch (pc->sProd_type[0] )
	{
		case PROD_TYPE_FUTURES:
			nMarginMode = GetMarginMode(pc);
			if (nMarginMode<0)
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"合约[%d %s] 取保证金模式出错 ",pc->nContractId,pc->sContractCode);
				return false;
			}
			if (nMarginMode == 2) //大边保证金
			{
				if (info->sEoFlag[0] == EO_FLAG_OPEN) //开仓
				{
					if (info->sBsFlag[0] == BS_FLAG_BUY)  //开多仓
					{
						//查找表
						pbm = m_pMemDb->m_pDbTrader.m_pBigNetMargin.Select(info->sTxAcc,pc->nProdId);
						if (pbm == NULL)
						{
							//成交但找不到记录
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"成交但大边记录找不到[%s %d]的记录",info->sTxAcc,pc->nProdId);
							return false;
						}
						m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"开多仓 资金账号[%s]品种[%d] 大边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 冻结[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
						//找到原来冻结的资金
						pbm->dBuyFroze = pbm->dBuyFroze - po->dFrozAmt*info->nDoneNum/po->nOrderNum; //买冻结减少
						pbm->nBuyQty = pbm->nBuyQty-info->nDoneNum;//买冻量减少
						pbm->nBuyPosi+=info->nDoneNum;//买持量增加
						//重新计算持有保证金
						if (pbm->nMarginFlag == 1) //大边在多 方向相同
						{
							pbm->dBuyMargin = pbm->dBuyMargin+info->dExMargin+info->dMemberMargin;
							//更新资金，即增加成交时的保证金
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_BUY,info->dExMargin+info->dMemberMargin,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
								return false;
							}
						}
						else //大边在空 方向相反
						{
							pbm->dBuyMargin = pbm->dBuyMargin+info->dExMargin+info->dMemberMargin;
							//原来是空多，现在要重新算一下
							if (pbm->dBuyMargin >pbm->dSellMargin+MAXDOUBLECOMPARE ) //大边在多了
							{
								pbm->nMarginFlag = 1;
								//换了个方向，大边在多 先去掉原方向的值
								if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_SELL,0.00-pbm->dSellMargin,true))
								{
									m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
									return false;
								}
								//再增加买方的值
								if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_BUY,pbm->dBuyMargin,true))
								{
									m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
									return false;
								}
							}
							else  //方向是空，开多后方向不变，啥都不用动
							{
								//方向不变，不用动
							}
						}
						m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"开多仓 资金账号[%s]品种[%d] 大边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 冻结[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
						if (pbm->nFrozeFlag == 1)
						{
							//将原来的释放
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,0.00-pbm->dFrozeAmt,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
								return false;
							}
						}
						else
						{
							//将原来的释放
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,0.00-pbm->dFrozeAmt,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
								return false;
							}
						}
						dTmp = pbm->dBuyMargin + pbm->dBuyFroze - pbm->dSellMargin - pbm->dSellFroze;
						if (dTmp > MINDOUBLECOMPARE) //大于等于0
						{
							pbm->nFrozeFlag = 1; //冻结的在多
							if (pbm->nMarginFlag == 1) //持有的在多
							{
								//pbm->dFrozeAmt  = pbm->dBuyMargin+pbm->dBuyFroze-pbm->dBuyMargin;
								pbm->dFrozeAmt =pbm->dBuyFroze;//同上面一样，少了运算
							}
							else //持有的在空
							{
								//现有(冻结+持有) 减去占有，即为该再去冻结的
								pbm->dFrozeAmt  = pbm->dBuyMargin+pbm->dBuyFroze-pbm->dSellMargin;
							}
							//进行新的冻结
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,pbm->dFrozeAmt,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
								return false;
							}
						}
						else
						{
							pbm->nFrozeFlag = 2;
							if (pbm->nMarginFlag == 1) //持有的在多
							{
								pbm->dFrozeAmt  = pbm->dSellMargin+pbm->dSellFroze-pbm->dBuyMargin;
							}
							else //持有的在空
							{
								//现有(冻结+持有) 减去占有，即为该再去冻结的
								//pbm->dFrozeAmt  = pbm->dSellMargin+pbm->dSellFroze-pbm->dSellMargin;
								pbm->dFrozeAmt = pbm->dSellFroze;//同上面一样，少了运算
							}
							//进行新的冻结
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,pbm->dFrozeAmt,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
								return false;
							}
						}
						m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"开多仓 资金账号[%s]品种[%d] 大边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 冻结[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
						//解冻手续费
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dUnfrozeFee,true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund 出错");
							return false;
						}
						//更新手续费
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FEE,FUND_CODE_FEE_EXCNANGE,info->dExFee+info->dMemberFee,true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund 出错");
							return false;
						}
					}
					else  //卖开，开空仓
					{
						//查找表
						pbm = m_pMemDb->m_pDbTrader.m_pBigNetMargin.Select(info->sTxAcc,pc->nProdId);
						if (pbm == NULL)
						{
							//成交但找不到记录
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"成交但大边记录找不到[%s %d]的记录",info->sTxAcc,pc->nProdId);
							return false;
						}
						m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"开空仓 资金账号[%s]品种[%d] 大边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 冻结[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
						//找到原来冻结的资金
						pbm->dSellFroze = pbm->dSellFroze - po->dFrozAmt*info->nDoneNum/po->nOrderNum; //卖冻结减少
						pbm->nSellQty = pbm->nSellQty-info->nDoneNum;//卖量减少
						pbm->nSellPosi +=info->nDoneNum;//卖持量增加
						//重新计算持有保证金
						if (pbm->nMarginFlag == 1) //大边在多 方向相反
						{
							pbm->dSellMargin = pbm->dSellMargin+info->dExMargin+info->dMemberMargin;
							if (pbm->dBuyMargin >pbm->dSellMargin+MAXDOUBLECOMPARE ) //大边在多，不变
							{
							}
							else  //大边变成空了
							{
								pbm->nMarginFlag = 2;
								//换了个方向，大边在多 先去掉原方向的值
								if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_SELL,0.00-pbm->dBuyMargin,true))
								{
									m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
									return false;
								}
								//再增加卖方的值
								if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_BUY,pbm->dSellMargin,true))
								{
									m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
									return false;
								}
							}
						}
						else //大边在空 方向相同
						{
							pbm->dSellMargin = pbm->dSellMargin+info->dExMargin+info->dMemberMargin;
							//更新资金
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_SELL,info->dExMargin+info->dMemberMargin,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
								return false;
							}
						}
						m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"开空仓 资金账号[%s]品种[%d] 大边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 冻结[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
						//重新计算冻结
						if (pbm->nFrozeFlag == 1)
						{
							//将原来的释放
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,0.00-pbm->dFrozeAmt,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
								return false;
							}
						}
						else
						{
							//将原来的释放
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,0.00-pbm->dFrozeAmt,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
								return false;
							}
						}
						dTmp = pbm->dBuyMargin + pbm->dBuyFroze - pbm->dSellMargin - pbm->dSellFroze;
						if (dTmp > MINDOUBLECOMPARE) //大于等于0
						{
							pbm->nFrozeFlag = 1; //冻结的在多
							if (pbm->nMarginFlag == 1) //持有的在多
							{
								//pbm->dFrozeAmt  = pbm->dBuyMargin+pbm->dBuyFroze-pbm->dBuyMargin;
								pbm->dFrozeAmt =pbm->dBuyFroze;//同上面一样，少了运算
							}
							else //持有的在空
							{
								//现有(冻结+持有) 减去占有，即为该再去冻结的
								pbm->dFrozeAmt  = pbm->dBuyMargin+pbm->dBuyFroze-pbm->dSellMargin;
							}
							//进行新的冻结
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,pbm->dFrozeAmt,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
								return false;
							}
						}
						else //变为空了
						{
							pbm->nFrozeFlag = 2;
							if (pbm->nMarginFlag == 1) //持有的在多
							{
								pbm->dFrozeAmt  = pbm->dSellMargin+pbm->dSellFroze-pbm->dBuyMargin;
							}
							else //持有的在空
							{
								//现有(冻结+持有) 减去占有，即为该再去冻结的
								//pbm->dFrozeAmt  = pbm->dSellMargin+pbm->dSellFroze-pbm->dSellMargin;
								pbm->dFrozeAmt = pbm->dSellFroze;//同上面一样，少了运算
							}
							//进行新的冻结
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,pbm->dFrozeAmt,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
								return false;
							}
						}
						m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"开空仓 资金账号[%s]品种[%d] 大边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 冻结[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
						//解冻手续费
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dUnfrozeFee,true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund 出错");
							return false;
						}
						//更新手续费
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FEE,FUND_CODE_FEE_EXCNANGE,info->dExFee+info->dMemberFee,true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund 出错");
							return false;
						}
					}

				}
				else //平仓 
				{
					//解冻委托冻结的持仓及手续费
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dUnfrozeFee,true))
					{
						return false;
					}
					pPosi = m_pMemDb->m_pDbTrader.m_pPosition.Select(info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo);
					if (pPosi == NULL)
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"无持仓 资金账号[%s] 席位[%s] 投保标志[%s] 合约[%d %s] 数量[%d] 开多",\
							info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo,info->sContractCode,info->nDoneNum);
						return false;
					}
					//计算持仓
					if (info->sBsFlag[0] == BS_FLAG_BUY) //买平  平空
					{
						//查找表
						pbm = m_pMemDb->m_pDbTrader.m_pBigNetMargin.Select(info->sTxAcc,pc->nProdId);
						if (pbm == NULL)
						{
							//成交但找不到记录
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"成交但大边记录找不到[%s %d]的记录",info->sTxAcc,pc->nProdId);
							return false;
						}
						m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"平空仓 资金账号[%s]品种[%d] 大边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 冻结[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
						//按合约的比例释放
						if (pPosi->nCurPS<=0)
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"平仓成交但找不到持仓 账户[%s] 品种[%d]",info->sTxAcc,pc->nProdId);
							return false;
						}
						//释放原来的
						if (pbm->nMarginFlag == 1) //大边在多
						{
							dTmp = pbm->dBuyMargin;
							//释放资金
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_BUY,0.0-pbm->dBuyMargin,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
								return false;
							}
						}
						else //大边在空 
						{
							dTmp = pbm->dSellMargin;
							//释放资金
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_SELL,0.0-pbm->dSellMargin,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
								return false;
							}
						}
						//重新计算持有保证金
						pbm->dSellMargin = pbm->dSellMargin - pPosi->dPS_Margin*(1.00*info->nDoneNum/pPosi->nCurPS);
						pbm->nSellPosi -= info->nDoneNum;//卖持量减少
						if (pbm->dBuyMargin >pbm->dSellMargin+MAXDOUBLECOMPARE ) //大边在多
						{
							pbm->nMarginFlag = 1;
							dTmp = pbm->dBuyMargin-dTmp;
							//更新买方的值
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_BUY,pbm->dBuyMargin,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
								return false;
							}
						}
						else
						{
							pbm->nMarginFlag = 2;
							dTmp = pbm->dSellMargin-dTmp;
							//更新买方的值
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_SELL,pbm->dSellMargin,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
								return false;
							}
						}
						m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"平空仓 资金账号[%s]品种[%d] 大边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 冻结[%lf] 保证金差额[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt,dTmp);
						//重新计算冻结	 20210422  平仓买卖冻结不变化，但因为平掉后保证金减少，方向可能会发生变化
						//释放原来的
						if (pbm->nFrozeFlag == 1) //大边在多
						{
							//释放资金
							if (!UpdateFund(lockinfo, fundinfo, FUND_TYPE_FROZE, FUND_CODE_BUYFROZEMARGIN, 0.0 - pbm->dFrozeAmt, true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "UpdateFund出错");
								return false;
							}
						}
						else //大边在空 
						{
							//释放资金
							if (!UpdateFund(lockinfo, fundinfo, FUND_TYPE_FROZE, FUND_CODE_SELLFROZEMARGIN, 0.0 - pbm->dFrozeAmt, true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "UpdateFund出错");
								return false;
							}
						}
						dTmp = pbm->dBuyMargin + pbm->dBuyFroze - pbm->dSellMargin - pbm->dSellFroze;
						if (dTmp > MINDOUBLECOMPARE) //大于等于0
						{
							pbm->nFrozeFlag = 1; //冻结的在多
							if (pbm->nMarginFlag == 1) //持有的在多
							{
								//pbm->dFrozeAmt  = pbm->dBuyMargin+pbm->dBuyFroze-pbm->dBuyMargin;
								pbm->dFrozeAmt = pbm->dBuyFroze;//同上面一样，少了运算
							}
							else //持有的在空
							{
								//现有(冻结+持有) 减去占有，即为该再去冻结的
								pbm->dFrozeAmt = pbm->dBuyMargin + pbm->dBuyFroze - pbm->dSellMargin;
							}
							//进行新的冻结
							if (!UpdateFund(lockinfo, fundinfo, FUND_TYPE_FROZE, FUND_CODE_BUYFROZEMARGIN, pbm->dFrozeAmt, true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "UpdateFund出错");
								return false;
							}
						}
						else
						{
							pbm->nFrozeFlag = 2;
							if (pbm->nMarginFlag == 1) //持有的在多
							{
								pbm->dFrozeAmt = pbm->dSellMargin + pbm->dSellFroze - pbm->dBuyMargin;
							}
							else //持有的在空
							{
								//现有(冻结+持有) 减去占有，即为该再去冻结的
								//pbm->dFrozeAmt  = pbm->dSellMargin+pbm->dSellFroze-pbm->dSellMargin;
								pbm->dFrozeAmt = pbm->dSellFroze;//同上面一样，少了运算
							}
							//进行新的冻结
							if (!UpdateFund(lockinfo, fundinfo, FUND_TYPE_FROZE, FUND_CODE_SELLFROZEMARGIN, pbm->dFrozeAmt, true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "UpdateFund出错");
								return false;
							}
						}
						m_pLog->LogMp(LOG_DEBUG, __FILE__, __LINE__, "平空仓 资金账号[%s]品种[%d] 大边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 冻结[%lf] 保证金差额[%lf]", pbm->sTxAcc, pbm->nVarietyId, pbm->nMarginFlag, pbm->dBuyMargin, pbm->dSellMargin, pbm->nFrozeFlag, pbm->dBuyFroze, pbm->dSellFroze, pbm->dFrozeAmt, dTmp);
						//重新计算冻结	 20210422 END
						//费用
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FEE,FUND_CODE_FEE_EXCNANGE,info->dExFee+info->dMemberFee,true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund 出错");
							return false;
						}
					}
					else //卖平 平多
					{
						//查找表
						//查找表
						pbm = m_pMemDb->m_pDbTrader.m_pBigNetMargin.Select(info->sTxAcc,pc->nProdId);
						if (pbm == NULL)
						{
							//成交但找不到记录
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"成交但大边记录找不到[%s %d]的记录",info->sTxAcc,pc->nProdId);
							return false;
						}
						m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"平多仓 资金账号[%s]品种[%d] 大边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 冻结[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
						//按合约的比例释放
						if (pPosi->nCurPB<=0)
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"平仓成交但找不到持仓 账户[%s] 品种[%d]",info->sTxAcc,pc->nProdId);
							return false;
						}
						//先释放原来的
						if (pbm->nMarginFlag == 1) //大边在多 方向相同
						{
							dTmp =  pbm->dBuyMargin;
							//释放资金
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_BUY,0.00-pbm->dBuyMargin,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
								return false;
							}
						}
						else //大边在空 方向相反
						{
							dTmp =  pbm->dSellMargin;
							//释放资金
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_SELL,0.00-pbm->dSellMargin,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
								return false;
							}
						}
						//重新计算持有保证金
						pbm->dBuyMargin = pbm->dBuyMargin - pPosi->dPB_Margin*info->nDoneNum/pPosi->nCurPB;
						pbm->nBuyPosi -= info->nDoneNum;//买持量减少
						if (pbm->dBuyMargin >pbm->dSellMargin+MAXDOUBLECOMPARE ) //大边在多
						{
							pbm->nMarginFlag = 1;
							dTmp = pbm->dBuyMargin- dTmp;
							//更新买方的值
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_BUY,pbm->dBuyMargin,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
								return false;
							}
						}
						else
						{
							pbm->nMarginFlag = 2;
							dTmp = pbm->dSellMargin- dTmp;
							//更新买方的值
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_SELL,pbm->dSellMargin,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
								return false;
							}
						}
						m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"平多仓 资金账号[%s]品种[%d] 大边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 冻结[%lf] 保证金差额[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt,dTmp);
						//重新计算冻结	 20210422  平仓买卖冻结不变化，但因为平掉后保证金减少，方向可能会发生变化
						//释放原来的
						if (pbm->nFrozeFlag == 1) //大边在多
						{
							//释放资金
							if (!UpdateFund(lockinfo, fundinfo, FUND_TYPE_FROZE, FUND_CODE_BUYFROZEMARGIN, 0.0 - pbm->dFrozeAmt, true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "UpdateFund出错");
								return false;
							}
						}
						else //大边在空 
						{
							//释放资金
							if (!UpdateFund(lockinfo, fundinfo, FUND_TYPE_FROZE, FUND_CODE_SELLFROZEMARGIN, 0.0 - pbm->dFrozeAmt, true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "UpdateFund出错");
								return false;
							}
						}
						dTmp = pbm->dBuyMargin + pbm->dBuyFroze - pbm->dSellMargin - pbm->dSellFroze;
						if (dTmp > MINDOUBLECOMPARE) //大于等于0
						{
							pbm->nFrozeFlag = 1; //冻结的在多
							if (pbm->nMarginFlag == 1) //持有的在多
							{
								//pbm->dFrozeAmt  = pbm->dBuyMargin+pbm->dBuyFroze-pbm->dBuyMargin;
								pbm->dFrozeAmt = pbm->dBuyFroze;//同上面一样，少了运算
							}
							else //持有的在空
							{
								//现有(冻结+持有) 减去占有，即为该再去冻结的
								pbm->dFrozeAmt = pbm->dBuyMargin + pbm->dBuyFroze - pbm->dSellMargin;
							}
							//进行新的冻结
							if (!UpdateFund(lockinfo, fundinfo, FUND_TYPE_FROZE, FUND_CODE_BUYFROZEMARGIN, pbm->dFrozeAmt, true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "UpdateFund出错");
								return false;
							}
						}
						else
						{
							pbm->nFrozeFlag = 2;
							if (pbm->nMarginFlag == 1) //持有的在多
							{
								pbm->dFrozeAmt = pbm->dSellMargin + pbm->dSellFroze - pbm->dBuyMargin;
							}
							else //持有的在空
							{
								//现有(冻结+持有) 减去占有，即为该再去冻结的
								//pbm->dFrozeAmt  = pbm->dSellMargin+pbm->dSellFroze-pbm->dSellMargin;
								pbm->dFrozeAmt = pbm->dSellFroze;//同上面一样，少了运算
							}
							//进行新的冻结
							if (!UpdateFund(lockinfo, fundinfo, FUND_TYPE_FROZE, FUND_CODE_SELLFROZEMARGIN, pbm->dFrozeAmt, true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "UpdateFund出错");
								return false;
							}
						}
						m_pLog->LogMp(LOG_DEBUG, __FILE__, __LINE__, "平多仓 资金账号[%s]品种[%d] 大边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 冻结[%lf] 保证金差额[%lf]", pbm->sTxAcc, pbm->nVarietyId, pbm->nMarginFlag, pbm->dBuyMargin, pbm->dSellMargin, pbm->nFrozeFlag, pbm->dBuyFroze, pbm->dSellFroze, pbm->dFrozeAmt, dTmp);
						//重新计算冻结	 20210422 END
						//费用
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FEE,FUND_CODE_FEE_EXCNANGE,info->dExFee+info->dMemberFee,true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund 出错");
							return false;
						}

					}
				}
			}
			else if (nMarginMode == 1) //净边保证金
			{
				if (!ProcDoneNetMargin(lockinfo,fundinfo,info,po,pc,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"处理净边成交出错");
					return false;
				}
			}
			else 
			{
				if (info->sEoFlag[0] == EO_FLAG_OPEN) //开仓
				{
					if (info->sBsFlag[0] == BS_FLAG_BUY)
					{
						//解冻委托的保证金及费用
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,info->dUnfrozeAmt,true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund 出错");
							return false;
						}
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dUnfrozeFee,true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund 出错");
							return false;
						}
						//更新资金
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_BUY,info->dExMargin+info->dMemberMargin,true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund 出错");
							return false;
						}
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FEE,FUND_CODE_FEE_EXCNANGE,info->dExFee+info->dMemberFee,true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund 出错");
							return false;
						}

					}
					else
					{
						//解冻委托的保证金及费用
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,info->dUnfrozeAmt,true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund 出错");
							return false;
						}
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dUnfrozeFee,true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund 出错");
							return false;
						}
						//更新资金	
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_SELL,info->dExMargin+info->dMemberMargin,true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund 出错");
							return false;
						}
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FEE,FUND_CODE_FEE_EXCNANGE,info->dExFee+info->dMemberFee,true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund 出错");
							return false;
						}
					}
					//更新持仓
					//pPosi = m_pMemDb->m_pDbTrader.m_pPosition.Select(info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo);
					//if (pPosi == NULL)
					//{
					//	S_MDB_CUST_POSITION posi;
					//	bzero(&posi,sizeof(S_MDB_CUST_POSITION));
					//	strcpy(posi.sTxAcc,info->sTxAcc);
					//	strcpy(posi.sSeatNo,info->sSeatNo);
					//	strcpy(posi.sShFlag,info->sShFlag);
					//	strcpy(posi.sTradeCode,info->sTxCode);
					//	posi.nContractNo = info->nContractNo;
					//	strcpy(posi.sContractCode,info->sContractCode);
					//	posi.nVarietyId = info->nVarietyId;

					//	if (info->sBsFlag[0] == BS_FLAG_BUY) //买开
					//	{
					//		posi.dPB_Avp = info->dDonePrice;
					//		posi.dPB_Margin = info->dExMargin+info->dMemberMargin;
					//		posi.nDayPB = info->nDoneNum;
					//		posi.nCurCanUsePB = info->nDoneNum;
					//		posi.nCurPB = info->nDoneNum;
					//		posi.dPB_OAvp = info->dDonePrice;
					//	}
					//	else //卖开
					//	{
					//		posi.dPS_Avp = info->dDonePrice;
					//		posi.dPS_Margin = info->dExMargin+info->dMemberMargin;
					//		posi.nDayPS = info->nDoneNum;
					//		posi.nCurCanUsePS = info->nDoneNum;
					//		posi.nCurPS = info->nDoneNum;
					//		posi.dPS_OAvp = info->dDonePrice;
					//	}
					//	posi.nVarietyId = pc->nProdId;
					//	if (m_pMemDb->m_pDbTrader.m_pPosition.Insert(posi)<0)
					//	{
					//		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"写入持仓表失败 资金账号[%s] 席位[%s] 投保标志[%s] 合约[%d %s] 数量[%d] 开多",\
					//			info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo,info->sContractCode,info->nDoneNum);
					//		return false;
					//	}
					//}
					//else
					//{
					//	//计算持仓均价
					//	if (info->sBsFlag[0] == BS_FLAG_BUY) //买开 多仓
					//	{
					//		//均价
					//		pPosi->dPB_Avp = (pPosi->dPB_Avp*pPosi->nCurPB+info->dDonePrice*info->nDoneNum)/(pPosi->nCurPB+info->nDoneNum);
					//		pPosi->dPB_Margin = pPosi->dPB_Margin + info->dExMargin+info->dMemberMargin;
					//		pPosi->nDayPB = pPosi->nDayPB+info->nDoneNum;
					//		pPosi->nCurCanUsePB = pPosi->nCurCanUsePB+info->nDoneNum;
					//		pPosi->nCurPB = pPosi->nCurPB+info->nDoneNum;
					//		//开仓均价
					//		pPosi->dPB_OAvp = (pPosi->dPB_OAvp*pPosi->nCurPB+info->dDonePrice*info->nDoneNum)/(pPosi->nCurPB+info->nDoneNum);
					//	}
					//	else //卖开 空仓
					//	{
					//		//均价
					//		pPosi->dPS_Avp = (pPosi->dPS_Avp*pPosi->nCurPS+info->dDonePrice*info->nDoneNum)/(pPosi->nCurPS+info->nDoneNum);
					//		pPosi->dPS_Margin = pPosi->dPS_Margin + info->dExMargin+info->dMemberMargin;
					//		pPosi->nDayPS = pPosi->nDayPS+info->nDoneNum;
					//		pPosi->nCurCanUsePS = pPosi->nCurCanUsePS+info->nDoneNum;
					//		pPosi->nCurPS = pPosi->nCurPS+info->nDoneNum;
					//		//开仓均价
					//		pPosi->dPS_OAvp = (pPosi->dPS_OAvp*pPosi->nCurPS+info->dDonePrice*info->nDoneNum)/(pPosi->nCurPS+info->nDoneNum);
					//	}
					//}
				}
				else //平仓 
				{
					//解冻委托冻结的持仓及手续费

					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dUnfrozeFee,true))
					{
						return false;
					}
					pPosi = m_pMemDb->m_pDbTrader.m_pPosition.Select(info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo);
					if (pPosi == NULL)
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"无持仓 资金账号[%s] 席位[%s] 投保标志[%s] 合约[%d %s] 数量[%d] 开多",\
							info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo,info->sContractCode,info->nDoneNum);
						return false;
					}
					//计算持仓
					if (info->sBsFlag[0] == BS_FLAG_BUY) //买平  平空
					{
						//均价不变 保证金按数量比例减小
						//更新保证金
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_SELL,0.00-pPosi->dPS_Margin*(1.00*info->nDoneNum/pPosi->nCurPS),true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund 出错");
							return false;
						}
						//费用
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FEE,FUND_CODE_FEE_EXCNANGE,info->dExFee+info->dMemberFee,true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund 出错");
							return false;
						}
						//取持仓信息
						//pPosi = m_pMemDb->m_pDbTrader.m_pPosition.Select(info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo);
						//if (pPosi == NULL)
						//{
						//	m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"无持仓 资金账号[%s] 席位[%s] 投保标志[%s] 合约[%d %s] 数量[%d] 开多",\
						//		info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo,info->sContractCode,info->nDoneNum);
						//	return false;

						//}
						//if (pPosi->nCurPS<=0)
						//{
						//	m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"平仓成交但找不到持仓 账户[%s] 品种[%d]",info->sTxAcc,pc->nProdId);
						//	return false;
						//}
						//pPosi->dPS_Margin = pPosi->dPS_Margin*(1.00-1.00*info->nDoneNum/pPosi->nCurPS);
						//pPosi->nDAY_COV_PS_Froze = pPosi->nDAY_COV_PS_Froze-info->nDoneNum;
						////冻结减少，可用不变
						////pPosi->nCurCanUsePS = pPosi->nCurCanUsePS-info->nDoneNum;
						//pPosi->nCurPS = pPosi->nCurPS-info->nDoneNum;
						////计算今开持仓
						//if (info->sEoFlag[0] == EO_FLAG_CLOSETODAY) //平今仓
						//{
						//	pPosi->nDayPS = pPosi->nDayPS -  info->nDoneNum;
						//}
						//else //平昨
						//{
						//	if (pPosi->nCurPS < pPosi->nDayPS)
						//	{
						//		pPosi->nDayPS = pPosi->nCurPS;//剩下的全是今持仓了
						//	}
						//}
						////计算平仓盈亏
						//if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_SURPLUS,FUND_CODE_SURPLUS_CLOSE,(pPosi->dPS_Avp-info->dDonePrice)*info->nDoneNum*pc->dTxUnit,true))
						//{
						//	m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
						//	return false;
						//}

					}
					else //卖平 平多
					{
						//均价不变 保证金按数量比例减小
						//更新保证金
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_BUY,0.00-pPosi->dPB_Margin*(1.00*info->nDoneNum/pPosi->nCurPB),true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund 出错");
							return false;
						}
						//费用
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FEE,FUND_CODE_FEE_EXCNANGE,info->dExFee+info->dMemberFee,true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund 出错");
							return false;
						}
						//取持仓信息
						//pPosi = m_pMemDb->m_pDbTrader.m_pPosition.Select(info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo);
						//if (pPosi == NULL)
						//{
						//	m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"无持仓 资金账号[%s] 席位[%s] 投保标志[%s] 合约[%d %s] 数量[%d] 开多",\
						//		info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo,info->sContractCode,info->nDoneNum);
						//	return false;

						//}
						//if (pPosi->nCurPB<=0)
						//{
						//	m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"平仓成交但找不到持仓 账户[%s] 品种[%d]",info->sTxAcc,pc->nProdId);
						//	return false;
						//}
						//pPosi->dPB_Margin = pPosi->dPB_Margin*(1.00-1.00*info->nDoneNum/pPosi->nCurPB);
						////冻结减少，可用不变
						//pPosi->nDAY_COV_PB_Froze = pPosi->nDAY_COV_PB_Froze-info->nDoneNum;
						////pPosi->nCurCanUsePB = pPosi->nCurCanUsePB-info->nDoneNum;
						//pPosi->nCurPB = pPosi->nCurPB-info->nDoneNum;
						////计算今开持仓
						//if (info->sEoFlag[0] == EO_FLAG_CLOSETODAY) //平今仓
						//{
						//	pPosi->nDayPB = pPosi->nDayPB -  info->nDoneNum;
						//}
						//else //平昨
						//{
						//	if (pPosi->nCurPB < pPosi->nDayPB)
						//	{
						//		pPosi->nDayPB = pPosi->nCurPB;//剩下的全是今持仓了
						//	}

						//}
						////计算平仓盈亏
						//if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_SURPLUS,FUND_CODE_SURPLUS_CLOSE,(info->dDonePrice-pPosi->dPB_Avp)*info->nDoneNum*pc->dTxUnit,true))
						//{
						//	m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
						//	return false;
						//}
					}
				}
			}
			if (!UpdatePosition(lockinfo,fundinfo,info,pPosi,pc,false))
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdatePosition 出错 ");
			}
			break;
		case PROD_TYPE_SPOT:
		case PROD_TYPE_SEC:

			//更新持仓
			pPosi = m_pMemDb->m_pDbTrader.m_pPosition.Select(info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo);
			if (info->sBsFlag[0] == BS_FLAG_BUY)
			{
				//解冻委托的保证金及费用
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZE,info->dUnfrozeAmt,true))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund 出错");
					return false;
				}
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dUnfrozeFee,true))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund 出错");
					return false;
				}
				//更新资金
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_INOUT,FUND_CODE_BUY,info->dAmt,true))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund 出错");
					return false;
				}
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FEE,FUND_CODE_FEE_EXCNANGE,info->dExFee+info->dMemberFee,true))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund 出错");
					return false;
				}
				if (pPosi == NULL)
				{
					S_MDB_CUST_POSITION posi;
					bzero(&posi,sizeof(S_MDB_CUST_POSITION));
					strcpy(posi.sTxAcc,info->sTxAcc);
					strcpy(posi.sSeatNo,info->sSeatNo);
					strcpy(posi.sShFlag,info->sShFlag);
					strcpy(posi.sTradeCode,info->sTxCode);
					posi.nContractNo = info->nContractNo;
					strcpy(posi.sContractCode,info->sContractCode);
					posi.nVarietyId = info->nVarietyId;

					posi.dPB_Margin = 0.00-info->dAmt-info->dExFee-info->dMemberFee;
					posi.dPB_Avp = posi.dPB_Margin/info->nDoneNum;//成本均价/每手
					posi.nDayPB = info->nDoneNum;
					posi.nCurCanUsePB = info->nDoneNum;
					posi.nCurPB = info->nDoneNum;
					posi.dPB_OAvp = posi.dPB_Avp;//开仓成本均价 

					posi.nVarietyId = pc->nProdId;
					if (m_pMemDb->m_pDbTrader.m_pPosition.Insert(posi)<0)
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"写入持仓表失败 资金账号[%s] 席位[%s] 投保标志[%s] 合约[%d %s] 数量[%d] 开多",\
							info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo,info->sContractCode,info->nDoneNum);
						return false;
					}
				}
				else
				{
					//持有均价
					pPosi->dPB_Avp = (pPosi->dPB_Avp*pPosi->nCurPB-info->dAmt-info->dExFee-info->dMemberFee)/(pPosi->nCurPB+info->nDoneNum);
					pPosi->dPB_Margin = pPosi->dPB_Margin - info->dAmt-info->dExFee-info->dMemberFee;
					pPosi->nDayPB = pPosi->nDayPB+info->nDoneNum;
					pPosi->nCurCanUsePB = pPosi->nCurCanUsePB+info->nDoneNum;
					pPosi->nCurPB = pPosi->nCurPB+info->nDoneNum;
					//开仓均价
					pPosi->dPB_OAvp = (pPosi->dPB_OAvp*pPosi->nDayPB+info->dAmt+info->dExFee+info->dMemberFee)/(pPosi->nDayPB+info->nDoneNum);

				}
			}
			else  //卖出
			{
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dUnfrozeFee,true))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund 出错");
					return false;
				}
				//更新资金	
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_INOUT,FUND_CODE_SELL,info->dAmt,true))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund 出错");
					return false;
				}
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FEE,FUND_CODE_FEE_EXCNANGE,info->dExFee+info->dMemberFee,true))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund 出错");
					return false;
				}
				if (pPosi == NULL)
				{

					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"写入持仓表失败 资金账号[%s] 席位[%s] 投保标志[%s] 合约[%d %s] 数量[%d] 开多",\
						info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo,info->sContractCode,info->nDoneNum);
					return false;
				}
				//计算持仓均价
				if (pPosi->nCurPB<=0)
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"卖出成交但找不到库存 账户[%s]",info->sTxAcc);
					return false;
				}
				pPosi->dPB_Margin = pPosi->dPB_Margin*(pPosi->nCurPB-info->nDoneNum)/pPosi->nCurPB;
				pPosi->nCurCanUsePB = pPosi->nCurCanUsePB+info->nDoneNum;
				pPosi->nCurPB = pPosi->nCurPB+info->nDoneNum;
				//开仓均价不变
				//将盈亏重新到均价上
				pPosi->dPB_Avp = (pPosi->dPB_OAvp*pPosi->nCurPB+info->dCovSurplus)/pPosi->nCurPB;

				//计算卖出盈亏
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_SURPLUS,FUND_CODE_SURPLUS_CLOSE,(info->dDonePrice-pPosi->dPB_Avp)*info->nDoneNum*pc->dTxUnit,true))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
					return false;
				}
			}
			break;
		case PROD_TYPE_OPTION:
			if (info->sEoFlag[0] == EO_FLAG_OPEN) //开仓
			{
				if (info->sBsFlag[0] == BS_FLAG_BUY)
				{
					//解冻委托的保证金及费用
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,info->dUnfrozeAmt,true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund 出错");
						return false;
					}
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dUnfrozeFee,true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund 出错");
						return false;
					}
					//更新资金
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_PAYRIGHT,info->dAmt,true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund 出错");
						return false;
					}
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FEE,FUND_CODE_FEE_EXCNANGE,info->dExFee+info->dMemberFee,true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund 出错");
						return false;
					}

				}
				else
				{
					//解冻委托的保证金及费用
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,info->dUnfrozeAmt,true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund 出错");
						return false;
					}
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dUnfrozeFee,true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund 出错");
						return false;
					}
					//更新资金	
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_SELL,info->dExMargin+info->dMemberMargin,true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund 出错");
						return false;
					}
					//更新资金
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_GETRIGHT,info->dAmt,true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund 出错");
						return false;
					}
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FEE,FUND_CODE_FEE_EXCNANGE,info->dExFee+info->dMemberFee,true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund 出错");
						return false;
					}
				}
				//更新持仓
				//pPosi = m_pMemDb->m_pDbTrader.m_pPosition.Select(info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo);
				//if (pPosi == NULL)
				//{
				//	S_MDB_CUST_POSITION posi;
				//	bzero(&posi,sizeof(S_MDB_CUST_POSITION));
				//	strcpy(posi.sTxAcc,info->sTxAcc);
				//	strcpy(posi.sSeatNo,info->sSeatNo);
				//	strcpy(posi.sShFlag,info->sShFlag);
				//	strcpy(posi.sTradeCode,info->sTxCode);
				//	posi.nContractNo = info->nContractNo;
				//	strcpy(posi.sContractCode,info->sContractCode);
				//	posi.nVarietyId = info->nVarietyId;

				//	if (info->sBsFlag[0] == BS_FLAG_BUY) //买开
				//	{
				//		posi.dPB_Avp = info->dDonePrice;
				//		posi.dPB_Margin = info->dExMargin+info->dMemberMargin;
				//		posi.nDayPB = info->nDoneNum;
				//		posi.nCurCanUsePB = info->nDoneNum;
				//		posi.nCurPB = info->nDoneNum;
				//		posi.dPB_OAvp = info->dDonePrice;
				//	}
				//	else //卖开
				//	{
				//		posi.dPS_Avp = info->dDonePrice;
				//		posi.dPS_Margin = info->dExMargin+info->dMemberMargin;
				//		posi.nDayPS = info->nDoneNum;
				//		posi.nCurCanUsePS = info->nDoneNum;
				//		posi.nCurPS = info->nDoneNum;
				//		posi.dPS_OAvp = info->dDonePrice;
				//	}
				//	posi.nVarietyId = pc->nProdId;
				//	if (m_pMemDb->m_pDbTrader.m_pPosition.Insert(posi)<0)
				//	{
				//		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"写入持仓表失败 资金账号[%s] 席位[%s] 投保标志[%s] 合约[%d %s] 数量[%d] 开多",\
				//			info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo,info->sContractCode,info->nDoneNum);
				//		return false;
				//	}
				//}
				//else
				//{
				//	//计算持仓均价
				//	if (info->sBsFlag[0] == BS_FLAG_BUY) //买开 多仓
				//	{
				//		//均价
				//		pPosi->dPB_Avp = (pPosi->dPB_Avp*pPosi->nCurPB+info->dDonePrice*info->nDoneNum)/(pPosi->nCurPB+info->nDoneNum);
				//		pPosi->dPB_Margin = pPosi->dPB_Margin + info->dExMargin+info->dMemberMargin;
				//		pPosi->nDayPB = pPosi->nDayPB+info->nDoneNum;
				//		pPosi->nCurCanUsePB = pPosi->nCurCanUsePB+info->nDoneNum;
				//		pPosi->nCurPB = pPosi->nCurPB+info->nDoneNum;
				//		//开仓均价
				//		pPosi->dPB_OAvp = (pPosi->dPB_OAvp*pPosi->nCurPB+info->dDonePrice*info->nDoneNum)/(pPosi->nCurPB+info->nDoneNum);
				//	}
				//	else //卖开 空仓
				//	{
				//		//均价
				//		pPosi->dPS_Avp = (pPosi->dPS_Avp*pPosi->nCurPS+info->dDonePrice*info->nDoneNum)/(pPosi->nCurPS+info->nDoneNum);
				//		pPosi->dPS_Margin = pPosi->dPS_Margin + info->dExMargin+info->dMemberMargin;
				//		pPosi->nDayPS = pPosi->nDayPS+info->nDoneNum;
				//		pPosi->nCurCanUsePS = pPosi->nCurCanUsePS+info->nDoneNum;
				//		pPosi->nCurPS = pPosi->nCurPS+info->nDoneNum;
				//		//开仓均价
				//		pPosi->dPS_OAvp = (pPosi->dPS_OAvp*pPosi->nCurPS+info->dDonePrice*info->nDoneNum)/(pPosi->nCurPS+info->nDoneNum);
				//	}
				//}
			}
			else //平仓
			{
				//解冻委托冻结的持仓及手续费

				if (UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dUnfrozeFee,true))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund 出错");
					return false;
				}
				//取持仓信息
				//pPosi = m_pMemDb->m_pDbTrader.m_pPosition.Select(info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo);
				//if (pPosi == NULL)
				//{
				//	m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"无持仓 资金账号[%s] 席位[%s] 投保标志[%s] 合约[%d %s] 数量[%d] 开多",\
				//		info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo,info->sContractCode,info->nDoneNum);
				//	return false;

				//}
				//计算持仓
				if (info->sBsFlag[0] == BS_FLAG_BUY) //买平  平空
				{
					//均价不变 保证金按数量比例减小
					//更新保证金
					if (!UpdateFund(lockinfo, fundinfo, FUND_TYPE_MARGIN, FUND_CODE_MARGIN_POSI_SELL, pPosi->dPS_Margin * (1.00 * info->nDoneNum / pPosi->nCurPS), true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "UpdateFund 出错");
						return false;
					}
					//费用
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FEE,FUND_CODE_FEE_EXCNANGE,info->dExFee+info->dMemberFee,true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund 出错");
						return false;
					}
					if (pPosi->nCurPS<=0)
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"平仓成交但找不到库存 账户[%s]",info->sTxAcc);
						return false;
					}
					//pPosi->dPS_Margin = pPosi->dPS_Margin*(1.00-1.00*info->nDoneNum/pPosi->nCurPS);
					//pPosi->nDAY_COV_PS_Froze = pPosi->nDAY_COV_PS_Froze-info->nDoneNum;
					////冻结减少，可用不变
					////pPosi->nCurCanUsePS = pPosi->nCurCanUsePS-info->nDoneNum;
					//pPosi->nCurPS = pPosi->nCurPS-info->nDoneNum;
					////计算今开持仓
					//if (info->sEoFlag[0] == EO_FLAG_CLOSETODAY) //平今仓
					//{
					//	pPosi->nDayPS = pPosi->nDayPS -  info->nDoneNum;
					//}
					//else //平昨
					//{
					//	if (pPosi->nCurPS < pPosi->nDayPS)
					//	{
					//		pPosi->nDayPS = pPosi->nCurPS;//剩下的全是今持仓了
					//	}

					//}
					////计算平仓盈亏
					//if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_SURPLUS,FUND_CODE_SURPLUS_CLOSE,(pPosi->dPS_Avp-info->dDonePrice)*info->nDoneNum*pc->dTxUnit,true))
					//{
					//	m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
					//	return false;
					//}
				}
				else //卖平 平多
				{
					////均价不变 保证金按数量比例减小
					////更新保证金
					//if (!UpdateFund(lockinfo, fundinfo, FUND_TYPE_MARGIN, FUND_CODE_MARGIN_POSI_BUY, pPosi->dPB_Margin * (1.00 * info->nDoneNum / pPosi->nCurPB), true))
					//{
					//	m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "UpdateFund 出错");
					//	return false;
					//}
					//费用
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FEE,FUND_CODE_FEE_EXCNANGE,info->dExFee+info->dMemberFee,true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund 出错");
						return false;
					}
					if (pPosi->nCurPB<=0)
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"平仓成交但找不到库存 账户[%s]",info->sTxAcc);
						return false;
					}
					//pPosi->dPB_Margin = pPosi->dPB_Margin*(1.00-1.00*info->nDoneNum/pPosi->nCurPB);
					////冻结减少，可用不变
					//pPosi->nDAY_COV_PB_Froze = pPosi->nDAY_COV_PB_Froze-info->nDoneNum;
					////pPosi->nCurCanUsePB = pPosi->nCurCanUsePB-info->nDoneNum;
					//pPosi->nCurPB = pPosi->nCurPB-info->nDoneNum;
					////计算今开持仓
					//if (info->sEoFlag[0] == EO_FLAG_CLOSETODAY) //平今仓
					//{
					//	pPosi->nDayPB = pPosi->nDayPB -  info->nDoneNum;
					//}
					//else //平昨
					//{
					//	if (pPosi->nCurPB < pPosi->nDayPB)
					//	{
					//		pPosi->nDayPB = pPosi->nCurPB;//剩下的全是今持仓了
					//	}

					//}
					////计算平仓盈亏
					//if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_SURPLUS,FUND_CODE_SURPLUS_CLOSE,(info->dDonePrice-pPosi->dPB_Avp)*info->nDoneNum*pc->dTxUnit,true))
					//{
					//	m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
					//	return false;
					//}
				}
			}
			if (!UpdatePosition(lockinfo,fundinfo,info,pPosi,pc,false))
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdatePosition 出错 ");
			}
			break;
		default:
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"合约类型[%c] 不符 ",pc->sProd_type[0]);
			return false;
	}
	
	//更新报单同步标志
	if (po->nSyncFlag == 2)
	{
		po->nSyncFlag = 4;//撤单成交同步
	}
	else
	{
		po->nSyncFlag = 3;//成交同步
	}
	return true;
}
int CKernelPub::ProceeOneCancelOrder(S_MDB_TXACC_LOCK *pAcclock,S_MDB_ORDER_INFO *po,int nCancelNum )
{
	//加锁
	CBF_PMutex pp(pAcclock->mutex);

	if (po->sCancelFlag[0] == BOOL_TRUE)
	{
		//报单已撤
		return 0;
	}


	po->nCancelNum = nCancelNum;
	po->nRemainNum = 0;
	po->sCancelFlag[0] = BOOL_TRUE;
	po->sCancelFlag[1] = 0;
	//报单状态
	if (po->nCancelNum == po->nOrderNum)
	{
		po->sOrderStatus[0] = ORDER_STATUS_CANCEL;//全撤
	}
	else
	{
		po->sOrderStatus[0] = ORDER_STATUS_PARTCANCEL;//部成部撤
	}
	po->sOrderStatus[1] = 0;
	m_pDate.Update();
	strcpy(po->sCancelTime,m_pDate.ToStringDT17().c_str());
	if (po->nSyncFlag == 3 )
	{
		po->nSyncFlag = 4;//成交和撤单
	}
	else
	{
		po->nSyncFlag = 2;//撤单
	}

	//解冻资金或持仓
	S_MDB_CONTRACT_INFO *pc = m_pMemDb->m_pDbBaseinfo.m_pContract.Select(po->nContractNo);
	if (pc == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"撤单通知 合约[%d]找不到",po->nContractNo);
		return -1;
	}

	S_MDB_CUST_FUND *pFund = m_pMemDb->GetFund(pAcclock,pc->sCurrency);
	if (pFund == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"撤单通知 资金账号[%s] 币种[%s] 在资金表找不到",po->sTxAcc,pc->sCurrency);
		return -1;
	}  
	S_MDB_ORDER_INFO *info = po;
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"处理撤单  账户[%s]合约编号[%d %s]  开平[%s] 买卖[%s] 投保[%s] 本地报单号[%s] 冻结[%lf] 委托量[%d] 剩余量[%d]",\
		info->sTxAcc,info->nContractNo,info->sContractCode,info->sEoFlag,info->sBsFlag,info->sShFlag,info->sLocalSerial,info->dFrozAmt,info->nOrderNum,info->nRemainNum);

	S_MDB_CUST_POSITION *pPosi=NULL;
	
	S_MDB_BIG_MARGIN *pbm=NULL;
	int nMarginMode=0;
	double dTmp=0.0;
	switch (pc->sProd_type[0])
	{
		case PROD_TYPE_FUTURES://期货
			nMarginMode = GetMarginMode(pc);
			if (nMarginMode<0)
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"合约[%d %s] 取保证金模式出错 ",pc->nContractId,pc->sContractCode);
				return false;
			}
			if (nMarginMode == 2) //大边保证金
			{
				if (po->sEoFlag[0] == EO_FLAG_OPEN) //开仓
				{
					if (po->sBsFlag[0] == BS_FLAG_BUY) //买开 多仓
					{
						//查找表
						pbm = m_pMemDb->m_pDbTrader.m_pBigNetMargin.Select(po->sTxAcc,pc->nProdId);
						if (pbm == NULL)
						{
							//撤单但找不到记录
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"撤单但大边记录找不到[%s %d]的记录",po->sTxAcc,pc->nProdId);
							return false;
						}
						//按合约的比例释放
						dTmp =  po->dFrozAmt*(1.00*nCancelNum/po->nOrderNum);
						pbm->dBuyFroze = pbm->dBuyFroze - dTmp;
						pbm->nBuyQty -= nCancelNum;
						if (pbm->nFrozeFlag == 1)
						{
							//将原来的释放
							if (!UpdateFund(pAcclock,pFund,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,0.00-pbm->dFrozeAmt,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
								return false;
							}
						}
						else
						{
							//将原来的释放
							if (!UpdateFund(pAcclock,pFund,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,0.00-pbm->dFrozeAmt,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
								return false;
							}
						}
						dTmp = pbm->dBuyMargin + pbm->dBuyFroze - pbm->dSellMargin - pbm->dSellFroze;
						if (dTmp > MINDOUBLECOMPARE) //大于等于0
						{
							pbm->nFrozeFlag = 1; //冻结的在多
							if (pbm->nMarginFlag == 1) //持有的在多
							{
								//pbm->dFrozeAmt  = pbm->dBuyMargin+pbm->dBuyFroze-pbm->dBuyMargin;
								pbm->dFrozeAmt =pbm->dBuyFroze;//同上面一样，少了运算
							}
							else //持有的在空
							{
								//现有(冻结+持有) 减去占有，即为该再去冻结的
								pbm->dFrozeAmt  = pbm->dBuyMargin+pbm->dBuyFroze-pbm->dSellMargin;
							}
							m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"资金账号[%s]品种[%d] 大边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 冻结[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
							//进行新的冻结
							if (!UpdateFund(pAcclock,pFund,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,pbm->dFrozeAmt,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
								return false;
							}
						}
						else
						{
							pbm->nFrozeFlag = 2;
							if (pbm->nMarginFlag == 1) //持有的在多
							{
								pbm->dFrozeAmt  = pbm->dSellMargin+pbm->dSellFroze-pbm->dBuyMargin;
							}
							else //持有的在空
							{
								//现有(冻结+持有) 减去占有，即为该再去冻结的
								//pbm->dFrozeAmt  = pbm->dSellMargin+pbm->dSellFroze-pbm->dSellMargin;
								pbm->dFrozeAmt = pbm->dSellFroze;//同上面一样，少了运算
							}
							m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"资金账号[%s]品种[%d] 大边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 冻结[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
							//进行新的冻结
							if (!UpdateFund(pAcclock,pFund,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,pbm->dFrozeAmt,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
								return false;
							}
						}
					}
					else //卖开 空仓
					{
						//查找表
						pbm = m_pMemDb->m_pDbTrader.m_pBigNetMargin.Select(po->sTxAcc,pc->nProdId);
						if (pbm == NULL)
						{
							//撤单但找不到记录
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"撤单但大边记录找不到[%s %d]的记录",po->sTxAcc,pc->nProdId);
							return false;
						}
						//按合约的比例释放
						dTmp =  po->dFrozAmt*(1.00*nCancelNum/po->nOrderNum);
						pbm->dSellFroze = pbm->dSellFroze - dTmp;
						pbm->nSellQty -= nCancelNum;
						if (pbm->nFrozeFlag == 1)
						{
							//将原来的释放
							if (!UpdateFund(pAcclock,pFund,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,0.00-pbm->dFrozeAmt,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
								return false;
							}
						}
						else
						{
							//将原来的释放
							if (!UpdateFund(pAcclock,pFund,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,0.00-pbm->dFrozeAmt,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
								return false;
							}
						}
						dTmp = pbm->dBuyMargin + pbm->dBuyFroze - pbm->dSellMargin - pbm->dSellFroze;
						if (dTmp > MINDOUBLECOMPARE) //大于等于0
						{
							pbm->nFrozeFlag = 1; //冻结的在多
							if (pbm->nMarginFlag == 1) //持有的在多
							{
								//pbm->dFrozeAmt  = pbm->dBuyMargin+pbm->dBuyFroze-pbm->dBuyMargin;
								pbm->dFrozeAmt =pbm->dBuyFroze;//同上面一样，少了运算
							}
							else //持有的在空
							{
								//现有(冻结+持有) 减去占有，即为该再去冻结的
								pbm->dFrozeAmt  = pbm->dBuyMargin+pbm->dBuyFroze-pbm->dSellMargin;
							}
							m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"资金账号[%s]品种[%d] 大边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 冻结[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
							//进行新的冻结
							if (!UpdateFund(pAcclock,pFund,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,pbm->dFrozeAmt,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
								return false;
							}
						}
						else
						{
							pbm->nFrozeFlag = 2;
							if (pbm->nMarginFlag == 1) //持有的在多
							{
								pbm->dFrozeAmt  = pbm->dSellMargin+pbm->dSellFroze-pbm->dBuyMargin;
							}
							else //持有的在空
							{
								//现有(冻结+持有) 减去占有，即为该再去冻结的
								//pbm->dFrozeAmt  = pbm->dSellMargin+pbm->dSellFroze-pbm->dSellMargin;
								pbm->dFrozeAmt = pbm->dSellFroze;//同上面一样，少了运算
							}
							m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"资金账号[%s]品种[%d] 大边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 冻结[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
							//进行新的冻结
							if (!UpdateFund(pAcclock,pFund,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,pbm->dFrozeAmt,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
								return false;
							}
						}
					}
					//解冻剩余的手续费
					if (!UpdateFund(pAcclock,pFund,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,(po->dExFee*nCancelNum/po->nOrderNum),true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund 出错");
						return -1;
					}
					
				}
				else //平仓  
				{
					//解冻剩余的手续费和持仓
					pPosi = m_pMemDb->m_pDbTrader.m_pPosition.Select(po->sTxAcc,po->sSeatNo,po->sShFlag,po->nContractNo);
					if (pPosi == NULL)
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"资金账号[%s] 平仓时 席位[%s] 投保标志[%s] 合约[%d %s]无持仓",po->sTxAcc,po->sSeatNo,po->sShFlag,po->nContractNo,po->sContractCode);
						return -1;
					}
					//解冻结报单数量
					if (po->sBsFlag[0] == BS_FLAG_BUY) //买平 平空仓
					{
						//可用空仓增加
						pPosi->nCurCanUsePS = pPosi->nCurCanUsePS + nCancelNum;
						//冻结空仓减少
						pPosi->nDAY_COV_PS_Froze = pPosi->nDAY_COV_PS_Froze - nCancelNum;
					}
					else //卖平 平多仓
					{
						//可用多仓增加
						pPosi->nCurCanUsePB = pPosi->nCurCanUsePB + nCancelNum;
						//冻结多仓减少
						pPosi->nDAY_COV_PB_Froze = pPosi->nDAY_COV_PB_Froze - nCancelNum;
					}
					//解冻费用
					if (!UpdateFund(pAcclock,pFund,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,po->dExFee*nCancelNum/po->nOrderNum,true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"撤单更新资金出错 ");
						return -1;
					}
				}
			}
			else if (nMarginMode == 1) //净边保证金
			{
				if (!ProcCancelOrderNetMargin(pAcclock,pFund,po,pc))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"撤单 净边处理出错 ");
					return -1;
				}
			}
			else
			{
				if (po->sEoFlag[0] == EO_FLAG_OPEN) //开仓
				{
					//解冻剩余的手续费和保证金
					if (!UpdateFund(pAcclock,pFund,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,(0.00-po->dFrozAmt*nCancelNum/po->nOrderNum),true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund 出错");
						return -1;
					}
					if (!UpdateFund(pAcclock,pFund,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,(po->dExFee*nCancelNum/po->nOrderNum),true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund 出错");
						return -1;
					}
				}
				else
				{
					//解冻剩余的手续费和持仓
					pPosi = m_pMemDb->m_pDbTrader.m_pPosition.Select(po->sTxAcc,po->sSeatNo,po->sShFlag,po->nContractNo);
					if (pPosi == NULL)
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"资金账号[%s] 平仓时 席位[%s] 投保标志[%s] 合约[%d %s]无持仓",po->sTxAcc,po->sSeatNo,po->sShFlag,po->nContractNo,po->sContractCode);
						return -1;
					}
					//解冻结报单数量
					if (po->sBsFlag[0] == BS_FLAG_BUY) //买平 平空仓
					{
						//可用空仓增加
						pPosi->nCurCanUsePS = pPosi->nCurCanUsePS + nCancelNum;
						//冻结空仓减少
						pPosi->nDAY_COV_PS_Froze = pPosi->nDAY_COV_PS_Froze - nCancelNum;
					}
					else //卖平 平多仓
					{
						//可用多仓增加
						pPosi->nCurCanUsePB = pPosi->nCurCanUsePB + nCancelNum;
						//冻结多仓减少
						pPosi->nDAY_COV_PB_Froze = pPosi->nDAY_COV_PB_Froze - nCancelNum;
					}
					//解冻费用
					if (!UpdateFund(pAcclock,pFund,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,(po->dExFee*nCancelNum/po->nOrderNum),true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"撤单更新资金出错 ");
						return -1;
					}
				}
			}
			
			break;
		case PROD_TYPE_OPTION://期权
			if (po->sEoFlag[0] == EO_FLAG_OPEN) //开仓
			{
				//解冻资金
				if (po->sBsFlag[0] == BS_FLAG_BUY) //买开
				{
					//解冻权利金
					if (!UpdateFund(pAcclock,pFund,FUND_TYPE_FROZE,FUND_CODE_BUYFROZE,(0.00-po->dAmt*nCancelNum/po->nOrderNum),true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"撤单更新资金出错");
						return -1;
					}
					//冻手续费
					if (!UpdateFund(pAcclock,pFund,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,(0.00-po->dExFee*nCancelNum/po->nOrderNum),true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"撤单更新资金出错 ");
						return -1;
					}
				}
				else  //卖开
				{

					//解冻结卖出保证金
					if (!UpdateFund(pAcclock,pFund,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,(0.00-po->dFrozAmt*nCancelNum/po->nOrderNum),true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"撤单更新资金出错");
						return -1;
					}
					//解冻结手续费
					if (!UpdateFund(pAcclock,pFund,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,(po->dExFee*nCancelNum/po->nOrderNum),true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"撤单更新资金出错");
						return -1;
					}
				}
			}
			else //平
			{
				//解冻持仓
				pPosi = m_pMemDb->m_pDbTrader.m_pPosition.Select(po->sTxAcc,po->sSeatNo,po->sShFlag,po->nContractNo);
				if (pPosi == NULL)
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"资金账号[%s] 平仓时 席位[%s] 投保标志[%s] 合约[%d %s]无持仓",po->sTxAcc,po->sSeatNo,po->sShFlag,po->nContractNo,po->sContractCode);
					return -1;
				}
				//只冻剩余数量
				if (po->sBsFlag[0] == BS_FLAG_BUY) //买平 平空仓
				{
					//解冻权利金
					if (!UpdateFund(pAcclock,pFund,FUND_TYPE_FROZE,FUND_CODE_BUYFROZE,(0.00-po->dAmt*nCancelNum/po->nOrderNum),true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"撤单更新资金出错");
						return -1;
					}
					//空仓可用增加
					pPosi->nCurCanUsePS = pPosi->nCurCanUsePS +nCancelNum;
					//冻结减少
					pPosi->nDAY_COV_PS_Froze = pPosi->nDAY_COV_PS_Froze - nCancelNum;
				}
				else //卖平 平多仓
				{
					//多仓可用增加
					pPosi->nCurCanUsePB = pPosi->nCurCanUsePB + nCancelNum;
					//冻结减少
					pPosi->nDAY_COV_PB_Froze = pPosi->nDAY_COV_PB_Froze - nCancelNum;
				}
				//冻费用
				if (!UpdateFund(pAcclock,pFund,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,(0.00-po->dExFee*nCancelNum/po->nOrderNum),true))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"撤单更新资金出错");
					return -1;
				}

			}
			break;
		case PROD_TYPE_SPOT://现货
		case PROD_TYPE_SEC://证券
			if (po->sBsFlag[0] == BS_FLAG_BUY) //买 
			{
				//解买冻结
				if (!UpdateFund(pAcclock,pFund,FUND_TYPE_FROZE,FUND_CODE_BUYFROZE,(0.00-po->dAmt*nCancelNum/po->nOrderNum),true))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"撤单更新资金出错");
					return -1;
				}
				if (!UpdateFund(pAcclock,pFund,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,(po->dExFee*nCancelNum/po->nOrderNum),true))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"撤单更新资金出错");
					return -1;
				}
			}
			else
			{
				//解冻持仓库存
				pPosi = m_pMemDb->m_pDbTrader.m_pPosition.Select(po->sTxAcc,po->sSeatNo,po->sShFlag,po->nContractNo);
				if (pPosi == NULL)
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"资金账号[%s] 平仓时 席位[%s] 投保标志[%s] 合约[%d %s]无持仓",po->sTxAcc,po->sSeatNo,po->sShFlag,po->nContractNo,po->sContractCode);
					return -1;
				}
				//可用多仓增加
				pPosi->nCurCanUsePB = pPosi->nCurCanUsePB +nCancelNum;
				//冻结多仓减少
				pPosi->nDAY_COV_PB_Froze = pPosi->nDAY_COV_PB_Froze -nCancelNum;
				//冻费用
				if (!UpdateFund(pAcclock,pFund,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,(0.00-po->dExFee*nCancelNum/po->nOrderNum),true))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"撤单更新资金出错");
					return -1;
				}
			}

			break;
		default:
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托 合约代码[%s]的品种类别[%s]不符",po->sContractCode,pc->sProd_type);
			return -1;
	}
	return 0;
}

bool CKernelPub::CheckOperAccRight(const char *oper, const char *acc)
{
	std::vector<S_MDB_OPER_CUST_INFO *> oclist;
	if (m_pMemDb->m_pDbBaseinfo.m_pOperTxAcc.Select((char *)oper,oclist))
	{
		 //有设置操作员对应账号,以此为准，否则以机构来判断
		S_MDB_OPER_CUST_INFO *poc= m_pMemDb->m_pDbBaseinfo.m_pOperTxAcc.Select((char *)oper,(char *)acc);
		if (poc != NULL)
		{
			return true;
		}
		return false;
	}
	S_MDB_OPER_INFO *poper = m_pMemDb->m_pDbPublic.m_pOper.Select(oper);
	if (poper == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"操作员[%s]在操作员表无记录",oper);
		return false;
	}
	S_MDB_CUST_FUNDACC_INFO *pacc = m_pMemDb->m_pDbCust.m_pCustFundAcc.SelectByAcc((char *)acc);
	if (pacc == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"资金账号[%s]在资金账号表无记录",acc);
		return false;
	}
	S_MDB_ORG_INFO *operorg = m_pMemDb->m_pDbPublic.m_pOrg.Select(poper->sOrgCode);
	if (operorg == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"操作员[%s]的机构[%s]在机构表无记录",oper,poper->sOrgCode);
		return false;
	}

	S_MDB_ORG_INFO *accorg = m_pMemDb->m_pDbPublic.m_pOrg.Select(pacc->sSignOrg);
	if (accorg == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"资金账号[%s]的机构[%s]在机构表无记录",acc,pacc->sSignOrg);
		return false;
	}
	if (strcmp(operorg->sUpOrg,"NULL")==0) //操作员是顶级机构，肯定有权限
	{
		return true;
	}
	if (strcmp(accorg->sUpOrg,"NULL")==0) //账户为顶级机构，到这里肯定没有权限
	{
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"操作员机构[%s] 对账户机构[%s]无权限 UpOrg[%s]",operorg->sOrgCode,accorg->sOrgCode,accorg->sUpOrg);
		return false;
	}
	//比较上级机构+当前机构，如果账户的机构大于等于则有权限
	char soperorg[300];
	char scustorg[300];
	sprintf(soperorg,"%s|%s",operorg->sUpOrg,operorg->sOrgCode);
	sprintf(scustorg,"%s|%s",accorg->sUpOrg,accorg->sOrgCode);
	if (strncmp(soperorg,scustorg,strlen(soperorg)) == 0 )
	{
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"操作员机构[%s] 对账户机构[%s]有权限 操作员Org[%s] 账户Org[%s]",operorg->sOrgCode,accorg->sOrgCode,soperorg,scustorg);
		return true;
	}
	return false;
}
bool CKernelPub::CheckOperOrgRight(const char *oper, const char *org)
{
	
	S_MDB_OPER_INFO *poper = m_pMemDb->m_pDbPublic.m_pOper.Select(oper);
	if (poper == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"操作员[%s]在操作员表无记录",oper);
		return false;
	}

	S_MDB_ORG_INFO *operorg = m_pMemDb->m_pDbPublic.m_pOrg.Select(poper->sOrgCode);
	if (operorg == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"操作员[%s]的机构[%s]在机构表无记录",oper,poper->sOrgCode);
		return false;
	}
	S_MDB_ORG_INFO *orginfo = m_pMemDb->m_pDbPublic.m_pOrg.Select((char *)org);
	if (operorg == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"机构[%s]在机构表无记录",org);
		return false;
	}
	if (strcmp(operorg->sUpOrg,"NULL")==0) //操作员是顶级机构，肯定有权限
	{
		return true;
	}
	if (strcmp(orginfo->sUpOrg,"NULL")==0) //账户为顶级机构，到这里肯定没有权限
	{
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"操作员机构[%s] 对机构[%s]无权限 UpOrg[%s]",operorg->sOrgCode,orginfo->sOrgCode,orginfo->sUpOrg);
		return false;
	}
	//比较上级机构+当前机构，如果账户的机构大于等于则有权限
	char soperorg[300];
	char scustorg[300];
	sprintf(soperorg,"%s|%s",operorg->sUpOrg,operorg->sOrgCode);
	sprintf(scustorg,"%s|%s",orginfo->sUpOrg,orginfo->sOrgCode);
	if (strncmp(soperorg,scustorg,strlen(soperorg)) == 0 )
	{
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"操作员机构[%s] 对账户机构[%s]有权限 操作员Org[%s] 机构Org[%s]",operorg->sOrgCode,orginfo->sOrgCode,soperorg,scustorg);
		return true;
	}
	return false;
}


bool CKernelPub::CheckOperProdRight(const char *oper, const char *prod)
{
	S_MDB_OPER_INFO *poper = m_pMemDb->m_pDbPublic.m_pOper.Select(oper);
	if (poper == NULL)
	{
		poper = m_pMemDb->m_pDbPublic.m_pOper.At(0);
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"操作员[%s]在操作员表无记录",oper);
		return false;
	}
	S_MDB_FINA_PROD_INFO *pProd = m_pMemDb->m_pDbBaseinfo.m_pProd.Select((char *)prod);
	if (pProd == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"产品[%s]在产品表无记录",prod);
		return false;
	}
	S_MDB_ORG_INFO *operorg = m_pMemDb->m_pDbPublic.m_pOrg.Select(poper->sOrgCode);
	if (operorg == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"操作员[%s]的机构[%s]在机构表无记录",oper,poper->sOrgCode);
		return false;
	}
	if (strcmp(operorg->sUpOrg,"NULL")==0 || operorg->sOrgLevel[0] == '0')
	{
		//顶级机构，肯定有权限
		return true;
	}
	S_MDB_ORG_INFO *prodorg = m_pMemDb->m_pDbPublic.m_pOrg.Select(pProd->sOrgCode);
	if (prodorg == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"产品[%s]的机构[%s]在机构表无记录",prod,pProd->sOrgCode);
		return false;
	}

	if (strcmp(prodorg->sUpOrg,"NULL")==0) //账户为顶级机构，到这里肯定没有权限
	{
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"操作员机构[%s] 对机构[%s]无权限 UpOrg[%s]",operorg->sOrgCode,prodorg->sOrgCode,prodorg->sUpOrg);
		return false;
	}
	//比较上级机构+当前机构，如果账户的机构大于等于则有权限
	char soperorg[300];
	char scustorg[300];
	sprintf(soperorg,"%s|%s",operorg->sUpOrg,operorg->sOrgCode);
	sprintf(scustorg,"%s|%s",prodorg->sUpOrg,prodorg->sOrgCode);
	if (strncmp(soperorg,scustorg,strlen(soperorg)) == 0 )
	{
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"操作员机构[%s] 对机构[%s]有权限 操作员Org[%s] 产品Org[%s]",operorg->sOrgCode,prodorg->sOrgCode,soperorg,scustorg);
		return true;
	}
	return false;
}


bool CKernelPub::GetTradeFeeValue(S_MDB_CONTRACT_INFO *pc, S_MDB_CUST_FUNDACC_INFO *pcust,int flag ,int num, double amt, double &exfee, double &memberfee)
{
	unsigned int nProdId = pc->nProdId;
	if (pc->sProd_type[0] == PROD_TYPE_OPTION)
	{
		S_MDB_CONTRACT_INFO *pcd = m_pMemDb->m_pDbBaseinfo.m_pContract.Select(nProdId);
		if (pcd == NULL)
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"期权合约[%d %s]的标的合约[%d]不存在",pc->nContractId,pc->sContractCode,nProdId);
			return false;
		}
		nProdId = pcd->nProdId;//这里为期权合约的标的合约对应的品种
	}
	char sFeeNo[4];
	switch (flag)
	{
		case 1:
			strcpy(sFeeNo,"101");//买入开仓
			break;
		case 2:
			strcpy(sFeeNo,"102");//卖出平仓
			break;
		case 3:
			strcpy(sFeeNo,"103");//平今仓
			break;
		default:
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"报单类型[%d]输入不符",flag);
			return false;
	}
	//先取交易所的费率
	std::vector<S_MDB_FEE_TEMPLET_DETAIL *> exfeelist;
	if (!m_pMemDb->m_pDbSet.m_pFeeTempletDetail.Select(pcust->sExchFareModel,pc->sProd_type,nProdId,pc->nContractId,sFeeNo,exfeelist))
	{
		//按合约找不到再按品种找
		if (!m_pMemDb->m_pDbSet.m_pFeeTempletDetail.Select(pcust->sExchFareModel,pc->sProd_type,nProdId,0,sFeeNo,exfeelist))
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"取交易所手续费 交易所模板[%s]合约[%d %s] 品种[%s %d] 找不到配置",pcust->sExchFareModel,pc->nContractId,pc->sContractCode,pc->sProd_type,nProdId);
			return false;
		}
	}
	//if (exfeelist.size()>1)
	//{
	//	m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"交易所手续费 交易所模板[%s]合约[%d %s] 品种[%s %d] 有多个配置",pcust->sExchFareModel,pc->nContractId,pc->sContractCode,pc->sProd_type,nProdId);
	//	return false;
	//}
	int i;
	double dTmp;
	// #define  FEE_MODEL_DONERATE   '1'		        //成交金额比率(不含交易所)
	// #define  FEE_MODEL_WEIGHT     '2'		        //重量(元/克)(不含交易所)
	// #define  FEE_MODEL_HAND       '3'		        //手数(元/手)(不含交易所)
	// #define  FEE_MODEL_EXCH_RATE   '4'		        //交易所手续费比率(在交易所费率上乘以，不含交易所)
	// #define  FEE_MODEL_EXCH_RATE_INCLUDE   '5'		//交易所手续费比率(在交易所费率上乘以，包含交易所)
	//根据收费模式，计算手续费
	exfee= 0.0;
	for (i=0 ; i< exfeelist.size(); i++)
	{
		switch (exfeelist[0]->sFareMode[0])
		{
			case FEE_MODEL_DONERATE:
				dTmp = amt*exfeelist[0]->dValue;
				break;
			case FEE_MODEL_WEIGHT:
				dTmp =  exfeelist[0]->dValue*num*pc->dTxUnit;//手数*合约乘数即为总重量，乘上价格即为金额
				break;
			case FEE_MODEL_HAND:
				dTmp =  exfeelist[0]->dValue*num;//每手多少
				break;
			default:
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"交易所手续费 交易所模板[%s]合约[%d %s] 品种[%d] 的收费模式[%s]不符",pcust->sExchFareModel,pc->nContractId,pc->sContractCode,nProdId,exfeelist[0]->sFareMode);
				return false;
		}
		exfee += dTmp;
	}
	//取会员的手续费
	std::vector<S_MDB_FEE_TEMPLET_DETAIL *> memberfeelist;
	std::vector<S_MDB_CUST_FEE *> reslist;

	bool issetmember=true;//是否设置了会员手续费率，未设默认为0
	bool iscust=true;//是否特殊

	//先从客户特殊费率表里面取
	if (!m_pMemDb->m_pDbSet.m_pCustFeeDetail.Select(pcust->sTxAcc,pc->sProd_type,nProdId,pc->nContractId,sFeeNo,reslist))
	{
		//按合约没取到，按品种再取，即合约为0
		if (!m_pMemDb->m_pDbSet.m_pCustFeeDetail.Select(pcust->sTxAcc,pc->sProd_type,nProdId,0,sFeeNo,reslist))
		{
			//没有找到，从费率模板表里取
			iscust = false;
			if (strlen(pcust->sFsFeeModel)>0) //设了旧的手续费模板
			{
				if (!m_pMemDb->m_pDbSet.m_pFeeTempletDetail.Select(pcust->sFsFeeModel,pc->sProd_type,pc->nProdId,pc->nContractId,sFeeNo,memberfeelist))
				{
					//按合约找不到再按品种找
					if (!m_pMemDb->m_pDbSet.m_pFeeTempletDetail.Select(pcust->sFsFeeModel,pc->sProd_type,pc->nProdId,0,sFeeNo,memberfeelist))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"取会员手续费 fs手续模板[%s]合约[%d %s] 品种[%s %d] 找不到配置",pcust->sFsFeeModel,pc->nContractId,pc->sContractCode,pc->sProd_type,nProdId);
						issetmember = false;
						//return false;
					}
				}
			}
			else
			{
				if (!m_pMemDb->m_pDbSet.m_pFeeTempletDetail.Select(pcust->sMemberFareModel,pc->sProd_type,pc->nProdId,pc->nContractId,sFeeNo,memberfeelist))
				{
					//按合约找不到再按品种找
					if (!m_pMemDb->m_pDbSet.m_pFeeTempletDetail.Select(pcust->sMemberFareModel,pc->sProd_type,pc->nProdId,0,sFeeNo,memberfeelist))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"取会员手续费 会员手续模板[%s]合约[%d %s] 品种[%s %d] 找不到配置",pcust->sMemberFareModel,pc->nContractId,pc->sContractCode,pc->sProd_type,nProdId);
						issetmember = false;
						//return false;
					}
				}	
			}
		}
	}
	if (!issetmember)
	{
		memberfee= 0.0;
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"未设会员手续费率，默认会员不收");
		return true;
	}

	bool isinclude=false;
	if (iscust)
	{
		memberfee = 0.0;
		//特殊费率表单独一个字段保存是否包含交易所
		for (i=0 ; i< reslist.size(); i++)
		{
			if (reslist[i]->sInclude[0] == '1')
			{
				//有一个包含的，统一以包含算
				isinclude = true;
				break;
			}
		}
		//根据收费模式，计算手续费
		for (i=0 ; i< reslist.size(); i++)
		{
			switch (reslist[i]->sFareMode[0])
			{
			case FEE_MODEL_DONERATE:
				if (isinclude) //包含交易所
				{
					if (reslist[i]->sInclude[0] == '1')
					{
						//本设置为包含
						dTmp = amt*reslist[i]->dValue;
					}
					else //本设置为不包含，要加上交易所
					{
						dTmp = exfee+ amt*reslist[i]->dValue;
					}
				}
				else //不包含，就以设置的为准
				{
					dTmp = amt*reslist[i]->dValue;
				}
				break;
			case FEE_MODEL_WEIGHT:
				if (isinclude) //包含交易所
				{
					if (reslist[i]->sInclude[0] == '1')
					{
						//本设置包含
						dTmp = reslist[i]->dValue*num*pc->dTxUnit;//手数*合约乘数即为总重量，乘上价格即为金额
					}
					else //本设置为不包含，要加上交易所
					{
						dTmp = exfee+ reslist[i]->dValue*num*pc->dTxUnit;//手数*合约乘数即为总重量，乘上价格即为金额
					}
				}
				else //不包含，就以设置的为准
				{
					dTmp =  reslist[i]->dValue*num*pc->dTxUnit;//手数*合约乘数即为总重量，乘上价格即为金额
				}
				break;
			case FEE_MODEL_HAND:
				if (isinclude) //包含交易所
				{
					if (reslist[i]->sInclude[0] == '1')
					{
						//本设置为包含
						dTmp =  reslist[i]->dValue*num;//每手多少
					}
					else //本设置为不包含，要加上交易所
					{
						dTmp = exfee + reslist[i]->dValue*num;//每手多少
					}
				}
				else //不包含，就以设置的为准
				{
					dTmp =  reslist[i]->dValue*num;//每手多少
				}
				break;
			case FEE_MODEL_EXCH_RATE:
				if (isinclude) //包含交易所
				{
					if (reslist[i]->sInclude[0] == '1') //本设置为包含
					{
						//忽略标志，以此值为准
						m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"设置出错了,是否包含交易所标志为1,但值为不含交易所,以值为准");
						dTmp = exfee*reslist[i]->dValue;
					}
					else //本设置为不包含，要加上交易所
					{
						dTmp = exfee+ exfee*reslist[i]->dValue;//交易所手续费比率(在交易所费率上乘以，不含交易所)
					}
				}
				else //不包含，就以设置的为准
				{
					dTmp =  exfee*reslist[i]->dValue;//交易所手续费比率(在交易所费率上乘以，不含交易所)
				}

				break;
			case FEE_MODEL_EXCH_RATE_INCLUDE:
				if (isinclude) //包含交易所
				{
					if (reslist[i]->sInclude[0] == '1')
					{
						//本设置为包含
						dTmp =  exfee*reslist[i]->dValue;//交易所手续费比率(在交易所费率上乘以，含交易所)
					}
					else //包含 本设置为不包含，应该要报错的
					{
						//忽略标志，以此值为准
						m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"设置出错了,是否包含交易所标志为0,但值为含交易所,以值为准");
						dTmp = exfee+ exfee*reslist[i]->dValue;//交易所手续费比率(在交易所费率上乘以，含交易所)
					}
				}
				else //不包含，设置的为包含，要减去交易所
				{
					dTmp =  exfee*reslist[i]->dValue-exfee;//交易所手续费比率(在交易所费率上乘以，含交易所)
				}

				break;
			default:
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"会员手续费 客户特殊费率设置 合约[%d %s] 品种[%s %d] 的收费模式[%s]不符",pc->nContractId,pc->sContractCode,pc->sProd_type,nProdId,exfeelist[i]->sFareMode);
				return false;
			}
			memberfee = memberfee+dTmp;//多个配置相加
		}
		if (isinclude) //含了交易所的
		{
			memberfee = memberfee - exfee;//这个才是会员多收的
		}
	}
	else
	{
		memberfee = 0.0;
		//根据收费模式，计算手续费
		for (i=0 ; i< memberfeelist.size(); i++)
		{
			switch (memberfeelist[i]->sFareMode[0])
			{
			case FEE_MODEL_DONERATE:
				dTmp = amt*memberfeelist[i]->dValue;
				break;
			case FEE_MODEL_WEIGHT:
				dTmp =  memberfeelist[i]->dValue*num*pc->dTxUnit;//手数*合约乘数即为总重量，乘上价格即为金额
				break;
			case FEE_MODEL_HAND:
				dTmp =  memberfeelist[i]->dValue*num;//每手多少
				break;
			case FEE_MODEL_EXCH_RATE:
				dTmp =  exfee*memberfeelist[i]->dValue;//交易所手续费比率(在交易所费率上乘以，不含交易所)
				break;
			case FEE_MODEL_EXCH_RATE_INCLUDE:
				dTmp =  exfee*memberfeelist[i]->dValue;//交易所手续费比率(在交易所费率上乘以，含交易所)
				isinclude = true;
				break;
			default:
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"会员手续费 模板[%s] 合约[%d %s] 品种[%d] 的收费模式[%s]不符",pcust->sMemberFareModel,pc->nContractId,pc->sContractCode,nProdId,exfeelist[i]->sFareMode);
				return false;
			}
			memberfee = memberfee+dTmp;//多个配置相加
		}
		if (isinclude) //含了交易所的
		{
			memberfee = memberfee - exfee;//这个才是会员多收的
		}
	}
	return true;
}

bool CKernelPub::GetMarginValue(S_MDB_CONTRACT_INFO *pc, S_MDB_CUST_FUNDACC_INFO *pcust, int flag, int num, double amt, double &exmargin, double &membermargin)
{
	// 	    买开：付权利金
	// 		卖平：得权利金
	// 		卖开：得权利金，冻结保证金
	// 		买平：付权利金，释放保证金

	//保证金为会员保证金
	//期权卖开保证金  = max
	//(权利金(昨结算)+标的合约保证金(昨结算)-期权虚值额(标的合约昨结算和期权执行价的比较)的一半)
	//权利金+标的合约保证金(昨结算)的一半

	//期权虚值额的计算   
	//看涨
	//MAX((行权价-标的昨结算)*合约乘数,0.0)
	//看跌
	//MAX((标的昨结算-行权价)*合约乘数,0.0)
	if (flag == 1) //期权
	{
		if (pc->sProd_type[0] != PROD_TYPE_OPTION)
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"传入为计算期权合约保证金，但合约[%d %s]不是期权合约",pc->nContractId,pc->sContractCode);
			return false;
		}
		S_MDB_CONTRACT_INFO *pdc = m_pMemDb->m_pDbBaseinfo.m_pContract.Select(pc->nProdId);
		if (pdc == NULL)
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"合约[%d %s]的标的合约[%d]不存在",pc->nContractId,pc->sContractCode,pc->nProdId);
			return false;
		}
		//取得标的合约的昨结算
		S_MDB_CONTRACT_SETTLE_PRICE *psp = m_pMemDb->m_pDbBaseinfo.m_pSettlePrice.Select(pdc->nContractId);
		if (psp == NULL)
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"合约[%d %s]的标的合约[%d %s]的昨结算价不存在",pc->nContractId,pc->sContractCode,pdc->nContractId,pdc->sContractCode);
			return false;
		}
		//取标的合约的保证金
		double dExMarginTmp;
		double dMemberMarginTmp;
		if (!GetMarginValue(pdc,pcust,0,num,psp->dSettlePrice*num*pdc->dTxUnit,dExMarginTmp,dMemberMarginTmp))
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"取得 合约[%d %s]的标的合约[%d %s]的保证金出错",pc->nContractId,pc->sContractCode,pdc->nContractId,pdc->sContractCode);
			return false;
		}
		//计算 期权虚值额
		double dOptionVirValue;
		if (pc->sIsOptionCall[0] == BOOL_TRUE)
		{
			dOptionVirValue = (pc->dRefPrice - psp->dSettlePrice)*pc->dTxUnit;
			if (dOptionVirValue < MAXDOUBLECOMPARE)
			{
				dOptionVirValue = 0.0;
			}
		}
		else
		{
			dOptionVirValue = (psp->dSettlePrice-pc->dRefPrice)*pc->dTxUnit;
			if (dOptionVirValue < MAXDOUBLECOMPARE)
			{
				dOptionVirValue = 0.0;
			}
		}
		//计算保证金
		//取得合约的昨结算
		S_MDB_CONTRACT_SETTLE_PRICE *pcsp = m_pMemDb->m_pDbBaseinfo.m_pSettlePrice.Select(pc->nContractId);
		if (psp == NULL)
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"合约[%d %s]昨结算价不存在",pc->nContractId,pc->sContractCode);
			return false;
		}
		//取下面2个的最大值为保证金值
		//(权利金(昨结算)+标的合约保证金(昨结算)-期权虚值额(标的合约昨结算和期权执行价的比较)的一半)
		//权利金+标的合约保证金(昨结算)的一半
		double aa = pcsp->dSettlePrice*num*pc->dTxUnit+dExMarginTmp+dMemberMarginTmp-dOptionVirValue/2.00;
		double bb = amt + (dExMarginTmp+dMemberMarginTmp)/2.00;
		if( aa > bb)
		{
			membermargin = aa;
			exmargin = pcsp->dSettlePrice*num*pc->dTxUnit+dExMarginTmp-dOptionVirValue/2.00;
		}
		else
		{
			membermargin = bb;
			exmargin = amt + (dExMarginTmp)/2.00;
		}
		return true;
	}
	//取期货保证金

	char sFeeNo[4];
	strcpy(sFeeNo,"105");//交易保证金

	//先取交易所的费率
	std::vector<S_MDB_FEE_TEMPLET_DETAIL *> exfeelist;
	if (!m_pMemDb->m_pDbSet.m_pFeeTempletDetail.Select(pcust->sExchFareModel,pc->sProd_type,pc->nProdId,pc->nContractId,sFeeNo,exfeelist))
	{
		//按合约找不到再按品种找
		if (!m_pMemDb->m_pDbSet.m_pFeeTempletDetail.Select(pcust->sExchFareModel,pc->sProd_type,pc->nProdId,0,sFeeNo,exfeelist))
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"取交易所保证金 交易所模板[%s]合约[%d %s] 品种[%s %d] 找不到配置",pcust->sExchFareModel,pc->nContractId,pc->sContractCode,pc->sProd_type,pc->nProdId);
			return false;
		}
	}
	if (exfeelist.size()>1)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"交易所保证金 交易所模板[%s]合约[%d %s] 品种[%s %d] 有多个配置",pcust->sExchFareModel,pc->nContractId,pc->sContractCode,pc->sProd_type,pc->nProdId);
		return false;
	}
	// #define  FEE_MODEL_DONERATE   '1'		        //成交金额比率(不含交易所)
	// #define  FEE_MODEL_WEIGHT     '2'		        //重量(元/克)(不含交易所)
	// #define  FEE_MODEL_HAND       '3'		        //手数(元/手)(不含交易所)
	// #define  FEE_MODEL_EXCH_RATE   '4'		        //交易所手续费比率(在交易所费率上乘以，不含交易所)
	// #define  FEE_MODEL_EXCH_RATE_INCLUDE   '5'		//交易所手续费比率(在交易所费率上乘以，包含交易所)
	//根据收费模式，计算保证金
	int i;
	double dTmp=0.0;
	exmargin=0.0;
	for (i=0 ; i< exfeelist.size(); i++)
	{
		switch (exfeelist[0]->sFareMode[0])
		{
		case FEE_MODEL_DONERATE:
			dTmp = amt*exfeelist[0]->dValue;
			break;
		case FEE_MODEL_WEIGHT:
			dTmp =  exfeelist[0]->dValue*num*pc->dTxUnit;//手数*合约乘数即为总重量，乘上价格即为金额
			break;
		case FEE_MODEL_HAND:
			dTmp =  exfeelist[0]->dValue*num;//每手多少
			break;
		default:
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"交易所保证金 交易所模板[%s]合约[%d %s] 品种[%s %d] 的收费模式[%s]不符",pcust->sExchFareModel,pc->nContractId,pc->sContractCode,pc->sProd_type,pc->nProdId,exfeelist[0]->sFareMode);
			return false;
		}
		exmargin += dTmp;
	}
	
	//取会员的保证金
	std::vector<S_MDB_FEE_TEMPLET_DETAIL *> memberfeelist;
	std::vector<S_MDB_CUST_FEE *> reslist;
	


	//先取客户特殊的
	bool issetmember=true;//是否设置了会员手续费率，未设默认为0
	bool iscust=true;//是否特殊
	//先从客户特殊费率表里面取
	if (!m_pMemDb->m_pDbSet.m_pCustFeeDetail.Select(pcust->sTxAcc,pc->sProd_type,pc->nProdId,pc->nContractId,sFeeNo,reslist))
	{
		//按合约没取到，按品种再取，即合约为0
		if (!m_pMemDb->m_pDbSet.m_pCustFeeDetail.Select(pcust->sTxAcc,pc->sProd_type,pc->nProdId,0,sFeeNo,reslist))
		{
			//没有找到，从费率模板表里取
			iscust = false;
			if (!m_pMemDb->m_pDbSet.m_pFeeTempletDetail.Select(pcust->sMemberFareModel,pc->sProd_type,pc->nProdId,pc->nContractId,sFeeNo,memberfeelist))
			{
				//按合约找不到再按品种找
				if (!m_pMemDb->m_pDbSet.m_pFeeTempletDetail.Select(pcust->sMemberFareModel,pc->sProd_type,pc->nProdId,0,sFeeNo,memberfeelist))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"取会员保证金 会员模板[%s]合约[%d %s] 品种[%s %d] 找不到配置",pcust->sMemberFareModel,pc->nContractId,pc->sContractCode,pc->sProd_type,pc->nProdId);
					issetmember=false;
					//return false;
				}
			}
		}
	}
	if (!issetmember)
	{
		membermargin= 0.0;
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"未设会员保证金率，默认会员不收");
		return true;
	}	

	bool isinclude=false;
	membermargin = 0.0;
	if (iscust)
	{
		//特殊费率表单独一个字段保存是否包含交易所
		for (i=0 ; i< reslist.size(); i++)
		{
			if (reslist[i]->sInclude[0] == '1')
			{
				//有一个包含的，统一以包含算
				isinclude = true;
				break;
			}
		}
		//根据收费模式，计算保证金
		for (i=0 ; i< reslist.size(); i++)
		{
			switch (reslist[i]->sFareMode[0])
			{
				case FEE_MODEL_DONERATE:
					if (isinclude) //包含交易所
					{
						if (reslist[i]->sInclude[0] == '1')
						{
							//本设置为包含
							dTmp = amt*reslist[i]->dValue;
						}
						else //本设置为不包含，要加上交易所
						{
							dTmp = exmargin+ amt*reslist[i]->dValue;
						}
					}
					else //不包含，就以设置的为准
					{
						dTmp = amt*reslist[i]->dValue;
					}
					break;
				case FEE_MODEL_WEIGHT:
					if (isinclude) //包含交易所
					{
						if (reslist[i]->sInclude[0] == '1')
						{
							//本设置为包含
							dTmp =  reslist[i]->dValue*num*pc->dTxUnit;//手数*合约乘数即为总重量，乘上价格即为金额
						}
						else //本设置为不包含，要加上交易所
						{
							dTmp = exmargin+ reslist[i]->dValue*num*pc->dTxUnit;//手数*合约乘数即为总重量，乘上价格即为金额
						}
					}
					else //不包含，就以设置的为准
					{
						dTmp = reslist[i]->dValue*num*pc->dTxUnit;//手数*合约乘数即为总重量，乘上价格即为金额
					}
					break;
				case FEE_MODEL_HAND:
					if (isinclude) //包含交易所
					{
						if (reslist[i]->sInclude[0] == '1')
						{
							//本设置为包含
							dTmp =  reslist[i]->dValue*num;//每手多少
						}
						else //本设置为不包含，要加上交易所
						{
							dTmp = exmargin+reslist[i]->dValue*num;//每手多少
						}
					}
					else //不包含，就以设置的为准
					{
						dTmp =  reslist[i]->dValue*num;//每手多少
					}
					
					break;
				case FEE_MODEL_EXCH_RATE:
					if (isinclude) //包含交易所
					{
						if (reslist[i]->sInclude[0] == '1') //本设置为包含
						{
							//忽略标志，以此值为准
							m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"设置出错了,是否包含交易所标志为1,但值为不含交易所,以值为准");
							dTmp = exmargin*reslist[i]->dValue;
						}
						else //本设置为不包含，要加上交易所
						{
							dTmp = exmargin+ exmargin*reslist[i]->dValue;//交易所手续费比率(在交易所费率上乘以，不含交易所)
						}
					}
					else //不包含，就以设置的为准
					{
						dTmp =  exmargin*reslist[i]->dValue;//交易所手续费比率(在交易所费率上乘以，不含交易所)
					}
					break;
				case FEE_MODEL_EXCH_RATE_INCLUDE:
					if (isinclude) //包含交易所
					{
						if (reslist[i]->sInclude[0] == '1')
						{
							//本设置为包含
							dTmp =  exmargin*reslist[i]->dValue;//交易所手续费比率(在交易所费率上乘以，含交易所)
						}
						else //包含 本设置为不包含，应该要报错的
						{
							//忽略标志，以此值为准
							m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"设置出错了,是否包含交易所标志为0,但值为含交易所,以值为准");
							dTmp = exmargin+ exmargin*reslist[i]->dValue;//交易所手续费比率(在交易所费率上乘以，含交易所)
						}
					}
					else //不包含，设置的为包含，要减去交易所
					{
						dTmp =  exmargin*reslist[i]->dValue-exmargin;//交易所手续费比率(在交易所费率上乘以，含交易所)
					}
					break;
				default:
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"会员保证金 客户特殊费率设置 合约[%d %s] 品种[%s %d] 的收费模式[%s]不符",pc->nContractId,pc->sContractCode,pc->sProd_type,pc->nProdId,exfeelist[i]->sFareMode);
					return false;
			}
			membermargin = membermargin+dTmp;//多个配置相加
		}
		if (isinclude) //含了交易所的
		{
			membermargin = membermargin - exmargin;//这个才是会员多收的
		}
	}
	else
	{
		//根据收费模式，计算保证金
		for (i=0 ; i< memberfeelist.size(); i++)
		{
			switch (memberfeelist[i]->sFareMode[0])
			{
				case FEE_MODEL_DONERATE:
					dTmp = amt*memberfeelist[i]->dValue;
					break;
				case FEE_MODEL_WEIGHT:
					dTmp =  memberfeelist[i]->dValue*num*pc->dTxUnit;//手数*合约乘数即为总重量，乘上价格即为金额
					break;
				case FEE_MODEL_HAND:
					dTmp =  memberfeelist[i]->dValue*num;//每手多少
					break;
				case FEE_MODEL_EXCH_RATE:
					dTmp =  exmargin*memberfeelist[i]->dValue;//交易所保证金比率(在交易所费率上乘以，不含交易所)
					break;
				case FEE_MODEL_EXCH_RATE_INCLUDE:
					dTmp =  exmargin*memberfeelist[i]->dValue;//交易所保证金比率(在交易所费率上乘以，含交易所)
					isinclude = true;
					break;
				default:
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"会员保证金 模板[%s] 合约[%d %s] 品种[%s %d] 的收费模式[%s]不符",pcust->sMemberFareModel,pc->nContractId,pc->sContractCode,pc->sProd_type,pc->nProdId,exfeelist[i]->sFareMode);
					return false;
			}
			membermargin = membermargin+dTmp;//多个配置相加
		}
		if (isinclude) //含了交易所的
		{
			membermargin = membermargin - exmargin;//这个才是会员多收的
		}
	}

	

	return true;
}


bool CKernelPub::CheckRight(char *custorg, char *operorg)
{
	S_MDB_ORG_INFO *pcust = m_pMemDb->m_pDbPublic.m_pOrg.Select(custorg);
	if (pcust == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"机构[%s]在机构表中不存在",custorg);
		return false;
	}
	S_MDB_ORG_INFO *poper = m_pMemDb->m_pDbPublic.m_pOrg.Select(operorg);
	if (poper == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"机构[%s]在机构表中不存在",operorg);
		return false;
	}
	if (strcmp(poper->sUpOrg,"NULL")==0) //操作员为顶级机构，肯定有权限
	{
		return true;
	}
	if (strcmp(pcust->sUpOrg,"NULL")==0) //账户为顶级机构，到这里肯定没有权限
	{
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"操作员机构[%s] 对账户机构[%s]无权限 UpOrg[%s]",operorg,custorg,poper->sUpOrg);
		return false;
	}
	//比较上级机构+当前机构，如果账户的机构大于等于则有权限
	char soperorg[300];
	char scustorg[300];
	sprintf(soperorg,"%s|%s",poper->sUpOrg,poper->sOrgCode);
	sprintf(scustorg,"%s|%s",pcust->sUpOrg,pcust->sOrgCode);
	if (strncmp(soperorg,scustorg,strlen(soperorg)) == 0 )
	{
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"操作员机构[%s] 对账户机构[%s]有权限 操作员Org[%s] 账户Org[%s]",operorg,custorg,soperorg,scustorg);
		return true;
	}
	return false;
}

int CKernelPub::GetPosiRiskLevel(S_MDB_TXACC_LOCK *acclock,S_MDB_CUST_TEMPLET_RELA *templet,S_MDB_CUST_POSITION *custposi,char *traderight,char *warnmsg)
{
	int j;
	std::vector<S_MDB_POSITION_TEMPLET *> positemplet;
	S_MDB_CONTRACT_INFO *pCon=NULL;
	S_MDB_CONTRACT_INFO *pConOpt=NULL;
	pCon = m_pMemDb->m_pDbBaseinfo.m_pContract.Select(custposi->nContractNo);
	if (pCon == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"合约[%d]在合约表中不存在",custposi->nContractNo);
		return -1;
	}
	switch (pCon->sProd_type[0] )
	{
	case PROD_TYPE_OPTION:
		//取标的合约对应的品种
		pConOpt = m_pMemDb->m_pDbBaseinfo.m_pContract.Select(pCon->nProdId);
		if (pConOpt == NULL)
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"合约[%d]在合约表中不存在",pCon->nProdId);
			return -1;
		}
		if (!m_pMemDb->m_pDbSet.m_pPosiTempletDetail.Select(templet->sPosiTempletId,pCon->sProd_type,pConOpt->nProdId,pCon->nProdId,pCon->nContractId,positemplet))
		{
			//按合约没有找到，按标的合约去找
			if (!m_pMemDb->m_pDbSet.m_pPosiTempletDetail.Select(templet->sPosiTempletId,pCon->sProd_type,pConOpt->nProdId,pCon->nProdId,0,positemplet))
			{
				//按标的合约没有找到，按品种去找
				if (!m_pMemDb->m_pDbSet.m_pPosiTempletDetail.Select(templet->sPosiTempletId,pCon->sProd_type,pConOpt->nProdId,0,0,positemplet))
				{
					m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"持仓模板[%s]合约[%d %s]标的合约[%d %s]及对应的期货品种[%d]在模板表中未设置",templet->sPosiTempletId,custposi->nContractNo,custposi->sContractCode,pCon->nProdId,pCon->sProd_code,pConOpt->nProdId);
					return -1;
				}
			}
		}

		break;
	case PROD_TYPE_FUTURES:
		if (!m_pMemDb->m_pDbSet.m_pPosiTempletDetail.Select(templet->sPosiTempletId,pCon->sProd_type,pCon->nProdId,0,pCon->nContractId,positemplet))
		{
			//按合约没有找到，按品种去找
			if (!m_pMemDb->m_pDbSet.m_pPosiTempletDetail.Select(templet->sPosiTempletId,pCon->sProd_type,pCon->nProdId,0,0,positemplet))
			{
				m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"持仓模板[%s]合约[%d]及品种[%d]在模板表中未设置",templet->sPosiTempletId,custposi->nContractNo,pCon->nProdId);
				return -1;
			}
		}
		break;
	case PROD_TYPE_SEC:
		if (!m_pMemDb->m_pDbSet.m_pPosiTempletDetail.Select(templet->sPosiTempletId,pCon->sProd_type,0,0,pCon->nContractId,positemplet))
		{
			m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"持仓模板[%s]合约[%d]及品种[%d]在模板表中未设置",templet->sPosiTempletId,custposi->nContractNo,pCon->nProdId);
			return -1;
		}
		break;
	case PROD_TYPE_SPOT:
		if (!m_pMemDb->m_pDbSet.m_pPosiTempletDetail.Select(templet->sPosiTempletId,pCon->sProd_type,pCon->nProdId,0,pCon->nContractId,positemplet))
		{
			//按合约没有找到，按品种去找
			if (!m_pMemDb->m_pDbSet.m_pPosiTempletDetail.Select(templet->sPosiTempletId,pCon->sProd_type,pCon->nProdId,0,0,positemplet))
			{
				m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"持仓模板[%s]合约[%d]及品种[%d]在模板表中未设置",templet->sPosiTempletId,custposi->nContractNo,pCon->nProdId);
				return -1;
			}
		}
		break;
	default:
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"合约[%d]的品种类型[%s]不符",custposi->nContractNo,pCon->sProd_type);
		return -1;

	}
	//从模板里取出预警序号最小的

	S_MDB_CUST_FUND *fund = m_pMemDb->GetFund(acclock,pCon->sCurrency);
	if (fund == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"资金账号[%s] 币种[%s]在资金表不存在",acclock->sTxAcc,pCon->sCurrency);
		return -1;
	}
	if (fund->dBalance > MINDOUBLECOMPARE && fund->dBalance<MAXDOUBLECOMPARE)
	{
		//结存权益为0
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"资金账号[%s] 结存为0 [%.2lf]",acclock->sTxAcc,fund->dBalance);
		return -1;
	}
	S_MDB_QUOTATION *quo = m_pMemDb->m_pDbTrader.m_pQuotation.Select(custposi->nContractNo);
	if (quo == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"合约[%d] 在行情表中没有最新价",custposi->nContractNo);
		return -1;
	}
	//保证金占比
	double dMarginRate = (custposi->dPB_Margin+custposi->dPS_Margin)/fund->dBalance;
	//持仓价值比
	double dPosiValueRate = quo->dNewPrice*pCon->dTxUnit*(custposi->nCurPB+custposi->nCurPS)/fund->dBalance;
	//持仓轧差价值比
	double dPosiDiffValueRate = quo->dNewPrice*pCon->dTxUnit*(custposi->nCurPB-custposi->nCurPS)/fund->dBalance;
	for (j=0; j<positemplet.size(); j++)
	{
		switch (positemplet[j]->sPosiModel[0])
		{
		case PM_MARGINRATE://保证金占比
			if (positemplet[j]->sCondition[0] == EF_LARGEEQUAL)
			{
				if (dMarginRate >= positemplet[j]->dValue)
				{
					strcpy(traderight,positemplet[j]->sRight);
					sprintf(warnmsg,"合约[%d %s]保证金占比大于等于[%f]",custposi->nContractNo,custposi->sContractCode,positemplet[j]->dValue);
					return positemplet[j]->nWarnNo;
				}
			}
			else if (positemplet[j]->sCondition[0] == EF_LARGE)
			{
				if (dMarginRate > positemplet[j]->dValue)
				{
					strcpy(traderight,positemplet[j]->sRight);
					sprintf(warnmsg,"合约[%d %s]保证金占比大于[%f]",custposi->nContractNo,custposi->sContractCode,positemplet[j]->dValue);
					return positemplet[j]->nWarnNo;
				}
			}
			else if (positemplet[j]->sCondition[0] == EF_EQUAL)
			{
				if ( (dMarginRate - positemplet[j]->dValue) > MINDOUBLECOMPARE && (dMarginRate - positemplet[j]->dValue) < MAXDOUBLECOMPARE )
				{
					strcpy(traderight,positemplet[j]->sRight);
					sprintf(warnmsg,"合约[%d %s]保证金占比等于[%f]",custposi->nContractNo,custposi->sContractCode,positemplet[j]->dValue);
					return positemplet[j]->nWarnNo;
				}
			}
			else //EF_SMALLEQUAL 小于等
			{
				if (dMarginRate <= positemplet[j]->dValue)
				{
					strcpy(traderight,positemplet[j]->sRight);
					sprintf(warnmsg,"合约[%d %s]保证金占比小于等于[%f]",custposi->nContractNo,custposi->sContractCode,positemplet[j]->dValue);
					return positemplet[j]->nWarnNo;
				}
			}

			break;
		case PM_POSIVALUE_RATE://持仓价值比
			if (positemplet[j]->sCondition[0] == EF_LARGEEQUAL)
			{
				if (dPosiValueRate >= positemplet[j]->dValue)
				{
					strcpy(traderight,positemplet[j]->sRight);
					sprintf(warnmsg,"合约[%d %s]持仓价值比大于等于[%f]",custposi->nContractNo,custposi->sContractCode,positemplet[j]->dValue);
					return positemplet[j]->nWarnNo;
				}
			}
			else if (positemplet[j]->sCondition[0] == EF_LARGE)
			{
				if (dPosiValueRate > positemplet[j]->dValue)
				{
					strcpy(traderight,positemplet[j]->sRight);
					sprintf(warnmsg,"合约[%d %s]持仓价值比大于[%f]",custposi->nContractNo,custposi->sContractCode,positemplet[j]->dValue);
					return positemplet[j]->nWarnNo;
				}
			}
			else if (positemplet[j]->sCondition[0] == EF_EQUAL)
			{
				if ( (dPosiValueRate - positemplet[j]->dValue) > MINDOUBLECOMPARE && (dPosiValueRate - positemplet[j]->dValue) < MAXDOUBLECOMPARE )
				{
					strcpy(traderight,positemplet[j]->sRight);
					sprintf(warnmsg,"合约[%d %s]持仓价值比等于[%f]",custposi->nContractNo,custposi->sContractCode,positemplet[j]->dValue);
					return positemplet[j]->nWarnNo;
				}
			}
			else //EF_SMALLEQUAL 小于等
			{
				if (dPosiValueRate <= positemplet[j]->dValue)
				{
					strcpy(traderight,positemplet[j]->sRight);
					sprintf(warnmsg,"合约[%d %s]持仓价值比小于等于[%f]",custposi->nContractNo,custposi->sContractCode,positemplet[j]->dValue);
					return positemplet[j]->nWarnNo;
				}
			}
			break;
		case PM_POSI_OFFSET_RATE://持仓轧差价值比
			if (positemplet[j]->sCondition[0] == EF_LARGEEQUAL)
			{
				if (dPosiDiffValueRate >= positemplet[j]->dValue)
				{
					strcpy(traderight,positemplet[j]->sRight);
					sprintf(warnmsg,"合约[%d %s]持仓轧差价值比大于等于[%f]",custposi->nContractNo,custposi->sContractCode,positemplet[j]->dValue);
					return positemplet[j]->nWarnNo;
				}
			}
			else if (positemplet[j]->sCondition[0] == EF_LARGE)
			{
				if (dPosiDiffValueRate > positemplet[j]->dValue)
				{
					strcpy(traderight,positemplet[j]->sRight);
					sprintf(warnmsg,"合约[%d %s]持仓轧差价值比大于[%f]",custposi->nContractNo,custposi->sContractCode,positemplet[j]->dValue);
					return positemplet[j]->nWarnNo;
				}
			}
			else if (positemplet[j]->sCondition[0] == EF_EQUAL)
			{
				if ( (dPosiDiffValueRate - positemplet[j]->dValue) > MINDOUBLECOMPARE && (dPosiDiffValueRate - positemplet[j]->dValue) < MAXDOUBLECOMPARE )
				{
					strcpy(traderight,positemplet[j]->sRight);
					sprintf(warnmsg,"合约[%d %s]持仓轧差价值比等于[%f]",custposi->nContractNo,custposi->sContractCode,positemplet[j]->dValue);
					return positemplet[j]->nWarnNo;
				}
			}
			else //EF_SMALLEQUAL 小于等
			{
				if (dPosiDiffValueRate <= positemplet[j]->dValue)
				{
					strcpy(traderight,positemplet[j]->sRight);
					sprintf(warnmsg,"合约[%d %s]持仓轧差价值比小于等于[%f]",custposi->nContractNo,custposi->sContractCode,positemplet[j]->dValue);
					return positemplet[j]->nWarnNo;
				}
			}
			break;
		default:
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"持仓指标[%s]不符",positemplet[j]->sPosiModel);
			return -1;

		}
	}
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"GetPosiRiskLevel取持仓风险完成，无持仓风险");
	strcpy(warnmsg,"无匹配风险设置，无风险");
	return 10;
}
int CKernelPub::GetRiskLevel(S_MDB_CUST_FUND *fund,char *traderight)
{
	//先找资金账号自己的模板，如果2个都设了，按2个都来找预警序号，得到一个最小的，即级别最高的，如果只设了一个，
	//就按那1个来找。
	//如果资金账号没有对应模板，那么取$的，重复以上
	traderight[0] = TR_NORMAL;//默认为正常

	bool isposition=false;
	//查询是否有持仓
	std::vector<S_MDB_CUST_POSITION *> posilist;
	if (m_pMemDb->m_pDbTrader.m_pPosition.SelectByAcc(fund->sTxAcc,posilist))
	{
		for (int j=0;j<posilist.size() ; j++)
		{
			if (posilist[j]->nCurPB>0 || posilist[j]->nCurPS>0)
			{
				isposition = true;
				break;
			}
		}
	}

	//取资金账号对应的模板
	S_MDB_CUST_TEMPLET_RELA *templet = m_pMemDb->m_pDbSet.m_pCustTemplet.Select(fund->sTxAcc);
	if (templet == NULL)
	{
		//没有找到资金账号对应的模板,取$对应的模板
		templet = m_pMemDb->m_pDbSet.m_pCustTemplet.Select("$");
		if (templet == NULL)
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"GetRiskLevel error  $ not found ");
			return -1;
		}
	}

	//根据模板ID，从资金模板设置表查找
	std::vector<S_MDB_FUND_TEMPLET *> warnlist;
	if (!m_pMemDb->m_pDbSet.m_pFundTempletDetail.Select(templet->sFundTempletId,warnlist))
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"templetid %s not found ",templet->sFundTempletId);
		return -1;
	}
	int nRet = warnlist.size();
	fund->nRisk_level = 10;
	for (int i=0 ; i<nRet ; i++)
	{
		if (warnlist[i]->sFundModel[0] == FMM_RISK) //指标为风险度
		{
			if (warnlist[i]->sCondition[0] == EF_LARGEEQUAL)
			{
				if (fund->dRisk > warnlist[i]->dValue -MAXDOUBLECOMPARE)//大于等于
				{
					fund->nRisk_level= warnlist[i]->nWarnNo;
					traderight[0] = warnlist[i]->sRight[0];
					sprintf(fund->sRiskDesc,"风险度[%f]大于等于 %f",fund->dRisk,warnlist[i]->dValue);
					break;
				}
			}
			else if (warnlist[i]->sCondition[0] == EF_LARGE)
			{
				if (fund->dRisk > warnlist[i]->dValue )//大于
				{
					fund->nRisk_level= warnlist[i]->nWarnNo;
					traderight[0] = warnlist[i]->sRight[0];
					sprintf(fund->sRiskDesc,"风险度[%f]大于 %f",fund->dRisk,warnlist[i]->dValue);
					break;
				}
			}
			else
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"templetid %s condition %s error ",templet->sFundTempletId,warnlist[i]->sCondition);
				return 10;
			}

		}
		else //权益
		{
			if (warnlist[i]->sCondition[0] == EF_SMALLEQUAL)
			{
				if (fund->dBalance < warnlist[i]->dValue+MAXDOUBLECOMPARE)//小于等于
				{
					if (warnlist[i]->nWarnNo <3 ) //当级别为强平等时要判断是否有持仓
					{
						if (isposition) //有持仓
						{
							fund->nRisk_level= warnlist[i]->nWarnNo;
							traderight[0] = warnlist[i]->sRight[0];
							sprintf(fund->sRiskDesc,"权益[%f]小于等于 %f",fund->dBalance,warnlist[i]->dValue);
							break;
						}
						else
						{
							continue;
						}
					}
					else
					{
						fund->nRisk_level= warnlist[i]->nWarnNo;
						traderight[0] = warnlist[i]->sRight[0];
						sprintf(fund->sRiskDesc,"权益[%f]小于等于 %f",fund->dBalance,warnlist[i]->dValue);
						break;
					}
				}
				
			}
			else
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"templetid %s condition %s error ",templet->sFundTempletId,warnlist[i]->sCondition);
				return 10;
			}
		}
	}
	return fund->nRisk_level;
}
int CKernelPub::GetProdRiskLevel(char *prodid,double unitvalue,double risk,char *traderight,char *warnmsg)
{
	traderight[0] = TR_NORMAL;//默认为正常
	int nRiskLevel=10;

	S_MDB_PRODORG_TEMPLET_RELA *pProdTemplet = m_pMemDb->m_pDbSet.m_pProdTemplet.Select(prodid);
	if (pProdTemplet == NULL)
	{
		pProdTemplet = m_pMemDb->m_pDbSet.m_pProdTemplet.Select("$");
		if (pProdTemplet == NULL)
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"产品[%s]无对应模板",prodid);
			return 10;
		}
	}

	std::vector<S_MDB_PROD_MONITOR *> reslist;
	if (!m_pMemDb->m_pDbSet.m_pProdFundMonitor.Select(pProdTemplet->sFundTempletId,reslist))
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"产品资金监控未配置");
		return 10;
	}

	for (int i=0 ; i<reslist.size() ; i++)
	{
		if (reslist[i]->sModel[0] == FMM_RISK) //指标为风险度
		{
			if (reslist[i]->sCondition[0] == EF_LARGEEQUAL)
			{
				if (risk > reslist[i]->dValue-MAXDOUBLECOMPARE)//大于等于
				{
					nRiskLevel= reslist[i]->nWarnNo;
					traderight[0] = reslist[i]->sRight[0];
					sprintf(warnmsg,"风险度[%d]大于等于[%d]",risk,reslist[i]->dValue);
					break;
				}
			}
			else if (reslist[i]->sCondition[0] == EF_LARGE)
			{
				if (risk > reslist[i]->dValue)//大于
				{
					nRiskLevel= reslist[i]->nWarnNo;
					traderight[0] = reslist[i]->sRight[0];
					sprintf(warnmsg,"风险度[%d]大于[%d]",risk,reslist[i]->dValue);
					break;
				}
			}
			else
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"condition %s error ",reslist[i]->sCondition);
				return 10;
			}

		}
		else //单位净值
		{
			if (reslist[i]->sCondition[0] == EF_SMALLEQUAL)
			{
				if (unitvalue < reslist[i]->dValue+MAXDOUBLECOMPARE)//小于等于
				{
					nRiskLevel= reslist[i]->nWarnNo;
					traderight[0] = reslist[i]->sRight[0];
					sprintf(warnmsg,"单位净值[%d]小于等于[%d]",unitvalue,reslist[i]->dValue);
					break;
				}
			}
			else
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__," condition %s error ",reslist[i]->sCondition);
				return 10;
			}
		}
	}
	return nRiskLevel;
}
int CKernelPub::GetProdRiskLevel(char *prodid,double marginrate,double posivalue,double posidiffvalue,char *traderight,char *warnmsg)
{
	traderight[0] = TR_NORMAL;//默认为正常
	int nRiskLevel=10;
	S_MDB_PRODORG_TEMPLET_RELA *pProdTemplet = m_pMemDb->m_pDbSet.m_pProdTemplet.Select(prodid);
	if (pProdTemplet == NULL)
	{
		pProdTemplet = m_pMemDb->m_pDbSet.m_pProdTemplet.Select("$");
		if (pProdTemplet == NULL)
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"产品[%s]无对应模板",prodid);
			return 10;
		}
	}
	std::vector<S_MDB_PROD_MONITOR *> reslist;
	if (!m_pMemDb->m_pDbSet.m_pProdPosiMonitor.Select(pProdTemplet->sPosiTempletId,reslist))
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"产品持仓监控未配置");
		return 10;
	}
	for (int i=0 ; i<reslist.size() ; i++)
	{
		if (reslist[i]->sModel[0] == PM_MARGINRATE) //保证金占比
		{
			if (reslist[i]->sCondition[0] == EF_LARGEEQUAL)
			{
				if (marginrate > reslist[i]->dValue-MAXDOUBLECOMPARE)//大于等于
				{
					nRiskLevel= reslist[i]->nWarnNo;
					traderight[0] = reslist[i]->sRight[0];
					sprintf(warnmsg,"保证金占比[%d]大于等于[%d]",marginrate,reslist[i]->dValue);
					break;
				}
			}
			else if (reslist[i]->sCondition[0] == EF_LARGE)
			{
				if (marginrate > reslist[i]->dValue)//大于
				{
					nRiskLevel= reslist[i]->nWarnNo;
					traderight[0] = reslist[i]->sRight[0];
					sprintf(warnmsg,"保证金占比[%d]大于[%d]",marginrate,reslist[i]->dValue);
					break;
				}
			}
			else if (reslist[i]->sCondition[0] == EF_SMALLEQUAL)
			{
				if (marginrate < reslist[i]->dValue+MAXDOUBLECOMPARE)//小于等于
				{
					nRiskLevel= reslist[i]->nWarnNo;
					traderight[0] = reslist[i]->sRight[0];
					sprintf(warnmsg,"保证金占比[%d]小于等于[%d]",posivalue,reslist[i]->dValue);
					break;
				}
			}
			else
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"condition %s error ",reslist[i]->sCondition);
				return 10;
			}

		}
		else if (reslist[i]->sModel[0] == PM_POSIVALUE_RATE) //持仓价值比
		{
			if (reslist[i]->sCondition[0] == EF_LARGEEQUAL)
			{
				if (posivalue > reslist[i]->dValue-MAXDOUBLECOMPARE)//大于等于
				{
					nRiskLevel= reslist[i]->nWarnNo;
					traderight[0] = reslist[i]->sRight[0];
					sprintf(warnmsg,"持仓价值比[%d]大于等于[%d]",posivalue,reslist[i]->dValue);
					break;
				}
			}
			else if (reslist[i]->sCondition[0] == EF_LARGE)
			{
				if (posivalue > reslist[i]->dValue)//大于
				{
					nRiskLevel= reslist[i]->nWarnNo;
					traderight[0] = reslist[i]->sRight[0];
					sprintf(warnmsg,"持仓价值比[%d]大于[%d]",posivalue,reslist[i]->dValue);
					break;
				}
			}
			else if (reslist[i]->sCondition[0] == EF_SMALLEQUAL)
			{
				if (posivalue < reslist[i]->dValue+MAXDOUBLECOMPARE)//小于等于
				{
					nRiskLevel= reslist[i]->nWarnNo;
					traderight[0] = reslist[i]->sRight[0];
					sprintf(warnmsg,"持仓价值比[%d]小于等于[%d]",posivalue,reslist[i]->dValue);
					break;
				}
			}
			else
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__," condition %s error ",reslist[i]->sCondition);
				return 10;
			}
		}
		else //持仓轧差价值比
		{
			if (reslist[i]->sCondition[0] == EF_LARGEEQUAL)
			{
				if (posidiffvalue > reslist[i]->dValue-MAXDOUBLECOMPARE)//大于等于
				{
					nRiskLevel= reslist[i]->nWarnNo;
					traderight[0] = reslist[i]->sRight[0];
					sprintf(warnmsg,"持仓轧差价值比[%d]大于等于[%d]",posidiffvalue,reslist[i]->dValue);
					break;
				}
			}
			else if (reslist[i]->sCondition[0] == EF_LARGE)
			{
				if (posidiffvalue > reslist[i]->dValue)//大于
				{
					nRiskLevel= reslist[i]->nWarnNo;
					traderight[0] = reslist[i]->sRight[0];
					sprintf(warnmsg,"持仓轧差价值比[%d]大于[%d]",posidiffvalue,reslist[i]->dValue);
					break;
				}
			}
			else if (reslist[i]->sCondition[0] == EF_SMALLEQUAL)
			{
				if (posidiffvalue < reslist[i]->dValue+MAXDOUBLECOMPARE)//小于等于
				{
					nRiskLevel= reslist[i]->nWarnNo;
					traderight[0] = reslist[i]->sRight[0];
					sprintf(warnmsg,"持仓轧差价值比[%d]小于等于[%d]",posidiffvalue,reslist[i]->dValue);
					break;
				}
			}
			else
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__," condition %s error ",reslist[i]->sCondition);
				return 10;
			}
		}
	}
	return nRiskLevel;
}
int CKernelPub::GetOrgRiskLevel(char *orgcode,double unitvalue,double risk,char *traderight,char *warnmsg)
{
	traderight[0] = TR_NORMAL;//默认为正常
	int nRiskLevel=10;

	S_MDB_PRODORG_TEMPLET_RELA *pOrgTemplet = m_pMemDb->m_pDbSet.m_pOrgTemplet.Select(orgcode);
	if (pOrgTemplet == NULL)
	{
		pOrgTemplet = m_pMemDb->m_pDbSet.m_pOrgTemplet.Select("$");
		if (pOrgTemplet == NULL)
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"机构[%s]无对应模板",orgcode);
			return 10;
		}
	}

	std::vector<S_MDB_PROD_MONITOR *> reslist;
	if (!m_pMemDb->m_pDbSet.m_pProdFundMonitor.Select(pOrgTemplet->sFundTempletId,reslist))
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"产品资金监控未配置");
		return 10;
	}

	for (int i=0 ; i<reslist.size() ; i++)
	{
		if (reslist[i]->sModel[0] == FMM_RISK) //指标为风险度
		{
			if (reslist[i]->sCondition[0] == EF_LARGEEQUAL)
			{
				if (risk > reslist[i]->dValue-MAXDOUBLECOMPARE)//大于等于
				{
					nRiskLevel= reslist[i]->nWarnNo;
					traderight[0] = reslist[i]->sRight[0];
					sprintf(warnmsg,"风险度[%d]大于等于[%d]",risk,reslist[i]->dValue);
					break;
				}
			}
			else if (reslist[i]->sCondition[0] == EF_LARGE)
			{
				if (risk > reslist[i]->dValue)//大于
				{
					nRiskLevel= reslist[i]->nWarnNo;
					traderight[0] = reslist[i]->sRight[0];
					sprintf(warnmsg,"风险度[%d]大于[%d]",risk,reslist[i]->dValue);
					break;
				}
			}
			else
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"condition %s error ",reslist[i]->sCondition);
				return 10;
			}

		}
		else //单位净值
		{
			if (reslist[i]->sCondition[0] == EF_SMALLEQUAL)
			{
				if (unitvalue < reslist[i]->dValue+MAXDOUBLECOMPARE)//小于等于
				{
					nRiskLevel= reslist[i]->nWarnNo;
					traderight[0] = reslist[i]->sRight[0];
					sprintf(warnmsg,"单位净值[%d]小于等于[%d]",unitvalue,reslist[i]->dValue);
					break;
				}
			}
			else
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__," condition %s error ",reslist[i]->sCondition);
				return 10;
			}
		}
	}
	return nRiskLevel;
}
int CKernelPub::GetOrgRiskLevel(char *orgcode,double marginrate,double posivalue,double posidiffvalue,char *traderight,char *warnmsg)
{
	traderight[0] = TR_NORMAL;//默认为正常
	int nRiskLevel=10;
	S_MDB_PRODORG_TEMPLET_RELA *pOrgTemplet = m_pMemDb->m_pDbSet.m_pOrgTemplet.Select(orgcode);
	if (pOrgTemplet == NULL)
	{
		pOrgTemplet = m_pMemDb->m_pDbSet.m_pOrgTemplet.Select("$");
		if (pOrgTemplet == NULL)
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"机构[%s]无对应模板",orgcode);
			return 10;
		}
	}
	std::vector<S_MDB_PROD_MONITOR *> reslist;
	if (!m_pMemDb->m_pDbSet.m_pProdPosiMonitor.Select(pOrgTemplet->sPosiTempletId,reslist))
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"产品持仓监控未配置");
		return 10;
	}
	for (int i=0 ; i<reslist.size() ; i++)
	{
		if (reslist[i]->sModel[0] == PM_MARGINRATE) //保证金占比
		{
			if (reslist[i]->sCondition[0] == EF_LARGEEQUAL)
			{
				if (marginrate > reslist[i]->dValue-MAXDOUBLECOMPARE)//大于等于
				{
					nRiskLevel= reslist[i]->nWarnNo;
					traderight[0] = reslist[i]->sRight[0];
					sprintf(warnmsg,"保证金占比[%d]大于等于[%d]",marginrate,reslist[i]->dValue);
					break;
				}
			}
			else if (reslist[i]->sCondition[0] == EF_LARGE)
			{
				if (marginrate > reslist[i]->dValue)//大于
				{
					nRiskLevel= reslist[i]->nWarnNo;
					traderight[0] = reslist[i]->sRight[0];
					sprintf(warnmsg,"保证金占比[%d]大于[%d]",marginrate,reslist[i]->dValue);
					break;
				}
			}
			else if (reslist[i]->sCondition[0] == EF_SMALLEQUAL)
			{
				if (marginrate < reslist[i]->dValue+MAXDOUBLECOMPARE)//小于等于
				{
					nRiskLevel= reslist[i]->nWarnNo;
					traderight[0] = reslist[i]->sRight[0];
					sprintf(warnmsg,"保证金占比[%d]小于等于[%d]",posivalue,reslist[i]->dValue);
					break;
				}
			}
			else
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"condition %s error ",reslist[i]->sCondition);
				return 10;
			}

		}
		else if (reslist[i]->sModel[0] == PM_POSIVALUE_RATE) //持仓价值比
		{
			if (reslist[i]->sCondition[0] == EF_LARGEEQUAL)
			{
				if (posivalue > reslist[i]->dValue-MAXDOUBLECOMPARE)//大于等于
				{
					nRiskLevel= reslist[i]->nWarnNo;
					traderight[0] = reslist[i]->sRight[0];
					sprintf(warnmsg,"持仓价值比[%d]大于等于[%d]",posivalue,reslist[i]->dValue);
					break;
				}
			}
			else if (reslist[i]->sCondition[0] == EF_LARGE)
			{
				if (posivalue > reslist[i]->dValue)//大于
				{
					nRiskLevel= reslist[i]->nWarnNo;
					traderight[0] = reslist[i]->sRight[0];
					sprintf(warnmsg,"持仓价值比[%d]大于[%d]",posivalue,reslist[i]->dValue);
					break;
				}
			}
			else if (reslist[i]->sCondition[0] == EF_SMALLEQUAL)
			{
				if (posivalue < reslist[i]->dValue+MAXDOUBLECOMPARE)//小于等于
				{
					nRiskLevel= reslist[i]->nWarnNo;
					traderight[0] = reslist[i]->sRight[0];
					sprintf(warnmsg,"持仓价值比[%d]小于等于[%d]",posivalue,reslist[i]->dValue);
					break;
				}
			}
			else
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__," condition %s error ",reslist[i]->sCondition);
				return 10;
			}
		}
		else //持仓轧差价值比
		{
			if (reslist[i]->sCondition[0] == EF_LARGEEQUAL)
			{
				if (posidiffvalue > reslist[i]->dValue-MAXDOUBLECOMPARE)//大于等于
				{
					nRiskLevel= reslist[i]->nWarnNo;
					traderight[0] = reslist[i]->sRight[0];
					sprintf(warnmsg,"持仓轧差价值比[%d]大于等于[%d]",posidiffvalue,reslist[i]->dValue);
					break;
				}
			}
			else if (reslist[i]->sCondition[0] == EF_LARGE)
			{
				if (posidiffvalue > reslist[i]->dValue)//大于
				{
					nRiskLevel= reslist[i]->nWarnNo;
					traderight[0] = reslist[i]->sRight[0];
					sprintf(warnmsg,"持仓轧差价值比[%d]大于[%d]",posidiffvalue,reslist[i]->dValue);
					break;
				}
			}
			else if (reslist[i]->sCondition[0] == EF_SMALLEQUAL)
			{
				if (posidiffvalue < reslist[i]->dValue+MAXDOUBLECOMPARE)//小于等于
				{
					nRiskLevel= reslist[i]->nWarnNo;
					traderight[0] = reslist[i]->sRight[0];
					sprintf(warnmsg,"持仓轧差价值比[%d]小于等于[%d]",posidiffvalue,reslist[i]->dValue);
					break;
				}
			}
			else
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__," condition %s error ",reslist[i]->sCondition);
				return 10;
			}
		}
	}
	return nRiskLevel;
}



bool CKernelPub::GetSettlePrice(int contractid, double &settleprice)
{
	S_MDB_CONTRACT_SETTLE_PRICE *psp = m_pMemDb->m_pDbBaseinfo.m_pSettlePrice.Select((unsigned int)contractid);
	if (psp == NULL)
	{
		//结算价没有，从行情表取
		S_MDB_QUOTATION *pq = m_pMemDb->m_pDbTrader.m_pQuotation.Select(contractid);
		if (pq == NULL)
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"合约编号[%d]没有结算价且无行情",contractid);
			return false;
		}
		if (pq->dLastSetllePrice <MAXDOUBLECOMPARE || pq->dLastSetllePrice>99999999)
		{
			//无上日结算价  取最新价
			settleprice = pq->dNewPrice;
			m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"合约编号[%d]没有结算价且无行情里也无结算价,取最新价",contractid);
			return true;
		}
		settleprice = pq->dLastSetllePrice;
		return true;
	}
	settleprice = psp->dSettlePrice;
	return true;
}
int CKernelPub::ProcDoneNetMargin(S_MDB_TXACC_LOCK *lockinfo,S_MDB_CUST_FUND *fundinfo,S_MDB_DONE_INFO *pd,S_MDB_ORDER_INFO *po,S_MDB_CONTRACT_INFO *pc,bool bisinit)
{
	int rid;
	S_MDB_BIG_MARGIN *pbm=NULL;
	S_MDB_BIG_MARGIN bm;
	S_MDB_CUST_POSITION *pPosi=NULL;
	
	int nQty=0;
	double dTmp=0.0;
	double dFrozeTmp=0.0;
	bzero(&bm,sizeof(S_MDB_BIG_MARGIN));
	
	if (pd->sEoFlag[0] == EO_FLAG_OPEN) //开仓
	{
		if (pd->sBsFlag[0] == BS_FLAG_BUY)  //开多仓
		{
			//查找表
			pbm = m_pMemDb->m_pDbTrader.m_pBigNetMargin.Select(pd->sTxAcc,pc->nProdId);
			if (pbm == NULL)
			{
				//成交但找不到记录
				pbm = AddBigMarginNull(pd->sTxAcc,pc->nProdId);
				if (pbm == NULL)
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"增加净边记录出错[%s %d]的",pd->sTxAcc,pc->nProdId);
					return -1;
				}
			}
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"开多仓 资金账号[%s]品种[%d] 净边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 冻结[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
			if (!bisinit) //不是初始化
			{
				//找到原来冻结的资金
				if (pbm->nFrozeFlag == 1)
				{
					//将原来的释放
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,0.00-pbm->dFrozeAmt,false))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
						return -1;
					}
				}
				else
				{
					//将原来的释放
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,0.00-pbm->dFrozeAmt,false))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
						return -1;
					}
				}
				if (po == NULL)
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"新成交，但无对应的报单");
					return -1;
				}
				pbm->dBuyFroze = pbm->dBuyFroze - po->dFrozAmt*pd->nDoneNum/po->nOrderNum; //买冻结减少
				pbm->nBuyQty = pbm->nBuyQty-pd->nDoneNum;//买冻量减少
			}
			//先释放持有保证金
			nQty = pbm->nBuyPosi-pbm->nSellPosi;
			if (pbm->nMarginFlag == 1) //净边在多 方向相同
			{
				if (nQty!=0)
				{
					dTmp = pbm->dBuyMargin*(nQty)/pbm->nBuyPosi;
					//释放保证金
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_BUY,0.0-dTmp,false))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
						return false;
					}
				}
					
			}
			else //净边在空 方向相反
			{
				dTmp = pbm->dSellMargin*(0-nQty)/pbm->nSellPosi;
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_BUY,0.0-dTmp,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
					return -1;
				}
						
			}
			//重新计算持有保证金及冻结资金
			pbm->nBuyPosi += pd->nDoneNum;
			pbm->dBuyMargin = pbm->dBuyMargin + pd->dExMargin+pd->dMemberMargin;
			nQty = pbm->nBuyPosi-pbm->nSellPosi;
			if (nQty == 0)
			{
				pbm->nMarginFlag =1;
			}
			else if (nQty > 0)
			{
				pbm->nMarginFlag =1;
				dTmp = pbm->dBuyMargin*(nQty)/pbm->nBuyPosi;
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_BUY,dTmp,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
					return -1;
				}
			}
			else
			{
				pbm->nMarginFlag =2;
				dTmp = pbm->dSellMargin*(nQty)/pbm->nSellPosi;
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_SELL,dTmp,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
					return -1;
				}
			}
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"开多仓 资金账号[%s]品种[%d] 净边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 冻结[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
			//重新计算冻结
			nQty = pbm->nBuyQty+pbm->nBuyPosi - pbm->nSellQty -pbm->nSellPosi;
			if (nQty == 0)  //买卖相等，啥都不用做
			{
				pbm->nFrozeFlag = 1; //冻结的在多
				pbm->dFrozeAmt=0.0;
			}
			else if (nQty > 0) //大于0 冻结的在买方
			{
				pbm->nFrozeFlag = 1; //冻结的在多
				if (pbm->nMarginFlag == 1) //持有的在多，同方向
				{
					//持有保证金
					dTmp = pbm->dBuyMargin*(pbm->nBuyPosi-pbm->nSellPosi)/pbm->nBuyPosi;
					dFrozeTmp = (pbm->dBuyMargin+pbm->dBuyFroze)*(nQty)/(pbm->nBuyPosi+pbm->nBuyQty);
					pbm->dFrozeAmt = dFrozeTmp-dTmp;
					if (pbm->dFrozeAmt > MAXDOUBLECOMPARE)
					{
						//进行新的冻结
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,pbm->dFrozeAmt,false))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
							return -1;
						}
					}
					else
					{
						pbm->dFrozeAmt = 0.0;//不用冻结
					}
				}
				else //持有的在空， 不同方向
				{
					//持有保证金
					dTmp = pbm->dSellMargin*(pbm->nSellPosi-pbm->nBuyPosi)/pbm->nSellPosi;
					dFrozeTmp = (pbm->dBuyMargin+pbm->dBuyFroze)*(nQty)/(pbm->nBuyPosi+pbm->nBuyQty);

					pbm->dFrozeAmt = dFrozeTmp-dTmp;//相减大于0是要去冻结的
					if (pbm->dFrozeAmt > MAXDOUBLECOMPARE)
					{
						//进行新的冻结
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,pbm->dFrozeAmt,false))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
							return -1;
						}
					}
					else
					{
						pbm->dFrozeAmt = 0.0;//不用冻结
					}
				}

			}
			else //冻结的在卖方
			{
				pbm->nFrozeFlag = 2;
				if (pbm->nMarginFlag == 1) //持有的在多，不同方向
				{
					//持有保证金
					dTmp = pbm->dBuyMargin*(pbm->nBuyPosi-pbm->nSellPosi)/pbm->nBuyPosi;
					dFrozeTmp = (pbm->dSellMargin+pbm->dSellFroze)*(0-nQty)/(pbm->nSellPosi+pbm->nSellQty);
					pbm->dFrozeAmt = dFrozeTmp-dTmp;
					if (pbm->dFrozeAmt > MAXDOUBLECOMPARE)
					{
						//进行新的冻结
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,pbm->dFrozeAmt,false))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
							return -1;
						}
					}
					else
					{
						pbm->dFrozeAmt = 0.0;//不用冻结
					}
				}
				else //持有的在空， 相同方向
				{
					//持有保证金
					dTmp = pbm->dSellMargin*(pbm->nSellPosi-pbm->nBuyPosi)/pbm->nSellPosi;
					dFrozeTmp = (pbm->dSellMargin+pbm->dSellFroze)*(0-nQty)/(pbm->nSellPosi+pbm->nSellQty);

					pbm->dFrozeAmt = dFrozeTmp-dTmp;//相减大于0是要去冻结的
					if (pbm->dFrozeAmt > MAXDOUBLECOMPARE)
					{
						//进行新的冻结
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,pbm->dFrozeAmt,false))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
							return -1;
						}
					}
					else
					{
						pbm->dFrozeAmt = 0.0;//不用冻结
					}
				}
			}
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"资金账号[%s]品种[%d] 净边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 冻结[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
			//手续费处理
			if (!bisinit) //不是初始化
			{
				//解冻手续费
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-pd->dUnfrozeAmt,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund 出错");
					return -1;
				}
			}
			//扣手续费
			if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FEE,FUND_CODE_FEE_EXCNANGE,pd->dExFee+pd->dMemberFee,false))
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund 出错");
				return -1;
			}
		}
		else  //卖开，开空仓
		{
			//查找表
			pbm = m_pMemDb->m_pDbTrader.m_pBigNetMargin.Select(pd->sTxAcc,pc->nProdId);
			if (pbm == NULL)
			{
				pbm = AddBigMarginNull(pd->sTxAcc,pc->nProdId);
				if (pbm == NULL)
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"增加净边记录出错[%s %d]的",pd->sTxAcc,pc->nProdId);
					return -1;
				}
			}
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"开空仓 资金账号[%s]品种[%d] 净边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 冻结[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
			if (!bisinit) //不是初始化
			{
				//找到原来冻结的资金
				if (pbm->nFrozeFlag == 1)
				{
					//将原来的释放
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,0.00-pbm->dFrozeAmt,false))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
						return -1;
					}
				}
				else
				{
					//将原来的释放
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,0.00-pbm->dFrozeAmt,false))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
						return -1;
					}
				}
				if (po == NULL)
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"新成交，但无对应的报单");
					return -1;
				}
				pbm->dSellFroze = pbm->dSellFroze - po->dFrozAmt*pd->nDoneNum/po->nOrderNum; //买冻结减少
				pbm->nSellQty = pbm->nSellQty-pd->nDoneNum;//买冻量减少
			}
			//先释放持有保证金
			nQty = pbm->nBuyPosi-pbm->nSellPosi;
			if (pbm->nMarginFlag == 1) //净边在多 方向相同
			{
				if (nQty!=0)
				{
					dTmp = pbm->dBuyMargin*(nQty)/pbm->nBuyPosi;
					//释放保证金
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_BUY,0.0-dTmp,false))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
						return -1;
					}
				}

			}
			else //净边在空 方向相反
			{
				dTmp = pbm->dSellMargin*(0-nQty)/pbm->nSellPosi;
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_BUY,0.0-dTmp,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
					return -1;
				}

			}
			//重新计算持有保证金及冻结资金
			pbm->nSellPosi += pd->nDoneNum;
			pbm->dSellMargin = pbm->dSellMargin + pd->dExMargin+pd->dMemberMargin;
			nQty = pbm->nBuyPosi-pbm->nSellPosi;
			if (nQty == 0)
			{
				pbm->nMarginFlag =1;
			}
			else if (nQty > 0)
			{
				pbm->nMarginFlag =1;
				dTmp = pbm->dBuyMargin*(nQty)/pbm->nBuyPosi;
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_BUY,dTmp,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
					return -1;
				}
			}
			else
			{
				pbm->nMarginFlag =2;
				dTmp = pbm->dSellMargin*(nQty)/pbm->nSellPosi;
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_SELL,dTmp,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
					return -1;
				}
			}
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"开空仓 资金账号[%s]品种[%d] 净边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 冻结[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
			//重新计算冻结
			nQty = pbm->nBuyQty+pbm->nBuyPosi - pbm->nSellQty -pbm->nSellPosi;
			if (nQty == 0)  //买卖相等，啥都不用做
			{
				pbm->nFrozeFlag = 1; //冻结的在多
				pbm->dFrozeAmt=0.0;
			}
			else if (nQty > 0) //大于0 冻结的在买方
			{
				pbm->nFrozeFlag = 1; //冻结的在多
				if (pbm->nMarginFlag == 1) //持有的在多，同方向
				{
					//持有保证金
					dTmp = pbm->dBuyMargin*(pbm->nBuyPosi-pbm->nSellPosi)/pbm->nBuyPosi;
					dFrozeTmp = (pbm->dBuyMargin+pbm->dBuyFroze)*(nQty)/(pbm->nBuyPosi+pbm->nBuyQty);
					pbm->dFrozeAmt = dFrozeTmp-dTmp;
					if (pbm->dFrozeAmt > MAXDOUBLECOMPARE)
					{
						//进行新的冻结
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,pbm->dFrozeAmt,false))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
							return -1;
						}
					}
					else
					{
						pbm->dFrozeAmt = 0.0;//不用冻结
					}
				}
				else //持有的在空， 不同方向
				{
					//持有保证金
					dTmp = pbm->dSellMargin*(pbm->nSellPosi-pbm->nBuyPosi)/pbm->nSellPosi;
					dFrozeTmp = (pbm->dBuyMargin+pbm->dBuyFroze)*(nQty)/(pbm->nBuyPosi+pbm->nBuyQty);

					pbm->dFrozeAmt = dFrozeTmp-dTmp;//相减大于0是要去冻结的
					if (pbm->dFrozeAmt > MAXDOUBLECOMPARE)
					{
						//进行新的冻结
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,pbm->dFrozeAmt,false))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
							return -1;
						}
					}
					else
					{
						pbm->dFrozeAmt = 0.0;//不用冻结
					}
				}

			}
			else //冻结的在卖方
			{
				pbm->nFrozeFlag = 2;
				if (pbm->nMarginFlag == 1) //持有的在多，不同方向
				{
					//持有保证金
					dTmp = pbm->dBuyMargin*(pbm->nBuyPosi-pbm->nSellPosi)/pbm->nBuyPosi;
					dFrozeTmp = (pbm->dSellMargin+pbm->dSellFroze)*(0-nQty)/(pbm->nSellPosi+pbm->nSellQty);
					pbm->dFrozeAmt = dFrozeTmp-dTmp;
					if (pbm->dFrozeAmt > MAXDOUBLECOMPARE)
					{
						//进行新的冻结
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,pbm->dFrozeAmt,false))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
							return -1;
						}
					}
					else
					{
						pbm->dFrozeAmt = 0.0;//不用冻结
					}
				}
				else //持有的在空， 相同方向
				{
					//持有保证金
					dTmp = pbm->dSellMargin*(pbm->nSellPosi-pbm->nBuyPosi)/pbm->nSellPosi;
					dFrozeTmp = (pbm->dSellMargin+pbm->dSellFroze)*(0-nQty)/(pbm->nSellPosi+pbm->nSellQty);

					pbm->dFrozeAmt = dFrozeTmp-dTmp;//相减大于0是要去冻结的
					if (pbm->dFrozeAmt > MAXDOUBLECOMPARE)
					{
						//进行新的冻结
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,pbm->dFrozeAmt,false))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
							return -1;
						}
					}
					else
					{
						pbm->dFrozeAmt = 0.0;//不用冻结
					}
				}
			}
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"资金账号[%s]品种[%d] 净边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 冻结[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
			//手续费处理
			if (!bisinit) //不是初始化
			{
				//解冻手续费
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-pd->dUnfrozeAmt,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund 出错");
					return -1;
				}
			}
			//扣手续费
			if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FEE,FUND_CODE_FEE_EXCNANGE,pd->dExFee+pd->dMemberFee,false))
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund 出错");
				return -1;
			}
		}
	}
	else //平仓 
	{
		//取持仓信息
		pPosi = m_pMemDb->m_pDbTrader.m_pPosition.Select(pd->sTxAcc,pd->sSeatNo,pd->sShFlag,pd->nContractNo);
		if (pPosi == NULL)
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"无持仓 资金账号[%s] 席位[%s] 投保标志[%s] 合约[%d %s] 数量[%d] 平仓",\
				pd->sTxAcc,pd->sSeatNo,pd->sShFlag,pd->nContractNo,pd->sContractCode,pd->nDoneNum);
			return -1;
		}
		//计算持仓
		if (pd->sBsFlag[0] == BS_FLAG_BUY) //买平  平空
		{
			//查找表
			pbm = m_pMemDb->m_pDbTrader.m_pBigNetMargin.Select(pd->sTxAcc,pc->nProdId);
			if (pbm == NULL)
			{
				//成交但找不到记录
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"成交但净边记录找不到[%s %d]的记录",pd->sTxAcc,pc->nProdId);
				return -1;
			}
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"平空仓 资金账号[%s]品种[%d] 净边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 冻结[%lf]",\
				pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,\
				pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
			if (!bisinit) //不是初始化
			{
				//找到原来冻结的资金
				if (pbm->nFrozeFlag == 1)
				{
					//将原来的释放
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,0.00-pbm->dFrozeAmt,false))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
						return -1;
					}
				}
				else
				{
					//将原来的释放
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,0.00-pbm->dFrozeAmt,false))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
						return -1;
					}
				}
			}
			//先释放持有保证金
			nQty = pbm->nBuyPosi-pbm->nSellPosi;
			if (pbm->nMarginFlag == 1) //净边在多 方向相同
			{
				if (nQty!=0)
				{
					dTmp = pbm->dBuyMargin*(nQty)/pbm->nBuyPosi;
					//释放保证金
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_BUY,0.0-dTmp,false))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
						return -1;
					}
				}

			}
			else //大边在空 方向相反
			{
				dTmp = pbm->dSellMargin*(0-nQty)/pbm->nSellPosi;
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_BUY,0.0-dTmp,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
					return -1;
				}

			}
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"平空仓 资金账号[%s]品种[%d] 净边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 冻结[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
			//重新计算持有保证金及冻结资金
			pbm->nSellPosi -= pd->nDoneNum;
			pbm->dSellMargin = pbm->dSellMargin - pPosi->dPS_Margin*(1.00*pd->nDoneNum/pPosi->nCurPS);
			nQty = pbm->nBuyPosi-pbm->nSellPosi;
			if (nQty == 0)
			{
				pbm->nMarginFlag =1;
			}
			else if (nQty > 0)
			{
				pbm->nMarginFlag =1;
				dTmp = pbm->dBuyMargin*(nQty)/pbm->nBuyPosi;
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_BUY,dTmp,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
					return -1;
				}
			}
			else
			{
				pbm->nMarginFlag =2;
				dTmp = pbm->dSellMargin*(nQty)/pbm->nSellPosi;
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_SELL,dTmp,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
					return -1;
				}
			}
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"平空仓 资金账号[%s]品种[%d] 净边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 冻结[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
			//重新计算冻结
			nQty = pbm->nBuyQty+pbm->nBuyPosi - pbm->nSellQty -pbm->nSellPosi;
			if (nQty == 0)  //买卖相等，啥都不用做
			{
				pbm->nFrozeFlag = 1; //冻结的在多
				pbm->dFrozeAmt=0.0;
			}
			else if (nQty > 0) //大于0 冻结的在买方
			{
				pbm->nFrozeFlag = 1; //冻结的在多
				if (pbm->nMarginFlag == 1) //持有的在多，同方向
				{
					//持有保证金
					dTmp = pbm->dBuyMargin*(pbm->nBuyPosi-pbm->nSellPosi)/pbm->nBuyPosi;
					dFrozeTmp = (pbm->dBuyMargin+pbm->dBuyFroze)*(nQty)/(pbm->nBuyPosi+pbm->nBuyQty);
					pbm->dFrozeAmt = dFrozeTmp-dTmp;
					if (pbm->dFrozeAmt > MAXDOUBLECOMPARE)
					{
						//进行新的冻结
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,pbm->dFrozeAmt,false))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
							return -1;
						}
					}
					else
					{
						pbm->dFrozeAmt = 0.0;//不用冻结
					}
				}
				else //持有的在空， 不同方向
				{
					//持有保证金
					dTmp = pbm->dSellMargin*(pbm->nSellPosi-pbm->nBuyPosi)/pbm->nSellPosi;
					dFrozeTmp = (pbm->dBuyMargin+pbm->dBuyFroze)*(nQty)/(pbm->nBuyPosi+pbm->nBuyQty);

					pbm->dFrozeAmt = dFrozeTmp-dTmp;//相减大于0是要去冻结的
					if (pbm->dFrozeAmt > MAXDOUBLECOMPARE)
					{
						//进行新的冻结
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,pbm->dFrozeAmt,false))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
							return -1;
						}
					}
					else
					{
						pbm->dFrozeAmt = 0.0;//不用冻结
					}
				}

			}
			else //冻结的在卖方
			{
				pbm->nFrozeFlag = 2;
				if (pbm->nMarginFlag == 1) //持有的在多，不同方向
				{
					//持有保证金
					dTmp = pbm->dBuyMargin*(pbm->nBuyPosi-pbm->nSellPosi)/pbm->nBuyPosi;
					dFrozeTmp = (pbm->dSellMargin+pbm->dSellFroze)*(0-nQty)/(pbm->nSellPosi+pbm->nSellQty);
					pbm->dFrozeAmt = dFrozeTmp-dTmp;
					if (pbm->dFrozeAmt > MAXDOUBLECOMPARE)
					{
						//进行新的冻结
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,pbm->dFrozeAmt,false))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
							return -1;
						}
					}
					else
					{
						pbm->dFrozeAmt = 0.0;//不用冻结
					}
				}
				else //持有的在空， 相同方向
				{
					//持有保证金
					dTmp = pbm->dSellMargin*(pbm->nSellPosi-pbm->nBuyPosi)/pbm->nSellPosi;
					dFrozeTmp = (pbm->dSellMargin+pbm->dSellFroze)*(0-nQty)/(pbm->nSellPosi+pbm->nSellQty);

					pbm->dFrozeAmt = dFrozeTmp-dTmp;//相减大于0是要去冻结的
					if (pbm->dFrozeAmt > MAXDOUBLECOMPARE)
					{
						//进行新的冻结
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,pbm->dFrozeAmt,false))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
							return -1;
						}
					}
					else
					{
						pbm->dFrozeAmt = 0.0;//不用冻结
					}
				}
			}
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"平空仓 资金账号[%s]品种[%d] 净边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 冻结[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);

			if (!bisinit)
			{
				//解冻委托冻结的持仓及手续费
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-pd->dUnfrozeFee,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund 出错");
					return -1;
				}
			}
			//费用
			if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FEE,FUND_CODE_FEE_EXCNANGE,pd->dExFee+pd->dMemberFee,false))
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund 出错");
				return -1;
			}
		}
		else //卖平 平多
		{
			//查找表
			pbm = m_pMemDb->m_pDbTrader.m_pBigNetMargin.Select(pd->sTxAcc,pc->nProdId);
			if (pbm == NULL)
			{
				//成交但找不到记录
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"成交但净边记录找不到[%s %d]的记录",pd->sTxAcc,pc->nProdId);
				return -1;
			}
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"平多仓 资金账号[%s]品种[%d] 净边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 冻结[%lf]",\
				pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,\
				pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
			if (!bisinit) //不是初始化
			{
				//找到原来冻结的资金
				if (pbm->nFrozeFlag == 1)
				{
					//将原来的释放
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,0.00-pbm->dFrozeAmt,false))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
						return -1;
					}
				}
				else
				{
					//将原来的释放
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,0.00-pbm->dFrozeAmt,false))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
						return -1;
					}
				}
			}
			//先释放持有保证金
			nQty = pbm->nBuyPosi-pbm->nSellPosi;
			if (pbm->nMarginFlag == 1) //净边在多 方向相同
			{
				if (nQty!=0)
				{
					dTmp = pbm->dBuyMargin*(nQty)/pbm->nBuyPosi;
					//释放保证金
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_BUY,0.0-dTmp,false))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
						return -1;
					}
				}

			}
			else //大边在空 方向相反
			{
				dTmp = pbm->dSellMargin*(0-nQty)/pbm->nSellPosi;
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_BUY,0.0-dTmp,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
					return -1;
				}

			}
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"平多仓 资金账号[%s]品种[%d] 净边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 冻结[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
			//重新计算持有保证金及冻结资金
			pbm->nBuyPosi -= pd->nDoneNum;
			pbm->dBuyMargin = pbm->dBuyMargin - pPosi->dPB_Margin*(1.00*pd->nDoneNum/pPosi->nCurPB);
			nQty = pbm->nBuyPosi-pbm->nSellPosi;
			if (nQty == 0)
			{
				pbm->nMarginFlag =1;
			}
			else if (nQty > 0)
			{
				pbm->nMarginFlag =1;
				dTmp = pbm->dBuyMargin*(nQty)/pbm->nBuyPosi;
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_BUY,dTmp,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
					return -1;
				}
			}
			else
			{
				pbm->nMarginFlag =2;
				dTmp = pbm->dSellMargin*(nQty)/pbm->nSellPosi;
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_SELL,dTmp,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
					return -1;
				}
			}
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"平多仓 资金账号[%s]品种[%d] 净边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 冻结[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
			//重新计算冻结
			nQty = pbm->nBuyQty+pbm->nBuyPosi - pbm->nSellQty -pbm->nSellPosi;
			if (nQty == 0)  //买卖相等，啥都不用做
			{
				pbm->nFrozeFlag = 1; //冻结的在多
				pbm->dFrozeAmt=0.0;
			}
			else if (nQty > 0) //大于0 冻结的在买方
			{
				pbm->nFrozeFlag = 1; //冻结的在多
				if (pbm->nMarginFlag == 1) //持有的在多，同方向
				{
					//持有保证金
					dTmp = pbm->dBuyMargin*(pbm->nBuyPosi-pbm->nSellPosi)/pbm->nBuyPosi;
					dFrozeTmp = (pbm->dBuyMargin+pbm->dBuyFroze)*(nQty)/(pbm->nBuyPosi+pbm->nBuyQty);
					pbm->dFrozeAmt = dFrozeTmp-dTmp;
					if (pbm->dFrozeAmt > MAXDOUBLECOMPARE)
					{
						//进行新的冻结
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,pbm->dFrozeAmt,false))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
							return -1;
						}
					}
					else
					{
						pbm->dFrozeAmt = 0.0;//不用冻结
					}
				}
				else //持有的在空， 不同方向
				{
					//持有保证金
					dTmp = pbm->dSellMargin*(pbm->nSellPosi-pbm->nBuyPosi)/pbm->nSellPosi;
					dFrozeTmp = (pbm->dBuyMargin+pbm->dBuyFroze)*(nQty)/(pbm->nBuyPosi+pbm->nBuyQty);

					pbm->dFrozeAmt = dFrozeTmp-dTmp;//相减大于0是要去冻结的
					if (pbm->dFrozeAmt > MAXDOUBLECOMPARE)
					{
						//进行新的冻结
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,pbm->dFrozeAmt,false))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
							return -1;
						}
					}
					else
					{
						pbm->dFrozeAmt = 0.0;//不用冻结
					}
				}

			}
			else //冻结的在卖方
			{
				pbm->nFrozeFlag = 2;
				if (pbm->nMarginFlag == 1) //持有的在多，不同方向
				{
					//持有保证金
					dTmp = pbm->dBuyMargin*(pbm->nBuyPosi-pbm->nSellPosi)/pbm->nBuyPosi;
					dFrozeTmp = (pbm->dSellMargin+pbm->dSellFroze)*(0-nQty)/(pbm->nSellPosi+pbm->nSellQty);
					pbm->dFrozeAmt = dFrozeTmp-dTmp;
					if (pbm->dFrozeAmt > MAXDOUBLECOMPARE)
					{
						//进行新的冻结
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,pbm->dFrozeAmt,false))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
							return -1;
						}
					}
					else
					{
						pbm->dFrozeAmt = 0.0;//不用冻结
					}
				}
				else //持有的在空， 相同方向
				{
					//持有保证金
					dTmp = pbm->dSellMargin*(pbm->nSellPosi-pbm->nBuyPosi)/pbm->nSellPosi;
					dFrozeTmp = (pbm->dSellMargin+pbm->dSellFroze)*(0-nQty)/(pbm->nSellPosi+pbm->nSellQty);

					pbm->dFrozeAmt = dFrozeTmp-dTmp;//相减大于0是要去冻结的
					if (pbm->dFrozeAmt > MAXDOUBLECOMPARE)
					{
						//进行新的冻结
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,pbm->dFrozeAmt,false))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
							return -1;
						}
					}
					else
					{
						pbm->dFrozeAmt = 0.0;//不用冻结
					}
				}
			}
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"平多仓 资金账号[%s]品种[%d] 净边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 冻结[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);

			if (!bisinit)
			{
				//解冻委托冻结的持仓及手续费
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-pd->dUnfrozeFee,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund 出错");
					return -1;
				}
			}
			//费用
			if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FEE,FUND_CODE_FEE_EXCNANGE,pd->dExFee+pd->dMemberFee,false))
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund 出错");
				return -1;
			}
		}
	}
	
	return SUCCESS;
}
S_MDB_BIG_MARGIN *CKernelPub::AddBigMarginNull(char *txacc,int nvariety)
{
	S_MDB_BIG_MARGIN info;
	bzero(&info,sizeof(S_MDB_BIG_MARGIN));
	strcpy(info.sTxAcc,txacc);
	info.nVarietyId = nvariety;
	info.nFrozeFlag = 1;
	info.nMarginFlag = 1;
	int rid = m_pMemDb->m_pDbTrader.m_pBigNetMargin.Insert(info);
	if (rid<0)
	{
		return NULL;
	}
	return m_pMemDb->m_pDbTrader.m_pBigNetMargin.At(rid);
}
bool CKernelPub::UpdatePosition(S_MDB_TXACC_LOCK *lockinfo,S_MDB_CUST_FUND *fundinfo,S_MDB_DONE_INFO *pd,S_MDB_CUST_POSITION *ps,S_MDB_CONTRACT_INFO *pc,bool isinit)
{
	//更新持仓
	S_MDB_CUST_POSITION *pPosi=ps;
	if (pPosi == NULL)
	{
		pPosi = m_pMemDb->m_pDbTrader.m_pPosition.Select(pd->sTxAcc,pd->sSeatNo,pd->sShFlag,pd->nContractNo);
		if (pPosi == NULL)
		{
			if (pd->sEoFlag[0] != EO_FLAG_OPEN) //不是开仓
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"不是开仓，但无持仓信息 资金账号[%s] 席位[%s] 投保标志[%s] 合约[%d %s] 数量[%d] 成交编号[%s]",\
					pd->sTxAcc,pd->sSeatNo,pd->sShFlag,pd->nContractNo,pd->sContractCode,pd->nDoneNum,pd->sDoneNo);
				return false;
			}
			S_MDB_CUST_POSITION posi;
			bzero(&posi,sizeof(S_MDB_CUST_POSITION));
			strcpy(posi.sTxAcc,pd->sTxAcc);
			strcpy(posi.sSeatNo,pd->sSeatNo);
			strcpy(posi.sShFlag,pd->sShFlag);
			strcpy(posi.sTradeCode,pd->sTxCode);
			posi.nContractNo = pd->nContractNo;
			strcpy(posi.sContractCode,pd->sContractCode);
			posi.nVarietyId = pd->nVarietyId;

			if (pd->sBsFlag[0] == BS_FLAG_BUY) //买开
			{
				posi.dPB_Avp = pd->dDonePrice;
				posi.dPB_Margin = pd->dExMargin+pd->dMemberMargin;
				posi.nDayPB = pd->nDoneNum;
				posi.nCurCanUsePB = pd->nDoneNum;
				posi.nCurPB = pd->nDoneNum;
				posi.dPB_OAvp = pd->dDonePrice;
			}
			else //卖开
			{
				posi.dPS_Avp = pd->dDonePrice;
				posi.dPS_Margin = pd->dExMargin+pd->dMemberMargin;
				posi.nDayPS = pd->nDoneNum;
				posi.nCurCanUsePS = pd->nDoneNum;
				posi.nCurPS = pd->nDoneNum;
				posi.dPS_OAvp = pd->dDonePrice;
			}
			posi.nVarietyId = pc->nProdId;
			if (m_pMemDb->m_pDbTrader.m_pPosition.Insert(posi)<0)
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"写入持仓表失败 资金账号[%s] 席位[%s] 投保标志[%s] 合约[%d %s] 数量[%d] 开多",\
					pd->sTxAcc,pd->sSeatNo,pd->sShFlag,pd->nContractNo,pd->sContractCode,pd->nDoneNum);
				return false;
			}
			return true;
		}
	}
	

	if (pc->sIsBigMargin[0] == BOOL_FALSE) //双边持仓
	{
		if (pd->sEoFlag[0] == EO_FLAG_OPEN) //开仓
		{
			//计算持仓均价
			if (pd->sBsFlag[0] == BS_FLAG_BUY) //买开 多仓
			{
				//均价
				pPosi->dPB_Avp = (pPosi->dPB_Avp*pPosi->nCurPB+pd->dDonePrice*pd->nDoneNum)/(pPosi->nCurPB+pd->nDoneNum);
				pPosi->dPB_Margin = pPosi->dPB_Margin + pd->dExMargin+pd->dMemberMargin;
				pPosi->nDayPB = pPosi->nDayPB+pd->nDoneNum;
				pPosi->nCurCanUsePB = pPosi->nCurCanUsePB+pd->nDoneNum;
				pPosi->nCurPB = pPosi->nCurPB+pd->nDoneNum;
				//开仓均价
				pPosi->dPB_OAvp = (pPosi->dPB_OAvp*pPosi->nCurPB+pd->dDonePrice*pd->nDoneNum)/(pPosi->nCurPB+pd->nDoneNum);
			}
			else //卖开 空仓
			{
				//均价
				pPosi->dPS_Avp = (pPosi->dPS_Avp*pPosi->nCurPS+pd->dDonePrice*pd->nDoneNum)/(pPosi->nCurPS+pd->nDoneNum);
				pPosi->dPS_Margin = pPosi->dPS_Margin + pd->dExMargin+pd->dMemberMargin;
				pPosi->nDayPS = pPosi->nDayPS+pd->nDoneNum;
				pPosi->nCurCanUsePS = pPosi->nCurCanUsePS+pd->nDoneNum;
				pPosi->nCurPS = pPosi->nCurPS+pd->nDoneNum;
				//开仓均价
				pPosi->dPS_OAvp = (pPosi->dPS_OAvp*pPosi->nCurPS+pd->dDonePrice*pd->nDoneNum)/(pPosi->nCurPS+pd->nDoneNum);
			}
		}
		else //平仓
		{
			//计算持仓
			if (pd->sBsFlag[0] == BS_FLAG_BUY) //买平  平空
			{
				pPosi->dPS_Margin = pPosi->dPS_Margin - (pPosi->dPS_Margin*pd->nDoneNum/pPosi->nCurPS);
				if (!isinit)
				{
					pPosi->nDAY_COV_PS_Froze = pPosi->nDAY_COV_PS_Froze-pd->nDoneNum;
					
				}
				
				//冻结减少，可用不变
				pPosi->nCurPS = pPosi->nCurPS-pd->nDoneNum;
				//计算今开持仓
				if (pd->sEoFlag[0] == EO_FLAG_CLOSETODAY) //平今仓
				{
					pPosi->nDayPS = pPosi->nDayPS -  pd->nDoneNum;
				}
				else //平昨
				{
					if (pPosi->nCurPS < pPosi->nDayPS)
					{
						pPosi->nDayPS = pPosi->nCurPS;//剩下的全是今持仓了
					}
				}
				//计算平仓盈亏
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_SURPLUS,FUND_CODE_SURPLUS_CLOSE,(pPosi->dPS_Avp-pd->dDonePrice)*pd->nDoneNum*pc->dTxUnit,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
					return false;
				}
			}
			else //卖平 平多
			{
				pPosi->dPB_Margin = pPosi->dPB_Margin - pPosi->dPB_Margin*pd->nDoneNum/pPosi->nCurPB;
				//冻结减少，可用不变
				if (!isinit)
				{
					pPosi->nDAY_COV_PB_Froze = pPosi->nDAY_COV_PB_Froze-pd->nDoneNum;
				}
				//pPosi->nCurCanUsePB = pPosi->nCurCanUsePB-info->nDoneNum;
				pPosi->nCurPB = pPosi->nCurPB-pd->nDoneNum;
				//计算今开持仓
				if (pd->sEoFlag[0] == EO_FLAG_CLOSETODAY) //平今仓
				{
					pPosi->nDayPB = pPosi->nDayPB -  pd->nDoneNum;
				}
				else //平昨
				{
					if (pPosi->nCurPB < pPosi->nDayPB)
					{
						pPosi->nDayPB = pPosi->nCurPB;//剩下的全是今持仓了
					}

				}
				//计算平仓盈亏
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_SURPLUS,FUND_CODE_SURPLUS_CLOSE,(pd->dDonePrice-pPosi->dPB_Avp)*pd->nDoneNum*pc->dTxUnit,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
					return false;
				}
			}
		}
	}
	else //外盘单边持仓处理
	{
		if (pd->sBsFlag[0] == BS_FLAG_BUY) //买开 多仓
		{
			if (pPosi->nCurPB>0) //单边为多
			{
				//均价
				pPosi->dPB_Avp = (pPosi->dPB_Avp*pPosi->nCurPB+pd->dDonePrice*pd->nDoneNum)/(pPosi->nCurPB+pd->nDoneNum);
				pPosi->dPB_Margin = pPosi->dPB_Margin + pd->dExMargin+pd->dMemberMargin;
				pPosi->nDayPB = pPosi->nDayPB+pd->nDoneNum;
				pPosi->nCurCanUsePB = pPosi->nCurCanUsePB+pd->nDoneNum;
				pPosi->nCurPB = pPosi->nCurPB+pd->nDoneNum;
				//开仓均价
				pPosi->dPB_OAvp = (pPosi->dPB_OAvp*pPosi->nCurPB+pd->dDonePrice*pd->nDoneNum)/(pPosi->nCurPB+pd->nDoneNum);
			}
			else //原持仓为空，改为平仓
			{
				if (pPosi->nCurPS > pd->nDoneNum) //持仓仍为空
				{
					pPosi->dPS_Margin = pPosi->dPS_Margin - (pPosi->dPS_Margin*pd->nDoneNum/pPosi->nCurPS);
					//单边无冻结
					pPosi->nCurPS = pPosi->nCurPS-pd->nDoneNum;
					//计算今开持仓
					if (pPosi->nCurPS < pPosi->nDayPS)
					{
						pPosi->nDayPS = pPosi->nCurPS;//剩下的全是今持仓了
					}
					//计算平仓盈亏
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_SURPLUS,FUND_CODE_SURPLUS_CLOSE,(pPosi->dPS_Avp-pd->dDonePrice)*pd->nDoneNum*pc->dTxUnit,false))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
						return false;
					}
				}
				else //全部平掉，持仓变为多仓
				{
					//均价
					pPosi->dPB_Avp = pd->dDonePrice;
					pPosi->dPB_Margin = (pd->dExMargin+pd->dMemberMargin)*(pd->nDoneNum-pPosi->nCurPS)/pd->nDoneNum;
					pPosi->nDayPB = pd->nDoneNum-pPosi->nCurPS;
					pPosi->nCurCanUsePB = pPosi->nDayPB;
					pPosi->nCurPB = pPosi->nDayPB;
					//开仓均价
					pPosi->dPB_OAvp = pPosi->dPB_Avp;

					pPosi->dPS_Margin = 0;
					//单边无冻结
					pPosi->nCurPS = 0;
					pPosi->nDayPS=0;
					//计算平仓盈亏
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_SURPLUS,FUND_CODE_SURPLUS_CLOSE,(pPosi->dPS_Avp-pd->dDonePrice)*pd->nDoneNum*pc->dTxUnit,false))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
						return false;
					}
				}
			}
			
		}
		else //卖开 空仓
		{
			if (pPosi->nCurPS>0) //单边为空
			{
				//均价
				pPosi->dPS_Avp = (pPosi->dPS_Avp*pPosi->nCurPS+pd->dDonePrice*pd->nDoneNum)/(pPosi->nCurPS+pd->nDoneNum);
				pPosi->dPS_Margin = pPosi->dPS_Margin + pd->dExMargin+pd->dMemberMargin;
				pPosi->nDayPS = pPosi->nDayPS+pd->nDoneNum;
				pPosi->nCurCanUsePS = pPosi->nCurCanUsePS+pd->nDoneNum;
				pPosi->nCurPS = pPosi->nCurPS+pd->nDoneNum;
				//开仓均价
				pPosi->dPS_OAvp = (pPosi->dPS_OAvp*pPosi->nCurPS+pd->dDonePrice*pd->nDoneNum)/(pPosi->nCurPS+pd->nDoneNum);
			}
			else //原持仓为多，改为平仓
			{
				if (pPosi->nCurPB > pd->nDoneNum) //持仓仍为空
				{
					pPosi->dPB_Margin = pPosi->dPB_Margin - (pPosi->dPB_Margin*pd->nDoneNum/pPosi->nCurPB);
					//单边无冻结
					pPosi->nCurPB = pPosi->nCurPB-pd->nDoneNum;
					//计算今开持仓
					if (pPosi->nCurPB < pPosi->nDayPB)
					{
						pPosi->nDayPB = pPosi->nCurPB;//剩下的全是今持仓了
					}
					//计算平仓盈亏
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_SURPLUS,FUND_CODE_SURPLUS_CLOSE,(pd->dDonePrice-pPosi->dPB_Avp)*pd->nDoneNum*pc->dTxUnit,false))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
						return false;
					}
				}
				else //全部平掉，持仓变为多仓
				{
					//均价
					pPosi->dPS_Avp = pd->dDonePrice;
					pPosi->dPS_Margin = (pd->dExMargin+pd->dMemberMargin)*(pd->nDoneNum-pPosi->nCurPB)/pd->nDoneNum;
					pPosi->nDayPS = pd->nDoneNum-pPosi->nCurPB;
					pPosi->nCurCanUsePS = pPosi->nDayPS;
					pPosi->nCurPS = pPosi->nDayPS;
					//开仓均价
					pPosi->dPS_OAvp = pPosi->dPS_Avp;

					pPosi->dPB_Margin = 0;
					//单边无冻结
					pPosi->nCurPB = 0;
					pPosi->nDayPB=0;
					//计算平仓盈亏
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_SURPLUS,FUND_CODE_SURPLUS_CLOSE,(pd->dDonePrice-pPosi->dPB_Avp)*pd->nDoneNum*pc->dTxUnit,false))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
						return false;
					}

				}
			}
		}
	}
	return true;
}

int CKernelPub::GetFeeMargin(S_MDB_ORDER_INFO *porder,S_MDB_CONTRACT_INFO *pc, S_MDB_CUST_FUNDACC_INFO *pcust, double &exfee, double &memberfee,double &exmargin,double &membermargin)
{
	//取要冻结的手续费及保证金等等
	switch (pc->sProd_type[0])
	{
		case PROD_TYPE_FUTURES://期货
			if (porder->sEoFlag[0] == EO_FLAG_OPEN) //开仓
			{
				//开仓要算冻结和手续费
				if (!GetTradeFeeValue(pc,pcust,1,porder->nOrderNum,porder->dAmt,exfee,memberfee))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托通知取手续费出错 本地报单号[%s]",porder->sLocalSerial);
					return -1;
				}
				if (!GetMarginValue(pc,pcust,0,porder->nOrderNum,porder->dAmt,exmargin,membermargin))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托通知取保证金出错 本地报单号[%s]",porder->sLocalSerial);
					return -2;
				}
			}
			else
			{
				if (porder->sEoFlag[0] == EO_FLAG_CLOSETODAY) //平今仓
				{
					//平仓要算手续费
					if (!GetTradeFeeValue(pc,pcust,3,porder->nOrderNum,porder->dAmt,exfee,memberfee))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托通知取手续费出错 本地报单号[%s]",porder->sLocalSerial);
						return -1;
					}
				}
				else
				{
					//平仓要算手续费
					if (!GetTradeFeeValue(pc,pcust,2,porder->nOrderNum,porder->dAmt,exfee,memberfee))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托通知取手续费出错 本地报单号[%s]",porder->sLocalSerial);
						return -1;
					}
				}
			}
			break;
		case PROD_TYPE_OPTION://期权
			if (porder->sEoFlag[0] == EO_FLAG_OPEN) //开仓
			{
				//开仓要算冻结和手续费
				if (!GetTradeFeeValue(pc,pcust,1,porder->nOrderNum,porder->dAmt,exfee,memberfee))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托通知取手续费出错 本地报单号[%s]",porder->sLocalSerial);
					return -1;
				}
				if (porder->sBsFlag[0] == BS_FLAG_SELL) //卖开要冻结保证金
				{
					if (!GetMarginValue(pc,pcust,1,porder->nOrderNum,porder->dAmt,exmargin,membermargin))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托通知取保证金出错 本地报单号[%s]",porder->sLocalSerial);
						return -2;
					}
				}
			}
			else
			{
				//平仓要算手续费
				if (porder->sEoFlag[0] == EO_FLAG_CLOSETODAY) //平今仓
				{
					//平仓要算手续费
					if (!GetTradeFeeValue(pc,pcust,3,porder->nOrderNum,porder->dAmt,exfee,memberfee))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托通知取手续费出错 本地报单号[%s]",porder->sLocalSerial);
						return -1;
					}
				}
				else
				{
					//平仓要算手续费
					if (!GetTradeFeeValue(pc,pcust,2,porder->nOrderNum,porder->dAmt,exfee,memberfee))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托通知取手续费出错 本地报单号[%s]",porder->sLocalSerial);
						return -1;
					}
				}
			}
			break;
		case PROD_TYPE_SPOT://现货
			//现货有中立仓、交割等
			if (porder->sShFlag[0] == SH_FLAG_DERY)
			{
				//递延交割
			}
			else if (porder->sShFlag[0] == SH_FLAG_MID)
			{
				//中立仓
			}
			else
			{
				if (porder->sBsFlag[0] == BS_FLAG_BUY)
				{
					//买要算手续费及金额
					if (!GetTradeFeeValue(pc,pcust,1,porder->nOrderNum,porder->dAmt,exfee,memberfee))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托通知取手续费出错 本地报单号[%s]",porder->sLocalSerial);
						return -1;
					}
				}
				else
				{
					//卖要算手续费	
					if (!GetTradeFeeValue(pc,pcust,2,porder->nOrderNum,porder->dAmt,exfee,memberfee))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托通知取手续费出错 本地报单号[%s]",porder->sLocalSerial);
						return -1;
					}
				}
			}
			membermargin = porder->dAmt;
			break;
		case PROD_TYPE_SEC://证券
			if (porder->sBsFlag[0] == BS_FLAG_BUY)
			{
				//买要算手续费及金额
				if (!GetTradeFeeValue(pc,pcust,1,porder->nOrderNum,porder->dAmt,exfee,memberfee))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托通知取手续费出错 本地报单号[%s]",porder->sLocalSerial);
					return -1;
				}
			}
			else
			{
				//卖要算手续费
				if (!GetTradeFeeValue(pc,pcust,2,porder->nOrderNum,porder->dAmt,exfee,memberfee))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托通知取手续费出错 本地报单号[%s]",porder->sLocalSerial);
					return -1;
				}
			}
			membermargin = porder->dAmt;
			break;
		default:
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托通知合约代码[%s]的品种类别[%s]不符",porder->sContractCode,pc->sProd_type);
			return -3;
	}
	return 0;
}
int CKernelPub::ProcOrderNetMargin(S_MDB_TXACC_LOCK *lockinfo,S_MDB_CUST_FUND *fundinfo,S_MDB_ORDER_INFO *po,S_MDB_CONTRACT_INFO *pc,bool bisinit,bool checkfund)
{
	int rid;
	S_MDB_BIG_MARGIN *pbm=NULL;
	S_MDB_BIG_MARGIN bm;
	S_MDB_CUST_POSITION *pPosi=NULL;
	int nQty=0;
	double dTmp=0.0;
	double dFrozeTmp=0.0;
	bzero(&bm,sizeof(S_MDB_BIG_MARGIN));

	double dLastFroze=0.0;
	int    nLastFlag=1;

	bool ischeckfund = checkfund;
	if (bisinit)
	{
		ischeckfund=false;
	}

	//报单处理
	if (po->sEoFlag[0] == EO_FLAG_OPEN) //开仓
	{
		if (po->sBsFlag[0] == BS_FLAG_BUY) //买开
		{
			//查找表
			pbm = m_pMemDb->m_pDbTrader.m_pBigNetMargin.Select(po->sTxAcc,pc->nProdId);
			if (pbm == NULL)//没有新增
			{
				strcpy(bm.sTxAcc,po->sTxAcc);
				bm.nMarginFlag = 1;
				bm.sBailMode= BAIL_MODE_NET;
				bm.nVarietyId = pc->nProdId;
				if (bisinit) //表示从数据库读的，用剩余量来算
				{
					bm.dFrozeAmt = po->dFrozAmt*po->nRemainNum/po->nOrderNum;
					bm.dBuyFroze = bm.dFrozeAmt;
					bm.nBuyQty = po->nRemainNum;
				}
				else
				{
					bm.dFrozeAmt = po->dFrozAmt;
					bm.dBuyFroze = po->dFrozAmt;
					bm.nBuyQty = po->nOrderNum;
				}
				bm.nFrozeFlag = 1;
				rid = m_pMemDb->m_pDbTrader.m_pBigNetMargin.Insert(bm);
				if (rid<0)
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"写品种大边净边表出错 资金账号[%s]品种[%d]",po->sTxAcc,pc->nProdId);
					po->sOrderStatus[0] = ORDER_STATUS_INVALID;
					strcpy(po->sOrderRsp,"写品种大边净边表出错");
					return RET_ERR_SYSTEM;
				}
				pbm =  m_pMemDb->m_pDbTrader.m_pBigNetMargin.At(rid);
				m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"资金账号[%s]品种[%d] 净边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 买冻量[%d] 卖冻量[%d] 冻结[%lf]",\
					pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,\
					pbm->dBuyFroze,pbm->dSellFroze,pbm->nBuyQty,pbm->nSellQty,pbm->dFrozeAmt);
				if (ischeckfund)
				{
					if (fundinfo->dAvailFund < bm.dFrozeAmt-po->dExFee)
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"可用资金不足   客户[%s %s]可用[%.4lf]，要冻结[%.4lf]",fundinfo->sTxAcc,fundinfo->sCurrency,fundinfo->dAvailFund,bm.dFrozeAmt-po->dExFee);
						po->sOrderStatus[0] = ORDER_STATUS_INVALID;
						strcpy(po->sOrderRsp,"可用资金不足");
						return RET_ERR_AVAILFUND;
					}
				}
				//冻买保证金
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,bm.dFrozeAmt,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
					po->sOrderStatus[0] = ORDER_STATUS_INVALID;
					strcpy(po->sOrderRsp,"更新资金出错");
					return RET_ERR_SYSTEM;
				}
			}
			else //计算该冻结多少
			{
				m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"资金账号[%s]品种[%d] 净边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 买冻量[%d] 卖冻量[%d] 冻结[%lf]",\
					pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,\
					pbm->dBuyFroze,pbm->dSellFroze,pbm->nBuyQty,pbm->nSellQty,pbm->dFrozeAmt);
				if (bisinit) //表示从数据库读的，用剩余量来算
				{
					pbm->dBuyFroze = pbm->dBuyFroze+po->dFrozAmt*po->nRemainNum/po->nOrderNum;
					pbm->nBuyQty = pbm->nBuyQty+po->nRemainNum;
				}
				else
				{
					pbm->dBuyFroze = pbm->dBuyFroze+po->dFrozAmt;
					pbm->nBuyQty = pbm->nBuyQty+po->nOrderNum;
				}
				nLastFlag = pbm->nFrozeFlag;
				dLastFroze = pbm->dFrozeAmt;
				
				nQty = pbm->nBuyQty+pbm->nBuyPosi - pbm->nSellQty -pbm->nSellPosi;
				if (nQty == 0)  //买卖相等，啥都不用做
				{
					pbm->nFrozeFlag = 1; //冻结的在多
					pbm->dFrozeAmt=0.0;
				}
				else if (nQty > 0) //大于0 冻结的在买方
				{
					pbm->nFrozeFlag = 1; //冻结的在多
					if (pbm->nMarginFlag == 1) //持有的在多，同方向
					{
						//持有保证金
						dTmp = pbm->dBuyMargin*(pbm->nBuyPosi-pbm->nSellPosi)/pbm->nBuyPosi;
						dFrozeTmp = (pbm->dBuyMargin+pbm->dBuyFroze)*(nQty)/(pbm->nBuyPosi+pbm->nBuyQty);

						pbm->dFrozeAmt = dFrozeTmp-dTmp;
						if (pbm->dFrozeAmt > MAXDOUBLECOMPARE)
						{
							////进行新的冻结
							//if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,pbm->dFrozeAmt,false))
							//{
							//	m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
							//	return false;
							//}
						}
						else
						{
							pbm->dFrozeAmt = 0.0;//不用冻结
						}
					}
					else //持有的在空， 不同方向
					{
						//持有保证金
						dTmp = pbm->dSellMargin*(pbm->nSellPosi-pbm->nBuyPosi)/pbm->nSellPosi;
						dFrozeTmp = (pbm->dBuyMargin+pbm->dBuyFroze)*(nQty)/(pbm->nBuyPosi+pbm->nBuyQty);

						pbm->dFrozeAmt = dFrozeTmp-dTmp;//相减大于0是要去冻结的
						if (pbm->dFrozeAmt > MAXDOUBLECOMPARE)
						{
							////进行新的冻结
							//if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,pbm->dFrozeAmt,false))
							//{
							//	m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
							//	return false;
							//}
						}
						else
						{
							pbm->dFrozeAmt = 0.0;//不用冻结
						}
					}

				}
				else //冻结的在卖方
				{
					pbm->nFrozeFlag = 2;
					if (pbm->nMarginFlag == 1) //持有的在多，不同方向
					{
						//持有保证金
						dTmp = pbm->dBuyMargin*(pbm->nBuyPosi-pbm->nSellPosi)/pbm->nBuyPosi;
						dFrozeTmp = (pbm->dSellMargin+pbm->dSellFroze)*(0-nQty)/(pbm->nSellPosi+pbm->nSellQty);

						pbm->dFrozeAmt = dFrozeTmp-dTmp;
						if (pbm->dFrozeAmt > MAXDOUBLECOMPARE)
						{
							////进行新的冻结
							//if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,pbm->dFrozeAmt,false))
							//{
							//	m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
							//	return false;
							//}
						}
						else
						{
							pbm->dFrozeAmt = 0.0;//不用冻结
						}
					}
					else //持有的在空， 相同方向
					{
						//持有保证金
						dTmp = pbm->dSellMargin*(pbm->nSellPosi-pbm->nBuyPosi)/pbm->nSellPosi;
						dFrozeTmp = (pbm->dSellMargin+pbm->dSellFroze)*(0-nQty)/(pbm->nSellPosi+pbm->nSellQty);

						pbm->dFrozeAmt = dFrozeTmp-dTmp;//相减大于0是要去冻结的
						if (pbm->dFrozeAmt > MAXDOUBLECOMPARE)
						{
							////进行新的冻结
							//if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,pbm->dFrozeAmt,false))
							//{
							//	m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
							//	return false;
							//}
						}
						else
						{
							pbm->dFrozeAmt = 0.0;//不用冻结
						}
					}
				}

			}
			//判断资金是否足够
			if (ischeckfund)
			{
				dFrozeTmp = pbm->dFrozeAmt-dLastFroze;
				if (dFrozeTmp >MAXDOUBLECOMPARE) //需要冻结
				{
					if (fundinfo->dAvailFund < dFrozeTmp-po->dExFee+MAXDOUBLECOMPARE )
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"可用资金不足   客户[%s %s]可用[%.4lf]，要冻结[%.4lf]",fundinfo->sTxAcc,fundinfo->sCurrency,fundinfo->dAvailFund,dFrozeTmp-po->dExFee);
						po->sOrderStatus[0] = ORDER_STATUS_INVALID;
						strcpy(po->sOrderRsp,"可用资金不足");
						return RET_ERR_AVAILFUND;
					}
				}
				else
				{
					if (fundinfo->dAvailFund < 0.0-po->dExFee+MAXDOUBLECOMPARE)
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"可用资金不足   客户[%s %s]可用[%.4lf]，要冻结[%.4lf]",fundinfo->sTxAcc,fundinfo->sCurrency,fundinfo->dAvailFund,0.00-po->dExFee);
						po->sOrderStatus[0] = ORDER_STATUS_INVALID;
						strcpy(po->sOrderRsp,"可用资金不足");
						return RET_ERR_AVAILFUND;
					}
				}
			}
			//先解冻
			if (nLastFlag == 1)
			{
				//将原来的释放
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,0.00-dLastFroze,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
					po->sOrderStatus[0] = ORDER_STATUS_INVALID;
					strcpy(po->sOrderRsp,"更新资金出错");
					return RET_ERR_SYSTEM;
				}
			}
			else
			{
				//将原来的释放
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,0.00-dLastFroze,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
					po->sOrderStatus[0] = ORDER_STATUS_INVALID;
					strcpy(po->sOrderRsp,"更新资金出错");
					return RET_ERR_SYSTEM;
				}
			}
			//按最新的重新冻结
			if (pbm->nFrozeFlag == 1)
			{
				//进行新的冻结
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,pbm->dFrozeAmt,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
					po->sOrderStatus[0] = ORDER_STATUS_INVALID;
					strcpy(po->sOrderRsp,"更新资金出错");
					return RET_ERR_SYSTEM;
				}
			}
			else
			{
				//进行新的冻结
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,pbm->dFrozeAmt,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
					po->sOrderStatus[0] = ORDER_STATUS_INVALID;
					strcpy(po->sOrderRsp,"更新资金出错");
					return RET_ERR_SYSTEM;
				}
			}
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"资金账号[%s]品种[%d] 净边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 买冻量[%d] 卖冻量[%d] 冻结[%lf]",\
				pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,\
				pbm->dBuyFroze,pbm->dSellFroze,pbm->nBuyQty,pbm->nSellQty,pbm->dFrozeAmt);
			//冻手续费
			if (bisinit) //初始化，按剩余量算
			{
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-po->dExFee*po->nRemainNum/po->nOrderNum,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
					po->sOrderStatus[0] = ORDER_STATUS_INVALID;
					strcpy(po->sOrderRsp,"更新资金出错");
					return RET_ERR_SYSTEM;
				}
			}
			else
			{
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-po->dExFee,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
					po->sOrderStatus[0] = ORDER_STATUS_INVALID;
					strcpy(po->sOrderRsp,"更新资金出错");
					return RET_ERR_SYSTEM;
				}
			}
		}
		else  //卖开
		{
			//查找表
			pbm = m_pMemDb->m_pDbTrader.m_pBigNetMargin.Select(po->sTxAcc,pc->nProdId);
			if (pbm == NULL)//没有新增
			{
				strcpy(bm.sTxAcc,po->sTxAcc);
				bm.nMarginFlag = 1;
				bm.sBailMode= BAIL_MODE_NET;
				bm.nVarietyId = pc->nProdId;
				if (bisinit) //表示从数据库读的，用剩余量来算
				{
					bm.dFrozeAmt = po->dFrozAmt*po->nRemainNum/po->nOrderNum;
					bm.dSellFroze = bm.dFrozeAmt;
					bm.nSellQty = po->nRemainNum;
				}
				else
				{
					bm.dFrozeAmt = po->dFrozAmt;
					bm.dSellFroze = po->dFrozAmt;
					bm.nSellQty = po->nOrderNum;
				}
				bm.nFrozeFlag = 2;
				rid = m_pMemDb->m_pDbTrader.m_pBigNetMargin.Insert(bm);
				if (rid<0)
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"写品种大边净边表出错 资金账号[%s]品种[%d]",po->sTxAcc,pc->nProdId);
					po->sOrderStatus[0] = ORDER_STATUS_INVALID;
					strcpy(po->sOrderRsp,"写品种大边净边表出错");
					return RET_ERR_SYSTEM;
				}
				pbm =  m_pMemDb->m_pDbTrader.m_pBigNetMargin.At(rid);
				m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"资金账号[%s]品种[%d] 净边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 买冻量[%d] 卖冻量[%d] 冻结[%lf]",\
					pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,\
					pbm->dBuyFroze,pbm->dSellFroze,pbm->nBuyQty,pbm->nSellQty,pbm->dFrozeAmt);
				if (ischeckfund)
				{
					if (fundinfo->dAvailFund < pbm->dFrozeAmt-po->dExFee)
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"可用资金不足   客户[%s %s]可用[%.4lf]，要冻结[%.4lf]",fundinfo->sTxAcc,fundinfo->sCurrency,fundinfo->dAvailFund,pbm->dFrozeAmt-po->dExFee);
						po->sOrderStatus[0] = ORDER_STATUS_INVALID;
						strcpy(po->sOrderRsp,"可用资金不足");
						return RET_ERR_AVAILFUND;
					}
				}
				//冻卖保证金
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,pbm->dFrozeAmt,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
					po->sOrderStatus[0] = ORDER_STATUS_INVALID;
					strcpy(po->sOrderRsp,"更新资金出错");
					return RET_ERR_SYSTEM;
				}
			}
			else //计算该冻结多少
			{
				m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"资金账号[%s]品种[%d] 净边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 买冻量[%d] 卖冻量[%d] 冻结[%lf]",\
					pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,\
					pbm->dBuyFroze,pbm->dSellFroze,pbm->nBuyQty,pbm->nSellQty,pbm->dFrozeAmt);
				if (bisinit) //表示从数据库读的，用剩余量来算
				{
					pbm->dSellFroze = pbm->dSellFroze+po->dFrozAmt*po->nRemainNum/po->nOrderNum;
					pbm->nSellQty = pbm->nSellQty+po->nRemainNum;
				}
				else
				{
					pbm->dSellFroze = pbm->dSellFroze+po->dFrozAmt;
					pbm->nSellQty = pbm->nSellQty+po->nOrderNum;
				}

				nLastFlag = pbm->nFrozeFlag;
				dLastFroze = pbm->dFrozeAmt;

				
				m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"资金账号[%s]品种[%d] 净边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 冻结[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
				nQty = pbm->nBuyQty+pbm->nBuyPosi - pbm->nSellQty -pbm->nSellPosi;
				if (nQty == 0)  //买卖相等，啥都不用做
				{
					pbm->nFrozeFlag = 1; //冻结的在多
					pbm->dFrozeAmt=0.0;
				}
				else if (nQty > 0) //大于0 冻结的在买方
				{
					pbm->nFrozeFlag = 1; //冻结的在多
					if (pbm->nMarginFlag == 1) //持有的在多，同方向
					{
						//持有保证金
						dTmp = pbm->dBuyMargin*(pbm->nBuyPosi-pbm->nSellPosi)/pbm->nBuyPosi;
						dFrozeTmp = (pbm->dBuyMargin+pbm->dBuyFroze)*(nQty)/(pbm->nBuyPosi+pbm->nBuyQty);

						pbm->dFrozeAmt = dFrozeTmp-dTmp;
						if (pbm->dFrozeAmt > MAXDOUBLECOMPARE)
						{
							//进行新的冻结
							//if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,pbm->dFrozeAmt,false))
							//{
							//	m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
							//	return false;
							//}
						}
						else
						{
							pbm->dFrozeAmt = 0.0;//不用冻结
						}
					}
					else //持有的在空， 不同方向
					{
						//持有保证金
						dTmp = pbm->dSellMargin*(pbm->nSellPosi-pbm->nBuyPosi)/pbm->nSellPosi;
						dFrozeTmp = (pbm->dBuyMargin+pbm->dBuyFroze)*(nQty)/(pbm->nBuyPosi+pbm->nBuyQty);

						pbm->dFrozeAmt = dFrozeTmp-dTmp;//相减大于0是要去冻结的
						if (pbm->dFrozeAmt > MAXDOUBLECOMPARE)
						{
							//进行新的冻结
							//if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,pbm->dFrozeAmt,false))
							//{
							//	m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
							//	return false;
							//}
						}
						else
						{
							pbm->dFrozeAmt = 0.0;//不用冻结
						}
					}

				}
				else //冻结的在卖方
				{
					pbm->nFrozeFlag = 2;
					if (pbm->nMarginFlag == 1) //持有的在多，不同方向
					{
						//持有保证金
						dTmp = pbm->dBuyMargin*(pbm->nBuyPosi-pbm->nSellPosi)/pbm->nBuyPosi;
						dFrozeTmp = (pbm->dSellMargin+pbm->dSellFroze)*(0-nQty)/(pbm->nSellPosi+pbm->nSellQty);

						pbm->dFrozeAmt = dFrozeTmp-dTmp;
						if (pbm->dFrozeAmt > MAXDOUBLECOMPARE)
						{
							//进行新的冻结
							//if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,pbm->dFrozeAmt,false))
							//{
							//	m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
							//	return false;
							//}
						}
						else
						{
							pbm->dFrozeAmt = 0.0;//不用冻结
						}
					}
					else //持有的在空， 相同方向
					{
						//持有保证金
						dTmp = pbm->dSellMargin*(pbm->nSellPosi-pbm->nBuyPosi)/pbm->nSellPosi;
						dFrozeTmp = (pbm->dSellMargin+pbm->dSellFroze)*(0-nQty)/(pbm->nSellPosi+pbm->nSellQty);

						pbm->dFrozeAmt = dFrozeTmp-dTmp;//相减大于0是要去冻结的
						if (pbm->dFrozeAmt > MAXDOUBLECOMPARE)
						{
							//进行新的冻结
							//if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,pbm->dFrozeAmt,false))
							//{
							//	m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
							//	return false;
							//}
						}
						else
						{
							pbm->dFrozeAmt = 0.0;//不用冻结
						}
					}
				}
			}
			//判断资金是否足够
			if (ischeckfund)
			{
				dFrozeTmp = pbm->dFrozeAmt-dLastFroze;
				if (dFrozeTmp >MAXDOUBLECOMPARE) //需要冻结
				{
					if (fundinfo->dAvailFund < dFrozeTmp-po->dExFee+MAXDOUBLECOMPARE )
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"可用资金不足   客户[%s %s]可用[%.4lf]，要冻结[%.4lf]",fundinfo->sTxAcc,fundinfo->sCurrency,fundinfo->dAvailFund,dFrozeTmp-po->dExFee);
						po->sOrderStatus[0] = ORDER_STATUS_INVALID;
						strcpy(po->sOrderRsp,"可用资金不足");
						return RET_ERR_AVAILFUND;
					}
				}
				else
				{
					if (fundinfo->dAvailFund < 0.0-po->dExFee+MAXDOUBLECOMPARE)
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"可用资金不足   客户[%s %s]可用[%.4lf]，要冻结[%.4lf]",fundinfo->sTxAcc,fundinfo->sCurrency,fundinfo->dAvailFund,0.00-po->dExFee);
						po->sOrderStatus[0] = ORDER_STATUS_INVALID;
						strcpy(po->sOrderRsp,"可用资金不足");
						return RET_ERR_AVAILFUND;
					}
				}
			}
			if (nLastFlag == 1)
			{
				//将原来的释放
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,0.00-dLastFroze,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
					po->sOrderStatus[0] = ORDER_STATUS_INVALID;
					strcpy(po->sOrderRsp,"更新资金出错");
					return RET_ERR_SYSTEM;
				}
			}
			else
			{
				//将原来的释放
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,0.00-dLastFroze,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
					po->sOrderStatus[0] = ORDER_STATUS_INVALID;
					strcpy(po->sOrderRsp,"更新资金出错");
					return RET_ERR_SYSTEM;
				}
			}
			//按最新的重新冻结
			if (pbm->nFrozeFlag == 1)
			{
				//进行新的冻结
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,pbm->dFrozeAmt,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
					po->sOrderStatus[0] = ORDER_STATUS_INVALID;
					strcpy(po->sOrderRsp,"更新资金出错");
					return RET_ERR_SYSTEM;
				}
			}
			else
			{
				//进行新的冻结
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,pbm->dFrozeAmt,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
					po->sOrderStatus[0] = ORDER_STATUS_INVALID;
					strcpy(po->sOrderRsp,"更新资金出错");
					return RET_ERR_SYSTEM;
				}
			}
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"资金账号[%s]品种[%d] 净边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 买冻量[%d] 卖冻量[%d] 冻结[%lf]",\
				pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,\
				pbm->dBuyFroze,pbm->dSellFroze,pbm->nBuyQty,pbm->nSellQty,pbm->dFrozeAmt);
			//冻手续费
			if (bisinit) //初始化，按剩余量算
			{
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-po->dExFee*po->nRemainNum/po->nOrderNum,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
					po->sOrderStatus[0] = ORDER_STATUS_INVALID;
					strcpy(po->sOrderRsp,"更新资金出错");
					return RET_ERR_SYSTEM;
				}
			}
			else
			{
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-po->dExFee,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
					po->sOrderStatus[0] = ORDER_STATUS_INVALID;
					strcpy(po->sOrderRsp,"更新资金出错");
					return RET_ERR_SYSTEM;
				}
			}
		}
	}
	else //平
	{
		//冻持仓
		pPosi = m_pMemDb->m_pDbTrader.m_pPosition.Select(po->sTxAcc,po->sSeatNo,po->sShFlag,po->nContractNo);
		if (pPosi == NULL)
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"资金账号[%s] 平仓时 席位[%s] 投保标志[%s] 合约[%d %s]无持仓",po->sTxAcc,po->sSeatNo,po->sShFlag,po->nContractNo,po->sContractCode);
			po->sOrderStatus[0] = ORDER_STATUS_INVALID;
			strcpy(po->sOrderRsp,"可用持仓不足");
			return RET_ERR_POSITION_NOTFOUND;
		}
		if (bisinit) //初始化的处理，按剩余量
		{
			if (po->sBsFlag[0] == BS_FLAG_BUY) //买平 平空仓
			{
				//可用减少
				if (pPosi->nCurCanUsePS < po->nRemainNum)
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"资金账号[%s] 平仓时 席位[%s] 投保标志[%s] 合约[%d %s] 可用持仓不足",po->sTxAcc,po->sSeatNo,po->sShFlag,po->nContractNo,po->sContractCode);
					po->sOrderStatus[0] = ORDER_STATUS_INVALID;
					strcpy(po->sOrderRsp,"可用持仓不足");
					return RET_ERR_POSITION_NOTFOUND;
				}
				pPosi->nCurCanUsePS = pPosi->nCurCanUsePS - po->nRemainNum;
				//冻结增加
				pPosi->nDAY_COV_PS_Froze = pPosi->nDAY_COV_PS_Froze + po->nRemainNum;
			}
			else //卖平 平多仓
			{
				//可用减少
				if (pPosi->nCurCanUsePB < po->nRemainNum)
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"资金账号[%s] 平仓时 席位[%s] 投保标志[%s] 合约[%d %s] 可用持仓不足",po->sTxAcc,po->sSeatNo,po->sShFlag,po->nContractNo,po->sContractCode);
					po->sOrderStatus[0] = ORDER_STATUS_INVALID;
					strcpy(po->sOrderRsp,"可用持仓不足");
					return RET_ERR_POSITION_NOTFOUND;
				}
				pPosi->nCurCanUsePB = pPosi->nCurCanUsePB - po->nRemainNum;
				//冻结增加
				pPosi->nDAY_COV_PB_Froze = pPosi->nDAY_COV_PB_Froze - po->nRemainNum;
			}
			//冻费用
			if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-po->dExFee*po->nRemainNum/po->nOrderNum,false))
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
				po->sOrderStatus[0] = ORDER_STATUS_INVALID;
				strcpy(po->sOrderRsp,"更新资金出错");
				return RET_ERR_SYSTEM;
			}
		}
		else //正常的平仓
		{
			if (po->sBsFlag[0] == BS_FLAG_BUY) //买平 平空仓
			{
				//可用减少
				if (pPosi->nCurCanUsePS < po->nOrderNum)
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"资金账号[%s] 平仓时 席位[%s] 投保标志[%s] 合约[%d %s] 可用持仓不足",\
						po->sTxAcc,po->sSeatNo,po->sShFlag,po->nContractNo,po->sContractCode);
					po->sOrderStatus[0] = ORDER_STATUS_INVALID;
					strcpy(po->sOrderRsp,"可用持仓不足");
					return RET_ERR_POSITION_NOTFOUND;
				}
				pPosi->nCurCanUsePS = pPosi->nCurCanUsePS - po->nOrderNum;
				//冻结增加
				pPosi->nDAY_COV_PS_Froze = pPosi->nDAY_COV_PS_Froze + po->nOrderNum;
			}
			else //卖平 平多仓
			{
				//可用减少
				if (pPosi->nCurCanUsePB < po->nOrderNum)
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"资金账号[%s] 平仓时 席位[%s] 投保标志[%s] 合约[%d %s] 可用持仓不足",\
						po->sTxAcc,po->sSeatNo,po->sShFlag,po->nContractNo,po->sContractCode);
					po->sOrderStatus[0] = ORDER_STATUS_INVALID;
					strcpy(po->sOrderRsp,"可用持仓不足");
					return RET_ERR_POSITION_NOTFOUND;
				}
				pPosi->nCurCanUsePB = pPosi->nCurCanUsePB - po->nOrderNum;
				//冻结增加
				pPosi->nDAY_COV_PB_Froze = pPosi->nDAY_COV_PB_Froze - po->nOrderNum;
			}
			//冻费用
			if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-po->dExFee,false))
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
				po->sOrderStatus[0] = ORDER_STATUS_INVALID;
				strcpy(po->sOrderRsp,"更新资金出错");
				return RET_ERR_SYSTEM;
			}
		}
		
	}
	return SUCCESS;
}
int CKernelPub::ProcCancelOrderNetMargin(S_MDB_TXACC_LOCK *lockinfo,S_MDB_CUST_FUND *fundinfo,S_MDB_ORDER_INFO *po,S_MDB_CONTRACT_INFO *pc)
{
	int rid;
	S_MDB_BIG_MARGIN *pbm=NULL;
	S_MDB_BIG_MARGIN bm;
	S_MDB_CUST_POSITION *pPosi=NULL;
	int nQty=0;
	double dTmp=0.0;
	double dFrozeTmp=0.0;
	bzero(&bm,sizeof(S_MDB_BIG_MARGIN));

	
	pbm = m_pMemDb->m_pDbTrader.m_pBigNetMargin.Select(po->sTxAcc,pc->nProdId);
	if (pbm == NULL)
	{
		//成交但找不到记录
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"撤单通知 但净边记录找不到[%s %d]的记录",po->sTxAcc,pc->nProdId);
		return -1;
	}
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"撤单通知 净边处理开始 资金账号[%s]品种[%d] 净边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 冻结[%lf]",\
		pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
	//找到原来冻结的资金
	if (pbm->nFrozeFlag == 1)
	{
		//将原来的释放
		if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,0.00-pbm->dFrozeAmt,false))
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
			return -1;
		}
	}
	else
	{
		//将原来的释放
		if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,0.00-pbm->dFrozeAmt,false))
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
			return -1;
		}
	}
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"撤单通知 净边处理释放所有冻结 资金账号[%s]品种[%d] 净边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 冻结[%lf]",\
		pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
	//重新计算持有保证金及冻结资金
	if (po->sBsFlag[0] == BS_FLAG_BUY)
	{
		pbm->nBuyQty -= po->nCancelNum;
		pbm->dBuyFroze = pbm->dBuyFroze - po->dFrozAmt*po->nCancelNum/po->nOrderNum;
	}
	else
	{
		pbm->nSellQty -= po->nCancelNum;
		pbm->dSellFroze = pbm->dSellFroze - po->dFrozAmt*po->nCancelNum/po->nOrderNum;
	}
	//重新计算冻结
	nQty = pbm->nBuyQty+pbm->nBuyPosi - pbm->nSellQty -pbm->nSellPosi;
	if (nQty == 0)  //买卖相等，啥都不用做
	{
		pbm->nFrozeFlag = 1; //冻结的在多
		pbm->dFrozeAmt=0.0;
	}
	else if (nQty > 0) //大于0 冻结的在买方
	{
		pbm->nFrozeFlag = 1; //冻结的在多
		if (pbm->nMarginFlag == 1) //持有的在多，同方向
		{
			//持有保证金
			dTmp = pbm->dBuyMargin*(pbm->nBuyPosi-pbm->nSellPosi)/pbm->nBuyPosi;
			dFrozeTmp = (pbm->dBuyMargin+pbm->dBuyFroze)*(nQty)/(pbm->nBuyPosi+pbm->nBuyQty);
			pbm->dFrozeAmt = dFrozeTmp-dTmp;
			if (pbm->dFrozeAmt > MAXDOUBLECOMPARE)
			{
				//进行新的冻结
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,pbm->dFrozeAmt,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
					return -1;
				}
			}
			else
			{
				pbm->dFrozeAmt = 0.0;//不用冻结
			}
		}
		else //持有的在空， 不同方向
		{
			//持有保证金
			dTmp = pbm->dSellMargin*(pbm->nSellPosi-pbm->nBuyPosi)/pbm->nSellPosi;
			dFrozeTmp = (pbm->dBuyMargin+pbm->dBuyFroze)*(nQty)/(pbm->nBuyPosi+pbm->nBuyQty);

			pbm->dFrozeAmt = dFrozeTmp-dTmp;//相减大于0是要去冻结的
			if (pbm->dFrozeAmt > MAXDOUBLECOMPARE)
			{
				//进行新的冻结
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,pbm->dFrozeAmt,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
					return -1;
				}
			}
			else
			{
				pbm->dFrozeAmt = 0.0;//不用冻结
			}
		}

	}
	else //冻结的在卖方
	{
		pbm->nFrozeFlag = 2;
		if (pbm->nMarginFlag == 1) //持有的在多，不同方向
		{
			//持有保证金
			dTmp = pbm->dBuyMargin*(pbm->nBuyPosi-pbm->nSellPosi)/pbm->nBuyPosi;
			dFrozeTmp = (pbm->dSellMargin+pbm->dSellFroze)*(0-nQty)/(pbm->nSellPosi+pbm->nSellQty);
			pbm->dFrozeAmt = dFrozeTmp-dTmp;
			if (pbm->dFrozeAmt > MAXDOUBLECOMPARE)
			{
				//进行新的冻结
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,pbm->dFrozeAmt,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
					return -1;
				}
			}
			else
			{
				pbm->dFrozeAmt = 0.0;//不用冻结
			}
		}
		else //持有的在空， 相同方向
		{
			//持有保证金
			dTmp = pbm->dSellMargin*(pbm->nSellPosi-pbm->nBuyPosi)/pbm->nSellPosi;
			dFrozeTmp = (pbm->dSellMargin+pbm->dSellFroze)*(0-nQty)/(pbm->nSellPosi+pbm->nSellQty);

			pbm->dFrozeAmt = dFrozeTmp-dTmp;//相减大于0是要去冻结的
			if (pbm->dFrozeAmt > MAXDOUBLECOMPARE)
			{
				//进行新的冻结
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,pbm->dFrozeAmt,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund出错");
					return -1;
				}
			}
			else
			{
				pbm->dFrozeAmt = 0.0;//不用冻结
			}
		}
	}
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"撤单通知 净边处理重新冻结 资金账号[%s]品种[%d] 净边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 冻结[%lf]",\
		pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
	//解冻手续费
	if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,po->dExFee*po->nCancelNum/po->nOrderNum,false))
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund 出错");
		return -1;
	}
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"撤单通知 净边处理解冻手续费及撤单完成 资金账号[%s]品种[%d] 净边方向[%d] 买占用[%lf] 卖占用[%lf] 冻结方向[%d] 买冻结[%lf] 卖冻结[%lf] 冻结[%lf]",\
		pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
	return SUCCESS;
}