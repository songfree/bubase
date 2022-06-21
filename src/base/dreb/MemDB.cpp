// MemDB.cpp: implementation of the CMemDB class.
//
//////////////////////////////////////////////////////////////////////

#include "MemDB.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMemDB::CMemDB()
{
	m_nDeleteTime  = 300;//5分钟
	m_nServiceDeleteTime = 60;
	memset(&m_pLocalDreb,0,sizeof(S_DREB_ROUTE));

	m_pLog = NULL;
}

CMemDB::~CMemDB()
{
	m_pLog = NULL;
}

void CMemDB::SetPara(int svrtime, int servicetime)
{
	m_nDeleteTime = svrtime;
	m_nServiceDeleteTime = servicetime;
	m_serviceTbl.m_nDeleteTime  = servicetime;
	m_drebTbl.m_pLog = m_pLog;
	m_svrTbl.m_pLog = m_pLog;
	m_serviceTbl.m_pLog = m_pLog;
	m_routeTbl.m_pLog = m_pLog;
	
}
bool CMemDB::RegisterSvr(int svrid, int privateid, int index,CSendQueue **pque)
{
	S_SVR_ROUTE svr;
	if (!m_svrTbl.SelectPrivateid(svrid,privateid,svr))
	{
		//新注册
		svr.bIsClose = false;
		svr.cSvrPrivateId = privateid;
		svr.nCloseTime = 0;
		svr.nIndex = index;
		svr.nSvrMainId = svrid;
		svr.pSendQueue = new CSendQueue();
		*pque = svr.pSendQueue;
		return m_svrTbl.Insert(svr);
	}
	m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"服务[%d %d]对应的队列地址[%d]",svrid,privateid,svr.pSendQueue);
	if (!svr.bIsClose)
	{
		//已经注册
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"服务[%d %d]已注册并且未断开",svrid,privateid);
		return false;
	}
	if (time(NULL) - svr.nCloseTime < m_nDeleteTime) //关闭但未超时
	{
		svr.bIsClose = false;
		svr.nIndex = index;
		m_svrTbl.Update(svr);
		*pque = svr.pSendQueue;
		return true;
	}
	//已超时，先注销再注册
//20150723这里不能delete，在表里没有重置，否则死掉
// 	if (svr.pSendQueue != NULL)
// 	{
// 		delete svr.pSendQueue;
// 		svr.pSendQueue = NULL;
// 	}
	
	m_svrTbl.UnRegister(svrid,privateid);
	//新注册
	svr.bIsClose = false;
	svr.cSvrPrivateId = privateid;
	svr.nCloseTime = 0;
	svr.nIndex = index;
	svr.nSvrMainId = svrid;
	svr.pSendQueue = new CSendQueue();
	*pque = svr.pSendQueue;
	m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"新注册后服务[%d %d]对应的队列地址[%d]",svrid,privateid,svr.pSendQueue);
	return m_svrTbl.Insert(svr);

}

bool CMemDB::CloseSvr(int svrid, int privateid)
{
	//删除功能列表
	return m_svrTbl.OnClose(svrid,privateid);
}

bool CMemDB::UnRegisterSvr(int svrid, int privateid)
{
	//删除功能列表
	return m_svrTbl.UnRegister(svrid,privateid);
}

bool CMemDB::SelectASvr(int svrid, S_SVR_ROUTE &svr)
{
	int id;
	std::vector<S_SVR_ROUTE> svrlist;
	std::vector<int>s_list;
	std::vector<int>c_list;
	if (!m_svrTbl.SelectBySvr(svrid,svrlist))
	{
		return false;
	}
	if (svrlist.size()<1)
	{
		return false;
	}
	if (svrlist.size() == 1)
	{
		svr = svrlist[0];
		return true;
	}
	//使用随机查找一个
	
	for (unsigned int i=0;i<svrlist.size();i++)
	{
		if (svrlist[i].bIsClose && (time(NULL) - svrlist[i].nCloseTime > m_nDeleteTime))
		{
			m_svrTbl.UnRegister(svrlist[i].nSvrMainId,svrlist[i].cSvrPrivateId);//取消注册
		}
		else if (svrlist[i].bIsClose)
		{
			c_list.push_back(i); //加入关闭连接列表
		}
		else
		{
			s_list.push_back(i);//加入连接正常列表
		}
	}
	if (s_list.size() <1 && c_list.size()<1) //无服务
	{
		return false;
	}
	if (s_list.size() == 1)  //有正常一个则返回
	{
		svr = svrlist[s_list[0]];
		return true;
	}
	else if (s_list.size()>1)  //有正常多个则随机取一个
	{
		srand(CBF_Date_Time::GetTickCount());
		id = rand() % (s_list.size());
		svr = svrlist[id];
		return true;
	}
	else if (c_list.size() == 1)  //无正常且关闭有一个，则返回
	{
		svr = svrlist[c_list[0]];
		return true;
	}
	else if (c_list.size() > 1) //无正常且关闭有多个，随机取一个返回
	{
		srand(CBF_Date_Time::GetTickCount());
		id = rand() % (c_list.size());
		svr = svrlist[id];
		return true;
	}
	else
	{
		return false;
	}
	
}

