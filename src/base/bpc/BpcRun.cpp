// BpcRun.cpp: implementation of the CBpcRun class.
//
//////////////////////////////////////////////////////////////////////

#include "BpcRun.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CBpcRun::CBpcRun()
{
	m_pLog = NULL;
}

CBpcRun::~CBpcRun()
{
//	printf("CBpcRun exit ebgin\n");
	for (int i=0; i< m_pDispath.size(); i++)
	{
		delete m_pDispath[i];
		m_pDispath[i]= NULL;
	}
	m_pLog = NULL;
//	printf("CBpcRun exit ok\n");
}

bool CBpcRun::Init(const char *confile)
{
	m_pLog = m_pDrebApi.GetLogPoint();
	if (!m_pRes.Init((char *)confile, m_pLog))
	{
		printf("m_pRes.Init error:%s\n",m_pRes.GetMsg().c_str());
		return false;
	}
	m_pDrebSpi.Init(&m_pDrebApi,&m_pRes);
	m_pDrebSpi.SetGlobalVar(&m_pPoolData,&m_pFuncTbl,&m_pSockMgr);
	m_pDrebSpi.m_lBcRegister = &m_pRes.g_lBCFuncList;
	m_pDrebApi.Init(&m_pRes,&m_pDrebSpi);
	m_pMemPool = m_pDrebApi.GetBufferPool();

	if (m_pRes.g_nRegisterMode == 1) //静态注册
	{
		//读取交易信息文件
		CBF_Xml  txlist;
		if (!txlist.FromFile(m_pRes.g_sRegisterFile.c_str()))
		{
			printf("静态交易注册文件不存在或格式不符 %s\n",m_pRes.g_sRegisterFile.c_str());
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"静态交易注册文件不存在或格式不符 %s",m_pRes.g_sRegisterFile.c_str());
			return false;
		}
		char tmpchar[100];
		CXmlNode *tmpnode = NULL;
		std::string ss;
		S_FUNCINFO_TBL func;
		for (int j=0;j<m_pRes.g_vBpuGroupInfo.size();j++)
		{
			sprintf(tmpchar,"/txinfo/%s",m_pRes.g_vBpuGroupInfo.at(j).g_sBpuGroupName.c_str());
			tmpnode = txlist.GetNodeByPath((const char *)tmpchar,false);
			if (tmpnode == NULL)
			{
				printf("静态交易注册文件无节点 %s\n",tmpchar);
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"静态交易注册文件无节点 %s",tmpchar);
				return false;
			}
			bzero(&func,sizeof(S_FUNCINFO_TBL));
			CBF_Tools::StringCopy(func.sBpuGroupName,20,m_pRes.g_vBpuGroupInfo.at(j).g_sBpuGroupName.c_str());
			tmpnode = (CXmlNode *)tmpnode->GetFirstChild();//取得子节点，即bpu组节点的子节点
			while (tmpnode != NULL)
			{
				if (tmpnode->GetAttribute("code",false,ss) == NULL)
				{
					tmpnode = (CXmlNode *)tmpnode->getNextSibling();
					continue;
				}
				func.nFuncNo = atoi(ss.c_str());
				
				if (tmpnode->GetAttribute("name",false,ss) != NULL)
				{
					CBF_Tools::StringCopy(func.sFuncName,40,ss.c_str());
				}
				if (tmpnode->GetAttribute("prio",false,ss) != NULL)
				{
					func.cPrio = atoi(ss.c_str());
				}
				if (tmpnode->GetAttribute("flag",false,ss) != NULL)
				{
					func.cServiceFlag = atoi(ss.c_str());
				}
				func.nCallNum = 0;
				func.nBpuGroupIndex = j;
				if (!m_pFuncTbl.Insert(func))
				{
					printf("交易已注册 %s %d\n",func.sFuncName,func.nFuncNo);
				    m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"交易已注册 %s %d\n",func.sFuncName,func.nFuncNo);
				}
				tmpnode = (CXmlNode *)tmpnode->getNextSibling();
			}
		}
		m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"使用静态注册，注册文件为%s",m_pRes.g_sRegisterFile.c_str());
	}

	m_pRes.g_bToExit = false;
	m_pPoolData.m_pRes = &m_pRes;
	
	if (!m_pSockMgr.Init(&m_pRes,m_pMemPool))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"%s",m_pSockMgr.m_errMsg);
		return false;
	}
	//序号0为侦听，其后为dreb连接，最后则全是bpu连接
	if (!m_pSockMgr.at(0)->Listen(m_pRes.g_nPort,false,30))
	{
		printf("初始化失败:%s %s 建立%d sokcet侦听失败",m_pRes.g_sCurPath,m_pRes.g_sModuleName,m_pRes.g_nPort);
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"%s %s 初始化失败:建立%d sokcet侦听失败",m_pRes.g_sCurPath,m_pRes.g_sModuleName,m_pRes.g_nPort);
		return false;
	}
