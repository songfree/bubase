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
	InitFuncInfo(9800, (FuncPointer)&RiskDll::ProcSyncFund, "处理外部发来的资金同步信息", 1.0, "wms", 1);
	InitFuncInfo(9801, (FuncPointer)&RiskDll::ProcSyncPosi, "处理外部发来的持仓同步信息", 1.0, "wms", 1);
	InitFuncInfo(9802, (FuncPointer)&RiskDll::ProcBondScope, "处理外部发来的债券存续规模", 1.0, "wms", 1);
	InitFuncInfo(9803, (FuncPointer)&RiskDll::ProcBondValue, "处理外部发来的中证估值信息", 1.0, "wms", 1);
	InitFuncInfo(9804, (FuncPointer)&RiskDll::ProcBondDv01, "处理外部发来的债券DV01信息", 1.0, "wms", 1);

	InitFuncInfo(9808, (FuncPointer)&RiskDll::RcvQuotation, "处理外部发来的行情", 1.0, "wms", 1);

	InitFuncInfo(9901, (FuncPointer)&RiskDll::OrderRiskCheck, "处理外部发来的普通报单风控检查", 1.0, "wms", 1);
	InitFuncInfo(9902, (FuncPointer)&RiskDll::BilateralOrderRiskCheck, "处理外部发来的双边报价风控检查", 1.0, "wms", 1);

	InitFuncInfo(9903, (FuncPointer)&RiskDll::OrderMsg, "处理外部发来的报单通知,不校验资金仓位,默认为已报入且不调用外部接口", 1.0, "wms", 1);
	InitFuncInfo(9904, (FuncPointer)&RiskDll::DoneRtn, "处理外部发来的成交回报，计算资金持仓", 1.0, "wms", 1);
	InitFuncInfo(9905, (FuncPointer)&RiskDll::CancelOrderRtn, "外理外部发来的撤单回报消息，处理撤单后的资金持仓", 1.0, "wms", 1);
																			 
	//InitFuncInfo(9906, (FuncPointer)&RiskDll::BilateralOrderMsg, "外理外部发来的做市双边报价", 1.0, "wms", 1);
	//InitFuncInfo(9907, (FuncPointer)&RiskDll::BilateralDoneMsg, "外理外部发来的做市双边报价的成交", 1.0, "wms", 1);


	InitFuncInfo(9920, (FuncPointer)&RiskDll::Order, "接收外部的报单，进行资金仓位校验及处理，默认为未报，通过后调用外部的报单接口", 1.0, "wms", 1);
	InitFuncInfo(9921, (FuncPointer)&RiskDll::BilateralOrder, "接收外部的双边报价，调用外部的双边报价接口", 1.0, "wms", 1);
	InitFuncInfo(9922, (FuncPointer)&RiskDll::CancelOrder, "接收外部的撤单请求，调用外部的撤单接口", 1.0, "wms", 1);
	InitFuncInfo(9923, (FuncPointer)&RiskDll::OrderRtn, "对应Order的报单，收到外部系统的报单回报，进行回报的处理", 1.0, "wms", 1);



	InitFuncInfo(9926, (FuncPointer)&RiskDll::OrderSync, "接收主机发布的报单广播，同步至本备机，本身就含交易核心", 1.0, "wms", 1);

	InitFuncInfo(9999, (FuncPointer)&RiskDll::ComputeRisk, "定时任务，计算风险", 1.0, "wms", 1);
	InitFuncInfo(9998, (FuncPointer)&RiskDll::ProcessDoneNoOrder, "处理未找到报单的成交", 1.0, "wms", 1);
	InitFuncInfo(9997, (FuncPointer)&RiskDll::Sync2Db, "同步报单及成交至数据库", 1.0, "wms", 1);

	InitFuncInfo(9701, (FuncPointer)&RiskDll::ImportSecu, "导入合约信息", 1.0, "wms", 1);

	InitFuncInfo(1021, (FuncPointer)&RiskDll::test1021, "测试rpc", 1.0, "wms", 1);
	InitFuncInfo(1023, (FuncPointer)&RiskDll::test1023, "测试rpc", 1.0, "wms", 1);
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
			if (!m_pXdp->FromBuffer(ansdata.pData->sBuffer,ansdata.pData->sDBHead.nLen,m_sErrMsg))
			{
				m_pLog->LogMp(LOG_DEBUG, __FILE__, __LINE__, "FromBuffer出错");
				m_pClientLink->PoolFree(ansdata);
				return -1;
			}
			m_pClientLink->PoolFree(ansdata);
			//处理
			
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
	m_pXdp->ResetData();
	char tmpchar[500];
	sprintf(tmpchar, "test1023应答 %d", data.pData->sDBHead.s_Sinfo.s_nSerial);
	m_pXdp->SetFieldValue((unsigned short)80, tmpchar,strlen(tmpchar),m_sErrMsg);
	unsigned int len = sizeof(data.pData->sBuffer);
	if (!m_pXdp->ToBuffer(data.pData->sBuffer, len,m_sErrMsg))
	{
		sprintf(tmpchar, "xdp ToBuffer出错 ");
		strcpy(data.pData->sBuffer,tmpchar);
		len = strlen(data.pData->sBuffer);
	}
	data.pData->sDBHead.nLen = len;
	m_pClientLink->AnsData(data,0);
	return 0;
}

RiskDll::~RiskDll()
{

}
bool RiskDll::Init(CBF_Xml *xml,CIClientLink *pBusLink,CMemDb *pMemDb,	CXdp *pXdp,	CIErrlog *pLog,CDbApi *pSqlCmd,CIExchLink *pExchLink,int index)
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
	m_nIndex = index;
	m_pKernelPub.m_pMemDb = pMemDb;
	m_pKernelPub.m_pLog = pLog;
	m_nCallSerial = m_nIndex*1000000+1;
	m_pPubDb.m_pLog = pLog;
	m_pPubDb.m_pSqlCmd = m_pSqlCmd;
    m_pPubMethod.m_pLog = pLog;
	m_pPubMethod.m_pSqlCmd = m_pSqlCmd;
	return true;
}


int RiskDll::RcvQuotation(S_TRADE_DATA& data)
{
    if (!m_pXdp->FromBuffer(data.pData->sBuffer, data.pData->sDBHead.nLen, m_sErrMsg))
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "RcvQuotation Xdp FromBuffer error <%s>", m_sErrMsg);
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
    bzero(&qinfo, sizeof(S_MDB_QUOTATION));
	Xdp2Struct::ConvertQuotation(m_pXdp,qinfo);
    

    S_MDB_CONTRACT_INFO* pc = m_pMemDb->m_pDbBaseinfo.m_pContract.Select(qinfo.sContractCode);
    if (pc == NULL)
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "合约[%d %s]在合约表中不存在", qinfo.nContractNo, qinfo.sContractCode);
        return -1;
    }
	//行情序号是不是有值
	if (strlen(qinfo.sQuoSerial)<1)
	{
		//使用行情时间作为行情序号
		char tmpchar[50];
		bzero(tmpchar,sizeof(tmpchar));
		strncpy(tmpchar,qinfo.sTime,8);
		strcpy(tmpchar+8, qinfo.sTime+9);
		CBF_Tools::StringCopy(qinfo.sQuoSerial,sizeof(qinfo.sQuoSerial)-1,tmpchar);
	}
    qinfo.nContractNo = pc->nContractId;
    bool bret = m_pMemDb->m_pDbTrader.m_pQuotation.Update(qinfo);
    if (bret)
    {
        m_pMemDb->m_bIsNewQuotation = true;
        if (qinfo.dSettlePrice > MAXDOUBLECOMPARE)
        {
            //结算价有值，更新到数据库表里面
            UpdateSetllePrice(qinfo);
        }
        if (qinfo.dNewPrice > 99999999 || qinfo.dNewPrice < -99999999) //错误行情
        {
            return 0;
        }
        //查看是否是风险合约
        //处理是否风险合约
        m_pLog->LogMp(LOG_DEBUG, __FILE__, __LINE__, "开始计算风险合约[%d %s] 行情[%s %s]", qinfo.nContractNo, qinfo.sContractCode, qinfo.sDate, qinfo.sQuoSerial);
        ProcRiskContract(qinfo, pc);
        //盈亏计算 风险度计算
        m_pLog->LogMp(LOG_DEBUG, __FILE__, __LINE__, "开始计算盈亏 合约[%d %s] 行情[%s %s]", qinfo.nContractNo, qinfo.sContractCode, qinfo.sDate, qinfo.sQuoSerial);
        ProcRisk(qinfo, pc);

        //针对LME 3M合约，有到期日的合约按3M合约最新价计算盈亏
        if (strcmp(pc->sContractCode, "LME+CA+R+3M") == 0)
        {
            m_pDate.Update();
            char scurdate[9];
            m_pDate.ToCharDate(scurdate);
            std::vector<S_MDB_CONTRACT_INFO*> contlist;
            if (m_pMemDb->m_pDbBaseinfo.m_pContract.SelectByExch((char*)"B", contlist))
            {
                for (int k = 0; k < contlist.size(); k++)
                {
                    pc = contlist[k];
                    if (strncmp(pc->sContractCode, "LME+CA+S+", 9) == 0 && strncmp(pc->sContractCode + 9, scurdate, 8) >= 0) //3m对应的合约
                    {
                        //进行风险计算
                        qinfo.nContractNo = pc->nContractId;
                        strcpy(qinfo.sContractCode, pc->sContractCode);
                        strcpy(qinfo.sContractName, pc->sContractName);
                        //处理是否风险合约
                        m_pLog->LogMp(LOG_DEBUG, __FILE__, __LINE__, "开始计算风险合约[%d %s] 行情[%s %s]", qinfo.nContractNo, qinfo.sContractCode, qinfo.sDate, qinfo.sQuoSerial);
                        ProcRiskContract(qinfo, pc);
                        //盈亏计算 风险度计算
                        m_pLog->LogMp(LOG_DEBUG, __FILE__, __LINE__, "开始计算盈亏 合约[%d %s] 行情[%s %s]", qinfo.nContractNo, qinfo.sContractCode, qinfo.sDate, qinfo.sQuoSerial);
                        ProcRisk(qinfo, pc);
                    }
                }
            }
        }

        return 0;
    }
    m_pLog->LogMp(LOG_DEBUG, __FILE__, __LINE__, "合约[%d %s] 行情[%s %s]不是最新", qinfo.nContractNo, qinfo.sContractCode, qinfo.sDate, qinfo.sQuoSerial);
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

	sprintf(m_sSql,"select SETTLE_PRICE from CONTRACT_SETTLE_PRICE where EXCH_DATE=:1 and CONTRACT_NO=:2");
	m_pSqlCmd->setCommandText(m_sSql);
	m_pSqlCmd->Param(1).setAsString() = qinfo.sDate;
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
		m_pSqlCmd->Param(1).setAsString() = qinfo.sDate;
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
		m_pSqlCmd->Param(7).setAsString() = qinfo.sDate;
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
	unsigned int nVarietyNo=0;
	switch (pc->sProd_type[0])
	{
		case PROD_TYPE_FUTURES:
			nVarietyNo = pc->nProdId;
			break;
		case PROD_TYPE_SPOT:
			nVarietyNo = pc->nProdId;
			break;
		case PROD_TYPE_BOND_BANK:
		case PROD_TYPE_BOND:
		case PROD_TYPE_SEC:
			nVarietyNo = 0;
			break;
		case PROD_TYPE_OPTION:
		case PROD_TYPE_OPTION_SEC:
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
		m_pXdp->ResetData();
		m_pXdp->SetFieldValue((unsigned short)10,(unsigned int)rinfo.nContractNo,m_sErrMsg);
		m_pXdp->SetFieldValue((unsigned short)80,rinfo.sContractCode,strlen(rinfo.sContractCode),m_sErrMsg);
		m_pXdp->SetFieldValue((unsigned short)50,rinfo.dNewPrice,m_sErrMsg);
		m_pXdp->SetFieldValue((unsigned short)51,rinfo.dUpDown,m_sErrMsg);
		m_pXdp->SetFieldValue((unsigned short)52,rinfo.dUpDownRate,m_sErrMsg);
		m_pXdp->SetFieldValue((unsigned short)53,rinfo.dBuyPrice,m_sErrMsg);
		m_pXdp->SetFieldValue((unsigned short)54,rinfo.dSellPrice,m_sErrMsg);
		m_pXdp->SetFieldValue((unsigned short)55,rinfo.dHighPrice,m_sErrMsg);
		m_pXdp->SetFieldValue((unsigned short)56,rinfo.dLowPrice,m_sErrMsg);
		m_pXdp->SetFieldValue((unsigned short)40,(UINT64_)rinfo.nDoneQty,m_sErrMsg);
		m_pXdp->SetFieldValue((unsigned short)81,rinfo.sRiskDesc,strlen(rinfo.sRiskDesc),m_sErrMsg);
		m_pXdp->SetFieldValue((unsigned short)82,rinfo.sRiskFlag,strlen(rinfo.sRiskFlag),m_sErrMsg);
		m_pXdp->SetFieldValue((unsigned short)83, m_pDate.ToStringD8().c_str(), m_pDate.ToStringD8().length(), m_sErrMsg);
		m_pXdp->SetFieldValue((unsigned short)84, m_pDate.ToStringT6().c_str(), m_pDate.ToStringT6().length(), m_sErrMsg);
		
		char sBuffer[4096];
		unsigned int len = sizeof(sBuffer);
		if (!m_pXdp->ToBuffer(sBuffer,len,m_sErrMsg))
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"Xdp ToBuffer error");
			return ;
		}
		ReleaseBroadcast(sBuffer,len,0,0,9939);
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
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"合约[%d %s]行情[%s %s]变化 计算盈亏耗时[%ld]微秒",pc->nContractId,pc->sContractCode,info.sDate,info.sQuoSerial,m_nEndTime-m_nBeginTime);
	//计算完盈亏就行了，在另外的定时任务，根据m_pSurplusLockRowId去计算交易账号的风险度风险等级
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
	if (pc->sProd_type[0] == PROD_TYPE_FUTURES || pc->sProd_type[0] == PROD_TYPE_OPTION || pc->sProd_type[0] == PROD_TYPE_OPTION_SEC)
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
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"交易账号锁表rowid[%d]不存在",rowid);
		return ;
	}
	S_MDB_CUST_FUND* fund = m_pMemDb->GetFund(info);
	std::vector<S_MDB_CUST_FUND *>fundlist;
	if (fund == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"交易账号[%s] 在资金表里不存在",info->sTxAcc);
		return ;
	}
	fundlist.push_back(fund);
	
	S_MDB_TRADEACC_INFO *tradeacc = m_pMemDb->m_pDbCust.m_pTradeAcc.At(info->nTradeAccRowid);
	if (tradeacc == NULL)
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
		//计算持仓总规模
		fund->dPosiTotalValue =  GetPosiTotalValue(tradeacc->sTxAcc);

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
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"计算交易账号[%s %s]风险度完成  风险度[%lf] 风险级别[%d] 交易权限[%s]",fund->sTxAcc,fund->sCurrency,fund->dRisk,fund->nRisk_level,fund->sTradeRight);
		if (oldrisklevel != fund->nRisk_level)
		{
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"发送风险通知 交易账号[%s]  风险度[%lf] 风险级别[%d] 交易权限[%s]",fund->sTxAcc,fund->dRisk,fund->nRisk_level,fund->sTradeRight);
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
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"交易账号锁表rowid[%d]不存在",rowid);
		return ;
	}
	S_MDB_TRADEACC_INFO *pTradeAcc = m_pMemDb->m_pDbCust.m_pTradeAcc.At(info->nTradeAccRowid);
	if (pTradeAcc == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"交易账号rowid[%d]不存在",info->nTradeAccRowid);
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
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"交易账号[%s]无持仓",info->sTxAcc);
		return;
	}
	//取交易账号对应的模板
	S_MDB_CUST_TEMPLET_RELA *templet = m_pMemDb->m_pDbSet.m_pCustTemplet.Select(info->sTxAcc);
	if (templet == NULL)
	{
		//没有找到交易账号对应的模板,取$对应的模板
		templet = m_pMemDb->m_pDbSet.m_pCustTemplet.Select("$");
		if (templet == NULL)
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"交易账号[%s] 没有对应的持仓模板 ",info->sTxAcc);
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
            //80		交易账户	20	y
            //    10		证券编号	10	y
            //    81		证券代码	20	y
            //    82		投保标志	1
            //    30		多头持仓量	18
            //    31		空头持仓量	18
            //    83		风险标志	1	y	‘0’取消风险’1’产生风险
            //    11		风险级别	2	y	数字越小，风险越高
            //    84		风险说明	80	y	说明哪个指标产生风险
            //    85		交易权限	1	y
            //    86		账户名称	40	y
            //    87		风险模板代码	12	y
            //    88		风险模板名称	40	y
            //    89		客户号	10	y
            //    90		所属机构	12	y
            //    91		所属机构名称	40	y
            //    92		代理机构	12	n
            //    93		代理机构名称	40	n
            //	  94		日期	8	y
            //    95		时间	8	y


			char riskdata[1000];
			unsigned int riskdatalen=sizeof(riskdata) ;
			m_pXdp->ResetData();
			m_pXdp->SetFieldValue((unsigned short)80,risk->sTxAcc,strlen(risk->sTxAcc),m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)10,(unsigned int)risk->nContractNo,m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)81,risk->sContractCode,strlen(risk->sContractCode),m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)82,risk->sShFlag,strlen(risk->sShFlag),m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)30,risk->nBuyQty,m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)31,risk->nSellQty,m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)83, risk->sRiskFlag, strlen(risk->sRiskFlag), m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)11, (int)risk->nWarnLevel, m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)84,risk->sRiskDesc,strlen(risk->sRiskDesc),m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)85, traderight, strlen(traderight), m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)86,pTradeAcc->sAccName,strlen(pTradeAcc->sAccName),m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)87,templet->sPosiTempletId,strlen(templet->sPosiTempletId),m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)88,pPosiTemplet->sTempletName,strlen(pPosiTemplet->sTempletName),m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)89,pTradeAcc->sCustNo,strlen(pTradeAcc->sCustNo),m_sErrMsg);
			m_pXdp->SetFieldValue((unsigned short)90, pTradeAcc->sSignOrg,strlen(pTradeAcc->sSignOrg),m_sErrMsg);
			
			pOrg = m_pMemDb->m_pDbPublic.m_pOrg.Select(pTradeAcc->sSignOrg);
			if (pOrg != NULL)
			{
				m_pXdp->SetFieldValue((unsigned short)91,pOrg->sOrgName,strlen(pOrg->sOrgName),m_sErrMsg);
			}
			else
			{
				m_pXdp->SetFieldValue((unsigned short)91," ",1,m_sErrMsg);
			}
			if (strlen(pTradeAcc->sMgrOrg)<1)
			{
				m_pXdp->SetFieldValue((unsigned short)92," ",1,m_sErrMsg);
				m_pXdp->SetFieldValue((unsigned short)93," ",1,m_sErrMsg);
			}
			else
			{
				m_pXdp->SetFieldValue((unsigned short)92, pTradeAcc->sMgrOrg,strlen(pTradeAcc->sMgrOrg),m_sErrMsg);
				pOrg = m_pMemDb->m_pDbPublic.m_pOrg.Select(pTradeAcc->sMgrOrg);
				if (pOrg != NULL)
				{
					m_pXdp->SetFieldValue((unsigned short)93,pOrg->sOrgName,strlen(pOrg->sOrgName),m_sErrMsg);
				}
				else
				{
					m_pXdp->SetFieldValue((unsigned short)93," ",1,m_sErrMsg);
				}
			}
            char tmpchar[30];
            sprintf(tmpchar, "%ld", risk->nRiskdate);
            m_pXdp->SetFieldValue((unsigned short)94, tmpchar, strlen(tmpchar), m_sErrMsg);
            sprintf(tmpchar, "%ld", risk->nRisktime);
            m_pXdp->SetFieldValue((unsigned short)95, tmpchar, strlen(tmpchar), m_sErrMsg);
			if (!m_pXdp->ToBuffer(riskdata,riskdatalen,m_sErrMsg))
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"Xdp ToBuffer error %s",m_sErrMsg);
				return;
			}
			ReleaseBroadcast(riskdata,riskdatalen,atoi(risk->sTxAcc),0,9931);
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"发送账户[%s]持仓风险通知完成",info->sTxAcc);
		}
	}
	return ;
}


