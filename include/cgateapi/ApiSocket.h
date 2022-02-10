// ApiSocket.h: interface for the CApiSocket class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_APISOCKET_H__9C4C1318_7B57_4F18_A6C4_E3090A0DEEE6__INCLUDED_)
#define AFX_APISOCKET_H__9C4C1318_7B57_4F18_A6C4_E3090A0DEEE6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GateHead.h"
#include "SendQueue.h"
#include "GlobalResource.h"
#include "DrebEndian.h"
#include "BF_Tools.h"
#include "BF_Des.h"
#include "PoolData.h"


#ifdef CGATEAPI_EXPORTS
#define CGATEAPI_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define CGATEAPI_EXPORT __declspec(dllimport)
#else
#define CGATEAPI_EXPORT 
#endif
#endif

class CGATEAPI_EXPORT CApiSocket  
{
public:

	// ������: GetRcvDataLen
	// ���  : ������ 2013-6-15 14:19:21
	// ����  : int 
	// ����  : ȡ�յ������ݳ���
	int GetRcvDataLen();

	// ������: Pki
	// ���  : ������ 2013-5-30 14:49:29
	// ����  : bool 
	// ����  : PKI������֤
	bool Pki();

	SOCKET_HANDLE GetSocket();

	// ������: ConnectGate
	// ���  : ������ 2013-5-29 14:42:13
	// ����  : bool 
	// ����  : ��������
	bool ConnectGate();

	// ������: OnRcv
	// ���  : ������ 2013-5-28 10:49:16
	// ����  : int ������С��0���ʾ���ӹرգ�С��ͨѶͷ��Ҫ�����գ�����ͨѶͷ����ȡ���ݽ��д�����
	// ����  : �������ݣ���select�̵߳���
	int OnRcv();


	// ������: GetRecvData
	// ���  : ������ 2013-5-28 11:50:47
	// ����  : int ������С��0���ʾ���ӹرգ�=0δ��������������0�յ����ݰ�
	// ����  : CGATE_COMMSTRU *msg
	// ����  : ȡ�ý��յĽ�������
	int GetRecvData(CGATE_COMMSTRU *msg);
	
	// ������: SetPara
	// ���  : ������ 2013-5-27 15:59:44
	// ����  : void 
	// ����  : CGlobalResource *res
	// ����  : ���ò���
	void SetPara(CGlobalResource *res);

	// ������: OnClose
	// ���  : ������ 2013-5-27 15:45:50
	// ����  : void 
	// ����  : const char *filename
	// ����  : int fileline
	// ����  : const char *msg
	// ����  : int loglevel
	// ����  : �ر�����
	void OnClose(const char *filename,int fileline,const char *msg,int loglevel);

	// ������: OnSend
	// ���  : ������ 2013-5-27 14:45:15
	// ����  : int 
	// ����  : ��socket��дʱ���������Ͷ���������ݣ���select�̵߳���
//	int OnSend();

	// ������: Send
	// ���  : ������ 2013-5-27 14:38:52
	// ����  : int  -1 δ�������� -2���ͳ���  1���Ͳ������ݣ��ر����� 2�������
	// ����  : PCGATE_COMMSTRU data
	// ����  : ���Ͷ��У���δ��ȫ���ͻ���ʧ���������У���socket��дʱ�ٷ���  ���û�����
	int Send2Gate(PCGATE_COMMSTRU data);

	
	CPoolData       m_pAnsQueue;//Ӧ�����
	
	CPoolData       m_pReqQueue;//���Ͷ���


	CApiSocket();
	virtual ~CApiSocket();
	
	int    m_nSendTime; //����ʱ�� ��
	int    m_nRcvTime;  //����ʱ�� ��
	int    m_nPingTime; //����pingʱ��

	unsigned int m_nIndex;//cgate���ص���������

	int          m_nSocketStatus;//����״̬ 0δ���� 1������  

	// ������: GetMsgType
	// ���  : ������ 2013-5-28 10:40:52
	// ����  : std::string 
	// ����  : int type
	// ����  : ȡ������Ϣ����
	std::string GetMsgType(int type);

	CCgateLog      *m_pGateLog;

private:



