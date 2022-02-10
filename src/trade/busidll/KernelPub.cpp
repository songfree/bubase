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
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�ʽ���޴��ʽ��˺�[%s %s]",acclock->sTxAcc,flowlist[i]->sCurrency);
			return false;
		}
		//�жϳ������
		if (amt > MAXDOUBLECOMPARE)//���
		{
			fundflag = FUND_TYPE_INOUT;
			strcpy(sFundCode,FUND_CODE_IN);//���
			if (!UpdateFund(acclock,fund,fundflag,sFundCode,amt,true))
			{
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"�ʽ��˺�[%s] �����ʽ����",acclock->sTxAcc);
				return false;
			}
		}
		else if (amt<MINDOUBLECOMPARE) 
		{
			fundflag = FUND_TYPE_INOUT;
			strcpy(sFundCode,FUND_CODE_OUT);//����
			if (!UpdateFund(acclock,fund,fundflag,sFundCode,amt,true))
			{
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"�ʽ��˺�[%s] �����ʽ���� ",acclock->sTxAcc);
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
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�ʽ��˺�[%s] ����[%s] �ĺ�Լ���[%d]�ں�Լ������",orderlist[i]->sTxAcc,orderlist[i]->sLocalSerial,orderlist[i]->nContractNo);
			return false;
		}
		fund = m_pMemDb->GetFund(acclock,pc->sCurrency);
		if (fund == NULL)
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�ʽ���޴��ʽ��˺�[%s %s]",acclock->sTxAcc,pc->sCurrency);
			return false;
		}
		if (!ComputeOneOrder(acclock,fund,orderlist[i],pc))
		{
			//sprintf(errmsg,"���㱨��[%s %d %s ] ���� %s",donelist[i]->sTxAcc,orderlist[i]->nContractNo,orderlist[i]->sDoneNo,errmsg);
			return false;
		}
	}
	return true;
}

bool CKernelPub::ProcDone(S_MDB_TXACC_LOCK *acclock,std::vector<S_MDB_DONE_INFO *> &donelist)
{
	//Ҫһ�ʱʳɽ�����������ۣ�ӯ�������³ֱֲ��ʽ��
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
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�ʽ��˺�[%s] �ɽ�������[%s] �ɽ���[%s] �ĺ�Լ���[%d]�ں�Լ������",\
				donelist[i]->sTxAcc,donelist[i]->sLocalNo,donelist[i]->sDoneNo,donelist[i]->nContractNo);
			return false;
		}
		fund = m_pMemDb->GetFund(acclock,pc->sCurrency);
		if (fund == NULL)
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�ʽ���޴��ʽ��˺�[%s %s]",acclock->sTxAcc,pc->sCurrency);
			return false;
		}
		if (!ComputeOneDone(acclock,fund,donelist[i],pc))
		{
			//sprintf(errmsg,"����ɽ�[%s %d %s ] ���� %s",donelist[i]->sTxAcc,donelist[i]->nContractNo,donelist[i]->sDoneNo,errmsg);
			return false;
		}
	}
	return true;
}
bool CKernelPub::ProcPosition(S_MDB_TXACC_LOCK *acclock,std::vector<S_MDB_CUST_POSITION *> &posilist)
{
	//�����жϳֲ��Ƿ��Ǵ��
	int ntmp=0;
	int rid;
	S_MDB_BIG_MARGIN info;
	bzero(&info,sizeof(S_MDB_BIG_MARGIN));
	strcpy(info.sTxAcc,acclock->sTxAcc);

	
	//����ǹ�̨��������ģ�û���ʽ���ϸ������Ҳ���ø����ʽ���ϸ��������ʾ�ʽ���ϸʱ��Ϊ����
	S_MDB_CUST_FUND_DETAIL sfd;
	bzero(&sfd,sizeof(S_MDB_CUST_FUND_DETAIL));
	S_MDB_CUST_FUND_DETAIL *psfd = NULL;

	S_MDB_CUST_FUNDACC_INFO *accinfo = m_pMemDb->m_pDbCust.m_pCustFundAcc.At(acclock->nFundAccRowid);
	if (accinfo == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"�ʽ��˻�������ʽ��˻�[%s] rowid[%d]���ʽ��˻����Ҳ���",info.sTxAcc,acclock->nFundAccRowid);
		return false;
	}
	bool isWriteDetail=true;
	S_MDB_BROKERAGE_INFO *pbk = m_pMemDb->m_pDbSet.m_pBrokerage.Select(accinfo->nSysId);
	if (pbk == NULL)
	{
		isWriteDetail=false;//�Ҳ�����Ӧ�����ã��Ȳ�д
	}
	else
	{
		if (pbk->nClearFlag != 0)
		{
			//˵�����ǵ���ģ�����д��ϸ
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
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�ֲ� [%d %s] [%d %d] Ʒ��[%d]",pPosi->nContractNo,pPosi->sContractCode,pPosi->nCurPB,pPosi->nCurPS,pPosi->nVarietyId);
		//�ж��Ƿ����ڻ�
		S_MDB_CONTRACT_INFO *pc = m_pMemDb->m_pDbBaseinfo.m_pContract.Select(posilist[i]->nContractNo);
		if (pc == NULL)
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"��Լ[%d %s]�ں�Լ������",posilist[i]->nContractNo,posilist[i]->sContractCode);
			return false;
		}
		if (pc->sProd_type[0] != PROD_TYPE_FUTURES )
		{
			//�����ڻ�
			continue;
		}
		nMarginMode = GetMarginMode(pc);
		if (nMarginMode<0)
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"��Լ[%d %s] ȡ��֤��ģʽ���� ",pc->nContractId,pc->sContractCode);
			return false;
		}
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"��Լ[%d %s] ��֤��ģʽ[%d] ",pc->nContractId,pc->sContractCode,nMarginMode);
		if (nMarginMode != 1 && nMarginMode !=2) 
		{
			if (!isWriteDetail)
			{
				continue;
			}
			//д�������ߵ���ϸ
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
						m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"д�ʽ���ϸ��ʧ�� [%s %s %s %s]",sfd.sTxAcc,sfd.sCurrency,sfd.sFundType,sfd.sFundCode);
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
						m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"д�ʽ���ϸ��ʧ�� [%s %s %s %s]",sfd.sTxAcc,sfd.sCurrency,sfd.sFundType,sfd.sFundCode);
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
		//�Ǵ�߻򾻱�
		pbm = m_pMemDb->m_pDbTrader.m_pBigNetMargin.Select(posilist[i]->sTxAcc,posilist[i]->nVarietyId);
		if (pbm == NULL)
		{
			//������߾���
			bzero(&info,sizeof(S_MDB_BIG_MARGIN));
			strcpy(info.sTxAcc,acclock->sTxAcc);
			info.dBuyMargin = posilist[i]->dPB_Margin;
			info.dSellMargin = posilist[i]->dPS_Margin;
			info.nBuyPosi = posilist[i]->nCurPB;
			info.nSellPosi = posilist[i]->nCurPS;
			info.nVarietyId = posilist[i]->nVarietyId;
			info.sBailMode = 48+nMarginMode;
			info.nFrozeFlag = 1;
			if (nMarginMode == 1) //����
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
								m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"д�ʽ���ϸ��ʧ�� [%s %s %s %s]",sfd.sTxAcc,sfd.sCurrency,sfd.sFundType,sfd.sFundCode);
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
								m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"д�ʽ���ϸ��ʧ�� [%s %s %s %s]",sfd.sTxAcc,sfd.sCurrency,sfd.sFundType,sfd.sFundCode);
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
			else //���
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
								m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"д�ʽ���ϸ��ʧ�� [%s %s %s %s]",sfd.sTxAcc,sfd.sCurrency,sfd.sFundType,sfd.sFundCode);
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
								m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"д�ʽ���ϸ��ʧ�� [%s %s %s %s]",sfd.sTxAcc,sfd.sCurrency,sfd.sFundType,sfd.sFundCode);
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
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"д��߾��߼�¼��ʧ�� [%s %d]",info.sTxAcc,info.nVarietyId);
				return false;
			}
			pbm = m_pMemDb->m_pDbTrader.m_pBigNetMargin.At(rid);
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�ʽ��˺�[%s]Ʒ��[%d] ��߾��߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%lf]",\
				pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,\
				pbm->dSellFroze,pbm->dFrozeAmt);
		}
		else //���´�߾���
		{
			//Ҫ�����ʽ���ϸ
			if (isWriteDetail)
			{
				//�Ƚⶳ
				if (nMarginMode == 1) //����
				{

				}

			}
			pbm->dBuyMargin = posilist[i]->dPB_Margin;
			pbm->dSellMargin = posilist[i]->dPS_Margin;
			pbm->nBuyPosi = posilist[i]->nCurPB;
			pbm->nSellPosi = posilist[i]->nCurPS;
			if (nMarginMode == 1) //����
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
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�ʽ��˺�[%s]Ʒ��[%d] ��߾��߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%lf]",\
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
	//����ȥ�ⶳ��(���������) Ҫ�ж��Ƿ���
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"����ɽ�  �˻�[%s]��Լ���[%d %s]  ��ƽ[%s] ����[%s] Ͷ��[%s] ���ر�����[%s] �ⶳ��[%lf %lf] �ɽ���[%d]",\
		info->sTxAcc,info->nContractNo,info->sContractCode,info->sEoFlag,info->sBsFlag,info->sShFlag,info->sLocalNo,info->dUnfrozeAmt,info->dUnfrozeFee,info->nDoneNum);
	switch (pc->sProd_type[0] )
	{
		case PROD_TYPE_FUTURES:
			//ȡ��֤��ģʽ
			nBailMode = GetMarginMode(pc);
			if (nBailMode <0)
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"��֤��ģʽ����");
				return false;
			}
			if (nBailMode  == 2) //���
			{
				if (info->sEoFlag[0] == EO_FLAG_OPEN) //����
				{
					if (info->sBsFlag[0] == BS_FLAG_BUY) //����
					{
						//���ұ�
						pbm = m_pMemDb->m_pDbTrader.m_pBigNetMargin.Select(info->sTxAcc,pc->nProdId);
						if (pbm == NULL)//û������
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
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"дƷ�ִ�߾��߱���� �ʽ��˺�[%s]Ʒ��[%d]",info->sTxAcc,pc->nProdId);
								return false;
							}
							pbm  = m_pMemDb->m_pDbTrader.m_pBigNetMargin.At(rid);
							m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"����� �ʽ��˺�[%s]Ʒ��[%d] ��߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
							//�����ʽ�
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_BUY,info->dExMargin+info->dMemberMargin,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
								return false;
							}
						}
						else
						{
							m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"����� �ʽ��˺�[%s]Ʒ��[%d] ��߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
							pbm->dBuyMargin = pbm->dBuyMargin+info->dExMargin+info->dMemberMargin;
							pbm->nBuyPosi+=info->nDoneNum;//���������
							if (pbm->nMarginFlag == 1) //����ڶ� ������ͬ
							{
								
								//�����ʽ�
								if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_BUY,info->dExMargin+info->dMemberMargin,true))
								{
									m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
									return false;
								}
							}
							else //����ڿ� �����෴
							{
								if (pbm->dBuyMargin >pbm->dSellMargin+MAXDOUBLECOMPARE ) //����ڶ���
								{
									pbm->nMarginFlag = 1;
									//���˸����򣬴���ڶ� ��ȥ��ԭ�����ֵ
									if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_SELL,0.00-pbm->dSellMargin,true))
									{
										m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
										return false;
									}
									//�������򷽵�ֵ
									if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_BUY,pbm->dBuyMargin,true))
									{
										m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
										return false;
									}
								}
								else
								{
									//���򲻱䣬���ö�
								}
							}
							
						}
						//����������
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FEE,FUND_CODE_FEE_EXCNANGE,info->dExFee+info->dMemberFee,true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
							return false;
						}
						m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"����� �ʽ��˺�[%s]Ʒ��[%d] ��߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
					}
					else //����
					{
						//���ұ�
						pbm = m_pMemDb->m_pDbTrader.m_pBigNetMargin.Select(info->sTxAcc,pc->nProdId);
						if (pbm == NULL)//û������
						{
							//����ڿ�
							strcpy(bm.sTxAcc,info->sTxAcc);
							bm.nSellPosi = info->nDoneNum;
							bm.dSellMargin = info->dExMargin+info->dMemberMargin;
							bm.nMarginFlag = 2;
							bm.sBailMode= BAIL_MODE_LARGE;
							bm.nVarietyId = pc->nProdId;
							rid =  m_pMemDb->m_pDbTrader.m_pBigNetMargin.Insert(bm);
							if (rid<0)
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"дƷ�ִ�߾��߱���� �ʽ��˺�[%s]Ʒ��[%d]",info->sTxAcc,pc->nProdId);
								return false;
							}
							pbm  = m_pMemDb->m_pDbTrader.m_pBigNetMargin.At(rid);
							m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"���ղ� �ʽ��˺�[%s]Ʒ��[%d] ��߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
							//�����ʽ�
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_SELL,info->dExMargin+info->dMemberMargin,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
								return false;
							}
						}
						else
						{
							m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"���ղ� �ʽ��˺�[%s]Ʒ��[%d] ��߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
							pbm->dSellMargin = pbm->dSellMargin+info->dExMargin+info->dMemberMargin;
							pbm->nSellPosi+=info->nDoneNum;//����������
							if (pbm->nMarginFlag == 2) //����ڿ� ������ͬ
							{
								//�����ʽ�
								if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_SELL,info->dExMargin+info->dMemberMargin,true))
								{
									m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
									return false;
								}
							}
							else //����ڶ� �����෴
							{
								if (pbm->dBuyMargin >pbm->dSellMargin+MAXDOUBLECOMPARE ) //��߻��ڶ�
								{
									//���ö�
								}
								else
								{
									pbm->nMarginFlag = 2;
									//���˸����򣬴���ڿ� ��ȥ��ԭ�����ֵ
									if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_BUY,0.00-pbm->dBuyMargin,true))
									{
										m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
										return false;
									}
									//�������򷽵�ֵ
									if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_SELL,pbm->dSellMargin,true))
									{
										m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
										return false;
									}
								}
							}

						}
						m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"���ղ� �ʽ��˺�[%s]Ʒ��[%d] ��߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FEE,FUND_CODE_FEE_EXCNANGE,info->dExFee+info->dMemberFee,true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
							return false;
						}

					}
				}
				else //ƽ��  
				{
					//ȡ�ֲ���Ϣ
					pPosi = m_pMemDb->m_pDbTrader.m_pPosition.Select(info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo);
					if (pPosi == NULL)
					{
						m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"�޳ֲ� �ʽ��˺�[%s] ϯλ[%s] Ͷ����־[%s] ��Լ[%d %s] ����[%d] ƽ��  �����˳ɽ�",\
							info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo,info->sContractCode,info->nDoneNum);
						return true;
					}
					double dTmp=0.0;
					//����ֲ�
					if (info->sBsFlag[0] == BS_FLAG_BUY) //��ƽ  ƽ��
					{
						//���ұ�
						pbm = m_pMemDb->m_pDbTrader.m_pBigNetMargin.Select(info->sTxAcc,pc->nProdId);
						if (pbm == NULL)//û�п϶��ǳ�����
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"���ƽ��ʱ�Ҳ�����߾�����Ϣ �˻�[%s] Ʒ��[%d]",info->sTxAcc,pc->nProdId);
							return false;
						}
						m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"ƽ�ղ� �ʽ��˺�[%s]Ʒ��[%d] ��߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
						//�ͷ�ԭ����
						if (pbm->nMarginFlag == 1) //����ڶ�
						{
							//�ͷ��ʽ�
							dDiffMargin = pbm->dBuyMargin;
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_BUY,0.0-pbm->dBuyMargin,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
								return false;
							}
						}
						else //����ڿ� 
						{
							dDiffMargin = pbm->dSellMargin;
							//�ͷ��ʽ�
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_SELL,0.0-pbm->dSellMargin,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
								return false;
							}
						}
						//�ղּ��� �ղֱ�֤�����
						pbm->nSellPosi = pbm->nSellPosi-info->nDoneNum;
						pbm->dSellMargin = pbm->dSellMargin - pPosi->dPS_Margin*(1.00*info->nDoneNum/pPosi->nCurPS);
						if (pbm->dBuyMargin >pbm->dSellMargin+MAXDOUBLECOMPARE ) //����ڶ�
						{
							pbm->nMarginFlag = 1;
							//�����򷽵�ֵ
							dDiffMargin = pbm->dBuyMargin-dDiffMargin;
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_BUY,pbm->dBuyMargin,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
								return false;
							}
						}
						else
						{
							pbm->nMarginFlag = 2;
							dDiffMargin = pbm->dSellMargin-dDiffMargin;
							//�����򷽵�ֵ
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_SELL,pbm->dSellMargin,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
								return false;
							}
						}
						m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"ƽ�ղ� �ʽ��˺�[%s]Ʒ��[%d] ��߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%lf] ռ�ñ�֤����[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt,dDiffMargin);
						//���¼��㶳��	 20210422  ƽ���������᲻�仯������Ϊƽ����֤����٣�������ܻᷢ���仯
						//�ͷ�ԭ����
						if (pbm->nFrozeFlag == 1) //����ڶ�
						{
							//�ͷ��ʽ�
							if (!UpdateFund(lockinfo, fundinfo, FUND_TYPE_FROZE, FUND_CODE_BUYFROZEMARGIN, 0.0 - pbm->dFrozeAmt, true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "UpdateFund����");
								return false;
							}
						}
						else //����ڿ� 
						{
							//�ͷ��ʽ�
							if (!UpdateFund(lockinfo, fundinfo, FUND_TYPE_FROZE, FUND_CODE_SELLFROZEMARGIN, 0.0 - pbm->dFrozeAmt, true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "UpdateFund����");
								return false;
							}
						}
					    dTmp = pbm->dBuyMargin + pbm->dBuyFroze - pbm->dSellMargin - pbm->dSellFroze;
						if (dTmp > MINDOUBLECOMPARE) //���ڵ���0
						{
							pbm->nFrozeFlag = 1; //������ڶ�
							if (pbm->nMarginFlag == 1) //���е��ڶ�
							{
								//pbm->dFrozeAmt  = pbm->dBuyMargin+pbm->dBuyFroze-pbm->dBuyMargin;
								pbm->dFrozeAmt = pbm->dBuyFroze;//ͬ����һ������������
							}
							else //���е��ڿ�
							{
								//����(����+����) ��ȥռ�У���Ϊ����ȥ�����
								pbm->dFrozeAmt = pbm->dBuyMargin + pbm->dBuyFroze - pbm->dSellMargin;
							}
							//�����µĶ���
							if (!UpdateFund(lockinfo, fundinfo, FUND_TYPE_FROZE, FUND_CODE_BUYFROZEMARGIN, pbm->dFrozeAmt, true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "UpdateFund����");
								return false;
							}
						}
						else
						{
							pbm->nFrozeFlag = 2;
							if (pbm->nMarginFlag == 1) //���е��ڶ�
							{
								pbm->dFrozeAmt = pbm->dSellMargin + pbm->dSellFroze - pbm->dBuyMargin;
							}
							else //���е��ڿ�
							{
								//����(����+����) ��ȥռ�У���Ϊ����ȥ�����
								//pbm->dFrozeAmt  = pbm->dSellMargin+pbm->dSellFroze-pbm->dSellMargin;
								pbm->dFrozeAmt = pbm->dSellFroze;//ͬ����һ������������
							}
							//�����µĶ���
							if (!UpdateFund(lockinfo, fundinfo, FUND_TYPE_FROZE, FUND_CODE_SELLFROZEMARGIN, pbm->dFrozeAmt, true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "UpdateFund����");
								return false;
							}
						}
						m_pLog->LogMp(LOG_DEBUG, __FILE__, __LINE__, "ƽ�ղ� �ʽ��˺�[%s]Ʒ��[%d] ��߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%lf] ��֤����[%lf]", pbm->sTxAcc, pbm->nVarietyId, pbm->nMarginFlag, pbm->dBuyMargin, pbm->dSellMargin, pbm->nFrozeFlag, pbm->dBuyFroze, pbm->dSellFroze, pbm->dFrozeAmt, dTmp);
						//���¼��㶳��	 20210422 END
						//����
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FEE,FUND_CODE_FEE_EXCNANGE,info->dExFee+info->dMemberFee,true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
							return false;
						}
					}
					else //��ƽ ƽ��
					{
						//���ұ�
						pbm = m_pMemDb->m_pDbTrader.m_pBigNetMargin.Select(info->sTxAcc,pc->nProdId);
						if (pbm == NULL)//û�п϶��ǳ�����
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"���ƽ��ʱ�Ҳ�����߾�����Ϣ �˻�[%s] Ʒ��[%d]",info->sTxAcc,pc->nProdId);
							return false;
						}
						m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"ƽ��� �ʽ��˺�[%s]Ʒ��[%d] ��߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
						
						//���ͷ�ԭ����
						if (pbm->nMarginFlag == 1) //����ڶ� ������ͬ
						{
							dDiffMargin = pbm->dBuyMargin;
							//�ͷ��ʽ�
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_BUY,0.00-pbm->dBuyMargin,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
								return false;
							}
						}
						else //����ڿ� �����෴
						{
							dDiffMargin = pbm->dSellMargin;
							//�ͷ��ʽ�
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_SELL,0.00-pbm->dSellMargin,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
								return false;
							}
						}
						//���¼�����б�֤�� //��ּ��� ��ֱ�֤�����
						pbm->nBuyPosi = pbm->nBuyPosi-info->nDoneNum;
						pbm->dBuyMargin = pbm->dBuyMargin - pPosi->dPB_Margin*(1.00*info->nDoneNum/pPosi->nCurPB);
						if (pbm->dBuyMargin >pbm->dSellMargin+MAXDOUBLECOMPARE ) //����ڶ�
						{
							pbm->nMarginFlag = 1;
							dDiffMargin = pbm->dBuyMargin-dDiffMargin;
							//�����򷽵�ֵ
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_BUY,pbm->dBuyMargin,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
								return false;
							}
						}
						else
						{
							pbm->nMarginFlag = 2;
							dDiffMargin = pbm->dSellMargin-dDiffMargin;
							//�����򷽵�ֵ
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_SELL,pbm->dSellMargin,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
								return false;
							}
						}
						//���¼��㶳��	 20210422  ƽ���������᲻�仯������Ϊƽ����֤����٣�������ܻᷢ���仯
						//�ͷ�ԭ����
						if (pbm->nFrozeFlag == 1) //����ڶ�
						{
							//�ͷ��ʽ�
							if (!UpdateFund(lockinfo, fundinfo, FUND_TYPE_FROZE, FUND_CODE_BUYFROZEMARGIN, 0.0 - pbm->dFrozeAmt, true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "UpdateFund����");
								return false;
							}
						}
						else //����ڿ� 
						{
							//�ͷ��ʽ�
							if (!UpdateFund(lockinfo, fundinfo, FUND_TYPE_FROZE, FUND_CODE_SELLFROZEMARGIN, 0.0 - pbm->dFrozeAmt, true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "UpdateFund����");
								return false;
							}
						}
						dTmp = pbm->dBuyMargin + pbm->dBuyFroze - pbm->dSellMargin - pbm->dSellFroze;
						if (dTmp > MINDOUBLECOMPARE) //���ڵ���0
						{
							pbm->nFrozeFlag = 1; //������ڶ�
							if (pbm->nMarginFlag == 1) //���е��ڶ�
							{
								//pbm->dFrozeAmt  = pbm->dBuyMargin+pbm->dBuyFroze-pbm->dBuyMargin;
								pbm->dFrozeAmt = pbm->dBuyFroze;//ͬ����һ������������
							}
							else //���е��ڿ�
							{
								//����(����+����) ��ȥռ�У���Ϊ����ȥ�����
								pbm->dFrozeAmt = pbm->dBuyMargin + pbm->dBuyFroze - pbm->dSellMargin;
							}
							//�����µĶ���
							if (!UpdateFund(lockinfo, fundinfo, FUND_TYPE_FROZE, FUND_CODE_BUYFROZEMARGIN, pbm->dFrozeAmt, true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "UpdateFund����");
								return false;
							}
						}
						else
						{
							pbm->nFrozeFlag = 2;
							if (pbm->nMarginFlag == 1) //���е��ڶ�
							{
								pbm->dFrozeAmt = pbm->dSellMargin + pbm->dSellFroze - pbm->dBuyMargin;
							}
							else //���е��ڿ�
							{
								//����(����+����) ��ȥռ�У���Ϊ����ȥ�����
								//pbm->dFrozeAmt  = pbm->dSellMargin+pbm->dSellFroze-pbm->dSellMargin;
								pbm->dFrozeAmt = pbm->dSellFroze;//ͬ����һ������������
							}
							//�����µĶ���
							if (!UpdateFund(lockinfo, fundinfo, FUND_TYPE_FROZE, FUND_CODE_SELLFROZEMARGIN, pbm->dFrozeAmt, true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "UpdateFund����");
								return false;
							}
						}
						m_pLog->LogMp(LOG_DEBUG, __FILE__, __LINE__, "ƽ�ղ� �ʽ��˺�[%s]Ʒ��[%d] ��߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%lf] ��֤����[%lf]", pbm->sTxAcc, pbm->nVarietyId, pbm->nMarginFlag, pbm->dBuyMargin, pbm->dSellMargin, pbm->nFrozeFlag, pbm->dBuyFroze, pbm->dSellFroze, pbm->dFrozeAmt, dTmp);
						//���¼��㶳��	 20210422 END
						m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"ƽ��� �ʽ��˺�[%s]Ʒ��[%d] ��߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%lf] ռ�ñ�֤����[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt,dDiffMargin);
						//����
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FEE,FUND_CODE_FEE_EXCNANGE,info->dExFee+info->dMemberFee,true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
							return false;
						}
					}
				}
			}
			else if (nBailMode == 1) //����
			{
				if (ProcDoneNetMargin(lockinfo,fundinfo,info,NULL,pc,true))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����߳�ʼ�ɽ�����");
					return false;
				}
			}
			else  //˫��
			{
				if (info->sEoFlag[0] == EO_FLAG_OPEN) //����
				{
					if (info->sBsFlag[0] == BS_FLAG_BUY)
					{
						//�����ʽ�
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_BUY,info->dExMargin+info->dMemberMargin,true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
							return false;
						}
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FEE,FUND_CODE_FEE_EXCNANGE,info->dExFee+info->dMemberFee,true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
							return false;
						}

					}
					else
					{
						//�����ʽ�	
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_SELL,info->dExMargin+info->dMemberMargin,true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
							return false;
						}
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FEE,FUND_CODE_FEE_EXCNANGE,info->dExFee+info->dMemberFee,true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
							return false;
						}
					}
				}
				else //ƽ��
				{
					//ȡ�ֲ���Ϣ
					pPosi = m_pMemDb->m_pDbTrader.m_pPosition.Select(info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo);
					if (pPosi == NULL)
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�޳ֲ� �ʽ��˺�[%s] ϯλ[%s] Ͷ����־[%s] ��Լ[%d %s] ����[%d] ƽ��",\
							info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo,info->sContractCode,info->nDoneNum);
						return false;

					}
					//����ֲ�
					if (info->sBsFlag[0] == BS_FLAG_BUY) //��ƽ  ƽ��
					{
						//���۲��� ��֤������������С
						//���±�֤��
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_SELL,0.00-pPosi->dPS_Margin*(1.00*info->nDoneNum/pPosi->nCurPS),true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
							return false;
						}
						//����
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FEE,FUND_CODE_FEE_EXCNANGE,info->dExFee+info->dMemberFee,true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
							return false;
						}
					}
					else //��ƽ ƽ��
					{
						//���۲��� ��֤������������С
						//���±�֤��
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_BUY,0.00-pPosi->dPB_Margin*(1.00*info->nDoneNum/pPosi->nCurPB),true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
							return false;
						}
						//����
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FEE,FUND_CODE_FEE_EXCNANGE,info->dExFee+info->dMemberFee,true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
							return false;
						}
					}
				}
			}
			if (!UpdatePosition(lockinfo,fundinfo,info,pPosi,pc,true))
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdatePosition ���� ");
			}
			break;
		case PROD_TYPE_SPOT://�ֻ�
		case PROD_TYPE_SEC://��Ʊ
			//���³ֲ�
			pPosi = m_pMemDb->m_pDbTrader.m_pPosition.Select(info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo);
			if (info->sBsFlag[0] == BS_FLAG_BUY)
			{
				//�����ʽ�
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_INOUT,FUND_CODE_BUY,info->dAmt,true))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
					return false;
				}
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FEE,FUND_CODE_FEE_EXCNANGE,info->dExFee+info->dMemberFee,true))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
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
					posi.dPB_Avp = posi.dPB_Margin/info->nDoneNum;//�ɱ�����/ÿ��
					posi.nDayPB = info->nDoneNum;
					posi.nCurCanUsePB = info->nDoneNum;
					posi.nCurPB = info->nDoneNum;
					posi.dPB_OAvp = posi.dPB_Avp;//���ֳɱ����� 
					if (m_pMemDb->m_pDbTrader.m_pPosition.Insert(posi)<0)
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"д��ֱֲ�ʧ�� �ʽ��˺�[%s] ϯλ[%s] Ͷ����־[%s] ��Լ[%d %s] ����[%d] ����",\
							info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo,info->sContractCode,info->nDoneNum);
						return false;
					}
				}
				else
				{
					//���о���
					pPosi->dPB_Avp = (pPosi->dPB_Avp*pPosi->nCurPB-info->dAmt-info->dExFee-info->dMemberFee)/(pPosi->nCurPB+info->nDoneNum);
					pPosi->dPB_Margin = pPosi->dPB_Margin - info->dAmt-info->dExFee-info->dMemberFee;
					pPosi->nDayPB = pPosi->nDayPB+info->nDoneNum;
					pPosi->nCurCanUsePB = pPosi->nCurCanUsePB+info->nDoneNum;
					pPosi->nCurPB = pPosi->nCurPB+info->nDoneNum;
					//���־���
					pPosi->dPB_OAvp = (pPosi->dPB_OAvp*pPosi->nDayPB+info->dAmt+info->dExFee+info->dMemberFee)/(pPosi->nDayPB+info->nDoneNum);

				}
			}
			else  //����
			{
				//�����ʽ�	
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_INOUT,FUND_CODE_SELL,info->dAmt,true))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
					return false;
				}
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FEE,FUND_CODE_FEE_EXCNANGE,info->dExFee+info->dMemberFee,true))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
					return false;
				}
				if (pPosi == NULL)
				{

					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"д��ֱֲ�ʧ�� �ʽ��˺�[%s] ϯλ[%s] Ͷ����־[%s] ��Լ[%d %s] ����[%d] ����",\
						info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo,info->sContractCode,info->nDoneNum);
					return false;
				}
				//����ֲ־���
				pPosi->dPB_Margin = pPosi->dPB_Margin*(pPosi->nCurPB-info->nDoneNum)/pPosi->nCurPB;
				pPosi->nCurCanUsePB = pPosi->nCurCanUsePB+info->nDoneNum;
				pPosi->nCurPB = pPosi->nCurPB+info->nDoneNum;
				//���־��۲���
				//��ӯ�����µ�������
				pPosi->dPB_Avp = (pPosi->dPB_OAvp*pPosi->nCurPB+info->dCovSurplus)/pPosi->nCurPB;
				//��������ӯ��
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_SURPLUS,FUND_CODE_SURPLUS_CLOSE,(info->dDonePrice-pPosi->dPB_Avp)*info->nDoneNum*pc->dTxUnit,true))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
					return false;
				}
			}
			break;
		case PROD_TYPE_OPTION:
			if (info->sEoFlag[0] == EO_FLAG_OPEN) //����
			{
				if (info->sBsFlag[0] == BS_FLAG_BUY)
				{
					//�����ʽ�
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_BUY,info->dExMargin+info->dMemberMargin,true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
						return false;
					}
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FEE,FUND_CODE_FEE_EXCNANGE,info->dExFee+info->dMemberFee,true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
						return false;
					}

				}
				else
				{
					//�����ʽ�	
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_SELL,info->dExMargin+info->dMemberMargin,true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
						return false;
					}
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FEE,FUND_CODE_FEE_EXCNANGE,info->dExFee+info->dMemberFee,true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
						return false;
					}
				}
			}
			else //ƽ��
			{
				//ȡ�ֲ���Ϣ
				pPosi = m_pMemDb->m_pDbTrader.m_pPosition.Select(info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo);
				if (pPosi == NULL)
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�޳ֲ� �ʽ��˺�[%s] ϯλ[%s] Ͷ����־[%s] ��Լ[%d %s] ����[%d] ƽ��",\
						info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo,info->sContractCode,info->nDoneNum);
					return false;

				}
				//����ֲ�
				if (info->sBsFlag[0] == BS_FLAG_BUY) //��ƽ  ƽ��
				{
					//���۲��� ��֤������������С
					//���±�֤��
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_SELL,pPosi->dPS_Margin*(1.00*info->nDoneNum/pPosi->nCurPS),true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
						return false;
					}
					//����
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FEE,FUND_CODE_FEE_EXCNANGE,info->dExFee+info->dMemberFee,true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
						return false;
					}
				}
				else //��ƽ ƽ��
				{
					//���۲��� ��֤������������С
					//���±�֤��
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_BUY,pPosi->dPB_Margin*(1.00*info->nDoneNum/pPosi->nCurPB),true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
						return false;
					}
					//����
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FEE,FUND_CODE_FEE_EXCNANGE,info->dExFee+info->dMemberFee,true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
						return false;
					}
				}
			}
			if (!UpdatePosition(lockinfo,fundinfo,info,pPosi,pc,true))
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdatePosition ���� ");
			}
			break;
		default:
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"��Լ����[%s] ���� ",pc->sProd_type);
			return false;
	}
	
	return true;
}
bool CKernelPub::UpdateFund(S_MDB_TXACC_LOCK *info,S_MDB_CUST_FUND *fund,const char fundflag,const  char *fundcode,double amt,bool isadd,bool islock)
{
	// #define  FUND_TYPE_INOUT            '1'   //��֧
	// #define  FUND_TYPE_FROZE            '2'   //����
	// #define  FUND_TYPE_MARGIN           '3'   //��֤��
	// #define  FUND_TYPE_FEE              '4'   //����
	// #define  FUND_TYPE_RESERVE          '5'   //������
	// #define  FUND_TYPE_SURPLUS          '6'   //ӯ��
	// #define  FUND_TYPE_FLOATSURPLUS     '7'   //����ӯ��
	// #define  FUND_TYPE_ARAP             '8'   //Ӧ��Ӧ��
	// #define  FUND_TYPE_INCR_INTE        '9'   //��Ϣ
	
	if (amt > MINDOUBLECOMPARE && amt <MAXDOUBLECOMPARE) //���Ϊ0
	{
		return true;
	}
	if (islock)
	{
		CBF_PMutex pp(info->mutex);
	}
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�ʽ��˻�[%s] UpdateFundǰ�ʽ� ���[%lf] ��֤��[%lf] ����[%lf] ����[%lf] ����ӯ��[%lf] �ʽ�����[%c] �ʽ����[%s] ���[%lf]",info->sTxAcc,fund->dBalance,fund->dMargin,fund->dFrozeFund,fund->dFee,fund->dFloatSurplus,fundflag,fundcode,amt);
	S_MDB_CUST_FUND_DETAIL sfd;
	sfd.sFundType[0] = fundflag;
	sfd.sFundType[1] = 0;
	double dTmp;
	S_MDB_CUST_FUND_DETAIL *psfd= m_pMemDb->m_pDbTrader.m_pFundDetail.Select((char *)info->sTxAcc,fund->sCurrency,sfd.sFundType,(char *)fundcode);
	switch (fundflag)
	{
		case FUND_TYPE_INOUT://��֧����   ��Ϊ���Ӹ�Ϊ��С
			if (isadd)
			{
				fund->dIncomeExpense= fund->dIncomeExpense+amt;
				//���ÿ��ᵱ�ս��
				fund->dBalance= fund->dBalance+amt;
				fund->dAvailFund= fund->dAvailFund+amt;
				fund->dGetFund= fund->dGetFund+amt;
			}
			else //����ֵ�滻
			{
				dTmp = amt - fund->dIncomeExpense;
				fund->dBalance= fund->dBalance+dTmp;
				fund->dAvailFund= fund->dAvailFund+dTmp;
				fund->dGetFund= fund->dGetFund+dTmp;
				fund->dIncomeExpense = amt;
			}
			break;
		case FUND_TYPE_FROZE: //����  ��Ӱ����   �ʲ���Ϊ����
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
		case FUND_TYPE_MARGIN: //��֤�� ��Ӱ����  �ʲ���Ϊ����
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
		case FUND_TYPE_FEE: //����   ��Ϊ���Ӹ�Ϊ��С
			if (isadd)
			{
				fund->dFee = fund->dFee+amt;
				//���ÿ��ᵱ�ս��
				fund->dBalance= fund->dBalance+amt;
				fund->dAvailFund= fund->dAvailFund+amt;
				fund->dGetFund= fund->dGetFund+amt;
			}
			else
			{
				dTmp = amt - fund->dFee;

				//���ÿ��ᵱ�ս��
				fund->dBalance= fund->dBalance+dTmp;
				fund->dAvailFund= fund->dAvailFund+dTmp;
				fund->dGetFund= fund->dGetFund+dTmp;
				fund->dFee = amt;
			}
			break;
		case FUND_TYPE_RESERVE: //������ ��Ӱ����  �ʲ���Ϊ����
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
		case FUND_TYPE_SURPLUS: //ӯ��   ��Ϊ���Ӹ�Ϊ��С
			if (isadd)
			{
				fund->dSurplus = fund->dSurplus+amt;
				//���ÿ��ᵱ�ս��
				fund->dBalance= fund->dBalance+amt;
				fund->dAvailFund= fund->dAvailFund+amt;
				fund->dGetFund= fund->dGetFund+amt;
			}
			else
			{
				dTmp = amt - fund->dSurplus;

				//���ÿ��ᵱ�ս��
				fund->dBalance= fund->dBalance+dTmp;
				fund->dAvailFund= fund->dAvailFund+dTmp;
				fund->dGetFund= fund->dGetFund+dTmp;
				fund->dSurplus = amt;
			}
			break;
		case FUND_TYPE_FLOATSURPLUS: //����ӯ��  ��Ϊ���Ӹ�Ϊ��С
			if (isadd)
			{
				if (fund->dFloatSurplus < MINDOUBLECOMPARE) //ԭ���ĸ�ӯΪ��
				{
					//�ָ�ԭ������ȥ���Ǽ���
					fund->dAvailFund= fund->dAvailFund -fund->dFloatSurplus;
					fund->dGetFund= fund->dGetFund-fund->dFloatSurplus;
				}
				//�µĸ���ӯ��
				fund->dFloatSurplus = fund->dFloatSurplus+amt;
				//���ս��Ҫ���ϸ���ӯ��
				fund->dBalance= fund->dBalance+amt;
				//���ÿ���
				if (fund->dFloatSurplus <MINDOUBLECOMPARE)
				{
					//����ӯ��Ϊ������Ҫ��ȥ
					fund->dAvailFund= fund->dAvailFund+fund->dFloatSurplus;
					fund->dGetFund= fund->dGetFund+fund->dFloatSurplus;
				}
				 //��ӯΪ�������ӵ����ÿ�������

			}
			else
			{
				if (fund->dFloatSurplus < MINDOUBLECOMPARE) //ԭ���ĸ�ӯΪ��
				{
					//�ָ�ԭ������ȥ���Ǽ���
					fund->dAvailFund= fund->dAvailFund -fund->dFloatSurplus;
					fund->dGetFund= fund->dGetFund-fund->dFloatSurplus;
				}
				//���ս��Ҫ���ϸ���ӯ��
				fund->dBalance = fund->dBalance -fund->dFloatSurplus;
				fund->dBalance= fund->dBalance+amt;
				if (amt <MINDOUBLECOMPARE) //��ӯΪ�������ӵ����ÿ���������
				{
					//���ÿ���
					fund->dAvailFund= fund->dAvailFund+amt;
					fund->dGetFund= fund->dGetFund+amt;
				}
				fund->dFloatSurplus = amt;
			}
			break;
		case FUND_TYPE_ARAP: //Ӧ��Ӧ��   ��Ϊ���Ӹ�Ϊ��С
			if (isadd)
			{
				fund->dArap = fund->dArap+amt;
				//���ÿ��ᵱ�ս��
				fund->dBalance= fund->dBalance+amt;
				fund->dAvailFund= fund->dAvailFund+amt;
				fund->dGetFund= fund->dGetFund+amt;
			}
			else
			{
				dTmp = amt - fund->dArap;

				//���ÿ��ᵱ�ս��
				fund->dBalance= fund->dBalance+dTmp;
				fund->dAvailFund= fund->dAvailFund+dTmp;
				fund->dGetFund= fund->dGetFund+dTmp;
				fund->dArap = amt;
			}
			break;
		case FUND_TYPE_INCR_INTE: //��Ϣ  ��Ϊ���Ӹ�Ϊ��С
			if (isadd)
			{
				fund->dIncrInte = fund->dIncrInte+amt;
				//���ÿ��ᵱ�ս��
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
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�ʽ�����[%c]����",fundflag);
			return false;
	}
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�ʽ��˻�[%s] UpdateFund���ʽ� ���[%lf] ��֤��[%lf] ����[%lf] ����[%lf] ����ӯ��[%lf] �ʽ�����[%c] �ʽ����[%s]",info->sTxAcc,fund->dBalance,fund->dMargin,fund->dFrozeFund,fund->dFee,fund->dFloatSurplus,fundflag,fundcode);
	//������ϸ
	if (psfd == NULL) //����
	{
		strcpy(sfd.sTxAcc,info->sTxAcc);
		strcpy(sfd.sFundCode,fundcode);
		sfd.dFundValue = amt;
		strcpy(sfd.sCurrency,fund->sCurrency);
		if (m_pMemDb->m_pDbTrader.m_pFundDetail.Insert(sfd)<0)
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ʽ���ϸ��ʧ��[%s %s %s %s]",sfd.sTxAcc,sfd.sCurrency,sfd.sFundType,sfd.sFundCode);
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
		if (pdate.GetWeek() >0 &&  pdate.GetWeek()<6) //������
		{
			pd = m_pMemDb->m_pDbBaseinfo.m_pSpecDate.Select((char *)market,(char *)excode,(char *)pdate.ToStringD8().c_str());
			if (pd == NULL) //���ǽڼ���
			{
				if (excode[0] != '$') //���г���ȡһ��
				{
					pd = m_pMemDb->m_pDbBaseinfo.m_pSpecDate.Select((char *)market,(char *)excode,(char *)pdate.ToStringD8().c_str());
					if (pd == NULL) //���ǽڼ���
					{
						i++;
						pdate--;
						continue;
					}
					//���г�ȡ���ǽڼ���
					pdate--;
					continue;
				}
				else  //���г�ȡ�ģ�û�о��ǽ�����
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
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"��Լ[%d %s]��Ʒ��[%d]��Ʒ�ֱ��в�����",pc->nContractId,pc->sContractCode,pc->nProdId);
		return -1;
	}
	if (pv->sBailMode[0] != BAIL_MODE_LARGE)
	{
		return atoi(pv->sBailMode);
	}
	//�Ǵ��ģʽ����鵽����
	S_MDB_EXCH_INFO *pe = m_pMemDb->m_pDbBaseinfo.m_pExchange.Select(pc->sExcode);
	if (pe == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"������[%s]�ڽ�����������",pc->sExcode);
		return -1;
	}
	//�н����ǵ����£��޴���Ż�
	if (pc->sExcode[0] == '4') //�н���
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

	//������6�������գ�����������
	int lastdate = GetLastTradeDate(pe->sMarket,pc->sExcode,pc->sDueDate,6);
	if (lastdate < atoi(pe->sTradeDate)) 
	{
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"��Լ[%d %s]������[%s]��ǰ������[%s] ����߽�����[%d] �����������ǰ5���������ڣ���˫�߱�֤�����",pc->nContractId,pc->sContractCode,pc->sDueDate,pe->sTradeDate,lastdate);
		return 0;
	}
	return 2;
}

