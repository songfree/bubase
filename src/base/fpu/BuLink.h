// BuLink.h: interface for the CBuLink class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BULINK_H__0DD4D8AF_9AA3_48A5_ADAF_5F6B93E465F3__INCLUDED_)
#define AFX_BULINK_H__0DD4D8AF_9AA3_48A5_ADAF_5F6B93E465F3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "BfDefine.h"
#include "BuRes.h"
#include "CallBu.h"
#include "BF_Date_Time.h"
#include "BF_SimpleLog.h"
#include "BF_Md5.h"


#ifdef WIN32
#ifdef _DEBUG

#pragma comment(lib, "bf_bumoduled.lib") 
#pragma message("Automatically linking with  bf_bumoduled.lib")
#else
#pragma comment(lib, "bf_bumodule.lib") 
#pragma message("Automatically linking with  bf_bumodule.lib")
#endif
#endif

class CBuLink  : public CIPcLink
{
public:
	CBuLink();
	virtual ~CBuLink();

public:
	CBuRes             m_pRes;
	CCallBu            m_pBuProc;
	CBF_SocketTcp      m_tcpSocket;
	CBF_SimpleLog      *m_pLog;
	CBF_Date_Time       m_publog;
	SOCKET_HANDLE       m_sock;                   //socket id

	char g_sCurPath[300];
	char g_sModulePath[300];
	char g_sModuleName[300];


	bool m_bIsConnect;

public:
	bool IsSocketEvent(int sec);
	int OnRecv(S_BF_RSMSG_FPC *data);
	int OnSend(S_BF_RSMSG_FPC *data);
	void CallService();
	void ResetData();
	void OnEvent();
	void OnNoEvent();
	void  OnSend();
	void OnRecv();


	// ������: Reverse
	// ���  : ������ 2010-3-19 9:42:37
	// ����  : void 
	// ����  : unsigned char *buf
	// ����  : int len
	// ����  : �ֽ�����
	void Reverse(unsigned char *buf, int len);
	// ������: OnClose
	// ���  : ������ 2010-3-19 9:18:20
	// ����  : void 
	// ����  : const char *msg
	// ����  : �رպ�bffcc������
	void OnClose(const char *msg);

	// ������: Run
	// ���  : ������ 2010-3-19 9:17:10
	// ����  : int <0���Ӵ������ޣ��˳� =0��������
	// ����  : BU����������
	int Run();

	// ������: Stop
	// ���  : ������ 2010-3-19 9:16:54
	// ����  : void 
	// ����  : ֹͣBU
	void Stop();


	// ������: Connect
	// ���  : ������ 2010-3-18 17:37:25
	// ����  : int =0�ɹ� <0���ӳ��� >0���ӳ�������
	// ����  : ����bffcc
	int Connect();

public:
	/// ������: SetGPara1
	/// ���  : ������ 2008-12-30 9:42:48
	/// ����  : virtual bool true�ɹ�falseʧ��
	/// ����  : bool setflag=true
	/// ����  : �ӿ��麯��������Ҫʵ��.����ȫ�ֱ���1,���������򷵻�false
	virtual bool SetGPara1(bool setflag=true);
	
	
	/// ������: isSetGPara2
	/// ���  : ������ 2008-12-30 9:36:10
	/// ����  : virtual bool true�ɹ�falseʧ��
	/// ����  : bool setflag=true
	/// ����  : ����ȫ�ֱ���2,���������򷵻�false
	virtual bool SetGPara2(bool setflag=true);
	
	
	/// ������: isSetGPara3
	/// ���  : ������ 2008-12-30 9:36:13
	/// ����  : virtual bool true�ɹ�falseʧ��
	/// ����  : bool setflag=true
	/// ����  : ����ȫ�ֱ���3,���������򷵻�false
	virtual bool SetGPara3(bool setflag=true);
	