void RiskDll::SendRiskMsg(S_MDB_TXACC_LOCK *acc,S_MDB_CUST_FUND *fund, char traderight)
{
	if (*g_nSvrHostStatus != 	ARBSTATUS_MASTER && *g_nSvrHostStatus != ARBSTATUS_MASTER_AYN && *g_nSvrHostStatus!=MASTERHOST)
	{
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"SendRiskMsg 不是主机，略过");
		return;
	}
	m_pXdp->ResetData();

	S_MDB_TRADEACC_INFO *pFundAcc = m_pMemDb->m_pDbCust.m_pTradeAcc.At(acc->nTradeAccRowid);
	if (pFundAcc == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"交易账号rowid[%d]不存在",acc->nTradeAccRowid);
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
	//取交易账号对应的模板
	S_MDB_CUST_TEMPLET_RELA *templet = m_pMemDb->m_pDbSet.m_pCustTemplet.Select(acc->sTxAcc);
	if (templet == NULL)
	{
		//没有找到交易账号对应的模板,取$对应的模板
		templet = m_pMemDb->m_pDbSet.m_pCustTemplet.Select("$");
		if (templet == NULL)
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"交易账号[%s] 没有对应的持仓模板 ",acc->sTxAcc);
		}
		else
		{
			strcpy(sRiskTempletId, templet->sFundTempletId);
		}
	}
	if (templet != NULL)
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
        //80		交易账户	20	y
        //81		币种	20	y
        //50		风险度	18.4	y
        //10		风险级别	2	y	数字越小，风险越高
        //82		交易权限	1	y
        //83		风险标志	1	y	‘0’取消风险’1’产生风险
        //84		风险说明	80	y	说明哪个指标产生风险
        //51		可用资金	18.4	y
        //52		可提资金	18.4	y
        //53		保证金	18.4	y
		//53		权益	18.4	y
        //85		账户名称	40	y
        //86		风险模板代码	12	y
        //87		风险模板名称	40	y
        //88		所属机构	12	y
        //89		所属机构名称	40	y
        //90		代理机构	12	n
        //91		代理机构名称	40	n
        //92		客户号	10	y
        // 93		日期	8	y	
		//94		时间	8	y

	//设置值
	m_pXdp->SetFieldValue((unsigned short)80,fund->sTxAcc,strlen(fund->sTxAcc),m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)81,fund->sCurrency,strlen(fund->sCurrency),m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)82,fund->sTradeRight, strlen(fund->sTradeRight), m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)50,fund->dRisk,m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)10,(int)fund->nRisk_level,m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)51,fund->dAvailFund,m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)52,fund->dGetFund,m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)53,fund->dMargin,m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)54,fund->dBalance,m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)84,fund->sRiskDesc,strlen(fund->sRiskDesc),m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)85,pFundAcc->sAccName,strlen(pFundAcc->sAccName),m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)86,sRiskTempletId,strlen(sRiskTempletId),m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)87,sRiskTempletName,strlen(sRiskTempletName),m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)92,pFundAcc->sCustNo,strlen(pFundAcc->sCustNo),m_sErrMsg);
	m_pXdp->SetFieldValue((unsigned short)88,pFundAcc->sSignOrg,strlen(pFundAcc->sSignOrg),m_sErrMsg);
	pOrg = m_pMemDb->m_pDbPublic.m_pOrg.Select(pFundAcc->sSignOrg);
	if (pOrg != NULL)
	{
		m_pXdp->SetFieldValue((unsigned short)89,pOrg->sOrgName,strlen(pOrg->sOrgName),m_sErrMsg);
	}
	else
	{
		m_pXdp->SetFieldValue((unsigned short)89," ",1,m_sErrMsg);
	}
	if (strlen(pFundAcc->sMgrOrg)<1)
	{
		m_pXdp->SetFieldValue((unsigned short)90," ",1,m_sErrMsg);
		m_pXdp->SetFieldValue((unsigned short)91," ",1,m_sErrMsg);
	}
	else
	{
		m_pXdp->SetFieldValue((unsigned short)90,pFundAcc->sMgrOrg,strlen(pFundAcc->sMgrOrg),m_sErrMsg);
		pOrg = m_pMemDb->m_pDbPublic.m_pOrg.Select(pFundAcc->sMgrOrg);
		if (pOrg != NULL)
		{
			m_pXdp->SetFieldValue((unsigned short)91,pOrg->sOrgName,strlen(pOrg->sOrgName),m_sErrMsg);
		}
		else
		{
			m_pXdp->SetFieldValue((unsigned short)91," ",1,m_sErrMsg);
		}
	}
	char tmpchar[30];
	sprintf(tmpchar,"%ld", fund->nRiskdate);
	m_pXdp->SetFieldValue((unsigned short)93, tmpchar,strlen(tmpchar),m_sErrMsg);
    sprintf(tmpchar, "%ld", fund->nRisktime);
    m_pXdp->SetFieldValue((unsigned short)94, tmpchar, strlen(tmpchar), m_sErrMsg);
	
	char sRiskdata[1000];
	unsigned int len = sizeof(sRiskdata);
	if (!m_pXdp->ToBuffer(sRiskdata,len,m_sErrMsg))
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"Xdp ToBuffer error");
		return ;
	}
	ReleaseBroadcast(sRiskdata,len,atoi(fund->sTxAcc),0,9930);
	
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

	char sTxAcc[21];//交易账号
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
	std::vector<S_MDB_TRADEACC_INFO *> fundacclist;
	std::vector<S_MDB_ORG_INFO *> orglist;
	CInt acclist;
	S_MDB_ORG_INFO *pOrgTmp=NULL;
	S_MDB_TRADEACC_INFO *pCustAccTmp=NULL;
	

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
	S_MDB_TRADEACC_INFO *pacc=NULL;
	S_MDB_ORDER_INFO *pordertmp=NULL;
	if (nFlag == 0)//客户
	{
		if (strlen(sTxAcc)>0)
		{

			pacc = m_pMemDb->m_pDbCust.m_pTradeAcc.SelectByAcc(sTxAcc);
			if (pacc == NULL)
			{
				data.pData->sDBHead.nLen = 0;
				data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_NOTFOUND;
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"交易账号 [%s]没有找到",sTxAcc);
				AnsData(data);
				return -1;
			}
			if (strcmp(pacc->sCustNo,sOper) != 0)
			{
				data.pData->sDBHead.nLen = 0;
				data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_NO_RIGHT;
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"交易账号[%s]不属于客户[%s]",sTxAcc,sOper);
				AnsData(data);
				return -1;
			}
			m_pMemDb->m_pDbTrader.m_pOrder.SelectByAcc(sTxAcc,reslist);
		}
		else //没有输入交易账号 按报单号
		{
			if (strlen(sLocalSerial)>0)
			{
				//按本地报单号查询
				pordertmp = m_pMemDb->m_pDbTrader.m_pOrder.Select(sLocalSerial);
				if (pordertmp != NULL)
				{
					//是否有权限
					pacc = m_pMemDb->m_pDbCust.m_pTradeAcc.SelectByAcc(pordertmp->sTxAcc);
					if (pacc == NULL)
					{
						data.pData->sDBHead.nLen = 0;
						data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_NOTFOUND;
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"交易账号 [%s]没有找到",pordertmp->sTxAcc);
						AnsData(data);
						return -1;
					}
					if (strcmp(pacc->sCustNo,sOper) != 0)
					{
						data.pData->sDBHead.nLen = 0;
						data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_NO_RIGHT;
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"交易账号[%s]不属于客户[%s]",pordertmp->sTxAcc,sOper);
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
					pacc = m_pMemDb->m_pDbCust.m_pTradeAcc.SelectByAcc(reslist[0]->sTxAcc);
					if (pacc == NULL)
					{
						data.pData->sDBHead.nLen = 0;
						data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_NOTFOUND;
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"交易账号 [%s]没有找到",reslist[0]->sTxAcc);
						AnsData(data);
						return -1;
					}
					if (strcmp(pacc->sCustNo,sOper) != 0)
					{
						data.pData->sDBHead.nLen = 0;
						data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_NO_RIGHT;
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"交易账号[%s]不属于客户[%s]",reslist[0]->sTxAcc,sOper);
						AnsData(data);
						return -1;
					}
				}
			}
			else
			{
				//按客户号
				if (!m_pMemDb->m_pDbCust.m_pTradeAcc.SelectByCust(sOper,fundacclist))
				{
					data.pData->sDBHead.nLen = 0;
					data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_NOTFOUND;
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"客户号[%s]无所属交易账号",sOper);
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
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"操作员[%s]无交易账号[%s]权限",sOper,sTxAcc);
				AnsData(data);
				return -1;
			}
			m_pMemDb->m_pDbTrader.m_pOrder.SelectByAcc(sTxAcc,reslist);
		}
		else //没有输入交易账号 
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
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"操作员[%s]无交易账号[%s]权限",sOper,pordertmp->sTxAcc);
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
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"操作员[%s]无交易账号[%s]权限",sOper,reslist[0]->sTxAcc);
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
							m_pMemDb->m_pDbCust.m_pTradeAcc.SelectAll(acclist);
						}
						else
						{
							//循环取每个机构的资金账户
							for (i=0; i<orglist.size();i++)
							{
								//查询此机构下面的客户 所属机构
								m_pMemDb->m_pDbCust.m_pTradeAcc.SelectByAccOrg(orglist[i]->sOrgCode,acclist);
								//代理机构
								m_pMemDb->m_pDbCust.m_pTradeAcc.SelectByMgrOrg(orglist[i]->sOrgCode,acclist);
							}
						}
					}
					else
					{
						//查询此机构下面的客户 所属机构
						if (!m_pMemDb->m_pDbCust.m_pTradeAcc.SelectByAccOrg(sOrgCode,acclist))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"机构[%s]下无交易账号",sOrgCode);
							}
						//代理机构
						if (!m_pMemDb->m_pDbCust.m_pTradeAcc.SelectByMgrOrg(sOrgCode,acclist))
						{
							m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"机构[%s]下无交易账号",sOrgCode);
						}
					}
					bRet = acclist.First(rid);
					if (!bRet)
					{
						data.pData->sDBHead.nLen = 0;
						data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_NOTFOUND;
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"机构[%s]没有交易账号",sOrgCode);
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
					if (!m_pMemDb->m_pDbCust.m_pTradeAcc.SelectByAccOrg(operorg->sOrgCode,acclist))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"操作员[%s]所属机构[%s]下无交易账号",sOper,operorg->sOrgCode);
					}
					//代理机构
					if (!m_pMemDb->m_pDbCust.m_pTradeAcc.SelectByMgrOrg(operorg->sOrgCode,acclist))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"操作员[%s]所属机构[%s]下无交易账号",sOper,sOrgCode);
					}
					bRet = acclist.First(rid);
					if (!bRet)
					{
						data.pData->sDBHead.nLen = 0;
						data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_NOTFOUND;
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"机构[%s]没有交易账号",sOrgCode);
						AnsData(data);
						return -1;
					}
				}
				while (bRet)
				{
					//查询报单
					m_pMemDb->m_pDbTrader.m_pOrder.SelectByAcc(m_pMemDb->m_pDbCust.m_pTradeAcc.At(rid)->sTxAcc,reslist);
					bRet = acclist.Next(rid);
				}

			}
		}
	}
	
	int curNum=0;
	S_MDB_TRADEACC_INFO *pFundAcc=NULL;
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
				pFundAcc = m_pMemDb->m_pDbCust.m_pTradeAcc.SelectByAcc(reslist[i]->sTxAcc);
				if (pFundAcc != NULL)
				{
					m_pXdp->SetFieldValue((unsigned short)curNum+1,(unsigned short)119,pFundAcc->sAccName,strlen(pFundAcc->sAccName),m_sErrMsg);
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
			pFundAcc = m_pMemDb->m_pDbCust.m_pTradeAcc.SelectByAcc(reslist[i]->sTxAcc);
			if (pFundAcc != NULL)
			{
				m_pXdp->SetFieldValue((unsigned short)i+1,(unsigned short)119,pFundAcc->sAccName,strlen(pFundAcc->sAccName),m_sErrMsg);
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
 	INT64_  nBeginTime=CBF_Date_Time::GetTickUS();
	INT64_  nEndTime;
	S_MDB_ORDER_INFO order;
	S_MDB_ORDER_INFO *po=NULL;
	S_MDB_TXACC_LOCK *acclock=NULL;
	bzero(&order,sizeof(S_MDB_ORDER_INFO));
	Xdp2Struct::ConvertOrder(m_pXdp,order);

    char errmsg[256];
	int fieldtype;
    unsigned int len = sizeof(order.sLocalSerial);
	if (!m_pXdp->GetFieldValue((unsigned short)95, order.sLocalSerial, len, fieldtype, errmsg))
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "报单消息 本地报单号为空");
		return -1;
	}
    len = sizeof(order.sSystemNo);
    if (!m_pXdp->GetFieldValue((unsigned short)96, order.sSystemNo, len, fieldtype, errmsg))
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "报单消息 系统报单号为空");
        return -1;
    }
    len = sizeof(order.sOrderStatus);
    if (!m_pXdp->GetFieldValue((unsigned short)97, order.sOrderStatus, len, fieldtype, errmsg))
    {
		order.sOrderStatus[0]=ORDER_STATUS_OK;//默认为已报入
    }

	//根据本地报单号去查询，若找到说明是风控发起的报单，否则为外报单
	po = m_pMemDb->m_pDbTrader.m_pOrder.Select(order.sLocalSerial);
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
		m_pMemDb->m_pDbTrader.m_pOrder.SetUpdate(*po); //设置需要更新
		nEndTime =CBF_Date_Time::GetTickUS();
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"委托回报 处理完成 本地报单号[%s]  处理时间[%d]微秒",po->sLocalSerial,nEndTime-nBeginTime);
		acclock = m_pMemDb->SelectLock(po->sTxAcc);
		if (acclock == NULL)
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托回报  交易账号[%s]不存在",po->sTxAcc);
			return -1;
		}
		//通知要对此客户进行风险计算
		MsgCustRiskCalc(acclock->nLockRowid);
        if (*g_nSvrHostStatus == ARBSTATUS_MASTER || *g_nSvrHostStatus == ARBSTATUS_MASTER_AYN || *g_nSvrHostStatus == MASTERHOST)
        {
            //是主机发送回报推送
            SendOrderBroadcast(acclock, po);
        }
		
		return 0;
	}
	order.nRemainNum = order.nOrderNum;//剩余数量默认为委托数量
	order.sCancelFlag[0] = BOOL_FALSE;
	order.sCancelFlag[1] =0;
	//查找交易日期
	S_MDB_CONTRACT_INFO *pc = m_pMemDb->m_pDbBaseinfo.m_pContract.Select(order.sContractCode);
	if (pc == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托通知合约代码[%s]在合约表中不存在",order.sContractCode);
		return -1;
	}
	strcpy(order.sContractName,pc->sContractName);
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
	strcpy(order.sTxDate,pe->sTradeDate);
	acclock = m_pMemDb->SelectLock(order.sTxAcc);
	if (acclock == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托通知交易账号[%s]不存在",order.sTxAcc);
		return -1;
	}
	S_MDB_TRADEACC_INFO *pcustacc = m_pMemDb->m_pDbCust.m_pTradeAcc.At(acclock->nTradeAccRowid);
	if (pcustacc == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托通知交易账号[%s]不存在",order.sTxAcc);
		return -1;
	}
	order.dAmt = order.dOrderPrice*pc->dTxUnit*order.nOrderNum;//金额
	//取交易编码及席位号
	S_MDB_CUST_TRADECODE *ptx = m_pMemDb->m_pDbTrader.m_pTradecode.Select(order.sTxAcc,order.sExchCode);
	if (ptx != NULL)
	{
		strcpy(order.sTxCode,ptx->sTradeCode);
		strcpy(order.sSeatNo, ptx->sSeatNo);
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
	if (strlen(order.sAccOrg) < 1)
	{
		strcpy(order.sAccOrg, pcustacc->sSignOrg);
	}
	if (strlen(order.sTxOrg) < 1)
	{
		strcpy(order.sTxOrg, pcustacc->sSignOrg);
	}
	//获取报单的手续费及冻结金额
	if (m_pKernelPub.GetOrderFeeAmt(order,pc,pcustacc) !=0)
	{
		return -1;
	}
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
	m_pMemDb->m_pDbTrader.m_pOrder.SetUpdate(orderrid);	//设置更新标志
	po->nSyncFlag = 1; //报入同步
	S_MDB_CUST_FUND *pfund = m_pMemDb->GetFund(acclock);
	if (pfund == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托通知 找不到交易账号资金信息 [%s %s]",order.sTxAcc,pc->sCurrency);
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
    if (*g_nSvrHostStatus == ARBSTATUS_MASTER || *g_nSvrHostStatus == ARBSTATUS_MASTER_AYN || *g_nSvrHostStatus == MASTERHOST)
    {
        //是主机发送回报推送
        SendOrderBroadcast(acclock, po);
    }
	
	return 0;
}
int RiskDll::DoneRtn(S_TRADE_DATA& data)
{
    m_nBeginTime = CBF_Date_Time::GetTickUS();
    if (!m_pXdp->FromBuffer(data.pData->sBuffer, data.pData->sDBHead.nLen, m_sErrMsg))
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "请求非XDP数据 [%s]", m_sErrMsg);
        m_pClientLink->PoolFree(data);
        return -1;
    }
    m_pClientLink->PoolFree(data);
    S_MDB_ORDER_INFO* po = NULL;
    S_MDB_DONE_INFO info;
    bzero(&info, sizeof(S_MDB_DONE_INFO));
	Xdp2Struct::ConvertDone(m_pXdp,info);

    char sTxAcc[21];
    bzero(sTxAcc, sizeof(sTxAcc));
    char sLocalNo[38];
    bzero(sLocalNo, sizeof(sLocalNo));
    

    //根据交易所代码+系统报单号 去取报单信息
    std::vector<S_MDB_ORDER_INFO*> orderlist;
    if (!m_pMemDb->m_pDbTrader.m_pOrder.SelectBySystemno(info.sExchCode, info.sSystemNo, orderlist))
    {
        m_pLog->LogMp(LOG_PROMPT, __FILE__, __LINE__, "成交回报 交易所[%s] 系统报单号[%s] 在委托表中不存在  放入待处理队列", info.sExchCode, info.sSystemNo);
        //放入未处理成功队列，定时来处理
        m_pMemDb->m_pUnProcessDoneMatch.push_back(info);
        return -1;
    }
    if (orderlist.size() != 1)
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "成交回报 交易所[%s] 系统报单号[%s]不是唯一", info.sExchCode, info.sSystemNo);
        return -1;
    }
	m_pDate.Update();
    po = orderlist[0];
	//从报单信息中补齐
    strcpy(info.sDoneDate, po->sTxDate);
    strcpy(info.sTxAcc, po->sTxAcc);
    strcpy(info.sContractCode, po->sContractCode);
    info.nContractNo = po->nContractNo;
    strcpy(info.sLocalNo, po->sLocalSerial);
    strcpy(info.sShFlag, po->sShFlag);
    strcpy(info.sEoFlag, po->sEoFlag);
    strcpy(info.sBsFlag, po->sBsFlag);
    strcpy(info.sTxOrg, po->sTxOrg);
    strcpy(info.sAccOrg, po->sAccOrg);
    strcpy(info.sTxType, po->sTxType);
    strcpy(info.sCmdType, po->sCmdType);
    strcpy(info.sCmdAttr, po->sCmdAttr);
    strcpy(info.sOrderOper, po->sOrderOper);
    strcpy(info.sTxCode, po->sTxCode);
    strcpy(info.sSeatNo, po->sSeatNo);
	strcpy(info.sOrderChannel, po->sOrderChannel);
	strcpy(info.sForceFlag, po->sForceFlag);
	strcpy(info.sRuleAlgoCode, po->sRuleAlgoCode);
	info.nVarietyId = po->nVarietyId;
	strcpy(info.sDoneTime,m_pDate.ToStringDT17().c_str());
	strcpy(info.sContractName, po->sContractName);
	strcpy(info.sSystemDate,m_pDate.ToStringD8().c_str());

    S_MDB_CONTRACT_INFO* pc = m_pMemDb->m_pDbBaseinfo.m_pContract.Select(info.sContractCode);
    if (pc == NULL)
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "成交回报 的合约[%d %s]不存在", info.nContractNo, info.sContractCode);
        return -1;
    }
    S_MDB_EXCH_INFO* pexch = m_pMemDb->m_pDbBaseinfo.m_pExchange.Select(pc->sExcode);
    if (pexch == NULL)
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "成交回报 的合约代码[%s]所属交易所[%s]不存在", info.sContractCode, pc->sExcode);
        return -1;
    }
	//info.nVarietyId = pc->nProdId;
	//strcpy(info.sContractName,pc->sContractName);

    S_MDB_TRADEACC_INFO* pcustacc = m_pMemDb->m_pDbCust.m_pTradeAcc.SelectByAcc(info.sTxAcc);
    if (pcustacc == NULL)
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "成交回报 交易账号[%s]不存在", info.sTxAcc);
        return -1;
    }
    info.dAmt = info.dDonePrice * pc->dTxUnit * info.nDoneNum;//金额
    //查询此成交是否存在
    S_MDB_DONE_INFO* pdinfo = m_pMemDb->m_pDbTrader.m_pDone.Select(info.sExchCode, info.sBsFlag, info.sDoneNo);
    if (pdinfo != NULL)
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "成交回报 交易账号[%s] 的成交[%s %s %s]已存在", info.sTxAcc, info.sExchCode, info.sBsFlag, info.sDoneNo);
        return -1;
    }

    ProcessDone(pcustacc, po, pc, pexch, info);
    return 0;
}
int RiskDll::CancelOrderRtn(S_TRADE_DATA& data)
{
    m_nBeginTime = CBF_Date_Time::GetTickUS();
    if (!m_pXdp->FromBuffer(data.pData->sBuffer, data.pData->sDBHead.nLen, m_sErrMsg))
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "请求非XDP数据 [%s]", m_sErrMsg);
        m_pClientLink->PoolFree(data);
        return -1;
    }
    m_pClientLink->PoolFree(data);
    //80		系统号	20	y	交易所的报单号
    //    81		交易所代码	1	y
    //    82		撤单成功标志	1	y	撤单成功标志’1’ 成功
    //    10		撤单数量	8	y


    char sExcode[2];//交易所代码
    char sSystemNo[38];//系统报单号
    char sStatus[10];
	char sOper[21];	        //操作员
	char sChannel[3];		  //渠道
    bzero(sExcode, sizeof(sExcode));
    bzero(sSystemNo, sizeof(sSystemNo));
    bzero(sStatus, sizeof(sStatus));
	bzero(sOper, sizeof(sOper));
	bzero(sChannel, sizeof(sChannel));

    unsigned int len;
    int fieldtype;
    unsigned int   nCancelNum;//撤单数量

    len = sizeof(sExcode);
    if (!m_pXdp->GetFieldValue((unsigned short)81, sExcode, len, fieldtype, m_sErrMsg))
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "撤单通知 无交易所代码 %s", m_sErrMsg);
        return -1;
    }
    len = sizeof(sSystemNo);
    if (!m_pXdp->GetFieldValue((unsigned short)80, sSystemNo, len, fieldtype, m_sErrMsg))
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "撤单通知 无系统报单号 %s", m_sErrMsg);
        return -1;
    }
    if (!m_pXdp->GetFieldValue((unsigned short)10, nCancelNum, m_sErrMsg))
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "撤单通知 无撤单数量 %s", m_sErrMsg);
        return -1;
    }
    len = sizeof(sStatus);
    if (!m_pXdp->GetFieldValue((unsigned short)82, sStatus, len, fieldtype, m_sErrMsg))
    {
        strcpy(sStatus, "1");
    }
    if (strcmp(sStatus, "1") != 0)
    {
        m_pLog->LogMp(LOG_DEBUG, __FILE__, __LINE__, "撤单通知 交易所[%s] 系统号[%s] 撤单不成功", sExcode, sSystemNo);
        return -1;
    }
    len = sizeof(sOper);
    m_pXdp->GetFieldValue((unsigned short)83, sOper, len, fieldtype, m_sErrMsg);
    len = sizeof(sChannel);
    m_pXdp->GetFieldValue((unsigned short)84, sChannel, len, fieldtype, m_sErrMsg);
    std::vector<S_MDB_ORDER_INFO*> orderlist;
    if (!m_pMemDb->m_pDbTrader.m_pOrder.SelectBySystemno(sExcode, sSystemNo, orderlist))
    {
        SLEEP(1000);
        if (!m_pMemDb->m_pDbTrader.m_pOrder.SelectBySystemno(sExcode, sSystemNo, orderlist))
        {
            m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "撤单通知 交易所[%s] 系统号[%s] 找不到报单", sExcode, sSystemNo);
        }
        return -1;
    }
    if (orderlist.size() != 1)
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "撤单通知 交易所[%s] 系统号[%s] 不唯一", sExcode, sSystemNo);
        return -1;
    }

    S_MDB_ORDER_INFO* po = orderlist[0];

    S_MDB_TXACC_LOCK* pAcclock = m_pMemDb->SelectLock(po->sTxAcc);
    if (pAcclock == NULL)
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "撤单通知 交易账号[%s]找不到", po->sTxAcc);
        return -1;
    }
	strcpy(po->sCancelChannel,sChannel);
	strcpy(po->sCancelOper,sOper);
	m_pDate.Update();
	strcpy(po->sCancelTime,m_pDate.ToStringDT17().c_str());
    MsgCustRiskCalc(pAcclock->nLockRowid);
    if (m_pKernelPub.ProceeOneCancelOrder(pAcclock, po, nCancelNum) == 0)
    {
        m_nEndTime = CBF_Date_Time::GetTickUS();
        m_pLog->LogMp(LOG_DEBUG, __FILE__, __LINE__, "撤单通知 处理完成 本地报单号[%s] 耗时[%ld]微秒", po->sLocalSerial, m_nEndTime - m_nBeginTime);
        //发送撤单推送
        if (*g_nSvrHostStatus == ARBSTATUS_MASTER || *g_nSvrHostStatus == ARBSTATUS_MASTER_AYN || *g_nSvrHostStatus == MASTERHOST)
        {
            //是主机发送撤单推送
			SendOrderBroadcast(pAcclock, po);
        }
    }
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
	sprintf(m_sSql,"insert into t_order_info(TX_ACC,EX_CODE,SEAT_NO,TX_CODE,SECU_NO,SECU_CODE,SECU_NAME,CLIENT_SERIAL,LOCAL_NO,SYSTEM_NO,ORDER_VOL,ORDER_PRICE,REMAIN_VOL,EO_FLAG,BS_FLAG,SH_FLAG,TXDATE,AMT,FROZ_AMT,EX_FEE,TX_TYPE,CMD_TYPE,CMD_ATTR,FORCE_FLAG,ORDER_STATUS,CANCEL_FLAG,ANS_TIME,ORDER_CHANNEL,ACC_ORG,TX_ORG,ORDER_OPER,CANCEL_CHANNEL,CANCEL_OPER,CANCEL_TIME,ORDER_RSP,FROZ_USEAMT,FROZ_OUTAMT,CANCEL_VOL,DONE_AVP,VARIETY,RULE_ALGO_CODE,CLEAR_SPEED,DELIVERY_TYPE,CLEAR_METHOD,MATCH_ACC) values(:1,:2,:3,:4,:5,:6,:7,:8,:9,:10,:11,:12,:13,:14,:15,:16,:17,:18,:19,:20,:21,:22,:23,:24,:25,:26,:27,:28,:29,:30,:31,:32,:33,:34,:35,:36,:37,:38,:39,:40,:41,:42,:43,:44,:45) ");
	m_pSqlCmd->setCommandText(m_sSql);
	m_pSqlCmd->Param(1).setAsString() = porder->sTxAcc;
	m_pSqlCmd->Param(2).setAsString() = porder->sExchCode;
	m_pSqlCmd->Param(3).setAsString() = porder->sSeatNo;
	m_pSqlCmd->Param(4).setAsString() = porder->sTxCode;
	m_pSqlCmd->Param(5).setAsLong() = porder->nContractNo;
	m_pSqlCmd->Param(6).setAsString() = porder->sContractCode;
	m_pSqlCmd->Param(7).setAsString() = porder->sContractName;
	m_pSqlCmd->Param(8).setAsString() = porder->sClientSerial;
	m_pSqlCmd->Param(9).setAsString() = porder->sLocalSerial;
	m_pSqlCmd->Param(10).setAsString() = porder->sSystemNo;
	m_pSqlCmd->Param(11).setAsLong() = porder->nOrderNum;
	m_pSqlCmd->Param(12).setAsDouble() = porder->dOrderPrice;
	m_pSqlCmd->Param(13).setAsLong() = porder->nRemainNum;
	m_pSqlCmd->Param(14).setAsString() = porder->sEoFlag;
	m_pSqlCmd->Param(15).setAsString() = porder->sBsFlag;
	m_pSqlCmd->Param(16).setAsString() = porder->sShFlag;
	m_pSqlCmd->Param(17).setAsString() = porder->sTxDate;
	m_pSqlCmd->Param(18).setAsDouble() = porder->dAmt;
	m_pSqlCmd->Param(19).setAsDouble() = porder->dFrozAmt;
	m_pSqlCmd->Param(20).setAsDouble() = porder->dExFee;
	m_pSqlCmd->Param(21).setAsString() = porder->sTxType;
	m_pSqlCmd->Param(22).setAsString() = porder->sCmdType;
	m_pSqlCmd->Param(23).setAsString() = porder->sCmdAttr;
	m_pSqlCmd->Param(24).setAsString() = porder->sForceFlag;
	m_pSqlCmd->Param(25).setAsString() = porder->sOrderStatus;
	m_pSqlCmd->Param(26).setAsString() = porder->sCancelFlag;
	m_pSqlCmd->Param(27).setAsString() = porder->sAnsTime;
	m_pSqlCmd->Param(28).setAsString() = porder->sOrderChannel;
	m_pSqlCmd->Param(29).setAsString() = porder->sAccOrg;
	m_pSqlCmd->Param(30).setAsString() = porder->sTxOrg;
	m_pSqlCmd->Param(31).setAsString() = porder->sOrderOper;
	m_pSqlCmd->Param(32).setAsString() = porder->sCancelChannel;
	m_pSqlCmd->Param(33).setAsString() = porder->sCancelOper;
	m_pSqlCmd->Param(34).setAsString() = porder->sCancelTime;
	m_pSqlCmd->Param(35).setAsString() = porder->sOrderRsp;
	m_pSqlCmd->Param(36).setAsDouble() = porder->dFrozUseamt;
	m_pSqlCmd->Param(37).setAsDouble() = porder->dFrozOutamt;
	m_pSqlCmd->Param(38).setAsLong() = porder->nCancelNum;
	m_pSqlCmd->Param(39).setAsDouble() = porder->dDoneAvp;
	m_pSqlCmd->Param(40).setAsLong() = porder->nVarietyId;
	m_pSqlCmd->Param(41).setAsString() = porder->sRuleAlgoCode;
	m_pSqlCmd->Param(42).setAsLong() = porder->nClearSpeed;
	m_pSqlCmd->Param(43).setAsLong() = porder->nDeliveryType;
	m_pSqlCmd->Param(44).setAsLong() = porder->nClearMethod;
	m_pSqlCmd->Param(45).setAsString() = porder->sMatchAcc;
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
	CTbl_Char_Key custlist;//保存此市场下所有的交易账号
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
	//重置这些交易账号的信息
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
	CTbl_Char_Key custlist;//保存此市场下所有的交易账号
	
	std::vector<S_MDB_CUST_TRADECODE *> tclist;
	if (!m_pMemDb->m_pDbTrader.m_pTradecode.SelectByExch(exchange,tclist))
	{
		//根据交易编码表查找此交易所对应的交易账号
		m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"交易所[%s] 交易编码对应表未找到交易账号",exchange);
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
	//重置这些交易账号的信息
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
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"删除交易账号[%s]的特殊费率设置出错",stxacc);
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
	//找到此市场的所有客户  注意交易账号不能跨多个市场
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
	//找到此交易所的所有客户 注意交易账号不能跨多个交易所 
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
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"日切 开始读取 交易账号[%s]的信息",txacc);
	//读交易账号信息
	S_MDB_TRADEACC_INFO *pFundAcc = m_pMemDb->m_pDbCust.m_pTradeAcc.SelectByAcc((char *)txacc);
	if (pFundAcc == NULL)
	{
		//没有此交易账号，是新增加的
		S_MDB_TRADEACC_INFO info;
		sprintf(m_sSql,"select TX_ACC,CUST_NO,ACC_TYPE,ACC_SIMPLENAME,SIGN_ORG,MANAGER_ORG,FUTURES_FLAG,ACC_LEVEL,EX_FEE,MEMBER_FEE,BROKERAGE_CODE,CURRENCY,FS_FEE_MODEL,ACC_NAME from CUST_FUNDACC_INFO where TX_ACC=:1 ");
		m_pSqlCmd->setCommandText(m_sSql);
		m_pSqlCmd->Param(1).setAsString() = txacc;
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"日切 读取新增的 交易账号[%s]的信息  %s",txacc,m_sSql);
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
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"日切  交易账号CUST_FUNDACC_INFO表无 %s 数据 ",txacc);
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
		strcpy(info.sAccName,m_pSqlCmd->Field(13).asString());
		fundaccrid = m_pMemDb->m_pDbCust.m_pTradeAcc.Insert(info);
		if (fundaccrid <0 )
		{
#ifdef _ENGLISH_
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"日切  CUST_FUNDACC_INFO [%s] repeat",info.sTxAcc);
#else
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"日切  交易账号CUST_FUNDACC_INFO表 [%s]重复",info.sTxAcc);
#endif
			return false;
		}
		pFundAcc = m_pMemDb->m_pDbCust.m_pTradeAcc.At(fundaccrid);
		//读客户信息
		S_MDB_CUSTINFO cinfo;
		
		sprintf(m_sSql,"select CUST_NO,CUST_NAME,CUST_TYPE,CUST_CERT,CUST_CERT_NO,CUST_PHONE,TXCHANNLE,PKI_CERT_SERIAL,PKI_CERT_DATE,CUST_STATUS,LAST_RISK_LEVEL from CUST_BASIC_INFO where CUST_NO=:1");
		m_pSqlCmd->setCommandText(m_sSql);
		m_pSqlCmd->Param(1).setAsString() = info.sCustNo;
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"日切 读取新增的 交易账号[%s] 客户号[%s]的信息  %s",txacc,info.sCustNo,m_sSql);
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
		//cinfo.dLastRisk = m_pSqlCmd->Field(10).asDouble();
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
		S_MDB_TRADEACC_INFO info;
		sprintf(m_sSql,"select TX_ACC,CUST_NO,ACC_TYPE,ACC_SIMPLENAME,SIGN_ORG,MANAGER_ORG,FUTURES_FLAG,ACC_LEVEL,EX_FEE,MEMBER_FEE,BROKERAGE_CODE,CURRENCY,FS_FEE_MODEL,ACC_NAME from CUST_FUNDACC_INFO where TX_ACC=:1 ");
		m_pSqlCmd->setCommandText(m_sSql);
		m_pSqlCmd->Param(1).setAsString() = txacc;

		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"日切 读取 交易账号[%s]的 信息 更新 %s",txacc,m_sSql);
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
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"日切  交易账号CUST_FUNDACC_INFO表无数据 ");
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
		strcpy(info.sAccName,m_pSqlCmd->Field(13).asString());
		
		fundaccrid = m_pMemDb->m_pDbCust.m_pTradeAcc.Update(info);
		if (fundaccrid <0 )
		{
#ifdef _ENGLISH_
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"日切  CUST_FUNDACC_INFO [%s] update error",info.sTxAcc);
#else
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"日切  交易账号CUST_FUNDACC_INFO表 [%s] 更新失败",info.sTxAcc);
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
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"日切 读取 交易账号[%s]的 资金信息 %s",txacc,m_sSql);
	if (!m_pSqlCmd->Execute())
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"日切  Execute <%s> error %d %s",m_sSql,m_pSqlCmd->m_dbconn.m_nErrCode,m_pSqlCmd->m_dbconn.m_sErrInfo.c_str());
		return false;
	}
	bret = m_pSqlCmd->FetchNext();
	if (!bret)
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"日切  交易账号[%s]在资金表中不存在",txacc);
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
		pFund =  m_pMemDb->m_pDbTrader.m_pFund.Select(finfo.sTxAcc);
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
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"读交易账号[%s %s]的资金信息 上日结存[%lf] 今日结存[%lf]",pFund->sTxAcc,pFund->sCurrency,pFund->dLast_bal,pFund->dBalance);
		bret = m_pSqlCmd->FetchNext();
	}
	
	S_MDB_TXACC_LOCK *pacclock=m_pMemDb->SelectLock((char *)txacc);
	if (pacclock == NULL)
	{
		S_MDB_TXACC_LOCK  acclock;
		strcpy(acclock.sTxAcc,finfo.sTxAcc);
		acclock.nTradeAccRowid=-1;
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
			pacclock->nTradeAccRowid = -1;
			pacclock=m_pMemDb->SelectLock((char *)txacc);
		}
	}
	//读取资金详细表
	S_MDB_CUST_FUND_DETAIL fdinfo;
	sprintf(m_sSql,"select TX_ACC,CURRENCY,FUND_TYPE,FUND_CODE,FUND_VALUE from CUST_FUND_DETAIL where TX_ACC=:1");
	m_pSqlCmd->setCommandText(m_sSql);
	m_pSqlCmd->Param(1).setAsString() = txacc;
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"日切 读取 交易账号[%s]的 资金详细信息 %s",txacc,m_sSql);
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
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"日切 读取 交易账号[%s]的 特殊费率设置 %s",txacc,m_sSql);
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
	sprintf(m_sSql,"select TX_ACC,SECU_NO,SECU_CODE,SEAT_ID,TX_CODE,TB_FLAG,LAST_PB,LAST_PS,CUR_PB,CUR_PS,CUR_CANUSE_PB,CUR_CANUSE_PS,DAY_PB,DAY_PS,DAY_COV_PB_FROZ,DAY_COV_PB,DAY_COV_PS_FROZ,DAY_COV_PS,PB_OPEN_AVG_PRICE,PS_OPEN_AVG_PRICE,PB_AVG_PRICE,PS_AVG_PRICE,PB_MARGIN,PS_MARGIN,DAY_PB_SURPLUS,DAY_PS_SURPLUS,LAST_SETTLE_PRICE,DAY_SETTLE_PRICE,VARIETY,SECU_NAME from t_position where TX_ACC=:1");
	m_pSqlCmd->setCommandText(m_sSql);
	m_pSqlCmd->Param(1).setAsString() = txacc;

	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"日切 读取 交易账号[%s]的 持仓信息 %s",txacc,m_sSql);
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

		cpinfo.dPB_OAvp =m_pSqlCmd->Field(18).asDouble();
		cpinfo.dPS_OAvp =m_pSqlCmd->Field(19).asDouble();
		cpinfo.dPB_Avp =m_pSqlCmd->Field(20).asDouble();
		cpinfo.dPS_Avp =m_pSqlCmd->Field(21).asDouble();
		cpinfo.dPB_Margin =m_pSqlCmd->Field(22).asDouble();
		cpinfo.dPS_Margin =m_pSqlCmd->Field(23).asDouble();
		cpinfo.dDAY_PB_SURPLUS =m_pSqlCmd->Field(24).asDouble();
		cpinfo.dDAY_PS_SURPLUS =m_pSqlCmd->Field(25).asDouble();
		cpinfo.dLAST_SETTLE_PRICE = m_pSqlCmd->Field(26).asDouble();
		cpinfo.dSETTLE_PRICE = m_pSqlCmd->Field(27).asDouble();
		cpinfo.nVarietyId = m_pSqlCmd->Field(28).asLong();

		strcpy(cpinfo.sContractName, m_pSqlCmd->Field(29).asString());

		S_MDB_CUST_POSITION *pcpinfo = m_pMemDb->m_pDbTrader.m_pPosition.Select(cpinfo.sTxAcc,cpinfo.sShFlag,cpinfo.sContractCode);
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
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"日切 读取 交易账号[%s]的 持仓信息 合约[%d %s] 投保[%s] 多仓[%d] 空仓[%d]",\
			pcpinfo->sTxAcc,pcpinfo->nContractNo,pcpinfo->sContractCode,pcpinfo->sShFlag,pcpinfo->nCurPB,pcpinfo->nCurPS);
		bret = m_pSqlCmd->FetchNext();
	}
	//出入金、报单、成交已在内存，不用重新读取了


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
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"日切 处理 交易账号[%s] 出入金[%d]条",pacclock->sTxAcc,flowlist.size());
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
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"日切 处理 交易账号[%s] 持仓[%d]条",pacclock->sTxAcc,posilis.size());
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
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"日切 处理 交易账号[%s] 成交[%d]条",pacclock->sTxAcc,donelist.size());
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
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"日切 处理 交易账号[%s] 报单[%d]条",pacclock->sTxAcc,orderlist.size());
	}
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"日切  读取 交易账号[%s]的信息 完成",txacc);
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
	S_MDB_TRADEACC_INFO *pfundacc = m_pMemDb->m_pDbCust.m_pTradeAcc.At(plock->nTradeAccRowid);
	if (pfundacc == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"交易账号[%s] not found",txacc);
		return false;
	}
	S_MDB_CUST_FUND *pfund = m_pMemDb->GetFund(plock);
	if (pfund == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"交易账号[%s %s] not found",txacc,coninfo->sCurrency);
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
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"交易账号[%s] 取风险级别出错",txacc);
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
	
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"开始更新报单表");
	int flag = order->nSyncFlag;
	sprintf(m_sSql,"select  REMAIN_VOL,CANCEL_FLAG,CANCEL_VOL from t_order_info where LOCAL_NO=:1");
	m_pSqlCmd->setCommandText(m_sSql);
	m_pSqlCmd->Param(1).setAsString() = order->sLocalSerial;
	if (!m_pSqlCmd->Execute())
	{
		m_pSqlCmd->Rollback();
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"Execute %s error %s",m_sSql,m_pSqlCmd->GetCmdErrInfo());
		return true;
	}
	if (!m_pSqlCmd->FetchNext())
	{
        m_pLog->LogMp(LOG_DEBUG, __FILE__, __LINE__, "无此报单[%s] 开始写入报单表", order->sLocalSerial);
        if (!UpdateOrder2Db(order))
        {
            return false;
        }
        if (order->nSyncFlag == flag)
        {
            order->nSyncFlag = 0;
        }
        m_pLog->LogMp(LOG_DEBUG, __FILE__, __LINE__, "写入报单表完成");
        return true;
		
	}
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
	sprintf(m_sSql,"update t_order_info set REMAIN_VOL=:1,CANCEL_FLAG=:2,CANCEL_VOL=:3,DONE_AVP=:4,ORDER_STATUS=:6,ORDER_RSP=:7,CANCEL_CHANNEL=:8,CANCEL_OPER=:9,CANCEL_TIME=:10 where LOCAL_NO=:5");
	m_pSqlCmd->setCommandText(m_sSql);
	m_pSqlCmd->Param(1).setAsLong() = order->nRemainNum;
		
	m_pSqlCmd->Param(2).setAsString() = order->sCancelFlag;
	m_pSqlCmd->Param(3).setAsLong() = order->nCancelNum;
	m_pSqlCmd->Param(4).setAsDouble() = order->dDoneAvp;
	m_pSqlCmd->Param(5).setAsString() = order->sLocalSerial;
	m_pSqlCmd->Param(6).setAsString() = order->sOrderStatus;
	m_pSqlCmd->Param(7).setAsString() = order->sOrderRsp;
	m_pSqlCmd->Param(8).setAsString() = order->sCancelChannel;
	m_pSqlCmd->Param(9).setAsString() = order->sCancelOper;
	m_pSqlCmd->Param(10).setAsString() = order->sCancelTime;
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