bool CMemDB::SelectSvr(int svrid, int privateid, S_SVR_ROUTE &svr)
{
	if (!m_svrTbl.SelectPrivateid(svrid,privateid,svr))
	{
		return false;
	}
	if (svr.bIsClose && (time(NULL) - svr.nCloseTime > m_nDeleteTime)) //连接关闭且超时
	{
		m_svrTbl.UnRegister(svrid,privateid);
		return false;
	}
	return true;
}

bool CMemDB::RegisterDreb(int nodeid, int privateid, int index,int bdrate, CSendQueue **psque)
{
	S_DREB_ROUTE dreb;
	if (!m_drebTbl.SelectPrivateid(nodeid,privateid,dreb))
	{
		//新注册
		dreb.bIsClose = false;
		dreb.cNodePrivateId = privateid;
		dreb.nCloseTime = 0;
		dreb.nStep = 1;
		dreb.nIndex = index;
		dreb.nNodeId = nodeid;
		dreb.nbandwidth =  bdrate;
		dreb.pSendQueue = new CSendQueue();
		*psque = dreb.pSendQueue;
		m_routeTbl.DeleteByIndex(index);//删除该index所有的总线路由
		return m_drebTbl.Insert(dreb);
	}
	if (!dreb.bIsClose)
	{
		//已经注册
		return false;
	}
	if (time(NULL) - dreb.nCloseTime < m_nDeleteTime) //关闭但未超时
	{
		dreb.bIsClose = false;
		dreb.nIndex = index;
		dreb.nStep = 1;
		*psque = dreb.pSendQueue;
		m_drebTbl.Update(dreb);
		return true;
	}
	//已超时，先注销再注册
	m_drebTbl.UnRegister(nodeid,privateid);
	m_routeTbl.DeleteByIndex(index);//删除该index所有的路由
	//新注册
	dreb.bIsClose = false;
	dreb.cNodePrivateId = privateid;
	dreb.nCloseTime = 0;
	dreb.nStep = 1;
	dreb.nIndex = index;
	dreb.nNodeId = nodeid;
	dreb.nbandwidth =  bdrate;
	dreb.pSendQueue = new CSendQueue();
	*psque = dreb.pSendQueue;

	return m_drebTbl.Insert(dreb);
}

bool CMemDB::CloseDreb(int nodeid, int privateid, int index)
{
	m_routeTbl.DeleteByIndex(index);
	return m_drebTbl.OnClose(nodeid,privateid);
}

bool CMemDB::UnRegisterDreb(int nodeid, int privateid, int index)
{
	m_routeTbl.DeleteByIndex(index);
	return m_drebTbl.UnRegister(nodeid,privateid);
}

bool CMemDB::AddRoute(S_DREB_ROUTE rt)
{
	if (NULL == m_drebTbl.SelectByIndex(rt.nIndex))
	{
		return false;
	}
	//同一个index只保留最小step的
	return m_routeTbl.UpdateRoute(rt);
}

bool CMemDB::SelectRoute(int nodeid, S_DREB_ROUTE &dreb)
{
	std::vector<S_DREB_ROUTE *>dl;
	//先从连接的通讯平台查找
	if (m_drebTbl.SelectByNode(nodeid,dl))
	{
		return GetARoute(dl,dreb);
	}
	std::vector<S_DREB_ROUTE *>rt;
	if (!m_routeTbl.SelectByNode(nodeid,rt))
	{
		return false;
	}
	return GetARoute(rt,dreb);
}

