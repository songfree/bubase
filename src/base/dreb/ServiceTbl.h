// ServiceTbl.h: interface for the CServiceTbl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERVICETBL_H__5398A809_0D06_4C51_B7DB_2E189754723A__INCLUDED_)
#define AFX_SERVICETBL_H__5398A809_0D06_4C51_B7DB_2E189754723A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MdbBase.h"
#include "BF_Mutex.h"
#include "BF_LogClient.h"
//#include "IErrLog.h"

typedef struct 
{
	unsigned short nNodeId;        //DREB���
	char           cNodePrivateId; //DREB˽�����	
	unsigned int   nSvrMainId;     //ע����DREB���������ܺ�   ֻ�з������ʱ������д
	char           cSvrPrivateId;  //ע����DREB��˽����� 0��ʾ���� >0Ϊÿ���������ţ�s_nSvrMainId+s_cSvrPrivateId ��Ϊ˽�й��ܺ�   ֻ�з������ʱ������д
	unsigned int   nFuncNo;        //�����ܺ�
	unsigned int   nStep;          //0����1ֱ�����������߽ڵ�>1Ϊ�������߽ڵ��Ĳ���
	unsigned int   nIndex;         //��������  ͨ�������������ɵõ��������߽ڵ������
	unsigned int   nUpdateTime;    //���ܵĸ���ʱ��
	unsigned int   nbandwidth;     //����
}S_SERVICE_ROUTE;

class CServiceTbl  
{
public:

	

	// ������: UpdateRouteServiceByIndex
	// ���  : ������ 2013-10-28 14:37:42
	// ����  : bool 
	// ����  : int index dreb��������
	// ����  : ���½���·����Ϣ���������ϵĹ��ڽ���ɾ��
	bool UpdateRouteServiceByIndex(int index);

	// ������: GetRouteServiceByIndex
	// ���  : ������ 2010-9-7 14:47:39
	// ����  : bool 
	// ����  : int drebid
	// ����  : int privateid
	// ����  : int index
	// ����  : vector<S_SERVICE_ROUTE> &rtlist
	// ����  : ����dreb�������ȡҪ�ڴ������Ϸ��͵ķ���·����Ϣ
	bool GetRouteServiceByIndex(int drebid,int privateid,int index,std::vector<S_SERVICE_ROUTE> &rtlist);
	bool GetRouteServiceByIndex(int drebid,int privateid,int index,std::vector<S_SERVICE_ROUTE *> &rtlist);

	// ������: UnRegisterSvr
	// ���  : ������ 2010-9-7 10:26:06
	// ����  : bool 
	// ����  : int nodeid
	// ����  : int nodeprivated
	// ����  : int svrid
	// ����  : int privateid
	// ����  : ��dreb�ϵķ���Ͽ�ʱȡ���������ϵĹ���
	bool UnRegisterSvr(int nodeid,int nodeprivated,int svrid,int privateid);

	// ������: DeleteByIndex
	// ���  : ������ 2010-9-7 10:24:39
	// ����  : bool 
	// ����  : int drebindex
	// ����  : ����·��ʱ�ȸ���·�ɹ�����drebindexɾ����drebindex�ϵķ�������Ϣ
	bool DeleteByIndex(int drebindex);


	// ������: GetAllServiceOrderSvr
	// ���  : ������ 2010-7-27 11:28:05
	// ����  : bool 
	// ����  : vector<S_SERVICE_ROUTE *> &rtlist
	// ����  : ��ȡ���еĹ��ܣ������� dreb��������

	bool GetAllServiceOrderDrebSvr(std::vector<S_SERVICE_ROUTE *> &rtlist);

	// ������: SelectBySvr
	// ���  : ������ 2010-7-27 11:27:00
	// ����  : bool 
	// ����  : int svr
	// ����  : int privatesvr
	// ����  : vector<S_SERVICE_ROUTE> &rtlist
	// ����  : ��ȡ���ܣ�ֻȡ1��
	bool SelectBySvr(int svr,int privatesvr , std::vector<S_SERVICE_ROUTE> &rtlist);
	bool SelectBySvr(int svr,int privatesvr , std::vector<S_SERVICE_ROUTE *> &rtlist);

	bool SelectByFuncSvr(int func,int svr, std::vector<S_SERVICE_ROUTE *> &rtlist);
	bool SelectByDrebFuncSvr(int drebid,int func,int svr, std::vector<S_SERVICE_ROUTE *> &rtlist);

	// ������: SelectBySvr
	// ���  : ������ 2010-7-27 11:26:45
	// ����  : bool 
	// ����  : int svr
	// ����  : vector<S_SERVICE_ROUTE> &rtlist
	// ����  : ͨ������Ż�ȡΨһ�ķ����б�
	bool SelectBySvr(int svr, std::vector<S_SERVICE_ROUTE> &rtlist);
	bool SelectBySvr(int svr, std::vector<S_SERVICE_ROUTE *> &rtlist);


