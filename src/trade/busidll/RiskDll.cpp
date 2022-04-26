// RiskDll.cpp: implementation of the RiskDll class.
//
//////////////////////////////////////////////////////////////////////

#include "RiskDll.h"
#include "BF_Slist.h"

#ifndef ARBSTATUS_MASTER
#define   ARBSTATUS_UNKNOW       0  //未知状态
#define   ARBSTATUS_MASTER       1  //主机状态
#define   ARBSTATUS_MASTER_AYN   3  //主机同步状态  3-4对应，说明主机正在和备机在同步数据 
#define   ARBSTATUS_BACKUP       2  //备机状态
#define   ARBSTATUS_BACKUP_AYN   4  //备机同步状态
#define   MASTERHOST             10  //负载均衡主机状态
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//注意：下面的定义在每个类里面都要有，主程序根据此方法获取本类的实例
RiskDll * getInstance() 
{ 
    return new RiskDll(); 
}
void RiskDll::GenAcc(const char *oldacc, int syscode,int destlen, char *txacc)
{
	int len = strlen(oldacc);
	int addlen = destlen-len;
	sprintf(txacc,"%d",syscode);
	for (int i=0; i<addlen; i++)
	{
		strcat(txacc,"0");
	}
	strcat(txacc,oldacc);
}

RiskDll::RiskDll()
{
	m_nRpcSerial=0;
	InitFuncInfo(9999999,(FuncPointer )&RiskDll::RcvQuotation,"行情处理",1.0,"作者",1);

	InitFuncInfo(9805,(FuncPointer )&RiskDll::QueryCustOrder,"查询客户报单",1.0,"作者",1);

	InitFuncInfo(9202,(FuncPointer )&RiskDll::OrderMsg,"报单处理",1.0,"作者",1);


	InitFuncInfo(9830,(FuncPointer )&RiskDll::Order,"委托报单",1.0,"作者",1);

	InitFuncInfo(1021, (FuncPointer)&RiskDll::test1021, "委托报单", 1.0, "作者", 1);
	InitFuncInfo(1023, (FuncPointer)&RiskDll::test1023, "委托报单", 1.0, "作者", 1);
}

int RiskDll::test1021(S_TRADE_DATA& data)
{
	m_nRpcSerial= m_pClientLink->GetSerial();

	bzero(&(data.pData->sDBHead),sizeof(DREB_HEAD));
	data.pData->sDBHead.cCmd = CMD_DPCALL;
	data.pData->sDBHead.d_Dinfo.d_nServiceNo = 1023;
    strcpy(data.pData->sBuffer, "test1023请求");
    data.pData->sDBHead.nLen = strlen(data.pData->sBuffer);
	if (m_pClientLink->RpcRequest(data, m_nRpcSerial, [=](S_TRADE_DATA &rpcdata, S_TRADE_DATA &ansdata) -> int
		{
			if (ansdata.pData->sDBHead.nLen>0)
			{
				m_pLog->LogMp(LOG_DEBUG, __FILE__, __LINE__, "请求[%s] 应答[%s] requestid[%d]", rpcdata.pData->sBuffer, ansdata.pData->sBuffer, ansdata.pData->sDBHead.s_Sinfo.s_nSerial);
			}
			else
			{
				m_pLog->LogMp(LOG_DEBUG, __FILE__, __LINE__, "rpc总线应答返回出错[%d] requestid[%d]", ansdata.pData->sDBHead.a_Ainfo.a_nRetCode, ansdata.pData->sDBHead.s_Sinfo.s_nSerial);
			}
			m_pClientLink->PoolFree(ansdata);
			return 0;
		}
		
	)<0)
	{
		m_pLog->LogMp(LOG_DEBUG, __FILE__, __LINE__, "RpcRequest 返回<0");
	}
	return 0;
}
int RiskDll::test1023(S_TRADE_DATA& data)
{
    sprintf(data.pData->sBuffer,"test1023应答 %d",data.pData->sDBHead.s_Sinfo.s_nSerial);
	data.pData->sDBHead.nLen = strlen(data.pData->sBuffer);
	m_pClientLink->AnsData(data,0);
	return 0;
}

RiskDll::~RiskDll()
{

}
bool RiskDll::Init(CBF_Xml *xml,CIClientLink *pBusLink,CMemDb *pMemDb,	CXdp *pXdp,	CIErrlog *pLog,CBF_BufferPool *pMemPool,CDbApi *pSqlCmd,CIExchLink *pExchLink,int index)
{
	m_pClientLink = pBusLink;
	m_pXdp = pXdp;
	if (m_pXdp == NULL)
	{
		return false;
	}
	if (!m_pSendXdp.XdpCopy(m_pXdp))
	{
		return false;
	}
	m_pMemDb= pMemDb;
	m_pLog = pLog;
	m_pSqlCmd = pSqlCmd;
	m_pExchLink = pExchLink;
	m_pMemPool = pMemPool;
	m_nIndex = index;
	m_pKernelPub.m_pMemDb = pMemDb;
	m_pKernelPub.m_pLog = pLog;
	m_nCallSerial = m_nIndex*1000000+1;
	return true;
}


int RiskDll::RcvQuotation(S_TRADE_DATA &data)
{
	if (!m_pXdp->FromBuffer(data.pData->sBuffer,data.pData->sDBHead.nLen,m_sErrMsg))
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"RcvQuotation Xdp FromBuffer error <%s>",m_sErrMsg);
		m_pClientLink->PoolFree(data);
		return -1;
	}
	//收到行情后不需要应答，直接释放空间
	m_pClientLink->PoolFree(data);

	

	//接收行情
	//更新行情表
	//若发生变化，去更新持仓表的浮动盈亏
	//更新完后，置风险计算标志，另外定时去统计各个风险度等

	S_MDB_QUOTATION qinfo;
	bzero(&qinfo,sizeof(S_MDB_QUOTATION));
	int fieldtype;
	unsigned int len;
	len = sizeof(qinfo.sContractCode);
	if (!m_pXdp->GetFieldValue((unsigned short)100,qinfo.sContractCode,len,fieldtype,m_sErrMsg))
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"GetFieldValue error [%s]!",m_sErrMsg);
		return -1;
	}
	if (!m_pXdp->GetFieldValue((unsigned short)26,qinfo.nContractNo,m_sErrMsg))
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"GetFieldValue error [%s]!",m_sErrMsg);
		return -1;
	}

	//取出行情日期、行情序号
	if (!m_pXdp->GetFieldValue((unsigned short)23,qinfo.nDate,m_sErrMsg))
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"GetFieldValue error [%s]!",m_sErrMsg);
		return -1;
	}
	if (!m_pXdp->GetFieldValue((unsigned short)25,qinfo.nQuoSerial,m_sErrMsg))
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"GetFieldValue error [%s]!",m_sErrMsg);
		return -1;
	}
		
	len = sizeof(qinfo.sContractName);
	m_pXdp->GetFieldValue((unsigned short)101,qinfo.sContractName,len,fieldtype,m_sErrMsg);
	m_pXdp->GetFieldValue((unsigned short)60,qinfo.dLastSetllePrice,m_sErrMsg);
	m_pXdp->GetFieldValue((unsigned short)61,qinfo.dLastClosePrice,m_sErrMsg);
	m_pXdp->GetFieldValue((unsigned short)62,qinfo.dOpenPrice,m_sErrMsg);
	m_pXdp->GetFieldValue((unsigned short)63,qinfo.dHighPrice,m_sErrMsg);
	m_pXdp->GetFieldValue((unsigned short)64,qinfo.dLowPrice,m_sErrMsg);
	m_pXdp->GetFieldValue((unsigned short)65,qinfo.dNewPrice,m_sErrMsg);
	m_pXdp->GetFieldValue((unsigned short)66,qinfo.dClosePrice,m_sErrMsg);
	m_pXdp->GetFieldValue((unsigned short)67,qinfo.dSettlePrice,m_sErrMsg);
	m_pXdp->GetFieldValue((unsigned short)70,qinfo.dBuyPrice1,m_sErrMsg);
	m_pXdp->GetFieldValue((unsigned short)71,qinfo.dBuyPrice2,m_sErrMsg);
	m_pXdp->GetFieldValue((unsigned short)72,qinfo.dBuyPrice3,m_sErrMsg);
	m_pXdp->GetFieldValue((unsigned short)73,qinfo.dBuyPrice4,m_sErrMsg);
	m_pXdp->GetFieldValue((unsigned short)74,qinfo.dBuyPrice5,m_sErrMsg);
	m_pXdp->GetFieldValue((unsigned short)75,qinfo.dBuyPrice6,m_sErrMsg);
	m_pXdp->GetFieldValue((unsigned short)76,qinfo.dBuyPrice7,m_sErrMsg);
	m_pXdp->GetFieldValue((unsigned short)77,qinfo.dBuyPrice8,m_sErrMsg);
	m_pXdp->GetFieldValue((unsigned short)78,qinfo.dBuyPrice9,m_sErrMsg);
	m_pXdp->GetFieldValue((unsigned short)79,qinfo.dBuyPrice10,m_sErrMsg);
	m_pXdp->GetFieldValue((unsigned short)40,qinfo.nBuy1,m_sErrMsg);
	m_pXdp->GetFieldValue((unsigned short)41,qinfo.nBuy2,m_sErrMsg);
	m_pXdp->GetFieldValue((unsigned short)42,qinfo.nBuy3,m_sErrMsg);
	m_pXdp->GetFieldValue((unsigned short)43,qinfo.nBuy4,m_sErrMsg);
	m_pXdp->GetFieldValue((unsigned short)44,qinfo.nBuy5,m_sErrMsg);
	m_pXdp->GetFieldValue((unsigned short)45,qinfo.nBuy6,m_sErrMsg);
	m_pXdp->GetFieldValue((unsigned short)46,qinfo.nBuy7,m_sErrMsg);
	m_pXdp->GetFieldValue((unsigned short)47,qinfo.nBuy8,m_sErrMsg);
	m_pXdp->GetFieldValue((unsigned short)48,qinfo.nBuy9,m_sErrMsg);
	m_pXdp->GetFieldValue((unsigned short)49,qinfo.nBuy10,m_sErrMsg);
	m_pXdp->GetFieldValue((unsigned short)80,qinfo.dSellPrice1,m_sErrMsg);
	m_pXdp->GetFieldValue((unsigned short)81,qinfo.dSellPrice2,m_sErrMsg);
	m_pXdp->GetFieldValue((unsigned short)82,qinfo.dSellPrice3,m_sErrMsg);
	m_pXdp->GetFieldValue((unsigned short)83,qinfo.dSellPrice4,m_sErrMsg);
	m_pXdp->GetFieldValue((unsigned short)84,qinfo.dSellPrice5,m_sErrMsg);
	m_pXdp->GetFieldValue((unsigned short)85,qinfo.dSellPrice6,m_sErrMsg);
	m_pXdp->GetFieldValue((unsigned short)86,qinfo.dSellPrice7,m_sErrMsg);
	m_pXdp->GetFieldValue((unsigned short)87,qinfo.dSellPrice8,m_sErrMsg);
	m_pXdp->GetFieldValue((unsigned short)88,qinfo.dSellPrice9,m_sErrMsg);
	m_pXdp->GetFieldValue((unsigned short)89,qinfo.dSellPrice10,m_sErrMsg);
	m_pXdp->GetFieldValue((unsigned short)50,qinfo.nSell1,m_sErrMsg);
	m_pXdp->GetFieldValue((unsigned short)51,qinfo.nSell2,m_sErrMsg);
	m_pXdp->GetFieldValue((unsigned short)52,qinfo.nSell3,m_sErrMsg);
	m_pXdp->GetFieldValue((unsigned short)53,qinfo.nSell4,m_sErrMsg);
	m_pXdp->GetFieldValue((unsigned short)54,qinfo.nSell5,m_sErrMsg);
	m_pXdp->GetFieldValue((unsigned short)55,qinfo.nSell6,m_sErrMsg);
	m_pXdp->GetFieldValue((unsigned short)56,qinfo.nSell7,m_sErrMsg);
	m_pXdp->GetFieldValue((unsigned short)57,qinfo.nSell8,m_sErrMsg);
	m_pXdp->GetFieldValue((unsigned short)58,qinfo.nSell9,m_sErrMsg);
	m_pXdp->GetFieldValue((unsigned short)59,qinfo.nSell10,m_sErrMsg);
	m_pXdp->GetFieldValue((unsigned short)20,qinfo.nDoneQty,m_sErrMsg);
	m_pXdp->GetFieldValue((unsigned short)121,qinfo.dDoneWeight,m_sErrMsg);
	m_pXdp->GetFieldValue((unsigned short)21,qinfo.nPosition,m_sErrMsg);
	m_pXdp->GetFieldValue((unsigned short)122,qinfo.dUpLimit,m_sErrMsg);
	m_pXdp->GetFieldValue((unsigned short)123,qinfo.dDownLimit,m_sErrMsg);
	m_pXdp->GetFieldValue((unsigned short)127,qinfo.dUpDown,m_sErrMsg);
	m_pXdp->GetFieldValue((unsigned short)124,qinfo.dUpDownRate,m_sErrMsg);
	m_pXdp->GetFieldValue((unsigned short)125,qinfo.dDownAmt,m_sErrMsg);
	m_pXdp->GetFieldValue((unsigned short)126,qinfo.dAvp,m_sErrMsg);

	S_MDB_CONTRACT_INFO *pc = m_pMemDb->m_pDbBaseinfo.m_pContract.Select(qinfo.nContractNo);
	if (pc == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"合约[%d %s]在合约表中不存在",qinfo.nContractNo,qinfo.sContractCode);
		return -1;
	}
	strcpy(qinfo.sContractCode,pc->sContractCode);
	bool bret = m_pMemDb->m_pDbTrader.m_pQuotation.Update(qinfo);
	if (bret)
	{
		m_pMemDb->m_bIsNewQuotation = true;
		
		if (qinfo.dSettlePrice > MAXDOUBLECOMPARE)
		{
			//结算价有值，更新到数据库表里面
			UpdateSetllePrice(qinfo);
		}

		if (qinfo.dNewPrice > 99999999 || qinfo.dNewPrice <-99999999) //错误行情
		{
			return 0;
		}
		//查看是否是风险合约
		//处理是否风险合约
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"开始计算风险合约[%d %s] 行情[%d %d]",qinfo.nContractNo,qinfo.sContractCode,qinfo.nDate,qinfo.nQuoSerial);
		ProcRiskContract(qinfo,pc);
		//盈亏计算 风险度计算
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"开始计算盈亏 合约[%d %s] 行情[%d %d]",qinfo.nContractNo,qinfo.sContractCode,qinfo.nDate,qinfo.nQuoSerial);
		ProcRisk(qinfo,pc);

		//针对LME 3M合约，有到期日的合约按3M合约最新价计算盈亏
		if (strcmp(pc->sContractCode,"LME+CA+R+3M") == 0)
		{
			m_pDate.Update();
			char scurdate[9];
			m_pDate.ToCharDate(scurdate);
			std::vector<S_MDB_CONTRACT_INFO *> contlist;
			if (m_pMemDb->m_pDbBaseinfo.m_pContract.SelectByExch((char *)"B",contlist))
			{
				for (int k=0; k<contlist.size(); k++)
				{
					pc = contlist[k];
					if (  strncmp(pc->sContractCode,"LME+CA+S+",9)==0 && strncmp(pc->sContractCode+9,scurdate,8)>= 0 ) //3m对应的合约
					{
						//进行风险计算
						qinfo.nContractNo = pc->nContractId;
						strcpy(qinfo.sContractCode,pc->sContractCode);
						strcpy(qinfo.sContractName,pc->sContractName);
						//处理是否风险合约
						m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"开始计算风险合约[%d %s] 行情[%d %d]",qinfo.nContractNo,qinfo.sContractCode,qinfo.nDate,qinfo.nQuoSerial);
						ProcRiskContract(qinfo,pc);
						//盈亏计算 风险度计算
						m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"开始计算盈亏 合约[%d %s] 行情[%d %d]",qinfo.nContractNo,qinfo.sContractCode,qinfo.nDate,qinfo.nQuoSerial);
						ProcRisk(qinfo,pc);
					}
				}
			}
		}
		
		return 0;
	}
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"合约[%d %s] 行情[%d %d]不是最新",qinfo.nContractNo,qinfo.sContractCode,qinfo.nDate,qinfo.nQuoSerial);
	return 0;
}


void RiskDll::UpdateSetllePrice(S_MDB_QUOTATION qinfo)
{
	if (m_pSqlCmd == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"Not connect risk database");
		return ;
	}
	if (qinfo.dSettlePrice > 99999999 || qinfo.dSettlePrice <-99999999)
	{
		return ;
	}
	char tmpchar[10];
	sprintf(tmpchar,"%d",qinfo.nDate);
	sprintf(m_sSql,"select SETTLE_PRICE from CONTRACT_SETTLE_PRICE where EXCH_DATE=:1 and CONTRACT_NO=:2");
	m_pSqlCmd->setCommandText(m_sSql);
	m_pSqlCmd->Param(1).setAsString() = tmpchar;
	m_pSqlCmd->Param(2).setAsLong() = qinfo.nContractNo;
	if (!m_pSqlCmd->Execute())
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"Execute %s error %s",m_sSql,m_pSqlCmd->GetCmdErrInfo());
		return ;
	}
	if (qinfo.dClosePrice > 99999999 || qinfo.dClosePrice <-99999999)
	{
		qinfo.dClosePrice =0.0;
	}
	if (qinfo.dHighPrice > 99999999 || qinfo.dHighPrice <-99999999)
	{
		qinfo.dHighPrice =0.0;
	}
	if (qinfo.dLowPrice > 99999999 || qinfo.dLowPrice <-99999999)
	{
		qinfo.dLowPrice =0.0;
	}
	if (qinfo.dOpenPrice > 99999999 || qinfo.dOpenPrice <-99999999)
	{
		qinfo.dOpenPrice =0.0;
	}
	if (!m_pSqlCmd->FetchNext())
	{
		sprintf(m_sSql,"insert into CONTRACT_SETTLE_PRICE(EXCH_DATE,CONTRACT_NO,CONTRACT_CODE,SETTLE_PRICE,HOLD_POSI,CLOSE_PRICE,PRICE_CHANGE,SIMPLE_RETURN,LOG_RETURN,HIGH,LOW,OPEN) values(:1,:2,:3,:4,:5,:6,:7,:8,:9,:10,:11,:12)");
		m_pSqlCmd->setCommandText(m_sSql);
		m_pSqlCmd->Param(1).setAsString() = tmpchar;
		m_pSqlCmd->Param(2).setAsLong() = qinfo.nContractNo;
		m_pSqlCmd->Param(3).setAsString() = qinfo.sContractCode;
		m_pSqlCmd->Param(4).setAsDouble() = qinfo.dSettlePrice;
		m_pSqlCmd->Param(5).setAsLong() = qinfo.nPosition;
		m_pSqlCmd->Param(6).setAsDouble() = qinfo.dClosePrice;
		m_pSqlCmd->Param(7).setAsDouble() = 0.0;
		m_pSqlCmd->Param(8).setAsDouble() = 0.0;
		m_pSqlCmd->Param(9).setAsDouble() = 0.0;
		m_pSqlCmd->Param(10).setAsDouble() = qinfo.dHighPrice;
		m_pSqlCmd->Param(11).setAsDouble() = qinfo.dLowPrice;
		m_pSqlCmd->Param(12).setAsDouble() = qinfo.dOpenPrice;
		if (!m_pSqlCmd->Execute())
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"Execute %s error %s",m_sSql,m_pSqlCmd->GetCmdErrInfo());
			return ;
		}
		m_pSqlCmd->Commit();
		
	}
	else
	{
		sprintf(m_sSql,"update CONTRACT_SETTLE_PRICE set SETTLE_PRICE=:1,HOLD_POSI=:2,CLOSE_PRICE=:3,HIGH=:4,LOW=:5,OPEN=:6  where EXCH_DATE=:7 and CONTRACT_NO=:8");
		m_pSqlCmd->setCommandText(m_sSql);
		m_pSqlCmd->Param(1).setAsDouble() = qinfo.dSettlePrice;
		m_pSqlCmd->Param(2).setAsLong() = qinfo.nPosition;
		m_pSqlCmd->Param(3).setAsDouble() = qinfo.dClosePrice;
		m_pSqlCmd->Param(4).setAsDouble() = qinfo.dHighPrice;
		m_pSqlCmd->Param(5).setAsDouble() = qinfo.dLowPrice;
		m_pSqlCmd->Param(6).setAsDouble() = qinfo.dOpenPrice;
		m_pSqlCmd->Param(7).setAsString() = tmpchar;
		m_pSqlCmd->Param(8).setAsLong() = qinfo.nContractNo;
		
		if (!m_pSqlCmd->Execute())
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"Execute %s error %s",m_sSql,m_pSqlCmd->GetCmdErrInfo());
			return ;
		}
		m_pSqlCmd->Commit();
	}
	return;
}



void RiskDll::ProcRiskContract(S_MDB_QUOTATION qinfo,S_MDB_CONTRACT_INFO *pc)
{
	m_nBeginTime = CBF_Date_Time::GetTickUS();
	unsigned int nVarietyNo;
	switch (pc->sProd_type[0])
	{
		case PROD_TYPE_FUTURES:
			nVarietyNo = pc->nProdId;
			break;
		case PROD_TYPE_SPOT:
			nVarietyNo = pc->nProdId;
			break;
		case PROD_TYPE_SEC:
			nVarietyNo = 0;
			break;
		case PROD_TYPE_OPTION:
			S_MDB_CONTRACT_INFO *pc2 = m_pMemDb->m_pDbBaseinfo.m_pContract.Select(pc->nProdId);
			if (pc2 == NULL)
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"合约[%d]在合约表中不存在",pc->nProdId);
				return ;
			}
			nVarietyNo = pc2->nProdId;
			break;
	}
	std::vector<S_MDB_RISKCONTRACT_SET *> reslist;
	if (!m_pMemDb->m_pDbSet.m_pRiskContractSet.Select(nVarietyNo,qinfo.nContractNo,reslist))
	{
		//按品种再取
		if (!m_pMemDb->m_pDbSet.m_pRiskContractSet.Select(nVarietyNo,0,reslist))
		{
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"品种[%d]合约[%d %s]未设置风险参数",nVarietyNo,qinfo.nContractNo,qinfo.sContractCode);
			return ;
		}
	}
	bool bIsRC=false;
	S_MDB_RISKCONTRACT_INFO rinfo;
	bzero(&rinfo,sizeof(S_MDB_RISKCONTRACT_INFO));
	rinfo.nContractNo = qinfo.nContractNo;
	strcpy(rinfo.sContractCode,qinfo.sContractCode);
	rinfo.dBuyPrice = qinfo.dBuyPrice1;
	rinfo.dHighPrice = qinfo.dHighPrice;
	rinfo.dLowPrice = qinfo.dLowPrice;
	rinfo.dNewPrice = qinfo.dNewPrice;
	rinfo.dSellPrice = qinfo.dSellPrice1;
	rinfo.dUpDown = qinfo.dUpDown;
	rinfo.dUpDownRate = qinfo.dUpDownRate;
	rinfo.nDoneQty = qinfo.nDoneQty;
	CBF_Date_Time pEnddate;
	CBF_Date_Time pbegindate;
	for (int i=0; i<reslist.size(); i++)
	{
		switch (reslist[i]->sWarnItem[0])
		{
			case WARN_ITEM_UPDOWN:
				if (qinfo.dUpDown>reslist[i]->dValue || (qinfo.dUpDown <MINDOUBLECOMPARE && (0.0 - qinfo.dUpDown) >reslist[i]->dValue) ) //风险合约
				{
					bIsRC = true;
					strcpy(rinfo.sRiskDesc,"涨跌超过限制");
					strcpy(rinfo.sRiskFlag,"1");
				}
				break;
			case WARN_ITEM_UPDOWNRATE:
				if (qinfo.dUpDownRate>reslist[i]->dValue || (qinfo.dUpDownRate <MINDOUBLECOMPARE &&  (0.0 - qinfo.dUpDownRate) > reslist[i]->dValue)) //风险合约
				{
					bIsRC = true;
					strcpy(rinfo.sRiskDesc,"涨跌幅超过限制");
					strcpy(rinfo.sRiskFlag,"1");
				}
				break;
			case WARN_ITEM_DONENUM:
				if ((double)qinfo.nDoneQty>reslist[i]->dValue) //风险合约
				{
					bIsRC = true;
					strcpy(rinfo.sRiskDesc,"成交量超过限制");
					strcpy(rinfo.sRiskFlag,"1");
				}
				break;
			case WARN_ITEM_DUEDAYS:
				if (strlen(pc->sDueDate)!=8)
				{
					break;
				}
				pEnddate.Set(pc->sDueDate);
				if (double(pEnddate - pbegindate) < reslist[i]->dValue)
				{
					bIsRC = true;
					sprintf(rinfo.sRiskDesc,"离到期日%d天 设定[%d]",pEnddate - pbegindate,reslist[i]->dValue);
					strcpy(rinfo.sRiskFlag,"1");
				}
				break;
			case WARN_ITEM_IV:
				break;
			case WARN_ITEM_DELTA:
				break;
			case WARN_ITEM_GAMMA:
				break;
			case WARN_ITEM_THETA:
				break;
			case WARN_ITEM_VEGA:
				break;
			default:
				break;
		}
		if (!bIsRC)
		{
			continue;
		}
		else
		{
			break;
		}
	}
	if (!bIsRC)
	{
		strcpy(rinfo.sRiskDesc,"取消风险");
		strcpy(rinfo.sRiskFlag,"0");
	}
	S_MDB_RISKCONTRACT_INFO *rc = m_pMemDb->m_pDbTrader.m_pRiskContractInfo.Select(qinfo.nContractNo);
	//判断是否要发风险合约通知
	bool issend=false;
	if (rinfo.sRiskFlag[0] == '1')
	{
		if (rc == NULL  ||  (rc != NULL && rc->sRiskFlag[0] == '0'))
		{
			issend = true;
		}
	}
	else
	{
		if (rc != NULL && rc->sRiskFlag[0] == '1')
		{
			issend = true;
		}
	}
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"合约[%d %s] 风险标志[%s %s] 是否发送[%d]",rinfo.nContractNo,rinfo.sContractCode,rinfo.sRiskFlag,rinfo.sRiskDesc,issend);
	m_pMemDb->m_pDbTrader.m_pRiskContractInfo.Update(rinfo);
	if (issend)
	{
		//发送风险合约通知
		S_TRADE_DATA sdata;
//		sdata.pData = (PBPCCOMMSTRU)m_pMemPool->PoolMalloc();
		if (m_pClientLink->PoolMalloc(sdata)<0)
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"PoolMalloc error");
			return ;
		}
		m_pXdp->ResetData();
		m_pXdp->SetFieldValue((unsigned short)20,(unsigned int)rinfo.nContractNo,m_sErrMsg);
		m_pXdp->SetFieldValue((unsigned short)100,rinfo.sContractCode,strlen(rinfo.sContractCode),m_sErrMsg);
		m_pXdp->SetFieldValue((unsigned short)60,rinfo.dNewPrice,m_sErrMsg);
		m_pXdp->SetFieldValue((unsigned short)61,rinfo.dUpDown,m_sErrMsg);
		m_pXdp->SetFieldValue((unsigned short)62,rinfo.dUpDownRate,m_sErrMsg);
		m_pXdp->SetFieldValue((unsigned short)63,rinfo.dBuyPrice,m_sErrMsg);
		m_pXdp->SetFieldValue((unsigned short)64,rinfo.dSellPrice,m_sErrMsg);
		m_pXdp->SetFieldValue((unsigned short)65,rinfo.dHighPrice,m_sErrMsg);
		m_pXdp->SetFieldValue((unsigned short)66,rinfo.dLowPrice,m_sErrMsg);
		m_pXdp->SetFieldValue((unsigned short)21,(unsigned int)rinfo.nDoneQty,m_sErrMsg);
		m_pXdp->SetFieldValue((unsigned short)101,rinfo.sRiskDesc,strlen(rinfo.sRiskDesc),m_sErrMsg);
		m_pXdp->SetFieldValue((unsigned short)102,rinfo.sRiskFlag,strlen(rinfo.sRiskFlag),m_sErrMsg);
		unsigned int len = BPUDATASIZE;
		if (!m_pXdp->ToBuffer(sdata.pData->sBuffer,len,m_sErrMsg))
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"Xdp ToBuffer error");
			m_pClientLink->PoolFree(sdata);
			return ;
		}
		bzero(&(sdata.pData->sDBHead),DREBHEADLEN);
		sdata.pData->sDBHead.cRaflag = 0;
		sdata.pData->sDBHead.cCmd = CMD_DPABC;
		sdata.pData->sDBHead.d_Dinfo.d_nServiceNo = 9817;
		sdata.src = 0;
		sdata.pData->sDBHead.nLen = len;
		m_pClientLink->Send2Client(sdata);
	}
	m_nEndTime = CBF_Date_Time::GetTickUS();
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"计算风险合约完成 耗时[%ld]",m_nEndTime-m_nBeginTime);
	return ;
}

void RiskDll::ProcRisk(S_MDB_QUOTATION info,S_MDB_CONTRACT_INFO *pc)
{
	m_nBeginTime = CBF_Date_Time::GetTickUS();
	std::vector<S_MDB_CUST_POSITION *> pPosiList;
	if (!m_pMemDb->m_pDbTrader.m_pPosition.SelectByContract(info.nContractNo,pPosiList))
	{
		//合约无持仓
		return ;
	}
	
	for (int i=0 ;i<pPosiList.size(); i++)
	{
		ComputeSurplus(info,pc,pPosiList[i]);
	}
	m_nEndTime = CBF_Date_Time::GetTickUS();
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"合约[%d %s]行情[%d %d]变化 计算盈亏耗时[%ld]微秒",pc->nContractId,pc->sContractCode,info.nDate,info.nQuoSerial,m_nEndTime-m_nBeginTime);
	//计算完盈亏就行了，在另外的定时任务，根据m_pSurplusLockRowId去计算资金账号的风险度风险等级
}

void RiskDll::ComputeSurplus(S_MDB_QUOTATION info, S_MDB_CONTRACT_INFO *pc, S_MDB_CUST_POSITION *posi)
{
	//更新持仓记录的持仓盈亏
	double oldsurplus = posi->dDAY_PB_SURPLUS+posi->dDAY_PS_SURPLUS;
	posi->dDAY_PB_SURPLUS = pc->dTxUnit*posi->nCurPB*(info.dNewPrice-posi->dPB_Avp);
	posi->dDAY_PS_SURPLUS = pc->dTxUnit*posi->nCurPS*(posi->dPS_Avp-info.dNewPrice);
	//算出和原来的浮动盈亏的差
	double surplus =posi->dDAY_PB_SURPLUS+posi->dDAY_PS_SURPLUS-oldsurplus;   //= posi->dDAY_PB_SURPLUS+posi->dDAY_PS_SURPLUS;
	//更新客户的浮动盈亏到资金表
	char fundflag = FUND_TYPE_FLOATSURPLUS;
	char fundcode[7];
	if (pc->sProd_type[0] == PROD_TYPE_FUTURES || pc->sProd_type[0] == PROD_TYPE_OPTION) 
	{
		//持仓
		strcpy(fundcode,FUND_CODE_FLOATSURPLUS_POSI);
	}
	else
	{
		strcpy(fundcode,FUND_CODE_FLOATSURPLUS);
	}
	int rowid;
	if (!UpdateFundLock(posi->sTxAcc,pc->sCurrency,fundflag,fundcode,surplus,true,rowid))
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"UpdateFund 出错");
		return;
	}
	//合计所有浮动盈亏
	//double surplus =0.0;
	//std::vector<S_MDB_CUST_POSITION *> posilist;
	//if (!m_pMemDb->m_pDbTrader.m_pPosition.SelectByAcc(posi->sTxAcc,posilist))
	//{
	//	m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"取账户[%s]所有的持仓出错",posi->sTxAcc);
	//	return;
	//}
	//for (int i=0; i<posilist.size(); i++)
	//{
	//	surplus +=posilist[i]->dDAY_PB_SURPLUS;
	//	surplus +=posilist[i]->dDAY_PS_SURPLUS;
	//}
	//int rowid;
	//if (!UpdateFundLock(posi->sTxAcc,fundflag,fundcode,surplus,false,rowid))
	//{
	//	m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"UpdateFund 出错");
	//	return;
	//}
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"已计算盈亏的客户锁表rowid %d",rowid);
	MsgCustRiskCalc(rowid);
	return ;
}

bool RiskDll::GetAccList(std::vector<int> &acclist)
{
	CBF_PMutex pp(&(m_pMemDb->m_pSurplusLockRowIdLock));
	int id=0;
	bool bret = m_pMemDb->m_pSurplusLockRowId.First(id);
	while (bret)
	{
		acclist.push_back(id);
		bret = m_pMemDb->m_pSurplusLockRowId.Next(id);
	}
	m_pMemDb->m_pSurplusLockRowId.Clear();
	return true;
}

