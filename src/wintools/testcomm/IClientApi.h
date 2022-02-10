/// IClientApi.h: interface for the CIClientApi class.
///
/////////////////////////////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ICLIENTAPI_H__A9EB780E_A8D1_4491_8911_1D6845DBEBAB__INCLUDED_)
#define AFX_ICLIENTAPI_H__A9EB780E_A8D1_4491_8911_1D6845DBEBAB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif /// _MSC_VER > 1000

#include <stdio.h>
#include <string.h>
#include <string>

class CXmlPack;
class CXmlPackArray;

class  CIClientApi  
{
public:
	char m_errMsg[256];///出错信息

	CIClientApi()
	{

	}
	virtual ~CIClientApi()
	{

	}
	virtual void Close()=0;
	/// 函数名: CallService
	/// 编程  : 王明松 2008-7-3 9:22:38
	/// 返回  : int <0失败 =0成功
	/// 参数  : const char *bizinfobuf
	/// 参数  : string &bizinfoansbuf  应答数据缓冲
	/// 描述  : 兼容bizinfo格式的数据，将bizinfo转成xml发送至后台
	virtual int CallService(const char *bizinfobuf,std::string &bizinfoansbuf)=0;


	/// 函数名: SetTuxedoPara
	/// 编程  : 王明松 2008-7-3 9:18:42
	/// 返回  : int <0失败 =0成功
	/// 参数  : bool shortconnect=true
	/// 参数  : int tuxedobuflen=8192
	/// 参数  : bool tuxedoalias=false
	/// 描述  : 设置tuxedo的连接参数及数据缓冲长度，如果不调用本方进行设置的话，默认为短连接，缓冲为8192
	virtual int SetTuxedoPara(bool shortconnect=true,int tuxedobuflen=8192, bool tuxedoalias=false)=0;



	/// 函数名: GetFile
	/// 编程  : 王明松 2008-6-27 14:57:47
	/// 返回  : int <0失败 =0成功
	/// 参数  : const char *srcfilenamepath  主机上的文件
	/// 参数  : const char * destfilenamepath 本地保存的文件
	/// 参数  : char *szmsg
	/// 描述  : 取文件
	virtual int GetFile(const char *srcfilenamepath,const char * destfilenamepath,char *szmsg)=0;


	/// 函数名: SetTuxedoPara
	/// 编程  : 王明松 2008-6-27 14:58:18
	/// 返回  : int int <0失败 =0成功
	/// 参数  : const char *wsnaddr  tuxedo的wsnaddr ///192.168.22.216L17090
	/// 参数  : const char *serviceName  tuxedo服务名
	/// 参数  : int zip 是否加密码压缩  0不加密码不压缩 1加密 2压缩
	/// 参数  : int pwdflag 密码是否加密 (若为1表示xml包中密码已加密，本api不再加密)
	/// 参数  : char *logfile 日志文件名
	/// 参数  : int loglevel 日志级别
	/// 参数  : bool istransfile = true 后续包是否通过传输入文件
	/// 描述  : 设置tuxedo的通讯参数
	virtual int SetTuxedoPara(const char *wsnaddr,const char *serviceName,int zip ,int pwdflag,char *logfile,int loglevel,bool istransfile = true)=0;

	


	/// 函数名: SetCommPara
	/// 编程  : 王明松 2008-6-27 14:58:34
	/// 返回  : int int <0失败 =0成功
	/// 参数  : const char *ip ip地址
	/// 参数  : int port  侦听端口
	/// 参数  : int zip  是否加密码压缩  0不加密码不压缩 1加密 2压缩
	/// 参数  : int pwdflag  密码是否加密 (若为1表示xml包中密码已加密，本api不再加密)
	/// 参数  : int timeout   超时时间
	/// 参数  : int headlen   通讯报文头长度
	/// 参数  : int inchead   是否包含通讯报文头
	/// 参数  : const char *logfile  日志文件名
	/// 参数  : int loglevel  日志级别
	/// 参数  : bool istransfile = true 后续包是否通过传输入文件
	/// 描述  : 设置socket通讯的参数
	virtual int SetCommPara(const char *ip, int port, int zip, int pwdflag, int timeout, int headlen, int inchead,const char *logfile,int loglevel,bool istransfile = true,bool shortconnect=false)=0;

	
	/// 函数名: SendFile
	/// 编程  : 王明松 2008-5-29 8:37:16
	/// 返回  : int int <0失败 =0成功
	/// 参数  : const char *srcfilenamepath 要发送的文件名(包含path)
	/// 参数  : const char * destfilenamepath 后台的文件名(包含path)
	/// 参数  : char *szmsg
	/// 描述  : 发送文件至后台
	virtual int SendFile(const char *srcfilenamepath,const char * destfilenamepath,char *szmsg)=0;

	/// 函数名: CallServiceFile
	/// 编程  : 王明松 2007-12-6 19:15:11
	/// 返回  : int int <0失败 =0成功
	/// 参数  : const char *reqfilename
	/// 参数  : char *ansfilename
	/// 描述  : 调用后返回多包数据文件名
	virtual int CallServiceFile(CXmlPack &apack,const char *reqfilename,char *ansfilename)=0;

	/// 函数名: CallService
	/// 编程  : 王明松 2007-10-15 14:29:06
	/// 返回  : int <0失败
	/// 参数  : CXmlPack &xmlpack 请求、单包应答
	/// 参数  : CXmlPackArray &muxmlpack 多包应答
	/// 描述  : 调用后台服务，xmlpack为请求，xmpack及muxmlpack为应答
	virtual int CallService(CXmlPack &xmlpack, CXmlPackArray &muxmlpack)=0;

	/// 函数名: Init
	/// 编程  : 王明松 2007-10-15 14:28:26
	/// 返回  : bool 
	/// 参数  : char *file 配置文件名 在linux下带路径
	/// 描述  : 初始化,从配置文件读参数
	virtual bool Init(char *file)=0;

	virtual int Xml2Biz(CXmlPack *xmlpack,std::string &bizbuf)=0;

	virtual int Biz2Xml(const char *bizinfobuf ,CXmlPack &xmlpack)=0;



};

#endif /// !defined(AFX_ICLIENTAPI_H__A9EB780E_A8D1_4491_8911_1D6845DBEBAB__INCLUDED_)