	// ������: Uncompress
	// ���  : ������ 2012-4-20 9:32:29
	// ����  : bool 
	// �������  : unsigned char* destbuf �⿪�������
	// �����������  : unsigned int * destlen ����Ϊdestbuf�Ĵ�С�����Ϊ�⿪������ݳ���
	// �������  : unsigned char* srbuf  ѹ������
	// �������  : int srbuflen  ѹ�����ݳ���
	// ����  : ��ѹ������
	bool Uncompress(unsigned char *destbuf, unsigned int &destlen, unsigned char *srbuf, int srbuflen);
	
	// ������: Compress
	// ���  : ������ 2012-4-20 9:32:29
	// ����  : bool 
	// �������  : unsigned char * destbuf  ѹ���������
	// �����������  : unsigned int * destlen  ����Ϊdestbuf�Ĵ�С�����Ϊѹ��������ݳ���
	// �������  : unsigned char * srbuf   Ҫѹ��������
	// �������  : int srbuflen Ҫѹ�������ݳ���
	// ����  : ѹ������
	bool Compress(unsigned char *destbuf,unsigned int &destlen,unsigned char *srbuf,int srbuflen);

	std::string GenSignString();

	// ������: Endian2CommCrc
	// ���  : ������ 2013-5-28 10:41:08
	// ����  : void 
	// ����  : CGATE_HEAD &head
	// ����  : �ֽ���ת��ͨѶ��ʽ������crcУ��λ
	void Endian2CommCrc(CGATE_HEAD &head);


	// ������: Endian2HostCrc
	// ���  : ������ 2013-5-28 10:41:02
	// ����  : bool 
	// ����  : CGATE_HEAD &head
	// ����  : ת��Ϊ�����ֽ���У��crc
	bool Endian2HostCrc(CGATE_HEAD &head);

	// ������: ZipBuf
	// ���  : ������ 2013-5-27 21:45:15
	// ����  : bool 
	// ����  : PCGATE_COMMSTRU  data
	// ����  : int nZipFlag=4
	// ����  : ����ѹ������
	bool ZipBuf(PCGATE_COMMSTRU  data,int nZipFlag=4);
	
	// ������: UnzipBuf
	// ���  : ������ 2013-5-27 21:44:59
	// ����  : bool 
	// ����  : CGATE_COMMSTRU &data
	// ����  : ��ѹ����
	bool UnzipBuf(CGATE_COMMSTRU &data);

	// ������: GetDrebCmdType
	// ���  : ������ 2013-5-28 10:40:57
	// ����  : std::string 
	// ����  : int cmdtype
	// ����  : 
	std::string GetDrebCmdType(int cmdtype);

	char           m_sRcvBuffer[DREBBUFSIZE];  //�������ݻ���
	int            m_nRcvBufferLen;//�������ݻ����С
	unsigned char  m_sSendBuffer[DREBBUFSIZE];  //�������ݻ���
	CGATE_HEAD     m_head; //���ձ���ͷ

	char           m_sSessionKey[49];//�Ự��Կ
	char           m_sGatePubSignKey[2049]; //����ǩ����Կ
	unsigned short m_nGatePubSignKeyLen;   //����ǩ����Կ����
	char           m_sGatePubRsaKey[2049]; //���ؼ��ܹ�Կ
	unsigned short m_nGatePubRsaKeyLen;    //���ؼ��ܹ�Կ����

	char           m_sLocalPubSignKey[2049]; //����ǩ����Կ
	unsigned short m_nLocalPubSignKeyLen;   //����ǩ����Կ����
	char           m_sLocalPubRsaKey[2049]; //���ؼ��ܹ�Կ
	unsigned short m_nLocalPubRsaKeyLen;    //���ؼ��ܹ�Կ����


	
	

	CBF_SocketTcp  m_pSocketTcp;//socket������
	
	CDrebEndian    m_pEndian;
	CBF_Des        m_pDes;

	CBF_Des        m_pDesZip;

	CGlobalResource  *m_pRes;
	CIErrlog      *m_pLog;
	

	CBF_Mutex      m_mutex;
	CBF_Mutex      m_mutexCompress;

};

#endif // !defined(AFX_APISOCKET_H__9C4C1318_7B57_4F18_A6C4_E3090A0DEEE6__INCLUDED_)
