// MemDB.h: interface for the CMemDB class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MEMDB_H__8CF47224_C394_4279_858C_0ED49F03E2A5__INCLUDED_)
#define AFX_MEMDB_H__8CF47224_C394_4279_858C_0ED49F03E2A5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "BF_Tools.h"
#include "SvrTbl.h"
#include "DrebTbl.h"
#include "RouteTbl.h"
#include "ServiceTbl.h"
#include "BF_Mutex.h"
#include "IErrLog.h"
#include "BF_Date_Time.h"


typedef struct 
{
	unsigned short nNodeId;        //Ŀ���������߽ڵ���
	char           cNodePrivateId; //�������߽ڵ�˽�����
	int            nStep;          //0����1ֱ�����������߽ڵ�>1Ϊ�������߽ڵ��Ĳ���
	int            nIndex;         //��������  ͨ�������������ɵõ��������߽ڵ������
	CSendQueue     *pSendQueue;    //���Ͷ���
	int            nbandwidth;     //����
	bool           bIsClose;       //�����Ƿ�ر�
	int            nCloseTime;     //���ӹر�ʱ�䣬����һ��ʱ����Զ�ע��
	unsigned int   nSvrMainId;     //ע�����������߽ڵ��ϵ��������ܺ�   ֻ�з������ʱ������д
	char           cSvrPrivateId;  //ע�����������߽ڵ��ϵ�˽����� 0��ʾ���� >0Ϊÿ���������ţ�s_nSvrMainId+s_cSvrPrivateId ��Ϊ˽�й��ܺ�   ֻ�з������ʱ������д
}S_SERVICE_QUERY_RESULT;

class CMemDB  
{
public:

	// ������: SelectLocalBySvr
	// ���  : ������ 2016-1-13 13:34:07
	// ����  : bool 
	// ����  : unsigned int svr
	// ����  : int privatesvr
	// ����  : S_SERVICE_ROUTE &srt
	// ����  : ���ݷ����ڱ��ز�ѯ��Ϣ��������
	bool SelectLocalBySvr(unsigned int svr,int privatesvr, S_SERVICE_ROUTE &srt);

	// ������: SelectLocalBySvr
	// ���  : ������ 2016-1-13 13:34:03
	// ����  : bool 
	// ����  : unsigned int svr
	// ����  : S_SERVICE_ROUTE &srt
	// ����  : ���ݷ����ڱ��ز�ѯ��Ϣ��������
	bool SelectLocalBySvr(unsigned int svr,S_SERVICE_ROUTE &srt);

	// ������: SelectLocalByFunc
	// ���  : ������ 2016-1-13 12:57:35
	// ����  : bool 
	// ����  : unsigned int func
	// ����  : S_SERVICE_ROUTE &func
	// ����  : ���ݽ������ڱ��ز�ѯ������Ϣ�������͸���Ӧ�ķ���
	bool SelectLocalByFunc(unsigned int func,S_SERVICE_ROUTE &fc);

	// ������: GetAllRouteBc
	// ���  : ������ 2011-1-11 21:28:20
	// ����  : bool 
	// ����  : vector<S_ROUTE_TBL *> &rt
	// ����  : ȡҪ���͹㲥�����߽ڵ���Ϣ(���������ڵ�) �Ѿ�ȥ�ء�
	bool GetAllRouteBc(std::vector<S_DREB_ROUTE *> &rt);


	// ������: SetLocalDrebInfo
	// ���  : ������ 2011-1-11 21:29:03
	// ����  : bool 
	// ����  : S_DREB_ROUTE rt
	// ����  : //���ñ��ؽڵ���Ϣ (����Ϊ0��indexΪ0)
	bool SetLocalDrebInfo(S_DREB_ROUTE rt);

	// ������: DeleteRouteByIndex
	// ���  : ������ 2010-9-22 9:12:06
	// ����  : bool 
	// ����  : int index
	// ����  : �����������ɾ������·�ɣ������߶Ͽ�ʱ��Ҫ�������߹���������·��ɾ��
	bool DeleteRouteByIndex(int index);

	// ������: SetPara
	// ���  : ������ 2010-9-22 9:11:53
	// ����  : void 
	// ����  : int svrtime
	// ����  : int servicetime
	// ����  : ���ó�ʱ����
	void SetPara(int svrtime,int servicetime);

