// BF_SocketTcp.h: interface for the CBF_SocketTcp class.
//
//////////////////////////////////////////////////////////////////////

/***********************************************************************************
  ������־��¼
  ������: ������
  �������� 2012-4-17 14:07:33 �ع�
  �汾: V1.00 
  ˵��: tcpͨѶ��  ����ʱҪ���� -D_REENTRANT
 ***********************************************************************************/


#if !defined(AFX_BF_SOCKETTCP_H__99507094_1B43_494E_9D1C_573498387EC3__INCLUDED_)
#define AFX_BF_SOCKETTCP_H__99507094_1B43_494E_9D1C_573498387EC3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "BF_Socket.h"




#ifdef KERNEL_EXPORTS
#define KERNEL_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define KERNEL_EXPORT __declspec(dllimport)
#else
#define KERNEL_EXPORT 
#endif
#endif

#define  BF_SOCKET_CLIENT        0    //��Ϊ�ͻ���    
#define  BF_SOCKET_SERVER        1    //��Ϊ����˵�����
#define  BF_SOCKET_ATTACHBIND    2    //��Ϊ����˽������ӵ��Ŀͻ��ˣ�����󶨣������ر�socket
#define  BF_SOCKET_ATTACH        3    //��Ϊ����˽������ӵ��Ŀͻ��ˣ����಻��
#define  BF_SOCKET_ATTACH_UNKNOW 9    //δ֪����ʼ״̬

class KERNEL_EXPORT CBF_SocketTcp  
{
public:

	// ������: GetConnectStatus
	// ���  : ������ 2012-5-11 16:11:15
	// ����  : bool 
	// ����  : �������ӵ�״̬
	bool GetConnectStatus();

	// ������: Send
	// ���  : ������ 2012-4-24 12:48:35
	// ����  : int <0����ʧ�ܣ�Ҫ�ر�������   >=0���͵����ݳ���  ==0����������ԭ����ɣ�������ر����ӣ��ط�����
	// ����  : const char *databuf  Ҫ���͵�����
	// ����  : unsigned int datalen  Ҫ���͵����ݳ���
	// ����  : ����ָ���������ݣ�������̫�󣬿������������֡���ͣ�����ٶ���������
	int Send(const char *databuf,unsigned int datalen);

	// ������: SendFrame
	// ���  : ������ 2012-4-24 12:40:35
	// ����  : int <0����ʧ�ܣ�Ҫ�ر�������,���ܷ��ͳɹ�һ���֣�datalen���ѷ������ݳ��� >=0���͵����ݳ��� ����Ҫ���͵ĳ��Ȳ��ȿ���������ԭ����ɣ�������ر����ӣ��ط�ʣ�µļ���
	// ����  : const char *databuf   Ҫ���͵�����
	// ����  : unsigned int &datalen   Ҫ���͵����ݳ���/�ѷ������ݳ���
	// ����  : ��֡�������ݣ�Ŀǰ�������Ϊ8192��������ֵ�ֿ�����
	int SendFrame(const char *databuf,unsigned int &datalen);

	// ������: Receive
	// ���  : ������ 2012-4-24 11:19:51
	// ����  : int <0����ʧ��,���Թر������� =0δ�յ��κ�����  >0�յ������ݳ���
	// ����  : char *buf
	// ����  : unsigned int bufsize  �����С
	// ����  : �������������壬ע����Receive֮ǰҪ����Ƿ��ж��¼�����������в��ܵ��ã���Ϊ�Է������ص�ʱ��recv��������0����ʱ�˺�������-1
	int Receive(char *buf,unsigned int bufsize);

	// ������: Receive
	// ���  : ������ 2012-4-24 11:08:27
	// ����  : int <0����ʧ��,���Թر�������,�������յ�����   =0��ʱδ�յ��κ�����   >0�յ������ݳ���
	// �������  : char *buf ��������
	// ����  : unsigned int &wantlen  ����ΪҪ�յ����ݳ��ȣ����Ϊʵ���յ������ݳ���
	// ����  : unsigned int tms  ��ʱʱ��ms
	// ����  : �ڹ涨ʱ������ȡָ�����ȵ�����
	int Receive(char *buf,unsigned int &wantlen,unsigned int tms);

	// ������: GetReceiveDataLen
	// ���  : ������ 2012-4-23 16:57:09
	// ����  : int =0 �����ݿɶ� -1�����ݿɶ��������ݳ���Ϊ0�������ѶϿ� -2�����ˣ������ѶϿ�
	// ����  : �õ�socket��Ҫ�յ����ݳ��ȣ�>0�����ݵ���׼ȷ
	int GetReceiveDataLen();

