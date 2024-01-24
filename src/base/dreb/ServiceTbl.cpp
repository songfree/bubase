// ServiceTbl.cpp: implementation of the CServiceTbl class.
//
//////////////////////////////////////////////////////////////////////

#include "ServiceTbl.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CServiceTbl::CServiceTbl()
{
	m_nDeleteTime = 300;//���5���ڷ���û�и�����ɾ��
}

CServiceTbl::~CServiceTbl()
{

}

bool CServiceTbl::Insert(S_SERVICE_ROUTE rt)
{
	int id;
	bool bRet;
	//ͨ���������ң�������������
	CBF_PMutex pp(&m_mutex);
	if (!m_pkey.Find(rt.nIndex,rt.nNodeId,rt.cNodePrivateId,rt.nSvrMainId,rt.cSvrPrivateId,rt.nFuncNo))
	{
		rt.nUpdateTime = time(NULL);
		id = m_table.Add(rt);//���ӵ���
		m_pkey.Add(id,rt.nIndex,rt.nNodeId,rt.cNodePrivateId,rt.nSvrMainId,rt.cSvrPrivateId,rt.nFuncNo);//��������
		m_index_drebindex.Add(id,rt.nIndex);//dreb������ŵ�����������·��ʱ���ݴ�������ɾ��������
		m_index_func.Add(id,rt.nFuncNo);//��������,ͨ�����ܺŲ���·��ʱʹ��
		m_index_svr.Add(id,rt.nSvrMainId);//ͨ���������·��ʱʹ��
		m_index_svrpri.Add(id,rt.nSvrMainId,rt.cSvrPrivateId);//ͨ���������·��ʱʹ��
		m_index_route.Add(id,rt.nNodeId,rt.cNodePrivateId,rt.nSvrMainId,rt.cSvrPrivateId);
		m_index_indexroute.Add(id,rt.nIndex,rt.nNodeId,rt.cNodePrivateId,rt.nSvrMainId,rt.cSvrPrivateId);
		m_index_funcsvr.Add(id,rt.nFuncNo,rt.nSvrMainId);
		m_index_drebfuncsvr.Add(id,rt.nNodeId,rt.nFuncNo,rt.nSvrMainId);
		return true;
	}
	m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"����·�� node[%d %d] svr[%d %d] txcode[%d] �Ѵ���",\
		rt.nNodeId,rt.cNodePrivateId,rt.nSvrMainId,rt.cSvrPrivateId,rt.nFuncNo);
	return false;
}

bool CServiceTbl::SelectByFunc(unsigned int func,std::vector<S_SERVICE_ROUTE> &rt)
{
	CInt iset;
	CInt delset;
	CBF_PMutex pp(&m_mutex);
	if (!m_index_func.Select(iset,func))
	{
		return false;
	}
	int id;
	bool bRet;
	bRet = iset.First(id);
	while (bRet)
	{
		if (time(NULL) - m_table.m_table[id].nUpdateTime > m_nDeleteTime && m_table.m_table[id].nStep >0) //��ʱ
		{
			delset.Add(id);
			bRet = iset.Next(id);
			continue;
		}
		rt.push_back(m_table.m_table[id]);
		bRet = iset.Next(id);
	}
	bRet = delset.First(id);
	while(bRet)
	{
		DeleteById(id);
		bRet = delset.Next(id);
	}
	return true;
}
bool CServiceTbl::SelectByFunc(unsigned int func, std::vector<S_SERVICE_ROUTE *> &rt)
{
	CInt iset;
	CInt delset;
	CBF_PMutex pp(&m_mutex);
	if (!m_index_func.Select(iset,func))
	{
		return false;
	}
	int id;
	bool bRet;
	CIndexInt<1> txroutebystep; //����������
	bRet = iset.First(id);
	
	while (bRet)
	{
		if (time(NULL) - m_table.m_table[id].nUpdateTime > m_nDeleteTime && m_table.m_table[id].nStep >0) //��ʱ
		{
			delset.Add(id);
			bRet = iset.Next(id);
			continue;
		}
		//�ȷŵ�������ȥ
		txroutebystep.Add(id,m_table.m_table[id].nStep);
		bRet = iset.Next(id);
	}
	bRet = delset.First(id);
	while(bRet)
	{
		DeleteById(id);
		bRet = delset.Next(id);
	}

	unsigned int curstep=10000;
	unsigned int stepnum=0; //��ͬ�Ĳ�������
	bRet = txroutebystep.First(id);
	while(bRet)
	{
		//����ǰ�Ĳ�����ֵ
		if (curstep != m_table.m_table[id].nStep)
		{
			curstep = m_table.m_table[id].nStep;
			stepnum++;
		}
		if (stepnum == 1)
		{
			//��С������ֵ����
			rt.push_back(&(m_table.m_table[id]));
		}
		else if (stepnum>1)
		{
			//ֻȡ��С������ֵ��ֱ�ӷ�����
			return true;
		}
		bRet = txroutebystep.Next(id);
	}
	return true;
}
bool CServiceTbl::Update(S_SERVICE_ROUTE rt)
{
	int id;
	CBF_PMutex pp(&m_mutex);
	if (!m_pkey.Select(id,rt.nIndex,rt.nNodeId,rt.cNodePrivateId,rt.nSvrMainId,rt.cSvrPrivateId,rt.nFuncNo))
	{
		return false;
	}
//	CBF_PMutex pp(&m_mutex);
//	m_table.m_table[id].nNodeId = rt.nNodeId;
//	m_table.m_table[id].cSvrPrivateId = rt.cSvrPrivateId;
//	m_table.m_table[id].nFuncNo = rt.nFuncNo;
//	m_table.m_table[id].nIndex = rt.nIndex;
//	m_table.m_table[id].cNodePrivateId = rt.cNodePrivateId;
	m_table.m_table[id].nStep = rt.nStep;
//	m_table.m_table[id].nSvrMainId = rt.nSvrMainId;
	m_table.m_table[id].nUpdateTime = time(NULL);
	return true;
}

