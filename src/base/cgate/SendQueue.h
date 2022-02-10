// SendQueue.h: interface for the CSendQueue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SENDQUEUE_H__20329B7E_8E2B_485E_9161_864775BD05D9__INCLUDED_)
#define AFX_SENDQUEUE_H__20329B7E_8E2B_485E_9161_864775BD05D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "BF_Mutex.h"
#include "GateHead.h"

#define  PRIORITIES         3    //队列优先级   心跳为0路由为1  其它为2  

typedef std::list<S_GATEDREB_RSMSG> RDQUEUE;

typedef struct
{
	RDQUEUE  datas;
	CBF_Mutex   m_mutex;
	CBF_MutexEvent m_pEvent;
} RQUEUEDATA;



class  CSendQueue  
{
public:
	
	CSendQueue();
	virtual ~CSendQueue();
public:


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

	
	void Clear();

	/// 函数名: PutSendMsg
	/// 编程  : 王明松 2009-10-26 15:48:11
	/// 返回  : int 
	/// 参数  : S_BPC_RSMSG msg
	/// 参数  : int prio
	/// 描述  : 放入一条发送信息入队列
	int PutSendMsg(S_GATEDREB_RSMSG msg);

	RQUEUEDATA       m_qSendData;  //发送队列



protected:

};

#endif // !defined(AFX_SENDQUEUE_H__20329B7E_8E2B_485E_9161_864775BD05D9__INCLUDED_)
