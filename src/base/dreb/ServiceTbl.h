// ServiceTbl.h: interface for the CServiceTbl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERVICETBL_H__5398A809_0D06_4C51_B7DB_2E189754723A__INCLUDED_)
#define AFX_SERVICETBL_H__5398A809_0D06_4C51_B7DB_2E189754723A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MdbBase.h"
#include "BF_Mutex.h"
#include "BF_LogClient.h"
//#include "IErrLog.h"

typedef struct 
{
	unsigned short nNodeId;        //DREB编号
	char           cNodePrivateId; //DREB私有序号	
	unsigned int   nSvrMainId;     //注册在DREB的主服务功能号   只有服务调用时才用填写
	char           cSvrPrivateId;  //注册在DREB的私有序号 0表示公共 >0为每个服务的序号，s_nSvrMainId+s_cSvrPrivateId 即为私有功能号   只有服务调用时才用填写
	unsigned int   nFuncNo;        //服务功能号
	unsigned int   nStep;          //0本地1直连的数据总线节点>1为数据总线节点间的步进
	unsigned int   nIndex;         //连接索引  通过此连接索引可得到数据总线节点的连接
	unsigned int   nUpdateTime;    //功能的更新时间
	unsigned int   nbandwidth;     //带宽
}S_SERVICE_ROUTE;

class CServiceTbl  
{
public:

	

	// 函数名: UpdateRouteServiceByIndex
	// 编程  : 王明松 2013-10-28 14:37:42
	// 返回  : bool 
	// 参数  : int index dreb连接索引
	// 描述  : 更新交易路由信息，将连接上的过期交易删除
	bool UpdateRouteServiceByIndex(int index);

	// 函数名: GetRouteServiceByIndex
	// 编程  : 王明松 2010-9-7 14:47:39
	// 返回  : bool 
	// 参数  : int drebid
	// 参数  : int privateid
	// 参数  : int index
	// 参数  : vector<S_SERVICE_ROUTE> &rtlist
	// 描述  : 根据dreb连接序号取要在此连接上发送的服务路由信息
	bool GetRouteServiceByIndex(int drebid,int privateid,int index,std::vector<S_SERVICE_ROUTE> &rtlist);
	bool GetRouteServiceByIndex(int drebid,int privateid,int index,std::vector<S_SERVICE_ROUTE *> &rtlist);

	// 函数名: UnRegisterSvr
	// 编程  : 王明松 2010-9-7 10:26:06
	// 返回  : bool 
	// 参数  : int nodeid
	// 参数  : int nodeprivated
	// 参数  : int svrid
	// 参数  : int privateid
	// 描述  : 本dreb上的服务断开时取消服务及其上的功能
	bool UnRegisterSvr(int nodeid,int nodeprivated,int svrid,int privateid);

	// 函数名: DeleteByIndex
	// 编程  : 王明松 2010-9-7 10:24:39
	// 返回  : bool 
	// 参数  : int drebindex
	// 描述  : 服务路由时先根据路由过来的drebindex删除此drebindex上的服务功能信息
	bool DeleteByIndex(int drebindex);


	// 函数名: GetAllServiceOrderSvr
	// 编程  : 王明松 2010-7-27 11:28:05
	// 返回  : bool 
	// 参数  : vector<S_SERVICE_ROUTE *> &rtlist
	// 描述  : 获取所有的功能，按索引 dreb服务排序

	bool GetAllServiceOrderDrebSvr(std::vector<S_SERVICE_ROUTE *> &rtlist);

	// 函数名: SelectBySvr
	// 编程  : 王明松 2010-7-27 11:27:00
	// 返回  : bool 
	// 参数  : int svr
	// 参数  : int privatesvr
	// 参数  : vector<S_SERVICE_ROUTE> &rtlist
	// 描述  : 获取功能，只取1个
	bool SelectBySvr(int svr,int privatesvr , std::vector<S_SERVICE_ROUTE> &rtlist);
	bool SelectBySvr(int svr,int privatesvr , std::vector<S_SERVICE_ROUTE *> &rtlist);

	bool SelectByFuncSvr(int func,int svr, std::vector<S_SERVICE_ROUTE *> &rtlist);
	bool SelectByDrebFuncSvr(int drebid,int func,int svr, std::vector<S_SERVICE_ROUTE *> &rtlist);

	// 函数名: SelectBySvr
	// 编程  : 王明松 2010-7-27 11:26:45
	// 返回  : bool 
	// 参数  : int svr
	// 参数  : vector<S_SERVICE_ROUTE> &rtlist
	// 描述  : 通过服务号获取唯一的服务列表
	bool SelectBySvr(int svr, std::vector<S_SERVICE_ROUTE> &rtlist);
	bool SelectBySvr(int svr, std::vector<S_SERVICE_ROUTE *> &rtlist);