	// ������: CheckWriteEvent
	// ���  : ������ 2012-4-23 16:29:54
	// ����  : int <0�����ˣ����������ѶϿ� 0����д 1��д
	// ����  : unsigned int tms  ��ʱms
	// ����  : ���socket�Ƿ��д
	int CheckWriteEvent(unsigned int tms);

	// ������: CheckReadEvent
	// ���  : ������ 2012-4-23 16:29:23
	// ����  : int <0�����ˣ����������ѶϿ� 0���ɶ� 1�ɶ�
	// ����  : unsigned int tms  ��ʱms
	// ����  : ���socket�Ƿ�ɶ�
	int CheckReadEvent(unsigned int tms);

	// ������: CheckConnect
	// ���  : ������ 2012-4-23 16:06:51
	// ����  : bool 
	// ����  : unsigned int tms  ��ʱms
	// ����  : ����������ʽ�����Ƿ��������ô˷���ǰҪ�ж�m_nErrNo�Ƿ�Ϊ EWOULDBLOCK��EINPROGRESS
	bool CheckConnect(unsigned int tms);

	// ������: ConnectServer
	// ���  : ������ 2012-4-23 15:06:25
	// ����  : bool 
	// ����  : const char *host      ����IP
	// ����  : unsigned short port   ����˿�
	// ����  : unsigned int tms=0   =0����ʽ���ӣ�  >0������ʽ���ӳ�ʱms,��������Ϊ��������ʽ ע����WINDOWS����СΪ10ms
	// ����  : ��Ϊ�ͻ������ӷ����  ��ʱΪ0ʱ������������socketΪ������ʽ��Ҫ��Createָ�������SetBlockFlag
	bool ConnectServer(const char *host ,unsigned short port,unsigned int tms=0);


	// ������: Connect
	// ���  : ������ 2012-5-17 11:35:22
	// ����  : bool 
	// ����  : SOCKADDR_IN iaddr  �����ַ���˿�
	// ����  : ��Ϊ�ͻ������ӷ����  ������������socketΪ������ʽ��Ҫ��Createָ�������SetBlockFlag
	bool Connect(SOCKADDR_IN iaddr);

	// ������: SetBlockFlag
	// ���  : ������ 2009-5-8 10:27:12
	// ����  : void 
	// ����  : bool isblock  true����ģʽ  false������ģʽ 
	// ����  : ����socket������ģʽ  
	void SetBlockFlag(bool isblock);

	// ������: AttachBindSocket
	// ���  : ������ 2012-4-23 12:55:42
	// ����  : void 
	// ����  : SOCKET_HANDLE socket
	// ����  : const char *clientip
	// ����  : Ϊ��������һ��socket���ӣ�ʹ֮�����շ����ݵȣ�����ʱ�ر�����
	void AttachBindSocket(SOCKET_HANDLE socket,const char *clientip);

	// ������: AttachSocket
	// ���  : ������ 2012-4-23 12:55:42
	// ����  : void 
	// ����  : SOCKET_HANDLE socket
	// ����  : const char *clientip
	// ����  : Ϊ��������һ��socket���ӣ�ʹ֮�����շ����ݵ�
	void AttachSocket(SOCKET_HANDLE socket,const char *clientip);

	// ������: Accept
	// ���  : ������ 2005-3-20 10:52:28
	// ����  : int >0 socket���ӱ�ʶ  <0accept�������������ѹر�
	// �������  : char* lpaddress  //���ӹ����Ŀͻ���IP
	// �������  : bool blocked=true  true����ģʽ false������ģʽ
	// ����  : ����Զ�̵��������������ӵ�����ģʽ
	SOCKET_HANDLE Accept(char* lpaddress,bool blocked=true);


	// ������: Accept
	// ���  : ������ 2012-5-22 10:55:11
	// ����  : SOCKET_HANDLE >0 socket���ӱ�ʶ  <0accept�������������ѹر�
	// ����  : SOCKADDR_IN &iaddr   �ͻ��˵�IP���˿�   ����AF_UNIX��Ϊ127.0.0.1�����������˿�
	// ����  : bool blocked=true    true����ģʽ false������ģʽ
	// ����  : ����Զ�̵��������������ӵ�����ģʽ 
	SOCKET_HANDLE Accept(SOCKADDR_IN &iaddr, bool blocked=true);

	// ������: Listen
	// ���  : ������ 2012-4-23 12:31:42
	// ����  : bool 
	// ����  : unsigned short port
	// ����  : unsigned int queuenum=5
	// ����  : ��ʼ���������������Create
	bool Listen(unsigned short port,unsigned int queuenum=5,const char *localip=NULL);
	

