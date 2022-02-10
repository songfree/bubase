// DrebClient.h: interface for the CDrebClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DrebClient_H__CDCD9144_228C_44B3_ABB0_373102494D80__INCLUDED_)
#define AFX_DrebClient_H__CDCD9144_228C_44B3_ABB0_373102494D80__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DrebPubDefine.h"
#include "DrebEndian.h"
#include "dreberrcode.h"
#include "EncryptCommBuffer.h"
#include "BF_SimpleLog.h"


#ifdef _WINDOWS
#ifdef _DEBUG
#pragma comment(lib, "bf_kerneld.lib") 
#pragma comment(lib, "bf_dbpubd.lib") 
#pragma message("Automatically linking with  bf_dbpubd.lib bf_kerneld.lib ")
#else
#pragma comment(lib, "bf_kernel.lib") 
#pragma comment(lib, "bf_dbpub.lib") 
#pragma message("Automatically linking with  bf_dbpub.lib bf_kernel.lib ")
#endif

#endif

//��ͨѶƽ̨�ͻ���API
#ifdef DREBAPI_EXPORTS
#define DREBAPI_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define DREBAPI_EXPORT __declspec(dllimport)
#else
#define DREBAPI_EXPORT 
#endif
#endif




class DREBAPI_EXPORT CDrebClient  
{
public:
	CDrebClient();
	virtual ~CDrebClient();
public:


	// ������: Monitor_Dreb
	// ���  : ������ 2009-12-2 15:23:40
	// ����  : int >0<DREBHEADLENͨѶƽ̨���ش� >DREBHEADLEN�յ������ݳ��� =0��ָ��ʱ����δ�յ�����  <0ʧ�� -100δ���� -1���ӶϿ� -2ͨѶƽ̨Ӧ�����ݲ��� -3CRC����
	// ����  : int hook
	// ����  : int serial
	// ����  : DREB_D_NODEINFO dest
	// ����  : const char *data  ��ӦcZip������(���ļ��ܻ�ѹ��)
	// ����  : int datalen ��ӦcZip�����ݳ���
	// ����  : int cZip
	// ����  : DREB_HEAD &head
	// ����  : char *ansdata   ��Ӧhead->cZip�⿪�����������,�Զ������
	// ����  : ͨѶƽ̨��� Ҫ�����մ�����ɺ�Ӧ��
	int Monitor_Dreb(int hook,int serial,DREB_D_NODEINFO dest, const char *data, int datalen,int cZip,DREB_HEAD &head,char *ansdata);

	// ������: Monitor_Bpc
	// ���  : ������ 2009-12-2 15:23:40
	// ����  : int >0<DREBHEADLENͨѶƽ̨���ش� >DREBHEADLEN�յ������ݳ��� =0��ָ��ʱ����δ�յ�����  <0ʧ�� -100δ���� -1���ӶϿ� -2ͨѶƽ̨Ӧ�����ݲ��� -3CRC����
	// ����  : int hook
	// ����  : int serial
	// ����  : DREB_D_NODEINFO dest
	// ����  : const char *data  ��ӦcZip������(���ļ��ܻ�ѹ��)
	// ����  : int datalen ��ӦcZip�����ݳ���
	// ����  : int cZip
	// ����  : DREB_HEAD &head
	// ����  : char *ansdata   ��Ӧhead->cZip�⿪�����������,�Զ������
	// ����  : BPC��� Ҫ�����մ�����ɺ�Ӧ��
	int Monitor_Bpc(int hook,int serial,DREB_D_NODEINFO dest, const char *data, int datalen,int cZip,DREB_HEAD &head,char *ansdata);

	// ������: GetDrebHead
	// ���  : ������ 2010-9-28 9:18:45
	// ����  : void 
	// ����  : DREB_HEAD &head
	// ����  : ��ȡ����ʱ���ص�drebͷ
	void GetDrebHead(DREB_HEAD &head);

	// ������: DPABC
	// ���  : ������ 2009-12-2 15:25:00
	// ����  : int <0����ʧ�ܣ�>=���͵����ݳ���
	// ����  : int hook
	// ����  : int serial
	// ����  : DREB_D_NODEINFO dest
	// ����  : const char *data  ��ӦcZip������(���ļ��ܻ�ѹ��)
	// ����  : int datalen ��ӦcZip�����ݳ���
	// ����  : int cZip
	// ����  : ͨѶƽ̨�㲥��������Ϣ����ָ��ͨѶƽ̨������ע��ķ���,��Ҫ��ͨѶƽ̨Ӧ��
	int DPABC(int hook, int serial,int gateindex,DREB_D_NODEINFO dest, const char *data, int datalen,int cZip,int nextno=0);