	// ������: DeleteServiceByIndex
	// ���  : ������ 2010-9-14 15:08:49
	// ����  : bool 
	// ����  : int index
	// ����  : �����������ɾ��������������·�ɹ����Ľ���·��
	bool DeleteServiceByIndex(int index);

	// ������: UpdateRouteServiceByIndex
	// ���  : ������ 2010-9-14 15:10:01
	// ����  : bool 
	// ����  : int index
	// ����  : ������������ϵĽ���·��
	bool UpdateRouteServiceByIndex(int index);

	// ������: GetRouteServiceByIndex
	// ���  : ������ 2010-9-7 14:49:20
	// ����  : bool 
	// ����  : int drebid
	// ����  : int privateid
	// ����  : int index
	// ����  : vector<S_SERVICE_ROUTE *> &rtlist
	// ����  : ����dreb�������ȡҪ�ڴ������Ϸ��͵ķ���·����Ϣ
	bool GetRouteServiceByIndex(int drebid,int privateid,int index, std::vector<S_SERVICE_ROUTE *> &rtlist );

	// ������: UnRegisterService
	// ���  : ������ 2010-7-30 10:26:05
	// ����  : bool 
	// ����  : int nodeid
	// ����  : int privateid
	// ����  : ����Ͽ�ȡ��ע��
	bool UnRegisterService(int nodeid,int privateid,int svr,int svrprivate);


	// ������: SelectRouteByFunc
	// ���  : ������ 2010-7-27 9:03:40
	// ����  : bool 
	// ����  : unsigned int func
	// ����  : S_DREB_ROUTE &rt
	// ����  : ���ݹ��ܺŲ���·��
	bool SelectRouteByFunc(unsigned int func,S_DREB_ROUTE &rt);

	// ������: SelectRouteBySvr
	// ���  : ������ 2010-7-27 15:23:10
	// ����  : bool 
	// ����  : int svr
	// ����  : int privatesvr
	// ����  : S_DREB_ROUTE &rt
	// ����  : ���ݷ������·��
	bool SelectRouteBySvr(int svr,int privatesvr,S_DREB_ROUTE &rt);

	// ������: SelectRouteBySvr
	// ���  : ������ 2010-7-27 15:22:41
	// ����  : bool 
	// ����  : int svr
	// ����  : S_DREB_ROUTE &rt
	// ����  : ���ݷ������·��
	bool SelectRouteBySvr(int svr,S_DREB_ROUTE &rt);

	// ������: SelectRouteByFuncSvr
	// ���  : ������ 2010-7-27 15:22:41
	// ����  : bool 
	// ����  : int func ������
	// ����  : int svr
	// ����  : S_DREB_ROUTE &rt
	// ����  : ���ݽ�����������·��
	bool SelectRouteByFuncSvr(int func,int svr, S_DREB_ROUTE &rt);

	// ������: SelectRouteByFuncSvr
	// ���  : ������ 2010-7-27 15:22:41
	// ����  : bool 
	// ����  : int drebid ����id
	// ����  : int func ������
	// ����  : int svr
	// ����  : S_SERVICE_ROUTE &rt
	// ����  : ���ݽ�����������·��
	bool SelectRouteByDrebFuncSvr(int drebid,int func,int svr, S_SERVICE_ROUTE &rt);

	// ������: GetAllService
	// ���  : ������ 2010-7-27 11:44:18
	// ����  : bool 
	// ����  : vector<S_SERVICE_ROUTE *> &rtlist
	// ����  : ��ȡ���еĹ��ܣ���index dreb��������
	bool GetAllServiceOrder(std::vector<S_SERVICE_ROUTE *> &rtlist);

	
	// ������: RegisterFunc
	// ���  : ������ 2010-7-27 15:14:33
	// ����  : bool 
	// ����  : S_SERVICE_ROUTE rt
	// ����  : ע�ύ�ף���ʱ���ø���dreb����ɾ������·��
	bool AddServiceRoute(S_SERVICE_ROUTE rt);


	// ������: GetAllDreb
	// ���  : ������ 2010-2-25 15:28:23
	// ����  : bool 
	// ����  : vector<S_DREB_ROUTE> &dp
	// ����  : bool inclocal=false   false��������Dreb  true����
	// ����  : ��ȡ�������ӵ�ͨѶƽ̨������������DREB������
	bool GetAllDreb(std::vector<S_DREB_ROUTE *> &dp,bool inclocal=false);