bool CKernelPub::ComputeOneOrder(S_MDB_TXACC_LOCK *lockinfo,S_MDB_CUST_FUND *fundinfo,S_MDB_ORDER_INFO *info,S_MDB_CONTRACT_INFO *pc)
{
	
	S_MDB_CUST_POSITION *pPosi = NULL;
	if (info->sCancelFlag[0] == BOOL_TRUE   || info->nCancelNum !=0 )
	{ 
		//�ѳ��� ���ô���
		return true;
	}
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"������  �˻�[%s]��Լ���[%d %s]  ��ƽ[%s] ����[%s] Ͷ��[%s] ���ر�����[%s] ����[%lf] ί����[%d] ʣ����[%d]",\
		info->sTxAcc,info->nContractNo,info->sContractCode,info->sEoFlag,info->sBsFlag,info->sShFlag,info->sLocalSerial,info->dFrozAmt,info->nOrderNum,info->nRemainNum);
	S_MDB_BIG_MARGIN *pbm=NULL;
	S_MDB_BIG_MARGIN bm;
	int nMarginMode=0;
	double dTmp=0.0;
	int rid;
	bzero(&bm,sizeof(S_MDB_BIG_MARGIN));
	switch (pc->sProd_type[0])
	{
		case PROD_TYPE_FUTURES://�ڻ�
			nMarginMode = GetMarginMode(pc);
			if (nMarginMode<0)
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"��Լ[%d %s] ȡ��֤��ģʽ���� ",pc->nContractId,pc->sContractCode);
				return false;
			}
			if (nMarginMode == 2) //��߱�֤��
			{
				if (info->sEoFlag[0] == EO_FLAG_OPEN) //����
				{
					//���ʽ�
					if (info->sBsFlag[0] == BS_FLAG_BUY) //��
					{
						//���ұ�
						pbm = m_pMemDb->m_pDbTrader.m_pBigNetMargin.Select(info->sTxAcc,pc->nProdId);
						if (pbm == NULL)//û������
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
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"дƷ�ִ�߾��߱���� �ʽ��˺�[%s]Ʒ��[%d]",info->sTxAcc,pc->nProdId);
								return false;
							}
							pbm = m_pMemDb->m_pDbTrader.m_pBigNetMargin.At(rid);
							m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�ʽ��˺�[%s]Ʒ��[%d] ��߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
							//��ʣ����������֤��
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,pbm->dFrozeAmt,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
								return false;
							}
						}
						else //����ö������
						{
							m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�ʽ��˺�[%s]Ʒ��[%d] ��߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
							pbm->dBuyFroze = pbm->dBuyFroze+info->dFrozAmt*info->nRemainNum/(1.00*info->nOrderNum);
							pbm->nBuyQty = pbm->nBuyQty+info->nRemainNum;
							dTmp = pbm->dBuyMargin+pbm->dBuyFroze-pbm->dSellMargin-pbm->dSellFroze;
							if (pbm->nFrozeFlag == 1)
							{
								//��ԭ�����ͷ�
								if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,0.00-pbm->dFrozeAmt,true))
								{
									m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
									return false;
								}
							}
							else
							{
								//��ԭ�����ͷ�
								if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,0.00-pbm->dFrozeAmt,true))
								{
									m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
									return false;
								}
							}
							if (dTmp > MINDOUBLECOMPARE) //���ڵ���0
							{
								pbm->nFrozeFlag = 1; //������ڶ�
								if (pbm->nMarginFlag == 1) //���е��ڶ�
								{
									//pbm->dFrozeAmt  = pbm->dBuyMargin+pbm->dBuyFroze-pbm->dBuyMargin;
									pbm->dFrozeAmt =pbm->dBuyFroze;//ͬ����һ������������
								}
								else //���е��ڿ�
								{
									//����(����+����) ��ȥռ�У���Ϊ����ȥ�����
									pbm->dFrozeAmt  = pbm->dBuyMargin+pbm->dBuyFroze-pbm->dSellMargin;
								}
								//�����µĶ���
								if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,pbm->dFrozeAmt,true))
								{
									m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
									return false;
								}
							}
							else
							{
								pbm->nFrozeFlag = 2;
								if (pbm->nMarginFlag == 1) //���е��ڶ�
								{
									pbm->dFrozeAmt  = pbm->dSellMargin+pbm->dSellFroze-pbm->dBuyMargin;
								}
								else //���е��ڿ�
								{
									//����(����+����) ��ȥռ�У���Ϊ����ȥ�����
									//pbm->dFrozeAmt  = pbm->dSellMargin+pbm->dSellFroze-pbm->dSellMargin;
									pbm->dFrozeAmt = pbm->dSellFroze;//ͬ����һ������������
								}
								//�����µĶ���
								if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,pbm->dFrozeAmt,true))
								{
									m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
									return false;
								}
							}
							
						}
						m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�ʽ��˺�[%s]Ʒ��[%d] ��߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
						//��ʣ��������������
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dExFee*info->nRemainNum/(1.00*info->nOrderNum),true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
							return false;
						}
					}
					else  //����
					{
						//���ұ�
						pbm = m_pMemDb->m_pDbTrader.m_pBigNetMargin.Select(info->sTxAcc,pc->nProdId);
						if (pbm == NULL)//û������
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
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"дƷ�ִ�߾��߱���� �ʽ��˺�[%s]Ʒ��[%d]",info->sTxAcc,pc->nProdId);
								return false;
							}
							pbm = m_pMemDb->m_pDbTrader.m_pBigNetMargin.At(rid);
							m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�ʽ��˺�[%s]Ʒ��[%d] ��߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
							//��ʣ����������֤��
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,pbm->dFrozeAmt,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
								return false;
							}
						}
						else //����ö������
						{
							m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�ʽ��˺�[%s]Ʒ��[%d] ��߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
							pbm->dSellFroze = pbm->dSellFroze+info->dFrozAmt*info->nRemainNum/(1.00*info->nOrderNum);
							pbm->nSellQty = pbm->nSellQty+info->nRemainNum;
							dTmp = pbm->dBuyMargin+pbm->dBuyFroze-pbm->dSellMargin-pbm->dSellFroze;

							if (pbm->nFrozeFlag == 1)
							{
								//��ԭ�����ͷ�
								if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,0.00-pbm->dFrozeAmt,true))
								{
									m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
									return false;
								}
							}
							else
							{
								//��ԭ�����ͷ�
								if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,0.00-pbm->dFrozeAmt,true))
								{
									m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
									return false;
								}
							}
							if (dTmp > MINDOUBLECOMPARE) //���ڵ���0
							{
								pbm->nFrozeFlag = 1; //������ڶ�
								if (pbm->nMarginFlag == 1) //���е��ڶ�
								{
									//pbm->dFrozeAmt  = pbm->dBuyMargin+pbm->dBuyFroze-pbm->dBuyMargin;
									pbm->dFrozeAmt =pbm->dBuyFroze;//ͬ����һ������������
								}
								else //���е��ڿ�
								{
									//����(����+����) ��ȥռ�У���Ϊ����ȥ�����
									pbm->dFrozeAmt  = pbm->dBuyMargin+pbm->dBuyFroze-pbm->dSellMargin;
								}
								//�����µĶ���
								if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,pbm->dFrozeAmt,true))
								{
									m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
									return false;
								}
							}
							else
							{
								pbm->nFrozeFlag = 2;
								if (pbm->nMarginFlag == 1) //���е��ڶ�
								{
									pbm->dFrozeAmt  = pbm->dSellMargin+pbm->dSellFroze-pbm->dBuyMargin;
								}
								else //���е��ڿ�
								{
									//����(����+����) ��ȥռ�У���Ϊ����ȥ�����
									//pbm->dFrozeAmt  = pbm->dSellMargin+pbm->dSellFroze-pbm->dSellMargin;
									pbm->dFrozeAmt = pbm->dSellFroze;//ͬ����һ������������
								}
								//�����µĶ���
								if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,pbm->dFrozeAmt,true))
								{
									m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
									return false;
								}
							}

						}
						m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�ʽ��˺�[%s]Ʒ��[%d] ��߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
						//��ʣ��������������
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dExFee*info->nRemainNum/(1.00*info->nOrderNum),true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
							return false;
						}
					}
				}
				else //ƽ
				{
					//���ֲ�
					pPosi = m_pMemDb->m_pDbTrader.m_pPosition.Select(info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo);
					if (pPosi == NULL)
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�ʽ��˺�[%s] ƽ��ʱ ϯλ[%s] Ͷ����־[%s] ��Լ[%d %s]�޳ֲ�",info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo,info->sContractCode);
						return false;
					}
					//ֻ��ʣ������
					if (info->sBsFlag[0] == BS_FLAG_BUY) //��ƽ ƽ�ղ�
					{
						//���ü���
						pPosi->nCurCanUsePS = pPosi->nCurCanUsePS - info->nRemainNum;
						//��������
						pPosi->nDAY_COV_PS_Froze = pPosi->nDAY_COV_PS_Froze + info->nRemainNum;
					}
					else //��ƽ ƽ���
					{
						//���ü���
						pPosi->nCurCanUsePB = pPosi->nCurCanUsePB - info->nRemainNum;
						//��������
						pPosi->nDAY_COV_PB_Froze = pPosi->nDAY_COV_PB_Froze - info->nRemainNum;
					}
					//������
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dExFee*info->nRemainNum/(1.00*info->nOrderNum),true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
						return false;
					}
				}
			}
			else if (nMarginMode == 1) //���߱�֤��
			{
				if (ProcOrderNetMargin(lockinfo,fundinfo,info,pc,true,false) != 0)
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����߳�ʼ��������");
					return false;
				}
			}
			else
			{
				if (info->sEoFlag[0] == EO_FLAG_OPEN) //����
				{
					//���ʽ�
					if (info->sBsFlag[0] == BS_FLAG_BUY)
					{
						//��ʣ����������֤��
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,info->dFrozAmt*info->nRemainNum/(1.00*info->nOrderNum),true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
							return false;
						}
						//��ʣ��������������
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dExFee*info->nRemainNum/(1.00*info->nOrderNum),true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
							return false;
						}
					}
					else
					{
						//��ʣ������������֤��
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,info->dFrozAmt*info->nRemainNum/(1.00*info->nOrderNum),true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
							return false;
						}
						//��ʣ��������������
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dExFee*info->nRemainNum/(1.00*info->nOrderNum),true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
							return false;
						}
					}
				}
				else //ƽ
				{
					//���ֲ�
					pPosi = m_pMemDb->m_pDbTrader.m_pPosition.Select(info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo);
					if (pPosi == NULL)
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�ʽ��˺�[%s] ƽ��ʱ ϯλ[%s] Ͷ����־[%s] ��Լ[%d %s]�޳ֲ�",info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo,info->sContractCode);
						return false;
					}
					//ֻ��ʣ������
					if (info->sBsFlag[0] == BS_FLAG_BUY) //��ƽ ƽ�ղ�
					{
						//���ü���
						pPosi->nCurCanUsePS = pPosi->nCurCanUsePS - info->nRemainNum;
						//��������
						pPosi->nDAY_COV_PS_Froze = pPosi->nDAY_COV_PS_Froze + info->nRemainNum;
					}
					else //��ƽ ƽ���
					{
						//���ü���
						pPosi->nCurCanUsePB = pPosi->nCurCanUsePB - info->nRemainNum;
						//��������
						pPosi->nDAY_COV_PB_Froze = pPosi->nDAY_COV_PB_Froze - info->nRemainNum;
					}
					//������
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dExFee*info->nRemainNum/(1.00*info->nOrderNum),true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
						return false;
					}
					
				}
			}
			
			break;
		case PROD_TYPE_SPOT:
		case PROD_TYPE_SEC:
			if (info->sBsFlag[0] == BS_FLAG_BUY) //�� 
			{
				//��ʣ���������򶳽�
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZE,info->dAmt*info->nRemainNum/(1.00*info->nOrderNum),true))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
					return false;
				}
				//��ʣ��������������
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dExFee*info->nRemainNum/(1.00*info->nOrderNum),true))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
					return false;
				}
			}
			else
			{
				//���ֲ�
				pPosi = m_pMemDb->m_pDbTrader.m_pPosition.Select(info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo);
				if (pPosi == NULL)
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�ʽ��˺�[%s] ƽ��ʱ ϯλ[%s] Ͷ����־[%s] ��Լ[%d %s]�޳ֲ�",info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo,info->sContractCode);
					return false;
				}
				//ֻ��ʣ������  ���ö�ּ��٣�����������
				pPosi->nCurCanUsePB = pPosi->nCurCanUsePB - info->nRemainNum;
				pPosi->nDAY_COV_PB_Froze = pPosi->nDAY_COV_PB_Froze + info->nRemainNum;

				//������
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dExFee*info->nRemainNum/(1.00*info->nOrderNum),true))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
					return false;
				}
			}
			break;
		case PROD_TYPE_OPTION:
			if (info->sEoFlag[0] == EO_FLAG_OPEN) //����
			{
				//���ʽ�
				if (info->sBsFlag[0] == BS_FLAG_BUY)
				{
					//�������ʽ�
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZE,info->dAmt*info->nRemainNum/(1.00*info->nOrderNum),true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
						return false;
					}
					//������������
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dExFee*info->nRemainNum/(1.00*info->nOrderNum),true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
						return false;
					}
				}
				else
				{
					//����������֤��
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,info->dFrozAmt*info->nRemainNum/(1.00*info->nOrderNum),true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
						return false;
					}
					//����������
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dExFee*info->nRemainNum/(1.00*info->nOrderNum),true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
						return false;
					}
				}
			}
			else //ƽ
			{
				//���ֲ�
				pPosi = m_pMemDb->m_pDbTrader.m_pPosition.Select(info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo);
				if (pPosi == NULL)
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�ʽ��˺�[%s] ƽ��ʱ ϯλ[%s] Ͷ����־[%s] ��Լ[%d %s]�޳ֲ�",info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo,info->sContractCode);
					return false;
				}
				//ֻ��ʣ������
				if (info->sBsFlag[0] == BS_FLAG_BUY) //��ƽ ƽ�ղ�
				{
					//��Ȩ����
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZE,info->dAmt*info->nRemainNum/(1.00*info->nOrderNum),true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
						return false;
					}
					//���ü��ٶ�������
					pPosi->nCurCanUsePS = pPosi->nCurCanUsePS - info->nRemainNum;
					pPosi->nDAY_COV_PS_Froze = pPosi->nDAY_COV_PS_Froze + info->nRemainNum;
				}
				else //��ƽ ƽ���
				{
					//���ü��ٶ�������
					pPosi->nCurCanUsePB = pPosi->nCurCanUsePB - info->nRemainNum;
					pPosi->nDAY_COV_PB_Froze = pPosi->nDAY_COV_PB_Froze + info->nRemainNum;
				}
				//������
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dExFee*info->nRemainNum/(1.00*info->nOrderNum),true))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
					return false;
				}

			}
			break;
		default:
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"��Լ����[%c] ���� ",pc->sProd_type[0]);
			return false;
	}
	return true;
}

