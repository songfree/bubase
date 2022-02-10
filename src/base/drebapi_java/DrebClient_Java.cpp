// DrebClient_Java.cpp: implementation of the CDrebClient_Java class.
//
//////////////////////////////////////////////////////////////////////

#include "DrebClient_Java.h"

CDrebClient_Java g_bfdreb_client_java;


CDrebClient_Java::CDrebClient_Java()
{
	//��javaͨѶʹ�õ��ֽ���
	m_pEndian.SetCommEndian(false);//���java��ʹ��������
}
CDrebClient_Java::~CDrebClient_Java()
{
	
}

char CDrebClient_Java::NewInstance(char *ip, char *port, char *timeout)
{
	if (m_pInstanceList.Size()>MAXINSTANCE_NUM)
	{
		return INSTANCENUM_ERR;
	}
	CDrebClient *cli = new CDrebClient();
	if (cli == NULL)
	{
		return INSTANCENEW_ERR;
	}
	unsigned char tmpchar[10];
	bzero(tmpchar,sizeof(tmpchar));
	//port����Ϊ4�ֽ�����
	memcpy(tmpchar,port,4);
	//ת�ɱ������õ��ֽ���
	m_pEndian.Endian2LocalHost(tmpchar,4);
	int nport=0;
	memcpy(&nport,tmpchar,4);

	int ntimeout=30000;
	memcpy(tmpchar,timeout,4);
	//ת�ɱ������õ��ֽ���
	m_pEndian.Endian2LocalHost(tmpchar,4);
	memcpy(&ntimeout,tmpchar,4);
	if (ntimeout<2000)
	{
		ntimeout= 2000;
	}
#ifdef DEBUG
	printf("init ip[%s] port[%d] timeout[%d]\n",ip,nport,ntimeout);
#endif

	if (!cli->Init(ip,nport,ntimeout))
	{
		return INSTANCEINIT_ERR;
	}
// 	S_DREB_CLIENT client;
// 	client.client = cli;
// 	client.id = 0;
	int ret = m_pInstanceList.Insert(cli);
	cli = NULL;
	//����re
	return ret;
}

void CDrebClient_Java::SetPara(char index, char *ip, char *port, char *timeoutms)
{
	int ret = index;
	CDrebClient *cli = m_pInstanceList.Find(ret);
	if (cli != NULL)
	{
		unsigned char tmpchar[10];
		bzero(tmpchar,sizeof(tmpchar));
		//port����Ϊ4�ֽ�����
		memcpy(tmpchar,port,4);
		//ת�ɱ������õ��ֽ���
		m_pEndian.Endian2LocalHost(tmpchar,4);
		int nport=0;
		memcpy(&nport,tmpchar,4);
		
		int ntimeout=30000;
		memcpy(tmpchar,timeoutms,4);
		//ת�ɱ������õ��ֽ���
		m_pEndian.Endian2LocalHost(tmpchar,4);
		memcpy(&ntimeout,tmpchar,4);
		if (ntimeout<2000)
		{
			ntimeout= 2000;
		}
#ifdef DEBUG
		printf("init ip[%s] port[%d] timeout[%d]\n",ip,nport,ntimeout);
#endif
		
		if (!cli->Init(ip,nport,ntimeout))
		{
			return ;
		}
	}
	return;
}
void CDrebClient_Java::FreeInstance(char index)
{
	int ret = index;
	CDrebClient *cli = m_pInstanceList.Find(ret);
	if (cli != NULL )
	{
		delete cli;
		cli = NULL;
		m_pInstanceList.Delete(ret);
	}
	return;
}

char CDrebClient_Java::Connect(char index)
{
	int ret = index;
	CDrebClient *cli = m_pInstanceList.Find(ret);
	if (cli != NULL)
	{
		if (!cli->Connect())
		{
			return 0;
		}
		else
		{
			return 1;
		}
	}
	return 0;
}

void CDrebClient_Java::Close(char index)
{
	int ret = index;
	CDrebClient *cli = m_pInstanceList.Find(ret);
	if (cli != NULL)
	{
		cli->Close();
	}
	return ;
}

