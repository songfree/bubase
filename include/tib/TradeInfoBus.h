// TradeInfoBus.h: interface for the TradeInfoBus class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRADEINFOBUS_H__33ABF484_F174_4DFD_9D27_8B245E822BAA__INCLUDED_)
#define AFX_TRADEINFOBUS_H__33ABF484_F174_4DFD_9D27_8B245E822BAA__INCLUDED_

#include "public.h"
#include "BF_Xml.h"
#include "BF_SocketUdp.h"
#include "IErrLog.h"

#include "BF_Thread.h"
#include "TibHead.h"
#include "PortTbl.h"
#include "TibRcvMgr.h"
#include "TibSendMgr.h"


#if _MSC_VER > 100
#pragma once
#endif // _MSC_VER > 1000

#ifdef TIB_EXPORTS
#define TIB_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define TIB_EXPORT __declspec(dllimport)
#else
#define TIB_EXPORT 
#endif
#endif


class TIB_EXPORT CTradeInfoBus  
{
public:
	bool IsMsgtype(int msgtype);

	// ������: MonitorRecv
	// ���  : ������ 2015-7-1 15:14:47
	// ����  : void 
	// ����  : ��ؽ������߽����̣߳���ֹͣ������
	void MonitorRecv();

	// ������: StopRecv
	// ���  : ������ 2015-7-1 15:14:35
	// ����  : void 
	// ����  : ֹͣ�������߽����߳�
	void StopRecv();

	// ������: StartRecv
	// ���  : ������ 2015-7-1 15:14:21
	// ����  : bool 
	// ����  : �����������߽����߳�
	bool StartRecv();

	// ������: GetQueueSize
	// ���  : ������ 2015-7-1 14:11:56
	// ����  : int 
	// ����  : ȡ�ö��д��������ݴ�С
	int GetQueueSize();

	// ������: GetData
	// ���  : ������ 2015-7-1 14:10:27
	// ����  : int 
	// ����  : TIBCOMMSTRU &data
	// ����  : ��ȡ���ݽ�
	int GetData(TIBCOMMSTRU &data);

	// ������: SendData
	// ���  : ������ 2015-6-18 10:49:52
	// ����  : int 
	// ����  : TIBCOMMSTRU data
	// ����  : ������������������
	int SendData(TIBCOMMSTRU data);

	// ������: GetPara
	// ���  : ������ 2015-6-18 10:45:35
	// ����  : bool 
	// ����  : int &apptype
	// ����  : int &apppubid
	// ����  : int &appprivateid
	// ����  : ȡӦ�����ͣ�Ӧ��ID����
	bool GetPara(int &apptype, int &apppubid,int &appprivateid);

	// ������: Init
	// ���  : ������ 2015-6-15 11:35:13
	// ����  : bool 
	// ����  : CIErrlog *errlog
	// ����  : const char *tibxml
	// ����  : ��ʼ������ȡxml�����ļ��еĽ��׶˿����á�Ӧ������
	bool Init(CIErrlog *errlog,const char *tibxml);

		
	CTradeInfoBus(unsigned short apptype);
	CTradeInfoBus();
	virtual ~CTradeInfoBus();
	

protected:
	
	bool      m_bIsInit;

	CIErrlog *m_pLog;          //��־��

	CPortTbl    m_pListPort;  //�˿��б�

	unsigned short  m_nAppType;//Ӧ�ó�������
	unsigned int    m_nAppPublicId;  //Ӧ�ù���ID
	unsigned int    m_nAppPrivateId; //Ӧ��˽��ID

	CTibRcvMgr      m_pRcvMgr;  //���չ���
	CTibSendMgr     m_pSendMgr; //���͹���


};

#endif // !defined(AFX_TRADEINFOBUS_H__33ABF484_F174_4DFD_9D27_8B245E822BAA__INCLUDED_)
