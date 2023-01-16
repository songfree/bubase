// DrebTbl.h: interface for the CDrebTbl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DREBTBL_H__AC496175_A6AC_4B82_AAD4_82853BFFA88A__INCLUDED_)
#define AFX_DREBTBL_H__AC496175_A6AC_4B82_AAD4_82853BFFA88A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MdbBase.h"
#include "BF_Mutex.h"
#include "SendQueue.h"
#include "IErrLog.h"

typedef struct 
{
	unsigned short nNodeId;        //目标数据总线节点编号
	char           cNodePrivateId; //数据总线节点私有序号
	unsigned int   nStep;          //0本地1直连的数据总线节点>1为数据总线节点间的步进   关闭时统一置为100
	unsigned int   nIndex;         //连接索引  通过此连接索引可得到数据总线节点的连接  0为本地或半闭的
	unsigned int   nbandwidth;     //带宽
	CSendQueue     *pSendQueue;    //发送队列
	bool           bIsClose;       //连接是否关闭
	int            nCloseTime;     //连接关闭时间，超过一定时间后将自动注销
}S_DREB_ROUTE;


//此表只存放本数据总线节点及相连的数据总线节点的数据
//直连的总线断开后并不删除，只是置bIsClose为true nStep标志为100，nIndex为0，当重连时重新使用此记录
class CDrebTbl  
{
public:
	CDrebTbl();
	virtual ~CDrebTbl();

public:


	// 函数名: SelectByIndex
	// 编程  : 王明松 2009-10-29 10:37:19
	// 返回  : bool 
	// 参数  : int index
	// 参数  : S_DREB_ROUTE &dreb
	// 描述  : 通过连接索引查找数据总线节点
	S_DREB_ROUTE * SelectByIndex(int index);

	//是连接索引取此连接需要路由的信息
	bool SelectRouteByIndex(int index, std::vector<S_DREB_ROUTE*>& dreb);

	/// 函数名: UnRegister
	/// 编程  : 王明松 2009-10-27 14:19:27
	/// 返回  : bool 
	/// 参数  : int nodeid
	/// 参数  : int privateid
	/// 描述  : 取消数据总线节点的注册，销毁发送队列
	bool UnRegister(int nodeid,int privateid);
	
	/// 函数名: OnClose
	/// 编程  : 王明松 2009-7-29 14:34:15
	/// 返回  : bool 
	/// 参数  : int nodeid
	/// 参数  : int privateid
	/// 描述  : 根据数据总线节点ID及私有ID关闭数据总线节点的信息，不删除且不销毁发送队列
	bool OnClose(int nodeid,int privateid);

	/// 函数名: Update
	/// 编程  : 王明松 2009-7-29 14:32:52
	/// 返回  : bool 
	/// 参数  : S_DREB_ROUTE dreb
	/// 描述  : 根据数据总线节点ID及私有ID更新数据总线节点信息，步进小于当前步进，发送队列若为空更新
	bool Update(S_DREB_ROUTE dreb);

	/// 函数名: Delete
	/// 编程  : 王明松 2009-7-29 14:32:31
	/// 返回  : bool 
	/// 参数  : int nodeid
	/// 参数  : int privateid
	/// 描述  : 根据数据总线节点ID及私有ID删除一条数据总线节点记录
	bool Delete(int nodeid,int privateid);

	/// 函数名: SelectPrivateid
	/// 编程  : 王明松 2009-7-29 14:31:23
	/// 返回  : bool 
	/// 参数  : int nodeid
	/// 参数  : int privateid
	/// 参数  : S_DREB_ROUTE &dreb
	/// 描述  : 根据数据总线节点ID及私有ID查找数据总线节点信息
	bool SelectPrivateid(int nodeid,int privateid,S_DREB_ROUTE &dreb);
	S_DREB_ROUTE * SelectPrivateid(int nodeid,int privateid);
	/// 函数名: SelectByNode
	/// 编程  : 王明松 2009-7-29 14:30:58
	/// 返回  : bool 
	/// 参数  : int nodeid
	/// 参数  : vector<S_DREB_ROUTE> &dreb
	/// 描述  : 根据数据总线节点ID查找所有的数据总线节点信息
	bool SelectByNode(int nodeid,std::vector<S_DREB_ROUTE> &dreb);

	// 函数名: SelectByNode
	// 编程  : 王明松 2016-1-13 10:35:14
	// 返回  : bool 
	// 参数  : int nodeid
	// 参数  : vector<S_DREB_ROUTE *> &dreb
	// 描述  : 取最小步进的总线节点信息  按带宽排序
	bool SelectByNode(int nodeid, std::vector<S_DREB_ROUTE *> &dreb);

	/// 函数名: Insert
	/// 编程  : 王明松 2009-7-29 14:30:35
	/// 返回  : bool 
	/// 参数  : S_DREB_ROUTE dreb
	/// 描述  : 插入一条数据总线节点记录
	bool Insert(S_DREB_ROUTE dreb);

	bool Select(std::vector<S_DREB_ROUTE*>& dreb);

	char m_errMsg[256];
	CIErrlog *m_pLog;
protected:
	
	CMemTableNew <S_DREB_ROUTE> m_table;        //数据总线节点内存表

 	CIndexInt<1>             m_index_node;   //数据总线节点id索引
 	CPkeyInt<2>             m_pkey;         //数据总线节点id+私有id索引，唯一索引
	CIndexInt<1>             m_index_index;   //数据总线节点连接索引
	CBF_Mutex m_mutex;
};



#endif // !defined(AFX_DREBTBL_H__AC496175_A6AC_4B82_AAD4_82853BFFA88A__INCLUDED_)
