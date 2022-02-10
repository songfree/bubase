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
	std::string nodeid;//�ڵ�����
	std::string processname;//��������
	std::string prgname;//��������
	std::string prgpath;//����Ŀ¼
	std::string runpath;//����Ŀ¼
	std::string prgpara;//���в���
	std::string stopscript;//ֹͣ�ű�
	std::string status;//״̬
	std::string heartstart;//�������Ƿ�����
	int         pid;//����id
	std::string acceptcontrol;//�Ƿ���ܿ���
	std::string controltype;//���Ƴ���
	int         lastUseTIme;//�ϴ�ʹ��ʱ��

	CBF_Mutex      mutex;//����ֹͣ�Ļ�����
}S_PROCESS_NODE;
//���̹�����
class CProcessMgr  
{
public:
	void Init();
	CProcessMgr();
	virtual ~CProcessMgr();


//	CNode *pPrgNode ������
//  <a name="ͨѶ������" prgname="java" prgpath="/usr/java5_64/bin" runpath="/home/appgess/ylink/gessserver" prgpara="" restar="0" status="0" pid="" />
	
	/// ������: StartProcess
	/// ���  : ������ 2009-6-12 12:14:35
	/// ����  : bool 
	/// ����  : S_PROCESS_NODE *pPrgNode
	/// ����  : ����һ������
	bool StartProcess(S_PROCESS_NODE *pPrgNode);

	/// ������: StopProcess
	/// ���  : ������ 2009-6-12 12:14:50
	/// ����  : int  =0�ɹ�ֹͣ ������ֹͣ 
	/// ����  : S_PROCESS_NODE *pPrgNode
	/// ����  : ֹͣһ������
	int StopProcess(S_PROCESS_NODE *pPrgNode);

	/// ������: FindProcess
	/// ���  : ������ 2009-6-12 12:14:54
	/// ����  : int  =0�ҵ�
	/// ����  : S_PROCESS_NODE *pPrgNode
	/// ����  : ����һ������
	int FindProcess(S_PROCESS_NODE *pPrgNode);


	bool SeparateFilePathAndFileName(char *pFilePathName, char *pFilePath,int iFilePathLen, char *pFileName, int iFileNameLen);

	CBF_SimpleLog m_log;
	//Cpubstrtime m_publog;
protected:
	
	// ������: CheckPrgRunPara
	// ���  : ������ 2013-12-14 14:37:17
	// ����  : bool 
	// ����  : S_PROCESS_NODE *pPrgNode
	// ����  : ����id���ں������Ŀ¼��������������Ŀ¼�Ƿ���ͬ
	bool CheckPrgRunPara(S_PROCESS_NODE *pPrgNode);
	
	void GetNamePath(const char *filepathname,char *filepath,char *filename);
};

#endif // !defined(AFX_PROCESSMGR_H__2F90E2B9_0A37_4887_BA5C_23C01DA150EE__INCLUDED_)
