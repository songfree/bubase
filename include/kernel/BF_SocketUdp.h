// BF_SocketUdp.h: interface for the CBF_SocketUdp class.
//
//////////////////////////////////////////////////////////////////////

/***********************************************************************************
  ������־��¼
  ������: ������
  �������� 2012-4-17 14:07:47 �ع�
  �汾: V1.00 
  ˵��: UdpͨѶ��  ����Ҫô�Ƿ���Ҫô�ǿͻ�������ͬʱ
 ***********************************************************************************/


#if !defined(AFX_BF_SOCKETUDP_H__67745822_F0AF_45DC_97C6_22F4E0255E5D__INCLUDED_)
#define AFX_BF_SOCKETUDP_H__67745822_F0AF_45DC_97C6_22F4E0255E5D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BF_Socket.h"
#include "public.h"

#ifdef KERNEL_EXPORTS
#define KERNEL_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define KERNEL_EXPORT __declspec(dllimport)
#else
#define KERNEL_EXPORT 
#endif
#endif

class KERNEL_EXPORT CBF_SocketUdp  
{
public:
	// ������: CheckReadEvent
	// ���  : ������ 2012-4-23 16:29:23
	// ����  : int <0�����ˣ����������ѶϿ� 0���ɶ� 1�ɶ�
	// ����  : unsigned int tms  ��ʱms
	// ����  : ���socket�Ƿ�ɶ�
	int CheckReadEvent(unsigned int tms);
	
	// ������: GetSockAddr
	// ���  : ������ 2012-4-27 13:25:41
	// ����  : sockaddr_in & 
	// ����  : ȡ�ý��յ��Ŀͻ��˵�ַ��Ϣ����Ϊ�ͻ������õ�������Ϣ
	sockaddr_in & GetSockAddr();

	// ������: GetSockAddr
	// ���  : ������ 2012-4-27 13:26:25
	// ����  : void 
	// ����  : sockaddr_in &addr
	// ����  : ȡ�ý��յ��Ŀͻ��˵�ַ��Ϣ����Ϊ�ͻ������õ�������Ϣ
	void GetSockAddr(sockaddr_in &addr);


	// ������: GetHostString
	// ���  : ������ 2012-4-27 13:26:34
	// ����  : std::string 
	// ����  : ȡ����Ϊ�ͻ������õ�������Ϣ
	std::string GetHostString();

	// ������: GetPort
	// ���  : ������ 2012-4-27 13:26:50
	// ����  : unsigned int 
	// ����  : ȡ��UDP�˿�
	unsigned int GetPort();

	// ������: IsServer
	// ���  : ������ 2012-4-27 13:26:59
	// ����  : bool 
	// ����  : ��ʵ���Ƿ���˻��ǿͻ���
	bool IsServer();

	// ������: Close
	// ���  : ������ 2012-4-27 13:16:47
	// ����  : void 
	// ����  : �ر�socket
	void Close();

	// ������: BindClient
	// ���  : ������ 2012-4-27 13:04:42
	// ����  : bool 
	// ����  : const char *sip �����IP��������<7��Ϊ�㲥�����򷢸�ָ����IP
	// ����  : unsigned int port  �����UDP�˿�
	// ����  : ��Ҫ���͵ķ���ķ���˿���Ϣ��������˵ĵ�ַ����m_sSockAddr
	bool BindClient(const char *sip,unsigned int port);

	// ������: RecvFrom
	// ���  : ������ 2012-4-27 12:24:56
	// ����  : int =0������  -1������  >0�յ������ݳ���
	// ����  : char *databuf       ����
	// ����  : unsigned int buflen  ���ݳ���
	// ����  : sockaddr_in &addr  ���ͷ��ĵ�ַ
	// ����  : ���շ���������
	int RecvFrom(char *databuf,unsigned int buflen,sockaddr_in &addr);


	// ������: SendTo
	// ���  : ������ 2012-4-27 13:08:23
	// ����  : int <0����ʧ��, >0���ͳɹ������ݳ��� =0δ���ͳɹ������ܶԷ�������
	// ����  : sockaddr_in addr  ����Ŀ�ĵصĵ�ַ
	// ����  : const char *databuf
	// ����  : unsigned int buflen
	// ����  : ��������
	int SendTo(sockaddr_in addr,const char *databuf,unsigned int buflen);

	// ������: SendTo
	// ���  : ������ 2012-4-27 13:08:23
	// ����  : int <0����ʧ��, >0���ͳɹ������ݳ��� =0δ���ͳɹ������ܶԷ�������
	// ����  : const char *databuf
	// ����  : unsigned int buflen
	// ����  : �������ݸ�����ˣ�����֮ǰ����BindClient
	int SendTo(const char *databuf,unsigned int buflen);

	// ������: BindServer
	// ���  : ������ 2012-4-27 11:20:32
	// ����  : bool 
	// ����  : unsigned int port   �˿�
	// ����  : unsigned int bufsize  ��λk
	// ����  : �󶨷���ˣ����ý��ջ����С
	bool BindServer(unsigned int port,unsigned int bufsize=1024);

	// ������: GetLastErrorInfo
	// ���  : ������ 2012-4-23 11:04:55
	// ����  : void 
	// ����  : ��ȡ������Ϣ��m_errno��m_errMsg��
	void GetLastErrorInfo();

	// ������: SetBlockFlag
	// ���  : ������ 2009-5-8 10:27:12
	// ����  : void 
	// ����  : bool isblock  true����ģʽ  false������ģʽ 
	// ����  : ����socket������ģʽ  
	void SetBlockFlag(bool isblock);

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
	// ���  : ������ 2012-4-27 10:34:54
	// ����  : bool 
	// ����  : ����һ��Socket udp�Ͳ���AF_UNIX�ˣ�û��Ҫ
	bool Create();

	CBF_SocketUdp();
	virtual ~CBF_SocketUdp();

protected:
	
	
	SOCKET_HANDLE   m_socket;  //socket���ӱ�ʶ
	int             m_nErrno; //socket errno
	char            m_sErrMsg[300]; //������Ϣ
	int             m_nAddressFamily; //ͨѶģʽ���̶�ΪAF_INET

	char            m_sAddress[32];//Ҫ���͵ķ���˵ĵ�ַ��Ϣ,��BindClient����
	unsigned short  m_nPort;       //UDP�˿�
	
	sockaddr_in     m_sSockAddr; //��ַ��Ϣ �ͻ�����BindClient����
	bool            m_bIsServer;//�Ƿ�����

};

#endif // !defined(AFX_BF_SOCKETUDP_H__67745822_F0AF_45DC_97C6_22F4E0255E5D__INCLUDED_)
