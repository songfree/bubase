// MsgProcThread.h: interface for the CMsgProcThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MSGPROCTHREAD_H__56614FBC_0A67_43E7_A8B2_51D6CBB36441__INCLUDED_)
#define AFX_MSGPROCTHREAD_H__56614FBC_0A67_43E7_A8B2_51D6CBB36441__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//��Ϣ�����߳�
#include "public.h"
#include "IErrLog.h"
#include "DrebPubDefine.h"
#include "BF_SocketTcp.h"
#include "DrebEndian.h"
#include "MemDB.h"
#include "GlobalVar.h"
#include "SocketMgr.h"
#include "BF_BufferPool.h"
#include "BF_Thread.h"
#include "PoolData.h"
#include "BF_Xml.h"




class CMsgProcThread : public CBF_Thread  
{
public:
	void PrintServiceRoute();
	void PrintRoute();
	
	

	// ������: OnCmdUnReqTx
	// ���  : ������ 2016-1-13 8:42:19
	// ����  : void 
	// ����  : S_DREB_RSMSG *msg
	// ����  : ȡ������Ľ���ע��
	void OnCmdUnReqTx(S_DREB_RSMSG *msg);

	void OnMsgMonitor(S_DREB_RSMSG *msg);

	void LogDrebHead(int loglevel, DREB_HEAD head, const char *msg, const char *filename, int fileline);

	// ������: Monitor_ReportStop
	// ���  : ������ 2013-3-6 16:44:17
	// ����  : void 
	// ����  : S_DREB_RSMSG *msg
	// ����  : ֹͣ��ر���
	void Monitor_ReportStop(S_DREB_RSMSG *msg);

	// ������: Monitor_ReportStart
	// ���  : ������ 2013-3-6 16:44:12
	// ����  : void 
	// ����  : S_DREB_RSMSG *msg
	// ����  : ������ر���
	void Monitor_ReportStart(S_DREB_RSMSG *msg);

	// ������: Monitor_ConnInfo
	// ���  : ������ 2013-3-6 16:44:08
	// ����  : void 
	// ����  : S_DREB_RSMSG *msg
	// ����  : ����dreb������Ϣ
	void Monitor_ConnInfo(S_DREB_RSMSG *msg);

	// ������: Monitor_Report
	// ���  : ������ 2013-3-6 16:44:04
	// ����  : void 
	// ����  : S_DREB_RSMSG *msg
	// ����  : ����DREB����ת����Ϣ
	void Monitor_Report(S_DREB_RSMSG *msg);

	// ������: Monitor_RouteService
	// ���  : ������ 2013-3-6 16:44:00
	// ����  : void 
	// ����  : S_DREB_RSMSG *msg
	// ����  : ����·���б�
	void Monitor_RouteService(S_DREB_RSMSG *msg);

	// ������: Monitor_RouteDreb
	// ���  : ������ 2013-3-6 16:43:56
	// ����  : void 
	// ����  : S_DREB_RSMSG *msg
	// ����  : �ڵ�·�ɱ�
	void Monitor_RouteDreb(S_DREB_RSMSG *msg);

	// ������: Monitor_CurTxList
	// ���  : ������ 2013-3-6 16:43:48
	// ����  : void 
	// ����  : S_DREB_RSMSG *msg
	// ����  : ��ǰ�ڵ㽻���б�
	void Monitor_CurTxList(S_DREB_RSMSG *msg);

	// ������: Monitor_Host
	// ���  : ������ 2013-3-6 16:43:45
	// ����  : void 
	// ����  : S_DREB_RSMSG *msg
	// ����  : ������Դ��Ϣ
	void Monitor_Host(S_DREB_RSMSG *msg);

	// ������: Monitor_info
	// ���  : ������ 2013-3-6 16:43:41
	// ����  : void 
	// ����  : S_DREB_RSMSG *msg
	// ����  : DREB��Ϣ
	void Monitor_info(S_DREB_RSMSG *msg);

	

	// ������: OnCmdMonitorBpc
	// ���  : ������ 2013-3-4 15:20:41
	// ����  : void 
	// ����  : S_DREB_RSMSG *msg
	// ����  : bpc��ؽ���
	void OnCmdMonitorBpc(S_DREB_RSMSG *msg);

