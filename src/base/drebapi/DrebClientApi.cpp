// DrebClientApi.cpp: implementation of the CDrebClientApi class.
//
//////////////////////////////////////////////////////////////////////

#include "DrebClientApi.h"
#include "DrebClient.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


// 函数名: BFDREBCLIENT_API_Monitor_Dreb
// 编程  : 王明松 2009-12-2 15:23:40
// 返回  : int >0<DREBHEADLEN通讯平台返回错 >DREBHEADLEN收到的数据长度 =0在指定时间内未收到数据  <0失败 -100未连接 -1连接断开 -2通讯平台应答数据不符 -3CRC错误
// 参数  : void *pClass    客户端类指针
// 参数  : int hook
// 参数  : int serial
// 参数  : DREB_D_NODEINFO dest
// 参数  : const char *data  对应cZip的数据(明文加密或压缩)
// 参数  : int datalen 对应cZip的数据长度
// 参数  : int cZip
// 参数  : DREB_HEAD &head
// 参数  : char *ansdata   对应head->cZip解开后的明文数据,自定义除外
// 描述  : 通讯平台监控 要求最终处理完成后应答
int BFDREBCLIENT_API_Monitor_Dreb(void *pClass,int hook,int serial,DREB_D_NODEINFO dest, const char *data, int datalen,int cZip,DREB_HEAD &head,char *ansdata)
{
	if (pClass == NULL)
	{
		return -3;
	}
	CDrebClient *cliapi = (CDrebClient *)pClass;
	return cliapi->Monitor_Dreb(hook,serial,dest,data,datalen,cZip,head,ansdata);
}

// 函数名: BFDREBCLIENT_API_Monitor_Bpc
// 编程  : 王明松 2009-12-2 15:23:40
// 返回  : int >0<DREBHEADLEN通讯平台返回错 >DREBHEADLEN收到的数据长度 =0在指定时间内未收到数据  <0失败 -100未连接 -1连接断开 -2通讯平台应答数据不符 -3CRC错误
// 参数  : void *pClass    客户端类指针
// 参数  : int hook
// 参数  : int serial
// 参数  : DREB_D_NODEINFO dest
// 参数  : const char *data  对应cZip的数据(明文加密或压缩)
// 参数  : int datalen 对应cZip的数据长度
// 参数  : int cZip
// 参数  : DREB_HEAD &head
// 参数  : char *ansdata   对应head->cZip解开后的明文数据,自定义除外
// 描述  : BPC监控 要求最终处理完成后应答
int BFDREBCLIENT_API_Monitor_Bpc(void *pClass,int hook,int serial,DREB_D_NODEINFO dest, const char *data, int datalen,int cZip,DREB_HEAD &head,char *ansdata)
{
	if (pClass == NULL)
	{
		return -3;
	}
	CDrebClient *cliapi = (CDrebClient *)pClass;
	return cliapi->Monitor_Bpc(hook,serial,dest,data,datalen,cZip,head,ansdata);
}

// 函数名: BFDREBCLIENT_API_GetDrebHead
// 编程  : 王明松 2010-9-28 9:18:45
// 返回  : void 
// 参数  : DREB_HEAD &head
// 描述  : 获取连接时返回的dreb头
void BFDREBCLIENT_API_GetDrebHead(void *pClass,DREB_HEAD &head)
{
	if (pClass == NULL)
	{
		return ;
	}
	CDrebClient *cliapi = (CDrebClient *)pClass;
	cliapi->GetDrebHead(head);
	return;
}

// 函数名: BFDREBCLIENT_API_DPABC
// 编程  : 王明松 2009-12-2 15:25:00
// 返回  : int <0发送失败，>=发送的数据长度
// 参数  : void *pClass    客户端类指针
// 参数  : int hook
// 参数  : int serial
// 参数  : DREB_D_NODEINFO dest
// 参数  : const char *data  对应cZip的数据(明文加密或压缩)
// 参数  : int datalen 对应cZip的数据长度
// 参数  : int cZip
// 描述  : 通讯平台广播，即将信息发给指定通讯平台上所有注册的服务,不要求通讯平台应答
int BFDREBCLIENT_API_DPABC(void *pClass,int hook, int serial,int gateindex,DREB_D_NODEINFO dest, const char *data, int datalen,int cZip,int nextno)
{
	if (pClass == NULL)
	{
		return -3;
	}
	CDrebClient *cliapi = (CDrebClient *)pClass;
	return cliapi->DPABC(hook,serial,gateindex,dest,data,datalen,cZip,nextno);
}

