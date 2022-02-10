// BPCLink.cpp: implementation of the CBPCLink class.
//
//////////////////////////////////////////////////////////////////////

#include "BPCLink.h"
#include "BF_Zip.h"
#include "BF_Des.h"
#include "dreberrcode.h"
#include "BF_Tools.h"

#define  DESENCRYPTKEY    "rd402wms"
#define  DESENCRYPTKEY3   "wms13711songfree"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



CBPCLink::CBPCLink()
{
	m_pConfXml = NULL;
	m_pLog = NULL;
	m_nConnNum = 0;
	
	memset(m_sRcvBuffer,0,sizeof(m_sRcvBuffer));
	m_nRcvBufLen = 0;
	
	memset(m_sSendBuffer,0,sizeof(m_sSendBuffer));
	m_nSendBufLen = 0;

	bzero(&m_pHeadData,sizeof(BPCCOMMSTRU));

	m_nConntime=0;              //连接建立时间
	m_nReadtime=0;              //上次读数据时间
	m_nSendtime=0;              //上次发送数据时间 
	
	m_nHeartTime =5;
	m_nDisconnectTime = 600;

	m_bIsConnect = false;
	m_nBpcHeadLen = BPCHEADLEN+DREBHEADLEN;
	m_bIsFree = false;
	m_pDate.Update();
	m_nUseIpc = 0;
	m_nBpcPort=0;

	CBF_Tools::MakeDir("data");

	m_bIsBpuCall = false;//是否BPU调用(流程控制调用)
	m_listCall=NULL;

	bzero(g_sCurPath,sizeof(g_sCurPath));
	bzero(g_sModuleName,sizeof(g_sModuleName));
	bzero(g_sModulePath,sizeof(g_sModulePath));
}

CBPCLink::~CBPCLink()
{
	m_tcpSocket.Close();
	m_pConfXml = NULL;
	m_pLog = NULL;

}
std::string CBPCLink::GetBpcMsgType(int msgtype)
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
	default:
		return "";
	}
}
std::string CBPCLink::GetDrebCmdType(int cmdtype)
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
	case  CMD_MONITOR_DREB:     //15 //数据总线节点监控
		return "CMD_MONITOR_DREB";
	case  CMD_MONITOR_BPC:     //15 //数据总线节点监控
		return "CMD_MONITOR_BPC";
	default:
		return "";
	}
}

int CBPCLink::Run()
{
	int ret=-1;
	while (!m_bIsConnect)
	{
		ret = ConnectBPC();
		if (ret != 0 )
		{
			m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"Run 连接BPC失败 重连次数[%d]",m_nConnNum);
			//连接失败次数超过5次，退出bpu
			if (m_nConnNum >5 )
			{
				m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"Run 连接次数[%d]超过5次，退出BPU",m_nConnNum);
				return 1;
			}
			//每次重连间隔2秒
			SLEEP_SECONDS(2);
		}
		break;
	}
	
	if (IsSocketEvent(1)) //1秒内是否有事件
	{
		//接收数据至m_sRcvBuffer
		ret = OnReceive();
		if (ret <=0)
		{
			//有读事件但未收到数据，肯定连接出问题了，关闭之
			OnClose(__FILE__,__LINE__,"有读事件但未收到数据，肯定连接出问题了，关闭之");
			return 0;
		}
		while (m_nRcvBufLen >= m_nBpcHeadLen)
		{
			//将请求数据放至m_pHeadData中
			ret = GetRcvData(m_pHeadData);
			if (ret > 0)
			{
				m_bIsFree = false;
				if (m_pHeadData.sBpcHead.cMsgType == MSG_REQ || m_pHeadData.sBpcHead.cMsgType == MSG_BPCCALL)
				{
					//m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"接收到的DREB命令 %s",GetDrebCmdType(m_pHeadData.sDBHead.cCmd).c_str());
					//处理一个数据
					//m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"交易码[%d]",m_pHeadData.sDBHead.d_Dinfo.d_nServiceNo);
					if (m_pHeadData.sDBHead.cRaflag == 1)
					{
						//应答数据，这里将丢弃
						m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"接收到的DREB命令[%s] 交易码[%d]",GetDrebCmdType(m_pHeadData.sDBHead.cCmd).c_str(),m_pHeadData.sDBHead.d_Dinfo.d_nServiceNo);
						continue;
					}
					m_pCallBu.Run(&m_pHeadData);//应答由callbu调用ansData
					//当空闲状态为忙时，说明业务没有响应最后一个包(这时可能多包应答会有些问题)
					OnFree();
				}
				else
				{
//					m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"接收到的BPC消息 %s",GetBpcMsgType(m_pHeadData.sBpcHead.cMsgType).c_str());
				}

			}
			else
			{
				//退出while，数据不全，下次再收
				break;	
			}
		}
	}
	else
	{
//		OnNoEvent();//无事件,心跳,删除临时文件
		OnNoReadEvent();//无读事件，是否断开
	}
	
	return 0;
}

std::string CBPCLink::GetBpcIp()
{
	string sip;
	if (m_pConfXml == NULL)
	{
		return "";
	}
	if (m_pConfXml->GetNodeValueByPath("package/head/PU配置/BPCIP",false,sip) == NULL)
	{
		return "";
	}
	return sip;
}

int CBPCLink::GetBpcPort()
{
	int nport;
	if (m_pConfXml == NULL)
	{
		return 0;
	}
	if (m_pConfXml->GetNodeValueByPath("package/head/PU配置/BPCPORT",false,nport) == NULL)
	{
		return 0;
	}
	return nport;
}

bool CBPCLink::IsSocketEvent(int sec)
{
	struct timeval tv;
	int result;
	if (m_sock == INVALID_SOCKET)
	{
		return false;
	}
	FD_ZERO(&m_rset);
	FD_ZERO(&m_wset);
#if defined(_WINDOWS)
	FD_ZERO(&m_eset);
#endif
	tv.tv_sec = sec;
	tv.tv_usec = 0;
	FD_SET(m_sock, &m_rset);//加入到读select集合
#if defined(_WINDOWS)
	result = select(m_sock+1, &m_rset, &m_wset, &m_eset, &tv);
#else
	result = select(m_sock+1, &m_rset, &m_wset, NULL, &tv);
#endif
	if (result == 0)
	{
		return false;
	}
	else
	{
		result = FD_ISSET(m_sock,&m_rset);
		if (result != 0) //有读事件
		{
			return true;
		}
		return false;
	}
}


