// BuModule.cpp: implementation of the CBuModule class.
//
//////////////////////////////////////////////////////////////////////

#include "BuModule.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBuModule::CBuModule()
{
	m_pSqlCmd = NULL;
	m_pLog = NULL;
	m_pPcLink = NULL;
	m_pConfigXml = NULL;
	m_bIsInit = false;
	m_bIsSetPara = false;
	//功能号 函数指针  功能名称 版本 作者  优先级  冲正标志 是否交易单位  是否停用
	//新增加功能要在此进行设置
	//InitFuncInfo(1000,(FuncPointer)&CBuModule::test01,"测试",1.0,"作者",1,false,true,false);
	//setFuncInfo(1000,(FuncPointerOld)&CBuModule::test01xml,"测试",1.0,"作者",1,false,true,false,0);
	BindOldFunc();
}

CBuModule::~CBuModule()
{
	m_pConfigXml = NULL;//配置文件
	m_pPcLink = NULL;//外部调用
	m_pLog = NULL;  //日志
	m_pSqlCmd = NULL; //数据库连接
	m_pFuncTbl.Clear();
}
void CBuModule::InitFuncInfo(int funcNo, FuncPointer pfunc, const char *funcName, double version, const char *author, int level, bool repealFlag, bool isTxUnit, bool isStop)
{
	S_TBL_FUNC pFunc;
	bzero(&pFunc,sizeof(S_TBL_FUNC));
	pFunc.nFuncNo = funcNo;
	pFunc.pFunc = pfunc;
	CBF_Tools::StringCopy(pFunc.sFuncName,80,funcName);
	pFunc.dVersion = version;
	CBF_Tools::StringCopy(pFunc.sAuthor,40,author);
	pFunc.nLevel = level;
	pFunc.bRepealFlag = repealFlag;
	pFunc.bIsTxUnit = isTxUnit;
	pFunc.bIsStop = isStop;
	pFunc.pBuInstance = NULL;
	strcpy(pFunc.sDllName,"");
	m_pFuncTbl.Insert(pFunc);
}
void CBuModule::setFuncInfo(int funcNo,FuncPointerOld pfunc,char *funcName,double version,char *author,int level,bool repealFlag,bool isTxUnit,bool isStop,int appType)
{
	S_TBL_FUNC_OLD pFunc;
	bzero(&pFunc,sizeof(S_TBL_FUNC_OLD));
	pFunc.nFuncNo = funcNo;
	pFunc.pFunc = pfunc;
	CBF_Tools::StringCopy(pFunc.sFuncName,80,funcName);
	pFunc.dVersion = version;
	CBF_Tools::StringCopy(pFunc.sAuthor,40,author);
	pFunc.nLevel = level;
	pFunc.bRepealFlag = repealFlag;
	pFunc.bIsTxUnit = isTxUnit;
	pFunc.bIsStop = isStop;
	pFunc.pBuInstance = NULL;
	strcpy(pFunc.sDllName,"");
	m_pFuncTblOld.Insert(pFunc);

}
int CBuModule::Run(PBPCCOMMSTRU data)
{
	if (!m_bIsSetPara)
	{
		printf("SetBuPara未调用\n");
		return -1;
	}
	S_TBL_FUNC pFunc = m_pFuncTbl.Find(data->sDBHead.d_Dinfo.d_nServiceNo);

	if (pFunc.nFuncNo == 0)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"无此功能 %d",data->sDBHead.d_Dinfo.d_nServiceNo);
		return -1;
	}
	int nRes=0;
	try
	{
		nRes= (this->*(pFunc.pFunc))(data);
		pFunc.nCallNum++;
		
	}
	catch (...)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"调用功能异常出错 %d",pFunc.nFuncNo);
		return -1;
	}
	return nRes;
}

