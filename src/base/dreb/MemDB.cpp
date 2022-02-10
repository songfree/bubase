// MemDB.cpp: implementation of the CMemDB class.
//
//////////////////////////////////////////////////////////////////////

#include "MemDB.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMemDB::CMemDB()
{
	m_nDeleteTime  = 300;//5����
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
//	CBF_PMutex pp(&m_mutex);
	if (!m_svrTbl.SelectPrivateid(svrid,privateid,svr))
	{
		//��ע��
		svr.bIsClose = false;
		svr.cSvrPrivateId = privateid;
		svr.nCloseTime = 0;
		svr.nIndex = index;
		svr.nSvrMainId = svrid;
		svr.pSendQueue = new CSendQueue();
		*pque = svr.pSendQueue;
		return m_svrTbl.Insert(svr);
	}
	m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"����[%d %d]��Ӧ�Ķ��е�ַ[%d]",svrid,privateid,svr.pSendQueue);
	if (!svr.bIsClose)
	{
		//�Ѿ�ע��
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"����[%d %d]��ע�Ტ��δ�Ͽ�",svrid,privateid);
		return false;
	}
	if (time(NULL) - svr.nCloseTime < m_nDeleteTime) //�رյ�δ��ʱ
	{
		svr.bIsClose = false;
		svr.nIndex = index;
		m_svrTbl.Update(svr);
		*pque = svr.pSendQueue;
		return true;
	}
	//�ѳ�ʱ����ע����ע��
//20150723���ﲻ��delete���ڱ���û�����ã���������
// 	if (svr.pSendQueue != NULL)
// 	{
// 		delete svr.pSendQueue;
// 		svr.pSendQueue = NULL;
// 	}
	
	m_svrTbl.UnRegister(svrid,privateid);
	//��ע��
	svr.bIsClose = false;
	svr.cSvrPrivateId = privateid;
	svr.nCloseTime = 0;
	svr.nIndex = index;
	svr.nSvrMainId = svrid;
	svr.pSendQueue = new CSendQueue();
	*pque = svr.pSendQueue;
	m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"��ע������[%d %d]��Ӧ�Ķ��е�ַ[%d]",svrid,privateid,svr.pSendQueue);
	return m_svrTbl.Insert(svr);

}

bool CMemDB::CloseSvr(int svrid, int privateid)
{
//	CBF_PMutex pp(&m_mutex);
	//ɾ�������б�
	return m_svrTbl.OnClose(svrid,privateid);
}

bool CMemDB::UnRegisterSvr(int svrid, int privateid)
{
//	CBF_PMutex pp(&m_mutex);
	//ɾ�������б�
	return m_svrTbl.UnRegister(svrid,privateid);
}

bool CMemDB::SelectASvr(int svrid, S_SVR_ROUTE &svr)
{
	int id;
	vector<S_SVR_ROUTE> svrlist;
	vector<int>s_list;
	vector<int>c_list;
//	CBF_PMutex pp(&m_mutex);
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
	//ʹ���������һ��
	
	for (unsigned int i=0;i<svrlist.size();i++)
	{
		if (svrlist[i].bIsClose && (time(NULL) - svrlist[i].nCloseTime > m_nDeleteTime))
		{
			m_svrTbl.UnRegister(svrlist[i].nSvrMainId,svrlist[i].cSvrPrivateId);//ȡ��ע��
		}
		else if (svrlist[i].bIsClose)
		{
			c_list.push_back(i); //����ر������б�
		}
		else
		{
			s_list.push_back(i);//�������������б�
		}
	}
	if (s_list.size() <1 && c_list.size()<1) //�޷���
	{
		return false;
	}
	if (s_list.size() == 1)  //������һ���򷵻�
	{
		svr = svrlist[s_list[0]];
		return true;
	}
	else if (s_list.size()>1)  //��������������ȡһ��
	{
		srand(CBF_Date_Time::GetTickCount());
		id = rand() % (s_list.size());
		svr = svrlist[id];
		return true;
	}
	else if (c_list.size() == 1)  //�������ҹر���һ�����򷵻�
	{
		svr = svrlist[c_list[0]];
		return true;
	}
	else if (c_list.size() > 1) //�������ҹر��ж�������ȡһ������
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
//	CBF_PMutex pp(&m_mutex);
	if (!m_svrTbl.SelectPrivateid(svrid,privateid,svr))
	{
		return false;
	}
	if (svr.bIsClose && (time(NULL) - svr.nCloseTime > m_nDeleteTime)) //���ӹر��ҳ�ʱ
	{
		m_svrTbl.UnRegister(svrid,privateid);
		return false;
	}
	return true;
}