bool RiskDll::SyncDone(S_MDB_DONE_INFO *done)
{
	if (done->nSyncFlag == 3)
	{
		sprintf(m_sSql,"select DONE_NO from  t_match_info where DONE_NO=:1 and EX_CODE=:2 and BS_FLAG=:3 ");
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

		sprintf(m_sSql,"insert into t_match_info(DONE_NO,SYSTEM_NO,EX_CODE,SEAT_NO,TX_CODE,LOCAL_NO,TX_ACC,DONE_DATE,DONE_TIME,SECU_NO,SECU_CODE,SECU_NAME,DONE_PRICE,DONE_QTY,BS_FLAG,SH_FLAG,EO_FLAG,ORDER_PRICE,TX_TYPE,CMD_TYPE,CMD_ATTR,AMT,EX_FEE,MEMBER_FEE,EX_MARGIN,MEMBER_MARGIN,FLOAT_SURPLUS,COV_SURPLUS,TX_ORG,ACC_ORG,FORCE_FLAG,ORDER_CHANNEL,ORDER_OPER,SYSTEM_DATE,VARIETY,RULE_ALGO_CODE,CLEAR_SPEED) values(:1,:2,:3,:4,:5,:6,:7,:8,:9,:10,:11,:12,:13,:14,:15,:16,:17,:18,:19,:20,:21,:22,:23,:24,:25,:26,:27,:28,:29,:30,:31,:32,:33,:34,:35,:36,:37) ");
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
		m_pSqlCmd->Param(12).setAsString() = done->sContractName;
		m_pSqlCmd->Param(13).setAsDouble() = done->dDonePrice;
		m_pSqlCmd->Param(14).setAsLong() = done->nDoneNum;
		m_pSqlCmd->Param(15).setAsString() = done->sBsFlag;
		m_pSqlCmd->Param(16).setAsString() = done->sShFlag;
		m_pSqlCmd->Param(17).setAsString() = done->sEoFlag;
		m_pSqlCmd->Param(18).setAsDouble() = done->dOrderPrice;
		m_pSqlCmd->Param(19).setAsString() = done->sTxType;
		m_pSqlCmd->Param(20).setAsString() = done->sCmdType;
		m_pSqlCmd->Param(21).setAsString() = done->sCmdAttr;
		m_pSqlCmd->Param(22).setAsDouble() = done->dAmt;
		m_pSqlCmd->Param(23).setAsDouble() = done->dExFee;
		m_pSqlCmd->Param(24).setAsDouble() = done->dMemberFee;
		m_pSqlCmd->Param(25).setAsDouble() = done->dExMargin;
		m_pSqlCmd->Param(26).setAsDouble() = done->dMemberMargin;
		m_pSqlCmd->Param(27).setAsDouble() = done->dFloatSurplus;
		m_pSqlCmd->Param(28).setAsDouble() = done->dCovSurplus;
		m_pSqlCmd->Param(29).setAsString() = done->sTxOrg;
		m_pSqlCmd->Param(30).setAsString() = done->sAccOrg;
		m_pSqlCmd->Param(31).setAsString() = done->sForceFlag;
		m_pSqlCmd->Param(32).setAsString() = done->sOrderChannel;
		m_pSqlCmd->Param(33).setAsString() = done->sOrderOper;
		m_pSqlCmd->Param(34).setAsString() = done->sSystemDate;
		m_pSqlCmd->Param(35).setAsLong() = done->nVarietyId;
		m_pSqlCmd->Param(36).setAsString() = done->sRuleAlgoCode;
		m_pSqlCmd->Param(37).setAsLong() = done->nClearSpeed;
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
bool RiskDll::SyncBond(S_MDB_BOND_INFO* bond)
{
    sprintf(m_sSql, "update  sys_bond_ext_info set MARKET_SCOPE=:2,DV01=:3,APPR_VALUE=:4   where BOND_CODE=:1 ");
    m_pSqlCmd->setCommandText(m_sSql);
    m_pSqlCmd->Param(1).setAsString() = bond->sBondCode;
    m_pSqlCmd->Param(2).setAsDouble() = bond->dMarketScope;
    m_pSqlCmd->Param(3).setAsDouble() = bond->dDv01;
	m_pSqlCmd->Param(4).setAsDouble() = bond->dApprValue;
    if (!m_pSqlCmd->Execute())
    {
		m_pSqlCmd->Rollback();
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "Execute %s error %s", m_sSql, m_pSqlCmd->GetCmdErrInfo());
        return false;
    }  
    m_pSqlCmd->Commit();
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
			//某一个交易账号
			//根据交易账号去取资金，结存及保证金
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
			//某一个交易账号的持仓
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
		double posiValue=GetProdPosiTotalValue(risk.sProdCode);	//取产品持仓总规模
		risk.nRiskLevel = m_pKernelPub.GetProdFundRiskLevel(risk.sProdCode,risk.dUnitValue,risk.dRisk, posiValue,traderight,risk.sRiskDesc);
		strcpy(risk.sRight,traderight);

		posirisk.nRiskLevel = m_pKernelPub.GetProdPosiRiskLevel(risk.sProdCode,posirisk.dMarginRate,posirisk.dPosiValueRate,posirisk.dPosiDiffValueRate,posirisk.sRight,posirisk.sRiskDesc);
		
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
				//Send2OperProdOrgRiskMsg(pProd->sOrgCode,risk.sRight[0],riskdata,riskdatalen,9821);
				ReleaseBroadcast(riskdata,riskdatalen,0,atoi(risk.sProdCode),9932);
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
				//Send2OperProdOrgRiskMsg(pProd->sOrgCode,risk.sRight[0],riskdata,riskdatalen,9822);
				ReleaseBroadcast(riskdata, riskdatalen, 0, atoi(risk.sProdCode), 9933);
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
	std::vector<S_MDB_TRADEACC_INFO*> orgcustlist;
	std::vector<S_MDB_TRADEACC_INFO*> orgmgrcustlist;
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
			if (!m_pMemDb->m_pDbCust.m_pTradeAcc.SelectAll(orgcustlist))
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
				m_pMemDb->m_pDbCust.m_pTradeAcc.SelectByAccOrg(calcorglist[j]->sOrgCode,orgcustlist);

				m_pMemDb->m_pDbCust.m_pTradeAcc.SelectByMgrOrg(calcorglist[j]->sOrgCode,orgmgrcustlist);
			}
		}
		ComputeOneOrg(compdate,orglist[i]->sOrgCode,orglist[i]->sOrgName,"0",orgcustlist);//计算所属机构
		ComputeOneOrg(compdate,orglist[i]->sOrgCode,orglist[i]->sOrgName,"1",orgmgrcustlist);//计算代理机构
	}
	return true;
}
bool RiskDll::ComputeOneOrg(const char *compdate,char *orgcode, const char *orgname,const char *risktype,std::vector<S_MDB_TRADEACC_INFO*> orgcustlist)
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
		//某一个交易账号
		//根据交易账号去取资金，结存及保证金
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
		//某一个交易账号的持仓
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
	double posiValue=GetOrgPosiTotalValue(risk.sOrgCode); //取机构持仓总规模
	risk.nRiskLevel = m_pKernelPub.GetOrgFundRiskLevel(risk.sOrgCode,risk.dUnitValue,risk.dRisk, posiValue,traderight,risk.sRiskDesc);
	strcpy(risk.sRight,traderight);

	posirisk.nRiskLevel = m_pKernelPub.GetOrgPosiRiskLevel(risk.sOrgCode,posirisk.dMarginRate,posirisk.dPosiValueRate,posirisk.dPosiDiffValueRate,posirisk.sRight,posirisk.sRiskDesc);

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
			ReleaseBroadcast(riskdata,riskdatalen,0,atoi(risk.sOrgCode),9934);	//公共key是机构代码
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
			ReleaseBroadcast(riskdata, riskdatalen, 0, atoi(risk.sOrgCode), 9935);	//公共key是机构代码
		}
	}

	return true;
}

