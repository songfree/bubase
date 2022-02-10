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
// SampleNew.h: interface for the CSampleNew class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(BUBASE_AUTO_APPWIZARD_20100708_33383875_INCLUDED_)
#define BUBASE_AUTO_APPWIZARD_20100708_33383875_INCLUDED_

#include "BuModule.h"
#include "DrebBpcLog.h"


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class DLL_EXPORT CSampleNew : public CBuModule  
{
public:
	
public:
	CSampleNew();
	virtual ~CSampleNew();

public:
	// 函数名: BuInit
	// 编程  : 王明松 2010-6-29 16:17:11
	// 返回  : virtual bool 
	// 描述  : 初始化，可在此方法里做一些特殊处理，但要置m_bIsInit为true
	virtual bool BuInit();
	
//      在此处增加新功能
public:
	int Test1001(PBPCCOMMSTRU data);
	int Test1002(PBPCCOMMSTRU data);
	int Test1003(PBPCCOMMSTRU data);
	int Test1004(PBPCCOMMSTRU data);
	int Test1005(PBPCCOMMSTRU data);
	int Test1006(PBPCCOMMSTRU data);
	int Test1007(PBPCCOMMSTRU data);
	int Test1008(PBPCCOMMSTRU data);
	int Test1009(PBPCCOMMSTRU data);
	int Test1010(PBPCCOMMSTRU data);
	int Test99001(PBPCCOMMSTRU data);
	CDrebBpcLog m_pLogDat;

};
//注意：下面的定义在每个类里面都要有，主程序根据此方法获取本类的实例
extern "C" 
{ 
    DLL_EXPORT CSampleNew * getInstance() ; 
}

#endif // !BUBASE_AUTO_APPWIZARD_20100708_33383875_INCLUDED_)
