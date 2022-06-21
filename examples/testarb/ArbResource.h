// ArbResource.h: interface for the CArbResource class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ARBRESOURCE_H__7EB044C4_3163_4702_85BF_1C99CAA5C716__INCLUDED_)
#define AFX_ARBRESOURCE_H__7EB044C4_3163_4702_85BF_1C99CAA5C716__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define  APP_QUERY_HOST    9100001   //应用查询状态
#define  APP_PUSH_STATUS   9100003   //应用推送状态

#define  ARB_SEND_QUERY    9100002  //arb发起主机状态查询
#define  ARB_SEND_CONTROL  9100004  //arb发起指定主机
#define  ARB_SEND_UNREG_SVR    9100005  //arb发起取消服务在总线上的注册

#define   ARBSTATUS_UNKNOW       0  //未知状态
#define   ARBSTATUS_MASTER       1  //主机状态
#define   ARBSTATUS_MASTER_AYN   3  //主机同步状态  3-4对应，说明主机正在和备机在同步数据 
#define   ARBSTATUS_BACKUP       2  //备机状态
#define   ARBSTATUS_BACKUP_AYN   4  //备机同步状态


#include "BF_DrebResource.h"

class CArbResource : public CBF_DrebResource  
{
public:
	virtual bool Init(const char *confile);
	CArbResource();
	virtual ~CArbResource();

	bool          g_bToExit;
	std::string   m_sXdpDefineXml;//xdp定义文件
	int           m_nArbReportTime;//切换时间 即服务上报给仲裁的时间
	unsigned  int m_nSvrHostStatus;//服务的主备状态   

	std::vector<int> g_lSvrTxList;//服务在总线上注册的交易列表
};

#endif // !defined(AFX_ARBRESOURCE_H__7EB044C4_3163_4702_85BF_1C99CAA5C716__INCLUDED_)