int CBPCLink::ConnectBPC()
{
	m_tcpSocket.Close();
#ifdef UNIX
	if (m_nUseIpc == 1)
	{
		m_tcpSocket.Create(AF_UNIX,true);
	}
	else
	{
		m_tcpSocket.Create(AF_INET,true);
	}
#else
	m_tcpSocket.Create(AF_INET,true);
#endif	
	m_nConntime = time(NULL);
	if (!m_tcpSocket.ConnectServer((char *)m_sBpcIp.c_str(),m_nBpcPort))
//	if (!m_tcpSocket.ConnectServer((char *)GetBpcIp().c_str(),GetBpcPort()))
	{
		m_nConnNum++;
		m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"连接BPC %s %d 失败",(char *)m_sBpcIp.c_str(),m_nBpcPort);
		m_sock = INVALID_SOCKET;
		m_tcpSocket.Close();
		return -1;
	}
	//非阻塞模式
	m_tcpSocket.SetBlockFlag(false);
	m_sock = m_tcpSocket.GetSocket();

	//注册功能
	int ret;
	BPCCOMMSTRU data;
	bzero(&data,sizeof(BPCCOMMSTRU));
	data.sBpcHead.nBpcLen = DREBHEADLEN + m_sBpuGroupName.length();
	data.sBpcHead.cMsgType = MSG_BPUISREG;

	data.sDBHead.nLen = 0;
	data.sDBHead.cRaflag = 0;
	data.sDBHead.cNextFlag = 0;
	data.sDBHead.cZip = 0;

	strcpy(data.sBuffer,m_sBpuGroupName.c_str()); //组名称附在后面
	data.sDBHead.nLen = m_sBpuGroupName.length();
	data.sBpcHead.nRtime = GetPid();//进程ID放在nRtime里
	//发送是否可注册消息
	if (OnSend(&data) <0 )
	{
		m_nConnNum++;
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"发送MSG_BPUISREG失败"); 
		OnClose(__FILE__,__LINE__,"发送MSG_BPUISREG失败");
		return -1;
	}
	while (true)
	{
		//是否可读
		if (!IsSocketEvent(5)) 
		{
			m_nConnNum++;
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"未收到BPC是否可注册消息返回"); 
			OnClose(__FILE__,__LINE__,"未收到BPC是否可注册消息返回");
			return -1;
		}
		ret = OnReceive();
		if (ret <=0)
		{
			m_nConnNum++;
			OnClose(__FILE__,__LINE__,"远端关闭，接收数据不完整");
			return -1;
		}
		ret = GetRcvData(data);
		if (ret <1)
		{
			m_nConnNum++;
			if (m_nConnNum>5)
			{
				OnClose(__FILE__,__LINE__,"接收数据不完整");
				return -1;
			}
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"接收注册返回数据不完整");
			continue;
		}
		if (ret > 0) //收到完整数据了
		{
			data.sBuffer[data.sDBHead.nLen]=0;
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"收到BPC注册应答[%s] ",data.sBuffer);
			if (data.sDBHead.a_Ainfo.a_nRetCode != SUCCESS )
			{
				if (data.sDBHead.a_Ainfo.a_nRetCode == ERR_BPC_NOREGISTER)
				{
					m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"注册BpuGroup:%s 无需注册,BPC返回码:%d",m_sBpuGroupName.c_str(),data.sDBHead.a_Ainfo.a_nRetCode); 
					break;
				}
				m_nConnNum++;
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"注册BpuGroup:%s 失败,BPC返回码:%d",m_sBpuGroupName.c_str(),data.sDBHead.a_Ainfo.a_nRetCode); 
				OnClose(__FILE__,__LINE__,"注册BpuGroup失败");
				return -1;
			}
			//是否可注册返回0表示可注册，1为已注册，2为bpu组不符
			ret = atol(data.sBuffer);
			if (ret == 0)
			{
				m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"开始注册交易 ");
				//注册
				if (!RegisterBuGroup())
				{
					m_nConnNum++;
					m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"注册BpuGroup:%s 失败",m_sBpuGroupName.c_str()); 
					OnClose(__FILE__,__LINE__,"注册BpuGroup失败");
					return -1;
				}
			}
			else
			{
				if (ret == 2)
				{
					m_nConnNum++;
//					printf("BpuGroup:%s 在bpc配置中不存在\n",m_sBpuGroupName.c_str());
					m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"BpuGroup:%s 在bpc配置中不存在",m_sBpuGroupName.c_str()); 
					OnClose(__FILE__,__LINE__,"BpuGroup在BPC配置中不存在");
					return -1;
				}
				else if (ret == 3)
				{
					m_nConnNum++;
//					printf("BpuGroup:%s 连接已满\n",m_sBpuGroupName.c_str());
					m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"BpuGroup:%s 连接已满",m_sBpuGroupName.c_str()); 
					OnClose(__FILE__,__LINE__,"BpuGroup连接已满");
					return -1;
				}
				else
				{
					m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"BpuGroup:%s 已注册",m_sBpuGroupName.c_str());
				}
			}
			break;
		}
	}
	if (OnFree())
	{
		m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"连接BPC %s %d 成功",(char *)(char *)GetBpcIp().c_str(),GetBpcPort());
	}
	else
	{
		OnClose(__FILE__,__LINE__,"注册失败");
		m_nConnNum++;
		return -1;
	}
	m_bIsConnect = true;
	m_nConnNum = 0;
	return 0;
}