#ifdef UNIX
	if (!m_pSockMgr.at(1)->ListenIpc(m_pRes.g_nPort,false,30))
	{
		printf("初始化失败:%s %s 建立IPC %d sokcet侦听失败",m_pRes.g_sCurPath,m_pRes.g_sModuleName,m_pRes.g_nPort);
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"%s %s 初始化失败:建立%d sokcet侦听失败",m_pRes.g_sCurPath,m_pRes.g_sModuleName,m_pRes.g_nPort);
		return false;
	}
	printf("启用BPU IPC连接支持\n");
	
#endif
	int i=0;
	int nbegin=0;
	char tmpchar[400];
	//接收线程有 dreb接收+bpu组接收
	//dreb及侦听为一个线程
	m_pBpuLinkThread.SetGlobalVar(&m_pRes,&m_pPoolData,m_pMemPool,&m_pSockMgr,&m_pFuncTbl);
	m_pBpuLinkThread.m_nBegin = 0;
	m_pBpuLinkThread.m_nEnd = 31;
	nbegin = 31;

	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"总线接收、BPU连接线程连接池范围[%d - %d] 不含[%d]",\
		m_pBpuLinkThread.m_nBegin, m_pBpuLinkThread.m_nEnd, m_pBpuLinkThread.m_nEnd);

	//启动bpu组分派线程
	CDispathThread *disthread=NULL;
	m_pBuThread.m_nBegin = nbegin;
	for (i=0;i<m_pRes.g_vBpuGroupInfo.size();i++)
	{
		m_pRes.g_vBpuGroupInfo[i].g_nBeginSocketIndex = nbegin;
		m_pRes.g_vBpuGroupInfo[i].g_nEndSocketIndex = nbegin+m_pRes.g_vBpuGroupInfo[i].g_nBpuNum;
		nbegin+=100;
		disthread = new CDispathThread();
		disthread->m_nBegin = m_pRes.g_vBpuGroupInfo[i].g_nBeginSocketIndex;
		disthread->m_nEnd = m_pRes.g_vBpuGroupInfo[i].g_nEndSocketIndex;
		disthread->m_nBpugroupindex = i;
		m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"BPU组[%s %d]分派线程 起始socket 连接池[%d %d],不包含%d",\
			m_pRes.g_vBpuGroupInfo[i].g_sBpuGroupName.c_str(),i,disthread->m_nBegin,\
			disthread->m_nEnd,disthread->m_nEnd);
		disthread->SetGlobalVar(&m_pRes,m_pRes.g_vBpuGroupInfo[i].pdispathQueue,m_pMemPool,&m_pSockMgr,&m_pFuncTbl);
		m_pDispath.push_back(disthread);

	}
	m_pBuThread.m_nEnd = nbegin;
	m_pBuThread.SetGlobalVar(&m_pRes,&m_pPoolData,m_pMemPool,&m_pSockMgr,&m_pFuncTbl, &m_pDrebApi);
		m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"BPU接收线程的起始socket index[%d %d],不包含%d",\
			m_pBuThread.m_nBegin,m_pBuThread.m_nEnd,m_pBuThread.m_nEnd);

	//启动线程
	m_pBpuLinkThread.CreateThread();//侦听dreb线程
	m_pBuThread.CreateThread();
	m_pDrebApi.Start();

	//消息处理线程 处理dreb来的请求及bpu外调内调的请求
	m_pMsgThread.SetGlobalVar(&m_pRes,&m_pPoolData,m_pMemPool,&m_pSockMgr,&m_pFuncTbl,&m_pDrebApi);
	m_pMsgThread.CreateThread();
	
	for (i=0;i<m_pDispath.size();i++)
	{
		m_pDispath[i]->CreateThread();
	}
	//启动定时器
	m_pBpuLinkThread.StartTimer();
	m_pBuThread.StartTimer();
	
	if (m_pBpuPidXml.FromFile("BpuPid.xml"))
	{
		for (i=0;i<m_pRes.g_vBpuGroupInfo.size();i++)
		{
			if (m_pRes.g_vBpuGroupInfo[i].g_nDispatchMode == 1) //java版本
			{
				//将BPU的进程ID从xml中取出
				sprintf(tmpchar,"BPU/%s",m_pRes.g_vBpuGroupInfo[i].g_sBpuGroupName.c_str());
				m_pBpuPidXml.GetNodeValueByPath(tmpchar,false,m_pRes.g_vBpuGroupInfo[i].g_nJavaBpuPid);
			}
		}
	}
	return true;
}