	// ������: GetRouteList
	// ���  : ������ 2010-2-25 15:27:57
	// ����  : bool 
	// ����  : vector<S_ROUTE_TBL> &rt
	// ����  : ��ȡ·�ɹ�����������Ϣ�����������ؼ�ֱ���ӵ�����
	bool GetRouteList(std::vector<S_DREB_ROUTE *> &rt);

	// ������: GetAllRoute
	// ���  : ������ 2010-2-25 15:27:57
	// ����  : bool 
	// ����  : vector<S_ROUTE_TBL> &rt
	// ����  : ��ȡ���е�·�ɣ��������ӵ�ͨѶƽ̨
	bool GetAllRoute(std::vector<S_DREB_ROUTE *> &rt);
	

	// ������: GetAllSvr
	// ���  : ������ 2009-12-4 16:53:26
	// ����  : bool 
	// ����  : vector<S_SVR_ROUTE> &svrlist
	// ����  : ��ȡע������з���
	bool GetAllSvr(std::vector<S_SVR_ROUTE> &svrlist);

	// ������: AddRoute
	// ���  : ������ 2009-10-30 10:46:20
	// ����  : bool 
	// ����  : int index
	// ����  : vector<S_ROUTE_TBL>rtlist
	// ����  : ����index���ӹ�����·�ɣ���ɾ��������
	bool AddRoute(int index, std::vector<S_DREB_ROUTE>rtlist);

	// ������: GetRouteByIndex
	// ���  : ������ 2009-10-29 16:56:14
	// ����  : bool 
	// ����  : int index
	// ����  : vector<S_ROUTE_TBL> &rt
	// ����  : ��ȡͨѶƽ̨����index��Ҫ���͵�·��(����·�ɱ��г�index���·�ɶ���Ҫ���͵�)
	bool GetRouteByIndex(int index, std::vector<S_DREB_ROUTE *> &rt);
	

	// ������: SelectRoute
	// ���  : ������ 2009-10-29 10:28:56
	// ����  : bool 
	// ����  : int nodeid
	// ����  : int privateid
	// ����  : S_DREB_ROUTE *dreb
	// ����  : ����ͨѶƽ̨˽��ID����·��
	bool SelectRoute(int nodeid,int privateid,S_DREB_ROUTE &dreb);

	// ������: SelectRoute
	// ���  : ������ 2009-10-29 10:28:37
	// ����  : bool 
	// ����  : int nodeid
	// ����  : S_DREB_ROUTE &dreb
	// ����  : ����ͨѶƽ̨ID����·��
	bool SelectRoute(int nodeid,S_DREB_ROUTE &dreb);

	

	// ������: UnRegisterDreb
	// ���  : ������ 2009-10-29 9:44:09
	// ����  : bool 
	// ����  : int nodeid
	// ����  : int privateid
	// ����  : int index
	// ����  : ע��ͨѶƽ̨���� ��ɾ��·�ɼ�ͨѶƽ̨���ӱ�
	bool UnRegisterDreb(int nodeid, int privateid, int index);

	// ������: CloseDreb
	// ���  : ������ 2009-10-29 9:43:55
	// ����  : bool 
	// ����  : int nodeid
	// ����  : int privateid
	// ����  : int index
	// ����  : �ر�ͨѶƽ̨����,ɾ��·�ɲ���ͨѶƽ̨���ӱ��ͨѶƽ̨״̬Ϊ���
	bool CloseDreb(int nodeid,int privateid,int index);


	// ������: RegisterDreb
	// ���  : ������ 2009-10-29 9:42:41
	// ����  : bool 
	// ����  : int nodeid
	// ����  : int privateid
	// ����  : int index
	// ����  : int bdrate
	// ����  : CSendQueue **psque  ���صķ��Ͷ���
	// ����  : ע�����ӵ�ͨѶƽ̨
	bool RegisterDreb(int nodeid,int privateid, int index,int bdrate,CSendQueue **psque);

