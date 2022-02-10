// DrebClient_Java.cpp: implementation of the CDrebClient_Java class.
//
//////////////////////////////////////////////////////////////////////

#include "DrebClient_Java.h"

CDrebClient_Java g_bfdreb_client_java;


CDrebClient_Java::CDrebClient_Java()
{
	//和java通讯使用的字节序
	m_pEndian.SetCommEndian(false);//针对java，使用网络序
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
	//port定义为4字节整型
	memcpy(tmpchar,port,4);
	//转成本机所用的字节序
	m_pEndian.Endian2LocalHost(tmpchar,4);
	int nport=0;
	memcpy(&nport,tmpchar,4);

	int ntimeout=30000;
	memcpy(tmpchar,timeout,4);
	//转成本机所用的字节序
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
	//返回re
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
		//port定义为4字节整型
		memcpy(tmpchar,port,4);
		//转成本机所用的字节序
		m_pEndian.Endian2LocalHost(tmpchar,4);
		int nport=0;
		memcpy(&nport,tmpchar,4);
		
		int ntimeout=30000;
		memcpy(tmpchar,timeoutms,4);
		//转成本机所用的字节序
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
	//转成本机所用的字节序
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
	else  //小于0
	{	
		return 102;
	}
}

void CDrebClient_Java::SetCallerEndian(char bigendian)
{
	if (bigendian == 1)
	{
		m_pEndian.SetCommEndian(false);//使用网络序
	}
	else
	{
		m_pEndian.SetCommEndian(true);//使用主机序
	}
	return;
}

// 函数名: SetCallerEndian
// 编程  : 王明松 2016-9-6 9:49:37
// 返回  : void 
// 参数  : char bigendian  1网络序:JAVA C/C++(hp_ux aix)   2主机序 C/C++(windows/linux)
// 描述  : 设置调用者字节序   默认为1网络序
void BFDREB_CLIENT_SetCallerEndian(char bigendian)
{
	g_bfdreb_client_java.SetCallerEndian(bigendian);
	return;
}

// 函数名: RecvData
// 编程  : 王明松 2016-9-6 9:12:55
// 返回  : char 0在指定时间内未收到数据  100成功 102接收失败 101未连接 103接收长度超限 1连接断开 2通讯平台应答数据不符 3CRC错误
// 参数  : char index 实例对应的索引
// 参数  : char *drebheaddata   对应总线的COMMSTRU结构
// 参数  : char *timeoutms  接收超时 毫秒
// 描述  : 接收应答
char BFDREB_CLIENT_RecvData(char index,char *drebheaddata,char *timeoutms)
{
	return g_bfdreb_client_java.RecvData(index,drebheaddata,timeoutms);
}

// 函数名: SendData
// 编程  : 王明松 2016-9-6 9:11:29
// 返回  : char 0失败  1发送出错 2数据太大  100发送成功
// 参数  : char index 实例对应的索引
// 参数  : char *drebheaddata  对应总线的COMMSTRU结构  
// 描述  : 发送请求，注意是同步阻塞连接，发送成功后确定要收应答就要马上调用RecvData
char BFDREB_CLIENT_SendData(char index,char *drebheaddata)
{
	return g_bfdreb_client_java.SendData(index,drebheaddata);
}

// 函数名: GetDrebHead
// 编程  : 王明松 2016-9-5 16:23:25
// 返回  : char  0失败  1成功
// 参数  : char index   实例对应的索引
// 参数  : char *head   DREB_HEAD的结构内容，已转为网络序 注意head空间要大于等于DREBHEADLEN
// 描述  : 取得连接上总线的报文头信息
char BFDREB_CLIENT_GetDrebHead(char index,char *head)
{
	return g_bfdreb_client_java.GetDrebHead(index,head);
}

// 函数名: IsConnect
// 编程  : 王明松 2016-9-5 16:10:46
// 返回  : char 0没连上，1成功连上
// 参数  : char index 实例对应的索引
// 描述  : 是否连接上总线
char BFDREB_CLIENT_IsConnect(char index)
{
	return g_bfdreb_client_java.IsConnect(index);
}

// 函数名: Close
// 编程  : 王明松 2016-9-5 15:44:09
// 返回  : void 
// 参数  : char index 实例对应的索引
// 描述  : 关闭总线连接
void BFDREB_CLIENT_Close(char index)
{
	g_bfdreb_client_java.Close(index);
	return;
}

// 函数名: Connect
// 编程  : 王明松 2016-9-5 15:42:57
// 返回  : char 0失败 1成功
// 参数  : char index 实例对应的索引
// 描述  : 连接总线
char BFDREB_CLIENT_Connect(char index)
{
	return g_bfdreb_client_java.Connect(index);
}

// 函数名: FreeInstance
// 编程  : 王明松 2016-9-5 15:19:56
// 返回  : void 
// 参数  : char index  实例对应的索引
// 描述  : 释放一个客户端实例
void BFDREB_CLIENT_FreeInstance(char index)
{
	g_bfdreb_client_java.FreeInstance(index);
	return;
}

// 函数名: NewInstance
// 编程  : 王明松 2016-9-5 14:56:53
// 返回  : char  返回的为实例对应的索引，每次调用要传入此索引  大于200为失败，即同时只能有200个实例。
// 参数  : char *ip   ip地址
// 参数  : char *port      4字节的整型   为了处理方便 java统一使用bytes字节数组传入整型值
// 参数  : char *timeout   4字节的整型   为了处理方便 java统一使用bytes字节数组传入整型值
// 描述  : 生成一个新的客户端实例  
char BFDREB_CLIENT_NewInstance(char *ip,char *port,char *timeout)
{
	return g_bfdreb_client_java.NewInstance(ip,port,timeout);
}
// 函数名: SetPara
// 编程  : 王明松 2016-9-6 10:20:41
// 返回  : void 
// 参数  : char index 实例对应的索引
// 参数  : char *ip   ip地址
// 参数  : char *port      4字节的整型   为了处理方便 java统一使用bytes字节数组传入整型值
// 参数  : char *timeout   4字节的整型   为了处理方便 java统一使用bytes字节数组传入整型值
// 描述  : 设置通讯参数
void BFDREB_CLIENT_SetPara(char index,char *ip,char *port ,char *timeoutms)
{
	g_bfdreb_client_java.SetPara(index,ip,port,timeoutms);
	return;
}
// 函数名: GetErrMsg
// 编程  : 王明松 2016-9-6 16:18:50
// 返回  : char  0失败 1成功 
// 参数  : char index 实例对应的索引
// 参数  : char *errmsg 出错信息
// 描述  : 取得出错信息
char BFDREB_CLIENT_GetErrMsg(char index,char *errmsg)
{
	return g_bfdreb_client_java.GetErrMsg(index,errmsg);
}
