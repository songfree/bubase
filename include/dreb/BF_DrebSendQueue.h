// BF_DrebSendQueue.h: interface for the CBF_DrebSendQueue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BF_DREBSENDQUEUE_H__ED7DF2D9_25FD_4088_8A89_EA3F81490F65__INCLUDED_)
#define AFX_BF_DREBSENDQUEUE_H__ED7DF2D9_25FD_4088_8A89_EA3F81490F65__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "BF_Mutex.h"
#include "BpcHead.h"


typedef std::list<S_BPC_RSMSG> RDQUEUE;

typedef struct
{
	RDQUEUE  datas;
	CBF_Mutex   m_mutex;
	CBF_MutexEvent m_pEvent;
} RQUEUEDATA;



#ifdef DREBAPI_EXPORTS
#define DREBAPI_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define DREBAPI_EXPORT __declspec(dllimport)
#else
#define DREBAPI_EXPORT 
#endif
#endif

class DREBAPI_EXPORT CBF_DrebSendQueue  
{
public:
	CBF_DrebSendQueue();
	virtual ~CBF_DrebSendQueue();
public:
	
	// 函数名: Clear
	// 编程  : 王明松 2013-10-18 9:17:36
	// 返回  : void 
	// 描述  : 清空队列
	void Clear();
	
	/// 函数名: DeleteSend
	/// 编程  : 王明松 2009-10-26 15:47:34
	/// 返回  : void 
	/// 参数  : int prio
	/// 参数  : RDQUEUE::iterator  data
	/// 描述  : 删除发送记录(发送成功或多次发送失败)
	void DeleteSend(RDQUEUE::iterator  data);
	
	/// 函数名: NeedSend
	/// 编程  : 王明松 2009-10-26 14:57:46
	/// 返回  : bool 
	/// 描述  : 是否需要发送数据
	bool NeedSend();
	
	
	
	/// 函数名: PutSendMsg
	/// 编程  : 王明松 2009-10-26 15:48:11
	/// 返回  : int <0失败  0成功
	/// 参数  : S_BPC_RSMSG msg
	/// 描述  : 放入一条发送信息入队列
	int PutSendMsg(S_BPC_RSMSG msg);
	
	RQUEUEDATA       m_qSendData;  //发送队列
	
protected:
	
	bool m_bIsNeedSend;     //是否需要发送数据
	int  m_nNeedSendNum;
	int m_sendNum;
};

#endif // !defined(AFX_BF_DREBSENDQUEUE_H__ED7DF2D9_25FD_4088_8A89_EA3F81490F65__INCLUDED_)
