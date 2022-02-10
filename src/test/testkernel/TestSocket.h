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
		char          address[32];//�ͻ���IP
		SOCKET_HANDLE socket;
	}SOCKET_POOL_DATA;

	SOCKET_POOL_DATA m_pPoolSocket[MAX_SOCKET_CONNECT] ;

	CBF_SimpleLog m_log;
	CTestSocket()
	{
		m_bIsExit = false;
		//���ӳ��ó�ֵ
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
			//ȡ������socket����socket���뵽fd_set����
			for (int i=0;i<MAX_SOCKET_CONNECT;i++)
			{
				if (m_pPoolSocket[i].socket != INVALID_SOCKET)
				{
					//��Ҫ������
					FD_SET(m_pPoolSocket[i].socket, &m_rset);//���뵽��select����
					//��Ҫ�������ݻ���������ʱ
					FD_SET(m_pPoolSocket[i].socket, &m_wset);//���뵽дselect����
#if defined(_WINDOWS)
					FD_SET(m_pPoolSocket[i].socket, &m_eset);//���뵽�����쳣����
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
			if (0 ==  result ) //���¼�����,��������
			{
				continue;
			}
			else //���¼�����
			{
				OnTcpSelectEvent();
			}
			
		}
	}
	void OnTcpSelectEvent()
	{
		int ret;
		bool isFindFree=false;
		//����������˿��ж����¼�����ȥ����accept
		ret = FD_ISSET(m_pPoolSocket[0].socket,&m_rset);
		if (ret != 0) 
		{
			m_pTcp.AttachSocket(m_pPoolSocket[0].socket,"");
			SOCKET_HANDLE tmpsocket = m_pTcp.Accept(m_sClientIp,false);
			if (tmpsocket != INVALID_SOCKET)
			{
				//�ڳ�����һ���յģ��Ž�ȥ��ʵ��Ӧ�ÿ��Ż�
				for (int i=1 ; i<MAX_SOCKET_CONNECT ; i++)
				{
					if (m_pPoolSocket[i].socket == INVALID_SOCKET)
					{
						isFindFree=true;
						m_pPoolSocket[i].socket = tmpsocket;
						strcpy(m_pPoolSocket[i].address,m_sClientIp);
						printf("���տͻ���[%s-%d]����\n",m_sClientIp,i);
						break;
					}
				}
				if (!isFindFree) //û�п��е�����
				{
					printf("���ӳ������޿������������տͻ���[%s]����\n",m_sClientIp);
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
			//����Ƿ������ݹ���
			ret = FD_ISSET(m_pPoolSocket[i].socket,&m_rset);
			if (ret != 0) //�ж��¼�
			{
				//��������
				int recvLen=0;
				char recvBuffer[8192];
				int recvSize = sizeof(recvBuffer)-1;
				m_pTcp.AttachSocket(m_pPoolSocket[i].socket,m_pPoolSocket[i].address);
				recvLen = m_pTcp.Receive(recvBuffer,recvSize);
				if (recvLen < 0)
				{
					//���ճ������ӹر�
					m_log.LogMp(LOG_ERROR,__FILE__,__LINE__,"���ճ������ӹر�[%s-%d] %d",m_pPoolSocket[i].address,m_pPoolSocket[i].index,recvLen);
					printf("���ճ������ӹر�[%s-%d]\n",m_pPoolSocket[i].address,m_pPoolSocket[i].index);
					m_pTcp.Close();
					m_pPoolSocket[i].socket = INVALID_SOCKET;
					continue;
				}
				else //�յ�����
				{
					if (recvLen >0)
					{
						//m_log.LogBin(LOG_ERROR,__FILE__,__LINE__,"���յ�������",recvBuffer,recvLen);
						//�˴��ǲ��ԣ�ֱ�ӷ���
						m_nTcpRcvPackNum++;
						m_nTcpRcvLen+=recvLen;
						ret = m_pTcp.Send(recvBuffer,recvLen);
						if (ret < 0)
						{
							//���ͳ������ӹر�
							printf("���ͳ������ӹر�[%s-%d]\n",m_pPoolSocket[i].address,m_pPoolSocket[i].index);
							m_pTcp.Close();
							m_pPoolSocket[i].socket = INVALID_SOCKET;
							continue;
						}
						m_nTcpSendPackNum++;
						m_nTcpSendLen+=ret;
					}
				}
			}
			//û�ж��¼�,��������Ƿ��д
#if defined(_WINDOWS)
			ret = FD_ISSET(m_pPoolSocket[i].socket, &m_eset);
			if (ret != 0)
			{
				//�������ӳ�ȥ��
				printf("��������δ�ɹ�[%s-%d]\n",m_pPoolSocket[i].index,m_pPoolSocket[i].address);
				m_pTcp.AttachSocket(m_pPoolSocket[i].socket,m_pPoolSocket[i].address);
				m_pTcp.Close();
				m_pPoolSocket[i].socket = INVALID_SOCKET;
				continue;
			}
			else if (FD_ISSET(m_pPoolSocket[i].socket, &m_wset) !=0 )
			{
				//�������������������ݴ�����
			}
			//socket����д
			continue;
#else
			
			int ok;
			int oklen;
			if (FD_ISSET(m_pPoolSocket[i].socket, &m_wset) !=0)
			{
				//������������ӳ�ȥ�����ӵĻ�����Ҫ���ж�
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
						//���ӳɹ��������������ˡ�
					}
					else
					{
						//�������ӳ�ȥ��
						printf("��������δ�ɹ�[%s-%d]\n",m_pPoolSocket[i].index,m_pPoolSocket[i].address);
						m_pTcp.AttachSocket(m_pPoolSocket[i].socket,m_pPoolSocket[i].address);
						m_pTcp.Close();
						m_pPoolSocket[i].socket = INVALID_SOCKET;
						continue;
					} // end if (ok == 0)
				}
				else  
				{
					//�������������������ݴ�����
				}
			}
			//socket����д
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
			CBF_Tools::GetConsoleEnter("������TCP�����˿�",5,tmpchar);
			nPort = atoi(tmpchar);
			if (nPort<1024)
			{
				printf("�����˿�[%s]���벻�Ϸ�!\n",tmpchar);
				continue;
			}
#ifdef _WINDOWS
			addressfamily = AF_INET;
#else
tcpmode:
			CBF_Tools::GetConsoleEnter("������TCPģʽ 0-AF_INET 1-AF_UNIX",1,tmpchar);
			switch(tmpchar[0])
			{
				case '0':
					addressfamily = AF_INET;
					break;
				case '1':
					addressfamily = AF_UNIX;
					break;
				default:
					printf("TCPģʽ[%s]���벻�Ϸ�!\n",tmpchar);
					goto tcpmode;
			}
#endif
			CBF_Tools::GetConsoleEnter("��������������ʱ��(��)",5,tmpchar);
			nTesttime = atoi(tmpchar)*1000;
			CBF_Tools::GetConsoleEnter("������ȷ��(y/n)",1,tmpchar);
			if (tmpchar[0] == 'y' || tmpchar[0] == 'Y')
			{
				break;
			}
		}
		
		if (!m_pTcp.Create(addressfamily,false))
		{
			printf("Createʧ��\n");
			return;
		}
		if (!m_pTcp.Listen(nPort))
		{
			printf("Listenʧ��\n");
			return;
		}
		m_bIsExit = false;
		m_pPoolSocket[0].socket = m_pTcp.GetSocket();
		m_pTimer.SetTimer(0,4000,&tCallBack,this);
		m_pTimer.SetTimer(1,nTesttime,&tCallBack,this);
		m_pTimer.Start();
		//ʹ��selectģ���������
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
		CBF_Tools::GetConsoleEnter("������TCPģʽ 0-AF_INET 1-AF_UNIX",1,tmpchar);
		switch(tmpchar[0])
		{
			case '0':
				addressfamily = AF_INET;
				break;
			case '1':
				addressfamily = AF_UNIX;
				break;
			default:
				printf("TCPģʽ[%s]���벻�Ϸ�!\n",tmpchar);
				goto tcpmode;
		}
#endif
		while (1)
		{
			CBF_Tools::GetConsoleEnter("����������IP",30,sip);
			CBF_Tools::GetConsoleEnter("���������˶˿�",5,tmpchar);
			nPort = atoi(tmpchar);
			CBF_Tools::GetConsoleEnter("���������Ӧ��ʱʱ��(����)",5,tmpchar);
			timeout = atoi(tmpchar);
			CBF_Tools::GetConsoleEnter("��ѡ���첽����ͬ����ʽ 0-�첽 1-ͬ��",1,tmpchar);
			nmode = atoi(tmpchar);
enterfile:
			CBF_Tools::GetConsoleEnter("�����뷢�����ݰ��ļ�(С��8k)",40,testfilename);
			
			fs.st_size =-1;
			FSTAT(testfilename,&fs);
			if (fs.st_size<1 || fs.st_size>=8192)
			{
				printf("������ļ�%s�����򲻴���!",testfilename);
				goto enterfile;
			}
			CBF_Tools::GetConsoleEnter("������ͻ�������ʱ��(��)",5,tmpchar);
			nTesttime = atoi(tmpchar)*1000;
			CBF_Tools::GetConsoleEnter("������ȷ��(y/n)",1,tmpchar);
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
			printf("���ļ�[%s]ʧ��\n",testfilename);
			return;
		}
		sendlen = fread(sSendBuf,1,fs.st_size,fp);
		fclose(fp);
		bool isconnect=true;
		
		if (nmode == 0) //�첽��ʽ
		{
			if (isconnect)
			{
				if (!m_pTcp.Create(addressfamily,false))
				{
					printf("Createʧ��\n");
					return;
				}
				if (!m_pTcp.ConnectServer(sip,nPort,timeout))
				{
					printf("�첽���ӷ����[%s-%d %d]ʧ��\n",sip,nPort,timeout);
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
						printf("Createʧ��\n");
						m_pTimer.Stop();
						return;
					}
					if (!m_pTcp.ConnectServer(sip,nPort,timeout))
					{
						printf("�첽���ӷ����[%s-%d %d]ʧ��\n",sip,nPort,timeout);
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
					printf("����ʧ��,�ر�����\n");
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
					printf("����ʧ��,���ܶԷ������رգ��ر�����\n");
					continue;
				}
				else if (ret >0)
				{
					m_nTcpRcvLen +=ret;
					m_nTcpRcvPackNum++;
				}
				
			}
		}
		else  //ͬ����ʽ
		{
			if (isconnect)
			{
				if (!m_pTcp.Create(addressfamily,true))
				{
					printf("Createʧ��\n");
					m_pTimer.Stop();
					return;
				}
				if (!m_pTcp.ConnectServer(sip,nPort,0))
				{
					printf("ͬ�����ӷ����[%s-%d]ʧ��\n",sip,nPort);
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
						printf("Createʧ��\n");
						return;
					}
					if (!m_pTcp.ConnectServer(sip,nPort,0))
					{
						printf("ͬ�����ӷ����[%s-%d]ʧ��\n",sip,nPort);
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
						printf("CheckWriteEvent<0,�ر�����\n");
					}
					continue;
				}
				ret = m_pTcp.Send(sSendBuf,sendlen);
				if (ret <0)
				{
					isconnect = true;
					m_pTcp.Close();
					printf("����ʧ��,�ر�����\n");
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
						printf("�յ���������[%d  %d] ",recvlen,sendlen);
						m_nTcpRcvLen +=recvlen;
						m_nTcpRcvPackNum++;
					}
					printf("����ʧ��,���ܶԷ������رգ��ر�����\n");
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
			CBF_Tools::GetConsoleEnter("��ѡ�����TCP����˻��ǿͻ��ˣ�0-SERVER 1-CLIENT",1,tmpchar);
			if (tmpchar[0] == '0')
			{
				TestTcpServer();
			}
			if (tmpchar[0] == '1')
			{
				TestTcpClient();
			}
			CBF_Tools::GetConsoleEnter("�˳�TCP������(y/n)",1,tmpchar);
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
			CBF_Tools::GetConsoleEnter("��ѡ�����UDP����˻��ǿͻ��ˣ�0-SERVER 1-CLIENT",1,tmpchar);
			if (tmpchar[0] == '0')
			{
				TestUdpServer();
			}
			if (tmpchar[0] == '1')
			{
				TestUdpClient();
			}
			CBF_Tools::GetConsoleEnter("�˳�UDP������(y/n)",1,tmpchar);
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
			CBF_Tools::GetConsoleEnter("������UDP�����˿�",5,tmpchar);
			nPort = atoi(tmpchar);
			if (nPort<1024)
			{
				printf("UDP�����˿�[%s]���벻�Ϸ�!\n",tmpchar);
				continue;
			}

			CBF_Tools::GetConsoleEnter("��������������ʱ��(��)",5,tmpchar);
			nTesttime = atoi(tmpchar)*1000;
			CBF_Tools::GetConsoleEnter("������ȷ��(y/n)",1,tmpchar);
			if (tmpchar[0] == 'y' || tmpchar[0] == 'Y')
			{
				break;
			}
		}

		if (!m_pUdp.Create())
		{
			printf("Createʧ��\n");
			return;
		}
		if (!m_pUdp.BindServer(nPort))
		{
			printf("BindServerʧ��\n");
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
				//���ͻ�ȥ
				recvlen = m_pUdp.SendTo(cliaddr,sRecvBuf,ret);
				if (recvlen <0)
				{
					printf("UDP���ʹ�socket�ر�\n");
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
					printf("UDP���մ�socket�ر�\n");
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
			CBF_Tools::GetConsoleEnter("����������Ip ����С��8��Ϊ�㲥",20,sIp);
			CBF_Tools::GetConsoleEnter("����������UDP�˿�",5,tmpchar);
			nPort = atoi(tmpchar);
			if (nPort<1024)
			{
				printf("UDP�����˿�[%s]���벻�Ϸ�!\n",tmpchar);
				continue;
			}

			CBF_Tools::GetConsoleEnter("������ȷ��(y/n)",1,tmpchar);
			if (tmpchar[0] == 'y' || tmpchar[0] == 'Y')
			{
				break;
			}
		}
		while (1)
		{
			CBF_Tools::GetConsoleEnter("�����뷢�����ݰ��ļ�(С��8k)",40,testfilename);

			fs.st_size =-1;
			FSTAT(testfilename,&fs);
			if (fs.st_size<1 || fs.st_size>=8192)
			{
				printf("������ļ�%s�����򲻴���!",testfilename);
				continue;;
			}
			CBF_Tools::GetConsoleEnter("������ͻ�������ʱ��(��)",5,tmpchar);
			nTesttime = atoi(tmpchar)*1000;
			CBF_Tools::GetConsoleEnter("������ȷ��(y/n)",1,tmpchar);
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
			printf("���ļ�[%s]ʧ��\n",testfilename);
			m_pTimer.Stop();
			return;
		}
		int sendlen = fread(sSendBuf,1,fs.st_size,fp);
		fclose(fp);

		if (!m_pUdp.Create())
		{
			printf("Createʧ��\n");
			return;
		}
		if (!m_pUdp.BindClient(sIp,nPort))
		{
			printf("BindClientʧ��\n");
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

				//����Ӧ��
				if (m_pUdp.CheckReadEvent(0) != 1)
				{
					continue;
				}
				int rcvlen= sizeof(sRecvBuf)-1;
				sockaddr_in srvaddr;
				int nrcvlen = m_pUdp.RecvFrom(sRecvBuf,rcvlen,srvaddr);
				if (ret <0)
				{
					printf("UDP���մ�socket�ر�\n");
					m_pUdp.Close();
					m_pTimer.Stop();
					return;
				}
				if (ret != nrcvlen)
				{
					printf("���յĺͷ��͵����ݲ����\n");
				}
				m_nUdpRcvLen +=ret;
				m_nUdpRcvPackNum++;

			}
			else if (ret<0)
			{
				printf("SendToʧ�� %d %d\n",ret,m_pUdp.GetErrorNo());
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
			CBF_Tools::GetConsoleEnter("��ѡ��TCP����UDP���ԣ�0-TCP 1-UDP",1,tmpchar);
			if (tmpchar[0] == '0')
			{
				TestTcp();
			}
			if (tmpchar[0] == '1')
			{
				TestUdp();
			}
			CBF_Tools::GetConsoleEnter("�˳�SOCKET������(y/n)",1,tmpchar);
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
				printf("TCP����˽������ݳ��ȣ�%I64d ���հ���Ŀ:%ld �������ݳ���:%I64d ���Ͱ���Ŀ%d \n",pp->m_nTcpRcvLen,\
					pp->m_nTcpRcvPackNum,pp->m_nTcpSendLen,pp->m_nTcpSendPackNum);
#else
				printf("TCP����˽������ݳ��ȣ�%lld ���հ���Ŀ:%ld �������ݳ���:%lld ���Ͱ���Ŀ%d \n",pp->m_nTcpRcvLen,\
					pp->m_nTcpRcvPackNum,pp->m_nTcpSendLen,pp->m_nTcpSendPackNum);
#endif
				
				for (i=1 ; i< MAX_SOCKET_CONNECT ;i++)
				{
					if (pp->m_pPoolSocket[i].socket != INVALID_SOCKET)
					{
						num++;
					}

				}
				printf("TCP�ͻ���������[%d]\n",num);
				break;
			case 1:
				pp->m_bIsExit = true;
				printf("TCP���������ʱ�䵽���˳�!\n");
				pp->m_pTimer.Stop();
				break;
			case 2:
#ifdef _WINDOWS
				printf("TCP�ͻ��˽������ݳ��ȣ�%I64d ���հ���Ŀ:%ld �������ݳ���:%I64d ���Ͱ���Ŀ%d \n",pp->m_nTcpRcvLen,\
					pp->m_nTcpRcvPackNum,pp->m_nTcpSendLen,pp->m_nTcpSendPackNum);
#else
				printf("TCP�ͻ��˽������ݳ��ȣ�%lld ���հ���Ŀ:%ld �������ݳ���:%lld ���Ͱ���Ŀ%d \n",pp->m_nTcpRcvLen,\
					pp->m_nTcpRcvPackNum,pp->m_nTcpSendLen,pp->m_nTcpSendPackNum);
#endif
				break;
			case 3:
				pp->m_bIsExit = true;
				printf("TCP�ͻ�������ʱ�䵽���˳�!\n");
				pp->m_pTimer.Stop();
				break;
			case 6:
#ifdef _WINDOWS
				printf("UDP����˽������ݳ��ȣ�%I64d ���հ���Ŀ:%ld  �������ݳ��ȣ�%I64d ���Ͱ���Ŀ:%ld\n",\
					pp->m_nUdpRcvLen,pp->m_nUdpRcvPackNum,pp->m_nUdpSendLen,pp->m_nUdpSendPackNum);
#else
				printf("UDP����˽������ݳ��ȣ�%lld ���հ���Ŀ:%ld �������ݳ��ȣ�%lld ���Ͱ���Ŀ:%ld\n",pp->m_nUdpRcvLen,\
					pp->m_nUdpRcvPackNum,pp->m_nUdpSendLen,pp->m_nUdpSendPackNum);
#endif
				break;
			case 7:
				pp->m_bIsExit = true;
				printf("UDP���������ʱ�䵽���˳�!\n");
				pp->m_pTimer.Stop();
				break;
			case 8:
#ifdef _WINDOWS
				printf("UDP�ͻ��˷������ݳ���:%I64d ���Ͱ���Ŀ%d �������ݳ���:%I64d ���հ���Ŀ%d\n",\
					pp->m_nUdpSendLen,pp->m_nUdpSendPackNum,pp->m_nUdpRcvLen,pp->m_nUdpRcvPackNum);
#else
				printf("UDP�ͻ��˷������ݳ���:%lld ���Ͱ���Ŀ%d �������ݳ���:%lld ���հ���Ŀ%d\n",\
					pp->m_nUdpSendLen,pp->m_nUdpSendPackNum,pp->m_nUdpRcvLen,pp->m_nUdpRcvPackNum);
#endif
				break;
			case 9:
				pp->m_bIsExit = true;
				printf("UDP�ͻ�������ʱ�䵽���˳�!\n");
				pp->m_pTimer.Stop();
				break;
			default:
				break;

		}
		return 0;
	}

};

#endif // !defined(AFX_TESTSOCKET_H__CE2A91BA_2042_4876_A7B6_F4FA6512FC06__INCLUDED_)