bool CServiceTbl::Delete(int index,int nodeid,int nodeprivated,int svrid, int privateid,unsigned int func)
{
	int id;
	CBF_PMutex pp(&m_mutex);
	if (!m_pkey.Select(id,index,nodeid,nodeprivated,svrid,privateid,func))
	{
		return false;
	}
	CInt iset;
	iset.Add(id);
	m_pkey.Delete(index,nodeid,nodeprivated,svrid,privateid,func);
	m_index_func.Delete(iset,func);
	m_index_drebindex.Delete(iset,m_table.m_table[id].nIndex);
	m_index_svr.Delete(iset,svrid);
	m_index_svrpri.Delete(iset,svrid,privateid);
	m_index_route.Delete(iset,nodeid,nodeprivated,svrid,privateid);
	m_index_indexroute.Delete(iset,m_table.m_table[id].nIndex,nodeid,nodeprivated,svrid,privateid);
	m_index_funcsvr.Delete(iset,func,svrid);
	m_index_drebfuncsvr.Delete(iset,nodeid,func,svrid);
	m_table.Delete(id);
	return true;
}

bool CServiceTbl::SelectByPk(int index,int nodeid,int nodeprivated,int svrid, int privateid,unsigned int func, S_SERVICE_ROUTE &rt)
{
	int id;
	CBF_PMutex pp(&m_mutex);
	if (!m_pkey.Select(id,index,nodeid,nodeprivated,svrid,privateid,func))
	{
		return false;
	}
	rt = m_table.m_table[id];
	return true;
}
S_SERVICE_ROUTE * CServiceTbl::SelectByPk(int index,int nodeid,int nodeprivated,int svrid, int privateid,unsigned int func)
{
	int id;
	CBF_PMutex pp(&m_mutex);
	if (!m_pkey.Select(id,index,nodeid,nodeprivated,svrid,privateid,func))
	{
		return NULL;
	}
	return &(m_table.m_table[id]);
}
bool CServiceTbl::Select(std::vector<S_SERVICE_ROUTE*>& rtlist)
{
	int id;
	CBF_PMutex pp(&m_mutex);
	bool bret = m_pkey.First(id);
	if (!bret)
	{
		return false;
	}
	while (bret)
	{
		rtlist.push_back(&(m_table.m_table[id]));
		bret = m_pkey.Next(id);
	}
	return true;
}

