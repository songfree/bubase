// IBuModule.h: interface for the CIBuModule class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IBUMODULE_H__F6A715DF_DB67_4547_B40B_1259BA78EE15__INCLUDED_)
#define AFX_IBUMODULE_H__F6A715DF_DB67_4547_B40B_1259BA78EE15__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "BpcHead.h"
#include "XmlPack.h"

class CIBuModule;

typedef  int(CIBuModule::*FuncPointer)(PBPCCOMMSTRU data);

typedef  int(CIBuModule::*FuncPointerOld)(CXmlPack *xmlpack,CXmlPackArray *muxmlpack);


typedef struct _S_TBL_FUNC
{   
	int         nFuncNo; //功能号 
	FuncPointer pFunc;///函数指针
	char        sFuncName[81];///功能名称
	double      dVersion;///版本
	char        sAuthor[41];///作者
	int         nLevel;///优先级
	bool        bRepealFlag;///冲正标志
	CIBuModule *pBuInstance;///处理类实例
	char        sDllName[201];///动态库名称
	bool        bIsTxUnit;///是否是交易单位(如果是交易单位则不需在bcc中注册，由流程控制来调用，否则要在bcc注册)
	bool        bIsStop;///此功能是否停止
	unsigned int nCallNum;//调用次数
}S_TBL_FUNC;
typedef struct _S_TBL_FUNC_OLD
{   
	int         nFuncNo; //功能号 
	FuncPointerOld pFunc;///函数指针
	char        sFuncName[81];///功能名称
	double      dVersion;///版本
	char        sAuthor[41];///作者
	int         nLevel;///优先级
	bool        bRepealFlag;///冲正标志
	CIBuModule *pBuInstance;///处理类实例
	char        sDllName[201];///动态库名称
	bool        bIsTxUnit;///是否是交易单位(如果是交易单位则不需在bcc中注册，由流程控制来调用，否则要在bcc注册)
	bool        bIsStop;///此功能是否停止
	unsigned int nCallNum;//调用次数
}S_TBL_FUNC_OLD;  //为了兼容早期的框架xmlpack 

class CIBuModule  
{
public:
	
	CIBuModule()
	{

	}
	virtual ~CIBuModule()
	{

	}
	// 函数名: NextFuncInfo
	// 编程  : 王明松 2010-7-1 9:20:42
	// 返回  : virtual bool 
	// 参数  : S_TBL_FUNC &funcinfo
	// 描述  : 得到下一个功能信息，前提是要调用FirstFuncInfo
	virtual bool NextFuncInfo(S_TBL_FUNC &funcinfo)=0;

	// 函数名: FirstFuncInfo
	// 编程  : 王明松 2010-7-1 9:20:30
	// 返回  : virtual bool 
	// 参数  : S_TBL_FUNC &funcinfo
	// 描述  : 得到第一个功能信息
	virtual bool FirstFuncInfo(S_TBL_FUNC &funcinfo)=0;

	// 函数名: Run
	// 编程  : 王明松 2010-6-29 16:17:00
	// 返回  : virtual int 
	// 参数  : PBPCCOMMSTRU data
	// 描述  : 调用原子功能
	virtual int Run(PBPCCOMMSTRU data) =0;

	// 函数名: BuInit
	// 编程  : 王明松 2010-6-29 16:17:11
	// 返回  : virtual bool 
	// 描述  : 初始化，可在此方法里做一些特殊处理，但要置m_bIsInit为true
	virtual bool BuInit()=0;


};

#endif // !defined(AFX_IBUMODULE_H__F6A715DF_DB67_4547_B40B_1259BA78EE15__INCLUDED_)
