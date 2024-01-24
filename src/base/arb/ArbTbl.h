// ArbTbl.h: interface for the CArbTbl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ARBTBL_H__CB8A3EEC_D62E_44EE_98D7_1CC3098F2B8D__INCLUDED_)
#define AFX_ARBTBL_H__CB8A3EEC_D62E_44EE_98D7_1CC3098F2B8D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MdbBase.h"
#include "IErrLog.h"

#define   ARBSTATUS_UNKNOW       0  //δ֪״̬
#define   ARBSTATUS_MASTER       1  //����״̬
#define   ARBSTATUS_MASTER_AYN   3  //����ͬ��״̬  3-4��Ӧ��˵���������ںͱ�����ͬ������ 
#define   ARBSTATUS_BACKUP       2  //����״̬
#define   ARBSTATUS_BACKUP_AYN   4  //����ͬ��״̬

//��¼Ӧ����������Ϣ��
typedef struct 
{
	unsigned short nNodeId;        //Ŀ���������߽ڵ���
	unsigned int   nSvrMainId;     //ע�����������߽ڵ��ϵ��������ܺ�   ֻ�з������ʱ������д
	char           cSvrPrivateId;  //ע�����������߽ڵ��ϵ�˽����� 0��ʾ���� >0Ϊÿ���������ţ�s_nSvrMainId+s_cSvrPrivateId ��Ϊ˽�й��ܺ�   ֻ�з������ʱ������д
	unsigned int   nUpdateTime;       //���һ�θ���ʱ�� ��Ӧ��֪ͨ��ʱ�� ��
	char           cStatus;           //״̬
	unsigned int   nLastSwtichTime;  //����л�ʱ�� ��
	unsigned int   nQueryStatusTime;  //���һ�β�ѯӦ��״̬ʱ�� ��
	unsigned int   nAddTime;          //�����ʱ�� ��
}  ARB_S_APPINFO;        //Ӧ��������Ϣ��

class CArbTbl  
{
public:
	void DeleteApp(unsigned int svr,char pid);

	// ������: GetAppInfo
	// ���  : ������ 2015-12-30 9:48:09
	// ����  : ARB_S_APPINFO * 
	// ����  : unsigned int svrid
	// ����  : char pid
	// ����  : ȡĳһӦ�õ���Ϣ
	ARB_S_APPINFO * GetAppInfo(unsigned int svrid,char pid);

	// ������: GetAppInfo
	// ���  : ������ 2015-12-30 8:55:59
	// ����  : bool 
	// ����  : int svrid
	// ����  : std::vector<ARB_S_APPINFO *> &applist
	// ����  : ���ݷ���IDȡ���еķ�����Ϣ
	bool GetAppInfo(unsigned int svrid,std::vector<ARB_S_APPINFO *> &applist);

	// ������: GetDistinctApp
	// ���  : ������ 2015-12-29 17:04:26
	// ����  : bool 
	// ����  : std::vector<int> &applist
	// ����  : ȡΨһ��Ӧ���б�
	bool GetDistinctApp(std::vector<int> &applist);

	// ������: GetAppList
	// ���  : ������ 2015-12-29 14:42:17
	// ����  : bool 
	// ����  : ȡ���е�Ӧ����Ϣ����app����
	bool GetAppList(std::vector<ARB_S_APPINFO> &applist);

	// ������: UpdateArb
	// ���  : ������ 2015-12-29 14:42:02
	// ����  : ARB_S_APPINFO * 
	// ����  : unsigned int svr
	// ����  : char pid
	// ����  : char status
	// ����  : unsigned short drebid
	// ����  : ����app����Ϣ
	ARB_S_APPINFO * UpdateArb(unsigned int svr,char pid,char status,unsigned short drebid);

	CArbTbl();
	virtual ~CArbTbl();

	CIErrlog    *m_pLog;

protected:
	CMemTableNew <ARB_S_APPINFO> m_table;        //�����ڴ��

	CIndexUInt<1>             m_index_appid;   //����id����
	CPkeyUInt<2>              m_pkey;         //����id+˽��id������Ψһ����
	
};

#endif // !defined(AFX_ARBTBL_H__CB8A3EEC_D62E_44EE_98D7_1CC3098F2B8D__INCLUDED_)
