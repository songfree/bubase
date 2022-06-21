// GateLink.h: interface for the CGateLink class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GATELINK_H__3EB6AE6D_984A_42CE_8D97_DE65D0C9C3FE__INCLUDED_)
#define AFX_GATELINK_H__3EB6AE6D_984A_42CE_8D97_DE65D0C9C3FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "BF_Thread.h"
#include "GateHead.h"
#include "BF_Mutex.h"
#include "DrebEndian.h"
#include "BF_Xml.h"
#include "ICertApiBase.h"
#include "BF_Tools.h"
#include "PoolData.h"
#include "ApiSocket.h"
#include "GlobalResource.h"

#ifdef _WINDOWS
#ifdef _DEBUG
#pragma comment(lib, "bf_dbpubd.lib") 
#pragma comment(lib, "bf_kerneld.lib") 
#pragma message("Automatically linking with bf_dbpubd.lib bf_kerneld.lib ")
#else
#pragma comment(lib, "bf_kernel.lib") 
#pragma comment(lib, "bf_dbpub.lib") 
#pragma message("Automatically linking with bf_dbpub.lib bf_kernel.lib ")
#endif

#endif

// <?xml version="1.0" encoding="GB2312"?>
// <����API>
// <��־>
// <��־���� remark="��־���ټ���0���ش���1��Ҫ����2һ�����3����4��ʾ����5������Ϣ">5</��־����>
// <��־�ļ��� remark="��¼��־��Ϣ���ļ���,ǰ������">cgateapi.log</��־�ļ���>
// <��־Ŀ¼ remark="�����־��Ŀ¼"></��־Ŀ¼>
// </��־>
// <����  ����ʱ��="30" remark="ͨѶʱ���������ļ�� ��">
// <gate1  ip="" port="" remark="���ط�������IP���˿�" />
// <gate2  ip="" port="" remark="���ط�������IP���˿�" />
// </����>
// <֤�� use="0" dllpath="/home/songfree/lib" remark="�Ƿ�ʹ��֤�鼰֤�鶯̬�����ڵ�Ŀ¼">
// <shcaapi.dll  method="getInstance"  remark="֤�鹦�ܶ�̬��,methodΪʵ����������" />
// </֤��>
// </����API>
#ifdef CGATEAPI_EXPORTS
#define CGATEAPI_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define CGATEAPI_EXPORT __declspec(dllimport)
#else
#define CGATEAPI_EXPORT 
#endif
#endif

class CGATEAPI_EXPORT CGateLink : public CBF_Thread  
{
public:
	void SetDatLogName(const char *name);

	// ������: SendLogin
	// ���  : ������ 2013-5-23 15:12:27
	// ����  : int -1δ����  -2���ͳ���  =0 ��ʱδ�յ�Ӧ�� ���ڵ���1�յ�Ӧ�� ����1ΪӦ���������ֵΪ-1
	// ����  : PCGATE_COMMSTRU data   ����Ӧ������
	// ����  : unsigned int timeout    ����Ӧ��ʱʱ�� <=0ֱ�ӷ���0��������Ӧ��
	// ����  : �������󲢽���Ӧ��
	int SendLogin(PCGATE_COMMSTRU data,unsigned int timeout);

	// ������: SendSms
	// ���  : ������ 2013-5-23 15:12:27
	// ����  : int -1δ����  -2���ͳ���  =0 ��ʱδ�յ�Ӧ�� ���ڵ���1�յ�Ӧ�� ����1ΪӦ���������ֵΪ-1
	// ����  : PCGATE_COMMSTRU data   ����Ӧ������
	// ����  : unsigned int timeout    ����Ӧ��ʱʱ�� <=0ֱ�ӷ���0��������Ӧ��
	// ����  : �������󲢽���Ӧ��
	int SendSms(PCGATE_COMMSTRU data,unsigned int timeout);