	// ������: Close
	// ���  : ������ 2012-4-23 12:29:17
	// ����  : void 
	// ����  : �ر����� ����BF_SOCKET_CLIENT��m_bConnected=false
	void Close();

	// ������: GetPort
	// ���  : ������ 2012-4-23 12:28:55
	// ����  : unsigned short 
	// ����  : ȡ�˿���Ϣ BF_SOCKET_CLIENT:����������˿�  BF_SOCKET_SERVER:�����˿�        
	unsigned short GetPort();

	// ������: GetAddress
	// ���  : ������ 2012-4-23 12:27:19
	// ����  : std::string 
	// ����  : ȡIP��ַ��Ϣ BF_SOCKET_CLIENT:�����IP   BF_SOCKET_SERVER:127.0.0.1  BF_SOCKET_ATTACH/BF_SOCKET_ATTACHBIND: �ͻ������ӹ�����IP
	std::string GetAddress();

	// ������: GetSocketType
	// ���  : ������ 2012-4-23 11:44:39
	// ����  : unsigned int 
	// ����  : ��ȡsocket���� BF_SOCKET_CLIENT  BF_SOCKET_SERVER BF_SOCKET_ATTACH BF_SOCKET_ATTACHBIND
	unsigned int GetSocketType();
	
	// ������: GetSocketMode
	// ���  : ������ 2012-4-23 11:39:05
	// ����  : int 
	// ����  : ����ͨѶģʽAF_INET��AF_UNIX
	int GetSocketMode();

	// ������: GetErrorMsg
	// ���  : ������ 2012-4-23 11:16:14
	// ����  : std::string 
	// ����  : ����socket������Ϣ
	std::string GetErrorMsg();

	// ������: GetErrorNo
	// ���  : ������ 2012-4-23 11:15:06
	// ����  : int  ����h_errno(WINDOWS)��errno(UNIX)
	// ����  : ȡ�ô����
	int GetErrorNo();

	// ������: GetSocket
	// ���  : ������ 2012-4-23 11:13:44
	// ����  : SOCKET_HANDLE 
	// ����  : ���ر����SOCKET
	SOCKET_HANDLE GetSocket();

	// ������: Create
	// ���  : ������ 2012-4-23 10:45:51
	// ����  : bool 
	// ����  : int AddressFamily=AF_INET  ͨѶģʽ AF_INET  AF_UNIX
	// ����  : bool blocked=true          true����ģʽfalse������ģʽ
	// ����  : ����socket
	bool Create(int AddressFamily=AF_INET,bool blocked=true);

	CBF_SocketTcp(int addressFamily=AF_INET);
	virtual ~CBF_SocketTcp();

protected:

	
	SOCKET_HANDLE   m_socket;  //socket���ӱ�ʶ
	int             m_nErrno; //socket errno
	char            m_sErrMsg[300]; //������Ϣ
	int             m_nAddressFamily; //ͨѶģʽ��������ģʽ����UNIX DOMAIN sockģʽ
	
	unsigned int    m_nSocketType;   //socket����  BF_SOCKET_CLIENT BF_SOCKET_SERVER BF_SOCKET_ATTACH BF_SOCKET_ATTACHBIND
	char            m_sAddress[80];//BF_SOCKET_CLIENT:�����IP   BF_SOCKET_SERVER:127.0.0.1  BF_SOCKET_ATTACH/BF_SOCKET_ATTACHBIND: �ͻ������ӹ�����IP
	unsigned short  m_nPort;    //BF_SOCKET_CLIENT:����������˿�  BF_SOCKET_SERVER:�����˿�        
	bool            m_bConnected;  //���ͻ���BF_SOCKET_CLIENT�Ƿ����ӳɹ�    
	
protected:

	// ������: Connect
	// ���  : ������ 2012-4-23 14:58:05
	// ����  : bool 
	// ����  : ���ӷ���� ʹ��m_sAddress��m_nPort
	bool Connect();

	// ������: GetLastErrorInfo
	// ���  : ������ 2012-4-23 11:04:55
	// ����  : void 
	// ����  : ��ȡ������Ϣ��m_errno��m_errMsg��
	void GetLastErrorInfo();

	unsigned int  m_nSendBlockSize;//ÿ�η������ݿ�Ĵ�С
};

#endif // !defined(AFX_BF_SOCKETTCP_H__99507094_1B43_494E_9D1C_573498387EC3__INCLUDED_)