bool CServiceTbl::SelectBySvr(int svr, std::vector<S_SERVICE_ROUTE> &rtlist)
{
	CInt iset;
	CInt delset;
	CBF_PMutex pp(&m_mutex);
	if (!m_index_svr.Select(iset,svr))
	{
		return false;
	}
	int id;
	unsigned int j;
	bool bRet;
	bool isfind=false;
	bRet = iset.First(id);
	while (bRet)
	{
		if (time(NULL) - m_table.m_table[id].nUpdateTime > m_nDeleteTime && m_table.m_table[id].nStep >0) //��ʱ
		{
			delset.Add(id);
			bRet = iset.Next(id);
			continue;
		}
		isfind = false;
		for (j=0;j<rtlist.size();j++)
		{
			if (rtlist[j].cSvrPrivateId == m_table.m_table[id].cSvrPrivateId && rtlist[j].nNodeId == m_table.m_table[id].nNodeId \
				&& rtlist[j].cNodePrivateId == m_table.m_table[id].cNodePrivateId) //˽�к�DREBҲ��ͬ�����˵�
			{
				isfind = true;
				break;
			}
		}
		if (!isfind) 
		{
			rtlist.push_back(m_table.m_table[id]);//�Ž�ȥ���ǲ�ͬ�ķ���
		}
		bRet = iset.Next(id);
	}
	bRet = delset.First(id);
	while(bRet)
	{
		DeleteById(id);
		bRet = delset.Next(id);
	}
	return true;
}