	// ������: OnCmdMonitor
	// ���  : ������ 2013-3-4 15:20:14
	// ����  : void 
	// ����  : S_DREB_RSMSG *msg
	// ����  : dreb��ؽ���
	void OnCmdMonitorDreb(S_DREB_RSMSG *msg);
	

	// ������: SetGlobalVar
	// ���  : ������ 2010-7-22 14:36:50
	// ����  : bool 
	// ����  : CGlobalVar *pVar
	// ����  : CSocketMgr *sockmgr
	// ����  : CMemDB *pMemDb
	// ����  : CPoolBuffer *pool
	// ����  : ����ȫ�ֲ���
	bool SetGlobalVar(CGlobalVar *pVar,CSocketMgr *sockmgr,CMemDB *pMemDb,CBF_BufferPool *pool,CPoolData *msgqueue);

	/// ������: Run
	/// ���  : ������ 2009-8-3 10:44:00
	/// ����  : int 
	/// ����  : S_DREB_RSMSG *msg
	/// ����  : ��Ϣ������
	int Run(S_DREB_RSMSG *msg);


	CMsgProcThread();
	virtual ~CMsgProcThread();

	char m_errMsg[256];

	int  m_nIndex;

	std::string GetDrebCmdType(int cmdtype);
	std::string GetDrebSockType(int socktype);
	std::string GetIndexFlag(int index);

protected:

	// ������: TransBroadCast
	// ���  : ������ 2010-9-7 14:25:23
	// ����  : void 
	// ����  : S_DREB_RSMSG *msg
	// ����  : bool isaffirm=true
	// ����  : �㲥��Ϣ
	void TransBroadCast(S_DREB_RSMSG *msg,bool isaffirm=true);


	// ������: TransMsgAns
	// ���  : ������ 2010-9-7 14:25:19
	// ����  : void 
	// ����  : S_DREB_RSMSG *msg
	// ����  : ת��Ӧ��,ͨѶת��Ӧ������
	void TransMsgAns(S_DREB_RSMSG *msg);

	// ������: TransMsgRequest
	// ���  : ������ 2010-9-7 14:26:20
	// ����  : void 
	// ����  : S_DREB_RSMSG *msg
	// ����  : bool isaffirm
	// ����  : ת������,ͨѶת����������
	void TransMsgRequest(S_DREB_RSMSG *msg,bool isaffirm);


	// ������: RouteMsgRequest
	// ���  : ������ 2016-1-13 8:43:09
	// ����  : int 0 SUCCESS ��0������ 
	// ����  : S_DREB_RSMSG *msg
	// ����  : bool isaffirm �Ƿ�ȷ��Ӧ��
	// ����  : ������Ϣ��·��ת��
	void RouteMsgRequest(S_DREB_RSMSG *msg,bool isaffirm);


	// ������: AnsMsg
	// ���  : ������ 2010-9-7 14:27:03
	// ����  : bool 
	// ����  : S_DREB_RSMSG *msg
	// ����  : unsigned short errcode  �������
	// ����  : char *errmsg   ������Ϣ
	// ����  : Ӧ�����ݸ�����
	bool AnsMsg(S_DREB_RSMSG *msg,unsigned short errcode,const char *errmsg);

	// ������: OnCmdServiceReg
	// ���  : ������ 2010-9-7 14:24:19
	// ����  : void 
	// ����  : S_DREB_RSMSG *msg
	// ����  : ���صķ���ע��
	void OnCmdServiceReg(S_DREB_RSMSG *msg);



	// ������: OnCmdUnServiceReg
	// ���  : ������ 2014-12-12 9:56:02
	// ����  : void 
	// ����  : S_DREB_RSMSG *msg
	// ����  : ȡ������ע��
	void OnCmdUnServiceReg(S_DREB_RSMSG *msg);

	// ������: OnCmdService
	// ���  : ������ 2010-9-7 14:27:30
	// ����  : void 
	// ����  : S_DREB_RSMSG *msg
	// ����  : ����dreb�ϵķ���·��
	void OnCmdService(S_DREB_RSMSG *msg);

	// ������: OnCmdDpPost
	// ���  : ������ 2010-9-7 14:28:09
	// ����  : void 
	// ����  : S_DREB_RSMSG *msg
	// ����  : ����DPPOST�ɿ�������Ϣ
	void OnCmdDpPost(S_DREB_RSMSG *msg);

