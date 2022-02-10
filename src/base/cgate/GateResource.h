// GateResource.h: interface for the CGateResource class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GATERESOURCE_H__B9E4A73C_7ADB_404E_A678_7919376804D6__INCLUDED_)
#define AFX_GATERESOURCE_H__B9E4A73C_7ADB_404E_A678_7919376804D6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BF_AIO_Resource.h"
#include "BF_Xml.h"
#include "ICertApiBase.h"
#include "CgateLog.h"
#include "BF_CommDataLog.h"


typedef struct
{
	std::string sIp;       //IP
	int         nPort;     //端口
	int         nBandwidth;//带宽
}S_DREBLINK_INFO;

class CGateResource : public CBF_AIO_Resource  
{
public:
	CGateResource();
	virtual ~CGateResource();
	
	unsigned int   g_nSvrMainId;     //目标服务功能号 ,必须填写
	unsigned int   g_nSvrPrivateId;  //目标私有序号 0表示公共 >0为每个服务的序号，必须填写
	
	int   g_nUnCommpress;   //是不是对客户端的数据解压缩至明文
	int   g_nZipFlag;  //发给客户端的数据是否压缩
	int   g_nEncryptFlag;  //发给客户端的数据是否加密

	int   g_nUseMonitor;       //是否监控
	int   g_nMonitorHost;      //是否报告主机资源
	int   g_nMonitorDrebId;            //公共节点号
	int   g_nMonitorDrebPrivateId;     //私有节点号
	int   g_nMonitorSvrId;            //公共服务节点号
	int   g_nMonitorSvrPrivateId;     //私有服务节点号
	int   g_nMonitorTxCode;           //监控交易码

	int   g_nSendQueThreadNum;        //行情发送线程数

	int   g_nMaxDataLogSize;          //data日志大小
	int   g_nDataLoglevel;            //data日志级别
	int g_nCrcFlag; //crc校验标志  0不校验不生成  1校验 生成

	int   g_nQuoSubScribe;//是否行情订阅   0不用订阅  1需订阅


	int   g_nBroadCastMode;//广播模式 0一直发  1禁发    2只发

	CUInt  g_lBCFuncList;//广播配置 交易码列表  禁止发送或只发送的
	// 函数名: Init
	// 编程  : 王明松 2013-6-7 20:25:58
	// 返回  : virtual bool 
	// 参数  : const char *confile
	// 描述  : 初始化，读配置文件
	virtual bool Init(const char *confile);

	vector<S_DREBLINK_INFO> g_vDrebLinkInfo;
	CICertApiBase  *g_pCertClass;          //证书api操作类
	int           g_nUseCert;//是否使用证书 0不使用 1使用

	CBF_Xml  g_pXml;
	CCgateLog          g_pGateLog;//写cgate日志的线程类
	CBF_CommDataLog    g_pDrebLog;//写dreb日志的线程类

	// 函数名: GetMsg
	// 编程  : 王明松 2013-5-29 14:37:52
	// 返回  : std::string 
	// 描述  : 返回出错信息
	std::string GetMsg();

	char  m_sStartDate[20];

private:
	// 函数名: OpenDll
	// 编程  : 王明松 2013-5-30 15:07:45
	// 返回  : bool 
	// 参数  : DLLHANDLE *pDll  动态库句柄 
	// 参数  : const char *dllname
	// 参数  : const char *dllpath
	// 描述  : 打开动态库
	bool OpenDll(DLLHANDLE *pDll, const char *dllname, const char *dllpath);

	char          m_sErrMsg[256];

	DLLHANDLE     m_pDll;
};

#endif // !defined(AFX_GATERESOURCE_H__B9E4A73C_7ADB_404E_A678_7919376804D6__INCLUDED_)
