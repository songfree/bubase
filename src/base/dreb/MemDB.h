// MemDB.h: interface for the CMemDB class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MEMDB_H__8CF47224_C394_4279_858C_0ED49F03E2A5__INCLUDED_)
#define AFX_MEMDB_H__8CF47224_C394_4279_858C_0ED49F03E2A5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "BF_Tools.h"
#include "SvrTbl.h"
#include "DrebTbl.h"
#include "RouteTbl.h"
#include "ServiceTbl.h"
#include "BF_Mutex.h"
#include "IErrLog.h"
#include "BF_Date_Time.h"


typedef struct 
{
	unsigned short nNodeId;        //目标数据总线节点编号
	char           cNodePrivateId; //数据总线节点私有序号
	int            nStep;          //0本地1直连的数据总线节点>1为数据总线节点间的步进
	int            nIndex;         //连接索引  通过此连接索引可得到数据总线节点的连接
	CSendQueue     *pSendQueue;    //发送队列
	int            nbandwidth;     //带宽
	bool           bIsClose;       //连接是否关闭
	int            nCloseTime;     //连接关闭时间，超过一定时间后将自动注销
	unsigned int   nSvrMainId;     //注册在数据总线节点上的主服务功能号   只有服务调用时才用填写
	char           cSvrPrivateId;  //注册在数据总线节点上的私有序号 0表示公共 >0为每个服务的序号，s_nSvrMainId+s_cSvrPrivateId 即为私有功能号   只有服务调用时才用填写
}S_SERVICE_QUERY_RESULT;

class CMemDB  
{
public:

	// 函数名: SelectLocalBySvr
	// 编程  : 王明松 2016-1-13 13:34:07
	// 返回  : bool 
	// 参数  : unsigned int svr
	// 参数  : int privatesvr
	// 参数  : S_SERVICE_ROUTE &srt
	// 描述  : 根据服务在本地查询信息，供发送
	bool SelectLocalBySvr(unsigned int svr,int privatesvr, S_SERVICE_ROUTE &srt);

	// 函数名: SelectLocalBySvr
	// 编程  : 王明松 2016-1-13 13:34:03
	// 返回  : bool 
	// 参数  : unsigned int svr
	// 参数  : S_SERVICE_ROUTE &srt
	// 描述  : 根据服务在本地查询信息，供发送
	bool SelectLocalBySvr(unsigned int svr,S_SERVICE_ROUTE &srt);

	// 函数名: SelectLocalByFunc
	// 编程  : 王明松 2016-1-13 12:57:35
	// 返回  : bool 
	// 参数  : unsigned int func
	// 参数  : S_SERVICE_ROUTE &func
	// 描述  : 根据交易码在本地查询交易信息，供发送给对应的服务
	bool SelectLocalByFunc(unsigned int func,S_SERVICE_ROUTE &fc);

	// 函数名: GetAllRouteBc
	// 编程  : 王明松 2011-1-11 21:28:20
	// 返回  : bool 
	// 参数  : vector<S_ROUTE_TBL *> &rt
	// 描述  : 取要发送广播的总线节点信息(不包括本节点) 已经去重。
	bool GetAllRouteBc(std::vector<S_DREB_ROUTE *> &rt);


	// 函数名: SetLocalDrebInfo
	// 编程  : 王明松 2011-1-11 21:29:03
	// 返回  : bool 
	// 参数  : S_DREB_ROUTE rt
	// 描述  : //设置本地节点信息 (步进为0，index为0)
	bool SetLocalDrebInfo(S_DREB_ROUTE rt);

	// 函数名: DeleteRouteByIndex
	// 编程  : 王明松 2010-9-22 9:12:06
	// 返回  : bool 
	// 参数  : int index
	// 描述  : 根据连接序号删除总线路由，当总线断开时，要将此总线过来的总线路由删除
	bool DeleteRouteByIndex(int index);

	// 函数名: SetPara
	// 编程  : 王明松 2010-9-22 9:11:53
	// 返回  : void 
	// 参数  : int svrtime
	// 参数  : int servicetime
	// 描述  : 设置超时参数
	void SetPara(int svrtime,int servicetime);