	// ������: OnCmdDpPush
	// ���  : ������ 2010-9-7 14:28:28
	// ����  : void 
	// ����  : S_DREB_RSMSG *msg
	// ����  : ����DPPUSH���ɿ�������Ϣ
	void OnCmdDpPush(S_DREB_RSMSG *msg);


	// ������: OnCmdDpABc
	// ���  : ������ 2010-9-7 14:28:49
	// ����  : void 
	// ����  : S_DREB_RSMSG *msg
	// ����  : ����DPABC�޻�Ӧ�㲥
	void OnCmdDpABc(S_DREB_RSMSG *msg);

	// ������: OnCmdDpBc
	// ���  : ������ 2010-9-7 14:29:39
	// ����  : void 
	// ����  : S_DREB_RSMSG *msg
	// ����  : ����DPBC�л�Ӧ�㲥
	void OnCmdDpBc(S_DREB_RSMSG *msg);

	// ������: OnCmdDpACall
	// ���  : ������ 2010-9-7 14:29:53
	// ����  : void 
	// ����  : S_DREB_RSMSG *msg
	// ����  : ����DPACALL�޻�Ӧ������Ϣ
	void OnCmdDpACall(S_DREB_RSMSG *msg);

	// ������: OnCmdDpCall
	// ���  : ������ 2010-9-7 14:30:24
	// ����  : void 
	// ����  : S_DREB_RSMSG *msg
	// ����  : ����DPCALL�л�Ӧ������Ϣ
	void OnCmdDpCall(S_DREB_RSMSG *msg);

	// ������: AnsConnect
	// ���  : ������ 2010-7-22 15:35:38
	// ����  : bool 
	// ����  : S_DREB_RSMSG *msg
	// ����  : unsigned short errcode
	// ����  : char *errmsg
	// ����  : Ӧ������ӷ�
	bool AnsConnect(S_DREB_RSMSG *msg, unsigned short errcode,const char *errmsg);
	
	
	/// ������: OnCmdDelNode
	/// ���  : ������ 2009-8-3 10:45:39
	/// ����  : void 
	/// ����  : S_DREB_RSMSG *msg
	/// ����  : ע���������߽ڵ�ڵ�
	void OnCmdDelNode(S_DREB_RSMSG *msg);
	
	/// ������: OnCmdDelSvr
	/// ���  : ������ 2009-8-3 10:44:58
	/// ����  : void 
	/// ����  : S_DREB_RSMSG *msg
	/// ����  : ע������
	void OnCmdDelSvr(S_DREB_RSMSG *msg);
	
	/// ������: OnCmdRoute
	/// ���  : ������ 2009-8-3 10:44:40
	/// ����  : void 
	/// ����  : S_DREB_RSMSG *msg
	/// ����  : ·��֪ͨ
	void OnCmdRoute(S_DREB_RSMSG *msg);

	/// ������: AcceptDreb
	/// ���  : ������ 2009-8-3 10:44:29
	/// ����  : void 
	/// ����  : S_DREB_RSMSG *msg
	/// ����  : �����������߽ڵ�ļ���
	void AcceptDreb(S_DREB_RSMSG *msg);
	
	/// ������: AcceptSvr
	/// ���  : ������ 2009-8-3 10:44:20
	/// ����  : void 
	/// ����  : S_DREB_RSMSG *msg
	/// ����  : ���ܷ��������
	void AcceptSvr(S_DREB_RSMSG *msg);
	
	/// ������: AcceptClient
	/// ���  : ������ 2009-8-3 10:44:09
	/// ����  : void 
	/// ����  : S_DREB_RSMSG *msg
	/// ����  : ���ܿͻ��˵�����
	void AcceptClient(S_DREB_RSMSG *msg);


	// ������: OnCmdConnect
	// ���  : ������ 2009-11-26 10:07:51
	// ����  : void 
	// ����  : S_DREB_RSMSG *msg
	// ����  : ���նԶ˵�����ע�ᣬ�����������ͷֱ����AcceptSvr��AcceptClient��AcceptDreb
	void OnCmdConnect(S_DREB_RSMSG *msg);