void RiskDll::SendOrderBroadcast(S_MDB_TXACC_LOCK *acclock,S_MDB_ORDER_INFO *info)
{
	//准备数据
	m_pXdp->ResetData();
	Xdp2Struct::ConvertOrderBroadcast2Xdp(*info,m_pXdp);
	
	S_MDB_TRADEACC_INFO *pFundAcc=NULL;

	pFundAcc = m_pMemDb->m_pDbCust.m_pTradeAcc.At(acclock->nTradeAccRowid);
	if (pFundAcc != NULL)
	{
		m_pXdp->SetFieldValue((unsigned short)110,pFundAcc->sAccName,strlen(pFundAcc->sAccName),m_sErrMsg);
	} 
	char sPushdata[2000];
	unsigned int len = sizeof(sPushdata);
	if (!m_pXdp->ToBuffer(sPushdata,len,m_sErrMsg))
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"Xdp ToBuffer error");
		return ;
	}
	ReleaseBroadcast(sPushdata, len, atoi(pFundAcc->sTxAcc), 0, 9936);
}

void RiskDll::SendDoneBroadcast(S_MDB_TXACC_LOCK *acclock ,S_MDB_DONE_INFO *info)
{
	//准备数据
	m_pXdp->ResetData();
	Xdp2Struct::ConvertDoneBroadcast2Xdp(*info,m_pXdp);
	S_MDB_TRADEACC_INFO *pFundAcc=NULL;

	pFundAcc = m_pMemDb->m_pDbCust.m_pTradeAcc.At(acclock->nTradeAccRowid);
	if (pFundAcc != NULL)
	{
		m_pXdp->SetFieldValue((unsigned short)110, pFundAcc->sAccName,strlen(pFundAcc->sAccName),m_sErrMsg);
	} 
	char sPushdata[2000];
	unsigned int len = sizeof(sPushdata);
	if (!m_pXdp->ToBuffer(sPushdata,len,m_sErrMsg))
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"Xdp ToBuffer error");
		return ;
	}
    ReleaseBroadcast(sPushdata,len,atoi(pFundAcc->sTxAcc),0,9938);
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
	S_MDB_TRADEACC_INFO *pFundAcc=NULL;

	pFundAcc = m_pMemDb->m_pDbCust.m_pTradeAcc.At(acclock->nTradeAccRowid);
	if (pFundAcc != NULL)
	{
		m_pXdp->SetFieldValue((unsigned short)119,pFundAcc->sAccName,strlen(pFundAcc->sAccName),m_sErrMsg);
	} 
	char sPushdata[1000];
	unsigned int len = sizeof(sPushdata);
	if (!m_pXdp->ToBuffer(sPushdata,len,m_sErrMsg))
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"Xdp ToBuffer error");
		return ;
	}
 /*   Send2CustPushMsg(acclock, sPushdata, len, 9835);
    Send2OperPushMsg(acclock, sPushdata, len, 9835);*/
}


int RiskDll::ProcessDone(S_MDB_TRADEACC_INFO *pcustacc, S_MDB_ORDER_INFO *po,S_MDB_CONTRACT_INFO *pc,S_MDB_EXCH_INFO *pexch,S_MDB_DONE_INFO &info)
{
	
	info.dUnfrozeFee = po->dExFee * info.nDoneNum/po->nOrderNum*(-1.0);  //变为正数
	info.dUnfrozeAmt = po->dFrozAmt * info.nDoneNum/po->nOrderNum*(-1.0);  //变为负数

	
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
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"成交通知取手续费出错 本地报单号[%s] 交易账号[%s] 成交号[%s]",info.sLocalNo,info.sTxAcc,info.sDoneNo);
					return -1;
				}
				if (!m_pKernelPub.GetMarginValue(pc,pcustacc,0,info.nDoneNum,info.dAmt,dExchMargin,dMemberMargin))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"成交通知取保证金出错 本地报单号[%s] 交易账号[%s] 成交号[%s]",info.sLocalNo,info.sTxAcc,info.sDoneNo);
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
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"成交通知取手续费出错 本地报单号[%s] 交易账号[%s] 成交号[%s]",info.sLocalNo,info.sTxAcc,info.sDoneNo);
						return -1;
					}
				}
				else
				{
					//平仓要算手续费
					if (!m_pKernelPub.GetTradeFeeValue(pc,pcustacc,2,info.nDoneNum,info.dAmt,dExchFee,dMemberFee))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"成交通知取手续费出错 本地报单号[%s] 交易账号[%s] 成交号[%s]",info.sLocalNo,info.sTxAcc,info.sDoneNo);
						return -1;
					}
				}
			}
			break;
		case PROD_TYPE_OPTION://期权
		case PROD_TYPE_OPTION_SEC://期权
			if (info.sEoFlag[0] == EO_FLAG_OPEN) //开仓
			{
				//开仓要算冻结和手续费
				if (!m_pKernelPub.GetTradeFeeValue(pc,pcustacc,1,info.nDoneNum,info.dAmt,dExchFee,dMemberFee))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"成交通知取手续费出错 本地报单号[%s] 交易账号[%s] 成交号[%s]",info.sLocalNo,info.sTxAcc,info.sDoneNo);
					return -1;
				}
				if (info.sBsFlag[0] == BS_FLAG_SELL) //卖开要冻结保证金
				{
					if (!m_pKernelPub.GetMarginValue(pc,pcustacc,1,info.nDoneNum,info.dAmt,dExchMargin,dMemberMargin))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"成交通知取保证金出错 本地报单号[%s] 交易账号[%s] 成交号[%s]",info.sLocalNo,info.sTxAcc,info.sDoneNo);
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
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"成交通知取手续费出错 本地报单号[%s] 交易账号[%s] 成交号[%s]",info.sLocalNo,info.sTxAcc,info.sDoneNo);
						return -1;
					}
				}
				else
				{
					//平仓要算手续费
					if (!m_pKernelPub.GetTradeFeeValue(pc,pcustacc,2,info.nDoneNum,info.dAmt,dExchFee,dMemberFee))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"成交通知取手续费出错 本地报单号[%s] 交易账号[%s] 成交号[%s]",info.sLocalNo,info.sTxAcc,info.sDoneNo);
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
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"成交通知取手续费出错 本地报单号[%s] 交易账号[%s] 成交号[%s]",info.sLocalNo,info.sTxAcc,info.sDoneNo);
						return -1;
					}
				}
				else
				{
					//卖要算手续费	
					if (!m_pKernelPub.GetTradeFeeValue(pc,pcustacc,2,info.nDoneNum,info.dAmt,dExchFee,dMemberFee))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"成交通知取手续费出错 本地报单号[%s] 交易账号[%s] 成交号[%s]",info.sLocalNo,info.sTxAcc,info.sDoneNo);
						return -1;
					}
				}
			}
			break;
		case PROD_TYPE_BOND_BANK://银行间债券
		case PROD_TYPE_BOND://交易所债券
		case PROD_TYPE_SEC://证券
			if (info.sBsFlag[0] == BS_FLAG_BUY)
			{
				//买要算手续费及金额
				if (!m_pKernelPub.GetTradeFeeValue(pc,pcustacc,1,info.nDoneNum,info.dAmt,dExchFee,dMemberFee))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"成交通知取手续费出错 本地报单号[%s] 交易账号[%s] 成交号[%s]",info.sLocalNo,info.sTxAcc,info.sDoneNo);
					return -1;
				}
			}
			else
			{
				//卖要算手续费
				if (!m_pKernelPub.GetTradeFeeValue(pc,pcustacc,2,info.nDoneNum,info.dAmt,dExchFee,dMemberFee))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"成交通知取手续费出错 本地报单号[%s] 交易账号[%s] 成交号[%s]",info.sLocalNo,info.sTxAcc,info.sDoneNo);
					return -1;
				}
			}
			break;
		default:
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"成交通知合约代码[%s]的品种类别[%s]不符 交易账号[%s] 成交号[%s]",info.sContractCode,pc->sProd_type,info.sTxAcc,info.sDoneNo);
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
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"成交通知 找不到交易账号信息 [%s]",info.sTxAcc);
		return -1;
	}
	S_MDB_CUST_FUND *pfund = m_pMemDb->GetFund(acclock);
	if (pfund == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"成交通知 找不到交易账号资金信息 [%s]",info.sTxAcc);
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
	m_pMemDb->m_pDbTrader.m_pDone.SetUpdate(rid);//设置成交写入db的标志
	//更新资金、冻结等
	if (!m_pKernelPub.ComputeNewDone(acclock,pfund,pdone,po,pc))
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"成交通知 更新资金 持仓等失败  本地报单号[%s] 交易账号[%s] 成交号[%s]",info.sLocalNo,info.sTxAcc,info.sDoneNo);
		return -1;
	}
	//通知要对此客户进行风险计算
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"CUST acclock rowid :%d",acclock->nLockRowid);
	MsgCustRiskCalc(acclock->nLockRowid);

	m_nEndTime = CBF_Date_Time::GetTickUS();
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"成交通知 处理完成 成交号[%s] 本地报单号[%s] 耗时[%ld]微秒",info.sDoneNo,info.sLocalNo,m_nEndTime-m_nBeginTime);
    if (*g_nSvrHostStatus == ARBSTATUS_MASTER || *g_nSvrHostStatus == ARBSTATUS_MASTER_AYN || *g_nSvrHostStatus == MASTERHOST)
    {
        //是主机发送回报推送
        SendOrderBroadcast(acclock, po);
        SendDoneBroadcast(acclock, pdone);
    }
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
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"无此交易账号[%s]",txacc);
			return false;
		}
		lockrowid = info->nLockRowid;
		return true;
	}
	info =  m_pMemDb->SelectLock((char *)txacc);
	if (info == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"无此交易账号[%s]",txacc);
		return false;
	}
	lockrowid = info->nLockRowid;
	S_MDB_CUST_FUND *fund = m_pMemDb->GetFund(info);
	if (fund == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"资金表无此交易账号[%s]",txacc);
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
    INT64_  nBeginTime = CBF_Date_Time::GetTickUS();
	if (!m_pXdp->FromBuffer(data.pData->sBuffer,data.pData->sDBHead.nLen,m_sErrMsg))
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"请求非XDP数据 [%s]",m_sErrMsg);
		data.pData->sDBHead.nLen = 0;
		data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_INPUT;
		AnsData(data);
		return -1;
	}
	
	S_MDB_ORDER_INFO order;
	bzero(&order,sizeof(S_MDB_ORDER_INFO));
	Xdp2Struct::ConvertOrder(m_pXdp,order);
	int nRet;
	S_ORDER_PUB order_pub;
	//报单检查
	nRet = OrderCheck(order,order_pub);
	if (nRet != 0)
	{
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "报单检查未通过");
        data.pData->sDBHead.nLen = 0;
        data.pData->sDBHead.a_Ainfo.a_nRetCode = nRet;
        AnsData(data);
        return -1;
	}
	order.dAmt = order.dOrderPrice* order_pub.pc->dTxUnit*order.nOrderNum;//金额

	//生成本地报单号
	sprintf(order.sLocalSerial,INT64PFM,g_pPubData->GetLocalSerial());//  会定时写到文件里面，重启从文件读取并根据报单表的最大本地单号更新
	order.nSyncFlag = 1; //报入同步
	int orderrid;
	//报单状态默认为正申报
	order.sOrderStatus[0] = ORDER_STATUS_SENDOK;//正在申报
	order.sOrderStatus[1]=0;
	//复制一份xdp
    nRet = m_pKernelPub.GetOrderFeeAmt(order, order_pub.pc, order_pub.pCustFuncAcc);
    if (nRet != 0)
    {
        order.sOrderStatus[0] = ORDER_STATUS_INVALID;//无效单
        order.sOrderStatus[1] = 0;
		data.pData->sDBHead.a_Ainfo.a_nRetCode = nRet;
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "交易账号[%s]交易所代码[%s]席位号[%s]本地报单号[%s]取手续费保证金出错 [%d]", order.sTxAcc, order.sExchCode, order.sSeatNo, order.sLocalSerial, nRet);
        data.pData->sDBHead.nLen = 0;
        AnsData(data);
        //写入报单表
        orderrid = m_pMemDb->m_pDbTrader.m_pOrder.Insert(order);
        if (orderrid < 0)
        {
            m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托 写委托表出错 本地报单号[%s]", order.sLocalSerial);
			return -1;
        }
		m_pMemDb->m_pDbTrader.m_pOrder.SetUpdate(orderrid);
        //同步到备机
        Xdp2Struct::ConvertOrderHost2Xdp(order, &m_pSendXdp);
        ReleaseBroadcast(&m_pSendXdp, 0, 0, 9926);
        return -1;
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
	
	order_pub.pOrder = m_pMemDb->m_pDbTrader.m_pOrder.At(orderrid);
	if (order_pub.pOrder == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托  委托表出错 rowid[%d]找不到委托",orderrid);
		data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_SYSTEM;
		data.pData->sDBHead.nLen = 0;
		AnsData(data);
		return -1;
	}
    //同步到备机
    Xdp2Struct::ConvertOrderHost2Xdp(order, &m_pSendXdp);
    ReleaseBroadcast(&m_pSendXdp, 0, 0, 9926);
	//冻结资金持仓等等
	nRet = m_pKernelPub.ComputeNewOrder(order_pub.pAcclock, order_pub.pCustFund, order_pub.pOrder, order_pub.pc,true);
	if (nRet != SUCCESS)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托 计算出错 返回[%d]",nRet);
		data.pData->sDBHead.a_Ainfo.a_nRetCode = nRet;
		data.pData->sDBHead.nLen = 0;
		AnsData(data);
		order_pub.pOrder->sOrderStatus[0] = ORDER_STATUS_INVALID;//无效单
		order_pub.pOrder->sOrderStatus[1]=0;
		strcpy(order_pub.pOrder->sOrderRsp,"委托计算出错");
		order_pub.pOrder->nSyncFlag = 5; //回报同步
		m_pMemDb->m_pDbTrader.m_pOrder.SetUpdate(orderrid);
		return -1;
	}
	m_pMemDb->m_pDbTrader.m_pOrder.SetUpdate(orderrid);
	//发送报单给柜台或交易所
	S_TRADE_DATA rpcdata;
	//申请一块内存
    if (m_pClientLink->PoolMalloc(rpcdata) < 0)
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "PoolMalloc error");
        data.pData->sDBHead.a_Ainfo.a_nRetCode = nRet;
        data.pData->sDBHead.nLen = 0;
        AnsData(data);
        order_pub.pOrder->sOrderStatus[0] = ORDER_STATUS_INVALID;//无效单
        order_pub.pOrder->sOrderStatus[1] = 0;
        strcpy(order_pub.pOrder->sOrderRsp, "PoolMalloc error");
        order_pub.pOrder->nSyncFlag = 5; //回报同步
        m_pMemDb->m_pDbTrader.m_pOrder.SetUpdate(orderrid);
        return -1;
    }
	unsigned int len;
	m_nRpcSerial = m_pClientLink->GetSerial();
	Xdp2Struct::ConvertOrderBroadcast2Xdp(*(order_pub.pOrder), m_pXdp);

    len = sizeof(rpcdata.pData->sBuffer);
	//生成rpc的xdp数据
    if (!m_pXdp->ToBuffer(rpcdata.pData->sBuffer, len, m_sErrMsg))
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "xdp ToBuffer error");
        data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_SYSTEM;
        data.pData->sDBHead.nLen = 0;
        AnsData(data);
        order_pub.pOrder->sOrderStatus[0] = ORDER_STATUS_INVALID;//无效单
        order_pub.pOrder->sOrderStatus[1] = 0;
        strcpy(order_pub.pOrder->sOrderRsp, "xdp ToBuffer error");
        order_pub.pOrder->nSyncFlag = 5; //回报同步
        m_pMemDb->m_pDbTrader.m_pOrder.SetUpdate(orderrid);
		m_pClientLink->PoolFree(rpcdata);
		return -1;
    }
	//准备报文头
	bzero(&(rpcdata.pData->sDBHead),DREBHEADLEN);
	rpcdata.pData->sDBHead.nLen = len;
	rpcdata.pData->sDBHead.cCmd = CMD_DPCALL;
	rpcdata.pData->sDBHead.d_Dinfo.d_nServiceNo = 9924;   // 交易所报单接口
	rpcdata.pData->sDBHead.d_Dinfo.d_nSvrMainId = atoi(order_pub.pExTrader->sPara);//交易所、席位对应的在总线上注册的服务号
	//rpc调用
    if (m_pClientLink->RpcRequest(rpcdata, m_nRpcSerial, [=](S_TRADE_DATA& rpcdata, S_TRADE_DATA& ansdata) -> int
        {
			//报单响应处理
			return OrderRsp(rpcdata,ansdata);
        }

    ) < 0)
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "调用 RpcRequest 返回<0");
        data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_SYSTEM;
        data.pData->sDBHead.nLen = 0;
        AnsData(data);
		order_pub.pOrder->sOrderStatus[0] = ORDER_STATUS_INVALID;//无效单
		order_pub.pOrder->sOrderStatus[1] = 0;
        strcpy(order_pub.pOrder->sOrderRsp, "发送报单 出错");
		order_pub.pOrder->nSyncFlag = 5; //回报同步
		m_pMemDb->m_pDbTrader.m_pOrder.SetUpdate(orderrid);
        return -1;
    }
    //应答回去
	Xdp2Struct::ConvertOrderBroadcast2Xdp(*(order_pub.pOrder),m_pXdp);
	len = sizeof(data.pData->sBuffer);
	if (!m_pXdp->ToBuffer(data.pData->sBuffer, len, m_sErrMsg))
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "RpcRequest 返回<0");
		data.pData->sDBHead.nLen = 0;
	}
	else
	{
		data.pData->sDBHead.nLen = len;
	}
    data.pData->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
    AnsData(data);
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"报单[%s] 内部处理延时[%d]微秒",order_pub.pOrder->sLocalSerial, CBF_Date_Time::GetTickUS()- nBeginTime);
	return 0;
}