bool CMemDB::RegisterDreb(int nodeid, int privateid, int index,int bdrate, CSendQueue **psque)
{
	S_DREB_ROUTE dreb;
//	CBF_PMutex pp(&m_mutex);
	if (!m_drebTbl.SelectPrivateid(nodeid,privateid,dreb))
	{
		//��ע��
		dreb.bIsClose = false;
		dreb.cNodePrivateId = privateid;
		dreb.nCloseTime = 0;
		dreb.nStep = 1;
		dreb.nIndex = index;
		dreb.nNodeId = nodeid;
		dreb.nbandwidth =  bdrate;
		dreb.pSendQueue = new CSendQueue();
		*psque = dreb.pSendQueue;
		m_routeTbl.DeleteByIndex(index);//ɾ����index���е�����·��
		return m_drebTbl.Insert(dreb);
	}
	if (!dreb.bIsClose)
	{
		//�Ѿ�ע��
		return false;
	}
	if (time(NULL) - dreb.nCloseTime < m_nDeleteTime) //�رյ�δ��ʱ
	{
		dreb.bIsClose = false;
		dreb.nIndex = index;
		dreb.nStep = 1;
		*psque = dreb.pSendQueue;
		m_drebTbl.Update(dreb);
		return true;
	}
	//�ѳ�ʱ����ע����ע��
	m_drebTbl.UnRegister(nodeid,privateid);
	m_routeTbl.DeleteByIndex(index);//ɾ����index���е�·��
	//��ע��
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
//	CBF_PMutex pp(&m_mutex);
	m_routeTbl.DeleteByIndex(index);
	return m_drebTbl.OnClose(nodeid,privateid);
}

bool CMemDB::UnRegisterDreb(int nodeid, int privateid, int index)
{
//	CBF_PMutex pp(&m_mutex);
	m_routeTbl.DeleteByIndex(index);
	return m_drebTbl.UnRegister(nodeid,privateid);
}

bool CMemDB::AddRoute(S_DREB_ROUTE rt)
{
	S_DREB_ROUTE dreb;
//	CBF_PMutex pp(&m_mutex);
	if (!m_drebTbl.SelectByIndex(rt.nIndex,dreb))
	{
		return false;
	}
	//ͬһ��indexֻ������Сstep��
	return m_routeTbl.UpdateRoute(rt);
}

bool CMemDB::SelectRoute(int nodeid, S_DREB_ROUTE &dreb)
{
	vector<S_DREB_ROUTE *>dl;
	//�ȴ����ӵ�ͨѶƽ̨����
	if (m_drebTbl.SelectByNode(nodeid,dl))
	{
		return GetARoute(dl,dreb);
	}
	vector<S_DREB_ROUTE *>rt;
	if (!m_routeTbl.SelectByNode(nodeid,rt))
	{
		return false;
	}
	return GetARoute(rt,dreb);
}

bool CMemDB::SelectRoute(int nodeid, int privateid, S_DREB_ROUTE &dreb)
{
	//�ȴ����ӵ�ͨѶƽ̨����
	S_DREB_ROUTE *dr = m_drebTbl.SelectPrivateid(nodeid,privateid);
	if (dr != NULL)
	{
		//ֱ��������δ�Ͽ���Ϊ���ڵ㣬��ֱ�ӷ���
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
			m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���ػ�ֱ����·�� ����·��[%d %d] nStep[%d] nIndex[%d]",\
				dreb.nNodeId,dreb.cNodePrivateId,dreb.nStep,dreb.nIndex);
			return true;
		}
		m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"ָ�������߽ڵ�[%d %d] �ѶϿ� nStep[%d] ���Դ�·�ɱ������·��",nodeid,privateid,dr->nStep);
		//���Դ�·�ɱ������ҿ���
	}
	vector<S_DREB_ROUTE *>rt;
	if (!m_routeTbl.SelectByPrivateNode(nodeid,privateid,rt))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"ָ�������߽ڵ�[%d %d] ������·�ɱ�����Ҳ�����",nodeid,privateid);
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
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"����·�ɱ�·�� ����·��[%d %d] nStep[%d] nIndex[%d]",\
				dreb.nNodeId,dreb.cNodePrivateId,dreb.nStep,dreb.nIndex);
		return true;
	}
	return GetARoute(rt,dreb);
}