	// ������: SendService
	// ���  : ������ 2010-7-29 10:07:32
	// ����  : void 
	// ����  : int index dreb�������
	// ����  : vector<S_SERVICE_ROUTE> rtlist
	// ����  : ���ͷ���·��
	void SendService(int index, std::vector<S_SERVICE_ROUTE *> rtlist);

	// ������: OnMsgService
	// ���  : ������ 2010-7-26 15:40:13
	// ����  : void 
	// ����  : S_DREB_RSMSG *msg
	// ����  : ���ͷ����б�֪ͨ
	void OnMsgServiceRoute(S_DREB_RSMSG *msg);

	// ������: OnMsgRoute
	// ���  : ������ 2009-10-30 9:16:23
	// ����  : void 
	// ����  : S_DREB_RSMSG *msg
	// ����  : ������·����Ϣ
	void OnMsgRoute(S_DREB_RSMSG *msg);
	
	/// ������: OnMsgClose
	/// ���  : ������ 2009-5-25 14:47:12
	/// ����  : void 
	/// ����  : S_DREB_RSMSG *msg
	/// ����  : ����ر�������Ϣ
	void OnMsgClose(S_DREB_RSMSG *msg);
	
	
	/// ������: OnMsgRead
	/// ���  : ������ 2009-5-25 14:47:27
	/// ����  : void 
	/// ����  : S_DREB_RSMSG *msg
	/// ����  : ������յ������ݣ��������������ͽ��д���
	void OnMsgRead(S_DREB_RSMSG *msg);
	
    /// ������: OnMsgRead
    /// ���  : ������ 2022-6-14 11:30:10
    /// ����  : void 
    /// ����  : S_DREB_RSMSG *msg
    /// ����  : ��������Ĺ㲥
	void OnCmdSubscribe(S_DREB_RSMSG* msg);

    /// ������: OnMsgRead
    /// ���  : ������ 2022-6-14 11:30:10
    /// ����  : void 
    /// ����  : S_DREB_RSMSG *msg
    /// ����  : �������ȡ�����Ĺ㲥
	void OnCmdUnSubscribe(S_DREB_RSMSG* msg);

	virtual int Run();
	virtual bool InitThreadInstance();
	virtual void ExitThreadInstance();

protected:

	// ������: RouteMsgRequestByFunc
	// ���  : ������ 2016-1-13 8:48:37
	// ����  : int 
	// ����  : S_DREB_RSMSG *msg
	// ����  : bool isaffirm  �Ƿ���ȷ��Ӧ��
	// ����  : ͨ������·��
	void RouteMsgRequestByFunc(S_DREB_RSMSG *msg,bool isaffirm);

	// ������: RouteMsgRequestBySvr
	// ���  : ������ 2016-1-13 8:48:34
	// ����  : int 
	// ����  : S_DREB_RSMSG *msg
	// ����  : bool isaffirm �Ƿ���ȷ��Ӧ��
	// ����  : ͨ������·��
	void RouteMsgRequestBySvr(S_DREB_RSMSG *msg,bool isaffirm);

	// ������: RouteMsgRequestByDreb
	// ���  : ������ 2016-1-13 8:48:30
	// ����  : int 
	// ����  : S_DREB_RSMSG *msg
	// ����  : bool isaffirm �Ƿ���ȷ��Ӧ��
	// ����  : ͨ�����߽ڵ�·��
	void RouteMsgRequestByDreb(S_DREB_RSMSG *msg,bool isaffirm);

	
	
	
//	DREB_HEAD m_pRsMsgHead;
		
	CDrebEndian  m_pDrebEndian;

	CIErrlog      *m_log;

	//CBF_SimpleLog m_pLogRoute;

	CBF_Date_Time m_datetime;

	CBF_SocketTcp  m_tcpSocket;
	CGlobalVar     *m_pRes;
	CMemDB         *m_pMemDb;
	CSocketMgr     *m_pSocketMgr;
	CBF_BufferPool *m_pMemPool;
	CPoolData      *m_pMsgQueue;

	

	char  m_headMsg[8192];
};

#endif // !defined(AFX_MSGPROCTHREAD_H__56614FBC_0A67_43E7_A8B2_51D6CBB36441__INCLUDED_)