int CKernelPub::ComputeNewOrder(S_MDB_TXACC_LOCK *lockinfo,S_MDB_CUST_FUND *fundinfo,S_MDB_ORDER_INFO *info,S_MDB_CONTRACT_INFO *pc,bool checkfund)
{
	//����
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

	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�����±���  �˻�[%s]��Լ���[%d %s]  ��ƽ[%s] ����[%s] Ͷ��[%s] ���ر�����[%s] ����[%lf] ί����[%d] ʣ����[%d]",\
		info->sTxAcc,info->nContractNo,info->sContractCode,info->sEoFlag,info->sBsFlag,info->sShFlag,info->sLocalSerial,info->dFrozAmt,info->nOrderNum,info->nRemainNum);
	switch (pc->sProd_type[0])
	{
		case PROD_TYPE_FUTURES://�ڻ�
			nMarginMode = GetMarginMode(pc);
			if (nMarginMode<0)
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"��Լ[%d %s] ȡ��֤��ģʽ���� ",pc->nContractId,pc->sContractCode);
				info->sOrderStatus[0] = ORDER_STATUS_INVALID;
				strcpy(info->sOrderRsp,"ȡ��֤��ģʽ����");
				return RET_ERR_SYSTEM;
			}
			if (nMarginMode == 2) //��߱�֤��
			{
				if (info->sEoFlag[0] == EO_FLAG_OPEN) //����
				{
					//���ʽ�
					if (info->sBsFlag[0] == BS_FLAG_BUY) //��
					{
						//���ұ�
						pbm = m_pMemDb->m_pDbTrader.m_pBigNetMargin.Select(info->sTxAcc,pc->nProdId);
						if (pbm == NULL)//û������
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
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"дƷ�ִ�߾��߱���� �ʽ��˺�[%s]Ʒ��[%d]",info->sTxAcc,pc->nProdId);
								info->sOrderStatus[0] = ORDER_STATUS_INVALID;
								strcpy(info->sOrderRsp,"дƷ�ִ�߾��߱����");
								return RET_ERR_SYSTEM;
							}
							pbm =  m_pMemDb->m_pDbTrader.m_pBigNetMargin.At(rid);
							m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�ʽ��˺�[%s]Ʒ��[%d] ��߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
							//����֤��
							if (checkfund)
							{
								if (fundinfo->dAvailFund < pbm->dFrozeAmt-info->dExFee)
								{
									m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ʽ���   �ͻ�[%s %s]����[%.4lf]��Ҫ����[%.4lf]",fundinfo->sTxAcc,fundinfo->sCurrency,fundinfo->dAvailFund,pbm->dFrozeAmt-info->dExFee);
									info->sOrderStatus[0] = ORDER_STATUS_INVALID;
									strcpy(info->sOrderRsp,"�����ʽ���");
									return RET_ERR_AVAILFUND;
								}
							}
							
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,bm.dFrozeAmt,false))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
								info->sOrderStatus[0] = ORDER_STATUS_INVALID;
								strcpy(info->sOrderRsp,"�����ʽ����");
								return RET_ERR_SYSTEM;
							}
						}
						else //����ö������
						{
							m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�ʽ��˺�[%s]Ʒ��[%d] ��߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
							pbm->dBuyFroze = pbm->dBuyFroze+info->dFrozAmt;
							pbm->nBuyQty = pbm->nBuyQty+info->nOrderNum;
							dTmp = pbm->dBuyMargin+pbm->dBuyFroze-pbm->dSellMargin-pbm->dSellFroze;

							nLastFlag = pbm->nFrozeFlag;
							dLastFroze = pbm->dFrozeAmt;

							
							if (dTmp > MINDOUBLECOMPARE) //���ڵ���0
							{
								pbm->nFrozeFlag = 1; //������ڶ�
								if (pbm->nMarginFlag == 1) //���е��ڶ�
								{
									//pbm->dFrozeAmt  = pbm->dBuyMargin+pbm->dBuyFroze-pbm->dBuyMargin;
									pbm->dFrozeAmt =pbm->dBuyFroze;//ͬ����һ������������
								}
								else //���е��ڿ�
								{
									//����(����+����) ��ȥռ�У���Ϊ����ȥ�����
									pbm->dFrozeAmt  = pbm->dBuyMargin+pbm->dBuyFroze-pbm->dSellMargin;
								}

							}
							else
							{
								pbm->nFrozeFlag = 2;
								if (pbm->nMarginFlag == 1) //���е��ڶ�
								{
									pbm->dFrozeAmt  = pbm->dSellMargin+pbm->dSellFroze-pbm->dBuyMargin;
								}
								else //���е��ڿ�
								{
									//����(����+����) ��ȥռ�У���Ϊ����ȥ�����
									//pbm->dFrozeAmt  = pbm->dSellMargin+pbm->dSellFroze-pbm->dSellMargin;
									pbm->dFrozeAmt = pbm->dSellFroze;//ͬ����һ������������
								}
								
								
							}
						}
						//�ж��ʽ��Ƿ��㹻
						if (checkfund)
						{
							dTmp = pbm->dFrozeAmt-dLastFroze;
							if (dTmp >MAXDOUBLECOMPARE) //��Ҫ����
							{
								if (fundinfo->dAvailFund < dTmp-info->dExFee+MAXDOUBLECOMPARE )
								{
									m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ʽ���   �ͻ�[%s %s]����[%.4lf]��Ҫ����[%.4lf]",fundinfo->sTxAcc,fundinfo->sCurrency,fundinfo->dAvailFund,dTmp-info->dExFee);
									info->sOrderStatus[0] = ORDER_STATUS_INVALID;
									strcpy(info->sOrderRsp,"�����ʽ���");
									return RET_ERR_AVAILFUND;
								}
							}
							else
							{
								if (fundinfo->dAvailFund < 0.0-info->dExFee+MAXDOUBLECOMPARE)
								{
									m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ʽ���   �ͻ�[%s %s]����[%.4lf]��Ҫ����[%.4lf]",fundinfo->sTxAcc,fundinfo->sCurrency,fundinfo->dAvailFund,0.00-info->dExFee);
									info->sOrderStatus[0] = ORDER_STATUS_INVALID;
									strcpy(info->sOrderRsp,"�����ʽ���");
									return RET_ERR_AVAILFUND;
								}
							}
						}
						//�Ƚⶳ
						if (nLastFlag == 1)
						{
							//��ԭ�����ͷ�
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,0.00-dLastFroze,false))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
								info->sOrderStatus[0] = ORDER_STATUS_INVALID;
								strcpy(info->sOrderRsp,"�����ʽ����");
								return RET_ERR_SYSTEM;
							}
						}
						else
						{
							//��ԭ�����ͷ�
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,0.00-dLastFroze,false))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
								info->sOrderStatus[0] = ORDER_STATUS_INVALID;
								strcpy(info->sOrderRsp,"�����ʽ����");
								return RET_ERR_SYSTEM;
							}
						}
						//���¶���
						if (pbm->nFrozeFlag == 1)
						{
							//�����µĶ���
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,pbm->dFrozeAmt,false))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
								info->sOrderStatus[0] = ORDER_STATUS_INVALID;
								strcpy(info->sOrderRsp,"�����ʽ����");
								return RET_ERR_SYSTEM;
							}
						}
						else
						{
							//�����µĶ���
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,pbm->dFrozeAmt,false))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
								info->sOrderStatus[0] = ORDER_STATUS_INVALID;
								strcpy(info->sOrderRsp,"�����ʽ����");
								return RET_ERR_SYSTEM;
							}
						}
						m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�ʽ��˺�[%s]Ʒ��[%d] ��߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
						//��������
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dExFee,false))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
							info->sOrderStatus[0] = ORDER_STATUS_INVALID;
							strcpy(info->sOrderRsp,"�����ʽ����");
							return RET_ERR_SYSTEM;
						}
					}
					else  //����
					{
						//���ұ�
						pbm = m_pMemDb->m_pDbTrader.m_pBigNetMargin.Select(info->sTxAcc,pc->nProdId);
						if (pbm == NULL)//û������
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
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"дƷ�ִ�߾��߱���� �ʽ��˺�[%s]Ʒ��[%d]",info->sTxAcc,pc->nProdId);
								info->sOrderStatus[0] = ORDER_STATUS_INVALID;
								strcpy(info->sOrderRsp,"дƷ�ִ�߾��߱����");
								return RET_ERR_SYSTEM;
							}
							pbm =  m_pMemDb->m_pDbTrader.m_pBigNetMargin.At(rid);
							m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�ʽ��˺�[%s]Ʒ��[%d] ��߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
							if (checkfund)
							{
								if (fundinfo->dAvailFund < pbm->dFrozeAmt-info->dExFee)
								{
									m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ʽ���   �ͻ�[%s %s]����[%.4lf]��Ҫ����[%.4lf]",fundinfo->sTxAcc,fundinfo->sCurrency,fundinfo->dAvailFund,pbm->dFrozeAmt-info->dExFee);
									info->sOrderStatus[0] = ORDER_STATUS_INVALID;
									strcpy(info->sOrderRsp,"�����ʽ���");
									return RET_ERR_AVAILFUND;
								}
							}
							//������֤��
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,pbm->dFrozeAmt,false))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
								info->sOrderStatus[0] = ORDER_STATUS_INVALID;
								strcpy(info->sOrderRsp,"�����ʽ����");
								return RET_ERR_SYSTEM;
							}
						}
						else //����ö������
						{
							m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�ʽ��˺�[%s]Ʒ��[%d] ��߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
							pbm->dSellFroze = pbm->dSellFroze+info->dFrozAmt;
							pbm->nSellQty = pbm->nSellQty+info->nOrderNum;
							dTmp = pbm->dBuyMargin+pbm->dBuyFroze-pbm->dSellMargin-pbm->dSellFroze;

							if (dTmp > MINDOUBLECOMPARE) //���ڵ���0
							{
								pbm->nFrozeFlag = 1; //������ڶ�
								if (pbm->nMarginFlag == 1) //���е��ڶ�
								{
									//pbm->dFrozeAmt  = pbm->dBuyMargin+pbm->dBuyFroze-pbm->dBuyMargin;
									pbm->dFrozeAmt = pbm->dBuyFroze;//ͬ����һ������������
								}
								else //���е��ڿ�
								{
									//����(����+����) ��ȥռ�У���Ϊ����ȥ�����
									pbm->dFrozeAmt  = pbm->dBuyMargin+pbm->dBuyFroze-pbm->dSellMargin;
								}
							}
							else
							{
								pbm->nFrozeFlag = 2;
								if (pbm->nMarginFlag == 1) //���е��ڶ�
								{
									pbm->dFrozeAmt  = pbm->dSellMargin+pbm->dSellFroze-pbm->dBuyMargin;
								}
								else //���е��ڿ�
								{
									//����(����+����) ��ȥռ�У���Ϊ����ȥ�����
									//pbm->dFrozeAmt  = pbm->dSellMargin+pbm->dSellFroze-pbm->dSellMargin;
									pbm->dFrozeAmt = pbm->dSellFroze;//ͬ����һ������������
								}
							}

						}
						//�ж��ʽ��Ƿ��㹻
						if (checkfund)
						{
							dTmp = pbm->dFrozeAmt-dLastFroze;
							if (dTmp >MAXDOUBLECOMPARE) //��Ҫ����
							{
								if (fundinfo->dAvailFund < dTmp-info->dExFee+MAXDOUBLECOMPARE )
								{
									m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ʽ���   �ͻ�[%s %s]����[%.4lf]��Ҫ����[%.4lf]",fundinfo->sTxAcc,fundinfo->sCurrency,fundinfo->dAvailFund,dTmp-info->dExFee);
									info->sOrderStatus[0] = ORDER_STATUS_INVALID;
									strcpy(info->sOrderRsp,"�����ʽ���");
									return RET_ERR_AVAILFUND;
								}
							}
							else
							{
								if (fundinfo->dAvailFund < 0.0-info->dExFee+MAXDOUBLECOMPARE)
								{
									m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ʽ���   �ͻ�[%s %s]����[%.4lf]��Ҫ����[%.4lf]",fundinfo->sTxAcc,fundinfo->sCurrency,fundinfo->dAvailFund,0.00-info->dExFee);
									info->sOrderStatus[0] = ORDER_STATUS_INVALID;
									strcpy(info->sOrderRsp,"�����ʽ���");
									return RET_ERR_AVAILFUND;
								}
							}
						}
						//�Ƚⶳ
						if (nLastFlag == 1)
						{
							//��ԭ�����ͷ�
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,0.00-dLastFroze,false))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
								info->sOrderStatus[0] = ORDER_STATUS_INVALID;
								strcpy(info->sOrderRsp,"�����ʽ����");
								return RET_ERR_SYSTEM;
							}
						}
						else
						{
							//��ԭ�����ͷ�
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,0.00-dLastFroze,false))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
								info->sOrderStatus[0] = ORDER_STATUS_INVALID;
								strcpy(info->sOrderRsp,"�����ʽ����");
								return RET_ERR_SYSTEM;
							}
						}
						//���¶���
						if (pbm->nFrozeFlag == 1)
						{
							//�����µĶ���
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,pbm->dFrozeAmt,false))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
								info->sOrderStatus[0] = ORDER_STATUS_INVALID;
								strcpy(info->sOrderRsp,"�����ʽ����");
								return RET_ERR_SYSTEM;
							}
						}
						else
						{
							//�����µĶ���
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,pbm->dFrozeAmt,false))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
								info->sOrderStatus[0] = ORDER_STATUS_INVALID;
								strcpy(info->sOrderRsp,"�����ʽ����");
								return RET_ERR_SYSTEM;
							}
						}
						m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�ʽ��˺�[%s]Ʒ��[%d] ��߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
						//��������
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dExFee,false))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
							info->sOrderStatus[0] = ORDER_STATUS_INVALID;
							strcpy(info->sOrderRsp,"�����ʽ����");
							return RET_ERR_SYSTEM;
						}
					}
				}
				else //ƽ
				{
					//���ֲ�
					pPosi = m_pMemDb->m_pDbTrader.m_pPosition.Select(info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo);
					if (pPosi == NULL)
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�ʽ��˺�[%s] ƽ��ʱ ϯλ[%s] Ͷ����־[%s] ��Լ[%d %s]�޳ֲ�",info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo,info->sContractCode);
						info->sOrderStatus[0] = ORDER_STATUS_INVALID;
						strcpy(info->sOrderRsp,"�ֲֲ���");
						return RET_ERR_POSITION_NOTFOUND;
					}
					//ֻ��ʣ������
					if (info->sBsFlag[0] == BS_FLAG_BUY) //��ƽ ƽ�ղ�
					{
						//���ü���
						if (pPosi->nCurCanUsePS < info->nOrderNum)
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�ʽ��˺�[%s] ƽ��ʱ ϯλ[%s] Ͷ����־[%s] ��Լ[%d %s] ���óֲֲ���",info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo,info->sContractCode);
							info->sOrderStatus[0] = ORDER_STATUS_INVALID;
							strcpy(info->sOrderRsp,"�ֲֲ���");
							return RET_ERR_POSITION_NOTFOUND;
						}
						pPosi->nCurCanUsePS = pPosi->nCurCanUsePS - info->nOrderNum;
						//��������
						pPosi->nDAY_COV_PS_Froze = pPosi->nDAY_COV_PS_Froze + info->nOrderNum;
					}
					else //��ƽ ƽ���
					{
						//���ü���
						if (pPosi->nCurCanUsePB < info->nOrderNum)
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�ʽ��˺�[%s] ƽ��ʱ ϯλ[%s] Ͷ����־[%s] ��Լ[%d %s] ���óֲֲ���",info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo,info->sContractCode);
							info->sOrderStatus[0] = ORDER_STATUS_INVALID;
							strcpy(info->sOrderRsp,"�ֲֲ���");
							return RET_ERR_POSITION_NOTFOUND;
						}
						pPosi->nCurCanUsePB = pPosi->nCurCanUsePB - info->nOrderNum;
						//��������
						pPosi->nDAY_COV_PB_Froze = pPosi->nDAY_COV_PB_Froze - info->nOrderNum;
					}
					//ƽ�ֲ������������Ƿ��㹻
					//if (checkfund)
					//{
					//	if (fundinfo->dAvailFund <  0.00-info->dExFee+MINDOUBLECOMPARE)
					//	{
					//		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ʽ���   �ͻ�[%s %s]����[%.4lf]��Ҫ����[%.4lf]",fundinfo->sTxAcc,fundinfo->sCurrency,fundinfo->dAvailFund,0.00-info->dExFee);
					//		return RET_ERR_AVAILFUND;
					//	}
					//}
					//������
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dExFee,false))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
						info->sOrderStatus[0] = ORDER_STATUS_INVALID;
						strcpy(info->sOrderRsp,"�����ʽ����");
						return RET_ERR_SYSTEM;
					}
				}
			}
			else if (nMarginMode == 1) //���߱�֤��
			{
				nRet = ProcOrderNetMargin(lockinfo,fundinfo,info,pc,false,checkfund);
				if (nRet != 0)
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����߱�������");
					return nRet;
				}
			}
			else //˫�߱�֤��
			{
				if (info->sEoFlag[0] == EO_FLAG_OPEN) //����
				{
					if (checkfund)
					{
						if (fundinfo->dAvailFund <  info->dFrozAmt-info->dExFee+MINDOUBLECOMPARE)
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ʽ���   �ͻ�[%s %s]����[%.4lf]��Ҫ����[%.4lf]",fundinfo->sTxAcc,fundinfo->sCurrency,fundinfo->dAvailFund, info->dFrozAmt-info->dExFee);
							info->sOrderStatus[0] = ORDER_STATUS_INVALID;
							strcpy(info->sOrderRsp,"�����ʽ���");
							return RET_ERR_AVAILFUND;
						}
					}
					//���ʽ�
					if (info->sBsFlag[0] == BS_FLAG_BUY)
					{
						//������֤��
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,info->dFrozAmt,false))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund ����");
							info->sOrderStatus[0] = ORDER_STATUS_INVALID;
							strcpy(info->sOrderRsp,"�����ʽ����");
							return RET_ERR_SYSTEM;
						}
						//����������
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dExFee,false))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund ����");
							info->sOrderStatus[0] = ORDER_STATUS_INVALID;
							strcpy(info->sOrderRsp,"�����ʽ����");
							return RET_ERR_SYSTEM;
						}
					}
					else
					{
						//��������֤��
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,info->dFrozAmt,false))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund ����");
							info->sOrderStatus[0] = ORDER_STATUS_INVALID;
							strcpy(info->sOrderRsp,"�����ʽ����");
							return RET_ERR_SYSTEM;
						}
						//����������
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dExFee,false))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund ����");
							info->sOrderStatus[0] = ORDER_STATUS_INVALID;
							strcpy(info->sOrderRsp,"�����ʽ����");
							return RET_ERR_SYSTEM;
						}
					}
				}
				else //ƽ
				{
					//���ֲ�
					pPosi = m_pMemDb->m_pDbTrader.m_pPosition.Select(info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo);
					if (pPosi == NULL)
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�ʽ��˺�[%s] ƽ��ʱ ϯλ[%s] Ͷ����־[%s] ��Լ[%d %s]�޳ֲ�",info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo,info->sContractCode);
						info->sOrderStatus[0] = ORDER_STATUS_INVALID;
						strcpy(info->sOrderRsp,"�ֲֲ���");
						return RET_ERR_POSITION_NOTFOUND;
					}
					//���ᱨ������
					if (info->sBsFlag[0] == BS_FLAG_BUY) //��ƽ ƽ�ղ�
					{
						//���ÿղּ���
						if (pPosi->nCurCanUsePS < info->nOrderNum)
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�ʽ��˺�[%s] ƽ��ʱ ϯλ[%s] Ͷ����־[%s] ��Լ[%d %s] ���óֲֲ���",info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo,info->sContractCode);
							info->sOrderStatus[0] = ORDER_STATUS_INVALID;
							strcpy(info->sOrderRsp,"�ֲֲ���");
							return RET_ERR_POSITION_NOTFOUND;
						}
						pPosi->nCurCanUsePS = pPosi->nCurCanUsePS - info->nOrderNum;
						//����ղ�����
						pPosi->nDAY_COV_PS_Froze = pPosi->nDAY_COV_PS_Froze + info->nOrderNum;
					}
					else //��ƽ ƽ���
					{
						//���ö�ּ���
						if (pPosi->nCurCanUsePB < info->nOrderNum)
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�ʽ��˺�[%s] ƽ��ʱ ϯλ[%s] Ͷ����־[%s] ��Լ[%d %s] ���óֲֲ���",info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo,info->sContractCode);
							info->sOrderStatus[0] = ORDER_STATUS_INVALID;
							strcpy(info->sOrderRsp,"�ֲֲ���");
							return RET_ERR_POSITION_NOTFOUND;
						}
						pPosi->nCurCanUsePB = pPosi->nCurCanUsePB - info->nOrderNum;
						//����������
						pPosi->nDAY_COV_PB_Froze = pPosi->nDAY_COV_PB_Froze + info->nOrderNum;
					}
					//ƽ�ֲ����ж��������Ƿ��㹻
					//������
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dExFee,false))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund ����");
						info->sOrderStatus[0] = ORDER_STATUS_INVALID;
						strcpy(info->sOrderRsp,"�����ʽ����");
						return RET_ERR_SYSTEM;
					}

				}
			}
			break;
		case PROD_TYPE_SPOT:
		case PROD_TYPE_SEC:
			if (info->sBsFlag[0] == BS_FLAG_BUY) //�� 
			{
				if (checkfund)
				{
					if (fundinfo->dAvailFund <  info->dAmt-info->dExFee+MINDOUBLECOMPARE)
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ʽ���   �ͻ�[%s %s]����[%.4lf]��Ҫ����[%.4lf]",fundinfo->sTxAcc,fundinfo->sCurrency,fundinfo->dAvailFund, info->dAmt-info->dExFee);
						info->sOrderStatus[0] = ORDER_STATUS_INVALID;
						strcpy(info->sOrderRsp,"�����ʽ���");
						return RET_ERR_AVAILFUND;
					}
				}
				//�򶳽�
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZE,info->dAmt,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund ����");
					info->sOrderStatus[0] = ORDER_STATUS_INVALID;
					strcpy(info->sOrderRsp,"�����ʽ����");
					return RET_ERR_SYSTEM;
				}
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dExFee,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund ����");
					info->sOrderStatus[0] = ORDER_STATUS_INVALID;
					strcpy(info->sOrderRsp,"�����ʽ����");
					return RET_ERR_SYSTEM;
				}
			}
			else
			{
				//���ֲֿ��
				pPosi = m_pMemDb->m_pDbTrader.m_pPosition.Select(info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo);
				if (pPosi == NULL)
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�ʽ��˺�[%s] ƽ��ʱ ϯλ[%s] Ͷ����־[%s] ��Լ[%d %s]�޳ֲ�",info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo,info->sContractCode);
					info->sOrderStatus[0] = ORDER_STATUS_INVALID;
					strcpy(info->sOrderRsp,"���в���");
					return RET_ERR_POSITION_NOTFOUND;
				}
				if (pPosi->nCurCanUsePB < info->nOrderNum)
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�ʽ��˺�[%s] ƽ��ʱ ϯλ[%s] Ͷ����־[%s] ��Լ[%d %s] ���óֲֲ���",info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo,info->sContractCode);
					info->sOrderStatus[0] = ORDER_STATUS_INVALID;
					strcpy(info->sOrderRsp,"���в���");
					return RET_ERR_POSITION_NOTFOUND;
				}
				//���ö�ּ���
				pPosi->nCurCanUsePB = pPosi->nCurCanUsePB - info->nOrderNum;
				//����������
				pPosi->nDAY_COV_PB_Froze = pPosi->nDAY_COV_PB_Froze + info->nOrderNum;
				//������
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dExFee,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund ����");
					info->sOrderStatus[0] = ORDER_STATUS_INVALID;
					strcpy(info->sOrderRsp,"�����ʽ����");
					return RET_ERR_SYSTEM;
				}
			}
			break;
		case PROD_TYPE_OPTION:
			if (info->sEoFlag[0] == EO_FLAG_OPEN) //����
			{
				//���ʽ�
				if (info->sBsFlag[0] == BS_FLAG_BUY) //��
				{
					if (checkfund)
					{
						if (fundinfo->dAvailFund <  info->dAmt-info->dExFee+MINDOUBLECOMPARE)
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ʽ���   �ͻ�[%s %s]����[%.4lf]��Ҫ����[%.4lf]",fundinfo->sTxAcc,fundinfo->sCurrency,fundinfo->dAvailFund, info->dAmt-info->dExFee);
							info->sOrderStatus[0] = ORDER_STATUS_INVALID;
							strcpy(info->sOrderRsp,"�����ʽ���");
							return RET_ERR_AVAILFUND;
						}
					}
					//��Ȩ����
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZE,info->dAmt,false))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund ����");
						info->sOrderStatus[0] = ORDER_STATUS_INVALID;
						strcpy(info->sOrderRsp,"�����ʽ����");
						return RET_ERR_SYSTEM;
					}
					//��������
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dExFee,false))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund ����");
						info->sOrderStatus[0] = ORDER_STATUS_INVALID;
						strcpy(info->sOrderRsp,"�����ʽ����");
						return RET_ERR_SYSTEM;
					}
				}
				else  //����
				{
					if (checkfund)
					{
						if (fundinfo->dAvailFund <  info->dFrozAmt-info->dExFee+MINDOUBLECOMPARE)
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ʽ���   �ͻ�[%s %s]����[%.4lf]��Ҫ����[%.4lf]",fundinfo->sTxAcc,fundinfo->sCurrency,fundinfo->dAvailFund, info->dFrozAmt -info->dExFee);
							info->sOrderStatus[0] = ORDER_STATUS_INVALID;
							strcpy(info->sOrderRsp,"�����ʽ���");
							return RET_ERR_AVAILFUND;
						}
					}
					//����������֤��
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,info->dFrozAmt,false))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund ����");
						info->sOrderStatus[0] = ORDER_STATUS_INVALID;
						strcpy(info->sOrderRsp,"�����ʽ����");
						return RET_ERR_SYSTEM;
					}
					//����������
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dExFee,false))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund ����");
						info->sOrderStatus[0] = ORDER_STATUS_INVALID;
						strcpy(info->sOrderRsp,"�����ʽ����");
						return RET_ERR_SYSTEM;
					}
				}
			}
			else //ƽ
			{
				//���ֲ�
				pPosi = m_pMemDb->m_pDbTrader.m_pPosition.Select(info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo);
				if (pPosi == NULL)
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�ʽ��˺�[%s] ƽ��ʱ ϯλ[%s] Ͷ����־[%s] ��Լ[%d %s]�޳ֲ�",info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo,info->sContractCode);
					info->sOrderStatus[0] = ORDER_STATUS_INVALID;
					strcpy(info->sOrderRsp,"�ֲֲ���");
					return RET_ERR_POSITION_NOTFOUND;
				}
				//ֻ��ʣ������
				if (info->sBsFlag[0] == BS_FLAG_BUY) //��ƽ ƽ�ղ�
				{
					if (pPosi->nCurCanUsePS < info->nOrderNum)
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�ʽ��˺�[%s] ƽ��ʱ ϯλ[%s] Ͷ����־[%s] ��Լ[%d %s] ���óֲֲ���",info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo,info->sContractCode);
						info->sOrderStatus[0] = ORDER_STATUS_INVALID;
						strcpy(info->sOrderRsp,"�ֲֲ���");
						return RET_ERR_POSITION_NOTFOUND;
					}
					if (checkfund)
					{
						if (fundinfo->dAvailFund <  info->dAmt-info->dExFee+MINDOUBLECOMPARE)
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ʽ���   �ͻ�[%s %s]����[%.4lf]��Ҫ����[%.4lf]",fundinfo->sTxAcc,fundinfo->sCurrency,fundinfo->dAvailFund, info->dAmt -info->dExFee);
							info->sOrderStatus[0] = ORDER_STATUS_INVALID;
							strcpy(info->sOrderRsp,"�����ʽ���");
							return RET_ERR_AVAILFUND;
						}
					}
					//��Ȩ����
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZE,info->dAmt,false))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund ����");
						info->sOrderStatus[0] = ORDER_STATUS_INVALID;
						strcpy(info->sOrderRsp,"�����ʽ����");
						return RET_ERR_SYSTEM;
					}
					
					//�ղֿ��ü���
					pPosi->nCurCanUsePS = pPosi->nCurCanUsePS - info->nOrderNum;
					//��������
					pPosi->nDAY_COV_PS_Froze = pPosi->nDAY_COV_PS_Froze + info->nOrderNum;
				}
				else //��ƽ ƽ���
				{
					//��ֿ��ü���
					if (pPosi->nCurCanUsePB < info->nOrderNum)
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�ʽ��˺�[%s] ƽ��ʱ ϯλ[%s] Ͷ����־[%s] ��Լ[%d %s] ���óֲֲ���",info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo,info->sContractCode);
						info->sOrderStatus[0] = ORDER_STATUS_INVALID;
						strcpy(info->sOrderRsp,"�ֲֲ���");
						return RET_ERR_POSITION_NOTFOUND;
					}
					pPosi->nCurCanUsePB = pPosi->nCurCanUsePB - info->nOrderNum;
					//��������
					pPosi->nDAY_COV_PB_Froze = pPosi->nDAY_COV_PB_Froze + info->nOrderNum;
				}
				//������
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dExFee,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund ����");
					info->sOrderStatus[0] = ORDER_STATUS_INVALID;
					strcpy(info->sOrderRsp,"�����ʽ����");
					return RET_ERR_SYSTEM;
				}

			}
			break;
		default:
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"��Լ����[%c] ���� ",pc->sProd_type[0]);
			info->sOrderStatus[0] = ORDER_STATUS_INVALID;
			strcpy(info->sOrderRsp,"��Լ���Ͳ���");
			return RET_ERR_SYSTEM;
	}
	return SUCCESS;
}

