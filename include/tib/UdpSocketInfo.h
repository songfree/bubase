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

	// 函数名: SetPara
	// 编程  : 王明松 2015-11-24 9:09:40
	// 返回  : void 
	// 参数  : unsigned short port
	// 参数  : int index
	// 参数  : std::string sendaddr
	// 描述  : 设置参数
	void SetPara(unsigned short port ,int index,std::string sendaddr);

	// 函数名: GetIndex
	// 编程  : 王明松 2015-11-24 9:09:51
	// 返回  : int  
	// 描述  : 取得索引
	int  GetIndex();

	// 函数名: GetPort
	// 编程  : 王明松 2015-11-24 9:09:59
	// 返回  : unsigned short 
	// 描述  : 取得端口
	unsigned short GetPort();

	// 函数名: GetDataLen
	// 编程  : 王明松 2015-6-29 17:21:39
	// 返回  : int 
	// 描述  : 取得接收缓冲数据长度
	int GetDataLen();


	// 函数名: GetSocket
	// 编程  : 王明松 2015-6-29 17:22:00
	// 返回  : SOCKET_HANDLE 
	// 描述  : 取得socket描述符
	SOCKET_HANDLE GetSocket();

	// 函数名: SetCommEndian
	// 编程  : 王明松 2013-4-25 11:13:08
	// 返回  : void 
	// 参数  : bool islittle true是windows linux所使用的主机序  false为网络序
	// 描述  : 设置通讯时的字节序 
	void SetCommEndian(bool islittle);
	
	// 函数名: Endian2Comm
	// 编程  : 王明松 2013-4-25 10:41:58
	// 返回  : void 
	// 参数  : PS_TIB_HEAD data
	// 描述  : 通讯时，将字节序转为通讯时所用的字节序
	void Endian2Comm(PS_TIB_HEAD data);
	
	
	// 函数名: Endian2Comm
	// 编程  : 王明松 2013-5-6 14:42:15
	// 返回  : void 
	// 参数  : unsigned char *data
	// 参数  : int datalen
	// 描述  : 通讯时，将字节序转为通讯时所用的字节序
	void Endian2Comm(unsigned char *data,int datalen);
	
	
	// 函数名: Endian2LocalHost
	// 编程  : 王明松 2013-4-25 10:42:03
	// 返回  : void 
	// 参数  : PS_TIB_HEAD data
	// 描述  : 将通讯的主机字节序转为本机所使用的字节序
	void Endian2LocalHost(PS_TIB_HEAD data);
	

	
	
	// 函数名: Endian2LocalHost
	// 编程  : 王明松 2013-5-6 14:42:02
	// 返回  : void 
	// 参数  : unsigned char *data
	// 参数  : int datalen
	// 描述  : 将通讯的主机字节序转为本机所使用的字节序
	void Endian2LocalHost(unsigned char *data,int datalen);
	
	
	// 函数名: Reverse
	// 编程  : 王明松 2013-5-6 14:41:45
	// 返回  : void 
	// 参数  : unsigned char *buf
	// 参数  : int len
	// 描述  : 进行反转
	void Reverse(unsigned char *buf, int len);


	// 函数名: GetRcvData
	// 编程  : 王明松 2015-6-29 16:11:38
	// 返回  : int 
	// 参数  : PTIBCOMMSTRU data
	// 描述  : 取得一条完整数据
	int GetRcvData(PTIBCOMMSTRU data);

	// 函数名: Init
	// 编程  : 王明松 2015-6-29 14:54:30
	// 返回  : bool 
	// 参数  : CIErrlog *log
	// 描述  : 初始化
	bool Init(CIErrlog *log);

	// 函数名: Send2Bus
	// 编程  : 王明松 2015-6-29 14:41:32
	// 返回  : int 
	// 参数  : const char *data
	// 参数  : int datalen
	// 描述  : 发送数据
	int OnSend(const char *data,int datalen);

	// 函数名: OnRecv
	// 编程  : 王明松 2015-6-29 14:40:37
	// 返回  : int  <0出错 关闭，需重绑定   >=0为数据长度
	// 描述  : 接收数据
	int OnRecv();

	// 函数名: OnServer
	// 编程  : 王明松 2015-6-29 14:40:06
	// 返回  : bool 
	// 描述  : 绑定为服务端  create并bind
	bool OnServer();

	// 函数名: OnClient
	// 编程  : 王明松 2015-6-29 14:39:45
	// 返回  : bool 
	// 描述  : 绑定为客户端 create并bind
	bool OnClient();

	// 函数名: OnClose
	// 编程  : 王明松 2015-6-29 14:26:17
	// 返回  : void 
	// 描述  : 关闭UDP
	void OnClose();

	CUdpSocketInfo();
	virtual ~CUdpSocketInfo();

	
protected:
	int            m_nIndex;   //索引
	unsigned short m_nPort;    //UDP端口
	std::string    m_sSendAddr; //UDP发送地址，广播地址

	char      m_sRcvBuffer[UDPRCVBUFSIZE]; //接收缓冲区
	unsigned  int m_nRcvBufferLen;      //接收数据长度

	CBF_SocketUdp   m_pSocket;
	CIErrlog        *m_pLog;   

	CBF_Mutex       m_mutex;

	bool m_bIsLittle;//是否网络序 false为网络序 
	
	bool m_bUseLittleEndian;//通讯是否使用linux,windows主机字节序 false则为网络序
	            
};

#endif // !defined(AFX_UDPSOCKETINFO_H__FB03A890_2401_4755_A3CB_D94C6C9A83C9__INCLUDED_)
