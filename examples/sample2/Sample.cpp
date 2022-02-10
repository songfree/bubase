/**
* Copyright (c) 2008 songfree
* All rights reserved.
*
* 文件名称:Sample.cpp
* 摘要:
*       
*        
*
* 当前版本号:1.0.0
*
* 版本历史信息:
* 版本号            作者/修改者       完成日期                 原因
* 1.0.0                AUTO           20130514                  创建
**/
// Sample.cpp: implementation of the CSample class.
//
//////////////////////////////////////////////////////////////////////



#include "Sample.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CSample::~CSample()
{

}
bool CSample::BuInit()
{
	m_bIsInit = true;
	return m_bIsInit;
}


void CSample::SetBuPara(CBF_Xml *conxml,CIPcLink *pclink,CDbApi *sqldb,CBF_SimpleLog *bulog)
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

	int level;
	std::string logpath;
	std::string logname;
	int id;
	
	
	m_pLog->GetLogPara(level,logpath,logname,id);
	m_pLogDat.SetLogPara(level,logpath.c_str(),"bpudreb.dat","bpudreberr.dat","bpubpc.dat","bpubpcerr.dat");

	
}
//注意：下面的定义在每个类里面都要有，主程序根据此方法获取本类的实例
CSample * getInstance() 
{ 
    return new CSample(); 
}
CSample::CSample()
{
	
	//功能号 函数指针  功能名称 版本 作者  优先级  冲正标志 是否交易单位  是否停用
	//InitFuncInfo(1000,(FuncPointer )&CSample::test01,"测试",1.0,"作者",1,false,true,false);
	//新增加功能要在此进行设置
	InitFuncInfo(2001,(FuncPointer )&CSample::Test2001,"测试外调",1.0,"作者",1,false,true,false);
	InitFuncInfo(2002,(FuncPointer )&CSample::Test2002,"测试内调",1.0,"作者",1,false,true,false);
	InitFuncInfo(2003,(FuncPointer )&CSample::Test2003,"测试bpc调用",1.0,"作者",1,false,true,false);
	InitFuncInfo(2004,(FuncPointer )&CSample::Test2004,"测试转移",1.0,"作者",1,false,true,false);
	InitFuncInfo(2005,(FuncPointer )&CSample::Test2005,"测试",1.0,"作者",1,false,true,false);
	InitFuncInfo(2006,(FuncPointer )&CSample::Test2006,"测试",1.0,"作者",1,false,true,false);
	InitFuncInfo(2007,(FuncPointer )&CSample::Test2007,"测试",1.0,"作者",1,false,true,false);
	InitFuncInfo(2008,(FuncPointer )&CSample::Test2008,"测试",1.0,"作者",1,false,true,false);
	InitFuncInfo(2009,(FuncPointer )&CSample::Test2009,"测试",1.0,"作者",1,false,true,false);
	InitFuncInfo(2010,(FuncPointer )&CSample::Test2010,"测试",1.0,"作者",1,false,true,false);

	setFuncInfo(3001,(FuncPointerOld)&CSample::Test3001,"测试",1.0,"作者",1,false,true,false,0);

	BindOldFunc();
}


int CSample::Test3001(CXmlPack *xmlpack,CXmlPackArray *muxmlpack)
{
	xmlpack->setPackNameValue("测试","122345636355325242");
	return 0;
}

