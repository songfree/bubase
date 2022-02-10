// SrvThread.cpp: implementation of the CSrvThread class.
//
//////////////////////////////////////////////////////////////////////

#include "SrvThread.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CSrvThread::CSrvThread()
{

}

CSrvThread::~CSrvThread()
{

}

int CSrvThread::RunSelect()
{
	int nRet;
	int nLen;
	char sip[100];
	int offset=0;
	char databuffer[200*1024+1];
	int nDatalen= sizeof(databuffer)-1;
	CBF_SocketTcp  pSocketCli;
	if ( m_pSocket.CheckReadEvent(1000)==1)
	{
		SOCKET_HANDLE clisocket = m_pSocket.Accept(sip,false);
		if (clisocket != NULL)
		{
			printf("���յ�%s������\n",sip);
			pSocketCli.AttachSocket(clisocket,"");
			FILE *fp;
			fp = fopen("bfbpcd.exe","rb");
			if (fp == NULL)
			{
				printf("���ļ���\n");
				pSocketCli.Close();
				return -1;
			}
			struct_stat fszie;
			FSTAT("bfbpcd.exe",&fszie);
			sprintf(sip,"%08d",fszie.st_size);
			nRet = pSocketCli.Send(sip,8);
			if (nRet !=8 )
			{
				printf("Send���� %d\n",nRet);
				pSocketCli.Close();
				return -1;
			}

			bool isend=false;
			nLen = fread(databuffer,1,nDatalen,fp);
			if (nLen <1)
			{
				printf("�ļ�������\n");
				fclose(fp);
				return -1;
			}
			if (nLen<nDatalen)
			{
				isend = true;
			}
			while (true)
			{
				printf("��ʼ�������� offset[%d]  nLen[%d]\n",offset,nLen);
				nRet = pSocketCli.Send(databuffer+offset,nLen);
				if (nRet <0)
				{
					printf("Send���� %d\n",nRet);
					pSocketCli.Close();
					return -1;
				}
				if (nRet != nLen)
				{
					printf("���Ͳ������� %d\n",nRet);
					nLen = nLen - nRet;
					offset = nRet;
					while (pSocketCli.CheckWriteEvent(20000) != 1)
					{
						printf("�ȴ����ӿ�д \n");
						SLEEP(1000);	
					}
					printf("���������� %d\n",nLen);
					continue;
				}
				else
				{
					printf("���������� %d\n",nLen);
					if (isend)
					{
						printf("�������\n");
						SLEEP(100000);
						pSocketCli.Close();
						fclose(fp);
						return 0;
					}
					offset = 0;
					nLen = fread(databuffer,1,nDatalen,fp);
					if (nLen <0)
					{
						printf("�������\n");
						pSocketCli.Close();
						fclose(fp);
						return 0;
					}
					printf("Ҫ�������� offset[0] nLen[%d]\n",nLen);
					if (nLen<nDatalen)
					{
						isend = true;
					}
				}
				
				
				
			}
		}
	}
	return 0;
}
// ������: InitThreadInstance
// ���  : ������ 2012-4-9 12:00:54
// ����  : virtual bool 
// ����  : �̳߳�ʼ�����߳�����ʱ����һ��
bool CSrvThread::InitThreadInstance()
{
	return true;
}

// ������: ExitThreadInstance
// ���  : ������ 2012-4-9 12:01:05
// ����  : virtual void 
// ����  : �˳��߳�Ҫ���Ĺ���,�߳��˳�ʱ����һ��
void CSrvThread::ExitThreadInstance()
{
	return;
}
int CSrvThread::Run()
{
	return 0;
}
bool CSrvThread::Listen(unsigned short port)
{
	m_pSocket.Create(AF_INET,false);
	if (!m_pSocket.Listen(port,5))
	{
		printf("Listenʧ�� %d\n",port);
		return false;
	}
	printf("��ʼ[%d]����\n",port);
	return true;
}