	// 函数名: DeleteServiceByIndex
	// 编程  : 王明松 2010-9-14 15:08:49
	// 返回  : bool 
	// 参数  : int index
	// 描述  : 根据连接序号删除在这个连接序号路由过来的交易路由
	bool DeleteServiceByIndex(int index);

	// 函数名: UpdateRouteServiceByIndex
	// 编程  : 王明松 2010-9-14 15:10:01
	// 返回  : bool 
	// 参数  : int index
	// 描述  : 更新连接序号上的交易路由
	bool UpdateRouteServiceByIndex(int index);

	// 函数名: GetRouteServiceByIndex
	// 编程  : 王明松 2010-9-7 14:49:20
	// 返回  : bool 
	// 参数  : int drebid
	// 参数  : int privateid
	// 参数  : int index
	// 参数  : vector<S_SERVICE_ROUTE *> &rtlist
	// 描述  : 根据dreb连接序号取要在此连接上发送的服务路由信息
	bool GetRouteServiceByIndex(int drebid,int privateid,int index, std::vector<S_SERVICE_ROUTE *> &rtlist );

	// 函数名: UnRegisterService
	// 编程  : 王明松 2010-7-30 10:26:05
	// 返回  : bool 
	// 参数  : int nodeid
	// 参数  : int privateid
	// 描述  : 服务断开取消注册
	bool UnRegisterService(int nodeid,int privateid,int svr,int svrprivate);


	// 函数名: SelectRouteByFunc
	// 编程  : 王明松 2010-7-27 9:03:40
	// 返回  : bool 
	// 参数  : unsigned int func
	// 参数  : S_DREB_ROUTE &rt
	// 描述  : 根据功能号查找路由
	bool SelectRouteByFunc(unsigned int func,S_DREB_ROUTE &rt);

	// 函数名: SelectRouteBySvr
	// 编程  : 王明松 2010-7-27 15:23:10
	// 返回  : bool 
	// 参数  : int svr
	// 参数  : int privatesvr
	// 参数  : S_DREB_ROUTE &rt
	// 描述  : 根据服务查找路由
	bool SelectRouteBySvr(int svr,int privatesvr,S_DREB_ROUTE &rt);

	// 函数名: SelectRouteBySvr
	// 编程  : 王明松 2010-7-27 15:22:41
	// 返回  : bool 
	// 参数  : int svr
	// 参数  : S_DREB_ROUTE &rt
	// 描述  : 根据服务查找路由
	bool SelectRouteBySvr(int svr,S_DREB_ROUTE &rt);

	// 函数名: SelectRouteByFuncSvr
	// 编程  : 王明松 2010-7-27 15:22:41
	// 返回  : bool 
	// 参数  : int func 交易码
	// 参数  : int svr
	// 参数  : S_DREB_ROUTE &rt
	// 描述  : 根据交易码服务查找路由
	bool SelectRouteByFuncSvr(int func,int svr, S_DREB_ROUTE &rt);

	// 函数名: SelectRouteByFuncSvr
	// 编程  : 王明松 2010-7-27 15:22:41
	// 返回  : bool 
	// 参数  : int drebid 总线id
	// 参数  : int func 交易码
	// 参数  : int svr
	// 参数  : S_SERVICE_ROUTE &rt
	// 描述  : 根据交易码服务查找路由
	bool SelectRouteByDrebFuncSvr(int drebid,int func,int svr, S_SERVICE_ROUTE &rt);

	// 函数名: GetAllService
	// 编程  : 王明松 2010-7-27 11:44:18
	// 返回  : bool 
	// 参数  : vector<S_SERVICE_ROUTE *> &rtlist
	// 描述  : 获取所有的功能，按index dreb服务排序
	bool GetAllServiceOrder(std::vector<S_SERVICE_ROUTE *> &rtlist);

	
	// 函数名: RegisterFunc
	// 编程  : 王明松 2010-7-27 15:14:33
	// 返回  : bool 
	// 参数  : S_SERVICE_ROUTE rt
	// 描述  : 注册交易，此时不用根据dreb索引删除服务路由
	bool AddServiceRoute(S_SERVICE_ROUTE rt);