// 函数名: Init
// 编程  : 王明松 2010-7-6 17:18:12
// 返回  : bool 
// 参数  : const char *filepathname
// 描述  : 初始化BPc连接
bool CBPCLink::InitLink(const char *confile)
{
	m_pHeadCheck.SetCommEndian(true);//使用主机序

	m_sConfigXmlFile = confile;
	m_pCallBu.SetPara(this);

	if (!m_pCallBu.BuInit(m_sConfigXmlFile.c_str()))
	{
		printf("%s\n",m_pCallBu.GetMsg().c_str());
		return false;
	}
	
	m_pConfXml = m_pCallBu.GetXmlConfig();
	m_pLog = m_pCallBu.GetLogClass();
	
	//日志级别 日志目录 日志文件
	int id;
	char tmpchar[400];
	m_pLog->GetLogPara(m_nLogLevel,m_sLogPath,m_sLogFile,id);
	m_sLogPath = std::string(g_sCurPath);
	m_pLogFile.SetLogPara(m_nLogLevel,m_sLogPath.c_str(),"dreb.dat","dreberr.dat","bpu.dat","bpuerr.dat");
	
	if (m_pConfXml->GetNodeValueByPath("package/head/bpc/bpugroup",false,m_sBpuGroupName) == NULL)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"package/head/bpc/bpugroup未配置");
		return false;
	}
	if (m_sBpuGroupName.length()<1)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"package/head/bpc/bpugroup配置错误");
		return false;
	}
	
	if (m_pConfXml->GetNodeValueByPath("package/head/bpc/disconnecttime",false,m_nDisconnectTime) == NULL)
	{
		m_nDisconnectTime = 60000;
		m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"[disconnecttime]未配置，默认为60000秒");
	}
	if (m_pConfXml->GetNodeValueByPath("package/head/bpc/hearttime",false,m_nHeartTime) == NULL)
	{
		m_nHeartTime = 1;
		m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"[hearttime]未配置，默认为1秒");
	}
	
	if (m_pConfXml->GetNodeAttribute("package/head/bpc/compress",false,"datalen",false,m_nZipLen) == NULL)
	{
		m_nZipLen = 200;
		m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"默认加密压缩数据长度为200");
	}
	if (m_pConfXml->GetNodeAttribute("package/head/bpc/compress",false,"flag",false,m_nZipFlag) == NULL)
	{
		m_nZipFlag = 0;
		m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"默认加密压缩标志为0不处理");
	}
	if (m_nZipFlag <0 || m_nZipFlag >4 )
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"加密压缩标志设置不符%d",m_nZipFlag);
		return false;
	}
	
	if (m_pConfXml->GetNodeValueByPath("package/head/bpc/useipc",false,m_nUseIpc) == NULL)
	{
		m_nUseIpc = 0;
		m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"[useipc]未配置，默认为不使用");
	}

	if (m_pConfXml->GetNodeValueByPath("package/head/bpc/bpcip",false,m_sBpcIp,false) == NULL)
	{
		m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"[bpcip]未配置，默认为不使用");
		return false;
	}
	if (m_pConfXml->GetNodeValueByPath("package/head/bpc/bpcport",false,m_nBpcPort) == NULL)
	{
		m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"[bpcport]未配置，默认为不使用");
		return false;
	}

// 	int ret = ConnectBPC();
// 	if (ret !=0 )
// 	{
// 		printf("连接BPC %s %d 出错，可能连接不上或BPU注册已满，请查看日志\n",(char *)(char *)GetBpcIp().c_str(),GetBpcPort());
// 		return false;
// 	}
	return true;
}

/// 函数名: SetGPara1
/// 编程  : 王明松 2008-12-30 9:42:48
/// 返回  : virtual bool 
/// 参数  : bool setflag=true
/// 描述  : 接口虚函数，这里要实现.设置全局变量1,若已设置则返回false
bool CBPCLink::SetGPara1(bool setflag)
{
	int ret;
	while (!m_bIsConnect)
	{
		ret = ConnectBPC();
		if (ret != 0 )
		{
			m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"连接BPC失败 重连次数[%d]",m_nConnNum);
			//连接失败次数超过5次，退出bpu
			if (m_nConnNum >5 )
			{
				m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"连接超过[%d]次，退出BPU");
				return 1;
			}
			//每次重连间隔1秒
			SLEEP_SECONDS(1);
		}
		break;
	}
	BPCCOMMSTRU data;
	bzero(&data,sizeof(BPCCOMMSTRU));
	data.sBpcHead.cMsgType = MSG_SETGPARA1;
	data.sBpcHead.nBpcLen = DREBHEADLEN;
	data.sDBHead.nLen = 0;
	data.sDBHead.cRaflag = 0;
	data.sDBHead.cNextFlag = 0;
	data.sDBHead.cZip = 0;
	
	if (OnSend(&data)<0)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"发送MSG_SETGPARA1失败"); 
		OnClose(__FILE__,__LINE__,"发送MSG_SETGPARA1失败");
		return false;
	}
	while (true)
	{
		//是否可读
		if (!IsSocketEvent(5)) 
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"未收到MSG_SETGPARA1消息返回"); 
			OnClose(__FILE__,__LINE__,"未收到MSG_SETGPARA1消息返回");
			return false;
		}
		ret = OnReceive();
		if (ret <=0)
		{
			OnClose(__FILE__,__LINE__,"远端关闭，接收数据不完整");
			return false;
		}
		ret = GetRcvData(data);
		if (ret > 0) //收到完整数据了
		{
			data.sBuffer[data.sDBHead.nLen]=0;
			if (data.sDBHead.a_Ainfo.a_nRetCode != SUCCESS )
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"BpuGroup:%s  MSG_SETGPARA1 失败,BPC返回码:%d",m_sBpuGroupName.c_str(),data.sDBHead.a_Ainfo.a_nRetCode); 
				return false;
			}
			if (atoi(data.sBuffer) == 0)
			{
				return false;
			}
			else
			{
				return true;
			}
		}
	}
	return false;
}