bool CServiceTbl::SelectBySvr(int svr, std::vector<S_SERVICE_ROUTE *> &rtlist)
{
	CInt iset;
	CInt delset;
	CBF_PMutex pp(&m_mutex);
	if (!m_index_svr.Select(iset,svr))
	{
		return false;
	}
	CPkeyInt<5> indexKey;//���ڵ�+����+�������򣬹��˵���ͬ�ķ���
	CIndexInt<1> indexStepbd;//����������
	int id;
	bool bRet;
	bool isfind=false;
	bRet = iset.First(id);
	while (bRet)
	{
		if (time(NULL) - m_table.m_table[id].nUpdateTime > m_nDeleteTime && m_table.m_table[id].nStep >0) //��ʱ
		{
			delset.Add(id);
			bRet = iset.Next(id);
			continue;
		}
		indexKey.Add(id,m_table.m_table[id].nNodeId,m_table.m_table[id].cNodePrivateId,m_table.m_table[id].nSvrMainId,m_table.m_table[id].cSvrPrivateId,m_table.m_table[id].nStep);
		bRet = iset.Next(id);
	}
	bRet = delset.First(id);
	while(bRet)
	{
		DeleteById(id);
		bRet = delset.Next(id);
	}
	unsigned int curstep=10000;
	unsigned int stepnum=0; //��ͬ�Ĳ�������
	bRet = indexKey.First(id);
	while(bRet)
	{
		//����ǰ�Ĳ�����ֵ
		if (curstep != m_table.m_table[id].nStep)
		{
			curstep = m_table.m_table[id].nStep;
			stepnum++;
		}
		if (stepnum == 1)
		{
			//��С������ֵ���벢����������
			indexStepbd.Add(id,m_table.m_table[id].nbandwidth);
			
		}
		else if (stepnum>1)
		{
			//ֻȡ��С������ֵ��ֱ�ӷ�����
			return true;
		}
		bRet = indexKey.Next(id);
	}
	bRet = indexStepbd.First(id);
	while(bRet)
	{
		rtlist.push_back(&(m_table.m_table[id]));
		indexStepbd.Next(id);
	}
	return true;
}
bool CServiceTbl::SelectBySvr(int svr, int privatesvr, std::vector<S_SERVICE_ROUTE> &rtlist)
{
	CInt iset;
	CInt delset;
	CBF_PMutex pp(&m_mutex);
	if (!m_index_svrpri.Select(iset,svr,privatesvr))
	{
		return false;
	}
	int id;
	unsigned int j;
	bool bRet;
	bool isfind=false;
	bRet = iset.First(id);
	while (bRet)
	{
		if (time(NULL) - m_table.m_table[id].nUpdateTime > m_nDeleteTime && m_table.m_table[id].nStep >0) //��ʱ
		{
			delset.Add(id);
			bRet = iset.Next(id);
			continue;
		}
		isfind = false;
		for (j=0;j<rtlist.size();j++)
		{
			if (rtlist[j].cSvrPrivateId == m_table.m_table[id].cSvrPrivateId && rtlist[j].nNodeId == m_table.m_table[id].nNodeId \
				&& rtlist[j].cNodePrivateId == m_table.m_table[id].cNodePrivateId) //˽�к�DREBҲ��ͬ�����˵�
			{
				isfind = true;
				break;
			}
		}
		if (!isfind) 
		{
			rtlist.push_back(m_table.m_table[id]);//�Ž�ȥ���ǲ�ͬ�ķ���
		}
		bRet = iset.Next(id);
	}
	bRet = delset.First(id);
	while(bRet)
	{
		DeleteById(id);
		bRet = delset.Next(id);
	}
	return true;
}
bool CServiceTbl::SelectBySvr(int svr, int privatesvr, std::vector<S_SERVICE_ROUTE *> &rtlist)
{
	CInt iset;
	CInt delset;
	CBF_PMutex pp(&m_mutex);
	if (!m_index_svrpri.Select(iset,svr,privatesvr))
	{
		return false;
	}
	CPkeyInt<5> indexKey;//���ڵ�+����+�������򣬹��˵���ͬ�ķ���
	int id;
	bool bRet;
	bool isfind=false;
	bRet = iset.First(id);
	while (bRet)
	{
		if (time(NULL) - m_table.m_table[id].nUpdateTime > m_nDeleteTime && m_table.m_table[id].nStep >0) //��ʱ
		{
			delset.Add(id);
			bRet = iset.Next(id);
			continue;
		}
		indexKey.Add(id,m_table.m_table[id].nNodeId,m_table.m_table[id].cNodePrivateId,m_table.m_table[id].nSvrMainId,m_table.m_table[id].cSvrPrivateId,m_table.m_table[id].nStep);
		bRet = iset.Next(id);
	}
	bRet = delset.First(id);
	while(bRet)
	{
		DeleteById(id);
		bRet = delset.Next(id);
	}
	unsigned int curstep=10000;
	unsigned int stepnum=0; //��ͬ�Ĳ�������
	bRet = indexKey.First(id);
	while(bRet)
	{
		//����ǰ�Ĳ�����ֵ
		if (curstep != m_table.m_table[id].nStep)
		{
			curstep = m_table.m_table[id].nStep;
			stepnum++;
		}
		if (stepnum == 1)
		{
			//��С������ֵ����
			rtlist.push_back(&(m_table.m_table[id]));
		}
		else if (stepnum>1)
		{
			//ֻȡ��С������ֵ��ֱ�ӷ�����
			return true;
		}
		bRet = indexKey.Next(id);
	}
	return true;
}
bool CServiceTbl::SelectByFuncSvr(int func,int svr, std::vector<S_SERVICE_ROUTE *> &rtlist)
{
	CInt iset;
	CInt delset;
	CBF_PMutex pp(&m_mutex);
	if (!m_index_funcsvr.Select(iset,func,svr))
	{
		return false;
	}
	CPkeyInt<5> indexKey;//���ڵ�+����+�������򣬹��˵���ͬ�ķ���
	int id;
	bool bRet;
	bool isfind=false;
	bRet = iset.First(id);
	while (bRet)
	{
		if (time(NULL) - m_table.m_table[id].nUpdateTime > m_nDeleteTime && m_table.m_table[id].nStep >0) //��ʱ
		{
			delset.Add(id);
			bRet = iset.Next(id);
			continue;
		}
		indexKey.Add(id,m_table.m_table[id].nNodeId,m_table.m_table[id].cNodePrivateId,m_table.m_table[id].nSvrMainId,m_table.m_table[id].cSvrPrivateId,m_table.m_table[id].nStep);
		bRet = iset.Next(id);
	}
	bRet = delset.First(id);
	while(bRet)
	{
		DeleteById(id);
		bRet = delset.Next(id);
	}
	unsigned int curstep=10000;
	unsigned int stepnum=0; //��ͬ�Ĳ�������
	bRet = indexKey.First(id);
	while(bRet)
	{
		//����ǰ�Ĳ�����ֵ
		if (curstep != m_table.m_table[id].nStep)
		{
			curstep = m_table.m_table[id].nStep;
			stepnum++;
		}
		if (stepnum == 1)
		{
			//��С������ֵ����
			rtlist.push_back(&(m_table.m_table[id]));
		}
		else if (stepnum>1)
		{
			//ֻȡ��С������ֵ��ֱ�ӷ�����
			return true;
		}
		bRet = indexKey.Next(id);
	}
	return true;
}
bool CServiceTbl::SelectByDrebFuncSvr(int drebid,int func,int svr, std::vector<S_SERVICE_ROUTE *> &rtlist)
{
	CInt iset;
	CInt delset;
	CBF_PMutex pp(&m_mutex);
	if (!m_index_drebfuncsvr.Select(iset,drebid,func,svr))
	{
		return false;
	}
	CPkeyInt<5> indexKey;//���ڵ�+����+�������򣬹��˵���ͬ�ķ���
	int id;
	bool bRet;
	bool isfind=false;
	bRet = iset.First(id);
	while (bRet)
	{
		if (time(NULL) - m_table.m_table[id].nUpdateTime > m_nDeleteTime && m_table.m_table[id].nStep >0) //��ʱ
		{
			delset.Add(id);
			bRet = iset.Next(id);
			continue;
		}
		indexKey.Add(id,m_table.m_table[id].nNodeId,m_table.m_table[id].cNodePrivateId,m_table.m_table[id].nSvrMainId,m_table.m_table[id].cSvrPrivateId,m_table.m_table[id].nStep);
		bRet = iset.Next(id);
	}
	bRet = delset.First(id);
	while(bRet)
	{
		DeleteById(id);
		bRet = delset.Next(id);
	}
	unsigned int curstep=10000;
	unsigned int stepnum=0; //��ͬ�Ĳ�������
	bRet = indexKey.First(id);
	while(bRet)
	{
		//����ǰ�Ĳ�����ֵ
		if (curstep != m_table.m_table[id].nStep)
		{
			curstep = m_table.m_table[id].nStep;
			stepnum++;
		}
		if (stepnum == 1)
		{
			//��С������ֵ����
			rtlist.push_back(&(m_table.m_table[id]));
		}
		else if (stepnum>1)
		{
			//ֻȡ��С������ֵ��ֱ�ӷ�����
			return true;
		}
		bRet = indexKey.Next(id);
	}
	return true;
}
bool CServiceTbl::GetAllServiceOrderDrebSvr(std::vector<S_SERVICE_ROUTE *> &rtlist)
{
	bool bRet;
	int id;
	CBF_PMutex pp(&m_mutex);
	bRet = m_index_indexroute.First(id);
	while (bRet)
	{
		rtlist.push_back(&(m_table.m_table[id]));
		bRet = m_index_indexroute.Next(id);
	}
	
	return true;
}
bool CServiceTbl::DeleteByIndex(int drebindex)
{
	CInt iset;
	CBF_PMutex pp(&m_mutex);
	if (!m_index_drebindex.Select(iset,drebindex))
	{
		return true;
	}
	int id;
	bool bRet = iset.First(id);
	while (bRet)
	{
		if (drebindex == m_table.m_table[id].nIndex)
		{
			DeleteById(id);
		}
// 		else
// 		{
// 			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"����indexɾ�����ҵ���index %d %d",drebindex,m_table.m_table[id].nIndex);
// 		}
		
		bRet = iset.Next(id);
	}
	return true;
}