void RiskDll::ComputeCustRisk(int rowid)
{
	S_MDB_TXACC_LOCK *info = m_pMemDb->SelectLock(rowid);
	if (info == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"资金账号锁表rowid[%d]不存在",rowid);
		return ;
	}
	std::vector<S_MDB_CUST_FUND *>fundlist;
	if (!m_pMemDb->m_pDbTrader.m_pFund.Select(info->sTxAcc,fundlist))
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"资金账号[%s] 在资金表里不存在",info->sTxAcc);
		return ;
	}
	S_MDB_CUST_FUND *fund=NULL;
	S_MDB_CUST_FUNDACC_INFO *fundacc = m_pMemDb->m_pDbCust.m_pCustFundAcc.At(info->nFundAccRowid);
	if (fundacc == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"fundacc 没找到");
	}
	m_pDate.Update();
	char sTime[9];
	strcpy(sTime,m_pDate.ToStringT6().c_str());
	int nRiskOld;
	int nRiskNew;
	for (int i=0; i<fundlist.size(); i++)
	{
		fund = fundlist[i];
		//计算风险度，得到风险级别
		double oldrisk=fund->dRisk;
		int oldrisklevel=fund->nRisk_level;
		char sTradeRight[2]; 
		bzero(sTradeRight,sizeof(sTradeRight));
		if (fund->dBalance > MINDOUBLECOMPARE && fund->dBalance < MAXDOUBLECOMPARE )//结存为0
		{
			if (fund->dMargin+fund->dFrozeFund > MAXDOUBLECOMPARE)//说明有保证金
			{
				fund->dRisk = 2.00;
			}
			else
			{
				fund->dRisk = 0.00;
			}
		}
		else if  (fund->dBalance < MINDOUBLECOMPARE) //结存为负数
		{
			if (fund->dMargin+fund->dFrozeFund > MAXDOUBLECOMPARE)//说明有保证金
			{
				fund->dRisk = 2.00;
			}
			else //无保证金但结存为负数，风险为1
			{
				fund->dRisk = 1.00;
			}
		}
		else //  保证金/结存
		{
			fund->dRisk = 1.00*(fund->dMargin+fund->dFrozeFund)/fund->dBalance;
		}
		
		strcpy(fund->sRiskDesc,"");
		//取得风险级别
		if (m_pKernelPub.GetRiskLevel(fund,sTradeRight)<0)
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"取风险级别出错");
			return ;
		}
		//写日间风险表
		nRiskOld = oldrisk*10000;
		nRiskNew = fund->dRisk*10000;
		fund->nRiskdate = atoi(m_pDate.ToStringD8().c_str());
		fund->nRisktime = atoi(m_pDate.ToStringT6().c_str());
		if (nRiskNew!=nRiskOld) //和上次相比，不相同则写入
		{
			S_MDB_CUST_RISK_TICK risktick;
			strcpy(risktick.sTxAcc,fund->sTxAcc);
			strcpy(risktick.sCurrency,fund->sCurrency);
			strcpy(risktick.sTime,sTime);
			risktick.dRisk = fund->dRisk;
			risktick.nRisk_level = fund->nRisk_level;
			m_pMemDb->m_pDbTrader.m_pRiskTick.Insert(risktick);
		}
		fund->sTradeRight[0] = sTradeRight[0];
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"计算资金账号[%s %s]风险度完成  风险度[%lf] 风险级别[%d] 交易权限[%s]",fund->sTxAcc,fund->sCurrency,fund->dRisk,fund->nRisk_level,fund->sTradeRight);
		if (oldrisklevel != fund->nRisk_level)
		{
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"发送风险通知 资金账号[%s]  风险度[%lf] 风险级别[%d] 交易权限[%s]",fund->sTxAcc,fund->dRisk,fund->nRisk_level,fund->sTradeRight);
			//风险级别不一样了，发送通知
			SendRiskMsg(info,fund,sTradeRight[0]);
		}
	}
	return ;

}
void RiskDll::ComputeCustPosiRisk(int rowid)
{
	S_MDB_TXACC_LOCK *info = m_pMemDb->SelectLock(rowid);
	if (info == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"资金账号锁表rowid[%d]不存在",rowid);
		return ;
	}
	S_MDB_CUST_FUNDACC_INFO *pFundAcc = m_pMemDb->m_pDbCust.m_pCustFundAcc.At(info->nFundAccRowid);
	if (pFundAcc == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"资金账号rowid[%d]不存在",info->nFundAccRowid);
		return ;
	}
	S_MDB_CUSTINFO *pCust= m_pMemDb->m_pDbCust.m_pCustom.At(info->nCustRowid);
	if (pCust == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"客户rowid[%d]不存在",info->nCustRowid);
		return ;
	}
	//计算持仓风险
	std::vector<S_MDB_CUST_POSITION *> posilist;
	if (!m_pMemDb->m_pDbTrader.m_pPosition.SelectByAcc(info->sTxAcc,posilist))
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"资金账号[%s]无持仓",info->sTxAcc);
		return;
	}
	//取资金账号对应的模板
	S_MDB_CUST_TEMPLET_RELA *templet = m_pMemDb->m_pDbSet.m_pCustTemplet.Select(info->sTxAcc);
	if (templet == NULL)
	{
		//没有找到资金账号对应的模板,取$对应的模板
		templet = m_pMemDb->m_pDbSet.m_pCustTemplet.Select("$");
		if (templet == NULL)
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"资金账号[%s] 没有对应的持仓模板 ",info->sTxAcc);
			return ;
		}
	}

	S_MDB_TEMPLET_INFO *pPosiTemplet = m_pMemDb->m_pDbSet.m_pPosiTemplet.Select(templet->sPosiTempletId);
	if (pPosiTemplet == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"持仓模板[%s]不存在",templet->sPosiTempletId);
		return ;
	}
	//根据模板ID，从持仓模板设置表查找
	S_MDB_ORG_INFO *pOrg = NULL;
	int warnno=10;
	char warnmsg[81];
	char traderight[2];
	char sRight[2];

	CBF_Slist slist;
	slist.SetSeparateString("-");
	int nRet;
	S_MDB_CUST_POSI_RISK rinfo;
	bzero(m_sErrMsg,sizeof(m_sErrMsg));
	bzero(warnmsg,sizeof(warnmsg));
	bzero(traderight,sizeof(traderight));
	bzero(sRight,sizeof(sRight));
	S_MDB_CUST_POSITION *pPosi=NULL;
	m_pDate.Update();
	for (int i=0; i<posilist.size(); i++)
	{
		pPosi = posilist[i];
		if (pPosi->nCurPB <=0 && pPosi->nCurPS <=0)
		{
			continue;
		}
		bzero(warnmsg,sizeof(warnmsg));
		//取持仓对应的风险级别
		warnno = m_pKernelPub.GetPosiRiskLevel(info,templet,posilist[i],traderight,warnmsg);
		if (warnno<0)
		{
			//没有此合约持仓风险设置
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"账户[%s] 模板[%s] 取持仓风险错",info->sTxAcc,templet->sPosiTempletId);
			continue;
		}
		bool issend = false;

		//得到最新的预警序号，和原来比对，如果不变不用发通知，否则发通知
		S_MDB_CUST_POSI_RISK *risk =  m_pMemDb->m_pDbTrader.m_pCustPosiRisk.Select(info->sTxAcc,posilist[i]->sContractCode);
		if (risk == NULL)
		{
			//增加并通知
			bzero(&rinfo,sizeof(S_MDB_CUST_POSI_RISK));
			rinfo.nWarnLevel = warnno;
			strcpy(rinfo.sTxAcc,info->sTxAcc);
			rinfo.nBuyQty = posilist[i]->nCurPB;
			rinfo.nSellQty = posilist[i]->nCurPS;
			rinfo.nContractNo = posilist[i]->nContractNo;
			strcpy(rinfo.sContractCode,posilist[i]->sContractCode);
			strcpy(rinfo.sShFlag,posilist[i]->sShFlag);
			rinfo.nRiskdate = atoi(m_pDate.ToStringD8().c_str());
			rinfo.nRisktime = atoi(m_pDate.ToStringT6().c_str());
			if (warnno  == 10) //无风险
			{
				strcpy(rinfo.sRiskFlag,"0");
			}
			else
			{
				strcpy(rinfo.sRiskFlag,"1");
			}
			strcpy(rinfo.sRiskDesc,warnmsg);

			nRet = m_pMemDb->m_pDbTrader.m_pCustPosiRisk.Insert(rinfo);
			if (nRet <0)
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"写客户持仓风险表出错");
				return;
			}
			risk = m_pMemDb->m_pDbTrader.m_pCustPosiRisk.At(nRet);
			if (risk == NULL)
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"写客户持仓风险表出错");
				return;
			}
			issend = true;
		}
		else
		{
			risk->nRiskdate = atoi(m_pDate.ToStringD8().c_str());
			risk->nRisktime = atoi(m_pDate.ToStringT6().c_str());
			if (warnno < risk->nWarnLevel )//级别变高
			{
				//更新并通知
				risk->nWarnLevel = warnno;
				strcpy(risk->sTxAcc,info->sTxAcc);
				risk->nBuyQty = posilist[i]->nCurPB;
				risk->nSellQty = posilist[i]->nCurPS;
				risk->nContractNo = posilist[i]->nContractNo;
				strcpy(risk->sContractCode,posilist[i]->sContractCode);
				strcpy(risk->sShFlag,posilist[i]->sShFlag);
				if (warnno  == 10) //无风险
				{
					strcpy(risk->sRiskFlag,"0");
				}
				else
				{
					strcpy(risk->sRiskFlag,"1");
				} 
				strcpy(risk->sRiskDesc,warnmsg);
				issend = true;
			}
			else if (warnno == risk->nWarnLevel )//级别相同
			{
				//更新风险表
				risk->nBuyQty = posilist[i]->nCurPB;
				risk->nSellQty = posilist[i]->nCurPS;
				risk->nContractNo = posilist[i]->nContractNo;
				strcpy(risk->sContractCode,posilist[i]->sContractCode);
				strcpy(risk->sShFlag,posilist[i]->sShFlag);
				strcpy(risk->sRiskDesc,warnmsg);
				if (warnno  == 10) //无风险
				{
					strcpy(risk->sRiskFlag,"0");
				}
				else
				{
					strcpy(risk->sRiskFlag,"1");
				} 
			}
			else 
			{
				risk->nWarnLevel = warnno;
				strcpy(risk->sTxAcc,info->sTxAcc);
				risk->nBuyQty = posilist[i]->nCurPB;
				risk->nSellQty = posilist[i]->nCurPS;
				risk->nContractNo = posilist[i]->nContractNo;
				strcpy(risk->sContractCode,posilist[i]->sContractCode);
				strcpy(risk->sShFlag,posilist[i]->sShFlag);
				strcpy(risk->sRiskDesc,warnmsg);
				if (warnno  == 10) //无风险
				{
					strcpy(risk->sRiskFlag,"0");
				}
				else //风险降低
				{
					strcpy(risk->sRiskFlag,"1");
				}
				issend = true;
			}
		}
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"账户[%s] 持仓[%d %s] 持仓风险计算完成 风险标志[%s %s] 是否发送[%d]",info->sTxAcc,posilist[i]->nContractNo,posilist[i]->sContractCode,risk->sRiskFlag,risk->sRiskDesc,issend);
		if (issend)
		{
			// 		102	varrchar3	交易账号	20	Y	
			// 			40	int1	风险等级	2	y	
			// 			20	uint1	合约编号	9	y	
			// 			104	varchar5	合约代码	20	Y	
			// 			103	short1	投机保值	1	y	
			// 			21	uint2	买持仓	10	y	
			// 			22	uint3	卖持仓	10	y	
			// 			101	varchar2	风险说明	80	y	
			// 			102	varchar3	风险标志	1	y	'1'产生风险'0'取消风险
			//106	varchar7	账户名称	40	y	
			//	107	varchar8	风险模板代码	12	n	产生风险则有
			//	108	varchar9	风险模板名称	40	n	产生风险则有
			//	109	varchar10	客户号	10	y	
			//	110	varchar11	所属机构	12	y	
			//	111	varchar12	所属机构名称	40	y	
			//	112	varchar12	代理机构	12	n	可为空
			//	113	varchar13	代理机构名称	40	n	可为空


			char riskdata[1000];
			unsigned int riskdatalen=sizeof(riskdata) ;
			m_pXdp->ResetData();
			m_pXdp->SetFieldValue((unsigned short)103,risk->sTxAcc,strlen(risk->sTxAcc),m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)40,(int)risk->nWarnLevel,m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)20,(unsigned int)risk->nContractNo,m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)104,risk->sContractCode,strlen(risk->sContractCode),m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)105,risk->sShFlag,strlen(risk->sShFlag),m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)21,(unsigned int)risk->nBuyQty,m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)22,(unsigned int)risk->nSellQty,m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)101,risk->sRiskDesc,strlen(risk->sRiskDesc),m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)102,risk->sRiskFlag,strlen(risk->sRiskFlag),m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)106,pFundAcc->sCustName,strlen(pFundAcc->sCustName),m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)107,templet->sPosiTempletId,strlen(templet->sPosiTempletId),m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)108,pPosiTemplet->sTempletName,strlen(pPosiTemplet->sTempletName),m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)109,pFundAcc->sCustNo,strlen(pFundAcc->sCustNo),m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)110,pFundAcc->sSignOrg,strlen(pFundAcc->sSignOrg),m_sErrMsg);
			nRet = slist.FillSepString(pFundAcc->sCustSimpleName);
			if (nRet !=2 )
			{
				m_pXdp->SetFieldValue((unsigned short)96,pFundAcc->sCustSimpleName,strlen(pFundAcc->sCustSimpleName),m_sErrMsg);
			}
			else
			{
				m_pXdp->SetFieldValue((unsigned short)96,(char *)slist.GetAt(0).c_str(),slist.GetAt(0).length(),m_sErrMsg);
				m_pXdp->SetFieldValue((unsigned short)97,(char *)slist.GetAt(1).c_str(),slist.GetAt(1).length(),m_sErrMsg);
			}

			pOrg = m_pMemDb->m_pDbPublic.m_pOrg.Select(pFundAcc->sSignOrg);
			if (pOrg != NULL)
			{
				m_pXdp->SetFieldValue((unsigned short)111,pOrg->sOrgName,strlen(pOrg->sOrgName),m_sErrMsg);
			}
			else
			{
				m_pXdp->SetFieldValue((unsigned short)111," ",1,m_sErrMsg);
			}
			if (strlen(pFundAcc->sMgrOrg)<1)
			{
				m_pXdp->SetFieldValue((unsigned short)112," ",1,m_sErrMsg);
				m_pXdp->SetFieldValue((unsigned short)113," ",1,m_sErrMsg);
			}
			else
			{
				m_pXdp->SetFieldValue((unsigned short)112,pFundAcc->sMgrOrg,strlen(pFundAcc->sMgrOrg),m_sErrMsg);
				pOrg = m_pMemDb->m_pDbPublic.m_pOrg.Select(pFundAcc->sMgrOrg);
				if (pOrg != NULL)
				{
					m_pXdp->SetFieldValue((unsigned short)113,pOrg->sOrgName,strlen(pOrg->sOrgName),m_sErrMsg);
				}
				else
				{
					m_pXdp->SetFieldValue((unsigned short)113," ",1,m_sErrMsg);
				}
			}
			m_pXdp->SetFieldValue((unsigned short)38,risk->nRiskdate,m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)39,risk->nRisktime,m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)115,pFundAcc->sCustName,strlen(pFundAcc->sCustName),m_sErrMsg);

			if (!m_pXdp->ToBuffer(riskdata,riskdatalen,m_sErrMsg))
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"Xdp ToBuffer error %s",m_sErrMsg);
				return;
			}
			Send2CustRiskMsg(info,traderight[0],riskdata,riskdatalen,9844);
			Send2OperRiskMsg(info,traderight[0],riskdata,riskdatalen,9844);
			Send2PgSync(info,riskdata,riskdatalen,9844);
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"发送账户[%s]持仓风险通知完成",info->sTxAcc);
		}
	}
	return ;
}


void RiskDll::SendRiskMsg(S_MDB_TXACC_LOCK *acc,S_MDB_CUST_FUND *fund, char traderight)
{
	if (*g_nSvrHostStatus != 	ARBSTATUS_MASTER && *g_nSvrHostStatus != ARBSTATUS_MASTER_AYN && *g_nSvrHostStatus!=MASTERHOST)
	{
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"Send2CustRiskMsg 不是主机，略过");
		return;
	}
	m_pXdp->ResetData();

	S_MDB_CUST_FUNDACC_INFO *pFundAcc = m_pMemDb->m_pDbCust.m_pCustFundAcc.At(acc->nFundAccRowid);
	if (pFundAcc == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"资金账号rowid[%d]不存在",acc->nFundAccRowid);
		return ;
	}
	S_MDB_CUSTINFO *pCust= m_pMemDb->m_pDbCust.m_pCustom.At(acc->nCustRowid);
	if (pCust == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"客户rowid[%d]不存在",acc->nCustRowid);
		return ;
	}
	char sRiskTempletId[13];
	char sRiskTempletName[41];
	bzero(sRiskTempletId,sizeof(sRiskTempletId));
	bzero(sRiskTempletName,sizeof(sRiskTempletName));
	//取资金账号对应的模板
	S_MDB_CUST_TEMPLET_RELA *templet = m_pMemDb->m_pDbSet.m_pCustTemplet.Select(acc->sTxAcc);
	if (templet == NULL)
	{
		//没有找到资金账号对应的模板,取$对应的模板
		templet = m_pMemDb->m_pDbSet.m_pCustTemplet.Select("$");
		if (templet == NULL)
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"资金账号[%s] 没有对应的持仓模板 ",acc->sTxAcc);
		}
		strcpy(sRiskTempletId,templet->sFundTempletId);
	}
	else
	{
		S_MDB_TEMPLET_INFO *pFundTemplet = m_pMemDb->m_pDbSet.m_pFundTemplet.Select(templet->sFundTempletId);
		if (pFundTemplet == NULL)
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"资金模板[%s]不存在",templet->sFundTempletId);
		}
		else
		{
			strcpy(sRiskTempletName,pFundTemplet->sTempletName);
		}
	}
	
	//根据模板ID，从持仓模板设置表查找
	S_MDB_ORG_INFO *pOrg = NULL;
	// 	102	varrchar3	交易账号	20	Y	
	// 	103	varrchar4	币种	1	Y	
	// 		60	double1	风险度	18.4	y	
	// 		40	int1	风险等级	2	y	
	// 		61	double2	可用资金	18.4	y	
	// 		62	double3	可提资金	18.4	y	
	// 		63	double4	保证金	18.4	y	
	// 		64	double5	客户权益	18.4	y	
	//	101	varchar2	风险说明	80	y

	//106	varchar7	账户名称	40	y	
	//	107	varchar8	风险模板代码	12	n	产生风险则有
	//	108	varchar9	风险模板名称	40	n	产生风险则有
	//109	varchar10	客户号	10	y	
	//	110	varchar11	所属机构	12	y	
	//	111	varchar12	所属机构名称	40	y	
	//	112	varchar12	代理机构	12	n	可为空
	//	113	varchar13	代理机构名称	40	n	可为空
	CBF_Slist slist;
	slist.SetSeparateString("-");
	//设置值
	m_pXdp->SetFieldValue((unsigned short)102,fund->sTxAcc,strlen(fund->sTxAcc),m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)103,fund->sCurrency,strlen(fund->sCurrency),m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)60,fund->dRisk,m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)40,(int)fund->nRisk_level,m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)61,fund->dAvailFund,m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)62,fund->dGetFund,m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)63,fund->dMargin,m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)64,fund->dBalance,m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)101,fund->sRiskDesc,strlen(fund->sRiskDesc),m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)106,pFundAcc->sCustName,strlen(pFundAcc->sCustName),m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)107,sRiskTempletId,strlen(sRiskTempletId),m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)108,sRiskTempletName,strlen(sRiskTempletName),m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)109,pFundAcc->sCustNo,strlen(pFundAcc->sCustNo),m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)110,pFundAcc->sSignOrg,strlen(pFundAcc->sSignOrg),m_sErrMsg);
	//brokerid-客户号
	int nRet = slist.FillSepString(pFundAcc->sCustSimpleName);
	if (nRet !=2 )
	{
		m_pXdp->SetFieldValue((unsigned short)96,pFundAcc->sCustSimpleName,strlen(pFundAcc->sCustSimpleName),m_sErrMsg);
	}
	else
	{
		m_pXdp->SetFieldValue((unsigned short)96,(char *)slist.GetAt(0).c_str(),slist.GetAt(0).length(),m_sErrMsg);
		m_pXdp->SetFieldValue((unsigned short)97,(char *)slist.GetAt(1).c_str(),slist.GetAt(1).length(),m_sErrMsg);
	}
	
	pOrg = m_pMemDb->m_pDbPublic.m_pOrg.Select(pFundAcc->sSignOrg);
	if (pOrg != NULL)
	{
		m_pXdp->SetFieldValue((unsigned short)111,pOrg->sOrgName,strlen(pOrg->sOrgName),m_sErrMsg);
	}
	else
	{
		m_pXdp->SetFieldValue((unsigned short)111," ",1,m_sErrMsg);
	}
	if (strlen(pFundAcc->sMgrOrg)<1)
	{
		m_pXdp->SetFieldValue((unsigned short)112," ",1,m_sErrMsg);
		m_pXdp->SetFieldValue((unsigned short)113," ",1,m_sErrMsg);
	}
	else
	{
		m_pXdp->SetFieldValue((unsigned short)112,pFundAcc->sMgrOrg,strlen(pFundAcc->sMgrOrg),m_sErrMsg);
		pOrg = m_pMemDb->m_pDbPublic.m_pOrg.Select(pFundAcc->sMgrOrg);
		if (pOrg != NULL)
		{
			m_pXdp->SetFieldValue((unsigned short)113,pOrg->sOrgName,strlen(pOrg->sOrgName),m_sErrMsg);
		}
		else
		{
			m_pXdp->SetFieldValue((unsigned short)113," ",1,m_sErrMsg);
		}
	}
	m_pXdp->SetFieldValue((unsigned short)38,fund->nRiskdate,m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)39,fund->nRisktime,m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)115,pFundAcc->sCustName,strlen(pFundAcc->sCustName),m_sErrMsg);
	char sRiskdata[1000];
	unsigned int len = sizeof(sRiskdata);
	if (!m_pXdp->ToBuffer(sRiskdata,len,m_sErrMsg))
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"Xdp ToBuffer error");
		return ;
	}
	//发送给此客户，若客户已登录
	Send2CustRiskMsg(acc,traderight,sRiskdata,len,9802);
	//发送给操作员，代理该客户的操作员或该客户的上级机构操作员，操作员已登录且订阅了消息
	Send2OperRiskMsg(acc,traderight,sRiskdata,len,9802);
	Send2PgSync(acc,sRiskdata,len,9802);
	
}
void RiskDll::Send2PgSync(S_MDB_TXACC_LOCK *acc,const char *riskdata,int riskdatalen,int funcno)
{
	S_TRADE_DATA sdata;
	
	S_MDB_CUST_FUNDACC_INFO *pCustFuncAcc = m_pMemDb->m_pDbCust.m_pCustFundAcc.At(acc->nFundAccRowid);
	if (pCustFuncAcc == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"资金账号rowid[%d]不存在",acc->nFundAccRowid);
		return ;
	}
	S_MDB_BROKERAGE_INFO *pbrok = m_pMemDb->m_pDbSet.m_pBrokerage.Select(pCustFuncAcc->nSysId);
	if (pbrok == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托 资金账号无所属经纪公司[%d]",pCustFuncAcc->nSysId);
		return ;
	}
	
	int drebsvrid;
	if (pbrok->nClearFlag == 0) //对接的是柜台
	{
		drebsvrid = pbrok->nSvrId;
	}
	else if (pbrok->nClearFlag == 2) //对接的是柜台
	{
		CBF_Slist slist;
		slist.SetSeparateString("-");
		S_MDB_BROKERAGE_PG *pginfo =NULL;
		//brokerid-客户号
		int nRet = slist.FillSepString(pCustFuncAcc->sCustSimpleName);
		if (nRet !=2 )
		{
			pginfo = m_pMemDb->m_pDbSet.m_pBrokeragePg.Select(pCustFuncAcc->nSysId,pCustFuncAcc->sCustSimpleName);
		}
		else
		{
			pginfo = m_pMemDb->m_pDbSet.m_pBrokeragePg.Select(pCustFuncAcc->nSysId,(char *)slist.GetAt(0).c_str());
		}
		if (pginfo == NULL)
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"配置为连接多个PG，但BROKERAGE_PG表里无pg配置[%d %s]",pCustFuncAcc->nSysId,pCustFuncAcc->sCustSimpleName);
			return ;
		}
		drebsvrid = pginfo->nSvrId;
	}
	else
	{
		return ;
	}
	if (m_pClientLink->PoolMalloc(sdata)<0)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"PoolMalloc error");
		return ;
	}
	bzero(&(sdata.pData->sDBHead),DREBHEADLEN);

	memcpy(sdata.pData->sBuffer,riskdata,riskdatalen);
	sdata.pData->sDBHead.nLen = riskdatalen;


	sdata.pData->sDBHead.cRaflag = 0;
	//sdata.pData->sDBHead.cCmd = CMD_DPPUSH;
	sdata.pData->sDBHead.cCmd = CMD_DPABC;
	sdata.pData->sDBHead.d_Dinfo.d_nServiceNo = funcno;
	
	sdata.src = 0;
	sdata.pData->sDBHead.d_Dinfo.d_nNodeId = 0;
	sdata.pData->sDBHead.d_Dinfo.d_cNodePrivateId = 0;
	sdata.pData->sDBHead.d_Dinfo.d_nSvrMainId = drebsvrid;
	sdata.pData->sDBHead.d_Dinfo.d_cSvrPrivateId = 0;
	sdata.pData->sDBHead.s_Sinfo.s_nGateIndex = 0;
	sdata.pData->sDBHead.s_Sinfo.s_nHook = 0;
	m_pClientLink->Send2Client(sdata);
	return;
}
void RiskDll::Send2CustRiskMsg(S_MDB_TXACC_LOCK *acc, char traderight,const char *riskdata,int riskdatalen,int funcno)
{
// 	if (*g_nSvrHostStatus != 	ARBSTATUS_MASTER && *g_nSvrHostStatus != ARBSTATUS_MASTER_AYN && *g_nSvrHostStatus!=MASTERHOST)
// 	{
// 		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"Send2CustRiskMsg 不是主机，略过");
// 		return;
// 	}
	S_TRADE_DATA sdata;
	//查找登录表，推送风险消息
	S_MDB_CUSTINFO *cust=	m_pMemDb->m_pDbCust.m_pCustom.At(acc->nCustRowid);
	if (cust == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"SendRiskMsg cust is NULL");
		return;
	}
	std::vector<S_MDB_CUST_ONLINE *> onlist;
	bool bret = m_pMemDb->m_pDbTrader.m_pCustOnline.Select(cust->sCustNo,onlist);
	if (!bret)
	{
		return;
	}

	for (int i=0; i<onlist.size();i++)
	{
		if (onlist[i]->sOnlineStatus[0] != LOGIN_STATUS_OK)
		{
			continue;
		}
//		sdata.pData = (PBPCCOMMSTRU)m_pMemPool->PoolMalloc();
		if (m_pClientLink->PoolMalloc(sdata)<0)
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"PoolMalloc error");
			return ;
		}
		bzero(&(sdata.pData->sDBHead),DREBHEADLEN);

		memcpy(sdata.pData->sBuffer,riskdata,riskdatalen);
		sdata.pData->sDBHead.nLen = riskdatalen;

		
		sdata.pData->sDBHead.cRaflag = 0;
		sdata.pData->sDBHead.cCmd = CMD_DPPUSH;
		sdata.pData->sDBHead.d_Dinfo.d_nServiceNo = funcno;
		if (strcmp(onlist[i]->sTermType,TERM_TYPE_RPCAPI) ==0)
		{
			//socket
			sdata.src = 1;
			sdata.pData->sBpcHead.nIndex = onlist[i]->nGateIndex;
			sdata.pData->sBpcHead.nConnectTime = onlist[i]->nTimeStamp;
		}
		else //总线
		{
			sdata.src = 0;
			sdata.pData->sDBHead.d_Dinfo.d_nNodeId = onlist[i]->nDrebId;
			sdata.pData->sDBHead.d_Dinfo.d_cNodePrivateId = onlist[i]->nDrebPid;
			sdata.pData->sDBHead.d_Dinfo.d_nSvrMainId = onlist[i]->nSvrId;
			sdata.pData->sDBHead.d_Dinfo.d_cSvrPrivateId = onlist[i]->nSvrPid;
			sdata.pData->sDBHead.s_Sinfo.s_nGateIndex = onlist[i]->nGateIndex;
			sdata.pData->sDBHead.s_Sinfo.s_nHook = onlist[i]->nTimeStamp;
			
		}
		m_pClientLink->Send2Client(sdata);
	}
}

void RiskDll::Send2OperRiskMsg(S_MDB_TXACC_LOCK *acc, char traderight,const char *riskdata,int riskdatalen,int funcno)
{
// 	if (*g_nSvrHostStatus != 	ARBSTATUS_MASTER && *g_nSvrHostStatus != ARBSTATUS_MASTER_AYN && *g_nSvrHostStatus!=MASTERHOST)
// 	{
// 		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"Send2CustRiskMsg 不是主机，略过");
// 		return;
// 	}
	//发送给操作员，代理该客户的操作员或该客户的上级机构操作员，操作员已登录且订阅了消息
	
	S_MDB_CUST_FUNDACC_INFO *pcust = m_pMemDb->m_pDbCust.m_pCustFundAcc.At(acc->nFundAccRowid);
	if (pcust == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"资金账号为空 [%d]",acc->nFundAccRowid);
		return;
	}
	//资金账号所属机构
	int len = strlen(pcust->sSignOrg);
	//查找在线并且订阅了风险的操作员
	std::vector<S_MDB_SUBSCRIBE *> sublist;
	if (!m_pMemDb->m_pDbTrader.m_pSubcribe.Select((char *)"0",sublist))
	{
		return;
	}
	S_MDB_SUBSCRIBE  *psub=NULL;
	int i,j;
	std::vector<S_MDB_OPER_ONLINE *> onlist;
	for (i=0;i<sublist.size(); i++)
	{
		//查看操作员是否在线，是否有权限
		onlist.clear();
		psub = sublist[i];
		if (!m_pMemDb->m_pDbPublic.m_pOperOnline.Select(sublist[i]->sOper,onlist))
		{
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"订阅风险的操作员[%s] 没有登录",sublist[i]->sOper);
			continue;
		}
		for (j=0 ; j<onlist.size(); j++)
		{
			//是否登录
			if (onlist[j]->sOnlineStatus[0] != LOGIN_STATUS_OK )
			{
				continue;
			}
			//查找操作员机构
			S_MDB_OPER_INFO *poper = m_pMemDb->m_pDbPublic.m_pOper.Select(onlist[j]->sOperCode);
			if (poper== NULL)
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"操作员[%s]在操作员表中不存在",onlist[j]->sOperCode);
				continue;
			}
			//查看机构是否有权限
			if (m_pKernelPub.CheckRight(pcust->sSignOrg,poper->sOrgCode))
			{
				S_TRADE_DATA sdata;
				//有权限，发送报文
//				sdata.pData = (PBPCCOMMSTRU)m_pMemPool->PoolMalloc();
				if (m_pClientLink->PoolMalloc(sdata)<0)
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"PoolMalloc error");
					return ;
				}
				bzero(&(sdata.pData->sDBHead),DREBHEADLEN);
				
				memcpy(sdata.pData->sBuffer,riskdata,riskdatalen);
				sdata.pData->sDBHead.nLen = riskdatalen;
				
				
				sdata.pData->sDBHead.cRaflag = 0;
				sdata.pData->sDBHead.cCmd = CMD_DPPUSH;
				sdata.pData->sDBHead.d_Dinfo.d_nServiceNo = funcno;
				if (strcmp(onlist[j]->sTermType,TERM_TYPE_RPCAPI) ==0)
				{
					//socket
					sdata.src = 1;
					sdata.pData->sBpcHead.nIndex = onlist[j]->nGateIndex;
					sdata.pData->sBpcHead.nConnectTime = onlist[j]->nTimeStamp;
				}
				else //总线
				{
					sdata.src = 0;
					sdata.pData->sDBHead.d_Dinfo.d_nNodeId = onlist[j]->nDrebId;
					sdata.pData->sDBHead.d_Dinfo.d_cNodePrivateId = onlist[j]->nDrebPid;
					sdata.pData->sDBHead.d_Dinfo.d_nSvrMainId = onlist[j]->nSvrId;
					sdata.pData->sDBHead.d_Dinfo.d_cSvrPrivateId = onlist[j]->nSvrPid;
					sdata.pData->sDBHead.s_Sinfo.s_nGateIndex = onlist[j]->nGateIndex;
					sdata.pData->sDBHead.s_Sinfo.s_nHook = onlist[j]->nTimeStamp;
					
				}
				m_pClientLink->Send2Client(sdata);
			}
		}
	}
	
}



int RiskDll::QueryCustOrder(S_TRADE_DATA &data)
{
	if (!m_pXdp->FromBuffer(data.pData->sBuffer,data.pData->sDBHead.nLen,m_sErrMsg))
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"请求非XDP数据 [%s]",m_sErrMsg);
		data.pData->sDBHead.nLen = 0;
		data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_INPUT;
		AnsData(data);
		return -1;
	}