bool CMemDB::SelectRoute(int nodeid, int privateid, S_DREB_ROUTE &dreb)
{
	//先从连接的通讯平台查找
	S_DREB_ROUTE *dr = m_drebTbl.SelectPrivateid(nodeid,privateid);
	if (dr != NULL)
	{
		//直连的总线未断开或为本节点，则直接返回
		if (!dr->bIsClose || 0 == dreb.nStep)
		{
			dreb.bIsClose = dr->bIsClose;
			dreb.cNodePrivateId = dr->cNodePrivateId;
			dreb.nbandwidth = dr->nbandwidth;
			dreb.nCloseTime = dr->nCloseTime;
			dreb.nIndex = dr->nIndex;
			dreb.nNodeId = dr->nNodeId;
			dreb.nStep = dr->nStep;
			dreb.pSendQueue = dr->pSendQueue;
			m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"本地或直连的路由 总线路由[%d %d] nStep[%d] nIndex[%d]",\
				dreb.nNodeId,dreb.cNodePrivateId,dreb.nStep,dreb.nIndex);
			return true;
		}
		m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"指定的总线节点[%d %d] 已断开 nStep[%d] 尝试从路由表里查找路由",nodeid,privateid,dr->nStep);
		//尝试从路由表里找找看吧
	}
	std::vector<S_DREB_ROUTE *>rt;
	if (!m_routeTbl.SelectByPrivateNode(nodeid,privateid,rt))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"指定的总线节点[%d %d] 在总线路由表里查找不存在",nodeid,privateid);
		return false;
	}
	if (1 == rt.size())
	{
		dreb.bIsClose = false;
		dreb.cNodePrivateId = rt[0]->cNodePrivateId;
		dreb.nbandwidth = rt[0]->nbandwidth;
		dreb.nIndex = rt[0]->nIndex;
		dreb.nNodeId = rt[0]->nNodeId;
		dreb.nStep = rt[0]->nStep;
		dreb.pSendQueue = NULL;
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"总线路由表路由 总线路由[%d %d] nStep[%d] nIndex[%d]",\
				dreb.nNodeId,dreb.cNodePrivateId,dreb.nStep,dreb.nIndex);
		return true;
	}
	return GetARoute(rt,dreb);
}

bool CMemDB::GetARoute(std::vector<S_DREB_ROUTE *> rt, S_DREB_ROUTE &dreb)
{
	int i;
	//因为已按步进和带宽排序，所以只需要比较带宽即可
	int rtsize= rt.size();
	if (rtsize<1)
	{
		return false;
	}
	if (1 == rtsize)
	{
		dreb.bIsClose = false;
		dreb.cNodePrivateId = rt[0]->cNodePrivateId;
		dreb.nbandwidth = rt[0]->nbandwidth;
		dreb.nIndex = rt[0]->nIndex;
		dreb.nNodeId = rt[0]->nNodeId;
		dreb.nStep = rt[0]->nStep;
		dreb.pSendQueue = NULL;
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"总线路由表路由 总线路由[%d %d] nStep[%d] nIndex[%d]",\
				dreb.nNodeId,dreb.cNodePrivateId,dreb.nStep,dreb.nIndex);
		return true;
	}
	int lastIndex=-1;
	unsigned int bd=rt[rtsize-1]->nbandwidth;//最大的一个带宽
	for (i=rtsize-2 ; i>=0 ; i--)
	{
		if (rt[i]->nbandwidth < bd) 
		{
			//有小的就退出，不用再找了
			lastIndex = i;//最小
			break;
		}
	}
	int resNum= rtsize-1 - lastIndex;

	if (1 == resNum ) //说明只有一个
	{
		dreb.bIsClose = false;
		dreb.cNodePrivateId = rt[rtsize-1]->cNodePrivateId;
		dreb.nbandwidth = rt[rtsize-1]->nbandwidth;
		dreb.nIndex = rt[rtsize-1]->nIndex;
		dreb.nNodeId = rt[rtsize-1]->nNodeId;
		dreb.nStep = rt[rtsize-1]->nStep;
		dreb.pSendQueue = NULL;
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"总线路由表路由 总线路由[%d %d] nStep[%d] nIndex[%d]",\
				dreb.nNodeId,dreb.cNodePrivateId,dreb.nStep,dreb.nIndex);
		return true;
	}
	else //有多个相同的
	{
		//完全相同的有多个，随机取一个
		srand(CBF_Date_Time::GetTickCount());
		int id = rand() % resNum;
		id = id+lastIndex+1;
		dreb.bIsClose = false;
		dreb.cNodePrivateId = rt[id]->cNodePrivateId;
		dreb.nbandwidth = rt[id]->nbandwidth;
		dreb.nIndex = rt[id]->nIndex;
		dreb.nNodeId = rt[id]->nNodeId;
		dreb.nStep = rt[id]->nStep;
		dreb.pSendQueue = NULL;
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"总线路由表路由 总线路由[%d %d] nStep[%d] nIndex[%d]",\
				dreb.nNodeId,dreb.cNodePrivateId,dreb.nStep,dreb.nIndex);
		return true;
	}
}

