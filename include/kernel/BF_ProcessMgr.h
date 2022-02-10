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

#ifdef _WINDOWS
#include "psapi.h"
#pragma comment(lib, "psapi.lib") 
#pragma message("Automatically linking with   psapi.lib")
#endif 

#ifdef KERNEL_EXPORTS
#define KERNEL_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define KERNEL_EXPORT __declspec(dllimport)
#else
#define KERNEL_EXPORT 
#endif
#endif

typedef struct _PROCESS_NODE
{
	std::string nodeid;//节点名称
	std::string processname;//进程名称
	std::string prgname;//程序名称
	std::string prgpath;//程序目录
	std::string runpath;//运行目录
	std::string prgpara;//运行参数
	std::string stopscript;//停止脚本
	std::string status;//状态  0为正常 1为停止 2为无心跳
	std::string heartstart;//无心跳是否重启
	int         pid;//进程id
	std::string acceptcontrol;//是否接受控制  
	std::string controltype;//控制程序  //0为由主机资源控制启动  1为由客户端远程控制启动，只是客户端停止时置为1
	int         lastUseTIme;//上次使用时间
	int         winconsole;//windows下启动是否需要显示控制台  0,不显示 1显示
	CBF_Mutex      mutex;//启动停止的互斥量
} S_PROCESS_NODE;
//进程管理类
class KERNEL_EXPORT CBF_ProcessMgr  
{
public:
	
	CBF_ProcessMgr();
	virtual ~CBF_ProcessMgr();

	/// 函数名: StartProcess
	/// 编程  : 王明松 2009-6-12 12:14:35
	/// 返回  : bool 
	/// 参数  : S_PROCESS_NODE *pPrgNode
	/// 参数  : char *errmsg 出错信息
	/// 参数  : bool noConsole     WINDOWS下是否有控制台窗口
	/// 描述  : 启动一个进程
	bool StartProcess(S_PROCESS_NODE *pPrgNode,char *errmsg,bool noConsole=false);

	/// 函数名: StopProcess
	/// 编程  : 王明松 2009-6-12 12:14:50
	/// 返回  : int  =0成功停止 其它已停止 
	/// 参数  : S_PROCESS_NODE *pPrgNode
	/// 参数  : char *errmsg 出错信息
	/// 描述  : 停止一个进程
	int StopProcess(S_PROCESS_NODE *pPrgNode,char *errmsg);

	/// 函数名: FindProcess
	/// 编程  : 王明松 2009-6-12 12:14:54
	/// 返回  : int  =0找到
	/// 参数  : S_PROCESS_NODE *pPrgNode
	/// 参数  : char *errmsg 出错信息
	/// 描述  : 查找一个进程
	int FindProcess(S_PROCESS_NODE *pPrgNode,char *errmsg);


	// 函数名: SetLogPara
	// 编程  : 王明松 2014-11-14 9:55:23
	// 返回  : void 
	// 参数  : int level
	// 参数  : const char *logfilepath
	// 参数  : const char *logfilename
	// 参数  : int id=-1
	// 描述  : 设置进程管理日志参数
	void SetLogPara(int level,const char *logfilepath,const char *logfilename,int id=-1);

	

protected:

	CBF_SimpleLog m_log;

	// 函数名: CheckPrgRunPara
	// 编程  : 王明松 2013-12-14 14:37:17
	// 返回  : int 0存在  非0不存在
	// 参数  : S_PROCESS_NODE *pPrgNode
	// 参数  : char *errmsg 出错信息
	// 描述  : 进程id存在后的检查 linux检查运行目录、程序目录及程序名 
	// windows若程序是JAVA.exe则直接返回0,否则检查程序目录和参数
	// hp_ux和aix不做检查直接返回0
	int CheckPrgRunPara(S_PROCESS_NODE *pPrgNode,char *errmsg);

	// 函数名: SeparateFilePathAndFileName
	// 编程  : 王明松 2009-11-14 9:24:45
	// 返回  : bool 
	// 参数  : char *pFilePathName
	// 参数  : char *pFilePath
	// 参数  : int iFilePathLen
	// 参数  : char *pFileName
	// 参数  : int iFileNameLen
	// 描述  : 得到文件名和路径
	bool SeparateFilePathAndFileName(char *pFilePathName, char *pFilePath,int iFilePathLen, char *pFileName, int iFileNameLen);
	void GetNamePath(const char *filepathname,char *filepath,char *filename);
	

};

#endif // !defined(AFX_PROCESSMGR_H__2F90E2B9_0A37_4887_BA5C_23C01DA150EE__INCLUDED_)
