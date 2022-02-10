// CallBu.h: interface for the CCallBu class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CALLBU_H__7860C80B_A128_4E4F_9920_F5486A403FB5__INCLUDED_)
#define AFX_CALLBU_H__7860C80B_A128_4E4F_9920_F5486A403FB5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "public.h"
#include "BF_Xml.h"	// Added by ClassView
#include "BF_SimpleLog.h"
#include "BuFuncMgr.h"
#include "DbApi.h"
#include "BF_3Des.h"
#include "BuFlowControl.h"



#ifdef BUMODULE_EXPORTS
#define BUMODULE_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define BUMODULE_EXPORT __declspec(dllimport)
#else
#define BUMODULE_EXPORT 
#endif
#endif

class BUMODULE_EXPORT CCallBu   
{
public:

	// 函数名: AnsErr
	// 编程  : 王明松 2013-5-3 16:09:24
	// 返回  : void 
	// 参数  : int errcode
	// 参数  : const char *file
	// 参数  : int fileline
	// 参数  : const char *msg
	// 参数  : PBPCCOMMSTRU data
	// 描述  : 出错返回
	void AnsErr(int errcode,const char *file,int fileline,const char *msg,PBPCCOMMSTRU data);

	// 函数名: GetLogClass
	// 编程  : 王明松 2010-7-6 17:20:58
	// 返回  : CIBuLog * 
	// 描述  : 返回日志类
	CBF_SimpleLog * GetLogClass();

	// 函数名: GetMsg
	// 编程  : 王明松 2010-7-6 17:21:07
	// 返回  : std::string & 
	// 描述  : 返回出错信息
	std::string & GetMsg();

	// 函数名: GetXmlConfig
	// 编程  : 王明松 2010-7-6 17:21:16
	// 返回  : CBFXml * 
	// 描述  : 返回配置XML
	CBF_Xml * GetXmlConfig();

	// 函数名: SetPara
	// 编程  : 王明松 2010-7-6 16:48:13
	// 返回  : void 
	// 参数  : CIPcLink *pBulink
	// 描述  : 设置和PC连接参数(BPC业务处理中心/FPC前置处理中心)
	void SetPara(CIPcLink *pBulink);

	// 函数名: GetAllTxInfo
	// 编程  : 王明松 2010-7-5 16:21:14
	// 返回  : int 
	// 参数  : vector<S_TBL_FLOWTX> &txinfo
	// 描述  : 获取所有的交易信息，以便注册到BCC
	int GetAllTxInfo(vector<S_TBL_FLOWTX> &txinfo);

	
	// 函数名: BuInit
	// 编程  : 王明松 2010-7-1 10:17:37
	// 返回  : bool 
	// 参数  : const char *filename
	// 描述  : 初始化
	bool BuInit(const char *filename);

	// 函数名: Run
	// 编程  : 王明松 2010-6-29 15:18:32
	// 返回  : virtual int 
	// 参数  : PBPCCOMMSTRU data
	// 描述  : 调用内部流程交易或原子功能
	virtual int Run(PBPCCOMMSTRU data);
	

	CCallBu();
	virtual ~CCallBu();

protected:
	
	// 函数名: GetTxInfo
	// 编程  : 王明松 2010-7-5 16:21:36
	// 返回  : int 
	// 参数  : PBPCCOMMSTRU data
	// 描述  : 获取交易信息
	int GetTxInfo(PBPCCOMMSTRU data);
	
	// 函数名: GetDllInfo
	// 编程  : 王明松 2010-7-5 16:21:45
	// 返回  : int 
	// 参数  : PBPCCOMMSTRU data
	// 描述  : 获取动态库信息
	int GetDllInfo(PBPCCOMMSTRU data);
	
	// 函数名: PutFile
	// 编程  : 王明松 2010-7-5 16:21:55
	// 返回  : int 
	// 参数  : PBPCCOMMSTRU data
	// 描述  : 上传文件
	int PutFile(PBPCCOMMSTRU data);
	
	// 函数名: GetFile
	// 编程  : 王明松 2010-7-5 16:22:04
	// 返回  : int 
	// 参数  : PBPCCOMMSTRU data
	// 描述  : 取文件
	int GetFile(PBPCCOMMSTRU data);
	
	// 函数名: RunLocalFunc
	// 编程  : 王明松 2010-7-5 15:09:45
	// 返回  : int 
	// 参数  : PBPCCOMMSTRU data
	// 描述  : 运行内部交易
	int RunLocalFunc(PBPCCOMMSTRU data);
	
	// 函数名: InitAll
	// 编程  : 王明松 2010-7-5 10:40:43
	// 返回  : bool 
	// 描述  : 初始化所有
	bool InitAll();
	
	// 函数名: InitFlow
	// 编程  : 王明松 2010-7-1 15:36:00
	// 返回  : bool 
	// 描述  : 初始化流程控制
	bool InitFlow();
	
	// 函数名: InitBu
	// 编程  : 王明松 2010-7-1 15:35:52
	// 返回  : bool 
	// 描述  : 初始化BU
	bool InitBu();
	
	// 函数名: InitDatabase
	// 编程  : 王明松 2010-7-1 15:35:44
	// 返回  : bool 
	// 描述  : 初始化数据库
	bool InitDatabase();
	
	bool InitLogClass();

	CBF_Xml     m_pConfXml;     //配置xml
	
	std::string m_errMsg;
	char        m_cMsg[200];
	CBuFuncMgr  m_pFuncMgr;//功能列表
	
	CBF_Des     m_pEncrypt;//des加解密类
	
	std::string m_sConfigXmlFile;
	int         m_nFlowUse;       //是否使用流程控制
	bool        m_bNeedConnectDb;//是否连接数据库
	int         m_nAutoCommit;   //自动提交数据库选项 1自动提交，其它否
	int         m_nAutoLoadDll;  //是否自动重载动态库
	int         m_nTimeTest;     //是否记录交易时间
	int         m_nTxUnit2Func;  //是否把原子转换为交易
	int         m_nTestLopBack;  //是否测试回路
	std::string m_sLopBackFilePath;//测试回路文件目录

	
	bool        m_bIsSetPara;
	CIPcLink        *m_pPcLink;      //PC连接类，提供外调等
	CDbApi          *m_pSqlCmd;//数据库api
	CBF_SimpleLog    m_pLog;     //日志类
	CBuFlowControl  *m_pFlow;    //流程控制


};
#undef BUMODULE_EXPORT
#endif // !defined(AFX_CALLBU_H__7860C80B_A128_4E4F_9920_F5486A403FB5__INCLUDED_)
