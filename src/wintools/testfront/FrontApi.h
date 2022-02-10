// FrontApi.h: interface for the CFrontApi class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FRONTAPI_H__8E987EC6_8C8C_45BC_A26F_40C54476EA64__INCLUDED_)
#define AFX_FRONTAPI_H__8E987EC6_8C8C_45BC_A26F_40C54476EA64__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "BF_Mutex.h"
#include "BF_SocketTcp.h"
#include "BF_SimpleLog.h"
#include "BF_Date_Time.h"
#include "BF_Tools.h"



#ifdef API_EXPORTS
#define API_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define API_EXPORT __declspec(dllimport)
#else
#define API_EXPORT 
#endif
#endif

class API_EXPORT CFrontApi  
{
public:
	CFrontApi();
	virtual ~CFrontApi();


public: 
	
public: //下面为通讯的api部分
	

	/// 函数名: IsConnect
	/// 编程  : 王明松 2008-10-27 15:11:41
	/// 返回  : bool 
	/// 描述  : 是否连接上远程服务端
	bool IsConnect();

	/// 函数名: GetErrMsg
	/// 编程  : 王明松 2008-10-27 15:11:56
	/// 返回  : string 
	/// 描述  : 获取出错信息
	string GetErrMsg();


	/// 函数名: SetPara
	/// 编程  : 王明松 2008-10-29 9:30:33
	/// 返回  : void 
	/// 参数  : const char *ip 远程服务端IP
	/// 参数  : int port 远程服务端端口
	/// 参数  : const char *logname 日志文件名
	/// 参数  : int loglevel 日志级别
	/// 参数  : int headlen=5 通讯报文头长度
	/// 参数  : bool isincludehead=false 通讯报文头是否包含自身
	/// 参数  : int pwdflag=0  0不加密1加密 9自定义加密，需要设置加密的类方法
	/// 参数  : int encryptflag=0 0不加密码不压缩 1加密 2压缩3DES加密4DES加密并压缩5zlib压缩 9自定义加密 需要设置加密的类方法
	/// 描述  : 设置通讯参数
	void SetPara(const char *ip,int port,const char *logname,int loglevel,int headlen=5,bool isincludehead=false,int pwdflag=0,int encryptflag=0);

	/// 函数名: DisConnect
	/// 编程  : 王明松 2008-10-27 15:10:13
	/// 返回  : void 
	/// 描述  : 断开连接
	void DisConnect();

	/// 函数名: Connect
	/// 编程  : 王明松 2008-10-27 15:09:52
	/// 返回  : bool true成功 false失败
	/// 描述  : 连接远程服务端
	bool Connect();

	

	/// 函数名: RecvBuf
	/// 编程  : 王明松 2008-10-27 15:07:35
	/// 返回  : int <0接收失败，要重新连接 =0未接到数据 >0接收到的数据长度
	/// 参数  : char *buffer 数据缓冲
	/// 参数  : int &buflen  输入为缓冲大小 输出为接收的数据长度
	/// 参数  : int timeout 接收超时时间
	/// 描述  : 接收数据至缓冲，接收失败关闭连接，仅仅接收数据，不对数据进行mac校验
	int RecvBuf(char *buffer,int &buflen,int timeout);

	

	/// 函数名: SendBuf
	/// 编程  : 王明松 2008-10-27 15:06:11
	/// 返回  : int <=0失败 >0成功
	/// 参数  : const char *buffer 数据缓冲
	/// 参数  : int buflen 数据长度
	/// 描述  : 发送数据，不对数据生成mac，也不对密码进行加密，仅仅发送数据
	int SendBuf(const char *buffer,int buflen);

	/// 函数名: SetXTSegTime
	/// 编程  : 王明松 2008-10-29 10:48:10
	/// 返回  : void 
	/// 参数  : int ntime=600000
	/// 描述  : 设置心跳的时间，即连接在ntime内若没有使用，则主动发起心跳
	void SetXTSegTime(int ntime=600000);
	
	
	
	

protected:
	CBF_SocketTcp m_tcpSocket;
	CBF_Mutex m_mutexSend;//发送互斥量
	CBF_Mutex m_mutexRcv;//接收互斥量

	std::string m_sServerIp;//服务端IP
	int m_nPort;//端口
	int m_nTimeOut;//接收超时
	unsigned int m_nHeadLen;//报文头长度
	bool m_bIsIncludeSelf;//报文头是否包含自身
	bool m_bIsConnect;//是否连接上服务端
	int m_nLastUseTime;//上次使用时间
	bool m_bIsXtBegin;//是否开始心跳
	int m_nXtBeginTime;//心跳开始时间
	int m_nXTUseTIme;//连接在多少时间内没有使用，就发送心跳

	char m_errMsg[256];//出错信息

	char m_sDesEncryptKey[33];//报文des及3des加解密的密钥
	char m_sPinKey[33];//密码加解密的密钥
	char m_sMacKey[33];//mac加密的密钥

	int m_nEncryptZipFlag;//加密压缩标志 0不加密码不压缩 1加密 2压缩3DES加密4DES加密并压缩5zlib压缩 9自定义加解密

	int m_nPwdEncryptFlag;//密码加密标志 0不加密1 des加密 9自定义加解密

	CBF_SimpleLog m_log;
	CBF_Date_Time m_publog;
	CBF_Tools     m_pTool;
};

#endif // !defined(AFX_FRONTAPI_H__8E987EC6_8C8C_45BC_A26F_40C54476EA64__INCLUDED_)