/// 函数名: isSetGPara2
/// 编程  : 王明松 2008-12-30 9:36:10
/// 返回  : virtual bool 
/// 参数  : bool setflag=true
/// 描述  : 设置全局变量2,若已设置则返回false
bool CBPCLink::SetGPara2(bool setflag)
{
	int ret;
	while (!m_bIsConnect)
	{
		ret = ConnectBPC();
		if (ret != 0 )
		{
			m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"连接BPC失败 重连次数[%d]",m_nConnNum);
			//连接失败次数超过5次，退出bpu
			if (m_nConnNum >5 )
			{
				m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"连接超过[%d]次，退出BPU");
				return 1;
			}
			//每次重连间隔1秒
			SLEEP_SECONDS(1);
		}
		break;
	}
	BPCCOMMSTRU data;
	bzero(&data,sizeof(BPCCOMMSTRU));
	data.sBpcHead.cMsgType = MSG_SETGPARA2;
	data.sBpcHead.nBpcLen = DREBHEADLEN;
	data.sDBHead.nLen = 0;
	data.sDBHead.cRaflag = 0;
	data.sDBHead.cNextFlag = 0;
	data.sDBHead.cZip = 0;
	
	if (OnSend(&data)<0)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"发送MSG_SETGPARA2失败"); 
		OnClose(__FILE__,__LINE__,"发送MSG_SETGPARA2失败");
		return false;
	}
	while (true)
	{
		//是否可读
		if (!IsSocketEvent(5)) 
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"未收到MSG_SETGPARA2消息返回"); 
			OnClose(__FILE__,__LINE__,"未收到MSG_SETGPARA2消息返回");
			return false;
		}
		ret = OnReceive();
		if (ret <=0)
		{
			OnClose(__FILE__,__LINE__,"远端关闭，接收数据不完整");
			return false;
		}
		ret = GetRcvData(data);
		if (ret > 0) //收到完整数据了
		{
			data.sBuffer[data.sDBHead.nLen]=0;
			if (data.sDBHead.a_Ainfo.a_nRetCode != SUCCESS )
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"BpuGroup:%s  MSG_SETGPARA2 失败,BPC返回码:%d",m_sBpuGroupName.c_str(),data.sDBHead.a_Ainfo.a_nRetCode); 
				return false;
			}
			if (atoi(data.sBuffer) == 0)
			{
				return false;
			}
			else
			{
				return true;
			}
		}
	}
	return false;

}


/// 函数名: isSetGPara3
/// 编程  : 王明松 2008-12-30 9:36:13
/// 返回  : virtual bool 
/// 参数  : bool setflag=true
/// 描述  : 设置全局变量3,若已设置则返回false
bool CBPCLink::SetGPara3(bool setflag)
{
	int ret;
	while (!m_bIsConnect)
	{
		ret = ConnectBPC();
		if (ret != 0 )
		{
			m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"连接BPC失败 重连次数[%d]",m_nConnNum);
			//连接失败次数超过5次，退出bpu
			if (m_nConnNum >5 )
			{
				m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"连接超过[%d]次，退出BPU");
				return 1;
			}
			//每次重连间隔1秒
			SLEEP(1000);
		}
		break;
	}
	BPCCOMMSTRU data;
	bzero(&data,sizeof(BPCCOMMSTRU));
	data.sBpcHead.cMsgType = MSG_SETGPARA3;
	data.sBpcHead.nBpcLen = DREBHEADLEN;
	data.sDBHead.nLen = 0;
	data.sDBHead.cRaflag = 0;
	data.sDBHead.cNextFlag = 0;
	data.sDBHead.cZip = 0;
	
	if (OnSend(&data)<0)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"发送MSG_SETGPARA3失败"); 
		OnClose(__FILE__,__LINE__,"发送MSG_SETGPARA3失败");
		return false;
	}
	while (true)
	{
		//是否可读
		if (!IsSocketEvent(5)) 
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"未收到MSG_SETGPARA3消息返回"); 
			OnClose(__FILE__,__LINE__,"未收到MSG_SETGPARA3消息返回");
			return false;
		}
		ret = OnReceive();
		if (ret <=0)
		{
			OnClose(__FILE__,__LINE__,"远端关闭，接收数据不完整");
			return false;
		}
		ret = GetRcvData(data);
		if (ret > 0) //收到完整数据了
		{
			data.sBuffer[data.sDBHead.nLen]=0;
			if (data.sDBHead.a_Ainfo.a_nRetCode != SUCCESS )
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"BpuGroup:%s MSG_SETGPARA3 失败,BPC返回码:%d",m_sBpuGroupName.c_str(),data.sDBHead.a_Ainfo.a_nRetCode); 
				return false;
			}
			if (atoi(data.sBuffer) == 0)
			{
				return false;
			}
			else
			{
				return true;
			}
		}
	}
	return false;

}

/// 函数名: GetThreadShareSerial
/// 编程  : 王明松 2008-12-30 9:36:13
/// 返回  : virtual long 
/// 描述  : 取得唯一的流水号
long CBPCLink::GetThreadShareSerial()
{
	int ret;
	while (!m_bIsConnect)
	{
		ret = ConnectBPC();
		if (ret != 0 )
		{
			m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"连接BPC失败 重连次数[%d]",m_nConnNum);
			//连接失败次数超过5次，退出bpu
			if (m_nConnNum >5 )
			{
				m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"连接超过[%d]次，退出BPU");
				return 1;
			}
			//每次重连间隔1秒
			SLEEP(1000);
		}
		break;
	}
	BPCCOMMSTRU data;
	bzero(&data,sizeof(BPCCOMMSTRU));
	data.sBpcHead.cMsgType = MSG_GETSERIAL;
	data.sBpcHead.nBpcLen = DREBHEADLEN;
	data.sDBHead.nLen = 0;
	data.sDBHead.cRaflag = 0;
	data.sDBHead.cNextFlag = 0;
	data.sDBHead.cZip = 0;

	if (OnSend(&data)<0)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"发送MSG_GETSERIAL失败"); 
		OnClose(__FILE__,__LINE__,"发送MSG_GETSERIAL失败");
		return -1;
	}
	while (true)
	{
		//是否可读
		if (!IsSocketEvent(5)) 
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"未收到MSG_GETSERIAL消息返回"); 
			OnClose(__FILE__,__LINE__,"未收到MSG_GETSERIAL消息返回");
			return -1;
		}
		ret = OnReceive();
		if (ret <=0)
		{
			OnClose(__FILE__,__LINE__,"远端关闭，接收数据不完整");
			return ret;
		}
		ret = GetRcvData(data);
		if (ret > 0) //收到完整数据了
		{
			data.sBuffer[data.sDBHead.nLen]=0;
			if (data.sDBHead.a_Ainfo.a_nRetCode != SUCCESS )
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"注册BpuGroup:%s 失败,BPC返回码:%d",m_sBpuGroupName.c_str(),data.sDBHead.a_Ainfo.a_nRetCode); 
				return -1;
			}
			return atol(data.sBuffer);
		}
	}
	return -1;
}


