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
// %s.h: interface for the C%s class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(%s)
#define %s

#include "BuModule.h"



#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class DLL_EXPORT C%s : public CBuModule  
{
public:
	
public:
	C%s();
	virtual ~C%s();

public:
	// 函数名: BuInit
	// 编程  : 王明松 2010-6-29 16:17:11
	// 返回  : virtual bool 
	// 描述  : 初始化，可在此方法里做一些特殊处理，但要置m_bIsInit为true
	virtual bool BuInit();
	

	// 函数名: SetBuPara
	// 编程  : 王明松 2010-6-30 11:00:30
	// 返回  : void 
	// 参数  : CBuXml *conxml
	// 参数  : CICallBu *callbu
	// 参数  : CDbApi *sqldb
	// 参数  : CIBuLog *bulog
	// 描述  : 设置参数
	virtual void SetBuPara(CBF_Xml *conxml,CIPcLink *pclink,CDbApi *sqldb,CBF_SimpleLog *bulog);
//      在此处增加新功能
public:
	
	

};
//注意：下面的定义在每个类里面都要有，主程序根据此方法获取本类的实例
extern "C" 
{ 
    DLL_EXPORT C%s * getInstance() ; 
}

#endif // !%s)
