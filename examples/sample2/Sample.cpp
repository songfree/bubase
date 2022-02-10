/**
* Copyright (c) 2008 songfree
* All rights reserved.
*
* �ļ�����:Sample.cpp
* ժҪ:
*       
*        
*
* ��ǰ�汾��:1.0.0
*
* �汾��ʷ��Ϣ:
* �汾��            ����/�޸���       �������                 ԭ��
* 1.0.0                AUTO           20130514                  ����
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
//ע�⣺����Ķ�����ÿ�������涼Ҫ�У���������ݴ˷�����ȡ�����ʵ��
CSample * getInstance() 
{ 
    return new CSample(); 
}
CSample::CSample()
{
	
	//���ܺ� ����ָ��  �������� �汾 ����  ���ȼ�  ������־ �Ƿ��׵�λ  �Ƿ�ͣ��
	//InitFuncInfo(1000,(FuncPointer )&CSample::test01,"����",1.0,"����",1,false,true,false);
	//�����ӹ���Ҫ�ڴ˽�������
	InitFuncInfo(2001,(FuncPointer )&CSample::Test2001,"�������",1.0,"����",1,false,true,false);
	InitFuncInfo(2002,(FuncPointer )&CSample::Test2002,"�����ڵ�",1.0,"����",1,false,true,false);
	InitFuncInfo(2003,(FuncPointer )&CSample::Test2003,"����bpc����",1.0,"����",1,false,true,false);
	InitFuncInfo(2004,(FuncPointer )&CSample::Test2004,"����ת��",1.0,"����",1,false,true,false);
	InitFuncInfo(2005,(FuncPointer )&CSample::Test2005,"����",1.0,"����",1,false,true,false);
	InitFuncInfo(2006,(FuncPointer )&CSample::Test2006,"����",1.0,"����",1,false,true,false);
	InitFuncInfo(2007,(FuncPointer )&CSample::Test2007,"����",1.0,"����",1,false,true,false);
	InitFuncInfo(2008,(FuncPointer )&CSample::Test2008,"����",1.0,"����",1,false,true,false);
	InitFuncInfo(2009,(FuncPointer )&CSample::Test2009,"����",1.0,"����",1,false,true,false);
	InitFuncInfo(2010,(FuncPointer )&CSample::Test2010,"����",1.0,"����",1,false,true,false);

	setFuncInfo(3001,(FuncPointerOld)&CSample::Test3001,"����",1.0,"����",1,false,true,false,0);

	BindOldFunc();
}


int CSample::Test3001(CXmlPack *xmlpack,CXmlPackArray *muxmlpack)
{
	xmlpack->setPackNameValue("����","122345636355325242");
	return 0;
}

int CSample::Test2001(PBPCCOMMSTRU data)
{
	m_pLogDat.LogBpc(LOG_DEBUG,data);

	data->sDBHead.cRaflag =1;
	data->sDBHead.cNextFlag = 0;
	data->sDBHead.cZip = 0;
	strcpy(data->sBuffer,"����2001");
	data->sDBHead.nLen = strlen(data->sBuffer);
	data->sBpcHead.nBpcLen = DREBHEADLEN+data->sDBHead.nLen;
	LIST_BPC_RSMSG ansdata;
	BPCCOMMSTRU calldata;
	memcpy(&calldata,data,sizeof(BPCCOMMSTRU));

	//���ݽ�����ȥ����
	calldata.sDBHead.d_Dinfo.d_nNodeId = 0;
	calldata.sDBHead.d_Dinfo.d_cNodePrivateId = 0;
	calldata.sDBHead.d_Dinfo.d_nSvrMainId = 0;
	calldata.sDBHead.d_Dinfo.d_cSvrPrivateId = 0;
	calldata.sDBHead.d_Dinfo.d_nServiceNo = 1001;//����1001
	

	if (m_pPcLink->ExtCall(&calldata,ansdata,5)<0)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"ExtCallʧ��");
		data->sDBHead.a_Ainfo.a_nRetCode = 90;
		strcpy(data->sBuffer,"����2001�������");
		data->sDBHead.nLen = strlen(data->sBuffer);
		m_pPcLink->AnsData(data);
		return 0;
	}
	if (ansdata.size()<1)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"ExtCallʧ�� �޷��ؽ����");
		data->sDBHead.a_Ainfo.a_nRetCode = 90;
		strcpy(data->sBuffer,"����2001������� �޷��ؽ����");
		data->sDBHead.nLen = strlen(data->sBuffer);
		m_pPcLink->AnsData(data);
		return 0;
	}
	if (ansdata.size()>1)
	{
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"ExtCall���ض�����");
		data->sDBHead.a_Ainfo.a_nRetCode = 0;
		strcpy(data->sBuffer,"����2001������ض�����");
		data->sDBHead.nLen = strlen(data->sBuffer);
		m_pPcLink->AnsData(data);
		return 0;
	}
	memcpy(data->sBuffer,ansdata[0].sBuffer,ansdata[0].sDBHead.nLen);
	data->sDBHead.a_Ainfo.a_nRetCode = 0;
	data->sDBHead.nLen = ansdata[0].sDBHead.nLen;
	m_pLog->LogBin(LOG_DEBUG,__FILE__,__LINE__,"���ؽ��",data->sBuffer,data->sDBHead.nLen);
	m_pPcLink->AnsData(data);
	return 0;
}

int CSample::Test2002(PBPCCOMMSTRU data)
{
	m_pLogDat.LogBpc(LOG_DEBUG,data);
	data->sDBHead.cRaflag =1;
	data->sDBHead.cNextFlag = 0;
	data->sDBHead.cZip = 0;
	strcpy(data->sBuffer,"����2002");
	data->sDBHead.nLen = strlen(data->sBuffer);
	data->sBpcHead.nBpcLen = DREBHEADLEN+data->sDBHead.nLen;
	LIST_BPC_RSMSG ansdata;
	BPCCOMMSTRU calldata;
	memcpy(&calldata,data,sizeof(BPCCOMMSTRU));
	
	//���ݽ�����ȥ����
	calldata.sDBHead.d_Dinfo.d_nNodeId = 0;
	calldata.sDBHead.d_Dinfo.d_cNodePrivateId = 0;
	calldata.sDBHead.d_Dinfo.d_nSvrMainId = 0;
	calldata.sDBHead.d_Dinfo.d_cSvrPrivateId = 0;
	calldata.sDBHead.d_Dinfo.d_nServiceNo = 1002;//����1002
	
	
	if (m_pPcLink->BpcCall(&calldata,ansdata,5)<0)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"BpcCallʧ��");
		data->sDBHead.a_Ainfo.a_nRetCode = 90;
		strcpy(data->sBuffer,"����2002bpccall����");
		data->sDBHead.nLen = strlen(data->sBuffer);
		m_pPcLink->AnsData(data);
		return 0;
	}
	if (ansdata.size()<1)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"bpccallʧ�� �޷��ؽ����");
		data->sDBHead.a_Ainfo.a_nRetCode = 90;
		strcpy(data->sBuffer,"����2002bpccall���� �޷��ؽ����");
		data->sDBHead.nLen = strlen(data->sBuffer);
		m_pPcLink->AnsData(data);
		return 0;
	}
	if (ansdata.size()>1)
	{
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"bpccall���ض�����");
		data->sDBHead.a_Ainfo.a_nRetCode = 0;
		strcpy(data->sBuffer,"����2002bpccall���ض�����");
		data->sDBHead.nLen = strlen(data->sBuffer);
		m_pPcLink->AnsData(data);
		return 0;
	}
	memcpy(data->sBuffer,ansdata[0].sBuffer,ansdata[0].sDBHead.nLen);
	data->sDBHead.a_Ainfo.a_nRetCode = 0;
	data->sDBHead.nLen = ansdata[0].sDBHead.nLen;
	m_pLog->LogBin(LOG_DEBUG,__FILE__,__LINE__,"���ؽ��",data->sBuffer,data->sDBHead.nLen);
	m_pPcLink->AnsData(data);
	return 0;

}

int CSample::Test2003(PBPCCOMMSTRU data)
{
	m_pLogDat.LogBpc(LOG_DEBUG,data);
	data->sDBHead.cRaflag =1;
	data->sDBHead.cNextFlag = 0;
	data->sDBHead.cZip = 0;
	strcpy(data->sBuffer,"����2003");
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
	strcpy(data->sBuffer,"����2004");
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
	strcpy(data->sBuffer,"����2005");
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
	strcpy(data->sBuffer,"����2006");
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
	strcpy(data->sBuffer,"����2007");
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
	strcpy(data->sBuffer,"����2008");
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
	strcpy(data->sBuffer,"����2009");
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
	strcpy(data->sBuffer,"����2010");
	data->sDBHead.nLen = strlen(data->sBuffer);
	data->sBpcHead.nBpcLen = DREBHEADLEN+data->sDBHead.nLen;
	m_pPcLink->AnsData(data);
	return 0;
}