int CSample::Test2001(PBPCCOMMSTRU data)
{
	m_pLogDat.LogBpc(LOG_DEBUG,data);

	data->sDBHead.cRaflag =1;
	data->sDBHead.cNextFlag = 0;
	data->sDBHead.cZip = 0;
	strcpy(data->sBuffer,"测试2001");
	data->sDBHead.nLen = strlen(data->sBuffer);
	data->sBpcHead.nBpcLen = DREBHEADLEN+data->sDBHead.nLen;
	LIST_BPC_RSMSG ansdata;
	BPCCOMMSTRU calldata;
	memcpy(&calldata,data,sizeof(BPCCOMMSTRU));

	//根据交易码去调用
	calldata.sDBHead.d_Dinfo.d_nNodeId = 0;
	calldata.sDBHead.d_Dinfo.d_cNodePrivateId = 0;
	calldata.sDBHead.d_Dinfo.d_nSvrMainId = 0;
	calldata.sDBHead.d_Dinfo.d_cSvrPrivateId = 0;
	calldata.sDBHead.d_Dinfo.d_nServiceNo = 1001;//调用1001
	

	if (m_pPcLink->ExtCall(&calldata,ansdata,5)<0)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"ExtCall失败");
		data->sDBHead.a_Ainfo.a_nRetCode = 90;
		strcpy(data->sBuffer,"测试2001外调出错");
		data->sDBHead.nLen = strlen(data->sBuffer);
		m_pPcLink->AnsData(data);
		return 0;
	}
	if (ansdata.size()<1)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"ExtCall失败 无返回结果集");
		data->sDBHead.a_Ainfo.a_nRetCode = 90;
		strcpy(data->sBuffer,"测试2001外调出错 无返回结果集");
		data->sDBHead.nLen = strlen(data->sBuffer);
		m_pPcLink->AnsData(data);
		return 0;
	}
	if (ansdata.size()>1)
	{
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"ExtCall返回多结果集");
		data->sDBHead.a_Ainfo.a_nRetCode = 0;
		strcpy(data->sBuffer,"测试2001外调返回多结果集");
		data->sDBHead.nLen = strlen(data->sBuffer);
		m_pPcLink->AnsData(data);
		return 0;
	}
	memcpy(data->sBuffer,ansdata[0].sBuffer,ansdata[0].sDBHead.nLen);
	data->sDBHead.a_Ainfo.a_nRetCode = 0;
	data->sDBHead.nLen = ansdata[0].sDBHead.nLen;
	m_pLog->LogBin(LOG_DEBUG,__FILE__,__LINE__,"返回结果",data->sBuffer,data->sDBHead.nLen);
	m_pPcLink->AnsData(data);
	return 0;
}

int CSample::Test2002(PBPCCOMMSTRU data)
{
	m_pLogDat.LogBpc(LOG_DEBUG,data);
	data->sDBHead.cRaflag =1;
	data->sDBHead.cNextFlag = 0;
	data->sDBHead.cZip = 0;
	strcpy(data->sBuffer,"测试2002");
	data->sDBHead.nLen = strlen(data->sBuffer);
	data->sBpcHead.nBpcLen = DREBHEADLEN+data->sDBHead.nLen;
	LIST_BPC_RSMSG ansdata;
	BPCCOMMSTRU calldata;
	memcpy(&calldata,data,sizeof(BPCCOMMSTRU));
	
	//根据交易码去调用
	calldata.sDBHead.d_Dinfo.d_nNodeId = 0;
	calldata.sDBHead.d_Dinfo.d_cNodePrivateId = 0;
	calldata.sDBHead.d_Dinfo.d_nSvrMainId = 0;
	calldata.sDBHead.d_Dinfo.d_cSvrPrivateId = 0;
	calldata.sDBHead.d_Dinfo.d_nServiceNo = 1002;//调用1002
	
	
	if (m_pPcLink->BpcCall(&calldata,ansdata,5)<0)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"BpcCall失败");
		data->sDBHead.a_Ainfo.a_nRetCode = 90;
		strcpy(data->sBuffer,"测试2002bpccall出错");
		data->sDBHead.nLen = strlen(data->sBuffer);
		m_pPcLink->AnsData(data);
		return 0;
	}
	if (ansdata.size()<1)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"bpccall失败 无返回结果集");
		data->sDBHead.a_Ainfo.a_nRetCode = 90;
		strcpy(data->sBuffer,"测试2002bpccall出错 无返回结果集");
		data->sDBHead.nLen = strlen(data->sBuffer);
		m_pPcLink->AnsData(data);
		return 0;
	}
	if (ansdata.size()>1)
	{
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"bpccall返回多结果集");
		data->sDBHead.a_Ainfo.a_nRetCode = 0;
		strcpy(data->sBuffer,"测试2002bpccall返回多结果集");
		data->sDBHead.nLen = strlen(data->sBuffer);
		m_pPcLink->AnsData(data);
		return 0;
	}
	memcpy(data->sBuffer,ansdata[0].sBuffer,ansdata[0].sDBHead.nLen);
	data->sDBHead.a_Ainfo.a_nRetCode = 0;
	data->sDBHead.nLen = ansdata[0].sDBHead.nLen;
	m_pLog->LogBin(LOG_DEBUG,__FILE__,__LINE__,"返回结果",data->sBuffer,data->sDBHead.nLen);
	m_pPcLink->AnsData(data);
	return 0;

}