// 函数名: AnsData
// 编程  : 王明松 2013-4-23 15:16:43
// 返回  : virtual int 
// 参数  : PBPCCOMMSTRU data
// 描述  : 应答数据,由BPU业务里面的原子调用
int CBPCLink::AnsData(PBPCCOMMSTRU data) 
{
	if (m_bIsBpuCall)
	{
		m_listCall->push_back(*data);
	}
	else
	{
		int ret;
		while (!m_bIsConnect)
		{
			ret = ConnectBPC();
			if (ret != 0 )
			{
				m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"应答数据  连接BPC失败 重连次数[%d]",m_nConnNum);
				//连接失败次数超过5次，退出bpu
				if (m_nConnNum >5 )
				{
					m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"应答数据 连接[%d]次，退出BPU",m_nConnNum);
					return 1;
				}
				//每次重连间隔1秒
				SLEEP(1000);
			}
			break;
		}
		if (OnSend(data)<0)
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"发送应答出错");
			return -1;
		}
	
	}
	
	return 0;
}

// 函数名: DPTrans
// 编程  : 王明松 2013-4-23 15:11:48
// 返回  : virtual int 
// 参数  : PBPCCOMMSTRU calldata
// 描述  : 转移至其它服务处理
int CBPCLink::DPTrans(PBPCCOMMSTRU calldata) 
{
	int ret;
	while (!m_bIsConnect)
	{
		ret = ConnectBPC();
		if (ret != 0 )
		{
			m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"DPTrans 连接BPC失败 重连次数[%d]",m_nConnNum);
			//连接失败次数超过5次，退出bpu
			if (m_nConnNum >5 )
			{
				m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"DPTrans 连接超过[%d]次，退出BPU",m_nConnNum);
				return 1;
			}
			//每次重连间隔1秒
			SLEEP(1000);
		}
		break;
	}
	calldata->sBpcHead.cMsgType = MSG_TRANS;
	calldata->sDBHead.cRaflag = 0;
	//转移至别的服务进行处理
	if (OnSend(calldata)<0)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"发送DPTrans失败"); 
		OnClose(__FILE__,__LINE__,"发送DPTrans失败");
		return -1;
	}
	//我是空闲的，可以处理另外的业务了
	OnFree();
	return 0;
}

// 函数名: BpuCall
// 编程  : 王明松 2013-4-23 15:09:13
// 返回  : virtual int -1失败 0成功
// 参数  : PBPCCOMMSTRU calldata
// 参数  : LIST_BPC_RSMSG &ansdata
// 描述  : 调用BPU内的功能，给流程控制使用
int CBPCLink::BpuCall(PBPCCOMMSTRU calldata,LIST_BPC_RSMSG &ansdata)
{
	m_bIsBpuCall = true;
	m_listCall = &ansdata;

	calldata->sDBHead.cRaflag = 0;
	//处理一个数据
	m_pCallBu.Run(calldata);

	m_bIsBpuCall = false;
	m_listCall = NULL;
	if (ansdata.size()<1)
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"BpuCall调用无返回数据");
		return -1;
	}
	return 0;
}

// 函数名: BpcCall
// 编程  : 王明松 2013-4-23 15:08:18
// 返回  : virtual int 
// 参数  : PBPCCOMMSTRU calldata
// 参数  : LIST_BPC_RSMSG &ansdata
// 描述  : 调用bpc功能
int CBPCLink::BpcCall(PBPCCOMMSTRU calldata,LIST_BPC_RSMSG &ansdata,unsigned int stimeout)
{
	int ret;
	while (!m_bIsConnect)
	{
		ret = ConnectBPC();
		if (ret != 0 )
		{
			m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"BpcCall 连接BPC失败 重连次数[%d]",m_nConnNum);
			//连接失败次数超过5次，退出bpu
			if (m_nConnNum >5 )
			{
				m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"BpcCall 连接超过[%d]次，退出BPU",m_nConnNum);
				return 1;
			}
			//每次重连间隔1秒
			SLEEP(1000);
		}
		break;
	}
	calldata->sBpcHead.cMsgType = MSG_BPCCALL;
	calldata->sDBHead.cRaflag = 0;
	if (OnSend(calldata)<0)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"发送BpcCall失败"); 
		OnClose(__FILE__,__LINE__,"发送BpcCall失败");
		return -1;
	}
	ret = GetAns(calldata->sDBHead.cCmd,ansdata,stimeout);
	if (ret <0)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"取BpcCall应答出错");
		return -1;
	}
	return 0;
}

// 函数名: ExtCall
// 编程  : 王明松 2013-4-23 15:01:25
// 返回  : virtual int 
// 参数  : PBPCCOMMSTRU calldata
// 参数  : vector<S_BPC_RSMSG> &ansdata
// 参数  : unsigned int stimeout 外调超时时间 秒
// 描述  : 外调BPC
int CBPCLink::ExtCall(PBPCCOMMSTRU calldata,LIST_BPC_RSMSG &ansdata,unsigned int stimeout)
{
	int ret;
	while (!m_bIsConnect)
	{
		ret = ConnectBPC();
		if (ret != 0 )
		{
			m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"ExtCall 连接BPC失败 重连次数[%d]",m_nConnNum);
			//连接失败次数超过5次，退出bpu
			if (m_nConnNum >5 )
			{
				m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"ExtCall 连接超过[%d]次，退出BPU",m_nConnNum);
				return 1;
			}
			//每次重连间隔1秒
			SLEEP(1000);
		}
		break;
	}
	calldata->sBpcHead.cMsgType = MSG_EXTCALL;
	calldata->sDBHead.cRaflag = 0;
	if (OnSend(calldata)<0)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"发送ExtCall失败"); 
		OnClose(__FILE__,__LINE__,"发送ExtCall失败");
		return -1;
	}
	ret = GetAns(calldata->sDBHead.cCmd,ansdata,stimeout);
	if (ret <0)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"取ExtCall应答出错");
		return -1;
	}
	return 0;
}