	// ������: DPBC
	// ���  : ������ 2009-12-2 15:24:42
	// ����  : int =100�ɹ� >0��<100dreb���ش� <0����ʧ�� =0δ�յ�ȷ����Ϣ
	// ����  : int hook
	// ����  : int serial
	// ����  : DREB_D_NODEINFO dest
	// ����  : const char *data  ��ӦcZip������(���ļ��ܻ�ѹ��)
	// ����  : int datalen ��ӦcZip�����ݳ���
	// ����  : int cZip
	// ����  : ͨѶƽ̨�㲥��������Ϣ����ָ��ͨѶƽ̨������ע��ķ���,Ҫ��ͨѶƽ̨Ӧ��
	int DPBC(int hook, int serial,DREB_D_NODEINFO dest, const char *data, int datalen,int cZip);

	// ������: DPPost
	// ���  : ������ 2009-12-2 15:24:28
	// ����  : int =100�ɹ� >0��<100dreb���ش� <0����ʧ�� =0δ�յ�ȷ����Ϣ
	// ����  : int hook
	// ����  : int serial
	// ����  : DREB_D_NODEINFO dest
	// ����  : const char *data  ��ӦcZip������(���ļ��ܻ�ѹ��)
	// ����  : int datalen ��ӦcZip�����ݳ���
	// ����  : int cZip
	// ����  : ͨѶƽ̨Ͷ�ݣ�Ҫ����յ���ͨѶƽ̨Ӧ��
	int DPPost(int hook, int serial, DREB_D_NODEINFO dest, const char *data, int datalen,int cZip);

	// ������: DPPush
	// ���  : ������ 2009-12-2 15:24:13
	// ����  : int <0����ʧ�ܣ�>=���͵����ݳ���
	// ����  : int hook
	// ����  : int serial
	// ����  : DREB_D_NODEINFO dest
	// ����  : const char *data  ��ӦcZip������(���ļ��ܻ�ѹ��)
	// ����  : int datalen ��ӦcZip�����ݳ���
	// ����  : int cZip
	// ����  : ͨѶƽ̨���ͣ�����Ӧ��
	int DPPush(int hook, int serial, DREB_D_NODEINFO dest, const char *data, int datalen,int cZip);

	// ������: DPACall
	// ���  : ������ 2009-12-2 15:24:06
	// ����  : int =100�ɹ� >0��<100dreb���ش� <0����ʧ�� =0δ�յ�ȷ����Ϣ
	// ����  : int hook
	// ����  : int serial
	// ����  : DREB_D_NODEINFO dest
	// ����  : const char *data  ��ӦcZip������(���ļ��ܻ�ѹ��)
	// ����  : int datalen ��ӦcZip�����ݳ���
	// ����  : int cZip
	// ����  : ͨѶƽ̨�첽���� Ҫ����յ���ͨѶƽ̨�����������ȷ��Ӧ��
	int DPACall(int hook, int serial, DREB_D_NODEINFO dest, const char *data, int datalen,int cZip);

	// ������: DPCall
	// ���  : ������ 2009-12-2 15:23:40
	// ����  : int >0<DREBHEADLENͨѶƽ̨���ش� >DREBHEADLEN�յ������ݳ��� =0��ָ��ʱ����δ�յ�����  <0ʧ�� -100δ���� -1���ӶϿ� -2ͨѶƽ̨Ӧ�����ݲ��� -3CRC����
	// ����  : int hook
	// ����  : int serial
	// ����  : DREB_D_NODEINFO dest
	// ����  : const char *data  ��ӦcZip������(���ļ��ܻ�ѹ��)
	// ����  : int datalen ��ӦcZip�����ݳ���
	// ����  : int cZip
	// ����  : DREB_HEAD &head
	// ����  : char *ansdata   ��Ӧhead->cZip�⿪�����������,�Զ������
	// ����  : ͨѶƽ̨ͬ������ Ҫ�����մ�����ɺ�Ӧ��
	int DPCall(int hook,int serial,DREB_D_NODEINFO dest, const char *data, int datalen,int cZip,DREB_HEAD &head,char *ansdata);

	// ������: IsConnect
	// ���  : ������ 2009-12-2 15:13:30
	// ����  : bool 
	// ����  : �Ƿ�����
	bool IsConnect();

