// UdpSocketInfo.h: interface for the CUdpSocketInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UDPSOCKETINFO_H__FB03A890_2401_4755_A3CB_D94C6C9A83C9__INCLUDED_)
#define AFX_UDPSOCKETINFO_H__FB03A890_2401_4755_A3CB_D94C6C9A83C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "BF_SocketUdp.h"
#include "TibHead.h"
#include "IErrLog.h"
#include "BF_Mutex.h"

#ifdef TIB_EXPORTS
#define TIB_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define TIB_EXPORT __declspec(dllimport)
#else
#define TIB_EXPORT 
#endif
#endif

class TIB_EXPORT CUdpSocketInfo  
{
public:

	// ������: SetPara
	// ���  : ������ 2015-11-24 9:09:40
	// ����  : void 
	// ����  : unsigned short port
	// ����  : int index
	// ����  : std::string sendaddr
	// ����  : ���ò���
	void SetPara(unsigned short port ,int index,std::string sendaddr);

	// ������: GetIndex
	// ���  : ������ 2015-11-24 9:09:51
	// ����  : int  
	// ����  : ȡ������
	int  GetIndex();

	// ������: GetPort
	// ���  : ������ 2015-11-24 9:09:59
	// ����  : unsigned short 
	// ����  : ȡ�ö˿�
	unsigned short GetPort();

	// ������: GetDataLen
	// ���  : ������ 2015-6-29 17:21:39
	// ����  : int 
	// ����  : ȡ�ý��ջ������ݳ���
	int GetDataLen();


	// ������: GetSocket
	// ���  : ������ 2015-6-29 17:22:00
	// ����  : SOCKET_HANDLE 
	// ����  : ȡ��socket������
	SOCKET_HANDLE GetSocket();

	// ������: SetCommEndian
	// ���  : ������ 2013-4-25 11:13:08
	// ����  : void 
	// ����  : bool islittle true��windows linux��ʹ�õ�������  falseΪ������
	// ����  : ����ͨѶʱ���ֽ��� 
	void SetCommEndian(bool islittle);
	
	// ������: Endian2Comm
	// ���  : ������ 2013-4-25 10:41:58
	// ����  : void 
	// ����  : PS_TIB_HEAD data
	// ����  : ͨѶʱ�����ֽ���תΪͨѶʱ���õ��ֽ���
	void Endian2Comm(PS_TIB_HEAD data);
	
	
	// ������: Endian2Comm
	// ���  : ������ 2013-5-6 14:42:15
	// ����  : void 
	// ����  : unsigned char *data
	// ����  : int datalen
	// ����  : ͨѶʱ�����ֽ���תΪͨѶʱ���õ��ֽ���
	void Endian2Comm(unsigned char *data,int datalen);
	
	
	// ������: Endian2LocalHost
	// ���  : ������ 2013-4-25 10:42:03
	// ����  : void 
	// ����  : PS_TIB_HEAD data
	// ����  : ��ͨѶ�������ֽ���תΪ������ʹ�õ��ֽ���
	void Endian2LocalHost(PS_TIB_HEAD data);
	

	
	
	// ������: Endian2LocalHost
	// ���  : ������ 2013-5-6 14:42:02
	// ����  : void 
	// ����  : unsigned char *data
	// ����  : int datalen
	// ����  : ��ͨѶ�������ֽ���תΪ������ʹ�õ��ֽ���
	void Endian2LocalHost(unsigned char *data,int datalen);
	
	
	// ������: Reverse
	// ���  : ������ 2013-5-6 14:41:45
	// ����  : void 
	// ����  : unsigned char *buf
	// ����  : int len
	// ����  : ���з�ת
	void Reverse(unsigned char *buf, int len);


	// ������: GetRcvData
	// ���  : ������ 2015-6-29 16:11:38
	// ����  : int 
	// ����  : PTIBCOMMSTRU data
	// ����  : ȡ��һ����������
	int GetRcvData(PTIBCOMMSTRU data);

	// ������: Init
	// ���  : ������ 2015-6-29 14:54:30
	// ����  : bool 
	// ����  : CIErrlog *log
	// ����  : ��ʼ��
	bool Init(CIErrlog *log);

	// ������: Send2Bus
	// ���  : ������ 2015-6-29 14:41:32
	// ����  : int 
	// ����  : const char *data
	// ����  : int datalen
	// ����  : ��������
	int OnSend(const char *data,int datalen);

	// ������: OnRecv
	// ���  : ������ 2015-6-29 14:40:37
	// ����  : int  <0���� �رգ����ذ�   >=0Ϊ���ݳ���
	// ����  : ��������
	int OnRecv();

	// ������: OnServer
	// ���  : ������ 2015-6-29 14:40:06
	// ����  : bool 
	// ����  : ��Ϊ�����  create��bind
	bool OnServer();

	// ������: OnClient
	// ���  : ������ 2015-6-29 14:39:45
	// ����  : bool 
	// ����  : ��Ϊ�ͻ��� create��bind
	bool OnClient();

	// ������: OnClose
	// ���  : ������ 2015-6-29 14:26:17
	// ����  : void 
	// ����  : �ر�UDP
	void OnClose();

	CUdpSocketInfo();
	virtual ~CUdpSocketInfo();

	
protected:
	int            m_nIndex;   //����
	unsigned short m_nPort;    //UDP�˿�
	std::string    m_sSendAddr; //UDP���͵�ַ���㲥��ַ

	char      m_sRcvBuffer[UDPRCVBUFSIZE]; //���ջ�����
	unsigned  int m_nRcvBufferLen;      //�������ݳ���

	CBF_SocketUdp   m_pSocket;
	CIErrlog        *m_pLog;   

	CBF_Mutex       m_mutex;

	bool m_bIsLittle;//�Ƿ������� falseΪ������ 
	
	bool m_bUseLittleEndian;//ͨѶ�Ƿ�ʹ��linux,windows�����ֽ��� false��Ϊ������
	            
};

#endif // !defined(AFX_UDPSOCKETINFO_H__FB03A890_2401_4755_A3CB_D94C6C9A83C9__INCLUDED_)