bool CMemDB::GetARoute(std::vector<S_SERVICE_ROUTE *> rt, S_DREB_ROUTE &dreb)
{
	int i;
	//因为已按步进和带宽排序，所以只需要比较带宽即可
	int rtsize= rt.size();
	if (rtsize<1)
	{
		return false;
	}
	if (1 == rtsize)
	{
		dreb.bIsClose = false;
		dreb.cNodePrivateId = rt[0]->cNodePrivateId;
		dreb.nbandwidth = rt[0]->nbandwidth;
		dreb.nIndex = rt[0]->nIndex;
		dreb.nNodeId = rt[0]->nNodeId;
		dreb.nStep = rt[0]->nStep;
		dreb.pSendQueue = NULL;
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"交易表路由 交易路由[%d ] nStep[%d] nIndex[%d] 目标[%d %d %d %d]",\
			rt[0]->nFuncNo,dreb.nStep,dreb.nIndex,dreb.nNodeId,dreb.cNodePrivateId,rt[0]->nSvrMainId,rt[0]->cSvrPrivateId);
		return true;
	}
	int lastIndex=-1;
	unsigned int bd=rt[rtsize-1]->nbandwidth;//最大的一个带宽
	for (i=rtsize-2 ; i>=0 ; i--)
	{
		if (rt[i]->nbandwidth < bd) 
		{
			//有小的就退出，不用再找了
			lastIndex = i;//最小
			break;
		}
	}
	int resNum= rtsize-1 - lastIndex;
	
	if (1 == resNum ) //说明只有一个
	{
		dreb.bIsClose = false;
		dreb.cNodePrivateId = rt[rtsize-1]->cNodePrivateId;
		dreb.nbandwidth = rt[rtsize-1]->nbandwidth;
		dreb.nIndex = rt[rtsize-1]->nIndex;
		dreb.nNodeId = rt[rtsize-1]->nNodeId;
		dreb.nStep = rt[rtsize-1]->nStep;
		dreb.pSendQueue = NULL;
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"交易表路由 交易路由[%d ] nStep[%d] nIndex[%d] 目标[%d %d %d %d]",\
			rt[rtsize-1]->nFuncNo,dreb.nStep,dreb.nIndex,dreb.nNodeId,dreb.cNodePrivateId,rt[rtsize-1]->nSvrMainId,rt[rtsize-1]->cSvrPrivateId);
		return true;
	}
	else //有多个相同的
	{
		//完全相同的有多个，随机取一个
		srand(CBF_Date_Time::GetTickCount());
		int id = rand() % resNum;
		id = id+lastIndex+1;
		dreb.bIsClose = false;
		dreb.cNodePrivateId = rt[id]->cNodePrivateId;
		dreb.nbandwidth = rt[id]->nbandwidth;
		dreb.nIndex = rt[id]->nIndex;
		dreb.nNodeId = rt[id]->nNodeId;
		dreb.nStep = rt[id]->nStep;
		dreb.pSendQueue = NULL;
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"交易表路由 交易路由[%d ] nStep[%d] nIndex[%d] 目标[%d %d %d %d]",\
			rt[id]->nFuncNo,dreb.nStep,dreb.nIndex,dreb.nNodeId,dreb.cNodePrivateId,rt[id]->nSvrMainId,rt[id]->cSvrPrivateId);
		return true;
	}
}
bool CMemDB::GetARoute(std::vector<S_SERVICE_ROUTE *> rt, S_SERVICE_ROUTE &dreb)
{
	int i;
	//因为已按步进和带宽排序，所以只需要比较带宽即可
	int rtsize= rt.size();
	if (rtsize<1)
	{
		return false;
	}
	if (1 == rtsize)
	{
		memcpy(&dreb,&(rt[0]),sizeof(S_SERVICE_ROUTE));
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"交易表路由 交易路由[%d ] nStep[%d] nIndex[%d] 目标[%d %d %d %d]",\
			rt[0]->nFuncNo,dreb.nStep,dreb.nIndex,dreb.nNodeId,dreb.cNodePrivateId,rt[0]->nSvrMainId,rt[0]->cSvrPrivateId);
		return true;
	}
	int lastIndex=-1;
	unsigned int bd=rt[rtsize-1]->nbandwidth;//最大的一个带宽
	for (i=rtsize-2 ; i>=0 ; i--)
	{
		if (rt[i]->nbandwidth < bd) 
		{
			//有小的就退出，不用再找了
			lastIndex = i;//最小
			break;
		}
	}
	int resNum= rtsize-1 - lastIndex;

	if (1 == resNum ) //说明只有一个
	{
		memcpy(&dreb,&(rt[0]),sizeof(S_SERVICE_ROUTE));
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"交易表路由 交易路由[%d ] nStep[%d] nIndex[%d] 目标[%d %d %d %d]",\
			rt[rtsize-1]->nFuncNo,dreb.nStep,dreb.nIndex,dreb.nNodeId,dreb.cNodePrivateId,rt[rtsize-1]->nSvrMainId,rt[rtsize-1]->cSvrPrivateId);
		return true;
	}
	else //有多个相同的
	{
		//完全相同的有多个，随机取一个
		srand(CBF_Date_Time::GetTickCount());
		int id = rand() % resNum;
		id = id+lastIndex+1;
		memcpy(&dreb,&(rt[id]),sizeof(S_SERVICE_ROUTE));
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"交易表路由 交易路由[%d ] nStep[%d] nIndex[%d] 目标[%d %d %d %d]",\
			rt[id]->nFuncNo,dreb.nStep,dreb.nIndex,dreb.nNodeId,dreb.cNodePrivateId,rt[id]->nSvrMainId,rt[id]->cSvrPrivateId);
		return true;
	}
}