// 函数名: BFDREBCLIENT_API_DPBC
// 编程  : 王明松 2009-12-2 15:24:42
// 返回  : int =100成功 >0且<100dreb返回错 <0发送失败 =0未收到确认消息
// 参数  : void *pClass    客户端类指针
// 参数  : int hook
// 参数  : int serial
// 参数  : DREB_D_NODEINFO dest
// 参数  : const char *data  对应cZip的数据(明文加密或压缩)
// 参数  : int datalen 对应cZip的数据长度
// 参数  : int cZip
// 描述  : 通讯平台广播，即将信息发给指定通讯平台上所有注册的服务,要求通讯平台应答
int BFDREBCLIENT_API_DPBC(void *pClass,int hook, int serial,DREB_D_NODEINFO dest, const char *data, int datalen,int cZip)
{
	if (pClass == NULL)
	{
		return -3;
	}
	CDrebClient *cliapi = (CDrebClient *)pClass;
	return cliapi->DPBC(hook,serial,dest,data,datalen,cZip);
}

// 函数名: BFDREBCLIENT_API_DPPost
// 编程  : 王明松 2009-12-2 15:24:28
// 返回  : int =100成功 >0且<100dreb返回错 <0发送失败 =0未收到确认消息
// 参数  : void *pClass    客户端类指针
// 参数  : int hook
// 参数  : int serial
// 参数  : DREB_D_NODEINFO dest
// 参数  : const char *data  对应cZip的数据(明文加密或压缩)
// 参数  : int datalen 对应cZip的数据长度
// 参数  : int cZip
// 描述  : 通讯平台投递，要求接收到的通讯平台应答
int BFDREBCLIENT_API_DPPost(void *pClass,int hook, int serial, DREB_D_NODEINFO dest, const char *data, int datalen,int cZip)
{
	if (pClass == NULL)
	{
		return -3;
	}
	CDrebClient *cliapi = (CDrebClient *)pClass;
	return cliapi->DPPost(hook,serial,dest,data,datalen,cZip);
}

// 函数名: BFDREBCLIENT_API_DPPush
// 编程  : 王明松 2009-12-2 15:24:13
// 返回  : int <0发送失败，>=发送的数据长度
// 参数  : void *pClass    客户端类指针
// 参数  : int hook
// 参数  : int serial
// 参数  : DREB_D_NODEINFO dest
// 参数  : const char *data  对应cZip的数据(明文加密或压缩)
// 参数  : int datalen 对应cZip的数据长度
// 参数  : int cZip
// 描述  : 通讯平台推送，无须应答
int BFDREBCLIENT_API_DPPush(void *pClass,int hook, int serial, DREB_D_NODEINFO dest, const char *data, int datalen,int cZip)
{
	if (pClass == NULL)
	{
		return -3;
	}
	CDrebClient *cliapi = (CDrebClient *)pClass;
	return cliapi->DPPush(hook,serial,dest,data,datalen,cZip);
}

// 函数名: BFDREBCLIENT_API_DPACall
// 编程  : 王明松 2009-12-2 15:24:06
// 返回  : int =100成功 >0且<100dreb返回错 <0发送失败 =0未收到确认消息
// 参数  : void *pClass    客户端类指针
// 参数  : int hook
// 参数  : int serial
// 参数  : DREB_D_NODEINFO dest
// 参数  : const char *data  对应cZip的数据(明文加密或压缩)
// 参数  : int datalen 对应cZip的数据长度
// 参数  : int cZip
// 描述  : 通讯平台异步调用 要求接收到的通讯平台发送至服务后确认应答
int BFDREBCLIENT_API_DPACall(void *pClass,int hook, int serial, DREB_D_NODEINFO dest, const char *data, int datalen,int cZip)
{
	if (pClass == NULL)
	{
		return -3;
	}
	CDrebClient *cliapi = (CDrebClient *)pClass;
	return cliapi->DPACall(hook,serial,dest,data,datalen,cZip);
}

// 函数名: BFDREBCLIENT_API_DPCall
// 编程  : 王明松 2009-12-2 15:23:40
// 返回  : int >0<DREBHEADLEN通讯平台返回错 >DREBHEADLEN收到的数据长度 =0在指定时间内未收到数据  <0失败 -100未连接 -1连接断开 -2通讯平台应答数据不符 -3CRC错误
// 参数  : void *pClass    客户端类指针
// 参数  : int hook
// 参数  : int serial
// 参数  : DREB_D_NODEINFO dest
// 参数  : const char *data  对应cZip的数据(明文加密或压缩)
// 参数  : int datalen 对应cZip的数据长度
// 参数  : int cZip
// 参数  : DREB_HEAD &head
// 参数  : char *ansdata   对应head->cZip解开后的明文数据,自定义除外
// 描述  : 通讯平台同步调用 要求最终处理完成后应答
int BFDREBCLIENT_API_DPCall(void *pClass,int hook,int serial,DREB_D_NODEINFO dest, const char *data, int datalen,int cZip,DREB_HEAD &head,char *ansdata)
{
	if (pClass == NULL)
	{
		return -3;
	}
	CDrebClient *cliapi = (CDrebClient *)pClass;
	return cliapi->DPCall(hook,serial,dest,data,datalen,cZip,head,ansdata);
}

// 函数名: BFDREBCLIENT_API_IsConnect
// 编程  : 王明松 2009-12-2 15:13:30
// 返回  : int    1连接  0未连接 
// 参数  : void *pClass    客户端类指针
// 描述  : 是否连接
int BFDREBCLIENT_API_IsConnect(void *pClass)
{
	if (pClass == NULL)
	{
		return -3;
	}
	CDrebClient *cliapi = (CDrebClient *)pClass;
	if (cliapi->IsConnect())
	{
		return 1;
	}
	return 0;
}