bool CKernelPub::ComputeNewDone(S_MDB_TXACC_LOCK *lockinfo,S_MDB_CUST_FUND *fundinfo,S_MDB_DONE_INFO *info,S_MDB_ORDER_INFO *po,S_MDB_CONTRACT_INFO *pc)
{
	CBF_PMutex pp(lockinfo->mutex);
	//���³ɽ�����
	po->dDoneAvp = (po->dDoneAvp*(po->nOrderNum-po->nRemainNum) + info->dDonePrice*info->nDoneNum)/(po->nOrderNum-po->nRemainNum+info->nDoneNum);
	//���±���ʣ������
	if (po->nRemainNum == 0 || po->sOrderStatus[0] == ORDER_STATUS_CANCEL || po->sOrderStatus[0] == ORDER_STATUS_PARTCANCEL)
	{
		//������Ϊ����
	}
	else
	{
		po->nRemainNum = po->nRemainNum - info->nDoneNum;
		//���±���״̬
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
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"��Լ[%d %s] ȡ��֤��ģʽ���� ",pc->nContractId,pc->sContractCode);
				return false;
			}
			if (nMarginMode == 2) //��߱�֤��
			{
				if (info->sEoFlag[0] == EO_FLAG_OPEN) //����
				{
					if (info->sBsFlag[0] == BS_FLAG_BUY)  //�����
					{
						//���ұ�
						pbm = m_pMemDb->m_pDbTrader.m_pBigNetMargin.Select(info->sTxAcc,pc->nProdId);
						if (pbm == NULL)
						{
							//�ɽ����Ҳ�����¼
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�ɽ�����߼�¼�Ҳ���[%s %d]�ļ�¼",info->sTxAcc,pc->nProdId);
							return false;
						}
						m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"����� �ʽ��˺�[%s]Ʒ��[%d] ��߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
						//�ҵ�ԭ��������ʽ�
						pbm->dBuyFroze = pbm->dBuyFroze - po->dFrozAmt*info->nDoneNum/po->nOrderNum; //�򶳽����
						pbm->nBuyQty = pbm->nBuyQty-info->nDoneNum;//��������
						pbm->nBuyPosi+=info->nDoneNum;//���������
						//���¼�����б�֤��
						if (pbm->nMarginFlag == 1) //����ڶ� ������ͬ
						{
							pbm->dBuyMargin = pbm->dBuyMargin+info->dExMargin+info->dMemberMargin;
							//�����ʽ𣬼����ӳɽ�ʱ�ı�֤��
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_BUY,info->dExMargin+info->dMemberMargin,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
								return false;
							}
						}
						else //����ڿ� �����෴
						{
							pbm->dBuyMargin = pbm->dBuyMargin+info->dExMargin+info->dMemberMargin;
							//ԭ���ǿն࣬����Ҫ������һ��
							if (pbm->dBuyMargin >pbm->dSellMargin+MAXDOUBLECOMPARE ) //����ڶ���
							{
								pbm->nMarginFlag = 1;
								//���˸����򣬴���ڶ� ��ȥ��ԭ�����ֵ
								if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_SELL,0.00-pbm->dSellMargin,true))
								{
									m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
									return false;
								}
								//�������򷽵�ֵ
								if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_BUY,pbm->dBuyMargin,true))
								{
									m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
									return false;
								}
							}
							else  //�����ǿգ�������򲻱䣬ɶ�����ö�
							{
								//���򲻱䣬���ö�
							}
						}
						m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"����� �ʽ��˺�[%s]Ʒ��[%d] ��߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
						if (pbm->nFrozeFlag == 1)
						{
							//��ԭ�����ͷ�
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,0.00-pbm->dFrozeAmt,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
								return false;
							}
						}
						else
						{
							//��ԭ�����ͷ�
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,0.00-pbm->dFrozeAmt,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
								return false;
							}
						}
						dTmp = pbm->dBuyMargin + pbm->dBuyFroze - pbm->dSellMargin - pbm->dSellFroze;
						if (dTmp > MINDOUBLECOMPARE) //���ڵ���0
						{
							pbm->nFrozeFlag = 1; //������ڶ�
							if (pbm->nMarginFlag == 1) //���е��ڶ�
							{
								//pbm->dFrozeAmt  = pbm->dBuyMargin+pbm->dBuyFroze-pbm->dBuyMargin;
								pbm->dFrozeAmt =pbm->dBuyFroze;//ͬ����һ������������
							}
							else //���е��ڿ�
							{
								//����(����+����) ��ȥռ�У���Ϊ����ȥ�����
								pbm->dFrozeAmt  = pbm->dBuyMargin+pbm->dBuyFroze-pbm->dSellMargin;
							}
							//�����µĶ���
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,pbm->dFrozeAmt,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
								return false;
							}
						}
						else
						{
							pbm->nFrozeFlag = 2;
							if (pbm->nMarginFlag == 1) //���е��ڶ�
							{
								pbm->dFrozeAmt  = pbm->dSellMargin+pbm->dSellFroze-pbm->dBuyMargin;
							}
							else //���е��ڿ�
							{
								//����(����+����) ��ȥռ�У���Ϊ����ȥ�����
								//pbm->dFrozeAmt  = pbm->dSellMargin+pbm->dSellFroze-pbm->dSellMargin;
								pbm->dFrozeAmt = pbm->dSellFroze;//ͬ����һ������������
							}
							//�����µĶ���
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,pbm->dFrozeAmt,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
								return false;
							}
						}
						m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"����� �ʽ��˺�[%s]Ʒ��[%d] ��߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
						//�ⶳ������
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dUnfrozeFee,true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund ����");
							return false;
						}
						//����������
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FEE,FUND_CODE_FEE_EXCNANGE,info->dExFee+info->dMemberFee,true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund ����");
							return false;
						}
					}
					else  //���������ղ�
					{
						//���ұ�
						pbm = m_pMemDb->m_pDbTrader.m_pBigNetMargin.Select(info->sTxAcc,pc->nProdId);
						if (pbm == NULL)
						{
							//�ɽ����Ҳ�����¼
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�ɽ�����߼�¼�Ҳ���[%s %d]�ļ�¼",info->sTxAcc,pc->nProdId);
							return false;
						}
						m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"���ղ� �ʽ��˺�[%s]Ʒ��[%d] ��߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
						//�ҵ�ԭ��������ʽ�
						pbm->dSellFroze = pbm->dSellFroze - po->dFrozAmt*info->nDoneNum/po->nOrderNum; //���������
						pbm->nSellQty = pbm->nSellQty-info->nDoneNum;//��������
						pbm->nSellPosi +=info->nDoneNum;//����������
						//���¼�����б�֤��
						if (pbm->nMarginFlag == 1) //����ڶ� �����෴
						{
							pbm->dSellMargin = pbm->dSellMargin+info->dExMargin+info->dMemberMargin;
							if (pbm->dBuyMargin >pbm->dSellMargin+MAXDOUBLECOMPARE ) //����ڶ࣬����
							{
							}
							else  //��߱�ɿ���
							{
								pbm->nMarginFlag = 2;
								//���˸����򣬴���ڶ� ��ȥ��ԭ�����ֵ
								if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_SELL,0.00-pbm->dBuyMargin,true))
								{
									m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
									return false;
								}
								//������������ֵ
								if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_BUY,pbm->dSellMargin,true))
								{
									m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
									return false;
								}
							}
						}
						else //����ڿ� ������ͬ
						{
							pbm->dSellMargin = pbm->dSellMargin+info->dExMargin+info->dMemberMargin;
							//�����ʽ�
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_SELL,info->dExMargin+info->dMemberMargin,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
								return false;
							}
						}
						m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"���ղ� �ʽ��˺�[%s]Ʒ��[%d] ��߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
						//���¼��㶳��
						if (pbm->nFrozeFlag == 1)
						{
							//��ԭ�����ͷ�
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,0.00-pbm->dFrozeAmt,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
								return false;
							}
						}
						else
						{
							//��ԭ�����ͷ�
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,0.00-pbm->dFrozeAmt,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
								return false;
							}
						}
						dTmp = pbm->dBuyMargin + pbm->dBuyFroze - pbm->dSellMargin - pbm->dSellFroze;
						if (dTmp > MINDOUBLECOMPARE) //���ڵ���0
						{
							pbm->nFrozeFlag = 1; //������ڶ�
							if (pbm->nMarginFlag == 1) //���е��ڶ�
							{
								//pbm->dFrozeAmt  = pbm->dBuyMargin+pbm->dBuyFroze-pbm->dBuyMargin;
								pbm->dFrozeAmt =pbm->dBuyFroze;//ͬ����һ������������
							}
							else //���е��ڿ�
							{
								//����(����+����) ��ȥռ�У���Ϊ����ȥ�����
								pbm->dFrozeAmt  = pbm->dBuyMargin+pbm->dBuyFroze-pbm->dSellMargin;
							}
							//�����µĶ���
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,pbm->dFrozeAmt,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
								return false;
							}
						}
						else //��Ϊ����
						{
							pbm->nFrozeFlag = 2;
							if (pbm->nMarginFlag == 1) //���е��ڶ�
							{
								pbm->dFrozeAmt  = pbm->dSellMargin+pbm->dSellFroze-pbm->dBuyMargin;
							}
							else //���е��ڿ�
							{
								//����(����+����) ��ȥռ�У���Ϊ����ȥ�����
								//pbm->dFrozeAmt  = pbm->dSellMargin+pbm->dSellFroze-pbm->dSellMargin;
								pbm->dFrozeAmt = pbm->dSellFroze;//ͬ����һ������������
							}
							//�����µĶ���
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,pbm->dFrozeAmt,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
								return false;
							}
						}
						m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"���ղ� �ʽ��˺�[%s]Ʒ��[%d] ��߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
						//�ⶳ������
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dUnfrozeFee,true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund ����");
							return false;
						}
						//����������
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FEE,FUND_CODE_FEE_EXCNANGE,info->dExFee+info->dMemberFee,true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund ����");
							return false;
						}
					}

				}
				else //ƽ�� 
				{
					//�ⶳί�ж���ĳֲּ�������
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dUnfrozeFee,true))
					{
						return false;
					}
					pPosi = m_pMemDb->m_pDbTrader.m_pPosition.Select(info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo);
					if (pPosi == NULL)
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�޳ֲ� �ʽ��˺�[%s] ϯλ[%s] Ͷ����־[%s] ��Լ[%d %s] ����[%d] ����",\
							info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo,info->sContractCode,info->nDoneNum);
						return false;
					}
					//����ֲ�
					if (info->sBsFlag[0] == BS_FLAG_BUY) //��ƽ  ƽ��
					{
						//���ұ�
						pbm = m_pMemDb->m_pDbTrader.m_pBigNetMargin.Select(info->sTxAcc,pc->nProdId);
						if (pbm == NULL)
						{
							//�ɽ����Ҳ�����¼
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�ɽ�����߼�¼�Ҳ���[%s %d]�ļ�¼",info->sTxAcc,pc->nProdId);
							return false;
						}
						m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"ƽ�ղ� �ʽ��˺�[%s]Ʒ��[%d] ��߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
						//����Լ�ı����ͷ�
						if (pPosi->nCurPS<=0)
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"ƽ�ֳɽ����Ҳ����ֲ� �˻�[%s] Ʒ��[%d]",info->sTxAcc,pc->nProdId);
							return false;
						}
						//�ͷ�ԭ����
						if (pbm->nMarginFlag == 1) //����ڶ�
						{
							dTmp = pbm->dBuyMargin;
							//�ͷ��ʽ�
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_BUY,0.0-pbm->dBuyMargin,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
								return false;
							}
						}
						else //����ڿ� 
						{
							dTmp = pbm->dSellMargin;
							//�ͷ��ʽ�
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_SELL,0.0-pbm->dSellMargin,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
								return false;
							}
						}
						//���¼�����б�֤��
						pbm->dSellMargin = pbm->dSellMargin - pPosi->dPS_Margin*(1.00*info->nDoneNum/pPosi->nCurPS);
						pbm->nSellPosi -= info->nDoneNum;//����������
						if (pbm->dBuyMargin >pbm->dSellMargin+MAXDOUBLECOMPARE ) //����ڶ�
						{
							pbm->nMarginFlag = 1;
							dTmp = pbm->dBuyMargin-dTmp;
							//�����򷽵�ֵ
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_BUY,pbm->dBuyMargin,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
								return false;
							}
						}
						else
						{
							pbm->nMarginFlag = 2;
							dTmp = pbm->dSellMargin-dTmp;
							//�����򷽵�ֵ
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_SELL,pbm->dSellMargin,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
								return false;
							}
						}
						m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"ƽ�ղ� �ʽ��˺�[%s]Ʒ��[%d] ��߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%lf] ��֤����[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt,dTmp);
						//���¼��㶳��	 20210422  ƽ���������᲻�仯������Ϊƽ����֤����٣�������ܻᷢ���仯
						//�ͷ�ԭ����
						if (pbm->nFrozeFlag == 1) //����ڶ�
						{
							//�ͷ��ʽ�
							if (!UpdateFund(lockinfo, fundinfo, FUND_TYPE_FROZE, FUND_CODE_BUYFROZEMARGIN, 0.0 - pbm->dFrozeAmt, true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "UpdateFund����");
								return false;
							}
						}
						else //����ڿ� 
						{
							//�ͷ��ʽ�
							if (!UpdateFund(lockinfo, fundinfo, FUND_TYPE_FROZE, FUND_CODE_SELLFROZEMARGIN, 0.0 - pbm->dFrozeAmt, true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "UpdateFund����");
								return false;
							}
						}
						dTmp = pbm->dBuyMargin + pbm->dBuyFroze - pbm->dSellMargin - pbm->dSellFroze;
						if (dTmp > MINDOUBLECOMPARE) //���ڵ���0
						{
							pbm->nFrozeFlag = 1; //������ڶ�
							if (pbm->nMarginFlag == 1) //���е��ڶ�
							{
								//pbm->dFrozeAmt  = pbm->dBuyMargin+pbm->dBuyFroze-pbm->dBuyMargin;
								pbm->dFrozeAmt = pbm->dBuyFroze;//ͬ����һ������������
							}
							else //���е��ڿ�
							{
								//����(����+����) ��ȥռ�У���Ϊ����ȥ�����
								pbm->dFrozeAmt = pbm->dBuyMargin + pbm->dBuyFroze - pbm->dSellMargin;
							}
							//�����µĶ���
							if (!UpdateFund(lockinfo, fundinfo, FUND_TYPE_FROZE, FUND_CODE_BUYFROZEMARGIN, pbm->dFrozeAmt, true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "UpdateFund����");
								return false;
							}
						}
						else
						{
							pbm->nFrozeFlag = 2;
							if (pbm->nMarginFlag == 1) //���е��ڶ�
							{
								pbm->dFrozeAmt = pbm->dSellMargin + pbm->dSellFroze - pbm->dBuyMargin;
							}
							else //���е��ڿ�
							{
								//����(����+����) ��ȥռ�У���Ϊ����ȥ�����
								//pbm->dFrozeAmt  = pbm->dSellMargin+pbm->dSellFroze-pbm->dSellMargin;
								pbm->dFrozeAmt = pbm->dSellFroze;//ͬ����һ������������
							}
							//�����µĶ���
							if (!UpdateFund(lockinfo, fundinfo, FUND_TYPE_FROZE, FUND_CODE_SELLFROZEMARGIN, pbm->dFrozeAmt, true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "UpdateFund����");
								return false;
							}
						}
						m_pLog->LogMp(LOG_DEBUG, __FILE__, __LINE__, "ƽ�ղ� �ʽ��˺�[%s]Ʒ��[%d] ��߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%lf] ��֤����[%lf]", pbm->sTxAcc, pbm->nVarietyId, pbm->nMarginFlag, pbm->dBuyMargin, pbm->dSellMargin, pbm->nFrozeFlag, pbm->dBuyFroze, pbm->dSellFroze, pbm->dFrozeAmt, dTmp);
						//���¼��㶳��	 20210422 END
						//����
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FEE,FUND_CODE_FEE_EXCNANGE,info->dExFee+info->dMemberFee,true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund ����");
							return false;
						}
					}
					else //��ƽ ƽ��
					{
						//���ұ�
						//���ұ�
						pbm = m_pMemDb->m_pDbTrader.m_pBigNetMargin.Select(info->sTxAcc,pc->nProdId);
						if (pbm == NULL)
						{
							//�ɽ����Ҳ�����¼
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�ɽ�����߼�¼�Ҳ���[%s %d]�ļ�¼",info->sTxAcc,pc->nProdId);
							return false;
						}
						m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"ƽ��� �ʽ��˺�[%s]Ʒ��[%d] ��߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
						//����Լ�ı����ͷ�
						if (pPosi->nCurPB<=0)
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"ƽ�ֳɽ����Ҳ����ֲ� �˻�[%s] Ʒ��[%d]",info->sTxAcc,pc->nProdId);
							return false;
						}
						//���ͷ�ԭ����
						if (pbm->nMarginFlag == 1) //����ڶ� ������ͬ
						{
							dTmp =  pbm->dBuyMargin;
							//�ͷ��ʽ�
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_BUY,0.00-pbm->dBuyMargin,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
								return false;
							}
						}
						else //����ڿ� �����෴
						{
							dTmp =  pbm->dSellMargin;
							//�ͷ��ʽ�
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_SELL,0.00-pbm->dSellMargin,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
								return false;
							}
						}
						//���¼�����б�֤��
						pbm->dBuyMargin = pbm->dBuyMargin - pPosi->dPB_Margin*info->nDoneNum/pPosi->nCurPB;
						pbm->nBuyPosi -= info->nDoneNum;//���������
						if (pbm->dBuyMargin >pbm->dSellMargin+MAXDOUBLECOMPARE ) //����ڶ�
						{
							pbm->nMarginFlag = 1;
							dTmp = pbm->dBuyMargin- dTmp;
							//�����򷽵�ֵ
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_BUY,pbm->dBuyMargin,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
								return false;
							}
						}
						else
						{
							pbm->nMarginFlag = 2;
							dTmp = pbm->dSellMargin- dTmp;
							//�����򷽵�ֵ
							if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_SELL,pbm->dSellMargin,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
								return false;
							}
						}
						m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"ƽ��� �ʽ��˺�[%s]Ʒ��[%d] ��߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%lf] ��֤����[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt,dTmp);
						//���¼��㶳��	 20210422  ƽ���������᲻�仯������Ϊƽ����֤����٣�������ܻᷢ���仯
						//�ͷ�ԭ����
						if (pbm->nFrozeFlag == 1) //����ڶ�
						{
							//�ͷ��ʽ�
							if (!UpdateFund(lockinfo, fundinfo, FUND_TYPE_FROZE, FUND_CODE_BUYFROZEMARGIN, 0.0 - pbm->dFrozeAmt, true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "UpdateFund����");
								return false;
							}
						}
						else //����ڿ� 
						{
							//�ͷ��ʽ�
							if (!UpdateFund(lockinfo, fundinfo, FUND_TYPE_FROZE, FUND_CODE_SELLFROZEMARGIN, 0.0 - pbm->dFrozeAmt, true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "UpdateFund����");
								return false;
							}
						}
						dTmp = pbm->dBuyMargin + pbm->dBuyFroze - pbm->dSellMargin - pbm->dSellFroze;
						if (dTmp > MINDOUBLECOMPARE) //���ڵ���0
						{
							pbm->nFrozeFlag = 1; //������ڶ�
							if (pbm->nMarginFlag == 1) //���е��ڶ�
							{
								//pbm->dFrozeAmt  = pbm->dBuyMargin+pbm->dBuyFroze-pbm->dBuyMargin;
								pbm->dFrozeAmt = pbm->dBuyFroze;//ͬ����һ������������
							}
							else //���е��ڿ�
							{
								//����(����+����) ��ȥռ�У���Ϊ����ȥ�����
								pbm->dFrozeAmt = pbm->dBuyMargin + pbm->dBuyFroze - pbm->dSellMargin;
							}
							//�����µĶ���
							if (!UpdateFund(lockinfo, fundinfo, FUND_TYPE_FROZE, FUND_CODE_BUYFROZEMARGIN, pbm->dFrozeAmt, true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "UpdateFund����");
								return false;
							}
						}
						else
						{
							pbm->nFrozeFlag = 2;
							if (pbm->nMarginFlag == 1) //���е��ڶ�
							{
								pbm->dFrozeAmt = pbm->dSellMargin + pbm->dSellFroze - pbm->dBuyMargin;
							}
							else //���е��ڿ�
							{
								//����(����+����) ��ȥռ�У���Ϊ����ȥ�����
								//pbm->dFrozeAmt  = pbm->dSellMargin+pbm->dSellFroze-pbm->dSellMargin;
								pbm->dFrozeAmt = pbm->dSellFroze;//ͬ����һ������������
							}
							//�����µĶ���
							if (!UpdateFund(lockinfo, fundinfo, FUND_TYPE_FROZE, FUND_CODE_SELLFROZEMARGIN, pbm->dFrozeAmt, true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "UpdateFund����");
								return false;
							}
						}
						m_pLog->LogMp(LOG_DEBUG, __FILE__, __LINE__, "ƽ��� �ʽ��˺�[%s]Ʒ��[%d] ��߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%lf] ��֤����[%lf]", pbm->sTxAcc, pbm->nVarietyId, pbm->nMarginFlag, pbm->dBuyMargin, pbm->dSellMargin, pbm->nFrozeFlag, pbm->dBuyFroze, pbm->dSellFroze, pbm->dFrozeAmt, dTmp);
						//���¼��㶳��	 20210422 END
						//����
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FEE,FUND_CODE_FEE_EXCNANGE,info->dExFee+info->dMemberFee,true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund ����");
							return false;
						}

					}
				}
			}
			else if (nMarginMode == 1) //���߱�֤��
			{
				if (!ProcDoneNetMargin(lockinfo,fundinfo,info,po,pc,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����߳ɽ�����");
					return false;
				}
			}
			else 
			{
				if (info->sEoFlag[0] == EO_FLAG_OPEN) //����
				{
					if (info->sBsFlag[0] == BS_FLAG_BUY)
					{
						//�ⶳί�еı�֤�𼰷���
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,info->dUnfrozeAmt,true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund ����");
							return false;
						}
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dUnfrozeFee,true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund ����");
							return false;
						}
						//�����ʽ�
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_BUY,info->dExMargin+info->dMemberMargin,true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund ����");
							return false;
						}
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FEE,FUND_CODE_FEE_EXCNANGE,info->dExFee+info->dMemberFee,true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund ����");
							return false;
						}

					}
					else
					{
						//�ⶳί�еı�֤�𼰷���
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,info->dUnfrozeAmt,true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund ����");
							return false;
						}
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dUnfrozeFee,true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund ����");
							return false;
						}
						//�����ʽ�	
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_SELL,info->dExMargin+info->dMemberMargin,true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund ����");
							return false;
						}
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FEE,FUND_CODE_FEE_EXCNANGE,info->dExFee+info->dMemberFee,true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund ����");
							return false;
						}
					}
					//���³ֲ�
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

					//	if (info->sBsFlag[0] == BS_FLAG_BUY) //��
					//	{
					//		posi.dPB_Avp = info->dDonePrice;
					//		posi.dPB_Margin = info->dExMargin+info->dMemberMargin;
					//		posi.nDayPB = info->nDoneNum;
					//		posi.nCurCanUsePB = info->nDoneNum;
					//		posi.nCurPB = info->nDoneNum;
					//		posi.dPB_OAvp = info->dDonePrice;
					//	}
					//	else //����
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
					//		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"д��ֱֲ�ʧ�� �ʽ��˺�[%s] ϯλ[%s] Ͷ����־[%s] ��Լ[%d %s] ����[%d] ����",\
					//			info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo,info->sContractCode,info->nDoneNum);
					//		return false;
					//	}
					//}
					//else
					//{
					//	//����ֲ־���
					//	if (info->sBsFlag[0] == BS_FLAG_BUY) //�� ���
					//	{
					//		//����
					//		pPosi->dPB_Avp = (pPosi->dPB_Avp*pPosi->nCurPB+info->dDonePrice*info->nDoneNum)/(pPosi->nCurPB+info->nDoneNum);
					//		pPosi->dPB_Margin = pPosi->dPB_Margin + info->dExMargin+info->dMemberMargin;
					//		pPosi->nDayPB = pPosi->nDayPB+info->nDoneNum;
					//		pPosi->nCurCanUsePB = pPosi->nCurCanUsePB+info->nDoneNum;
					//		pPosi->nCurPB = pPosi->nCurPB+info->nDoneNum;
					//		//���־���
					//		pPosi->dPB_OAvp = (pPosi->dPB_OAvp*pPosi->nCurPB+info->dDonePrice*info->nDoneNum)/(pPosi->nCurPB+info->nDoneNum);
					//	}
					//	else //���� �ղ�
					//	{
					//		//����
					//		pPosi->dPS_Avp = (pPosi->dPS_Avp*pPosi->nCurPS+info->dDonePrice*info->nDoneNum)/(pPosi->nCurPS+info->nDoneNum);
					//		pPosi->dPS_Margin = pPosi->dPS_Margin + info->dExMargin+info->dMemberMargin;
					//		pPosi->nDayPS = pPosi->nDayPS+info->nDoneNum;
					//		pPosi->nCurCanUsePS = pPosi->nCurCanUsePS+info->nDoneNum;
					//		pPosi->nCurPS = pPosi->nCurPS+info->nDoneNum;
					//		//���־���
					//		pPosi->dPS_OAvp = (pPosi->dPS_OAvp*pPosi->nCurPS+info->dDonePrice*info->nDoneNum)/(pPosi->nCurPS+info->nDoneNum);
					//	}
					//}
				}
				else //ƽ�� 
				{
					//�ⶳί�ж���ĳֲּ�������

					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dUnfrozeFee,true))
					{
						return false;
					}
					pPosi = m_pMemDb->m_pDbTrader.m_pPosition.Select(info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo);
					if (pPosi == NULL)
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�޳ֲ� �ʽ��˺�[%s] ϯλ[%s] Ͷ����־[%s] ��Լ[%d %s] ����[%d] ����",\
							info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo,info->sContractCode,info->nDoneNum);
						return false;
					}
					//����ֲ�
					if (info->sBsFlag[0] == BS_FLAG_BUY) //��ƽ  ƽ��
					{
						//���۲��� ��֤������������С
						//���±�֤��
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_SELL,0.00-pPosi->dPS_Margin*(1.00*info->nDoneNum/pPosi->nCurPS),true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund ����");
							return false;
						}
						//����
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FEE,FUND_CODE_FEE_EXCNANGE,info->dExFee+info->dMemberFee,true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund ����");
							return false;
						}
						//ȡ�ֲ���Ϣ
						//pPosi = m_pMemDb->m_pDbTrader.m_pPosition.Select(info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo);
						//if (pPosi == NULL)
						//{
						//	m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�޳ֲ� �ʽ��˺�[%s] ϯλ[%s] Ͷ����־[%s] ��Լ[%d %s] ����[%d] ����",\
						//		info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo,info->sContractCode,info->nDoneNum);
						//	return false;

						//}
						//if (pPosi->nCurPS<=0)
						//{
						//	m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"ƽ�ֳɽ����Ҳ����ֲ� �˻�[%s] Ʒ��[%d]",info->sTxAcc,pc->nProdId);
						//	return false;
						//}
						//pPosi->dPS_Margin = pPosi->dPS_Margin*(1.00-1.00*info->nDoneNum/pPosi->nCurPS);
						//pPosi->nDAY_COV_PS_Froze = pPosi->nDAY_COV_PS_Froze-info->nDoneNum;
						////������٣����ò���
						////pPosi->nCurCanUsePS = pPosi->nCurCanUsePS-info->nDoneNum;
						//pPosi->nCurPS = pPosi->nCurPS-info->nDoneNum;
						////����񿪳ֲ�
						//if (info->sEoFlag[0] == EO_FLAG_CLOSETODAY) //ƽ���
						//{
						//	pPosi->nDayPS = pPosi->nDayPS -  info->nDoneNum;
						//}
						//else //ƽ��
						//{
						//	if (pPosi->nCurPS < pPosi->nDayPS)
						//	{
						//		pPosi->nDayPS = pPosi->nCurPS;//ʣ�µ�ȫ�ǽ�ֲ���
						//	}
						//}
						////����ƽ��ӯ��
						//if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_SURPLUS,FUND_CODE_SURPLUS_CLOSE,(pPosi->dPS_Avp-info->dDonePrice)*info->nDoneNum*pc->dTxUnit,true))
						//{
						//	m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
						//	return false;
						//}

					}
					else //��ƽ ƽ��
					{
						//���۲��� ��֤������������С
						//���±�֤��
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_BUY,0.00-pPosi->dPB_Margin*(1.00*info->nDoneNum/pPosi->nCurPB),true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund ����");
							return false;
						}
						//����
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FEE,FUND_CODE_FEE_EXCNANGE,info->dExFee+info->dMemberFee,true))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund ����");
							return false;
						}
						//ȡ�ֲ���Ϣ
						//pPosi = m_pMemDb->m_pDbTrader.m_pPosition.Select(info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo);
						//if (pPosi == NULL)
						//{
						//	m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�޳ֲ� �ʽ��˺�[%s] ϯλ[%s] Ͷ����־[%s] ��Լ[%d %s] ����[%d] ����",\
						//		info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo,info->sContractCode,info->nDoneNum);
						//	return false;

						//}
						//if (pPosi->nCurPB<=0)
						//{
						//	m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"ƽ�ֳɽ����Ҳ����ֲ� �˻�[%s] Ʒ��[%d]",info->sTxAcc,pc->nProdId);
						//	return false;
						//}
						//pPosi->dPB_Margin = pPosi->dPB_Margin*(1.00-1.00*info->nDoneNum/pPosi->nCurPB);
						////������٣����ò���
						//pPosi->nDAY_COV_PB_Froze = pPosi->nDAY_COV_PB_Froze-info->nDoneNum;
						////pPosi->nCurCanUsePB = pPosi->nCurCanUsePB-info->nDoneNum;
						//pPosi->nCurPB = pPosi->nCurPB-info->nDoneNum;
						////����񿪳ֲ�
						//if (info->sEoFlag[0] == EO_FLAG_CLOSETODAY) //ƽ���
						//{
						//	pPosi->nDayPB = pPosi->nDayPB -  info->nDoneNum;
						//}
						//else //ƽ��
						//{
						//	if (pPosi->nCurPB < pPosi->nDayPB)
						//	{
						//		pPosi->nDayPB = pPosi->nCurPB;//ʣ�µ�ȫ�ǽ�ֲ���
						//	}

						//}
						////����ƽ��ӯ��
						//if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_SURPLUS,FUND_CODE_SURPLUS_CLOSE,(info->dDonePrice-pPosi->dPB_Avp)*info->nDoneNum*pc->dTxUnit,true))
						//{
						//	m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
						//	return false;
						//}
					}
				}
			}
			if (!UpdatePosition(lockinfo,fundinfo,info,pPosi,pc,false))
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdatePosition ���� ");
			}
			break;
		case PROD_TYPE_SPOT:
		case PROD_TYPE_SEC:

			//���³ֲ�
			pPosi = m_pMemDb->m_pDbTrader.m_pPosition.Select(info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo);
			if (info->sBsFlag[0] == BS_FLAG_BUY)
			{
				//�ⶳί�еı�֤�𼰷���
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZE,info->dUnfrozeAmt,true))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund ����");
					return false;
				}
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dUnfrozeFee,true))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund ����");
					return false;
				}
				//�����ʽ�
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_INOUT,FUND_CODE_BUY,info->dAmt,true))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund ����");
					return false;
				}
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FEE,FUND_CODE_FEE_EXCNANGE,info->dExFee+info->dMemberFee,true))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund ����");
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
					posi.dPB_Avp = posi.dPB_Margin/info->nDoneNum;//�ɱ�����/ÿ��
					posi.nDayPB = info->nDoneNum;
					posi.nCurCanUsePB = info->nDoneNum;
					posi.nCurPB = info->nDoneNum;
					posi.dPB_OAvp = posi.dPB_Avp;//���ֳɱ����� 

					posi.nVarietyId = pc->nProdId;
					if (m_pMemDb->m_pDbTrader.m_pPosition.Insert(posi)<0)
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"д��ֱֲ�ʧ�� �ʽ��˺�[%s] ϯλ[%s] Ͷ����־[%s] ��Լ[%d %s] ����[%d] ����",\
							info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo,info->sContractCode,info->nDoneNum);
						return false;
					}
				}
				else
				{
					//���о���
					pPosi->dPB_Avp = (pPosi->dPB_Avp*pPosi->nCurPB-info->dAmt-info->dExFee-info->dMemberFee)/(pPosi->nCurPB+info->nDoneNum);
					pPosi->dPB_Margin = pPosi->dPB_Margin - info->dAmt-info->dExFee-info->dMemberFee;
					pPosi->nDayPB = pPosi->nDayPB+info->nDoneNum;
					pPosi->nCurCanUsePB = pPosi->nCurCanUsePB+info->nDoneNum;
					pPosi->nCurPB = pPosi->nCurPB+info->nDoneNum;
					//���־���
					pPosi->dPB_OAvp = (pPosi->dPB_OAvp*pPosi->nDayPB+info->dAmt+info->dExFee+info->dMemberFee)/(pPosi->nDayPB+info->nDoneNum);

				}
			}
			else  //����
			{
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dUnfrozeFee,true))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund ����");
					return false;
				}
				//�����ʽ�	
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_INOUT,FUND_CODE_SELL,info->dAmt,true))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund ����");
					return false;
				}
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FEE,FUND_CODE_FEE_EXCNANGE,info->dExFee+info->dMemberFee,true))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund ����");
					return false;
				}
				if (pPosi == NULL)
				{

					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"д��ֱֲ�ʧ�� �ʽ��˺�[%s] ϯλ[%s] Ͷ����־[%s] ��Լ[%d %s] ����[%d] ����",\
						info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo,info->sContractCode,info->nDoneNum);
					return false;
				}
				//����ֲ־���
				if (pPosi->nCurPB<=0)
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ɽ����Ҳ������ �˻�[%s]",info->sTxAcc);
					return false;
				}
				pPosi->dPB_Margin = pPosi->dPB_Margin*(pPosi->nCurPB-info->nDoneNum)/pPosi->nCurPB;
				pPosi->nCurCanUsePB = pPosi->nCurCanUsePB+info->nDoneNum;
				pPosi->nCurPB = pPosi->nCurPB+info->nDoneNum;
				//���־��۲���
				//��ӯ�����µ�������
				pPosi->dPB_Avp = (pPosi->dPB_OAvp*pPosi->nCurPB+info->dCovSurplus)/pPosi->nCurPB;

				//��������ӯ��
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_SURPLUS,FUND_CODE_SURPLUS_CLOSE,(info->dDonePrice-pPosi->dPB_Avp)*info->nDoneNum*pc->dTxUnit,true))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
					return false;
				}
			}
			break;
		case PROD_TYPE_OPTION:
			if (info->sEoFlag[0] == EO_FLAG_OPEN) //����
			{
				if (info->sBsFlag[0] == BS_FLAG_BUY)
				{
					//�ⶳί�еı�֤�𼰷���
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,info->dUnfrozeAmt,true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund ����");
						return false;
					}
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dUnfrozeFee,true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund ����");
						return false;
					}
					//�����ʽ�
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_PAYRIGHT,info->dAmt,true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund ����");
						return false;
					}
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FEE,FUND_CODE_FEE_EXCNANGE,info->dExFee+info->dMemberFee,true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund ����");
						return false;
					}

				}
				else
				{
					//�ⶳί�еı�֤�𼰷���
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,info->dUnfrozeAmt,true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund ����");
						return false;
					}
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dUnfrozeFee,true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund ����");
						return false;
					}
					//�����ʽ�	
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_SELL,info->dExMargin+info->dMemberMargin,true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund ����");
						return false;
					}
					//�����ʽ�
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_GETRIGHT,info->dAmt,true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund ����");
						return false;
					}
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FEE,FUND_CODE_FEE_EXCNANGE,info->dExFee+info->dMemberFee,true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund ����");
						return false;
					}
				}
				//���³ֲ�
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

				//	if (info->sBsFlag[0] == BS_FLAG_BUY) //��
				//	{
				//		posi.dPB_Avp = info->dDonePrice;
				//		posi.dPB_Margin = info->dExMargin+info->dMemberMargin;
				//		posi.nDayPB = info->nDoneNum;
				//		posi.nCurCanUsePB = info->nDoneNum;
				//		posi.nCurPB = info->nDoneNum;
				//		posi.dPB_OAvp = info->dDonePrice;
				//	}
				//	else //����
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
				//		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"д��ֱֲ�ʧ�� �ʽ��˺�[%s] ϯλ[%s] Ͷ����־[%s] ��Լ[%d %s] ����[%d] ����",\
				//			info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo,info->sContractCode,info->nDoneNum);
				//		return false;
				//	}
				//}
				//else
				//{
				//	//����ֲ־���
				//	if (info->sBsFlag[0] == BS_FLAG_BUY) //�� ���
				//	{
				//		//����
				//		pPosi->dPB_Avp = (pPosi->dPB_Avp*pPosi->nCurPB+info->dDonePrice*info->nDoneNum)/(pPosi->nCurPB+info->nDoneNum);
				//		pPosi->dPB_Margin = pPosi->dPB_Margin + info->dExMargin+info->dMemberMargin;
				//		pPosi->nDayPB = pPosi->nDayPB+info->nDoneNum;
				//		pPosi->nCurCanUsePB = pPosi->nCurCanUsePB+info->nDoneNum;
				//		pPosi->nCurPB = pPosi->nCurPB+info->nDoneNum;
				//		//���־���
				//		pPosi->dPB_OAvp = (pPosi->dPB_OAvp*pPosi->nCurPB+info->dDonePrice*info->nDoneNum)/(pPosi->nCurPB+info->nDoneNum);
				//	}
				//	else //���� �ղ�
				//	{
				//		//����
				//		pPosi->dPS_Avp = (pPosi->dPS_Avp*pPosi->nCurPS+info->dDonePrice*info->nDoneNum)/(pPosi->nCurPS+info->nDoneNum);
				//		pPosi->dPS_Margin = pPosi->dPS_Margin + info->dExMargin+info->dMemberMargin;
				//		pPosi->nDayPS = pPosi->nDayPS+info->nDoneNum;
				//		pPosi->nCurCanUsePS = pPosi->nCurCanUsePS+info->nDoneNum;
				//		pPosi->nCurPS = pPosi->nCurPS+info->nDoneNum;
				//		//���־���
				//		pPosi->dPS_OAvp = (pPosi->dPS_OAvp*pPosi->nCurPS+info->dDonePrice*info->nDoneNum)/(pPosi->nCurPS+info->nDoneNum);
				//	}
				//}
			}
			else //ƽ��
			{
				//�ⶳί�ж���ĳֲּ�������

				if (UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-info->dUnfrozeFee,true))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund ����");
					return false;
				}
				//ȡ�ֲ���Ϣ
				//pPosi = m_pMemDb->m_pDbTrader.m_pPosition.Select(info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo);
				//if (pPosi == NULL)
				//{
				//	m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�޳ֲ� �ʽ��˺�[%s] ϯλ[%s] Ͷ����־[%s] ��Լ[%d %s] ����[%d] ����",\
				//		info->sTxAcc,info->sSeatNo,info->sShFlag,info->nContractNo,info->sContractCode,info->nDoneNum);
				//	return false;

				//}
				//����ֲ�
				if (info->sBsFlag[0] == BS_FLAG_BUY) //��ƽ  ƽ��
				{
					//���۲��� ��֤������������С
					//���±�֤��
					if (!UpdateFund(lockinfo, fundinfo, FUND_TYPE_MARGIN, FUND_CODE_MARGIN_POSI_SELL, pPosi->dPS_Margin * (1.00 * info->nDoneNum / pPosi->nCurPS), true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "UpdateFund ����");
						return false;
					}
					//����
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FEE,FUND_CODE_FEE_EXCNANGE,info->dExFee+info->dMemberFee,true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund ����");
						return false;
					}
					if (pPosi->nCurPS<=0)
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"ƽ�ֳɽ����Ҳ������ �˻�[%s]",info->sTxAcc);
						return false;
					}
					//pPosi->dPS_Margin = pPosi->dPS_Margin*(1.00-1.00*info->nDoneNum/pPosi->nCurPS);
					//pPosi->nDAY_COV_PS_Froze = pPosi->nDAY_COV_PS_Froze-info->nDoneNum;
					////������٣����ò���
					////pPosi->nCurCanUsePS = pPosi->nCurCanUsePS-info->nDoneNum;
					//pPosi->nCurPS = pPosi->nCurPS-info->nDoneNum;
					////����񿪳ֲ�
					//if (info->sEoFlag[0] == EO_FLAG_CLOSETODAY) //ƽ���
					//{
					//	pPosi->nDayPS = pPosi->nDayPS -  info->nDoneNum;
					//}
					//else //ƽ��
					//{
					//	if (pPosi->nCurPS < pPosi->nDayPS)
					//	{
					//		pPosi->nDayPS = pPosi->nCurPS;//ʣ�µ�ȫ�ǽ�ֲ���
					//	}

					//}
					////����ƽ��ӯ��
					//if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_SURPLUS,FUND_CODE_SURPLUS_CLOSE,(pPosi->dPS_Avp-info->dDonePrice)*info->nDoneNum*pc->dTxUnit,true))
					//{
					//	m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
					//	return false;
					//}
				}
				else //��ƽ ƽ��
				{
					////���۲��� ��֤������������С
					////���±�֤��
					//if (!UpdateFund(lockinfo, fundinfo, FUND_TYPE_MARGIN, FUND_CODE_MARGIN_POSI_BUY, pPosi->dPB_Margin * (1.00 * info->nDoneNum / pPosi->nCurPB), true))
					//{
					//	m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "UpdateFund ����");
					//	return false;
					//}
					//����
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FEE,FUND_CODE_FEE_EXCNANGE,info->dExFee+info->dMemberFee,true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund ����");
						return false;
					}
					if (pPosi->nCurPB<=0)
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"ƽ�ֳɽ����Ҳ������ �˻�[%s]",info->sTxAcc);
						return false;
					}
					//pPosi->dPB_Margin = pPosi->dPB_Margin*(1.00-1.00*info->nDoneNum/pPosi->nCurPB);
					////������٣����ò���
					//pPosi->nDAY_COV_PB_Froze = pPosi->nDAY_COV_PB_Froze-info->nDoneNum;
					////pPosi->nCurCanUsePB = pPosi->nCurCanUsePB-info->nDoneNum;
					//pPosi->nCurPB = pPosi->nCurPB-info->nDoneNum;
					////����񿪳ֲ�
					//if (info->sEoFlag[0] == EO_FLAG_CLOSETODAY) //ƽ���
					//{
					//	pPosi->nDayPB = pPosi->nDayPB -  info->nDoneNum;
					//}
					//else //ƽ��
					//{
					//	if (pPosi->nCurPB < pPosi->nDayPB)
					//	{
					//		pPosi->nDayPB = pPosi->nCurPB;//ʣ�µ�ȫ�ǽ�ֲ���
					//	}

					//}
					////����ƽ��ӯ��
					//if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_SURPLUS,FUND_CODE_SURPLUS_CLOSE,(info->dDonePrice-pPosi->dPB_Avp)*info->nDoneNum*pc->dTxUnit,true))
					//{
					//	m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
					//	return false;
					//}
				}
			}
			if (!UpdatePosition(lockinfo,fundinfo,info,pPosi,pc,false))
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdatePosition ���� ");
			}
			break;
		default:
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"��Լ����[%c] ���� ",pc->sProd_type[0]);
			return false;
	}
	
	//���±���ͬ����־
	if (po->nSyncFlag == 2)
	{
		po->nSyncFlag = 4;//�����ɽ�ͬ��
	}
	else
	{
		po->nSyncFlag = 3;//�ɽ�ͬ��
	}
	return true;
}
int CKernelPub::ProceeOneCancelOrder(S_MDB_TXACC_LOCK *pAcclock,S_MDB_ORDER_INFO *po,int nCancelNum )
{
	//����
	CBF_PMutex pp(pAcclock->mutex);

	if (po->sCancelFlag[0] == BOOL_TRUE)
	{
		//�����ѳ�
		return 0;
	}


	po->nCancelNum = nCancelNum;
	po->nRemainNum = 0;
	po->sCancelFlag[0] = BOOL_TRUE;
	po->sCancelFlag[1] = 0;
	//����״̬
	if (po->nCancelNum == po->nOrderNum)
	{
		po->sOrderStatus[0] = ORDER_STATUS_CANCEL;//ȫ��
	}
	else
	{
		po->sOrderStatus[0] = ORDER_STATUS_PARTCANCEL;//���ɲ���
	}
	po->sOrderStatus[1] = 0;
	m_pDate.Update();
	strcpy(po->sCancelTime,m_pDate.ToStringDT17().c_str());
	if (po->nSyncFlag == 3 )
	{
		po->nSyncFlag = 4;//�ɽ��ͳ���
	}
	else
	{
		po->nSyncFlag = 2;//����
	}

	//�ⶳ�ʽ��ֲ�
	S_MDB_CONTRACT_INFO *pc = m_pMemDb->m_pDbBaseinfo.m_pContract.Select(po->nContractNo);
	if (pc == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"����֪ͨ ��Լ[%d]�Ҳ���",po->nContractNo);
		return -1;
	}

	S_MDB_CUST_FUND *pFund = m_pMemDb->GetFund(pAcclock,pc->sCurrency);
	if (pFund == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"����֪ͨ �ʽ��˺�[%s] ����[%s] ���ʽ���Ҳ���",po->sTxAcc,pc->sCurrency);
		return -1;
	}  
	S_MDB_ORDER_INFO *info = po;
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"������  �˻�[%s]��Լ���[%d %s]  ��ƽ[%s] ����[%s] Ͷ��[%s] ���ر�����[%s] ����[%lf] ί����[%d] ʣ����[%d]",\
		info->sTxAcc,info->nContractNo,info->sContractCode,info->sEoFlag,info->sBsFlag,info->sShFlag,info->sLocalSerial,info->dFrozAmt,info->nOrderNum,info->nRemainNum);

	S_MDB_CUST_POSITION *pPosi=NULL;
	
	S_MDB_BIG_MARGIN *pbm=NULL;
	int nMarginMode=0;
	double dTmp=0.0;
	switch (pc->sProd_type[0])
	{
		case PROD_TYPE_FUTURES://�ڻ�
			nMarginMode = GetMarginMode(pc);
			if (nMarginMode<0)
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"��Լ[%d %s] ȡ��֤��ģʽ���� ",pc->nContractId,pc->sContractCode);
				return false;
			}
			if (nMarginMode == 2) //��߱�֤��
			{
				if (po->sEoFlag[0] == EO_FLAG_OPEN) //����
				{
					if (po->sBsFlag[0] == BS_FLAG_BUY) //�� ���
					{
						//���ұ�
						pbm = m_pMemDb->m_pDbTrader.m_pBigNetMargin.Select(po->sTxAcc,pc->nProdId);
						if (pbm == NULL)
						{
							//�������Ҳ�����¼
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"��������߼�¼�Ҳ���[%s %d]�ļ�¼",po->sTxAcc,pc->nProdId);
							return false;
						}
						//����Լ�ı����ͷ�
						dTmp =  po->dFrozAmt*(1.00*nCancelNum/po->nOrderNum);
						pbm->dBuyFroze = pbm->dBuyFroze - dTmp;
						pbm->nBuyQty -= nCancelNum;
						if (pbm->nFrozeFlag == 1)
						{
							//��ԭ�����ͷ�
							if (!UpdateFund(pAcclock,pFund,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,0.00-pbm->dFrozeAmt,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
								return false;
							}
						}
						else
						{
							//��ԭ�����ͷ�
							if (!UpdateFund(pAcclock,pFund,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,0.00-pbm->dFrozeAmt,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
								return false;
							}
						}
						dTmp = pbm->dBuyMargin + pbm->dBuyFroze - pbm->dSellMargin - pbm->dSellFroze;
						if (dTmp > MINDOUBLECOMPARE) //���ڵ���0
						{
							pbm->nFrozeFlag = 1; //������ڶ�
							if (pbm->nMarginFlag == 1) //���е��ڶ�
							{
								//pbm->dFrozeAmt  = pbm->dBuyMargin+pbm->dBuyFroze-pbm->dBuyMargin;
								pbm->dFrozeAmt =pbm->dBuyFroze;//ͬ����һ������������
							}
							else //���е��ڿ�
							{
								//����(����+����) ��ȥռ�У���Ϊ����ȥ�����
								pbm->dFrozeAmt  = pbm->dBuyMargin+pbm->dBuyFroze-pbm->dSellMargin;
							}
							m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�ʽ��˺�[%s]Ʒ��[%d] ��߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
							//�����µĶ���
							if (!UpdateFund(pAcclock,pFund,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,pbm->dFrozeAmt,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
								return false;
							}
						}
						else
						{
							pbm->nFrozeFlag = 2;
							if (pbm->nMarginFlag == 1) //���е��ڶ�
							{
								pbm->dFrozeAmt  = pbm->dSellMargin+pbm->dSellFroze-pbm->dBuyMargin;
							}
							else //���е��ڿ�
							{
								//����(����+����) ��ȥռ�У���Ϊ����ȥ�����
								//pbm->dFrozeAmt  = pbm->dSellMargin+pbm->dSellFroze-pbm->dSellMargin;
								pbm->dFrozeAmt = pbm->dSellFroze;//ͬ����һ������������
							}
							m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�ʽ��˺�[%s]Ʒ��[%d] ��߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
							//�����µĶ���
							if (!UpdateFund(pAcclock,pFund,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,pbm->dFrozeAmt,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
								return false;
							}
						}
					}
					else //���� �ղ�
					{
						//���ұ�
						pbm = m_pMemDb->m_pDbTrader.m_pBigNetMargin.Select(po->sTxAcc,pc->nProdId);
						if (pbm == NULL)
						{
							//�������Ҳ�����¼
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"��������߼�¼�Ҳ���[%s %d]�ļ�¼",po->sTxAcc,pc->nProdId);
							return false;
						}
						//����Լ�ı����ͷ�
						dTmp =  po->dFrozAmt*(1.00*nCancelNum/po->nOrderNum);
						pbm->dSellFroze = pbm->dSellFroze - dTmp;
						pbm->nSellQty -= nCancelNum;
						if (pbm->nFrozeFlag == 1)
						{
							//��ԭ�����ͷ�
							if (!UpdateFund(pAcclock,pFund,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,0.00-pbm->dFrozeAmt,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
								return false;
							}
						}
						else
						{
							//��ԭ�����ͷ�
							if (!UpdateFund(pAcclock,pFund,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,0.00-pbm->dFrozeAmt,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
								return false;
							}
						}
						dTmp = pbm->dBuyMargin + pbm->dBuyFroze - pbm->dSellMargin - pbm->dSellFroze;
						if (dTmp > MINDOUBLECOMPARE) //���ڵ���0
						{
							pbm->nFrozeFlag = 1; //������ڶ�
							if (pbm->nMarginFlag == 1) //���е��ڶ�
							{
								//pbm->dFrozeAmt  = pbm->dBuyMargin+pbm->dBuyFroze-pbm->dBuyMargin;
								pbm->dFrozeAmt =pbm->dBuyFroze;//ͬ����һ������������
							}
							else //���е��ڿ�
							{
								//����(����+����) ��ȥռ�У���Ϊ����ȥ�����
								pbm->dFrozeAmt  = pbm->dBuyMargin+pbm->dBuyFroze-pbm->dSellMargin;
							}
							m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�ʽ��˺�[%s]Ʒ��[%d] ��߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
							//�����µĶ���
							if (!UpdateFund(pAcclock,pFund,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,pbm->dFrozeAmt,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
								return false;
							}
						}
						else
						{
							pbm->nFrozeFlag = 2;
							if (pbm->nMarginFlag == 1) //���е��ڶ�
							{
								pbm->dFrozeAmt  = pbm->dSellMargin+pbm->dSellFroze-pbm->dBuyMargin;
							}
							else //���е��ڿ�
							{
								//����(����+����) ��ȥռ�У���Ϊ����ȥ�����
								//pbm->dFrozeAmt  = pbm->dSellMargin+pbm->dSellFroze-pbm->dSellMargin;
								pbm->dFrozeAmt = pbm->dSellFroze;//ͬ����һ������������
							}
							m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�ʽ��˺�[%s]Ʒ��[%d] ��߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
							//�����µĶ���
							if (!UpdateFund(pAcclock,pFund,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,pbm->dFrozeAmt,true))
							{
								m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
								return false;
							}
						}
					}
					//�ⶳʣ���������
					if (!UpdateFund(pAcclock,pFund,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,(po->dExFee*nCancelNum/po->nOrderNum),true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund ����");
						return -1;
					}
					
				}
				else //ƽ��  
				{
					//�ⶳʣ��������Ѻͳֲ�
					pPosi = m_pMemDb->m_pDbTrader.m_pPosition.Select(po->sTxAcc,po->sSeatNo,po->sShFlag,po->nContractNo);
					if (pPosi == NULL)
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�ʽ��˺�[%s] ƽ��ʱ ϯλ[%s] Ͷ����־[%s] ��Լ[%d %s]�޳ֲ�",po->sTxAcc,po->sSeatNo,po->sShFlag,po->nContractNo,po->sContractCode);
						return -1;
					}
					//�ⶳ�ᱨ������
					if (po->sBsFlag[0] == BS_FLAG_BUY) //��ƽ ƽ�ղ�
					{
						//���ÿղ�����
						pPosi->nCurCanUsePS = pPosi->nCurCanUsePS + nCancelNum;
						//����ղּ���
						pPosi->nDAY_COV_PS_Froze = pPosi->nDAY_COV_PS_Froze - nCancelNum;
					}
					else //��ƽ ƽ���
					{
						//���ö������
						pPosi->nCurCanUsePB = pPosi->nCurCanUsePB + nCancelNum;
						//�����ּ���
						pPosi->nDAY_COV_PB_Froze = pPosi->nDAY_COV_PB_Froze - nCancelNum;
					}
					//�ⶳ����
					if (!UpdateFund(pAcclock,pFund,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,po->dExFee*nCancelNum/po->nOrderNum,true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"���������ʽ���� ");
						return -1;
					}
				}
			}
			else if (nMarginMode == 1) //���߱�֤��
			{
				if (!ProcCancelOrderNetMargin(pAcclock,pFund,po,pc))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"���� ���ߴ������ ");
					return -1;
				}
			}
			else
			{
				if (po->sEoFlag[0] == EO_FLAG_OPEN) //����
				{
					//�ⶳʣ��������Ѻͱ�֤��
					if (!UpdateFund(pAcclock,pFund,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,(0.00-po->dFrozAmt*nCancelNum/po->nOrderNum),true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund ����");
						return -1;
					}
					if (!UpdateFund(pAcclock,pFund,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,(po->dExFee*nCancelNum/po->nOrderNum),true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund ����");
						return -1;
					}
				}
				else
				{
					//�ⶳʣ��������Ѻͳֲ�
					pPosi = m_pMemDb->m_pDbTrader.m_pPosition.Select(po->sTxAcc,po->sSeatNo,po->sShFlag,po->nContractNo);
					if (pPosi == NULL)
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�ʽ��˺�[%s] ƽ��ʱ ϯλ[%s] Ͷ����־[%s] ��Լ[%d %s]�޳ֲ�",po->sTxAcc,po->sSeatNo,po->sShFlag,po->nContractNo,po->sContractCode);
						return -1;
					}
					//�ⶳ�ᱨ������
					if (po->sBsFlag[0] == BS_FLAG_BUY) //��ƽ ƽ�ղ�
					{
						//���ÿղ�����
						pPosi->nCurCanUsePS = pPosi->nCurCanUsePS + nCancelNum;
						//����ղּ���
						pPosi->nDAY_COV_PS_Froze = pPosi->nDAY_COV_PS_Froze - nCancelNum;
					}
					else //��ƽ ƽ���
					{
						//���ö������
						pPosi->nCurCanUsePB = pPosi->nCurCanUsePB + nCancelNum;
						//�����ּ���
						pPosi->nDAY_COV_PB_Froze = pPosi->nDAY_COV_PB_Froze - nCancelNum;
					}
					//�ⶳ����
					if (!UpdateFund(pAcclock,pFund,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,(po->dExFee*nCancelNum/po->nOrderNum),true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"���������ʽ���� ");
						return -1;
					}
				}
			}
			
			break;
		case PROD_TYPE_OPTION://��Ȩ
			if (po->sEoFlag[0] == EO_FLAG_OPEN) //����
			{
				//�ⶳ�ʽ�
				if (po->sBsFlag[0] == BS_FLAG_BUY) //��
				{
					//�ⶳȨ����
					if (!UpdateFund(pAcclock,pFund,FUND_TYPE_FROZE,FUND_CODE_BUYFROZE,(0.00-po->dAmt*nCancelNum/po->nOrderNum),true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"���������ʽ����");
						return -1;
					}
					//��������
					if (!UpdateFund(pAcclock,pFund,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,(0.00-po->dExFee*nCancelNum/po->nOrderNum),true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"���������ʽ���� ");
						return -1;
					}
				}
				else  //����
				{

					//�ⶳ��������֤��
					if (!UpdateFund(pAcclock,pFund,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,(0.00-po->dFrozAmt*nCancelNum/po->nOrderNum),true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"���������ʽ����");
						return -1;
					}
					//�ⶳ��������
					if (!UpdateFund(pAcclock,pFund,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,(po->dExFee*nCancelNum/po->nOrderNum),true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"���������ʽ����");
						return -1;
					}
				}
			}
			else //ƽ
			{
				//�ⶳ�ֲ�
				pPosi = m_pMemDb->m_pDbTrader.m_pPosition.Select(po->sTxAcc,po->sSeatNo,po->sShFlag,po->nContractNo);
				if (pPosi == NULL)
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�ʽ��˺�[%s] ƽ��ʱ ϯλ[%s] Ͷ����־[%s] ��Լ[%d %s]�޳ֲ�",po->sTxAcc,po->sSeatNo,po->sShFlag,po->nContractNo,po->sContractCode);
					return -1;
				}
				//ֻ��ʣ������
				if (po->sBsFlag[0] == BS_FLAG_BUY) //��ƽ ƽ�ղ�
				{
					//�ⶳȨ����
					if (!UpdateFund(pAcclock,pFund,FUND_TYPE_FROZE,FUND_CODE_BUYFROZE,(0.00-po->dAmt*nCancelNum/po->nOrderNum),true))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"���������ʽ����");
						return -1;
					}
					//�ղֿ�������
					pPosi->nCurCanUsePS = pPosi->nCurCanUsePS +nCancelNum;
					//�������
					pPosi->nDAY_COV_PS_Froze = pPosi->nDAY_COV_PS_Froze - nCancelNum;
				}
				else //��ƽ ƽ���
				{
					//��ֿ�������
					pPosi->nCurCanUsePB = pPosi->nCurCanUsePB + nCancelNum;
					//�������
					pPosi->nDAY_COV_PB_Froze = pPosi->nDAY_COV_PB_Froze - nCancelNum;
				}
				//������
				if (!UpdateFund(pAcclock,pFund,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,(0.00-po->dExFee*nCancelNum/po->nOrderNum),true))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"���������ʽ����");
					return -1;
				}

			}
			break;
		case PROD_TYPE_SPOT://�ֻ�
		case PROD_TYPE_SEC://֤ȯ
			if (po->sBsFlag[0] == BS_FLAG_BUY) //�� 
			{
				//���򶳽�
				if (!UpdateFund(pAcclock,pFund,FUND_TYPE_FROZE,FUND_CODE_BUYFROZE,(0.00-po->dAmt*nCancelNum/po->nOrderNum),true))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"���������ʽ����");
					return -1;
				}
				if (!UpdateFund(pAcclock,pFund,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,(po->dExFee*nCancelNum/po->nOrderNum),true))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"���������ʽ����");
					return -1;
				}
			}
			else
			{
				//�ⶳ�ֲֿ��
				pPosi = m_pMemDb->m_pDbTrader.m_pPosition.Select(po->sTxAcc,po->sSeatNo,po->sShFlag,po->nContractNo);
				if (pPosi == NULL)
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�ʽ��˺�[%s] ƽ��ʱ ϯλ[%s] Ͷ����־[%s] ��Լ[%d %s]�޳ֲ�",po->sTxAcc,po->sSeatNo,po->sShFlag,po->nContractNo,po->sContractCode);
					return -1;
				}
				//���ö������
				pPosi->nCurCanUsePB = pPosi->nCurCanUsePB +nCancelNum;
				//�����ּ���
				pPosi->nDAY_COV_PB_Froze = pPosi->nDAY_COV_PB_Froze -nCancelNum;
				//������
				if (!UpdateFund(pAcclock,pFund,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,(0.00-po->dExFee*nCancelNum/po->nOrderNum),true))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"���������ʽ����");
					return -1;
				}
			}

			break;
		default:
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"ί�� ��Լ����[%s]��Ʒ�����[%s]����",po->sContractCode,pc->sProd_type);
			return -1;
	}
	return 0;
}