bool CMemDB::GetARoute(vector<S_DREB_ROUTE *> rt, S_DREB_ROUTE &dreb)
{
	int i;
	//��Ϊ�Ѱ������ʹ�����������ֻ��Ҫ�Ƚϴ�����
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
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"����·�ɱ�·�� ����·��[%d %d] nStep[%d] nIndex[%d]",\
				dreb.nNodeId,dreb.cNodePrivateId,dreb.nStep,dreb.nIndex);
		return true;
	}
	int lastIndex=-1;
	unsigned int bd=rt[rtsize-1]->nbandwidth;//����һ������
	for (i=rtsize-2 ; i>=0 ; i--)
	{
		if (rt[i]->nbandwidth < bd) 
		{
			//��С�ľ��˳�������������
			lastIndex = i;//��С
			break;
		}
	}
	int resNum= rtsize-1 - lastIndex;

	if (1 == resNum ) //˵��ֻ��һ��
	{
		dreb.bIsClose = false;
		dreb.cNodePrivateId = rt[rtsize-1]->cNodePrivateId;
		dreb.nbandwidth = rt[rtsize-1]->nbandwidth;
		dreb.nIndex = rt[rtsize-1]->nIndex;
		dreb.nNodeId = rt[rtsize-1]->nNodeId;
		dreb.nStep = rt[rtsize-1]->nStep;
		dreb.pSendQueue = NULL;
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"����·�ɱ�·�� ����·��[%d %d] nStep[%d] nIndex[%d]",\
				dreb.nNodeId,dreb.cNodePrivateId,dreb.nStep,dreb.nIndex);
		return true;
	}
	else //�ж����ͬ��
	{
		//��ȫ��ͬ���ж�������ȡһ��
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
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"����·�ɱ�·�� ����·��[%d %d] nStep[%d] nIndex[%d]",\
				dreb.nNodeId,dreb.cNodePrivateId,dreb.nStep,dreb.nIndex);
		return true;
	}
}

