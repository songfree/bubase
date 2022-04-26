// RouteTbl.h: interface for the CRouteTbl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ROUTETBL_H__D6D9B85B_A348_4786_AA6B_B4D8E5112F50__INCLUDED_)
#define AFX_ROUTETBL_H__D6D9B85B_A348_4786_AA6B_B4D8E5112F50__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MdbBase.h"
#include "BF_Mutex.h"
#include "IErrLog.h"
#include "DrebTbl.h"

// typedef struct 
// {
// 	unsigned short nNodeId;        //目标数据总线节点编号
// 	char           cNodePrivateId; //数据总线节点私有序号
// 	unsigned int            nStep;          //0本地1直连的数据总线节点>1为数据总线节点间的步进
// 	unsigned int            nIndex;         //连接索引  通过此连接索引可得到数据总线节点的连接
// 	unsigned int            nbandwidth;     //带宽
// }S_ROUTE_TBL;

// typedef struct 
// {
// 	unsigned short nNodeId;        //目标数据总线节点编号
// 	char           cNodePrivateId; //数据总线节点私有序号
// 	int            nStep;          //0本地1直连的数据总线节点>1为数据总线节点间的步进   关闭时统一置为100
// 	int            nIndex;         //连接索引  通过此连接索引可得到数据总线节点的连接  0为本地或半闭的
// 	int            nbandwidth;     //带宽
// 	CSendQueue     *pSendQueue;    //发送队列
// 	bool           bIsClose;       //连接是否关闭
// 	int            nCloseTime;     //连接关闭时间，超过一定时间后将自动注销
// }S_DREB_ROUTE;

//此表存放数据总线节点的路由数据

class CRouteTbl  
{
public:

	//获取所有的路由信息按id pid排序
	bool  SelectById(std::vector<S_DREB_ROUTE*>& rtlist);

	// 函数名: GetRouteByIndex
	// 编程  : 王明松 2013-9-17 18:47:14
	// 返回  : bool 
	// 参数  : int index
	// 参数  : vector<S_ROUTE_TBL> &rtlist
	// 描述  : 通过连接索引获取路由(即除连接index外的路由都取出来)
	bool GetRouteByIndex(int index, std::vector<S_DREB_ROUTE *> &rtlist);

	// 函数名: UpdateRoute
	// 编程  : 王明松 2013-9-17 18:47:33
	// 返回  : bool 
	// 参数  : S_ROUTE_TBL rt
	// 描述  : 更新路由
	bool UpdateRoute(S_DREB_ROUTE rt);
	

	// 函数名: SelectByPrivateNode
	// 编程  : 王明松 2016-1-13 9:24:13
	// 返回  : bool 
	// 参数  : int nodeid
	// 参数  : int privateid
	// 参数  : vector<S_ROUTE_TBL *> &rt  仅返回最小步进的路由，若有多个相同最小步进的，按带宽从小大到排序
	// 描述  : 根据数据总线节点私有节点路由，若直接的断开则去路由表查找
	bool SelectByPrivateNode(int nodeid,int privateid, std::vector<S_DREB_ROUTE *> &rt);

	// 函数名: SelectByIndex
	// 编程  : 王明松 2009-10-28 16:29:15
	// 返回  : bool 
	// 参数  : int index
	// 参数  : vector<S_ROUTE_TBL> &rt
	// 描述  : 根据连接查找路由
	bool SelectByIndex(int index, std::vector<S_DREB_ROUTE *> &rt);

	// 函数名: SelectByNode
	// 编程  : 王明松 2009-10-28 16:28:52
	// 返回  : bool 
	// 参数  : int nodeid
	// 参数  : vector<S_ROUTE_TBL> &rt
	// 描述  : 根据数据总线节点ID查找所有路由
	bool SelectByNode(int nodeid, std::vector<S_DREB_ROUTE *> &rt);

	// 函数名: DeleteByIndex
	// 编程  : 王明松 2009-10-28 16:28:30
	// 返回  : bool 
	// 参数  : int index
	// 描述  : 根据连接删除路由
	bool DeleteByIndex(int index);

	// 函数名: DeleteByPk
	// 编程  : 王明松 2009-10-28 16:28:00
	// 返回  : bool 
	// 参数  : S_ROUTE_TBL rt
	// 描述  : 根据唯一索引删除，(四项)
	bool DeleteByPk(S_DREB_ROUTE rt);

	// 函数名: Update
	// 编程  : 王明松 2009-10-28 16:27:37
	// 返回  : bool 
	// 参数  : S_ROUTE_TBL rt
	// 描述  : 更新一条路由,只更新带宽
	bool Update(S_DREB_ROUTE rt);


	// 函数名: Insert
	// 编程  : 王明松 2009-10-28 16:27:22
	// 返回  : bool 
	// 参数  : S_ROUTE_TBL rt
	// 描述  : 插入一条路由
	bool Insert(S_DREB_ROUTE rt);
	CRouteTbl();
	virtual ~CRouteTbl();
	
	char m_errMsg[256];
	CIErrlog *m_pLog;
protected:
	
	CMemTableNew <S_DREB_ROUTE> m_table;        //数据总线节点内存表
	
	CIndexInt<1>             m_index_node;   //数据总线节点id索引
	CIndexInt<4>             m_pkey;         //数据总线节点id+私有id+连接序号+步进，唯一索引
	CIndexInt<1>             m_index_index;  //数据总线节点连接序号索引
	CIndexInt<2>             m_index_private;   //数据总线节点id+私有id索引
	
	
	CBF_Mutex m_mutex;
};


#endif // !defined(AFX_ROUTETBL_H__D6D9B85B_A348_4786_AA6B_B4D8E5112F50__INCLUDED_)