	// ������: GetConnectStatus
	// ���  : ������ 2013-5-30 17:36:08
	// ����  : 0δ����  1������   2��������
	// ����  : ȡ����״̬  ǰ�˵��ô˷���ʱ���������״̬Ϊ2ʱ����ѭ��sleep������Ƿ����Ӻã�����ֱ�ӵ���connectgate��رյ�ǰ����
	int GetConnectStatus();

	// ������: ConnectGate
	// ���  : ������ 2013-5-29 15:27:27
	// ����  : int 0δ���� 1���ӳɹ�  -1����ʧ��
	// ����  : �������� �رյ�ǰ����(������ϵĻ�)����������
	int ConnectGate();

	// ������: Logout
	// ���  : ������ 2013-5-23 15:20:54
	// ����  : void 
	// ����  : �ر�����,�˳��߳�
	void Logout();

	// ������: InitApi
	// ���  : ������ 2013-5-23 14:49:22
	// ����  : bool true�ɹ�
	// ����  : const char *apixml xml�����ļ�
	// ����  : ��ʼ��api����ȡ�����ļ����õ�֤��Ŀ¼�����ص�ַ�˿�
	bool InitApi(const char *apixml);

	// ������: SendReqData
	// ���  : ������ 2013-5-23 15:12:27
	// ����  : int -1δ����  -2���ͳ���  =0 ��ʱδ�յ�Ӧ�� ���ڵ���1�յ�Ӧ�� ����1ΪӦ���������ֵΪ-1
	// ����  : PCGATE_COMMSTRU data   ����Ӧ������  data->head.stDest.nSerialΪָ�����
	// ����  : unsigned int timeout    ����Ӧ��ʱʱ�� <=0ֱ�ӷ���0��������Ӧ��
	// ����  : �������󲢽���Ӧ��
	int SendReqData(PCGATE_COMMSTRU data,unsigned int timeout);

	// ������: GetReqData
	// ���  : ������ 2013-5-23 15:12:42
	// ����  : int <0���� =0��ʱ������  >1�ɹ�ȡ����������  ʣ��������Ϊ����-1
	// ����  : PCGATE_COMMSTRU data   ��������
	// ����  : int timeout ��ʱʱ����
	// ����  : ȡ���ͻ�㲥����
	int GetReqData(PCGATE_COMMSTRU data,int timeout);

	// ������: GetAnsData
	// ���  : ������ 2013-5-23 15:14:47
	// ����  : int <0���� =0��ʱ������  1�ɹ�ȡ����������
	// ����  : PCGATE_COMMSTRU data   ��������  data->head.stDest.nSerial >0Ϊָ����ŵ�Ӧ�� =0ΪȡӦ��
	// ����  : int timeout ��ʱʱ����
	// ����  : ȡ��ָ����ˮ��Ӧ��
	int GetAnsData(PCGATE_COMMSTRU data,int timeout);


	// ������: Subscribe
	// ���  : ������ 2014-10-21 15:29:00
	// ����  : int -1δ����  -2���ͳ���  =0 ��ʱδ�յ�Ӧ�� ���ڵ���1�յ�Ӧ�� ����1ΪӦ���������ֵ
	// ����  : PCGATE_COMMSTRU data
	// ����  : unsigned int timeout <=0ֱ�ӷ���0��������Ӧ��
	// ����  : �����ط��Ͷ�������ע��������һ���ṹ������ʹ��LE ������
	int	Subscribe(PCGATE_COMMSTRU data,unsigned int timeout);

	int Subscribe(int flag,std::vector<int> &keylist);

	CGateLink();
	virtual ~CGateLink();

    //��Ϣ����ص����û��ɼ̳�ʵ�֣�����͵��� GetReqData��GetAnsData
	virtual void ProcessMsg(CGATE_COMMSTRU& data);


	CGlobalResource m_pRes;
private:

	

	// ������: ExitThreadInstance
	// ���  : ������ 2013-5-27 10:01:19
	// ����  : virtual void 
	// ����  : �˳��߳�
	virtual void ExitThreadInstance();