// bool CMemDB::GetARoute(vector<S_SERVICE_QUERY_RESULT> rt,S_DREB_ROUTE &dreb,S_SVR_ROUTE &svr)
// {
// 	if (rt.size()<1)
// 	{
// 		return false;
// 	}
// 	if (rt.size() == 1) //���ֻ��һ��
// 	{
// 		dreb.bIsClose = rt[0].bIsClose;
// 		dreb.cNodePrivateId = rt[0].cNodePrivateId;
// 		dreb.nbandwidth = rt[0].nbandwidth;
// 		dreb.nCloseTime = rt[0].nCloseTime;
// 		dreb.nIndex = rt[0].nIndex;
// 		dreb.nNodeId = rt[0].nNodeId;
// 		dreb.nStep = rt[0].nStep;
// 		dreb.pSendQueue = rt[0].pSendQueue;
// 		svr.nSvrMainId = rt[0].nSvrMainId;
// 		svr.cSvrPrivateId = rt[0].cSvrPrivateId;
// 		return true;
// 	}
// 	int i,j;
// 	vector <S_SERVICE_QUERY_RESULT *> data;
// 	data.push_back(&rt[0]);
// 	//�Ƚϲ���
// 	for (i=1;i<rt.size();i++)
// 	{
// 		if (rt[i].nStep < data[0]->nStep)
// 		{
// 			data.clear();
// 			data.push_back(&rt[i]);
// 		}
// 		else if (rt[i].nStep == data[0]->nStep)
// 		{
// 			data.push_back(&rt[i]);
// 		}
// 	}
// 	if (data.size() == 1) //������С��ֻ��һ��
// 	{
// 		dreb.bIsClose = data[0]->bIsClose;
// 		dreb.cNodePrivateId = data[0]->cNodePrivateId;
// 		dreb.nbandwidth = data[0]->nbandwidth;
// 		dreb.nCloseTime = data[0]->nCloseTime;
// 		dreb.nIndex = data[0]->nIndex;
// 		dreb.nNodeId = data[0]->nNodeId;
// 		dreb.nStep = data[0]->nStep;
// 		dreb.pSendQueue = data[0]->pSendQueue;
// 		svr.nSvrMainId = data[0]->nSvrMainId;
// 		svr.cSvrPrivateId = data[0]->cSvrPrivateId;
// 		return true;
// 	}
// 	//������ͬ�ıȽϴ���
// 
// 	vector <S_SERVICE_QUERY_RESULT *> databd;
// 	databd.push_back(data[0]);
// 	//�Ƚϴ���
// 	for (i=1;i<data.size();i++)
// 	{
// 		if (data[i]->nbandwidth > databd[0]->nbandwidth)
// 		{
// 			databd.clear();
// 			databd.push_back(data[i]);
// 		}
// 		else if (data[i]->nbandwidth == data[0]->nbandwidth)
// 		{
// 			databd.push_back(data[i]);
// 		}
// 	}
// 	
// 	if (databd.size() == 1) //��������ֻ��һ��
// 	{
// 		dreb.bIsClose = databd[0]->bIsClose;
// 		dreb.cNodePrivateId = databd[0]->cNodePrivateId;
// 		dreb.nbandwidth = databd[0]->nbandwidth;
// 		dreb.nCloseTime = databd[0]->nCloseTime;
// 		dreb.nIndex = databd[0]->nIndex;
// 		dreb.nNodeId = databd[0]->nNodeId;
// 		dreb.nStep = databd[0]->nStep;
// 		dreb.pSendQueue = databd[0]->pSendQueue;
// 		svr.nSvrMainId = databd[0]->nSvrMainId;
// 		svr.cSvrPrivateId = databd[0]->cSvrPrivateId;
// 		return true;
// 	}
// 	//��ȫ��ͬ���ж�������ȡһ��
// 	srand(CBF_Date_Time::GetTickCount());
// 	int id = rand() % (databd.size());
// 	dreb.bIsClose = databd[id]->bIsClose;
// 	dreb.cNodePrivateId = databd[id]->cNodePrivateId;
// 	dreb.nbandwidth = databd[id]->nbandwidth;
// 	dreb.nCloseTime = databd[id]->nCloseTime;
// 	dreb.nIndex = databd[id]->nIndex;
// 	dreb.nNodeId = databd[id]->nNodeId;
// 	dreb.nStep = databd[id]->nStep;
// 	dreb.pSendQueue = databd[id]->pSendQueue;
// 	svr.nSvrMainId = databd[id]->nSvrMainId;
// 	svr.cSvrPrivateId = databd[id]->cSvrPrivateId;
// 	return true;
// }
bool CMemDB::GetARoute(vector<S_SERVICE_ROUTE *> rt, S_DREB_ROUTE &dreb)
{
	int i;
	//��Ϊ�Ѱ������ʹ�����������ֻ��Ҫ�Ƚϴ�����
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
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���ױ�·�� ����·��[%d ] nStep[%d] nIndex[%d] Ŀ��[%d %d %d %d]",\
			rt[0]->nFuncNo,dreb.nStep,dreb.nIndex,dreb.nNodeId,dreb.cNodePrivateId,rt[0]->nSvrMainId,rt[0]->cSvrPrivateId);
		return true;
	}
	int lastIndex=-1;
	unsigned int bd=rt[rtsize-1]->nbandwidth;//����һ������
	for (i=rtsize-2 ; i>=0 ; i--)
	{
		if (rt[i]->nbandwidth < bd) 
		{
			//��С�ľ��˳�������������
			lastIndex = i;//��С
			break;
		}
	}
	int resNum= rtsize-1 - lastIndex;
	
	if (1 == resNum ) //˵��ֻ��һ��
	{
		dreb.bIsClose = false;
		dreb.cNodePrivateId = rt[rtsize-1]->cNodePrivateId;
		dreb.nbandwidth = rt[rtsize-1]->nbandwidth;
		dreb.nIndex = rt[rtsize-1]->nIndex;
		dreb.nNodeId = rt[rtsize-1]->nNodeId;
		dreb.nStep = rt[rtsize-1]->nStep;
		dreb.pSendQueue = NULL;
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���ױ�·�� ����·��[%d ] nStep[%d] nIndex[%d] Ŀ��[%d %d %d %d]",\
			rt[rtsize-1]->nFuncNo,dreb.nStep,dreb.nIndex,dreb.nNodeId,dreb.cNodePrivateId,rt[rtsize-1]->nSvrMainId,rt[rtsize-1]->cSvrPrivateId);
		return true;
	}
	else //�ж����ͬ��
	{
		//��ȫ��ͬ���ж�������ȡһ��
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
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���ױ�·�� ����·��[%d ] nStep[%d] nIndex[%d] Ŀ��[%d %d %d %d]",\
			rt[id]->nFuncNo,dreb.nStep,dreb.nIndex,dreb.nNodeId,dreb.cNodePrivateId,rt[id]->nSvrMainId,rt[id]->cSvrPrivateId);
		return true;
	}
}
bool CMemDB::GetARoute(vector<S_SERVICE_ROUTE *> rt, S_SERVICE_ROUTE &dreb)
{
	int i;
	//��Ϊ�Ѱ������ʹ�����������ֻ��Ҫ�Ƚϴ�����
	int rtsize= rt.size();
	if (rtsize<1)
	{
		return false;
	}
	if (1 == rtsize)
	{
		memcpy(&dreb,&(rt[0]),sizeof(S_SERVICE_ROUTE));
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���ױ�·�� ����·��[%d ] nStep[%d] nIndex[%d] Ŀ��[%d %d %d %d]",\
			rt[0]->nFuncNo,dreb.nStep,dreb.nIndex,dreb.nNodeId,dreb.cNodePrivateId,rt[0]->nSvrMainId,rt[0]->cSvrPrivateId);
		return true;
	}
	int lastIndex=-1;
	unsigned int bd=rt[rtsize-1]->nbandwidth;//����һ������
	for (i=rtsize-2 ; i>=0 ; i--)
	{
		if (rt[i]->nbandwidth < bd) 
		{
			//��С�ľ��˳�������������
			lastIndex = i;//��С
			break;
		}
	}
	int resNum= rtsize-1 - lastIndex;

	if (1 == resNum ) //˵��ֻ��һ��
	{
		memcpy(&dreb,&(rt[0]),sizeof(S_SERVICE_ROUTE));
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���ױ�·�� ����·��[%d ] nStep[%d] nIndex[%d] Ŀ��[%d %d %d %d]",\
			rt[rtsize-1]->nFuncNo,dreb.nStep,dreb.nIndex,dreb.nNodeId,dreb.cNodePrivateId,rt[rtsize-1]->nSvrMainId,rt[rtsize-1]->cSvrPrivateId);
		return true;
	}
	else //�ж����ͬ��
	{
		//��ȫ��ͬ���ж�������ȡһ��
		srand(CBF_Date_Time::GetTickCount());
		int id = rand() % resNum;
		id = id+lastIndex+1;
		memcpy(&dreb,&(rt[id]),sizeof(S_SERVICE_ROUTE));
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���ױ�·�� ����·��[%d ] nStep[%d] nIndex[%d] Ŀ��[%d %d %d %d]",\
			rt[id]->nFuncNo,dreb.nStep,dreb.nIndex,dreb.nNodeId,dreb.cNodePrivateId,rt[id]->nSvrMainId,rt[id]->cSvrPrivateId);
		return true;
	}
}
// bool CMemDB::GetARoute(vector<S_DREB_ROUTE *> rt, S_DREB_ROUTE &dreb)
// {
// 	int i;
// 	//��Ϊ�Ѱ������ʹ�����������ֻ��Ҫ�Ƚϴ�����
// 	int rtsize= rt.size();
// 	if (rtsize<1)
// 	{
// 		return false;
// 	}
// 	if (1 == rtsize)
// 	{
// 		dreb.bIsClose = false;
// 		dreb.cNodePrivateId = rt[0]->cNodePrivateId;
// 		dreb.nbandwidth = rt[0]->nbandwidth;
// 		dreb.nIndex = rt[0]->nIndex;
// 		dreb.nNodeId = rt[0]->nNodeId;
// 		dreb.nStep = rt[0]->nStep;
// 		dreb.pSendQueue = rt[0]->pSendQueue;
// 		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"����ֱ����·�� ����·��[%d %d] nStep[%d] nIndex[%d]",\
// 				dreb.nNodeId,dreb.cNodePrivateId,dreb.nStep,dreb.nIndex);
// 		return true;
// 	}
// 	int lastIndex=-1;
// 	unsigned int bd=rt[rtsize-1]->nbandwidth;//����һ������
// 	for (i=rtsize-2 ; i>=0 ; i--)
// 	{
// 		if (rt[i]->nbandwidth < bd) 
// 		{
// 			//��С�ľ��˳�������������
// 			lastIndex = i;//��С
// 			break;
// 		}
// 	}
// 	int resNum= rtsize-1 - lastIndex;
// 	
// 	if (1 == resNum ) //˵��ֻ��һ��
// 	{
// 		dreb.bIsClose = false;
// 		dreb.cNodePrivateId = rt[rtsize-1]->cNodePrivateId;
// 		dreb.nbandwidth = rt[rtsize-1]->nbandwidth;
// 		dreb.nIndex = rt[rtsize-1]->nIndex;
// 		dreb.nNodeId = rt[rtsize-1]->nNodeId;
// 		dreb.nStep = rt[rtsize-1]->nStep;
// 		dreb.pSendQueue = rt[rtsize-1]->pSendQueue;
// 		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"����ֱ����·�� ����·��[%d %d] nStep[%d] nIndex[%d]",\
// 				dreb.nNodeId,dreb.cNodePrivateId,dreb.nStep,dreb.nIndex);
// 		return true;
// 	}
// 	else //�ж����ͬ��
// 	{
// 		//��ȫ��ͬ���ж�������ȡһ��
// 		srand(CBF_Date_Time::GetTickCount());
// 		int id = rand() % resNum;
// 		id = id+lastIndex+1;
// 		dreb.bIsClose = false;
// 		dreb.cNodePrivateId = rt[id]->cNodePrivateId;
// 		dreb.nbandwidth = rt[id]->nbandwidth;
// 		dreb.nIndex = rt[id]->nIndex;
// 		dreb.nNodeId = rt[id]->nNodeId;
// 		dreb.nStep = rt[id]->nStep;
// 		dreb.pSendQueue = rt[rtsize-1]->pSendQueue;
// 		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"����ֱ����·�� ����·��[%d %d] nStep[%d] nIndex[%d]",\
// 				dreb.nNodeId,dreb.cNodePrivateId,dreb.nStep,dreb.nIndex);
// 		return true;
// 	}
// }