// 100	varchar1	交易账号	20	n	
// 105	varchar6	客户端流水号	18	n	
// 106	varchar7	本地报单号	37	n	
// 40	int1	当前第几页	8	n	
// 41	int2	每页记录数	8	n	
// 59	int20	客户/操作员	1	y	0客户 1操作员操作员需要验证权限
// 91	char2	渠道	2	Y	
// 92	char3	操作员代码/客户号	16	Y	
	//116	varchar17	机构	12	n	按操作员查询时可选
	//	58	int19	是否包含下属机构	1	n	0不包括 1包括下属机构

	
	
	char sTxAcc[21];//资金账号
	int  nFlag; //0客户 1操作员
	char sOper[21];//操作员/客户号
	char sLocalSerial[38];//本地报单号
	char sClientSerial[19];//前端流水号

	
	
	
	bzero(sLocalSerial,sizeof(sLocalSerial));
	bzero(sClientSerial,sizeof(sClientSerial));
	char sOrgCode[17];
	int nIsInclude=0;
	bzero(sTxAcc,sizeof(sTxAcc));
	bzero(sOper,sizeof(sOper));
	bzero(sOrgCode,sizeof(sOrgCode));
	int nRet;
	bool bRet;
	int rid;
	int i;
	unsigned int len;
	int fieldtype;
	
	std::vector<S_MDB_ORDER_INFO *> reslist;
	std::vector<S_MDB_CUST_FUNDACC_INFO *> fundacclist;
	std::vector<S_MDB_ORG_INFO *> orglist;
	CInt acclist;
	S_MDB_ORG_INFO *pOrgTmp=NULL;
	S_MDB_CUST_FUNDACC_INFO *pCustAccTmp=NULL;
	

	if (!m_pXdp->GetFieldValue((unsigned short)59,nFlag,m_sErrMsg))
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"Input error [%s]",m_sErrMsg);
		data.pData->sDBHead.nLen = 0;
		data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_INPUT;
		AnsData(data);
		return -1;
	}
	len  = sizeof(sOper);
	if (!m_pXdp->GetFieldValue((unsigned short)92,sOper,len,fieldtype,m_sErrMsg))
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"Input error [%s]",m_sErrMsg);
		data.pData->sDBHead.nLen = 0;
		data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_INPUT;
		AnsData(data);
		return -1;
	}
	len = sizeof(sTxAcc);
	m_pXdp->GetFieldValue((unsigned short)100,sTxAcc,len,fieldtype,m_sErrMsg);
	len = sizeof(sLocalSerial);
	m_pXdp->GetFieldValue((unsigned short)106,sLocalSerial,len,fieldtype,m_sErrMsg);
	len = sizeof(sClientSerial);
	m_pXdp->GetFieldValue((unsigned short)105,sClientSerial,len,fieldtype,m_sErrMsg);

	int nCurPage;//当前页
	int nPageNum;//每页记录数
	bool bIsPage=true;//是否分页
	if (!m_pXdp->GetFieldValue((unsigned short)40,nCurPage,m_sErrMsg))
	{
		bIsPage = false;
	}
	if (bIsPage)
	{
		if (!m_pXdp->GetFieldValue((unsigned short)41,nPageNum,m_sErrMsg))
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"Input error [%s]",m_sErrMsg);
			data.pData->sDBHead.nLen = 0;
			data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_INPUT;
			AnsData(data);
			return -1;
		}
	}
	S_MDB_CUST_FUNDACC_INFO *pacc=NULL;
	S_MDB_ORDER_INFO *pordertmp=NULL;
	if (nFlag == 0)//客户
	{
		if (strlen(sTxAcc)>0)
		{

			pacc = m_pMemDb->m_pDbCust.m_pCustFundAcc.SelectByAcc(sTxAcc);
			if (pacc == NULL)
			{
				data.pData->sDBHead.nLen = 0;
				data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_NOTFOUND;
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"资金账号 [%s]没有找到",sTxAcc);
				AnsData(data);
				return -1;
			}
			if (strcmp(pacc->sCustNo,sOper) != 0)
			{
				data.pData->sDBHead.nLen = 0;
				data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_NO_RIGHT;
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"资金账号[%s]不属于客户[%s]",sTxAcc,sOper);
				AnsData(data);
				return -1;
			}
			m_pMemDb->m_pDbTrader.m_pOrder.SelectByAcc(sTxAcc,reslist);
		}
		else //没有输入资金账号 按报单号
		{
			if (strlen(sLocalSerial)>0)
			{
				//按本地报单号查询
				pordertmp = m_pMemDb->m_pDbTrader.m_pOrder.Select(sLocalSerial);
				if (pordertmp != NULL)
				{
					//是否有权限
					pacc = m_pMemDb->m_pDbCust.m_pCustFundAcc.SelectByAcc(pordertmp->sTxAcc);
					if (pacc == NULL)
					{
						data.pData->sDBHead.nLen = 0;
						data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_NOTFOUND;
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"资金账号 [%s]没有找到",pordertmp->sTxAcc);
						AnsData(data);
						return -1;
					}
					if (strcmp(pacc->sCustNo,sOper) != 0)
					{
						data.pData->sDBHead.nLen = 0;
						data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_NO_RIGHT;
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"资金账号[%s]不属于客户[%s]",pordertmp->sTxAcc,sOper);
						AnsData(data);
						return -1;
					}
					reslist.push_back(pordertmp);
				}
			}
			else if (strlen(sClientSerial)>0)
			{
				//前端流水号
				if (m_pMemDb->m_pDbTrader.m_pOrder.SelectByClient(sClientSerial,reslist))
				{
					//是否有权限
					pacc = m_pMemDb->m_pDbCust.m_pCustFundAcc.SelectByAcc(reslist[0]->sTxAcc);
					if (pacc == NULL)
					{
						data.pData->sDBHead.nLen = 0;
						data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_NOTFOUND;
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"资金账号 [%s]没有找到",reslist[0]->sTxAcc);
						AnsData(data);
						return -1;
					}
					if (strcmp(pacc->sCustNo,sOper) != 0)
					{
						data.pData->sDBHead.nLen = 0;
						data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_NO_RIGHT;
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"资金账号[%s]不属于客户[%s]",reslist[0]->sTxAcc,sOper);
						AnsData(data);
						return -1;
					}
				}
			}
			else
			{
				//按客户号
				if (!m_pMemDb->m_pDbCust.m_pCustFundAcc.SelectByCust(sOper,fundacclist))
				{
					data.pData->sDBHead.nLen = 0;
					data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_NOTFOUND;
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"客户号[%s]无所属资金账号",sOper);
					AnsData(data);
					return -1;
				}
				for (i=0; i<fundacclist.size();i++)
				{
					m_pMemDb->m_pDbTrader.m_pOrder.SelectByAcc(fundacclist[i]->sTxAcc,reslist);
				}
			}
		}
	}
	else // 按操作员查询
	{
		if (strlen(sTxAcc)>0)
		{
			//看操作员是否有权限
			if (!m_pKernelPub.CheckOperAccRight(sOper,sTxAcc))
			{
				data.pData->sDBHead.nLen = 0;
				data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_NO_RIGHT;
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"操作员[%s]无资金账号[%s]权限",sOper,sTxAcc);
				AnsData(data);
				return -1;
			}
			m_pMemDb->m_pDbTrader.m_pOrder.SelectByAcc(sTxAcc,reslist);
		}
		else //没有输入资金账号 
		{
			if (strlen(sLocalSerial)>0)
			{
				//按本地报单号查询
				pordertmp = m_pMemDb->m_pDbTrader.m_pOrder.Select(sLocalSerial);
				if (pordertmp != NULL)
				{
					//是否有权限
					if (m_pKernelPub.CheckOperAccRight(sOper,pordertmp->sTxAcc))
					{
						data.pData->sDBHead.nLen = 0;
						data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_NO_RIGHT;
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"操作员[%s]无资金账号[%s]权限",sOper,pordertmp->sTxAcc);
						AnsData(data);
						return -1;
					}
					reslist.push_back(pordertmp);
				}
			}
			else if (strlen(sClientSerial)>0)
			{
				//前端流水号
				if (m_pMemDb->m_pDbTrader.m_pOrder.SelectByClient(sClientSerial,reslist))
				{
					//是否有权限
					if (m_pKernelPub.CheckOperAccRight(sOper,reslist[0]->sTxAcc))
					{
						data.pData->sDBHead.nLen = 0;
						data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_NO_RIGHT;
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"操作员[%s]无资金账号[%s]权限",sOper,reslist[0]->sTxAcc);
						AnsData(data);
						return -1;
					}
				}
			}
			else //按机构查询
			{
				//按机构查询
				m_pXdp->GetFieldValue((unsigned short)58,nIsInclude,m_sErrMsg);
				//输入机构
				len  = sizeof(sOrgCode);
				if (m_pXdp->GetFieldValue((unsigned short)116,sOrgCode,len,fieldtype,m_sErrMsg))
				{
					//检查是否有权限
					if (!m_pKernelPub.CheckOperOrgRight(sOper,sOrgCode))
					{
						data.pData->sDBHead.nLen = 0;
						data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_NO_RIGHT;
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"操作员[%s]无机构[%s]权限",sOper,sOrgCode);
						AnsData(data);
						return -1;
					}

					if (nIsInclude == 1) //包含下属机构
					{
						nRet = m_pMemDb->m_pDbPublic.m_pOrg.GetAllChildOrg(sOrgCode,orglist,true);
						if (nRet <0)
						{
							data.pData->sDBHead.nLen = 0;
							data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_SYSTEM;
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"机构[%s]取所有子机构出错",sOrgCode);
							AnsData(data);
							return -1;
						}
						if (nRet == 1) //顶级机构
						{
							//取所有的资金账户
							m_pMemDb->m_pDbCust.m_pCustFundAcc.SelectAll(acclist);
						}
						else
						{
							//循环取每个机构的资金账户
							for (i=0; i<orglist.size();i++)
							{
								//查询此机构下面的客户 所属机构
								m_pMemDb->m_pDbCust.m_pCustFundAcc.SelectByAccOrg(orglist[i]->sOrgCode,acclist);
								//代理机构
								m_pMemDb->m_pDbCust.m_pCustFundAcc.SelectByMgrOrg(orglist[i]->sOrgCode,acclist);
							}
						}
					}
					else
					{
						//查询此机构下面的客户 所属机构
						if (!m_pMemDb->m_pDbCust.m_pCustFundAcc.SelectByAccOrg(sOrgCode,acclist))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"机构[%s]下无资金账号",sOrgCode);
							}
						//代理机构
						if (!m_pMemDb->m_pDbCust.m_pCustFundAcc.SelectByMgrOrg(sOrgCode,acclist))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"机构[%s]下无资金账号",sOrgCode);
						}
					}
					bRet = acclist.First(rid);
					if (!bRet)
					{
						data.pData->sDBHead.nLen = 0;
						data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_NOTFOUND;
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"机构[%s]没有资金账号",sOrgCode);
						AnsData(data);
						return -1;
					}

				}
				else //按操作员代理的客户查
				{
					//按操作员代理的客户查询
					S_MDB_OPER_INFO *poper = m_pMemDb->m_pDbPublic.m_pOper.Select(sOper);
					if (poper == NULL)
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"操作员[%s]在操作员表无记录",sOper);
						return false;
					}
					S_MDB_ORG_INFO *operorg = m_pMemDb->m_pDbPublic.m_pOrg.Select(poper->sOrgCode);
					if (operorg == NULL)
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"操作员[%s]的机构[%s]在机构表无记录",sOper,poper->sOrgCode);
						return false;
					}
					if (!m_pMemDb->m_pDbCust.m_pCustFundAcc.SelectByAccOrg(operorg->sOrgCode,acclist))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"操作员[%s]所属机构[%s]下无资金账号",sOper,operorg->sOrgCode);
					}
					//代理机构
					if (!m_pMemDb->m_pDbCust.m_pCustFundAcc.SelectByMgrOrg(operorg->sOrgCode,acclist))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"操作员[%s]所属机构[%s]下无资金账号",sOper,sOrgCode);
					}
					bRet = acclist.First(rid);
					if (!bRet)
					{
						data.pData->sDBHead.nLen = 0;
						data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_NOTFOUND;
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"机构[%s]没有资金账号",sOrgCode);
						AnsData(data);
						return -1;
					}
				}
				while (bRet)
				{
					//查询报单
					m_pMemDb->m_pDbTrader.m_pOrder.SelectByAcc(m_pMemDb->m_pDbCust.m_pCustFundAcc.At(rid)->sTxAcc,reslist);
					bRet = acclist.Next(rid);
				}

			}
		}
	}
	
	int curNum=0;
	S_MDB_CUST_FUNDACC_INFO *pFundAcc=NULL;
	//找出来结果了，是否分页查询
	if (bIsPage)
	{
		int bNum = nCurPage*nPageNum;
		int eNum=  (nCurPage+1)*nPageNum;
			
		for (i=0; i<reslist.size(); i++)
		{
			if (i>eNum-1)
			{
				break;
			}
			if (i>=bNum)
			{
				m_pXdp->SetFieldValue((unsigned short)curNum+1,(unsigned short)100,reslist[i]->sTxAcc,strlen(reslist[i]->sTxAcc),m_sErrMsg);
				m_pXdp->SetFieldValue((unsigned short)curNum+1,(unsigned short)101,reslist[i]->sExchCode,strlen(reslist[i]->sExchCode),m_sErrMsg);
				m_pXdp->SetFieldValue((unsigned short)curNum+1,(unsigned short)102,reslist[i]->sSeatNo,strlen(reslist[i]->sSeatNo),m_sErrMsg);
				m_pXdp->SetFieldValue((unsigned short)curNum+1,(unsigned short)103,reslist[i]->sTxCode,strlen(reslist[i]->sTxCode),m_sErrMsg);
				m_pXdp->SetFieldValue((unsigned short)curNum+1,(unsigned short)20,(unsigned int)reslist[i]->nContractNo,m_sErrMsg);
				m_pXdp->SetFieldValue((unsigned short)curNum+1,(unsigned short)104,reslist[i]->sContractCode,strlen(reslist[i]->sContractCode),m_sErrMsg);
				m_pXdp->SetFieldValue((unsigned short)curNum+1,(unsigned short)105,reslist[i]->sClientSerial,strlen(reslist[i]->sClientSerial),m_sErrMsg);
				m_pXdp->SetFieldValue((unsigned short)curNum+1,(unsigned short)106,reslist[i]->sLocalSerial,strlen(reslist[i]->sLocalSerial),m_sErrMsg);
				m_pXdp->SetFieldValue((unsigned short)curNum+1,(unsigned short)107,reslist[i]->sSystemNo,strlen(reslist[i]->sSystemNo),m_sErrMsg);
				m_pXdp->SetFieldValue((unsigned short)curNum+1,(unsigned short)21,(unsigned int)reslist[i]->nOrderNum,m_sErrMsg);
				m_pXdp->SetFieldValue((unsigned short)curNum+1,(unsigned short)60,reslist[i]->dOrderPrice,m_sErrMsg);
				m_pXdp->SetFieldValue((unsigned short)curNum+1,(unsigned short)22,(unsigned int)reslist[i]->nRemainNum,m_sErrMsg);
				m_pXdp->SetFieldValue((unsigned short)curNum+1,(unsigned short)108,reslist[i]->sTxDate,strlen(reslist[i]->sTxDate),m_sErrMsg);
				m_pXdp->SetFieldValue((unsigned short)curNum+1,(unsigned short)61,reslist[i]->dAmt,m_sErrMsg);
				m_pXdp->SetFieldValue((unsigned short)curNum+1,(unsigned short)62,reslist[i]->dFrozAmt,m_sErrMsg);
				m_pXdp->SetFieldValue((unsigned short)curNum+1,(unsigned short)63,reslist[i]->dExFee,m_sErrMsg);
				m_pXdp->SetFieldValue((unsigned short)curNum+1,(unsigned short)5,(short)reslist[i]->sEoFlag[0],m_sErrMsg);
				m_pXdp->SetFieldValue((unsigned short)curNum+1,(unsigned short)6,(short)reslist[i]->sBsFlag[0],m_sErrMsg);
				m_pXdp->SetFieldValue((unsigned short)curNum+1,(unsigned short)7,(short)reslist[i]->sShFlag[0],m_sErrMsg);
				m_pXdp->SetFieldValue((unsigned short)curNum+1,(unsigned short)8,(short)reslist[i]->sTxType[0],m_sErrMsg);
				m_pXdp->SetFieldValue((unsigned short)curNum+1,(unsigned short)9,(short)reslist[i]->sCmdType[0],m_sErrMsg);
				m_pXdp->SetFieldValue((unsigned short)curNum+1,(unsigned short)10,(short)reslist[i]->sCmdAttr[0],m_sErrMsg);
				m_pXdp->SetFieldValue((unsigned short)curNum+1,(unsigned short)11,(short)reslist[i]->sForceFlag[0],m_sErrMsg);
				m_pXdp->SetFieldValue((unsigned short)curNum+1,(unsigned short)109,reslist[i]->sOrderStatus,strlen(reslist[i]->sOrderStatus),m_sErrMsg);
				m_pXdp->SetFieldValue((unsigned short)curNum+1,(unsigned short)110,reslist[i]->sCancelFlag,strlen(reslist[i]->sCancelFlag),m_sErrMsg);
				m_pXdp->SetFieldValue((unsigned short)curNum+1,(unsigned short)111,reslist[i]->sAnsTime,strlen(reslist[i]->sAnsTime),m_sErrMsg);
				m_pXdp->SetFieldValue((unsigned short)curNum+1,(unsigned short)12,(short)atoi(reslist[i]->sOrderChannel),m_sErrMsg);
				m_pXdp->SetFieldValue((unsigned short)curNum+1,(unsigned short)112,reslist[i]->sAccOrg,strlen(reslist[i]->sAccOrg),m_sErrMsg);
				m_pXdp->SetFieldValue((unsigned short)curNum+1,(unsigned short)113,reslist[i]->sTxOrg,strlen(reslist[i]->sTxOrg),m_sErrMsg);
				m_pXdp->SetFieldValue((unsigned short)curNum+1,(unsigned short)114,reslist[i]->sOrderOper,strlen(reslist[i]->sOrderOper),m_sErrMsg);
				m_pXdp->SetFieldValue((unsigned short)curNum+1,(unsigned short)13,(short)atoi(reslist[i]->sCancelChannel),m_sErrMsg);
				m_pXdp->SetFieldValue((unsigned short)curNum+1,(unsigned short)115,reslist[i]->sCancelOper,strlen(reslist[i]->sCancelOper),m_sErrMsg);
				m_pXdp->SetFieldValue((unsigned short)curNum+1,(unsigned short)116,reslist[i]->sCancelTime,strlen(reslist[i]->sCancelTime),m_sErrMsg);
				m_pXdp->SetFieldValue((unsigned short)curNum+1,(unsigned short)117,reslist[i]->sOrderRsp,strlen(reslist[i]->sOrderRsp),m_sErrMsg);
				m_pXdp->SetFieldValue((unsigned short)curNum+1,(unsigned short)64,reslist[i]->dFrozUseamt,m_sErrMsg);
				m_pXdp->SetFieldValue((unsigned short)curNum+1,(unsigned short)65,reslist[i]->dFrozOutamt,m_sErrMsg);
				m_pXdp->SetFieldValue((unsigned short)curNum+1,(unsigned short)23,(unsigned int)reslist[i]->nCancelNum,m_sErrMsg);
				m_pXdp->SetFieldValue((unsigned short)curNum+1,(unsigned short)66,reslist[i]->dDoneAvp,m_sErrMsg);
				pFundAcc = m_pMemDb->m_pDbCust.m_pCustFundAcc.SelectByAcc(reslist[i]->sTxAcc);
				if (pFundAcc != NULL)
				{
					m_pXdp->SetFieldValue((unsigned short)curNum+1,(unsigned short)119,pFundAcc->sCustName,strlen(pFundAcc->sCustName),m_sErrMsg);
				}
				curNum++;
			}
		}
		m_pXdp->SetFieldValue((unsigned short)42,curNum,m_sErrMsg);
	}
	else
	{
		for (i=0; i<reslist.size(); i++)
		{
			m_pXdp->SetFieldValue((unsigned short)i+1,(unsigned short)100,reslist[i]->sTxAcc,strlen(reslist[i]->sTxAcc),m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)i+1,(unsigned short)101,reslist[i]->sExchCode,strlen(reslist[i]->sExchCode),m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)i+1,(unsigned short)102,reslist[i]->sSeatNo,strlen(reslist[i]->sSeatNo),m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)i+1,(unsigned short)103,reslist[i]->sTxCode,strlen(reslist[i]->sTxCode),m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)i+1,(unsigned short)20,(unsigned int)reslist[i]->nContractNo,m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)i+1,(unsigned short)104,reslist[i]->sContractCode,strlen(reslist[i]->sContractCode),m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)i+1,(unsigned short)105,reslist[i]->sClientSerial,strlen(reslist[i]->sClientSerial),m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)i+1,(unsigned short)106,reslist[i]->sLocalSerial,strlen(reslist[i]->sLocalSerial),m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)i+1,(unsigned short)107,reslist[i]->sSystemNo,strlen(reslist[i]->sSystemNo),m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)i+1,(unsigned short)21,(unsigned int)reslist[i]->nOrderNum,m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)i+1,(unsigned short)60,reslist[i]->dOrderPrice,m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)i+1,(unsigned short)22,(unsigned int)reslist[i]->nRemainNum,m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)i+1,(unsigned short)108,reslist[i]->sTxDate,strlen(reslist[i]->sTxDate),m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)i+1,(unsigned short)61,reslist[i]->dAmt,m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)i+1,(unsigned short)62,reslist[i]->dFrozAmt,m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)i+1,(unsigned short)63,reslist[i]->dExFee,m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)i+1,(unsigned short)5,(short)reslist[i]->sEoFlag[0],m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)i+1,(unsigned short)6,(short)reslist[i]->sBsFlag[0],m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)i+1,(unsigned short)7,(short)reslist[i]->sShFlag[0],m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)i+1,(unsigned short)8,(short)reslist[i]->sTxType[0],m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)i+1,(unsigned short)9,(short)reslist[i]->sCmdType[0],m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)i+1,(unsigned short)10,(short)reslist[i]->sCmdAttr[0],m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)i+1,(unsigned short)11,(short)reslist[i]->sForceFlag[0],m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)i+1,(unsigned short)109,reslist[i]->sOrderStatus,strlen(reslist[i]->sOrderStatus),m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)i+1,(unsigned short)110,reslist[i]->sCancelFlag,strlen(reslist[i]->sCancelFlag),m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)i+1,(unsigned short)111,reslist[i]->sAnsTime,strlen(reslist[i]->sAnsTime),m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)i+1,(unsigned short)12,(short)atoi(reslist[i]->sOrderChannel),m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)i+1,(unsigned short)112,reslist[i]->sAccOrg,strlen(reslist[i]->sAccOrg),m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)i+1,(unsigned short)113,reslist[i]->sTxOrg,strlen(reslist[i]->sTxOrg),m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)i+1,(unsigned short)114,reslist[i]->sOrderOper,strlen(reslist[i]->sOrderOper),m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)i+1,(unsigned short)13,(short)atoi(reslist[i]->sCancelChannel),m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)i+1,(unsigned short)115,reslist[i]->sCancelOper,strlen(reslist[i]->sCancelOper),m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)i+1,(unsigned short)116,reslist[i]->sCancelTime,strlen(reslist[i]->sCancelTime),m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)i+1,(unsigned short)117,reslist[i]->sOrderRsp,strlen(reslist[i]->sOrderRsp),m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)i+1,(unsigned short)64,reslist[i]->dFrozUseamt,m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)i+1,(unsigned short)65,reslist[i]->dFrozOutamt,m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)i+1,(unsigned short)23,(unsigned int)reslist[i]->nCancelNum,m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)i+1,(unsigned short)66,reslist[i]->dDoneAvp,m_sErrMsg);
			pFundAcc = m_pMemDb->m_pDbCust.m_pCustFundAcc.SelectByAcc(reslist[i]->sTxAcc);
			if (pFundAcc != NULL)
			{
				m_pXdp->SetFieldValue((unsigned short)i+1,(unsigned short)119,pFundAcc->sCustName,strlen(pFundAcc->sCustName),m_sErrMsg);
			}
			
		}
		m_pXdp->SetFieldValue((unsigned short)42,(int)reslist.size(),m_sErrMsg);
	}
	
	len = BPUDATASIZE;
	if (!m_pXdp->ToBuffer(data.pData->sBuffer,len,m_sErrMsg))
	{
		data.pData->sDBHead.nLen = 0;
		data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_SYSTEM;
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"ToBuffer error [%s]",m_sErrMsg);
		AnsData(data);
		return -1;
	}
	data.pData->sDBHead.nLen = len;
	data.pData->sDBHead.a_Ainfo.a_nRetCode = 0;
	AnsData(data);
	
	return 0;
}



int RiskDll::OrderMsg(S_TRADE_DATA &data)
{
	if (!m_pXdp->FromBuffer(data.pData->sBuffer,data.pData->sDBHead.nLen,m_sErrMsg))
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"请求非XDP数据 [%s]",m_sErrMsg);
		m_pClientLink->PoolFree(data);
		return -1;
	}
	m_pClientLink->PoolFree(data);

// 	40	int1	交易日期	8	n	yyyymmdd
// 		102	varrchar3	交易账号	20	Y	
// 		100	varchar1	合约代码	20	Y	
// 		5	short1	投保标志	1	Y	
// 		6	short2	开平标志	1	Y	
// 		7	short3	买卖标志	1	Y	
// 		8	short4	交易类型	1	Y	
// 		9	short5	指令类型	1	Y	报单类型
// 		13	short9	报单属性	1	Y	成交属性
// 		10	short6	强平标志	1	Y	
// 		11	short7	撤单标志	1	Y	
// 		41	int2	委托数量	8	Y	
// 		60	double1	委托价格	18.4	Y	
// 		103	varchar4	本地报单号	37	Y	
// 		104	varchar5	系统报单号	20	Y	
// 		14	short10	报单状态	2	Y	ascii
// 		105	varchar6	操作员	20	Y	此笔报单发起方，交易员号或资金账号
// 		3	ushort4	系统代码	3	y	风控数据导入设置里面配置的系统代码
// 		106	varchar7	交易所代码	1	y
	INT64_  nBeginTime=CBF_Date_Time::GetTickUS();
	INT64_  nEndTime;
	S_MDB_ORDER_INFO order;
	S_MDB_ORDER_INFO *po=NULL;
	S_MDB_TXACC_LOCK *acclock=NULL;
	bzero(&order,sizeof(S_MDB_ORDER_INFO));
	char sTxAcc[21];
	bzero(sTxAcc,sizeof(sTxAcc));
	char sLocalSerial[38];//本地报单号
	bzero(sLocalSerial,sizeof(sLocalSerial));

	unsigned int len;
	int fieldtype;
	short nShortValue;


	len = sizeof(order.sExchCode);
	if (!m_pXdp->GetFieldValue((unsigned short)106,order.sExchCode,len,fieldtype,m_sErrMsg))
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托回报 无交易所代码 %s",m_sErrMsg);
		return -1;
	}
	len = sizeof(order.sSystemNo);
	if (!m_pXdp->GetFieldValue((unsigned short)104,order.sSystemNo,len,fieldtype,m_sErrMsg))
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托回报 无系统报单号 %s",m_sErrMsg);
		return -1;
	}
	if (m_pXdp->GetFieldValue((unsigned short)14,nShortValue,m_sErrMsg))
	{
		if (nShortValue < ORDER_STATUS_SENDOK || nShortValue > MAXASCIINUM)
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托回报 报单状态不符 %d",nShortValue);
			return -1;
		}
		order.sOrderStatus[0] = nShortValue;
	}
	else
	{
		//没有报单状态默认为已报入
		order.sOrderStatus[0] = ORDER_STATUS_OK;
	}
	len = sizeof(sLocalSerial);
	if (!m_pXdp->GetFieldValue((unsigned short)103,sLocalSerial,len,fieldtype,m_sErrMsg))
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托回报  无本地报单号 %s",m_sErrMsg);
		return -1;
	}
	//根据本地报单号去查询，若找到说明是风控发起的报单，否则为外报单
	po = m_pMemDb->m_pDbTrader.m_pOrder.Select(sLocalSerial);
	if (po != NULL)
	{
		//只需更改状态并置同步标志
		po->sOrderStatus[0] = order.sOrderStatus[0];
		po->nSyncFlag = 5; //报入同步
		if (po->sOrderStatus[0] == ORDER_STATUS_OK)
		{
			strcpy(po->sOrderRsp,"报单成功");
		}
		else
		{
			strcpy(po->sOrderRsp,"回报完成");
		}
		
		nEndTime =CBF_Date_Time::GetTickUS();
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"委托回报 处理完成 本地报单号[%s]  处理时间[%d]微秒",po->sLocalSerial,nEndTime-nBeginTime);
		acclock = m_pMemDb->SelectLock(po->sTxAcc);
		if (acclock == NULL)
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托回报  资金账号[%s]不存在",po->sTxAcc);
			return -1;
		}
		//通知要对此客户进行风险计算
		MsgCustRiskCalc(acclock->nLockRowid);
		//发送委托回报推送
		SendOrderPush(acclock,po);
		return 0;
	}


	len = sizeof(order.sTxAcc);
	if (!m_pXdp->GetFieldValue((unsigned short)102,sTxAcc,len,fieldtype,m_sErrMsg))
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托通知无交易账号 %s",m_sErrMsg);
		return -1;
	}
	len = sizeof(order.sSeatNo);
	if (!m_pXdp->GetFieldValue((unsigned short)101,order.sSeatNo,len,fieldtype,m_sErrMsg))
	{
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"委托通知无席位代码，默认为$");
		strcpy(order.sSeatNo,"$");
	}

	len = sizeof(order.sContractCode);
	if (!m_pXdp->GetFieldValue((unsigned short)100,order.sContractCode,len,fieldtype,m_sErrMsg))
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托通知无合约代码 %s",m_sErrMsg);
		return -1;
	}
	
	
	len = sizeof(order.sOrderOper);
	m_pXdp->GetFieldValue((unsigned short)105,order.sOrderOper,len,fieldtype,m_sErrMsg);
	
	
	if (!m_pXdp->GetFieldValue((unsigned short)5,nShortValue,m_sErrMsg))
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托通知无投保标志 %s",m_sErrMsg);
		return -1;
	}
	if (nShortValue <MINASCIINUM || nShortValue>MAXASCIINUM)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托通知投保标志不符 %d",nShortValue);
		return -1;
	}
	order.sShFlag[0] = nShortValue;
	if (!m_pXdp->GetFieldValue((unsigned short)6,nShortValue,m_sErrMsg))
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托通知无开平标志 %s",m_sErrMsg);
		return -1;
	}
	if (nShortValue <MINASCIINUM || nShortValue>MAXASCIINUM)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托通知开平标志不符 %d",nShortValue);
		return -1;
	}
	order.sEoFlag[0] = nShortValue;
	
	if (!m_pXdp->GetFieldValue((unsigned short)7,nShortValue,m_sErrMsg))
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托通知无买卖标志 %s",m_sErrMsg);
		return -1;
	}
	if (nShortValue <MINASCIINUM || nShortValue>MAXASCIINUM)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托通知买卖标志不符 %d",nShortValue);
		return -1;
	}
	order.sBsFlag[0] = nShortValue;
	
	if (!m_pXdp->GetFieldValue((unsigned short)8,nShortValue,m_sErrMsg))
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托通知无交易类型 %s",m_sErrMsg);
		return -1;
	}
	if (nShortValue <MINASCIINUM || nShortValue>MAXASCIINUM)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托通知交易类型不符 %d",nShortValue);
		return -1;
	}
	order.sTxType[0] = nShortValue;

	if (!m_pXdp->GetFieldValue((unsigned short)9,nShortValue,m_sErrMsg))
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托通知无报单类型 %s",m_sErrMsg);
		return -1;
	}
	if (nShortValue <MINASCIINUM || nShortValue>MAXASCIINUM)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托通知报单类型不符 %d",nShortValue);
		return -1;
	}
	order.sCmdType[0] = nShortValue;

	if (!m_pXdp->GetFieldValue((unsigned short)13,nShortValue,m_sErrMsg))
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托通知无报单属性 %s",m_sErrMsg);
		return -1;
	}
	if (nShortValue <MINASCIINUM || nShortValue>MAXASCIINUM)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托通知报单属性不符 %d",nShortValue);
		return -1;
	}
	order.sCmdAttr[0] = nShortValue;
	order.nRemainNum = order.nOrderNum;//剩余数量默认为委托数量
	order.sCancelFlag[0] = BOOL_FALSE;
	order.sCancelFlag[1] =0;

	//强平标志
	if (m_pXdp->GetFieldValue((unsigned short)10,nShortValue,m_sErrMsg))
	{
		if (nShortValue != BOOL_ASC_FALSE && nShortValue != BOOL_ASC_TRUE)
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托通知强平标志不符 %d",nShortValue);
			return -1;
		}
		order.sForceFlag[0] = nShortValue;
	}
	else
	{
		order.sForceFlag[0] = '0';
	}

	//委托数量
	if (!m_pXdp->GetFieldValue((unsigned short)41,order.nOrderNum,m_sErrMsg))
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托通知无委托数量 %s",m_sErrMsg);
		return -1;
	}
	if (order.nOrderNum<1)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托通知委托数量不符 %d",order.nOrderNum);
		return -1;
	}
	//委托价格
	if (!m_pXdp->GetFieldValue((unsigned short)60,order.dOrderPrice,m_sErrMsg))
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托通知无委托价格 %s",m_sErrMsg);
		return -1;
	}
	//这里的价格是交易所返回的价格，所以可以直接按这个价格计算
	
	
	unsigned short nCcId;//系统代码
	if (!m_pXdp->GetFieldValue((unsigned short)3,nCcId,m_sErrMsg))
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托通知无系统代码 %s",m_sErrMsg);
		return -1;
	}
	strcpy(order.sOrderChannel,TERM_TYPE_SYSTEM);//从柜台过来的报单渠道默认为系统
	//拼资金账号
	GenAcc(sTxAcc,nCcId,12,order.sTxAcc);//旧系统资金账号为3-12长

