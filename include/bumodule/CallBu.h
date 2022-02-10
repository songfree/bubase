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

	// ������: AnsErr
	// ���  : ������ 2013-5-3 16:09:24
	// ����  : void 
	// ����  : int errcode
	// ����  : const char *file
	// ����  : int fileline
	// ����  : const char *msg
	// ����  : PBPCCOMMSTRU data
	// ����  : ������
	void AnsErr(int errcode,const char *file,int fileline,const char *msg,PBPCCOMMSTRU data);

	// ������: GetLogClass
	// ���  : ������ 2010-7-6 17:20:58
	// ����  : CIBuLog * 
	// ����  : ������־��
	CBF_SimpleLog * GetLogClass();

	// ������: GetMsg
	// ���  : ������ 2010-7-6 17:21:07
	// ����  : std::string & 
	// ����  : ���س�����Ϣ
	std::string & GetMsg();

	// ������: GetXmlConfig
	// ���  : ������ 2010-7-6 17:21:16
	// ����  : CBFXml * 
	// ����  : ��������XML
	CBF_Xml * GetXmlConfig();

	// ������: SetPara
	// ���  : ������ 2010-7-6 16:48:13
	// ����  : void 
	// ����  : CIPcLink *pBulink
	// ����  : ���ú�PC���Ӳ���(BPCҵ��������/FPCǰ�ô�������)
	void SetPara(CIPcLink *pBulink);

	// ������: GetAllTxInfo
	// ���  : ������ 2010-7-5 16:21:14
	// ����  : int 
	// ����  : vector<S_TBL_FLOWTX> &txinfo
	// ����  : ��ȡ���еĽ�����Ϣ���Ա�ע�ᵽBCC
	int GetAllTxInfo(vector<S_TBL_FLOWTX> &txinfo);

	
	// ������: BuInit
	// ���  : ������ 2010-7-1 10:17:37
	// ����  : bool 
	// ����  : const char *filename
	// ����  : ��ʼ��
	bool BuInit(const char *filename);

	// ������: Run
	// ���  : ������ 2010-6-29 15:18:32
	// ����  : virtual int 
	// ����  : PBPCCOMMSTRU data
	// ����  : �����ڲ����̽��׻�ԭ�ӹ���
	virtual int Run(PBPCCOMMSTRU data);
	

	CCallBu();
	virtual ~CCallBu();

protected:
	
	// ������: GetTxInfo
	// ���  : ������ 2010-7-5 16:21:36
	// ����  : int 
	// ����  : PBPCCOMMSTRU data
	// ����  : ��ȡ������Ϣ
	int GetTxInfo(PBPCCOMMSTRU data);
	
	// ������: GetDllInfo
	// ���  : ������ 2010-7-5 16:21:45
	// ����  : int 
	// ����  : PBPCCOMMSTRU data
	// ����  : ��ȡ��̬����Ϣ
	int GetDllInfo(PBPCCOMMSTRU data);
	
	// ������: PutFile
	// ���  : ������ 2010-7-5 16:21:55
	// ����  : int 
	// ����  : PBPCCOMMSTRU data
	// ����  : �ϴ��ļ�
	int PutFile(PBPCCOMMSTRU data);
	
	// ������: GetFile
	// ���  : ������ 2010-7-5 16:22:04
	// ����  : int 
	// ����  : PBPCCOMMSTRU data
	// ����  : ȡ�ļ�
	int GetFile(PBPCCOMMSTRU data);
	
	// ������: RunLocalFunc
	// ���  : ������ 2010-7-5 15:09:45
	// ����  : int 
	// ����  : PBPCCOMMSTRU data
	// ����  : �����ڲ�����
	int RunLocalFunc(PBPCCOMMSTRU data);
	
	// ������: InitAll
	// ���  : ������ 2010-7-5 10:40:43
	// ����  : bool 
	// ����  : ��ʼ������
	bool InitAll();
	
	// ������: InitFlow
	// ���  : ������ 2010-7-1 15:36:00
	// ����  : bool 
	// ����  : ��ʼ�����̿���
	bool InitFlow();
	
	// ������: InitBu
	// ���  : ������ 2010-7-1 15:35:52
	// ����  : bool 
	// ����  : ��ʼ��BU
	bool InitBu();
	
	// ������: InitDatabase
	// ���  : ������ 2010-7-1 15:35:44
	// ����  : bool 
	// ����  : ��ʼ�����ݿ�
	bool InitDatabase();
	
	bool InitLogClass();

	CBF_Xml     m_pConfXml;     //����xml
	
	std::string m_errMsg;
	char        m_cMsg[200];
	CBuFuncMgr  m_pFuncMgr;//�����б�
	
	CBF_Des     m_pEncrypt;//des�ӽ�����
	
	std::string m_sConfigXmlFile;
	int         m_nFlowUse;       //�Ƿ�ʹ�����̿���
	bool        m_bNeedConnectDb;//�Ƿ��������ݿ�
	int         m_nAutoCommit;   //�Զ��ύ���ݿ�ѡ�� 1�Զ��ύ��������
	int         m_nAutoLoadDll;  //�Ƿ��Զ����ض�̬��
	int         m_nTimeTest;     //�Ƿ��¼����ʱ��
	int         m_nTxUnit2Func;  //�Ƿ��ԭ��ת��Ϊ����
	int         m_nTestLopBack;  //�Ƿ���Ի�·
	std::string m_sLopBackFilePath;//���Ի�·�ļ�Ŀ¼

	
	bool        m_bIsSetPara;
	CIPcLink        *m_pPcLink;      //PC�����࣬�ṩ�����
	CDbApi          *m_pSqlCmd;//���ݿ�api
	CBF_SimpleLog    m_pLog;     //��־��
	CBuFlowControl  *m_pFlow;    //���̿���


};
#undef BUMODULE_EXPORT
#endif // !defined(AFX_CALLBU_H__7860C80B_A128_4E4F_9920_F5486A403FB5__INCLUDED_)