bool CMemDB::GetRouteByIndex(int index, std::vector<S_DREB_ROUTE *> &rt)
{
	m_drebTbl.SelectRouteByIndex(index,rt);
	m_routeTbl.GetRouteByIndex(index,rt);
	if (rt.size() < 1)
	{
		return false;
	}
	return true;
}

bool CMemDB::AddRoute(int index, std::vector<S_DREB_ROUTE>rtlist)
{
	S_DREB_ROUTE dreb;
	for (int i=0;i<rtlist.size();i++)
	{
		//若是直连接的DREB则不用加入了
		if (!m_drebTbl.SelectPrivateid(rtlist[i].nNodeId,rtlist[i].cNodePrivateId,dreb))
		{
			AddRoute(rtlist[i]);
		}
	}
	return true;
}

bool CMemDB::GetAllSvr(std::vector<S_SVR_ROUTE> &svrlist)
{
	std::vector<S_SVR_ROUTE*> reslist;
	m_svrTbl.Select(reslist);
	for (unsigned int i=0 ; i< reslist.size(); i++)
	{
		if (reslist[i]->bIsClose && time(NULL)- reslist[i]->nCloseTime>m_nDeleteTime)
		{
			UnRegisterSvr(reslist[i]->nSvrMainId, reslist[i]->cSvrPrivateId);
		}
		else
		{
			svrlist.push_back(*reslist[i]);
		}
	}
	return true;
}

bool CMemDB::GetRouteList(std::vector<S_DREB_ROUTE *> &rt)
{
	m_routeTbl.SelectById(rt);
    if (rt.size() < 1)
    {
        return false;
    }
    return true;
}
bool CMemDB::GetAllRoute(std::vector<S_DREB_ROUTE *> &rt)
{
	std::vector<S_DREB_ROUTE*>dreblist;
	m_drebTbl.Select(dreblist);
	for(unsigned int i=0; i<dreblist.size(); i++) 
	{
		if (dreblist[i]->nIndex>0)
		{
			rt.push_back(dreblist[i]);
		}
	}
	m_routeTbl.SelectById(rt);
	if (rt.size() < 1)
	{
		return false;
	}
	return true;	
}

