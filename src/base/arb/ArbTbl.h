// ArbTbl.h: interface for the CArbTbl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ARBTBL_H__CB8A3EEC_D62E_44EE_98D7_1CC3098F2B8D__INCLUDED_)
#define AFX_ARBTBL_H__CB8A3EEC_D62E_44EE_98D7_1CC3098F2B8D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MdbBase.h"
#include "IErrLog.h"

#define   ARBSTATUS_UNKNOW       0  //未知状态
#define   ARBSTATUS_MASTER       1  //主机状态
#define   ARBSTATUS_MASTER_AYN   3  //主机同步状态  3-4对应，说明主机正在和备机在同步数据 
#define   ARBSTATUS_BACKUP       2  //备机状态
#define   ARBSTATUS_BACKUP_AYN   4  //备机同步状态

//记录应用主备的信息表
typedef struct 
{
	unsigned short nNodeId;        //目标数据总线节点编号
	unsigned int   nSvrMainId;     //注册在数据总线节点上的主服务功能号   只有服务调用时才用填写
	char           cSvrPrivateId;  //注册在数据总线节点上的私有序号 0表示公共 >0为每个服务的序号，s_nSvrMainId+s_cSvrPrivateId 即为私有功能号   只有服务调用时才用填写
	unsigned int   nUpdateTime;       //最后一次更新时间 即应用通知的时间 秒
	char           cStatus;           //状态
	unsigned int   nLastSwtichTime;  //最近切换时间 秒
	unsigned int   nQueryStatusTime;  //最后一次查询应用状态时间 秒
	unsigned int   nAddTime;          //加入表时间 秒
}  ARB_S_APPINFO;        //应用主备信息表

class CArbTbl  
{
public:
	void DeleteApp(unsigned int svr,char pid);

	// 函数名: GetAppInfo
	// 编程  : 王明松 2015-12-30 9:48:09
	// 返回  : ARB_S_APPINFO * 
	// 参数  : unsigned int svrid
	// 参数  : char pid
	// 描述  : 取某一应用的信息
	ARB_S_APPINFO * GetAppInfo(unsigned int svrid,char pid);

	// 函数名: GetAppInfo
	// 编程  : 王明松 2015-12-30 8:55:59
	// 返回  : bool 
	// 参数  : int svrid
	// 参数  : std::vector<ARB_S_APPINFO *> &applist
	// 描述  : 根据服务ID取所有的服务信息
	bool GetAppInfo(unsigned int svrid,std::vector<ARB_S_APPINFO *> &applist);

	// 函数名: GetDistinctApp
	// 编程  : 王明松 2015-12-29 17:04:26
	// 返回  : bool 
	// 参数  : std::vector<int> &applist
	// 描述  : 取唯一的应用列表
	bool GetDistinctApp(std::vector<int> &applist);

	// 函数名: GetAppList
	// 编程  : 王明松 2015-12-29 14:42:17
	// 返回  : bool 
	// 描述  : 取所有的应用信息，按app排序
	bool GetAppList(std::vector<ARB_S_APPINFO> &applist);

	// 函数名: UpdateArb
	// 编程  : 王明松 2015-12-29 14:42:02
	// 返回  : ARB_S_APPINFO * 
	// 参数  : unsigned int svr
	// 参数  : char pid
	// 参数  : char status
	// 参数  : unsigned short drebid
	// 描述  : 更新app的信息
	ARB_S_APPINFO * UpdateArb(unsigned int svr,char pid,char status,unsigned short drebid);

	CArbTbl();
	virtual ~CArbTbl();

	CIErrlog    *m_pLog;

protected:
	CMemTableNew <ARB_S_APPINFO> m_table;        //服务内存表

	CIndexUInt<1>             m_index_appid;   //服务id索引
	CPkeyUInt<2>              m_pkey;         //服务id+私有id索引，唯一索引
	
};

#endif // !defined(AFX_ARBTBL_H__CB8A3EEC_D62E_44EE_98D7_1CC3098F2B8D__INCLUDED_)