int CSample::Test2003(PBPCCOMMSTRU data)
{
	m_pLogDat.LogBpc(LOG_DEBUG,data);
	data->sDBHead.cRaflag =1;
	data->sDBHead.cNextFlag = 0;
	data->sDBHead.cZip = 0;
	strcpy(data->sBuffer,"测试2003");
	data->sDBHead.nLen = strlen(data->sBuffer);
	data->sBpcHead.nBpcLen = DREBHEADLEN+data->sDBHead.nLen;
	m_pPcLink->AnsData(data);
	return 0;
}

int CSample::Test2004(PBPCCOMMSTRU data)
{
	m_pLogDat.LogBpc(LOG_DEBUG,data);
	data->sDBHead.cRaflag =1;
	data->sDBHead.cNextFlag = 0;
	data->sDBHead.cZip = 0;
	strcpy(data->sBuffer,"测试2004");
	data->sDBHead.nLen = strlen(data->sBuffer);
	data->sBpcHead.nBpcLen = DREBHEADLEN+data->sDBHead.nLen;
	m_pPcLink->AnsData(data);
	return 0;
}

int CSample::Test2005(PBPCCOMMSTRU data)
{
	m_pLogDat.LogBpc(LOG_DEBUG,data);
	data->sDBHead.cRaflag =1;
	data->sDBHead.cNextFlag = 0;
	data->sDBHead.cZip = 0;
	strcpy(data->sBuffer,"测试2005");
	data->sDBHead.nLen = strlen(data->sBuffer);
	data->sBpcHead.nBpcLen = DREBHEADLEN+data->sDBHead.nLen;
	m_pPcLink->AnsData(data);
	return 0;
}

int CSample::Test2006(PBPCCOMMSTRU data)
{
	m_pLogDat.LogBpc(LOG_DEBUG,data);
	data->sDBHead.cRaflag =1;
	data->sDBHead.cNextFlag = 0;
	data->sDBHead.cZip = 0;
	strcpy(data->sBuffer,"测试2006");
	data->sDBHead.nLen = strlen(data->sBuffer);
	data->sBpcHead.nBpcLen = DREBHEADLEN+data->sDBHead.nLen;
	m_pPcLink->AnsData(data);
	return 0;
}

int CSample::Test2007(PBPCCOMMSTRU data)
{
	m_pLogDat.LogBpc(LOG_DEBUG,data);
	data->sDBHead.cRaflag =1;
	data->sDBHead.cNextFlag = 0;
	data->sDBHead.cZip = 0;
	strcpy(data->sBuffer,"测试2007");
	data->sDBHead.nLen = strlen(data->sBuffer);
	data->sBpcHead.nBpcLen = DREBHEADLEN+data->sDBHead.nLen;
	m_pPcLink->AnsData(data);
	return 0;
}

int CSample::Test2008(PBPCCOMMSTRU data)
{
	m_pLogDat.LogBpc(LOG_DEBUG,data);
	data->sDBHead.cRaflag =1;
	data->sDBHead.cNextFlag = 0;
	data->sDBHead.cZip = 0;
	strcpy(data->sBuffer,"测试2008");
	data->sDBHead.nLen = strlen(data->sBuffer);
	data->sBpcHead.nBpcLen = DREBHEADLEN+data->sDBHead.nLen;
	m_pPcLink->AnsData(data);
	return 0;
}

int CSample::Test2009(PBPCCOMMSTRU data)
{
	m_pLogDat.LogBpc(LOG_DEBUG,data);
	data->sDBHead.cRaflag =1;
	data->sDBHead.cNextFlag = 0;
	data->sDBHead.cZip = 0;
	strcpy(data->sBuffer,"测试2009");
	data->sDBHead.nLen = strlen(data->sBuffer);
	data->sBpcHead.nBpcLen = DREBHEADLEN+data->sDBHead.nLen;
	m_pPcLink->AnsData(data);
	return 0;
}

int CSample::Test2010(PBPCCOMMSTRU data)
{
	m_pLogDat.LogBpc(LOG_DEBUG,data);
	data->sDBHead.cRaflag =1;
	data->sDBHead.cNextFlag = 0;
	data->sDBHead.cZip = 0;
	strcpy(data->sBuffer,"测试2010");
	data->sDBHead.nLen = strlen(data->sBuffer);
	data->sBpcHead.nBpcLen = DREBHEADLEN+data->sDBHead.nLen;
	m_pPcLink->AnsData(data);
	return 0;
}