	// ������: InitThreadInstance
	// ���  : ������ 2013-5-27 10:01:23
	// ����  : virtual bool 
	// ����  : ��ʼ�߳�
	virtual bool InitThreadInstance();

	
	// ������: Run
	// ���  : ������ 2013-5-23 15:26:51
	// ����  : virtual int 
	// ����  : �߳�������
	virtual int Run();

	CApiSocket      m_pApiSocket;
	

	int m_nIsConnect; //����״̬ 0δ����  1���ӳɹ�   2��������

	char          m_sErrMsg[256];

	unsigned short m_nSerial;//�ͻ��˷��͵���ˮ

	CBF_Mutex     m_mutex;

	bool          m_bIsRunThread;

	fd_set m_rset;
	fd_set m_wset;
#if defined(_WINDOWS)
	fd_set m_eset;
#endif
	
	

protected:
	

	void OnRecv();

	// ������: OnNoEvent
	// ���  : ������ 2013-5-30 14:48:37
	// ����  : void 
	// ����  : û���¼���������Ӷ�ʱ�䣬������������ʱ�Ͽ�
	void OnNoEvent();


	// ������: OnEvent
	// ���  : ������ 2013-5-30 14:47:19
	// ����  : void 
	// ����  : ���¼������ж�д���
	void OnEvent();


	// ������: OnPing
	// ���  : ������ 2013-5-30 14:47:51
	// ����  : void 
	// ����  : ����ping
	void OnPing();
};