// 函数名: GetRcvData
// 编程  : 王明松 2013-4-23 14:57:54
// 返回  : virtual int 1有请求，其它无
// 参数  : BPCCOMMSTRU &data
// 描述  : 异步接收后取请求数据
int CBPCLink::GetRcvData(BPCCOMMSTRU &data)
{
	if (m_nRcvBufLen >= m_nBpcHeadLen)
	{
		//将bpchead及drebhead复制过去
		memcpy(&data,m_sRcvBuffer,m_nBpcHeadLen);
		//字节序处理,转成本机字节序
		m_pHeadCheck.Endian2LocalHost(&data);
		if (data.sDBHead.nLen>0)
		{
			if (m_nRcvBufLen < m_nBpcHeadLen+data.sDBHead.nLen)
			{
				//未收全
				return -1;
			}
			memcpy(data.sBuffer,m_sRcvBuffer+m_nBpcHeadLen,data.sDBHead.nLen);
		}
		//收多了，将原包重新给m_sRcvBuffer
		for (int i=0; i<m_nRcvBufLen-m_nBpcHeadLen-data.sDBHead.nLen; i++)
		{
			m_sRcvBuffer[i]=m_sRcvBuffer[i+m_nBpcHeadLen+data.sDBHead.nLen];
		}
		m_nRcvBufLen =  m_nRcvBufLen - m_nBpcHeadLen - data.sDBHead.nLen;

		if (data.sBpcHead.cMsgType != MSG_FREE && data.sBpcHead.cMsgType != MSG_BPUISREG && data.sBpcHead.cMsgType != MSG_BPUREG) //这类消息可能比较多，就不用写报文流
		{
			LogFile(LOG_DEBUG+1,false,&data);
		}
		//解包
		if (data.sDBHead.cZip >0 && data.sDBHead.cZip < 4)
		{
			if (!UnzipBuf(data))
			{
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"解压缩数据出错");
				return -2;
			}
		}
		if (data.sBpcHead.cMsgType != MSG_FREE && data.sBpcHead.cMsgType != MSG_BPUISREG && data.sBpcHead.cMsgType != MSG_BPUREG)
		{
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"接收到的BPC的消息[%s]  DREB命令[%s] RA标志[%d] 交易码[%d]  标识[%d %d %d] cZip[%d] 业务数据长度[%d]",\
					GetBpcMsgType(data.sBpcHead.cMsgType).c_str(),GetDrebCmdType(data.sDBHead.cCmd).c_str(),\
					data.sDBHead.cRaflag,data.sDBHead.d_Dinfo.d_nServiceNo,data.sDBHead.s_Sinfo.s_nNodeId,\
					data.sDBHead.s_Sinfo.s_cNodePrivateId,data.sDBHead.s_Sinfo.s_nDrebSerial,data.sDBHead.cZip,data.sDBHead.nLen);
		}
		else
		{
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"接收到的BPC的消息[%s]  RA标志[%d] 业务数据长度[%d]",\
				GetBpcMsgType(data.sBpcHead.cMsgType).c_str(),data.sDBHead.cRaflag,data.sDBHead.nLen);
		}
		return 1;

	}
	return 0;
}
	
// 函数名: Receive
// 编程  : 王明松 2013-4-23 14:47:49
// 返回  : virtual int 
// 描述  : 接收数据
int CBPCLink::OnReceive()
{
	int ret;
	m_tcpSocket.AttachSocket(m_sock,"");
	if (m_nRcvBufLen >=0 && m_nRcvBufLen< BPCBUFSIZE)
	{
		//包头未接收完
		ret = m_tcpSocket.Receive(m_sRcvBuffer+m_nRcvBufLen,BPCBUFSIZE-m_nRcvBufLen);
		if (ret <= 0)
		{
			//OnClose("远端关闭，接收数据不完整");
			return ret;
		}
		m_nRcvBufLen+=ret;
		m_nReadtime =time(NULL);
		
	}
	return m_nRcvBufLen;
}
	
	
// 函数名: OnSend
// 编程  : 王明松 2013-4-23 14:57:08
// 返回  : virtual int <0连接出错了   =0对方缓冲区满   >0发送成功
// 参数  : PBPCCOMMSTRU data
// 描述  : 发送数据给BPC或FPC
int CBPCLink::OnSend(PBPCCOMMSTRU data)
{
	int ret;
	if (data->sBpcHead.cMsgType != MSG_FREE && data->sBpcHead.cMsgType != MSG_BPUREG && data->sBpcHead.cMsgType!=MSG_BPUISREG)
	{
		if (!ZipBuf(data))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"加密压缩处理失败");
			return -1;
		}
	}
	if (data->sBpcHead.cMsgType != MSG_FREE && data->sBpcHead.cMsgType != MSG_BPUISREG && data->sBpcHead.cMsgType != MSG_BPUREG)
	{
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"发送给BPC的消息[%s]  DREB命令[%s] RA标志[%d] 交易码[%d]  标识[%d %d %d] cZip[%d] 业务数据长度[%d]",\
			GetBpcMsgType(data->sBpcHead.cMsgType).c_str(),GetDrebCmdType(data->sDBHead.cCmd).c_str(),\
			data->sDBHead.cRaflag,data->sDBHead.d_Dinfo.d_nServiceNo,data->sDBHead.s_Sinfo.s_nNodeId,\
			data->sDBHead.s_Sinfo.s_cNodePrivateId,data->sDBHead.s_Sinfo.s_nDrebSerial,\
			data->sDBHead.cZip,data->sDBHead.nLen);
	}
	else
	{
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"发送给BPC的消息[%s] 业务数据长度[%d]",\
			GetBpcMsgType(data->sBpcHead.cMsgType).c_str(),data->sDBHead.nLen);
	}
	data->sBpcHead.nBpcLen = DREBHEADLEN +data->sDBHead.nLen;
	memcpy(m_sSendBuffer,data,m_nBpcHeadLen+data->sDBHead.nLen);
	m_nSendBufLen = m_nBpcHeadLen+data->sDBHead.nLen;
	PBPCCOMMSTRU dd = (PBPCCOMMSTRU)m_sSendBuffer;
	m_pHeadCheck.Endian2Comm(dd);
	ret = m_tcpSocket.Send(m_sSendBuffer,m_nSendBufLen);
	if (ret != m_nSendBufLen)
	{
		LogFile(LOG_ERROR,true,data);
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"发送数据失败 ret[%d]!=sendlen[%d] lasterr[%d %s]",\
			ret,m_nSendBufLen,m_tcpSocket.GetErrorNo(),m_tcpSocket.GetErrorMsg().c_str());
		
		//发送失败
		if (ret < 0)
		{
			OnClose(__FILE__,__LINE__,"发送数据失败 返回为连接异常");
		}
		else
		{

		}
		return -1;
	}
	m_nSendtime = time(NULL);
	if (data->sBpcHead.cMsgType != MSG_FREE && data->sBpcHead.cMsgType != MSG_BPUREG && data->sBpcHead.cMsgType != MSG_BPUISREG) //这类消息可能比较多，就不用写报文流
	{
		LogFile(LOG_DEBUG+1,false,data);
	}
	return ret;
}