bool CMemDB::GetRouteByIndex(int index, vector<S_DREB_ROUTE *> &rt)
{
	S_DREB_ROUTE *dreb=NULL;
//	CBF_PMutex pp(&m_mutex);
	dreb = m_drebTbl.First();
	while (dreb != NULL)
	{
		//index����ͬ������������
		if (dreb->nIndex != index && !dreb->bIsClose)
		{
			rt.push_back(dreb);
		}
		dreb = m_drebTbl.Next();
	}
	
	return m_routeTbl.GetRouteByIndex(index,rt);

}

bool CMemDB::AddRoute(int index,vector<S_DREB_ROUTE>rtlist)
{
	S_DREB_ROUTE dreb;
	for (int i=0;i<rtlist.size();i++)
	{
		//����ֱ���ӵ�DREB���ü�����
		if (!m_drebTbl.SelectPrivateid(rtlist[i].nNodeId,rtlist[i].cNodePrivateId,dreb))
		{
			AddRoute(rtlist[i]);
		}
	}
	return true;
}

bool CMemDB::GetAllSvr(vector<S_SVR_ROUTE> &svrlist)
{
	bool bret;
	S_SVR_ROUTE data;
	bret = m_svrTbl.First(data);
	while (bret)
	{
		if (data.bIsClose && time(NULL)-data.nCloseTime>m_nDeleteTime)
		{
			UnRegisterSvr(data.nSvrMainId,data.cSvrPrivateId);
		}
		else
		{
			svrlist.push_back(data);
		}
		bret = m_svrTbl.Next(data);
	}
	return true;
}

