/**
* Copyright (c) 2008 songfree
* All rights reserved.
*
* 文件名称:%s.cpp
* 摘要:
*       
*        
*
* 当前版本号:1.0.0
*
* 版本历史信息:
* 版本号            作者/修改者       完成日期                 原因
* 1.0.0                AUTO           %s                  创建
**/
// %s.cpp: implementation of the C%s class.
//
//////////////////////////////////////////////////////////////////////



#include "%s.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


C%s::~C%s()
{

}
bool C%s::BuInit()
{
	m_bIsInit = true;
	return m_bIsInit;
}


void C%s::SetBuPara(CBF_Xml *conxml,CIPcLink *pclink,CDbApi *sqldb,CBF_SimpleLog *bulog)
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
//注意：下面的定义在每个类里面都要有，主程序根据此方法获取本类的实例
C%s * getInstance() 
{ 
    return new C%s(); 
}
C%s::C%s()
{
	
	//功能号 函数指针  功能名称 版本 作者  优先级  冲正标志 是否交易单位  是否停用
	//InitFuncInfo(1000,(FuncPointer )&CSample::test01,"测试",1.0,"作者",1,false,true,false);
	//新增加功能要在此进行设置
}