int RiskDll::BilateralOrder(S_TRADE_DATA& data)
{
    if (!m_pXdp->FromBuffer(data.pData->sBuffer, data.pData->sDBHead.nLen, m_sErrMsg))
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "请求非XDP数据 [%s]", m_sErrMsg);
        data.pData->sDBHead.nLen = 0;
        data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_INPUT;
        AnsData(data);
        return -1;
    }
    //101	varchar2	席位代码	6	n	没有席位默认’$’
    //102	varchar3	交易账号	20	Y	交易账号
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
    bzero(&order, sizeof(S_MDB_ORDER_INFO));
    unsigned int len;
    int nRet;
    char sChannel[11];
    char sTxOrg[21];
    bzero(sChannel, sizeof(sChannel));
    bzero(sTxOrg, sizeof(sTxOrg));
    int fieldtype;
    short nShortValue;
    S_MDB_CONTRACT_INFO* pc = NULL;
    S_MDB_QUOTATION* pq = NULL;
    S_MDB_EXCH_INFO* pe = NULL;
    S_MDB_CONTRACT_SETTLE_PRICE* psp = NULL;
    S_MDB_VARIETY_INFO* pv = NULL;
    S_MDB_TRADEACC_INFO* pCustFuncAcc = NULL;
    S_MDB_CUSTINFO* pCust = NULL;
    S_MDB_CUST_FUND* pCustFund = NULL;
    S_MDB_ORG_INFO* pOrg = NULL;
    S_MDB_TXACC_LOCK* pAcclock = NULL;
    S_MDB_ORDER_INFO* pOrder = NULL;
    bool iscust = false;

    len = sizeof(order.sSeatNo);
    if (!m_pXdp->GetFieldValue((unsigned short)101, order.sSeatNo, len, fieldtype, m_sErrMsg))
    {
        strcpy(order.sSeatNo, "$");
    }
    len = sizeof(order.sTxAcc);
    if (!m_pXdp->GetFieldValue((unsigned short)102, order.sTxAcc, len, fieldtype, m_sErrMsg))
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "Input error [%s]", m_sErrMsg);
        data.pData->sDBHead.nLen = 0;
        data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_INPUT;
        AnsData(data);
        return -1;
    }
    len = sizeof(order.sClientSerial);
    if (!m_pXdp->GetFieldValue((unsigned short)108, order.sClientSerial, len, fieldtype, m_sErrMsg))
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "Input error [%s]", m_sErrMsg);
        data.pData->sDBHead.nLen = 0;
        data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_INPUT;
        AnsData(data);
        return -1;
    }
    if (!m_pXdp->GetFieldValue((unsigned short)20, order.nContractNo, m_sErrMsg))
    {
        len = sizeof(order.sContractCode);
        if (!m_pXdp->GetFieldValue((unsigned short)100, order.sContractCode, len, fieldtype, m_sErrMsg))
        {
            m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "Input error [%s]", m_sErrMsg);
            data.pData->sDBHead.nLen = 0;
            data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_INPUT;
            AnsData(data);
            return -1;
        }
        pc = m_pMemDb->m_pDbBaseinfo.m_pContract.Select(order.sContractCode);
        if (pc == NULL)
        {
            m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "合约代码 [%s] 不存在", order.sContractCode);
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
            m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "合约编号 [%d] 不存在", order.nContractNo);
            data.pData->sDBHead.nLen = 0;
            data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_CONTRACTINPUT;
            AnsData(data);
            return -1;
        }
        strcpy(order.sContractCode, pc->sContractCode);
    }
    strcpy(order.sExchCode, pc->sExcode);
    //取交易所信息
    pe = m_pMemDb->m_pDbBaseinfo.m_pExchange.Select(pc->sExcode);
    if (pe == NULL)
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "合约编号[%d %s] 的交易所[%s]不存在", order.nContractNo, order.sContractCode, pc->sExcode);
        data.pData->sDBHead.nLen = 0;
        data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_EXCHANGE;
        AnsData(data);
        return -1;
    }
    if (!m_pXdp->GetFieldValue((unsigned short)5, nShortValue, m_sErrMsg))
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托无投保标志 %s", m_sErrMsg);
        data.pData->sDBHead.nLen = 0;
        data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_SHFLAG;
        AnsData(data);
        return -1;
    }
    if (nShortValue <MINASCIINUM || nShortValue>MAXASCIINUM)
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托投保标志不符 %d", nShortValue);
        data.pData->sDBHead.nLen = 0;
        data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_SHFLAG;
        AnsData(data);
        return -1;
    }
    order.sShFlag[0] = nShortValue;
    if (!m_pXdp->GetFieldValue((unsigned short)6, nShortValue, m_sErrMsg))
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托无开平标志 %s", m_sErrMsg);
        data.pData->sDBHead.nLen = 0;
        data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_EOFLAG;
        AnsData(data);
        return -1;
    }
    if (nShortValue <MINASCIINUM || nShortValue>MAXASCIINUM)
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托开平标志不符 %d", nShortValue);
        data.pData->sDBHead.nLen = 0;
        data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_EOFLAG;
        AnsData(data);
        return -1;
    }
    order.sEoFlag[0] = nShortValue;

    if (!m_pXdp->GetFieldValue((unsigned short)7, nShortValue, m_sErrMsg))
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托无买卖标志 %s", m_sErrMsg);
        data.pData->sDBHead.nLen = 0;
        data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_BSFLAG;
        AnsData(data);
        return -1;
    }
    if (nShortValue <MINASCIINUM || nShortValue>MAXASCIINUM)
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托买卖标志不符 %d", nShortValue);
        data.pData->sDBHead.nLen = 0;
        data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_BSFLAG;
        AnsData(data);
        return -1;
    }
    order.sBsFlag[0] = nShortValue;

    if (!m_pXdp->GetFieldValue((unsigned short)8, nShortValue, m_sErrMsg))
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托无交易类型 %s", m_sErrMsg);
        data.pData->sDBHead.nLen = 0;
        data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_BSFLAG;
        AnsData(data);
        return -1;
    }
    if (nShortValue <MINASCIINUM || nShortValue>MAXASCIINUM)
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托交易类型不符 %d", nShortValue);
        data.pData->sDBHead.nLen = 0;
        data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_TXTYPE;
        AnsData(data);
        return -1;
    }
    order.sTxType[0] = nShortValue;

    if (!m_pXdp->GetFieldValue((unsigned short)9, nShortValue, m_sErrMsg))
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托无报单类型 %s", m_sErrMsg);
        data.pData->sDBHead.nLen = 0;
        data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_ORDERTYPE;
        AnsData(data);
        return -1;
    }
    if (nShortValue <MINASCIINUM || nShortValue>MAXASCIINUM)
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托报单类型不符 %d", nShortValue);
        data.pData->sDBHead.nLen = 0;
        data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_ORDERTYPE;
        AnsData(data);
        return -1;
    }
    order.sCmdType[0] = nShortValue;

    if (!m_pXdp->GetFieldValue((unsigned short)13, nShortValue, m_sErrMsg))
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托无报单属性 %s", m_sErrMsg);
        data.pData->sDBHead.nLen = 0;
        data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_ORDERATTR;
        AnsData(data);
        return -1;
    }
    if (nShortValue <MINASCIINUM || nShortValue>MAXASCIINUM)
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托报单属性不符 %d", nShortValue);
        data.pData->sDBHead.nLen = 0;
        data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_ORDERATTR;
        AnsData(data);
        return -1;
    }
    order.sCmdAttr[0] = nShortValue;

    order.nRemainNum = order.nOrderNum;//剩余数量默认为委托数量
    order.sCancelFlag[0] = BOOL_FALSE;
    order.sCancelFlag[1] = 0;

    //强平标志
    if (m_pXdp->GetFieldValue((unsigned short)10, nShortValue, m_sErrMsg))
    {
        if (nShortValue != BOOL_ASC_FALSE && nShortValue != BOOL_ASC_TRUE)
        {
            m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托强平标志不符 %d", nShortValue);
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
    if (!m_pXdp->GetFieldValue((unsigned short)41, order.nOrderNum, m_sErrMsg))
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托无委托数量 %s", m_sErrMsg);
        data.pData->sDBHead.nLen = 0;
        data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_BSFLAG;
        AnsData(data);
        return -1;
    }
    if (order.nOrderNum < 1)
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托委托数量不符 %d", order.nOrderNum);
        data.pData->sDBHead.nLen = 0;
        data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_BSFLAG;
        AnsData(data);
        return -1;
    }
    //委托价格

    //如果是市价委托，取合约的最新价，以便计算手续费、保证金
    if (order.sCmdType[0] == ORDER_TYPE_MARKET || order.sCmdType[0] == ORDER_TYPE_MS_LOSS || order.sCmdType[0] == ORDER_TYPE_MS_PROFIT || order.sCmdType[0] == ORDER_TYPE_LEVEL1_M || order.sCmdType[0] == ORDER_TYPE_LEVEL5_M)
    {
        pq = m_pMemDb->m_pDbTrader.m_pQuotation.Select(order.nContractNo);
        if (pq == NULL)
        {
            //行情没有，取结算价
            psp = m_pMemDb->m_pDbBaseinfo.m_pSettlePrice.Select(order.nContractNo);
            if (psp == NULL)
            {
                m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托  市价无行情且取不到此合约[%d %s]的结算价 %s", order.nContractNo, order.sContractCode);
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
        if (!m_pXdp->GetFieldValue((unsigned short)60, order.dOrderPrice, m_sErrMsg))
        {
            m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托无委托价格 %s", m_sErrMsg);
            data.pData->sDBHead.nLen = 0;
            data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_ORDERPRICE;
            AnsData(data);
            return -1;
        }
    }
    //发起渠道
    len = sizeof(sChannel);
    if (!m_pXdp->GetFieldValue((unsigned short)91, sChannel, len, fieldtype, m_sErrMsg))
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托 发起渠道未输入 [%s]", m_sErrMsg);
        data.pData->sDBHead.nLen = 0;
        data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_CHANNEL;
        AnsData(data);
        return -1;
    }
    strcpy(order.sOrderChannel, sChannel);
    //客户、交易员
    len = sizeof(order.sOrderOper);
    if (!m_pXdp->GetFieldValue((unsigned short)92, order.sOrderOper, len, fieldtype, m_sErrMsg))
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托 发起客户/交易员未输入 [%s]", m_sErrMsg);
        data.pData->sDBHead.nLen = 0;
        data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_CUSTOPER;
        AnsData(data);
        return -1;
    }
    len = sizeof(sTxOrg);
    if (!m_pXdp->GetFieldValue((unsigned short)93, sTxOrg, len, fieldtype, m_sErrMsg))
    {
        m_pLog->LogMp(LOG_PROMPT, __FILE__, __LINE__, "委托 交易网点未输入，取资金账户开户网点");
    }
    //取品种
    if (pc->sProd_type[0] == PROD_TYPE_OPTION)
    {
        S_MDB_CONTRACT_INFO* pco = m_pMemDb->m_pDbBaseinfo.m_pContract.Select(pc->nProdId);
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
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托 合约[%d %s] 取品种信息出错", pc->nContractId, pc->sContractCode);
        data.pData->sDBHead.nLen = 0;
        data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_SYSTEMPARA;
        AnsData(data);
        return -1;
    }
    //是否有此交易账号
    pAcclock = m_pMemDb->SelectLock(order.sTxAcc);
    if (pAcclock == NULL)
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托 交易账号[%s]非法不存在", order.sTxAcc);
        data.pData->sDBHead.nLen = 0;
        data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_CUSTFUNDACC;
        AnsData(data);
        return -1;
    }
    pCustFuncAcc = m_pMemDb->m_pDbCust.m_pTradeAcc.At(pAcclock->nTradeAccRowid);
    if (pCustFuncAcc == NULL)
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托 交易账号[%s] 非法，不存在", order.sTxAcc);
        data.pData->sDBHead.nLen = 0;
        data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_CUSTFUNDACC;
        AnsData(data);
        return -1;
    }
    pCust = m_pMemDb->m_pDbCust.m_pCustom.At(pAcclock->nCustRowid);
    //	pCust = m_pMemDb->m_pDbCust.m_pCustom.SelectByNo(pCustFuncAcc->sCustNo);
    if (pCust == NULL)
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托 交易账号[%s]的客户号[%s]不存在", order.sTxAcc, pCustFuncAcc->sCustNo);
        data.pData->sDBHead.nLen = 0;
        data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_CUSTNO;
        AnsData(data);
        return -1;
    }
    //比较交易账号对应的客户号是否和输入的相同
    if (strcmp(pCustFuncAcc->sCustNo, order.sOrderOper) == 0)
    {
        iscust = true;
    }
    else
    {
        iscust = false;//不是客户发起
    }
    //取资金表，按账号+币种
    pCustFund = m_pMemDb->GetFund(pAcclock);
    if (pCustFund == NULL)
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托 交易账号[%s]+合约[%d %s]对应的币种[%s]在资金表不存在", order.sTxAcc, order.nContractNo, order.sContractCode, pc->sCurrency);
        data.pData->sDBHead.nLen = 0;
        data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_CUSTFUND;
        AnsData(data);
        return -1;
    }
    //判断开通渠道
    int nChannel = atoi(order.sOrderChannel);
    if (nChannel <= 0 || nChannel > 59)
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托渠道[%s]非法", order.sOrderChannel);
        data.pData->sDBHead.nLen = 0;
        data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_CHANNEL;
        AnsData(data);
        return -1;
    }
    if (pCust->sChannleOpen[nChannel] != '1')
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托 客户号[%s]交易账号[%s]未开通此渠道[%s]", pCustFuncAcc->sCustNo, order.sTxAcc, order.sOrderChannel);
        data.pData->sDBHead.nLen = 0;
        data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_CHANNELOPEN;
        AnsData(data);
        return -1;
    }
    if (!iscust) //判断操作员是否有此账号权限
    {
        if (!m_pKernelPub.CheckOperAccRight(order.sOrderOper, order.sTxAcc))
        {
            m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托 操作员[%s] 无交易账号[%s] 的操作权限", order.sOrderOper, order.sTxAcc);
            data.pData->sDBHead.nLen = 0;
            data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_NO_RIGHT;
            AnsData(data);
            return -1;
        }
    }
    strcpy(order.sTxDate, pe->sTradeDate);
    m_pDate.Update();
    strcpy(order.sAnsTime, m_pDate.ToStringDT17().c_str());
    strcpy(order.sAccOrg, pCustFuncAcc->sSignOrg);
    //取交易网点
    if (strlen(sTxOrg) < 1)
    {
        strcpy(order.sTxOrg, pCustFuncAcc->sSignOrg);
    }
    else
    {
        strcpy(order.sTxOrg, sTxOrg);
    }
    //交易网点是否存在
    pOrg = m_pMemDb->m_pDbPublic.m_pOrg.Select(order.sTxOrg);
    if (pOrg == NULL)
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托 网点[%s]不存在", order.sTxOrg);
        data.pData->sDBHead.nLen = 0;
        data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_TXORG;
        AnsData(data);
        return -1;
    }
    //判断委托类型、成交属性
    int nTxCmd = atoi(order.sCmdType);
    if (nTxCmd < 0 || nTxCmd>11)
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托 报单类型[%s]非法", order.sCmdType);
        data.pData->sDBHead.nLen = 0;
        data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_ORDERTYPE;
        AnsData(data);
        return -1;
    }
    int nTxAttr = atoi(order.sCmdAttr);
    if (nTxAttr < 0 || nTxAttr>99)
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托 成交属性[%s]非法", order.sCmdAttr);
        data.pData->sDBHead.nLen = 0;
        data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_ORDERATTR;
        AnsData(data);
        return -1;
    }
    if (pc->sOrderCmd[nTxCmd] != '1')
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托 报单类型[%s]此合约[%d %s]不支持", order.sCmdType, order.nContractNo, order.sContractCode);
        data.pData->sDBHead.nLen = 0;
        data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_ORDERTYPE_SUPPORT;
        AnsData(data);
        return -1;
    }
    if (pc->sDoneAttr[nTxAttr * 10 + nTxAttr] != '1')
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托 成交属性+报单位类型[%s %s]此合约[%d %s]不支持", order.sCmdAttr, order.sCmdType, order.nContractNo, order.sContractCode);
        data.pData->sDBHead.nLen = 0;
        data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_ORDERATTR_SUPPORT;
        AnsData(data);
        return -1;
    }

    //取交易编码
    S_MDB_CUST_TRADECODE* ptx = m_pMemDb->m_pDbTrader.m_pTradecode.Select(order.sTxAcc, order.sExchCode);
    if (ptx == NULL)
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托 交易账号[%s]交易所代码[%s]席位号[%s]无对应的交易编码", order.sTxAcc, order.sExchCode, order.sSeatNo);
        data.pData->sDBHead.nLen = 0;
        data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_TRADECODE;
        AnsData(data);
        return -1;
    }
    strcpy(order.sTxCode, ptx->sTradeCode);


    //这里可以根据网点，判断是否可以交易，后续根据需要增加

    //客户风险度，是否有交易权限
    if (pCustFund->sTradeRight[0] == TR_NONE)
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托 交易账号[%s] 币种[%s]交易所代码[%s]席位号[%s]无交易权限[%c] 风险度[%.2f] ", order.sTxAcc, order.sExchCode, order.sSeatNo, pCustFund->sCurrency, pCustFund->sTradeRight[0], pCustFund->dRisk);
        data.pData->sDBHead.nLen = 0;
        data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_TRADERIGHT;
        AnsData(data);
        return -1;
    }

    order.dAmt = order.dOrderPrice * pc->dTxUnit * order.nOrderNum;//金额

    //生成本地报单号
    sprintf(order.sLocalSerial, INT64PFM, g_pPubData->GetLocalSerial());

    order.nSyncFlag = 1; //报入同步

    //报单状态默认为正申报
    order.sOrderStatus[0] = ORDER_STATUS_SENDOK;//正在申报
    order.sOrderStatus[1] = 0;

    //算出保证金手续费
    double dExchFee = 0.0;//交易所手续费
    double dMemberFee = 0.0;//会员手续费
    double dExchMargin = 0.0;//交易所保证金
    double dMemberMargin = 0.0;//会员保证金
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

    m_pSendXdp.SetFieldValue((unsigned short)101, order.sSeatNo, strlen(order.sSeatNo), m_sErrMsg);
    m_pSendXdp.SetFieldValue((unsigned short)103, order.sLocalSerial, strlen(order.sLocalSerial), m_sErrMsg);
    m_pSendXdp.SetFieldValue((unsigned short)104, order.sTxCode, strlen(order.sTxCode), m_sErrMsg);
    m_pSendXdp.SetFieldValue((unsigned short)20, order.nContractNo, m_sErrMsg);
    m_pSendXdp.SetFieldValue((unsigned short)100, order.sContractCode, strlen(order.sContractCode), m_sErrMsg);
    m_pSendXdp.SetFieldValue((unsigned short)10, (short)order.sForceFlag[0], m_sErrMsg);
    m_pSendXdp.SetFieldValue((unsigned short)60, order.dOrderPrice, m_sErrMsg);
    m_pSendXdp.SetFieldValue((unsigned short)61, order.dAmt, m_sErrMsg);
    m_pSendXdp.SetFieldValue((unsigned short)109, order.sOrderStatus, strlen(order.sOrderStatus), m_sErrMsg);
    m_pSendXdp.SetFieldValue((unsigned short)117, order.sOrderRsp, strlen(order.sOrderRsp), m_sErrMsg);
    m_pSendXdp.SetFieldValue((unsigned short)116, order.sAnsTime, strlen(order.sAnsTime), m_sErrMsg);

    m_pSendXdp.SetFieldValue((unsigned short)22, order.nVarietyId, m_sErrMsg);
    nRet = m_pKernelPub.GetFeeMargin(&order, pc, pCustFuncAcc, dExchFee, dMemberFee, dExchMargin, dMemberMargin);
    if (nRet != 0)
    {
        order.sOrderStatus[0] = ORDER_STATUS_INVALID;//无效单
        order.sOrderStatus[1] = 0;
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "交易账号[%s]交易所代码[%s]席位号[%s]本地报单号[%s]取手续费保证金出错 [%d]", order.sTxAcc, order.sExchCode, order.sSeatNo, order.sLocalSerial, nRet);
        switch (nRet)
        {
        case -1:
            strcpy(order.sOrderRsp, "取手续费出错");
            data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_GETFEE;
            break;
        case -2:
            strcpy(order.sOrderRsp, "取保证金出错");
            data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_GETMARGIN;
            break;
        case -3:
            strcpy(order.sOrderRsp, "合约对应的品种类型不符");
            data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_SYSTEMPARA;
            break;
        default:
            break;
        }
        data.pData->sDBHead.nLen = 0;
        AnsData(data);
        //同步到备机
        m_pSendXdp.SetFieldValue((unsigned short)109, order.sOrderStatus, strlen(order.sOrderStatus), m_sErrMsg);
        m_pSendXdp.SetFieldValue((unsigned short)117, order.sOrderRsp, strlen(order.sOrderRsp), m_sErrMsg);
        if (SendBus(&m_pSendXdp, 0, 9848, 0, CMD_DPABC) != 0)
        {
            m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "发送报单同步 9848出错 本地报单号[%s]", order.sLocalSerial);
        }
        //写入报单表
        orderrid = m_pMemDb->m_pDbTrader.m_pOrder.Insert(order);
        if (orderrid < 0)
        {
            m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托 写委托表出错 本地报单号[%s]", order.sLocalSerial);
            return -1;
        }
        pOrder = m_pMemDb->m_pDbTrader.m_pOrder.At(orderrid);
        if (pOrder == NULL)
        {
            m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托 委托表出错 rowid[%d]找不到委托 本地报单号[%s]", orderrid, order.sLocalSerial);
            return -1;
        }
        return -1;
    }
    order.dExFee = 0.0 - (dExchFee + dMemberFee);//总手续费 为负数
    order.dFrozAmt = dExchMargin + dMemberMargin;//总保证金

    //发送报单同步广播

    m_pSendXdp.SetFieldValue((unsigned short)63, order.dExFee, m_sErrMsg);
    m_pSendXdp.SetFieldValue((unsigned short)62, order.dFrozAmt, m_sErrMsg);

    if (SendBus(&m_pSendXdp, 0, 9848, 0, CMD_DPABC) != 0)
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "发送报单同步 9848出错 本地报单号[%s]", order.sLocalSerial);
    }

    //写入报单表
    orderrid = m_pMemDb->m_pDbTrader.m_pOrder.Insert(order);
    if (orderrid < 0)
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托 写委托表出错 本地报单号[%s]", order.sLocalSerial);
        data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_SYSTEM;
        data.pData->sDBHead.nLen = 0;
        AnsData(data);
        return -1;
    }
    pOrder = m_pMemDb->m_pDbTrader.m_pOrder.At(orderrid);
    if (pOrder == NULL)
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托  委托表出错 rowid[%d]找不到委托", orderrid);
        data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_SYSTEM;
        data.pData->sDBHead.nLen = 0;
        AnsData(data);
        return -1;
    }
    //发送报单给柜台或交易所
    int drebsvrid = 0;
    //if (strcmp(pOrder->sSeatNo,"$") == 0)
    //{
    //	std::vector<S_MDB_BROKERAGE_EXCHANGE *> belist;
    //	//没有输入的度位号，说明只有一个席位
    //	if (!m_pMemDb->m_pDbSet.m_pBrokerageExchange.Select(pCustFuncAcc->nSysId,pOrder->sExchCode,belist))
    //	{
    //		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托 交易账号所属经纪公司[%d] 无对应的交易所[%s]信息",pCustFuncAcc->nSysId,pOrder->sExchCode);
    //		data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_SYSTEM;
    //		data.pData->sDBHead.nLen = 0;
    //		AnsData(data);
    //		pOrder->sOrderStatus[0] = ORDER_STATUS_INVALID;//无效单
    //		pOrder->sOrderStatus[1]=0;
    //		strcpy(pOrder->sOrderRsp,"交易账号所属经纪公司无对应的交易所信息");
    //		pOrder->nSyncFlag = 5; //回报同步
    //		return -1;
    //	}
    //	if (belist.size() != 1)
    //	{
    //		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托 交易账号所属经纪公司[%d] 对应的交易所[%s]有多条信息",pCustFuncAcc->nSysId,pOrder->sExchCode);
    //		data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_SYSTEM;
    //		data.pData->sDBHead.nLen = 0;
    //		AnsData(data);
    //		pOrder->sOrderStatus[0] = ORDER_STATUS_INVALID;//无效单
    //		pOrder->sOrderStatus[1]=0;
    //		strcpy(pOrder->sOrderRsp,"交易账号所属经纪公司对应多个交易所席位信息");
    //		pOrder->nSyncFlag = 5; //回报同步
    //		return -1;
    //	}
    //	drebsvrid = belist[0]->nSvrId;
    //}
    //else
    //{
    //	S_MDB_BROKERAGE_EXCHANGE *pbrokexch = m_pMemDb->m_pDbSet.m_pBrokerageExchange.Select(pCustFuncAcc->nSysId,pOrder->sExchCode,pOrder->sSeatNo);
    //	if (pbrokexch == NULL)
    //	{
    //		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"委托 交易账号所属经纪公司[%d] 交易所[%s] 无席位[%s]信息",pCustFuncAcc->nSysId,pOrder->sExchCode,pOrder->sSeatNo);
    //		data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_SYSTEM;
    //		data.pData->sDBHead.nLen = 0;
    //		AnsData(data);
    //		pOrder->sOrderStatus[0] = ORDER_STATUS_INVALID;//无效单
    //		pOrder->sOrderStatus[1]=0;
    //		strcpy(pOrder->sOrderRsp,"交易账号所属经纪公司无席位信息");
    //		pOrder->nSyncFlag = 5; //回报同步
    //		return -1;
    //	}
    //	drebsvrid =pbrokexch->nSvrId;
    //}


    //冻结资金持仓等等
    nRet = m_pKernelPub.ComputeNewOrder(pAcclock, pCustFund, pOrder, pc, true);
    if (nRet != SUCCESS)
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托 计算出错 返回[%d]", nRet);
        data.pData->sDBHead.a_Ainfo.a_nRetCode = nRet;
        data.pData->sDBHead.nLen = 0;
        AnsData(data);
        pOrder->sOrderStatus[0] = ORDER_STATUS_INVALID;//无效单
        pOrder->sOrderStatus[1] = 0;
        strcpy(pOrder->sOrderRsp, "委托计算出错");
        pOrder->nSyncFlag = 5; //回报同步
        return -1;
    }
    if (SendBus(&m_pSendXdp, drebsvrid, 9206, GetNextSerial(), CMD_DPCALL) != 0)
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托 发送总线出错");
        data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_SYSTEM;
        data.pData->sDBHead.nLen = 0;
        AnsData(data);
        pOrder->sOrderStatus[0] = ORDER_STATUS_INVALID;//无效单
        pOrder->sOrderStatus[1] = 0;
        strcpy(pOrder->sOrderRsp, "发送报单 出错");
        pOrder->nSyncFlag = 5; //回报同步
        return -1;
    }
    nRet = GetAnsData(m_pSendData, 5000);
    while (nRet == 0)
    {
        if (m_pSendData.pData == NULL)
        {
            m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托 取应答，指针为NULL");
            data.pData->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
            data.pData->sDBHead.nLen = 0;
            AnsData(data);
            return -1;
        }
        if (m_pSendData.pData->sDBHead.s_Sinfo.s_nSerial != GetCurSerial())
        {
            m_pClientLink->PoolFree(m_pSendData);
            nRet = GetAnsData(m_pSendData, 5000);
            continue;
        }
        if (m_pSendData.pData->sDBHead.a_Ainfo.a_nRetCode == SUCCESS)
        {
            break;
        }
        else
        {
            m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托 接口返回出错[%d]", m_pSendData.pData->sDBHead.a_Ainfo.a_nRetCode);
            data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_SYSTEM;
            data.pData->sDBHead.nLen = 0;
            AnsData(data);
            pOrder->sOrderStatus[0] = ORDER_STATUS_INVALID;//无效单
            pOrder->sOrderStatus[1] = 0;
            strcpy(pOrder->sOrderRsp, "发送报单 返回错");
            pOrder->nSyncFlag = 5; //回报同步
            return -1;
        }
    }
    if (nRet != 0)
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托 接口返回超时出错");
        //没有收到应答
    }
    //应答回去
    data.pData->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
    data.pData->sDBHead.nLen = 0;
    AnsData(data);

    return 0;
}