void CBPCLink::OnNoEvent()
{
	if (time(NULL) - m_nSendtime >= m_nHeartTime)
	{
		//5秒没有发送则发送MSG_FREE，类似心跳
		OnFree();
	}
}

void CBPCLink::OnNoReadEvent()
{
	int tmptime;
	if (m_sock != INVALID_SOCKET)
	{
		tmptime = time(NULL) - m_nReadtime;
		if (tmptime > m_nDisconnectTime)//超时时间没有收到数据，断开
		{
			OnClose(__FILE__,__LINE__,"连接长时间未用，断开");
		}
		else if (tmptime > m_nHeartTime)
		{
			//直接置空闲状态为空闲
			OnFree();
		}
	}
}
void CBPCLink::OnClose(const char *file,int fileline,const char *msg)
{
	m_tcpSocket.Close();
	m_sock = INVALID_SOCKET;
	m_pLog->LogMp(LOG_WARNNING,file,fileline,"%s",msg);
	m_bIsConnect = false;
}

bool CBPCLink::RegisterBuGroup()
{
	int ret;
	vector<S_TBL_FLOWTX> txinfo;
	int nAllTxNum = m_pCallBu.GetAllTxInfo(txinfo);
	if (nAllTxNum<1)
	{
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"无交易注册");
		return true;
	}
	//每个数据包最大交易数
	int nPackTxNum = BPUDATASIZE/sizeof(S_BPU_FUNC_REG);
	BPCCOMMSTRU data;
	//有几个数据包
	int nRegPackNum = (nAllTxNum/nPackTxNum)+1;
	int txnum=0;
	int datalen=0;
	S_BPU_FUNC_REG *bpugroup=NULL;
	S_BPU_FUNCINFO *fn=NULL;
	for (int i=0 ; i<nRegPackNum; i++ )
	{
		memset(&data,0,sizeof(BPCCOMMSTRU));
		data.sBpcHead.cMsgType = MSG_BPUREG;//注册
		data.sDBHead.cRaflag = 0;
		data.sDBHead.cNextFlag = 0;
		data.sDBHead.cZip = 0;
		bpugroup=(S_BPU_FUNC_REG *)data.sBuffer;
		strcpy(bpugroup->sBpuGroupName,m_sBpuGroupName.c_str());
		fn=(S_BPU_FUNCINFO *)(data.sBuffer+25);
		txnum=0;
		datalen+=25;
		for (int j= i*nPackTxNum ; j<(i+1)*nPackTxNum && j <nAllTxNum ; j++)
		{
			fn->nFuncNo = txinfo[j].nFuncNo;
			m_pHeadCheck.Endian2Comm((unsigned char *)&(fn->nFuncNo),sizeof(fn->nFuncNo));
			fn->cPrio = txinfo[j].cPrio;
			fn->cServiceFlag = txinfo[j].cServiceFlag;
			CBF_Tools::StringCopy(fn->sFuncName,40,txinfo[j].sFuncName.c_str());
			datalen+=sizeof(S_BPU_FUNCINFO);
			txnum++;
			fn++;
		}
		bpugroup->nFuncNum = txnum;
		m_pHeadCheck.Endian2Comm((unsigned char *)&(bpugroup->nFuncNum),sizeof(bpugroup->nFuncNum));
		data.sDBHead.nLen = datalen;
		data.sBpcHead.nBpcLen = DREBHEADLEN+datalen;
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"开始注册功能");
		if (OnSend(&data)<0)
		{
			OnClose(__FILE__,__LINE__,"发送注册信息失败");
			return false;
		}
	}
	return true;
}

bool CBPCLink::OnFree()
{
	BPCCOMMSTRU data;
	bzero(&data,sizeof(BPCCOMMSTRU));
	data.sBpcHead.nBpcLen = DREBHEADLEN;
	data.sBpcHead.cMsgType = MSG_FREE;
	data.sDBHead.cRaflag = 0;
	data.sDBHead.nLen = 0;
	data.sDBHead.cNextFlag = 0;
	
	if (OnSend(&data)<0)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"发送MSG_FREE失败"); 
		m_bIsFree = false;
		return false;
	}
	m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"发送MSG_FREE成功"); 
	m_bIsFree = true;
	m_bIsBpuCall = false;//是否BPU调用(流程控制调用)
	m_listCall=NULL;
	return m_bIsFree;
}

int CBPCLink::LogFile(int level, bool flag, PBPCCOMMSTRU data)
{
	m_pLogFile.LogBpc(level,data,flag);
	return 0;
}

int CBPCLink::GetAns(char msgtype, LIST_BPC_RSMSG &ansdata, unsigned int stimeout)
{
	int ret;
	BPCCOMMSTRU data;
	bzero(&data,sizeof(BPCCOMMSTRU));
// #define  CMD_DPCALL      8  //数据总线节点同步调用 要求最终处理完成后应答
// #define  CMD_DPACALL     9  //数据总线节点异步调用 要求接收到的数据总线节点发送至服务后确认应答
// #define  CMD_DPBC        10  //数据总线节点广播，即将信息发给指定数据总线节点上所有注册的服务,要求数据总线节点应答
// #define  CMD_DPABC       11  //数据总线节点广播，即将信息发给指定数据总线节点上所有注册的服务,不要求数据总线节点应答
// #define  CMD_DPPUSH      12 //数据总线节点推送，无须应答
// #define  CMD_DPPOST      13 //数据总线节点投递，要求接收到的数据总线节点应答
	switch (msgtype)
	{
		case CMD_DPCALL:
		case CMD_DPACALL:
			//接收应答
			break;
		case CMD_DPPOST:
		case CMD_DPBC:
			//接收确认
			break;
		case CMD_DPABC:
		case CMD_DPPUSH:
			return 0;
		default:
			return 0;
	}
	while (true)
	{
		//是否可读
		if (!IsSocketEvent(stimeout)) 
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"超时未收到消息返回"); 
			OnClose(__FILE__,__LINE__,"超时未收到消息返回");
			return -1;
		}
		ret = OnReceive();
		if (ret <=0)
		{
			OnClose(__FILE__,__LINE__,"接收应答远端关闭，接收数据不完整");
			return -1;
		}
		ret = GetRcvData(data);
		if (ret > 0) //收到完整数据了
		{
			if (data.sDBHead.cDrebAffirm == 1)
			{
				m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"已收到对端的DREB确认");
				if (data.sDBHead.a_Ainfo.a_nRetCode != SUCCESS )
				{
					m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"对端dreb确认返回码[%d]",data.sDBHead.a_Ainfo.a_nRetCode); 
					return 1;
				}
				if (msgtype ==  CMD_DPPOST || msgtype == CMD_DPBC)
				{
					return 0;
				}
				//dpacall
				continue;//继续接收应答
			}
			else
			{
				if (data.sDBHead.cRaflag == 0)
				{
					m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"出错了，收应答怎么会有请求出来%d %d %d",\
						data.sDBHead.s_Sinfo.s_nNodeId,data.sDBHead.s_Sinfo.s_cNodePrivateId,data.sDBHead.s_Sinfo.s_nDrebSerial); 
					continue;
				}
				ansdata.push_back(data);
				if (data.sDBHead.cNextFlag == 0 && data.sDBHead.n_Ninfo.n_nNextNo == 0)
				{
					//只有一个应答
					return 0;
				}
				else if (data.sDBHead.n_Ninfo.n_nNextNo == ansdata.size())
				{
					//总包数相等才返回
					return 0;
				}
			}
			
		}
	}
	return -1;
}