bool CKernelPub::CheckOperAccRight(const char *oper, const char *acc)
{
	std::vector<S_MDB_OPER_CUST_INFO *> oclist;
	if (m_pMemDb->m_pDbBaseinfo.m_pOperTxAcc.Select((char *)oper,oclist))
	{
		 //�����ò���Ա��Ӧ�˺�,�Դ�Ϊ׼�������Ի������ж�
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
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"����Ա[%s]�ڲ���Ա���޼�¼",oper);
		return false;
	}
	S_MDB_CUST_FUNDACC_INFO *pacc = m_pMemDb->m_pDbCust.m_pCustFundAcc.SelectByAcc((char *)acc);
	if (pacc == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�ʽ��˺�[%s]���ʽ��˺ű��޼�¼",acc);
		return false;
	}
	S_MDB_ORG_INFO *operorg = m_pMemDb->m_pDbPublic.m_pOrg.Select(poper->sOrgCode);
	if (operorg == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"����Ա[%s]�Ļ���[%s]�ڻ������޼�¼",oper,poper->sOrgCode);
		return false;
	}

	S_MDB_ORG_INFO *accorg = m_pMemDb->m_pDbPublic.m_pOrg.Select(pacc->sSignOrg);
	if (accorg == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�ʽ��˺�[%s]�Ļ���[%s]�ڻ������޼�¼",acc,pacc->sSignOrg);
		return false;
	}
	if (strcmp(operorg->sUpOrg,"NULL")==0) //����Ա�Ƕ����������϶���Ȩ��
	{
		return true;
	}
	if (strcmp(accorg->sUpOrg,"NULL")==0) //�˻�Ϊ����������������϶�û��Ȩ��
	{
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"����Ա����[%s] ���˻�����[%s]��Ȩ�� UpOrg[%s]",operorg->sOrgCode,accorg->sOrgCode,accorg->sUpOrg);
		return false;
	}
	//�Ƚ��ϼ�����+��ǰ����������˻��Ļ������ڵ�������Ȩ��
	char soperorg[300];
	char scustorg[300];
	sprintf(soperorg,"%s|%s",operorg->sUpOrg,operorg->sOrgCode);
	sprintf(scustorg,"%s|%s",accorg->sUpOrg,accorg->sOrgCode);
	if (strncmp(soperorg,scustorg,strlen(soperorg)) == 0 )
	{
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"����Ա����[%s] ���˻�����[%s]��Ȩ�� ����ԱOrg[%s] �˻�Org[%s]",operorg->sOrgCode,accorg->sOrgCode,soperorg,scustorg);
		return true;
	}
	return false;
}
bool CKernelPub::CheckOperOrgRight(const char *oper, const char *org)
{
	
	S_MDB_OPER_INFO *poper = m_pMemDb->m_pDbPublic.m_pOper.Select(oper);
	if (poper == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"����Ա[%s]�ڲ���Ա���޼�¼",oper);
		return false;
	}

	S_MDB_ORG_INFO *operorg = m_pMemDb->m_pDbPublic.m_pOrg.Select(poper->sOrgCode);
	if (operorg == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"����Ա[%s]�Ļ���[%s]�ڻ������޼�¼",oper,poper->sOrgCode);
		return false;
	}
	S_MDB_ORG_INFO *orginfo = m_pMemDb->m_pDbPublic.m_pOrg.Select((char *)org);
	if (operorg == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"����[%s]�ڻ������޼�¼",org);
		return false;
	}
	if (strcmp(operorg->sUpOrg,"NULL")==0) //����Ա�Ƕ����������϶���Ȩ��
	{
		return true;
	}
	if (strcmp(orginfo->sUpOrg,"NULL")==0) //�˻�Ϊ����������������϶�û��Ȩ��
	{
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"����Ա����[%s] �Ի���[%s]��Ȩ�� UpOrg[%s]",operorg->sOrgCode,orginfo->sOrgCode,orginfo->sUpOrg);
		return false;
	}
	//�Ƚ��ϼ�����+��ǰ����������˻��Ļ������ڵ�������Ȩ��
	char soperorg[300];
	char scustorg[300];
	sprintf(soperorg,"%s|%s",operorg->sUpOrg,operorg->sOrgCode);
	sprintf(scustorg,"%s|%s",orginfo->sUpOrg,orginfo->sOrgCode);
	if (strncmp(soperorg,scustorg,strlen(soperorg)) == 0 )
	{
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"����Ա����[%s] ���˻�����[%s]��Ȩ�� ����ԱOrg[%s] ����Org[%s]",operorg->sOrgCode,orginfo->sOrgCode,soperorg,scustorg);
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
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"����Ա[%s]�ڲ���Ա���޼�¼",oper);
		return false;
	}
	S_MDB_FINA_PROD_INFO *pProd = m_pMemDb->m_pDbBaseinfo.m_pProd.Select((char *)prod);
	if (pProd == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"��Ʒ[%s]�ڲ�Ʒ���޼�¼",prod);
		return false;
	}
	S_MDB_ORG_INFO *operorg = m_pMemDb->m_pDbPublic.m_pOrg.Select(poper->sOrgCode);
	if (operorg == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"����Ա[%s]�Ļ���[%s]�ڻ������޼�¼",oper,poper->sOrgCode);
		return false;
	}
	if (strcmp(operorg->sUpOrg,"NULL")==0 || operorg->sOrgLevel[0] == '0')
	{
		//�����������϶���Ȩ��
		return true;
	}
	S_MDB_ORG_INFO *prodorg = m_pMemDb->m_pDbPublic.m_pOrg.Select(pProd->sOrgCode);
	if (prodorg == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"��Ʒ[%s]�Ļ���[%s]�ڻ������޼�¼",prod,pProd->sOrgCode);
		return false;
	}

	if (strcmp(prodorg->sUpOrg,"NULL")==0) //�˻�Ϊ����������������϶�û��Ȩ��
	{
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"����Ա����[%s] �Ի���[%s]��Ȩ�� UpOrg[%s]",operorg->sOrgCode,prodorg->sOrgCode,prodorg->sUpOrg);
		return false;
	}
	//�Ƚ��ϼ�����+��ǰ����������˻��Ļ������ڵ�������Ȩ��
	char soperorg[300];
	char scustorg[300];
	sprintf(soperorg,"%s|%s",operorg->sUpOrg,operorg->sOrgCode);
	sprintf(scustorg,"%s|%s",prodorg->sUpOrg,prodorg->sOrgCode);
	if (strncmp(soperorg,scustorg,strlen(soperorg)) == 0 )
	{
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"����Ա����[%s] �Ի���[%s]��Ȩ�� ����ԱOrg[%s] ��ƷOrg[%s]",operorg->sOrgCode,prodorg->sOrgCode,soperorg,scustorg);
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
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"��Ȩ��Լ[%d %s]�ı�ĺ�Լ[%d]������",pc->nContractId,pc->sContractCode,nProdId);
			return false;
		}
		nProdId = pcd->nProdId;//����Ϊ��Ȩ��Լ�ı�ĺ�Լ��Ӧ��Ʒ��
	}
	char sFeeNo[4];
	switch (flag)
	{
		case 1:
			strcpy(sFeeNo,"101");//���뿪��
			break;
		case 2:
			strcpy(sFeeNo,"102");//����ƽ��
			break;
		case 3:
			strcpy(sFeeNo,"103");//ƽ���
			break;
		default:
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"��������[%d]���벻��",flag);
			return false;
	}
	//��ȡ�������ķ���
	std::vector<S_MDB_FEE_TEMPLET_DETAIL *> exfeelist;
	if (!m_pMemDb->m_pDbSet.m_pFeeTempletDetail.Select(pcust->sExchFareModel,pc->sProd_type,nProdId,pc->nContractId,sFeeNo,exfeelist))
	{
		//����Լ�Ҳ����ٰ�Ʒ����
		if (!m_pMemDb->m_pDbSet.m_pFeeTempletDetail.Select(pcust->sExchFareModel,pc->sProd_type,nProdId,0,sFeeNo,exfeelist))
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"ȡ������������ ������ģ��[%s]��Լ[%d %s] Ʒ��[%s %d] �Ҳ�������",pcust->sExchFareModel,pc->nContractId,pc->sContractCode,pc->sProd_type,nProdId);
			return false;
		}
	}
	//if (exfeelist.size()>1)
	//{
	//	m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"������������ ������ģ��[%s]��Լ[%d %s] Ʒ��[%s %d] �ж������",pcust->sExchFareModel,pc->nContractId,pc->sContractCode,pc->sProd_type,nProdId);
	//	return false;
	//}
	int i;
	double dTmp;
	// #define  FEE_MODEL_DONERATE   '1'		        //�ɽ�������(����������)
	// #define  FEE_MODEL_WEIGHT     '2'		        //����(Ԫ/��)(����������)
	// #define  FEE_MODEL_HAND       '3'		        //����(Ԫ/��)(����������)
	// #define  FEE_MODEL_EXCH_RATE   '4'		        //�����������ѱ���(�ڽ����������ϳ��ԣ�����������)
	// #define  FEE_MODEL_EXCH_RATE_INCLUDE   '5'		//�����������ѱ���(�ڽ����������ϳ��ԣ�����������)
	//�����շ�ģʽ������������
	exfee= 0.0;
	for (i=0 ; i< exfeelist.size(); i++)
	{
		switch (exfeelist[0]->sFareMode[0])
		{
			case FEE_MODEL_DONERATE:
				dTmp = amt*exfeelist[0]->dValue;
				break;
			case FEE_MODEL_WEIGHT:
				dTmp =  exfeelist[0]->dValue*num*pc->dTxUnit;//����*��Լ������Ϊ�����������ϼ۸�Ϊ���
				break;
			case FEE_MODEL_HAND:
				dTmp =  exfeelist[0]->dValue*num;//ÿ�ֶ���
				break;
			default:
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"������������ ������ģ��[%s]��Լ[%d %s] Ʒ��[%d] ���շ�ģʽ[%s]����",pcust->sExchFareModel,pc->nContractId,pc->sContractCode,nProdId,exfeelist[0]->sFareMode);
				return false;
		}
		exfee += dTmp;
	}
	//ȡ��Ա��������
	std::vector<S_MDB_FEE_TEMPLET_DETAIL *> memberfeelist;
	std::vector<S_MDB_CUST_FEE *> reslist;

	bool issetmember=true;//�Ƿ������˻�Ա�������ʣ�δ��Ĭ��Ϊ0
	bool iscust=true;//�Ƿ�����

	//�ȴӿͻ�������ʱ�����ȡ
	if (!m_pMemDb->m_pDbSet.m_pCustFeeDetail.Select(pcust->sTxAcc,pc->sProd_type,nProdId,pc->nContractId,sFeeNo,reslist))
	{
		//����Լûȡ������Ʒ����ȡ������ԼΪ0
		if (!m_pMemDb->m_pDbSet.m_pCustFeeDetail.Select(pcust->sTxAcc,pc->sProd_type,nProdId,0,sFeeNo,reslist))
		{
			//û���ҵ����ӷ���ģ�����ȡ
			iscust = false;
			if (strlen(pcust->sFsFeeModel)>0) //���˾ɵ�������ģ��
			{
				if (!m_pMemDb->m_pDbSet.m_pFeeTempletDetail.Select(pcust->sFsFeeModel,pc->sProd_type,pc->nProdId,pc->nContractId,sFeeNo,memberfeelist))
				{
					//����Լ�Ҳ����ٰ�Ʒ����
					if (!m_pMemDb->m_pDbSet.m_pFeeTempletDetail.Select(pcust->sFsFeeModel,pc->sProd_type,pc->nProdId,0,sFeeNo,memberfeelist))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"ȡ��Ա������ fs����ģ��[%s]��Լ[%d %s] Ʒ��[%s %d] �Ҳ�������",pcust->sFsFeeModel,pc->nContractId,pc->sContractCode,pc->sProd_type,nProdId);
						issetmember = false;
						//return false;
					}
				}
			}
			else
			{
				if (!m_pMemDb->m_pDbSet.m_pFeeTempletDetail.Select(pcust->sMemberFareModel,pc->sProd_type,pc->nProdId,pc->nContractId,sFeeNo,memberfeelist))
				{
					//����Լ�Ҳ����ٰ�Ʒ����
					if (!m_pMemDb->m_pDbSet.m_pFeeTempletDetail.Select(pcust->sMemberFareModel,pc->sProd_type,pc->nProdId,0,sFeeNo,memberfeelist))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"ȡ��Ա������ ��Ա����ģ��[%s]��Լ[%d %s] Ʒ��[%s %d] �Ҳ�������",pcust->sMemberFareModel,pc->nContractId,pc->sContractCode,pc->sProd_type,nProdId);
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
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"δ���Ա�������ʣ�Ĭ�ϻ�Ա����");
		return true;
	}

	bool isinclude=false;
	if (iscust)
	{
		memberfee = 0.0;
		//������ʱ���һ���ֶα����Ƿ����������
		for (i=0 ; i< reslist.size(); i++)
		{
			if (reslist[i]->sInclude[0] == '1')
			{
				//��һ�������ģ�ͳһ�԰�����
				isinclude = true;
				break;
			}
		}
		//�����շ�ģʽ������������
		for (i=0 ; i< reslist.size(); i++)
		{
			switch (reslist[i]->sFareMode[0])
			{
			case FEE_MODEL_DONERATE:
				if (isinclude) //����������
				{
					if (reslist[i]->sInclude[0] == '1')
					{
						//������Ϊ����
						dTmp = amt*reslist[i]->dValue;
					}
					else //������Ϊ��������Ҫ���Ͻ�����
					{
						dTmp = exfee+ amt*reslist[i]->dValue;
					}
				}
				else //���������������õ�Ϊ׼
				{
					dTmp = amt*reslist[i]->dValue;
				}
				break;
			case FEE_MODEL_WEIGHT:
				if (isinclude) //����������
				{
					if (reslist[i]->sInclude[0] == '1')
					{
						//�����ð���
						dTmp = reslist[i]->dValue*num*pc->dTxUnit;//����*��Լ������Ϊ�����������ϼ۸�Ϊ���
					}
					else //������Ϊ��������Ҫ���Ͻ�����
					{
						dTmp = exfee+ reslist[i]->dValue*num*pc->dTxUnit;//����*��Լ������Ϊ�����������ϼ۸�Ϊ���
					}
				}
				else //���������������õ�Ϊ׼
				{
					dTmp =  reslist[i]->dValue*num*pc->dTxUnit;//����*��Լ������Ϊ�����������ϼ۸�Ϊ���
				}
				break;
			case FEE_MODEL_HAND:
				if (isinclude) //����������
				{
					if (reslist[i]->sInclude[0] == '1')
					{
						//������Ϊ����
						dTmp =  reslist[i]->dValue*num;//ÿ�ֶ���
					}
					else //������Ϊ��������Ҫ���Ͻ�����
					{
						dTmp = exfee + reslist[i]->dValue*num;//ÿ�ֶ���
					}
				}
				else //���������������õ�Ϊ׼
				{
					dTmp =  reslist[i]->dValue*num;//ÿ�ֶ���
				}
				break;
			case FEE_MODEL_EXCH_RATE:
				if (isinclude) //����������
				{
					if (reslist[i]->sInclude[0] == '1') //������Ϊ����
					{
						//���Ա�־���Դ�ֵΪ׼
						m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"���ó�����,�Ƿ������������־Ϊ1,��ֵΪ����������,��ֵΪ׼");
						dTmp = exfee*reslist[i]->dValue;
					}
					else //������Ϊ��������Ҫ���Ͻ�����
					{
						dTmp = exfee+ exfee*reslist[i]->dValue;//�����������ѱ���(�ڽ����������ϳ��ԣ�����������)
					}
				}
				else //���������������õ�Ϊ׼
				{
					dTmp =  exfee*reslist[i]->dValue;//�����������ѱ���(�ڽ����������ϳ��ԣ�����������)
				}

				break;
			case FEE_MODEL_EXCH_RATE_INCLUDE:
				if (isinclude) //����������
				{
					if (reslist[i]->sInclude[0] == '1')
					{
						//������Ϊ����
						dTmp =  exfee*reslist[i]->dValue;//�����������ѱ���(�ڽ����������ϳ��ԣ���������)
					}
					else //���� ������Ϊ��������Ӧ��Ҫ�����
					{
						//���Ա�־���Դ�ֵΪ׼
						m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"���ó�����,�Ƿ������������־Ϊ0,��ֵΪ��������,��ֵΪ׼");
						dTmp = exfee+ exfee*reslist[i]->dValue;//�����������ѱ���(�ڽ����������ϳ��ԣ���������)
					}
				}
				else //�����������õ�Ϊ������Ҫ��ȥ������
				{
					dTmp =  exfee*reslist[i]->dValue-exfee;//�����������ѱ���(�ڽ����������ϳ��ԣ���������)
				}

				break;
			default:
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"��Ա������ �ͻ������������ ��Լ[%d %s] Ʒ��[%s %d] ���շ�ģʽ[%s]����",pc->nContractId,pc->sContractCode,pc->sProd_type,nProdId,exfeelist[i]->sFareMode);
				return false;
			}
			memberfee = memberfee+dTmp;//����������
		}
		if (isinclude) //���˽�������
		{
			memberfee = memberfee - exfee;//������ǻ�Ա���յ�
		}
	}
	else
	{
		memberfee = 0.0;
		//�����շ�ģʽ������������
		for (i=0 ; i< memberfeelist.size(); i++)
		{
			switch (memberfeelist[i]->sFareMode[0])
			{
			case FEE_MODEL_DONERATE:
				dTmp = amt*memberfeelist[i]->dValue;
				break;
			case FEE_MODEL_WEIGHT:
				dTmp =  memberfeelist[i]->dValue*num*pc->dTxUnit;//����*��Լ������Ϊ�����������ϼ۸�Ϊ���
				break;
			case FEE_MODEL_HAND:
				dTmp =  memberfeelist[i]->dValue*num;//ÿ�ֶ���
				break;
			case FEE_MODEL_EXCH_RATE:
				dTmp =  exfee*memberfeelist[i]->dValue;//�����������ѱ���(�ڽ����������ϳ��ԣ�����������)
				break;
			case FEE_MODEL_EXCH_RATE_INCLUDE:
				dTmp =  exfee*memberfeelist[i]->dValue;//�����������ѱ���(�ڽ����������ϳ��ԣ���������)
				isinclude = true;
				break;
			default:
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"��Ա������ ģ��[%s] ��Լ[%d %s] Ʒ��[%d] ���շ�ģʽ[%s]����",pcust->sMemberFareModel,pc->nContractId,pc->sContractCode,nProdId,exfeelist[i]->sFareMode);
				return false;
			}
			memberfee = memberfee+dTmp;//����������
		}
		if (isinclude) //���˽�������
		{
			memberfee = memberfee - exfee;//������ǻ�Ա���յ�
		}
	}
	return true;
}

bool CKernelPub::GetMarginValue(S_MDB_CONTRACT_INFO *pc, S_MDB_CUST_FUNDACC_INFO *pcust, int flag, int num, double amt, double &exmargin, double &membermargin)
{
	// 	    �򿪣���Ȩ����
	// 		��ƽ����Ȩ����
	// 		��������Ȩ���𣬶��ᱣ֤��
	// 		��ƽ����Ȩ�����ͷű�֤��

	//��֤��Ϊ��Ա��֤��
	//��Ȩ������֤��  = max
	//(Ȩ����(�����)+��ĺ�Լ��֤��(�����)-��Ȩ��ֵ��(��ĺ�Լ��������Ȩִ�м۵ıȽ�)��һ��)
	//Ȩ����+��ĺ�Լ��֤��(�����)��һ��

	//��Ȩ��ֵ��ļ���   
	//����
	//MAX((��Ȩ��-��������)*��Լ����,0.0)
	//����
	//MAX((��������-��Ȩ��)*��Լ����,0.0)
	if (flag == 1) //��Ȩ
	{
		if (pc->sProd_type[0] != PROD_TYPE_OPTION)
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"����Ϊ������Ȩ��Լ��֤�𣬵���Լ[%d %s]������Ȩ��Լ",pc->nContractId,pc->sContractCode);
			return false;
		}
		S_MDB_CONTRACT_INFO *pdc = m_pMemDb->m_pDbBaseinfo.m_pContract.Select(pc->nProdId);
		if (pdc == NULL)
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"��Լ[%d %s]�ı�ĺ�Լ[%d]������",pc->nContractId,pc->sContractCode,pc->nProdId);
			return false;
		}
		//ȡ�ñ�ĺ�Լ�������
		S_MDB_CONTRACT_SETTLE_PRICE *psp = m_pMemDb->m_pDbBaseinfo.m_pSettlePrice.Select(pdc->nContractId);
		if (psp == NULL)
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"��Լ[%d %s]�ı�ĺ�Լ[%d %s]�������۲�����",pc->nContractId,pc->sContractCode,pdc->nContractId,pdc->sContractCode);
			return false;
		}
		//ȡ��ĺ�Լ�ı�֤��
		double dExMarginTmp;
		double dMemberMarginTmp;
		if (!GetMarginValue(pdc,pcust,0,num,psp->dSettlePrice*num*pdc->dTxUnit,dExMarginTmp,dMemberMarginTmp))
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"ȡ�� ��Լ[%d %s]�ı�ĺ�Լ[%d %s]�ı�֤�����",pc->nContractId,pc->sContractCode,pdc->nContractId,pdc->sContractCode);
			return false;
		}
		//���� ��Ȩ��ֵ��
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
		//���㱣֤��
		//ȡ�ú�Լ�������
		S_MDB_CONTRACT_SETTLE_PRICE *pcsp = m_pMemDb->m_pDbBaseinfo.m_pSettlePrice.Select(pc->nContractId);
		if (psp == NULL)
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"��Լ[%d %s]�����۲�����",pc->nContractId,pc->sContractCode);
			return false;
		}
		//ȡ����2�������ֵΪ��֤��ֵ
		//(Ȩ����(�����)+��ĺ�Լ��֤��(�����)-��Ȩ��ֵ��(��ĺ�Լ��������Ȩִ�м۵ıȽ�)��һ��)
		//Ȩ����+��ĺ�Լ��֤��(�����)��һ��
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
	//ȡ�ڻ���֤��

	char sFeeNo[4];
	strcpy(sFeeNo,"105");//���ױ�֤��

	//��ȡ�������ķ���
	std::vector<S_MDB_FEE_TEMPLET_DETAIL *> exfeelist;
	if (!m_pMemDb->m_pDbSet.m_pFeeTempletDetail.Select(pcust->sExchFareModel,pc->sProd_type,pc->nProdId,pc->nContractId,sFeeNo,exfeelist))
	{
		//����Լ�Ҳ����ٰ�Ʒ����
		if (!m_pMemDb->m_pDbSet.m_pFeeTempletDetail.Select(pcust->sExchFareModel,pc->sProd_type,pc->nProdId,0,sFeeNo,exfeelist))
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"ȡ��������֤�� ������ģ��[%s]��Լ[%d %s] Ʒ��[%s %d] �Ҳ�������",pcust->sExchFareModel,pc->nContractId,pc->sContractCode,pc->sProd_type,pc->nProdId);
			return false;
		}
	}
	if (exfeelist.size()>1)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"��������֤�� ������ģ��[%s]��Լ[%d %s] Ʒ��[%s %d] �ж������",pcust->sExchFareModel,pc->nContractId,pc->sContractCode,pc->sProd_type,pc->nProdId);
		return false;
	}
	// #define  FEE_MODEL_DONERATE   '1'		        //�ɽ�������(����������)
	// #define  FEE_MODEL_WEIGHT     '2'		        //����(Ԫ/��)(����������)
	// #define  FEE_MODEL_HAND       '3'		        //����(Ԫ/��)(����������)
	// #define  FEE_MODEL_EXCH_RATE   '4'		        //�����������ѱ���(�ڽ����������ϳ��ԣ�����������)
	// #define  FEE_MODEL_EXCH_RATE_INCLUDE   '5'		//�����������ѱ���(�ڽ����������ϳ��ԣ�����������)
	//�����շ�ģʽ�����㱣֤��
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
			dTmp =  exfeelist[0]->dValue*num*pc->dTxUnit;//����*��Լ������Ϊ�����������ϼ۸�Ϊ���
			break;
		case FEE_MODEL_HAND:
			dTmp =  exfeelist[0]->dValue*num;//ÿ�ֶ���
			break;
		default:
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"��������֤�� ������ģ��[%s]��Լ[%d %s] Ʒ��[%s %d] ���շ�ģʽ[%s]����",pcust->sExchFareModel,pc->nContractId,pc->sContractCode,pc->sProd_type,pc->nProdId,exfeelist[0]->sFareMode);
			return false;
		}
		exmargin += dTmp;
	}
	
	//ȡ��Ա�ı�֤��
	std::vector<S_MDB_FEE_TEMPLET_DETAIL *> memberfeelist;
	std::vector<S_MDB_CUST_FEE *> reslist;
	


	//��ȡ�ͻ������
	bool issetmember=true;//�Ƿ������˻�Ա�������ʣ�δ��Ĭ��Ϊ0
	bool iscust=true;//�Ƿ�����
	//�ȴӿͻ�������ʱ�����ȡ
	if (!m_pMemDb->m_pDbSet.m_pCustFeeDetail.Select(pcust->sTxAcc,pc->sProd_type,pc->nProdId,pc->nContractId,sFeeNo,reslist))
	{
		//����Լûȡ������Ʒ����ȡ������ԼΪ0
		if (!m_pMemDb->m_pDbSet.m_pCustFeeDetail.Select(pcust->sTxAcc,pc->sProd_type,pc->nProdId,0,sFeeNo,reslist))
		{
			//û���ҵ����ӷ���ģ�����ȡ
			iscust = false;
			if (!m_pMemDb->m_pDbSet.m_pFeeTempletDetail.Select(pcust->sMemberFareModel,pc->sProd_type,pc->nProdId,pc->nContractId,sFeeNo,memberfeelist))
			{
				//����Լ�Ҳ����ٰ�Ʒ����
				if (!m_pMemDb->m_pDbSet.m_pFeeTempletDetail.Select(pcust->sMemberFareModel,pc->sProd_type,pc->nProdId,0,sFeeNo,memberfeelist))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"ȡ��Ա��֤�� ��Աģ��[%s]��Լ[%d %s] Ʒ��[%s %d] �Ҳ�������",pcust->sMemberFareModel,pc->nContractId,pc->sContractCode,pc->sProd_type,pc->nProdId);
					issetmember=false;
					//return false;
				}
			}
		}
	}
	if (!issetmember)
	{
		membermargin= 0.0;
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"δ���Ա��֤���ʣ�Ĭ�ϻ�Ա����");
		return true;
	}	

	bool isinclude=false;
	membermargin = 0.0;
	if (iscust)
	{
		//������ʱ���һ���ֶα����Ƿ����������
		for (i=0 ; i< reslist.size(); i++)
		{
			if (reslist[i]->sInclude[0] == '1')
			{
				//��һ�������ģ�ͳһ�԰�����
				isinclude = true;
				break;
			}
		}
		//�����շ�ģʽ�����㱣֤��
		for (i=0 ; i< reslist.size(); i++)
		{
			switch (reslist[i]->sFareMode[0])
			{
				case FEE_MODEL_DONERATE:
					if (isinclude) //����������
					{
						if (reslist[i]->sInclude[0] == '1')
						{
							//������Ϊ����
							dTmp = amt*reslist[i]->dValue;
						}
						else //������Ϊ��������Ҫ���Ͻ�����
						{
							dTmp = exmargin+ amt*reslist[i]->dValue;
						}
					}
					else //���������������õ�Ϊ׼
					{
						dTmp = amt*reslist[i]->dValue;
					}
					break;
				case FEE_MODEL_WEIGHT:
					if (isinclude) //����������
					{
						if (reslist[i]->sInclude[0] == '1')
						{
							//������Ϊ����
							dTmp =  reslist[i]->dValue*num*pc->dTxUnit;//����*��Լ������Ϊ�����������ϼ۸�Ϊ���
						}
						else //������Ϊ��������Ҫ���Ͻ�����
						{
							dTmp = exmargin+ reslist[i]->dValue*num*pc->dTxUnit;//����*��Լ������Ϊ�����������ϼ۸�Ϊ���
						}
					}
					else //���������������õ�Ϊ׼
					{
						dTmp = reslist[i]->dValue*num*pc->dTxUnit;//����*��Լ������Ϊ�����������ϼ۸�Ϊ���
					}
					break;
				case FEE_MODEL_HAND:
					if (isinclude) //����������
					{
						if (reslist[i]->sInclude[0] == '1')
						{
							//������Ϊ����
							dTmp =  reslist[i]->dValue*num;//ÿ�ֶ���
						}
						else //������Ϊ��������Ҫ���Ͻ�����
						{
							dTmp = exmargin+reslist[i]->dValue*num;//ÿ�ֶ���
						}
					}
					else //���������������õ�Ϊ׼
					{
						dTmp =  reslist[i]->dValue*num;//ÿ�ֶ���
					}
					
					break;
				case FEE_MODEL_EXCH_RATE:
					if (isinclude) //����������
					{
						if (reslist[i]->sInclude[0] == '1') //������Ϊ����
						{
							//���Ա�־���Դ�ֵΪ׼
							m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"���ó�����,�Ƿ������������־Ϊ1,��ֵΪ����������,��ֵΪ׼");
							dTmp = exmargin*reslist[i]->dValue;
						}
						else //������Ϊ��������Ҫ���Ͻ�����
						{
							dTmp = exmargin+ exmargin*reslist[i]->dValue;//�����������ѱ���(�ڽ����������ϳ��ԣ�����������)
						}
					}
					else //���������������õ�Ϊ׼
					{
						dTmp =  exmargin*reslist[i]->dValue;//�����������ѱ���(�ڽ����������ϳ��ԣ�����������)
					}
					break;
				case FEE_MODEL_EXCH_RATE_INCLUDE:
					if (isinclude) //����������
					{
						if (reslist[i]->sInclude[0] == '1')
						{
							//������Ϊ����
							dTmp =  exmargin*reslist[i]->dValue;//�����������ѱ���(�ڽ����������ϳ��ԣ���������)
						}
						else //���� ������Ϊ��������Ӧ��Ҫ�����
						{
							//���Ա�־���Դ�ֵΪ׼
							m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"���ó�����,�Ƿ������������־Ϊ0,��ֵΪ��������,��ֵΪ׼");
							dTmp = exmargin+ exmargin*reslist[i]->dValue;//�����������ѱ���(�ڽ����������ϳ��ԣ���������)
						}
					}
					else //�����������õ�Ϊ������Ҫ��ȥ������
					{
						dTmp =  exmargin*reslist[i]->dValue-exmargin;//�����������ѱ���(�ڽ����������ϳ��ԣ���������)
					}
					break;
				default:
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"��Ա��֤�� �ͻ������������ ��Լ[%d %s] Ʒ��[%s %d] ���շ�ģʽ[%s]����",pc->nContractId,pc->sContractCode,pc->sProd_type,pc->nProdId,exfeelist[i]->sFareMode);
					return false;
			}
			membermargin = membermargin+dTmp;//����������
		}
		if (isinclude) //���˽�������
		{
			membermargin = membermargin - exmargin;//������ǻ�Ա���յ�
		}
	}
	else
	{
		//�����շ�ģʽ�����㱣֤��
		for (i=0 ; i< memberfeelist.size(); i++)
		{
			switch (memberfeelist[i]->sFareMode[0])
			{
				case FEE_MODEL_DONERATE:
					dTmp = amt*memberfeelist[i]->dValue;
					break;
				case FEE_MODEL_WEIGHT:
					dTmp =  memberfeelist[i]->dValue*num*pc->dTxUnit;//����*��Լ������Ϊ�����������ϼ۸�Ϊ���
					break;
				case FEE_MODEL_HAND:
					dTmp =  memberfeelist[i]->dValue*num;//ÿ�ֶ���
					break;
				case FEE_MODEL_EXCH_RATE:
					dTmp =  exmargin*memberfeelist[i]->dValue;//��������֤�����(�ڽ����������ϳ��ԣ�����������)
					break;
				case FEE_MODEL_EXCH_RATE_INCLUDE:
					dTmp =  exmargin*memberfeelist[i]->dValue;//��������֤�����(�ڽ����������ϳ��ԣ���������)
					isinclude = true;
					break;
				default:
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"��Ա��֤�� ģ��[%s] ��Լ[%d %s] Ʒ��[%s %d] ���շ�ģʽ[%s]����",pcust->sMemberFareModel,pc->nContractId,pc->sContractCode,pc->sProd_type,pc->nProdId,exfeelist[i]->sFareMode);
					return false;
			}
			membermargin = membermargin+dTmp;//����������
		}
		if (isinclude) //���˽�������
		{
			membermargin = membermargin - exmargin;//������ǻ�Ա���յ�
		}
	}

	

	return true;
}