// 函数名: BFDREBCLIENT_API_ReceveData
// 编程  : 王明松 2009-11-26 16:16:19
// 返回  : int >0实际收到的数据长度 =0在指定时间内未收到数据  <0失败 -100未连接 -1连接断开 -2通讯平台应答数据不符 -3CRC错误
// 参数  : void *pClass    客户端类指针
// 参数  : DREB_HEAD &head  通讯平台通讯头 
// 参数  : char *data       接收到的数据 对应head->cZip解开后的明文数据,长度为head.nLen,自定义除外
// 参数  : int timeout      接收数据超时时间
// 描述  : 接收数据，注意返回值可能和head.nLen不等，head.nLen是解开后的明文数据
int BFDREBCLIENT_API_ReceveData(void *pClass,DREB_HEAD &head ,unsigned char *data,unsigned int &datalen,int timeout)
{
	if (pClass == NULL)
	{
		return -3;
	}
	CDrebClient *cliapi = (CDrebClient *)pClass;
	return cliapi->ReceveData(head,data,datalen,timeout);
}


// 函数名: BFDREBCLIENT_API_Close
// 编程  : 王明松 2009-11-26 16:16:10
// 返回  : void 
// 参数  : void *pClass    客户端类指针
// 描述  : 关闭连接
void BFDREBCLIENT_API_Close(void *pClass)
{
	if (pClass == NULL)
	{
		return ;
	}
	CDrebClient *cliapi = (CDrebClient *)pClass;
	cliapi->Close();
}

// 函数名: BFDREBCLIENT_API_Connect
// 编程  : 王明松 2009-11-26 16:15:58
// 返回  : int   0成功 其它不成功 
// 参数  : void *pClass    客户端类指针
// 描述  : 连接通讯平台
int BFDREBCLIENT_API_Connect(void *pClass)
{
	if (pClass == NULL)
	{
		return -3;
	}
	CDrebClient *cliapi = (CDrebClient *)pClass;
	if (cliapi->Connect())
	{
		return 0;
	}
	return -1;
}

// 函数名: BFDREBCLIENT_API_Init
// 编程  : 王明松 2009-11-26 14:45:45
// 返回  : int  0成功  其它失败
// 参数  : void *pClass    客户端类指针
// 参数  : const  char *ip 要连接的通讯平台ip
// 参数  : short port 要连接的通讯平台端口
// 参数  : int   timeout 接收超时
// 描述  : 设置连接的通讯平台参数
int BFDREBCLIENT_API_Init(void *pClass,const  char *ip,short port,int timeout)
{
	if (pClass == NULL)
	{
		return -3;
	}
	CDrebClient *cliapi = (CDrebClient *)pClass;
	if (cliapi->Init(ip,port,timeout))
	{
		return 0;
	}
	return -1;
}


// 函数名: BFDREBCLIENT_API_SendData
// 编程  : 王明松 2009-11-26 16:16:28
// 返回  : int <0发送失败 -1发送出错 -2数据太大 >0 为datalen的值
// 参数  : void *pClass    客户端类指针
// 参数  : int hook    客户端使用的字段，通讯平台不用，原样返回。
// 参数  : int serial  异步时填写的流水，通讯平台不用，原样返回
// 参数  : int cmd   命令字 CMD_DPCALL CMD_DPACALL CMD_DPBC CMD_DPABC CMD_DPPUSH CMD_DPPOST
// 参数  : DREB_D_NODEINFO dest  目标信息  
// 参数  : char *data  数据
// 参数  : int datalen 数据长度
// 参数  : int cZip 0不做操作 13DES加密,密码16位长度 2压缩 3DES加密,密码8位长度 4DES加密并压缩
// 描述  : 发送数据
int BFDREBCLIENT_API_SendData(void *pClass,int hook,int serial,int gateindex,int cmd,DREB_D_NODEINFO dest,const char *data, int datalen,int cZip,int nextno)
{
	if (pClass == NULL)
	{
		return -3;
	}
	CDrebClient *cliapi = (CDrebClient *)pClass;
	return cliapi->SendData(hook,serial,gateindex,cmd,dest,data,datalen,cZip,nextno);
}


// 函数名: BFDREBCLIENT_API_NewApi
// 编程  : 王明松 2013-12-4 14:20:53
// 返回  : int 0成功 <0失败
// 参数  : void **pClass  返回的总线连接类指针
// 描述  : 初始化api，得到连接类指针
int BFDREBCLIENT_API_NewApi(void **pClass)
{
	*pClass = new CDrebClient();
	if (*pClass == NULL)
	{
		return -1;
	}
	return 0;

}
void BFDREBCLIENT_API_FreeApi(void *pClass)
{
	if (pClass != NULL)
	{
		delete pClass;
		pClass = NULL;
	}
	return ;
}