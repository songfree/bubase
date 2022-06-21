// ProcThread.h: interface for the CProcThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROCTHREAD_H__B593B31C_E1FF_4EF6_B64D_9DDD29C19C3A__INCLUDED_)
#define AFX_PROCTHREAD_H__B593B31C_E1FF_4EF6_B64D_9DDD29C19C3A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BF_Thread.h"
#include "DbApi.h"
#include "Xdp.h"
#include "IClientLink.h"
#include "IExchLink.h"
#include "IProcBase.h"
#include "BF_BufferPool.h"
#include "MemDb.h"
#include "IErrLog.h"
#include "PoolData.h"

//处理线程
//此线程从队列取数据，根据交易码查找交易对应的动态库里的函数指针，调用处理

typedef struct
{
	CIProcBase             *iProcBase;//业务动态库的类指针
	DLLHANDLE              procdllhandle;//动态库句柄
	std::string            procdllname;//动态库名称
	std::string            procInstance;//实例的函数名
	std::string            procdllpath;//动态库路径，绝对
}S_PROCBASE_DLL;

class CProcThread : public CBF_Thread  
{
public:
	
	
	void Stop();
	bool Start();

	// 函数名: Init
	// 编程  : 王明松 2017-7-27 15:16:10
	// 返回  : bool 
	// 参数  : CBF_Xml *xml  配置文件内容
	// 描述  : 初始化，读取业务动态库，注册交易
	bool Init(CBF_Xml *xml);

	CProcThread();
	virtual ~CProcThread();

	CXdp            m_pXdp;//xdp报文
	CIExchLink     *m_pExchLink;//交易所连接调用  注意b_Cinfo.b_cIndex保存处理线的index，这样应答返回就知道放到哪个应答队列

	CIClientLink   *m_pClientLink; //客户端连接调用
	CMemDb         *m_pMemDb;//内存表
	CIErrlog       *m_pLog;
	CBF_Xml        *m_pXml;

	CPoolDataMsg    *m_pRcvQueue;//请求队列
	CPoolDataMsg    m_pAnsQueue;//同步应答队列
	CProcFuncTbl        m_pFuncList;

	unsigned  int *g_nSvrHostStatus;//主备状态
	CPubData   *g_pPubData;//公共数据类

	int    m_nIndex;
private:
	virtual int Run();

	
protected:

	bool           m_bIsRunning;
	S_TRADE_DATA   m_sData;

	bool OpenDll(DLLHANDLE *pDll, const char *dllname, const char *dllpath);

	CDbApi          *m_pSqlCmd; //数据库连接
	bool             m_bNeedConnectDb;
	bool             m_bAutoCommit;
	CBF_Des          m_pEncrypt;//des加解密类

	std::vector<S_PROCBASE_DLL> m_alldllclass; //保存动态库里的处理实例

	

};

#endif // !defined(AFX_PROCTHREAD_H__B593B31C_E1FF_4EF6_B64D_9DDD29C19C3A__INCLUDED_)