//	sprintf(order.sTxAcc,"%d%012s",nCcId,sTxAcc);//旧系统资金账号为3-12长
	//拼本地报单号
	sprintf(order.sLocalSerial,"%d%s",nCcId,sLocalSerial);
	
	

	//查找交易日期
	S_MDB_CONTRACT_INFO *pc = m_pMemDb->m_pDbBaseinfo.m_pContract.Select(order.sContractCode);
	if (pc == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托通知合约代码[%s]在合约表中不存在",order.sContractCode);
		return -1;
	}
	order.nContractNo = pc->nContractId;
	order.nVarietyId = pc->nProdId;
	strcpy(order.sExchCode,pc->sExcode);
	S_MDB_EXCH_INFO *pe = m_pMemDb->m_pDbBaseinfo.m_pExchange.Select(pc->sExcode);
	if (pe == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托通知合约代码[%s]所属交易所[%s]在交易所表中不存在",order.sContractCode,pc->sExcode);
		return -1;
	}
	if (strlen(pe->sTradeDate)!=8)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托通知合约代码[%s]所属交易所[%s]的交易日期[%s]不符",order.sContractCode,pc->sExcode,pe->sTradeDate);
		return -1;
	}
	int ntxdate=0;
	if (!m_pXdp->GetFieldValue((unsigned short)40,ntxdate,m_sErrMsg))
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托通知无交易日期 %s",m_sErrMsg);
		strcpy(order.sTxDate,pe->sTradeDate);
	}
	else
	{
		if (ntxdate != atoi(pe->sTradeDate))
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托通知 交易日期[%d]和交易所日期[%s]不符",ntxdate,pe->sTradeDate);
			return -1;
		}
		sprintf(order.sTxDate,"%d",ntxdate);
	}

	acclock = m_pMemDb->SelectLock(order.sTxAcc);
	if (acclock == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托通知资金账号[%s]不存在",order.sTxAcc);
		return -1;
	}
	S_MDB_CUST_FUNDACC_INFO *pcustacc = m_pMemDb->m_pDbCust.m_pCustFundAcc.At(acclock->nFundAccRowid);
	//S_MDB_CUST_FUNDACC_INFO *pcustacc = m_pMemDb->m_pDbCust.m_pCustFundAcc.SelectByAcc(order.sTxAcc);
	if (pcustacc == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托通知资金账号[%s]不存在",order.sTxAcc);
		return -1;
	}
	order.dAmt = order.dOrderPrice*pc->dTxUnit*order.nOrderNum;//金额
	
	//取交易编码
	S_MDB_CUST_TRADECODE *ptx = m_pMemDb->m_pDbTrader.m_pTradecode.Select(order.sTxAcc,order.sExchCode,order.sSeatNo);
	if (ptx != NULL)
	{
		strcpy(order.sTxCode,ptx->sTradeCode);
	}
	order.nCancelNum = 0;
	order.nRemainNum = order.nOrderNum;

	//查询本地报单号是否存在
	po = m_pMemDb->m_pDbTrader.m_pOrder.Select(order.sLocalSerial);
	if (po != NULL)
	{
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"委托通知 本地报单号[%s]已存在或已处理",order.sLocalSerial);
		return -1;
	}
	//这里的通知没有机构，统一默认为签约机构
	strcpy(order.sAccOrg,pcustacc->sSignOrg);
	strcpy(order.sTxOrg,pcustacc->sSignOrg);

	double dExchFee=0.0;//交易所手续费
	double dMemberFee=0.0;//会员手续费
	double dExchMargin=0.0;//交易所保证金
	double dMemberMargin=0.0;//会员保证金
	
	//取要冻结的手续费及保证金等等
	switch (pc->sProd_type[0])
	{
		case PROD_TYPE_FUTURES://期货
			if (order.sEoFlag[0] == EO_FLAG_OPEN) //开仓
			{
				//开仓要算冻结和手续费
				if (!m_pKernelPub.GetTradeFeeValue(pc,pcustacc,1,order.nOrderNum,order.dAmt,dExchFee,dMemberFee))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托通知取手续费出错 本地报单号[%s]",order.sLocalSerial);
					return -1;
				}
				if (!m_pKernelPub.GetMarginValue(pc,pcustacc,0,order.nOrderNum,order.dAmt,dExchMargin,dMemberMargin))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托通知取保证金出错 本地报单号[%s]",order.sLocalSerial);
					return -1;
				}
			}
			else
			{
				if (order.sEoFlag[0] == EO_FLAG_CLOSETODAY) //平今仓
				{
					//平仓要算手续费
					if (!m_pKernelPub.GetTradeFeeValue(pc,pcustacc,3,order.nOrderNum,order.dAmt,dExchFee,dMemberFee))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托通知取手续费出错 本地报单号[%s]",order.sLocalSerial);
						return -1;
					}
				}
				else
				{
					//平仓要算手续费
					if (!m_pKernelPub.GetTradeFeeValue(pc,pcustacc,2,order.nOrderNum,order.dAmt,dExchFee,dMemberFee))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托通知取手续费出错 本地报单号[%s]",order.sLocalSerial);
						return -1;
					}
				}
			}
			break;
		case PROD_TYPE_OPTION://期权
			if (order.sEoFlag[0] == EO_FLAG_OPEN) //开仓
			{
				//开仓要算冻结和手续费
				if (!m_pKernelPub.GetTradeFeeValue(pc,pcustacc,1,order.nOrderNum,order.dAmt,dExchFee,dMemberFee))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托通知取手续费出错 本地报单号[%s]",order.sLocalSerial);
					return -1;
				}
				if (order.sBsFlag[0] == BS_FLAG_SELL) //卖开要冻结保证金
				{
					if (!m_pKernelPub.GetMarginValue(pc,pcustacc,1,order.nOrderNum,order.dAmt,dExchMargin,dMemberMargin))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托通知取保证金出错 本地报单号[%s]",order.sLocalSerial);
						return -1;
					}
				}
			}
			else
			{
				//平仓要算手续费
				if (order.sEoFlag[0] == EO_FLAG_CLOSETODAY) //平今仓
				{
					//平仓要算手续费
					if (!m_pKernelPub.GetTradeFeeValue(pc,pcustacc,3,order.nOrderNum,order.dAmt,dExchFee,dMemberFee))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托通知取手续费出错 本地报单号[%s]",order.sLocalSerial);
						return -1;
					}
				}
				else
				{
					//平仓要算手续费
					if (!m_pKernelPub.GetTradeFeeValue(pc,pcustacc,2,order.nOrderNum,order.dAmt,dExchFee,dMemberFee))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托通知取手续费出错 本地报单号[%s]",order.sLocalSerial);
						return -1;
					}
				}
			}
			break;
		case PROD_TYPE_SPOT://现货
			//现货有中立仓、交割等
			if (order.sShFlag[0] == SH_FLAG_DERY)
			{
				//递延交割
			}
			else if (order.sShFlag[0] == SH_FLAG_MID)
			{
				//中立仓
			}
			else
			{
				if (order.sBsFlag[0] == BS_FLAG_BUY)
				{
					//买要算手续费及金额
					if (!m_pKernelPub.GetTradeFeeValue(pc,pcustacc,1,order.nOrderNum,order.dAmt,dExchFee,dMemberFee))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托通知取手续费出错 本地报单号[%s]",order.sLocalSerial);
						return -1;
					}
				}
				else
				{
					//卖要算手续费	
					if (!m_pKernelPub.GetTradeFeeValue(pc,pcustacc,2,order.nOrderNum,order.dAmt,dExchFee,dMemberFee))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托通知取手续费出错 本地报单号[%s]",order.sLocalSerial);
						return -1;
					}
				}
			}
			dMemberMargin = order.dAmt;
			break;
		case PROD_TYPE_SEC://证券
			if (order.sBsFlag[0] == BS_FLAG_BUY)
			{
				//买要算手续费及金额
				if (!m_pKernelPub.GetTradeFeeValue(pc,pcustacc,1,order.nOrderNum,order.dAmt,dExchFee,dMemberFee))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托通知取手续费出错 本地报单号[%s]",order.sLocalSerial);
					return -1;
				}
			}
			else
			{
				//卖要算手续费
				if (!m_pKernelPub.GetTradeFeeValue(pc,pcustacc,2,order.nOrderNum,order.dAmt,dExchFee,dMemberFee))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托通知取手续费出错 本地报单号[%s]",order.sLocalSerial);
					return -1;
				}
			}
			dMemberMargin = order.dAmt;
			break;
		default:
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托通知合约代码[%s]的品种类别[%s]不符",order.sContractCode,pc->sProd_type);
			return -1;
	}
	order.dExFee = 0.0 - (dExchFee+dMemberFee);//总手续费 为负数
	order.dFrozAmt = dExchMargin+dMemberMargin;//总保证金
	//报单时间
	m_pDate.Update();
	strcpy(order.sAnsTime,m_pDate.ToStringDT17().c_str());
	//取得费用和资金后，写入报单表

	int orderrid = m_pMemDb->m_pDbTrader.m_pOrder.Insert(order);
	if (orderrid <0)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托通知入委托表出错 本地报单号[%s]",order.sLocalSerial);
		return -1;
	}
	po = m_pMemDb->m_pDbTrader.m_pOrder.At(orderrid);
	if (po == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托通知  委托表出错 rowid[%d]找不到委托",orderrid);
		return -1;
	}
	po->nSyncFlag = 1; //报入同步


	S_MDB_CUST_FUND *pfund = m_pMemDb->GetFund(acclock,pc->sCurrency);
	if (pfund == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托通知 找不到资金账号资金信息 [%s %s]",order.sTxAcc,pc->sCurrency);
		return -1;
	}
	
	//更新资金、冻结等
	if (m_pKernelPub.ComputeNewOrder(acclock,pfund,po,pc,false) != SUCCESS)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托通知 更新资金 持仓等失败");
		return -1;
	}
	nEndTime=CBF_Date_Time::GetTickUS();
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"委托通知 处理完成 本地报单号[%s]  处理时间[%d]微秒",po->sLocalSerial,nEndTime-nBeginTime);

	//通知要对此客户进行风险计算
	MsgCustRiskCalc(acclock->nLockRowid);
	//发送委托回报推送
	SendOrderPush(acclock,po);
	return 0;
}

void RiskDll::AnsData(S_TRADE_DATA &data)
{
	data.pData->sDBHead.cRaflag = 1;
	m_pClientLink->Send2Client(data);
}

bool RiskDll::UpdateOrder2Db(S_MDB_ORDER_INFO *porder)
{
	if (m_pSqlCmd == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"Not connect risk database");
		return false;
	}
	sprintf(m_sSql,"select TX_ACC from ORDER_INFO where LOCAL_NO=:1");
	m_pSqlCmd->setCommandText(m_sSql);
	m_pSqlCmd->Param(1).setAsString() = porder->sLocalSerial;
	if (!m_pSqlCmd->Execute())
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"Execute %s error %s",m_sSql,m_pSqlCmd->GetCmdErrInfo());
		return false;
	}
	if (m_pSqlCmd->FetchNext())
	{
		//已有此报单，直接返回
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"数据库已有此报单 本地报单号[%s]",porder->sLocalSerial);
		return true;
	}

	sprintf(m_sSql,"insert into ORDER_INFO(TX_ACC,EX_CODE,SEAT_NO,TX_CODE,CONTRACT_NO,CONTRACT_CODE,CLIENT_SERIAL,LOCAL_NO,SYSTEM_NO,ORDER_NUM,ORDER_PRICE,REMAIN_NUM,EO_FLAG,BS_FLAG,SH_FLAG,TXDATE,AMT,FROZ_AMT,EX_FARE,TX_TYPE,CMD_TYPE,CMD_ATTR,FORCE_FLAG,ORDER_STATUS,CANCEL_FLAG,ANS_TIME,ORDER_CHANNEL,ACC_ORG,TX_ORG,ORDER_OPER,CANCEL_CHANNEL,CANCEL_OPER,CANCEL_TIME,ORDER_RSP,FROZ_USEAMT,FROZ_OUTAMT,CANCEL_NUM,DONE_AVP,VARIETY) values(:1,:2,:3,:4,:5,:6,:7,:8,:9,:10,:11,:12,:13,:14,:15,:16,:17,:18,:19,:20,:21,:22,:23,:24,:25,:26,:27,:28,:29,:30,:31,:32,:33,:34,:35,:36,:37,:38,:39) ");
	m_pSqlCmd->setCommandText(m_sSql);
	m_pSqlCmd->Param(1).setAsString() = porder->sTxAcc;
	m_pSqlCmd->Param(2).setAsString() = porder->sExchCode;
	m_pSqlCmd->Param(3).setAsString() = porder->sSeatNo;
	m_pSqlCmd->Param(4).setAsString() = porder->sTxCode;
	m_pSqlCmd->Param(5).setAsLong() = porder->nContractNo;
	m_pSqlCmd->Param(6).setAsString() = porder->sContractCode;
	m_pSqlCmd->Param(7).setAsString() = porder->sClientSerial;
	m_pSqlCmd->Param(8).setAsString() = porder->sLocalSerial;
	m_pSqlCmd->Param(9).setAsString() = porder->sSystemNo;
	m_pSqlCmd->Param(10).setAsLong() = porder->nOrderNum;
	m_pSqlCmd->Param(11).setAsDouble() = porder->dOrderPrice;
	m_pSqlCmd->Param(12).setAsLong() = porder->nRemainNum;
	m_pSqlCmd->Param(13).setAsString() = porder->sEoFlag;
	m_pSqlCmd->Param(14).setAsString() = porder->sBsFlag;
	m_pSqlCmd->Param(15).setAsString() = porder->sShFlag;
	m_pSqlCmd->Param(16).setAsString() = porder->sTxDate;
	m_pSqlCmd->Param(17).setAsDouble() = porder->dAmt;
	m_pSqlCmd->Param(18).setAsDouble() = porder->dFrozAmt;
	m_pSqlCmd->Param(19).setAsDouble() = porder->dExFee;
	m_pSqlCmd->Param(20).setAsString() = porder->sTxType;
	m_pSqlCmd->Param(21).setAsString() = porder->sCmdType;
	m_pSqlCmd->Param(22).setAsString() = porder->sCmdAttr;
	m_pSqlCmd->Param(23).setAsString() = porder->sForceFlag;
	m_pSqlCmd->Param(24).setAsString() = porder->sOrderStatus;
	m_pSqlCmd->Param(25).setAsString() = porder->sCancelFlag;
	m_pSqlCmd->Param(26).setAsString() = porder->sAnsTime;
	m_pSqlCmd->Param(27).setAsString() = porder->sOrderChannel;
	m_pSqlCmd->Param(28).setAsString() = porder->sAccOrg;
	m_pSqlCmd->Param(29).setAsString() = porder->sTxOrg;
	m_pSqlCmd->Param(30).setAsString() = porder->sOrderOper;
	m_pSqlCmd->Param(31).setAsString() = porder->sCancelChannel;
	m_pSqlCmd->Param(32).setAsString() = porder->sCancelOper;
	m_pSqlCmd->Param(33).setAsString() = porder->sCancelTime;
	m_pSqlCmd->Param(34).setAsString() = porder->sOrderRsp;
	m_pSqlCmd->Param(35).setAsDouble() = porder->dFrozUseamt;
	m_pSqlCmd->Param(36).setAsDouble() = porder->dFrozOutamt;
	m_pSqlCmd->Param(37).setAsLong() = porder->nCancelNum;
	m_pSqlCmd->Param(38).setAsDouble() = porder->dDoneAvp;
	m_pSqlCmd->Param(39).setAsLong() = porder->nVarietyId;
	if (!m_pSqlCmd->Execute())
	{
		m_pSqlCmd->Rollback();
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"Execute %s error %s",m_sSql,m_pSqlCmd->GetCmdErrInfo());
		return false;
	}
	m_pSqlCmd->Commit();
	return true;
}


bool RiskDll::ChgDateProcByMarket(char *market,char *curdate)
{
	//删除该市场下所有的客户资金、持仓、成交、报单、特殊费率
	std::vector<S_MDB_EXCH_INFO *> exlist ;
	if (!m_pMemDb->m_pDbBaseinfo.m_pExchange.SelectByMarket(market,exlist))
	{
		return true;
	}
	int i,j;
	CTbl_Char_Key custlist;//保存此市场下所有的资金账号
	std::vector<S_MDB_CUST_TRADECODE *> tclist;
	for (i=0 ; i < exlist.size(); i++)
	{
		tclist.clear();
		if (!m_pMemDb->m_pDbTrader.m_pTradecode.SelectByExch(exlist[i]->sExCode,tclist))
		{
			continue;
		}
		for (j=0; j<tclist.size();j++)
		{
			custlist.Insert(tclist[j]->sTxAcc);
		}
		//删除报单
		m_pMemDb->m_pDbTrader.m_pOrder.Delete(curdate,exlist[i]->sExCode);
		//删除成交
		m_pMemDb->m_pDbTrader.m_pDone.Delete(curdate,exlist[i]->sExCode);

	}
	
	//删除这些账号的出入金
	if (!DeleteMoneyIO(custlist,curdate))
	{
		return false;
	}
	//重置这些资金账号的信息
	if (!ResetCustInfo(custlist))
	{
		return false;
	}

	if (!ReadInfoFromDbByMarket(market))
	{
		return false;
	}
	//置市场的状态为日切完成
	S_MDB_MARKET_INFO *pMarket = m_pMemDb->m_pDbBaseinfo.m_pMarket.Select(market);
	if (pMarket == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"市场[%s]未找到",market);
		return true;
	}
//	pMarket->sClearStatus[0] = CLEAR_STATUS_DATEEND;
	return true;
}

bool RiskDll::ChgDateProcByExchange(char *exchange,char *curdate)
{
	//删除该交易所下所有的客户资金、持仓、成交、报单、特殊费率
	//根据交易所找客户

	int j;
	CTbl_Char_Key custlist;//保存此市场下所有的资金账号
	
	std::vector<S_MDB_CUST_TRADECODE *> tclist;
	if (!m_pMemDb->m_pDbTrader.m_pTradecode.SelectByExch(exchange,tclist))
	{
		//根据交易编码表查找此交易所对应的资金账号
		m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"交易所[%s] 交易编码对应表未找到资金账号",exchange);
		return true;
	}
	for (j=0; j<tclist.size();j++)
	{
		custlist.Insert(tclist[j]->sTxAcc);
	}
	//删除这些账号的出入金
	if (!DeleteMoneyIO(custlist,curdate))
	{
		return false;
	}
	//重置这些资金账号的信息
	if (!ResetCustInfo(custlist))
	{
		return false;
	}
	if (!ReadInfoFromDbByExch(exchange))
	{
		return false;
	}
	//置交易所的状态为日切完成
	S_MDB_EXCH_INFO *pexch = m_pMemDb->m_pDbBaseinfo.m_pExchange.Select(exchange);
	if (pexch == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"交易所[%s]未找到",exchange);
		return true;
	}
	pexch->sClearStatus[0] = CLEAR_STATUS_DATEEND;
	return true;
}

bool RiskDll::ResetCustInfo(CTbl_Char_Key &custlist)
{
	S_MDB_TXACC_LOCK *acclock=NULL;
	S_MDB_CUST_FUND *pfund=NULL;
	std::vector<S_MDB_CUST_FUND *>fundlist;
	char *stxacc = custlist.First();
	int i;
	while (stxacc != NULL)
	{
		acclock = m_pMemDb->SelectLock(stxacc);
		if (acclock == NULL)
		{
			stxacc = custlist.Next();
			continue;
		}
		
		//资金表
		if (!m_pMemDb->m_pDbTrader.m_pFund.Select(acclock->sTxAcc,fundlist))
		{
			stxacc = custlist.Next();
			continue;
		}
		for (i=0; i<fundlist.size(); i++)
		{
			pfund = fundlist[i];
			pfund->dArap =0.0;
			pfund->dLast_bal =0.0;//上日结存
			pfund->dBalance =0.0;//当日结存
			pfund->dAvailFund =0.0;//当日可用
			pfund->dGetFund =0.0;//当日可提
			pfund->dBaseMargin =0.0;//基础保证金
			pfund->dIncomeExpense =0.0;//收付款项
			pfund->dFrozeFund =0.0;//冻结资金
			pfund->dMargin =0.0;//保证金
			pfund->dFee =0.0;//费用
			pfund->dReserve =0.0;//备付金
			pfund->dSurplus =0.0;//盈亏
			pfund->dFloatSurplus =0.0;//浮动盈亏
			pfund->dIncrInte =0.0;//利息
			pfund->dArap =0.0;//应收应付
			pfund->nRisk_level = 11;//风险级别
			pfund->dRisk =0.0;//风险度
		}
		//重置资金明细
		m_pMemDb->m_pDbTrader.m_pFundDetail.Reset(stxacc);
		//重置持仓
		m_pMemDb->m_pDbTrader.m_pPosition.Reset(stxacc);
		//重置日间风险
		m_pMemDb->m_pDbTrader.m_pRiskTick.Reset(stxacc);

		//删除客户特殊费率，待重新读取
		if (!m_pMemDb->m_pDbSet.m_pCustFeeDetail.DeleteByAcc(stxacc))
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"删除资金账号[%s]的特殊费率设置出错",stxacc);
		}
		//删除持仓风险信息
		m_pMemDb->m_pDbTrader.m_pCustPosiRisk.Delete(stxacc);

		//删除风险tick信息
		m_pMemDb->m_pDbTrader.m_pRiskTick.Delete(stxacc);
		//通知日切的账户重算一遍风险
		MsgCustRiskCalc(acclock->nLockRowid);
		stxacc = custlist.Next();
	}
	return true;
}

bool RiskDll::ReadInfoFromDbByMarket(char *market)
{
// 	if (m_pSqlCmd == NULL)
// 	{
// 		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"Not connect risk database");
// 		return false;
// 	}
	//找到此市场的所有客户  注意资金账号不能跨多个市场
	sprintf(m_sSql,"select distinct a.TX_ACC from CUST_TRADE_INFO a,EX_INFO b where a.EX_CODE=b.EX_CODE and b.EX_TYPE=:1");
	m_pSqlCmd->setCommandText(m_sSql);
	m_pSqlCmd->Param(1).setAsString() = market;
	if (!m_pSqlCmd->Execute())
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"按市场读数据出错 %s %s",m_pSqlCmd->GetCmdErrInfo(),m_sSql);
		return false;
	}
	CTbl_Char_Key acclist;
	bool bret = m_pSqlCmd->FetchNext();
	while (bret )
	{
		acclist.Insert(m_pSqlCmd->Field(0).asString().GetBuffer(m_pSqlCmd->Field(0).asString().GetLength()));
		bret = m_pSqlCmd->FetchNext();
	}
	char *txacc = acclist.First();
	while (txacc != NULL)
	{
		ReadOneCustInfo(txacc);
		txacc = acclist.Next();
	}
	return true;
}

bool RiskDll::ReadInfoFromDbByExch(char *exch)
{
// 	if (m_pSqlCmd == NULL)
// 	{
// 		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"Not connect risk database");
// 		return false;
// 	}
	//找到此交易所的所有客户 注意资金账号不能跨多个交易所 
	sprintf(m_sSql,"select distinct a.TX_ACC from CUST_TRADE_INFO a where a.EX_CODE=:1");
	m_pSqlCmd->setCommandText(m_sSql);
	m_pSqlCmd->Param(1).setAsString() = exch;
	if (!m_pSqlCmd->Execute())
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"按交易所读数据出错 %s %s",m_pSqlCmd->GetCmdErrInfo(),m_sSql);
		return false;
	}
	CTbl_Char_Key acclist;
	bool bret = m_pSqlCmd->FetchNext();
	while (bret )
	{
		acclist.Insert(m_pSqlCmd->Field(0).asString().GetBuffer(m_pSqlCmd->Field(0).asString().GetLength()));
		bret = m_pSqlCmd->FetchNext();
	}
	char *txacc = acclist.First();
	while (txacc != NULL)
	{
		ReadOneCustInfo(txacc);
		txacc = acclist.Next();
	}
	return true;
}

bool RiskDll::ReadOneCustInfo(const char *txacc)
{
	bool bret;
	int fundaccrid=-1;
	int custrowid=-1;
	int fundrowid  = -1;
	int rowid;
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"日切 开始读取 资金账号[%s]的信息",txacc);
	//读资金账号信息
	S_MDB_CUST_FUNDACC_INFO *pFundAcc = m_pMemDb->m_pDbCust.m_pCustFundAcc.SelectByAcc((char *)txacc);
	if (pFundAcc == NULL)
	{
		//没有此资金账号，是新增加的
		S_MDB_CUST_FUNDACC_INFO info;
		sprintf(m_sSql,"select TX_ACC,CUST_NO,ACC_TYPE,ACC_SIMPLENAME,SIGN_ORG,MANAGER_ORG,FUTURES_FLAG,ACC_LEVEL,EX_FEE,MEMBER_FEE,BROKERAGE_CODE,CURRENCY,FS_FEE_MODEL,ACC_NAME from CUST_FUNDACC_INFO where TX_ACC=:1 ");
		m_pSqlCmd->setCommandText(m_sSql);
		m_pSqlCmd->Param(1).setAsString() = txacc;
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"日切 读取新增的 资金账号[%s]的信息  %s",txacc,m_sSql);
		if (!m_pSqlCmd->Execute())
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"日切  Execute <%s> error %d %s",m_sSql,m_pSqlCmd->m_dbconn.m_nErrCode,m_pSqlCmd->m_dbconn.m_sErrInfo.c_str());
			return false;
		}
		bret = m_pSqlCmd->FetchNext();
		if ( !bret )
		{
#ifdef _ENGLISH_
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"日切  Table CUST_FUNDACC_INFO no %s data",txacc);
#else
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"日切  资金账号CUST_FUNDACC_INFO表无 %s 数据 ",txacc);
#endif
			return true;
		}

		strcpy(info.sTxAcc,m_pSqlCmd->Field(0).asString());
		strcpy(info.sCustNo,m_pSqlCmd->Field(1).asString());
		strcpy(info.sAcctype,m_pSqlCmd->Field(2).asString());
		strcpy(info.sCustSimpleName,m_pSqlCmd->Field(3).asString());
		strcpy(info.sSignOrg,m_pSqlCmd->Field(4).asString());
		strcpy(info.sMgrOrg,m_pSqlCmd->Field(5).asString());
		strcpy(info.sFutureFlag,m_pSqlCmd->Field(6).asString());
		strcpy(info.sAccLevel,m_pSqlCmd->Field(7).asString());
		strcpy(info.sExchFareModel,m_pSqlCmd->Field(8).asString());
		strcpy(info.sMemberFareModel,m_pSqlCmd->Field(9).asString());
		info.nSysId = m_pSqlCmd->Field(10).asLong();
		strcpy(info.sCurrency,m_pSqlCmd->Field(11).asString());
		strcpy(info.sFsFeeModel,m_pSqlCmd->Field(12).asString());
		strcpy(info.sCustName,m_pSqlCmd->Field(13).asString());
		fundaccrid = m_pMemDb->m_pDbCust.m_pCustFundAcc.Insert(info);
		if (fundaccrid <0 )
		{
#ifdef _ENGLISH_
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"日切  CUST_FUNDACC_INFO [%s] repeat",info.sTxAcc);
#else
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"日切  资金账号CUST_FUNDACC_INFO表 [%s]重复",info.sTxAcc);
#endif
			return false;
		}
		pFundAcc = m_pMemDb->m_pDbCust.m_pCustFundAcc.At(fundaccrid);
		//读客户信息
		S_MDB_CUSTINFO cinfo;
		
		sprintf(m_sSql,"select CUST_NO,CUST_NAME,CUST_TYPE,CUST_CERT,CUST_CERT_NO,CUST_PHONE,TXCHANNLE,PKI_CERT_SERIAL,PKI_CERT_DATE,CUST_STATUS,LAST_RISK_LEVEL from CUST_BASIC_INFO where CUST_NO=:1");
		m_pSqlCmd->setCommandText(m_sSql);
		m_pSqlCmd->Param(1).setAsString() = info.sCustNo;
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"日切 读取新增的 资金账号[%s] 客户号[%s]的信息  %s",txacc,info.sCustNo,m_sSql);
		if (!m_pSqlCmd->Execute())
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"日切  Execute <%s> error %d %s",m_sSql,m_pSqlCmd->m_dbconn.m_nErrCode,m_pSqlCmd->m_dbconn.m_sErrInfo.c_str());
			return false;
		}
		bret = m_pSqlCmd->FetchNext();
		if ( !bret )
		{
#ifdef _ENGLISH_
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"日切  Table CUST_BASIC_INFO no data");
#else
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"日切  客户表CUST_BASIC_INFO无数据 ");
#endif
			return false;
		}
		strcpy(cinfo.sCustNo,m_pSqlCmd->Field(0).asString());
		strcpy(cinfo.sCustName,m_pSqlCmd->Field(1).asString());
		strcpy(cinfo.sCustType,m_pSqlCmd->Field(2).asString());
		strcpy(cinfo.sCertType,m_pSqlCmd->Field(3).asString());
		strcpy(cinfo.sCertNo,m_pSqlCmd->Field(4).asString());
		strcpy(cinfo.sMobile,m_pSqlCmd->Field(5).asString());
		strcpy(cinfo.sChannleOpen,m_pSqlCmd->Field(6).asString());
		strcpy(cinfo.sPkiCertNo,m_pSqlCmd->Field(7).asString());
		strcpy(cinfo.sPkiCertDate,m_pSqlCmd->Field(8).asString());
		strcpy(cinfo.sStatus,m_pSqlCmd->Field(9).asString());
		cinfo.dLastRisk = m_pSqlCmd->Field(10).asDouble();
		custrowid = m_pMemDb->m_pDbCust.m_pCustom.Insert(cinfo);
		if (custrowid <0 )
		{
#ifdef _ENGLISH_
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"日切  custom [%s] repeat",cinfo.sCustNo);
#else
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"日切  客户表 [%s]重复",cinfo.sCustNo);
#endif
			return false;
		}
			
	}
	else //更新模板信息
	{
		S_MDB_CUST_FUNDACC_INFO info;
		sprintf(m_sSql,"select TX_ACC,CUST_NO,ACC_TYPE,ACC_SIMPLENAME,SIGN_ORG,MANAGER_ORG,FUTURES_FLAG,ACC_LEVEL,EX_FEE,MEMBER_FEE,BROKERAGE_CODE,CURRENCY,FS_FEE_MODEL,ACC_NAME from CUST_FUNDACC_INFO where TX_ACC=:1 ");
		m_pSqlCmd->setCommandText(m_sSql);
		m_pSqlCmd->Param(1).setAsString() = txacc;

		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"日切 读取 资金账号[%s]的 信息 更新 %s",txacc,m_sSql);
		if (!m_pSqlCmd->Execute())
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"日切  Execute <%s> error %d %s",m_sSql,m_pSqlCmd->m_dbconn.m_nErrCode,m_pSqlCmd->m_dbconn.m_sErrInfo.c_str());
			return false;
		}
		bret = m_pSqlCmd->FetchNext();
		if ( !bret )
		{
#ifdef _ENGLISH_
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"日切  Table CUST_FUNDACC_INFO no data");
#else
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"日切  资金账号CUST_FUNDACC_INFO表无数据 ");
#endif
			return true;
		}
		
		strcpy(info.sTxAcc,m_pSqlCmd->Field(0).asString());
		strcpy(info.sCustNo,m_pSqlCmd->Field(1).asString());
		strcpy(info.sAcctype,m_pSqlCmd->Field(2).asString());
		strcpy(info.sCustSimpleName,m_pSqlCmd->Field(3).asString());
		strcpy(info.sSignOrg,m_pSqlCmd->Field(4).asString());
		strcpy(info.sMgrOrg,m_pSqlCmd->Field(5).asString());
		strcpy(info.sFutureFlag,m_pSqlCmd->Field(6).asString());
		strcpy(info.sAccLevel,m_pSqlCmd->Field(7).asString());
		strcpy(info.sExchFareModel,m_pSqlCmd->Field(8).asString());
		strcpy(info.sMemberFareModel,m_pSqlCmd->Field(9).asString());
		info.nSysId = m_pSqlCmd->Field(10).asLong();
		strcpy(info.sCurrency,m_pSqlCmd->Field(11).asString());
		strcpy(info.sFsFeeModel,m_pSqlCmd->Field(12).asString());
		strcpy(info.sCustName,m_pSqlCmd->Field(13).asString());
		
		fundaccrid = m_pMemDb->m_pDbCust.m_pCustFundAcc.Update(info);
		if (fundaccrid <0 )
		{
#ifdef _ENGLISH_
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"日切  CUST_FUNDACC_INFO [%s] update error",info.sTxAcc);
#else
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"日切  资金账号CUST_FUNDACC_INFO表 [%s] 更新失败",info.sTxAcc);
#endif
			return false;
		}
	}
	//读资金信息
	S_MDB_CUST_FUND finfo;
	S_MDB_CUST_FUND *pFund;
	sprintf(m_sSql,"select TX_ACC,CURRENCY,LAST_BAL,BALANCE,AVAIL_FUND,GET_FUND,BASE_MARGIN,INCOME_EXPENSE,FROZE_FUND,MARGIN,FEE,RESERVE,SURPLUS,FLOAT_SURPLUS,INCR_INTE,ARAP,RISK_LEVEL,RISK,TRADE_RIGHT from CUST_FUND where TX_ACC=:1");
	m_pSqlCmd->setCommandText(m_sSql);
	m_pSqlCmd->Param(1).setAsString() = txacc;
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"日切 读取 资金账号[%s]的 资金信息 %s",txacc,m_sSql);
	if (!m_pSqlCmd->Execute())
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"日切  Execute <%s> error %d %s",m_sSql,m_pSqlCmd->m_dbconn.m_nErrCode,m_pSqlCmd->m_dbconn.m_sErrInfo.c_str());
		return false;
	}
	bret = m_pSqlCmd->FetchNext();
	if (!bret)
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"日切  资金账号[%s]在资金表中不存在",txacc);
		return true;
	}
	bool acclockselect =false;
	while (bret)
	{
		strcpy(finfo.sTxAcc ,m_pSqlCmd->Field(0).asString());
		strcpy(finfo.sCurrency,m_pSqlCmd->Field(1).asString());
		finfo.dLast_bal =m_pSqlCmd->Field(2).asDouble();
		finfo.dBalance =m_pSqlCmd->Field(3).asDouble();
		finfo.dAvailFund =m_pSqlCmd->Field(4).asDouble();
		finfo.dGetFund =m_pSqlCmd->Field(5).asDouble();
		finfo.dBaseMargin =m_pSqlCmd->Field(6).asDouble();
		finfo.dIncomeExpense =m_pSqlCmd->Field(7).asDouble();
		finfo.dFrozeFund =m_pSqlCmd->Field(8).asDouble();
		finfo.dMargin =m_pSqlCmd->Field(9).asDouble();
		finfo.dFee =m_pSqlCmd->Field(10).asDouble();
		finfo.dReserve =m_pSqlCmd->Field(11).asDouble();
		finfo.dSurplus =m_pSqlCmd->Field(12).asDouble();
		finfo.dFloatSurplus =m_pSqlCmd->Field(13).asDouble();
		finfo.dIncrInte =m_pSqlCmd->Field(14).asDouble();
		finfo.dArap =m_pSqlCmd->Field(15).asDouble();
		finfo.nRisk_level =m_pSqlCmd->Field(16).asLong();
		finfo.dRisk =m_pSqlCmd->Field(17).asDouble();
		strcpy(finfo.sTradeRight,m_pSqlCmd->Field(18).asString());
		pFund =  m_pMemDb->m_pDbTrader.m_pFund.Select(finfo.sTxAcc,finfo.sCurrency);
		if (pFund == NULL)
		{
			fundrowid = m_pMemDb->m_pDbTrader.m_pFund.Insert(finfo);
			if (fundrowid <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"日切  CUST_FUND [%s] repeat",finfo.sTxAcc);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"日切  资金表 CUST_FUND [%s]重复",finfo.sTxAcc);
#endif
				return false;
			}
			acclockselect =true; //增加了账户币种，需要重新初始
			pFund = m_pMemDb->m_pDbTrader.m_pFund.At(fundrowid);
		}
		else
		{
			//修改更新资金表
			pFund->dLast_bal = finfo.dLast_bal;
			pFund->dBalance = finfo.dBalance ;
			pFund->dAvailFund = finfo.dAvailFund ;
			pFund->dGetFund = finfo.dGetFund ;
			pFund->dBaseMargin = finfo.dBaseMargin ;
			pFund->dIncomeExpense = finfo.dIncomeExpense ;
			pFund->dFrozeFund = finfo.dFrozeFund ;
			pFund->dMargin = finfo.dMargin ;
			pFund->dFee = finfo.dFee ;
			pFund->dReserve = finfo.dReserve ;
			pFund->dSurplus = finfo.dSurplus ;
			pFund->dFloatSurplus = finfo.dFloatSurplus ;
			pFund->dIncrInte = finfo.dIncrInte ;
			pFund->dArap = finfo.dArap ;
			pFund->nRisk_level = finfo.nRisk_level ;
			pFund->dRisk = finfo.dRisk ;
			strcpy(pFund->sTradeRight,finfo.sTradeRight);
		}
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"读资金账号[%s %s]的资金信息 上日结存[%lf] 今日结存[%lf]",pFund->sTxAcc,pFund->sCurrency,pFund->dLast_bal,pFund->dBalance);
		bret = m_pSqlCmd->FetchNext();
	}
	
	S_MDB_TXACC_LOCK *pacclock=m_pMemDb->SelectLock((char *)txacc);
	if (pacclock == NULL)
	{
		S_MDB_TXACC_LOCK  acclock;
		strcpy(acclock.sTxAcc,finfo.sTxAcc);
		acclock.nFundAccRowid=-1;
		acclock.nCustRowid=-1;
		int rid = m_pMemDb->InsertAccLock(acclock);
		if (rid<0)
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"日切  InsertAccLock error");
			return false;
		}
		pacclock = m_pMemDb->SelectLock(rid);
	}
	else
	{
		if (acclockselect)
		{
			//重新读取一遍,可能增加了币种
			pacclock->nFundAccRowid = -1;
			pacclock=m_pMemDb->SelectLock((char *)txacc);
		}
	}
	//读取资金详细表
	S_MDB_CUST_FUND_DETAIL fdinfo;
	sprintf(m_sSql,"select TX_ACC,CURRENCY,FUND_TYPE,FUND_CODE,FUND_VALUE from CUST_FUND_DETAIL where TX_ACC=:1");
	m_pSqlCmd->setCommandText(m_sSql);
	m_pSqlCmd->Param(1).setAsString() = txacc;
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"日切 读取 资金账号[%s]的 资金详细信息 %s",txacc,m_sSql);
	if (!m_pSqlCmd->Execute())
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"日切  Execute <%s> error %d %s",m_sSql,m_pSqlCmd->m_dbconn.m_nErrCode,m_pSqlCmd->m_dbconn.m_sErrInfo.c_str());
		return false;
	}
	bret = m_pSqlCmd->FetchNext();
	while (bret)
	{
		strcpy(fdinfo.sTxAcc ,m_pSqlCmd->Field(0).asString());
		strcpy(fdinfo.sCurrency ,m_pSqlCmd->Field(1).asString());
		strcpy(fdinfo.sFundType,m_pSqlCmd->Field(2).asString());
		strcpy(fdinfo.sFundCode,m_pSqlCmd->Field(3).asString());
		fdinfo.dFundValue =m_pSqlCmd->Field(4).asDouble();
		
		S_MDB_CUST_FUND_DETAIL *pfdinfo=  m_pMemDb->m_pDbTrader.m_pFundDetail.Select(fdinfo.sTxAcc,fdinfo.sCurrency,fdinfo.sFundType,fdinfo.sFundCode);
		if (pfdinfo == NULL)
		{
			//新增资金明细
			if (m_pMemDb->m_pDbTrader.m_pFundDetail.Insert(fdinfo) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"日切  CUST_FUND_DETAIL [%s] repeat",fdinfo.sTxAcc);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"日切  资金详细表 CUST_FUND_DETAIL [%s]重复",fdinfo.sTxAcc);
#endif
				return false;
			}
		}
		else
		{
			//修改更新资金明细
			pfdinfo->dFundValue = fdinfo.dFundValue;
		}
		
		bret = m_pSqlCmd->FetchNext();
	}
	//读特殊费率设置
	S_MDB_CUST_FEE custfeeinfo;
	sprintf(m_sSql,"select a.TX_ACC,a.PROD_TYPE,a.VARIETY_NO,a.VARIETY_CODE,a.CONTRACT_NO,a.CONTRACT_CODE,a.FEE_NO,a.FEE_MODE,a.FEE_VALUE,a.EX_INCLUDE from CUST_FEE_RATE a where a.TX_ACC=:1");

	m_pSqlCmd->setCommandText(m_sSql);
	m_pSqlCmd->Param(1).setAsString() = txacc;
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"日切 读取 资金账号[%s]的 特殊费率设置 %s",txacc,m_sSql);
	if (!m_pSqlCmd->Execute())
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"日切  Execute <%s> error %d %s",m_sSql,m_pSqlCmd->m_dbconn.m_nErrCode,m_pSqlCmd->m_dbconn.m_sErrInfo.c_str());
		return false;
	}
	
	bret = m_pSqlCmd->FetchNext();
	while (bret)
	{
		strcpy(custfeeinfo.sTxAcc ,m_pSqlCmd->Field(0).asString());
		strcpy(custfeeinfo.sProdType,m_pSqlCmd->Field(1).asString());
		custfeeinfo.nVarietyNo  = m_pSqlCmd->Field(2).asLong();
		strcpy(custfeeinfo.sVarietyCode,m_pSqlCmd->Field(3).asString());
		custfeeinfo.nContractNo  = m_pSqlCmd->Field(4).asLong();
		strcpy(custfeeinfo.sContractCode,m_pSqlCmd->Field(5).asString());
		strcpy(custfeeinfo.sFareNo,m_pSqlCmd->Field(6).asString());
		strcpy(custfeeinfo.sFareMode,m_pSqlCmd->Field(7).asString());
		custfeeinfo.dValue = m_pSqlCmd->Field(8).asDouble();
		
		if (m_pMemDb->m_pDbSet.m_pCustFeeDetail.Insert(custfeeinfo) <0 )
		{
#ifdef _ENGLISH_
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"日切  CUST_FEE_RATE [%s] repeat",custfeeinfo.sTxAcc);
#else
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"日切  客户特殊费率设置表 CUST_FEE_RATE [%s]重复",custfeeinfo.sTxAcc);
#endif
			return false;
		}
		bret = m_pSqlCmd->FetchNext();
	}

	//读持仓信息
	S_MDB_CUST_POSITION cpinfo;
	sprintf(m_sSql,"select TX_ACC,CONTRACT_NO,CONTRACT_CODE,SEAT_ID,TX_CODE,TB_FLAG,LAST_PB,LAST_PS,CUR_PB,CUR_PS,CUR_CANUSE_PB,CUR_CANUSE_PS,DAY_PB,DAY_PS,DAY_COV_PB_FROZ,DAY_COV_PB,DAY_COV_PS_FROZ,DAY_COV_PS,DAY_DELI_PB_FROZ,DAY_DELI_PB,DAY_DELI_PS_FROZ,DAY_DELI_PS,DAY_MIDD_PB,DAY_MIDD_PS,PB_OPEN_AVG_PRICE,PS_OPEN_AVG_PRICE,PB_AVG_PRICE,PS_AVG_PRICE,PB_MARGIN,PS_MARGIN,DAY_PB_SURPLUS,DAY_PS_SURPLUS,LAST_SETTLE_PRICE,DAY_SETTLE_PRICE,VARIETY from CUST_POSI where TX_ACC=:1");
	m_pSqlCmd->setCommandText(m_sSql);
	m_pSqlCmd->Param(1).setAsString() = txacc;

	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"日切 读取 资金账号[%s]的 持仓信息 %s",txacc,m_sSql);
	if (!m_pSqlCmd->Execute())
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"日切  Execute <%s> error %d %s",m_sSql,m_pSqlCmd->m_dbconn.m_nErrCode,m_pSqlCmd->m_dbconn.m_sErrInfo.c_str());
		return false;
	}
	S_MDB_CONTRACT_INFO *pc=NULL;
	bret = m_pSqlCmd->FetchNext();
	while (bret)
	{
		strcpy(cpinfo.sTxAcc ,m_pSqlCmd->Field(0).asString());
		cpinfo.nContractNo = m_pSqlCmd->Field(1).asLong();
		strcpy(cpinfo.sContractCode,m_pSqlCmd->Field(2).asString());
		strcpy(cpinfo.sSeatNo,m_pSqlCmd->Field(3).asString());
		strcpy(cpinfo.sTradeCode,m_pSqlCmd->Field(4).asString());
		strcpy(cpinfo.sShFlag,m_pSqlCmd->Field(5).asString());
		cpinfo.nLastPB =m_pSqlCmd->Field(6).asLong();
		cpinfo.nLastPS =m_pSqlCmd->Field(7).asLong();
		cpinfo.nCurPB =m_pSqlCmd->Field(8).asLong();
		cpinfo.nCurPS =m_pSqlCmd->Field(9).asLong();
		cpinfo.nCurCanUsePB =m_pSqlCmd->Field(10).asLong();
		cpinfo.nCurCanUsePS =m_pSqlCmd->Field(11).asLong();
		cpinfo.nDayPB =m_pSqlCmd->Field(12).asLong();
		cpinfo.nDayPS =m_pSqlCmd->Field(13).asLong();
		cpinfo.nDAY_COV_PB_Froze =m_pSqlCmd->Field(14).asLong();
		cpinfo.nDAY_COV_PB =m_pSqlCmd->Field(15).asLong();
		cpinfo.nDAY_COV_PS_Froze =m_pSqlCmd->Field(16).asLong();
		cpinfo.nDAY_COV_PS =m_pSqlCmd->Field(17).asLong();
		cpinfo.nDAY_DELI_PB_Froze =m_pSqlCmd->Field(18).asLong();
		cpinfo.nDAY_DELI_PB =m_pSqlCmd->Field(19).asLong();
		cpinfo.nDAY_DELI_PS_Froze =m_pSqlCmd->Field(20).asLong();
		cpinfo.nDAY_DELI_PS =m_pSqlCmd->Field(21).asLong();
		cpinfo.nDAY_MIDD_PB =m_pSqlCmd->Field(22).asLong();
		cpinfo.nDAY_MIDD_PS =m_pSqlCmd->Field(23).asLong();
		cpinfo.dPB_OAvp =m_pSqlCmd->Field(24).asDouble();
		cpinfo.dPS_OAvp =m_pSqlCmd->Field(25).asDouble();
		cpinfo.dPB_Avp =m_pSqlCmd->Field(26).asDouble();
		cpinfo.dPS_Avp =m_pSqlCmd->Field(27).asDouble();
		cpinfo.dPB_Margin =m_pSqlCmd->Field(28).asDouble();
		cpinfo.dPS_Margin =m_pSqlCmd->Field(29).asDouble();
		cpinfo.dDAY_PB_SURPLUS =m_pSqlCmd->Field(30).asDouble();
		cpinfo.dDAY_PS_SURPLUS =m_pSqlCmd->Field(31).asDouble();
		cpinfo.dLAST_SETTLE_PRICE = m_pSqlCmd->Field(32).asDouble();
		cpinfo.dSETTLE_PRICE = m_pSqlCmd->Field(33).asDouble();
		cpinfo.nVarietyId = m_pSqlCmd->Field(34).asLong();
		S_MDB_CUST_POSITION *pcpinfo = m_pMemDb->m_pDbTrader.m_pPosition.Select(cpinfo.sTxAcc,cpinfo.sSeatNo,cpinfo.sShFlag,cpinfo.nContractNo);
		if (pcpinfo == NULL)
		{
			//新增
			pc = m_pMemDb->m_pDbBaseinfo.m_pContract.Select(cpinfo.nContractNo);
			if (pc == NULL)
			{
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"日切  持仓表 CUST_POSI 的合约编号[%d %s]在合约表不存在",cpinfo.nContractNo,cpinfo.sContractCode);
				return false;
			}
			cpinfo.nVarietyId = pc->nProdId;
			rowid = m_pMemDb->m_pDbTrader.m_pPosition.Insert(cpinfo);
			if (rowid <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"日切  CUST_POSI [%s] repeat",cpinfo.sTxAcc);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"日切  持仓表 CUST_POSI [%s]重复",cpinfo.sTxAcc);
