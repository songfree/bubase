// DrebMsgProcBase.h: interface for the CDrebMsgProcBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DREBMSGPROCBASE_H__EC38AEB7_E135_4E09_92E4_4D13D9D38982__INCLUDED_)
#define AFX_DREBMSGPROCBASE_H__EC38AEB7_E135_4E09_92E4_4D13D9D38982__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "BF_DrebServer.h"

//此为DREBAPI使用的一个例子，可继承来实现
//此线程从api队列里取数据进行处理

class CDrebMsgProcBase : public CBF_Thread  
{
public:
	CDrebMsgProcBase()
	{
		m_bIsRunning = false;
		m_pDrebApi = NULL;
		m_pRes = NULL;
	}
	virtual ~CDrebMsgProcBase()
	{

	}


	// 函数名: Start
	// 编程  : 王明松 2015-4-23 15:52:29
	// 返回  : virtual bool 
	// 描述  : 启动处理线程
	virtual bool Start()
	{
		if (NULL == m_pDrebApi || NULL == m_pRes)
		{
			return false;
		}
		m_bIsRunning = true;
		CreateThread();
		return true;
	}

	// 函数名: Stop
	// 编程  : 王明松 2015-4-23 15:52:43
	// 返回  : virtual void 
	// 描述  : 停止处理线程
	virtual void Stop()
	{
		m_bIsRunning = false;
		Join();
	}

	// 函数名: bool Init
	// 编程  : 王明松 2015-4-23 15:52:55
	// 返回  : virtual  
	// 参数  : CBF_DrebServer   *bus
	// 参数  : CBF_DrebResource *res
	// 描述  : 处理线程初始化
	virtual  bool Init(CBF_DrebServer   *bus,CBF_DrebResource *res)
	{
		m_pDrebApi = bus;
		m_pRes = res;
		return true;
	}
	CBF_DrebServer   *m_pDrebApi;//api实例
	CBF_DrebResource *m_pRes;    //配置项

	S_BPC_RSMSG      m_rMsgData;
	
	virtual std::string GetBpcMsgType(int msgtype)
	{
		switch (msgtype)
		{
		case MSG_REQ:        // 0      //接收请求,bpc发给BPU的请求
			return "MSG_REQ";
		case MSG_FREE:        //1      //BPU发送空闲请求
			return "MSG_FREE";
		case MSG_GETSERIAL:   //4      //BPU取共享唯一流水
			return "MSG_GETSERIAL";
		case MSG_SETGPARA1:  //5      //BPU发送SETGPARA1
			return "MSG_SETGPARA1";
		case MSG_SETGPARA2:   //6      //BPU发送SETGPARA2
			return "MSG_SETGPARA2";
		case MSG_SETGPARA3:   //7      //BPU发送SETGPARA3
			return "MSG_SETGPARA3";	
		case MSG_EXTCALL:      //10      //BPU发送外调请求	
			return "MSG_EXTCALL";
		case MSG_BPCCALL:      //11      //BPU发送PU组调用请求
			return "MSG_BPCCALL";
		case MSG_BPUREG:       //12      //BPU发送注册请求
			return "MSG_BPUREG";
		case MSG_BPUISREG:     //13      //BPU发送是否注册请求
			return "MSG_BPUISREG";
		case MSG_GETNEXT:      //15      //dreb过来的取后续包的消息
			return "MSG_GETNEXT";
		case MSG_BPCMONITOR:      //16      //主动的监控报告，定时发送
			return "MSG_BPCMONITOR";
		case MSG_GETBPCNEXT:      //17      //dreb过来的取在BPC缓存的后续包的消息
			return "MSG_GETBPCNEXT";
		case MSG_CONNECTDREB:      //18      //dreb过来的已连接DREB成功的消息，可在这消息里进行注册等(主备询问，注册)
			return "MSG_CONNECTDREB";
		case MSG_MONITOR_SAP:      //19      //dreb过来的请求监控消息，DREB命令字为CMD_MONITOR_BPC,具体的监控项在service_no里，详见DrebPubDefine.h里的BPCMONI_INFO那里
			return "MSG_MONITOR_SAP";
		case MSG_DREBANS:      //20      //DREB过来的应答信息
			return "MSG_DREBANS";
		default:
			return "";
		}
	}
	virtual std::string GetDrebCmdType(int cmdtype)
	{
		switch (cmdtype)
		{
			
		case  CMD_ROUTER:      //1  //数据总线节点发送路由通知的命令字
			return "CMD_ROUTER";
		case  CMD_DEL_NODE:    //2  //数据总线节点之间取消数据总线节点的命令字,只发送至主动连接的数据总线节点
			return "CMD_DEL_NODE";
		case  CMD_DEL_SVR:     //3  //服务端取消注册服务的命令字
			return "CMD_DEL_SVR";
		case  CMD_PING:        //4  //心跳请求的命令字
			return "CMD_PING";
		case  CMD_CONNECT:     //5  //连接注册
			return "CMD_CONNECT";
		case  CMD_SERVICE:     //6  //服务路由
			return "CMD_SERVICE";
		case  CMD_REGSERVICE:  //7  //注册服务
			return "CMD_REGSERVICE";			
		case  CMD_DPCALL:      //8  //数据总线节点同步调用 要求最终处理完成后应答
			return "CMD_DPCALL";
		case  CMD_DPACALL:     //9  //数据总线节点异步调用 要求接收到的数据总线节点发送至服务后确认应答
			return "CMD_DPACALL";
		case  CMD_DPBC:        //10  //数据总线节点广播，即将信息发给指定数据总线节点上所有注册的服务,要求数据总线节点应答
			return "CMD_DPBC";
		case  CMD_DPABC:       //11  //数据总线节点广播，即将信息发给指定数据总线节点上所有注册的服务,不要求数据总线节点应答
			return "CMD_DPABC";			
		case  CMD_DPPUSH:      //12 //数据总线节点推送，无须应答
			return "CMD_DPPUSH";
		case  CMD_DPPOST:      //13 //数据总线节点投递，要求接收到的数据总线节点应答
			return "CMD_DPPOST";
		case  CMD_UNREGSERVICE:      //14 //取消交易注册
			return "CMD_UNREGSERVICE";
		case  CMD_MONITOR_DREB:     //15 //数据总线节点监控
			return "CMD_MONITOR_DREB";
		case  CMD_MONITOR_BPC:     //16 //数据总线节点监控
			return "CMD_MONITOR_BPC";
		default:
			return "";
		}
	}
protected:
	bool  m_bIsRunning;

	// 函数名: Run
	// 编程  : 王明松 2015-4-23 15:53:15
	// 返回  : virtual int 
	// 描述  : 处理线程入口
	virtual int Run()
	{
		while (m_bIsRunning)
		{
			if (m_pDrebApi->GetMsgData(m_rMsgData))
			{
				ProcessDrebData(m_rMsgData);
			}
		}
		return 0;
	}
	virtual bool InitThreadInstance()
	{
		return true;
	}
	virtual void ExitThreadInstance()
	{

	}

	// 函数名: ProcessDrebData
	// 编程  : 王明松 2015-4-23 15:53:34
	// 返回  : virtual void 
	// 参数  : S_BPC_RSMSG &rcvdata
	// 描述  : 处理从总线接收过来的数据
	virtual void ProcessDrebData(S_BPC_RSMSG &rcvdata)
	{
	    if (rcvdata.sMsgBuf->sDBHead.cZip != 0)
		{
			if (!m_pDrebApi->UnzipBuf(*rcvdata.sMsgBuf))
			{
				sprintf(rcvdata.sMsgBuf->sBuffer,"数据解压缩出错");
				rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
				rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
				rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
				rcvdata.sMsgBuf->sDBHead.cZip = 0;
				m_pDrebApi->SendMsg(rcvdata);
				return ;
			}
		}
		switch (rcvdata.sMsgBuf->sBpcHead.cMsgType)
		{
			case MSG_GETBPCNEXT://在bpc上取后续包
				OnGetBpcNextPack(rcvdata);
				break;
			case MSG_GETNEXT://在BPU上取后续包，要发给BPU处理
				OnGetBpuNextPack(rcvdata);
				break;
			case MSG_REQ://DREB上收到的请求
				OnMsgRequest(rcvdata);
				break;
			case MSG_MONITOR_SAP://DREB上收到的监控指令
				OnMsgMonitor(rcvdata);
				break;
			case MSG_CONNECTDREB://DREB返回连接成功，可以在这里进行注册交易等
				OnMsgConnectBack(rcvdata);
				break;
			case MSG_DREBANS://DREB返回外调等的应答
				OnMsgDrebAns(rcvdata);
				break;	
			case MSG_BPCMONITOR://主动上报监控信息
				OnMsgReportBpc(rcvdata);
				break;	
			default:
				m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
				rcvdata.sMsgBuf = NULL;
				break;
		}
		return;
	}

