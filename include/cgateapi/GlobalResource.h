// GlobalResource.h: interface for the CGlobalResource class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLOBALRESOURCE_H__90EA415C_F2CC_4710_B1B8_DC6B55202635__INCLUDED_)
#define AFX_GLOBALRESOURCE_H__90EA415C_F2CC_4710_B1B8_DC6B55202635__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BF_SocketTcp.h"
#include "BF_LogClient.h"
#include "BF_BufferPool.h"
#include "ICertApiBase.h"
#include "ApiFlowInfo.h"
#include "BF_Xml.h"
#include "BF_Tools.h"
#include "ICertApiBase.h"
#include "CgateLog.h"

typedef struct 
{
	std::string   sIp;
	unsigned int  nPort;
}GATE_IPPORT;

#ifdef CGATEAPI_EXPORTS
#define CGATEAPI_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define CGATEAPI_EXPORT __declspec(dllimport)
#else
#define CGATEAPI_EXPORT 
#endif
#endif

class CGATEAPI_EXPORT CGlobalResource  
{
public:

	// 函数名: GetMsg
	// 编程  : 王明松 2013-5-29 14:37:52
	// 返回  : std::string 
	// 描述  : 返回出错信息
	std::string GetMsg();

	// 函数名: IsControlTx
	// 编程  : 王明松 2013-5-27 17:41:48
	// 返回  : bool 
	// 描述  : 是否达到发送交易数控制
	bool IsControlTx();

	// 函数名: IsControlBytes
	// 编程  : 王明松 2013-5-27 17:41:30
	// 返回  : bool 
	// 描述  : 是否达到发送字节数控制
	bool IsControlBytes();

	// 函数名: Init
	// 编程  : 王明松 2013-5-27 17:17:38
	// 返回  : bool 
	// 参数  : const char *apixml
	// 描述  : 读配置文件初始化
	bool Init(const char *apixml);


	// 函数名: OpenDll
	// 编程  : 王明松 2013-5-30 15:07:45
	// 返回  : bool 
	// 参数  : DLLHANDLE *pDll  动态库句柄 
	// 参数  : const char *dllname
	// 参数  : const char *dllpath
	// 描述  : 打开动态库
	bool OpenDll(DLLHANDLE *pDll, const char *dllname, const char *dllpath);

	CGlobalResource();
	virtual ~CGlobalResource();

	CBF_LogClient  g_pLog;
	CCgateLog      g_pCgateLog;
	CApiFlowInfo   g_pApiFlowInfo;        
	CICertApiBase  *g_pCertClass;          //证书api操作类

	CBF_Xml       g_pXml;
	unsigned int  g_nFlowControlSendBytes;//流量控制发送字节数/秒
	unsigned int  g_nFlowControlSendTxNum;//流量控制发送交易数/秒
	int           g_nHeartTime; //心跳时间  隔多长时间未收到网关数据主动发送心跳
	int           g_nCloseTime;//连接未读断开时间
	int           g_nLogLevel;//日志级别
	std::string   g_sLogPath; //日志路径
	std::string   g_sLogName; //日志文件名
	int           g_nMaxLogSize;//日志大小
	int           g_nMaxDataLogSize;//data日志大小
	int           g_nDataLogLevel;//data日志级别
	int           g_nCrcFlag; //crc校验标志  0不校验不生成  1校验 生成

	int           g_nZipFlag; //是否压缩  0不压，1压缩

	int           g_nUseCert;//是否使用证书 0不使用 1使用

	char          g_sCurPath[300];

	int           g_nConnGateTimeOut;//连接网关超时时间

	unsigned int  g_nTimeOutSend; //发送数据超时时间 秒 当数据在队列里超过此时间后不发送
	unsigned int  g_nDataSendNum; //数据发送次数
	
	vector<GATE_IPPORT>  g_lGateList; //网关IP地址端口列表
	
private:
	char          m_sErrMsg[256];
	DLLHANDLE     m_pDll;

};

#endif // !defined(AFX_GLOBALRESOURCE_H__90EA415C_F2CC_4710_B1B8_DC6B55202635__INCLUDED_)