	bool Select(std::vector<S_SERVICE_ROUTE*>& rtlist);


	// ������: SelectByPk
	// ���  : ������ 2010-7-27 11:26:15
	// ����  : bool 
	// ����  : int index
	// ����  : int nodeid
	// ����  : int nodeprivated
	// ����  : int svrid
	// ����  : int privateid
	// ����  : unsigned int func
	// ����  : S_SERVICE_ROUTE &rt
	// ����  : ��ȡ����
	bool SelectByPk(int index,int nodeid,int nodeprivated,int svrid, int privateid,unsigned int func,S_SERVICE_ROUTE &rt);

	// ������: SelectByPk
	// ���  : ������ 2010-7-27 11:26:15
	// ����  : S_SERVICE_ROUTE *  NULLδ�ҵ�������Ϊָ��
	// ����  : int index
	// ����  : int nodeid
	// ����  : int nodeprivated
	// ����  : int svrid
	// ����  : int privateid
	// ����  : unsigned int func
	// ����  : ��ȡ����
	S_SERVICE_ROUTE * SelectByPk(int index,int nodeid,int nodeprivated,int svrid, int privateid,unsigned int func);
	// ������: Delete
	// ���  : ������ 2010-7-27 11:26:06
	// ����  : bool 
	// ����  : int index
	// ����  : int nodeid
	// ����  : int nodeprivated
	// ����  : int svrid
	// ����  : int privateid
	// ����  : unsigned int func
	// ����  : ɾ��һ������
	bool Delete(int index,int nodeid,int nodeprivated,int svrid, int privateid,unsigned int func);

	// ������: Update
	// ���  : ������ 2010-7-27 11:25:38
	// ����  : bool 
	// ����  : S_SERVICE_ROUTE rt
	// ����  : ���¹��ܣ�ֻ����nIndex nStep nUpdateTime�����ֶ�
	bool Update(S_SERVICE_ROUTE rt);

	// ������: SelectByFunc
	// ���  : ������ 2010-7-27 11:25:48
	// ����  : bool 
	// ����  : unsigned int func
	// ����  : vector<S_SERVICE_ROUTE> &rt
	// ����  : ͨ�����ܺŻ�ȡ�����б�
	bool SelectByFunc(unsigned int func, std::vector<S_SERVICE_ROUTE> &rt);

	// ������: SelectByFunc
	// ���  : ������ 2010-7-27 11:25:48
	// ����  : bool 
	// ����  : unsigned int func
	// ����  : vector<S_SERVICE_ROUTE> &rt
	// ����  : ͨ�����ܺŻ�ȡ�����б�  �����������������
	bool SelectByFunc(unsigned int func, std::vector<S_SERVICE_ROUTE*> &rt);

	// ������: Insert
	// ���  : ������ 2010-7-27 11:25:25
	// ����  : bool 
	// ����  : S_SERVICE_ROUTE rt
	// ����  : insertһ������
	bool Insert(S_SERVICE_ROUTE rt);
	CServiceTbl();
	virtual ~CServiceTbl();

	unsigned int m_nDeleteTime;//����ʱʱ��

	CIErrlog *m_pLog;
protected:
    // ������: DeleteById
    // ���  : ������ 2016-1-12 9:05:14
    // ����  : bool 
    // ����  : int rowid
    // ����  : ����rowidɾ����¼����������
    bool DeleteById(int rowid);

	CMemTableNew <S_SERVICE_ROUTE> m_table;        //SERVICE �ڴ��

	CIndexInt<6>             m_pkey;         //index+dreb+dreb˽�к�+�����+����˽�к�+���ܺţ�Ψһ����

	CIndexInt<1>             m_index_func;   //�����ܺ�����

	CIndexInt<1>             m_index_drebindex;   //dreb������ŵ�����

	CIndexInt<1>             m_index_svr;       //���������

	CIndexInt<2>             m_index_svrpri;    //���������

	CIndexInt<4>             m_index_route;         //dreb+dreb˽�к�+�����+����˽�к�

	CIndexInt<5>             m_index_indexroute;    //index,dreb+dreb˽�к�+�����+����˽�к�

	CIndexInt<2>             m_index_funcsvr;   //�����ܺ�+���������

	CIndexInt<3>             m_index_drebfuncsvr;   //����id+�����ܺ�+���������
	CBF_Mutex m_mutex;	
};

#endif // !defined(AFX_SERVICETBL_H__5398A809_0D06_4C51_B7DB_2E189754723A__INCLUDED_)