	// 函数名: OnGetBpcNextPack
	// 编程  : 王明松 2015-4-23 15:53:52
	// 返回  : virtual void 
	// 参数  : S_BPC_RSMSG &rcvdata
	// 描述  : 取本地的后续包,(后续包存在本地)
	virtual void OnGetBpcNextPack(S_BPC_RSMSG &rcvdata)
	{
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
	}

	// 函数名: OnGetBpuNextPack
	// 编程  : 王明松 2015-4-23 15:54:14
	// 返回  : virtual void 
	// 参数  : S_BPC_RSMSG &rcvdata
	// 描述  : 取BPU上的后续包，要发给BPU处理
	virtual void OnGetBpuNextPack(S_BPC_RSMSG &rcvdata)
	{
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
	}

	// 函数名: OnMsgRequest
	// 编程  : 王明松 2015-4-23 15:54:38
	// 返回  : virtual void 
	// 参数  : S_BPC_RSMSG &rcvdata
	// 描述  : 处理总线过来的请求数据
	virtual void OnMsgRequest(S_BPC_RSMSG &rcvdata)
	{
		if (rcvdata.sMsgBuf->sDBHead.cRaflag !=0) //不为请求
		{
			m_pDrebApi->GetLogPoint()->LogMp(LOG_ERROR,__FILE__,__LINE__,"无此功能 消息[%s]  DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 源[%d %d] cZip[%d] 业务数据长度[%d]",\
				GetBpcMsgType(rcvdata.sMsgBuf->sBpcHead.cMsgType).c_str(),\
				GetDrebCmdType(rcvdata.sMsgBuf->sDBHead.cCmd).c_str(),rcvdata.sMsgBuf->sDBHead.cNextFlag,\
				rcvdata.sMsgBuf->sDBHead.cRaflag,rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,\
				rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial,\
				rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nSerial,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nHook,\
						rcvdata.sMsgBuf->sDBHead.cZip,rcvdata.sMsgBuf->sDBHead.nLen);
			m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
			rcvdata.sMsgBuf = NULL;
			return;
		}
		//根据不同的消息类型进行处理
		
	}

	// 函数名: OnMsgMonitor
	// 编程  : 王明松 2015-4-23 15:54:57
	// 返回  : virtual void 
	// 参数  : S_BPC_RSMSG &rcvdata
	// 描述  : 处理总线过来的监控指令
	virtual void OnMsgMonitor(S_BPC_RSMSG &rcvdata)
	{
		//DREB过来的调用
		switch (rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo)
		{
			case BPCMONI_INFO:
				MonitorInfo(rcvdata);
				break;
			case BPCMONI_HOST:
				MonitorHost(rcvdata);
				break;
			case BPCMONI_BPU:
				MonitorBpu(rcvdata);
				break;
			case BPCMONI_REPORT:
				MonitorReport(rcvdata);
				break;
			case BPCMONI_SETBPU:
				MonitorSetBpu(rcvdata);
				break;
			case BPCMONI_SETMODE:
				MonitorSetMode(rcvdata);
				break;
			case BPCMONI_STOPBPU:
				MonitorStopBpu(rcvdata);
				break;
			case BPCMONI_STARTBPU:
				MonitorStartBpu(rcvdata);
				break;
			case BPCMONI_DOWNLOAD:
				Download(rcvdata);
				break;
			case BPCMONI_UPLOAD:
				Upload(rcvdata);
				break;
			default:
				rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
				rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_FUNCNOTFUND;
				rcvdata.sMsgBuf->sDBHead.nLen = 0;
				rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN;
				m_pDrebApi->SendMsg(rcvdata);
				break;
		}
		return;
	}

	// 函数名: OnMsgConnectBack
	// 编程  : 王明松 2015-4-23 15:55:29
	// 返回  : virtual void 
	// 参数  : S_BPC_RSMSG &rcvdata
	// 描述  : 处理连接总线成功的响应
	virtual void OnMsgConnectBack(S_BPC_RSMSG &rcvdata)
	{
		//发送注册等
		m_pDrebApi->GetLogPoint()->LogMp(LOG_DEBUG,__FILE__,__LINE__,"收到DREB连接成功的响应");
	}

	// 函数名: OnMsgDrebAns
	// 编程  : 王明松 2015-4-23 15:55:58
	// 返回  : virtual void 
	// 参数  : S_BPC_RSMSG &rcvdata
	// 描述  : 响应总线过来的应答数据
	virtual void OnMsgDrebAns(S_BPC_RSMSG &rcvdata)
	{
		//应答数据
		m_pDrebApi->GetLogPoint()->LogMp(LOG_DEBUG,__FILE__,__LINE__,"收到DREB的应答响应");
	}

