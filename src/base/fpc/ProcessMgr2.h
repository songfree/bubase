// ProcessMgr.h: interface for the CProcessMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROCESSMGR_H__2F90E2B9_0A37_4887_BA5C_23C01DA150EE__INCLUDED_)
#define AFX_PROCESSMGR_H__2F90E2B9_0A37_4887_BA5C_23C01DA150EE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "public.h"
#include "BF_SimpleLog.h"
#include "BF_Mutex.h"

typedef struct _PROCESS_NODE
{
	std::string nodeid;//节点名称
	std::string processname;//进程名称
	std::string prgname;//程序名称
	std::string prgpath;//程序目录
	std::string runpath;//运行目录
	std::string prgpara;//运行参数
	std::string stopscript;//停止脚本
	std::string status;//状态
	std::string heartstart;//无心跳是否重启
	int         pid;//进程id
	std::string acceptcontrol;//是否接受控制
	std::string controltype;//控制程序
	int         lastUseTIme;//上次使用时间

	CBF_Mutex      mutex;//启动停止的互斥量
}S_PROCESS_NODE;
//进程管理类
class CProcessMgr  
{
public:
	void Init();
	CProcessMgr();
	virtual ~CProcessMgr();


//	CNode *pPrgNode 的内容
//  <a name="通讯服务器" prgname="java" prgpath="/usr/java5_64/bin" runpath="/home/appgess/ylink/gessserver" prgpara="" restar="0" status="0" pid="" />
	
	/// 函数名: StartProcess
	/// 编程  : 王明松 2009-6-12 12:14:35
	/// 返回  : bool 
	/// 参数  : S_PROCESS_NODE *pPrgNode
	/// 描述  : 启动一个进程
	bool StartProcess(S_PROCESS_NODE *pPrgNode);

	/// 函数名: StopProcess
	/// 编程  : 王明松 2009-6-12 12:14:50
	/// 返回  : int  =0成功停止 其它已停止 
	/// 参数  : S_PROCESS_NODE *pPrgNode
	/// 描述  : 停止一个进程
	int StopProcess(S_PROCESS_NODE *pPrgNode);

	/// 函数名: FindProcess
	/// 编程  : 王明松 2009-6-12 12:14:54
	/// 返回  : int  =0找到
	/// 参数  : S_PROCESS_NODE *pPrgNode
	/// 描述  : 查找一个进程
	int FindProcess(S_PROCESS_NODE *pPrgNode);


	bool SeparateFilePathAndFileName(char *pFilePathName, char *pFilePath,int iFilePathLen, char *pFileName, int iFileNameLen);

	CBF_SimpleLog m_log;
	//Cpubstrtime m_publog;
protected:
	
	// 函数名: CheckPrgRunPara
	// 编程  : 王明松 2013-12-14 14:37:17
	// 返回  : bool 
	// 参数  : S_PROCESS_NODE *pPrgNode
	// 描述  : 进程id存在后检查程序目录，程序名，运行目录是否相同
	bool CheckPrgRunPara(S_PROCESS_NODE *pPrgNode);
	
	void GetNamePath(const char *filepathname,char *filepath,char *filename);
};

#endif // !defined(AFX_PROCESSMGR_H__2F90E2B9_0A37_4887_BA5C_23C01DA150EE__INCLUDED_)
