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
	std::string nodeid;//�ڵ�����
	std::string processname;//��������
	std::string prgname;//��������
	std::string prgpath;//����Ŀ¼
	std::string runpath;//����Ŀ¼
	std::string prgpara;//���в���
	std::string stopscript;//ֹͣ�ű�
	std::string status;//״̬  0Ϊ���� 1Ϊֹͣ 2Ϊ������
	std::string heartstart;//�������Ƿ�����
	int         pid;//����id
	std::string acceptcontrol;//�Ƿ���ܿ���  
	std::string controltype;//���Ƴ���  //0Ϊ��������Դ��������  1Ϊ�ɿͻ���Զ�̿���������ֻ�ǿͻ���ֹͣʱ��Ϊ1
	int         lastUseTIme;//�ϴ�ʹ��ʱ��
	int         winconsole;//windows�������Ƿ���Ҫ��ʾ����̨  0,����ʾ 1��ʾ
	CBF_Mutex      mutex;//����ֹͣ�Ļ�����
} S_PROCESS_NODE;
//���̹�����
class KERNEL_EXPORT CBF_ProcessMgr  
{
public:
	
	CBF_ProcessMgr();
	virtual ~CBF_ProcessMgr();

	/// ������: StartProcess
	/// ���  : ������ 2009-6-12 12:14:35
	/// ����  : bool 
	/// ����  : S_PROCESS_NODE *pPrgNode
	/// ����  : char *errmsg ������Ϣ
	/// ����  : bool noConsole     WINDOWS���Ƿ��п���̨����
	/// ����  : ����һ������
	bool StartProcess(S_PROCESS_NODE *pPrgNode,char *errmsg,bool noConsole=false);

	/// ������: StopProcess
	/// ���  : ������ 2009-6-12 12:14:50
	/// ����  : int  =0�ɹ�ֹͣ ������ֹͣ 
	/// ����  : S_PROCESS_NODE *pPrgNode
	/// ����  : char *errmsg ������Ϣ
	/// ����  : ֹͣһ������
	int StopProcess(S_PROCESS_NODE *pPrgNode,char *errmsg);

	/// ������: FindProcess
	/// ���  : ������ 2009-6-12 12:14:54
	/// ����  : int  =0�ҵ�
	/// ����  : S_PROCESS_NODE *pPrgNode
	/// ����  : char *errmsg ������Ϣ
	/// ����  : ����һ������
	int FindProcess(S_PROCESS_NODE *pPrgNode,char *errmsg);


	// ������: SetLogPara
	// ���  : ������ 2014-11-14 9:55:23
	// ����  : void 
	// ����  : int level
	// ����  : const char *logfilepath
	// ����  : const char *logfilename
	// ����  : int id=-1
	// ����  : ���ý��̹�����־����
	void SetLogPara(int level,const char *logfilepath,const char *logfilename,int id=-1);

	

protected:

	CBF_SimpleLog m_log;

	// ������: CheckPrgRunPara
	// ���  : ������ 2013-12-14 14:37:17
	// ����  : int 0����  ��0������
	// ����  : S_PROCESS_NODE *pPrgNode
	// ����  : char *errmsg ������Ϣ
	// ����  : ����id���ں�ļ�� linux�������Ŀ¼������Ŀ¼�������� 
	// windows��������JAVA.exe��ֱ�ӷ���0,���������Ŀ¼�Ͳ���
	// hp_ux��aix�������ֱ�ӷ���0
	int CheckPrgRunPara(S_PROCESS_NODE *pPrgNode,char *errmsg);

	// ������: SeparateFilePathAndFileName
	// ���  : ������ 2009-11-14 9:24:45
	// ����  : bool 
	// ����  : char *pFilePathName
	// ����  : char *pFilePath
	// ����  : int iFilePathLen
	// ����  : char *pFileName
	// ����  : int iFileNameLen
	// ����  : �õ��ļ�����·��
	bool SeparateFilePathAndFileName(char *pFilePathName, char *pFilePath,int iFilePathLen, char *pFileName, int iFileNameLen);
	void GetNamePath(const char *filepathname,char *filepath,char *filename);
	

};

#endif // !defined(AFX_PROCESSMGR_H__2F90E2B9_0A37_4887_BA5C_23C01DA150EE__INCLUDED_)
