// BuFuncMgr.h: interface for the CBuFuncMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BUFUNCMGR_H__B8471207_2203_46BC_BAA1_B2D787065AE2__INCLUDED_)
#define AFX_BUFUNCMGR_H__B8471207_2203_46BC_BAA1_B2D787065AE2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BuModule.h"
#include "BF_Xml.h"




#ifdef BUMODULE_EXPORTS
#define BUMODULE_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define BUMODULE_EXPORT __declspec(dllimport)
#else
#define BUMODULE_EXPORT 
#endif
#endif

class BUMODULE_EXPORT CBuFuncMgr  
{
public:
	char * GetMsg();


	// 函数名: Clear
	// 编程  : 王明松 2010-7-1 14:59:35
	// 返回  : void 
	// 描述  : 清空功能及原子交易表及实例及动态库句柄
	void Clear();

	// 函数名: OpenDll
	// 编程  : 王明松 2010-7-1 14:58:05
	// 返回  : bool 
	// 参数  : DLLHANDLE *pDll  返回的动态库句柄
	// 参数  : const char *dllname  动态库名称
	// 参数  : const char *dllpath  动态库所在目录
	// 描述  : 打开动态库，返回动态库的句柄
	bool OpenDll(DLLHANDLE *pDll, const char *dllname,const char *dllpath);

	// 函数名: SetBuPara
	// 编程  : 王明松 2010-7-1 17:07:49
	// 返回  : void 
	// 参数  : CBFXml *conxml
	// 参数  : CIPcLink *callbu
	// 参数  : CDbApi *sqldb
	// 参数  : CIBuLog *bulog
	// 描述  : 设置BU运行参数
	void SetBuPara(CBF_Xml *conxml,CIPcLink *callbu,CDbApi *sqldb,CBF_SimpleLog *bulog);

	// 函数名: InitFuncInfo
	// 编程  : 王明松 2010-7-1 14:57:10
	// 返回  : bool 
	// 描述  : 初始化功能及原子交易，注意使用前先调用SetPara,可重复调用
	bool InitFuncInfo();

	CBuFuncMgr();
	virtual ~CBuFuncMgr();

	CFuncTbl  m_pFuncTbl;     //所有注册的功能(交易)表
	CFuncTbl  m_pTxUnitTbl;   //所有注册的原子功能表
	
protected:


	// 函数名: RegisterFunc
	// 编程  : 王明松 2010-7-1 14:59:07
	// 返回  : bool 
	// 参数  : S_TBL_FUNC funcinfo
	// 描述  : 注册功能或原子交易
	bool RegisterFunc(S_TBL_FUNC funcinfo);

	
	CBF_SimpleLog  *m_pLog;
	CBF_Xml    *m_pConfigXml;//配置文件
	CIPcLink   *m_pExtCall;//外部调用
	CDbApi    *m_pSqlCmd; //数据库连接

	
	char m_errMsg[256];
	
	vector <CBuModule *> m_alldllclass;
	vector <DLLHANDLE> m_allhandle;

};
#undef BUMODULE_EXPORT
#endif // !defined(AFX_BUFUNCMGR_H__B8471207_2203_46BC_BAA1_B2D787065AE2__INCLUDED_)
