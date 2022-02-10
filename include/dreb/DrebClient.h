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

//单通讯平台客户端API
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


	// 函数名: Monitor_Dreb
	// 编程  : 王明松 2009-12-2 15:23:40
	// 返回  : int >0<DREBHEADLEN通讯平台返回错 >DREBHEADLEN收到的数据长度 =0在指定时间内未收到数据  <0失败 -100未连接 -1连接断开 -2通讯平台应答数据不符 -3CRC错误
	// 参数  : int hook
	// 参数  : int serial
	// 参数  : DREB_D_NODEINFO dest
	// 参数  : const char *data  对应cZip的数据(明文加密或压缩)
	// 参数  : int datalen 对应cZip的数据长度
	// 参数  : int cZip
	// 参数  : DREB_HEAD &head
	// 参数  : char *ansdata   对应head->cZip解开后的明文数据,自定义除外
	// 描述  : 通讯平台监控 要求最终处理完成后应答
	int Monitor_Dreb(int hook,int serial,DREB_D_NODEINFO dest, const char *data, int datalen,int cZip,DREB_HEAD &head,char *ansdata);

	// 函数名: Monitor_Bpc
	// 编程  : 王明松 2009-12-2 15:23:40
	// 返回  : int >0<DREBHEADLEN通讯平台返回错 >DREBHEADLEN收到的数据长度 =0在指定时间内未收到数据  <0失败 -100未连接 -1连接断开 -2通讯平台应答数据不符 -3CRC错误
	// 参数  : int hook
	// 参数  : int serial
	// 参数  : DREB_D_NODEINFO dest
	// 参数  : const char *data  对应cZip的数据(明文加密或压缩)
	// 参数  : int datalen 对应cZip的数据长度
	// 参数  : int cZip
	// 参数  : DREB_HEAD &head
	// 参数  : char *ansdata   对应head->cZip解开后的明文数据,自定义除外
	// 描述  : BPC监控 要求最终处理完成后应答
	int Monitor_Bpc(int hook,int serial,DREB_D_NODEINFO dest, const char *data, int datalen,int cZip,DREB_HEAD &head,char *ansdata);

	// 函数名: GetDrebHead
	// 编程  : 王明松 2010-9-28 9:18:45
	// 返回  : void 
	// 参数  : DREB_HEAD &head
	// 描述  : 获取连接时返回的dreb头
	void GetDrebHead(DREB_HEAD &head);

	// 函数名: DPABC
	// 编程  : 王明松 2009-12-2 15:25:00
	// 返回  : int <0发送失败，>=发送的数据长度
	// 参数  : int hook
	// 参数  : int serial
	// 参数  : DREB_D_NODEINFO dest
	// 参数  : const char *data  对应cZip的数据(明文加密或压缩)
	// 参数  : int datalen 对应cZip的数据长度
	// 参数  : int cZip
	// 描述  : 通讯平台广播，即将信息发给指定通讯平台上所有注册的服务,不要求通讯平台应答
	int DPABC(int hook, int serial,int gateindex,DREB_D_NODEINFO dest, const char *data, int datalen,int cZip,int nextno=0);

	// 函数名: DPBC
	// 编程  : 王明松 2009-12-2 15:24:42
	// 返回  : int =100成功 >0且<100dreb返回错 <0发送失败 =0未收到确认消息
	// 参数  : int hook
	// 参数  : int serial
	// 参数  : DREB_D_NODEINFO dest
	// 参数  : const char *data  对应cZip的数据(明文加密或压缩)
	// 参数  : int datalen 对应cZip的数据长度
	// 参数  : int cZip
	// 描述  : 通讯平台广播，即将信息发给指定通讯平台上所有注册的服务,要求通讯平台应答
	int DPBC(int hook, int serial,DREB_D_NODEINFO dest, const char *data, int datalen,int cZip);

	// 函数名: DPPost
	// 编程  : 王明松 2009-12-2 15:24:28
	// 返回  : int =100成功 >0且<100dreb返回错 <0发送失败 =0未收到确认消息
	// 参数  : int hook
	// 参数  : int serial
	// 参数  : DREB_D_NODEINFO dest
	// 参数  : const char *data  对应cZip的数据(明文加密或压缩)
	// 参数  : int datalen 对应cZip的数据长度
	// 参数  : int cZip
	// 描述  : 通讯平台投递，要求接收到的通讯平台应答
	int DPPost(int hook, int serial, DREB_D_NODEINFO dest, const char *data, int datalen,int cZip);

	// 函数名: DPPush
	// 编程  : 王明松 2009-12-2 15:24:13
	// 返回  : int <0发送失败，>=发送的数据长度
	// 参数  : int hook
	// 参数  : int serial
	// 参数  : DREB_D_NODEINFO dest
	// 参数  : const char *data  对应cZip的数据(明文加密或压缩)
	// 参数  : int datalen 对应cZip的数据长度
	// 参数  : int cZip
	// 描述  : 通讯平台推送，无须应答
	int DPPush(int hook, int serial, DREB_D_NODEINFO dest, const char *data, int datalen,int cZip);

	// 函数名: DPACall
	// 编程  : 王明松 2009-12-2 15:24:06
	// 返回  : int =100成功 >0且<100dreb返回错 <0发送失败 =0未收到确认消息
	// 参数  : int hook
	// 参数  : int serial
	// 参数  : DREB_D_NODEINFO dest
	// 参数  : const char *data  对应cZip的数据(明文加密或压缩)
	// 参数  : int datalen 对应cZip的数据长度
	// 参数  : int cZip
	// 描述  : 通讯平台异步调用 要求接收到的通讯平台发送至服务后确认应答
	int DPACall(int hook, int serial, DREB_D_NODEINFO dest, const char *data, int datalen,int cZip);

	// 函数名: DPCall
	// 编程  : 王明松 2009-12-2 15:23:40
	// 返回  : int >0<DREBHEADLEN通讯平台返回错 >DREBHEADLEN收到的数据长度 =0在指定时间内未收到数据  <0失败 -100未连接 -1连接断开 -2通讯平台应答数据不符 -3CRC错误
	// 参数  : int hook
	// 参数  : int serial
	// 参数  : DREB_D_NODEINFO dest
	// 参数  : const char *data  对应cZip的数据(明文加密或压缩)
	// 参数  : int datalen 对应cZip的数据长度
	// 参数  : int cZip
	// 参数  : DREB_HEAD &head
	// 参数  : char *ansdata   对应head->cZip解开后的明文数据,自定义除外
	// 描述  : 通讯平台同步调用 要求最终处理完成后应答
	int DPCall(int hook,int serial,DREB_D_NODEINFO dest, const char *data, int datalen,int cZip,DREB_HEAD &head,char *ansdata);

	// 函数名: IsConnect
	// 编程  : 王明松 2009-12-2 15:13:30
	// 返回  : bool 
	// 描述  : 是否连接
	bool IsConnect();

	// 函数名: Receve
	// 编程  : 王明松 2009-11-26 16:16:19
	// 返回  : int >0实际收到的数据长度 =0在指定时间内未收到数据  <0失败 -100未连接 -1连接断开 -2通讯平台应答数据不符 -3CRC错误
	// 参数  : DREB_HEAD &head  通讯平台通讯头 
	// 参数  : char *data       接收到的数据 对应head->cZip解开后的明文数据,长度为head.nLen,自定义除外
	// 参数  : int timeout      接收数据超时时间
	// 描述  : 接收数据，注意返回值可能和head.nLen不等，head.nLen是解开后的明文数据
	int ReceveData(DREB_HEAD &head ,unsigned char *data,unsigned int &datalen,int timeout);


	// 函数名: Close
	// 编程  : 王明松 2009-11-26 16:16:10
	// 返回  : void 
	// 描述  : 关闭连接
	void Close();

	// 函数名: Connect
	// 编程  : 王明松 2009-11-26 16:15:58
	// 返回  : bool 
	// 描述  : 连接通讯平台
	bool Connect();

	// 函数名: Init
	// 编程  : 王明松 2009-11-26 14:45:45
	// 返回  : bool 
	// 参数  : const  char *ip 要连接的通讯平台ip
	// 参数  : short port 要连接的通讯平台端口
	// 参数  : int   timeout 接收超时 毫秒
	// 描述  : 设置连接的通讯平台参数
	bool Init(const  char *ip,short port,int timeout);


	// 函数名: ZipBuf
	// 编程  : 王明松 2010-9-28 9:49:30
	// 返回  : bool 
	// 参数  : int IsZipEncrypt   0不做操作 13DES加密,密码16位长度 2压缩 3DES加密,密码8位长度 4DES加密并压缩
	// 参数  : char *buf
	// 参数  : int &buflen
	// 描述  : 加密压缩 0-4为已定义  其它要自定义，不要调用本方法了