bool CMemDB::GetAllDreb(std::vector<S_DREB_ROUTE *> &dp,bool inclocal)
{
	std::vector<S_DREB_ROUTE*>dreblist;
	if (!m_drebTbl.Select(dreblist))
	{
		return false;
	}
	for (unsigned int i=0; i<dreblist.size() ; i++)
	{
		if (!inclocal)
		{
			if (dreblist[i]->nIndex>0)
			{
				dp.push_back(dreblist[i]);
			}
		}
	}
	return true;
}

bool CMemDB::GetAllServiceOrder(std::vector<S_SERVICE_ROUTE *> &rtlist)
{
	return m_serviceTbl.GetAllServiceOrderDrebSvr(rtlist);
}

bool CMemDB::SelectRouteByFunc(unsigned int func, S_DREB_ROUTE &rt)
{
	std::vector<S_SERVICE_ROUTE *> slist;

	//查找交易列表，按步进升序排序
	if (!m_serviceTbl.SelectByFunc(func,slist)) //找到功能列表  
	{
		return false;
	}
 	return GetARoute(slist,rt);
}

bool CMemDB::SelectRouteBySvr(int svr, S_DREB_ROUTE &rt)
{
	std::vector<S_SERVICE_ROUTE *> slist;
	
	if (!m_serviceTbl.SelectBySvr(svr,slist))
	{
		return false;
	}
	if (slist.size()<1)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"本地无[%d]服务或此服务已断开",svr);
		return false;
	}
	if (m_pLog->isWrite(LOG_DEBUG+1))
	{
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"根据服务[%d]查找路由结果",svr);
		for (int i=0; i< slist.size(); i++)
		{
			m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"服务 index[%d] step[%d] 目标[%d %d %d %d]",\
				slist[i]->nIndex,slist[i]->nStep,slist[i]->nNodeId,slist[i]->cNodePrivateId,\
				slist[i]->nSvrMainId,slist[i]->cSvrPrivateId);
		}
	}
	//上面的结果已取最小步进并按带宽排序
	return GetARoute(slist,rt);

}
bool CMemDB::SelectRouteByFuncSvr(int func,int svr, S_DREB_ROUTE &rt)
{
	std::vector<S_SERVICE_ROUTE *> slist;

	if (!m_serviceTbl.SelectByFuncSvr(func,svr,slist))
	{
		return false;
	}
	if (slist.size()<1)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"本地无[%d %d]交易码+服务或此服务已断开",func,svr);
		return false;
	}
	if (m_pLog->isWrite(LOG_DEBUG+1))
	{
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"根据交易码服务[%d %d]查找路由结果",func,svr);
		for (unsigned int i=0; i< slist.size(); i++)
		{
			m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"交易码服务 index[%d] step[%d] 目标[%d %d %d %d]",\
				slist[i]->nIndex,slist[i]->nStep,slist[i]->nNodeId,slist[i]->cNodePrivateId,\
				slist[i]->nSvrMainId,slist[i]->cSvrPrivateId);
		}
	}
	//上面的结果已取最小步进并按带宽排序
	return GetARoute(slist,rt);

}
bool CMemDB::SelectRouteByDrebFuncSvr(int drebid,int func,int svr, S_SERVICE_ROUTE &rt)
{
	std::vector<S_SERVICE_ROUTE *> slist;

	if (!m_serviceTbl.SelectByFuncSvr(func,svr,slist))
	{
		return false;
	}
	if (slist.size()<1)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"本地无[%d]服务或此服务已断开",svr);
		return false;
	}
	if (m_pLog->isWrite(LOG_DEBUG+1))
	{
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"根据服务[%d]查找路由结果",svr);
		for (unsigned int i=0; i< slist.size(); i++)
		{
			m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"服务 index[%d] step[%d] 目标[%d %d %d %d]",\
				slist[i]->nIndex,slist[i]->nStep,slist[i]->nNodeId,slist[i]->cNodePrivateId,\
				slist[i]->nSvrMainId,slist[i]->cSvrPrivateId);
		}
	}
	//上面的结果已取最小步进并按带宽排序
	return GetARoute(slist,rt);

}