	// 函数名: GetAllDreb
	// 编程  : 王明松 2010-2-25 15:28:23
	// 返回  : bool 
	// 参数  : vector<S_DREB_ROUTE> &dp
	// 参数  : bool inclocal=false   false不包括本Dreb  true包括
	// 描述  : 获取所有连接的通讯平台，包不包括本DREB看参数
	bool GetAllDreb(std::vector<S_DREB_ROUTE *> &dp,bool inclocal=false);

	// 函数名: GetRouteList
	// 编程  : 王明松 2010-2-25 15:27:57
	// 返回  : bool 
	// 参数  : vector<S_ROUTE_TBL> &rt
	// 描述  : 获取路由过来和总线信息，不包括本地及直连接的总线
	bool GetRouteList(std::vector<S_DREB_ROUTE *> &rt);

	// 函数名: GetAllRoute
	// 编程  : 王明松 2010-2-25 15:27:57
	// 返回  : bool 
	// 参数  : vector<S_ROUTE_TBL> &rt
	// 描述  : 获取所有的路由，包括连接的通讯平台
	bool GetAllRoute(std::vector<S_DREB_ROUTE *> &rt);
	

	// 函数名: GetAllSvr
	// 编程  : 王明松 2009-12-4 16:53:26
	// 返回  : bool 
	// 参数  : vector<S_SVR_ROUTE> &svrlist
	// 描述  : 获取注册的所有服务
	bool GetAllSvr(std::vector<S_SVR_ROUTE> &svrlist);

	// 函数名: AddRoute
	// 编程  : 王明松 2009-10-30 10:46:20
	// 返回  : bool 
	// 参数  : int index
	// 参数  : vector<S_ROUTE_TBL>rtlist
	// 描述  : 更新index连接过来的路由，先删除再增加
	bool AddRoute(int index, std::vector<S_DREB_ROUTE>rtlist);

	// 函数名: GetRouteByIndex
	// 编程  : 王明松 2009-10-29 16:56:14
	// 返回  : bool 
	// 参数  : int index
	// 参数  : vector<S_ROUTE_TBL> &rt
	// 描述  : 获取通讯平台连接index上要发送的路由(即在路由表中除index外的路由都是要发送的)
	bool GetRouteByIndex(int index, std::vector<S_DREB_ROUTE *> &rt);
	

	// 函数名: SelectRoute
	// 编程  : 王明松 2009-10-29 10:28:56
	// 返回  : bool 
	// 参数  : int nodeid
	// 参数  : int privateid
	// 参数  : S_DREB_ROUTE *dreb
	// 描述  : 根据通讯平台私有ID查找路由
	bool SelectRoute(int nodeid,int privateid,S_DREB_ROUTE &dreb);

	// 函数名: SelectRoute
	// 编程  : 王明松 2009-10-29 10:28:37
	// 返回  : bool 
	// 参数  : int nodeid
	// 参数  : S_DREB_ROUTE &dreb
	// 描述  : 根据通讯平台ID查找路由
	bool SelectRoute(int nodeid,S_DREB_ROUTE &dreb);

	

	// 函数名: UnRegisterDreb
	// 编程  : 王明松 2009-10-29 9:44:09
	// 返回  : bool 
	// 参数  : int nodeid
	// 参数  : int privateid
	// 参数  : int index
	// 描述  : 注销通讯平台连接 ，删除路由及通讯平台连接表
	bool UnRegisterDreb(int nodeid, int privateid, int index);

	// 函数名: CloseDreb
	// 编程  : 王明松 2009-10-29 9:43:55
	// 返回  : bool 
	// 参数  : int nodeid
	// 参数  : int privateid
	// 参数  : int index
	// 描述  : 关闭通讯平台连接,删除路由并置通讯平台连接表的通讯平台状态为半闭
	bool CloseDreb(int nodeid,int privateid,int index);


	// 函数名: RegisterDreb
	// 编程  : 王明松 2009-10-29 9:42:41
	// 返回  : bool 
	// 参数  : int nodeid
	// 参数  : int privateid
	// 参数  : int index
	// 参数  : int bdrate
	// 参数  : CSendQueue **psque  返回的发送队列
	// 描述  : 注册连接的通讯平台
	bool RegisterDreb(int nodeid,int privateid, int index,int bdrate,CSendQueue **psque);