bool CServiceTbl::UnRegisterSvr(int nodeid, int nodeprivated, int svrid, int privateid)
{
	CInt iset;
	CBF_PMutex pp(&m_mutex);
	if (!m_index_route.Select(iset,nodeid,nodeprivated,svrid,privateid))
	{
		return false;
	}
	int id;
	bool bRet = iset.First(id);
	CInt delset;
	while (bRet)
	{
		//����Ϊ0��Ϊ���صķ���
//		if (m_table.m_table[id].nIndex == 0)
		if (m_table.m_table[id].nStep == 0)
		{
			delset.Add(id);
		}
		bRet = iset.Next(id);
	}
	bRet = delset.First(id);
	while (bRet)
	{
		DeleteById(id);
		bRet = delset.Next(id);
	}
	return true;
}

bool CServiceTbl::GetRouteServiceByIndex(int drebid,int privateid,int index, std::vector<S_SERVICE_ROUTE> &rtlist)
{
	bool bRet;
	int id;
	CInt iset;
	CInt delset;
	CBF_PMutex pp(&m_mutex);
	bRet = m_pkey.First(id);
	while (bRet)
	{
		if (time(NULL) - m_table.m_table[id].nUpdateTime > m_nDeleteTime && m_table.m_table[id].nStep >0) //��ʱ
		{
			delset.Add(id);
		}
		else
		{
			//ֻ�е�������������ͬ�Ĳ���Ҫ����·��
			if (m_table.m_table[id].nIndex != index) //dreb������ͬ����ȡ��
			{
				if (m_table.m_table[id].nNodeId == drebid && m_table.m_table[id].cNodePrivateId ==privateid)
				{
					//dreb��ͬ�����÷��룬��ֹ�ظ�ѭ��
				}
				else
				{
					rtlist.push_back(m_table.m_table[id]);
				}
			}	
		}
		bRet = m_pkey.Next(id);
	}
	bRet = delset.First(id);
	while(bRet)
	{
		DeleteById(id);
		bRet = delset.Next(id);
	}
	return true;
}
bool CServiceTbl::GetRouteServiceByIndex(int drebid,int privateid,int index, std::vector<S_SERVICE_ROUTE *> &rtlist)
{
	bool bRet;
	int id;
	CInt iset;
	CInt delset;
	CBF_PMutex pp(&m_mutex);
	bRet = m_pkey.First(id);
	while (bRet)
	{
		if (time(NULL) - m_table.m_table[id].nUpdateTime > m_nDeleteTime && m_table.m_table[id].nStep >0) //��ʱ
		{
			delset.Add(id);
		}
		else
		{
			//ֻ�е�������������ͬ�Ĳ���Ҫ����·��
			if (m_table.m_table[id].nIndex != index) //dreb������ͬ����ȡ��
			{
				if (m_table.m_table[id].nNodeId == drebid && m_table.m_table[id].cNodePrivateId ==privateid)
				{
					//dreb��ͬ�����÷��룬��ֹ�ظ�ѭ��
				}
				else
				{
					rtlist.push_back(&(m_table.m_table[id]));
				}
			}	
		}
		bRet = m_pkey.Next(id);
	}
	bRet = delset.First(id);
	while(bRet)
	{
		DeleteById(id);
		bRet = delset.Next(id);
	}
	return true;
}
bool CServiceTbl::UpdateRouteServiceByIndex(int index)
{
	bool bRet;
	int id;
	CInt iset;
	CBF_PMutex pp(&m_mutex);
	bRet = m_pkey.First(id);
	while (bRet)
	{
		if (m_table.m_table[id].nIndex == index) //dreb������ͬ����ȡ��
		{
			if (time(NULL) - m_table.m_table[id].nUpdateTime > m_nDeleteTime && m_table.m_table[id].nStep >0) //��ʱ
			{
				iset.Add(id);
			}
		}	
		bRet = m_pkey.Next(id);
	}
	
	bRet = iset.First(id);
	if (!bRet)
	{
		return true;
	}
	char tmpchar[300];
	bzero(tmpchar,sizeof(tmpchar));
	std::string msg;
	msg = tmpchar;
	while(bRet)
	{
		DeleteById(id);
		sprintf(tmpchar," id[%d] pid[%d] svrid[%d] svrpid[%d] txcode[%d] step[%d] ",m_table.m_table[id].nNodeId,m_table.m_table[id].cNodePrivateId,\
			m_table.m_table[id].nSvrMainId,m_table.m_table[id].cSvrPrivateId,m_table.m_table[id].nFuncNo,\
			m_table.m_table[id].nStep);
		msg = msg+tmpchar;
		bRet = iset.Next(id);
	}
	if (msg.length()>1 && msg.length()<LOG_BUFFERSIZE)
	{
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"����ʱ��ɾ��index=%d ��·��:%s",index,msg.c_str());
	}
	return true;
}

