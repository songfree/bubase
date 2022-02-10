/// ClientApi.h: interface for the CClientApi class.
///
/////////////////////////////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLIENTAPI_H__C8ADEC7B_210F_41AE_9191_D0D8EE3277ED__INCLUDED_)
#define AFX_CLIENTAPI_H__C8ADEC7B_210F_41AE_9191_D0D8EE3277ED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif /// _MSC_VER > 1000

#include "IClientApi.h"
#include "XmlPack.h"
#include "XmlPackArray.h"
#include "BF_SimpleLog.h"
#include "BF_SocketTcp.h"
#include "BF_Date_Time.h"
#include "BF_Tools.h"


#ifdef TUXEDO
#include "atmi.h"
#endif


#ifdef WIN32
#ifdef _DEBUG
	#pragma comment(lib, "bf_oldxmld.lib") 
	#pragma comment(lib, "bf_kerneld.lib") 
	#pragma message("Automatically linking with  bf_kerneld.lib bf_oldxmld.lib")
#else
	#pragma comment(lib, "bf_oldxml.lib") 
	#pragma comment(lib, "bf_kernel.lib") 
	#pragma message("Automatically linking with  bf_kernel.lib bf_oldxml.lib")
#endif
#endif
///此版本含有tuxedo部分，只要在link里加上  wtuxws32.lib，并在编译选项里加上TUXEDO然后重新编译就是支持TUXEDO的版本

typedef struct _GLOBAL_VARS_API
{
	char g_sIp[50];///前置IP
	int  g_nPort;///侦听端口
	char g_logFileName[201];///日志文件名
	int  g_timeOut;///socket接收超时时间
	int  g_trace;///日志跟踪级别
	int  g_tpCallTime;///TUXEDO外调超时时间
	unsigned int  g_rcvHead;///通讯包头长度
	bool  g_bIsInclHead;///通讯包头是否包含自身
	bool  g_bIsTuxedo;///是否使用tuxedo
	int   g_bIsZipEncrypt;///是否加密压缩1加密2压缩0无
	bool  g_bIsPwdEncrypt;///密码是加密码set
	bool  g_bIsFileTrans;///是否使用文件传输
}G_VARS_API;
#ifdef API_EXPORTS
#define API_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define API_EXPORT __declspec(dllimport)
#else
#define API_EXPORT 
#endif
#endif
class API_EXPORT CClientApi  : public CIClientApi
{
public:
	CClientApi();
	virtual ~CClientApi();

	CXmlPack m_xmlconf;///配置文件内容
	CBF_SimpleLog m_log;///写日志 
	CBF_SocketTcp m_tcpSocket;///tcp
//	Cpubstrtime m_publog;
	CBF_Date_Time m_publog;
	CBF_Tools     m_pTool;

	
	
	bool m_bIsInit;///是否初始化
	
	G_VARS_API g_vars;
	
	bool m_isServiceName;
public:
	virtual void Close();

	/// 函数名: TcpInit
	/// 编程  : 王明松 2008-10-10 15:58:18
	/// 返回  : bool 
	/// 描述  : socket连接初始化
	bool TcpInit();
	

	/// 函数名: CallService
	/// 编程  : 王明松 2008-7-3 9:22:38
	/// 返回  : int <0失败 =0成功
	/// 参数  : const char *bizinfobuf
	/// 参数  : string &bizinfoansbuf  应答数据缓冲
	/// 描述  : 兼容bizinfo格式的数据，将bizinfo转成xml发送至后台
	virtual int CallService(const char *bizinfobuf,std::string &bizinfoansbuf);

	/// 函数名: SetTuxedoPara
	/// 编程  : 王明松 2008-7-3 9:18:42
	/// 返回  : int <0失败 =0成功
	/// 参数  : bool shortconnect=true
	/// 参数  : int tuxedobuflen=8192
	/// 参数  : bool tuxedoalias=false tuxedo别名，通过服务名+交易码作为别名
	/// 描述  : 设置tuxedo的连接参数及数据缓冲长度，如果不调用本方进行设置的话，默认为短连接，缓冲为8192
	virtual int SetTuxedoPara(bool shortconnect=true,int tuxedobuflen=8192, bool tuxedoalias=false);


