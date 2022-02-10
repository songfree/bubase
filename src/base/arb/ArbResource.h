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


#include "BF_DrebResource.h"

class CArbResource : public CBF_DrebResource  
{
public:
	virtual bool Init(const char *confile);
	CArbResource();
	virtual ~CArbResource();

	bool          g_bToExit;
	std::string   m_sXdpDefineXml;//xdp定义文件
	int           m_nSwitchTime;//切换时间 即主机在此时间未响应切换到备机的时间
	int           m_nSwitchSeg;//切换间隔   在这个时间点内不用检查是否切换

	vector<int> g_lArbTxList;//仲裁机在总线上注册的交易列表
};

#endif // !defined(AFX_ARBRESOURCE_H__7EB044C4_3163_4702_85BF_1C99CAA5C716__INCLUDED_)