bool CServiceTbl::DeleteById(int rowid)
{
	CInt iset;
	iset.Add(rowid);
	m_pkey.Delete(m_table.m_table[rowid].nIndex,m_table.m_table[rowid].nNodeId,m_table.m_table[rowid].cNodePrivateId,m_table.m_table[rowid].nSvrMainId,m_table.m_table[rowid].cSvrPrivateId,m_table.m_table[rowid].nFuncNo);
	m_index_func.Delete(iset,m_table.m_table[rowid].nFuncNo);
	m_index_drebindex.Delete(iset,m_table.m_table[rowid].nIndex);
	m_index_svr.Delete(iset,m_table.m_table[rowid].nSvrMainId);
	m_index_svrpri.Delete(iset,m_table.m_table[rowid].nSvrMainId,m_table.m_table[rowid].cSvrPrivateId);
	m_index_route.Delete(iset,m_table.m_table[rowid].nNodeId,m_table.m_table[rowid].cNodePrivateId,m_table.m_table[rowid].nSvrMainId,m_table.m_table[rowid].cSvrPrivateId);
	m_index_indexroute.Delete(iset,m_table.m_table[rowid].nIndex,m_table.m_table[rowid].nNodeId,m_table.m_table[rowid].cNodePrivateId,m_table.m_table[rowid].nSvrMainId,m_table.m_table[rowid].cSvrPrivateId);
	m_index_funcsvr.Delete(iset,m_table.m_table[rowid].nFuncNo,m_table.m_table[rowid].nSvrMainId);
	m_index_drebfuncsvr.Delete(iset,m_table.m_table[rowid].nNodeId,m_table.m_table[rowid].nFuncNo,m_table.m_table[rowid].nSvrMainId);
	m_table.Delete(rowid);
	return true;
}



