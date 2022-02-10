// TibSendMgr.h: interface for the CTibSendMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIBSENDMGR_H__576E2AA0_6032_4FB4_A588_6CF88018B3BA__INCLUDED_)
#define AFX_TIBSENDMGR_H__576E2AA0_6032_4FB4_A588_6CF88018B3BA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "public.h"
#include "BF_Xml.h"
#include "BF_SocketUdp.h"
#include "IErrLog.h"
#include "TibHead.h"
#include "PortTbl.h"
#include "UdpSocketInfo.h"

#ifdef TIB_EXPORTS
#define TIB_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define TIB_EXPORT __declspec(dllimport)
#else
#define TIB_EXPORT 
#endif
#endif

typedef struct 
{
	int  msgtype;
	CUdpSocketInfo *pUdpInfo;
} S_TIB_SEND_TBL;

//发送管理
class TIB_EXPORT CTibSendMgr  
{
public:

	// 函数名: IsMsgtype
	// 编程  : 王明松 2015-7-16 16:25:11
	// 返回  : bool 
	// 参数  : int msgtype
	// 描述  : 消息类型是否存在
	bool IsMsgtype(int msgtype);

	// 函数名: SendData
	// 编程  : 王明松 2015-7-16 16:25:00
	// 返回  : int 
	// 参数  : TIBCOMMSTRU data
	// 描述  : 发送数据
	int SendData(TIBCOMMSTRU data);
	

	// 函数名: Init
	// 编程  : 王明松 2015-6-30 14:42:16
	// 返回  : bool 
	// 参数  : CPortTbl *portlist
	// 参数  : CIErrlog *errlog
	// 参数  : CBF_Xml *tibxml
	// 描述  : 初始化
	bool Init(CPortTbl *portlist,CIErrlog *errlog,CBF_Xml *tibxml);

	CTibSendMgr();
	virtual ~CTibSendMgr();

protected:

	//消息类型对应的端口
	bool     m_bIsInit;
	
	
	CPortTbl   *m_pListPort;//端口列表
	CIErrlog   *m_pLog;//日志类
	std::string m_sSendAddr;//发送地址，如172.168.255.255  即广播地址
	
	std::vector<CUdpSocketInfo *> m_pSendInfo;//要发送的端口列表

	CMemTable<S_TIB_SEND_TBL> m_pMsgTbl;//消息类型对应的端口类表
	CPkeyInt<1>               m_pMsgKey;

	TIBCOMMSTRU               m_sSendData;
	
	CBF_Mutex                 m_mutex;
private:
	CUdpSocketInfo * SelectByMsgtype(int msgtype);
	bool Insert(S_TIB_SEND_TBL tbl);
};

#endif // !defined(AFX_TIBSENDMGR_H__576E2AA0_6032_4FB4_A588_6CF88018B3BA__INCLUDED_)
