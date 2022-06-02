// BFDreb.cpp: implementation of the CBFDreb class.
//
//////////////////////////////////////////////////////////////////////

#include "BFDreb.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
extern char g_VERSION[20];

CBFDreb::CBFDreb()
{
	
}

CBFDreb::~CBFDreb()
{
	
}


bool CBFDreb::Init(const char *confile)
{
	if (!g_Vars.Init(confile))
	{
		printf("��ʼ��ʧ��:%s \n",g_Vars.m_errMsg);
		//m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"%s %s ��ʼ��ʧ��:%s",g_Vars.g_curentpath,g_Vars.g_modulename,g_Vars.m_errMsg);
		return false;
	}
	m_log = &(g_Vars.g_pLog);

	

	g_pMemPool.Init(100,DREBBUFSIZE+sizeof(DREBQUEUEMSG_HEAD)+1);

	g_pMemDb.m_pLog = m_log;
	g_pMemDb.SetPara(g_Vars.g_nSvrDeleteTime,g_Vars.g_nServiceDeleteTime);

	

	g_pMsgQueue.m_pRes = &g_Vars;

	int i;
	//��ʼ�����ӳ�
	g_connInfoList.SetGlobalVar(&g_Vars,&g_pMemPool);
	if (!g_connInfoList.Init(g_Vars.g_nMaxConnCount))
	{
		printf("��ʼ��ʧ��:%s \n",g_connInfoList.m_errMsg);
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"%s %s ��ʼ��ʧ��:%s",g_Vars.g_curentpath,g_Vars.g_modulename,g_connInfoList.m_errMsg);
		return false;
	}
	int tmpindex=0;
	if (!g_connInfoList.at(0)->Listen(g_Vars.g_nPort,false,30))
	{
		printf("��ʼ��ʧ��:%s %s ����%d sokcet����ʧ��",g_Vars.g_curentpath,g_Vars.g_modulename,g_Vars.g_nPort);
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"%s %s ��ʼ��ʧ��:����%d sokcet����ʧ��",g_Vars.g_curentpath,g_Vars.g_modulename,g_Vars.g_nPort);
		return false;
	}
	//��ͨѶƽ̨������
	for (i=tmpindex; i<g_Vars.g_pCenterList.size(); i++)
	{
		g_connInfoList.at(i+1)->SetToDreb(g_Vars.g_pCenterList[i].sIp.c_str(),g_Vars.g_pCenterList[i].nPort,g_Vars.g_pCenterList[i].nSpeed);
	}
	

	S_DREB_ROUTE tbl;
	tbl.cNodePrivateId = g_Vars.g_nDrebPrivateId;
	tbl.nNodeId  = g_Vars.g_nDrebId;
	tbl.nIndex = 0;
	tbl.nStep = 0;
	tbl.nbandwidth=102400000;
	//����dreb����·��
	g_pMemDb.SetLocalDrebInfo(tbl);

	//�������մ����߳�
	if (!g_pPoolModule.SetGlobalVar(&g_Vars,&g_connInfoList,&g_pMemDb,&g_pMemPool,&g_pMsgQueue))
	{
		printf("��ʼ��ʧ��:%s \n","ģ�ͳ�ʼʧ��");
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"%s %s ��ʼ��ʧ��:%s",g_Vars.g_curentpath,g_Vars.g_modulename,"ģ�ͳ�ʼʧ��");
		return false;
	}
	g_pPoolModule.m_nBegin = 0;
	g_pPoolModule.m_nEnd = g_Vars.g_nMaxConnCount;

	g_Vars.g_bIsExit = false;
	g_pPoolModule.m_pMsgProc = &g_pProcThread[0];

	g_pPoolModule.CreateThread();
	if (g_pPoolModule.IsStoped())
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"%s %s ���������߳�ʧ��!",g_Vars.g_curentpath,g_Vars.g_modulename);
		g_Vars.g_bIsExit = true;
		return false;
	}
	//������ʱ��
	g_pPoolModule.StartTimer();

	//������Ϣ�����߳�
    if (g_Vars.g_nMsgProcThread>0)
	{
		for (int i = 0; i < g_Vars.g_nMsgProcThread; i++)
		{
            g_pProcThread[i].SetGlobalVar(&g_Vars, &g_connInfoList, &g_pMemDb, &g_pMemPool, &g_pMsgQueue);
            g_pProcThread[i].CreateThread();
            if (g_pProcThread[i].IsStoped())
            {
                m_log->LogMp(LOG_ERROR, __FILE__, __LINE__, "%s %s ������Ϣ�����߳�ʧ��!", g_Vars.g_curentpath, g_Vars.g_modulename);
                g_Vars.g_bIsExit = true;
                return false;
            }
		}
		
	}
	if (g_Vars.g_nUseMonitor == 1)
	{
		if (g_Vars.g_nMonitorHost == 1)
		{
			g_pDrebTimer.Init(&g_Vars);
			g_pDrebTimer.StartTimer();
		}
	}
	

	printf("%s[%s] �����ɹ� CopyRight 2009,pid=%d,port=%d! \n",g_Vars.g_modulename,g_VERSION,GetPid(),g_Vars.g_nPort);
	m_log->LogMp(LOG_INFO,__FILE__,__LINE__,"%s[%s]�����ɹ�,pid=%d,port=%d!",g_Vars.g_modulename,g_VERSION,GetPid(),g_Vars.g_nPort);
	return true;
}

void CBFDreb::OnStop()
{
	g_Vars.g_bIsExit = true;
	g_pDrebTimer.StopTimer();
	g_pPoolModule.StopTimer();
	m_log->LogMp(LOG_INFO,__FILE__,__LINE__,"DREB��ʼֹͣ");
	g_pPoolModule.Join();
	g_Vars.g_pLog.StopLog();
	g_Vars.g_pDataLog.StopLog();
    for (int i = 0; i < g_Vars.g_nMsgProcThread; i++)
    {
		g_pProcThread[i].Join();
	}
	SLEEP_SECONDS(2);
	g_connInfoList.at(0)->OnClose("DREB�����˳�",__FILE__,__LINE__);
	

}

void CBFDreb::Monitor()
{
	//ɨ���߳�
	if (g_pPoolModule.IsStoped())
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"���� ���������߳�CPoolModule  %d",g_Vars.g_bIsExit);
		g_pPoolModule.CreateThread();
	}
	if (g_Vars.g_nMsgProcThread >0)
	{
        for (int i = 0; i < g_Vars.g_nMsgProcThread; i++)
        {
            if (g_pProcThread[i].IsStoped())
            {
                m_log->LogMp(LOG_ERROR, __FILE__, __LINE__, "���� ������Ϣ�����߳� %d", i);
                g_pProcThread[i].CreateThread();
            }
        }
		
	}
	if (g_Vars.g_pLog.IsStopedThread())
	{
		g_Vars.g_pLog.StartLog();
	}
	if (g_Vars.g_pDataLog.IsStoped())
	{
		g_Vars.g_pDataLog.StartLog();
	}
	if (g_Vars.g_nPrintRoute ==1 )
	{
		if (g_Vars.g_pRouteLog.IsStopedThread())
		{
			g_Vars.g_pRouteLog.StartLog();
		}
	}
	
}