CSubscribeTbl::CSubscribeTbl()
{

}
CSubscribeTbl::~CSubscribeTbl()
{

}
void CSubscribeTbl::Subscribe(unsigned int   nIndex, std::vector<unsigned int>& funclist, unsigned int   nSvrMainId, char  cSvrPrivateId)
{
	if (funclist.size() == 0)
	{
		return UnSubscribe(nIndex);
	}
	int rid;
	S_SUBSCRIBE info;
	CBF_PMutex pp(&m_mutex);
	for (int i = 0; i < funclist.size(); i++)
	{
		if (!m_key.Find(nIndex, funclist[i]))
		{
			info.nIndex = nIndex;
			info.nFuncNo = funclist[i];
			info.nSvrMainId  =nSvrMainId;
			info.cSvrPrivateId = cSvrPrivateId;
			info.nUpdateTime = time(NULL);
			rid = m_table.Add(info);
			m_key.Add(rid,m_table.m_table[rid].nIndex, m_table.m_table[rid].nFuncNo);
			m_indexFunc.Add(rid, m_table.m_table[rid].nFuncNo) ;
			m_indexindex.Add(rid, m_table.m_table[rid].nIndex);
		}
	}
}
//ȡ������
void CSubscribeTbl::UnSubscribe(unsigned int   nIndex)
{
	CBF_PMutex pp(&m_mutex);
	CInt iset;
	if (m_indexindex.Select(iset, nIndex))
	{
		return;
	}
	//m_indexindex.Delete(iset);
	//m_indexFunc.Delete(iset);
	//m_key.Delete(iset);
	int rid;
	int bret = iset.First(rid);
	while (bret)
	{	
		m_key.Delete(m_table.m_table[rid].nIndex, m_table.m_table[rid].nFuncNo);
		m_indexindex.Delete(iset, m_table.m_table[rid].nIndex);
		m_indexFunc.Delete(iset, m_table.m_table[rid].nFuncNo);
		m_table.Delete(rid);
		bret = iset.Next(rid);
	}
	return;
}

bool CSubscribeTbl::GetIndex(unsigned int nFunc, std::vector<S_SUBSCRIBE>& indexlist)
{
	CInt iset;
	CBF_PMutex pp(&m_mutex);
	if (!m_indexFunc.Select(iset, nFunc))
	{
		return false;
	}
	int rid;
	S_SUBSCRIBE data;
	bool bret = iset.First(rid);
	while (bret)
	{
		data.cSvrPrivateId = m_table.m_table[rid].cSvrPrivateId;
		data.nFuncNo = m_table.m_table[rid].nFuncNo;
		data.nIndex = m_table.m_table[rid].nIndex;
		data.nSvrMainId = m_table.m_table[rid].nSvrMainId;
		data.nUpdateTime = m_table.m_table[rid].nUpdateTime;
		indexlist.push_back(data);
		bret = iset.Next(rid);
	}
	return true;
}