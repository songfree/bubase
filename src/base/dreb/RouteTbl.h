// RouteTbl.h: interface for the CRouteTbl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ROUTETBL_H__D6D9B85B_A348_4786_AA6B_B4D8E5112F50__INCLUDED_)
#define AFX_ROUTETBL_H__D6D9B85B_A348_4786_AA6B_B4D8E5112F50__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MdbBase.h"
#include "BF_Mutex.h"
#include "IErrLog.h"
#include "DrebTbl.h"

// typedef struct 
// {
// 	unsigned short nNodeId;        //Ŀ���������߽ڵ���
// 	char           cNodePrivateId; //�������߽ڵ�˽�����
// 	unsigned int            nStep;          //0����1ֱ�����������߽ڵ�>1Ϊ�������߽ڵ��Ĳ���
// 	unsigned int            nIndex;         //��������  ͨ�������������ɵõ��������߽ڵ������
// 	unsigned int            nbandwidth;     //����
// }S_ROUTE_TBL;

// typedef struct 
// {
// 	unsigned short nNodeId;        //Ŀ���������߽ڵ���
// 	char           cNodePrivateId; //�������߽ڵ�˽�����
// 	int            nStep;          //0����1ֱ�����������߽ڵ�>1Ϊ�������߽ڵ��Ĳ���   �ر�ʱͳһ��Ϊ100
// 	int            nIndex;         //��������  ͨ�������������ɵõ��������߽ڵ������  0Ϊ���ػ��յ�
// 	int            nbandwidth;     //����
// 	CSendQueue     *pSendQueue;    //���Ͷ���
// 	bool           bIsClose;       //�����Ƿ�ر�
// 	int            nCloseTime;     //���ӹر�ʱ�䣬����һ��ʱ����Զ�ע��
// }S_DREB_ROUTE;

//�˱����������߽ڵ��·������

class CRouteTbl  
{
public:

	//��ȡ���е�·����Ϣ��id pid����
	bool  SelectById(std::vector<S_DREB_ROUTE*>& rtlist);

	// ������: GetRouteByIndex
	// ���  : ������ 2013-9-17 18:47:14
	// ����  : bool 
	// ����  : int index
	// ����  : vector<S_ROUTE_TBL> &rtlist
	// ����  : ͨ������������ȡ·��(��������index���·�ɶ�ȡ����)
	bool GetRouteByIndex(int index, std::vector<S_DREB_ROUTE *> &rtlist);

	// ������: UpdateRoute
	// ���  : ������ 2013-9-17 18:47:33
	// ����  : bool 
	// ����  : S_ROUTE_TBL rt
	// ����  : ����·��
	bool UpdateRoute(S_DREB_ROUTE rt);
	

	// ������: SelectByPrivateNode
	// ���  : ������ 2016-1-13 9:24:13
	// ����  : bool 
	// ����  : int nodeid
	// ����  : int privateid
	// ����  : vector<S_ROUTE_TBL *> &rt  ��������С������·�ɣ����ж����ͬ��С�����ģ��������С������
	// ����  : �����������߽ڵ�˽�нڵ�·�ɣ���ֱ�ӵĶϿ���ȥ·�ɱ����
	bool SelectByPrivateNode(int nodeid,int privateid, std::vector<S_DREB_ROUTE *> &rt);

	// ������: SelectByIndex
	// ���  : ������ 2009-10-28 16:29:15
	// ����  : bool 
	// ����  : int index
	// ����  : vector<S_ROUTE_TBL> &rt
	// ����  : �������Ӳ���·��
	bool SelectByIndex(int index, std::vector<S_DREB_ROUTE *> &rt);

	// ������: SelectByNode
	// ���  : ������ 2009-10-28 16:28:52
	// ����  : bool 
	// ����  : int nodeid
	// ����  : vector<S_ROUTE_TBL> &rt
	// ����  : �����������߽ڵ�ID��������·��
	bool SelectByNode(int nodeid, std::vector<S_DREB_ROUTE *> &rt);

	// ������: DeleteByIndex
	// ���  : ������ 2009-10-28 16:28:30
	// ����  : bool 
	// ����  : int index
	// ����  : ��������ɾ��·��
	bool DeleteByIndex(int index);

	// ������: DeleteByPk
	// ���  : ������ 2009-10-28 16:28:00
	// ����  : bool 
	// ����  : S_ROUTE_TBL rt
	// ����  : ����Ψһ����ɾ����(����)
	bool DeleteByPk(S_DREB_ROUTE rt);

	// ������: Update
	// ���  : ������ 2009-10-28 16:27:37
	// ����  : bool 
	// ����  : S_ROUTE_TBL rt
	// ����  : ����һ��·��,ֻ���´���
	bool Update(S_DREB_ROUTE rt);


	// ������: Insert
	// ���  : ������ 2009-10-28 16:27:22
	// ����  : bool 
	// ����  : S_ROUTE_TBL rt
	// ����  : ����һ��·��
	bool Insert(S_DREB_ROUTE rt);
	CRouteTbl();
	virtual ~CRouteTbl();
	
	char m_errMsg[256];
	CIErrlog *m_pLog;
protected:
	
	CMemTableNew <S_DREB_ROUTE> m_table;        //�������߽ڵ��ڴ��
	
	CIndexInt<1>             m_index_node;   //�������߽ڵ�id����
	CIndexInt<4>             m_pkey;         //�������߽ڵ�id+˽��id+�������+������Ψһ����
	CIndexInt<1>             m_index_index;  //�������߽ڵ������������
	CIndexInt<2>             m_index_private;   //�������߽ڵ�id+˽��id����
	
	
	CBF_Mutex m_mutex;
};


#endif // !defined(AFX_ROUTETBL_H__D6D9B85B_A348_4786_AA6B_B4D8E5112F50__INCLUDED_)