#endif
				return false;
			}
			pcpinfo = m_pMemDb->m_pDbTrader.m_pPosition.At(rowid);
		}
		else
		{
			//修改更新
			pcpinfo->nLastPB = cpinfo.nLastPB ;
			pcpinfo->nLastPS = cpinfo.nLastPS ;
			pcpinfo->nCurPB = cpinfo.nCurPB ;
			pcpinfo->nCurPS = cpinfo.nCurPS ;
			pcpinfo->nCurCanUsePB = cpinfo.nCurCanUsePB ;
			pcpinfo->nCurCanUsePS = cpinfo.nCurCanUsePS ;
			pcpinfo->nDayPB = cpinfo.nDayPB ;
			pcpinfo->nDayPS = cpinfo.nDayPS ;
			pcpinfo->nDAY_COV_PB_Froze = cpinfo.nDAY_COV_PB_Froze ;
			pcpinfo->nDAY_COV_PB = cpinfo.nDAY_COV_PB ;
			pcpinfo->nDAY_COV_PS_Froze = cpinfo.nDAY_COV_PS_Froze ;
			pcpinfo->nDAY_COV_PS = cpinfo.nDAY_COV_PS ;
			pcpinfo->nDAY_DELI_PB_Froze = cpinfo.nDAY_DELI_PB_Froze ;
			pcpinfo->nDAY_DELI_PB = cpinfo.nDAY_DELI_PB ;
			pcpinfo->nDAY_DELI_PS_Froze = cpinfo.nDAY_DELI_PS_Froze ;
			pcpinfo->nDAY_DELI_PS = cpinfo.nDAY_DELI_PS ;
			pcpinfo->nDAY_MIDD_PB = cpinfo.nDAY_MIDD_PB ;
			pcpinfo->nDAY_MIDD_PS = cpinfo.nDAY_MIDD_PS ;
			pcpinfo->dPB_OAvp = cpinfo.dPB_OAvp ;
			pcpinfo->dPS_OAvp = cpinfo.dPS_OAvp ;
			pcpinfo->dPB_Avp = cpinfo.dPB_Avp ;
			pcpinfo->dPS_Avp = cpinfo.dPS_Avp;
			pcpinfo->dPB_Margin = cpinfo.dPB_Margin ;
			pcpinfo->dPS_Margin = cpinfo.dPS_Margin ;
			pcpinfo->dDAY_PB_SURPLUS = cpinfo.dDAY_PB_SURPLUS ;
			pcpinfo->dDAY_PS_SURPLUS = cpinfo.dDAY_PS_SURPLUS ;
			pcpinfo->dLAST_SETTLE_PRICE = 	cpinfo.dLAST_SETTLE_PRICE ;
			pcpinfo->dSETTLE_PRICE = cpinfo.dSETTLE_PRICE ;
		}
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"日切 读取 资金账号[%s]的 持仓信息 合约[%d %s] 投保[%s] 多仓[%d] 空仓[%d]",\
			pcpinfo->sTxAcc,pcpinfo->nContractNo,pcpinfo->sContractCode,pcpinfo->sShFlag,pcpinfo->nCurPB,pcpinfo->nCurPS);
		bret = m_pSqlCmd->FetchNext();
	}
	//出入金、报单、成交已在内存，不用重新读取了

//	//读出入金
//	S_MDB_FUND_FLOW flowinfo;
//	sprintf(m_sSql,"select TX_DATE,TX_ACC,TX_SERIAL,BANK_ACC,BANK_NAME,CURRENCY,AMT,FUND_FLAG,LAST_AMT,CUR_AMT from FUND_FLOW where IS_REC='1' and TX_ACC=:1");
//
//	m_pSqlCmd->setCommandText(m_sSql);
//	m_pSqlCmd->Param(1).setAsString() = txacc;
//
//	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"日切 读取 资金账号[%s]的 出入金信息 %s",txacc,m_sSql);
//	if (!m_pSqlCmd->Execute())
//	{
//		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"日切  Execute <%s> error %d %s",m_sSql,m_pSqlCmd->m_dbconn.m_nErrCode,m_pSqlCmd->m_dbconn.m_sErrInfo.c_str());
//		return false;
//	}
//	bret = m_pSqlCmd->FetchNext();
//	while (bret)
//	{
//		strcpy(flowinfo.sTxDate ,m_pSqlCmd->Field(0).asString());
//		strcpy(flowinfo.sTxAcc,m_pSqlCmd->Field(1).asString());
//		strcpy(flowinfo.sTxSerial,m_pSqlCmd->Field(2).asString());
//		strcpy(flowinfo.sBankAcc,m_pSqlCmd->Field(3).asString());
//		strcpy(flowinfo.sBankName,m_pSqlCmd->Field(4).asString());
//		strcpy(flowinfo.sCurrency,m_pSqlCmd->Field(5).asString());
//		flowinfo.dAmt = m_pSqlCmd->Field(6).asDouble();
//		strcpy(flowinfo.sFundFlag,m_pSqlCmd->Field(7).asString());
//		flowinfo.dLastAmt =m_pSqlCmd->Field(8).asDouble();
//		flowinfo.dCurAmt =m_pSqlCmd->Field(9).asDouble();
//
//		if (m_pMemDb->m_pDbTrader.m_pFundFlow.Insert(flowinfo) <0 )
//		{
//#ifdef _ENGLISH_
//			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"日切  FUND_FLOW [%s] repeat",flowinfo.sTxAcc);
//#else
//			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"日切  出入金表 FUND_FLOW [%s]重复",flowinfo.sTxAcc);
//#endif
//			return false;
//		}
//		bret = m_pSqlCmd->FetchNext();
//	}
//	//读报单
//	S_MDB_ORDER_INFO orderinfo;
//	sprintf(m_sSql,"select TX_ACC,EX_CODE,SEAT_NO,TX_CODE,CONTRACT_NO,CONTRACT_CODE,CLIENT_SERIAL,LOCAL_NO,SYSTEM_NO,ORDER_NUM,ORDER_PRICE,REMAIN_NUM,EO_FLAG,BS_FLAG,SH_FLAG,TXDATE,AMT,FROZ_AMT,EX_FARE,TX_TYPE,CMD_TYPE,CMD_ATTR,FORCE_FLAG,ORDER_STATUS,CANCEL_FLAG,ANS_TIME,ORDER_CHANNEL,ACC_ORG,TX_ORG,ORDER_OPER,CANCEL_CHANNEL,CANCEL_OPER,CANCEL_TIME,ORDER_RSP,FROZ_USEAMT,FROZ_OUTAMT,CANCEL_NUM,VARIETY from ORDER_INFO where TX_ACC=:1 ");
//	m_pSqlCmd->setCommandText(m_sSql);
//	m_pSqlCmd->Param(1).setAsString() = txacc;
//	if (!m_pSqlCmd->Execute())
//	{
//		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"日切  Execute <%s> error %d %s",m_sSql,m_pSqlCmd->m_dbconn.m_nErrCode,m_pSqlCmd->m_dbconn.m_sErrInfo.c_str());
//		return false;
//	}
//	
//	bret = m_pSqlCmd->FetchNext();
//	while (bret)
//	{
//		strcpy(orderinfo.sTxAcc ,m_pSqlCmd->Field(0).asString());
//		strcpy(orderinfo.sExchCode,m_pSqlCmd->Field(1).asString());
//		strcpy(orderinfo.sSeatNo,m_pSqlCmd->Field(2).asString());
//		strcpy(orderinfo.sTxCode,m_pSqlCmd->Field(3).asString());
//		orderinfo.nContractNo = m_pSqlCmd->Field(4).asLong();
//		strcpy(orderinfo.sContractCode,m_pSqlCmd->Field(5).asString());
//		strcpy(orderinfo.sClientSerial,m_pSqlCmd->Field(6).asString());
//		strcpy(orderinfo.sLocalSerial,m_pSqlCmd->Field(7).asString());
//		strcpy(orderinfo.sSystemNo,m_pSqlCmd->Field(8).asString());
//		orderinfo.nOrderNum =m_pSqlCmd->Field(9).asLong();
//		orderinfo.dOrderPrice =m_pSqlCmd->Field(10).asDouble();
//		orderinfo.nRemainNum =m_pSqlCmd->Field(11).asLong();
//		strcpy(orderinfo.sEoFlag ,m_pSqlCmd->Field(12).asString());
//		strcpy(orderinfo.sBsFlag ,m_pSqlCmd->Field(13).asString());
//		strcpy(orderinfo.sShFlag ,m_pSqlCmd->Field(14).asString());
//		strcpy(orderinfo.sTxDate ,m_pSqlCmd->Field(15).asString());
//		orderinfo.dAmt =m_pSqlCmd->Field(16).asDouble();
//		orderinfo.dFrozAmt =m_pSqlCmd->Field(17).asDouble();
//		orderinfo.dExFee =m_pSqlCmd->Field(18).asDouble();
//		strcpy(orderinfo.sTxType ,m_pSqlCmd->Field(19).asString());
//		strcpy(orderinfo.sCmdType ,m_pSqlCmd->Field(20).asString());
//		strcpy(orderinfo.sCmdAttr,m_pSqlCmd->Field(21).asString());
//		strcpy(orderinfo.sForceFlag ,m_pSqlCmd->Field(22).asString());
//		strcpy(orderinfo.sOrderStatus ,m_pSqlCmd->Field(23).asString());
//		strcpy(orderinfo.sCancelFlag ,m_pSqlCmd->Field(24).asString());
//		strcpy(orderinfo.sAnsTime ,m_pSqlCmd->Field(25).asString());
//		strcpy(orderinfo.sOrderChannel ,m_pSqlCmd->Field(26).asString());
//		strcpy(orderinfo.sAccOrg ,m_pSqlCmd->Field(27).asString());
//		strcpy(orderinfo.sTxOrg ,m_pSqlCmd->Field(28).asString());
//		strcpy(orderinfo.sOrderOper ,m_pSqlCmd->Field(29).asString());
//		strcpy(orderinfo.sCancelChannel ,m_pSqlCmd->Field(30).asString());
//		strcpy(orderinfo.sCancelOper ,m_pSqlCmd->Field(31).asString());
//		strcpy(orderinfo.sCancelTime ,m_pSqlCmd->Field(32).asString());
//		strcpy(orderinfo.sOrderRsp ,m_pSqlCmd->Field(33).asString());
//		orderinfo.dFrozUseamt = m_pSqlCmd->Field(34).asDouble();
//		orderinfo.dFrozOutamt = m_pSqlCmd->Field(35).asDouble();
//		orderinfo.nCancelNum = m_pSqlCmd->Field(36).asLong();
//		orderinfo.nVarietyId = m_pSqlCmd->Field(37).asLong();
//		if (m_pMemDb->m_pDbTrader.m_pOrder.Insert(orderinfo) <0 )
//		{
//#ifdef _ENGLISH_
//			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"日切  ORDER_INFO [%s] repeat",orderinfo.sTxAcc);
//#else
//			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"日切  报单表 ORDER_INFO [%s]重复",orderinfo.sTxAcc);
//#endif
//			return false;
//		}
//		bret = m_pSqlCmd->FetchNext();
//	}
//	
//	//读成交
//	S_MDB_DONE_INFO doneinfo;
//	sprintf(m_sSql,"select DONE_NO,SYSTEM_NO,EX_CODE,TX_CODE,LOCAL_NO,TX_ACC,DONE_DATE,DONE_TIME,CONTRACT_NO,CONTRACT_CODE,DONE_PRICE,DONE_QTY,BS_FLAG,SH_FLAG,EO_FLAG,TX_TYPE,ORDER_TYPE,DONE_ATTR,AMT,EX_FEE,MEMBER_FEE,EX_MARGIN,MEMBER_MARGIN,FLOAT_SURPLUS,COV_SURPLUS,TX_ORG,ACC_ORG,FORCE_FLAG,ORDER_CHANNEL,ORDER_OPER,SYSTEM_DATE,SEAT_NO,VARIETY from DONE_INFO where TX_ACC=:1");
//	m_pSqlCmd->setCommandText(m_sSql);
//	m_pSqlCmd->Param(1).setAsString() = txacc;
//	if (!m_pSqlCmd->Execute())
//	{
//		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"日切  Execute <%s> error %d %s",m_sSql,m_pSqlCmd->m_dbconn.m_nErrCode,m_pSqlCmd->m_dbconn.m_sErrInfo.c_str());
//		return false;
//	}
//	
//	bret = m_pSqlCmd->FetchNext();
//	while (bret)
//	{
//		strcpy(doneinfo.sDoneNo ,m_pSqlCmd->Field(0).asString());
//		strcpy(doneinfo.sSystemNo,m_pSqlCmd->Field(1).asString());
//		strcpy(doneinfo.sExchCode,m_pSqlCmd->Field(2).asString());
//		strcpy(doneinfo.sTxCode,m_pSqlCmd->Field(3).asString());
//		strcpy(doneinfo.sLocalNo,m_pSqlCmd->Field(4).asString());
//		strcpy(doneinfo.sTxAcc,m_pSqlCmd->Field(5).asString());
//		strcpy(doneinfo.sDoneDate,m_pSqlCmd->Field(6).asString());
//		strcpy(doneinfo.sDoneTime,m_pSqlCmd->Field(7).asString());
//		doneinfo.nContractNo =m_pSqlCmd->Field(8).asLong();
//		strcpy(doneinfo.sContractCode,m_pSqlCmd->Field(9).asString());
//		doneinfo.dDonePrice =m_pSqlCmd->Field(10).asDouble();
//		doneinfo.nDoneNum = m_pSqlCmd->Field(11).asLong();
//		strcpy(doneinfo.sEoFlag ,m_pSqlCmd->Field(12).asString());
//		strcpy(doneinfo.sBsFlag ,m_pSqlCmd->Field(13).asString());
//		strcpy(doneinfo.sShFlag ,m_pSqlCmd->Field(14).asString());
//		strcpy(doneinfo.sTxType ,m_pSqlCmd->Field(15).asString());
//		strcpy(doneinfo.sCmdType ,m_pSqlCmd->Field(16).asString());
//		strcpy(doneinfo.sCmdAttr ,m_pSqlCmd->Field(17).asString());
//		doneinfo.dAmt =m_pSqlCmd->Field(18).asDouble();
//		doneinfo.dExFee =m_pSqlCmd->Field(19).asDouble();
//		doneinfo.dMemberFee =m_pSqlCmd->Field(20).asDouble();
//		doneinfo.dExMargin =m_pSqlCmd->Field(21).asDouble();
//		doneinfo.dMemberMargin =m_pSqlCmd->Field(22).asDouble();
//		doneinfo.dFloatSurplus =m_pSqlCmd->Field(23).asDouble();
//		doneinfo.dCovSurplus =m_pSqlCmd->Field(24).asDouble();
//		strcpy(doneinfo.sTxOrg ,m_pSqlCmd->Field(25).asString());
//		strcpy(doneinfo.sAccOrg ,m_pSqlCmd->Field(26).asString());
//		strcpy(doneinfo.sForceFlag ,m_pSqlCmd->Field(27).asString());
//		strcpy(doneinfo.sOrderChannel ,m_pSqlCmd->Field(28).asString());
//		strcpy(doneinfo.sOrderOper ,m_pSqlCmd->Field(29).asString());
//		strcpy(doneinfo.sSystemDate ,m_pSqlCmd->Field(30).asString());
//		strcpy(doneinfo.sSeatNo ,m_pSqlCmd->Field(31).asString());
//		doneinfo.nVarietyId =m_pSqlCmd->Field(32).asLong();
//		if (m_pMemDb->m_pDbTrader.m_pDone.Insert(doneinfo) <0 )
//		{
//#ifdef _ENGLISH_
//			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"日切  DONE_INFO [%s] repeat",doneinfo.sTxAcc);
//#else
//			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"日切 成交表 DONE_INFO [%s]重复",doneinfo.sTxAcc);
//#endif
//			return false;
//		}
//		bret = m_pSqlCmd->FetchNext();
//	}
//
	//重新根据出入金、报单、成交计算资金持仓等
	//加锁
	CBF_PMutex pp(pacclock->mutex);
	//首先出入金
	std::vector<S_MDB_FUND_FLOW *> flowlist;
	if (m_pMemDb->m_pDbTrader.m_pFundFlow.SelectByAcc(pacclock->sTxAcc,flowlist))
	{
		//处理
		if (!m_pKernelPub.ProcFundFlow(pacclock,flowlist))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"日切  处理[%s]出入金流水失败",pacclock->sTxAcc);
			return false;
		}
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"日切 处理 资金账号[%s] 出入金[%d]条",pacclock->sTxAcc,flowlist.size());
	}
	//持仓
	//判断是否大边，进行大边处理 导入的持仓的保证金，已经在资金表里，但资金明细表没有。这里要经过持仓计算，分别得到买保证金、卖保证金，记录资金明细表。
	std::vector<S_MDB_CUST_POSITION *> posilis;
	if (m_pMemDb->m_pDbTrader.m_pPosition.SelectByAccOrderByVariety(pacclock->sTxAcc,posilis))
	{
		if (!m_pKernelPub.ProcPosition(pacclock,posilis))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"日切  处理[%s]持仓失败",pacclock->sTxAcc);
			return false;
		}
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"日切 处理 资金账号[%s] 持仓[%d]条",pacclock->sTxAcc,posilis.size());
	}
	
	
	//成交
	std::vector<S_MDB_DONE_INFO *> donelist;
	if (m_pMemDb->m_pDbTrader.m_pDone.SelectByAcc(pacclock->sTxAcc,donelist))
	{
		//处理
		if (!m_pKernelPub.ProcDone(pacclock,donelist))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"日切  处理[%s]成交失败",pacclock->sTxAcc);
			return false;
		}
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"日切 处理 资金账号[%s] 成交[%d]条",pacclock->sTxAcc,donelist.size());
	}
	//报单
	std::vector<S_MDB_ORDER_INFO *> orderlist;
	if (m_pMemDb->m_pDbTrader.m_pOrder.SelectByAcc(pacclock->sTxAcc,orderlist))
	{
		//处理
		if (!m_pKernelPub.ProcOrder(pacclock,orderlist))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"日切 处理[%s]报单失败",pacclock->sTxAcc);
			return false;
		}
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"日切 处理 资金账号[%s] 报单[%d]条",pacclock->sTxAcc,orderlist.size());
	}
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"日切  读取 资金账号[%s]的信息 完成",txacc);
	return true;
}


bool RiskDll::UpdateFeeTemplet()
{
	S_MDB_FEE_TEMPLET_DETAIL info;
	sprintf(m_sSql,"select a.FEE_MODEL_CODE,a.PROD_TYPE,a.VARIETY_NO,a.VARIETY_CODE,a.CONTRACT_NO,a.CONTRACT_CODE,a.FEE_NO,a.FEE_MODE,a.FEE_VALUE from FEE_MODEL_DETAIL a ");
	m_pSqlCmd->setCommandText(m_sSql);
	if (!m_pSqlCmd->Execute())
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Execute <%s> error %d %s",m_sSql,m_pSqlCmd->m_dbconn.m_nErrCode,m_pSqlCmd->m_dbconn.m_sErrInfo.c_str());
		return false;
	}
	bool bret = m_pSqlCmd->FetchNext();
	while (bret)
	{
		strcpy(info.sFareModeCode ,m_pSqlCmd->Field(0).asString());
		strcpy(info.sProdType,m_pSqlCmd->Field(1).asString());
		info.nVarietyNo  = m_pSqlCmd->Field(2).asLong();
		strcpy(info.sVarietyCode,m_pSqlCmd->Field(3).asString());
		info.nContractNo  = m_pSqlCmd->Field(4).asLong();
		strcpy(info.sContractCode,m_pSqlCmd->Field(5).asString());
		strcpy(info.sFareNo,m_pSqlCmd->Field(6).asString());
		strcpy(info.sFareMode,m_pSqlCmd->Field(7).asString());
		info.dValue = m_pSqlCmd->Field(8).asDouble();
		
		if (m_pMemDb->m_pDbSet.m_pFeeTempletDetail.Update(info) <0 )
		{
#ifdef _ENGLISH_
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"FEE_MODEL_DETAIL [%s] Update error",info.sFareModeCode);
#else
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"费率模板设置表 FEE_MODEL_DETAIL [%s] 更新失败",info.sFareModeCode);
#endif
			return false;
		}
		bret = m_pSqlCmd->FetchNext();
	}
	
	return true;
}



bool RiskDll::CalcRiskInfo(char *txacc, S_MDB_CONTRACT_INFO *coninfo, double newprice, S_MDB_CUST_FUND &fund)
{
	S_MDB_TXACC_LOCK *plock = m_pMemDb->SelectLock(txacc);
	if (plock == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"txacc [%s] not found",txacc);
		return false;
	}
	S_MDB_CUST_FUNDACC_INFO *pfundacc = m_pMemDb->m_pDbCust.m_pCustFundAcc.At(plock->nFundAccRowid);
	if (pfundacc == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"资金账号[%s] not found",txacc);
		return false;
	}
	S_MDB_CUST_FUND *pfund = m_pMemDb->GetFund(plock,coninfo->sCurrency);
	if (pfund == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"资金账号[%s %s] not found",txacc,coninfo->sCurrency);
		return false;
	}
	

	std::vector<S_MDB_CUST_POSITION *> posilist;
	if (!m_pMemDb->m_pDbTrader.m_pPosition.SelectByAccContract(txacc,coninfo->nContractId,posilist))
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"Position  [%s %d] not found",txacc,coninfo->nContractId);
		return false;
	}	
	int i;
	double dfloatsurplus=0.0;
	double dMarginDiff=0.0;
	double dexmargin=0.0;
	double dmembermargin=0.0;
	for (i =0 ; i<posilist.size(); i++)
	{
		if (coninfo->sProd_type[0] != PROD_TYPE_FUTURES && coninfo->sProd_type[0] != PROD_TYPE_OPTION)
		{
			dfloatsurplus = dfloatsurplus + (newprice-posilist[i]->dPB_Avp)*coninfo->dTxUnit*posilist[i]->nCurPB;
			continue;
		}
		//计算持仓产生的盈亏
		
	
		dfloatsurplus = dfloatsurplus + (newprice-posilist[i]->dPB_Avp)*coninfo->dTxUnit*posilist[i]->nCurPB;
		dfloatsurplus = dfloatsurplus + (posilist[i]->dPS_Avp-newprice)*coninfo->dTxUnit*posilist[i]->nCurPS;
		//计算持仓产生的保证金
		if (coninfo->sProd_type[0] == PROD_TYPE_FUTURES)
		{
			if (!m_pKernelPub.GetMarginValue(coninfo,pfundacc,0,posilist[i]->nCurPB,newprice*posilist[i]->nCurPB*coninfo->dTxUnit,dexmargin,dmembermargin))
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"Position  [%s %d] 取保证金出错",txacc,coninfo->nContractId);
				continue;
			}
			dMarginDiff = dMarginDiff+ dexmargin+dmembermargin - posilist[i]->dPB_Margin;
			if (!m_pKernelPub.GetMarginValue(coninfo,pfundacc,0,posilist[i]->nCurPS,newprice*posilist[i]->nCurPS*coninfo->dTxUnit,dexmargin,dmembermargin))
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"Position  [%s %d] 取保证金出错",txacc,coninfo->nContractId);
				continue;
			}
			dMarginDiff = dMarginDiff+ dexmargin+dmembermargin - posilist[i]->dPS_Margin;
		}
		else if (coninfo->sProd_type[0] == PROD_TYPE_OPTION && posilist[i]->nCurPS>0)
		{
			if (!m_pKernelPub.GetMarginValue(coninfo,pfundacc,1,posilist[i]->nCurPS,newprice*posilist[i]->nCurPS*coninfo->dTxUnit,dexmargin,dmembermargin))
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"Position  [%s %d] 取保证金出错",txacc,coninfo->nContractId);
				continue;
			}
			dMarginDiff = dMarginDiff+ dexmargin+dmembermargin - posilist[i]->dPS_Margin;
		}
		
	}
	//资金
	memcpy(&fund,pfund,sizeof(S_MDB_CUST_FUND));
	fund.dMargin = fund.dMargin + dMarginDiff;

	//得到最新的资金
	fund.dBalance = fund.dBalance + dfloatsurplus;
	fund.dFloatSurplus = fund.dFloatSurplus+dfloatsurplus;
	//得到风险
	if (fund.dBalance > MINDOUBLECOMPARE && fund.dBalance < MAXDOUBLECOMPARE )//结存为0
	{
		if (fund.dMargin > MAXDOUBLECOMPARE)//说明有保证金
		{
			fund.dRisk = 2.00;
		}
		else
		{
			fund.dRisk = 0.00;
		}
	}
	else if  (fund.dBalance < MINDOUBLECOMPARE) //结存为负数
	{
		if (fund.dMargin > MAXDOUBLECOMPARE)//说明有保证金
		{
			fund.dRisk = 2.00;
		}
		else //无保证金但结存为负数，风险为1
		{
			fund.dRisk = 1.00;
		}
	}
	else //  保证金/结存
	{
		fund.dRisk = 1.00*fund.dMargin/fund.dBalance;
	}
	//取风险级别
	char traderight[2];
	int level = m_pKernelPub.GetRiskLevel(&fund,traderight);
	if (level<0)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"资金账号[%s] 取风险级别出错",txacc);
		return false;
	}
	fund.nRisk_level = level;
	fund.dAvailFund = fund.dAvailFund-dMarginDiff+dfloatsurplus;
	return true;
}