int CBuModule::RunXml(PBPCCOMMSTRU data)
{
	if (!m_bIsSetPara)
	{
		printf("SetBuPara未调用\n");
		return -1;
	}
	S_TBL_FUNC_OLD pFunc = m_pFuncTblOld.Find(data->sDBHead.d_Dinfo.d_nServiceNo);
	if (pFunc.nFuncNo == 0)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"无此功能 %d",data->sDBHead.d_Dinfo.d_nServiceNo);
		return -1;
	}
	int nRes=0;
	try
	{
		data->sBuffer[data->sDBHead.nLen]=0;
		if (!m_xmlpack.fromBuf(data->sBuffer))
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"调用老框架的交易非xml %d",data->sDBHead.d_Dinfo.d_nServiceNo);
			data->sDBHead.nLen = 0;
			data->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_DATA;
			AnsData(data);
			return -1;
		}
		m_muxmlpack.clear();
		nRes= (this->*(pFunc.pFunc))(&m_xmlpack,&m_muxmlpack);
		pFunc.nCallNum++;
		int nlen;
		nlen = BPUDATASIZE;
		int recnum= m_muxmlpack.size();
		if (recnum>1)
		{
			for (int i=0;i<recnum;i++)
			{
				data->sDBHead.n_Ninfo.n_nNextNo = recnum;
				nlen = BPUDATASIZE;
				if (!m_muxmlpack.at(i)->toBufOld(data->sBuffer,nlen))
				{
					m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"框架数据不符 tobuffer出错 %d",data->sDBHead.d_Dinfo.d_nServiceNo);
					data->sDBHead.nLen = 0;
					data->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_DATA;
					data->sDBHead.cNextFlag = 0;//无后续包
					AnsData(data);
					return -1;
				}
				data->sDBHead.nLen = nlen;
				data->sDBHead.n_Ninfo.n_nNextOffset = i+1;
				if (i == recnum-1)
				{
					data->sDBHead.cNextFlag = 10;
				}
				else
				{
					data->sDBHead.cNextFlag = 1;
				}
				AnsData(data);
			}
		}
		else
		{
			nlen = BPUDATASIZE;
			//数据转换并应答
			if (!m_xmlpack.toBufOld(data->sBuffer,nlen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"框架数据不符 tobuffer出错 %d",data->sDBHead.d_Dinfo.d_nServiceNo);
				data->sDBHead.nLen = 0;
				data->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_DATA;
				data->sDBHead.cNextFlag = 0;//无后续包
				AnsData(data);
				return -1;
			}
			data->sDBHead.nLen = nlen;
			data->sDBHead.cNextFlag = 0;//无后续包
			AnsData(data);
		}
	}
	catch (...)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"调用功能异常出错 %d",pFunc.nFuncNo);
		return -1;
	}
	return nRes;
}


bool CBuModule::BuInit()
{
	m_bIsInit = true;
	return m_bIsInit;
}


void CBuModule::SetPuPara(CBF_Xml *conxml, CIPcLink *pclink, CDbApi *sqldb, CBF_SimpleLog *bulog)
{
	if (conxml == NULL || pclink == NULL || bulog == NULL)
	{
		m_bIsSetPara = false;
		return;
	}
	m_pLog = bulog;
	m_pConfigXml = conxml;
	m_pPcLink = pclink;
	m_pSqlCmd = sqldb;
	m_bIsSetPara = true;
}

bool CBuModule::FirstFuncInfo(S_TBL_FUNC &funcinfo)
{
	return m_pFuncTbl.First(funcinfo);
}

bool CBuModule::NextFuncInfo(S_TBL_FUNC &funcinfo)
{
	return m_pFuncTbl.Next(funcinfo);
}


int CBuModule::AnsData(PBPCCOMMSTRU data)
{
	memcpy(&m_pAnsData,data,sizeof(BPCCOMMSTRU));
	m_pAnsData.sDBHead.cRaflag = 1;
	return m_pPcLink->AnsData(&m_pAnsData);
}

int CBuModule::ExtCall(PBPCCOMMSTRU data,LIST_BPC_RSMSG &ansdata,unsigned int ts)
{
	data->sBpcHead.cMsgType = MSG_EXTCALL;
	return m_pPcLink->ExtCall(data,ansdata,ts);
}

int CBuModule::TransData(PBPCCOMMSTRU data)
{
	data->sBpcHead.cMsgType = MSG_TRANS;
	return m_pPcLink->DPTrans(data);
}

int CBuModule::BpcCall(PBPCCOMMSTRU data, LIST_BPC_RSMSG &ansdata, unsigned int ts)
{
	data->sBpcHead.cMsgType = MSG_BPCCALL;
	return m_pPcLink->BpcCall(data,ansdata,ts);
}

int CBuModule::BpuCall(PBPCCOMMSTRU data, LIST_BPC_RSMSG &ansdata)
{
	return m_pPcLink->BpuCall(data,ansdata);
}

void CBuModule::BindOldFunc()
{
	S_TBL_FUNC_OLD func;
	bool bRet = m_pFuncTblOld.First(func);
	while (bRet)
	{
		InitFuncInfo(func.nFuncNo,(FuncPointer)&CBuModule::RunXml,func.sFuncName,func.dVersion,func.sAuthor,func.nLevel,func.bRepealFlag,func.bIsTxUnit,func.bIsStop);
		bRet = m_pFuncTblOld.Next(func);
	}
}