	// 函数名: OnMsgReportBpc
	// 编程  : 王明松 2015-4-23 16:01:43
	// 返回  : virtual void 
	// 参数  : S_BPC_RSMSG &rcvdata
	// 描述  : 主动上报监控信息，发给总线
	virtual void OnMsgReportBpc(S_BPC_RSMSG &rcvdata)
	{
		//监控信息
		CBF_Xml        pXml;

		if (!pXml.FromBuffer(rcvdata.sMsgBuf->sBuffer))
		{
			m_pDrebApi->GetLogPoint()->LogMp(LOG_ERROR,__FILE__,__LINE__,"API过来的非xml格式");
			m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
			rcvdata.sMsgBuf = NULL;
			return;
		}
//		int i,k;
// 		int connNum=0;
// 		//BPC信息
// 		pXml.AddNodeByPath("Monitor/BPU信息",false,"BPULIST",false,"");
// 		for (i=0 ; i<m_pRes->g_vBpuGroupInfo.size(); i++)
// 		{
// 			connNum=0;
// 			for (k=m_pRes->g_vBpuGroupInfo[i].g_nBeginSocketIndex ; k < m_pRes->g_vBpuGroupInfo[i].g_nEndSocketIndex; k++)
// 			{
// 				if (m_pSockMgr->at(k)->m_sock != INVALID_SOCKET)
// 				{
// 					connNum++;
// 				}
// 			}
// 			CXmlNode *bpunode = pXml.AddNodeByPath("Monitor/BPU信息/BPULIST",false,m_pRes->g_vBpuGroupInfo[i].g_sBpuGroupName.c_str(),true,"");
// 			if (bpunode != NULL)
// 			{
// 				bpunode->SetAttribute("名称",false,m_pRes->g_vBpuGroupInfo[i].g_sBpuGroupName);
// 				bpunode->SetAttribute("数目",false,m_pRes->g_vBpuGroupInfo[i].g_nBpuNum);
// 				bpunode->SetAttribute("实际数",false,connNum);
// 				bpunode->SetAttribute("心跳应答",false,m_pRes->g_vBpuGroupInfo[i].g_bIsAnsPing);
// 				bpunode->SetAttribute("分配模式",false,m_pRes->g_vBpuGroupInfo[i].g_nDispatchMode);
// 				bpunode->SetAttribute("自动启动",false,m_pRes->g_vBpuGroupInfo[i].g_bIsAutoLoadBu);
// 				bpunode->SetAttribute("程序名",false,m_pRes->g_vBpuGroupInfo[i].g_sBuPrgName);
// 				bpunode->SetAttribute("程序目录",false,m_pRes->g_vBpuGroupInfo[i].g_sBuPrgPath);
// 				bpunode->SetAttribute("运行目录",false,m_pRes->g_vBpuGroupInfo[i].g_sBuRunPath);
// 				bpunode->SetAttribute("参数",false,m_pRes->g_vBpuGroupInfo[i].g_sBuPrgPara);
// 			}
// 		}

		int len = BPUDATASIZE;
		if (!pXml.ToBuffer(rcvdata.sMsgBuf->sBuffer,len))
		{
			m_pDrebApi->GetLogPoint()->LogMp(LOG_ERROR,__FILE__,__LINE__,"ToBuffer出错");
			m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
			rcvdata.sMsgBuf = NULL;
			return;
		}
		rcvdata.sMsgBuf->sDBHead.nLen = len;
		m_pDrebApi->GetLogPoint()->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"监控通知",rcvdata.sMsgBuf->sBuffer,rcvdata.sMsgBuf->sDBHead.nLen);
		//发给总线	
		rcvdata.sMsgBuf->sBpcHead.nIndex = 100;
		m_pDrebApi->SendMsg(rcvdata);
		return;
	}
	
	virtual void MonitorInfo(S_BPC_RSMSG &rcvdata)
	{
		int i;
		CBF_Xml xmlpack;
		xmlpack.SetNodeValueByPath("Monitor/BPC公共节点",false,(int)m_pRes->g_nSvrMainId);
		xmlpack.SetNodeValueByPath("Monitor/BPC私有节点",false,(int)m_pRes->g_nSvrPrivateId);
		xmlpack.SetNodeValueByPath("Monitor/启动时间",false,m_pRes->g_sStartDateTime,false);
		char sip[40];
		char drebstatus[40];
		int  drebport;
		int  drebid;
		int  drebprivateid;
		
		//连接信息
		for (i=0 ; i < m_pRes->g_vDrebLinkInfo.size() ; i++)
		{
			CXmlNode *connnode = xmlpack.AddNodeByPath("Monitor/DREB连接信息",false,"连接",false,"");
			if (connnode == NULL)
			{
				m_pDrebApi->GetLogPoint()->LogMp(LOG_ERROR,__FILE__,__LINE__,"AddNodeByPath 出错");
				continue;
			}
			bzero(sip,sizeof(sip));
			bzero(drebstatus,sizeof(drebstatus));
			m_pDrebApi->GetDrebInfo(i,sip,drebport,drebid,drebprivateid,drebstatus);
			connnode->SetAttribute("ip",false,sip);
			connnode->SetAttribute("port",false,drebport);
			connnode->SetAttribute("index",false,i);
			connnode->SetAttribute("公共节点",false,drebid);
			connnode->SetAttribute("私有节点",false,drebprivateid);
			connnode->SetAttribute("status",false,drebstatus);
			
		}
		
		//BPU信息
// 		int k;
// 		int connNum=0;
// 		xmlpack.AddNodeByPath("Monitor",false,"BPU列表",false,"");
// 		for (i=0 ; i<m_pRes->g_vBpuGroupInfo.size(); i++)
// 		{
// 			connNum=0;
// 			for (k=m_pRes->g_vBpuGroupInfo[i].g_nBeginSocketIndex ; k < m_pRes->g_vBpuGroupInfo[i].g_nEndSocketIndex; k++)
// 			{
// 				if (m_pSockMgr->at(k)->m_sock != INVALID_SOCKET)
// 				{
// 					connNum++;
// 				}
// 			}
// 			CXmlNode *bpunode = xmlpack.AddNodeByPath("Monitor/BPU列表",false,m_pRes->g_vBpuGroupInfo[i].g_sBpuGroupName.c_str(),true,"");
// 			bpunode->SetAttribute("配置BPU数",false,m_pRes->g_vBpuGroupInfo[i].g_nBpuNum);
// 			bpunode->SetAttribute("实际BPU数",false,connNum);
// 			bpunode->SetAttribute("分配模式",false,m_pRes->g_vBpuGroupInfo[i].g_nDispatchMode);
// 			bpunode->SetAttribute("自动启动",false,m_pRes->g_vBpuGroupInfo[i].g_bIsAutoLoadBu);
// 			bpunode->SetAttribute("开始连接索引",false,m_pRes->g_vBpuGroupInfo[i].g_nBeginSocketIndex);
// 			bpunode->SetAttribute("结束连接索引",false,m_pRes->g_vBpuGroupInfo[i].g_nEndSocketIndex);
// 		}
		
		int len = BPUDATASIZE;
		if (!xmlpack.ToBuffer(rcvdata.sMsgBuf->sBuffer,len))
		{
			sprintf(rcvdata.sMsgBuf->sBuffer,"缓冲不足，请下载bpcinfo.txt查看");
			rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
			m_pDrebApi->GetLogPoint()->LogMp(LOG_ERROR,__FILE__,__LINE__,"MonitorInfo 缓冲不足");
			xmlpack.ToFile("bpcinfo.txt");
		}
		else
		{
			rcvdata.sMsgBuf->sDBHead.nLen = len;
		}
		
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		m_pDrebApi->SendMsg(rcvdata);
	}
	
	virtual void MonitorHost(S_BPC_RSMSG &rcvdata)
	{
		CBF_Xml xmlpack;
		xmlpack.SetNodeValueByPath("Monitor/BPC公共节点",false,(int)m_pRes->g_nSvrMainId);
		xmlpack.SetNodeValueByPath("Monitor/BPC私有节点",false,(int)m_pRes->g_nSvrPrivateId);
		xmlpack.SetNodeValueByPath("Monitor/启动时间",false,m_pRes->g_sStartDateTime,false);
		
		int total;
		int unused;
		int bufsize;
		m_pDrebApi->GetBufferPoolInfo(total,unused,bufsize);
		xmlpack.SetNodeAttribute("Monitor/内存缓冲池",false,"总分配",false,total );
		xmlpack.SetNodeAttribute("Monitor/内存缓冲池",false,"未使用",false,unused );
		xmlpack.SetNodeAttribute("Monitor/内存缓冲池",false,"每块大小",false,bufsize );
		//取主机信息
		S_MONITOR_HOST hf;
		vector<S_MONITOR_DISK> di;
		m_pDrebApi->GetHostInfo(hf,di);
		
		xmlpack.SetNodeValueByPath("Monitor/主机资源/CPU",false,(int)hf.nCpuRate);
		
		
		xmlpack.SetNodeValueByPath("Monitor/主机资源/内存",false,"",false);
		xmlpack.SetNodeAttribute("Monitor/主机资源/内存",false,"总大小M",false,(int)hf.nTotalMemory);
		xmlpack.SetNodeAttribute("Monitor/主机资源/内存",false,"使用M",false,(int)hf.nUsedMemory);
		
		for (int i=0 ; i<di.size() ; i++)
		{
			CXmlNode *disknode = xmlpack.AddNodeByPath("Monitor/主机资源/磁盘列表",false,"磁盘",false,"");
			if (disknode != NULL)
			{
				disknode->SetAttribute("名称",false,di[i].sDiskName);
				disknode->SetAttribute("总大小M",false,(int)di[i].nTotalSpace);
				disknode->SetAttribute("使用大小M",false,(int)di[i].nUsedSpace);
			}
		}
		
		int len = BPUDATASIZE;
		if (!xmlpack.ToBuffer(rcvdata.sMsgBuf->sBuffer,len))
		{
			sprintf(rcvdata.sMsgBuf->sBuffer,"缓冲不足，请下载bpchost.txt查看");
			rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
			m_pDrebApi->GetLogPoint()->LogMp(LOG_ERROR,__FILE__,__LINE__,"MonitorHost 缓冲不足");
			xmlpack.ToFile("bpchost.txt");
		}
		else
		{
			rcvdata.sMsgBuf->sDBHead.nLen = len;
		}
		
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		m_pDrebApi->SendMsg(rcvdata);
	}

	virtual void MonitorBpu(S_BPC_RSMSG &rcvdata)
	{
		
		int connNum=0;
		CBF_Xml xmlpack;
		xmlpack.SetNodeValueByPath("Monitor/BPC公共节点",false,(int)m_pRes->g_nSvrMainId);
		xmlpack.SetNodeValueByPath("Monitor/BPC私有节点",false,(int)m_pRes->g_nSvrPrivateId);
		xmlpack.SetNodeValueByPath("Monitor/启动时间",false,m_pRes->g_sStartDateTime,false);
		//BPU信息
		xmlpack.AddNodeByPath("Monitor",false,"BPU列表",false,"");
//		int i,k;
// 		for (i=0 ; i<m_pRes->g_vBpuGroupInfo.size(); i++)
// 		{
// 			connNum=0;
// 			for (k=m_pRes->g_vBpuGroupInfo[i].g_nBeginSocketIndex ; k < m_pRes->g_vBpuGroupInfo[i].g_nEndSocketIndex; k++)
// 			{
// 				if (m_pSockMgr->at(k)->m_sock != INVALID_SOCKET)
// 				{
// 					connNum++;
// 				}
// 			}
// 			CXmlNode *bpunode = xmlpack.AddNodeByPath("Monitor/BPU列表",false,m_pRes->g_vBpuGroupInfo[i].g_sBpuGroupName.c_str(),true,"");
// 			if (bpunode != NULL)
// 			{
// 				bpunode->SetAttribute("组名",false,m_pRes->g_vBpuGroupInfo[i].g_sBpuGroupName);
// 				bpunode->SetAttribute("配置BPU数",false,m_pRes->g_vBpuGroupInfo[i].g_nBpuNum);
// 				bpunode->SetAttribute("实际BPU数",false,connNum);
// 				bpunode->SetAttribute("心跳应答",false,m_pRes->g_vBpuGroupInfo[i].g_bIsAnsPing);
// 				bpunode->SetAttribute("分配模式",false,m_pRes->g_vBpuGroupInfo[i].g_nDispatchMode);
// 				bpunode->SetAttribute("自动启动",false,m_pRes->g_vBpuGroupInfo[i].g_bIsAutoLoadBu);
// 				bpunode->SetAttribute("程序名",false,m_pRes->g_vBpuGroupInfo[i].g_sBuPrgName);
// 				bpunode->SetAttribute("程序目录",false,m_pRes->g_vBpuGroupInfo[i].g_sBuPrgPath);
// 				bpunode->SetAttribute("运行目录",false,m_pRes->g_vBpuGroupInfo[i].g_sBuRunPath);
// 				bpunode->SetAttribute("运行参数",false,m_pRes->g_vBpuGroupInfo[i].g_sBuPrgPara);
// 				bpunode->SetAttribute("开始连接索引",false,m_pRes->g_vBpuGroupInfo[i].g_nBeginSocketIndex);
// 				bpunode->SetAttribute("结束连接索引",false,m_pRes->g_vBpuGroupInfo[i].g_nEndSocketIndex);
// 				
// 			}
// 		}
		
		int len = BPUDATASIZE;
		if (!xmlpack.ToBuffer(rcvdata.sMsgBuf->sBuffer,len))
		{
			sprintf(rcvdata.sMsgBuf->sBuffer,"缓冲不足，请下载bpcbpu.txt查看");
			rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
			m_pDrebApi->GetLogPoint()->LogMp(LOG_ERROR,__FILE__,__LINE__,"MonitorBpu 缓冲不足");
			xmlpack.ToFile("bpcbpu.txt");
		}
		else
		{
			rcvdata.sMsgBuf->sDBHead.nLen = len;
		}
		
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		m_pDrebApi->SendMsg(rcvdata);
	}

	virtual void MonitorReport(S_BPC_RSMSG &rcvdata)
	{
		CBF_Xml xmlpack;
		xmlpack.SetNodeValueByPath("Monitor/BPC公共节点",false,(int)m_pRes->g_nSvrMainId);
		xmlpack.SetNodeValueByPath("Monitor/BPC私有节点",false,(int)m_pRes->g_nSvrPrivateId);
		xmlpack.SetNodeValueByPath("Monitor/启动时间",false,m_pRes->g_sStartDateTime,false);
		char tmpchar[100];
		char nodename[200];
// 		vector<S_FUNCINFO_TBL> funclist;
// 		m_pFuncTbl->GetAllFuncByGroup(funclist);
// 		
// 		xmlpack.SetNodeValueByPath("Monitor/BPU交易/交易数",false,(int)funclist.size());
// 
// 		std::string bpugroup;
// 		int txnum=0;
// 		bzero(nodename,sizeof(nodename));
// 		for (int i=0 ; i<funclist.size() ; i++)
// 		{
// 			if (bpugroup.compare(funclist[i].sBpuGroupName) !=0)
// 			{
// 				if (strlen(nodename) >0)
// 				{
// 					xmlpack.SetNodeAttribute(nodename,false,"交易数",false,txnum);
// 				}
// 				sprintf(nodename,"Monitor/BPU交易/%s",funclist[i].sBpuGroupName);
// 				bpugroup = funclist[i].sBpuGroupName;
// 				txnum = 0;
// 			}
// 			txnum++;
// 			sprintf(tmpchar,"%d",funclist[i].nFuncNo);
// 			CXmlNode *bpunode = xmlpack.AddNodeByPath(nodename,false,tmpchar,false,"",false);
// 			if (bpunode != NULL)
// 			{
// 				bpunode->SetAttribute("名称",false,funclist[i].sFuncName);
// 				bpunode->SetAttribute("优先级",false,(int)funclist[i].cPrio);
// 				bpunode->SetAttribute("调用次数",false,(int)funclist[i].nCallNum);
// 			}
// 		}
// 		xmlpack.SetNodeAttribute(nodename,false,"交易数",false,txnum);

		int len = BPUDATASIZE;
		if (!xmlpack.ToBuffer(rcvdata.sMsgBuf->sBuffer,len))
		{
			sprintf(rcvdata.sMsgBuf->sBuffer,"缓冲不足，请下载bpureport.txt查看");
			rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
			m_pDrebApi->GetLogPoint()->LogMp(LOG_ERROR,__FILE__,__LINE__,"MonitorReport 缓冲不足");
			xmlpack.ToFile("bpureport.txt");
		}
		else
		{
			rcvdata.sMsgBuf->sDBHead.nLen = len;
		}
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		m_pDrebApi->SendMsg(rcvdata);
	}

	virtual void MonitorSetBpu(S_BPC_RSMSG &rcvdata)
	{
		//设置BPU数目，启动模式，是否保存至配置文件
// 	<Monitor>
// 		<SetBpu mode="0" remark="0只更改内存数据1同时更新到配置文件">
//         <bpu1>
// 				<发送明文 remark="由BPU解压缩解密0,明文发给BPU1">0</发送明文>
// 				<心跳应答 remark="当为c/c++多bpu时，不用应答心跳消息">0</心跳应答>
// 				<分配模式 remark="0表示空闲分配，1表示BPU数只能为1，一直分配">0</分配模式>
// 				<可启动的BPU数 remark="即可以处理业务的单元数目，当请求分配模式为1时只能为1">1</可启动的BPU数>
// 				<AUTOLOADBPU use="0" remark="自动将BPU程序启动1，否则为0">
// 					<BPU程序名 remark="BU程序的名称，不带路径">bfBPUd.exe</BPU程序名>
// 					<BPU程序路径 remark="绝对或相对路径">D:\BUBASE\bin</BPU程序路径>
// 					<BPU运行目录 remark="绝对或相对目录">D:\BUBASE\bin</BPU运行目录>
// 					<BPU启动参数 remark="启动BPU所需的参数">D:\BUBASE\bin\bfBPU.xml</BPU启动参数>
// 				</AUTOLOADBPU>
//         </bpu1>
// 		</SetBpu>
// 	</Monitor>
		if (rcvdata.sMsgBuf->sDBHead.cZip != 0)
		{
			if (!m_pDrebApi->UnzipBuf(*rcvdata.sMsgBuf))
			{
				sprintf(rcvdata.sMsgBuf->sBuffer,"数据解压缩出错");
				rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
				rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
				rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
				rcvdata.sMsgBuf->sDBHead.cZip = 0;
				m_pDrebApi->SendMsg(rcvdata);
				return ;
			}
		}
		CBF_Xml  xmlpack;
		if (!xmlpack.FromBuffer(rcvdata.sMsgBuf->sBuffer))
		{
			sprintf(rcvdata.sMsgBuf->sBuffer,"请求数据非xml");
			rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			m_pDrebApi->SendMsg(rcvdata);
			return ;
		}
		CXmlNode *setnode = xmlpack.GetNodeByPath("Monitor/SetBpu",false);
		if (setnode == NULL)
		{
			sprintf(rcvdata.sMsgBuf->sBuffer,"xml无[Monitor/SetBpu]节点");
			rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			m_pDrebApi->SendMsg(rcvdata);
			return ;
		}
		int savemode=0;
		if (xmlpack.GetNodeAttribute("Monitor/SetBpu",false,"mode",false,savemode) == NULL)
		{
			savemode =0;//无属性默认为0
		}
		std::string bpugroupname;
		int i;
		int bpuindex=-1;
		char tmpchar[200];
		char tmpcharconf[200];
		bool isfind=false;
		std::string namevalue;
		if (savemode == 1)
		{
// 			CBF_Date_Time dt;
// 			sprintf(tmpchar,"%s.%s%s",m_pRes->m_sBpcParaFileName.c_str(),dt.ToStringD8().c_str(),dt.ToStringT6().c_str());
// 			m_pRes->m_xmlconf.ToFile(tmpchar);
		}
		
		
// 		CXmlNode *bpugnode = (CXmlNode *)setnode->GetFirstChild();
// 		while (bpugnode != NULL)
// 		{
// 			bpugroupname = bpugnode->GetNodeName();
// 			isfind=false;
// 			for (i=0; i< m_pRes->g_vBpuGroupInfo.size() ; i++)
// 			{
// 				if (bpugroupname.compare(m_pRes->g_vBpuGroupInfo[i].g_sBpuGroupName) == 0)
// 				{
// 					isfind = true;
// 					bpuindex = i;
// 					break;
// 				}
// 			}
// 			if (isfind)
// 			{
// 				//找到了
// 				sprintf(tmpchar,"Monitor/SetBpu/%s/发送明文",bpugroupname.c_str());
// 				if (xmlpack.GetNodeValueByPath(tmpchar,false,namevalue) != NULL)
// 				{
// 					if (savemode == 1) //更新配置文件
// 					{
// 						sprintf(tmpcharconf,"package/head/BPU配置/BPU组/%s/发送明文",bpugroupname.c_str());
// 						m_pRes->m_xmlconf.SetNodeValueByPath(tmpcharconf,false,namevalue.c_str());
// 					}
// 					//更新内存
// 					if (atoi(namevalue.c_str()) == 1)
// 					{
// 						m_pRes->g_vBpuGroupInfo[bpuindex].g_bIsUnencrypt  = true;
// 					}
// 					else
// 					{
// 						m_pRes->g_vBpuGroupInfo[bpuindex].g_bIsUnencrypt  = false;
// 					}
// 				}
// 				sprintf(tmpchar,"Monitor/SetBpu/%s/心跳应答",bpugroupname.c_str());
// 				if (xmlpack.GetNodeValueByPath(tmpchar,false,namevalue) != NULL)
// 				{
// 					if (savemode == 1) //更新配置文件
// 					{
// 						sprintf(tmpcharconf,"package/head/BPU配置/BPU组/%s/心跳应答",bpugroupname.c_str());
// 						m_pRes->m_xmlconf.SetNodeValueByPath(tmpcharconf,false,namevalue.c_str());
// 					}
// 					//更新内存
// 					if (atoi(namevalue.c_str()) == 1)
// 					{
// 						m_pRes->g_vBpuGroupInfo[bpuindex].g_bIsAnsPing  = true;
// 					}
// 					else
// 					{
// 						m_pRes->g_vBpuGroupInfo[bpuindex].g_bIsAnsPing  = false;
// 					}
// 				}
// 				sprintf(tmpchar,"Monitor/SetBpu/%s/分配模式",bpugroupname.c_str());
// 				if (xmlpack.GetNodeValueByPath(tmpchar,false,namevalue) != NULL)
// 				{
// 					if (savemode == 1) //更新配置文件
// 					{
// 						sprintf(tmpcharconf,"package/head/BPU配置/BPU组/%s/分配模式",bpugroupname.c_str());
// 						m_pRes->m_xmlconf.SetNodeValueByPath(tmpcharconf,false,namevalue.c_str());
// 					}
// 					//更新内存
// 					m_pRes->g_vBpuGroupInfo[bpuindex].g_nDispatchMode = atoi(namevalue.c_str());
// 				}
// 				sprintf(tmpchar,"Monitor/SetBpu/%s/可启动的BPU数",bpugroupname.c_str());
// 				if (xmlpack.GetNodeValueByPath(tmpchar,false,namevalue) != NULL)
// 				{
// 					if (m_pRes->g_vBpuGroupInfo[bpuindex].g_nDispatchMode == 1)
// 					{
// 						namevalue = "1";
// 					}
// 					//更新内存
// 					m_pRes->g_vBpuGroupInfo[bpuindex].g_nBpuNum  = atoi(namevalue.c_str());
// 					
// 					if (savemode == 1) //更新配置文件
// 					{
// 						sprintf(tmpcharconf,"package/head/BPU配置/BPU组/%s/可启动的BPU数",bpugroupname.c_str());
// 						m_pRes->m_xmlconf.SetNodeValueByPath(tmpcharconf,false,namevalue.c_str());
// 					}
// 				}
// 				
// 				sprintf(tmpchar,"Monitor/SetBpu/%s/AUTOLOADBPU",bpugroupname.c_str());
// 				if (xmlpack.GetNodeAttribute(tmpchar,false,"use",false,namevalue) != NULL)
// 				{
// 					if (atoi(namevalue.c_str()) == 1)
// 					{
// 						m_pRes->g_vBpuGroupInfo[bpuindex].g_bIsAutoLoadBu = true;
// 					}
// 					else
// 					{
// 						m_pRes->g_vBpuGroupInfo[bpuindex].g_bIsAutoLoadBu = false;
// 					}
// 
// 					if (savemode == 1) //更新配置文件
// 					{
// 						sprintf(tmpcharconf,"package/head/BPU配置/BPU组/%s/AUTOLOADBPU",bpugroupname.c_str());
// 						m_pRes->m_xmlconf.SetNodeAttribute(tmpcharconf,false,"use",false,namevalue.c_str());
// 					}
// 					if (m_pRes->g_vBpuGroupInfo[bpuindex].g_bIsAutoLoadBu)//自动启动BPU
// 					{
// 						sprintf(tmpchar,"Monitor/SetBpu/%s/AUTOLOADBPU/BPU程序名",bpugroupname.c_str());
// 						if (xmlpack.GetNodeValueByPath(tmpchar,false,namevalue) != NULL)
// 						{
// 							m_pRes->g_vBpuGroupInfo[bpuindex].g_sBuPrgName = namevalue;
// 							if (savemode == 1) //更新配置文件
// 							{
// 								sprintf(tmpcharconf,"package/head/BPU配置/BPU组/%s/AUTOLOADBPU/BPU程序名",bpugroupname.c_str());
// 								m_pRes->m_xmlconf.SetNodeValueByPath(tmpcharconf,false,namevalue.c_str());
// 							}
// 						}
// 						sprintf(tmpchar,"Monitor/SetBpu/%s/AUTOLOADBPU/BPU程序路径",bpugroupname.c_str());
// 						if (xmlpack.GetNodeValueByPath(tmpchar,false,namevalue) != NULL)
// 						{
// 							m_pRes->g_vBpuGroupInfo[bpuindex].g_sBuPrgPath = namevalue;
// 							if (savemode == 1) //更新配置文件
// 							{
// 								sprintf(tmpcharconf,"package/head/BPU配置/BPU组/%s/AUTOLOADBPU/BPU程序路径",bpugroupname.c_str());
// 								m_pRes->m_xmlconf.SetNodeValueByPath(tmpcharconf,false,namevalue.c_str());
// 							}
// 						}
// 						sprintf(tmpchar,"Monitor/SetBpu/%s/AUTOLOADBPU/BPU运行目录",bpugroupname.c_str());
// 						if (xmlpack.GetNodeValueByPath(tmpchar,false,namevalue) != NULL)
// 						{
// 							m_pRes->g_vBpuGroupInfo[bpuindex].g_sBuRunPath = namevalue;
// 							if (savemode == 1) //更新配置文件
// 							{
// 								sprintf(tmpcharconf,"package/head/BPU配置/BPU组/%s/AUTOLOADBPU/BPU运行目录",bpugroupname.c_str());
// 								m_pRes->m_xmlconf.SetNodeValueByPath(tmpcharconf,false,namevalue.c_str());
// 							}
// 						}
// 						sprintf(tmpchar,"Monitor/SetBpu/%s/AUTOLOADBPU/BPU启动参数",bpugroupname.c_str());
// 						if (xmlpack.GetNodeValueByPath(tmpchar,false,namevalue) != NULL)
// 						{
// 							m_pRes->g_vBpuGroupInfo[bpuindex].g_sBuPrgPara = namevalue;
// 							if (savemode == 1) //更新配置文件
// 							{
// 								sprintf(tmpcharconf,"package/head/BPU配置/BPU组/%s/AUTOLOADBPU/BPU启动参数",bpugroupname.c_str());
// 								m_pRes->m_xmlconf.SetNodeValueByPath(tmpcharconf,false,namevalue.c_str());
// 							}
// 						}
// 					}
// 				}
// 				if (savemode == 1)
// 				{
// 					if (!m_pRes->m_xmlconf.ToFile(m_pRes->m_sBpcParaFileName.c_str()))
// 					{
// 						m_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"更新配置文件[%s]失败",m_pRes->m_sBpcParaFileName.c_str());
// 					}
// 				}
// 			}
// 			else
// 			{
// 				m_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"bpu组[%s]在BPC配置中不存在",bpugroupname.c_str());
// 			}
// 			bpugnode = (CXmlNode *)bpugnode->getNextSibling();
// 		}
		sprintf(rcvdata.sMsgBuf->sBuffer,"设置参数成功");
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		m_pDrebApi->SendMsg(rcvdata);
		return ;
	}

	virtual  void MonitorSetMode(S_BPC_RSMSG &rcvdata)
	{
		if (rcvdata.sMsgBuf->sDBHead.cZip != 0)
		{
			if (!m_pDrebApi->UnzipBuf(*rcvdata.sMsgBuf))
			{
				sprintf(rcvdata.sMsgBuf->sBuffer,"数据解压缩出错");
				rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
				rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
				rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
				rcvdata.sMsgBuf->sDBHead.cZip = 0;
				m_pDrebApi->SendMsg(rcvdata);
				return ;
			}
		}
		CBF_Xml  xmlpack;
		if (!xmlpack.FromBuffer(rcvdata.sMsgBuf->sBuffer))
		{
			sprintf(rcvdata.sMsgBuf->sBuffer,"请求数据非xml");
			rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			m_pDrebApi->SendMsg(rcvdata);
			return ;
		}
		CXmlNode *setnode = xmlpack.GetNodeByPath("Monitor/SetBpu",false);
		if (setnode == NULL)
		{
			sprintf(rcvdata.sMsgBuf->sBuffer,"xml无[Monitor/SetBpu]节点");
			rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			m_pDrebApi->SendMsg(rcvdata);
			return ;
		}
		int savemode=0;
		if (xmlpack.GetNodeAttribute("Monitor/SetMonitor",false,"mode",false,savemode) == NULL)
		{
			savemode =0;//无属性默认为0
		}
		std::string bpugroupname;
		int i;
		
		char tmpchar[200];
		char tmpcharconf[200];
		bool isfind=false;
		std::string namevalue;
		if (savemode == 1)
		{
// 			CBF_Date_Time dt;
// 			sprintf(tmpchar,"%s.%s%s",m_pRes->m_sBpcParaFileName.c_str(),dt.ToStringD8().c_str(),dt.ToStringT6().c_str());
// 			m_pRes->m_xmlconf.ToFile(tmpchar);
		}
	// 	int   g_nUseMonitor;       //是否监控
	// 	int   g_nMonitorHost;      //是否报告主机资源
	// 	int   g_nMonitorDrebId;            //公共节点号
	// 	int   g_nMonitorDrebPrivateId;     //私有节点号
	// 	int   g_nMonitorSvrId;            //公共服务节点号
	// 	int   g_nMonitorSvrPrivateId;     //私有服务节点号
	// 	int   g_nMonitorTxCode;           //监控交易码
// 		if (xmlpack.GetNodeAttribute("Monitor/SetMonitor/监控配置",false,"use",false,namevalue) != NULL)
// 		{
// 			if (savemode == 1) //更新配置文件
// 			{
// 				sprintf(tmpcharconf,"package/head/监控配置");
// 				m_pRes->m_xmlconf.SetNodeAttribute(tmpcharconf,false,"use",false,namevalue.c_str(),false);
// 			}
// 			//更新内存
// 			m_pRes->g_nUseMonitor = atoi(namevalue.c_str());
// 		}
// 		if (xmlpack.GetNodeAttribute("Monitor/SetMonitor/监控配置",false,"host",false,namevalue) != NULL)
// 		{
// 			if (savemode == 1) //更新配置文件
// 			{
// 				sprintf(tmpcharconf,"package/head/监控配置");
// 				m_pRes->m_xmlconf.SetNodeAttribute(tmpcharconf,false,"host",false,namevalue.c_str(),false);
// 			}
// 			//更新内存
// 			m_pRes->g_nMonitorHost = atoi(namevalue.c_str());
// 		}
// 		if (xmlpack.GetNodeValueByPath("Monitor/SetMonitor/监控公共节点号",false,namevalue) != NULL)
// 		{
// 			if (savemode == 1) //更新配置文件
// 			{
// 				sprintf(tmpcharconf,"package/head/监控配置/监控公共节点号");
// 				m_pRes->m_xmlconf.SetNodeValueByPath(tmpcharconf,false,namevalue.c_str(),false);
// 			}
// 			//更新内存
// 			m_pRes->g_nMonitorDrebId = atoi(namevalue.c_str());
// 		}
// 		if (xmlpack.GetNodeValueByPath("Monitor/SetMonitor/监控私有节点号",false,namevalue) != NULL)
// 		{
// 			if (savemode == 1) //更新配置文件
// 			{
// 				sprintf(tmpcharconf,"package/head/监控配置/监控私有节点号");
// 				m_pRes->m_xmlconf.SetNodeValueByPath(tmpcharconf,false,namevalue.c_str(),false);
// 			}
// 			//更新内存
// 			m_pRes->g_nMonitorDrebPrivateId = atoi(namevalue.c_str());
// 		}
// 		if (xmlpack.GetNodeValueByPath("Monitor/SetMonitor/监控公共服务号",false,namevalue) != NULL)
// 		{
// 			if (savemode == 1) //更新配置文件
// 			{
// 				sprintf(tmpcharconf,"package/head/监控配置/监控公共服务号");
// 				m_pRes->m_xmlconf.SetNodeValueByPath(tmpcharconf,false,namevalue.c_str(),false);
// 			}
// 			//更新内存
// 			m_pRes->g_nMonitorSvrId = atoi(namevalue.c_str());
// 		}
// 		if (xmlpack.GetNodeValueByPath("Monitor/SetMonitor/监控私有服务号",false,namevalue) != NULL)
// 		{
// 			if (savemode == 1) //更新配置文件
// 			{
// 				sprintf(tmpcharconf,"package/head/监控配置/监控私有服务号");
// 				m_pRes->m_xmlconf.SetNodeValueByPath(tmpcharconf,false,namevalue.c_str(),false);
// 			}
// 			//更新内存
// 			m_pRes->g_nMonitorSvrPrivateId = atoi(namevalue.c_str());
// 		}
// 		if (xmlpack.GetNodeValueByPath("Monitor/SetMonitor/监控报告交易码",false,namevalue) != NULL)
// 		{
// 			if (savemode == 1) //更新配置文件
// 			{
// 				sprintf(tmpcharconf,"package/head/监控配置/监控报告交易码");
// 				m_pRes->m_xmlconf.SetNodeValueByPath(tmpcharconf,false,namevalue.c_str(),false);
// 			}
// 			//更新内存
// 			m_pRes->g_nMonitorTxCode = atoi(namevalue.c_str());
// 		}
// 		if (savemode == 1)
// 		{
// 			if (!m_pRes->m_xmlconf.ToFile(m_pRes->m_sBpcParaFileName.c_str()))
// 			{
// 				m_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"更新配置文件[%s]失败",m_pRes->m_sBpcParaFileName.c_str());
// 				sprintf(rcvdata.sMsgBuf->sBuffer,"更新配置文件[%s]失败",m_pRes->m_sBpcParaFileName.c_str());
// 				rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
// 				rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
// 				rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
// 				rcvdata.sMsgBuf->sDBHead.cZip = 0;
// 				m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
// 				return;
// 			}
// 		}
		sprintf(rcvdata.sMsgBuf->sBuffer,"设置参数成功");
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		m_pDrebApi->SendMsg(rcvdata);
		return;
	}

	virtual  void MonitorStopBpu(S_BPC_RSMSG &rcvdata)
	{
		if (rcvdata.sMsgBuf->sDBHead.cZip != 0)
		{
			if (!m_pDrebApi->UnzipBuf(*rcvdata.sMsgBuf))
			{
				sprintf(rcvdata.sMsgBuf->sBuffer,"数据解压缩出错");
				rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
				rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
				rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
				rcvdata.sMsgBuf->sDBHead.cZip = 0;
				m_pDrebApi->SendMsg(rcvdata);
				return ;
			}
		}
		CBF_Xml  xmlpack;
		if (!xmlpack.FromBuffer(rcvdata.sMsgBuf->sBuffer))
		{
			sprintf(rcvdata.sMsgBuf->sBuffer,"请求数据非xml");
			rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			m_pDrebApi->SendMsg(rcvdata);
			return ;
		}
		CXmlNode *stopnode = xmlpack.GetNodeByPath("Monitor/StopBpu",false);
		if (stopnode == NULL)
		{
			sprintf(rcvdata.sMsgBuf->sBuffer,"xml无[Monitor/StopBpu]节点");
			rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			m_pDrebApi->SendMsg(rcvdata);
			return ;
		}
		int i;
		int bpuindex=-1;
		std::string bpugroupname;
		int bpugnum=0;
//		S_PROCESS_NODE node;
//		bool isfind = false;
// 		CXmlNode *bpunode = (CXmlNode *)stopnode->GetFirstChild();
// 		while (bpunode != NULL)
// 		{
// 			bpugroupname = bpunode->GetNodeName();
// 			isfind=false;
// 			for (i=0; i< m_pRes->g_vBpuGroupInfo.size() ; i++)
// 			{
// 				if (bpugroupname.compare(m_pRes->g_vBpuGroupInfo[i].g_sBpuGroupName) == 0)
// 				{
// 					isfind = true;
// 					bpuindex = i;
// 					break;
// 				}
// 			}
// 			if (isfind)
// 			{
// 				node.prgname = m_pRes->g_vBpuGroupInfo[bpuindex].g_sBuPrgName;
// 				node.prgpara = m_pRes->g_vBpuGroupInfo[bpuindex].g_sBuPrgPara;
// 				node.prgpath = m_pRes->g_vBpuGroupInfo[bpuindex].g_sBuPrgPath;
// 				node.runpath = m_pRes->g_vBpuGroupInfo[bpuindex].g_sBuRunPath;
// 				node.nodeid = m_pRes->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName;
// 				
// 				if (m_pRes->g_vBpuGroupInfo[bpuindex].g_nDispatchMode == 1) //java版本
// 				{
// 					m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"查找原BPU[%s]进程是否存在[%d]",\
// 						m_pRes->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str(),m_pRes->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid);
// 					if (m_pRes->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid > 0) //查看进程是否存在
// 					{
// 						node.pid = m_pRes->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid;
// 						if (m_pMgrBpu.FindProcess(&node) == 0) //有此进程
// 						{
// 							m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"原BPU[%s]进程[%d]已存在，停止",\
// 								m_pRes->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str(),m_pRes->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid);
// 							if (m_pMgrBpu.StopProcess(&node)!=0)
// 							{
// 								m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"BPU[%s]进程[%d]停止失败",\
// 									m_pRes->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str(),m_pRes->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid);
// 							}
// 						}
// 					}
// 				}
// 				else
// 				{
// 					for (int j=m_pRes->g_vBpuGroupInfo[bpuindex].g_nBeginSocketIndex ; j<m_pRes->g_vBpuGroupInfo[bpuindex].g_nEndSocketIndex ;j++)
// 					{
// 						if (m_pSockMgr->at(j)->m_sock != INVALID_SOCKET)
// 						{
// 							node.pid = m_pSockMgr->at(j)->m_nPid;
// 							m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"停止BPU[%s]进程[%d]",\
// 								m_pRes->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str(),node.pid);
// 							if (m_pMgrBpu.StopProcess(&node)!=0)
// 							{
// 								m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"BPU[%s]进程[%d]停止失败",\
// 									m_pRes->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str(),node.pid);
// 							}
// 						}
// 					}
// 				}
// 				
// 			}
// 			bpunode = (CXmlNode *)bpunode->getNextSibling();
// 		}
		sprintf(rcvdata.sMsgBuf->sBuffer,"停止BPU成功");
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		m_pDrebApi->SendMsg(rcvdata);
		return;
	}

	virtual void MonitorStartBpu(S_BPC_RSMSG &rcvdata)
	{
		if (rcvdata.sMsgBuf->sDBHead.cZip != 0)
		{
			if (!m_pDrebApi->UnzipBuf(*rcvdata.sMsgBuf))
			{
				sprintf(rcvdata.sMsgBuf->sBuffer,"数据解压缩出错");
				rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
				rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
				rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
				rcvdata.sMsgBuf->sDBHead.cZip = 0;
				m_pDrebApi->SendMsg(rcvdata);
				return ;
			}
		}
		CBF_Xml  xmlpack;
		if (!xmlpack.FromBuffer(rcvdata.sMsgBuf->sBuffer))
		{
			sprintf(rcvdata.sMsgBuf->sBuffer,"请求数据非xml");
			rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			m_pDrebApi->SendMsg(rcvdata);
			return ;
		}
		CXmlNode *stopnode = xmlpack.GetNodeByPath("Monitor/StartBpu",false);
		if (stopnode == NULL)
		{
			sprintf(rcvdata.sMsgBuf->sBuffer,"xml无[Monitor/StartBpu]节点");
			rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			m_pDrebApi->SendMsg(rcvdata);
			return ;
		}
//		int i;
//		int bpuindex=-1;
//		std::string bpugroupname;
//		int bpugnum=0;
//		S_PROCESS_NODE node;
//		bool isfind = false;
//		int k;
//		int connNum=0;
// 		CXmlNode *bpunode = (CXmlNode *)stopnode->GetFirstChild();
// 		while (bpunode != NULL)
// 		{
// 			bpugroupname = bpunode->GetNodeName();
// 			isfind=false;
// 			for (i=0; i< m_pRes->g_vBpuGroupInfo.size() ; i++)
// 			{
// 				if (bpugroupname.compare(m_pRes->g_vBpuGroupInfo[i].g_sBpuGroupName) == 0)
// 				{
// 					isfind = true;
// 					bpuindex = i;
// 					break;
// 				}
// 			}
// 			if (isfind)
// 			{
// 				node.prgname = m_pRes->g_vBpuGroupInfo[bpuindex].g_sBuPrgName;
// 				node.prgpara = m_pRes->g_vBpuGroupInfo[bpuindex].g_sBuPrgPara;
// 				node.prgpath = m_pRes->g_vBpuGroupInfo[bpuindex].g_sBuPrgPath;
// 				node.runpath = m_pRes->g_vBpuGroupInfo[bpuindex].g_sBuRunPath;
// 				node.nodeid = m_pRes->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName;
// 
// 				connNum=0;
// 				for (k=m_pRes->g_vBpuGroupInfo[bpuindex].g_nBeginSocketIndex ; k < m_pRes->g_vBpuGroupInfo[bpuindex].g_nEndSocketIndex; k++)
// 				{
// 					if (m_pSockMgr->at(k)->m_sock != INVALID_SOCKET)
// 					{
// 						connNum++;
// 					}
// 				}
// 				if (!m_pRes->g_vBpuGroupInfo[bpuindex].g_bIsAutoLoadBu) //不是自动重启才进入启动
// 				{
// 					if (m_pRes->g_vBpuGroupInfo[bpuindex].g_nDispatchMode == 1) //java版本
// 					{
// 	//					bpugnum = m_pRes->g_vBpuGroupInfo[bpuindex].g_nBpuNum - m_pRes->g_vBpuGroupInfo[bpuindex].g_nStartBpuNum;
// 						bpugnum = m_pRes->g_vBpuGroupInfo[bpuindex].g_nBpuNum - connNum;
// 						if (bpugnum >0)
// 						{
// 							m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"查找原BPU[%s]进程是否存在[%d]",\
// 								m_pRes->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str(),m_pRes->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid);
// 							if (m_pRes->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid > 0) //查看进程是否存在
// 							{
// 								node.pid = m_pRes->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid;
// 								if (m_pMgrBpu.FindProcess(&node) == 0) //有此进程
// 								{
// 									m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"原BPU[%s]进程[%d]已存在，停止",\
// 										m_pRes->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str(),m_pRes->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid);
// 									if (m_pMgrBpu.StopProcess(&node)!=0)
// 									{
// 										m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"BPU[%s]进程[%d]停止失败",\
// 											m_pRes->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str(),m_pRes->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid);
// 									}
// 								}
// 								m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"BPU[%s]重启",\
// 									m_pRes->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str());
// 								if (m_pMgrBpu.StopProcess(&node)!=0)
// 								{
// 									m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"BPU[%s]进程[%d]停止失败",\
// 										m_pRes->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str(),m_pRes->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid);
// 								}
// 							}
// 						}
// 					}
// 					else
// 					{
// 	//					bpugnum = m_pRes->g_vBpuGroupInfo[bpuindex].g_nBpuNum - m_pRes->g_vBpuGroupInfo[bpuindex].g_nStartBpuNum;
// 						bpugnum = m_pRes->g_vBpuGroupInfo[bpuindex].g_nBpuNum - connNum;
// 						for (int j=0; j<bpugnum;j++)
// 						{
// 							if (m_pMgrBpu.StartProcess(&node)!=0)
// 							{
// 								m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"BPU[%s]启动失败",\
// 									m_pRes->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str());
// 							}
// 						}
// 					}
// 				}
// 			}
// 			bpunode = (CXmlNode *)bpunode->getNextSibling();
// 		}
		sprintf(rcvdata.sMsgBuf->sBuffer,"启动BPU成功");
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		m_pDrebApi->SendMsg(rcvdata);
		return;
	}
	
	virtual void Download(S_BPC_RSMSG &rcvdata)
	{
		if (rcvdata.sMsgBuf->sDBHead.cZip != 0)
		{
			if (!m_pDrebApi->UnzipBuf(*rcvdata.sMsgBuf))
			{
				sprintf(rcvdata.sMsgBuf->sBuffer,"数据解压缩出错");
				rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
				rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
				rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
				rcvdata.sMsgBuf->sDBHead.cZip = 0;
				m_pDrebApi->SendMsg(rcvdata);
				return ;
			}
		}
		S_BPC_DUFILE sfile;
		if (rcvdata.sMsgBuf->sDBHead.nLen  < sizeof(S_BPC_DUFILE))
		{
			m_pDrebApi->GetLogPoint()->LogBin(LOG_ERROR,__FILE__,__LINE__,"请求数据非下载数据",\
				rcvdata.sMsgBuf->sBuffer,rcvdata.sMsgBuf->sDBHead.nLen);
			sprintf(rcvdata.sMsgBuf->sBuffer,"请求数据非下载数据");
			rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_MONITOR_DOWNLOAD;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			m_pDrebApi->SendMsg(rcvdata);
			return ;
		}
		memcpy(&sfile,rcvdata.sMsgBuf->sBuffer,sizeof(S_BPC_DUFILE));
		struct_stat fstat;
		FSTAT(sfile.sFileName,&fstat);
		if (fstat.st_size <= 0)
		{
			sprintf(rcvdata.sMsgBuf->sBuffer,"文件[%s]不存在",sfile.sFileName);
			rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_MONITOR_DOWNLOAD;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			m_pDrebApi->SendMsg(rcvdata);
			return ;
		}
		int offset = atol(sfile.sFileOffset);
		if (offset >= fstat.st_size)
		{
			sprintf(rcvdata.sMsgBuf->sBuffer,"文件偏移量[%d]已大于等于文件大小",offset);
			rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_MONITOR_DOWNLOAD;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			m_pDrebApi->SendMsg(rcvdata);
			return ;
		}
		sprintf(sfile.sFileSize,"%011ld",fstat.st_size);
		FILE *fp = fopen(sfile.sFileName,"rb");
		if (fp == NULL)
		{
			sprintf(rcvdata.sMsgBuf->sBuffer,"文件[%s]不存在",sfile.sFileName);
			rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_MONITOR_DOWNLOAD;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			m_pDrebApi->SendMsg(rcvdata);
			return ;
		}
		fseek(fp,offset,1);
		int getlen = fread(rcvdata.sMsgBuf->sBuffer+sizeof(S_BPC_DUFILE),1,50*1024,fp);
		fclose(fp);
		sprintf(sfile.sFileOffset,"%011ld",getlen + offset);
		if ((getlen + offset) >= fstat.st_size)
		{
			sfile.cEndFlag = '1';
		}
		else
		{
			sfile.cEndFlag = '0';
		}
		memcpy(rcvdata.sMsgBuf->sBuffer,&sfile,sizeof(S_BPC_DUFILE));
		rcvdata.sMsgBuf->sDBHead.nLen = sizeof(S_BPC_DUFILE)+getlen;
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		m_pDrebApi->SendMsg(rcvdata);
		return;

	}

	virtual void Upload(S_BPC_RSMSG &rcvdata)
	{
		if (rcvdata.sMsgBuf->sDBHead.cZip != 0)
		{
			if (!m_pDrebApi->UnzipBuf(*rcvdata.sMsgBuf))
			{
				sprintf(rcvdata.sMsgBuf->sBuffer,"数据解压缩出错");
				rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
				rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
				rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
				rcvdata.sMsgBuf->sDBHead.cZip = 0;
				m_pDrebApi->SendMsg(rcvdata);
				return ;
			}
		}
		S_BPC_DUFILE sfile;
		if (rcvdata.sMsgBuf->sDBHead.nLen  <= sizeof(S_BPC_DUFILE))
		{
			m_pDrebApi->GetLogPoint()->LogBin(LOG_ERROR,__FILE__,__LINE__,"请求数据非上传数据",\
				rcvdata.sMsgBuf->sBuffer,rcvdata.sMsgBuf->sDBHead.nLen);
			sprintf(rcvdata.sMsgBuf->sBuffer,"请求数据非上传数据");
			rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_MONITOR_UPLOAD;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			m_pDrebApi->SendMsg(rcvdata);
			return ;
		}
		memcpy(&sfile,rcvdata.sMsgBuf->sBuffer,sizeof(S_BPC_DUFILE));
		m_pDrebApi->GetLogPoint()->LogBin(LOG_DEBUG,__FILE__,__LINE__,"上传文件",rcvdata.sMsgBuf->sBuffer,rcvdata.sMsgBuf->sDBHead.nLen );
		FILE *fp=NULL;
		if (sfile.cBeginFlag == '1')
		{
			char filenamepath[301];
			sprintf(filenamepath,"%s",sfile.sFileName);
			int j= strlen(filenamepath)-1;
			for (;j>0;j--)
			{
				if ((filenamepath[j] == '/') ||  (filenamepath[j] == '\\'))
				{
					filenamepath[j] =0;
					CBF_Tools::MakeDir(filenamepath);
					break;
				}
			}
			fp = fopen(sfile.sFileName,"wb");
		}
		else
		{
			fp = fopen(sfile.sFileName,"ab");
		}
		if (fp == NULL)
		{
			sprintf(rcvdata.sMsgBuf->sBuffer,"文件[%s]打开出错",sfile.sFileName);
			rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_MONITOR_UPLOAD;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			m_pDrebApi->SendMsg(rcvdata);
			return ;
		}
		fwrite(rcvdata.sMsgBuf->sBuffer+sizeof(S_BPC_DUFILE),1,rcvdata.sMsgBuf->sDBHead.nLen-sizeof(S_BPC_DUFILE),fp);
		fclose(fp);
		if (sfile.cEndFlag == '1')
		{
			struct_stat fstat;
			FSTAT(sfile.sFileName,&fstat);
			if (atol(sfile.sFileSize) != fstat.st_size)
			{
				sprintf(rcvdata.sMsgBuf->sBuffer,"上传文件[%s]失败，大小[%d]不等于[%s]",sfile.sFileName,fstat.st_size,sfile.sFileSize);
				rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
				rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_MONITOR_UPLOAD;
				rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
				rcvdata.sMsgBuf->sDBHead.cZip = 0;
				m_pDrebApi->SendMsg(rcvdata);
				return ;
			}
			sprintf(rcvdata.sMsgBuf->sBuffer,"上传文件[%s]成功",sfile.sFileName);
			rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			m_pDrebApi->SendMsg(rcvdata);
			return ;
		}
		else
		{
			sprintf(rcvdata.sMsgBuf->sBuffer,"写入文件[%s]成功",sfile.sFileName);
			rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			m_pDrebApi->SendMsg(rcvdata);
			return ;
		}
	}

};

#endif // !defined(AFX_DREBMSGPROCBASE_H__EC38AEB7_E135_4E09_92E4_4D13D9D38982__INCLUDED_)