bool RiskDll::SyncOrder(S_MDB_ORDER_INFO *order)
{
	if (strlen(order->sCancelFlag)<1)
	{
		order->sCancelFlag[0] = BOOL_FALSE;
		order->sCancelFlag[1] = 0;
	}
	if (order->nSyncFlag == 1)
	{
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"开始写入报单表");
		if (!UpdateOrder2Db(order))
		{
			return false;
		}
		if (order->nSyncFlag == 1)
		{
			order->nSyncFlag = 0;
		}
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"写入报单表完成");
		return true;
	}
	else if  (order->nSyncFlag >0 )//需要更新
	{
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"开始更新报单表");
		int flag = order->nSyncFlag;
		sprintf(m_sSql,"select  REMAIN_NUM,CANCEL_FLAG,CANCEL_NUM from ORDER_INFO where LOCAL_NO=:1");
		m_pSqlCmd->setCommandText(m_sSql);
		m_pSqlCmd->Param(1).setAsString() = order->sLocalSerial;
		if (!m_pSqlCmd->Execute())
		{
			m_pSqlCmd->Rollback();
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"Execute %s error %s",m_sSql,m_pSqlCmd->GetCmdErrInfo());
			return true;
		}
		if (m_pSqlCmd->FetchNext())
		{
			if (order->nRemainNum > m_pSqlCmd->Field(0).asLong()) //表里的剩余数量小于，说明已经更新过了
			{
				if (order->nSyncFlag == flag)
				{
					order->nSyncFlag = 0;
				}
				return true;
			}
			if (m_pSqlCmd->Field(1).asString()[0] == BOOL_TRUE) //表里已是撤单，说明撤过了
			{
				if (order->nSyncFlag == flag)
				{
					order->nSyncFlag = 0;
				}
				return true;
			}
		}
		else
		{
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"无此报单[%s] 开始写入报单表",order->sLocalSerial);
			if (!UpdateOrder2Db(order))
			{
				return false;
			}
			if (order->nSyncFlag == flag)
			{
				order->nSyncFlag = 0;
			}
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"写入报单表完成");
			return true;
		}

		sprintf(m_sSql,"update ORDER_INFO set REMAIN_NUM=:1,CANCEL_FLAG=:2,CANCEL_NUM=:3,DONE_AVP=:4,ORDER_STATUS=:6,ORDER_RSP=:7 where LOCAL_NO=:5");
		m_pSqlCmd->setCommandText(m_sSql);
		m_pSqlCmd->Param(1).setAsLong() = order->nRemainNum;
		
		m_pSqlCmd->Param(2).setAsString() = order->sCancelFlag;
		m_pSqlCmd->Param(3).setAsLong() = order->nCancelNum;
		m_pSqlCmd->Param(4).setAsDouble() = order->dDoneAvp;
		m_pSqlCmd->Param(5).setAsString() = order->sLocalSerial;
		m_pSqlCmd->Param(6).setAsString() = order->sOrderStatus;
		m_pSqlCmd->Param(7).setAsString() = order->sOrderRsp;
		if (!m_pSqlCmd->Execute())
		{
			m_pSqlCmd->Rollback();
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"Execute %s error %s",m_sSql,m_pSqlCmd->GetCmdErrInfo());
			return -1;
		}
		if (order->nSyncFlag == flag)
		{
			order->nSyncFlag = 0;
		}
		m_pSqlCmd->Commit();
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"更新报单表完成");
		return true;
	}
	return true;
}

bool RiskDll::SyncDone(S_MDB_DONE_INFO *done)
{
	if (done->nSyncFlag == 3)
	{
		sprintf(m_sSql,"select DONE_NO from  DONE_INFO where DONE_NO=:1 and EX_CODE=:2 and BS_FLAG=:3 ");
		m_pSqlCmd->setCommandText(m_sSql);
		m_pSqlCmd->Param(1).setAsString() = done->sDoneNo;
		m_pSqlCmd->Param(2).setAsString() = done->sExchCode;
		m_pSqlCmd->Param(3).setAsString() = done->sBsFlag;
		if (!m_pSqlCmd->Execute())
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"Execute %s error %s",m_sSql,m_pSqlCmd->GetCmdErrInfo());
			return false;
		}
		if (m_pSqlCmd->FetchNext())//已有此成交
		{
			done->nSyncFlag = 0;
			return true;
		}

		sprintf(m_sSql,"insert into DONE_INFO(DONE_NO,SYSTEM_NO,EX_CODE,SEAT_NO,TX_CODE,LOCAL_NO,TX_ACC,DONE_DATE,DONE_TIME,CONTRACT_NO,CONTRACT_CODE,DONE_PRICE,DONE_QTY,BS_FLAG,SH_FLAG,EO_FLAG,TX_TYPE,ORDER_TYPE,DONE_ATTR,AMT,EX_FEE,MEMBER_FEE,EX_MARGIN,MEMBER_MARGIN,FLOAT_SURPLUS,COV_SURPLUS,TX_ORG,ACC_ORG,FORCE_FLAG,ORDER_CHANNEL,ORDER_OPER,SYSTEM_DATE,VARIETY) values(:1,:2,:3,:4,:5,:6,:7,:8,:9,:10,:11,:12,:13,:14,:15,:16,:17,:18,:19,:20,:21,:22,:23,:24,:25,:26,:27,:28,:29,:30,:31,:32,:33) ");
		m_pSqlCmd->setCommandText(m_sSql);
		m_pSqlCmd->Param(1).setAsString() = done->sDoneNo;
		m_pSqlCmd->Param(2).setAsString() = done->sSystemNo;
		m_pSqlCmd->Param(3).setAsString() = done->sExchCode;
		m_pSqlCmd->Param(4).setAsString() = done->sSeatNo;
		m_pSqlCmd->Param(5).setAsString() = done->sTxCode;
		m_pSqlCmd->Param(6).setAsString() = done->sLocalNo;
		m_pSqlCmd->Param(7).setAsString() = done->sTxAcc;
		m_pSqlCmd->Param(8).setAsString() = done->sDoneDate;
		m_pSqlCmd->Param(9).setAsString() = done->sDoneTime;
		m_pSqlCmd->Param(10).setAsLong() = done->nContractNo;
		m_pSqlCmd->Param(11).setAsString() = done->sContractCode;
		m_pSqlCmd->Param(12).setAsDouble() = done->dDonePrice;
		m_pSqlCmd->Param(13).setAsLong() = done->nDoneNum;
		m_pSqlCmd->Param(14).setAsString() = done->sBsFlag;
		m_pSqlCmd->Param(15).setAsString() = done->sShFlag;
		m_pSqlCmd->Param(16).setAsString() = done->sEoFlag;
		m_pSqlCmd->Param(17).setAsString() = done->sTxType;
		m_pSqlCmd->Param(18).setAsString() = done->sCmdType;
		m_pSqlCmd->Param(19).setAsString() = done->sCmdAttr;
		m_pSqlCmd->Param(20).setAsDouble() = done->dAmt;
		m_pSqlCmd->Param(21).setAsDouble() = done->dExFee;
		m_pSqlCmd->Param(22).setAsDouble() = done->dMemberFee;
		m_pSqlCmd->Param(23).setAsDouble() = done->dExMargin;
		m_pSqlCmd->Param(24).setAsDouble() = done->dMemberMargin;
		m_pSqlCmd->Param(25).setAsDouble() = done->dFloatSurplus;
		m_pSqlCmd->Param(26).setAsDouble() = done->dCovSurplus;
		m_pSqlCmd->Param(27).setAsString() = done->sTxOrg;
		m_pSqlCmd->Param(28).setAsString() = done->sAccOrg;
		m_pSqlCmd->Param(29).setAsString() = done->sForceFlag;
		m_pSqlCmd->Param(30).setAsString() = done->sOrderChannel;
		m_pSqlCmd->Param(31).setAsString() = done->sOrderOper;
		m_pSqlCmd->Param(32).setAsString() = done->sSystemDate;
		m_pSqlCmd->Param(33).setAsLong() = done->nVarietyId;
		if (!m_pSqlCmd->Execute())
		{
			m_pSqlCmd->Rollback();
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"Execute %s error %s",m_sSql,m_pSqlCmd->GetCmdErrInfo());
			return false;
		}
		m_pSqlCmd->Commit();
		done->nSyncFlag = 0;
		return true;
	}
	return true;
}
bool RiskDll::ComputeProd(const char *compdate)
{
	int ret;
	int i,j,k;
	double dBal;//结存 权益
	double dMargin;//保证金
	double dPosiBuyTotal;//多仓价值
	double dPosiSellTotal;//空仓价值

	double dBalTmp;
	double dMarginTmp;
	double dPosiBuy;
	double dPosiSell;
	int ff;

	S_MDB_PROD_FUND_RISK risk;
	S_MDB_PROD_POSI_RISK posirisk;
	char traderight[2];
	bzero(traderight,sizeof(traderight));
	bzero(&risk,sizeof(S_MDB_PROD_FUND_RISK));
	bzero(&posirisk,sizeof(S_MDB_PROD_POSI_RISK));

	strcpy(risk.sDate,compdate);
	strcpy(posirisk.sDate,compdate);
	std::vector<S_MDB_PROD_CUST *> prodcustlist;
	std::vector<S_MDB_CUST_POSITION *> posilist;
	std::vector<S_MDB_CUST_FUND *>fundlist;
	S_MDB_CONTRACT_INFO *pcinfo=NULL;
	S_MDB_PROD_FUND_RISK *pfundrisk=NULL;
	S_MDB_PROD_POSI_RISK *pposirisk=NULL;
	S_MDB_CURRENCY_RATE  *pCurrencyRate=NULL;
	//计算产品资金
	S_MDB_FINA_PROD_INFO *pProd = m_pMemDb->m_pDbBaseinfo.m_pProd.First();
	
	m_pDate.Update();
	while (pProd != NULL)
	{
		//某一个产品
		strcpy(risk.sProdCode,pProd->sFinaProdCode);
		strcpy(posirisk.sProdCode,risk.sProdCode);
		posirisk.nRiskdate = atoi(m_pDate.ToStringD8().c_str());
		posirisk.nRisktime = atoi(m_pDate.ToStringT6().c_str());
		risk.nRiskdate = atoi(m_pDate.ToStringD8().c_str());
		risk.nRisktime = atoi(m_pDate.ToStringT6().c_str());
		prodcustlist.clear();
		if (!m_pMemDb->m_pDbBaseinfo.m_pProdCust.SelectByProd(risk.sProdCode,prodcustlist))
		{
			pProd = m_pMemDb->m_pDbBaseinfo.m_pProd.Next();
			continue;
		}
		
		dBal =0.0;
		dMargin=0.0;
		dPosiBuyTotal=0.0;
		dPosiSellTotal=0.0;
		
		for (j=0; j<prodcustlist.size(); j++)
		{
			//某一个资金账号
			//根据资金账号去取资金，结存及保证金
			fundlist.clear();
			if (!m_pMemDb->m_pDbTrader.m_pFund.Select(prodcustlist[j]->sTxAcc,fundlist))
			{
				continue;;
			}
			for (ff=0; ff<fundlist.size(); ff++) //计算产品总的保证金和权益
			{
				if (fundlist[ff]->sCurrency[0] != CURRENCY_CNY) 
				{
					pCurrencyRate = m_pMemDb->m_pDbSet.m_pCurrencyRate.Select(fundlist[ff]->sCurrency);
					if (pCurrencyRate == NULL)
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"币种 [%s] 未配置兑换比率",fundlist[ff]->sCurrency);
						//不是rmb，进行转换
						dBalTmp = fundlist[ff]->dBalance;
						dMarginTmp = fundlist[ff]->dMargin;
						dBal+=dBalTmp;
						dMargin+=dMarginTmp;
					}
					else
					{
						dBalTmp = fundlist[ff]->dBalance*pCurrencyRate->dRate;
						dMarginTmp = fundlist[ff]->dMargin*pCurrencyRate->dRate;
						dBal+=dBalTmp;
						dMargin+=dMarginTmp;
					}
					
				}
				else
				{
					dBalTmp = fundlist[ff]->dBalance;
					dMarginTmp = fundlist[ff]->dMargin;
					dBal+=dBalTmp;
					dMargin+=dMarginTmp;
				}
			}
			//某一个资金账号的持仓
			//取持仓，计算持仓总价值  合约轧差占比
			posilist.clear();
			if (m_pMemDb->m_pDbTrader.m_pPosition.SelectByAcc(prodcustlist[j]->sTxAcc,posilist))
			{
				for (k=0 ; k<posilist.size(); k++) 
				{
					pcinfo = m_pMemDb->m_pDbBaseinfo.m_pContract.Select(posilist[k]->nContractNo);
					if (pcinfo == NULL)
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"合约编号 [%d] 在合约表不存在",posilist[k]->nContractNo);
						continue;
					}
					if (pcinfo->sProd_type[0] == PROD_TYPE_OPTION) //期权
					{
						//取标的合约的结算价
						//取delta及标的合约的结算价
						S_MDB_CONTRACT_SETTLE_PRICE *pcsp = m_pMemDb->m_pDbBaseinfo.m_pSettlePrice.Select(pcinfo->nProdId);
						if (pcsp == NULL)
						{
							dPosiBuy = posilist[k]->dLAST_SETTLE_PRICE* posilist[k]->nCurPB * pcinfo->dTxUnit;
							dPosiSell = posilist[k]->dLAST_SETTLE_PRICE* posilist[k]->nCurPS * pcinfo->dTxUnit;
							dPosiSellTotal += dPosiSell;
							dPosiBuyTotal +=dPosiBuy;
							continue;
						}
						S_MDB_OPTION_CONTRACT_PRICE_MODEL *pomodel = m_pMemDb->m_pDbBaseinfo.m_pOptionModel.Select(posilist[k]->nContractNo);
						if (pomodel == NULL)
						{
							dPosiBuy = posilist[k]->dLAST_SETTLE_PRICE* posilist[k]->nCurPB * pcinfo->dTxUnit;
							dPosiSell = posilist[k]->dLAST_SETTLE_PRICE* posilist[k]->nCurPS * pcinfo->dTxUnit;
							dPosiSellTotal += dPosiSell;
							dPosiBuyTotal +=dPosiBuy;
							continue;
						}
						
						dPosiBuy = pomodel->dDelta*pcsp->dSettlePrice* posilist[k]->nCurPB * pcinfo->dTxUnit;
						dPosiSell = pomodel->dDelta*pcsp->dSettlePrice* posilist[k]->nCurPS * pcinfo->dTxUnit;
						
					}
					else //期货
					{
						//持仓价值
						dPosiBuy = posilist[k]->dLAST_SETTLE_PRICE* posilist[k]->nCurPB * pcinfo->dTxUnit;
						dPosiSell = posilist[k]->dLAST_SETTLE_PRICE* posilist[k]->nCurPS * pcinfo->dTxUnit;
						
					}
					dPosiSellTotal += dPosiSell;
					dPosiBuyTotal +=dPosiBuy;
				}
			}

		} // end 一个资金账户
		//保证金占比 单位净值 持仓价值比
		if (dBal> MINDOUBLECOMPARE && dBal<MAXDOUBLECOMPARE)
		{
			posirisk.dMarginRate = 0.0;
			posirisk.dPosiValueRate = 0.0;
			posirisk.dPosiDiffValueRate=0.0;
		}
		else 
		{
			posirisk.dMarginRate = dMargin/dBal;//保证金占比
			posirisk.dPosiValueRate = (dPosiBuyTotal+dPosiSellTotal)/dBal;//持仓价值比
			posirisk.dPosiDiffValueRate = (dPosiBuyTotal-dPosiSellTotal)/dBal;//持仓轧差价值比
		}
		posirisk.dT_Bal = dBal;
		posirisk.dT_Buy = dPosiBuyTotal;
		posirisk.dT_Sell = dPosiSellTotal;
		//单位净值
		if (pProd->nPublishUit == 0)
		{
			risk.dUnitValue = 0;
		}
		else 
		{
			risk.dUnitValue = dBal/pProd->nPublishUit;
		}
		risk.dBal = dBal;
		risk.dMargin = dMargin;
		if (dBal> MINDOUBLECOMPARE && dBal<MAXDOUBLECOMPARE)
		{
			risk.dRisk =0.0;
		}
		else
		{
			risk.dRisk = dMargin/dBal;//保证金/权益
		}
		risk.nRiskLevel = m_pKernelPub.GetProdRiskLevel(risk.sProdCode,risk.dUnitValue,risk.dRisk,traderight,risk.sRiskDesc);
		strcpy(risk.sRight,traderight);

		posirisk.nRiskLevel = m_pKernelPub.GetProdRiskLevel(risk.sProdCode,posirisk.dMarginRate,posirisk.dPosiValueRate,posirisk.dPosiDiffValueRate,posirisk.sRight,posirisk.sRiskDesc);
		
		bool issendfund=false;
		bool issendposi=false;
		//更新到表里
		pfundrisk = m_pMemDb->m_pDbTrader.m_pProdFundRisk.Select(risk.sProdCode);
		if (pfundrisk == NULL)
		{
			if (risk.nRiskLevel <10)
			{
				risk.sRiskFlag[0] = BOOL_TRUE;
				issendfund = true;
			}
			m_pMemDb->m_pDbTrader.m_pProdFundRisk.Insert(risk);
			
		}
		else
		{
			//若原来的
			if (risk.nRiskLevel != pfundrisk->nRiskLevel)
			{
				if (risk.nRiskLevel == 10)
				{
					risk.sRiskFlag[0] = BOOL_FALSE;//取消风险
				}
				else
				{
					risk.sRiskFlag[0] = BOOL_TRUE;//发生风险
				}
				issendfund = true;
				m_pMemDb->m_pDbTrader.m_pProdFundRisk.Update(risk);
			}

		}
		pposirisk = m_pMemDb->m_pDbTrader.m_pProdPosiRisk.Select(posirisk.sProdCode);
		if (pposirisk == NULL)
		{
			m_pMemDb->m_pDbTrader.m_pProdPosiRisk.Insert(posirisk);
		}
		else
		{
			//若原来的
			if (posirisk.nRiskLevel != pposirisk->nRiskLevel)
			{
				if (posirisk.nRiskLevel == 10)
				{
					posirisk.sRiskFlag[0] = BOOL_FALSE;//取消风险
				}
				else
				{
					posirisk.sRiskFlag[0] = BOOL_TRUE;//发生风险
				}
				issendposi = true;
				m_pMemDb->m_pDbTrader.m_pProdPosiRisk.Update(posirisk);
			}
			
		}
		char riskdata[1000];
		unsigned int riskdatalen=sizeof(riskdata) ;
		if (issendfund)
		{
			m_pXdp->ResetData();
			m_pXdp->SetFieldValue((unsigned short)100,risk.sProdCode,strlen(risk.sProdCode),m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)40,(int)risk.nRiskLevel,m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)60,risk.dRisk,m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)61,risk.dUnitValue,m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)62,risk.dMargin,m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)63,risk.dBal,m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)103,risk.sRight,strlen(risk.sRight),m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)101,risk.sRiskDesc,strlen(risk.sRiskDesc),m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)102,risk.sRiskFlag,strlen(risk.sRiskFlag),m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)38,risk.nRiskdate,m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)39,risk.nRisktime,m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)115,pProd->sFinaProdName,strlen(pProd->sFinaProdName),m_sErrMsg);
			if (!m_pXdp->ToBuffer(riskdata,riskdatalen,m_sErrMsg))
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"Xdp ToBuffer error %s",m_sErrMsg);
			}
			else
			{
				Send2OperProdOrgRiskMsg(pProd->sOrgCode,risk.sRight[0],riskdata,riskdatalen,9821);
			}
			
		}
		if (issendposi)
		{
			m_pXdp->ResetData();
			m_pXdp->SetFieldValue((unsigned short)100,posirisk.sProdCode,strlen(risk.sProdCode),m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)40,(int)posirisk.nRiskLevel,m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)62,posirisk.dMarginRate,m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)63,posirisk.dPosiValueRate,m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)64,posirisk.dPosiDiffValueRate,m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)65,posirisk.dT_Bal,m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)66,posirisk.dT_Buy,m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)67,posirisk.dT_Sell,m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)103,posirisk.sRight,strlen(risk.sRight),m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)101,posirisk.sRiskDesc,strlen(risk.sRiskDesc),m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)102,posirisk.sRiskFlag,strlen(risk.sRiskFlag),m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)38,posirisk.nRiskdate,m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)39,posirisk.nRisktime,m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)115,pProd->sFinaProdName,strlen(pProd->sFinaProdName),m_sErrMsg);
			if (!m_pXdp->ToBuffer(riskdata,riskdatalen,m_sErrMsg))
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"Xdp ToBuffer error %s",m_sErrMsg);
			}
			else
			{
				Send2OperProdOrgRiskMsg(pProd->sOrgCode,risk.sRight[0],riskdata,riskdatalen,9822);
			}
		}
		pProd = m_pMemDb->m_pDbBaseinfo.m_pProd.Next();
	}
	return true;
}
bool RiskDll::ComputeOrg(const char *compdate)
{
	int ret;
	int i,j,k;
	std::vector<S_MDB_CUST_FUNDACC_INFO *> orgcustlist;
	std::vector<S_MDB_CUST_FUNDACC_INFO *> orgmgrcustlist;
	std::vector<S_MDB_ORG_INFO *>orglist;
	std::vector<S_MDB_ORG_INFO *>calcorglist;
	//计算机构资金

	m_pMemDb->m_pDbPublic.m_pOrg.GetAllOrg(orglist);
	if (orglist.size()<1)
	{
		return true;
	}
	bool isTopOrg=false;
	for (i=0 ; i<orglist.size() ; i++)
	{
		orgcustlist.clear();
		orgmgrcustlist.clear();
		if (strcmp(orglist[i]->sUpOrg,"NULL")== 0 || orglist[i]->sOrgLevel[0] == '0')
		{
			//顶级机构，取所有资金账户
			isTopOrg = true;
			if (!m_pMemDb->m_pDbCust.m_pCustFundAcc.SelectAll(orgcustlist))
			{
				//无资金账户
				continue;
			}
		}
		else
		{
			isTopOrg = false;
			calcorglist.push_back(orglist[i]);
			if (!m_pMemDb->m_pDbPublic.m_pOrg.GetAllChildOrg(orglist[i]->sOrgCode,calcorglist,true))
			{
				//此机构无下属机构
			}
			//逐个按机构取资金账户
			for (j=0; j<calcorglist.size(); j++)
			{
				m_pMemDb->m_pDbCust.m_pCustFundAcc.SelectByAccOrg(calcorglist[j]->sOrgCode,orgcustlist);

				m_pMemDb->m_pDbCust.m_pCustFundAcc.SelectByMgrOrg(calcorglist[j]->sOrgCode,orgmgrcustlist);
			}
		}
		ComputeOneOrg(compdate,orglist[i]->sOrgCode,orglist[i]->sOrgName,"0",orgcustlist);//计算所属机构
		ComputeOneOrg(compdate,orglist[i]->sOrgCode,orglist[i]->sOrgName,"1",orgmgrcustlist);//计算代理机构
	}
	return true;
}
bool RiskDll::ComputeOneOrg(const char *compdate,char *orgcode, const char *orgname,const char *risktype,std::vector<S_MDB_CUST_FUNDACC_INFO *> orgcustlist)
{
	double dBal;//结存 权益
	double dMargin;//保证金
	double dPosiBuyTotal;//多仓价值
	double dPosiSellTotal;//空仓价值

	double dBalTmp;
	double dMarginTmp;
	double dPosiBuy;
	double dPosiSell;
	int ff;
	int ret;
	int i,j,k;

	S_MDB_ORG_FUND_RISK risk;
	S_MDB_ORG_POSI_RISK posirisk;
	char traderight[2];
	bzero(traderight,sizeof(traderight));
	bzero(&risk,sizeof(S_MDB_ORG_FUND_RISK));
	bzero(&posirisk,sizeof(S_MDB_ORG_POSI_RISK));
	
	std::vector<S_MDB_CUST_POSITION *> posilist;
	std::vector<S_MDB_CUST_FUND *>fundlist;
	S_MDB_CONTRACT_INFO *pcinfo=NULL;
	S_MDB_ORG_FUND_RISK *pfundrisk=NULL;
	S_MDB_ORG_POSI_RISK *pposirisk=NULL;
	S_MDB_CURRENCY_RATE  *pCurrencyRate=NULL;
	//某一个机构
	strcpy(risk.sDate,compdate);
	strcpy(risk.sRiskType,risktype);
	strcpy(risk.sOrgCode,orgcode);
	strcpy(posirisk.sDate,compdate);
	strcpy(posirisk.sOrgCode,risk.sOrgCode);
	strcpy(posirisk.sRiskType,risktype);
	dBal =0.0;
	dMargin=0.0;
	dPosiBuyTotal=0.0;
	dPosiSellTotal=0.0;
	m_pDate.Update();
	for (j=0; j<orgcustlist.size(); j++)
	{
		//某一个资金账号
		//根据资金账号去取资金，结存及保证金
		fundlist.clear();
		if (!m_pMemDb->m_pDbTrader.m_pFund.Select(orgcustlist[j]->sTxAcc,fundlist))
		{
			continue;;
		}
		for (ff=0; ff<fundlist.size(); ff++) //计算机构总的保证金和权益
		{
			if (fundlist[ff]->sCurrency[0] != CURRENCY_CNY) 
			{
				//不是rmb，进行转换
				pCurrencyRate = m_pMemDb->m_pDbSet.m_pCurrencyRate.Select(fundlist[ff]->sCurrency);
				if (pCurrencyRate == NULL)
				{
//					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"币种 [%s] 未配置兑换比率",fundlist[ff]->sCurrency);
					dBalTmp = fundlist[ff]->dBalance;
					dMarginTmp = fundlist[ff]->dMargin;
					dBal+=dBalTmp;
					dMargin+=dMarginTmp;
				}
				else
				{
					dBalTmp = fundlist[ff]->dBalance*pCurrencyRate->dRate;
					dMarginTmp = fundlist[ff]->dMargin*pCurrencyRate->dRate;
					dBal+=dBalTmp;
					dMargin+=dMarginTmp;
				}

			}
			else
			{
				dBalTmp = fundlist[ff]->dBalance;
				dMarginTmp = fundlist[ff]->dMargin;
				dBal+=dBalTmp;
				dMargin+=dMarginTmp;
			}
		}
		//某一个资金账号的持仓
		//取持仓，计算持仓总价值  合约轧差占比
		posilist.clear();
		if (m_pMemDb->m_pDbTrader.m_pPosition.SelectByAcc(orgcustlist[j]->sTxAcc,posilist))
		{
			for (k=0 ; k<posilist.size(); k++) 
			{
				pcinfo = m_pMemDb->m_pDbBaseinfo.m_pContract.Select(posilist[k]->nContractNo);
				if (pcinfo == NULL)
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"合约编号 [%d] 在合约表不存在",posilist[k]->nContractNo);
					continue;
				}
				if (pcinfo->sProd_type[0] == PROD_TYPE_OPTION) //期权
				{
					//取标的合约的结算价
					//取delta及标的合约的结算价
					S_MDB_CONTRACT_SETTLE_PRICE *pcsp = m_pMemDb->m_pDbBaseinfo.m_pSettlePrice.Select(pcinfo->nProdId);
					if (pcsp == NULL)
					{
						dPosiBuy = posilist[k]->dLAST_SETTLE_PRICE* posilist[k]->nCurPB * pcinfo->dTxUnit;
						dPosiSell = posilist[k]->dLAST_SETTLE_PRICE* posilist[k]->nCurPS * pcinfo->dTxUnit;
						dPosiSellTotal += dPosiSell;
						dPosiBuyTotal +=dPosiBuy;
						continue;
					}
					S_MDB_OPTION_CONTRACT_PRICE_MODEL *pomodel = m_pMemDb->m_pDbBaseinfo.m_pOptionModel.Select(posilist[k]->nContractNo);
					if (pomodel == NULL)
					{
						dPosiBuy = posilist[k]->dLAST_SETTLE_PRICE* posilist[k]->nCurPB * pcinfo->dTxUnit;
						dPosiSell = posilist[k]->dLAST_SETTLE_PRICE* posilist[k]->nCurPS * pcinfo->dTxUnit;
						dPosiSellTotal += dPosiSell;
						dPosiBuyTotal +=dPosiBuy;
						continue;
					}

					dPosiBuy = pomodel->dDelta*pcsp->dSettlePrice* posilist[k]->nCurPB * pcinfo->dTxUnit;
					dPosiSell = pomodel->dDelta*pcsp->dSettlePrice* posilist[k]->nCurPS * pcinfo->dTxUnit;

				}
				else //期货
				{
					//持仓价值
					dPosiBuy = posilist[k]->dLAST_SETTLE_PRICE* posilist[k]->nCurPB * pcinfo->dTxUnit;
					dPosiSell = posilist[k]->dLAST_SETTLE_PRICE* posilist[k]->nCurPS * pcinfo->dTxUnit;

				}
				dPosiSellTotal += dPosiSell;
				dPosiBuyTotal +=dPosiBuy;
			}
		}

	} // end 一个资金账户
	//保证金占比 单位净值 持仓价值比
	if (dBal> MINDOUBLECOMPARE && dBal<MAXDOUBLECOMPARE)
	{
		posirisk.dMarginRate = 0.0;
		posirisk.dPosiValueRate = 0.0;
		posirisk.dPosiDiffValueRate=0.0;
	}
	else 
	{
		posirisk.dMarginRate = dMargin/dBal;//保证金占比
		posirisk.dPosiValueRate = (dPosiBuyTotal+dPosiSellTotal)/dBal;//持仓价值比
		posirisk.dPosiDiffValueRate = (dPosiBuyTotal-dPosiSellTotal)/dBal;//持仓轧差价值比
	}
	//单位净值

	risk.dUnitValue = dBal;
	risk.dBal = dBal;
	risk.dMargin = dMargin;
	posirisk.dT_Bal = dBal;
	posirisk.dT_Buy = dPosiBuyTotal;
	posirisk.dT_Sell = dPosiSellTotal;

	posirisk.nRiskdate = atoi(m_pDate.ToStringD8().c_str());
	posirisk.nRisktime = atoi(m_pDate.ToStringT6().c_str());
	risk.nRiskdate = atoi(m_pDate.ToStringD8().c_str());
	risk.nRisktime = atoi(m_pDate.ToStringT6().c_str());

	if (dBal> MINDOUBLECOMPARE && dBal<MAXDOUBLECOMPARE)
	{
		risk.dRisk =0.0;
	}
	else
	{
		risk.dRisk = dMargin/dBal;//保证金/权益
	}
	risk.nRiskLevel = m_pKernelPub.GetOrgRiskLevel(risk.sOrgCode,risk.dUnitValue,risk.dRisk,traderight,risk.sRiskDesc);
	strcpy(risk.sRight,traderight);

	posirisk.nRiskLevel = m_pKernelPub.GetOrgRiskLevel(risk.sOrgCode,posirisk.dMarginRate,posirisk.dPosiValueRate,posirisk.dPosiDiffValueRate,posirisk.sRight,posirisk.sRiskDesc);

	bool issendfund=false;
	bool issendposi=false;
	//更新到表里
	pfundrisk = m_pMemDb->m_pDbTrader.m_pOrgFundRisk.Select(risk.sOrgCode,(char *)risktype);
	if (pfundrisk == NULL)
	{
		if (risk.nRiskLevel <10)
		{
			risk.sRiskFlag[0] = BOOL_TRUE;
			issendfund = true;
		}
		m_pMemDb->m_pDbTrader.m_pOrgFundRisk.Insert(risk);

	}
	else
	{
		//若原来的
		if (risk.nRiskLevel != pfundrisk->nRiskLevel)
		{
			if (risk.nRiskLevel == 10)
			{
				risk.sRiskFlag[0] = BOOL_FALSE;//取消风险
			}
			else
			{
				risk.sRiskFlag[0] = BOOL_TRUE;//发生风险
			}
			issendfund = true;
			m_pMemDb->m_pDbTrader.m_pOrgFundRisk.Update(risk);
		}

	}
	pposirisk = m_pMemDb->m_pDbTrader.m_pOrgPosiRisk.Select(posirisk.sOrgCode,(char *)risktype);
	if (pposirisk == NULL)
	{
		m_pMemDb->m_pDbTrader.m_pOrgPosiRisk.Insert(posirisk);
	}
	else
	{
		//若原来的
		if (posirisk.nRiskLevel != pposirisk->nRiskLevel)
		{
			if (posirisk.nRiskLevel == 10)
			{
				posirisk.sRiskFlag[0] = BOOL_FALSE;//取消风险
			}
			else
			{
				posirisk.sRiskFlag[0] = BOOL_TRUE;//发生风险
			}
			issendposi = true;
			m_pMemDb->m_pDbTrader.m_pOrgPosiRisk.Update(posirisk);
		}

	}
	char riskdata[1000];
	unsigned int riskdatalen=sizeof(riskdata) ;
	if (issendfund)
	{
		m_pXdp->ResetData();
		m_pXdp->SetFieldValue((unsigned short)100,risk.sOrgCode,strlen(risk.sOrgCode),m_sErrMsg);
		m_pXdp->SetFieldValue((unsigned short)104,risk.sRiskType,strlen(risk.sRiskType),m_sErrMsg);
		m_pXdp->SetFieldValue((unsigned short)40,(int)risk.nRiskLevel,m_sErrMsg);
		m_pXdp->SetFieldValue((unsigned short)60,risk.dRisk,m_sErrMsg);
		m_pXdp->SetFieldValue((unsigned short)61,risk.dUnitValue,m_sErrMsg);
		m_pXdp->SetFieldValue((unsigned short)62,risk.dMargin,m_sErrMsg);
		m_pXdp->SetFieldValue((unsigned short)63,risk.dBal,m_sErrMsg);
		m_pXdp->SetFieldValue((unsigned short)103,risk.sRight,strlen(risk.sRight),m_sErrMsg);
		m_pXdp->SetFieldValue((unsigned short)101,risk.sRiskDesc,strlen(risk.sRiskDesc),m_sErrMsg);
		m_pXdp->SetFieldValue((unsigned short)102,risk.sRiskFlag,strlen(risk.sRiskFlag),m_sErrMsg);
		m_pXdp->SetFieldValue((unsigned short)38,risk.nRiskdate,m_sErrMsg);
		m_pXdp->SetFieldValue((unsigned short)39,risk.nRisktime,m_sErrMsg);
		m_pXdp->SetFieldValue((unsigned short)115,orgname,strlen(orgname),m_sErrMsg);
		if (!m_pXdp->ToBuffer(riskdata,riskdatalen,m_sErrMsg))
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"Xdp ToBuffer error %s",m_sErrMsg);
		}
		else
		{
			Send2OperProdOrgRiskMsg(orgcode,risk.sRight[0],riskdata,riskdatalen,9849);
		}

	}
	if (issendposi)
	{
		m_pXdp->ResetData();
		m_pXdp->SetFieldValue((unsigned short)100,posirisk.sOrgCode,strlen(risk.sOrgCode),m_sErrMsg);
		m_pXdp->SetFieldValue((unsigned short)104,risk.sRiskType,strlen(risk.sRiskType),m_sErrMsg);
		m_pXdp->SetFieldValue((unsigned short)40,(int)posirisk.nRiskLevel,m_sErrMsg);
		m_pXdp->SetFieldValue((unsigned short)62,posirisk.dMarginRate,m_sErrMsg);
		m_pXdp->SetFieldValue((unsigned short)63,posirisk.dPosiValueRate,m_sErrMsg);
		m_pXdp->SetFieldValue((unsigned short)64,posirisk.dPosiDiffValueRate,m_sErrMsg);
		m_pXdp->SetFieldValue((unsigned short)65,posirisk.dT_Bal,m_sErrMsg);
		m_pXdp->SetFieldValue((unsigned short)66,posirisk.dT_Buy,m_sErrMsg);
		m_pXdp->SetFieldValue((unsigned short)67,posirisk.dT_Sell,m_sErrMsg);
		m_pXdp->SetFieldValue((unsigned short)103,posirisk.sRight,strlen(risk.sRight),m_sErrMsg);
		m_pXdp->SetFieldValue((unsigned short)101,posirisk.sRiskDesc,strlen(risk.sRiskDesc),m_sErrMsg);
		m_pXdp->SetFieldValue((unsigned short)102,posirisk.sRiskFlag,strlen(risk.sRiskFlag),m_sErrMsg);
		m_pXdp->SetFieldValue((unsigned short)38,posirisk.nRiskdate,m_sErrMsg);
		m_pXdp->SetFieldValue((unsigned short)39,posirisk.nRisktime,m_sErrMsg);
		m_pXdp->SetFieldValue((unsigned short)115,orgname,strlen(orgname),m_sErrMsg);
		if (!m_pXdp->ToBuffer(riskdata,riskdatalen,m_sErrMsg))
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"Xdp ToBuffer error %s",m_sErrMsg);
		}
		else
		{
			Send2OperProdOrgRiskMsg(orgcode,risk.sRight[0],riskdata,riskdatalen,9850);
		}
	}

	return true;
}
void RiskDll::Send2OperProdOrgRiskMsg(char *orgcode, char traderight,const char *riskdata,int riskdatalen,int funcno)
{
	if (*g_nSvrHostStatus != 	ARBSTATUS_MASTER && *g_nSvrHostStatus != ARBSTATUS_MASTER_AYN && *g_nSvrHostStatus!=MASTERHOST)
	{
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"Send2CustRiskMsg 不是主机，略过");
		return;
	}
	//发送给操作员，代理该产品的操作员或该产品的上级机构操作员，操作员已登录且订阅了消息
	

	//查找在线并且订阅了风险的操作员
	std::vector<S_MDB_SUBSCRIBE *> sublist;
	if (!m_pMemDb->m_pDbTrader.m_pSubcribe.Select((char *)"0",sublist))
	{
		return;
	}
	int i,j;
	std::vector<S_MDB_OPER_ONLINE *> onlist;
	for (i=0;i<sublist.size(); i++)
	{
		//查看操作员是否在线，是否有权限
		onlist.clear();
		if (!m_pMemDb->m_pDbPublic.m_pOperOnline.Select(sublist[i]->sOper,onlist))
		{
			continue;
		}
		for (j=0 ; j<onlist.size(); j++)
		{
			//是否登录
			if (onlist[j]->sOnlineStatus[0] != LOGIN_STATUS_OK )
			{
				continue;
			}
			//查找操作员机构
			S_MDB_OPER_INFO *poper = m_pMemDb->m_pDbPublic.m_pOper.Select(onlist[j]->sOperCode);
			if (poper== NULL)
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"操作员[%s]在操作员表中不存在",onlist[j]->sOperCode);
				continue;
			}
			//查看机构是否有权限
			if (m_pKernelPub.CheckRight(orgcode,poper->sOrgCode))
			{
				S_TRADE_DATA sdata;
				//有权限，发送报文
//				sdata.pData = (PBPCCOMMSTRU)m_pMemPool->PoolMalloc();
				if (m_pClientLink->PoolMalloc(sdata)<0)
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"PoolMalloc error");
					return ;
				}
				bzero(&(sdata.pData->sDBHead),DREBHEADLEN);
				
				memcpy(sdata.pData->sBuffer,riskdata,riskdatalen);
				sdata.pData->sDBHead.nLen = riskdatalen;
				
				
				sdata.pData->sDBHead.cRaflag = 0;
				sdata.pData->sDBHead.cCmd = CMD_DPPUSH;
				sdata.pData->sDBHead.d_Dinfo.d_nServiceNo = funcno;
				if (strcmp(onlist[j]->sTermType,TERM_TYPE_RPCAPI) ==0)
				{
					//socket
					sdata.src = 1;
					sdata.pData->sBpcHead.nIndex = onlist[j]->nGateIndex;
					sdata.pData->sBpcHead.nConnectTime = onlist[j]->nTimeStamp;
				}
				else //总线
				{
					sdata.src = 0;
					sdata.pData->sDBHead.d_Dinfo.d_nNodeId = onlist[j]->nDrebId;
					sdata.pData->sDBHead.d_Dinfo.d_cNodePrivateId = onlist[j]->nDrebPid;
					sdata.pData->sDBHead.d_Dinfo.d_nSvrMainId = onlist[j]->nSvrId;
					sdata.pData->sDBHead.d_Dinfo.d_cSvrPrivateId = onlist[j]->nSvrPid;
					sdata.pData->sDBHead.s_Sinfo.s_nGateIndex = onlist[j]->nGateIndex;
					sdata.pData->sDBHead.s_Sinfo.s_nHook = onlist[j]->nTimeStamp;
					
				}
				m_pClientLink->Send2Client(sdata);
			}
		}
	}
	
}


