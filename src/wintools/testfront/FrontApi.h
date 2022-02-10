// FrontApi.h: interface for the CFrontApi class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FRONTAPI_H__8E987EC6_8C8C_45BC_A26F_40C54476EA64__INCLUDED_)
#define AFX_FRONTAPI_H__8E987EC6_8C8C_45BC_A26F_40C54476EA64__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "BF_Mutex.h"
#include "BF_SocketTcp.h"
#include "BF_SimpleLog.h"
#include "BF_Date_Time.h"
#include "BF_Tools.h"



#ifdef API_EXPORTS
#define API_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define API_EXPORT __declspec(dllimport)
#else
#define API_EXPORT 
#endif
#endif

class API_EXPORT CFrontApi  
{
public:
	CFrontApi();
	virtual ~CFrontApi();


public: 
	
public: //����ΪͨѶ��api����
	

	/// ������: IsConnect
	/// ���  : ������ 2008-10-27 15:11:41
	/// ����  : bool 
	/// ����  : �Ƿ�������Զ�̷����
	bool IsConnect();

	/// ������: GetErrMsg
	/// ���  : ������ 2008-10-27 15:11:56
	/// ����  : string 
	/// ����  : ��ȡ������Ϣ
	string GetErrMsg();


	/// ������: SetPara
	/// ���  : ������ 2008-10-29 9:30:33
	/// ����  : void 
	/// ����  : const char *ip Զ�̷����IP
	/// ����  : int port Զ�̷���˶˿�
	/// ����  : const char *logname ��־�ļ���
	/// ����  : int loglevel ��־����
	/// ����  : int headlen=5 ͨѶ����ͷ����
	/// ����  : bool isincludehead=false ͨѶ����ͷ�Ƿ��������
	/// ����  : int pwdflag=0  0������1���� 9�Զ�����ܣ���Ҫ���ü��ܵ��෽��
	/// ����  : int encryptflag=0 0�������벻ѹ�� 1���� 2ѹ��3DES����4DES���ܲ�ѹ��5zlibѹ�� 9�Զ������ ��Ҫ���ü��ܵ��෽��
	/// ����  : ����ͨѶ����
	void SetPara(const char *ip,int port,const char *logname,int loglevel,int headlen=5,bool isincludehead=false,int pwdflag=0,int encryptflag=0);

	/// ������: DisConnect
	/// ���  : ������ 2008-10-27 15:10:13
	/// ����  : void 
	/// ����  : �Ͽ�����
	void DisConnect();

	/// ������: Connect
	/// ���  : ������ 2008-10-27 15:09:52
	/// ����  : bool true�ɹ� falseʧ��
	/// ����  : ����Զ�̷����
	bool Connect();

	

	/// ������: RecvBuf
	/// ���  : ������ 2008-10-27 15:07:35
	/// ����  : int <0����ʧ�ܣ�Ҫ�������� =0δ�ӵ����� >0���յ������ݳ���
	/// ����  : char *buffer ���ݻ���
	/// ����  : int &buflen  ����Ϊ�����С ���Ϊ���յ����ݳ���
	/// ����  : int timeout ���ճ�ʱʱ��
	/// ����  : �������������壬����ʧ�ܹر����ӣ������������ݣ��������ݽ���macУ��
	int RecvBuf(char *buffer,int &buflen,int timeout);

	

	/// ������: SendBuf
	/// ���  : ������ 2008-10-27 15:06:11
	/// ����  : int <=0ʧ�� >0�ɹ�
	/// ����  : const char *buffer ���ݻ���
	/// ����  : int buflen ���ݳ���
	/// ����  : �������ݣ�������������mac��Ҳ����������м��ܣ�������������
	int SendBuf(const char *buffer,int buflen);

	/// ������: SetXTSegTime
	/// ���  : ������ 2008-10-29 10:48:10
	/// ����  : void 
	/// ����  : int ntime=600000
	/// ����  : ����������ʱ�䣬��������ntime����û��ʹ�ã���������������
	void SetXTSegTime(int ntime=600000);
	
	
	
	

protected:
	CBF_SocketTcp m_tcpSocket;
	CBF_Mutex m_mutexSend;//���ͻ�����
	CBF_Mutex m_mutexRcv;//���ջ�����

	std::string m_sServerIp;//�����IP
	int m_nPort;//�˿�
	int m_nTimeOut;//���ճ�ʱ
	unsigned int m_nHeadLen;//����ͷ����
	bool m_bIsIncludeSelf;//����ͷ�Ƿ��������
	bool m_bIsConnect;//�Ƿ������Ϸ����
	int m_nLastUseTime;//�ϴ�ʹ��ʱ��
	bool m_bIsXtBegin;//�Ƿ�ʼ����
	int m_nXtBeginTime;//������ʼʱ��
	int m_nXTUseTIme;//�����ڶ���ʱ����û��ʹ�ã��ͷ�������

	char m_errMsg[256];//������Ϣ

	char m_sDesEncryptKey[33];//����des��3des�ӽ��ܵ���Կ
	char m_sPinKey[33];//����ӽ��ܵ���Կ
	char m_sMacKey[33];//mac���ܵ���Կ

	int m_nEncryptZipFlag;//����ѹ����־ 0�������벻ѹ�� 1���� 2ѹ��3DES����4DES���ܲ�ѹ��5zlibѹ�� 9�Զ���ӽ���

	int m_nPwdEncryptFlag;//������ܱ�־ 0������1 des���� 9�Զ���ӽ���

	CBF_SimpleLog m_log;
	CBF_Date_Time m_publog;
	CBF_Tools     m_pTool;
};

#endif // !defined(AFX_FRONTAPI_H__8E987EC6_8C8C_45BC_A26F_40C54476EA64__INCLUDED_)
