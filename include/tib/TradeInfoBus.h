// TradeInfoBus.h: interface for the TradeInfoBus class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRADEINFOBUS_H__33ABF484_F174_4DFD_9D27_8B245E822BAA__INCLUDED_)
#define AFX_TRADEINFOBUS_H__33ABF484_F174_4DFD_9D27_8B245E822BAA__INCLUDED_

#include "public.h"
#include "BF_Xml.h"
#include "BF_SocketUdp.h"
#include "IErrLog.h"

#include "BF_Thread.h"
#include "TibHead.h"
#include "PortTbl.h"
#include "TibRcvMgr.h"
#include "TibSendMgr.h"


#if _MSC_VER > 100
#pragma once
#endif // _MSC_VER > 1000

#ifdef TIB_EXPORTS
#define TIB_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define TIB_EXPORT __declspec(dllimport)
#else
#define TIB_EXPORT 
#endif
#endif


class TIB_EXPORT CTradeInfoBus  
{
public:
	bool IsMsgtype(int msgtype);

	// 函数名: MonitorRecv
	// 编程  : 王明松 2015-7-1 15:14:47
	// 返回  : void 
	// 描述  : 监控交易总线接收线程，若停止则启动
	void MonitorRecv();

	// 函数名: StopRecv
	// 编程  : 王明松 2015-7-1 15:14:35
	// 返回  : void 
	// 描述  : 停止交易总线接收线程
	void StopRecv();

	// 函数名: StartRecv
	// 编程  : 王明松 2015-7-1 15:14:21
	// 返回  : bool 
	// 描述  : 启动交易总线接收线程
	bool StartRecv();

	// 函数名: GetQueueSize
	// 编程  : 王明松 2015-7-1 14:11:56
	// 返回  : int 
	// 描述  : 取得队列待处理数据大小
	int GetQueueSize();

	// 函数名: GetData
	// 编程  : 王明松 2015-7-1 14:10:27
	// 返回  : int 
	// 参数  : TIBCOMMSTRU &data
	// 描述  : 获取数据进
	int GetData(TIBCOMMSTRU &data);

	// 函数名: SendData
	// 编程  : 王明松 2015-6-18 10:49:52
	// 返回  : int 
	// 参数  : TIBCOMMSTRU data
	// 描述  : 发送数据至交易总线
	int SendData(TIBCOMMSTRU data);

	// 函数名: GetPara
	// 编程  : 王明松 2015-6-18 10:45:35
	// 返回  : bool 
	// 参数  : int &apptype
	// 参数  : int &apppubid
	// 参数  : int &appprivateid
	// 描述  : 取应用类型，应用ID参数
	bool GetPara(int &apptype, int &apppubid,int &appprivateid);

	// 函数名: Init
	// 编程  : 王明松 2015-6-15 11:35:13
	// 返回  : bool 
	// 参数  : CIErrlog *errlog
	// 参数  : const char *tibxml
	// 描述  : 初始化，读取xml配置文件中的交易端口配置、应用配置
	bool Init(CIErrlog *errlog,const char *tibxml);

		
	CTradeInfoBus(unsigned short apptype);
	CTradeInfoBus();
	virtual ~CTradeInfoBus();
	

protected:
	
	bool      m_bIsInit;

	CIErrlog *m_pLog;          //日志类

	CPortTbl    m_pListPort;  //端口列表

	unsigned short  m_nAppType;//应用程序类型
	unsigned int    m_nAppPublicId;  //应用公共ID
	unsigned int    m_nAppPrivateId; //应用私有ID

	CTibRcvMgr      m_pRcvMgr;  //接收管理
	CTibSendMgr     m_pSendMgr; //发送管理


};

#endif // !defined(AFX_TRADEINFOBUS_H__33ABF484_F174_4DFD_9D27_8B245E822BAA__INCLUDED_)
