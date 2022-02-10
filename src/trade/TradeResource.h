// ArbResource.h: interface for the CArbResource class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRADERESOURCE_H__7EB044C4_3163_4702_85BF_1C99CAA5C716__INCLUDED_)
#define AFX_TRADERESOURCE_H__7EB044C4_3163_4702_85BF_1C99CAA5C716__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define  APP_QUERY_HOST    9100001   //应用查询状态
#define  APP_PUSH_STATUS   9100003   //应用推送状态

#define  ARB_SEND_QUERY    9100002  //arb发起主机状态查询
#define  ARB_SEND_CONTROL  9100004  //arb发起指定主机
#define  ARB_SEND_UNREG_SVR    9100005  //arb发起取消服务在总线上的注册

#define  TRADEFRONT_BAEAKPOINT 70000030  //报盘机断点续传交易码


#define   ARBSTATUS_UNKNOW       0  //未知状态
#define   ARBSTATUS_MASTER       1  //主机状态
#define   ARBSTATUS_MASTER_AYN   3  //主机同步状态  3-4对应，说明主机正在和备机在同步数据 
#define   ARBSTATUS_BACKUP       2  //备机状态
#define   ARBSTATUS_BACKUP_AYN   4  //备机同步状态
#define   MASTERHOST             10  //负载均衡主机状态

#include "BF_DrebResource.h"
#include "BF_Queue.h"
#include "BF_3Des.h"

#define  DBPWDKEY   "rd402wms"
#define  DBPWD3DESKEY   "rd402wms20092017Ylink123" 

#ifdef __cplusplus
extern "C" {
#endif
	
	//字节对齐
#ifdef _WINDOWS
#define __PACKED__ 
#pragma pack(push,1)
#else
#ifdef __GNUC__
#define __PACKED__	__attribute__ ((packed))
	//#pragma pack(push,1)
#else
#define __PACKED__
#pragma options align=packed
#endif
#endif
	
typedef struct
{
	long           nSerial_get;   //本地报单号
	unsigned int   serial;        //出入金流水号

}__PACKED__ S_SHM_PUBLIC;
	//取消字节对齐
#ifdef _WINDOWS
#pragma pack(pop)
#else
#ifdef __GNUC__
	//#pragma pack(pop)
#else
#pragma options align=reset
#endif
#endif
#undef __PACKED__
	
#ifdef __cplusplus
}
#endif

class CTradeResource : public CBF_DrebResource  
{
public:
	int  GetSerial();
	long GetLocalNo();

	CIErrlog  *m_log;

	virtual bool Init(const char *confile);
	CTradeResource();
	virtual ~CTradeResource();


	bool          g_bToExit;
	std::string   m_sXdpDefineXml;//xdp定义文件
	int           m_nTradeReportTime;//切换时间 即服务上报给仲裁的时间
	unsigned  int m_nSvrHostStatus;//服务的主备状态 
	int           m_nArbMode;//仲裁模式  1为主备  
	
	int            g_nListenPort;//侦听端口
	int            g_nMaxConnectNum;//最大的连接数
	
	int            g_nProcTheadNum;//业务处理线程数


	std::vector<int> g_lSvrTxList;//服务在总线上注册的交易列表

	std::vector<int> g_lArbTxList;//服务在总线上注册的仲裁交易

	CBF_Des       m_pDes;
	CBF_3Des      m_p3Des;

	int           g_nStatus;//程序状态 0正在初始化  1正在启动    3正常，启动完成

	S_SHM_PUBLIC *m_pShmpub;//共享内存中的数据

	CBF_Mutex   m_mutex;
};

#endif // !defined(AFX_TRADERESOURCE_H__7EB044C4_3163_4702_85BF_1C99CAA5C716__INCLUDED_)