void CBpcRun::Stop()
{
	m_pRes.g_bToExit = true;
	m_pBpuLinkThread.StopTimer();
	m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"BPC开始停止");
	m_pBpuLinkThread.Join();
	m_pMsgThread.Join();
	m_pBuThread.StopTimer();
	m_pBuThread.Join();
	m_pDrebApi.Stop();
	int i;
	for (i=0;i<m_pDispath.size();i++)
	{
		m_pDispath[i]->Join();
	}
	m_pSockMgr.at(0)->OnClose("BPC正常退出",__FILE__,__LINE__);
	SLEEP(2000);
	//将bpu的进程ID保存到xml文件
	char tmpchar[200];
	bool isw = false;
	for (i=0;i<m_pRes.g_vBpuGroupInfo.size();i++)
	{
		if (m_pRes.g_vBpuGroupInfo[i].g_nDispatchMode == 1)
		{
			sprintf(tmpchar,"BPU/%s",m_pRes.g_vBpuGroupInfo[i].g_sBpuGroupName.c_str());
			m_pBpuPidXml.SetNodeValueByPath(tmpchar,false,m_pRes.g_vBpuGroupInfo[i].g_nJavaBpuPid);
			isw = true;
		}
		
	}
	if (isw)
	{
		m_pBpuPidXml.ToFile("BpuPid.xml");
	}

}

void CBpcRun::Monitor()
{
	int i,j,k;
	if (m_pBpuLinkThread.IsStoped())
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"重启DREB连接及BPU侦听线程[%s]","CLinkThread");
		m_pBpuLinkThread.CreateThread();
	}
	if (m_pMsgThread.IsStoped())
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"重启BPU分派线程[%s]","MsgThread");
		m_pMsgThread.CreateThread();
	}
	if (m_pBuThread.IsStoped())
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"重启BPU接收线程");
		m_pBuThread.CreateThread();
	}
	for (i=0;i<m_pDispath.size();i++)
	{
		if (m_pDispath[i]->IsStoped())
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"重启分派线程 %d",i);
			m_pDispath[i]->CreateThread();
		}
	}

	S_PROCESS_NODE node;
	int bpugnum=0;
	int connNum=0;
	bool isstart = false;
	char errmsg[200];
	//检查BPU的启动