int RiskDll::CancelOrder(S_TRADE_DATA& data)
{
	return 0;
}

int RiskDll::SendBus(CXdp *xdp,int svrid,int funcno,int serial,short cmd)
{
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

int RiskDll::ProcSyncFund(S_TRADE_DATA& data)
{
    if (!m_pXdp->FromBuffer(data.pData->sBuffer, data.pData->sDBHead.nLen, m_sErrMsg))
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "请求非XDP数据 [%s]", m_sErrMsg);
        m_pClientLink->PoolFree(data);
        return -1;
    }
    m_pClientLink->PoolFree(data);
	S_MDB_CUST_FUND fund;
	bzero(&fund,sizeof(S_MDB_CUST_FUND));
	Xdp2Struct::ConvertFund(m_pXdp,fund);
	m_pMemDb->m_pDbTrader.m_pFund.Update(fund);
	return 0;
}
int RiskDll::ProcSyncPosi(S_TRADE_DATA& data)
{
    if (!m_pXdp->FromBuffer(data.pData->sBuffer, data.pData->sDBHead.nLen, m_sErrMsg))
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "请求非XDP数据 [%s]", m_sErrMsg);
		m_pClientLink->PoolFree(data);
        return -1;
    }
	m_pClientLink->PoolFree(data);

	S_MDB_CUST_POSITION info;
    bzero(&info, sizeof(S_MDB_CUST_POSITION));
	Xdp2Struct::ConvertPosition(m_pXdp,info);
	m_pMemDb->m_pDbTrader.m_pPosition.Update(info);
    return 0;
}
int RiskDll::ProcBondScope(S_TRADE_DATA& data)
{
    if (!m_pXdp->FromBuffer(data.pData->sBuffer, data.pData->sDBHead.nLen, m_sErrMsg))
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "请求非XDP数据 [%s]", m_sErrMsg);
        m_pClientLink->PoolFree(data);
        return -1;
    }
    m_pClientLink->PoolFree(data);

    char sSecuCode[21];
	double dScope;
	int fieldtype;
	unsigned int len=sizeof(sSecuCode);
	m_pXdp->GetFieldValue((unsigned short)80, sSecuCode, len, fieldtype, m_sErrMsg);
	m_pXdp->GetFieldValue((unsigned short)50, dScope, m_sErrMsg);
	S_MDB_BOND_INFO *pb = m_pMemDb->m_pDbBaseinfo.m_pBond.Select(sSecuCode);
	if (NULL == pb)
	{
		S_MDB_BOND_INFO bond;
		bzero(&bond,sizeof(S_MDB_BOND_INFO));
		strcpy(bond.sBondCode,sSecuCode);
		bond.dMarketScope = dScope;
		int rid = m_pMemDb->m_pDbBaseinfo.m_pBond.Insert(bond);
		m_pMemDb->m_pDbBaseinfo.m_pBond.SetUpdate(rid);
		return 0;
	}
	pb->dMarketScope = dScope;
	m_pMemDb->m_pDbBaseinfo.m_pBond.SetUpdate(*pb);
    return 0;
}
int RiskDll::ProcBondValue(S_TRADE_DATA& data)
{
    if (!m_pXdp->FromBuffer(data.pData->sBuffer, data.pData->sDBHead.nLen, m_sErrMsg))
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "请求非XDP数据 [%s]", m_sErrMsg);
        m_pClientLink->PoolFree(data);
        return -1;
    }
    m_pClientLink->PoolFree(data);

    char sSecuCode[21];
    double dValue;
    int fieldtype;
    unsigned int len = sizeof(sSecuCode);
    m_pXdp->GetFieldValue((unsigned short)80, sSecuCode, len, fieldtype, m_sErrMsg);
    m_pXdp->GetFieldValue((unsigned short)50, dValue, m_sErrMsg);
    S_MDB_BOND_INFO* pb = m_pMemDb->m_pDbBaseinfo.m_pBond.Select(sSecuCode);
    if (NULL == pb)
    {
        S_MDB_BOND_INFO bond;
        bzero(&bond, sizeof(S_MDB_BOND_INFO));
        strcpy(bond.sBondCode, sSecuCode);
        bond.dApprValue = dValue;
        int rid = m_pMemDb->m_pDbBaseinfo.m_pBond.Insert(bond);
        m_pMemDb->m_pDbBaseinfo.m_pBond.SetUpdate(rid);
        return 0;
    }
    pb->dApprValue = dValue;
    m_pMemDb->m_pDbBaseinfo.m_pBond.SetUpdate(*pb);
    return 0;
}
int RiskDll::ProcBondDv01(S_TRADE_DATA& data)
{
    if (!m_pXdp->FromBuffer(data.pData->sBuffer, data.pData->sDBHead.nLen, m_sErrMsg))
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "请求非XDP数据 [%s]", m_sErrMsg);
        m_pClientLink->PoolFree(data);
        return -1;
    }
    m_pClientLink->PoolFree(data);

    char sSecuCode[21];
    double dValue;
    int fieldtype;
    unsigned int len = sizeof(sSecuCode);
    m_pXdp->GetFieldValue((unsigned short)80, sSecuCode, len, fieldtype, m_sErrMsg);
    m_pXdp->GetFieldValue((unsigned short)50, dValue, m_sErrMsg);
    S_MDB_BOND_INFO* pb = m_pMemDb->m_pDbBaseinfo.m_pBond.Select(sSecuCode);
    if (NULL == pb)
    {
        S_MDB_BOND_INFO bond;
        bzero(&bond, sizeof(S_MDB_BOND_INFO));
        strcpy(bond.sBondCode, sSecuCode);
        bond.dDv01 = dValue;
        int rid = m_pMemDb->m_pDbBaseinfo.m_pBond.Insert(bond);
        m_pMemDb->m_pDbBaseinfo.m_pBond.SetUpdate(rid);
        return 0;
    }
    pb->dDv01 = dValue;
    m_pMemDb->m_pDbBaseinfo.m_pBond.SetUpdate(*pb);
    return 0;
}
//处理外部发来的普通报单风控检查
int RiskDll::OrderRiskCheck(S_TRADE_DATA& data)
{
    
    if (!m_pXdp->FromBuffer(data.pData->sBuffer, data.pData->sDBHead.nLen, m_sErrMsg))
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "请求非XDP数据 [%s]", m_sErrMsg);
        data.pData->sDBHead.nLen = 0;
        data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_INPUT;
        AnsData(data);
        return -1;
    }
	S_MDB_ORDER_INFO info;
	bzero(&info,sizeof(S_MDB_ORDER_INFO));
	Xdp2Struct::ConvertOrder(m_pXdp,info);
	unsigned int len=0;

	//黑白名单检查
	data.pData->sDBHead.a_Ainfo.a_nRetCode = CheckBwgMenu(&info, info.sOrderRsp);
	if (data.pData->sDBHead.a_Ainfo.a_nRetCode != 0)
	{
		//校验不通过，返回应答
        m_pXdp->SetFieldValue((unsigned short)10, (unsigned int)data.pData->sDBHead.a_Ainfo.a_nRetCode, m_sErrMsg);
        m_pXdp->SetFieldValue((unsigned short)120, info.sOrderRsp, strlen(info.sOrderRsp), m_sErrMsg);
        len = BPUDATASIZE;
        if (!m_pXdp->ToBuffer(data.pData->sBuffer, len, m_sErrMsg))
        {
            m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "Xdp ToBuffer error");
            m_pClientLink->PoolFree(data);
            return -1;
        }
        data.pData->sDBHead.nLen = len;
        AnsData(data);
        return -1;
	}
	S_MDB_CONTRACT_INFO *pc = m_pMemDb->m_pDbBaseinfo.m_pContract.Select(info.sContractCode);
	if (pc == NULL)
	{
        sprintf(info.sOrderRsp, "证券代码[%s]在证券信息表中不存在", info.sContractCode);
        m_pLog->LogMp(LOG_WARNNING, __FILE__, __LINE__, info.sOrderRsp);

        m_pXdp->SetFieldValue((unsigned short)10, (unsigned int)RET_ERR_CONTRACTINPUT, m_sErrMsg);
        m_pXdp->SetFieldValue((unsigned short)120, info.sOrderRsp, strlen(info.sOrderRsp), m_sErrMsg);
        len = BPUDATASIZE;
        if (!m_pXdp->ToBuffer(data.pData->sBuffer, len, m_sErrMsg))
        {
            m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "Xdp ToBuffer error");
            m_pClientLink->PoolFree(data);
            return -1;
        }
        data.pData->sDBHead.nLen = len;
        data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_CONTRACTINPUT;
        AnsData(data);
		return -1;
	}
	//价格限定检查
	data.pData->sDBHead.a_Ainfo.a_nRetCode = CheckPriceLimit(&info,pc,info.sOrderRsp);
    if (data.pData->sDBHead.a_Ainfo.a_nRetCode != 0)
    {
        //校验不通过，返回应答
        m_pXdp->SetFieldValue((unsigned short)10, (unsigned int)data.pData->sDBHead.a_Ainfo.a_nRetCode, m_sErrMsg);
        m_pXdp->SetFieldValue((unsigned short)120, info.sOrderRsp, strlen(info.sOrderRsp), m_sErrMsg);
        len = BPUDATASIZE;
        if (!m_pXdp->ToBuffer(data.pData->sBuffer, len, m_sErrMsg))
        {
            m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "Xdp ToBuffer error");
            m_pClientLink->PoolFree(data);
            return -1;
        }
        data.pData->sDBHead.nLen = len;
        AnsData(data);
        return -1;
    }
	//检查风险合约
	data.pData->sDBHead.a_Ainfo.a_nRetCode = CheckRiskContract(&info, pc, info.sOrderRsp);
    if (data.pData->sDBHead.a_Ainfo.a_nRetCode != 0)
    {
        //校验不通过，返回应答
        m_pXdp->SetFieldValue((unsigned short)10, (unsigned int)data.pData->sDBHead.a_Ainfo.a_nRetCode, m_sErrMsg);
        m_pXdp->SetFieldValue((unsigned short)120, info.sOrderRsp, strlen(info.sOrderRsp), m_sErrMsg);
        len = BPUDATASIZE;
        if (!m_pXdp->ToBuffer(data.pData->sBuffer, len, m_sErrMsg))
        {
            m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "Xdp ToBuffer error");
            m_pClientLink->PoolFree(data);
            return -1;
        }
        data.pData->sDBHead.nLen = len;
        AnsData(data);
        return -1;
    }
    m_pXdp->SetFieldValue((unsigned short)10, (unsigned int)data.pData->sDBHead.a_Ainfo.a_nRetCode, m_sErrMsg);
    len= BPUDATASIZE;
    if (!m_pXdp->ToBuffer(data.pData->sBuffer, len, m_sErrMsg))
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "Xdp ToBuffer error");
        m_pClientLink->PoolFree(data);
        return -1;
    }
    data.pData->sDBHead.nLen = len;
    AnsData(data);
	return 0;
}
//处理外部发来的双边报价风控检查
int RiskDll::BilateralOrderRiskCheck(S_TRADE_DATA& data)
{
    data.pData->sDBHead.nLen = 0;
    data.pData->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
    AnsData(data);
    return 0;
}
int RiskDll::OrderRtn(S_TRADE_DATA& data)
{

    return 0;
}

int RiskDll::OrderSync(S_TRADE_DATA& data)
{
    if (!m_pXdp->FromBuffer(data.pData->sBuffer, data.pData->sDBHead.nLen, m_sErrMsg))
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "请求非XDP数据 [%s]", m_sErrMsg);
        m_pClientLink->PoolFree(data);
        return -1;
    }
    m_pClientLink->PoolFree(data);
   
    S_MDB_ORDER_INFO order;
    bzero(&order, sizeof(S_MDB_ORDER_INFO));
	Xdp2Struct::ConvertOrder(m_pXdp,order);
    int fieldtype;
    unsigned int len = sizeof(order.sLocalSerial);
    if (!m_pXdp->GetFieldValue((unsigned short)95, order.sLocalSerial, len, fieldtype, m_sErrMsg))
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托同步广播 本地报单号为空");
        return -1;
    }
    len = sizeof(order.sSystemNo);
    m_pXdp->GetFieldValue((unsigned short)96, order.sSystemNo, len, fieldtype, m_sErrMsg);

    len = sizeof(order.sOrderStatus);
    if (!m_pXdp->GetFieldValue((unsigned short)97, order.sOrderStatus, len, fieldtype, m_sErrMsg))
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托同步广播 报单状态为空");
        return -1;
    }
    if (!m_pXdp->GetFieldValue((unsigned short)51, order.dAmt, m_sErrMsg))
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托同步广播 无金额");
        return -1;
    }
    if (!m_pXdp->GetFieldValue((unsigned short)51, order.dExFee, m_sErrMsg))
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托同步广播 无手续费");
        return -1;
    }
    if (!m_pXdp->GetFieldValue((unsigned short)51, order.dFrozAmt, m_sErrMsg))
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托同步广播 无冻结资金");
        return -1;
    }
    len = sizeof(order.sAnsTime);
    if (!m_pXdp->GetFieldValue((unsigned short)98, order.sAnsTime, len, fieldtype, m_sErrMsg))
    {
        m_pLog->LogMp(LOG_PROMPT, __FILE__, __LINE__, "委托同步广播 报单时间为空");
    }
    len = sizeof(order.sOrderRsp);
    if (!m_pXdp->GetFieldValue((unsigned short)120, order.sOrderRsp, len, fieldtype, m_sErrMsg))
    {
        m_pLog->LogMp(LOG_PROMPT, __FILE__, __LINE__, "委托同步广播 报单响应为空");
    }
    int nRet;
    S_MDB_CONTRACT_INFO* pc = NULL;
    S_MDB_QUOTATION* pq = NULL;
    S_MDB_EXCH_INFO* pe = NULL;
    S_MDB_CONTRACT_SETTLE_PRICE* psp = NULL;
    S_MDB_VARIETY_INFO* pv = NULL;
    S_MDB_TRADEACC_INFO* pCustFuncAcc = NULL;
    S_MDB_CUSTINFO* pCust = NULL;
    S_MDB_CUST_FUND* pCustFund = NULL;
    S_MDB_ORG_INFO* pOrg = NULL;
    S_MDB_TXACC_LOCK* pAcclock = NULL;
    S_MDB_ORDER_INFO* pOrder = NULL;
    bool iscust = false;

    //比较更新共享内存中的报单号
    g_pPubData->UpdateLocalSerial(atoll(order.sLocalSerial));
    pOrder = m_pMemDb->m_pDbTrader.m_pOrder.Select(order.sLocalSerial);
    if (pOrder != NULL)
    {
        m_pLog->LogMp(LOG_DEBUG, __FILE__, __LINE__, "委托同步广播 本地报单号[%s]已处理", order.sLocalSerial);
        return -1;
    }
    pc = m_pMemDb->m_pDbBaseinfo.m_pContract.Select(order.sContractCode);
    if (pc == NULL)
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托同步广播 合约[%d %s] 在合约表中不存在", order.nContractNo, order.sContractCode);
        return -1;
    }
	order.nContractNo = pc->nContractId;
    strcpy(order.sExchCode, pc->sExcode);
    //取交易所信息
    pe = m_pMemDb->m_pDbBaseinfo.m_pExchange.Select(order.sExchCode);
    if (pe == NULL)
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托同步广播 合约编号[%d %s] 的交易所[%s]不存在", order.nContractNo, order.sContractCode, pc->sExcode);
        return -1;
    }
    order.nRemainNum = order.nOrderNum;//剩余数量默认为委托数量
    order.sCancelFlag[0] = BOOL_FALSE;
    order.sCancelFlag[1] = 0;
    //客户、交易员
    strcpy(order.sTxDate, pe->sTradeDate);
    strcpy(order.sAccOrg, pCustFuncAcc->sSignOrg);
    //取交易网点
    if (strlen(order.sTxOrg) < 1)
    {
        strcpy(order.sTxOrg, pCustFuncAcc->sSignOrg);
    }
    //是否有此交易账号
    pAcclock = m_pMemDb->SelectLock(order.sTxAcc);
    if (pAcclock == NULL)
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托同步广播 交易账号[%s]非法不存在", order.sTxAcc);
        return -1;
    }
    pCustFuncAcc = m_pMemDb->m_pDbCust.m_pTradeAcc.At(pAcclock->nTradeAccRowid);
    if (pCustFuncAcc == NULL)
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托同步广播 交易账号[%s] 非法，不存在", order.sTxAcc);
        return -1;
    }
    pCust = m_pMemDb->m_pDbCust.m_pCustom.At(pAcclock->nCustRowid);
    if (pCust == NULL)
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托同步广播 交易账号[%s]的客户号[%s]不存在", order.sTxAcc, pCustFuncAcc->sCustNo);
        return -1;
    }
    //取资金表，按账号+币种
    pCustFund = m_pMemDb->GetFund(pAcclock);
    if (pCustFund == NULL)
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托同步广播 交易账号[%s]+合约[%d %s]对应的币种[%s]在资金表不存在", order.sTxAcc, order.nContractNo, order.sContractCode, pc->sCurrency);
        return -1;
    }

    order.nSyncFlag = 1; //报入同步

    //写入报单表
    int orderrid = m_pMemDb->m_pDbTrader.m_pOrder.Insert(order);
    if (orderrid < 0)
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托同步广播 写委托表出错 本地报单号[%s]", order.sLocalSerial);
        return -1;
    }
    pOrder = m_pMemDb->m_pDbTrader.m_pOrder.At(orderrid);
    if (pOrder == NULL)
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托同步广播  委托表出错 rowid[%d]找不到委托", orderrid);
        return -1;
    }
	if (pOrder->sOrderStatus[0] == ORDER_STATUS_INVALID || pOrder->sOrderStatus[0] == ORDER_STATUS_CANCEL || pOrder->sOrderStatus[0] == ORDER_STATUS_SC)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托同步广播 报单状态[%s]为无效单,不用去计算资金持仓了", pOrder->sOrderStatus);
		return -1;
	}
    //冻结资金持仓等等
    nRet = m_pKernelPub.ComputeNewOrder(pAcclock, pCustFund, pOrder, pc, true);
    if (nRet != SUCCESS)
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托同步广播 委托计算出错 返回[%d]", nRet);
        return -1;
    }
    return 0;
}
int RiskDll::CheckBwgMenu(S_MDB_ORDER_INFO* order,  char* msg)
{
    if (order->sTxType[0] == TX_TYPE_CLICK)
    {
		//点击成交要检查白名单
		S_MDB_BOND_BWG_INFO* bwg = m_pMemDb->m_pDbSet.m_pBondBwgInfo.Select(order->sMatchAcc);
		if (bwg == NULL)
		{
			//没有配置黑白灰名单
			sprintf(msg, "点击成交对手方账号[%s]在黑名灰名单中未配置", order->sMatchAcc);
			m_pLog->LogMp(LOG_WARNNING, __FILE__, __LINE__, msg);
			return RET_ERR_BWG_MENU;
		}
		if (bwg->sBwgFlag[0] != BWG_FLAG_BLACK)
		{
			sprintf(msg, "不在在白名单当中，校验不通过", order->sMatchAcc);
			m_pLog->LogMp(LOG_WARNNING, __FILE__, __LINE__, msg);
			return RET_ERR_BWG_MENU;
		}
	}
	return 0;
}
int RiskDll::CheckPriceLimit(S_MDB_ORDER_INFO* order, S_MDB_CONTRACT_INFO* pc, char* msg)
{
    //价格偏离度控制
    S_MDB_PRICE_LIMIT* pl = m_pMemDb->m_pDbSet.m_pPriceLimit.Select(pc->sProd_type, pc->sContractCode);
    if (NULL == pl)
    {
        //按品种取配置
        pl = m_pMemDb->m_pDbSet.m_pPriceLimit.Select(pc->sProd_type, "$");
        if (NULL == pl)	 //没有配置，通过
        {
            m_pXdp->SetFieldValue((unsigned short)10, (unsigned int)0, m_sErrMsg);
            return 0;
        }
    }
    if (order->sBsFlag[0] == BS_FLAG_BUY)
    {
		//买
        if (pl->sLimitType[0] == PRICE_LIMIT_PRICE)	//直接价格
        {
			//直接价格控制
            if (order->dOrderPrice > pl->dUpLimit)
            {
				sprintf(msg,"直接价格限定 委托价[%.4f] > 限定价[%.4f]", order->dOrderPrice, pl->dUpLimit);
			    return RET_ERR_ORDERPRICE;
            }
        }
		else if (pl->sLimitType[0] == PRICE_LIMIT_SP_RATE) //结算价比率
		{
			S_MDB_CONTRACT_SETTLE_PRICE *sp = m_pMemDb->m_pDbBaseinfo.m_pSettlePrice.Select(pc->sContractCode);
			if (NULL == sp)	//无结算价
			{
				sprintf(msg,"证券代码[%s]结算价未设置", pc->sContractCode);
				return RET_ERR_SETTLE_PRICE;
			}
			if (order->dOrderPrice > (1.0 + pl->dUpLimit) * sp->dSettlePrice)
			{
                sprintf(msg, "结算价比率限定 委托价[%.4f] > 限定价[%.4f]  结算价[%.4f]比率[%.4f]", order->dOrderPrice, (1.0 + pl->dUpLimit) * sp->dSettlePrice, sp->dSettlePrice, pl->dUpLimit);
                return RET_ERR_ORDERPRICE;
			}
		}
        else if (pl->sLimitType[0] == PRICE_LIMIT_NP_RATE) //最新价比率
        {
            S_MDB_QUOTATION* quo = m_pMemDb->m_pDbTrader.m_pQuotation.Select(pc->sContractCode);
            if (NULL == quo)	//无行情
            {
                sprintf(msg, "证券代码[%s]无最新行情信息", pc->sContractCode);
                return RET_ERR_SETTLE_PRICE;
            }
            if (order->dOrderPrice > (1.0 + pl->dUpLimit) * quo->dNewPrice)
            {
                sprintf(msg, "最新价比率限定 委托价[%.4f] > 限定价[%.4f]  最新价[%.4f]比率[%.4f]", order->dOrderPrice, (1.0 + pl->dUpLimit) * quo->dNewPrice, quo->dNewPrice, pl->dUpLimit);
                return RET_ERR_ORDERPRICE;
            }
        }
        else if (pl->sLimitType[0] == PRICE_LIMIT_VALUE_RATE) //中证估值比率
        {
            S_MDB_BOND_INFO *bond = m_pMemDb->m_pDbBaseinfo.m_pBond.Select(pc->sContractCode);
            if (NULL == bond)	//无估值信息
            {
                sprintf(msg, "债券代码[%s]无估值等信息", pc->sContractCode);
                return RET_ERR_SETTLE_PRICE;
            }
            if (order->dOrderPrice > (1.0 + pl->dUpLimit) * bond->dApprValue)
            {
                sprintf(msg, "估值比率限定 委托价[%.4f] > 限定价[%.4f]  估值[%.4f]比率[%.4f]", order->dOrderPrice, (1.0 + pl->dUpLimit) * bond->dApprValue, bond->dApprValue, pl->dUpLimit);
                return RET_ERR_ORDERPRICE;
            }
        }
    }
    else
    {
        //卖
        if (pl->sLimitType[0] == PRICE_LIMIT_PRICE)	//直接价格
        {
            //直接价格控制
            if (order->dOrderPrice < pl->dDownLimit)
            {
                sprintf(msg, "直接价格限定 委托价[%.4f] < 限定价[%.4f]", order->dOrderPrice, pl->dDownLimit);
                return RET_ERR_ORDERPRICE;
            }
        }
        else if (pl->sLimitType[0] == PRICE_LIMIT_SP_RATE) //结算价比率
        {
            S_MDB_CONTRACT_SETTLE_PRICE* sp = m_pMemDb->m_pDbBaseinfo.m_pSettlePrice.Select(pc->sContractCode);
            if (NULL == sp)	//无结算价
            {
                sprintf(msg, "证券代码[%s]结算价未设置", pc->sContractCode);
                return RET_ERR_SETTLE_PRICE;
            }
            if (order->dOrderPrice < (1.0 - pl->dDownLimit) * sp->dSettlePrice)
            {
                sprintf(msg, "结算价比率限定 委托价[%.4f] < 限定价[%.4f]  结算价[%.4f]比率[%.4f]", order->dOrderPrice, (1.0 - pl->dDownLimit) * sp->dSettlePrice, sp->dSettlePrice, pl->dUpLimit);
                return RET_ERR_ORDERPRICE;
            }
        }
        else if (pl->sLimitType[0] == PRICE_LIMIT_NP_RATE) //最新价比率
        {
            S_MDB_QUOTATION* quo = m_pMemDb->m_pDbTrader.m_pQuotation.Select(pc->sContractCode);
            if (NULL == quo)	//无行情
            {
                sprintf(msg, "证券代码[%s]无最新行情信息", pc->sContractCode);
                return RET_ERR_SETTLE_PRICE;
            }
            if (order->dOrderPrice < (1.0 - pl->dDownLimit) * quo->dNewPrice)
            {
                sprintf(msg, "最新价比率限定 委托价[%.4f] < 限定价[%.4f]  最新价[%.4f]比率[%.4f]", order->dOrderPrice, (1.0 - pl->dDownLimit) * quo->dNewPrice, quo->dNewPrice, pl->dUpLimit);
                return RET_ERR_ORDERPRICE;
            }
        }
        else if (pl->sLimitType[0] == PRICE_LIMIT_VALUE_RATE) //中证估值比率
        {
            S_MDB_BOND_INFO* bond = m_pMemDb->m_pDbBaseinfo.m_pBond.Select(pc->sContractCode);
            if (NULL == bond)	//无估值信息
            {
                sprintf(msg, "债券代码[%s]无估值等信息", pc->sContractCode);
                return RET_ERR_SETTLE_PRICE;
            }
            if (order->dOrderPrice < (1.0 - pl->dUpLimit) * bond->dApprValue)
            {
                sprintf(msg, "估值比率限定 委托价[%.4f] > 限定价[%.4f]  估值[%.4f]比率[%.4f]", order->dOrderPrice, (1.0 - pl->dDownLimit) * bond->dApprValue, bond->dApprValue, pl->dUpLimit);
                return RET_ERR_ORDERPRICE;
            }
        }
    }
	return 0;
}
int RiskDll::CheckRiskContract(S_MDB_ORDER_INFO* order, S_MDB_CONTRACT_INFO* pc, char* msg)
{
	if (order->sBsFlag[0] == BS_FLAG_BUY)
	{
        S_MDB_RISKCONTRACT_INFO* info = m_pMemDb->m_pDbTrader.m_pRiskContractInfo.Select(pc->nContractId);
        if (info != NULL)
        {
            if (info->sRiskFlag[0] == '1')
            {
				sprintf(msg,"此合约是风险合约 %s",info->sRiskDesc);
                return RET_ERR_RISK_CONTRACT;
            }
        }
	}
    return 0;
}
int RiskDll::ComputeRisk(S_TRADE_DATA& data)
{
    m_nBeginTime = CBF_Date_Time::GetTickUS();
    //不需要应答，直接释放空间
    m_pClientLink->PoolFree(data);

    //找到计算过盈亏的客户，计算风险度及风险级别
    std::vector<int> acclist;
    GetAccList(acclist);
    int i;
    for (i = 0; i < acclist.size(); i++)
    {
        ComputeCustRisk(acclist[i]);
    }
    m_nEndTime = CBF_Date_Time::GetTickUS();
    m_pLog->LogMp(LOG_DEBUG, __FILE__, __LINE__, "计算客户风险完成  耗时[%ld]微秒", m_nEndTime - m_nBeginTime);
    for (i = 0; i < acclist.size(); i++)
    {
        ComputeCustPosiRisk(acclist[i]);
    }
	//计算产品资金风险
	//计算产品持仓风险
	m_pDate.Update();
	ComputeProd(m_pDate.ToStringD8().c_str());
	m_nBeginTime = CBF_Date_Time::GetTickUS();
	m_pLog->LogMp(LOG_DEBUG, __FILE__, __LINE__, "计算产品风险完成  耗时[%ld]微秒", m_nBeginTime - m_nEndTime);
    //计算机构资金风险
    //计算机构持仓风险
	ComputeOrg(m_pDate.ToStringD8().c_str());
	m_nEndTime = CBF_Date_Time::GetTickUS();
	m_pLog->LogMp(LOG_DEBUG, __FILE__, __LINE__, "计算机构风险完成  耗时[%ld]微秒", m_nEndTime - m_nBeginTime);
	return 0;
}

