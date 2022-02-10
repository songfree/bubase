// DrebClientApi.h: interface for the CDrebClientApi class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DREBCLIENTAPI_H__32520F7E_77D8_468D_A23F_0945004C9602__INCLUDED_)
#define AFX_DREBCLIENTAPI_H__32520F7E_77D8_468D_A23F_0945004C9602__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DrebPubDefine.h"

#ifdef DREBAPI_EXPORTS
#define DREBAPI_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define DREBAPI_EXPORT __declspec(dllimport)
#else
#define DREBAPI_EXPORT 
#endif
#endif

extern "C"
{	
	

	// ������: Monitor_Dreb
	// ���  : ������ 2009-12-2 15:23:40
	// ����  : int >0<DREBHEADLENͨѶƽ̨���ش� >DREBHEADLEN�յ������ݳ��� =0��ָ��ʱ����δ�յ�����  <0ʧ�� -100δ���� -1���ӶϿ� -2ͨѶƽ̨Ӧ�����ݲ��� -3CRC����
	// ����  : void *pClass    �ͻ�����ָ��
	// ����  : int hook
	// ����  : int serial
	// ����  : DREB_D_NODEINFO dest
	// ����  : const char *data  ��ӦcZip������(���ļ��ܻ�ѹ��)
	// ����  : int datalen ��ӦcZip�����ݳ���
	// ����  : int cZip
	// ����  : DREB_HEAD &head
	// ����  : char *ansdata   ��Ӧhead->cZip�⿪�����������,�Զ������
	// ����  : ͨѶƽ̨��� Ҫ�����մ�����ɺ�Ӧ��
	DREBAPI_EXPORT int BFDREBCLIENT_API_Monitor_Dreb(void *pClass,int hook,int serial,DREB_D_NODEINFO dest, const char *data, int datalen,int cZip,DREB_HEAD &head,char *ansdata);

	// ������: Monitor_Bpc
	// ���  : ������ 2009-12-2 15:23:40
	// ����  : int >0<DREBHEADLENͨѶƽ̨���ش� >DREBHEADLEN�յ������ݳ��� =0��ָ��ʱ����δ�յ�����  <0ʧ�� -100δ���� -1���ӶϿ� -2ͨѶƽ̨Ӧ�����ݲ��� -3CRC����
	// ����  : void *pClass    �ͻ�����ָ��
	// ����  : int hook
	// ����  : int serial
	// ����  : DREB_D_NODEINFO dest
	// ����  : const char *data  ��ӦcZip������(���ļ��ܻ�ѹ��)
	// ����  : int datalen ��ӦcZip�����ݳ���
	// ����  : int cZip
	// ����  : DREB_HEAD &head
	// ����  : char *ansdata   ��Ӧhead->cZip�⿪�����������,�Զ������
	// ����  : BPC��� Ҫ�����մ�����ɺ�Ӧ��
	DREBAPI_EXPORT int BFDREBCLIENT_API_Monitor_Bpc(void *pClass,int hook,int serial,DREB_D_NODEINFO dest, const char *data, int datalen,int cZip,DREB_HEAD &head,char *ansdata);

	// ������: GetDrebHead
	// ���  : ������ 2010-9-28 9:18:45
	// ����  : void 
	// ����  : DREB_HEAD &head
	// ����  : ��ȡ����ʱ���ص�drebͷ
	DREBAPI_EXPORT void BFDREBCLIENT_API_GetDrebHead(void *pClass,DREB_HEAD &head);

	// ������: DPABC
	// ���  : ������ 2009-12-2 15:25:00
	// ����  : int <0����ʧ�ܣ�>=���͵����ݳ���
	// ����  : void *pClass    �ͻ�����ָ��
	// ����  : int hook
	// ����  : int serial
	// ����  : int gateindex   
	// ����  : DREB_D_NODEINFO dest
	// ����  : const char *data  ��ӦcZip������(���ļ��ܻ�ѹ��)
	// ����  : int datalen ��ӦcZip�����ݳ���
	// ����  : int cZip
	// ����  : ͨѶƽ̨�㲥��������Ϣ����ָ��ͨѶƽ̨������ע��ķ���,��Ҫ��ͨѶƽ̨Ӧ��
	DREBAPI_EXPORT int BFDREBCLIENT_API_DPABC(void *pClass,int hook, int serial,int gateindex,DREB_D_NODEINFO dest, const char *data, int datalen,int cZip,int nextno=0);

	// ������: DPBC
	// ���  : ������ 2009-12-2 15:24:42
	// ����  : int =100�ɹ� >0��<100dreb���ش� <0����ʧ�� =0δ�յ�ȷ����Ϣ
	// ����  : void *pClass    �ͻ�����ָ��
	// ����  : int hook
	// ����  : int serial
	// ����  : DREB_D_NODEINFO dest
	// ����  : const char *data  ��ӦcZip������(���ļ��ܻ�ѹ��)
	// ����  : int datalen ��ӦcZip�����ݳ���
	// ����  : int cZip
	// ����  : ͨѶƽ̨�㲥��������Ϣ����ָ��ͨѶƽ̨������ע��ķ���,Ҫ��ͨѶƽ̨Ӧ��
	DREBAPI_EXPORT int BFDREBCLIENT_API_DPBC(void *pClass,int hook, int serial,DREB_D_NODEINFO dest, const char *data, int datalen,int cZip);

	// ������: DPPost
	// ���  : ������ 2009-12-2 15:24:28
	// ����  : int =100�ɹ� >0��<100dreb���ش� <0����ʧ�� =0δ�յ�ȷ����Ϣ
	// ����  : void *pClass    �ͻ�����ָ��
	// ����  : int hook
	// ����  : int serial
	// ����  : DREB_D_NODEINFO dest
	// ����  : const char *data  ��ӦcZip������(���ļ��ܻ�ѹ��)
	// ����  : int datalen ��ӦcZip�����ݳ���
	// ����  : int cZip
	// ����  : ͨѶƽ̨Ͷ�ݣ�Ҫ����յ���ͨѶƽ̨Ӧ��
	DREBAPI_EXPORT int BFDREBCLIENT_API_DPPost(void *pClass,int hook, int serial, DREB_D_NODEINFO dest, const char *data, int datalen,int cZip);

	// ������: DPPush
	// ���  : ������ 2009-12-2 15:24:13
	// ����  : int <0����ʧ�ܣ�>=���͵����ݳ���
	// ����  : void *pClass    �ͻ�����ָ��
	// ����  : int hook
	// ����  : int serial
	// ����  : DREB_D_NODEINFO dest
	// ����  : const char *data  ��ӦcZip������(���ļ��ܻ�ѹ��)
	// ����  : int datalen ��ӦcZip�����ݳ���
	// ����  : int cZip
	// ����  : ͨѶƽ̨���ͣ�����Ӧ��
	DREBAPI_EXPORT int BFDREBCLIENT_API_DPPush(void *pClass,int hook, int serial, DREB_D_NODEINFO dest, const char *data, int datalen,int cZip);

	// ������: DPACall
	// ���  : ������ 2009-12-2 15:24:06
	// ����  : int =100�ɹ� >0��<100dreb���ش� <0����ʧ�� =0δ�յ�ȷ����Ϣ
	// ����  : void *pClass    �ͻ�����ָ��
	// ����  : int hook
	// ����  : int serial
	// ����  : DREB_D_NODEINFO dest
	// ����  : const char *data  ��ӦcZip������(���ļ��ܻ�ѹ��)
	// ����  : int datalen ��ӦcZip�����ݳ���
	// ����  : int cZip
	// ����  : ͨѶƽ̨�첽���� Ҫ����յ���ͨѶƽ̨�����������ȷ��Ӧ��
	DREBAPI_EXPORT int BFDREBCLIENT_API_DPACall(void *pClass,int hook, int serial, DREB_D_NODEINFO dest, const char *data, int datalen,int cZip);

	// ������: DPCall
	// ���  : ������ 2009-12-2 15:23:40
	// ����  : int >0<DREBHEADLENͨѶƽ̨���ش� >DREBHEADLEN�յ������ݳ��� =0��ָ��ʱ����δ�յ�����  <0ʧ�� -100δ���� -1���ӶϿ� -2ͨѶƽ̨Ӧ�����ݲ��� -3CRC����
	// ����  : void *pClass    �ͻ�����ָ��
	// ����  : int hook
	// ����  : int serial
	// ����  : DREB_D_NODEINFO dest
	// ����  : const char *data  ��ӦcZip������(���ļ��ܻ�ѹ��)
	// ����  : int datalen ��ӦcZip�����ݳ���
	// ����  : int cZip
	// ����  : DREB_HEAD &head
	// ����  : char *ansdata   ��Ӧhead->cZip�⿪�����������,�Զ������
	// ����  : ͨѶƽ̨ͬ������ Ҫ�����մ�����ɺ�Ӧ��
	DREBAPI_EXPORT int BFDREBCLIENT_API_DPCall(void *pClass,int hook,int serial,DREB_D_NODEINFO dest, const char *data, int datalen,int cZip,DREB_HEAD &head,char *ansdata);

	// ������: IsConnect
	// ���  : ������ 2009-12-2 15:13:30
	// ����  : int    1����  0δ���� 
	// ����  : void *pClass    �ͻ�����ָ��
	// ����  : �Ƿ�����
	DREBAPI_EXPORT int BFDREBCLIENT_API_IsConnect(void *pClass);

	// ������: Receve
	// ���  : ������ 2009-11-26 16:16:19
	// ����  : int >0ʵ���յ������ݳ��� =0��ָ��ʱ����δ�յ�����  <0ʧ�� -100δ���� -1���ӶϿ� -2ͨѶƽ̨Ӧ�����ݲ��� -3CRC����
	// ����  : void *pClass    �ͻ�����ָ��
	// ����  : DREB_HEAD &head  ͨѶƽ̨ͨѶͷ 
	// ����  : char *data       ���յ������� ��Ӧhead->cZip�⿪�����������,����Ϊhead.nLen,�Զ������
	// ����  : int timeout      �������ݳ�ʱʱ��
	// ����  : �������ݣ�ע�ⷵ��ֵ���ܺ�head.nLen���ȣ�head.nLen�ǽ⿪�����������
	DREBAPI_EXPORT int BFDREBCLIENT_API_ReceveData(void *pClass,DREB_HEAD &head ,unsigned char *data,unsigned int &datalen,int timeout);


	// ������: Close
	// ���  : ������ 2009-11-26 16:16:10
	// ����  : void 
	// ����  : void *pClass    �ͻ�����ָ��
	// ����  : �ر�����
	DREBAPI_EXPORT void BFDREBCLIENT_API_Close(void *pClass);

	// ������: Connect
	// ���  : ������ 2009-11-26 16:15:58
	// ����  : int   0�ɹ� �������ɹ� 
	// ����  : void *pClass    �ͻ�����ָ��
	// ����  : ����ͨѶƽ̨
	DREBAPI_EXPORT int BFDREBCLIENT_API_Connect(void *pClass);

	// ������: Init
	// ���  : ������ 2009-11-26 14:45:45
	// ����  : int  0�ɹ�  ����ʧ��
	// ����  : void *pClass    �ͻ�����ָ��
	// ����  : const  char *ip Ҫ���ӵ�ͨѶƽ̨ip
	// ����  : short port Ҫ���ӵ�ͨѶƽ̨�˿�
	// ����  : int   timeout ���ճ�ʱ
	// ����  : �������ӵ�ͨѶƽ̨����
	DREBAPI_EXPORT int BFDREBCLIENT_API_Init(void *pClass,const  char *ip,short port,int timeout);


	// ������: SendData
	// ���  : ������ 2009-11-26 16:16:28
	// ����  : int <0����ʧ�� -1���ͳ��� -2����̫�� >0 Ϊdatalen��ֵ
	// ����  : void *pClass    �ͻ�����ָ��
	// ����  : int hook    �ͻ���ʹ�õ��ֶΣ�ͨѶƽ̨���ã�ԭ�����ء�
	// ����  : int serial  �첽ʱ��д����ˮ��ͨѶƽ̨���ã�ԭ������
	// ����  : int gateindex    
	// ����  : int cmd   ������ CMD_DPCALL CMD_DPACALL CMD_DPBC CMD_DPABC CMD_DPPUSH CMD_DPPOST
	// ����  : DREB_D_NODEINFO dest  Ŀ����Ϣ  
	// ����  : char *data  ����
	// ����  : int datalen ���ݳ���
	// ����  : int cZip 0�������� 13DES����,����16λ���� 2ѹ�� 3DES����,����8λ���� 4DES���ܲ�ѹ��
	// ����  : int nextno  ���������
	// ����  : ��������
	DREBAPI_EXPORT int BFDREBCLIENT_API_SendData(void *pClass,int hook,int serial,int gateindex,int cmd,DREB_D_NODEINFO dest,const char *data, int datalen,int cZip,int nextno=0);


	// ������: BFDREBCLIENT_API_NewApi
	// ���  : ������ 2013-12-4 14:20:53
	// ����  : int 0�ɹ� <0ʧ��
	// ����  : void **pClass  ���ص�����������ָ��
	// ����  : ��ʼ��api���õ�������ָ��
	DREBAPI_EXPORT	int BFDREBCLIENT_API_NewApi(void **pClass);
	
		
	DREBAPI_EXPORT	void BFDREBCLIENT_API_FreeApi(void *pClass);
}


#endif // !defined(AFX_DREBCLIENTAPI_H__32520F7E_77D8_468D_A23F_0945004C9602__INCLUDED_)
