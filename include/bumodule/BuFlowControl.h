// BuFlowControl.h: interface for the CBuFlowControl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BUFLOWCONTROL_H__0AA03C29_25AA_4558_B3F6_2B99F595A1B7__INCLUDED_)
#define AFX_BUFLOWCONTROL_H__0AA03C29_25AA_4558_B3F6_2B99F595A1B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "public.h"
#include "BF_Xml.h"
#include "IPcLink.h"
#include "BF_SimpleLog.h"
#include "DbApi.h"

#ifdef BUMODULE_EXPORTS
#define BUMODULE_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define BUMODULE_EXPORT __declspec(dllimport)
#else
#define BUMODULE_EXPORT 
#endif
#endif

typedef struct _S_TBL_FLOWTX
{
	int         nFuncNo; //功能号 
	std::string sFuncName;///功能名称
	char        cPrio;//优先级
	char        cServiceFlag;//服务标志
}S_TBL_FLOWTX;

class BUMODULE_EXPORT CBuFlowControl  
{
public:
	CBuFlowControl();
	virtual ~CBuFlowControl();

	// 函数名: SetBuPara
	// 编程  : 王明松 2010-6-30 11:00:30
	// 返回  : void 
	// 参数  : CBuXml *conxml
	// 参数  : CICallBu *callbu
	// 参数  : CDbApi *sqldb
	// 参数  : CIBuLog *bulog
	// 描述  : 设置参数
	virtual void SetBuPara(CBF_Xml *conxml,CIPcLink *callbu,CDbApi *sqldb,CBF_SimpleLog *bulog);
	
	
	// 函数名: InitFlow
	// 编程  : 王明松 2010-7-2 10:53:20
	// 返回  : virtual bool true成功false失败
	// 描述  : 初始化流程控制
	virtual bool InitFlow();
	
	
	// 函数名: IsRunFlow
	// 编程  : 王明松 2010-7-2 11:00:40
	// 返回  : virtual bool 
	// 参数  : int nTxcode
	// 描述  : 是否需要运行流程
	virtual bool IsRunFlow(int nTxcode);
	
	// 函数名:  RunFlow
	// 编程  : 王明松 2010-7-2 10:54:38
	// 返回  : virtual int   =0调用成功--提交  <0调用失败--回滚 要求在外面进行提交与回滚
	// 参数  : PBPCCOMMSTRU data
	// 描述  : 调用流程
	virtual int  RunFlow(PBPCCOMMSTRU data);
	
	
	// 函数名:  GetFlowFunc
	// 编程  : 王明松 2010-7-5 15:08:44
	// 返回  : virtual int 
	// 参数  : vector<S_TBL_FLOWTX> &txlist
	// 描述  : 取得流程交易信息
	virtual int  GetFlowFunc(std::vector<S_TBL_FLOWTX> &txlist);

protected:
	CDbApi         *m_pSqlCmd;//数据库api
	CBF_SimpleLog  *m_pLog;             //日志类
	CIPcLink       *m_pCallBu;          //内部功能调用
	CBF_Xml        *m_pConfXml;         //配置xml
	bool           m_bIsSetPara;
};
#undef BUMODULE_EXPORT
#endif // !defined(AFX_BUFLOWCONTROL_H__0AA03C29_25AA_4558_B3F6_2B99F595A1B7__INCLUDED_)
