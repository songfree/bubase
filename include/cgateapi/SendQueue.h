// SendQueue.h: interface for the CSendQueue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SENDQUEUE_H__20329B7E_8E2B_485E_9161_864775BD05D9__INCLUDED_)
#define AFX_SENDQUEUE_H__20329B7E_8E2B_485E_9161_864775BD05D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GateHead.h"
#include "BF_Mutex.h"

#ifdef CGATEAPI_EXPORTS
#define CGATEAPI_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define CGATEAPI_EXPORT __declspec(dllimport)
#else
#define CGATEAPI_EXPORT 
#endif
#endif

#ifdef CGATEAPI_EXPORTS
#define CGATEAPI_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define CGATEAPI_EXPORT __declspec(dllimport)
#else
#define CGATEAPI_EXPORT 
#endif
#endif

typedef std::list<S_CGATEAPI_SMSG> CGATE_SQUEUE;

typedef struct
{
	CGATE_SQUEUE   datas;
	CBF_Mutex      m_mutex;
} CGATE_SQUEUEDATA;

class CGATEAPI_EXPORT CSendQueue  
{
public:
	
	CSendQueue();
	virtual ~CSendQueue();
public:


	/// 函数名: DeleteSend
	/// 编程  : 王明松 2009-10-26 15:47:34
	/// 返回  : void 
	/// 参数  : RDQUEUE::iterator  data
	/// 描述  : 删除发送记录(发送成功或多次发送失败)
	void DeleteSend(CGATE_SQUEUE::iterator  data);

	/// 函数名: NeedSend
	/// 编程  : 王明松 2009-10-26 14:57:46
	/// 返回  : bool 
	/// 描述  : 是否需要发送数据
	bool NeedSend();



	/// 函数名: PutSendMsg
	/// 编程  : 王明松 2009-10-26 15:48:11
	/// 返回  : int 
	/// 参数  : S_DREB_RSMSG msg
	/// 描述  : 放入一条发送信息入队列
	int PutSendMsg(S_CGATEAPI_SMSG msg);

	
	CGATE_SQUEUEDATA       m_qSendData;  //发送队列
protected:

	

	bool m_bIsNeedSend;     //是否需要发送数据
	int  m_nNeedSendNum;
	int m_sendNum;
};

#endif // !defined(AFX_SENDQUEUE_H__20329B7E_8E2B_485E_9161_864775BD05D9__INCLUDED_)