void RiskDll::SendOrderPush(S_MDB_TXACC_LOCK *acclock,S_MDB_ORDER_INFO *info)
{
	//准备数据
	m_pXdp->ResetData();
	m_pXdp->SetFieldValue((unsigned short)40,(int)atoi(info->sTxDate),m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)101,info->sSeatNo,strlen(info->sSeatNo),m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)102,info->sTxAcc,strlen(info->sTxAcc),m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)100,info->sContractCode,strlen(info->sContractCode),m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)5,(short)info->sEoFlag[0],m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)6,(short)info->sBsFlag[0],m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)7,(short)info->sShFlag[0],m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)8,(short)info->sTxType[0],m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)9,(short)info->sCmdType[0],m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)10,(short)info->sCmdAttr[0],m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)11,(short)info->sForceFlag[0],m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)41,info->nOrderNum,m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)60,info->dOrderPrice,m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)103,info->sLocalSerial,strlen(info->sLocalSerial),m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)104,info->sSystemNo,strlen(info->sSystemNo),m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)14,(short)info->sOrderStatus[0],m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)105,info->sOrderOper,strlen(info->sOrderOper),m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)106,info->sExchCode,strlen(info->sExchCode),m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)107,info->sTxCode,strlen(info->sTxCode),m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)20,(unsigned int)info->nContractNo,m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)108,info->sClientSerial,strlen(info->sClientSerial),m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)42,(int)info->nRemainNum,m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)61,info->dAmt,m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)62,info->dFrozAmt,m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)63,info->dExFee,m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)109,info->sCancelFlag,strlen(info->sCancelFlag),m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)110,info->sAnsTime,strlen(info->sAnsTime),m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)111,info->sOrderChannel,strlen(info->sOrderChannel),m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)112,info->sAccOrg,strlen(info->sAccOrg),m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)113,info->sTxOrg,strlen(info->sTxOrg),m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)114,info->sCancelChannel,strlen(info->sCancelChannel),m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)115,info->sCancelOper,strlen(info->sCancelOper),m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)116,info->sCancelTime,strlen(info->sCancelTime),m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)66,info->dDoneAvp,m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)23,info->nCancelNum,m_sErrMsg);

	S_MDB_CUST_FUNDACC_INFO *pFundAcc=NULL;

	pFundAcc = m_pMemDb->m_pDbCust.m_pCustFundAcc.At(acclock->nFundAccRowid);
	if (pFundAcc != NULL)
	{
		m_pXdp->SetFieldValue((unsigned short)119,pFundAcc->sCustName,strlen(pFundAcc->sCustName),m_sErrMsg);
	} 
	char sPushdata[2000];
	unsigned int len = sizeof(sPushdata);
	if (!m_pXdp->ToBuffer(sPushdata,len,m_sErrMsg))
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"Xdp ToBuffer error");
		return ;
	}
	Send2CustPushMsg(acclock,sPushdata,len,9831);
	Send2OperPushMsg(acclock,sPushdata,len,9831);
}

void RiskDll::SendDonePush(S_MDB_TXACC_LOCK *acclock ,S_MDB_DONE_INFO *info)
{
	//准备数据
	m_pXdp->ResetData();
	
	m_pXdp->SetFieldValue((unsigned short)100,info->sTxAcc,strlen(info->sTxAcc),m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)101,info->sExchCode,strlen(info->sExchCode),m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)102,info->sSeatNo,strlen(info->sSeatNo),m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)103,info->sTxCode,strlen(info->sTxCode),m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)20,info->nContractNo,m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)104,info->sContractCode,strlen(info->sContractCode),m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)5,(short)info->sEoFlag[0],m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)6,(short)info->sBsFlag[0],m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)7,(short)info->sShFlag[0],m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)8,(short)info->sTxType[0],m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)9,(short)info->sCmdType[0],m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)10,(short)info->sCmdAttr[0],m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)11,(short)info->sForceFlag[0],m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)41,(int)info->nDoneNum,m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)60,info->dDonePrice,m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)105,info->sDoneDate,strlen(info->sDoneDate),m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)106,info->sDoneTime,strlen(info->sDoneTime),m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)107,info->sDoneNo,strlen(info->sDoneNo),m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)108,info->sLocalNo,strlen(info->sLocalNo),m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)109,info->sSystemNo,strlen(info->sSystemNo),m_sErrMsg);
	
	m_pXdp->SetFieldValue((unsigned short)61,info->dAmt,m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)62,info->dExFee,m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)63,info->dMemberFee,m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)64,info->dExMargin,m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)65,info->dMemberMargin,m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)66,info->dFloatSurplus,m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)67,info->dCovSurplus,m_sErrMsg);
	
	m_pXdp->SetFieldValue((unsigned short)110,info->sAccOrg,strlen(info->sAccOrg),m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)111,info->sTxOrg,strlen(info->sTxOrg),m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)112,info->sOrderChannel,strlen(info->sOrderChannel),m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)113,info->sOrderOper,strlen(info->sOrderOper),m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)114,info->sSystemDate,strlen(info->sSystemDate),m_sErrMsg);

	m_pXdp->SetFieldValue((unsigned short)68,info->dUnfrozeAmt,m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)69,info->dUnfrozeFee,m_sErrMsg);
	S_MDB_CUST_FUNDACC_INFO *pFundAcc=NULL;

	pFundAcc = m_pMemDb->m_pDbCust.m_pCustFundAcc.At(acclock->nFundAccRowid);
	if (pFundAcc != NULL)
	{
		m_pXdp->SetFieldValue((unsigned short)119,pFundAcc->sCustName,strlen(pFundAcc->sCustName),m_sErrMsg);
	} 
	char sPushdata[2000];
	unsigned int len = sizeof(sPushdata);
	if (!m_pXdp->ToBuffer(sPushdata,len,m_sErrMsg))
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"Xdp ToBuffer error");
		return ;
	}
	Send2CustPushMsg(acclock,sPushdata,len,9834);
	Send2OperPushMsg(acclock,sPushdata,len,9834);
}

void RiskDll::SendMoneyIOPush(S_MDB_TXACC_LOCK *acclock ,S_MDB_FUND_FLOW *info)
{
	//准备数据
	m_pXdp->ResetData();
	
	m_pXdp->SetFieldValue((unsigned short)40,(int)atoi(info->sTxDate),m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)102,info->sTxAcc,strlen(info->sTxAcc),m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)60,info->dAmt,m_sErrMsg);
	
	m_pXdp->SetFieldValue((unsigned short)0,(unsigned short)atoi(info->sFundFlag),m_sErrMsg);
	
	m_pXdp->SetFieldValue((unsigned short)2,(unsigned short)info->sCurrency[0],m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)103,info->sTxSerial,strlen(info->sTxSerial),m_sErrMsg);
	S_MDB_CUST_FUNDACC_INFO *pFundAcc=NULL;

	pFundAcc = m_pMemDb->m_pDbCust.m_pCustFundAcc.At(acclock->nFundAccRowid);
	if (pFundAcc != NULL)
	{
		m_pXdp->SetFieldValue((unsigned short)119,pFundAcc->sCustName,strlen(pFundAcc->sCustName),m_sErrMsg);
	} 
	char sPushdata[1000];
	unsigned int len = sizeof(sPushdata);
	if (!m_pXdp->ToBuffer(sPushdata,len,m_sErrMsg))
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"Xdp ToBuffer error");
		return ;
	}
	Send2CustPushMsg(acclock,sPushdata,len,9835);
	Send2OperPushMsg(acclock,sPushdata,len,9835);	
}

void RiskDll::Send2CustPushMsg(S_MDB_TXACC_LOCK *acc, const char *pushdata,int pushdatalen,int funcno)
{
	S_TRADE_DATA sdata;
	//查找登录表，推送消息
	S_MDB_CUSTINFO *cust=	m_pMemDb->m_pDbCust.m_pCustom.At(acc->nCustRowid);
	if (cust == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"Send2CustPushMsg cust is NULL");
		return;
	}
	std::vector<S_MDB_CUST_ONLINE *> onlist;
	bool bret = m_pMemDb->m_pDbTrader.m_pCustOnline.Select(cust->sCustNo,onlist);
	if (!bret)
	{
		return;
	}

	for (int i=0; i<onlist.size();i++)
	{
		if (onlist[i]->sOnlineStatus[0] != LOGIN_STATUS_OK)
		{
			continue;
		}
		if (m_pClientLink->PoolMalloc(sdata)<0)
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"PoolMalloc error");
			return ;
		}
		bzero(&(sdata.pData->sDBHead),DREBHEADLEN);

		memcpy(sdata.pData->sBuffer,pushdata,pushdatalen);
		sdata.pData->sDBHead.nLen = pushdatalen;

		
		sdata.pData->sDBHead.cRaflag = 0;
		sdata.pData->sDBHead.cCmd = CMD_DPPUSH;
		sdata.pData->sDBHead.d_Dinfo.d_nServiceNo = funcno;
		if (strcmp(onlist[i]->sTermType,TERM_TYPE_RPCAPI) ==0)
		{
			//socket
			sdata.src = 1;
			sdata.pData->sBpcHead.nIndex = onlist[i]->nGateIndex;
			sdata.pData->sBpcHead.nConnectTime = onlist[i]->nTimeStamp;
		}
		else //总线
		{
			sdata.src = 0;
			sdata.pData->sDBHead.d_Dinfo.d_nNodeId = onlist[i]->nDrebId;
			sdata.pData->sDBHead.d_Dinfo.d_cNodePrivateId = onlist[i]->nDrebPid;
			sdata.pData->sDBHead.d_Dinfo.d_nSvrMainId = onlist[i]->nSvrId;
			sdata.pData->sDBHead.d_Dinfo.d_cSvrPrivateId = onlist[i]->nSvrPid;
			sdata.pData->sDBHead.s_Sinfo.s_nGateIndex = onlist[i]->nGateIndex;
			sdata.pData->sDBHead.s_Sinfo.s_nHook = onlist[i]->nTimeStamp;
			
		}
		m_pClientLink->Send2Client(sdata);
	}
}

void RiskDll::Send2OperPushMsg(S_MDB_TXACC_LOCK *acc,const char *pushdata,int pushdatalen,int funcno)
{
	//发送给操作员，代理该客户的操作员或该客户的上级机构操作员，操作员已登录
	S_MDB_CUST_FUNDACC_INFO *pcust = m_pMemDb->m_pDbCust.m_pCustFundAcc.At(acc->nFundAccRowid);
	if (pcust == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"资金账号为空 [%d]",acc->nFundAccRowid);
		return;
	}
	//资金账号所属机构
	int len = strlen(pcust->sSignOrg);
	//查找在线并操作员
	int i,j;
	std::vector<S_MDB_OPER_ONLINE *> onlist;
	
	char sStatus[2];
	sStatus[0] =LOGIN_STATUS_OK;
	sStatus[1] =0;
	//查看操作员是否在线，是否有权限
	if (!m_pMemDb->m_pDbPublic.m_pOperOnline.SelectByStatus(sStatus,onlist))
	{
		return;
	}
	for (j=0 ; j<onlist.size(); j++)
	{
		//查找操作员机构
		S_MDB_OPER_INFO *poper = m_pMemDb->m_pDbPublic.m_pOper.Select(onlist[j]->sOperCode);
		if (poper== NULL)
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"操作员[%s]在操作员表中不存在",onlist[j]->sOperCode);
			continue;
		}
		//查看是否有权限
		if (m_pKernelPub.CheckRight(pcust->sSignOrg,poper->sOrgCode))
		{
			S_TRADE_DATA sdata;
			//有权限，发送报文
			if (m_pClientLink->PoolMalloc(sdata)<0)
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"PoolMalloc error");
				return ;
			}
			bzero(&(sdata.pData->sDBHead),DREBHEADLEN);
			
			memcpy(sdata.pData->sBuffer,pushdata,pushdatalen);
			sdata.pData->sDBHead.nLen = pushdatalen;
			
			
			sdata.pData->sDBHead.cRaflag = 0;
			sdata.pData->sDBHead.cCmd = CMD_DPPUSH;
			sdata.pData->sDBHead.d_Dinfo.d_nServiceNo = funcno;
			if (strcmp(onlist[j]->sTermType,TERM_TYPE_RPCAPI) ==0)
			{
				//socket
				sdata.src = 1;
				sdata.pData->sBpcHead.nIndex = onlist[j]->nGateIndex;
				sdata.pData->sBpcHead.nConnectTime = onlist[j]->nTimeStamp;
			}
			else //总线
			{
				sdata.src = 0;
				sdata.pData->sDBHead.d_Dinfo.d_nNodeId = onlist[j]->nDrebId;
				sdata.pData->sDBHead.d_Dinfo.d_cNodePrivateId = onlist[j]->nDrebPid;
				sdata.pData->sDBHead.d_Dinfo.d_nSvrMainId = onlist[j]->nSvrId;
				sdata.pData->sDBHead.d_Dinfo.d_cSvrPrivateId = onlist[j]->nSvrPid;
				sdata.pData->sDBHead.s_Sinfo.s_nGateIndex = onlist[j]->nGateIndex;
				sdata.pData->sDBHead.s_Sinfo.s_nHook = onlist[j]->nTimeStamp;
				
			}
			m_pClientLink->Send2Client(sdata);
		}
	}
	
	
}




int RiskDll::ProcessDone(S_MDB_CUST_FUNDACC_INFO *pcustacc, S_MDB_ORDER_INFO *po,S_MDB_CONTRACT_INFO *pc,S_MDB_EXCH_INFO *pexch,S_MDB_DONE_INFO &info)
{
	
	info.dUnfrozeFee = po->dExFee * info.nDoneNum/po->nOrderNum*(-1.0);  //变为正数
	info.dUnfrozeAmt = po->dFrozAmt * info.nDoneNum/po->nOrderNum*(-1.0);  //变为负数

	strcpy(info.sTxOrg,po->sTxOrg);
	strcpy(info.sAccOrg,po->sAccOrg);
	strcpy(info.sTxType,po->sTxType);
	strcpy(info.sCmdType,po->sCmdType);
	strcpy(info.sCmdAttr,po->sCmdAttr);
	strcpy(info.sOrderOper,po->sOrderOper);
	strcpy(info.sTxCode,po->sTxCode);
	strcpy(info.sSeatNo,po->sSeatNo);
	strcpy(info.sBsFlag,po->sBsFlag);
	strcpy(info.sEoFlag,po->sEoFlag);
	strcpy(info.sShFlag,po->sShFlag);
	info.nVarietyId = pc->nProdId;
	//成交金额
	info.dAmt = info.dDonePrice * info.nDoneNum*pc->dTxUnit;
	
	//得到手续费等
	double dExchFee=0.0;//交易所手续费
	double dMemberFee=0.0;//会员手续费
	double dExchMargin=0.0;//交易所保证金
	double dMemberMargin=0.0;//会员保证金
	
	//取要扣作手续费及保证金等等
	switch (pc->sProd_type[0])
	{
		case PROD_TYPE_FUTURES://期货
			if (info.sEoFlag[0] == EO_FLAG_OPEN) //开仓
			{
				//开仓要算手续费和保证金
				if (!m_pKernelPub.GetTradeFeeValue(pc,pcustacc,1,info.nDoneNum,info.dAmt,dExchFee,dMemberFee))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"成交通知取手续费出错 本地报单号[%s] 资金账号[%s] 成交号[%s]",info.sLocalNo,info.sTxAcc,info.sDoneNo);
					return -1;
				}
				if (!m_pKernelPub.GetMarginValue(pc,pcustacc,0,info.nDoneNum,info.dAmt,dExchMargin,dMemberMargin))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"成交通知取保证金出错 本地报单号[%s] 资金账号[%s] 成交号[%s]",info.sLocalNo,info.sTxAcc,info.sDoneNo);
					return -1;
				}
			}
			else
			{
				if (info.sEoFlag[0] == EO_FLAG_CLOSETODAY) //平今仓
				{
					//平仓要算手续费
					if (!m_pKernelPub.GetTradeFeeValue(pc,pcustacc,3,info.nDoneNum,info.dAmt,dExchFee,dMemberFee))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"成交通知取手续费出错 本地报单号[%s] 资金账号[%s] 成交号[%s]",info.sLocalNo,info.sTxAcc,info.sDoneNo);
						return -1;
					}
				}
				else
				{
					//平仓要算手续费
					if (!m_pKernelPub.GetTradeFeeValue(pc,pcustacc,2,info.nDoneNum,info.dAmt,dExchFee,dMemberFee))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"成交通知取手续费出错 本地报单号[%s] 资金账号[%s] 成交号[%s]",info.sLocalNo,info.sTxAcc,info.sDoneNo);
						return -1;
					}
				}
			}
			break;
		case PROD_TYPE_OPTION://期权
			if (info.sEoFlag[0] == EO_FLAG_OPEN) //开仓
			{
				//开仓要算冻结和手续费
				if (!m_pKernelPub.GetTradeFeeValue(pc,pcustacc,1,info.nDoneNum,info.dAmt,dExchFee,dMemberFee))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"成交通知取手续费出错 本地报单号[%s] 资金账号[%s] 成交号[%s]",info.sLocalNo,info.sTxAcc,info.sDoneNo);
					return -1;
				}
				if (info.sBsFlag[0] == BS_FLAG_SELL) //卖开要冻结保证金
				{
					if (!m_pKernelPub.GetMarginValue(pc,pcustacc,1,info.nDoneNum,info.dAmt,dExchMargin,dMemberMargin))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"成交通知取保证金出错 本地报单号[%s] 资金账号[%s] 成交号[%s]",info.sLocalNo,info.sTxAcc,info.sDoneNo);
						return -1;
					}
				}
			}
			else
			{
				//平仓要算手续费
				if (info.sEoFlag[0] == EO_FLAG_CLOSETODAY) //平今仓
				{
					//平仓要算手续费
					if (!m_pKernelPub.GetTradeFeeValue(pc,pcustacc,3,info.nDoneNum,info.dAmt,dExchFee,dMemberFee))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"成交通知取手续费出错 本地报单号[%s] 资金账号[%s] 成交号[%s]",info.sLocalNo,info.sTxAcc,info.sDoneNo);
						return -1;
					}
				}
				else
				{
					//平仓要算手续费
					if (!m_pKernelPub.GetTradeFeeValue(pc,pcustacc,2,info.nDoneNum,info.dAmt,dExchFee,dMemberFee))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"成交通知取手续费出错 本地报单号[%s] 资金账号[%s] 成交号[%s]",info.sLocalNo,info.sTxAcc,info.sDoneNo);
						return -1;
					}
				}
			}
			break;
		case PROD_TYPE_SPOT://现货
			//现货有中立仓、交割等
			if (info.sShFlag[0] == SH_FLAG_DERY)
			{
				//递延交割
			}
			else if (info.sShFlag[0] == SH_FLAG_MID)
			{
				//中立仓
			}
			else
			{
				if (info.sBsFlag[0] == BS_FLAG_BUY)
				{
					//买要算手续费及金额
					if (!m_pKernelPub.GetTradeFeeValue(pc,pcustacc,1,info.nDoneNum,info.dAmt,dExchFee,dMemberFee))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"成交通知取手续费出错 本地报单号[%s] 资金账号[%s] 成交号[%s]",info.sLocalNo,info.sTxAcc,info.sDoneNo);
						return -1;
					}
				}
				else
				{
					//卖要算手续费	
					if (!m_pKernelPub.GetTradeFeeValue(pc,pcustacc,2,info.nDoneNum,info.dAmt,dExchFee,dMemberFee))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"成交通知取手续费出错 本地报单号[%s] 资金账号[%s] 成交号[%s]",info.sLocalNo,info.sTxAcc,info.sDoneNo);
						return -1;
					}
				}
			}
			break;
		case PROD_TYPE_SEC://证券
			if (info.sBsFlag[0] == BS_FLAG_BUY)
			{
				//买要算手续费及金额
				if (!m_pKernelPub.GetTradeFeeValue(pc,pcustacc,1,info.nDoneNum,info.dAmt,dExchFee,dMemberFee))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"成交通知取手续费出错 本地报单号[%s] 资金账号[%s] 成交号[%s]",info.sLocalNo,info.sTxAcc,info.sDoneNo);
					return -1;
				}
			}
			else
			{
				//卖要算手续费
				if (!m_pKernelPub.GetTradeFeeValue(pc,pcustacc,2,info.nDoneNum,info.dAmt,dExchFee,dMemberFee))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"成交通知取手续费出错 本地报单号[%s] 资金账号[%s] 成交号[%s]",info.sLocalNo,info.sTxAcc,info.sDoneNo);
					return -1;
				}
			}
			break;
		default:
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"成交通知合约代码[%s]的品种类别[%s]不符 资金账号[%s] 成交号[%s]",info.sContractCode,pc->sProd_type,info.sTxAcc,info.sDoneNo);
			return -1;
	}
	info.dExFee = dExchFee*(-1.0);//交易所手续费  为负数
	info.dMemberFee = dMemberFee*(-1.0) ; //会员手续费  为负数
	info.dExMargin = dExchMargin;//交易所保证金
	info.dMemberMargin = dMemberMargin;//会员保证金
	info.nSyncFlag = 3;//同步标志
	
	S_MDB_TXACC_LOCK *acclock = m_pMemDb->SelectLock(info.sTxAcc);
	if (acclock == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"成交通知 找不到资金账号信息 [%s]",info.sTxAcc);
		return -1;
	}
	S_MDB_CUST_FUND *pfund = m_pMemDb->GetFund(acclock,pc->sCurrency);
	if (pfund == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"成交通知 找不到资金账号资金信息 [%s]",info.sTxAcc);
		return -1;
	}
	//写成交表
	int rid = m_pMemDb->m_pDbTrader.m_pDone.Insert(info);
	if (rid <0)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"成交通知 本地报单号[%s] 写入成交表失败，可能已处理",info.sLocalNo);
		return -1;
	}
	S_MDB_DONE_INFO *pdone = m_pMemDb->m_pDbTrader.m_pDone.At(rid);
	if (pdone == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"成交通知 本地报单号[%s] rowid[%d]不存在",info.sLocalNo,rid);
		return -1;
	}

	//更新资金、冻结等
	if (!m_pKernelPub.ComputeNewDone(acclock,pfund,pdone,po,pc))
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"成交通知 更新资金 持仓等失败  本地报单号[%s] 资金账号[%s] 成交号[%s]",info.sLocalNo,info.sTxAcc,info.sDoneNo);
		return -1;
	}
	//通知要对此客户进行风险计算
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"CUST acclock rowid :%d",acclock->nLockRowid);
	MsgCustRiskCalc(acclock->nLockRowid);

	m_nEndTime = CBF_Date_Time::GetTickUS();
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"成交通知 处理完成 成交号[%s] 本地报单号[%s] 耗时[%ld]微秒",info.sDoneNo,info.sLocalNo,m_nEndTime-m_nBeginTime);
	//发送回报推送
	SendDonePush(acclock,pdone);
	SendOrderPush(acclock,po);
	return 0;
}



bool RiskDll::UpdateFundLock(const char *txacc,char *currency,const char fundflag,const  char *fundcode,double amt,bool isadd,int &lockrowid)
{
	S_MDB_TXACC_LOCK *info=NULL;
	if (amt > MINDOUBLECOMPARE && amt <MAXDOUBLECOMPARE) //金额为0
	{
		info =  m_pMemDb->SelectLock((char *)txacc);
		if (info == NULL)
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"无此资金账号[%s]",txacc);
			return false;
		}
		lockrowid = info->nLockRowid;
		return true;
	}
	info =  m_pMemDb->SelectLock((char *)txacc);
	if (info == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"无此资金账号[%s]",txacc);
		return false;
	}
	lockrowid = info->nLockRowid;
	S_MDB_CUST_FUND *fund = m_pMemDb->GetFund(info,currency);
	if (fund == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"资金表无此资金账号[%s]",txacc);
		return false;
	}
	return m_pKernelPub.UpdateFund(info,fund,fundflag,fundcode,amt,isadd,true);
}

void RiskDll::MsgCustRiskCalc(int lockrowid)
{
	CBF_PMutex pp(&(m_pMemDb->m_pSurplusLockRowIdLock));
	m_pMemDb->m_pSurplusLockRowId.Add(lockrowid);
}



bool RiskDll::DeleteMoneyIO(CTbl_Char_Key &custlist,char *curdate)
{
	char *stxacc = custlist.First();
	int ret=0;
	int delnum=0;
	while (stxacc != NULL)
	{
		ret = m_pMemDb->m_pDbTrader.m_pFundFlow.Delete(stxacc,curdate);
		delnum+=ret;
		stxacc = custlist.Next();
	}
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"删除 日期[%s] 出入金 [%d]条",curdate,delnum);
	return true;
}