bool CMemDB::GetRouteList(vector<S_DREB_ROUTE *> &rt)
{
	S_DREB_ROUTE *data=NULL;
//	CBF_PMutex pp(&m_mutex);
	data = m_routeTbl.FirstById();
	while (data != NULL)
	{
		rt.push_back(data);
		data = m_routeTbl.NextById();
	}
	return true;	
}
bool CMemDB::GetAllRoute(vector<S_DREB_ROUTE *> &rt)
{
	S_DREB_ROUTE *dreb=NULL;
//	CBF_PMutex pp(&m_mutex);
	dreb = m_drebTbl.First();
	while (dreb != NULL)
	{
		if (dreb->nIndex>0)
		{
			rt.push_back(dreb);
		}
		dreb = m_drebTbl.Next();
	}

	dreb = m_routeTbl.FirstById();
	while (dreb != NULL)
	{
		rt.push_back(dreb);
		dreb = m_routeTbl.NextById();
	}
	return true;	
}

bool CMemDB::GetAllDreb(vector<S_DREB_ROUTE *> &dp,bool inclocal)
{
	S_DREB_ROUTE *dreb=NULL;
//	CBF_PMutex pp(&m_mutex);
	dreb = m_drebTbl.First();
	while (dreb!=NULL)
	{
		if (!inclocal)
		{
			if (dreb->nIndex>0)
			{
				dp.push_back(dreb);
			}
		}
		dreb = m_drebTbl.Next();
	}
	return true;
}

bool CMemDB::GetAllServiceOrder(vector<S_SERVICE_ROUTE *> &rtlist)
{
//	CBF_PMutex pp(&m_mutexService);
	return m_serviceTbl.GetAllServiceOrderDrebSvr(rtlist);
}

bool CMemDB::SelectRouteByFunc(unsigned int func, S_DREB_ROUTE &rt)
{
	vector<S_SERVICE_ROUTE *> slist;

	//���ҽ����б���������������
	if (!m_serviceTbl.SelectByFunc(func,slist)) //�ҵ������б�  
	{
		return false;
	}
 	return GetARoute(slist,rt);
}

