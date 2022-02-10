// SvrTbl.h: interface for the CSvrTbl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SVRTBL_H__2C910750_3DE2_4D30_99E9_BAC6EFB6338B__INCLUDED_)
#define AFX_SVRTBL_H__2C910750_3DE2_4D30_99E9_BAC6EFB6338B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "MdbBase.h"
#include "BF_Mutex.h"
#include "SendQueue.h"
#include "IErrLog.h"

typedef struct 
{
	unsigned int   nSvrMainId;     //注册在数据总线节点上的主服务功能号   只有服务调用时才用填写
	char           cSvrPrivateId;  //注册在数据总线节点上的私有序号 0表示公共 >0为每个服务的序号，s_nSvrMainId+s_cSvrPrivateId 即为私有功能号   只有服务调用时才用填写
	unsigned int   nIndex;         //连接索引 0为服务取消 通过此连接索引可得到通讯平台的连接
	CSendQueue     *pSendQueue;    //发送队列
	bool           bIsClose;       //连接是否关闭
	int            nCloseTime;     //连接关闭时间，超过一定时间后将自动注销
	
}S_SVR_ROUTE;

//存放本地总线直连的服务信息,当注册时写入，断开后并不删除，只是置nIndex为0，bIsClose为true

class CSvrTbl  
{
public:
	// 函数名: Next
	// 编程  : 王明松 2009-10-29 17:08:54
	// 返回  : bool 
	// 参数  : S_SVR_ROUTE &svr
	// 描述  : 下一条记录
	bool Next(S_SVR_ROUTE &svr);
	
	// 函数名: First
	// 编程  : 王明松 2009-10-29 17:08:46
	// 返回  : bool 
	// 参数  : S_SVR_ROUTE &svr
	// 描述  : 第一条记录
	bool First(S_SVR_ROUTE &svr);

	/// 函数名: UnRegister
	/// 编程  : 王明松 2009-10-27 14:54:02
	/// 返回  : bool 
	/// 参数  : int svrid
	/// 参数  : int privateid
	/// 描述  : 注销服务，删除队列
	bool UnRegister(int svrid,int privateid);
	

	/// 函数名: OnClose
	/// 编程  : 王明松 2009-10-27 14:26:43
	/// 返回  : bool 
	/// 参数  : int svrid
	/// 参数  : int privateid
	/// 描述  : 关闭服务
	bool OnClose(int svrid,int privateid);

	/// 函数名: Update
	/// 编程  : 王明松 2009-10-27 14:26:32
	/// 返回  : bool 
	/// 参数  : S_SVR_ROUTE svr
	/// 描述  : 更新服务
	bool Update(S_SVR_ROUTE svr);

	/// 函数名: Delete
	/// 编程  : 王明松 2009-10-27 14:26:25
	/// 返回  : bool 
	/// 参数  : int svrid
	/// 参数  : int privateid
	/// 描述  : 删除记录
	bool Delete(int svrid,int privateid);

	/// 函数名: SelectPrivateid
	/// 编程  : 王明松 2009-10-27 14:26:08
	/// 返回  : bool 
	/// 参数  : int svrid
	/// 参数  : int privateid
	/// 参数  : S_SVR_ROUTE &svr
	/// 描述  : 通过服务ID，私有ID查询服务
	bool SelectPrivateid(int svrid,int privateid,S_SVR_ROUTE &svr);

	// 函数名: SelectPrivateid
	// 编程  : 王明松 2016-1-13 13:43:07
	// 返回  : S_SVR_ROUTE * 
	// 参数  : unsigned int svrid
	// 参数  : int privateid
	// 描述  : 通过服务ID取服务信息，去掉连接关闭的
	S_SVR_ROUTE * SelectPrivateid(unsigned int svrid, int privateid);

	/// 函数名: SelectBySvr
	/// 编程  : 王明松 2009-10-27 14:25:51
	/// 返回  : bool 
	/// 参数  : int svrid
	/// 参数  : vector<S_SVR_ROUTE> &svr
	/// 描述  : 通过服务ID查询服务记录
	bool SelectBySvr(int svrid,vector<S_SVR_ROUTE> &svr);

	// 函数名: SelectBySvr
	// 编程  : 王明松 2016-1-13 13:42:33
	// 返回  : bool 
	// 参数  : unsigned int svrid
	// 参数  : vector<S_SVR_ROUTE *> &svr
	// 描述  : 通过服务ID取服务信息，去掉连接关闭的
	bool SelectBySvr(unsigned int svrid,vector<S_SVR_ROUTE *> &svr);

	/// 函数名: Insert
	/// 编程  : 王明松 2009-10-27 14:25:26
	/// 返回  : bool 
	/// 参数  : S_SVR_ROUTE svr
	/// 描述  : 插入一条服务记录
	bool Insert(S_SVR_ROUTE svr);
	CSvrTbl();
	virtual ~CSvrTbl();

	CIErrlog *m_pLog;
protected:
	
	CMemTable <S_SVR_ROUTE> m_table;        //通讯平台内存表
	
	CIndexInt<1>             m_index_node;   //服务id索引
	CIndexInt<2>             m_pkey;         //服务id+私有id索引，唯一索引


//	CBF_Mutex m_mutex;	
};

#endif // !defined(AFX_SVRTBL_H__2C910750_3DE2_4D30_99E9_BAC6EFB6338B__INCLUDED_)