char CDrebClient_Java::IsConnect(char index)
{
	int ret = index;
	CDrebClient *cli = m_pInstanceList.Find(ret);
	if (cli != NULL)
	{
		if (cli->IsConnect())
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	return 0;
}

char CDrebClient_Java::GetDrebHead(char index, char *head)
{
	int ret = index;
	CDrebClient *cli = m_pInstanceList.Find(ret);
	if (cli != NULL)
	{
		DREB_HEAD dhead;
		cli->GetDrebHead(dhead);
		m_pEndian.Endian2Comm(&dhead);
		memcpy(head,&dhead,DREBHEADLEN);
		return 1;
	}
	return 0;
}

char CDrebClient_Java::SendData(char index,char *drebheaddata)
{
	int ret = index;
	CDrebClient *cli = m_pInstanceList.Find(ret);
	if (cli == NULL )
	{
		return 0;
	}
	COMMSTRU data;
	memcpy(&(data.head),drebheaddata,DREBHEADLEN);
	m_pEndian.Endian2LocalHost(&(data.head));
	if (data.head.nLen <=0 || data.head.nLen>DREBDATASIZE)
	{
		return 2;
	}
	memcpy(data.buffer,drebheaddata+DREBHEADLEN,data.head.nLen);
	ret = cli->SendData(data.head.s_Sinfo.s_nHook,data.head.s_Sinfo.s_nSerial,data.head.cCmd,data.head.d_Dinfo,data.buffer,data.head.nLen,data.head.cZip,data.head.n_Ninfo.n_nNextNo);
	if (ret == -1)
	{
		return 1;
	}
	else if (ret == -2)
	{
		return 2;
	}
	else if (ret >0 )
	{
		return 100;
	}
	else
	{
		return 1;
	}
}

char CDrebClient_Java::GetErrMsg(char index,char *errmsg)
{
	int ret = index;
	CDrebClient *cli = m_pInstanceList.Find(ret);
	if (cli != NULL )
	{
		strcpy(errmsg,cli->m_errMsg);
		return 1;
	}
	return 0;
}

char CDrebClient_Java::RecvData(char index, char *drebheaddata, char *timeoutms)
{
	int ret = index;
	CDrebClient *cli = m_pInstanceList.Find(ret);
	if (cli == NULL)
	{
		return 0;
	}
	int ntimeout=30000;
	unsigned char tmpchar[10];
	bzero(tmpchar,sizeof(tmpchar));
	memcpy(tmpchar,timeoutms,4);
	//ת�ɱ������õ��ֽ���
	m_pEndian.Endian2LocalHost(tmpchar,4);
	memcpy(&ntimeout,tmpchar,4);
	if (ntimeout<2000)
	{
		ntimeout= 2000;
	}
	COMMSTRU data;
	unsigned int datalen=DREBDATASIZE;
	ret = cli->ReceveData(data.head,(unsigned char *)(data.buffer),datalen,ntimeout);
	if (ret == 0)
	{
		return 0;
	}
	else if (ret == -100)
	{
		return 101;
	}
	else if (ret == -1)
	{
		return 1;
	}
	else if (ret == -2)
	{
		return 2;
	}
	else if (ret == -3)
	{
		return 3;
	}
	else if (ret >0)
	{
		if (data.head.nLen>DREBDATASIZE)
		{
			return 103;
		}
		if (data.head.nLen>0)
		{
			memcpy(drebheaddata+DREBHEADLEN,data.buffer,data.head.nLen);
		}
		m_pEndian.Endian2Comm(&(data.head));
		memcpy(drebheaddata,&(data.head),DREBHEADLEN);
		return 100;
	}
	else  //С��0
	{	
		return 102;
	}
}

void CDrebClient_Java::SetCallerEndian(char bigendian)
{
	if (bigendian == 1)
	{
		m_pEndian.SetCommEndian(false);//ʹ��������
	}
	else
	{
		m_pEndian.SetCommEndian(true);//ʹ��������
	}
	return;
}

// ������: SetCallerEndian
// ���  : ������ 2016-9-6 9:49:37
// ����  : void 
// ����  : char bigendian  1������:JAVA C/C++(hp_ux aix)   2������ C/C++(windows/linux)
// ����  : ���õ������ֽ���   Ĭ��Ϊ1������
void BFDREB_CLIENT_SetCallerEndian(char bigendian)
{
	g_bfdreb_client_java.SetCallerEndian(bigendian);
	return;
}

// ������: RecvData
// ���  : ������ 2016-9-6 9:12:55
// ����  : char 0��ָ��ʱ����δ�յ�����  100�ɹ� 102����ʧ�� 101δ���� 103���ճ��ȳ��� 1���ӶϿ� 2ͨѶƽ̨Ӧ�����ݲ��� 3CRC����
// ����  : char index ʵ����Ӧ������
// ����  : char *drebheaddata   ��Ӧ���ߵ�COMMSTRU�ṹ
// ����  : char *timeoutms  ���ճ�ʱ ����
// ����  : ����Ӧ��
char BFDREB_CLIENT_RecvData(char index,char *drebheaddata,char *timeoutms)
{
	return g_bfdreb_client_java.RecvData(index,drebheaddata,timeoutms);
}

// ������: SendData
// ���  : ������ 2016-9-6 9:11:29
// ����  : char 0ʧ��  1���ͳ��� 2����̫��  100���ͳɹ�
// ����  : char index ʵ����Ӧ������
// ����  : char *drebheaddata  ��Ӧ���ߵ�COMMSTRU�ṹ  
// ����  : ��������ע����ͬ���������ӣ����ͳɹ���ȷ��Ҫ��Ӧ���Ҫ���ϵ���RecvData
char BFDREB_CLIENT_SendData(char index,char *drebheaddata)
{
	return g_bfdreb_client_java.SendData(index,drebheaddata);
}

// ������: GetDrebHead
// ���  : ������ 2016-9-5 16:23:25
// ����  : char  0ʧ��  1�ɹ�
// ����  : char index   ʵ����Ӧ������
// ����  : char *head   DREB_HEAD�Ľṹ���ݣ���תΪ������ ע��head�ռ�Ҫ���ڵ���DREBHEADLEN
// ����  : ȡ�����������ߵı���ͷ��Ϣ
char BFDREB_CLIENT_GetDrebHead(char index,char *head)
{
	return g_bfdreb_client_java.GetDrebHead(index,head);
}

// ������: IsConnect
// ���  : ������ 2016-9-5 16:10:46
// ����  : char 0û���ϣ�1�ɹ�����
// ����  : char index ʵ����Ӧ������
// ����  : �Ƿ�����������
char BFDREB_CLIENT_IsConnect(char index)
{
	return g_bfdreb_client_java.IsConnect(index);
}

// ������: Close
// ���  : ������ 2016-9-5 15:44:09
// ����  : void 
// ����  : char index ʵ����Ӧ������
// ����  : �ر���������
void BFDREB_CLIENT_Close(char index)
{
	g_bfdreb_client_java.Close(index);
	return;
}

// ������: Connect
// ���  : ������ 2016-9-5 15:42:57
// ����  : char 0ʧ�� 1�ɹ�
// ����  : char index ʵ����Ӧ������
// ����  : ��������
char BFDREB_CLIENT_Connect(char index)
{
	return g_bfdreb_client_java.Connect(index);
}

// ������: FreeInstance
// ���  : ������ 2016-9-5 15:19:56
// ����  : void 
// ����  : char index  ʵ����Ӧ������
// ����  : �ͷ�һ���ͻ���ʵ��
void BFDREB_CLIENT_FreeInstance(char index)
{
	g_bfdreb_client_java.FreeInstance(index);
	return;
}

// ������: NewInstance
// ���  : ������ 2016-9-5 14:56:53
// ����  : char  ���ص�Ϊʵ����Ӧ��������ÿ�ε���Ҫ���������  ����200Ϊʧ�ܣ���ͬʱֻ����200��ʵ����
// ����  : char *ip   ip��ַ
// ����  : char *port      4�ֽڵ�����   Ϊ�˴����� javaͳһʹ��bytes�ֽ����鴫������ֵ
// ����  : char *timeout   4�ֽڵ�����   Ϊ�˴����� javaͳһʹ��bytes�ֽ����鴫������ֵ
// ����  : ����һ���µĿͻ���ʵ��  
char BFDREB_CLIENT_NewInstance(char *ip,char *port,char *timeout)
{
	return g_bfdreb_client_java.NewInstance(ip,port,timeout);
}
// ������: SetPara
// ���  : ������ 2016-9-6 10:20:41
// ����  : void 
// ����  : char index ʵ����Ӧ������
// ����  : char *ip   ip��ַ
// ����  : char *port      4�ֽڵ�����   Ϊ�˴����� javaͳһʹ��bytes�ֽ����鴫������ֵ
// ����  : char *timeout   4�ֽڵ�����   Ϊ�˴����� javaͳһʹ��bytes�ֽ����鴫������ֵ
// ����  : ����ͨѶ����
void BFDREB_CLIENT_SetPara(char index,char *ip,char *port ,char *timeoutms)
{
	g_bfdreb_client_java.SetPara(index,ip,port,timeoutms);
	return;
}
// ������: GetErrMsg
// ���  : ������ 2016-9-6 16:18:50
// ����  : char  0ʧ�� 1�ɹ� 
// ����  : char index ʵ����Ӧ������
// ����  : char *errmsg ������Ϣ
// ����  : ȡ�ó�����Ϣ
char BFDREB_CLIENT_GetErrMsg(char index,char *errmsg)
{
	return g_bfdreb_client_java.GetErrMsg(index,errmsg);
}
