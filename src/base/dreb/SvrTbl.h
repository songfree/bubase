// SvrTbl.h: interface for the CSvrTbl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SVRTBL_H__2C910750_3DE2_4D30_99E9_BAC6EFB6338B__INCLUDED_)
#define AFX_SVRTBL_H__2C910750_3DE2_4D30_99E9_BAC6EFB6338B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "MdbBase.h"
#include "BF_Mutex.h"
#include "SendQueue.h"
#include "IErrLog.h"

typedef struct 
{
	unsigned int   nSvrMainId;     //ע�����������߽ڵ��ϵ��������ܺ�   ֻ�з������ʱ������д
	char           cSvrPrivateId;  //ע�����������߽ڵ��ϵ�˽����� 0��ʾ���� >0Ϊÿ���������ţ�s_nSvrMainId+s_cSvrPrivateId ��Ϊ˽�й��ܺ�   ֻ�з������ʱ������д
	unsigned int   nIndex;         //�������� 0Ϊ����ȡ�� ͨ�������������ɵõ�ͨѶƽ̨������
	CSendQueue     *pSendQueue;    //���Ͷ���
	bool           bIsClose;       //�����Ƿ�ر�
	int            nCloseTime;     //���ӹر�ʱ�䣬����һ��ʱ����Զ�ע��
	
}S_SVR_ROUTE;

//��ű�������ֱ���ķ�����Ϣ,��ע��ʱд�룬�Ͽ��󲢲�ɾ����ֻ����nIndexΪ0��bIsCloseΪtrue

class CSvrTbl  
{
public:
	// ������: Next
	// ���  : ������ 2009-10-29 17:08:54
	// ����  : bool 
	// ����  : S_SVR_ROUTE &svr
	// ����  : ��һ����¼
	bool Next(S_SVR_ROUTE &svr);
	
	// ������: First
	// ���  : ������ 2009-10-29 17:08:46
	// ����  : bool 
	// ����  : S_SVR_ROUTE &svr
	// ����  : ��һ����¼
	bool First(S_SVR_ROUTE &svr);

	/// ������: UnRegister
	/// ���  : ������ 2009-10-27 14:54:02
	/// ����  : bool 
	/// ����  : int svrid
	/// ����  : int privateid
	/// ����  : ע������ɾ������
	bool UnRegister(int svrid,int privateid);
	

	/// ������: OnClose
	/// ���  : ������ 2009-10-27 14:26:43
	/// ����  : bool 
	/// ����  : int svrid
	/// ����  : int privateid
	/// ����  : �رշ���
	bool OnClose(int svrid,int privateid);

	/// ������: Update
	/// ���  : ������ 2009-10-27 14:26:32
	/// ����  : bool 
	/// ����  : S_SVR_ROUTE svr
	/// ����  : ���·���
	bool Update(S_SVR_ROUTE svr);

	/// ������: Delete
	/// ���  : ������ 2009-10-27 14:26:25
	/// ����  : bool 
	/// ����  : int svrid
	/// ����  : int privateid
	/// ����  : ɾ����¼
	bool Delete(int svrid,int privateid);

	/// ������: SelectPrivateid
	/// ���  : ������ 2009-10-27 14:26:08
	/// ����  : bool 
	/// ����  : int svrid
	/// ����  : int privateid
	/// ����  : S_SVR_ROUTE &svr
	/// ����  : ͨ������ID��˽��ID��ѯ����
	bool SelectPrivateid(int svrid,int privateid,S_SVR_ROUTE &svr);

	// ������: SelectPrivateid
	// ���  : ������ 2016-1-13 13:43:07
	// ����  : S_SVR_ROUTE * 
	// ����  : unsigned int svrid
	// ����  : int privateid
	// ����  : ͨ������IDȡ������Ϣ��ȥ�����ӹرյ�
	S_SVR_ROUTE * SelectPrivateid(unsigned int svrid, int privateid);

	/// ������: SelectBySvr
	/// ���  : ������ 2009-10-27 14:25:51
	/// ����  : bool 
	/// ����  : int svrid
	/// ����  : vector<S_SVR_ROUTE> &svr
	/// ����  : ͨ������ID��ѯ�����¼
	bool SelectBySvr(int svrid,vector<S_SVR_ROUTE> &svr);

	// ������: SelectBySvr
	// ���  : ������ 2016-1-13 13:42:33
	// ����  : bool 
	// ����  : unsigned int svrid
	// ����  : vector<S_SVR_ROUTE *> &svr
	// ����  : ͨ������IDȡ������Ϣ��ȥ�����ӹرյ�
	bool SelectBySvr(unsigned int svrid,vector<S_SVR_ROUTE *> &svr);

	/// ������: Insert
	/// ���  : ������ 2009-10-27 14:25:26
	/// ����  : bool 
	/// ����  : S_SVR_ROUTE svr
	/// ����  : ����һ�������¼
	bool Insert(S_SVR_ROUTE svr);
	CSvrTbl();
	virtual ~CSvrTbl();

	CIErrlog *m_pLog;
protected:
	
	CMemTable <S_SVR_ROUTE> m_table;        //ͨѶƽ̨�ڴ��
	
	CIndexInt<1>             m_index_node;   //����id����
	CIndexInt<2>             m_pkey;         //����id+˽��id������Ψһ����


//	CBF_Mutex m_mutex;	
};

#endif // !defined(AFX_SVRTBL_H__2C910750_3DE2_4D30_99E9_BAC6EFB6338B__INCLUDED_)