	/// ������: GetThreadShareSerial
	/// ���  : ������ 2008-12-30 9:36:13
	/// ����  : virtual long ��ˮ��
	/// ����  : ȡ��Ψһ����ˮ��
	virtual long GetThreadShareSerial();
	
	
	// ������: AnsData
	// ���  : ������ 2013-4-23 15:16:43
	// ����  : virtual int -1ʧ�� 0�ɹ�
	// ����  : S_BPC_RSMSG *data
	// ����  : Ӧ������,��BPUҵ�������ԭ�ӵ���
	virtual int AnsData(PBPCCOMMSTRU data);
	
	// ������: DPTrans
	// ���  : ������ 2013-4-23 15:11:48
	// ����  : virtual int -1ʧ�� 0�ɹ�
	// ����  : PBPCCOMMSTRU calldata
	// ����  : ת��������������
	virtual int DPTrans(PBPCCOMMSTRU calldata);
	
	// ������: BpuCall
	// ���  : ������ 2013-4-23 15:09:13
	// ����  : virtual int -1ʧ�� 0�ɹ�
	// ����  : PBPCCOMMSTRU calldata
	// ����  : LIST_BPC_RSMSG &ansdata
	// ����  : ����BPU�ڵĹ��ܣ������̿���ʹ��
	virtual int BpuCall(PBPCCOMMSTRU calldata,LIST_BPC_RSMSG &ansdata);
	
	// ������: BpcCall
	// ���  : ������ 2013-4-23 15:08:18
	// ����  : virtual int -1ʧ�� 0�ɹ�
	// ����  : PBPCCOMMSTRU calldata
	// ����  : LIST_BPC_RSMSG &ansdata
	// ����  : ����bpc����
	virtual int BpcCall(PBPCCOMMSTRU calldata,LIST_BPC_RSMSG &ansdata,unsigned int stimeout);
	
	// ������: ExtCall
	// ���  : ������ 2013-4-23 15:01:25
	// ����  : virtual int -1ʧ�� 0�ɹ�
	// ����  : PBPCCOMMSTRU calldata
	// ����  : vector<S_BPC_RSMSG> &ansdata
	// ����  : ���BPC
	virtual int ExtCall(PBPCCOMMSTRU calldata,LIST_BPC_RSMSG &ansdata,unsigned int stimeout);
	
	
	// ������:  InitLink
	// ���  : ������ 2013-4-23 14:44:14
	// ����  : virtual bool true�ɹ� falseʧ��
	// ����  : const char *confile
	// ����  : ��ʼ���������� 
	virtual bool  InitLink(const char *confile);

protected:
	int  m_nConnNum;//���Ӵ���

	std::vector <BPCCOMMSTRU> m_listAnsData;

	S_BF_RSMSG_FPC    m_pHeadData;

	char *m_sRcvBuffer;            //�������ݻ���   
	unsigned int  m_nRcvBufLen;                //���յ����ݳ���  �ɶ�ν���һ�����ݰ�
	

	char *sRcvBuffer;            //�������ݻ���   
	unsigned int  nRcvBufLen;                //���յ����ݳ���  �ɶ�ν���һ�����ݰ�

	char *m_sReqAnsData;            //�������ݻ���   
	int  m_nDataLen;                //���͵����ݳ���  �ɶ�ν���һ�����ݰ�


	char *m_sSendBuffer;            //�������ݻ���   
	unsigned int  m_nSendBufLen;                //���͵����ݳ���  �ɶ�ν���һ�����ݰ�


	int            m_nConntime;              //���ӽ���ʱ��
	int            m_nReadtime;              //�ϴζ�����ʱ��
	int            m_nSendtime;              //�ϴη�������ʱ�� 

	fd_set m_rset;
	fd_set m_wset;
#if defined(_WINDOWS)
	fd_set m_eset;
#endif
};

#endif // !defined(AFX_BULINK_H__0DD4D8AF_9AA3_48A5_ADAF_5F6B93E465F3__INCLUDED_)