int RiskDll::Order(S_TRADE_DATA &data)
{
	if (!m_pXdp->FromBuffer(data.pData->sBuffer,data.pData->sDBHead.nLen,m_sErrMsg))
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"请求非XDP数据 [%s]",m_sErrMsg);
		data.pData->sDBHead.nLen = 0;
		data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_INPUT;
		AnsData(data);
		return -1;
	}
	//101	varchar2	席位代码	6	n	没有席位默认’$’
	//102	varchar3	交易账号	20	Y	资金账号
	//108	varchar9	客户端流水	18	y	请求方的流水，自己保证当天唯一，回报对应
	//20	uint1	合约编号	9	N	合约编号和合约代码必须输入一个，合约编号输入了代码就不起作用。
	//100	varchar1	合约代码	20	n	
	//5	short1	投保标志	1	Y	asc
	//6	short2	开平标志	1	Y	asc
	//7	short3	买卖标志	1	Y	asc
	//8	short4	交易类型	1	Y	asc
	//9	short5	指令类型	1	Y	报单类型 asc
	//13	short9	报单属性	1	Y	成交属性 asc
	//10	short6	强平标志	1	n	标识会员发起强平 asc
	//41	int2	委托数量	8	Y	
	//60	double1	委托价格	18.4	Y	

	//92	char3	操作员代码	16	Y	交易员/客户号
	//91	char2	渠道	2	Y	

	S_MDB_ORDER_INFO order;
	bzero(&order,sizeof(S_MDB_ORDER_INFO));
	unsigned int len;
	int nRet;
	char sChannel[11];
	char sTxOrg[21];
	bzero(sChannel,sizeof(sChannel));
	bzero(sTxOrg,sizeof(sTxOrg));
	int fieldtype;
	short nShortValue;
	S_MDB_CONTRACT_INFO *pc=NULL;
	S_MDB_QUOTATION *pq=NULL;
	S_MDB_EXCH_INFO *pe = NULL;
	S_MDB_CONTRACT_SETTLE_PRICE *psp=NULL;
	S_MDB_VARIETY_INFO *pv=NULL;
	S_MDB_CUST_FUNDACC_INFO *pCustFuncAcc=NULL;
	S_MDB_CUSTINFO *pCust=NULL;
	S_MDB_CUST_FUND *pCustFund=NULL;
	S_MDB_ORG_INFO  *pOrg = NULL;
	S_MDB_TXACC_LOCK *pAcclock=NULL;
	S_MDB_ORDER_INFO *pOrder=NULL;
	bool iscust=false;

	len = sizeof(order.sSeatNo);
	if (!m_pXdp->GetFieldValue((unsigned short)101,order.sSeatNo,len,fieldtype,m_sErrMsg))
	{
		strcpy(order.sSeatNo,"$");
	}
	len  = sizeof(order.sTxAcc);
	if (!m_pXdp->GetFieldValue((unsigned short)102,order.sTxAcc,len,fieldtype,m_sErrMsg))
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"Input error [%s]",m_sErrMsg);
		data.pData->sDBHead.nLen = 0;
		data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_INPUT;
		AnsData(data);
		return -1;
	}
	len = sizeof(order.sClientSerial);
	if (!m_pXdp->GetFieldValue((unsigned short)108,order.sClientSerial,len,fieldtype,m_sErrMsg))
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"Input error [%s]",m_sErrMsg);
		data.pData->sDBHead.nLen = 0;
		data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_INPUT;
		AnsData(data);
		return -1;
	}
	if (!m_pXdp->GetFieldValue((unsigned short)20,order.nContractNo,m_sErrMsg))
	{
		len = sizeof(order.sContractCode);
		if (!m_pXdp->GetFieldValue((unsigned short)100,order.sContractCode,len,fieldtype,m_sErrMsg))
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"Input error [%s]",m_sErrMsg);
			data.pData->sDBHead.nLen = 0;
			data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_INPUT;
			AnsData(data);
			return -1;
		}
		pc = m_pMemDb->m_pDbBaseinfo.m_pContract.Select(order.sContractCode);
		if (pc == NULL)
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"合约代码 [%s] 不存在",order.sContractCode);
			data.pData->sDBHead.nLen = 0;
			data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_CONTRACTINPUT;
			AnsData(data);
			return -1;
		}
		order.nContractNo = pc->nContractId;
	}
	else
	{
		pc = m_pMemDb->m_pDbBaseinfo.m_pContract.Select(order.nContractNo);
		if (pc == NULL)
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"合约编号 [%d] 不存在",order.nContractNo);
			data.pData->sDBHead.nLen = 0;
			data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_CONTRACTINPUT;
			AnsData(data);
			return -1;
		}
		strcpy(order.sContractCode,pc->sContractCode);
	}
	strcpy(order.sExchCode,pc->sExcode);
	//取交易所信息
	pe = m_pMemDb->m_pDbBaseinfo.m_pExchange.Select(pc->sExcode);
	if (pe == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"合约编号[%d %s] 的交易所[%s]不存在",order.nContractNo,order.sContractCode,pc->sExcode);
		data.pData->sDBHead.nLen = 0;
		data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_EXCHANGE;
		AnsData(data);
		return -1;
	}
	if (!m_pXdp->GetFieldValue((unsigned short)5,nShortValue,m_sErrMsg))
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托无投保标志 %s",m_sErrMsg);
		data.pData->sDBHead.nLen = 0;
		data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_SHFLAG;
		AnsData(data);
		return -1;
	}
	if (nShortValue <MINASCIINUM || nShortValue>MAXASCIINUM)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托投保标志不符 %d",nShortValue);
		data.pData->sDBHead.nLen = 0;
		data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_SHFLAG;
		AnsData(data);
		return -1;
	}
	order.sShFlag[0] = nShortValue;
	if (!m_pXdp->GetFieldValue((unsigned short)6,nShortValue,m_sErrMsg))
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托无开平标志 %s",m_sErrMsg);
		data.pData->sDBHead.nLen = 0;
		data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_EOFLAG;
		AnsData(data);
		return -1;
	}
	if (nShortValue <MINASCIINUM || nShortValue>MAXASCIINUM)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托开平标志不符 %d",nShortValue);
		data.pData->sDBHead.nLen = 0;
		data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_EOFLAG;
		AnsData(data);
		return -1;
	}
	order.sEoFlag[0] = nShortValue;

	if (!m_pXdp->GetFieldValue((unsigned short)7,nShortValue,m_sErrMsg))
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托无买卖标志 %s",m_sErrMsg);
		data.pData->sDBHead.nLen = 0;
		data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_BSFLAG;
		AnsData(data);
		return -1;
	}
	if (nShortValue <MINASCIINUM || nShortValue>MAXASCIINUM)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托买卖标志不符 %d",nShortValue);
		data.pData->sDBHead.nLen = 0;
		data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_BSFLAG;
		AnsData(data);
		return -1;
	}
	order.sBsFlag[0] = nShortValue;

	if (!m_pXdp->GetFieldValue((unsigned short)8,nShortValue,m_sErrMsg))
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托无交易类型 %s",m_sErrMsg);
		data.pData->sDBHead.nLen = 0;
		data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_BSFLAG;
		AnsData(data);
		return -1;
	}
	if (nShortValue <MINASCIINUM || nShortValue>MAXASCIINUM)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托交易类型不符 %d",nShortValue);
		data.pData->sDBHead.nLen = 0;
		data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_TXTYPE;
		AnsData(data);
		return -1;
	}
	order.sTxType[0] = nShortValue;

	if (!m_pXdp->GetFieldValue((unsigned short)9,nShortValue,m_sErrMsg))
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托无报单类型 %s",m_sErrMsg);
		data.pData->sDBHead.nLen = 0;
		data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_ORDERTYPE;
		AnsData(data);
		return -1;
	}
	if (nShortValue <MINASCIINUM || nShortValue>MAXASCIINUM)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托报单类型不符 %d",nShortValue);
		data.pData->sDBHead.nLen = 0;
		data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_ORDERTYPE;
		AnsData(data);
		return -1;
	}
	order.sCmdType[0] = nShortValue;

	if (!m_pXdp->GetFieldValue((unsigned short)13,nShortValue,m_sErrMsg))
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托无报单属性 %s",m_sErrMsg);
		data.pData->sDBHead.nLen = 0;
		data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_ORDERATTR;
		AnsData(data);
		return -1;
	}
	if (nShortValue <MINASCIINUM || nShortValue>MAXASCIINUM)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托报单属性不符 %d",nShortValue);
		data.pData->sDBHead.nLen = 0;
		data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_ORDERATTR;
		AnsData(data);
		return -1;
	}
	order.sCmdAttr[0] = nShortValue;

	order.nRemainNum = order.nOrderNum;//剩余数量默认为委托数量
	order.sCancelFlag[0] = BOOL_FALSE;
	order.sCancelFlag[1] =0;

	//强平标志
	if (m_pXdp->GetFieldValue((unsigned short)10,nShortValue,m_sErrMsg))
	{
		if (nShortValue != BOOL_ASC_FALSE && nShortValue != BOOL_ASC_TRUE)
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托强平标志不符 %d",nShortValue);
			data.pData->sDBHead.nLen = 0;
			data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_BSFLAG;
			AnsData(data);
			return -1;
		}
		order.sForceFlag[0] = nShortValue;
	}
	else
	{
		order.sForceFlag[0] = '0';
	}

	//委托数量
	if (!m_pXdp->GetFieldValue((unsigned short)41,order.nOrderNum,m_sErrMsg))
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托无委托数量 %s",m_sErrMsg);
		data.pData->sDBHead.nLen = 0;
		data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_BSFLAG;
		AnsData(data);
		return -1;
	}
	if (order.nOrderNum<1)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托委托数量不符 %d",order.nOrderNum);
		data.pData->sDBHead.nLen = 0;
		data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_BSFLAG;
		AnsData(data);
		return -1;
	}
	//委托价格
	
	//如果是市价委托，取合约的最新价，以便计算手续费、保证金
	if (order.sCmdType[0] == ORDER_TYPE_MARKET || order.sCmdType[0] == ORDER_TYPE_MS_LOSS ||  order.sCmdType[0] == ORDER_TYPE_MS_PROFIT || order.sCmdType[0] == ORDER_TYPE_LEVEL1_M || order.sCmdType[0] == ORDER_TYPE_LEVEL5_M)
	{
		pq = m_pMemDb->m_pDbTrader.m_pQuotation.Select(order.nContractNo);
		if (pq == NULL)
		{
			//行情没有，取结算价
			psp = m_pMemDb->m_pDbBaseinfo.m_pSettlePrice.Select(order.nContractNo);
			if (psp == NULL)
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托  市价无行情且取不到此合约[%d %s]的结算价 %s",order.nContractNo,order.sContractCode);
				data.pData->sDBHead.nLen = 0;
				data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_ORDERPRICE;
				AnsData(data);
				return -1;
			}
			order.dOrderPrice = psp->dSettlePrice;
		}
		else
		{
			order.dOrderPrice = pq->dNewPrice;
		}
	}
	else
	{
		if (!m_pXdp->GetFieldValue((unsigned short)60,order.dOrderPrice,m_sErrMsg))
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托无委托价格 %s",m_sErrMsg);
			data.pData->sDBHead.nLen = 0;
			data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_ORDERPRICE;
			AnsData(data);
			return -1;
		}
	}
	//发起渠道
	len  = sizeof(sChannel);
	if (!m_pXdp->GetFieldValue((unsigned short)91,sChannel,len,fieldtype,m_sErrMsg))
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托 发起渠道未输入 [%s]",m_sErrMsg);
		data.pData->sDBHead.nLen = 0;
		data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_CHANNEL;
		AnsData(data);
		return -1;
	}
	strcpy(order.sOrderChannel,sChannel);
	//客户、交易员
	len  = sizeof(order.sOrderOper);
	if (!m_pXdp->GetFieldValue((unsigned short)92,order.sOrderOper,len,fieldtype,m_sErrMsg))
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托 发起客户/交易员未输入 [%s]",m_sErrMsg);
		data.pData->sDBHead.nLen = 0;
		data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_CUSTOPER;
		AnsData(data);
		return -1;
	}
	len  = sizeof(sTxOrg);
	if (!m_pXdp->GetFieldValue((unsigned short)93,sTxOrg,len,fieldtype,m_sErrMsg))
	{
		m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"委托 交易网点未输入，取资金账户开户网点");
	}
	//取品种
	if (pc->sProd_type[0] == PROD_TYPE_OPTION)
	{
		S_MDB_CONTRACT_INFO *pco = m_pMemDb->m_pDbBaseinfo.m_pContract.Select(pc->nProdId);
		if (pco != NULL)
		{
			pv = m_pMemDb->m_pDbBaseinfo.m_pVariety.Select(pco->nProdId);
		}
		else
		{
			pv = NULL;
		}
	}
	else
	{
		pv = m_pMemDb->m_pDbBaseinfo.m_pVariety.Select(pc->nProdId);
	}
	order.nVarietyId = pc->nProdId;
	if (pv == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托 合约[%d %s] 取品种信息出错",pc->nContractId,pc->sContractCode);
		data.pData->sDBHead.nLen = 0;
		data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_SYSTEMPARA;
		AnsData(data);
		return -1;
	}
    //是否有此资金账号
	pAcclock = m_pMemDb->SelectLock(order.sTxAcc);
	if (pAcclock == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托 资金账号[%s]非法不存在",order.sTxAcc);
		data.pData->sDBHead.nLen = 0;
		data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_CUSTFUNDACC;
		AnsData(data);
		return -1;
	}
	pCustFuncAcc = m_pMemDb->m_pDbCust.m_pCustFundAcc.At(pAcclock->nFundAccRowid);
//	pCustFuncAcc  = m_pMemDb->m_pDbCust.m_pCustFundAcc.SelectByAcc(order.sTxAcc);
	if (pCustFuncAcc == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托 资金账号[%s] 非法，不存在",order.sTxAcc);
		data.pData->sDBHead.nLen = 0;
		data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_CUSTFUNDACC;
		AnsData(data);
		return -1;
	}
	pCust =  m_pMemDb->m_pDbCust.m_pCustom.At(pAcclock->nCustRowid);
//	pCust = m_pMemDb->m_pDbCust.m_pCustom.SelectByNo(pCustFuncAcc->sCustNo);
	if (pCust == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托 资金账号[%s]的客户号[%s]不存在",order.sTxAcc,pCustFuncAcc->sCustNo);
		data.pData->sDBHead.nLen = 0;
		data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_CUSTNO;
		AnsData(data);
		return -1;
	}
	//比较资金账号对应的客户号是否和输入的相同
	if (strcmp(pCustFuncAcc->sCustNo,order.sOrderOper) == 0)
	{
		iscust = true;
	}
	else
	{
	    iscust= false;//不是客户发起
	}
	//取资金表，按账号+币种
	pCustFund = m_pMemDb->GetFund(pAcclock,pc->sCurrency);
//	pCustFund = m_pMemDb->m_pDbTrader.m_pFund.Select(order.sTxAcc,pc->sCurrency);
	if (pCustFund == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托 资金账号[%s]+合约[%d %s]对应的币种[%s]在资金表不存在",order.sTxAcc,order.nContractNo,order.sContractCode,pc->sCurrency);
		data.pData->sDBHead.nLen = 0;
		data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_CUSTFUND;
		AnsData(data);
		return -1;
	}
	//判断开通渠道
	int nChannel= atoi(order.sOrderChannel);
	if (nChannel<=0 || nChannel>59)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托渠道[%s]非法",order.sOrderChannel);
		data.pData->sDBHead.nLen = 0;
		data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_CHANNEL;
		AnsData(data);
		return -1;
	}
	if (pCust->sChannleOpen[nChannel] != '1')
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托 客户号[%s]资金账号[%s]未开通此渠道[%s]",pCustFuncAcc->sCustNo,order.sTxAcc,order.sOrderChannel);
		data.pData->sDBHead.nLen = 0;
		data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_CHANNELOPEN;
		AnsData(data);
		return -1;
	}
	if (!iscust) //判断操作员是否有此账号权限
	{
		if (!m_pKernelPub.CheckOperAccRight(order.sOrderOper,order.sTxAcc))
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托 操作员[%s] 无资金账号[%s] 的操作权限",order.sOrderOper,order.sTxAcc);
			data.pData->sDBHead.nLen = 0;
			data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_NO_RIGHT;
			AnsData(data);
			return -1;
		}
	}
	strcpy(order.sTxDate,pe->sTradeDate);
	m_pDate.Update();
	strcpy(order.sAnsTime,m_pDate.ToStringDT17().c_str());
	strcpy(order.sAccOrg,pCustFuncAcc->sSignOrg);
	//取交易网点
	if (strlen(sTxOrg)<1)
	{
		strcpy(order.sTxOrg,pCustFuncAcc->sSignOrg);
	}
	else
	{
		strcpy(order.sTxOrg,sTxOrg);	
	}
	//交易网点是否存在
	pOrg = m_pMemDb->m_pDbPublic.m_pOrg.Select(order.sTxOrg);
	if (pOrg == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托 网点[%s]不存在",order.sTxOrg);
		data.pData->sDBHead.nLen = 0;
		data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_TXORG;
		AnsData(data);
		return -1;
	}
	//判断委托类型、成交属性
	int nTxCmd = atoi(order.sCmdType);
	if (nTxCmd<0 || nTxCmd>11)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托 报单类型[%s]非法",order.sCmdType);
		data.pData->sDBHead.nLen = 0;
		data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_ORDERTYPE;
		AnsData(data);
		return -1;
	}
	int nTxAttr = atoi(order.sCmdAttr);
	if (nTxAttr<0 || nTxAttr>99)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托 成交属性[%s]非法",order.sCmdAttr);
		data.pData->sDBHead.nLen = 0;
		data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_ORDERATTR;
		AnsData(data);
		return -1;
	}
	if (pc->sOrderCmd[nTxCmd] != '1')
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托 报单类型[%s]此合约[%d %s]不支持",order.sCmdType,order.nContractNo,order.sContractCode);
		data.pData->sDBHead.nLen = 0;
		data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_ORDERTYPE_SUPPORT;
		AnsData(data);
		return -1;
	}
	if (pc->sDoneAttr[nTxAttr*10+nTxAttr] != '1')
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托 成交属性+报单位类型[%s %s]此合约[%d %s]不支持",order.sCmdAttr,order.sCmdType,order.nContractNo,order.sContractCode);
		data.pData->sDBHead.nLen = 0;
		data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_ORDERATTR_SUPPORT;
		AnsData(data);
		return -1;
	}
	
	//取交易编码
	S_MDB_CUST_TRADECODE *ptx = m_pMemDb->m_pDbTrader.m_pTradecode.Select(order.sTxAcc,order.sExchCode,order.sSeatNo);
	if (ptx == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托 资金账号[%s]交易所代码[%s]席位号[%s]无对应的交易编码",order.sTxAcc,order.sExchCode,order.sSeatNo);
		data.pData->sDBHead.nLen = 0;
		data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_TRADECODE;
		AnsData(data);
		return -1;
	}
	strcpy(order.sTxCode,ptx->sTradeCode);
	

	//这里可以根据网点，判断是否可以交易，后续根据需要增加

	//客户风险度，是否有交易权限
	if (pCustFund->sTradeRight[0] == TR_NONE)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托 资金账号[%s] 币种[%s]交易所代码[%s]席位号[%s]无交易权限[%c] 风险度[%.2f] ",order.sTxAcc,order.sExchCode,order.sSeatNo,pCustFund->sCurrency,pCustFund->sTradeRight[0],pCustFund->dRisk);
		data.pData->sDBHead.nLen = 0;
		data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_TRADERIGHT;
		AnsData(data);
		return -1;
	}

	order.dAmt = order.dOrderPrice*pc->dTxUnit*order.nOrderNum;//金额

	//生成本地报单号
	sprintf(order.sLocalSerial,INT64PFM,g_pPubData->GetLocalSerial());

	order.nSyncFlag = 1; //报入同步

	//报单状态默认为正申报
	order.sOrderStatus[0] = ORDER_STATUS_SENDOK;//正在申报
	order.sOrderStatus[1]=0;

	//算出保证金手续费
	double dExchFee=0.0;//交易所手续费
	double dMemberFee=0.0;//会员手续费
	double dExchMargin=0.0;//交易所保证金
	double dMemberMargin=0.0;//会员保证金
	int orderrid;
	m_pSendXdp.XdpCopy(m_pXdp);
	//设置xdp报文
	// 101	varchar2	席位代码	6	y	没有席位默认’$’
	//	102	varchar3	交易账号	20	Y	
	//	103	Varchar4	本地报单号	20	Y	
	//	104	Varchar5	交易编码	20	y	

	//	20	uint1	合约编号	9	y	
	//	100	varchar1	合约代码	20	y	
	//	5	short1	投保标志	1	Y	asc
	//	6	short2	开平标志	1	Y	asc
	//	7	short3	买卖标志	1	Y	asc
	//	8	short4	交易类型	1	Y	asc
	//	9	short5	指令类型	1	Y	报单类型 asc
	//	13	short9	报单属性	1	Y	成交属性 asc
	//	10	short6	强平标志	1	n	标识会员发起强平 asc
	//	41	int2	委托数量	8	Y	
	//	60	double1	委托价格	18.4	Y	

	//	92	char3	操作员代码	16	Y	交易员/客户号
	//	91	char2	渠道	2	Y	

	//  61	Double2	金额	18.4	y	
	//	63	Double4	手续费	18.4	y	
	//	62	Double3	冻结资金	18.4	y	不含手续费
	//	22	Uint2	品种编号	8	y	
	//  109	varchar10	委托状态	1	y	
	//	117	Varchar18	报单响应	100	y	
	//	116	Varchar17	报单时间	17	y	

	m_pSendXdp.SetFieldValue((unsigned short)101,order.sSeatNo,strlen(order.sSeatNo),m_sErrMsg);
	m_pSendXdp.SetFieldValue((unsigned short)103,order.sLocalSerial,strlen(order.sLocalSerial),m_sErrMsg);
	m_pSendXdp.SetFieldValue((unsigned short)104,order.sTxCode,strlen(order.sTxCode),m_sErrMsg);
	m_pSendXdp.SetFieldValue((unsigned short)20,order.nContractNo,m_sErrMsg);
	m_pSendXdp.SetFieldValue((unsigned short)100,order.sContractCode,strlen(order.sContractCode),m_sErrMsg);
	m_pSendXdp.SetFieldValue((unsigned short)10,(short)order.sForceFlag[0],m_sErrMsg);
	m_pSendXdp.SetFieldValue((unsigned short)60,order.dOrderPrice,m_sErrMsg);
	m_pSendXdp.SetFieldValue((unsigned short)61,order.dAmt,m_sErrMsg);
	m_pSendXdp.SetFieldValue((unsigned short)109,order.sOrderStatus,strlen(order.sOrderStatus),m_sErrMsg);
	m_pSendXdp.SetFieldValue((unsigned short)117,order.sOrderRsp,strlen(order.sOrderRsp),m_sErrMsg);
	m_pSendXdp.SetFieldValue((unsigned short)116,order.sAnsTime,strlen(order.sAnsTime),m_sErrMsg);

	m_pSendXdp.SetFieldValue((unsigned short)22,order.nVarietyId,m_sErrMsg);
	nRet = m_pKernelPub.GetFeeMargin(&order,pc,pCustFuncAcc,dExchFee,dMemberFee,dExchMargin,dMemberMargin);
	if (nRet != 0)
	{
		order.sOrderStatus[0] = ORDER_STATUS_INVALID;//无效单
		order.sOrderStatus[1]=0;
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"资金账号[%s]交易所代码[%s]席位号[%s]本地报单号[%s]取手续费保证金出错 [%d]",order.sTxAcc,order.sExchCode,order.sSeatNo,order.sLocalSerial, nRet);
		switch (nRet)
		{
			case -1:
				strcpy(order.sOrderRsp,"取手续费出错");
				data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_GETFEE;
				break;
			case -2:
				strcpy(order.sOrderRsp,"取保证金出错");
				data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_GETMARGIN;
				break;
			case -3:
				strcpy(order.sOrderRsp,"合约对应的品种类型不符");
				data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_SYSTEMPARA;
				break;
			default:
				break;
		}
		data.pData->sDBHead.nLen = 0;
		AnsData(data);
		//同步到备机
		m_pSendXdp.SetFieldValue((unsigned short)109,order.sOrderStatus,strlen(order.sOrderStatus),m_sErrMsg);
		m_pSendXdp.SetFieldValue((unsigned short)117,order.sOrderRsp,strlen(order.sOrderRsp),m_sErrMsg);
		if (SendBus(&m_pSendXdp,0,9848,0,CMD_DPABC) !=0 )
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"发送报单同步 9848出错 本地报单号[%s]",order.sLocalSerial);
		}
		//写入报单表
		orderrid = m_pMemDb->m_pDbTrader.m_pOrder.Insert(order);
		if (orderrid <0)
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托 写委托表出错 本地报单号[%s]",order.sLocalSerial);
			return -1;
		}
		pOrder = m_pMemDb->m_pDbTrader.m_pOrder.At(orderrid);
		if (pOrder == NULL)
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托 委托表出错 rowid[%d]找不到委托 本地报单号[%s]",orderrid,order.sLocalSerial);
			return -1;
		}
		return -1;
	}
	order.dExFee = 0.0 - (dExchFee+dMemberFee);//总手续费 为负数
	order.dFrozAmt = dExchMargin+dMemberMargin;//总保证金

	//发送报单同步广播
	
	m_pSendXdp.SetFieldValue((unsigned short)63,order.dExFee,m_sErrMsg);
	m_pSendXdp.SetFieldValue((unsigned short)62,order.dFrozAmt,m_sErrMsg);
	
	if (SendBus(&m_pSendXdp,0,9848,0,CMD_DPABC) !=0 )
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"发送报单同步 9848出错 本地报单号[%s]",order.sLocalSerial);
	}

	//写入报单表
	orderrid = m_pMemDb->m_pDbTrader.m_pOrder.Insert(order);
	if (orderrid <0)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托 写委托表出错 本地报单号[%s]",order.sLocalSerial);
		data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_SYSTEM;
		data.pData->sDBHead.nLen = 0;
		AnsData(data);
		return -1;
	}
	pOrder = m_pMemDb->m_pDbTrader.m_pOrder.At(orderrid);
	if (pOrder == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托  委托表出错 rowid[%d]找不到委托",orderrid);
		data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_SYSTEM;
		data.pData->sDBHead.nLen = 0;
		AnsData(data);
		return -1;
	}
	//发送报单给柜台或交易所
	S_MDB_BROKERAGE_INFO *pbrok = m_pMemDb->m_pDbSet.m_pBrokerage.Select(pCustFuncAcc->nSysId);
	if (pbrok == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托 资金账号无所属经纪公司[%d]",pCustFuncAcc->nSysId);
		data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_SYSTEM;
		data.pData->sDBHead.nLen = 0;
		AnsData(data);
		pOrder->sOrderStatus[0] = ORDER_STATUS_INVALID;//无效单
		pOrder->sOrderStatus[1]=0;
		strcpy(pOrder->sOrderRsp,"资金账号无所属经纪公司");
		pOrder->nSyncFlag = 5; //回报同步
		return -1;
	}
	int drebsvrid;
	//0 不清算，导入柜台数据
	//1 清算       直接连交易所
	//	2 不清算   但连柜台的多个PG
	//	3 不清算       直接连交易所
	if (pbrok->nClearFlag == 0) //对接的是柜台
	{
		drebsvrid = pbrok->nSvrId;
	}
	else if (pbrok->nClearFlag == 2) //对接的是柜台的多个PG
	{
		CBF_Slist slist;
		slist.SetSeparateString("-");
		S_MDB_BROKERAGE_PG *pginfo =NULL;
		//brokerid-客户号
		int nRet = slist.FillSepString(pCustFuncAcc->sCustSimpleName);
		if (nRet !=2 )
		{
			pginfo = m_pMemDb->m_pDbSet.m_pBrokeragePg.Select(pCustFuncAcc->nSysId,pCustFuncAcc->sCustSimpleName);
		}
		else
		{
			pginfo = m_pMemDb->m_pDbSet.m_pBrokeragePg.Select(pCustFuncAcc->nSysId,(char *)slist.GetAt(0).c_str());
		}
		if (pginfo == NULL)
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"配置为连接多个PG，但BROKERAGE_PG表里无pg配置[%d %s]",pCustFuncAcc->nSysId,pCustFuncAcc->sCustSimpleName);
			data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_SYSTEM;
			data.pData->sDBHead.nLen = 0;
			AnsData(data);
			pOrder->sOrderStatus[0] = ORDER_STATUS_INVALID;//无效单
			pOrder->sOrderStatus[1]=0;
			strcpy(pOrder->sOrderRsp,"资金账号对应的pg参数未配置");
			pOrder->nSyncFlag = 5; //回报同步
			return -1;
		}
		drebsvrid = pginfo->nSvrId;
	}
	else //直接对接的交易所
	{

		if (strcmp(pOrder->sSeatNo,"$") == 0)
		{
			std::vector<S_MDB_BROKERAGE_EXCHANGE *> belist;
			//没有输入的度位号，说明只有一个席位
			if (!m_pMemDb->m_pDbSet.m_pBrokerageExchange.Select(pCustFuncAcc->nSysId,pOrder->sExchCode,belist))
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托 资金账号所属经纪公司[%d] 无对应的交易所[%s]信息",pCustFuncAcc->nSysId,pOrder->sExchCode);
				data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_SYSTEM;
				data.pData->sDBHead.nLen = 0;
				AnsData(data);
				pOrder->sOrderStatus[0] = ORDER_STATUS_INVALID;//无效单
				pOrder->sOrderStatus[1]=0;
				strcpy(pOrder->sOrderRsp,"资金账号所属经纪公司无对应的交易所信息");
				pOrder->nSyncFlag = 5; //回报同步
				return -1;
			}
			if (belist.size() != 1)
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托 资金账号所属经纪公司[%d] 对应的交易所[%s]有多条信息",pCustFuncAcc->nSysId,pOrder->sExchCode);
				data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_SYSTEM;
				data.pData->sDBHead.nLen = 0;
				AnsData(data);
				pOrder->sOrderStatus[0] = ORDER_STATUS_INVALID;//无效单
				pOrder->sOrderStatus[1]=0;
				strcpy(pOrder->sOrderRsp,"资金账号所属经纪公司对应多个交易所席位信息");
				pOrder->nSyncFlag = 5; //回报同步
				return -1;
			}
			drebsvrid = belist[0]->nSvrId;
		}
		else
		{
			S_MDB_BROKERAGE_EXCHANGE *pbrokexch = m_pMemDb->m_pDbSet.m_pBrokerageExchange.Select(pCustFuncAcc->nSysId,pOrder->sExchCode,pOrder->sSeatNo);
			if (pbrokexch == NULL)
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托 资金账号所属经纪公司[%d] 交易所[%s] 无席位[%s]信息",pCustFuncAcc->nSysId,pOrder->sExchCode,pOrder->sSeatNo);
				data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_SYSTEM;
				data.pData->sDBHead.nLen = 0;
				AnsData(data);
				pOrder->sOrderStatus[0] = ORDER_STATUS_INVALID;//无效单
				pOrder->sOrderStatus[1]=0;
				strcpy(pOrder->sOrderRsp,"资金账号所属经纪公司无席位信息");
				pOrder->nSyncFlag = 5; //回报同步
				return -1;
			}
			drebsvrid =pbrokexch->nSvrId;
		}

	}
	//冻结资金持仓等等
	nRet = m_pKernelPub.ComputeNewOrder(pAcclock,pCustFund,pOrder,pc,true);
	if (nRet != SUCCESS)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托 计算出错 返回[%d]",nRet);
		data.pData->sDBHead.a_Ainfo.a_nRetCode = nRet;
		data.pData->sDBHead.nLen = 0;
		AnsData(data);
		pOrder->sOrderStatus[0] = ORDER_STATUS_INVALID;//无效单
		pOrder->sOrderStatus[1]=0;
		strcpy(pOrder->sOrderRsp,"委托计算出错");
		pOrder->nSyncFlag = 5; //回报同步
		return -1;
	}
	if (SendBus(&m_pSendXdp,drebsvrid,9206,GetNextSerial(),CMD_DPCALL) !=0 )
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托 发送总线出错");
		data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_SYSTEM;
		data.pData->sDBHead.nLen = 0;
		AnsData(data);
		pOrder->sOrderStatus[0] = ORDER_STATUS_INVALID;//无效单
		pOrder->sOrderStatus[1]=0;
		strcpy(pOrder->sOrderRsp,"发送报单 出错");
		pOrder->nSyncFlag = 5; //回报同步
		return -1;
	}
	nRet = GetAnsData(m_pSendData,5000);
	while (nRet == 0)
	{
		if (m_pSendData.pData == NULL)
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托 取应答，指针为NULL");
			data.pData->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
			data.pData->sDBHead.nLen = 0;
			AnsData(data);
			return -1;
		}
		if (m_pSendData.pData->sDBHead.s_Sinfo.s_nSerial != GetCurSerial())
		{
			m_pClientLink->PoolFree(m_pSendData);
			nRet = GetAnsData(m_pSendData,5000);
			continue;
		}
		if (m_pSendData.pData->sDBHead.a_Ainfo.a_nRetCode == SUCCESS)
		{
			break;
		}
		else
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托 接口返回出错[%d]",m_pSendData.pData->sDBHead.a_Ainfo.a_nRetCode);
			data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_SYSTEM;
			data.pData->sDBHead.nLen = 0;
			AnsData(data);
			pOrder->sOrderStatus[0] = ORDER_STATUS_INVALID;//无效单
			pOrder->sOrderStatus[1]=0;
			strcpy(pOrder->sOrderRsp,"发送报单 返回错");
			pOrder->nSyncFlag = 5; //回报同步
			return -1;
		}
	}
	if (nRet != 0)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托 接口返回超时出错");
		//没有收到应答
	}
	//应答回去
	data.pData->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
	data.pData->sDBHead.nLen = 0;
	AnsData(data);
	
	return 0;
}


int RiskDll::SendBus(CXdp *xdp,int svrid,int funcno,int serial,short cmd)
{
	bzero(&m_pSendData,sizeof(S_TRADE_DATA));
	if (m_pClientLink->PoolMalloc(m_pSendData) !=0 )
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"发送总线 %d 分配内存出错",funcno);
		return -1;
	}
	bzero(&(m_pSendData.pData->sDBHead),DREBHEADLEN);
	m_pSendData.src = 0;
	m_pSendData.index = m_nIndex;
	m_pSendData.pData->sDBHead.cRaflag = 0;
	m_pSendData.pData->sDBHead.cCmd  = cmd;
	m_pSendData.pData->sDBHead.s_Sinfo.s_nSerial = serial;
	m_pSendData.pData->sDBHead.d_Dinfo.d_nSvrMainId = svrid;
	m_pSendData.pData->sDBHead.d_Dinfo.d_nServiceNo = funcno;
	if (Send2Exch(m_pSendData,xdp)!=0)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"发送总线 %d Send2Exch出错",funcno);
		return -1;
	}
	return 0;
}
int RiskDll::GetNextSerial()
{
	m_nCallSerial++;
	if (m_nCallSerial> m_nIndex*1000000+999999)
	{
		m_nCallSerial =  m_nIndex*1000000+1;
	}
	return m_nCallSerial;
}
int RiskDll::GetCurSerial()
{
	return m_nCallSerial;
}



bool RiskDll::ReadContract()
{
	bool bret;
	//合约表
	S_MDB_CONTRACT_INFO info;
	S_MDB_VARIETY_INFO *pv=NULL;
	S_MDB_CONTRACT_INFO *pc=NULL;
	sprintf(m_sSql,"select a.CONTRACT_NO,a.CONTRACT_CODE,a.CONTRACT_NAME,a.EX_CODE,a.PROD_TYPE,a.PROD_CODE,a.PROD_NO,a.MIN_CHG_PRICE,a.IST0,a.ISBAIL,a.BAIL_RATE,a.UL_LIMIT,a.UPPER_LIMIT_VALUE,a.LOWER_LIMIT_VALUE,a.ACTIVE_FLAG,a.TX_STATUS,a.REF_PRICE,a.RCV_RATE,a.BUSI_MODE,a.MAX_HAND,a.MIN_HAND,a.EX_UNIT,a.PRICE_UNIT,a.ORDER_CMD,a.ORDER_ATTR,a.DUE_DATE,a.LAST_DATE,a.DELI_DAYS,a.IS_BIGMARGIN,a.ISCALL,a.VARIETY_NO from CONTRACT_INFO a,VARIETY_INFO b where a.VARIETY_NO=b.VARIETY_NO order by a.CONTRACT_NO");
	try
	{
		if (!m_pSqlCmd->setCommandText(m_sSql))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"setCommandText error <%s>",m_sSql);
			return false;
		}
		if (!m_pSqlCmd->Execute())
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Execute <%s> error %s",m_sSql,m_pSqlCmd->GetCmdErrInfo());
			return false;
		}

		bret = m_pSqlCmd->FetchNext();
		if ( !bret )
		{
#ifdef _ENGLISH_
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Table CONTRACT_INFO no data");
#else
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"合约表无数据");
#endif
			return false;
		}
		while (bret)
		{
			info.nContractId = m_pSqlCmd->Field(0).asLong();
			strcpy(info.sContractCode,m_pSqlCmd->Field(1).asString());
			strcpy(info.sContractName,m_pSqlCmd->Field(2).asString());
			strcpy(info.sExcode,m_pSqlCmd->Field(3).asString());
			strcpy(info.sProd_type,m_pSqlCmd->Field(4).asString());
			strcpy(info.sProd_code,m_pSqlCmd->Field(5).asString());
			info.nProdId = m_pSqlCmd->Field(6).asLong();
			info.dMinChgPrice = m_pSqlCmd->Field(7).asDouble();
			strcpy(info.sIsT0,m_pSqlCmd->Field(8).asString());
			strcpy(info.sIsMargin,m_pSqlCmd->Field(9).asString());
			info.dMarginRate = m_pSqlCmd->Field(10).asDouble();
			strcpy(info.sIsUpDownLimit,m_pSqlCmd->Field(11).asString());
			info.dUpRate = m_pSqlCmd->Field(12).asDouble();
			info.dDownRate = m_pSqlCmd->Field(13).asDouble();
			strcpy(info.sActiveFlag,m_pSqlCmd->Field(14).asString());
			strcpy(info.sStatus,m_pSqlCmd->Field(15).asString());
			info.dRefPrice = m_pSqlCmd->Field(16).asDouble();
			info.dSellGetRate = m_pSqlCmd->Field(17).asDouble();
			strcpy(info.sDeliMode,m_pSqlCmd->Field(18).asString());
			info.nMaxHand = m_pSqlCmd->Field(19).asLong();
			info.nMinHand = m_pSqlCmd->Field(20).asLong();
			info.dTxUnit = m_pSqlCmd->Field(21).asDouble();
			strcpy(info.sOrderUnit,m_pSqlCmd->Field(22).asString());
			strcpy(info.sOrderCmd,m_pSqlCmd->Field(23).asString());
			strcpy(info.sDoneAttr,m_pSqlCmd->Field(24).asString());
			strcpy(info.sDueDate,m_pSqlCmd->Field(25).asString());
			strcpy(info.sLastDate,m_pSqlCmd->Field(26).asString());
			info.nDeliDays = m_pSqlCmd->Field(27).asLong();
			strcpy(info.sIsBigMargin,m_pSqlCmd->Field(28).asString());
			strcpy(info.sIsOptionCall,m_pSqlCmd->Field(29).asString());
			info.nVariety = m_pSqlCmd->Field(30).asLong();

			//去品种表取币种
			switch (info.sProd_type[0])
			{
			case PROD_TYPE_FUTURES:
			case PROD_TYPE_SEC:
			case PROD_TYPE_SPOT:
				pv = m_pMemDb->m_pDbBaseinfo.m_pVariety.Select(info.nProdId);
				if (pv == NULL)
				{
					m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"合约[%d %s]的品种[%d]在品种表不存在",info.nContractId,info.sContractCode,info.nProdId);
					bret = m_pSqlCmd->FetchNext();
					continue;
				}
				strcpy(info.sCurrency,pv->sCurrency);
				break;
			case PROD_TYPE_OPTION:
				pc = m_pMemDb->m_pDbBaseinfo.m_pContract.Select(info.nProdId);
				if (pc == NULL)
				{
					m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"期权合约[%d %s]的标的合约[%d]在合约表不存在",info.nContractId,info.sContractCode,info.nProdId);
					bret = m_pSqlCmd->FetchNext();
					continue;
				}
				pv = m_pMemDb->m_pDbBaseinfo.m_pVariety.Select(pc->nProdId);
				if (pv == NULL)
				{
					m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"期权合约[%d %s]的品种[%d]在品种表不存在",info.nContractId,info.sContractCode,pc->nProdId);
					bret = m_pSqlCmd->FetchNext();
					continue;
				}
				strcpy(info.sCurrency,pv->sCurrency);
				break;
			default:
				info.sCurrency[0]= CURRENCY_CNY;
				info.sCurrency[1]=0;
				break;
			}
			if (m_pMemDb->m_pDbBaseinfo.m_pContract.Insert(info) <0 )
			{
			}
			bret = m_pSqlCmd->FetchNext();
		}
	}
	catch (...)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Database operator Exception!");
#else
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"数据库操作异常");
#endif
		return false;
	}
	return true;
}