int RiskDll::Sync2Db(S_TRADE_DATA& data)
{
	//不需要应答，直接释放空间
	m_pClientLink->PoolFree(data);

	//将报单 成交同步至数据库
	if (m_pSqlCmd == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "Not connect risk database");
		return -1;
	}
	int i = 0;
	std::vector<S_MDB_ORDER_INFO*>orderlist;
	m_pMemDb->m_pDbTrader.m_pOrder.GetUpdateList(orderlist);
    //if (*g_nSvrHostStatus == ARBSTATUS_MASTER || *g_nSvrHostStatus == ARBSTATUS_MASTER_AYN || *g_nSvrHostStatus == MASTERHOST)

	for (i = 0; i < orderlist.size(); i++)
	{
		SyncOrder(orderlist[i]);
	}
	std::vector<S_MDB_DONE_INFO*>donelist;
	m_pMemDb->m_pDbTrader.m_pDone.GetUpdateList(donelist);
	for (i = 0; i < donelist.size(); i++)
	{
		SyncDone(donelist[i]);
	}
	//持仓是根据日初、报单、成交计算的，不需要同步至db
    std::vector<S_MDB_BOND_INFO*>bondlist;
    m_pMemDb->m_pDbBaseinfo.m_pBond.GetUpdateList(bondlist);
    for (i = 0; i < bondlist.size(); i++)
    {
        SyncBond(bondlist[i]);
    }
	return 0;
}

double RiskDll::GetPosiTotalValue(const char* stxacc)
{
	std::vector<S_MDB_CUST_POSITION*>posilist;
	double dvalue = 0.0;
	m_pMemDb->m_pDbTrader.m_pPosition.SelectByAcc((char*)stxacc, posilist);
	for (int i = 0; i < posilist.size(); i++)
	{
		S_MDB_CONTRACT_INFO* pc = m_pMemDb->m_pDbBaseinfo.m_pContract.Select(posilist[i]->sContractCode);
		if (pc != NULL)
		{
			if (PROD_TYPE_BOND == pc->sProd_type[0] || PROD_TYPE_BOND_BANK == pc->sProd_type[0])
			{
				dvalue += (1.00 * posilist[i]->nCurPB * pc->dTxUnit);
			}
			else  //使用最新价计算
			{
				S_MDB_QUOTATION* pq = m_pMemDb->m_pDbTrader.m_pQuotation.Select(posilist[i]->sContractCode);
				if (pq != NULL)
				{
					dvalue += (1.00 * posilist[i]->nCurPB * pc->dTxUnit * pq->dNewPrice + 1.00 * posilist[i]->nCurPS * pc->dTxUnit * pq->dNewPrice);
				}
			}
		}
	}
	return dvalue;
}
double RiskDll::GetProdPosiTotalValue(const char* prod)
{
	std::vector<S_MDB_CUST_POSITION*>posilist;
	double dvalue = 0.0;
	std::vector<S_MDB_PROD_CUST*>acclist;
	m_pMemDb->m_pDbBaseinfo.m_pProdCust.SelectByProd((char*)prod, acclist);
	for (int j = 0; j < acclist.size(); j++)
	{
		posilist.clear();
        m_pMemDb->m_pDbTrader.m_pPosition.SelectByAcc(acclist[j]->sTxAcc, posilist);
        for (int i = 0; i < posilist.size(); i++)
        {
            S_MDB_CONTRACT_INFO* pc = m_pMemDb->m_pDbBaseinfo.m_pContract.Select(posilist[i]->sContractCode);
            if (pc != NULL)
            {
                if (PROD_TYPE_BOND == pc->sProd_type[0] || PROD_TYPE_BOND_BANK == pc->sProd_type[0])
                {
                    dvalue += (1.00 * posilist[i]->nCurPB * pc->dTxUnit);
                }
                else  //使用最新价计算
                {
                    S_MDB_QUOTATION* pq = m_pMemDb->m_pDbTrader.m_pQuotation.Select(posilist[i]->sContractCode);
                    if (pq != NULL)
                    {
                        dvalue += (1.00 * posilist[i]->nCurPB * pc->dTxUnit * pq->dNewPrice + 1.00 * posilist[i]->nCurPS * pc->dTxUnit * pq->dNewPrice);
                    }
                }
            }
        }
	}
    
    return dvalue;
}
double RiskDll::GetOrgPosiTotalValue(const char* Org)
{
    std::vector<S_MDB_CUST_POSITION*>posilist;
    double dvalue = 0.0;
	std::vector<S_MDB_TRADEACC_INFO *>acclist;
	m_pMemDb->m_pDbCust.m_pTradeAcc.SelectByAccOrg((char *)Org, acclist);
    for (int j = 0; j < acclist.size(); j++)
    {
        posilist.clear();
		m_pMemDb->m_pDbTrader.m_pPosition.SelectByAcc(acclist[j]->sTxAcc, posilist);
		for (int i = 0; i < posilist.size(); i++)
		{
			S_MDB_CONTRACT_INFO* pc = m_pMemDb->m_pDbBaseinfo.m_pContract.Select(posilist[i]->sContractCode);
			if (pc != NULL)
			{
				if (PROD_TYPE_BOND == pc->sProd_type[0] || PROD_TYPE_BOND_BANK == pc->sProd_type[0])
				{
					dvalue += (1.00 * posilist[i]->nCurPB * pc->dTxUnit);
				}
				else  //使用最新价计算
				{
					S_MDB_QUOTATION* pq = m_pMemDb->m_pDbTrader.m_pQuotation.Select(posilist[i]->sContractCode);
					if (pq != NULL)
					{
						dvalue += (1.00 * posilist[i]->nCurPB * pc->dTxUnit * pq->dNewPrice + 1.00 * posilist[i]->nCurPS * pc->dTxUnit * pq->dNewPrice);
					}
				}
			}
		}
	}
    return dvalue;
}

int RiskDll::ProcessDoneNoOrder(S_TRADE_DATA& data)
{
    m_pClientLink->PoolFree(data);
    //保证只有一个线程处理成交
    CBF_PMutex pp(&m_pMemDb->m_pUnProcessDoneMatchMutex);
    std::list<S_MDB_DONE_INFO>::iterator ptr;
    std::list<S_MDB_DONE_INFO>::iterator rq;
    S_MDB_CONTRACT_INFO* pc = NULL;
    S_MDB_EXCH_INFO* pexch = NULL;
    S_MDB_TRADEACC_INFO* pcustacc = NULL;
    S_MDB_DONE_INFO* pdinfo = NULL;
    S_MDB_ORDER_INFO* po = NULL;
    S_MDB_DONE_INFO info;
    std::vector<S_MDB_ORDER_INFO*> orderlist;
    int nIntvalue;
    for (ptr = m_pMemDb->m_pUnProcessDoneMatch.begin(); ptr != m_pMemDb->m_pUnProcessDoneMatch.end();)
    {
        info = *ptr;
        rq = ptr;
        ptr++;

        orderlist.clear();
        //根据交易所代码+系统报单号 去取报单信息

        if (!m_pMemDb->m_pDbTrader.m_pOrder.SelectBySystemno(info.sExchCode, info.sSystemNo, orderlist))
        {
            m_pLog->LogMp(LOG_PROMPT, __FILE__, __LINE__, "成交回报 交易所[%s] 系统报单号[%s] 在委托表中不存在  放入待处理队列", info.sExchCode, info.sSystemNo);
            continue;
        }
        if (orderlist.size() != 1)
        {
            m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "成交回报 交易所[%s] 系统报单号[%s]不是唯一", info.sExchCode, info.sSystemNo);
            continue;
        }
        po = orderlist[0];
        //从报单信息中补齐
        strcpy(info.sDoneDate, po->sTxDate);
        strcpy(info.sTxAcc, po->sTxAcc);
        strcpy(info.sContractCode, po->sContractCode);
        info.nContractNo = po->nContractNo;
        strcpy(info.sLocalNo, po->sLocalSerial);
        strcpy(info.sShFlag, po->sShFlag);
        strcpy(info.sEoFlag, po->sEoFlag);
        strcpy(info.sBsFlag, po->sBsFlag);
        strcpy(info.sTxOrg, po->sTxOrg);
        strcpy(info.sAccOrg, po->sAccOrg);
        strcpy(info.sTxType, po->sTxType);
        strcpy(info.sCmdType, po->sCmdType);
        strcpy(info.sCmdAttr, po->sCmdAttr);
        strcpy(info.sOrderOper, po->sOrderOper);
        strcpy(info.sTxCode, po->sTxCode);
        strcpy(info.sSeatNo, po->sSeatNo);

        pc = m_pMemDb->m_pDbBaseinfo.m_pContract.Select(info.nContractNo);
        if (pc == NULL)
        {
            m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "成交回报 的合约[%d %s]不存在", info.nContractNo, info.sContractCode);
            //从队列删除
            m_pMemDb->m_pUnProcessDoneMatch.erase(rq);
            continue;
        }
        pexch = m_pMemDb->m_pDbBaseinfo.m_pExchange.Select(pc->sExcode);
        if (pexch == NULL)
        {
            m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "成交回报 的合约代码[%s]所属交易所[%s]不存在", info.sContractCode, pc->sExcode);
            //从队列删除
            m_pMemDb->m_pUnProcessDoneMatch.erase(rq);
            continue;
        }
        pcustacc = m_pMemDb->m_pDbCust.m_pTradeAcc.SelectByAcc(info.sTxAcc);
        if (pcustacc == NULL)
        {
            m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "成交回报 交易账号[%s]不存在", info.sTxAcc);
            //从队列删除
            m_pMemDb->m_pUnProcessDoneMatch.erase(rq);
            continue;
        }
        info.dAmt = info.dDonePrice * pc->dTxUnit * info.nDoneNum;//金额
        //查询此成交是否存在
        S_MDB_DONE_INFO* pdinfo = m_pMemDb->m_pDbTrader.m_pDone.Select(info.sExchCode, info.sBsFlag, info.sDoneNo);
        if (pdinfo != NULL)
        {
            m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "成交回报 交易账号[%s] 的成交[%s %s %s]已存在", info.sTxAcc, info.sExchCode, info.sBsFlag, info.sDoneNo);
            //从队列删除
            m_pMemDb->m_pUnProcessDoneMatch.erase(rq);
            continue;
        }

        ProcessDone(pcustacc, po, pc, pexch, info);
        //从队列删除
        m_pMemDb->m_pUnProcessDoneMatch.erase(rq);
    }

    return 0;
}

int RiskDll::ImportSecu(S_TRADE_DATA &data)
{
    if (m_pSqlCmd == NULL)
    {
        data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_DATABASE;
		data.pData->sDBHead.nLen = 0;
        AnsData(data);
        return -1;
    }
    if (!m_pXdp->FromBuffer(data.pData->sBuffer, data.pData->sDBHead.nLen, m_sErrMsg))
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "FromBuffer error [%s]!", m_sErrMsg);
		data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_INPUT;
		data.pData->sDBHead.nLen = 0;
		AnsData(data);
        return -1;
    }
    //取数据
