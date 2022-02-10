/**
* Copyright (c) 2008 songfree
* All rights reserved.
*
* 文件名称:SampleNew.cpp
* 摘要:
*       
*        
*
* 当前版本号:1.0.0
*
* 版本历史信息:
* 版本号            作者/修改者       完成日期                 原因
* 1.0.0                AUTO           20100708                  创建
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

//注意：下面的定义在每个类里面都要有，主程序根据此方法获取本类的实例
CSampleNew * getInstance() 
{ 
    return new CSampleNew(); 
}
CSampleNew::CSampleNew()
{
	
	//功能号 函数指针  功能名称 版本 作者  优先级  冲正标志 是否交易单位  是否停用
	//InitFuncInfo(1000,(FuncPointer )&CSample::test01,"测试",1.0,"作者",1,false,true,false);
	//新增加功能要在此进行设置
	InitFuncInfo(9991001,(FuncPointer )&CSampleNew::Test1001,"测试",1.0,"作者",1,false,true,false);
	InitFuncInfo(9991002,(FuncPointer )&CSampleNew::Test1002,"测试",1.0,"作者",1,false,true,false);
	InitFuncInfo(9991003,(FuncPointer )&CSampleNew::Test1003,"测试",1.0,"作者",1,false,true,false);
	InitFuncInfo(9991004,(FuncPointer )&CSampleNew::Test1004,"测试",1.0,"作者",1,false,true,false);
	InitFuncInfo(9991005,(FuncPointer )&CSampleNew::Test1005,"测试",1.0,"作者",1,false,true,false);
	InitFuncInfo(9991006,(FuncPointer )&CSampleNew::Test1006,"测试",1.0,"作者",1,false,true,false);
	InitFuncInfo(9991007,(FuncPointer )&CSampleNew::Test1007,"测试",1.0,"作者",1,false,true,false);
	InitFuncInfo(9991008,(FuncPointer )&CSampleNew::Test1008,"测试",1.0,"作者",1,false,true,false);
	InitFuncInfo(9991009,(FuncPointer )&CSampleNew::Test1009,"测试",1.0,"作者",1,false,true,false);
	InitFuncInfo(9991010,(FuncPointer )&CSampleNew::Test1010,"测试",1.0,"作者",1,false,true,false);
	InitFuncInfo(99001,(FuncPointer )&CSampleNew::Test99001,"测试",1.0,"作者",1,false,true,false);
}

int CSampleNew::Test99001(PBPCCOMMSTRU data)
{
	m_pLog->LogBin(LOG_DEBUG,__FILE__,__LINE__,"监控数据内容",data->sBuffer,data->sDBHead.nLen);
	return 0;
}

int CSampleNew::Test1001(PBPCCOMMSTRU data)
{
	m_pLogDat.LogBpc(LOG_DEBUG,data);
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"请求内容大小 %d",data->sDBHead.nLen);
	data->sDBHead.cRaflag =1;
	data->sDBHead.cNextFlag = 0;
	data->sDBHead.cZip = 0;
	
	char sdata[65534];
	memset(sdata,0,sizeof(sdata));
	memcpy(sdata,data->sBuffer,data->sDBHead.nLen);
	strcpy(data->sBuffer,"测试1001");
	memcpy(data->sBuffer+8,sdata,data->sDBHead.nLen);
	data->sDBHead.nLen+=8;
	
	data->sBpcHead.nBpcLen = DREBHEADLEN+data->sDBHead.nLen;
	m_pLog->LogBin(LOG_DEBUG,__FILE__,__LINE__,"返回结果",data->sBuffer,data->sDBHead.nLen);
	m_pPcLink->AnsData(data);
	return 0;
}

int CSampleNew::Test1002(PBPCCOMMSTRU data)
{
	m_pLogDat.LogBpc(LOG_DEBUG,data);
	data->sDBHead.cRaflag =1;
	data->sDBHead.cNextFlag = 0;
	data->sDBHead.cZip = 0;
	strcpy(data->sBuffer,"测试1002");
	data->sDBHead.nLen = strlen(data->sBuffer);
	data->sBpcHead.nBpcLen = DREBHEADLEN+data->sDBHead.nLen;
	m_pLog->LogBin(LOG_DEBUG,__FILE__,__LINE__,"返回结果",data->sBuffer,data->sDBHead.nLen);
	m_pPcLink->AnsData(data);
	return 0;
}

int CSampleNew::Test1003(PBPCCOMMSTRU data)
{
	m_pLogDat.LogBpc(LOG_DEBUG,data);
	data->sDBHead.cRaflag =1;
	data->sDBHead.cNextFlag = 0;
	data->sDBHead.cZip = 0;
	strcpy(data->sBuffer,"测试1003");
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
	strcpy(data->sBuffer,"测试1004");
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
	strcpy(data->sBuffer,"测试1005");
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
	strcpy(data->sBuffer,"测试1006");
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
	strcpy(data->sBuffer,"测试1007");
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
	strcpy(data->sBuffer,"测试1008");
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
	strcpy(data->sBuffer,"测试1009");
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
	strcpy(data->sBuffer,"测试1010");
	data->sDBHead.nLen = strlen(data->sBuffer);
	data->sBpcHead.nBpcLen = DREBHEADLEN+data->sDBHead.nLen;
	m_pPcLink->AnsData(data);
	return 0;
}