bool CMemDB::SelectRouteBySvr(int svr, S_DREB_ROUTE &rt)
{
	vector<S_SERVICE_ROUTE *> slist;
	
	if (!m_serviceTbl.SelectBySvr(svr,slist))
	{
		return false;
	}
	if (slist.size()<1)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"������[%d]�����˷����ѶϿ�",svr);
		return false;
	}
	if (m_pLog->isWrite(LOG_DEBUG+1))
	{
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���ݷ���[%d]����·�ɽ��",svr);
		for (int i=0; i< slist.size(); i++)
		{
			m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���� index[%d] step[%d] Ŀ��[%d %d %d %d]",\
				slist[i]->nIndex,slist[i]->nStep,slist[i]->nNodeId,slist[i]->cNodePrivateId,\
				slist[i]->nSvrMainId,slist[i]->cSvrPrivateId);
		}
	}
	//����Ľ����ȡ��С����������������
	return GetARoute(slist,rt);

}
bool CMemDB::SelectRouteByFuncSvr(int func,int svr, S_DREB_ROUTE &rt)
{
	vector<S_SERVICE_ROUTE *> slist;

	if (!m_serviceTbl.SelectByFuncSvr(func,svr,slist))
	{
		return false;
	}
	if (slist.size()<1)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"������[%d %d]������+�����˷����ѶϿ�",func,svr);
		return false;
	}
	if (m_pLog->isWrite(LOG_DEBUG+1))
	{
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���ݽ��������[%d %d]����·�ɽ��",func,svr);
		for (unsigned int i=0; i< slist.size(); i++)
		{
			m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"��������� index[%d] step[%d] Ŀ��[%d %d %d %d]",\
				slist[i]->nIndex,slist[i]->nStep,slist[i]->nNodeId,slist[i]->cNodePrivateId,\
				slist[i]->nSvrMainId,slist[i]->cSvrPrivateId);
		}
	}
	//����Ľ����ȡ��С����������������
	return GetARoute(slist,rt);

}
bool CMemDB::SelectRouteByDrebFuncSvr(int drebid,int func,int svr, S_SERVICE_ROUTE &rt)
{
	vector<S_SERVICE_ROUTE *> slist;

	if (!m_serviceTbl.SelectByFuncSvr(func,svr,slist))
	{
		return false;
	}
	if (slist.size()<1)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"������[%d]�����˷����ѶϿ�",svr);
		return false;
	}
	if (m_pLog->isWrite(LOG_DEBUG+1))
	{
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���ݷ���[%d]����·�ɽ��",svr);
		for (unsigned int i=0; i< slist.size(); i++)
		{
			m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���� index[%d] step[%d] Ŀ��[%d %d %d %d]",\
				slist[i]->nIndex,slist[i]->nStep,slist[i]->nNodeId,slist[i]->cNodePrivateId,\
				slist[i]->nSvrMainId,slist[i]->cSvrPrivateId);
		}
	}
	//����Ľ����ȡ��С����������������
	return GetARoute(slist,rt);

}

bool CMemDB::SelectRouteBySvr(int svr,int privatesvr, S_DREB_ROUTE &rt)
{
	vector<S_SERVICE_ROUTE *> slist;
	if (!m_serviceTbl.SelectBySvr(svr,privatesvr,slist))
	{
		return false;
	}
	if (slist.size()<1)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"������[%d]�����˷����ѶϿ�",svr);
		return false;
	}
	if (m_pLog->isWrite(LOG_DEBUG+1))
	{
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���ݷ���[%d %d]����·�ɽ��",svr,privatesvr);
		for (unsigned int i=0; i< slist.size(); i++)
		{
			m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���� index[%d] step[%d] Ŀ��[%d %d %d %d]",\
				slist[i]->nIndex,slist[i]->nStep,slist[i]->nNodeId,slist[i]->cNodePrivateId,\
				slist[i]->nSvrMainId,slist[i]->cSvrPrivateId);
		}
	}
	//����Ľ����ȡ��С����������������
	return GetARoute(slist,rt);

}

bool CMemDB::UnRegisterService(int nodeid, int privateid,int svr, int svrprivate)
{
//	CBF_PMutex pp(&m_mutexService);
	return m_serviceTbl.UnRegisterSvr(nodeid,privateid,svr,svrprivate);
}