	bool Select(std::vector<S_SERVICE_ROUTE*>& rtlist);


	// 函数名: SelectByPk
	// 编程  : 王明松 2010-7-27 11:26:15
	// 返回  : bool 
	// 参数  : int index
	// 参数  : int nodeid
	// 参数  : int nodeprivated
	// 参数  : int svrid
	// 参数  : int privateid
	// 参数  : unsigned int func
	// 参数  : S_SERVICE_ROUTE &rt
	// 描述  : 获取功能
	bool SelectByPk(int index,int nodeid,int nodeprivated,int svrid, int privateid,unsigned int func,S_SERVICE_ROUTE &rt);

	// 函数名: SelectByPk
	// 编程  : 王明松 2010-7-27 11:26:15
	// 返回  : S_SERVICE_ROUTE *  NULL未找到，否则为指针
	// 参数  : int index
	// 参数  : int nodeid
	// 参数  : int nodeprivated
	// 参数  : int svrid
	// 参数  : int privateid
	// 参数  : unsigned int func
	// 描述  : 获取功能
	S_SERVICE_ROUTE * SelectByPk(int index,int nodeid,int nodeprivated,int svrid, int privateid,unsigned int func);
	// 函数名: Delete
	// 编程  : 王明松 2010-7-27 11:26:06
	// 返回  : bool 
	// 参数  : int index
	// 参数  : int nodeid
	// 参数  : int nodeprivated
	// 参数  : int svrid
	// 参数  : int privateid
	// 参数  : unsigned int func
	// 描述  : 删除一条功能
	bool Delete(int index,int nodeid,int nodeprivated,int svrid, int privateid,unsigned int func);

	// 函数名: Update
	// 编程  : 王明松 2010-7-27 11:25:38
	// 返回  : bool 
	// 参数  : S_SERVICE_ROUTE rt
	// 描述  : 更新功能，只更新nIndex nStep nUpdateTime三个字段
	bool Update(S_SERVICE_ROUTE rt);

	// 函数名: SelectByFunc
	// 编程  : 王明松 2010-7-27 11:25:48
	// 返回  : bool 
	// 参数  : unsigned int func
	// 参数  : vector<S_SERVICE_ROUTE> &rt
	// 描述  : 通过功能号获取功能列表
	bool SelectByFunc(unsigned int func, std::vector<S_SERVICE_ROUTE> &rt);

	// 函数名: SelectByFunc
	// 编程  : 王明松 2010-7-27 11:25:48
	// 返回  : bool 
	// 参数  : unsigned int func
	// 参数  : vector<S_SERVICE_ROUTE> &rt
	// 描述  : 通过功能号获取功能列表  结果按步进升序排序
	bool SelectByFunc(unsigned int func, std::vector<S_SERVICE_ROUTE*> &rt);

	// 函数名: Insert
	// 编程  : 王明松 2010-7-27 11:25:25
	// 返回  : bool 
	// 参数  : S_SERVICE_ROUTE rt
	// 描述  : insert一条功能
	bool Insert(S_SERVICE_ROUTE rt);
	CServiceTbl();
	virtual ~CServiceTbl();

	unsigned int m_nDeleteTime;//服务超时时间

	CIErrlog *m_pLog;
protected:
    // 函数名: DeleteById
    // 编程  : 王明松 2016-1-12 9:05:14
    // 返回  : bool 
    // 参数  : int rowid
    // 描述  : 根据rowid删除记录，包括索引
    bool DeleteById(int rowid);

	CMemTableNew <S_SERVICE_ROUTE> m_table;        //SERVICE 内存表

	CIndexInt<6>             m_pkey;         //index+dreb+dreb私有号+服务号+服务私有号+功能号，唯一索引

	CIndexInt<1>             m_index_func;   //服务功能号索引

	CIndexInt<1>             m_index_drebindex;   //dreb连接序号的索引

	CIndexInt<1>             m_index_svr;       //服务号索引

	CIndexInt<2>             m_index_svrpri;    //服务号索引

	CIndexInt<4>             m_index_route;         //dreb+dreb私有号+服务号+服务私有号

	CIndexInt<5>             m_index_indexroute;    //index,dreb+dreb私有号+服务号+服务私有号

	CIndexInt<2>             m_index_funcsvr;   //服务功能号+服务号索引

	CIndexInt<3>             m_index_drebfuncsvr;   //总线id+服务功能号+服务号索引
	CBF_Mutex m_mutex;	
};

#endif // !defined(AFX_SERVICETBL_H__5398A809_0D06_4C51_B7DB_2E189754723A__INCLUDED_)