bool CBPCLink::UnzipBuf(BPCCOMMSTRU &data)
{
	//0明文 1des加密密钥长度16 2压缩 3DES加密 4DES加密并压缩

	if (data.sDBHead.cZip >0 && data.sDBHead.cZip <=4)
	{
		char zipDataBuf[BPUDATASIZE];
		bzero(zipDataBuf,sizeof(zipDataBuf));
		unsigned int dlen = BPUDATASIZE;
		int ddlen;
		switch (data.sDBHead.cZip)
		{
			case 1:
				ddlen = data.sDBHead.nLen;
				if (m_pDes.UncryptString(data.sBuffer,ddlen,DESENCRYPTKEY)<=0)
				{
					m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"解密出错");
					return false;
				}
				data.sDBHead.nLen = ddlen;
				data.sDBHead.cZip =0;
				break;
			case 2:	
				memcpy(zipDataBuf,data.sBuffer,data.sDBHead.nLen);
				if (!CBF_Tools::Uncompress((unsigned char *)(data.sBuffer),dlen,(unsigned char *)zipDataBuf,data.sDBHead.nLen))
				{
					m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"解压缩出错");
					return false;
				}
				data.sDBHead.nLen = dlen;
				data.sDBHead.cZip =0;
				break;
			case 3:
				ddlen = data.sDBHead.nLen;
				if (m_pDes.UncryptString(data.sBuffer,ddlen,DESENCRYPTKEY3)<=0)
				{
					m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"解密出错");
					return false;
				}
				data.sDBHead.nLen = ddlen;
				data.sDBHead.cZip =0;
				break;
			case 4:
				ddlen = data.sDBHead.nLen;
				if (m_pDes.UncryptString(data.sBuffer,ddlen,DESENCRYPTKEY)<=0)
				{
					m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"解密出错");
					return false;
				}
				data.sDBHead.nLen = ddlen;
				memcpy(zipDataBuf,data.sBuffer,data.sDBHead.nLen);
				if (!CBF_Tools::Uncompress((unsigned char *)(data.sBuffer),dlen,(unsigned char *)zipDataBuf,data.sDBHead.nLen))
				{
					m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"解压缩出错");
					return false;
				}
				data.sDBHead.nLen = dlen;
				data.sDBHead.cZip =0;
				break;
			default:
				return false;
		}
	}
	
	return true;
}
bool CBPCLink::ZipBuf(PBPCCOMMSTRU data)
{
	//0明文 1des加密密钥长度16 2压缩 3DES加密 4DES加密并压缩
	if (data->sDBHead.cZip != 0 || data->sDBHead.nLen<m_nZipLen)
	{
		return true;
	}
	char zipDataBuf[BPUDATASIZE];
	bzero(zipDataBuf,sizeof(zipDataBuf));
	unsigned int dlen = BPUDATASIZE;
	int ddlen;
	switch (m_nZipFlag)
	{
		case 1:
			ddlen = data->sDBHead.nLen;
			if (m_pDes.EncryptString(data->sBuffer,ddlen,DESENCRYPTKEY)<=0)
			{
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"加密出错");
				return false;
			}
			data->sDBHead.nLen = ddlen;
			data->sDBHead.cZip =1;
			break;
		case 2:	
			memcpy(zipDataBuf,data->sBuffer,data->sDBHead.nLen);
			if (!CBF_Tools::Compress((unsigned char *)(data->sBuffer),dlen,(unsigned char *)(zipDataBuf),data->sDBHead.nLen))
			{
				m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"数据压缩失败");
				memcpy(data->sBuffer,zipDataBuf,data->sDBHead.nLen);
				data->sDBHead.cZip =0;
				break;
			}
			else
			{
				data->sDBHead.nLen = dlen;
				data->sDBHead.cZip =2;
			}
			break;
		case 3:
			ddlen = data->sDBHead.nLen;
			if (m_pDes.EncryptString(data->sBuffer,ddlen,DESENCRYPTKEY3)<=0)
			{
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"加密出错");
				return false;
			}
			data->sDBHead.nLen = ddlen;
			data->sDBHead.cZip =3;
			break;
		case 4:
			memcpy(zipDataBuf,data->sBuffer,data->sDBHead.nLen);
			if (!CBF_Tools::Compress((unsigned char *)data->sBuffer,dlen,(unsigned char *)(zipDataBuf),data->sDBHead.nLen))
			{
				m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"数据压缩失败");
				memcpy(data->sBuffer,zipDataBuf,data->sDBHead.nLen);
				data->sDBHead.cZip =0;
				break;
			}
			data->sDBHead.nLen = dlen;
			ddlen = dlen;
			if (m_pDes.EncryptString(data->sBuffer,ddlen,DESENCRYPTKEY)<=0)
			{
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"加密出错");
				return false;
			}
			data->sDBHead.nLen = ddlen;
			data->sDBHead.cZip =4;
			break;
		default:
			data->sDBHead.cZip =0;
			return true;
	}
	return true;
}

void CBPCLink::Stop()
{
	OnClose(__FILE__,__LINE__,"停止BPU");
}