bool CMemDB::AddServiceRoute(S_SERVICE_ROUTE rt)
{
	S_SERVICE_ROUTE *rttmp=NULL;
//	CBF_PMutex pp(&m_mutexService);
	rttmp = m_serviceTbl.SelectByPk(rt.nIndex,rt.nNodeId,rt.cNodePrivateId,rt.nSvrMainId,rt.cSvrPrivateId,rt.nFuncNo);
	if (rttmp != NULL)
	{
		if (rt.nStep < rttmp->nStep ) //����С������
		{
			////����nIndex nStep nUpdateTime�����ֶ�
			if (!m_serviceTbl.Update(rt))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"����·�ɳ���index=%d %d %d %d %d %d",\
					rt.nIndex,rt.nNodeId,rt.cNodePrivateId,rt.nSvrMainId,rt.cSvrPrivateId,rt.nFuncNo);
				return false;
			}
		}
		else if (rt.nStep == rttmp->nStep ) //�������ڣ�Ҫ����ʱ��
		{
			m_pLog->LogMp(LOG_DEBUG+3,__FILE__,__LINE__,"���½���·�� �ڵ�[%d %d ] ����[%d %d] ����[%d] index[%d] �����nStep[%d] �ͽ���·�ɱ��е�nStep[%d]��ͬ�� indexΪ[%d]",\
				rt.nNodeId,rt.cNodePrivateId,rt.nSvrMainId,rt.cSvrPrivateId,rt.nFuncNo,rt.nIndex,rt.nStep,rttmp->nStep,rttmp->nIndex);
			//����ԭ����index����
			if (rt.nIndex != rttmp->nIndex)
			{
				rt.nIndex = rttmp->nIndex;
			}
			//����nIndex nStep nUpdateTime�����ֶ�
			if (!m_serviceTbl.Update(rt))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"�������ڣ�����·��ʱ����� index=%d %d %d %d %d %d",\
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
bool CMemDB::GetRouteServiceByIndex(int drebid,int privateid,int index,vector<S_SERVICE_ROUTE *> &rtlist)
{
//	CBF_PMutex pp(&m_mutexService);
	return m_serviceTbl.GetRouteServiceByIndex(drebid,privateid,index,rtlist);
}

bool CMemDB::DeleteServiceByIndex(int index)
{
//	CBF_PMutex pp(&m_mutexService);
	return m_serviceTbl.DeleteByIndex(index);
}

bool CMemDB::UpdateRouteServiceByIndex(int index)
{
//	CBF_PMutex pp(&m_mutexService);
	return m_serviceTbl.UpdateRouteServiceByIndex(index);
}

bool CMemDB::DeleteRouteByIndex(int index)
{
//	CBF_PMutex pp(&m_mutex);
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

bool CMemDB::GetAllRouteBc(vector<S_DREB_ROUTE *> &rt)
{
	S_DREB_ROUTE *dreb=NULL;
	//ȡ����ֱ����������Ϣ�����������ڵ�
	GetAllDreb(rt,false);

	unsigned short nodeid=0;
	char  privateid=0;
	dreb = m_routeTbl.FirstById();
	while (dreb != NULL)
	{
		//���˵���ͬ��
		if (dreb->nNodeId != nodeid || dreb->cNodePrivateId != privateid)
		{
			rt.push_back(dreb);
			nodeid = dreb->nNodeId;
			privateid = dreb->cNodePrivateId;
		}
		dreb = m_routeTbl.NextById();
	}
	return true;	
}

bool CMemDB::SelectLocalByFunc(unsigned int func,S_SERVICE_ROUTE &fc)
{
	vector<S_SERVICE_ROUTE *> rt;
	if (!m_serviceTbl.SelectByFunc(func,rt))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"�������߽ڵ�·�� ������[%d]δ�ڱ��ڵ�ע��",func);
		return false;
	}
	if (rt[0]->nStep != 0)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"�������߽ڵ�·�� ������[%d]δ�ڱ��ڵ�ע�� nStep[%d] nIndex[%d]",\
			func,rt[0]->nStep,rt[0]->nIndex);
		return false;
	}
	//�ҵ�һ����Ӧ��index����
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
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"������[%d]�����ҵ�����[%d %d] nIndex[%d]",\
			func,fc.nSvrMainId,fc.cSvrPrivateId,fc.nIndex);
		return true;
	}
	else
	{
		//�ж����ͬ�ģ����ѡһ��
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
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"������[%d]�����ҵ�����[%d %d] nIndex[%d]",\
			func,fc.nSvrMainId,fc.cSvrPrivateId,fc.nIndex);
		return true;
	}
	return true;
}

bool CMemDB::SelectLocalBySvr(unsigned int svr, S_SERVICE_ROUTE &srt)
{
	vector<S_SVR_ROUTE *> svrlist;
	if (!m_svrTbl.SelectBySvr(svr,svrlist))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"������[%d]�����˷����ѶϿ�",svr);
		return false;
	}
	if (svrlist.size()<1)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"������[%d]�����˷����ѶϿ�",svr);
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
		//�ж����ͬ�ģ����ѡһ��
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
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"������[%d %d]�����˷����ѶϿ�",svr,privatesvr);
		return false;
	}
	srt.nIndex = svrinfo->nIndex;
	srt.nSvrMainId = svrinfo->nSvrMainId;
	srt.cSvrPrivateId = svrinfo->cSvrPrivateId;
	return true;
}