	// ������: Receve
	// ���  : ������ 2009-11-26 16:16:19
	// ����  : int >0ʵ���յ������ݳ��� =0��ָ��ʱ����δ�յ�����  <0ʧ�� -100δ���� -1���ӶϿ� -2ͨѶƽ̨Ӧ�����ݲ��� -3CRC����
	// ����  : DREB_HEAD &head  ͨѶƽ̨ͨѶͷ 
	// ����  : char *data       ���յ������� ��Ӧhead->cZip�⿪�����������,����Ϊhead.nLen,�Զ������
	// ����  : int timeout      �������ݳ�ʱʱ��
	// ����  : �������ݣ�ע�ⷵ��ֵ���ܺ�head.nLen���ȣ�head.nLen�ǽ⿪�����������
	int ReceveData(DREB_HEAD &head ,unsigned char *data,unsigned int &datalen,int timeout);


	// ������: Close
	// ���  : ������ 2009-11-26 16:16:10
	// ����  : void 
	// ����  : �ر�����
	void Close();

	// ������: Connect
	// ���  : ������ 2009-11-26 16:15:58
	// ����  : bool 
	// ����  : ����ͨѶƽ̨
	bool Connect();

	// ������: Init
	// ���  : ������ 2009-11-26 14:45:45
	// ����  : bool 
	// ����  : const  char *ip Ҫ���ӵ�ͨѶƽ̨ip
	// ����  : short port Ҫ���ӵ�ͨѶƽ̨�˿�
	// ����  : int   timeout ���ճ�ʱ ����
	// ����  : �������ӵ�ͨѶƽ̨����
	bool Init(const  char *ip,short port,int timeout);


	// ������: ZipBuf
	// ���  : ������ 2010-9-28 9:49:30
	// ����  : bool 
	// ����  : int IsZipEncrypt   0�������� 13DES����,����16λ���� 2ѹ�� 3DES����,����8λ���� 4DES���ܲ�ѹ��
	// ����  : char *buf
	// ����  : int &buflen
	// ����  : ����ѹ�� 0-4Ϊ�Ѷ���  ����Ҫ�Զ��壬��Ҫ���ñ�������
//	bool ZipBuf(int IsZipEncrypt,char *buf,int &buflen);

	// ������: UnzipBuf
	// ���  : ������ 2010-9-28 9:49:27
	// ����  : bool 
	// ����  : int IsZipEncrypt 0�������� 13DES����,����16λ���� 2ѹ�� 3DES����,����8λ���� 4DES���ܲ�ѹ��
	// ����  : char *buf
	// ����  : int &buflen
	// ����  : ����ܽ�ѹ�� 0-4Ϊ�Ѷ���  ����Ҫ�Զ��壬��Ҫ���ñ�������
//	bool UnzipBuf(int IsZipEncrypt, char *buf,int &buflen);

	// ������: SendData
	// ���  : ������ 2009-11-26 16:16:28
	// ����  : int <0����ʧ�� -1���ͳ��� -2����̫�� >0 Ϊdatalen��ֵ
	// ����  : int hook    �ͻ���ʹ�õ��ֶΣ�ͨѶƽ̨���ã�ԭ�����ء�
	// ����  : int serial  �첽ʱ��д����ˮ��ͨѶƽ̨���ã�ԭ������
	// ����  : int cmd   ������ CMD_DPCALL CMD_DPACALL CMD_DPBC CMD_DPABC CMD_DPPUSH CMD_DPPOST
	// ����  : DREB_D_NODEINFO dest  Ŀ����Ϣ  
	// ����  : char *data  ����
	// ����  : int datalen ���ݳ���
	// ����  : int cZip 0�������� 13DES����,����16λ���� 2ѹ�� 3DES����,����8λ���� 4DES���ܲ�ѹ��
	// ����  : int nextno  ���������
	// ����  : ��������
	int SendData(int hook,int serial,int gateindex,int cmd,DREB_D_NODEINFO dest,const char *data, int datalen,int cZip,int nextno=0);

	char          m_errMsg[512];

	CBF_SimpleLog m_pLog;
protected:
	CEncryptCommBuffer m_pDesZip;

	DREB_HEAD     m_head; //ͨѶͷ,����ʱ�����ģ�һֱ����
	CBF_SocketTcp    m_tcpSocket;
	CDrebEndian    m_pCheckHead;
	std::string   m_sIp;//Ҫ���ӵ�ͨѶƽ̨ip
	short         m_nPort;//Ҫ���ӵ�ͨѶƽ̨�˿�
	int           m_nTimeOut;//���ճ�ʱ
	bool          m_bIsInit;
	bool          m_bIsConnect;//�Ƿ�������
	
	char          m_sRcvBuffer[DREBBUFSIZE];
	unsigned  int m_nRcvLen;
	char          m_sSendBuffer[DREBBUFSIZE];

	unsigned char m_tmpdata[DREBBUFSIZE];
	unsigned int  m_tmpdatalen;

};

#endif // !defined(AFX_DrebClient_H__CDCD9144_228C_44B3_ABB0_373102494D80__INCLUDED_)
