/**
* Copyright (c) 2008 songfree
* All rights reserved.
*
* �ļ�����:SampleNew.cpp
* ժҪ:
*       
*        
*
* ��ǰ�汾��:1.0.0
*
* �汾��ʷ��Ϣ:
* �汾��            ����/�޸���       �������                 ԭ��
* 1.0.0                AUTO           20100708                  ����
**/
// SampleNew.cpp: implementation of the CSampleNew class.
//
//////////////////////////////////////////////////////////////////////



#include "SampleNew.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CSampleNew::~CSampleNew()
{

}
bool CSampleNew::BuInit()
{
	m_bIsInit = true;
	int level;
	std::string logpath;
	std::string logname;
	int id;


	m_pLog->GetLogPara(level,logpath,logname,id);
	m_pLogDat.SetLogPara(level,logpath.c_str(),"bpudreb.dat","bpudreberr.dat","bpubpc.dat","bpubpcerr.dat");
	return m_bIsInit;
}

//ע�⣺����Ķ�����ÿ�������涼Ҫ�У���������ݴ˷�����ȡ�����ʵ��
CSampleNew * getInstance() 
{ 
    return new CSampleNew(); 
}
CSampleNew::CSampleNew()
{
	
	//���ܺ� ����ָ��  �������� �汾 ����  ���ȼ�  ������־ �Ƿ��׵�λ  �Ƿ�ͣ��
	//InitFuncInfo(1000,(FuncPointer )&CSample::test01,"����",1.0,"����",1,false,true,false);
	//�����ӹ���Ҫ�ڴ˽�������
	InitFuncInfo(9991001,(FuncPointer )&CSampleNew::Test1001,"����",1.0,"����",1,false,true,false);
	InitFuncInfo(9991002,(FuncPointer )&CSampleNew::Test1002,"����",1.0,"����",1,false,true,false);
	InitFuncInfo(9991003,(FuncPointer )&CSampleNew::Test1003,"����",1.0,"����",1,false,true,false);
	InitFuncInfo(9991004,(FuncPointer )&CSampleNew::Test1004,"����",1.0,"����",1,false,true,false);
	InitFuncInfo(9991005,(FuncPointer )&CSampleNew::Test1005,"����",1.0,"����",1,false,true,false);
	InitFuncInfo(9991006,(FuncPointer )&CSampleNew::Test1006,"����",1.0,"����",1,false,true,false);
	InitFuncInfo(9991007,(FuncPointer )&CSampleNew::Test1007,"����",1.0,"����",1,false,true,false);
	InitFuncInfo(9991008,(FuncPointer )&CSampleNew::Test1008,"����",1.0,"����",1,false,true,false);
	InitFuncInfo(9991009,(FuncPointer )&CSampleNew::Test1009,"����",1.0,"����",1,false,true,false);
	InitFuncInfo(9991010,(FuncPointer )&CSampleNew::Test1010,"����",1.0,"����",1,false,true,false);
	InitFuncInfo(99001,(FuncPointer )&CSampleNew::Test99001,"����",1.0,"����",1,false,true,false);
}

int CSampleNew::Test99001(PBPCCOMMSTRU data)
{
	m_pLog->LogBin(LOG_DEBUG,__FILE__,__LINE__,"�����������",data->sBuffer,data->sDBHead.nLen);
	return 0;
}

int CSampleNew::Test1001(PBPCCOMMSTRU data)
{
	m_pLogDat.LogBpc(LOG_DEBUG,data);
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�������ݴ�С %d",data->sDBHead.nLen);
	data->sDBHead.cRaflag =1;
	data->sDBHead.cNextFlag = 0;
	data->sDBHead.cZip = 0;
	
	char sdata[65534];
	memset(sdata,0,sizeof(sdata));
	memcpy(sdata,data->sBuffer,data->sDBHead.nLen);
	strcpy(data->sBuffer,"����1001");
	memcpy(data->sBuffer+8,sdata,data->sDBHead.nLen);
	data->sDBHead.nLen+=8;
	
	data->sBpcHead.nBpcLen = DREBHEADLEN+data->sDBHead.nLen;
	m_pLog->LogBin(LOG_DEBUG,__FILE__,__LINE__,"���ؽ��",data->sBuffer,data->sDBHead.nLen);
	m_pPcLink->AnsData(data);
	return 0;
}

int CSampleNew::Test1002(PBPCCOMMSTRU data)
{
	m_pLogDat.LogBpc(LOG_DEBUG,data);
	data->sDBHead.cRaflag =1;
	data->sDBHead.cNextFlag = 0;
	data->sDBHead.cZip = 0;
	strcpy(data->sBuffer,"����1002");
	data->sDBHead.nLen = strlen(data->sBuffer);
	data->sBpcHead.nBpcLen = DREBHEADLEN+data->sDBHead.nLen;
	m_pLog->LogBin(LOG_DEBUG,__FILE__,__LINE__,"���ؽ��",data->sBuffer,data->sDBHead.nLen);
	m_pPcLink->AnsData(data);
	return 0;
}