//	printf("消息队列大小[%d]\n",m_pPoolData.GetSize());
	for (i=0;i<m_pRes.g_vBpuGroupInfo.size();i++)
	{
//		printf("BPU组[%s]队列大小[%d]\n",m_pRes.g_vBpuGroupInfo[i].g_sBpuGroupName.c_str(),\
//			m_pRes.g_vBpuGroupInfo[i].pdispathQueue->GetSize());
		if (m_pRes.g_vBpuGroupInfo[i].g_bIsAutoLoadBu)
		{
			node.prgname = m_pRes.g_vBpuGroupInfo[i].g_sBuPrgName;
			node.prgpara = m_pRes.g_vBpuGroupInfo[i].g_sBuPrgPara;
			node.prgpath = m_pRes.g_vBpuGroupInfo[i].g_sBuPrgPath;
			node.runpath = m_pRes.g_vBpuGroupInfo[i].g_sBuRunPath;
			node.nodeid = m_pRes.g_vBpuGroupInfo[i].g_sBpuGroupName;

			//通过连接取得BPU组的实际启动BPU
			connNum=0;
			for (k=m_pRes.g_vBpuGroupInfo[i].g_nBeginSocketIndex ; k < m_pRes.g_vBpuGroupInfo[i].g_nEndSocketIndex; k++)
			{
				if (m_pSockMgr.at(k)->m_sock != INVALID_SOCKET)
				{
					connNum++;
				}
			}
			if (m_pRes.g_vBpuGroupInfo[i].g_nDispatchMode == 1) //java版本
			{
//				bpugnum = m_pRes.g_vBpuGroupInfo[i].g_nBpuNum - m_pRes.g_vBpuGroupInfo[i].g_nStartBpuNum;
				bpugnum = m_pRes.g_vBpuGroupInfo[i].g_nBpuNum - connNum;
				if (bpugnum >0)
				{
					m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"查找原BPU[%s]进程是否存在[%d]",\
						m_pRes.g_vBpuGroupInfo[i].g_sBpuGroupName.c_str(),m_pRes.g_vBpuGroupInfo[i].g_nJavaBpuPid);
					if (m_pRes.g_vBpuGroupInfo[i].g_nJavaBpuPid > 0) //查看进程是否存在
					{
						node.pid = m_pRes.g_vBpuGroupInfo[i].g_nJavaBpuPid;
						if (m_pMgrBpu.FindProcess(&node,errmsg) == 0) //有此进程
						{
							m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"原BPU[%s]进程[%d]已存在，不用重启",\
								m_pRes.g_vBpuGroupInfo[i].g_sBpuGroupName.c_str(),m_pRes.g_vBpuGroupInfo[i].g_nJavaBpuPid);
							continue;
						}
					}
					//启动进程
					m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"已有[%d]BPU 启动BPU 组[%s] 程序名[%s] 运行参数[%s] 运行目录[%s],程序目录[%s]",\
						connNum,m_pRes.g_vBpuGroupInfo[i].g_sBpuGroupName.c_str(),\
						node.prgname.c_str(),node.prgpara.c_str(),node.runpath.c_str(),node.prgpath.c_str());
					m_pMgrBpu.StartProcess(&node,errmsg);
					m_pRes.g_vBpuGroupInfo[i].g_nJavaBpuPid = node.pid;
					isstart = true;
				}
				
			}
			else
			{
//				bpugnum = m_pRes.g_vBpuGroupInfo[i].g_nBpuNum - m_pRes.g_vBpuGroupInfo[i].g_nStartBpuNum;
				bpugnum = m_pRes.g_vBpuGroupInfo[i].g_nBpuNum - connNum;
				for (int j=0;j<bpugnum;j++)
				{
					m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"已有[%d]BPU 启动BPU 组[%s] 程序名[%s] 运行参数[%s] 运行目录[%s],程序目录[%s]",\
						connNum,m_pRes.g_vBpuGroupInfo[i].g_sBpuGroupName.c_str(),\
						node.prgname.c_str(),node.prgpara.c_str(),node.runpath.c_str(),node.prgpath.c_str());
					m_pMgrBpu.StartProcess(&node,errmsg);
					isstart = true;
				}
			}
		}
	}
	if (isstart)
	{
		//将bpu的进程ID保存到xml文件
		char tmpchar[200];
		bool isw = false;
		for (i=0;i<m_pRes.g_vBpuGroupInfo.size();i++)
		{
			if (m_pRes.g_vBpuGroupInfo[i].g_nDispatchMode == 1)
			{
				sprintf(tmpchar,"BPU/%s",m_pRes.g_vBpuGroupInfo[i].g_sBpuGroupName.c_str());
				m_pBpuPidXml.SetNodeValueByPath(tmpchar,false,m_pRes.g_vBpuGroupInfo[i].g_nJavaBpuPid);
				isw = true;
			}
			
		}
		if (isw)
		{
			m_pBpuPidXml.ToFile("BpuPid.xml");
		}
	}
}