//	bool ZipBuf(int IsZipEncrypt,char *buf,int &buflen);

	// 函数名: UnzipBuf
	// 编程  : 王明松 2010-9-28 9:49:27
	// 返回  : bool 
	// 参数  : int IsZipEncrypt 0不做操作 13DES加密,密码16位长度 2压缩 3DES加密,密码8位长度 4DES加密并压缩
	// 参数  : char *buf
	// 参数  : int &buflen
	// 描述  : 解加密解压缩 0-4为已定义  其它要自定义，不要调用本方法了
//	bool UnzipBuf(int IsZipEncrypt, char *buf,int &buflen);

	// 函数名: SendData
	// 编程  : 王明松 2009-11-26 16:16:28
	// 返回  : int <0发送失败 -1发送出错 -2数据太大 >0 为datalen的值
	// 参数  : int hook    客户端使用的字段，通讯平台不用，原样返回。
	// 参数  : int serial  异步时填写的流水，通讯平台不用，原样返回
	// 参数  : int cmd   命令字 CMD_DPCALL CMD_DPACALL CMD_DPBC CMD_DPABC CMD_DPPUSH CMD_DPPOST
	// 参数  : DREB_D_NODEINFO dest  目标信息  
	// 参数  : char *data  数据
	// 参数  : int datalen 数据长度
	// 参数  : int cZip 0不做操作 13DES加密,密码16位长度 2压缩 3DES加密,密码8位长度 4DES加密并压缩
	// 参数  : int nextno  后续包序号
	// 描述  : 发送数据
	int SendData(int hook,int serial,int gateindex,int cmd,DREB_D_NODEINFO dest,const char *data, int datalen,int cZip,int nextno=0);

	char          m_errMsg[512];

	CBF_SimpleLog m_pLog;
protected:
	CEncryptCommBuffer m_pDesZip;

	DREB_HEAD     m_head; //通讯头,连接时保留的，一直不变
	CBF_SocketTcp    m_tcpSocket;
	CDrebEndian    m_pCheckHead;
	std::string   m_sIp;//要连接的通讯平台ip
	short         m_nPort;//要连接的通讯平台端口
	int           m_nTimeOut;//接收超时
	bool          m_bIsInit;
	bool          m_bIsConnect;//是否连接上
	
	char          m_sRcvBuffer[DREBBUFSIZE];
	unsigned  int m_nRcvLen;
	char          m_sSendBuffer[DREBBUFSIZE];

	unsigned char m_tmpdata[DREBBUFSIZE];
	unsigned int  m_tmpdatalen;

};

#endif // !defined(AFX_DrebClient_H__CDCD9144_228C_44B3_ABB0_373102494D80__INCLUDED_)
