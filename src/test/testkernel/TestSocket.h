// TestSocket.h: interface for the CTestSocket class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TESTSOCKET_H__CE2A91BA_2042_4876_A7B6_F4FA6512FC06__INCLUDED_)
#define AFX_TESTSOCKET_H__CE2A91BA_2042_4876_A7B6_F4FA6512FC06__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "BF_SocketTcp.h"
#include "BF_SocketUdp.h"
#include "BF_Timer.h"
#include "BF_SimpleLog.h"


class CTestSocket  
{
public:
#define  MAX_SOCKET_CONNECT   640   
	typedef struct 
	{
		unsigned int  index;
		char          address[32];//客户端IP
		SOCKET_HANDLE socket;
	}SOCKET_POOL_DATA;

	SOCKET_POOL_DATA m_pPoolSocket[MAX_SOCKET_CONNECT] ;

	CBF_SimpleLog m_log;
	CTestSocket()
	{
		m_bIsExit = false;
		//连接池置初值
		for (int i=0; i<MAX_SOCKET_CONNECT ;i++)
		{
			m_pPoolSocket[i].index = i;
			m_pPoolSocket[i].socket = INVALID_SOCKET;
			bzero(m_pPoolSocket[i].address,sizeof(m_pPoolSocket[i].address));
		}
		bzero(m_sClientIp,sizeof(m_sClientIp));
		m_nTcpRcvLen = 0;
		m_nTcpRcvPackNum = 0;
		m_nTcpSendLen = 0;
		m_nTcpSendPackNum = 0;
		m_pTimer.Init(10,true);
		m_log.SetLogPara(LOG_DEBUG,"","testsocket.log",GetPid());

		m_nUdpRcvLen = 0;
		m_nUdpRcvPackNum = 0;
		m_nUdpSendLen = 0;
		m_nUdpSendPackNum = 0;
	}
	virtual ~CTestSocket()
	{
		m_pTimer.Stop();
	}
	void TcpSelect()
	{
		struct timeval tv;
		int result;
		int i,j;
		SOCKET_HANDLE maxfd = INVALID_SOCKET;
		int ret;
		int curtime;
		while (!m_bIsExit)
		{
			FD_ZERO(&m_rset);
			FD_ZERO(&m_wset);
#if defined(_WINDOWS)
			FD_ZERO(&m_eset);
#endif
			tv.tv_sec = 1;
			tv.tv_usec = 0;
			//取得最大的socket并将socket加入到fd_set集合
			for (int i=0;i<MAX_SOCKET_CONNECT;i++)
			{
				if (m_pPoolSocket[i].socket != INVALID_SOCKET)
				{
					//需要读数据
					FD_SET(m_pPoolSocket[i].socket, &m_rset);//加入到读select集合
					//需要发送数据或主动连接时
					FD_SET(m_pPoolSocket[i].socket, &m_wset);//加入到写select集合
#if defined(_WINDOWS)
					FD_SET(m_pPoolSocket[i].socket, &m_eset);//加入到连接异常集合
#endif
					if (m_pPoolSocket[i].socket > maxfd)
					{
						maxfd = m_pPoolSocket[i].socket;
					}
				}
			}
#if defined(_WINDOWS)
			result = select(maxfd+1, &m_rset, &m_wset, &m_eset, &tv);
#else
			result = select(maxfd+1, &m_rset, &m_wset, NULL, &tv);
#endif
			if (0 ==  result ) //无事件发生,处理心跳
			{
				continue;
			}
			else //有事件发生
			{
				OnTcpSelectEvent();
			}
			
		}
	}
	void OnTcpSelectEvent()
	{
		int ret;
		bool isFindFree=false;
		//如果是侦听端口有读的事件，则去调用accept
		ret = FD_ISSET(m_pPoolSocket[0].socket,&m_rset);
		if (ret != 0) 
		{
			m_pTcp.AttachSocket(m_pPoolSocket[0].socket,"");
			SOCKET_HANDLE tmpsocket = m_pTcp.Accept(m_sClientIp,false);
			if (tmpsocket != INVALID_SOCKET)
			{
				//在池里找一个空的，放进去，实际应用可优化
				for (int i=1 ; i<MAX_SOCKET_CONNECT ; i++)
				{
					if (m_pPoolSocket[i].socket == INVALID_SOCKET)
					{
						isFindFree=true;
						m_pPoolSocket[i].socket = tmpsocket;
						strcpy(m_pPoolSocket[i].address,m_sClientIp);
						printf("接收客户端[%s-%d]连接\n",m_sClientIp,i);
						break;
					}
				}
				if (!isFindFree) //没有空闲的连接
				{
					printf("连接池满，无空闲连接来接收客户端[%s]连接\n",m_sClientIp);
					m_pTcp.AttachSocket(tmpsocket,m_sClientIp);
					m_pTcp.Close();
				}
			}
		}
		for (int i=1 ; i<MAX_SOCKET_CONNECT ; i++)
		{
			if (m_pPoolSocket[i].socket == INVALID_SOCKET)
			{
				continue;
			}
			//检查是否有数据过来
			ret = FD_ISSET(m_pPoolSocket[i].socket,&m_rset);
			if (ret != 0) //有读事件
			{
				//接收数据
				int recvLen=0;
				char recvBuffer[8192];
				int recvSize = sizeof(recvBuffer)-1;
				m_pTcp.AttachSocket(m_pPoolSocket[i].socket,m_pPoolSocket[i].address);
				recvLen = m_pTcp.Receive(recvBuffer,recvSize);
				if (recvLen < 0)
				{
					//接收出错，连接关闭
					m_log.LogMp(LOG_ERROR,__FILE__,__LINE__,"接收出错，连接关闭[%s-%d] %d",m_pPoolSocket[i].address,m_pPoolSocket[i].index,recvLen);
					printf("接收出错，连接关闭[%s-%d]\n",m_pPoolSocket[i].address,m_pPoolSocket[i].index);
					m_pTcp.Close();
					m_pPoolSocket[i].socket = INVALID_SOCKET;
					continue;
				}
				else //收到数据
				{
					if (recvLen >0)
					{
						//m_log.LogBin(LOG_ERROR,__FILE__,__LINE__,"接收到的数据",recvBuffer,recvLen);
						//此处是测试，直接返回
						m_nTcpRcvPackNum++;
						m_nTcpRcvLen+=recvLen;
						ret = m_pTcp.Send(recvBuffer,recvLen);
						if (ret < 0)
						{
							//发送出错，连接关闭
							printf("发送出错，连接关闭[%s-%d]\n",m_pPoolSocket[i].address,m_pPoolSocket[i].index);
							m_pTcp.Close();
							m_pPoolSocket[i].socket = INVALID_SOCKET;
							continue;
						}
						m_nTcpSendPackNum++;
						m_nTcpSendLen+=ret;
					}
				}
			}
			//没有读事件,继续检查是否可写
#if defined(_WINDOWS)
			ret = FD_ISSET(m_pPoolSocket[i].socket, &m_eset);
			if (ret != 0)
			{
				//主动连接出去的
				printf("尝试连接未成功[%s-%d]\n",m_pPoolSocket[i].index,m_pPoolSocket[i].address);
				m_pTcp.AttachSocket(m_pPoolSocket[i].socket,m_pPoolSocket[i].address);
				m_pTcp.Close();
				m_pPoolSocket[i].socket = INVALID_SOCKET;
				continue;
			}
			else if (FD_ISSET(m_pPoolSocket[i].socket, &m_wset) !=0 )
			{
				//可以在这里做发送数据处理了
			}
			//socket不可写
			continue;
#else
			
			int ok;
			int oklen;
			if (FD_ISSET(m_pPoolSocket[i].socket, &m_wset) !=0)
			{
				//如果是主动连接出去的连接的话，还要作判断
				bool isConnectOut = false;
				if (isConnectOut)
				{
					oklen=sizeof(ok);
#if defined(HP_UX)
					getsockopt(m_pPoolSocket[i].socket,SOL_SOCKET,SO_ERROR,(char *)&ok,&oklen);
#else	
					getsockopt(m_pPoolSocket[i].socket,SOL_SOCKET,SO_ERROR,(char *)&ok,(socklen_t *)&oklen);
#endif
					if (ok == 0)
					{
						//连接成功，可以作处理了。
					}
					else
					{
						//主动连接出去的
						printf("尝试连接未成功[%s-%d]\n",m_pPoolSocket[i].index,m_pPoolSocket[i].address);
						m_pTcp.AttachSocket(m_pPoolSocket[i].socket,m_pPoolSocket[i].address);
						m_pTcp.Close();
						m_pPoolSocket[i].socket = INVALID_SOCKET;
						continue;
					} // end if (ok == 0)
				}
				else  
				{
					//可以在这里做发送数据处理了
				}
			}
			//socket不可写
			continue;
#endif
		}
		return;
	}
	void TestTcpServer()
	{
		int nPort;
		int addressfamily;
		char tmpchar[20];
		int nTesttime;
		bzero(tmpchar,sizeof(tmpchar));
		m_nTcpRcvLen = 0;
		m_nTcpRcvPackNum = 0;
		m_nTcpSendLen = 0;
		m_nTcpSendPackNum = 0;
		m_nUdpRcvLen = 0;
		m_nUdpRcvPackNum = 0;
		m_nUdpSendLen = 0;
		m_nUdpSendPackNum = 0;
		while (1)
		{
			CBF_Tools::GetConsoleEnter("请输入TCP侦听端口",5,tmpchar);
			nPort = atoi(tmpchar);
			if (nPort<1024)
			{
				printf("侦听端口[%s]输入不合法!\n",tmpchar);
				continue;
			}
#ifdef _WINDOWS
			addressfamily = AF_INET;
#else
tcpmode:
			CBF_Tools::GetConsoleEnter("请输入TCP模式 0-AF_INET 1-AF_UNIX",1,tmpchar);
			switch(tmpchar[0])
			{
				case '0':
					addressfamily = AF_INET;
					break;
				case '1':
					addressfamily = AF_UNIX;
					break;
				default:
					printf("TCP模式[%s]输入不合法!\n",tmpchar);
					goto tcpmode;
			}
#endif
			CBF_Tools::GetConsoleEnter("请输入服务端运行时间(秒)",5,tmpchar);
			nTesttime = atoi(tmpchar)*1000;
			CBF_Tools::GetConsoleEnter("输入正确吗？(y/n)",1,tmpchar);
			if (tmpchar[0] == 'y' || tmpchar[0] == 'Y')
			{
				break;
			}
		}
		
		if (!m_pTcp.Create(addressfamily,false))
		{
			printf("Create失败\n");
			return;
		}
		if (!m_pTcp.Listen(nPort))
		{
			printf("Listen失败\n");
			return;
		}
		m_bIsExit = false;
		m_pPoolSocket[0].socket = m_pTcp.GetSocket();
		m_pTimer.SetTimer(0,4000,&tCallBack,this);
		m_pTimer.SetTimer(1,nTesttime,&tCallBack,this);
		m_pTimer.Start();
		//使用select模型作服务端
		TcpSelect();
		m_pTcp.Close();
		m_pTimer.Join();
		tCallBack(0,this);
		m_pTimer.KillTimer(0);
		m_pTimer.KillTimer(1);

	}
	void TestTcpClient()
	{
		int ret;
		int nmode;
		int timeout ;
		int nTesttime;
		int nPort;
		int addressfamily;
		char sip[32];
		char tmpchar[20];
		char testfilename[60];
		bzero(tmpchar,sizeof(tmpchar));
		bzero(testfilename,sizeof(testfilename));
		bzero(sip,sizeof(sip));
		struct_stat fs;

		m_nTcpRcvLen = 0;
		m_nTcpRcvPackNum = 0;
		m_nTcpSendLen = 0;
		m_nTcpSendPackNum = 0;
		m_nUdpRcvLen = 0;
		m_nUdpRcvPackNum = 0;
		m_nUdpSendLen = 0;
		m_nUdpSendPackNum = 0;

#ifdef _WINDOWS
		addressfamily = AF_INET;
#else
tcpmode:
		CBF_Tools::GetConsoleEnter("请输入TCP模式 0-AF_INET 1-AF_UNIX",1,tmpchar);
		switch(tmpchar[0])
		{
			case '0':
				addressfamily = AF_INET;
				break;
			case '1':
				addressfamily = AF_UNIX;
				break;
			default:
				printf("TCP模式[%s]输入不合法!\n",tmpchar);
				goto tcpmode;
		}
#endif
		while (1)
		{
			CBF_Tools::GetConsoleEnter("请输入服务端IP",30,sip);
			CBF_Tools::GetConsoleEnter("请输入服务端端口",5,tmpchar);
			nPort = atoi(tmpchar);
			CBF_Tools::GetConsoleEnter("请输入接收应答超时时间(毫秒)",5,tmpchar);
			timeout = atoi(tmpchar);
			CBF_Tools::GetConsoleEnter("请选择异步还是同步方式 0-异步 1-同步",1,tmpchar);
			nmode = atoi(tmpchar);
enterfile:
			CBF_Tools::GetConsoleEnter("请输入发送数据包文件(小于8k)",40,testfilename);
			
			fs.st_size =-1;
			FSTAT(testfilename,&fs);
			if (fs.st_size<1 || fs.st_size>=8192)
			{
				printf("输入的文件%s不符或不存在!",testfilename);
				goto enterfile;
			}
			CBF_Tools::GetConsoleEnter("请输入客户端运行时间(秒)",5,tmpchar);
			nTesttime = atoi(tmpchar)*1000;
			CBF_Tools::GetConsoleEnter("输入正确吗？(y/n)",1,tmpchar);
			if (tmpchar[0] == 'y' || tmpchar[0] == 'Y')
			{
				break;
			}
		}
		char sSendBuf[8192];
		char sRecvBuf[8192];
		int recvlen;
		int sendlen;
		bzero(sRecvBuf,sizeof(sRecvBuf));
		bzero(sSendBuf,sizeof(sSendBuf));
		FILE *fp = fopen(testfilename,"r");
		if (fp == NULL)
		{
			printf("打开文件[%s]失败\n",testfilename);
			return;
		}
		sendlen = fread(sSendBuf,1,fs.st_size,fp);
		fclose(fp);
		bool isconnect=true;
		
		if (nmode == 0) //异步方式
		{
			if (isconnect)
			{
				if (!m_pTcp.Create(addressfamily,false))
				{
					printf("Create失败\n");
					return;
				}
				if (!m_pTcp.ConnectServer(sip,nPort,timeout))
				{
					printf("异步连接服务端[%s-%d %d]失败\n",sip,nPort,timeout);
					return;
				}
				isconnect = false;
			}
			m_bIsExit = false;
			m_pTimer.SetTimer(2,4000,&tCallBack,this);
			m_pTimer.SetTimer(3,nTesttime,&tCallBack,this);
			m_pTimer.Start();
			while (!m_bIsExit)
			{
				if (isconnect)
				{
					if (!m_pTcp.Create(addressfamily,false))
					{
						printf("Create失败\n");
						m_pTimer.Stop();
						return;
					}
					if (!m_pTcp.ConnectServer(sip,nPort,timeout))
					{
						printf("异步连接服务端[%s-%d %d]失败\n",sip,nPort,timeout);
						m_pTimer.Stop();
						return;
					}
					isconnect = false;
				}
				
				ret = m_pTcp.Send(sSendBuf,sendlen);
				if (ret <0)
				{
					isconnect = true;
					m_pTcp.Close();
					printf("发送失败,关闭连接\n");
					continue;
				}
				m_nTcpSendLen +=ret;
				m_nTcpSendPackNum++;

				recvlen = sizeof(sRecvBuf);
				ret = m_pTcp.Receive(sRecvBuf,recvlen);
				if (ret <0)
				{
					isconnect = true;
					m_pTcp.Close();
					printf("接收失败,可能对方主动关闭，关闭连接\n");
					continue;
				}
				else if (ret >0)
				{
					m_nTcpRcvLen +=ret;
					m_nTcpRcvPackNum++;
				}
				
			}
		}
		else  //同步方式
		{
			if (isconnect)
			{
				if (!m_pTcp.Create(addressfamily,true))
				{
					printf("Create失败\n");
					m_pTimer.Stop();
					return;
				}
				if (!m_pTcp.ConnectServer(sip,nPort,0))
				{
					printf("同步连接服务端[%s-%d]失败\n",sip,nPort);
					m_pTimer.Stop();
					return;
				}
				isconnect = false;
			}
			m_bIsExit = false;
			m_pTimer.SetTimer(2,4000,&tCallBack,this);
			m_pTimer.SetTimer(3,nTesttime,&tCallBack,this);
			m_pTimer.Start();
			while (!m_bIsExit)
			{
				if (isconnect)
				{
					if (!m_pTcp.Create(addressfamily,true))
					{
						printf("Create失败\n");
						return;
					}
					if (!m_pTcp.ConnectServer(sip,nPort,0))
					{
						printf("同步连接服务端[%s-%d]失败\n",sip,nPort);
						return;
					}
					isconnect = false;
				}
				ret = m_pTcp.CheckWriteEvent(0);
				if (ret <=0 )
				{
					if (ret<0)
					{
						isconnect = true;
						m_pTcp.Close();
						printf("CheckWriteEvent<0,关闭连接\n");
					}
					continue;
				}
				ret = m_pTcp.Send(sSendBuf,sendlen);
				if (ret <0)
				{
					isconnect = true;
					m_pTcp.Close();
					printf("发送失败,关闭连接\n");
					continue;
				}
				m_nTcpSendLen +=ret;
				m_nTcpSendPackNum++;

				recvlen = sendlen;
				ret = m_pTcp.Receive(sRecvBuf,(unsigned int &)recvlen,(unsigned int )timeout);
				if (ret <0)
				{
					isconnect = true;
					m_pTcp.Close();
					if (recvlen >0 )
					{
						printf("收到部分数据[%d  %d] ",recvlen,sendlen);
						m_nTcpRcvLen +=recvlen;
						m_nTcpRcvPackNum++;
					}
					printf("接收失败,可能对方主动关闭，关闭连接\n");
					continue;
				}
				else if (ret >0)
				{
					m_nTcpRcvLen +=ret;
					m_nTcpRcvPackNum++;
				}
			
			}
			

		}
		m_pTcp.Close();
		m_pTimer.Join();
		tCallBack(2,this);
		m_pTimer.KillTimer(2);
		m_pTimer.KillTimer(3);
	}
	void TestTcp()
	{
		char tmpchar[20];
		bzero(tmpchar,sizeof(tmpchar));
		while (1)
		{
			CBF_Tools::GetConsoleEnter("请选择测试TCP服务端还是客户端，0-SERVER 1-CLIENT",1,tmpchar);
			if (tmpchar[0] == '0')
			{
				TestTcpServer();
			}
			if (tmpchar[0] == '1')
			{
				TestTcpClient();
			}
			CBF_Tools::GetConsoleEnter("退出TCP测试吗？(y/n)",1,tmpchar);
			if (tmpchar[0] == 'y' || tmpchar[0] == 'Y')
			{
				break;
			}
		}
	}
	void TestUdp()
	{
		char tmpchar[20];
		bzero(tmpchar,sizeof(tmpchar));
		while (1)
		{
			CBF_Tools::GetConsoleEnter("请选择测试UDP服务端还是客户端，0-SERVER 1-CLIENT",1,tmpchar);
			if (tmpchar[0] == '0')
			{
				TestUdpServer();
			}
			if (tmpchar[0] == '1')
			{
				TestUdpClient();
			}
			CBF_Tools::GetConsoleEnter("退出UDP测试吗？(y/n)",1,tmpchar);
			if (tmpchar[0] == 'y' || tmpchar[0] == 'Y')
			{
				break;
			}
		}
	}
	void TestUdpServer()
	{
		int nPort;
		int addressfamily;
		char tmpchar[20];
		int nTesttime;
		bzero(tmpchar,sizeof(tmpchar));
		m_nTcpRcvLen = 0;
		m_nTcpRcvPackNum = 0;
		m_nTcpSendLen = 0;
		m_nTcpSendPackNum = 0;
		m_nUdpRcvLen = 0;
		m_nUdpRcvPackNum = 0;
		m_nUdpSendLen = 0;
		m_nUdpSendPackNum = 0;

		while (1)
		{
			CBF_Tools::GetConsoleEnter("请输入UDP侦听端口",5,tmpchar);
			nPort = atoi(tmpchar);
			if (nPort<1024)
			{
				printf("UDP侦听端口[%s]输入不合法!\n",tmpchar);
				continue;
			}

			CBF_Tools::GetConsoleEnter("请输入服务端运行时间(秒)",5,tmpchar);
			nTesttime = atoi(tmpchar)*1000;
			CBF_Tools::GetConsoleEnter("输入正确吗？(y/n)",1,tmpchar);
			if (tmpchar[0] == 'y' || tmpchar[0] == 'Y')
			{
				break;
			}
		}

		if (!m_pUdp.Create())
		{
			printf("Create失败\n");
			return;
		}
		if (!m_pUdp.BindServer(nPort))
		{
			printf("BindServer失败\n");
			return;
		}
		m_bIsExit = false;
		m_pTimer.SetTimer(6,4000,&tCallBack,this);
		m_pTimer.SetTimer(7,nTesttime,&tCallBack,this);
		m_pTimer.Start();
		
		char sRecvBuf[8192];
		int recvlen;
		int ret;
		bzero(sRecvBuf,sizeof(sRecvBuf));
		while (!m_bIsExit)
		{
			if (m_pUdp.CheckReadEvent(1000) != 1)
			{
				continue;
			}
			recvlen = sizeof(sRecvBuf)-1;
			sockaddr_in cliaddr;
			ret = m_pUdp.RecvFrom(sRecvBuf,recvlen,cliaddr);
			if (ret >0)
			{
				m_nUdpRcvLen +=ret;
				m_nUdpRcvPackNum++;
				//发送回去
				recvlen = m_pUdp.SendTo(cliaddr,sRecvBuf,ret);
				if (recvlen <0)
				{
					printf("UDP发送错，socket关闭\n");
					m_pUdp.Close();
					m_pTimer.Stop();
					return;
				}
				m_nUdpSendLen+=recvlen;
				m_nUdpSendPackNum++;
			}
			else
			{
				if (ret <0)
				{
					printf("UDP接收错，socket关闭\n");
					m_pUdp.Close();
					m_pTimer.Stop();
					return;
				}
			}

		}
		m_pUdp.Close();
		m_pTimer.Join();
		tCallBack(6,this);
		m_pTimer.KillTimer(6);
		m_pTimer.KillTimer(7);
		
	}
	void TestUdpClient()
	{
		int nPort;
		int nType;
		int addressfamily;
		char sIp[33];
		char tmpchar[20];
		int nTesttime;
		bzero(tmpchar,sizeof(tmpchar));
		bzero(sIp,sizeof(sIp));
		char testfilename[60];
		
		bzero(testfilename,sizeof(testfilename));
		m_nTcpRcvLen = 0;
		m_nTcpRcvPackNum = 0;
		m_nTcpSendLen = 0;
		m_nTcpSendPackNum = 0;
		m_nUdpRcvLen = 0;
		m_nUdpRcvPackNum = 0;
		m_nUdpSendLen = 0;
		m_nUdpSendPackNum = 0;
		struct_stat fs;
		while (1)
		{
			CBF_Tools::GetConsoleEnter("请输入服务端Ip 长度小于8则为广播",20,sIp);
			CBF_Tools::GetConsoleEnter("请输入服务端UDP端口",5,tmpchar);
			nPort = atoi(tmpchar);
			if (nPort<1024)
			{
				printf("UDP侦听端口[%s]输入不合法!\n",tmpchar);
				continue;
			}

			CBF_Tools::GetConsoleEnter("输入正确吗？(y/n)",1,tmpchar);
			if (tmpchar[0] == 'y' || tmpchar[0] == 'Y')
			{
				break;
			}
		}
		while (1)
		{
			CBF_Tools::GetConsoleEnter("请输入发送数据包文件(小于8k)",40,testfilename);

			fs.st_size =-1;
			FSTAT(testfilename,&fs);
			if (fs.st_size<1 || fs.st_size>=8192)
			{
				printf("输入的文件%s不符或不存在!",testfilename);
				continue;;
			}
			CBF_Tools::GetConsoleEnter("请输入客户端运行时间(秒)",5,tmpchar);
			nTesttime = atoi(tmpchar)*1000;
			CBF_Tools::GetConsoleEnter("输入正确吗？(y/n)",1,tmpchar);
			if (tmpchar[0] == 'y' || tmpchar[0] == 'Y')
			{
				break;
			}

		}
		char sSendBuf[8192];
		bzero(sSendBuf,sizeof(sSendBuf));
		char sRecvBuf[8192];
		bzero(sRecvBuf,sizeof(sRecvBuf));
		FILE *fp = fopen(testfilename,"r");
		if (fp == NULL)
		{
			printf("打开文件[%s]失败\n",testfilename);
			m_pTimer.Stop();
			return;
		}
		int sendlen = fread(sSendBuf,1,fs.st_size,fp);
		fclose(fp);

		if (!m_pUdp.Create())
		{
			printf("Create失败\n");
			return;
		}
		if (!m_pUdp.BindClient(sIp,nPort))
		{
			printf("BindClient失败\n");
			return;
		}
		m_bIsExit = false;
		m_pTimer.SetTimer(8,4000,&tCallBack,this);
		m_pTimer.SetTimer(9,nTesttime,&tCallBack,this);
		m_pTimer.Start();
		int ret;
		while (!m_bIsExit)
		{
			ret = m_pUdp.SendTo(sSendBuf,sendlen);
			if (ret >0)
			{
				m_nUdpSendLen +=ret;
				m_nUdpSendPackNum++;

				//接收应答
				if (m_pUdp.CheckReadEvent(0) != 1)
				{
					continue;
				}
				int rcvlen= sizeof(sRecvBuf)-1;
				sockaddr_in srvaddr;
				int nrcvlen = m_pUdp.RecvFrom(sRecvBuf,rcvlen,srvaddr);
				if (ret <0)
				{
					printf("UDP接收错，socket关闭\n");
					m_pUdp.Close();
					m_pTimer.Stop();
					return;
				}
				if (ret != nrcvlen)
				{
					printf("接收的和发送的数据不相等\n");
				}
				m_nUdpRcvLen +=ret;
				m_nUdpRcvPackNum++;

			}
			else if (ret<0)
			{
				printf("SendTo失败 %d %d\n",ret,m_pUdp.GetErrorNo());
				m_pTimer.Stop();
				break;
			}
		}
		m_pUdp.Close();
		m_pTimer.Join();
		tCallBack(8,this);
		m_pTimer.KillTimer(8);
		m_pTimer.KillTimer(9);
	}
	void Test()
	{
		char tmpchar[20];
		bzero(tmpchar,sizeof(tmpchar));
		while (1)
		{
			CBF_Tools::GetConsoleEnter("请选择TCP还是UDP测试，0-TCP 1-UDP",1,tmpchar);
			if (tmpchar[0] == '0')
			{
				TestTcp();
			}
			if (tmpchar[0] == '1')
			{
				TestUdp();
			}
			CBF_Tools::GetConsoleEnter("退出SOCKET测试吗？(y/n)",1,tmpchar);
			if (tmpchar[0] == 'y' || tmpchar[0] == 'Y')
			{
				break;
			}
		}
		
	}
	CBF_SocketTcp m_pTcp;
	CBF_SocketUdp m_pUdp;
	bool m_bIsExit;
	fd_set m_rset;
	fd_set m_wset;
#if defined(_WINDOWS)
	fd_set m_eset;
#endif
	char m_sClientIp[32];