extern "C"
{	
	// ������: ConnectGate
	// ���  : ������ 2013-5-29 15:27:27
	// ����  : int  1�ɹ� 0Ϊ���� 2��������
	// ����  : ��������
	CGATEAPI_EXPORT	int BFCGATE_GetConnectStatus();

	// ������: ConnectGate
	// ���  : ������ 2013-5-29 15:27:27
	// ����  : int  0δ����   1���ӳɹ�  -1����δ�ɹ�  
	// ����  : ��������
	CGATEAPI_EXPORT	int BFCGATE_ConnectGate();
	
	// ������: Logout
	// ���  : ������ 2013-5-23 15:20:54
	// ����  : void 
	// ����  : �رձ߽�,�˳��߳�
	CGATEAPI_EXPORT	void BFCGATE_Logout();
	
	// ������: InitApi
	// ���  : ������ 2013-5-23 14:49:22
	// ����  : int 1�ɹ�
	// ����  : const char *apixml xml�����ļ�
	// ����  : ��ʼ��api����ȡ�����ļ����õ�֤��Ŀ¼�����ص�ַ�˿�
	CGATEAPI_EXPORT	int BFCGATE_InitApi(const char *apixml);
	
	
	// ������: SendReqData
	// ���  : ������ 2013-5-23 15:12:27
	// ����  : int -1δ����  -2���ͳ���  =0 ��ʱδ�յ�Ӧ�� ���ڵ���1�յ�Ӧ�� ����1ΪӦ���������ֵΪ-1
	// ����  : PCGATE_COMMSTRU data   ����Ӧ������
	// ����  : unsigned int timeout    ����Ӧ��ʱʱ��
	// ����  : �������󲢽���Ӧ��
	CGATEAPI_EXPORT int BFCGATE_SendReqData(void *data,unsigned int timeout);
	

	// ������: BFCGATE_Subscribe
	// ���  : ������ 2014-10-21 15:51:02
	// ����  : int  -1δ����  -2���ͳ���  =0 ��ʱδ�յ�Ӧ�� >=1�յ�Ӧ�� �������ֵΪ��ֵ��1
	// ����  : void *pClass ����������ָ��
	// ����  : void *data ����Ӧ������
	// ����  : unsigned int timeout ����Ӧ��ʱʱ��
	// ����  : �������ض�������
	CGATEAPI_EXPORT int BFCGATE_Subscribe(void *data,unsigned int timeout);

	// ������: GetReqData
	// ���  : ������ 2013-5-23 15:12:42
	// ����  : int <0���� =0��ʱ������  >=1�ɹ�ȡ���������� �㲥�������ֵΪ��ֵ��1
	// ����  : PCGATE_COMMSTRU data   ��������
	// ����  : unsigned int timeout ��ʱʱ����
	// ����  : ȡ���ͻ�㲥����
	CGATEAPI_EXPORT int BFCGATE_GetReqData(void *data,unsigned int timeout);
	
	// ������: GetAnsData
	// ���  : ������ 2013-5-23 15:14:47
	// ����  : int <0���� =0��ʱ������  >1�ɹ�ȡ���������� �������ֵΪ��ֵ��1
	// ����  : PCGATE_COMMSTRU data   �������� data->head.stDest.nSerial >0Ϊָ����ŵ�Ӧ�� =0ΪȡӦ��
	// ����  : unsigned int timeout ��ʱʱ����
	// ����  : ȡ��ָ����ˮ��Ӧ��
	CGATEAPI_EXPORT int BFCGATE_GetAnsData(void *data,unsigned int timeout);
	
	// ������: SendLogin
	// ���  : ������ 2013-5-23 15:12:27
	// ����  : int -1δ����  -2���ͳ���  =0 ��ʱδ�յ�Ӧ�� >=1�յ�Ӧ�� �������ֵΪ��ֵ��1
	// ����  : PCGATE_COMMSTRU data   ����Ӧ������
	// ����  : unsigned int timeout    ����Ӧ��ʱʱ��
	// ����  : �������󲢽���Ӧ��
	CGATEAPI_EXPORT int BFCGATE_SendLogin(void *data,unsigned int timeout);
	
	// ������: SendSms
	// ���  : ������ 2013-5-23 15:12:27
	// ����  : int -1δ����  -2���ͳ���  =0 ��ʱδ�յ�Ӧ�� >=1�յ�Ӧ�� �������ֵΪ��ֵ��1
	// ����  : PCGATE_COMMSTRU data   ����Ӧ������
	// ����  : unsigned int timeout    ����Ӧ��ʱʱ��
	// ����  : �������󲢽���Ӧ��
	CGATEAPI_EXPORT int BFCGATE_SendSms(void *data,unsigned int timeout);

	// ������: BFCGATE_TS_GetConnectStatus
	// ���  : ������ 2013-12-4 14:19:35
	// ����  : int 
	// ����  : void *pClass  ����������ָ��
	// ����  : ȡ����������״̬
	CGATEAPI_EXPORT	int BFCGATE_TS_GetConnectStatus(void *pClass);
	
	// ������: BFCGATE_TS_ConnectGate
	// ���  : ������ 2013-12-4 14:20:06
	// ����  : int 0�ɹ�
	// ����  : void *pClass ����������ָ��
	// ����  : ��������
	CGATEAPI_EXPORT	int BFCGATE_TS_ConnectGate(void *pClass);
	
	// ������: BFCGATE_TS_Logout
	// ���  : ������ 2013-12-4 14:20:32
	// ����  : void 
	// ����  : void *pClass ����������ָ��
	// ����  : �ر�����,�˳��̣߳��ͷ�ָ��ռ�
	CGATEAPI_EXPORT	void BFCGATE_TS_Logout(void *pClass);
	
	// ������: BFCGATE_TS_InitApi
	// ���  : ������ 2013-12-4 14:20:53
	// ����  : int 1�ɹ�
	// ����  : const char *apixml
	// ����  : void **pClass  ���ص�����������ָ��
	// ����  : ��ʼ��api����ȡ�����ļ����õ�֤��Ŀ¼�����ص�ַ�˿�
	CGATEAPI_EXPORT	int BFCGATE_TS_InitApi(const char *apixml,void **pClass);
	
	
	// ������: BFCGATE_TS_SendReqData
	// ���  : ������ 2013-5-23 15:12:27
	// ����  : int -1δ����  -2���ͳ���  =0 ��ʱδ�յ�Ӧ�� >=1�յ�Ӧ�� �������ֵΪ��ֵ��1
	// ����  : void *pClass  ����������ָ��
	// ����  : PCGATE_COMMSTRU data   ����Ӧ������
	// ����  : unsigned int timeout    ����Ӧ��ʱʱ��
	// ����  : �������󲢽���Ӧ��
	CGATEAPI_EXPORT int BFCGATE_TS_SendReqData(void *pClass,void *data,unsigned int timeout);

	// ������: BFCGATE_TS_Subscribe
	// ���  : ������ 2014-10-21 15:51:02
	// ����  : int  -1δ����  -2���ͳ���  =0 ��ʱδ�յ�Ӧ�� >=1�յ�Ӧ�� �������ֵΪ��ֵ��1
	// ����  : void *pClass ����������ָ��
	// ����  : void *data ����Ӧ������
	// ����  : unsigned int timeout ����Ӧ��ʱʱ��
	// ����  : �������ض�������
	CGATEAPI_EXPORT int BFCGATE_TS_Subscribe(void *pClass,void *data,unsigned int timeout);

	// ������: BFCGATE_TS_SendLogin
	// ���  : ������ 2013-5-23 15:12:27
	// ����  : int -1δ����  -2���ͳ���  =0 ��ʱδ�յ�Ӧ�� >=1�յ�Ӧ�� �������ֵΪ��ֵ��1
	// ����  : void *pClass          ����������ָ��
	// ����  : PCGATE_COMMSTRU *data            ����Ӧ������
	// ����  : unsigned int timeout    ����Ӧ��ʱʱ��
	// ����  : �������󲢽���Ӧ��
	CGATEAPI_EXPORT int BFCGATE_TS_SendLogin(void *pClass,void *data,unsigned int timeout);
	
	// ������: BFCGATE_TS_SendSms
	// ���  : ������ 2013-5-23 15:12:27
	// ����  : int -1δ����  -2���ͳ���  =0 ��ʱδ�յ�Ӧ�� >=1�յ�Ӧ�� �������ֵΪ��ֵ��1
	// ����  : void *pClass          ����������ָ��
	// ����  : PCGATE_COMMSTRU data             ����Ӧ������
	// ����  : unsigned int timeout    ����Ӧ��ʱʱ��
	// ����  : �������󲢽���Ӧ��
	CGATEAPI_EXPORT int BFCGATE_TS_SendSms(void *pClass,void *data,unsigned int timeout);

	// ������: BFCGATE_TS_GetAnsData
	// ���  : ������ 2013-5-23 15:14:47
	// ����  : int <0���� =0��ʱ������  >=1�ɹ�ȡ���������� �������ֵΪ��ֵ��1
	// ����  : void *pClass          ����������ָ��
	// ����  : PCGATE_COMMSTRU data   �������� data->head.stDest.nSerial >0Ϊָ����ŵ�Ӧ�� =0ΪȡӦ��
	// ����  : int timeout ��ʱʱ����
	// ����  : ȡ��ָ����ˮ��Ӧ��
	CGATEAPI_EXPORT int BFCGATE_TS_GetAnsData(void *pClass,void *data,int timeout);

	// ������: BFCGATE_TS_GetReqData
	// ���  : ������ 2013-5-23 15:12:42
	// ����  : int <0���� =0��ʱ������  >=1�ɹ�ȡ���������� �������ֵΪ��ֵ��1
	// ����  : void *pClass          ����������ָ��
	// ����  : PCGATE_COMMSTRU data   ��������
	// ����  : int timeout ��ʱʱ����
	// ����  : ȡ���ͻ�㲥����
	CGATEAPI_EXPORT int BFCGATE_TS_GetReqData(void *pClass,void *data,int timeout);
	
}


#endif // !defined(AFX_GATELINK_H__3EB6AE6D_984A_42CE_8D97_DE65D0C9C3FE__INCLUDED_)