bool CMemDB::SelectRouteBySvr(int svr,int privatesvr, S_DREB_ROUTE &rt)
{
	std::vector<S_SERVICE_ROUTE *> slist;
	if (!m_serviceTbl.SelectBySvr(svr,privatesvr,slist))
	{
		return false;
	}
	if (slist.size()<1)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"本地无[%d]服务或此服务已断开",svr);
		return false;
	}
	if (m_pLog->isWrite(LOG_DEBUG+1))
	{
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"根据服务[%d %d]查找路由结果",svr,privatesvr);
		for (unsigned int i=0; i< slist.size(); i++)
		{
			m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"服务 index[%d] step[%d] 目标[%d %d %d %d]",\
				slist[i]->nIndex,slist[i]->nStep,slist[i]->nNodeId,slist[i]->cNodePrivateId,\
				slist[i]->nSvrMainId,slist[i]->cSvrPrivateId);
		}
	}
	//上面的结果已取最小步进并按带宽排序
	return GetARoute(slist,rt);

}

bool CMemDB::UnRegisterService(int nodeid, int privateid,int svr, int svrprivate)
{
	return m_serviceTbl.UnRegisterSvr(nodeid,privateid,svr,svrprivate);
}

bool CMemDB::AddServiceRoute(S_SERVICE_ROUTE rt)
{
	S_SERVICE_ROUTE *rttmp=NULL;
	rttmp = m_serviceTbl.SelectByPk(rt.nIndex,rt.nNodeId,rt.cNodePrivateId,rt.nSvrMainId,rt.cSvrPrivateId,rt.nFuncNo);
	if (rttmp != NULL)
	{
		if (rt.nStep < rttmp->nStep ) //步进小，更新
		{
			////更新nIndex nStep nUpdateTime三个字段
			if (!m_serviceTbl.Update(rt))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"更新路由出错index=%d %d %d %d %d %d",\
					rt.nIndex,rt.nNodeId,rt.cNodePrivateId,rt.nSvrMainId,rt.cSvrPrivateId,rt.nFuncNo);
				return false;
			}
		}
		else if (rt.nStep == rttmp->nStep ) //步进等于，要更新时间
		{
			m_pLog->LogMp(LOG_DEBUG+3,__FILE__,__LINE__,"更新交易路由 节点[%d %d ] 服务[%d %d] 交易[%d] index[%d] 里面的nStep[%d] 和交易路由表中的nStep[%d]相同， index为[%d]",\
				rt.nNodeId,rt.cNodePrivateId,rt.nSvrMainId,rt.cSvrPrivateId,rt.nFuncNo,rt.nIndex,rt.nStep,rttmp->nStep,rttmp->nIndex);
			//保持原来的index不变
			if (rt.nIndex != rttmp->nIndex)
			{
				rt.nIndex = rttmp->nIndex;
			}
			//更新nIndex nStep nUpdateTime三个字段
			if (!m_serviceTbl.Update(rt))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"步进等于，更新路由时间出错 index=%d %d %d %d %d %d",\
					rttmp->nIndex,rttmp->nNodeId,rttmp->cNodePrivateId,rttmp->nSvrMainId,rttmp->cSvrPrivateId,rttmp->nFuncNo);
				return false;
			}
		}
	}
	else
	{
		m_serviceTbl.Insert(rt);
	}
	return true;
}
bool CMemDB::GetRouteServiceByIndex(int drebid,int privateid,int index, std::vector<S_SERVICE_ROUTE *> &rtlist)
{
	return m_serviceTbl.GetRouteServiceByIndex(drebid,privateid,index,rtlist);
}

bool CMemDB::DeleteServiceByIndex(int index)
{
	return m_serviceTbl.DeleteByIndex(index);
}

bool CMemDB::UpdateRouteServiceByIndex(int index)
{
	return m_serviceTbl.UpdateRouteServiceByIndex(index);
}

bool CMemDB::DeleteRouteByIndex(int index)
{
	return m_routeTbl.DeleteByIndex(index);
}

bool CMemDB::SetLocalDrebInfo(S_DREB_ROUTE rt)
{
	rt.nIndex = 0;
	rt.nStep = 0;
	rt.pSendQueue = NULL;
	rt.bIsClose = false;
	return m_drebTbl.Insert(rt);
}