	/// ������: SelectSvr
	/// ���  : ������ 2009-10-27 15:20:15
	/// ����  : bool 
	/// ����  : int svrid
	/// ����  : int privateid
	/// ����  : S_SVR_ROUTE &svr
	/// ����  : ���ݷ���ID,˽��ID���ұ��ڵ�ע��ķ���
	bool SelectSvr(int svrid,int privateid,S_SVR_ROUTE &svr);

	/// ������: SelectASvr
	/// ���  : ������ 2009-10-27 15:01:11
	/// ����  : bool 
	/// ����  : int svrid
	/// ����  : S_SVR_ROUTE &svr
	/// ����  : ���ݷ�����ڱ��ڵ�ע�����ҳ�һ������(���㷨����)
	bool SelectASvr(int svrid,S_SVR_ROUTE &svr);

	/// ������: UnRegisterSvr
	/// ���  : ������ 2009-10-27 14:57:43
	/// ����  : bool 
	/// ����  : int svrid
	/// ����  : int privateid
	/// ����  : ȡ�������ע��
	bool UnRegisterSvr(int svrid,int privateid);

	/// ������: CloseSvr
	/// ���  : ������ 2009-10-27 14:56:46
	/// ����  : bool 
	/// ����  : int svrid
	/// ����  : int privateid
	/// ����  : �رշ���
	bool CloseSvr(int svrid,int privateid);

	/// ������: RegisterSvr
	/// ���  : ������ 2009-10-27 14:41:21
	/// ����  : bool 
	/// ����  : int svrid      ����ID
	/// ����  : int privateid  ˽��ID
	/// ����  : int index  ��������
	/// ����  : CSendQueue **pque  �ɹ�ע�᷵�صķ��Ͷ���
	/// ����  : ע�����
	bool RegisterSvr(int svrid,int privateid,int index,CSendQueue **pque);


	CMemDB();
	virtual ~CMemDB();

	CDrebTbl  m_drebTbl;   //���ӵ�ͨѶƽ̨�б� �������ڵ㣬��index��stepΪ0
	CSvrTbl   m_svrTbl;    //ע��ķ����б�  ���ڵ���ע��ķ���

	CRouteTbl m_routeTbl;  //·�ɱ�  ����ֱ����dreb����ڴ�  ������·�ɱ�

	S_DREB_ROUTE m_pLocalDreb;//��ǰdreb��Ϣ

	CServiceTbl  m_serviceTbl;//�����б�  ���еĽ����б���������ע��Ľ���

	CIErrlog    *m_pLog;
protected:
	
	int   m_nDeleteTime;        //�������ʱ��
	int   m_nServiceDeleteTime; //�����ܹ���ʱ��
	
	// ������: AddRoute
	// ���  : ������ 2009-10-29 10:13:32
	// ����  : bool 
	// ����  : S_ROUTE_TBL rt
	// ����  : ����һ��·��
	bool AddRoute(S_DREB_ROUTE rt);

	// ������: GetARoute
	// ���  : ������ 2009-10-29 11:26:52
	// ����  : bool 
	// ����  : vector<S_ROUTE_TBL *> rt   �Ѱ������ʹ�����������
	// ����  : S_DREB_ROUTE &dreb
	// ����  : ��·���б���ȡ�����ʺ�������һ��·��
	bool GetARoute(std::vector<S_DREB_ROUTE *> rt,S_DREB_ROUTE &dreb);

//	bool GetARoute(vector<S_SERVICE_QUERY_RESULT> rt,S_DREB_ROUTE &dreb,S_SVR_ROUTE &svr);
	// ������: GetARoute
	// ���  : ������ 2009-10-29 11:26:52
	// ����  : bool 
	// ����  : vector<S_DREB_ROUTE> rt  ����������
	// ����  : S_DREB_ROUTE &dreb
	// ����  : ��·���б���ȡ�����ʺ�������һ��·�� 
//	bool GetARoute(vector<S_DREB_ROUTE *> rt,S_DREB_ROUTE &dreb);

	bool GetARoute(std::vector<S_SERVICE_ROUTE *> rt, S_DREB_ROUTE &dreb);
	bool GetARoute(std::vector<S_SERVICE_ROUTE *> rt, S_SERVICE_ROUTE &dreb);
};

#endif // !defined(AFX_MEMDB_H__8CF47224_C394_4279_858C_0ED49F03E2A5__INCLUDED_)