	/// 函数名: GetFile
	/// 编程  : 王明松 2008-6-27 14:57:47
	/// 返回  : int <0失败 =0成功
	/// 参数  : const char *srcfilenamepath  主机上的文件
	/// 参数  : const char * destfilenamepath 本地保存的文件
	/// 参数  : char *szmsg
	/// 描述  : 取文件
	virtual int GetFile(const char *srcfilenamepath,const char * destfilenamepath,char *szmsg);

	/// 函数名: SetTuxedoPara
	/// 编程  : 王明松 2008-6-27 14:58:18
	/// 返回  : int int <0失败 =0成功
	/// 参数  : const char *wsnaddr  tuxedo的wsnaddr ///192.168.22.216L17090
	/// 参数  : const char *serviceName  tuxedo服务名
	/// 参数  : int zip 是否加密码压缩  0不加密码不压缩 1加密 2压缩3DES加密4DES加密并压缩5zlib压缩
	/// 参数  : int pwdflag 密码是否加密 (若为1表示xml包中密码已加密，本api不再加密)
	/// 参数  : char *logfile 日志文件名
	/// 参数  : int loglevel 日志级别
	/// 参数  : bool istransfile = true 后续包是否通过传输入文件
	/// 描述  : 设置tuxedo的通讯参数
	virtual int SetTuxedoPara(const char *wsnaddr,const char *serviceName,int zip ,int pwdflag,char *logfile,int loglevel,bool istransfile = true);
	


	/// 函数名: SetCommPara
	/// 编程  : 王明松 2008-6-27 14:58:34
	/// 返回  : int int <0失败 =0成功
	/// 参数  : const char *ip ip地址
	/// 参数  : int port  侦听端口
	/// 参数  : int zip  是否加密码压缩  0不加密码不压缩 1加密 2压缩3DES加密4DES加密并压缩5zlib压缩
	/// 参数  : int pwdflag  密码是否加密 (若为1表示xml包中密码已加密，本api不再加密)
	/// 参数  : int timeout   超时时间
	/// 参数  : int headlen   通讯报文头长度
	/// 参数  : int inchead   是否包含通讯报文头
	/// 参数  : const char *logfile  日志文件名
	/// 参数  : int loglevel  日志级别
	/// 参数  : bool istransfile = true 后续包是否通过传输入文件
	/// 参数  : bool shortconnect=false 连接类型
	/// 描述  : 设置socket通讯的参数
	virtual int SetCommPara(const char *ip, int port, int zip, int pwdflag, int timeout, int headlen, int inchead,const char *logfile,int loglevel,bool istransfile = true,bool shortconnect=false);
	
	/// 函数名: SendFile
	/// 编程  : 王明松 2008-5-29 8:37:16
	/// 返回  : int int <0失败 =0成功
	/// 参数  : const char *srcfilenamepath 要发送的文件名(包含path)
	/// 参数  : const char * destfilenamepath 后台的文件名(包含path)
	/// 参数  : char *szmsg
	/// 描述  : 发送文件至后台
	virtual int SendFile(const char *srcfilenamepath,const char * destfilenamepath,char *szmsg);
	

	/// 函数名: CallServiceFile
	/// 编程  : 王明松 2007-12-6 19:15:11
	/// 返回  : int int <0失败 =0成功
	/// 参数  : const char *reqfilename
	/// 参数  : char *ansfilename
	/// 描述  : 调用后返回多包数据文件名
	virtual int CallServiceFile(CXmlPack &apack,const char *reqfilename,char *ansfilename);

	

	/// 函数名: CallService
	/// 编程  : 王明松 2007-10-15 14:29:06
	/// 返回  : int <0失败
	/// 参数  : CXmlPack &xmlpack 请求、单包应答
	/// 参数  : CXmlPackArray &muxmlpack 多包应答
	/// 描述  : 调用后台服务，xmlpack为请求，xmpack及muxmlpack为应答
	virtual int CallService(CXmlPack &xmlpack, CXmlPackArray &muxmlpack);

	