	INT64_ m_nTcpRcvLen;
	unsigned long m_nTcpRcvPackNum;
	INT64_ m_nTcpSendLen;
	unsigned long m_nTcpSendPackNum;

	INT64_ m_nUdpRcvLen;
	unsigned long m_nUdpRcvPackNum;
	INT64_ m_nUdpSendLen;
	unsigned long m_nUdpSendPackNum;

	CBF_Timer m_pTimer;

	static int tCallBack(unsigned int event,void *p)
	{
		CTestSocket *pp = (CTestSocket *)p;
		int num=0;
		int i;
		switch(event)
		{
			case 0:
#ifdef _WINDOWS
				printf("TCP服务端接收数据长度：%I64d 接收包数目:%ld 发送数据长度:%I64d 发送包数目%d \n",pp->m_nTcpRcvLen,\
					pp->m_nTcpRcvPackNum,pp->m_nTcpSendLen,pp->m_nTcpSendPackNum);
#else
				printf("TCP服务端接收数据长度：%lld 接收包数目:%ld 发送数据长度:%lld 发送包数目%d \n",pp->m_nTcpRcvLen,\
					pp->m_nTcpRcvPackNum,pp->m_nTcpSendLen,pp->m_nTcpSendPackNum);
#endif
				
				for (i=1 ; i< MAX_SOCKET_CONNECT ;i++)
				{
					if (pp->m_pPoolSocket[i].socket != INVALID_SOCKET)
					{
						num++;
					}

				}
				printf("TCP客户端连接数[%d]\n",num);
				break;
			case 1:
				pp->m_bIsExit = true;
				printf("TCP服务端运行时间到，退出!\n");
				pp->m_pTimer.Stop();
				break;
			case 2:
#ifdef _WINDOWS
				printf("TCP客户端接收数据长度：%I64d 接收包数目:%ld 发送数据长度:%I64d 发送包数目%d \n",pp->m_nTcpRcvLen,\
					pp->m_nTcpRcvPackNum,pp->m_nTcpSendLen,pp->m_nTcpSendPackNum);
#else
				printf("TCP客户端接收数据长度：%lld 接收包数目:%ld 发送数据长度:%lld 发送包数目%d \n",pp->m_nTcpRcvLen,\
					pp->m_nTcpRcvPackNum,pp->m_nTcpSendLen,pp->m_nTcpSendPackNum);
#endif
				break;
			case 3:
				pp->m_bIsExit = true;
				printf("TCP客户端运行时间到，退出!\n");
				pp->m_pTimer.Stop();
				break;
			case 6:
#ifdef _WINDOWS
				printf("UDP服务端接收数据长度：%I64d 接收包数目:%ld  发送数据长度：%I64d 发送包数目:%ld\n",\
					pp->m_nUdpRcvLen,pp->m_nUdpRcvPackNum,pp->m_nUdpSendLen,pp->m_nUdpSendPackNum);
#else
				printf("UDP服务端接收数据长度：%lld 接收包数目:%ld 发送数据长度：%lld 发送包数目:%ld\n",pp->m_nUdpRcvLen,\
					pp->m_nUdpRcvPackNum,pp->m_nUdpSendLen,pp->m_nUdpSendPackNum);
#endif
				break;
			case 7:
				pp->m_bIsExit = true;
				printf("UDP服务端运行时间到，退出!\n");
				pp->m_pTimer.Stop();
				break;
			case 8:
#ifdef _WINDOWS
				printf("UDP客户端发送数据长度:%I64d 发送包数目%d 接收数据长度:%I64d 接收包数目%d\n",\
					pp->m_nUdpSendLen,pp->m_nUdpSendPackNum,pp->m_nUdpRcvLen,pp->m_nUdpRcvPackNum);
#else
				printf("UDP客户端发送数据长度:%lld 发送包数目%d 接收数据长度:%lld 接收包数目%d\n",\
					pp->m_nUdpSendLen,pp->m_nUdpSendPackNum,pp->m_nUdpRcvLen,pp->m_nUdpRcvPackNum);
#endif
				break;
			case 9:
				pp->m_bIsExit = true;
				printf("UDP客户端运行时间到，退出!\n");
				pp->m_pTimer.Stop();
				break;
			default:
				break;

		}
		return 0;
	}

};

#endif // !defined(AFX_TESTSOCKET_H__CE2A91BA_2042_4876_A7B6_F4FA6512FC06__INCLUDED_)
