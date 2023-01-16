// DrebTbl.h: interface for the CDrebTbl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DREBTBL_H__AC496175_A6AC_4B82_AAD4_82853BFFA88A__INCLUDED_)
#define AFX_DREBTBL_H__AC496175_A6AC_4B82_AAD4_82853BFFA88A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MdbBase.h"
#include "BF_Mutex.h"
#include "SendQueue.h"
#include "IErrLog.h"

typedef struct 
{
	unsigned short nNodeId;        //Ŀ���������߽ڵ���
	char           cNodePrivateId; //�������߽ڵ�˽�����
	unsigned int   nStep;          //0����1ֱ�����������߽ڵ�>1Ϊ�������߽ڵ��Ĳ���   �ر�ʱͳһ��Ϊ100
	unsigned int   nIndex;         //��������  ͨ�������������ɵõ��������߽ڵ������  0Ϊ���ػ��յ�
	unsigned int   nbandwidth;     //����
	CSendQueue     *pSendQueue;    //���Ͷ���
	bool           bIsClose;       //�����Ƿ�ر�
	int            nCloseTime;     //���ӹر�ʱ�䣬����һ��ʱ����Զ�ע��
}S_DREB_ROUTE;


//�˱�ֻ��ű��������߽ڵ㼰�������������߽ڵ������
//ֱ�������߶Ͽ��󲢲�ɾ����ֻ����bIsCloseΪtrue nStep��־Ϊ100��nIndexΪ0��������ʱ����ʹ�ô˼�¼
class CDrebTbl  
{
public:
	CDrebTbl();
	virtual ~CDrebTbl();

public:


	// ������: SelectByIndex
	// ���  : ������ 2009-10-29 10:37:19
	// ����  : bool 
	// ����  : int index
	// ����  : S_DREB_ROUTE &dreb
	// ����  : ͨ���������������������߽ڵ�
	S_DREB_ROUTE * SelectByIndex(int index);

	//����������ȡ��������Ҫ·�ɵ���Ϣ
	bool SelectRouteByIndex(int index, std::vector<S_DREB_ROUTE*>& dreb);

	/// ������: UnRegister
	/// ���  : ������ 2009-10-27 14:19:27
	/// ����  : bool 
	/// ����  : int nodeid
	/// ����  : int privateid
	/// ����  : ȡ���������߽ڵ��ע�ᣬ���ٷ��Ͷ���
	bool UnRegister(int nodeid,int privateid);
	
	/// ������: OnClose
	/// ���  : ������ 2009-7-29 14:34:15
	/// ����  : bool 
	/// ����  : int nodeid
	/// ����  : int privateid
	/// ����  : �����������߽ڵ�ID��˽��ID�ر��������߽ڵ����Ϣ����ɾ���Ҳ����ٷ��Ͷ���
	bool OnClose(int nodeid,int privateid);

	/// ������: Update
	/// ���  : ������ 2009-7-29 14:32:52
	/// ����  : bool 
	/// ����  : S_DREB_ROUTE dreb
	/// ����  : �����������߽ڵ�ID��˽��ID�����������߽ڵ���Ϣ������С�ڵ�ǰ���������Ͷ�����Ϊ�ո���
	bool Update(S_DREB_ROUTE dreb);

	/// ������: Delete
	/// ���  : ������ 2009-7-29 14:32:31
	/// ����  : bool 
	/// ����  : int nodeid
	/// ����  : int privateid
	/// ����  : �����������߽ڵ�ID��˽��IDɾ��һ���������߽ڵ��¼
	bool Delete(int nodeid,int privateid);

	/// ������: SelectPrivateid
	/// ���  : ������ 2009-7-29 14:31:23
	/// ����  : bool 
	/// ����  : int nodeid
	/// ����  : int privateid
	/// ����  : S_DREB_ROUTE &dreb
	/// ����  : �����������߽ڵ�ID��˽��ID�����������߽ڵ���Ϣ
	bool SelectPrivateid(int nodeid,int privateid,S_DREB_ROUTE &dreb);
	S_DREB_ROUTE * SelectPrivateid(int nodeid,int privateid);
	/// ������: SelectByNode
	/// ���  : ������ 2009-7-29 14:30:58
	/// ����  : bool 
	/// ����  : int nodeid
	/// ����  : vector<S_DREB_ROUTE> &dreb
	/// ����  : �����������߽ڵ�ID�������е��������߽ڵ���Ϣ
	bool SelectByNode(int nodeid,std::vector<S_DREB_ROUTE> &dreb);

	// ������: SelectByNode
	// ���  : ������ 2016-1-13 10:35:14
	// ����  : bool 
	// ����  : int nodeid
	// ����  : vector<S_DREB_ROUTE *> &dreb
	// ����  : ȡ��С���������߽ڵ���Ϣ  ����������
	bool SelectByNode(int nodeid, std::vector<S_DREB_ROUTE *> &dreb);

	/// ������: Insert
	/// ���  : ������ 2009-7-29 14:30:35
	/// ����  : bool 
	/// ����  : S_DREB_ROUTE dreb
	/// ����  : ����һ���������߽ڵ��¼
	bool Insert(S_DREB_ROUTE dreb);

	bool Select(std::vector<S_DREB_ROUTE*>& dreb);

	char m_errMsg[256];
	CIErrlog *m_pLog;
protected:
	
	CMemTableNew <S_DREB_ROUTE> m_table;        //�������߽ڵ��ڴ��

 	CIndexInt<1>             m_index_node;   //�������߽ڵ�id����
 	CPkeyInt<2>             m_pkey;         //�������߽ڵ�id+˽��id������Ψһ����
	CIndexInt<1>             m_index_index;   //�������߽ڵ���������
	CBF_Mutex m_mutex;
};



#endif // !defined(AFX_DREBTBL_H__AC496175_A6AC_4B82_AAD4_82853BFFA88A__INCLUDED_)