int CSampleNew::Test1003(PBPCCOMMSTRU data)
{
	m_pLogDat.LogBpc(LOG_DEBUG,data);
	data->sDBHead.cRaflag =1;
	data->sDBHead.cNextFlag = 0;
	data->sDBHead.cZip = 0;
	strcpy(data->sBuffer,"����1003");
	data->sDBHead.nLen = strlen(data->sBuffer);
	data->sBpcHead.nBpcLen = DREBHEADLEN+data->sDBHead.nLen;
	m_pPcLink->AnsData(data);
	return 0;
}

int CSampleNew::Test1004(PBPCCOMMSTRU data)
{
	m_pLogDat.LogBpc(LOG_DEBUG,data);
	data->sDBHead.cRaflag =1;
	data->sDBHead.cNextFlag = 0;
	data->sDBHead.cZip = 0;
	strcpy(data->sBuffer,"����1004");
	data->sDBHead.nLen = strlen(data->sBuffer);
	data->sBpcHead.nBpcLen = DREBHEADLEN+data->sDBHead.nLen;
	m_pPcLink->AnsData(data);
	return 0;
}

int CSampleNew::Test1005(PBPCCOMMSTRU data)
{
	m_pLogDat.LogBpc(LOG_DEBUG,data);
	data->sDBHead.cRaflag =1;
	data->sDBHead.cNextFlag = 0;
	strcpy(data->sBuffer,"����1005");
	data->sDBHead.cZip = 0;
	data->sDBHead.nLen = strlen(data->sBuffer);
	data->sBpcHead.nBpcLen = DREBHEADLEN+data->sDBHead.nLen;
	m_pPcLink->AnsData(data);
	return 0;
}

int CSampleNew::Test1006(PBPCCOMMSTRU data)
{
	m_pLogDat.LogBpc(LOG_DEBUG,data);
	data->sDBHead.cRaflag =1;
	data->sDBHead.cNextFlag = 0;
	data->sDBHead.cZip = 0;
	strcpy(data->sBuffer,"����1006");
	data->sDBHead.nLen = strlen(data->sBuffer);
	data->sBpcHead.nBpcLen = DREBHEADLEN+data->sDBHead.nLen;
	m_pPcLink->AnsData(data);
	return 0;
}

int CSampleNew::Test1007(PBPCCOMMSTRU data)
{
	m_pLogDat.LogBpc(LOG_DEBUG,data);
	data->sDBHead.cRaflag =1;
	data->sDBHead.cNextFlag = 0;
	data->sDBHead.cZip = 0;
	strcpy(data->sBuffer,"����1007");
	data->sDBHead.nLen = strlen(data->sBuffer);
	data->sBpcHead.nBpcLen = DREBHEADLEN+data->sDBHead.nLen;
	m_pPcLink->AnsData(data);
	return 0;
}

int CSampleNew::Test1008(PBPCCOMMSTRU data)
{
	m_pLogDat.LogBpc(LOG_DEBUG,data);
	data->sDBHead.cRaflag =1;
	data->sDBHead.cNextFlag = 0;
	strcpy(data->sBuffer,"����1008");
	data->sDBHead.cZip = 0;
	data->sDBHead.nLen = strlen(data->sBuffer);
	data->sBpcHead.nBpcLen = DREBHEADLEN+data->sDBHead.nLen;
	m_pPcLink->AnsData(data);
	return 0;
}

int CSampleNew::Test1009(PBPCCOMMSTRU data)
{
	m_pLogDat.LogBpc(LOG_DEBUG,data);
	data->sDBHead.cRaflag =1;
	data->sDBHead.cNextFlag = 0;
	data->sDBHead.cZip = 0;
	strcpy(data->sBuffer,"����1009");
	data->sDBHead.nLen = strlen(data->sBuffer);
	data->sBpcHead.nBpcLen = DREBHEADLEN+data->sDBHead.nLen;
	m_pPcLink->AnsData(data);
	return 0;
}

int CSampleNew::Test1010(PBPCCOMMSTRU data)
{
	m_pLogDat.LogBpc(LOG_DEBUG,data);
	data->sDBHead.cRaflag =1;
	data->sDBHead.cNextFlag = 0;
	data->sDBHead.cZip = 0;
	strcpy(data->sBuffer,"����1010");
	data->sDBHead.nLen = strlen(data->sBuffer);
	data->sBpcHead.nBpcLen = DREBHEADLEN+data->sDBHead.nLen;
	m_pPcLink->AnsData(data);
	return 0;
}