// 119	varchar20	交易所代码	1	Y	
// 100	varchar1	合约代码	20	Y	
// 101	varchar2	合约名称	40	Y	
// 1	ushort2	交易市场类型	2	Y	
// 3	ushort4	品种类别	2	Y	ascii码
// 104	varchar5	品种代码/标的合约代码	20	Y	
// 60	double1	最小变动价位	18.4	Y	
// 5	short1	是否T+0	1	Y	
// 6	short2	是否保证金模式	1	Y	
// 61	double2	保证金比例	18.10	Y	
// 7	short3	是否涨跌停限制	1	Y	
// 62	double3	涨停板率	18.10	Y	
// 63	double4	跌停板率	18.10	Y	
// 8	short4	活跃标志	1	Y	
// 9	short5	合约交易状态	2	y	
// 64	double5	参考价	18.4	y	
// 65	double6	卖方收款比例	18.10	y	
// 10	short6	交收模式	1	Y	
// 22	uint3	最大申报数量(手)	10	Y	
// 23	uint4	最小申报数量(手)	10	Y	
// 66	double7	交易单位	10	Y	
// 25	uint6	报价单位	10	Y	
// 102	varchar3	支持报单指令	12	Y	
// 103	varchar4	支持报单属性	100	Y	
// 105	varchar6	到期日	8	Y	
// 106	varchar7	最后交易日	8	Y	
// 27	uint8	交割间隔天数	10	Y	
// 11	short7	是否大边保证金	1	Y	
// 12	short8	是否看涨(CALL_PUT)	1	y	
// 91	char2	渠道	2	Y	
// 92	char3	操作员代码	16	Y	

    int contractid;
    S_MDB_CONTRACT_INFO contract;
    bzero(&contract, sizeof(S_MDB_CONTRACT_INFO));
    Xdp2Struct::ConvertSecu(m_pXdp,contract);
	unsigned len=0;
   
    //判断合约是否存在
    contractid = m_pPubDb.GetContractId(contract.sContractCode);
    if (contractid > 0)
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "contract [%d %s] exists!", contractid, contract.sContractCode);
		m_pXdp->ResetData();
		m_pXdp->SetFieldValue((unsigned short)10, (unsigned int)contractid, m_sErrMsg);
        unsigned len = BPUDATASIZE;
        if (m_pXdp->ToBuffer(data.pData->sBuffer, len, m_sErrMsg))
        {
			data.pData->sDBHead.nLen = len;
            AnsData(data);
        }
        else
        {
			data.pData->sDBHead.nLen = 0;
			AnsData(data);
        }
        return -1;
    }
    //判断数据是否合法
    if ((contract.sIsOptionCall[0] != '0' && contract.sIsOptionCall[0] != '1') || \
        (contract.sIsBigMargin[0] != '0' && contract.sIsBigMargin[0] != '1') || \
        (contract.sIsMargin[0] != '0' && contract.sIsMargin[0] != '1') || \
        (contract.sIsT0[0] != '0' && contract.sIsT0[0] != '1') || \
        (contract.sIsUpDownLimit[0] != '0' && contract.sIsUpDownLimit[0] != '1') || \
        (contract.sActiveFlag[0] != '0' && contract.sActiveFlag[0] != '1'))
    {
        //是否字段值不符
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "BOOL illegal !");
		data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_INPUT;
		data.pData->sDBHead.nLen = 0;
		AnsData(data);
        return -1;
    }
    //交易所代码
    sprintf(m_sSql, "select count(*) from sys_ex_info where EX_CODE=:1 ");
    if (!m_pSqlCmd->setCommandText(m_sSql))
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "setCommandText error [%s]!", m_sSql);
		data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_DATABASE;
		data.pData->sDBHead.nLen = 0;
		AnsData(data);
        return -1;
    }
    m_pSqlCmd->Param(1).setAsString() = contract.sExcode;
    CSqlData sqldata = m_pSqlCmd->LoadResultSql();
    if (sqldata.GetSqlCount() < 1)
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "LoadResultSql error [%s]!", m_sSql);
		data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_DATABASE;
		data.pData->sDBHead.nLen = 0;
        AnsData(data);
        return -1;
    }
    if (sqldata.GetAtI(0, 0) < 1)
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "sys_ex_info EX_CODE=%s not found ", contract.sExcode);
		data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_NOTFOUND;
		data.pData->sDBHead.nLen = 0;
        AnsData(data);
        return -1;
    }
    //检查品种类型是否符合
    if (!m_pPubMethod.CheckDataDict("PROD_TYPE", contract.sProd_type))
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "PROD_TYPE  error [%s]!", contract.sProd_type);
		data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_INPUT;
		data.pData->sDBHead.nLen = 0;
        AnsData(data);
        return -1;
    }
    //检查合约状态
    if (!m_pPubMethod.CheckDataDict("VARIETY_STATUS", contract.sStatus))
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "VARIETY_STATUS  error [%s]!", contract.sStatus);
		data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_INPUT;
		data.pData->sDBHead.nLen = 0;
        AnsData(data);
        return -1;
    }
    int id;
    if (contract.sProd_type[0] == PROD_TYPE_OPTION || contract.sProd_type[0] == PROD_TYPE_OPTION_SEC) //期权
    {
        //检查标的合约是否存在
        id = m_pPubDb.GetContractId(contract.sProd_code);
        if (id < 0)
        {
            m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "PROD_CODE  not found [%s]!", contract.sProd_code);
			data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_INPUT;
			data.pData->sDBHead.nLen = 0;
            AnsData(data);
            return -1;
        }
        contract.nProdId = id;
    }
    else if (contract.sProd_type[0] == PROD_TYPE_FUTURES) //期货
    {
        //检查品种代码是否存在
        id = m_pPubDb.GetVarietyId(contract.sProd_code);
        if (id < 0)
        {
            m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "VARIETY_CODE  not found [%s]!", contract.sProd_code);
			data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_INPUT;
			data.pData->sDBHead.nLen = 0;
            AnsData(data);
            return -1;
        }
        contract.nProdId = id;
    }
    else if (contract.sProd_type[0] == PROD_TYPE_SPOT) //现货
    {
        //检查交割品代码是否存在  前期这里不用判断，由界面来修改。
        //这个字段存品种编号
        char tmpchar[21];
        if (m_pPubDb.GetVariety(contract.nProdId, tmpchar) < 0)
        {
            m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "varietyno [%d] not found!", contract.nProdId);
			data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_INPUT;
			data.pData->sDBHead.nLen = 0;
            AnsData(data);
            return -1;
        }
    }

    int ret;
    if (contract.nContractId < 1) //前台未输入合约编号
    {
        //生成合约ID
        ret = m_pPubDb.GenContractId(contract.sExcode);
        if (ret < 1)
        {
            m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "add contract , gen contract no error [%s]", contract.sContractCode);
			data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_GENCONTRACT_NO;
			data.pData->sDBHead.nLen = 0;
            AnsData(data);
            return -1;
        }
        contract.nContractId = ret;
    }
    //写合约表
    sprintf(m_sSql, "insert into sys_secu_info(SECU_NO,SECU_CODE,SECU_NAME,EX_CODE,PROD_TYPE,PROD_CODE,PROD_NO,MIN_CHG_PRICE,IST0,ISBAIL,BAIL_RATE,UL_LIMIT,UPPER_LIMIT_VALUE,LOWER_LIMIT_VALUE,ACTIVE_FLAG,TX_STATUS,REF_PRICE,RCV_RATE,BUSI_MODE,MAX_HAND,MIN_HAND,EX_UNIT,PRICE_UNIT,ORDER_CMD,ORDER_ATTR,DUE_DATE,LAST_DATE,DELI_DAYS,IS_BIGMARGIN,ISCALL,VARIETY_NO) values(:1,:2,:3,:4,:5,:6,:7,:8,:9,:10,:11,:12,:13,:14,:15,:16,:17,:18,:19,:20,:21,:22,:23,:24,:25,:26,:27,:28,:29,:30)");
    if (!m_pSqlCmd->setCommandText(m_sSql))
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "setCommandText error [%s]!", m_sSql);
		data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_DATABASE;
		data.pData->sDBHead.nLen = 0;
        AnsData(data);
        return -1;
    }
    m_pSqlCmd->Param(1).setAsLong() = contract.nContractId;
    m_pSqlCmd->Param(2).setAsString() = contract.sContractCode;
    m_pSqlCmd->Param(3).setAsString() = contract.sContractName;
    m_pSqlCmd->Param(4).setAsString() = contract.sExcode;
    m_pSqlCmd->Param(5).setAsString() = contract.sProd_type;
    m_pSqlCmd->Param(6).setAsString() = contract.sProd_code;
    m_pSqlCmd->Param(7).setAsLong() = contract.nProdId;
    m_pSqlCmd->Param(8).setAsDouble() = contract.dMinChgPrice;
    m_pSqlCmd->Param(9).setAsString() = contract.sIsT0;
    m_pSqlCmd->Param(10).setAsString() = contract.sIsMargin;
    m_pSqlCmd->Param(11).setAsDouble() = contract.dMarginRate;
    m_pSqlCmd->Param(12).setAsString() = contract.sIsUpDownLimit;
    m_pSqlCmd->Param(13).setAsDouble() = contract.dUpRate;
    m_pSqlCmd->Param(14).setAsDouble() = contract.dDownRate;
    m_pSqlCmd->Param(15).setAsString() = contract.sActiveFlag;
    m_pSqlCmd->Param(16).setAsString() = contract.sStatus;
    m_pSqlCmd->Param(17).setAsDouble() = contract.dRefPrice;
    m_pSqlCmd->Param(18).setAsDouble() = contract.dSellGetRate;
    m_pSqlCmd->Param(19).setAsString() = contract.sDeliMode;
    m_pSqlCmd->Param(20).setAsLong() = contract.nMaxHand;
    m_pSqlCmd->Param(21).setAsLong() = contract.nMinHand;
    m_pSqlCmd->Param(22).setAsDouble() = contract.dTxUnit;
    m_pSqlCmd->Param(23).setAsString() = contract.sOrderUnit;
    m_pSqlCmd->Param(24).setAsString() = contract.sOrderCmd;
    m_pSqlCmd->Param(25).setAsString() = contract.sDoneAttr;
    m_pSqlCmd->Param(26).setAsString() = contract.sDueDate;
    m_pSqlCmd->Param(27).setAsString() = contract.sLastDate;
    m_pSqlCmd->Param(28).setAsLong() = contract.nDeliDays;
    m_pSqlCmd->Param(29).setAsString() = contract.sIsBigMargin;
    m_pSqlCmd->Param(30).setAsString() = contract.sIsOptionCall;
	m_pSqlCmd->Param(31).setAsLong() = contract.nVariety;
    bool bret = m_pSqlCmd->Execute();
    if (!bret)
    {
        m_pSqlCmd->Rollback();
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "Execute error %s [%s]!", m_sSql, m_pSqlCmd->GetCmdErrInfo());
		data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_DATABASE;
		data.pData->sDBHead.nLen = 0;
        AnsData(data);
        return -1;
    }
    if (m_pSqlCmd->RowsAffected() < 1)
    {
        m_pSqlCmd->Rollback();
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "RowsAffected <1 ");
		data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_DATABASE;
		data.pData->sDBHead.nLen = 0;
        AnsData(data);
        return -1;
    }
    if (!m_pSqlCmd->Commit())
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "Commit error [%s]!", m_pSqlCmd->GetCmdErrInfo());
		data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_DATABASE;
		data.pData->sDBHead.nLen = 0;
        AnsData(data);
        return -1;
    }

	data.pData->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
    sprintf(m_sErrMsg, "合约登记 [%d %s]", contract.nContractId, contract.sContractCode);
    m_pXdp->ResetData();
    m_pXdp->SetFieldValue((unsigned short)10, contract.nContractId, m_sErrMsg);
    len = BPUDATASIZE;
    if (m_pXdp->ToBuffer(data.pData->sBuffer, len, m_sErrMsg))
    {
		data.pData->sDBHead.nLen = len;
        AnsData(data);
    }
    else
    {
		data.pData->sDBHead.nLen = 0;
        AnsData(data);
    }
	m_pMemDb->m_pDbBaseinfo.m_pContract.Insert(contract);
    return 0;
}

int RiskDll::OrderCheck(S_MDB_ORDER_INFO& order, S_ORDER_PUB& order_pub)
{
    //取合约信息
	bool iscust = false;
	order_pub.pc = m_pMemDb->m_pDbBaseinfo.m_pContract.Select(order.sContractCode);
    if (order_pub.pc == NULL)
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "合约代码 [%s] 不存在", order.sContractCode);
        return RET_ERR_CONTRACTINPUT;
    }
    order.nContractNo = order_pub.pc->nContractId;
    strcpy(order.sExchCode, order_pub.pc->sExcode);
	strcpy(order.sContractName,order_pub.pc->sContractName);
    //取交易所信息
	order_pub.pe = m_pMemDb->m_pDbBaseinfo.m_pExchange.Select(order_pub.pc->sExcode);
    if (order_pub.pe == NULL)
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "合约编号[%d %s] 的交易所[%s]不存在", order.nContractNo, order.sContractCode, order_pub.pc->sExcode);
        return RET_ERR_EXCHANGE;
    }
	//比较一下，看看校验这些数据耗时
	if (m_pMemDb->m_pDbBaseinfo.m_pCodeTable.Select((char *)"SH_FLAG",(char *) "0",order.sShFlag) == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托投保标志不符");
		return RET_ERR_SHFLAG;
	}
    if (m_pMemDb->m_pDbBaseinfo.m_pCodeTable.Select((char*)"EO_FLAG", (char*)"0", order.sEoFlag) == NULL)
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托开平标志不符");
        return RET_ERR_EOFLAG;
    }
    if (m_pMemDb->m_pDbBaseinfo.m_pCodeTable.Select((char*)"BS_FLAG", (char*)"0", order.sBsFlag) == NULL)
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托买卖标志不符");
        return RET_ERR_BSFLAG;
    }
    if (m_pMemDb->m_pDbBaseinfo.m_pCodeTable.Select((char*)"TX_TYPE", (char*)"0", order.sTxType) == NULL)
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托交易类型不符");
        return RET_ERR_TXTYPE;
    }
    if (m_pMemDb->m_pDbBaseinfo.m_pCodeTable.Select((char*)"ORDER_TYPE", (char*)"0", order.sCmdType) == NULL)
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托报单价格类型不符");
        return RET_ERR_ORDERTYPE;
    }
    if (m_pMemDb->m_pDbBaseinfo.m_pCodeTable.Select((char*)"ORDER_ATTR", (char*)"0", order.sCmdAttr) == NULL)
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托报单有效属性不符");
        return RET_ERR_ORDERTYPE;
    }
    if (order.nOrderNum < 1)
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托委托数量不符 %d", order.nOrderNum);
        return RET_ERR_INPUT;
    }
    ////委托价格
    //如果是市价委托，取合约的最新价，以便计算手续费、保证金
    if (order.sCmdType[0] == ORDER_TYPE_MARKET || order.sCmdType[0] == ORDER_TYPE_MS_LOSS || order.sCmdType[0] == ORDER_TYPE_MS_PROFIT || order.sCmdType[0] == ORDER_TYPE_LEVEL1_M || order.sCmdType[0] == ORDER_TYPE_LEVEL5_M)
    {
		order_pub.pq = m_pMemDb->m_pDbTrader.m_pQuotation.Select(order.sContractCode);
        if (order_pub.pq == NULL)
        {
            //行情没有，取结算价
			order_pub.psp = m_pMemDb->m_pDbBaseinfo.m_pSettlePrice.Select(order.sContractCode);
            if (order_pub.psp == NULL)
            {
                m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托  市价无行情且取不到此合约[%d %s]的结算价 %s", order.nContractNo, order.sContractCode);
                return RET_ERR_ORDERPRICE;
            }
            order.dOrderPrice = order_pub.psp->dSettlePrice;
        }
        else
        {
            order.dOrderPrice = order_pub.pq->dNewPrice;
        }
    }
    else
    {
        if (order.dOrderPrice>MINDOUBLECOMPARE && order.dOrderPrice< MAXDOUBLECOMPARE)
        {
            m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托价格不符");
            return RET_ERR_ORDERPRICE;
        }
    }
 
    //取品种
    if (order_pub.pc->sProd_type[0] == PROD_TYPE_OPTION || order_pub.pc->sProd_type[0] == PROD_TYPE_OPTION_SEC)
    {
        S_MDB_CONTRACT_INFO* pco = m_pMemDb->m_pDbBaseinfo.m_pContract.Select(order_pub.pc->nProdId);
        if (pco != NULL)
        {
			order_pub.pv = m_pMemDb->m_pDbBaseinfo.m_pVariety.Select(pco->nProdId);
        }
        else
        {
			order_pub.pv = NULL;
        }
    }
    else  if  (order_pub.pc->sProd_type[0] == PROD_TYPE_FUTURES || order_pub.pc->sProd_type[0] == PROD_TYPE_SPOT)  //  现货期货有品种
    {
		order_pub.pv = m_pMemDb->m_pDbBaseinfo.m_pVariety.Select(order_pub.pc->nProdId);
        order.nVarietyId = order_pub.pc->nProdId;
        if (order_pub.pv == NULL)
        {
            m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托 合约[%d %s] 取品种信息出错", order_pub.pc->nContractId, order_pub.pc->sContractCode);
            return RET_ERR_SYSTEMPARA;
        }
    }
    
    //是否有此交易账号
	order_pub.pAcclock = m_pMemDb->SelectLock(order.sTxAcc);
    if (order_pub.pAcclock == NULL)
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托 交易账号[%s]非法不存在", order.sTxAcc);
        return RET_ERR_CUSTFUNDACC;
    }
	order_pub.pCustFuncAcc = m_pMemDb->m_pDbCust.m_pTradeAcc.At(order_pub.pAcclock->nTradeAccRowid);
    if (order_pub.pCustFuncAcc == NULL)
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托 交易账号[%s] 非法，不存在", order.sTxAcc);
        return RET_ERR_CUSTFUNDACC;
    }
	order_pub.pCust = m_pMemDb->m_pDbCust.m_pCustom.At(order_pub.pAcclock->nCustRowid);
    if (order_pub.pCust == NULL)
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托 交易账号[%s]的客户号[%s]不存在", order.sTxAcc, order_pub.pCustFuncAcc->sCustNo);
        return RET_ERR_CUSTNO;
    }
    //比较交易账号对应的客户号是否和输入的相同
    if (strcmp(order_pub.pCustFuncAcc->sCustNo, order.sOrderOper) == 0)
    {
        iscust = true;
        //判断开通渠道
        int nChannel = atoi(order.sOrderChannel);
        if (nChannel <= 0 || nChannel > 59)
        {
            m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托渠道[%s]非法", order.sOrderChannel);
            return RET_ERR_CHANNEL;
        }
        if (order_pub.pCust->sChannleOpen[nChannel] != '1')
        {
            m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托 客户号[%s]交易账号[%s]未开通此渠道[%s]", order_pub.pCustFuncAcc->sCustNo, order.sTxAcc, order.sOrderChannel);
            return RET_ERR_CHANNELOPEN;
        }
    }
    else
    {
        iscust = false;//不是客户发起
        //判断操作员是否有此账号权限
        if (!m_pKernelPub.CheckOperAccRight(order.sOrderOper, order.sTxAcc))
        {
            m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托 操作员[%s] 无交易账号[%s] 的操作权限", order.sOrderOper, order.sTxAcc);
            return RET_ERR_NO_RIGHT;
        }
    }
    //取资金表，按账号+币种
	order_pub.pCustFund = m_pMemDb->GetFund(order_pub.pAcclock);
    if (order_pub.pCustFund == NULL)
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托 交易账号[%s]+合约[%d %s]对应的币种[%s]在资金表不存在", order.sTxAcc, order.nContractNo, order.sContractCode, order_pub.pc->sCurrency);
        return RET_ERR_CUSTFUND;
    }
    
    
    strcpy(order.sTxDate, order_pub.pe->sTradeDate);
    m_pDate.Update();
    strcpy(order.sAnsTime, m_pDate.ToStringDT17().c_str());
    strcpy(order.sAccOrg, order_pub.pCustFuncAcc->sSignOrg);
    //取交易网点
    if (strlen(order.sTxOrg) < 1)
    {
        strcpy(order.sTxOrg, order_pub.pCustFuncAcc->sSignOrg);
    }
    //判断委托类型、成交属性
    int nTxCmd = atoi(order.sCmdType);
    if (nTxCmd < 0 || nTxCmd>11)
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托 报单类型[%s]非法", order.sCmdType);
        return RET_ERR_ORDERTYPE;
    }
    //int nTxAttr = atoi(order.sCmdAttr);
    //if (nTxAttr < 0 || nTxAttr>99)
    //{
    //    m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托 成交属性[%s]非法", order.sCmdAttr);
    //    data.pData->sDBHead.nLen = 0;
    //    data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_ORDERATTR;
    //    AnsData(data);
    //    return -1;
    //}
    //if (pc->sOrderCmd[nTxCmd] != '1')
    //{
    //    m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托 报单类型[%s]此合约[%d %s]不支持", order.sCmdType, order.nContractNo, order.sContractCode);
    //    data.pData->sDBHead.nLen = 0;
    //    data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_ORDERTYPE_SUPPORT;
    //    AnsData(data);
    //    return -1;
    //}
    //if (pc->sDoneAttr[nTxAttr * 10 + nTxAttr] != '1')
    //{
    //    m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托 成交属性+报单位类型[%s %s]此合约[%d %s]不支持", order.sCmdAttr, order.sCmdType, order.nContractNo, order.sContractCode);
    //    data.pData->sDBHead.nLen = 0;
    //    data.pData->sDBHead.a_Ainfo.a_nRetCode = RET_ERR_ORDERATTR_SUPPORT;
    //    AnsData(data);
    //    return -1;
    //}

    //取交易编码
	order_pub.pCustTrader = m_pMemDb->m_pDbTrader.m_pTradecode.Select(order.sTxAcc, order.sExchCode);
    if (order_pub.pCustTrader == NULL)
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托 交易账号[%s]交易所代码[%s]席位号[%s]无对应的交易编码", order.sTxAcc, order.sExchCode, order.sSeatNo);
        return RET_ERR_TRADECODE;
    }
    strcpy(order.sTxCode, order_pub.pCustTrader->sTradeCode);
	strcpy(order.sSeatNo, order_pub.pCustTrader->sSeatNo);

    //取交易所席位信息
    order_pub.pExTrader = m_pMemDb->m_pDbTrader.m_pExchTrader.Select(order_pub.pCustTrader->sExCode, order_pub.pCustTrader->sSeatNo);
    if (order_pub.pExTrader == NULL)
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托 交易所代码[%s]席位号[%s]无对应的接口配置信息",order.sExchCode, order.sSeatNo);
        return RET_ERR_TRADECODE;
    }


    //这里可以根据网点，判断是否可以交易，后续根据需要增加

    //客户交易权限，是否有交易权限
    if (order_pub.pCustFund->sTradeRight[0] == TR_NONE)
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托 交易账号[%s] 币种[%s]交易所代码[%s]席位号[%s]无交易权限[%c] 风险度[%.2f] ", \
		order.sTxAcc, order.sExchCode, order.sSeatNo, order_pub.pCustFund->sCurrency, order_pub.pCustFund->sTradeRight[0], order_pub.pCustFund->dRisk);
        return RET_ERR_TRADERIGHT;
    }
	else  if (order_pub.pCustFund->sTradeRight[0] == TR_CLOSE) //只能卖出或平仓
	{
		if (order_pub.pc->sProd_type[0] == PROD_TYPE_FUTURES || order_pub.pc->sProd_type[0] == PROD_TYPE_OPTION || order_pub.pc->sProd_type[0] == PROD_TYPE_OPTION_SEC)
		{	//期货期权
            if (order.sEoFlag[0] != EO_FLAG_CLOSE && order.sEoFlag[0] != EO_FLAG_CLOSETODAY)
            {
                m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托 交易账号[%s] 币种[%s]交易所代码[%s]席位号[%s]无交易权限[%c] 风险度[%.2f] ", \
				order.sTxAcc, order.sExchCode, order.sSeatNo, order_pub.pCustFund->sCurrency, order_pub.pCustFund->sTradeRight[0], order_pub.pCustFund->dRisk);
                return RET_ERR_TRADERIGHT;
            }
		}
		else  if (order_pub.pc->sProd_type[0] == PROD_TYPE_SPOT || order_pub.pc->sProd_type[0] == PROD_TYPE_SEC || order_pub.pc->sProd_type[0] == PROD_TYPE_BOND || order_pub.pc->sProd_type[0] == PROD_TYPE_BOND_BANK)
		{	 //股票 现货 债券
            if (order.sBsFlag[0] != BS_FLAG_SELL )	   //不是卖
            {
                m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "委托 交易账号[%s] 币种[%s]交易所代码[%s]席位号[%s]无交易权限[%c] 风险度[%.2f] ", \
				order.sTxAcc, order.sExchCode, order.sSeatNo, order_pub.pCustFund->sCurrency, order_pub.pCustFund->sTradeRight[0], order_pub.pCustFund->dRisk);
                return RET_ERR_TRADERIGHT;
            }
		}
	}
	//检查点击成交白名单
	int ret= CheckBwgMenu(&order, m_sErrMsg);
	if (0 != ret)
	{
	     return	 ret;
	}
	//检查价格限定
    ret = CheckPriceLimit(&order, order_pub.pc, m_sErrMsg);
    if (0 != ret)
    {
        return	 ret;
    }
	//检查风险合约
	ret = CheckRiskContract(&order, order_pub.pc, m_sErrMsg);
	if (0 != ret)
	{
		return ret;
	}
	return 0;
}
int RiskDll::OrderRsp(S_TRADE_DATA& rpcdata, S_TRADE_DATA& ansdata)
{
    S_MDB_ORDER_INFO order;
	//bzero(&order,sizeof(S_MDB_ORDER_INFO));
    if (!m_pXdp->FromBuffer(rpcdata.pData->sBuffer, rpcdata.pData->sDBHead.nLen, m_sErrMsg))
    {
        m_pLog->LogMp(LOG_ERROR, __FILE__, __LINE__, "报单响应 FromBuffer出错");
        m_pClientLink->PoolFree(ansdata);
        return -1;
    }
    //Xdp2Struct::ConvertXdp2OrderBroadcast(m_pXdp, order);
    unsigned int len = sizeof(order.sLocalSerial);
    int fieldtype;
	m_pXdp->GetFieldValue((unsigned short)87, order.sLocalSerial, len, fieldtype, m_sErrMsg);
    S_MDB_ORDER_INFO* porder = m_pMemDb->m_pDbTrader.m_pOrder.Select(order.sLocalSerial);
    if (porder == NULL)
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "报单响应 报单号[%s] 找不到原报单", order.sLocalSerial);
        m_pClientLink->PoolFree(ansdata);
        return -1;
    }
	int nRet = ansdata.pData->sDBHead.a_Ainfo.a_nRetCode;
	//返回码不是成功
    if (nRet != SUCCESS)
    {
        m_pLog->LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "报单响应 报单[%s] 返回[%d]", order.sLocalSerial, nRet);
        if (!m_pXdp->FromBuffer(ansdata.pData->sBuffer, ansdata.pData->sDBHead.nLen, m_sErrMsg))
        {
            m_pLog->LogMp(LOG_DEBUG, __FILE__, __LINE__, "报单响应 FromBuffer出错");
            porder->sOrderStatus[0] = ORDER_STATUS_INVALID;	//无效单
			sprintf(order.sOrderRsp, "报单接口返回 返回码[%d]", ansdata.pData->sDBHead.a_Ainfo.a_nRetCode);
            m_pMemDb->m_pDbTrader.m_pOrder.SetUpdate(*porder);
			//释放内存
			m_pClientLink->PoolFree(ansdata);
            return -1;
        }
        len = sizeof(order.sOrderRsp);
		//取响应信息
        if (!m_pXdp->GetFieldValue((unsigned short)122, porder->sOrderRsp, len, fieldtype, m_sErrMsg))
        {
            sprintf(order.sOrderRsp, "报单接口返回[%d]", ansdata.pData->sDBHead.a_Ainfo.a_nRetCode);
        }
        porder->sOrderStatus[0] = ORDER_STATUS_INVALID;	//无效单
		m_pMemDb->m_pDbTrader.m_pOrder.SetUpdate(*porder);
		//释放内存
		m_pClientLink->PoolFree(ansdata);
		return -1;
    }
	//释放内存
	m_pClientLink->PoolFree(ansdata);
    len = sizeof(order.sSystemNo);
    //取系统报单号 某些同步柜台可能会有
    if (m_pXdp->GetFieldValue((unsigned short)96, porder->sSystemNo, len, fieldtype, m_sErrMsg))
    {
		m_pMemDb->m_pDbTrader.m_pOrder.SetUpdate(*porder);
    }
    return 0;
}
