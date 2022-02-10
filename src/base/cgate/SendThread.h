// SendThread.h: interface for the CSendThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SENDTHREAD_H__434A6929_6C84_4F36_943B_65F1828DC99D__INCLUDED_)
#define AFX_SENDTHREAD_H__434A6929_6C84_4F36_943B_65F1828DC99D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "BF_Thread.h"
#include "BF_Des.h"
#include "BF_Tools.h"
#include "CgateLog.h"
#include "BF_SocketTcp.h"
#include "GateResource.h"
#include "SendData.h"
#include "AIO_Work.h"
#include "DrebEndian.h"
#include "IErrLog.h"

class CSendThread : public CBF_Thread  
{
public:

	// ������: Encrypt
	// ���  : ������ 2014-6-14 2:28:23
	// ����  : bool 
	// ����  : S_CGATE_SMSG *msg
	// ����  : �����ݽ��м���
	bool Encrypt(S_CGATE_SMSG *msg);
	
	CSendThread();
	virtual ~CSendThread();

	bool SetGlobalVar(CGateResource *res,CSendData *senddata,CAIO_Work *aiowork);

	int  m_nIndex;

private:
	
	// ������: Run
	// ���  : ������ 2010-3-12 13:00:15
	// ����  : virtual int 
	// ����  : ���շ����߳�������
	virtual int Run();
	
	// ������: InitThreadInstance
	// ���  : ������ 2012-4-9 12:00:54
	// ����  : virtual bool 
	// ����  : �̳߳�ʼ�����߳�����ʱ����һ��
	virtual bool InitThreadInstance();
	
	// ������: ExitThreadInstance
	// ���  : ������ 2012-4-9 12:01:05
	// ����  : virtual void 
	// ����  : �˳��߳�Ҫ���Ĺ���,�߳��˳�ʱ����һ��
	virtual void ExitThreadInstance();

	bool ZipBuf(PSOCKET_POOL_DATA info,PCGATE_COMMSTRU  data,int nZipFlag);
	
protected:
	CIErrlog      *m_pLog;
	CBF_Date_Time m_datetime;
	CBF_Des        m_pDes; //����IO�߳̽��ղ�����ѹ�����ܵ�
	CBF_Des        m_pDesSend;
	
	CGateResource  *m_pRes;
	CSendData   *m_pSendData;//���״������ݶ���

	CAIO_Work   *m_pAioWork;

	CCgateLog       *m_pGateLog;
	CDrebEndian    m_pEndian;
	S_CGATE_SMSG  m_pSmsg;

	char          m_sSrcBuffer[CLIENTDATASIZE+2];//��Կͻ������ݵĽ��ܽ�ѹ����Դ
	char          m_sDestBuffer[CLIENTDATASIZE+2];//��Կͻ������ݵĽ��ܽ�ѹ����Ŀ��

	void SendBCMsg(S_CGATE_SMSG *msg);
	void SendSingleBcMsg(S_CGATE_SMSG *msg);
	void SendMsg(S_CGATE_SMSG *msg);

	int  m_nSendQuoteNum;
	int  m_nSendCustNum;

	// ������: GetDrebCmdType
	// ���  : ������ 2013-5-28 10:40:57
	// ����  : std::string 
	// ����  : int cmdtype
	// ����  : 
	std::string GetDrebCmdType(int cmdtype);
	
	// ������: GetMsgType
	// ���  : ������ 2013-5-28 10:40:52
	// ����  : std::string 
	// ����  : int type
	// ����  : ȡ������Ϣ����
	std::string GetMsgType(int type);

	// ������: Endian2CommCrc
	// ���  : ������ 2013-5-28 10:41:08
	// ����  : void 
	// ����  : CGATE_HEAD &head
	// ����  : �ֽ���ת��ͨѶ��ʽ������crcУ��λ
	void Endian2CommCrc(CGATE_HEAD &head);

};

#endif // !defined(AFX_SENDTHREAD_H__434A6929_6C84_4F36_943B_65F1828DC99D__INCLUDED_)
