// GateWay.cpp: implementation of the CGateWay class.
//
//////////////////////////////////////////////////////////////////////

#include "GateWay.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CGateWay::CGateWay()
{

}

CGateWay::~CGateWay()
{

}

bool CGateWay::Init(const char *confile)
{
	printf("%s\n",confile);
	//统一用drebapi的日志类及内存缓冲类
	m_pLog = m_pDrebapi.GetLogPoint();
	m_pMemPool = m_pDrebapi.GetBufferPool();
	if (!g_pRes.Init(confile,m_pLog))
	{
		sprintf(m_sErrMsg,"初始化配置文件[%s]出错",confile);
		return false;
	}
    //初始化 dreb连接缓冲池
    m_pPoolData.m_pRes = &g_pRes;
    m_pSendData.m_pRes = &g_pRes;
	//drebapi初始化
	m_pDrebSpi.SetGlobalVar(&m_pPoolData, &m_pSendData, &g_pAioWork);//设置参数

	m_pDrebSpi.Init(&m_pDrebapi, &g_pRes.g_pDrebRes);  //回调的参数
	m_pDrebSpi.g_pGateRes = &g_pRes;
	
	m_pDrebapi.Init(&g_pRes.g_pDrebRes,&m_pDrebSpi);   //总线api初始，传入回调类

	//设置IO参数
	g_pAioRun.SetParameter(&g_pAioWork,&g_pRes);
	
	//设置消息线程参数
	m_pMsgThread.SetGlobalVar(&g_pRes,&m_pPoolData,m_pMemPool,&m_pDrebapi);
	g_pAioWork.SetGlobalVar(&g_pRes,&m_pPoolData,m_pMemPool,&m_pSendData);
	for (int i=0;i<g_pRes.g_nSendQueThreadNum+1;i++)
	{
		m_pSendThread[i].m_nIndex= i;
		m_pSendThread[i].SetGlobalVar(&g_pRes,&m_pSendData,&g_pAioWork);
	}	
	return true;
}

bool CGateWay::Start()
{
	
	//启动IO线程
	if (!g_pAioRun.Start())
	{
		sprintf(m_sErrMsg,"启动IO线程出错[%s]",g_pAioRun.GetErrMsg().c_str());
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		return false;
	}
	SLEEP_SECONDS(1);
	//启动dreb接收线程
	if (!m_pDrebapi.Start())
	{
		sprintf(m_sErrMsg,"启动DREB api线程出错");
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		return false;
	}
	//启动消息处理线程
	if (!m_pMsgThread.CreateThread())
	{
		sprintf(m_sErrMsg,"启动消息处理线程出错");
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		return false;
	}
	//启动客户端发送处理线程
	for (int i=0;i<g_pRes.g_nSendQueThreadNum+1;i++)
	{

		if (!m_pSendThread[i].CreateThread())
		{
			sprintf(m_sErrMsg,"启动客户端发送处理线程出错[%d]",i);
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
			return false;
		}
	}
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"启动网关成功");
	return true;
}

void CGateWay::Stop()
{
	g_pRes.g_bIsExit = true;
	g_pAioRun.Stop();
	m_pMsgThread.Join();
	m_pDrebapi.Stop();
	m_pLog->LogMp(LOG_INFO, __FILE__, __LINE__, "停止网关1");
	for (int i=0;i<g_pRes.g_nSendQueThreadNum+1;i++)
	{
		m_pSendThread[i].Join();
	}
	m_pLog->LogMp(LOG_INFO, __FILE__, __LINE__, "停止网关2");
	SLEEP_SECONDS(5);

}

std::string CGateWay::GetErrMsg()
{
	return m_sErrMsg;
}

void CGateWay::Monitor()
{
	if (g_pRes.g_pGateLog.IsStoped())
	{
		g_pRes.g_pGateLog.StartLog();
		m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"重启CGATE日志线程");
	}
	if (m_pMsgThread.IsStoped())
	{
		m_pMsgThread.CreateThread();
		m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"重启消息处理线程");
	}
	for (int i=0;i<g_pRes.g_nSendQueThreadNum+1;i++)
	{
		if (m_pSendThread[i].IsStoped())
		{
			m_pSendThread[i].CreateThread();
			m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"重启发送客户端处理线程 %d",i);
		}
	}
	int total;
	int used;
	int size;
	m_pDrebapi.GetBufferPoolInfo(total,used,size);
	m_pLog->LogMp(LOG_PROMPT, __FILE__, __LINE__, "总线api缓存使用总共:%d 使用:%d 大小:%d 发送队列数:%d 请求队列数:%d", total,used,size, m_pSendData.GetSize(), m_pPoolData.GetSize());
    unsigned int i;
	std::vector<PSOCKET_POOL_DATA>reslist;
	g_pAioWork.GetConnInfo(reslist);
    if (reslist.size() > 0)
    {
        FILE* fp = fopen("cgate_connlist.csv", "wb");
        if (fp != NULL)
        {
            char tmpchar[300];
            sprintf(tmpchar, "序号,连接索引,对方ip\n");
            fwrite(tmpchar, 1, strlen(tmpchar), fp);
            for (i = 0; i < reslist.size(); i++)
            {
                if (reslist[i]->s_hSocket != INVALID_SOCKET)
                {
                    sprintf(tmpchar, "%d,%d,%s\n", i, reslist[i]->s_nIndex, inet_ntoa(reslist[i]->s_pSocketAddr.sin_addr));
                    fwrite(tmpchar, 1, strlen(tmpchar), fp);
                }
            }
            fclose(fp);
            fp = NULL;
        }
    }
}