	/// 函数名: SelectSvr
	/// 编程  : 王明松 2009-10-27 15:20:15
	/// 返回  : bool 
	/// 参数  : int svrid
	/// 参数  : int privateid
	/// 参数  : S_SVR_ROUTE &svr
	/// 描述  : 根据服务ID,私有ID查找本节点注册的服务
	bool SelectSvr(int svrid,int privateid,S_SVR_ROUTE &svr);

	/// 函数名: SelectASvr
	/// 编程  : 王明松 2009-10-27 15:01:11
	/// 返回  : bool 
	/// 参数  : int svrid
	/// 参数  : S_SVR_ROUTE &svr
	/// 描述  : 根据服务号在本节点注册表查找出一个服务(有算法策略)
	bool SelectASvr(int svrid,S_SVR_ROUTE &svr);

	/// 函数名: UnRegisterSvr
	/// 编程  : 王明松 2009-10-27 14:57:43
	/// 返回  : bool 
	/// 参数  : int svrid
	/// 参数  : int privateid
	/// 描述  : 取消服务的注册
	bool UnRegisterSvr(int svrid,int privateid);

	/// 函数名: CloseSvr
	/// 编程  : 王明松 2009-10-27 14:56:46
	/// 返回  : bool 
	/// 参数  : int svrid
	/// 参数  : int privateid
	/// 描述  : 关闭服务
	bool CloseSvr(int svrid,int privateid);

	/// 函数名: RegisterSvr
	/// 编程  : 王明松 2009-10-27 14:41:21
	/// 返回  : bool 
	/// 参数  : int svrid      服务ID
	/// 参数  : int privateid  私有ID
	/// 参数  : int index  连接索引
	/// 参数  : CSendQueue **pque  成功注册返回的发送队列
	/// 描述  : 注册服务
	bool RegisterSvr(int svrid,int privateid,int index,CSendQueue **pque);


	CMemDB();
	virtual ~CMemDB();

	CDrebTbl  m_drebTbl;   //连接的通讯平台列表 包括本节点，但index及step为0
	CSvrTbl   m_svrTbl;    //注册的服务列表  本节点上注册的服务

	CRouteTbl m_routeTbl;  //路由表  不是直连的dreb存放于此  即总线路由表

	S_DREB_ROUTE m_pLocalDreb;//当前dreb信息

	CServiceTbl  m_serviceTbl;//交易列表  所有的交易列表，包括本地注册的交易

	CIErrlog    *m_pLog;
protected:
	
	int   m_nDeleteTime;        //服务过期时间
	int   m_nServiceDeleteTime; //服务功能过期时间
	
	// 函数名: AddRoute
	// 编程  : 王明松 2009-10-29 10:13:32
	// 返回  : bool 
	// 参数  : S_ROUTE_TBL rt
	// 描述  : 增加一条路由
	bool AddRoute(S_DREB_ROUTE rt);

	// 函数名: GetARoute
	// 编程  : 王明松 2009-10-29 11:26:52
	// 返回  : bool 
	// 参数  : vector<S_ROUTE_TBL *> rt   已按步进和带宽升序排序
	// 参数  : S_DREB_ROUTE &dreb
	// 描述  : 从路由列表中取出最适合条件的一条路由
	bool GetARoute(std::vector<S_DREB_ROUTE *> rt,S_DREB_ROUTE &dreb);

//	bool GetARoute(vector<S_SERVICE_QUERY_RESULT> rt,S_DREB_ROUTE &dreb,S_SVR_ROUTE &svr);
	// 函数名: GetARoute
	// 编程  : 王明松 2009-10-29 11:26:52
	// 返回  : bool 
	// 参数  : vector<S_DREB_ROUTE> rt  按带宽排序
	// 参数  : S_DREB_ROUTE &dreb
	// 描述  : 从路由列表中取出最适合条件的一条路由 
//	bool GetARoute(vector<S_DREB_ROUTE *> rt,S_DREB_ROUTE &dreb);

	bool GetARoute(std::vector<S_SERVICE_ROUTE *> rt, S_DREB_ROUTE &dreb);
	bool GetARoute(std::vector<S_SERVICE_ROUTE *> rt, S_SERVICE_ROUTE &dreb);
};

#endif // !defined(AFX_MEMDB_H__8CF47224_C394_4279_858C_0ED49F03E2A5__INCLUDED_)