	/// 函数名: Init
	/// 编程  : 王明松 2007-10-15 14:28:26
	/// 返回  : bool 
	/// 参数  : char *file 配置文件名 在linux下带路径
	/// 描述  : 初始化,从配置文件读参数
	virtual bool Init(char *file);

	virtual int Xml2Biz(CXmlPack *xmlpack,std::string &bizbuf);
	virtual int Biz2Xml(const char *bizinfobuf ,CXmlPack &xmlpack);
protected:


#ifdef TUXEDO
	TPINIT * g_tpinitbuf;
#endif
	char  m_wsnaddr[80];
	bool  m_bIsShortConnect;///是否短连接
	int   m_TuxedoBufLen;///缓冲长度
	
	char m_service[60];///tuxedo服务名
	bool m_bIsTpInit ;///tuxedo是否初始化
	bool m_bIsTcpInit;//socket是否连接


	int m_txCode;///交易码
	bool m_tuxedoAlias;///是否调用tuxedo别名


	/// 函数名: GetFileName
	/// 编程  : 王明松 2007-12-3 10:27:16
	/// 返回  : int <0失败 =0成功
	/// 参数  : char *filename
	/// 描述  : 生成多包数据文件名
	int GetFileName(char *filename);

	/// 函数名: WriteToFile
	/// 编程  : 王明松 2007-12-3 10:27:00
	/// 返回  : int <0失败 =0成功 
	/// 参数  : FILE *fp
	/// 参数  : char *buf
	/// 参数  : int buflen
	/// 描述  : 将应答xml数据写入文件
	int WriteToFile(FILE *fp,char *buf,int buflen);

	/// 函数名: TuxedoInit
	/// 编程  : 王明松 2007-12-3 10:26:50
	/// 返回  : bool 
	/// 参数  : bool isinit
	/// 描述  : tuxedo初始化
	bool TuxedoInit(bool isinit);

	/// 函数名: TcpReceive
	/// 编程  : 王明松 2007-10-15 14:29:44
	/// 返回  : int <0失败 =0成功
	/// 参数  : char *buf
	/// 参数  : int &buflen
	/// 描述  : socket接收
	int TcpReceive(char *buf,int &buflen);
	
	/// 函数名: TcpSend
	/// 编程  : 王明松 2007-10-15 14:29:40
	/// 返回  : int <0失败 =0成功
	/// 参数  : char *buf
	/// 参数  : int buflen
	/// 描述  : socket发送
	int TcpSend(char *buf,int buflen);
	
	/// 函数名: GetService
	/// 编程  : 王明松 2007-10-15 14:29:36
	/// 返回  : bool 
	/// 参数  : int txcode
	/// 描述  : 根据交易码取tuxedo的服务名
	bool GetService(int txcode);

	/// 函数名: Send2Server
	/// 编程  : 王明松 2007-10-15 14:28:53
	/// 返回  : int <0失败 =0成功
	/// 参数  : char *sendbuf
	/// 参数  : int buflen
	/// 参数  : char *rcvbuf
	/// 参数  : int &rcvlen
	/// 描述  : 发送到后台
	int Send2Server(char *sendbuf,int buflen,char *rcvbuf,int &rcvlen);

	/// 函数名: GetMuPack
	/// 编程  : 王明松 2008-6-27 17:16:42
	/// 返回  : int <0失败 =0成功
	/// 参数  : const char *filename
	/// 参数  : CXmlPackArray &muxmlpack
	/// 参数  : int mupackans 是否多pack应答 1是其它否
	/// 参数  : bool first= false 第一包是否放到muxmlpack中
	/// 描述  : 将多包文件中的数据读取到muxmlpack中
	int GetMuPack(const char *filename,CXmlPackArray &muxmlpack,int mupackans,bool first= false);

	/// 函数名: GetFileSerial
	/// 编程  : 王明松 2008-6-27 17:17:18
	/// 返回  : int 
	/// 参数  : char *serial
	/// 描述  : 取唯一文件名
	int GetFileSerial(char *serial);
};
extern "C" 
{
	API_EXPORT CClientApi * getInstanceApi();
	
}
#undef  API_EXPORT
#endif /// !defined(AFX_CLIENTAPI_H__C8ADEC7B_210F_41AE_9191_D0D8EE3277ED__INCLUDED_)