bool CKernelPub::CheckRight(char *custorg, char *operorg)
{
	S_MDB_ORG_INFO *pcust = m_pMemDb->m_pDbPublic.m_pOrg.Select(custorg);
	if (pcust == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"����[%s]�ڻ������в�����",custorg);
		return false;
	}
	S_MDB_ORG_INFO *poper = m_pMemDb->m_pDbPublic.m_pOrg.Select(operorg);
	if (poper == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"����[%s]�ڻ������в�����",operorg);
		return false;
	}
	if (strcmp(poper->sUpOrg,"NULL")==0) //����ԱΪ�����������϶���Ȩ��
	{
		return true;
	}
	if (strcmp(pcust->sUpOrg,"NULL")==0) //�˻�Ϊ����������������϶�û��Ȩ��
	{
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"����Ա����[%s] ���˻�����[%s]��Ȩ�� UpOrg[%s]",operorg,custorg,poper->sUpOrg);
		return false;
	}
	//�Ƚ��ϼ�����+��ǰ����������˻��Ļ������ڵ�������Ȩ��
	char soperorg[300];
	char scustorg[300];
	sprintf(soperorg,"%s|%s",poper->sUpOrg,poper->sOrgCode);
	sprintf(scustorg,"%s|%s",pcust->sUpOrg,pcust->sOrgCode);
	if (strncmp(soperorg,scustorg,strlen(soperorg)) == 0 )
	{
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"����Ա����[%s] ���˻�����[%s]��Ȩ�� ����ԱOrg[%s] �˻�Org[%s]",operorg,custorg,soperorg,scustorg);
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
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"��Լ[%d]�ں�Լ���в�����",custposi->nContractNo);
		return -1;
	}
	switch (pCon->sProd_type[0] )
	{
	case PROD_TYPE_OPTION:
		//ȡ��ĺ�Լ��Ӧ��Ʒ��
		pConOpt = m_pMemDb->m_pDbBaseinfo.m_pContract.Select(pCon->nProdId);
		if (pConOpt == NULL)
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"��Լ[%d]�ں�Լ���в�����",pCon->nProdId);
			return -1;
		}
		if (!m_pMemDb->m_pDbSet.m_pPosiTempletDetail.Select(templet->sPosiTempletId,pCon->sProd_type,pConOpt->nProdId,pCon->nProdId,pCon->nContractId,positemplet))
		{
			//����Լû���ҵ�������ĺ�Լȥ��
			if (!m_pMemDb->m_pDbSet.m_pPosiTempletDetail.Select(templet->sPosiTempletId,pCon->sProd_type,pConOpt->nProdId,pCon->nProdId,0,positemplet))
			{
				//����ĺ�Լû���ҵ�����Ʒ��ȥ��
				if (!m_pMemDb->m_pDbSet.m_pPosiTempletDetail.Select(templet->sPosiTempletId,pCon->sProd_type,pConOpt->nProdId,0,0,positemplet))
				{
					m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�ֲ�ģ��[%s]��Լ[%d %s]��ĺ�Լ[%d %s]����Ӧ���ڻ�Ʒ��[%d]��ģ�����δ����",templet->sPosiTempletId,custposi->nContractNo,custposi->sContractCode,pCon->nProdId,pCon->sProd_code,pConOpt->nProdId);
					return -1;
				}
			}
		}

		break;
	case PROD_TYPE_FUTURES:
		if (!m_pMemDb->m_pDbSet.m_pPosiTempletDetail.Select(templet->sPosiTempletId,pCon->sProd_type,pCon->nProdId,0,pCon->nContractId,positemplet))
		{
			//����Լû���ҵ�����Ʒ��ȥ��
			if (!m_pMemDb->m_pDbSet.m_pPosiTempletDetail.Select(templet->sPosiTempletId,pCon->sProd_type,pCon->nProdId,0,0,positemplet))
			{
				m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�ֲ�ģ��[%s]��Լ[%d]��Ʒ��[%d]��ģ�����δ����",templet->sPosiTempletId,custposi->nContractNo,pCon->nProdId);
				return -1;
			}
		}
		break;
	case PROD_TYPE_SEC:
		if (!m_pMemDb->m_pDbSet.m_pPosiTempletDetail.Select(templet->sPosiTempletId,pCon->sProd_type,0,0,pCon->nContractId,positemplet))
		{
			m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"�ֲ�ģ��[%s]��Լ[%d]��Ʒ��[%d]��ģ�����δ����",templet->sPosiTempletId,custposi->nContractNo,pCon->nProdId);
			return -1;
		}
		break;
	case PROD_TYPE_SPOT:
		if (!m_pMemDb->m_pDbSet.m_pPosiTempletDetail.Select(templet->sPosiTempletId,pCon->sProd_type,pCon->nProdId,0,pCon->nContractId,positemplet))
		{
			//����Լû���ҵ�����Ʒ��ȥ��
			if (!m_pMemDb->m_pDbSet.m_pPosiTempletDetail.Select(templet->sPosiTempletId,pCon->sProd_type,pCon->nProdId,0,0,positemplet))
			{
				m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"�ֲ�ģ��[%s]��Լ[%d]��Ʒ��[%d]��ģ�����δ����",templet->sPosiTempletId,custposi->nContractNo,pCon->nProdId);
				return -1;
			}
		}
		break;
	default:
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"��Լ[%d]��Ʒ������[%s]����",custposi->nContractNo,pCon->sProd_type);
		return -1;

	}
	//��ģ����ȡ��Ԥ�������С��

	S_MDB_CUST_FUND *fund = m_pMemDb->GetFund(acclock,pCon->sCurrency);
	if (fund == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�ʽ��˺�[%s] ����[%s]���ʽ������",acclock->sTxAcc,pCon->sCurrency);
		return -1;
	}
	if (fund->dBalance > MINDOUBLECOMPARE && fund->dBalance<MAXDOUBLECOMPARE)
	{
		//���Ȩ��Ϊ0
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�ʽ��˺�[%s] ���Ϊ0 [%.2lf]",acclock->sTxAcc,fund->dBalance);
		return -1;
	}
	S_MDB_QUOTATION *quo = m_pMemDb->m_pDbTrader.m_pQuotation.Select(custposi->nContractNo);
	if (quo == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"��Լ[%d] ���������û�����¼�",custposi->nContractNo);
		return -1;
	}
	//��֤��ռ��
	double dMarginRate = (custposi->dPB_Margin+custposi->dPS_Margin)/fund->dBalance;
	//�ֲּ�ֵ��
	double dPosiValueRate = quo->dNewPrice*pCon->dTxUnit*(custposi->nCurPB+custposi->nCurPS)/fund->dBalance;
	//�ֲ������ֵ��
	double dPosiDiffValueRate = quo->dNewPrice*pCon->dTxUnit*(custposi->nCurPB-custposi->nCurPS)/fund->dBalance;
	for (j=0; j<positemplet.size(); j++)
	{
		switch (positemplet[j]->sPosiModel[0])
		{
		case PM_MARGINRATE://��֤��ռ��
			if (positemplet[j]->sCondition[0] == EF_LARGEEQUAL)
			{
				if (dMarginRate >= positemplet[j]->dValue)
				{
					strcpy(traderight,positemplet[j]->sRight);
					sprintf(warnmsg,"��Լ[%d %s]��֤��ռ�ȴ��ڵ���[%f]",custposi->nContractNo,custposi->sContractCode,positemplet[j]->dValue);
					return positemplet[j]->nWarnNo;
				}
			}
			else if (positemplet[j]->sCondition[0] == EF_LARGE)
			{
				if (dMarginRate > positemplet[j]->dValue)
				{
					strcpy(traderight,positemplet[j]->sRight);
					sprintf(warnmsg,"��Լ[%d %s]��֤��ռ�ȴ���[%f]",custposi->nContractNo,custposi->sContractCode,positemplet[j]->dValue);
					return positemplet[j]->nWarnNo;
				}
			}
			else if (positemplet[j]->sCondition[0] == EF_EQUAL)
			{
				if ( (dMarginRate - positemplet[j]->dValue) > MINDOUBLECOMPARE && (dMarginRate - positemplet[j]->dValue) < MAXDOUBLECOMPARE )
				{
					strcpy(traderight,positemplet[j]->sRight);
					sprintf(warnmsg,"��Լ[%d %s]��֤��ռ�ȵ���[%f]",custposi->nContractNo,custposi->sContractCode,positemplet[j]->dValue);
					return positemplet[j]->nWarnNo;
				}
			}
			else //EF_SMALLEQUAL С�ڵ�
			{
				if (dMarginRate <= positemplet[j]->dValue)
				{
					strcpy(traderight,positemplet[j]->sRight);
					sprintf(warnmsg,"��Լ[%d %s]��֤��ռ��С�ڵ���[%f]",custposi->nContractNo,custposi->sContractCode,positemplet[j]->dValue);
					return positemplet[j]->nWarnNo;
				}
			}

			break;
		case PM_POSIVALUE_RATE://�ֲּ�ֵ��
			if (positemplet[j]->sCondition[0] == EF_LARGEEQUAL)
			{
				if (dPosiValueRate >= positemplet[j]->dValue)
				{
					strcpy(traderight,positemplet[j]->sRight);
					sprintf(warnmsg,"��Լ[%d %s]�ֲּ�ֵ�ȴ��ڵ���[%f]",custposi->nContractNo,custposi->sContractCode,positemplet[j]->dValue);
					return positemplet[j]->nWarnNo;
				}
			}
			else if (positemplet[j]->sCondition[0] == EF_LARGE)
			{
				if (dPosiValueRate > positemplet[j]->dValue)
				{
					strcpy(traderight,positemplet[j]->sRight);
					sprintf(warnmsg,"��Լ[%d %s]�ֲּ�ֵ�ȴ���[%f]",custposi->nContractNo,custposi->sContractCode,positemplet[j]->dValue);
					return positemplet[j]->nWarnNo;
				}
			}
			else if (positemplet[j]->sCondition[0] == EF_EQUAL)
			{
				if ( (dPosiValueRate - positemplet[j]->dValue) > MINDOUBLECOMPARE && (dPosiValueRate - positemplet[j]->dValue) < MAXDOUBLECOMPARE )
				{
					strcpy(traderight,positemplet[j]->sRight);
					sprintf(warnmsg,"��Լ[%d %s]�ֲּ�ֵ�ȵ���[%f]",custposi->nContractNo,custposi->sContractCode,positemplet[j]->dValue);
					return positemplet[j]->nWarnNo;
				}
			}
			else //EF_SMALLEQUAL С�ڵ�
			{
				if (dPosiValueRate <= positemplet[j]->dValue)
				{
					strcpy(traderight,positemplet[j]->sRight);
					sprintf(warnmsg,"��Լ[%d %s]�ֲּ�ֵ��С�ڵ���[%f]",custposi->nContractNo,custposi->sContractCode,positemplet[j]->dValue);
					return positemplet[j]->nWarnNo;
				}
			}
			break;
		case PM_POSI_OFFSET_RATE://�ֲ������ֵ��
			if (positemplet[j]->sCondition[0] == EF_LARGEEQUAL)
			{
				if (dPosiDiffValueRate >= positemplet[j]->dValue)
				{
					strcpy(traderight,positemplet[j]->sRight);
					sprintf(warnmsg,"��Լ[%d %s]�ֲ������ֵ�ȴ��ڵ���[%f]",custposi->nContractNo,custposi->sContractCode,positemplet[j]->dValue);
					return positemplet[j]->nWarnNo;
				}
			}
			else if (positemplet[j]->sCondition[0] == EF_LARGE)
			{
				if (dPosiDiffValueRate > positemplet[j]->dValue)
				{
					strcpy(traderight,positemplet[j]->sRight);
					sprintf(warnmsg,"��Լ[%d %s]�ֲ������ֵ�ȴ���[%f]",custposi->nContractNo,custposi->sContractCode,positemplet[j]->dValue);
					return positemplet[j]->nWarnNo;
				}
			}
			else if (positemplet[j]->sCondition[0] == EF_EQUAL)
			{
				if ( (dPosiDiffValueRate - positemplet[j]->dValue) > MINDOUBLECOMPARE && (dPosiDiffValueRate - positemplet[j]->dValue) < MAXDOUBLECOMPARE )
				{
					strcpy(traderight,positemplet[j]->sRight);
					sprintf(warnmsg,"��Լ[%d %s]�ֲ������ֵ�ȵ���[%f]",custposi->nContractNo,custposi->sContractCode,positemplet[j]->dValue);
					return positemplet[j]->nWarnNo;
				}
			}
			else //EF_SMALLEQUAL С�ڵ�
			{
				if (dPosiDiffValueRate <= positemplet[j]->dValue)
				{
					strcpy(traderight,positemplet[j]->sRight);
					sprintf(warnmsg,"��Լ[%d %s]�ֲ������ֵ��С�ڵ���[%f]",custposi->nContractNo,custposi->sContractCode,positemplet[j]->dValue);
					return positemplet[j]->nWarnNo;
				}
			}
			break;
		default:
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�ֲ�ָ��[%s]����",positemplet[j]->sPosiModel);
			return -1;

		}
	}
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"GetPosiRiskLevelȡ�ֲַ�����ɣ��޳ֲַ���");
	strcpy(warnmsg,"��ƥ��������ã��޷���");
	return 10;
}
int CKernelPub::GetRiskLevel(S_MDB_CUST_FUND *fund,char *traderight)
{
	//�����ʽ��˺��Լ���ģ�壬���2�������ˣ���2��������Ԥ����ţ��õ�һ����С�ģ���������ߵģ����ֻ����һ����
	//�Ͱ���1�����ҡ�
	//����ʽ��˺�û�ж�Ӧģ�壬��ôȡ$�ģ��ظ�����
	traderight[0] = TR_NORMAL;//Ĭ��Ϊ����

	bool isposition=false;
	//��ѯ�Ƿ��гֲ�
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

	//ȡ�ʽ��˺Ŷ�Ӧ��ģ��
	S_MDB_CUST_TEMPLET_RELA *templet = m_pMemDb->m_pDbSet.m_pCustTemplet.Select(fund->sTxAcc);
	if (templet == NULL)
	{
		//û���ҵ��ʽ��˺Ŷ�Ӧ��ģ��,ȡ$��Ӧ��ģ��
		templet = m_pMemDb->m_pDbSet.m_pCustTemplet.Select("$");
		if (templet == NULL)
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"GetRiskLevel error  $ not found ");
			return -1;
		}
	}

	//����ģ��ID�����ʽ�ģ�����ñ����
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
		if (warnlist[i]->sFundModel[0] == FMM_RISK) //ָ��Ϊ���ն�
		{
			if (warnlist[i]->sCondition[0] == EF_LARGEEQUAL)
			{
				if (fund->dRisk > warnlist[i]->dValue -MAXDOUBLECOMPARE)//���ڵ���
				{
					fund->nRisk_level= warnlist[i]->nWarnNo;
					traderight[0] = warnlist[i]->sRight[0];
					sprintf(fund->sRiskDesc,"���ն�[%f]���ڵ��� %f",fund->dRisk,warnlist[i]->dValue);
					break;
				}
			}
			else if (warnlist[i]->sCondition[0] == EF_LARGE)
			{
				if (fund->dRisk > warnlist[i]->dValue )//����
				{
					fund->nRisk_level= warnlist[i]->nWarnNo;
					traderight[0] = warnlist[i]->sRight[0];
					sprintf(fund->sRiskDesc,"���ն�[%f]���� %f",fund->dRisk,warnlist[i]->dValue);
					break;
				}
			}
			else
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"templetid %s condition %s error ",templet->sFundTempletId,warnlist[i]->sCondition);
				return 10;
			}

		}
		else //Ȩ��
		{
			if (warnlist[i]->sCondition[0] == EF_SMALLEQUAL)
			{
				if (fund->dBalance < warnlist[i]->dValue+MAXDOUBLECOMPARE)//С�ڵ���
				{
					if (warnlist[i]->nWarnNo <3 ) //������Ϊǿƽ��ʱҪ�ж��Ƿ��гֲ�
					{
						if (isposition) //�гֲ�
						{
							fund->nRisk_level= warnlist[i]->nWarnNo;
							traderight[0] = warnlist[i]->sRight[0];
							sprintf(fund->sRiskDesc,"Ȩ��[%f]С�ڵ��� %f",fund->dBalance,warnlist[i]->dValue);
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
						sprintf(fund->sRiskDesc,"Ȩ��[%f]С�ڵ��� %f",fund->dBalance,warnlist[i]->dValue);
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
	traderight[0] = TR_NORMAL;//Ĭ��Ϊ����
	int nRiskLevel=10;

	S_MDB_PRODORG_TEMPLET_RELA *pProdTemplet = m_pMemDb->m_pDbSet.m_pProdTemplet.Select(prodid);
	if (pProdTemplet == NULL)
	{
		pProdTemplet = m_pMemDb->m_pDbSet.m_pProdTemplet.Select("$");
		if (pProdTemplet == NULL)
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"��Ʒ[%s]�޶�Ӧģ��",prodid);
			return 10;
		}
	}

	std::vector<S_MDB_PROD_MONITOR *> reslist;
	if (!m_pMemDb->m_pDbSet.m_pProdFundMonitor.Select(pProdTemplet->sFundTempletId,reslist))
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"��Ʒ�ʽ���δ����");
		return 10;
	}

	for (int i=0 ; i<reslist.size() ; i++)
	{
		if (reslist[i]->sModel[0] == FMM_RISK) //ָ��Ϊ���ն�
		{
			if (reslist[i]->sCondition[0] == EF_LARGEEQUAL)
			{
				if (risk > reslist[i]->dValue-MAXDOUBLECOMPARE)//���ڵ���
				{
					nRiskLevel= reslist[i]->nWarnNo;
					traderight[0] = reslist[i]->sRight[0];
					sprintf(warnmsg,"���ն�[%d]���ڵ���[%d]",risk,reslist[i]->dValue);
					break;
				}
			}
			else if (reslist[i]->sCondition[0] == EF_LARGE)
			{
				if (risk > reslist[i]->dValue)//����
				{
					nRiskLevel= reslist[i]->nWarnNo;
					traderight[0] = reslist[i]->sRight[0];
					sprintf(warnmsg,"���ն�[%d]����[%d]",risk,reslist[i]->dValue);
					break;
				}
			}
			else
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"condition %s error ",reslist[i]->sCondition);
				return 10;
			}

		}
		else //��λ��ֵ
		{
			if (reslist[i]->sCondition[0] == EF_SMALLEQUAL)
			{
				if (unitvalue < reslist[i]->dValue+MAXDOUBLECOMPARE)//С�ڵ���
				{
					nRiskLevel= reslist[i]->nWarnNo;
					traderight[0] = reslist[i]->sRight[0];
					sprintf(warnmsg,"��λ��ֵ[%d]С�ڵ���[%d]",unitvalue,reslist[i]->dValue);
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
	traderight[0] = TR_NORMAL;//Ĭ��Ϊ����
	int nRiskLevel=10;
	S_MDB_PRODORG_TEMPLET_RELA *pProdTemplet = m_pMemDb->m_pDbSet.m_pProdTemplet.Select(prodid);
	if (pProdTemplet == NULL)
	{
		pProdTemplet = m_pMemDb->m_pDbSet.m_pProdTemplet.Select("$");
		if (pProdTemplet == NULL)
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"��Ʒ[%s]�޶�Ӧģ��",prodid);
			return 10;
		}
	}
	std::vector<S_MDB_PROD_MONITOR *> reslist;
	if (!m_pMemDb->m_pDbSet.m_pProdPosiMonitor.Select(pProdTemplet->sPosiTempletId,reslist))
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"��Ʒ�ֲּ��δ����");
		return 10;
	}
	for (int i=0 ; i<reslist.size() ; i++)
	{
		if (reslist[i]->sModel[0] == PM_MARGINRATE) //��֤��ռ��
		{
			if (reslist[i]->sCondition[0] == EF_LARGEEQUAL)
			{
				if (marginrate > reslist[i]->dValue-MAXDOUBLECOMPARE)//���ڵ���
				{
					nRiskLevel= reslist[i]->nWarnNo;
					traderight[0] = reslist[i]->sRight[0];
					sprintf(warnmsg,"��֤��ռ��[%d]���ڵ���[%d]",marginrate,reslist[i]->dValue);
					break;
				}
			}
			else if (reslist[i]->sCondition[0] == EF_LARGE)
			{
				if (marginrate > reslist[i]->dValue)//����
				{
					nRiskLevel= reslist[i]->nWarnNo;
					traderight[0] = reslist[i]->sRight[0];
					sprintf(warnmsg,"��֤��ռ��[%d]����[%d]",marginrate,reslist[i]->dValue);
					break;
				}
			}
			else if (reslist[i]->sCondition[0] == EF_SMALLEQUAL)
			{
				if (marginrate < reslist[i]->dValue+MAXDOUBLECOMPARE)//С�ڵ���
				{
					nRiskLevel= reslist[i]->nWarnNo;
					traderight[0] = reslist[i]->sRight[0];
					sprintf(warnmsg,"��֤��ռ��[%d]С�ڵ���[%d]",posivalue,reslist[i]->dValue);
					break;
				}
			}
			else
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"condition %s error ",reslist[i]->sCondition);
				return 10;
			}

		}
		else if (reslist[i]->sModel[0] == PM_POSIVALUE_RATE) //�ֲּ�ֵ��
		{
			if (reslist[i]->sCondition[0] == EF_LARGEEQUAL)
			{
				if (posivalue > reslist[i]->dValue-MAXDOUBLECOMPARE)//���ڵ���
				{
					nRiskLevel= reslist[i]->nWarnNo;
					traderight[0] = reslist[i]->sRight[0];
					sprintf(warnmsg,"�ֲּ�ֵ��[%d]���ڵ���[%d]",posivalue,reslist[i]->dValue);
					break;
				}
			}
			else if (reslist[i]->sCondition[0] == EF_LARGE)
			{
				if (posivalue > reslist[i]->dValue)//����
				{
					nRiskLevel= reslist[i]->nWarnNo;
					traderight[0] = reslist[i]->sRight[0];
					sprintf(warnmsg,"�ֲּ�ֵ��[%d]����[%d]",posivalue,reslist[i]->dValue);
					break;
				}
			}
			else if (reslist[i]->sCondition[0] == EF_SMALLEQUAL)
			{
				if (posivalue < reslist[i]->dValue+MAXDOUBLECOMPARE)//С�ڵ���
				{
					nRiskLevel= reslist[i]->nWarnNo;
					traderight[0] = reslist[i]->sRight[0];
					sprintf(warnmsg,"�ֲּ�ֵ��[%d]С�ڵ���[%d]",posivalue,reslist[i]->dValue);
					break;
				}
			}
			else
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__," condition %s error ",reslist[i]->sCondition);
				return 10;
			}
		}
		else //�ֲ������ֵ��
		{
			if (reslist[i]->sCondition[0] == EF_LARGEEQUAL)
			{
				if (posidiffvalue > reslist[i]->dValue-MAXDOUBLECOMPARE)//���ڵ���
				{
					nRiskLevel= reslist[i]->nWarnNo;
					traderight[0] = reslist[i]->sRight[0];
					sprintf(warnmsg,"�ֲ������ֵ��[%d]���ڵ���[%d]",posidiffvalue,reslist[i]->dValue);
					break;
				}
			}
			else if (reslist[i]->sCondition[0] == EF_LARGE)
			{
				if (posidiffvalue > reslist[i]->dValue)//����
				{
					nRiskLevel= reslist[i]->nWarnNo;
					traderight[0] = reslist[i]->sRight[0];
					sprintf(warnmsg,"�ֲ������ֵ��[%d]����[%d]",posidiffvalue,reslist[i]->dValue);
					break;
				}
			}
			else if (reslist[i]->sCondition[0] == EF_SMALLEQUAL)
			{
				if (posidiffvalue < reslist[i]->dValue+MAXDOUBLECOMPARE)//С�ڵ���
				{
					nRiskLevel= reslist[i]->nWarnNo;
					traderight[0] = reslist[i]->sRight[0];
					sprintf(warnmsg,"�ֲ������ֵ��[%d]С�ڵ���[%d]",posidiffvalue,reslist[i]->dValue);
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
	traderight[0] = TR_NORMAL;//Ĭ��Ϊ����
	int nRiskLevel=10;

	S_MDB_PRODORG_TEMPLET_RELA *pOrgTemplet = m_pMemDb->m_pDbSet.m_pOrgTemplet.Select(orgcode);
	if (pOrgTemplet == NULL)
	{
		pOrgTemplet = m_pMemDb->m_pDbSet.m_pOrgTemplet.Select("$");
		if (pOrgTemplet == NULL)
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"����[%s]�޶�Ӧģ��",orgcode);
			return 10;
		}
	}

	std::vector<S_MDB_PROD_MONITOR *> reslist;
	if (!m_pMemDb->m_pDbSet.m_pProdFundMonitor.Select(pOrgTemplet->sFundTempletId,reslist))
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"��Ʒ�ʽ���δ����");
		return 10;
	}

	for (int i=0 ; i<reslist.size() ; i++)
	{
		if (reslist[i]->sModel[0] == FMM_RISK) //ָ��Ϊ���ն�
		{
			if (reslist[i]->sCondition[0] == EF_LARGEEQUAL)
			{
				if (risk > reslist[i]->dValue-MAXDOUBLECOMPARE)//���ڵ���
				{
					nRiskLevel= reslist[i]->nWarnNo;
					traderight[0] = reslist[i]->sRight[0];
					sprintf(warnmsg,"���ն�[%d]���ڵ���[%d]",risk,reslist[i]->dValue);
					break;
				}
			}
			else if (reslist[i]->sCondition[0] == EF_LARGE)
			{
				if (risk > reslist[i]->dValue)//����
				{
					nRiskLevel= reslist[i]->nWarnNo;
					traderight[0] = reslist[i]->sRight[0];
					sprintf(warnmsg,"���ն�[%d]����[%d]",risk,reslist[i]->dValue);
					break;
				}
			}
			else
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"condition %s error ",reslist[i]->sCondition);
				return 10;
			}

		}
		else //��λ��ֵ
		{
			if (reslist[i]->sCondition[0] == EF_SMALLEQUAL)
			{
				if (unitvalue < reslist[i]->dValue+MAXDOUBLECOMPARE)//С�ڵ���
				{
					nRiskLevel= reslist[i]->nWarnNo;
					traderight[0] = reslist[i]->sRight[0];
					sprintf(warnmsg,"��λ��ֵ[%d]С�ڵ���[%d]",unitvalue,reslist[i]->dValue);
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
	traderight[0] = TR_NORMAL;//Ĭ��Ϊ����
	int nRiskLevel=10;
	S_MDB_PRODORG_TEMPLET_RELA *pOrgTemplet = m_pMemDb->m_pDbSet.m_pOrgTemplet.Select(orgcode);
	if (pOrgTemplet == NULL)
	{
		pOrgTemplet = m_pMemDb->m_pDbSet.m_pOrgTemplet.Select("$");
		if (pOrgTemplet == NULL)
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"����[%s]�޶�Ӧģ��",orgcode);
			return 10;
		}
	}
	std::vector<S_MDB_PROD_MONITOR *> reslist;
	if (!m_pMemDb->m_pDbSet.m_pProdPosiMonitor.Select(pOrgTemplet->sPosiTempletId,reslist))
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"��Ʒ�ֲּ��δ����");
		return 10;
	}
	for (int i=0 ; i<reslist.size() ; i++)
	{
		if (reslist[i]->sModel[0] == PM_MARGINRATE) //��֤��ռ��
		{
			if (reslist[i]->sCondition[0] == EF_LARGEEQUAL)
			{
				if (marginrate > reslist[i]->dValue-MAXDOUBLECOMPARE)//���ڵ���
				{
					nRiskLevel= reslist[i]->nWarnNo;
					traderight[0] = reslist[i]->sRight[0];
					sprintf(warnmsg,"��֤��ռ��[%d]���ڵ���[%d]",marginrate,reslist[i]->dValue);
					break;
				}
			}
			else if (reslist[i]->sCondition[0] == EF_LARGE)
			{
				if (marginrate > reslist[i]->dValue)//����
				{
					nRiskLevel= reslist[i]->nWarnNo;
					traderight[0] = reslist[i]->sRight[0];
					sprintf(warnmsg,"��֤��ռ��[%d]����[%d]",marginrate,reslist[i]->dValue);
					break;
				}
			}
			else if (reslist[i]->sCondition[0] == EF_SMALLEQUAL)
			{
				if (marginrate < reslist[i]->dValue+MAXDOUBLECOMPARE)//С�ڵ���
				{
					nRiskLevel= reslist[i]->nWarnNo;
					traderight[0] = reslist[i]->sRight[0];
					sprintf(warnmsg,"��֤��ռ��[%d]С�ڵ���[%d]",posivalue,reslist[i]->dValue);
					break;
				}
			}
			else
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"condition %s error ",reslist[i]->sCondition);
				return 10;
			}

		}
		else if (reslist[i]->sModel[0] == PM_POSIVALUE_RATE) //�ֲּ�ֵ��
		{
			if (reslist[i]->sCondition[0] == EF_LARGEEQUAL)
			{
				if (posivalue > reslist[i]->dValue-MAXDOUBLECOMPARE)//���ڵ���
				{
					nRiskLevel= reslist[i]->nWarnNo;
					traderight[0] = reslist[i]->sRight[0];
					sprintf(warnmsg,"�ֲּ�ֵ��[%d]���ڵ���[%d]",posivalue,reslist[i]->dValue);
					break;
				}
			}
			else if (reslist[i]->sCondition[0] == EF_LARGE)
			{
				if (posivalue > reslist[i]->dValue)//����
				{
					nRiskLevel= reslist[i]->nWarnNo;
					traderight[0] = reslist[i]->sRight[0];
					sprintf(warnmsg,"�ֲּ�ֵ��[%d]����[%d]",posivalue,reslist[i]->dValue);
					break;
				}
			}
			else if (reslist[i]->sCondition[0] == EF_SMALLEQUAL)
			{
				if (posivalue < reslist[i]->dValue+MAXDOUBLECOMPARE)//С�ڵ���
				{
					nRiskLevel= reslist[i]->nWarnNo;
					traderight[0] = reslist[i]->sRight[0];
					sprintf(warnmsg,"�ֲּ�ֵ��[%d]С�ڵ���[%d]",posivalue,reslist[i]->dValue);
					break;
				}
			}
			else
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__," condition %s error ",reslist[i]->sCondition);
				return 10;
			}
		}
		else //�ֲ������ֵ��
		{
			if (reslist[i]->sCondition[0] == EF_LARGEEQUAL)
			{
				if (posidiffvalue > reslist[i]->dValue-MAXDOUBLECOMPARE)//���ڵ���
				{
					nRiskLevel= reslist[i]->nWarnNo;
					traderight[0] = reslist[i]->sRight[0];
					sprintf(warnmsg,"�ֲ������ֵ��[%d]���ڵ���[%d]",posidiffvalue,reslist[i]->dValue);
					break;
				}
			}
			else if (reslist[i]->sCondition[0] == EF_LARGE)
			{
				if (posidiffvalue > reslist[i]->dValue)//����
				{
					nRiskLevel= reslist[i]->nWarnNo;
					traderight[0] = reslist[i]->sRight[0];
					sprintf(warnmsg,"�ֲ������ֵ��[%d]����[%d]",posidiffvalue,reslist[i]->dValue);
					break;
				}
			}
			else if (reslist[i]->sCondition[0] == EF_SMALLEQUAL)
			{
				if (posidiffvalue < reslist[i]->dValue+MAXDOUBLECOMPARE)//С�ڵ���
				{
					nRiskLevel= reslist[i]->nWarnNo;
					traderight[0] = reslist[i]->sRight[0];
					sprintf(warnmsg,"�ֲ������ֵ��[%d]С�ڵ���[%d]",posidiffvalue,reslist[i]->dValue);
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
		//�����û�У��������ȡ
		S_MDB_QUOTATION *pq = m_pMemDb->m_pDbTrader.m_pQuotation.Select(contractid);
		if (pq == NULL)
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"��Լ���[%d]û�н������������",contractid);
			return false;
		}
		if (pq->dLastSetllePrice <MAXDOUBLECOMPARE || pq->dLastSetllePrice>99999999)
		{
			//�����ս����  ȡ���¼�
			settleprice = pq->dNewPrice;
			m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"��Լ���[%d]û�н��������������Ҳ�޽����,ȡ���¼�",contractid);
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
	
	if (pd->sEoFlag[0] == EO_FLAG_OPEN) //����
	{
		if (pd->sBsFlag[0] == BS_FLAG_BUY)  //�����
		{
			//���ұ�
			pbm = m_pMemDb->m_pDbTrader.m_pBigNetMargin.Select(pd->sTxAcc,pc->nProdId);
			if (pbm == NULL)
			{
				//�ɽ����Ҳ�����¼
				pbm = AddBigMarginNull(pd->sTxAcc,pc->nProdId);
				if (pbm == NULL)
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"���Ӿ��߼�¼����[%s %d]��",pd->sTxAcc,pc->nProdId);
					return -1;
				}
			}
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"����� �ʽ��˺�[%s]Ʒ��[%d] ���߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
			if (!bisinit) //���ǳ�ʼ��
			{
				//�ҵ�ԭ��������ʽ�
				if (pbm->nFrozeFlag == 1)
				{
					//��ԭ�����ͷ�
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,0.00-pbm->dFrozeAmt,false))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
						return -1;
					}
				}
				else
				{
					//��ԭ�����ͷ�
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,0.00-pbm->dFrozeAmt,false))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
						return -1;
					}
				}
				if (po == NULL)
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�³ɽ������޶�Ӧ�ı���");
					return -1;
				}
				pbm->dBuyFroze = pbm->dBuyFroze - po->dFrozAmt*pd->nDoneNum/po->nOrderNum; //�򶳽����
				pbm->nBuyQty = pbm->nBuyQty-pd->nDoneNum;//��������
			}
			//���ͷų��б�֤��
			nQty = pbm->nBuyPosi-pbm->nSellPosi;
			if (pbm->nMarginFlag == 1) //�����ڶ� ������ͬ
			{
				if (nQty!=0)
				{
					dTmp = pbm->dBuyMargin*(nQty)/pbm->nBuyPosi;
					//�ͷű�֤��
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_BUY,0.0-dTmp,false))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
						return false;
					}
				}
					
			}
			else //�����ڿ� �����෴
			{
				dTmp = pbm->dSellMargin*(0-nQty)/pbm->nSellPosi;
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_BUY,0.0-dTmp,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
					return -1;
				}
						
			}
			//���¼�����б�֤�𼰶����ʽ�
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
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
					return -1;
				}
			}
			else
			{
				pbm->nMarginFlag =2;
				dTmp = pbm->dSellMargin*(nQty)/pbm->nSellPosi;
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_SELL,dTmp,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
					return -1;
				}
			}
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"����� �ʽ��˺�[%s]Ʒ��[%d] ���߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
			//���¼��㶳��
			nQty = pbm->nBuyQty+pbm->nBuyPosi - pbm->nSellQty -pbm->nSellPosi;
			if (nQty == 0)  //������ȣ�ɶ��������
			{
				pbm->nFrozeFlag = 1; //������ڶ�
				pbm->dFrozeAmt=0.0;
			}
			else if (nQty > 0) //����0 ���������
			{
				pbm->nFrozeFlag = 1; //������ڶ�
				if (pbm->nMarginFlag == 1) //���е��ڶ࣬ͬ����
				{
					//���б�֤��
					dTmp = pbm->dBuyMargin*(pbm->nBuyPosi-pbm->nSellPosi)/pbm->nBuyPosi;
					dFrozeTmp = (pbm->dBuyMargin+pbm->dBuyFroze)*(nQty)/(pbm->nBuyPosi+pbm->nBuyQty);
					pbm->dFrozeAmt = dFrozeTmp-dTmp;
					if (pbm->dFrozeAmt > MAXDOUBLECOMPARE)
					{
						//�����µĶ���
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,pbm->dFrozeAmt,false))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
							return -1;
						}
					}
					else
					{
						pbm->dFrozeAmt = 0.0;//���ö���
					}
				}
				else //���е��ڿգ� ��ͬ����
				{
					//���б�֤��
					dTmp = pbm->dSellMargin*(pbm->nSellPosi-pbm->nBuyPosi)/pbm->nSellPosi;
					dFrozeTmp = (pbm->dBuyMargin+pbm->dBuyFroze)*(nQty)/(pbm->nBuyPosi+pbm->nBuyQty);

					pbm->dFrozeAmt = dFrozeTmp-dTmp;//�������0��Ҫȥ�����
					if (pbm->dFrozeAmt > MAXDOUBLECOMPARE)
					{
						//�����µĶ���
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,pbm->dFrozeAmt,false))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
							return -1;
						}
					}
					else
					{
						pbm->dFrozeAmt = 0.0;//���ö���
					}
				}

			}
			else //�����������
			{
				pbm->nFrozeFlag = 2;
				if (pbm->nMarginFlag == 1) //���е��ڶ࣬��ͬ����
				{
					//���б�֤��
					dTmp = pbm->dBuyMargin*(pbm->nBuyPosi-pbm->nSellPosi)/pbm->nBuyPosi;
					dFrozeTmp = (pbm->dSellMargin+pbm->dSellFroze)*(0-nQty)/(pbm->nSellPosi+pbm->nSellQty);
					pbm->dFrozeAmt = dFrozeTmp-dTmp;
					if (pbm->dFrozeAmt > MAXDOUBLECOMPARE)
					{
						//�����µĶ���
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,pbm->dFrozeAmt,false))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
							return -1;
						}
					}
					else
					{
						pbm->dFrozeAmt = 0.0;//���ö���
					}
				}
				else //���е��ڿգ� ��ͬ����
				{
					//���б�֤��
					dTmp = pbm->dSellMargin*(pbm->nSellPosi-pbm->nBuyPosi)/pbm->nSellPosi;
					dFrozeTmp = (pbm->dSellMargin+pbm->dSellFroze)*(0-nQty)/(pbm->nSellPosi+pbm->nSellQty);

					pbm->dFrozeAmt = dFrozeTmp-dTmp;//�������0��Ҫȥ�����
					if (pbm->dFrozeAmt > MAXDOUBLECOMPARE)
					{
						//�����µĶ���
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,pbm->dFrozeAmt,false))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
							return -1;
						}
					}
					else
					{
						pbm->dFrozeAmt = 0.0;//���ö���
					}
				}
			}
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�ʽ��˺�[%s]Ʒ��[%d] ���߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
			//�����Ѵ���
			if (!bisinit) //���ǳ�ʼ��
			{
				//�ⶳ������
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-pd->dUnfrozeAmt,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund ����");
					return -1;
				}
			}
			//��������
			if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FEE,FUND_CODE_FEE_EXCNANGE,pd->dExFee+pd->dMemberFee,false))
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund ����");
				return -1;
			}
		}
		else  //���������ղ�
		{
			//���ұ�
			pbm = m_pMemDb->m_pDbTrader.m_pBigNetMargin.Select(pd->sTxAcc,pc->nProdId);
			if (pbm == NULL)
			{
				pbm = AddBigMarginNull(pd->sTxAcc,pc->nProdId);
				if (pbm == NULL)
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"���Ӿ��߼�¼����[%s %d]��",pd->sTxAcc,pc->nProdId);
					return -1;
				}
			}
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"���ղ� �ʽ��˺�[%s]Ʒ��[%d] ���߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
			if (!bisinit) //���ǳ�ʼ��
			{
				//�ҵ�ԭ��������ʽ�
				if (pbm->nFrozeFlag == 1)
				{
					//��ԭ�����ͷ�
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,0.00-pbm->dFrozeAmt,false))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
						return -1;
					}
				}
				else
				{
					//��ԭ�����ͷ�
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,0.00-pbm->dFrozeAmt,false))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
						return -1;
					}
				}
				if (po == NULL)
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�³ɽ������޶�Ӧ�ı���");
					return -1;
				}
				pbm->dSellFroze = pbm->dSellFroze - po->dFrozAmt*pd->nDoneNum/po->nOrderNum; //�򶳽����
				pbm->nSellQty = pbm->nSellQty-pd->nDoneNum;//��������
			}
			//���ͷų��б�֤��
			nQty = pbm->nBuyPosi-pbm->nSellPosi;
			if (pbm->nMarginFlag == 1) //�����ڶ� ������ͬ
			{
				if (nQty!=0)
				{
					dTmp = pbm->dBuyMargin*(nQty)/pbm->nBuyPosi;
					//�ͷű�֤��
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_BUY,0.0-dTmp,false))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
						return -1;
					}
				}

			}
			else //�����ڿ� �����෴
			{
				dTmp = pbm->dSellMargin*(0-nQty)/pbm->nSellPosi;
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_BUY,0.0-dTmp,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
					return -1;
				}

			}
			//���¼�����б�֤�𼰶����ʽ�
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
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
					return -1;
				}
			}
			else
			{
				pbm->nMarginFlag =2;
				dTmp = pbm->dSellMargin*(nQty)/pbm->nSellPosi;
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_SELL,dTmp,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
					return -1;
				}
			}
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"���ղ� �ʽ��˺�[%s]Ʒ��[%d] ���߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
			//���¼��㶳��
			nQty = pbm->nBuyQty+pbm->nBuyPosi - pbm->nSellQty -pbm->nSellPosi;
			if (nQty == 0)  //������ȣ�ɶ��������
			{
				pbm->nFrozeFlag = 1; //������ڶ�
				pbm->dFrozeAmt=0.0;
			}
			else if (nQty > 0) //����0 ���������
			{
				pbm->nFrozeFlag = 1; //������ڶ�
				if (pbm->nMarginFlag == 1) //���е��ڶ࣬ͬ����
				{
					//���б�֤��
					dTmp = pbm->dBuyMargin*(pbm->nBuyPosi-pbm->nSellPosi)/pbm->nBuyPosi;
					dFrozeTmp = (pbm->dBuyMargin+pbm->dBuyFroze)*(nQty)/(pbm->nBuyPosi+pbm->nBuyQty);
					pbm->dFrozeAmt = dFrozeTmp-dTmp;
					if (pbm->dFrozeAmt > MAXDOUBLECOMPARE)
					{
						//�����µĶ���
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,pbm->dFrozeAmt,false))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
							return -1;
						}
					}
					else
					{
						pbm->dFrozeAmt = 0.0;//���ö���
					}
				}
				else //���е��ڿգ� ��ͬ����
				{
					//���б�֤��
					dTmp = pbm->dSellMargin*(pbm->nSellPosi-pbm->nBuyPosi)/pbm->nSellPosi;
					dFrozeTmp = (pbm->dBuyMargin+pbm->dBuyFroze)*(nQty)/(pbm->nBuyPosi+pbm->nBuyQty);

					pbm->dFrozeAmt = dFrozeTmp-dTmp;//�������0��Ҫȥ�����
					if (pbm->dFrozeAmt > MAXDOUBLECOMPARE)
					{
						//�����µĶ���
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,pbm->dFrozeAmt,false))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
							return -1;
						}
					}
					else
					{
						pbm->dFrozeAmt = 0.0;//���ö���
					}
				}

			}
			else //�����������
			{
				pbm->nFrozeFlag = 2;
				if (pbm->nMarginFlag == 1) //���е��ڶ࣬��ͬ����
				{
					//���б�֤��
					dTmp = pbm->dBuyMargin*(pbm->nBuyPosi-pbm->nSellPosi)/pbm->nBuyPosi;
					dFrozeTmp = (pbm->dSellMargin+pbm->dSellFroze)*(0-nQty)/(pbm->nSellPosi+pbm->nSellQty);
					pbm->dFrozeAmt = dFrozeTmp-dTmp;
					if (pbm->dFrozeAmt > MAXDOUBLECOMPARE)
					{
						//�����µĶ���
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,pbm->dFrozeAmt,false))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
							return -1;
						}
					}
					else
					{
						pbm->dFrozeAmt = 0.0;//���ö���
					}
				}
				else //���е��ڿգ� ��ͬ����
				{
					//���б�֤��
					dTmp = pbm->dSellMargin*(pbm->nSellPosi-pbm->nBuyPosi)/pbm->nSellPosi;
					dFrozeTmp = (pbm->dSellMargin+pbm->dSellFroze)*(0-nQty)/(pbm->nSellPosi+pbm->nSellQty);

					pbm->dFrozeAmt = dFrozeTmp-dTmp;//�������0��Ҫȥ�����
					if (pbm->dFrozeAmt > MAXDOUBLECOMPARE)
					{
						//�����µĶ���
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,pbm->dFrozeAmt,false))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
							return -1;
						}
					}
					else
					{
						pbm->dFrozeAmt = 0.0;//���ö���
					}
				}
			}
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�ʽ��˺�[%s]Ʒ��[%d] ���߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
			//�����Ѵ���
			if (!bisinit) //���ǳ�ʼ��
			{
				//�ⶳ������
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-pd->dUnfrozeAmt,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund ����");
					return -1;
				}
			}
			//��������
			if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FEE,FUND_CODE_FEE_EXCNANGE,pd->dExFee+pd->dMemberFee,false))
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund ����");
				return -1;
			}
		}
	}
	else //ƽ�� 
	{
		//ȡ�ֲ���Ϣ
		pPosi = m_pMemDb->m_pDbTrader.m_pPosition.Select(pd->sTxAcc,pd->sSeatNo,pd->sShFlag,pd->nContractNo);
		if (pPosi == NULL)
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�޳ֲ� �ʽ��˺�[%s] ϯλ[%s] Ͷ����־[%s] ��Լ[%d %s] ����[%d] ƽ��",\
				pd->sTxAcc,pd->sSeatNo,pd->sShFlag,pd->nContractNo,pd->sContractCode,pd->nDoneNum);
			return -1;
		}
		//����ֲ�
		if (pd->sBsFlag[0] == BS_FLAG_BUY) //��ƽ  ƽ��
		{
			//���ұ�
			pbm = m_pMemDb->m_pDbTrader.m_pBigNetMargin.Select(pd->sTxAcc,pc->nProdId);
			if (pbm == NULL)
			{
				//�ɽ����Ҳ�����¼
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�ɽ������߼�¼�Ҳ���[%s %d]�ļ�¼",pd->sTxAcc,pc->nProdId);
				return -1;
			}
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"ƽ�ղ� �ʽ��˺�[%s]Ʒ��[%d] ���߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%lf]",\
				pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,\
				pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
			if (!bisinit) //���ǳ�ʼ��
			{
				//�ҵ�ԭ��������ʽ�
				if (pbm->nFrozeFlag == 1)
				{
					//��ԭ�����ͷ�
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,0.00-pbm->dFrozeAmt,false))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
						return -1;
					}
				}
				else
				{
					//��ԭ�����ͷ�
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,0.00-pbm->dFrozeAmt,false))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
						return -1;
					}
				}
			}
			//���ͷų��б�֤��
			nQty = pbm->nBuyPosi-pbm->nSellPosi;
			if (pbm->nMarginFlag == 1) //�����ڶ� ������ͬ
			{
				if (nQty!=0)
				{
					dTmp = pbm->dBuyMargin*(nQty)/pbm->nBuyPosi;
					//�ͷű�֤��
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_BUY,0.0-dTmp,false))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
						return -1;
					}
				}

			}
			else //����ڿ� �����෴
			{
				dTmp = pbm->dSellMargin*(0-nQty)/pbm->nSellPosi;
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_BUY,0.0-dTmp,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
					return -1;
				}

			}
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"ƽ�ղ� �ʽ��˺�[%s]Ʒ��[%d] ���߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
			//���¼�����б�֤�𼰶����ʽ�
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
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
					return -1;
				}
			}
			else
			{
				pbm->nMarginFlag =2;
				dTmp = pbm->dSellMargin*(nQty)/pbm->nSellPosi;
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_SELL,dTmp,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
					return -1;
				}
			}
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"ƽ�ղ� �ʽ��˺�[%s]Ʒ��[%d] ���߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
			//���¼��㶳��
			nQty = pbm->nBuyQty+pbm->nBuyPosi - pbm->nSellQty -pbm->nSellPosi;
			if (nQty == 0)  //������ȣ�ɶ��������
			{
				pbm->nFrozeFlag = 1; //������ڶ�
				pbm->dFrozeAmt=0.0;
			}
			else if (nQty > 0) //����0 ���������
			{
				pbm->nFrozeFlag = 1; //������ڶ�
				if (pbm->nMarginFlag == 1) //���е��ڶ࣬ͬ����
				{
					//���б�֤��
					dTmp = pbm->dBuyMargin*(pbm->nBuyPosi-pbm->nSellPosi)/pbm->nBuyPosi;
					dFrozeTmp = (pbm->dBuyMargin+pbm->dBuyFroze)*(nQty)/(pbm->nBuyPosi+pbm->nBuyQty);
					pbm->dFrozeAmt = dFrozeTmp-dTmp;
					if (pbm->dFrozeAmt > MAXDOUBLECOMPARE)
					{
						//�����µĶ���
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,pbm->dFrozeAmt,false))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
							return -1;
						}
					}
					else
					{
						pbm->dFrozeAmt = 0.0;//���ö���
					}
				}
				else //���е��ڿգ� ��ͬ����
				{
					//���б�֤��
					dTmp = pbm->dSellMargin*(pbm->nSellPosi-pbm->nBuyPosi)/pbm->nSellPosi;
					dFrozeTmp = (pbm->dBuyMargin+pbm->dBuyFroze)*(nQty)/(pbm->nBuyPosi+pbm->nBuyQty);

					pbm->dFrozeAmt = dFrozeTmp-dTmp;//�������0��Ҫȥ�����
					if (pbm->dFrozeAmt > MAXDOUBLECOMPARE)
					{
						//�����µĶ���
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,pbm->dFrozeAmt,false))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
							return -1;
						}
					}
					else
					{
						pbm->dFrozeAmt = 0.0;//���ö���
					}
				}

			}
			else //�����������
			{
				pbm->nFrozeFlag = 2;
				if (pbm->nMarginFlag == 1) //���е��ڶ࣬��ͬ����
				{
					//���б�֤��
					dTmp = pbm->dBuyMargin*(pbm->nBuyPosi-pbm->nSellPosi)/pbm->nBuyPosi;
					dFrozeTmp = (pbm->dSellMargin+pbm->dSellFroze)*(0-nQty)/(pbm->nSellPosi+pbm->nSellQty);
					pbm->dFrozeAmt = dFrozeTmp-dTmp;
					if (pbm->dFrozeAmt > MAXDOUBLECOMPARE)
					{
						//�����µĶ���
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,pbm->dFrozeAmt,false))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
							return -1;
						}
					}
					else
					{
						pbm->dFrozeAmt = 0.0;//���ö���
					}
				}
				else //���е��ڿգ� ��ͬ����
				{
					//���б�֤��
					dTmp = pbm->dSellMargin*(pbm->nSellPosi-pbm->nBuyPosi)/pbm->nSellPosi;
					dFrozeTmp = (pbm->dSellMargin+pbm->dSellFroze)*(0-nQty)/(pbm->nSellPosi+pbm->nSellQty);

					pbm->dFrozeAmt = dFrozeTmp-dTmp;//�������0��Ҫȥ�����
					if (pbm->dFrozeAmt > MAXDOUBLECOMPARE)
					{
						//�����µĶ���
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,pbm->dFrozeAmt,false))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
							return -1;
						}
					}
					else
					{
						pbm->dFrozeAmt = 0.0;//���ö���
					}
				}
			}
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"ƽ�ղ� �ʽ��˺�[%s]Ʒ��[%d] ���߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);

			if (!bisinit)
			{
				//�ⶳί�ж���ĳֲּ�������
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-pd->dUnfrozeFee,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund ����");
					return -1;
				}
			}
			//����
			if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FEE,FUND_CODE_FEE_EXCNANGE,pd->dExFee+pd->dMemberFee,false))
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund ����");
				return -1;
			}
		}
		else //��ƽ ƽ��
		{
			//���ұ�
			pbm = m_pMemDb->m_pDbTrader.m_pBigNetMargin.Select(pd->sTxAcc,pc->nProdId);
			if (pbm == NULL)
			{
				//�ɽ����Ҳ�����¼
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�ɽ������߼�¼�Ҳ���[%s %d]�ļ�¼",pd->sTxAcc,pc->nProdId);
				return -1;
			}
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"ƽ��� �ʽ��˺�[%s]Ʒ��[%d] ���߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%lf]",\
				pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,\
				pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
			if (!bisinit) //���ǳ�ʼ��
			{
				//�ҵ�ԭ��������ʽ�
				if (pbm->nFrozeFlag == 1)
				{
					//��ԭ�����ͷ�
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,0.00-pbm->dFrozeAmt,false))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
						return -1;
					}
				}
				else
				{
					//��ԭ�����ͷ�
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,0.00-pbm->dFrozeAmt,false))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
						return -1;
					}
				}
			}
			//���ͷų��б�֤��
			nQty = pbm->nBuyPosi-pbm->nSellPosi;
			if (pbm->nMarginFlag == 1) //�����ڶ� ������ͬ
			{
				if (nQty!=0)
				{
					dTmp = pbm->dBuyMargin*(nQty)/pbm->nBuyPosi;
					//�ͷű�֤��
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_BUY,0.0-dTmp,false))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
						return -1;
					}
				}

			}
			else //����ڿ� �����෴
			{
				dTmp = pbm->dSellMargin*(0-nQty)/pbm->nSellPosi;
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_BUY,0.0-dTmp,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
					return -1;
				}

			}
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"ƽ��� �ʽ��˺�[%s]Ʒ��[%d] ���߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
			//���¼�����б�֤�𼰶����ʽ�
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
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
					return -1;
				}
			}
			else
			{
				pbm->nMarginFlag =2;
				dTmp = pbm->dSellMargin*(nQty)/pbm->nSellPosi;
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_MARGIN,FUND_CODE_MARGIN_POSI_SELL,dTmp,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
					return -1;
				}
			}
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"ƽ��� �ʽ��˺�[%s]Ʒ��[%d] ���߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
			//���¼��㶳��
			nQty = pbm->nBuyQty+pbm->nBuyPosi - pbm->nSellQty -pbm->nSellPosi;
			if (nQty == 0)  //������ȣ�ɶ��������
			{
				pbm->nFrozeFlag = 1; //������ڶ�
				pbm->dFrozeAmt=0.0;
			}
			else if (nQty > 0) //����0 ���������
			{
				pbm->nFrozeFlag = 1; //������ڶ�
				if (pbm->nMarginFlag == 1) //���е��ڶ࣬ͬ����
				{
					//���б�֤��
					dTmp = pbm->dBuyMargin*(pbm->nBuyPosi-pbm->nSellPosi)/pbm->nBuyPosi;
					dFrozeTmp = (pbm->dBuyMargin+pbm->dBuyFroze)*(nQty)/(pbm->nBuyPosi+pbm->nBuyQty);
					pbm->dFrozeAmt = dFrozeTmp-dTmp;
					if (pbm->dFrozeAmt > MAXDOUBLECOMPARE)
					{
						//�����µĶ���
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,pbm->dFrozeAmt,false))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
							return -1;
						}
					}
					else
					{
						pbm->dFrozeAmt = 0.0;//���ö���
					}
				}
				else //���е��ڿգ� ��ͬ����
				{
					//���б�֤��
					dTmp = pbm->dSellMargin*(pbm->nSellPosi-pbm->nBuyPosi)/pbm->nSellPosi;
					dFrozeTmp = (pbm->dBuyMargin+pbm->dBuyFroze)*(nQty)/(pbm->nBuyPosi+pbm->nBuyQty);

					pbm->dFrozeAmt = dFrozeTmp-dTmp;//�������0��Ҫȥ�����
					if (pbm->dFrozeAmt > MAXDOUBLECOMPARE)
					{
						//�����µĶ���
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,pbm->dFrozeAmt,false))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
							return -1;
						}
					}
					else
					{
						pbm->dFrozeAmt = 0.0;//���ö���
					}
				}

			}
			else //�����������
			{
				pbm->nFrozeFlag = 2;
				if (pbm->nMarginFlag == 1) //���е��ڶ࣬��ͬ����
				{
					//���б�֤��
					dTmp = pbm->dBuyMargin*(pbm->nBuyPosi-pbm->nSellPosi)/pbm->nBuyPosi;
					dFrozeTmp = (pbm->dSellMargin+pbm->dSellFroze)*(0-nQty)/(pbm->nSellPosi+pbm->nSellQty);
					pbm->dFrozeAmt = dFrozeTmp-dTmp;
					if (pbm->dFrozeAmt > MAXDOUBLECOMPARE)
					{
						//�����µĶ���
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,pbm->dFrozeAmt,false))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
							return -1;
						}
					}
					else
					{
						pbm->dFrozeAmt = 0.0;//���ö���
					}
				}
				else //���е��ڿգ� ��ͬ����
				{
					//���б�֤��
					dTmp = pbm->dSellMargin*(pbm->nSellPosi-pbm->nBuyPosi)/pbm->nSellPosi;
					dFrozeTmp = (pbm->dSellMargin+pbm->dSellFroze)*(0-nQty)/(pbm->nSellPosi+pbm->nSellQty);

					pbm->dFrozeAmt = dFrozeTmp-dTmp;//�������0��Ҫȥ�����
					if (pbm->dFrozeAmt > MAXDOUBLECOMPARE)
					{
						//�����µĶ���
						if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,pbm->dFrozeAmt,false))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
							return -1;
						}
					}
					else
					{
						pbm->dFrozeAmt = 0.0;//���ö���
					}
				}
			}
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"ƽ��� �ʽ��˺�[%s]Ʒ��[%d] ���߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);

			if (!bisinit)
			{
				//�ⶳί�ж���ĳֲּ�������
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-pd->dUnfrozeFee,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund ����");
					return -1;
				}
			}
			//����
			if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FEE,FUND_CODE_FEE_EXCNANGE,pd->dExFee+pd->dMemberFee,false))
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund ����");
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
	//���³ֲ�
	S_MDB_CUST_POSITION *pPosi=ps;
	if (pPosi == NULL)
	{
		pPosi = m_pMemDb->m_pDbTrader.m_pPosition.Select(pd->sTxAcc,pd->sSeatNo,pd->sShFlag,pd->nContractNo);
		if (pPosi == NULL)
		{
			if (pd->sEoFlag[0] != EO_FLAG_OPEN) //���ǿ���
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"���ǿ��֣����޳ֲ���Ϣ �ʽ��˺�[%s] ϯλ[%s] Ͷ����־[%s] ��Լ[%d %s] ����[%d] �ɽ����[%s]",\
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

			if (pd->sBsFlag[0] == BS_FLAG_BUY) //��
			{
				posi.dPB_Avp = pd->dDonePrice;
				posi.dPB_Margin = pd->dExMargin+pd->dMemberMargin;
				posi.nDayPB = pd->nDoneNum;
				posi.nCurCanUsePB = pd->nDoneNum;
				posi.nCurPB = pd->nDoneNum;
				posi.dPB_OAvp = pd->dDonePrice;
			}
			else //����
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
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"д��ֱֲ�ʧ�� �ʽ��˺�[%s] ϯλ[%s] Ͷ����־[%s] ��Լ[%d %s] ����[%d] ����",\
					pd->sTxAcc,pd->sSeatNo,pd->sShFlag,pd->nContractNo,pd->sContractCode,pd->nDoneNum);
				return false;
			}
			return true;
		}
	}
	

	if (pc->sIsBigMargin[0] == BOOL_FALSE) //˫�ֲ߳�
	{
		if (pd->sEoFlag[0] == EO_FLAG_OPEN) //����
		{
			//����ֲ־���
			if (pd->sBsFlag[0] == BS_FLAG_BUY) //�� ���
			{
				//����
				pPosi->dPB_Avp = (pPosi->dPB_Avp*pPosi->nCurPB+pd->dDonePrice*pd->nDoneNum)/(pPosi->nCurPB+pd->nDoneNum);
				pPosi->dPB_Margin = pPosi->dPB_Margin + pd->dExMargin+pd->dMemberMargin;
				pPosi->nDayPB = pPosi->nDayPB+pd->nDoneNum;
				pPosi->nCurCanUsePB = pPosi->nCurCanUsePB+pd->nDoneNum;
				pPosi->nCurPB = pPosi->nCurPB+pd->nDoneNum;
				//���־���
				pPosi->dPB_OAvp = (pPosi->dPB_OAvp*pPosi->nCurPB+pd->dDonePrice*pd->nDoneNum)/(pPosi->nCurPB+pd->nDoneNum);
			}
			else //���� �ղ�
			{
				//����
				pPosi->dPS_Avp = (pPosi->dPS_Avp*pPosi->nCurPS+pd->dDonePrice*pd->nDoneNum)/(pPosi->nCurPS+pd->nDoneNum);
				pPosi->dPS_Margin = pPosi->dPS_Margin + pd->dExMargin+pd->dMemberMargin;
				pPosi->nDayPS = pPosi->nDayPS+pd->nDoneNum;
				pPosi->nCurCanUsePS = pPosi->nCurCanUsePS+pd->nDoneNum;
				pPosi->nCurPS = pPosi->nCurPS+pd->nDoneNum;
				//���־���
				pPosi->dPS_OAvp = (pPosi->dPS_OAvp*pPosi->nCurPS+pd->dDonePrice*pd->nDoneNum)/(pPosi->nCurPS+pd->nDoneNum);
			}
		}
		else //ƽ��
		{
			//����ֲ�
			if (pd->sBsFlag[0] == BS_FLAG_BUY) //��ƽ  ƽ��
			{
				pPosi->dPS_Margin = pPosi->dPS_Margin - (pPosi->dPS_Margin*pd->nDoneNum/pPosi->nCurPS);
				if (!isinit)
				{
					pPosi->nDAY_COV_PS_Froze = pPosi->nDAY_COV_PS_Froze-pd->nDoneNum;
					
				}
				
				//������٣����ò���
				pPosi->nCurPS = pPosi->nCurPS-pd->nDoneNum;
				//����񿪳ֲ�
				if (pd->sEoFlag[0] == EO_FLAG_CLOSETODAY) //ƽ���
				{
					pPosi->nDayPS = pPosi->nDayPS -  pd->nDoneNum;
				}
				else //ƽ��
				{
					if (pPosi->nCurPS < pPosi->nDayPS)
					{
						pPosi->nDayPS = pPosi->nCurPS;//ʣ�µ�ȫ�ǽ�ֲ���
					}
				}
				//����ƽ��ӯ��
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_SURPLUS,FUND_CODE_SURPLUS_CLOSE,(pPosi->dPS_Avp-pd->dDonePrice)*pd->nDoneNum*pc->dTxUnit,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
					return false;
				}
			}
			else //��ƽ ƽ��
			{
				pPosi->dPB_Margin = pPosi->dPB_Margin - pPosi->dPB_Margin*pd->nDoneNum/pPosi->nCurPB;
				//������٣����ò���
				if (!isinit)
				{
					pPosi->nDAY_COV_PB_Froze = pPosi->nDAY_COV_PB_Froze-pd->nDoneNum;
				}
				//pPosi->nCurCanUsePB = pPosi->nCurCanUsePB-info->nDoneNum;
				pPosi->nCurPB = pPosi->nCurPB-pd->nDoneNum;
				//����񿪳ֲ�
				if (pd->sEoFlag[0] == EO_FLAG_CLOSETODAY) //ƽ���
				{
					pPosi->nDayPB = pPosi->nDayPB -  pd->nDoneNum;
				}
				else //ƽ��
				{
					if (pPosi->nCurPB < pPosi->nDayPB)
					{
						pPosi->nDayPB = pPosi->nCurPB;//ʣ�µ�ȫ�ǽ�ֲ���
					}

				}
				//����ƽ��ӯ��
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_SURPLUS,FUND_CODE_SURPLUS_CLOSE,(pd->dDonePrice-pPosi->dPB_Avp)*pd->nDoneNum*pc->dTxUnit,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
					return false;
				}
			}
		}
	}
	else //���̵��ֲִ߳���
	{
		if (pd->sBsFlag[0] == BS_FLAG_BUY) //�� ���
		{
			if (pPosi->nCurPB>0) //����Ϊ��
			{
				//����
				pPosi->dPB_Avp = (pPosi->dPB_Avp*pPosi->nCurPB+pd->dDonePrice*pd->nDoneNum)/(pPosi->nCurPB+pd->nDoneNum);
				pPosi->dPB_Margin = pPosi->dPB_Margin + pd->dExMargin+pd->dMemberMargin;
				pPosi->nDayPB = pPosi->nDayPB+pd->nDoneNum;
				pPosi->nCurCanUsePB = pPosi->nCurCanUsePB+pd->nDoneNum;
				pPosi->nCurPB = pPosi->nCurPB+pd->nDoneNum;
				//���־���
				pPosi->dPB_OAvp = (pPosi->dPB_OAvp*pPosi->nCurPB+pd->dDonePrice*pd->nDoneNum)/(pPosi->nCurPB+pd->nDoneNum);
			}
			else //ԭ�ֲ�Ϊ�գ���Ϊƽ��
			{
				if (pPosi->nCurPS > pd->nDoneNum) //�ֲ���Ϊ��
				{
					pPosi->dPS_Margin = pPosi->dPS_Margin - (pPosi->dPS_Margin*pd->nDoneNum/pPosi->nCurPS);
					//�����޶���
					pPosi->nCurPS = pPosi->nCurPS-pd->nDoneNum;
					//����񿪳ֲ�
					if (pPosi->nCurPS < pPosi->nDayPS)
					{
						pPosi->nDayPS = pPosi->nCurPS;//ʣ�µ�ȫ�ǽ�ֲ���
					}
					//����ƽ��ӯ��
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_SURPLUS,FUND_CODE_SURPLUS_CLOSE,(pPosi->dPS_Avp-pd->dDonePrice)*pd->nDoneNum*pc->dTxUnit,false))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
						return false;
					}
				}
				else //ȫ��ƽ�����ֱֲ�Ϊ���
				{
					//����
					pPosi->dPB_Avp = pd->dDonePrice;
					pPosi->dPB_Margin = (pd->dExMargin+pd->dMemberMargin)*(pd->nDoneNum-pPosi->nCurPS)/pd->nDoneNum;
					pPosi->nDayPB = pd->nDoneNum-pPosi->nCurPS;
					pPosi->nCurCanUsePB = pPosi->nDayPB;
					pPosi->nCurPB = pPosi->nDayPB;
					//���־���
					pPosi->dPB_OAvp = pPosi->dPB_Avp;

					pPosi->dPS_Margin = 0;
					//�����޶���
					pPosi->nCurPS = 0;
					pPosi->nDayPS=0;
					//����ƽ��ӯ��
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_SURPLUS,FUND_CODE_SURPLUS_CLOSE,(pPosi->dPS_Avp-pd->dDonePrice)*pd->nDoneNum*pc->dTxUnit,false))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
						return false;
					}
				}
			}
			
		}
		else //���� �ղ�
		{
			if (pPosi->nCurPS>0) //����Ϊ��
			{
				//����
				pPosi->dPS_Avp = (pPosi->dPS_Avp*pPosi->nCurPS+pd->dDonePrice*pd->nDoneNum)/(pPosi->nCurPS+pd->nDoneNum);
				pPosi->dPS_Margin = pPosi->dPS_Margin + pd->dExMargin+pd->dMemberMargin;
				pPosi->nDayPS = pPosi->nDayPS+pd->nDoneNum;
				pPosi->nCurCanUsePS = pPosi->nCurCanUsePS+pd->nDoneNum;
				pPosi->nCurPS = pPosi->nCurPS+pd->nDoneNum;
				//���־���
				pPosi->dPS_OAvp = (pPosi->dPS_OAvp*pPosi->nCurPS+pd->dDonePrice*pd->nDoneNum)/(pPosi->nCurPS+pd->nDoneNum);
			}
			else //ԭ�ֲ�Ϊ�࣬��Ϊƽ��
			{
				if (pPosi->nCurPB > pd->nDoneNum) //�ֲ���Ϊ��
				{
					pPosi->dPB_Margin = pPosi->dPB_Margin - (pPosi->dPB_Margin*pd->nDoneNum/pPosi->nCurPB);
					//�����޶���
					pPosi->nCurPB = pPosi->nCurPB-pd->nDoneNum;
					//����񿪳ֲ�
					if (pPosi->nCurPB < pPosi->nDayPB)
					{
						pPosi->nDayPB = pPosi->nCurPB;//ʣ�µ�ȫ�ǽ�ֲ���
					}
					//����ƽ��ӯ��
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_SURPLUS,FUND_CODE_SURPLUS_CLOSE,(pd->dDonePrice-pPosi->dPB_Avp)*pd->nDoneNum*pc->dTxUnit,false))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
						return false;
					}
				}
				else //ȫ��ƽ�����ֱֲ�Ϊ���
				{
					//����
					pPosi->dPS_Avp = pd->dDonePrice;
					pPosi->dPS_Margin = (pd->dExMargin+pd->dMemberMargin)*(pd->nDoneNum-pPosi->nCurPB)/pd->nDoneNum;
					pPosi->nDayPS = pd->nDoneNum-pPosi->nCurPB;
					pPosi->nCurCanUsePS = pPosi->nDayPS;
					pPosi->nCurPS = pPosi->nDayPS;
					//���־���
					pPosi->dPS_OAvp = pPosi->dPS_Avp;

					pPosi->dPB_Margin = 0;
					//�����޶���
					pPosi->nCurPB = 0;
					pPosi->nDayPB=0;
					//����ƽ��ӯ��
					if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_SURPLUS,FUND_CODE_SURPLUS_CLOSE,(pd->dDonePrice-pPosi->dPB_Avp)*pd->nDoneNum*pc->dTxUnit,false))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
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
	//ȡҪ����������Ѽ���֤��ȵ�
	switch (pc->sProd_type[0])
	{
		case PROD_TYPE_FUTURES://�ڻ�
			if (porder->sEoFlag[0] == EO_FLAG_OPEN) //����
			{
				//����Ҫ�㶳���������
				if (!GetTradeFeeValue(pc,pcust,1,porder->nOrderNum,porder->dAmt,exfee,memberfee))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"ί��֪ͨȡ�����ѳ��� ���ر�����[%s]",porder->sLocalSerial);
					return -1;
				}
				if (!GetMarginValue(pc,pcust,0,porder->nOrderNum,porder->dAmt,exmargin,membermargin))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"ί��֪ͨȡ��֤����� ���ر�����[%s]",porder->sLocalSerial);
					return -2;
				}
			}
			else
			{
				if (porder->sEoFlag[0] == EO_FLAG_CLOSETODAY) //ƽ���
				{
					//ƽ��Ҫ��������
					if (!GetTradeFeeValue(pc,pcust,3,porder->nOrderNum,porder->dAmt,exfee,memberfee))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"ί��֪ͨȡ�����ѳ��� ���ر�����[%s]",porder->sLocalSerial);
						return -1;
					}
				}
				else
				{
					//ƽ��Ҫ��������
					if (!GetTradeFeeValue(pc,pcust,2,porder->nOrderNum,porder->dAmt,exfee,memberfee))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"ί��֪ͨȡ�����ѳ��� ���ر�����[%s]",porder->sLocalSerial);
						return -1;
					}
				}
			}
			break;
		case PROD_TYPE_OPTION://��Ȩ
			if (porder->sEoFlag[0] == EO_FLAG_OPEN) //����
			{
				//����Ҫ�㶳���������
				if (!GetTradeFeeValue(pc,pcust,1,porder->nOrderNum,porder->dAmt,exfee,memberfee))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"ί��֪ͨȡ�����ѳ��� ���ر�����[%s]",porder->sLocalSerial);
					return -1;
				}
				if (porder->sBsFlag[0] == BS_FLAG_SELL) //����Ҫ���ᱣ֤��
				{
					if (!GetMarginValue(pc,pcust,1,porder->nOrderNum,porder->dAmt,exmargin,membermargin))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"ί��֪ͨȡ��֤����� ���ر�����[%s]",porder->sLocalSerial);
						return -2;
					}
				}
			}
			else
			{
				//ƽ��Ҫ��������
				if (porder->sEoFlag[0] == EO_FLAG_CLOSETODAY) //ƽ���
				{
					//ƽ��Ҫ��������
					if (!GetTradeFeeValue(pc,pcust,3,porder->nOrderNum,porder->dAmt,exfee,memberfee))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"ί��֪ͨȡ�����ѳ��� ���ر�����[%s]",porder->sLocalSerial);
						return -1;
					}
				}
				else
				{
					//ƽ��Ҫ��������
					if (!GetTradeFeeValue(pc,pcust,2,porder->nOrderNum,porder->dAmt,exfee,memberfee))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"ί��֪ͨȡ�����ѳ��� ���ر�����[%s]",porder->sLocalSerial);
						return -1;
					}
				}
			}
			break;
		case PROD_TYPE_SPOT://�ֻ�
			//�ֻ��������֡������
			if (porder->sShFlag[0] == SH_FLAG_DERY)
			{
				//���ӽ���
			}
			else if (porder->sShFlag[0] == SH_FLAG_MID)
			{
				//������
			}
			else
			{
				if (porder->sBsFlag[0] == BS_FLAG_BUY)
				{
					//��Ҫ�������Ѽ����
					if (!GetTradeFeeValue(pc,pcust,1,porder->nOrderNum,porder->dAmt,exfee,memberfee))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"ί��֪ͨȡ�����ѳ��� ���ر�����[%s]",porder->sLocalSerial);
						return -1;
					}
				}
				else
				{
					//��Ҫ��������	
					if (!GetTradeFeeValue(pc,pcust,2,porder->nOrderNum,porder->dAmt,exfee,memberfee))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"ί��֪ͨȡ�����ѳ��� ���ر�����[%s]",porder->sLocalSerial);
						return -1;
					}
				}
			}
			membermargin = porder->dAmt;
			break;
		case PROD_TYPE_SEC://֤ȯ
			if (porder->sBsFlag[0] == BS_FLAG_BUY)
			{
				//��Ҫ�������Ѽ����
				if (!GetTradeFeeValue(pc,pcust,1,porder->nOrderNum,porder->dAmt,exfee,memberfee))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"ί��֪ͨȡ�����ѳ��� ���ر�����[%s]",porder->sLocalSerial);
					return -1;
				}
			}
			else
			{
				//��Ҫ��������
				if (!GetTradeFeeValue(pc,pcust,2,porder->nOrderNum,porder->dAmt,exfee,memberfee))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"ί��֪ͨȡ�����ѳ��� ���ر�����[%s]",porder->sLocalSerial);
					return -1;
				}
			}
			membermargin = porder->dAmt;
			break;
		default:
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"ί��֪ͨ��Լ����[%s]��Ʒ�����[%s]����",porder->sContractCode,pc->sProd_type);
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

	//��������
	if (po->sEoFlag[0] == EO_FLAG_OPEN) //����
	{
		if (po->sBsFlag[0] == BS_FLAG_BUY) //��
		{
			//���ұ�
			pbm = m_pMemDb->m_pDbTrader.m_pBigNetMargin.Select(po->sTxAcc,pc->nProdId);
			if (pbm == NULL)//û������
			{
				strcpy(bm.sTxAcc,po->sTxAcc);
				bm.nMarginFlag = 1;
				bm.sBailMode= BAIL_MODE_NET;
				bm.nVarietyId = pc->nProdId;
				if (bisinit) //��ʾ�����ݿ���ģ���ʣ��������
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
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"дƷ�ִ�߾��߱���� �ʽ��˺�[%s]Ʒ��[%d]",po->sTxAcc,pc->nProdId);
					po->sOrderStatus[0] = ORDER_STATUS_INVALID;
					strcpy(po->sOrderRsp,"дƷ�ִ�߾��߱����");
					return RET_ERR_SYSTEM;
				}
				pbm =  m_pMemDb->m_pDbTrader.m_pBigNetMargin.At(rid);
				m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�ʽ��˺�[%s]Ʒ��[%d] ���߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%d] ������[%d] ����[%lf]",\
					pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,\
					pbm->dBuyFroze,pbm->dSellFroze,pbm->nBuyQty,pbm->nSellQty,pbm->dFrozeAmt);
				if (ischeckfund)
				{
					if (fundinfo->dAvailFund < bm.dFrozeAmt-po->dExFee)
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ʽ���   �ͻ�[%s %s]����[%.4lf]��Ҫ����[%.4lf]",fundinfo->sTxAcc,fundinfo->sCurrency,fundinfo->dAvailFund,bm.dFrozeAmt-po->dExFee);
						po->sOrderStatus[0] = ORDER_STATUS_INVALID;
						strcpy(po->sOrderRsp,"�����ʽ���");
						return RET_ERR_AVAILFUND;
					}
				}
				//����֤��
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,bm.dFrozeAmt,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
					po->sOrderStatus[0] = ORDER_STATUS_INVALID;
					strcpy(po->sOrderRsp,"�����ʽ����");
					return RET_ERR_SYSTEM;
				}
			}
			else //����ö������
			{
				m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�ʽ��˺�[%s]Ʒ��[%d] ���߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%d] ������[%d] ����[%lf]",\
					pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,\
					pbm->dBuyFroze,pbm->dSellFroze,pbm->nBuyQty,pbm->nSellQty,pbm->dFrozeAmt);
				if (bisinit) //��ʾ�����ݿ���ģ���ʣ��������
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
				if (nQty == 0)  //������ȣ�ɶ��������
				{
					pbm->nFrozeFlag = 1; //������ڶ�
					pbm->dFrozeAmt=0.0;
				}
				else if (nQty > 0) //����0 ���������
				{
					pbm->nFrozeFlag = 1; //������ڶ�
					if (pbm->nMarginFlag == 1) //���е��ڶ࣬ͬ����
					{
						//���б�֤��
						dTmp = pbm->dBuyMargin*(pbm->nBuyPosi-pbm->nSellPosi)/pbm->nBuyPosi;
						dFrozeTmp = (pbm->dBuyMargin+pbm->dBuyFroze)*(nQty)/(pbm->nBuyPosi+pbm->nBuyQty);

						pbm->dFrozeAmt = dFrozeTmp-dTmp;
						if (pbm->dFrozeAmt > MAXDOUBLECOMPARE)
						{
							////�����µĶ���
							//if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,pbm->dFrozeAmt,false))
							//{
							//	m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
							//	return false;
							//}
						}
						else
						{
							pbm->dFrozeAmt = 0.0;//���ö���
						}
					}
					else //���е��ڿգ� ��ͬ����
					{
						//���б�֤��
						dTmp = pbm->dSellMargin*(pbm->nSellPosi-pbm->nBuyPosi)/pbm->nSellPosi;
						dFrozeTmp = (pbm->dBuyMargin+pbm->dBuyFroze)*(nQty)/(pbm->nBuyPosi+pbm->nBuyQty);

						pbm->dFrozeAmt = dFrozeTmp-dTmp;//�������0��Ҫȥ�����
						if (pbm->dFrozeAmt > MAXDOUBLECOMPARE)
						{
							////�����µĶ���
							//if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,pbm->dFrozeAmt,false))
							//{
							//	m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
							//	return false;
							//}
						}
						else
						{
							pbm->dFrozeAmt = 0.0;//���ö���
						}
					}

				}
				else //�����������
				{
					pbm->nFrozeFlag = 2;
					if (pbm->nMarginFlag == 1) //���е��ڶ࣬��ͬ����
					{
						//���б�֤��
						dTmp = pbm->dBuyMargin*(pbm->nBuyPosi-pbm->nSellPosi)/pbm->nBuyPosi;
						dFrozeTmp = (pbm->dSellMargin+pbm->dSellFroze)*(0-nQty)/(pbm->nSellPosi+pbm->nSellQty);

						pbm->dFrozeAmt = dFrozeTmp-dTmp;
						if (pbm->dFrozeAmt > MAXDOUBLECOMPARE)
						{
							////�����µĶ���
							//if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,pbm->dFrozeAmt,false))
							//{
							//	m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
							//	return false;
							//}
						}
						else
						{
							pbm->dFrozeAmt = 0.0;//���ö���
						}
					}
					else //���е��ڿգ� ��ͬ����
					{
						//���б�֤��
						dTmp = pbm->dSellMargin*(pbm->nSellPosi-pbm->nBuyPosi)/pbm->nSellPosi;
						dFrozeTmp = (pbm->dSellMargin+pbm->dSellFroze)*(0-nQty)/(pbm->nSellPosi+pbm->nSellQty);

						pbm->dFrozeAmt = dFrozeTmp-dTmp;//�������0��Ҫȥ�����
						if (pbm->dFrozeAmt > MAXDOUBLECOMPARE)
						{
							////�����µĶ���
							//if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,pbm->dFrozeAmt,false))
							//{
							//	m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
							//	return false;
							//}
						}
						else
						{
							pbm->dFrozeAmt = 0.0;//���ö���
						}
					}
				}

			}
			//�ж��ʽ��Ƿ��㹻
			if (ischeckfund)
			{
				dFrozeTmp = pbm->dFrozeAmt-dLastFroze;
				if (dFrozeTmp >MAXDOUBLECOMPARE) //��Ҫ����
				{
					if (fundinfo->dAvailFund < dFrozeTmp-po->dExFee+MAXDOUBLECOMPARE )
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ʽ���   �ͻ�[%s %s]����[%.4lf]��Ҫ����[%.4lf]",fundinfo->sTxAcc,fundinfo->sCurrency,fundinfo->dAvailFund,dFrozeTmp-po->dExFee);
						po->sOrderStatus[0] = ORDER_STATUS_INVALID;
						strcpy(po->sOrderRsp,"�����ʽ���");
						return RET_ERR_AVAILFUND;
					}
				}
				else
				{
					if (fundinfo->dAvailFund < 0.0-po->dExFee+MAXDOUBLECOMPARE)
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ʽ���   �ͻ�[%s %s]����[%.4lf]��Ҫ����[%.4lf]",fundinfo->sTxAcc,fundinfo->sCurrency,fundinfo->dAvailFund,0.00-po->dExFee);
						po->sOrderStatus[0] = ORDER_STATUS_INVALID;
						strcpy(po->sOrderRsp,"�����ʽ���");
						return RET_ERR_AVAILFUND;
					}
				}
			}
			//�Ƚⶳ
			if (nLastFlag == 1)
			{
				//��ԭ�����ͷ�
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,0.00-dLastFroze,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
					po->sOrderStatus[0] = ORDER_STATUS_INVALID;
					strcpy(po->sOrderRsp,"�����ʽ����");
					return RET_ERR_SYSTEM;
				}
			}
			else
			{
				//��ԭ�����ͷ�
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,0.00-dLastFroze,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
					po->sOrderStatus[0] = ORDER_STATUS_INVALID;
					strcpy(po->sOrderRsp,"�����ʽ����");
					return RET_ERR_SYSTEM;
				}
			}
			//�����µ����¶���
			if (pbm->nFrozeFlag == 1)
			{
				//�����µĶ���
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,pbm->dFrozeAmt,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
					po->sOrderStatus[0] = ORDER_STATUS_INVALID;
					strcpy(po->sOrderRsp,"�����ʽ����");
					return RET_ERR_SYSTEM;
				}
			}
			else
			{
				//�����µĶ���
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,pbm->dFrozeAmt,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
					po->sOrderStatus[0] = ORDER_STATUS_INVALID;
					strcpy(po->sOrderRsp,"�����ʽ����");
					return RET_ERR_SYSTEM;
				}
			}
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�ʽ��˺�[%s]Ʒ��[%d] ���߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%d] ������[%d] ����[%lf]",\
				pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,\
				pbm->dBuyFroze,pbm->dSellFroze,pbm->nBuyQty,pbm->nSellQty,pbm->dFrozeAmt);
			//��������
			if (bisinit) //��ʼ������ʣ������
			{
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-po->dExFee*po->nRemainNum/po->nOrderNum,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
					po->sOrderStatus[0] = ORDER_STATUS_INVALID;
					strcpy(po->sOrderRsp,"�����ʽ����");
					return RET_ERR_SYSTEM;
				}
			}
			else
			{
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-po->dExFee,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
					po->sOrderStatus[0] = ORDER_STATUS_INVALID;
					strcpy(po->sOrderRsp,"�����ʽ����");
					return RET_ERR_SYSTEM;
				}
			}
		}
		else  //����
		{
			//���ұ�
			pbm = m_pMemDb->m_pDbTrader.m_pBigNetMargin.Select(po->sTxAcc,pc->nProdId);
			if (pbm == NULL)//û������
			{
				strcpy(bm.sTxAcc,po->sTxAcc);
				bm.nMarginFlag = 1;
				bm.sBailMode= BAIL_MODE_NET;
				bm.nVarietyId = pc->nProdId;
				if (bisinit) //��ʾ�����ݿ���ģ���ʣ��������
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
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"дƷ�ִ�߾��߱���� �ʽ��˺�[%s]Ʒ��[%d]",po->sTxAcc,pc->nProdId);
					po->sOrderStatus[0] = ORDER_STATUS_INVALID;
					strcpy(po->sOrderRsp,"дƷ�ִ�߾��߱����");
					return RET_ERR_SYSTEM;
				}
				pbm =  m_pMemDb->m_pDbTrader.m_pBigNetMargin.At(rid);
				m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�ʽ��˺�[%s]Ʒ��[%d] ���߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%d] ������[%d] ����[%lf]",\
					pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,\
					pbm->dBuyFroze,pbm->dSellFroze,pbm->nBuyQty,pbm->nSellQty,pbm->dFrozeAmt);
				if (ischeckfund)
				{
					if (fundinfo->dAvailFund < pbm->dFrozeAmt-po->dExFee)
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ʽ���   �ͻ�[%s %s]����[%.4lf]��Ҫ����[%.4lf]",fundinfo->sTxAcc,fundinfo->sCurrency,fundinfo->dAvailFund,pbm->dFrozeAmt-po->dExFee);
						po->sOrderStatus[0] = ORDER_STATUS_INVALID;
						strcpy(po->sOrderRsp,"�����ʽ���");
						return RET_ERR_AVAILFUND;
					}
				}
				//������֤��
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,pbm->dFrozeAmt,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
					po->sOrderStatus[0] = ORDER_STATUS_INVALID;
					strcpy(po->sOrderRsp,"�����ʽ����");
					return RET_ERR_SYSTEM;
				}
			}
			else //����ö������
			{
				m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�ʽ��˺�[%s]Ʒ��[%d] ���߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%d] ������[%d] ����[%lf]",\
					pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,\
					pbm->dBuyFroze,pbm->dSellFroze,pbm->nBuyQty,pbm->nSellQty,pbm->dFrozeAmt);
				if (bisinit) //��ʾ�����ݿ���ģ���ʣ��������
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

				
				m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�ʽ��˺�[%s]Ʒ��[%d] ���߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%lf]",pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
				nQty = pbm->nBuyQty+pbm->nBuyPosi - pbm->nSellQty -pbm->nSellPosi;
				if (nQty == 0)  //������ȣ�ɶ��������
				{
					pbm->nFrozeFlag = 1; //������ڶ�
					pbm->dFrozeAmt=0.0;
				}
				else if (nQty > 0) //����0 ���������
				{
					pbm->nFrozeFlag = 1; //������ڶ�
					if (pbm->nMarginFlag == 1) //���е��ڶ࣬ͬ����
					{
						//���б�֤��
						dTmp = pbm->dBuyMargin*(pbm->nBuyPosi-pbm->nSellPosi)/pbm->nBuyPosi;
						dFrozeTmp = (pbm->dBuyMargin+pbm->dBuyFroze)*(nQty)/(pbm->nBuyPosi+pbm->nBuyQty);

						pbm->dFrozeAmt = dFrozeTmp-dTmp;
						if (pbm->dFrozeAmt > MAXDOUBLECOMPARE)
						{
							//�����µĶ���
							//if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,pbm->dFrozeAmt,false))
							//{
							//	m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
							//	return false;
							//}
						}
						else
						{
							pbm->dFrozeAmt = 0.0;//���ö���
						}
					}
					else //���е��ڿգ� ��ͬ����
					{
						//���б�֤��
						dTmp = pbm->dSellMargin*(pbm->nSellPosi-pbm->nBuyPosi)/pbm->nSellPosi;
						dFrozeTmp = (pbm->dBuyMargin+pbm->dBuyFroze)*(nQty)/(pbm->nBuyPosi+pbm->nBuyQty);

						pbm->dFrozeAmt = dFrozeTmp-dTmp;//�������0��Ҫȥ�����
						if (pbm->dFrozeAmt > MAXDOUBLECOMPARE)
						{
							//�����µĶ���
							//if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,pbm->dFrozeAmt,false))
							//{
							//	m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
							//	return false;
							//}
						}
						else
						{
							pbm->dFrozeAmt = 0.0;//���ö���
						}
					}

				}
				else //�����������
				{
					pbm->nFrozeFlag = 2;
					if (pbm->nMarginFlag == 1) //���е��ڶ࣬��ͬ����
					{
						//���б�֤��
						dTmp = pbm->dBuyMargin*(pbm->nBuyPosi-pbm->nSellPosi)/pbm->nBuyPosi;
						dFrozeTmp = (pbm->dSellMargin+pbm->dSellFroze)*(0-nQty)/(pbm->nSellPosi+pbm->nSellQty);

						pbm->dFrozeAmt = dFrozeTmp-dTmp;
						if (pbm->dFrozeAmt > MAXDOUBLECOMPARE)
						{
							//�����µĶ���
							//if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,pbm->dFrozeAmt,false))
							//{
							//	m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
							//	return false;
							//}
						}
						else
						{
							pbm->dFrozeAmt = 0.0;//���ö���
						}
					}
					else //���е��ڿգ� ��ͬ����
					{
						//���б�֤��
						dTmp = pbm->dSellMargin*(pbm->nSellPosi-pbm->nBuyPosi)/pbm->nSellPosi;
						dFrozeTmp = (pbm->dSellMargin+pbm->dSellFroze)*(0-nQty)/(pbm->nSellPosi+pbm->nSellQty);

						pbm->dFrozeAmt = dFrozeTmp-dTmp;//�������0��Ҫȥ�����
						if (pbm->dFrozeAmt > MAXDOUBLECOMPARE)
						{
							//�����µĶ���
							//if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,pbm->dFrozeAmt,false))
							//{
							//	m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
							//	return false;
							//}
						}
						else
						{
							pbm->dFrozeAmt = 0.0;//���ö���
						}
					}
				}
			}
			//�ж��ʽ��Ƿ��㹻
			if (ischeckfund)
			{
				dFrozeTmp = pbm->dFrozeAmt-dLastFroze;
				if (dFrozeTmp >MAXDOUBLECOMPARE) //��Ҫ����
				{
					if (fundinfo->dAvailFund < dFrozeTmp-po->dExFee+MAXDOUBLECOMPARE )
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ʽ���   �ͻ�[%s %s]����[%.4lf]��Ҫ����[%.4lf]",fundinfo->sTxAcc,fundinfo->sCurrency,fundinfo->dAvailFund,dFrozeTmp-po->dExFee);
						po->sOrderStatus[0] = ORDER_STATUS_INVALID;
						strcpy(po->sOrderRsp,"�����ʽ���");
						return RET_ERR_AVAILFUND;
					}
				}
				else
				{
					if (fundinfo->dAvailFund < 0.0-po->dExFee+MAXDOUBLECOMPARE)
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ʽ���   �ͻ�[%s %s]����[%.4lf]��Ҫ����[%.4lf]",fundinfo->sTxAcc,fundinfo->sCurrency,fundinfo->dAvailFund,0.00-po->dExFee);
						po->sOrderStatus[0] = ORDER_STATUS_INVALID;
						strcpy(po->sOrderRsp,"�����ʽ���");
						return RET_ERR_AVAILFUND;
					}
				}
			}
			if (nLastFlag == 1)
			{
				//��ԭ�����ͷ�
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,0.00-dLastFroze,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
					po->sOrderStatus[0] = ORDER_STATUS_INVALID;
					strcpy(po->sOrderRsp,"�����ʽ����");
					return RET_ERR_SYSTEM;
				}
			}
			else
			{
				//��ԭ�����ͷ�
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,0.00-dLastFroze,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
					po->sOrderStatus[0] = ORDER_STATUS_INVALID;
					strcpy(po->sOrderRsp,"�����ʽ����");
					return RET_ERR_SYSTEM;
				}
			}
			//�����µ����¶���
			if (pbm->nFrozeFlag == 1)
			{
				//�����µĶ���
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,pbm->dFrozeAmt,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
					po->sOrderStatus[0] = ORDER_STATUS_INVALID;
					strcpy(po->sOrderRsp,"�����ʽ����");
					return RET_ERR_SYSTEM;
				}
			}
			else
			{
				//�����µĶ���
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,pbm->dFrozeAmt,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
					po->sOrderStatus[0] = ORDER_STATUS_INVALID;
					strcpy(po->sOrderRsp,"�����ʽ����");
					return RET_ERR_SYSTEM;
				}
			}
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�ʽ��˺�[%s]Ʒ��[%d] ���߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%d] ������[%d] ����[%lf]",\
				pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,\
				pbm->dBuyFroze,pbm->dSellFroze,pbm->nBuyQty,pbm->nSellQty,pbm->dFrozeAmt);
			//��������
			if (bisinit) //��ʼ������ʣ������
			{
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-po->dExFee*po->nRemainNum/po->nOrderNum,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
					po->sOrderStatus[0] = ORDER_STATUS_INVALID;
					strcpy(po->sOrderRsp,"�����ʽ����");
					return RET_ERR_SYSTEM;
				}
			}
			else
			{
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-po->dExFee,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
					po->sOrderStatus[0] = ORDER_STATUS_INVALID;
					strcpy(po->sOrderRsp,"�����ʽ����");
					return RET_ERR_SYSTEM;
				}
			}
		}
	}
	else //ƽ
	{
		//���ֲ�
		pPosi = m_pMemDb->m_pDbTrader.m_pPosition.Select(po->sTxAcc,po->sSeatNo,po->sShFlag,po->nContractNo);
		if (pPosi == NULL)
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�ʽ��˺�[%s] ƽ��ʱ ϯλ[%s] Ͷ����־[%s] ��Լ[%d %s]�޳ֲ�",po->sTxAcc,po->sSeatNo,po->sShFlag,po->nContractNo,po->sContractCode);
			po->sOrderStatus[0] = ORDER_STATUS_INVALID;
			strcpy(po->sOrderRsp,"���óֲֲ���");
			return RET_ERR_POSITION_NOTFOUND;
		}
		if (bisinit) //��ʼ���Ĵ�����ʣ����
		{
			if (po->sBsFlag[0] == BS_FLAG_BUY) //��ƽ ƽ�ղ�
			{
				//���ü���
				if (pPosi->nCurCanUsePS < po->nRemainNum)
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�ʽ��˺�[%s] ƽ��ʱ ϯλ[%s] Ͷ����־[%s] ��Լ[%d %s] ���óֲֲ���",po->sTxAcc,po->sSeatNo,po->sShFlag,po->nContractNo,po->sContractCode);
					po->sOrderStatus[0] = ORDER_STATUS_INVALID;
					strcpy(po->sOrderRsp,"���óֲֲ���");
					return RET_ERR_POSITION_NOTFOUND;
				}
				pPosi->nCurCanUsePS = pPosi->nCurCanUsePS - po->nRemainNum;
				//��������
				pPosi->nDAY_COV_PS_Froze = pPosi->nDAY_COV_PS_Froze + po->nRemainNum;
			}
			else //��ƽ ƽ���
			{
				//���ü���
				if (pPosi->nCurCanUsePB < po->nRemainNum)
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�ʽ��˺�[%s] ƽ��ʱ ϯλ[%s] Ͷ����־[%s] ��Լ[%d %s] ���óֲֲ���",po->sTxAcc,po->sSeatNo,po->sShFlag,po->nContractNo,po->sContractCode);
					po->sOrderStatus[0] = ORDER_STATUS_INVALID;
					strcpy(po->sOrderRsp,"���óֲֲ���");
					return RET_ERR_POSITION_NOTFOUND;
				}
				pPosi->nCurCanUsePB = pPosi->nCurCanUsePB - po->nRemainNum;
				//��������
				pPosi->nDAY_COV_PB_Froze = pPosi->nDAY_COV_PB_Froze - po->nRemainNum;
			}
			//������
			if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-po->dExFee*po->nRemainNum/po->nOrderNum,false))
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
				po->sOrderStatus[0] = ORDER_STATUS_INVALID;
				strcpy(po->sOrderRsp,"�����ʽ����");
				return RET_ERR_SYSTEM;
			}
		}
		else //������ƽ��
		{
			if (po->sBsFlag[0] == BS_FLAG_BUY) //��ƽ ƽ�ղ�
			{
				//���ü���
				if (pPosi->nCurCanUsePS < po->nOrderNum)
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�ʽ��˺�[%s] ƽ��ʱ ϯλ[%s] Ͷ����־[%s] ��Լ[%d %s] ���óֲֲ���",\
						po->sTxAcc,po->sSeatNo,po->sShFlag,po->nContractNo,po->sContractCode);
					po->sOrderStatus[0] = ORDER_STATUS_INVALID;
					strcpy(po->sOrderRsp,"���óֲֲ���");
					return RET_ERR_POSITION_NOTFOUND;
				}
				pPosi->nCurCanUsePS = pPosi->nCurCanUsePS - po->nOrderNum;
				//��������
				pPosi->nDAY_COV_PS_Froze = pPosi->nDAY_COV_PS_Froze + po->nOrderNum;
			}
			else //��ƽ ƽ���
			{
				//���ü���
				if (pPosi->nCurCanUsePB < po->nOrderNum)
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�ʽ��˺�[%s] ƽ��ʱ ϯλ[%s] Ͷ����־[%s] ��Լ[%d %s] ���óֲֲ���",\
						po->sTxAcc,po->sSeatNo,po->sShFlag,po->nContractNo,po->sContractCode);
					po->sOrderStatus[0] = ORDER_STATUS_INVALID;
					strcpy(po->sOrderRsp,"���óֲֲ���");
					return RET_ERR_POSITION_NOTFOUND;
				}
				pPosi->nCurCanUsePB = pPosi->nCurCanUsePB - po->nOrderNum;
				//��������
				pPosi->nDAY_COV_PB_Froze = pPosi->nDAY_COV_PB_Froze - po->nOrderNum;
			}
			//������
			if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,0.00-po->dExFee,false))
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
				po->sOrderStatus[0] = ORDER_STATUS_INVALID;
				strcpy(po->sOrderRsp,"�����ʽ����");
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
		//�ɽ����Ҳ�����¼
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"����֪ͨ �����߼�¼�Ҳ���[%s %d]�ļ�¼",po->sTxAcc,pc->nProdId);
		return -1;
	}
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"����֪ͨ ���ߴ���ʼ �ʽ��˺�[%s]Ʒ��[%d] ���߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%lf]",\
		pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
	//�ҵ�ԭ��������ʽ�
	if (pbm->nFrozeFlag == 1)
	{
		//��ԭ�����ͷ�
		if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,0.00-pbm->dFrozeAmt,false))
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
			return -1;
		}
	}
	else
	{
		//��ԭ�����ͷ�
		if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,0.00-pbm->dFrozeAmt,false))
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
			return -1;
		}
	}
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"����֪ͨ ���ߴ����ͷ����ж��� �ʽ��˺�[%s]Ʒ��[%d] ���߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%lf]",\
		pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
	//���¼�����б�֤�𼰶����ʽ�
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
	//���¼��㶳��
	nQty = pbm->nBuyQty+pbm->nBuyPosi - pbm->nSellQty -pbm->nSellPosi;
	if (nQty == 0)  //������ȣ�ɶ��������
	{
		pbm->nFrozeFlag = 1; //������ڶ�
		pbm->dFrozeAmt=0.0;
	}
	else if (nQty > 0) //����0 ���������
	{
		pbm->nFrozeFlag = 1; //������ڶ�
		if (pbm->nMarginFlag == 1) //���е��ڶ࣬ͬ����
		{
			//���б�֤��
			dTmp = pbm->dBuyMargin*(pbm->nBuyPosi-pbm->nSellPosi)/pbm->nBuyPosi;
			dFrozeTmp = (pbm->dBuyMargin+pbm->dBuyFroze)*(nQty)/(pbm->nBuyPosi+pbm->nBuyQty);
			pbm->dFrozeAmt = dFrozeTmp-dTmp;
			if (pbm->dFrozeAmt > MAXDOUBLECOMPARE)
			{
				//�����µĶ���
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,pbm->dFrozeAmt,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
					return -1;
				}
			}
			else
			{
				pbm->dFrozeAmt = 0.0;//���ö���
			}
		}
		else //���е��ڿգ� ��ͬ����
		{
			//���б�֤��
			dTmp = pbm->dSellMargin*(pbm->nSellPosi-pbm->nBuyPosi)/pbm->nSellPosi;
			dFrozeTmp = (pbm->dBuyMargin+pbm->dBuyFroze)*(nQty)/(pbm->nBuyPosi+pbm->nBuyQty);

			pbm->dFrozeAmt = dFrozeTmp-dTmp;//�������0��Ҫȥ�����
			if (pbm->dFrozeAmt > MAXDOUBLECOMPARE)
			{
				//�����µĶ���
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_BUYFROZEMARGIN,pbm->dFrozeAmt,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
					return -1;
				}
			}
			else
			{
				pbm->dFrozeAmt = 0.0;//���ö���
			}
		}

	}
	else //�����������
	{
		pbm->nFrozeFlag = 2;
		if (pbm->nMarginFlag == 1) //���е��ڶ࣬��ͬ����
		{
			//���б�֤��
			dTmp = pbm->dBuyMargin*(pbm->nBuyPosi-pbm->nSellPosi)/pbm->nBuyPosi;
			dFrozeTmp = (pbm->dSellMargin+pbm->dSellFroze)*(0-nQty)/(pbm->nSellPosi+pbm->nSellQty);
			pbm->dFrozeAmt = dFrozeTmp-dTmp;
			if (pbm->dFrozeAmt > MAXDOUBLECOMPARE)
			{
				//�����µĶ���
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,pbm->dFrozeAmt,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
					return -1;
				}
			}
			else
			{
				pbm->dFrozeAmt = 0.0;//���ö���
			}
		}
		else //���е��ڿգ� ��ͬ����
		{
			//���б�֤��
			dTmp = pbm->dSellMargin*(pbm->nSellPosi-pbm->nBuyPosi)/pbm->nSellPosi;
			dFrozeTmp = (pbm->dSellMargin+pbm->dSellFroze)*(0-nQty)/(pbm->nSellPosi+pbm->nSellQty);

			pbm->dFrozeAmt = dFrozeTmp-dTmp;//�������0��Ҫȥ�����
			if (pbm->dFrozeAmt > MAXDOUBLECOMPARE)
			{
				//�����µĶ���
				if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_SELLFROZEMARGIN,pbm->dFrozeAmt,false))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund����");
					return -1;
				}
			}
			else
			{
				pbm->dFrozeAmt = 0.0;//���ö���
			}
		}
	}
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"����֪ͨ ���ߴ������¶��� �ʽ��˺�[%s]Ʒ��[%d] ���߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%lf]",\
		pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
	//�ⶳ������
	if (!UpdateFund(lockinfo,fundinfo,FUND_TYPE_FROZE,FUND_CODE_FEEFROZE,po->dExFee*po->nCancelNum/po->nOrderNum,false))
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"UpdateFund ����");
		return -1;
	}
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"����֪ͨ ���ߴ���ⶳ�����Ѽ�������� �ʽ��˺�[%s]Ʒ��[%d] ���߷���[%d] ��ռ��[%lf] ��ռ��[%lf] ���᷽��[%d] �򶳽�[%lf] ������[%lf] ����[%lf]",\
		pbm->sTxAcc,pbm->nVarietyId,pbm->nMarginFlag,pbm->dBuyMargin,pbm->dSellMargin,pbm->nFrozeFlag,pbm->dBuyFroze,pbm->dSellFroze,pbm->dFrozeAmt);
	return SUCCESS;
}