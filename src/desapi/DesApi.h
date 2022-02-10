// DesApi.h: interface for the CDesApi class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DESAPI_H__BD08438C_A910_43CC_9CFA_1D0BAF156AFB__INCLUDED_)
#define AFX_DESAPI_H__BD08438C_A910_43CC_9CFA_1D0BAF156AFB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "TcpSocket.h"
#include "Errlog.h"

class CDesApi  
{
public:

	// 函数名: ConvertPinBlock
	// 编程  : 王明松 2011-3-1 19:46:58
	// 返回  : int 0成功 <0出错 >0加密机返回错
	// 输入参数  : const char *srcpin 源PIN 可见字符  srcpinkey加密的
	// 输入参数  : const char *srcpinkey    可见字符  主密钥加密的
	// 输入参数  : const char *destpinkey   可见字符  主密钥加密的
	// 输出参数  : char *despin             可见字符  destpinkey加密的
	// 输入参数  : const char *bankacc      账户
	// 描述  : 转加密 此方法仅供邮储柜面的PIN转为加密机的PIN
	int ConvertPinBlock(const char *srcpin,const char *srcpinkey,const char *destpinkey,char *despin,const char *bankacc);

	// 函数名: CallSrv
	// 编程  : 王明松 2011-3-1 19:23:28
	// 返回  : int -1失败 0成功  >0错误码
	// 参数  : const char *data  发送数据(不包含消息头)
	// 参数  : int datalen   数据长度
	// 参数  : char *rdata   加密机应答数据
	// 参数  : int &rlen     加密机应答数据长度
	// 描述  : 调用加密机
	int CallSrv(const char *data,int datalen,char *rdata,int &rlen);

	// 函数名: DisConnect
	// 编程  : 王明松 2011-3-1 19:23:16
	// 返回  : void 
	// 描述  : 断开加密机
	void DisConnect();

	// 函数名: Connect
	// 编程  : 王明松 2011-3-1 19:23:07
	// 返回  : bool 
	// 参数  : const char *ip
	// 参数  : int port
	// 参数  : int timeout
	// 描述  : 连接加密机
	bool Connect(const char *ip,int port,int timeout);

	// 函数名: Connect
	// 编程  : 王明松 2011-3-1 19:22:56
	// 返回  : bool 
	// 描述  : 连接加密机
	bool Connect();

	

	// 函数名: SetHostPara
	// 编程  : 王明松 2011-3-1 19:22:41
	// 返回  : void 
	// 参数  : const char *ip
	// 参数  : int port
	// 参数  : int timeout
	// 描述  : 设置加密机参数
	void SetHostPara(const char *ip,int port,int timeout);
	CDesApi();
	virtual ~CDesApi();

	CErrlog m_pLog;

protected:
	CTcpSocket m_pSocket;
	
	char m_sRSBuffer[8192];
	std::string m_sIp;
	int         m_nPort;
	int         m_nTimeOut;

	bool        m_bIsConnect;
	bool        m_bIsSet;
};

#endif // !defined(AFX_DESAPI_H__BD08438C_A910_43CC_9CFA_1D0BAF156AFB__INCLUDED_)