bool CMemDB::GetAllRouteBc(std::vector<S_DREB_ROUTE *> &rt)
{
	//取所有直连的总线信息，不包括本节点
	GetAllDreb(rt,false);

	std::vector<S_DREB_ROUTE*> dreblist;
	m_routeTbl.SelectById(dreblist);
	unsigned short nodeid=0;
	char  privateid=0;
	
	for (unsigned int i=0;  i< dreblist.size(); i++) 
	{
		//过滤掉相同的
		if (dreblist[i]->nNodeId != nodeid || dreblist[i]->cNodePrivateId != privateid)
		{
			rt.push_back(dreblist[i]);
			nodeid = dreblist[i]->nNodeId;
			privateid = dreblist[i]->cNodePrivateId;
		}
	}
	return true;	
}

bool CMemDB::SelectLocalByFunc(unsigned int func,S_SERVICE_ROUTE &fc)
{
	std::vector<S_SERVICE_ROUTE *> rt;
	if (!m_serviceTbl.SelectByFunc(func,rt))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"根据总线节点路由 交易码[%d]未在本节点注册",func);
		return false;
	}
	if (rt[0]->nStep != 0)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"根据总线节点路由 交易码[%d]未在本节点注册 nStep[%d] nIndex[%d]",\
			func,rt[0]->nStep,rt[0]->nIndex);
		return false;
	}
	//找到一个对应的index返回
	if (rt.size() == 1)
	{
		fc.cNodePrivateId = rt[0]->cNodePrivateId;
		fc.cSvrPrivateId = rt[0]->cSvrPrivateId;
		fc.nFuncNo = rt[0]->nFuncNo;
		fc.nIndex = rt[0]->nIndex;
		fc.nNodeId = rt[0]->nNodeId;
		fc.nStep = rt[0]->nStep;
		fc.nSvrMainId = rt[0]->nSvrMainId;
		fc.nUpdateTime = rt[0]->nUpdateTime;
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"交易码[%d]本地找到服务[%d %d] nIndex[%d]",\
			func,fc.nSvrMainId,fc.cSvrPrivateId,fc.nIndex);
		return true;
	}
	else
	{
		//有多个相同的，随机选一个
		srand(CBF_Date_Time::GetTickCount());
		int id = rand() % rt.size();
		fc.cNodePrivateId = rt[id]->cNodePrivateId;
		fc.cSvrPrivateId = rt[id]->cSvrPrivateId;
		fc.nFuncNo = rt[id]->nFuncNo;
		fc.nIndex = rt[id]->nIndex;
		fc.nNodeId = rt[id]->nNodeId;
		fc.nStep = rt[id]->nStep;
		fc.nSvrMainId = rt[id]->nSvrMainId;
		fc.nUpdateTime = rt[id]->nUpdateTime;
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"交易码[%d]本地找到服务[%d %d] nIndex[%d]",\
			func,fc.nSvrMainId,fc.cSvrPrivateId,fc.nIndex);
		return true;
	}
	return true;
}

bool CMemDB::SelectLocalBySvr(unsigned int svr, S_SERVICE_ROUTE &srt)
{
	std::vector<S_SVR_ROUTE *> svrlist;
	if (!m_svrTbl.SelectBySvr(svr,svrlist))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"本地无[%d]服务或此服务已断开",svr);
		return false;
	}
	if (svrlist.size()<1)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"本地无[%d]服务或此服务已断开",svr);
		return false;
	}
	if (svrlist.size() == 1)
	{
		srt.nIndex = svrlist[0]->nIndex;
		srt.nSvrMainId = svrlist[0]->nSvrMainId;
		srt.cSvrPrivateId = svrlist[0]->cSvrPrivateId;
	}
	else
	{
		//有多个相同的，随机选一个
		srand(CBF_Date_Time::GetTickCount());
		int id = rand() % svrlist.size();
		srt.nIndex = svrlist[id]->nIndex;
		srt.nSvrMainId = svrlist[id]->nSvrMainId;
		srt.cSvrPrivateId = svrlist[id]->cSvrPrivateId;
	}
	return true;
}
bool CMemDB::SelectLocalBySvr(unsigned int svr,int privatesvr, S_SERVICE_ROUTE &srt)
{
	S_SVR_ROUTE *svrinfo = m_svrTbl.SelectPrivateid(svr,privatesvr);
	if (svrinfo == NULL)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"本地无[%d %d]服务或此服务已断开",svr,privatesvr);
		return false;
	}
	srt.nIndex = svrinfo->nIndex;
	srt.nSvrMainId = svrinfo->nSvrMainId;
	srt.cSvrPrivateId = svrinfo->cSvrPrivateId;
	return true;
}