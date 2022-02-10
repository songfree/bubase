#include "GateLink.h"


CGateLink g_pCgateApiClass;

//1连接成功  0未连接   2正在连接
int BFCGATE_GetConnectStatus()
{
	return g_pCgateApiClass.GetConnectStatus();
}

// 函数名: ConnectGate
// 编程  : 王明松 2013-5-29 15:27:27
// 返回  : int  1成功  -1连接失败 
// 描述  : 连接网关
int BFCGATE_ConnectGate()
{
	return g_pCgateApiClass.ConnectGate();
}

// 函数名: Logout
// 编程  : 王明松 2013-5-23 15:20:54
// 返回  : void 
// 描述  : 关闭边接,退出线程
void BFCGATE_Logout()
{
	g_pCgateApiClass.Logout();
	
}

// 函数名: InitApi
// 编程  : 王明松 2013-5-23 14:49:22
// 返回  : int 1成功
// 参数  : const char *apixml xml配置文件
// 描述  : 初始化api，读取配置文件，得到证书目录及网关地址端口
int BFCGATE_InitApi(const char *apixml)
{
	return g_pCgateApiClass.InitApi(apixml);

}

// 函数名: SendReqData
// 编程  : 王明松 2013-5-23 15:12:27
// 返回  : int -1未连接  -2发送出错  =0 超时未收到应答 1收到应答
// 参数  : PCGATE_COMMSTRU data   请求应答数据
// 参数  : unsigned int timeout    接收应答超时时间 <=0直接返回，不接收应答
// 描述  : 发送请求并接收应答
int BFCGATE_SendReqData(void *data,unsigned int timeout)
{
	return g_pCgateApiClass.SendReqData((PCGATE_COMMSTRU)data,timeout);
}

int BFCGATE_Subscribe(void *data,unsigned int timeout)
{
	return g_pCgateApiClass.Subscribe((PCGATE_COMMSTRU)data,timeout);
}

// 函数名: SendLogin
// 编程  : 王明松 2013-5-23 15:12:27
// 返回  : int -1未连接  -2发送出错  =0 超时未收到应答 1收到应答
// 参数  : PCGATE_COMMSTRU data   请求应答数据
// 参数  : unsigned int timeout    接收应答超时时间
// 描述  : 发送请求并接收应答
int BFCGATE_SendLogin(void *data,unsigned int timeout)
{
	return g_pCgateApiClass.SendLogin((PCGATE_COMMSTRU)data,timeout);
}

// 函数名: SendSms
// 编程  : 王明松 2013-5-23 15:12:27
// 返回  : int -1未连接  -2发送出错  =0 超时未收到应答 1收到应答
// 参数  : PCGATE_COMMSTRU data   请求应答数据
// 参数  : unsigned int timeout    接收应答超时时间
// 描述  : 发送短信请求并接收应答
int BFCGATE_SendSms(void *data,unsigned int timeout)
{
	return g_pCgateApiClass.SendSms((PCGATE_COMMSTRU)data,timeout);
}

// 函数名: GetReqData
// 编程  : 王明松 2013-5-23 15:12:42
// 返回  : int <0出错 =0超时无请求  >0请求交易码
// 参数  : char *data   请求数据
// 参数  : int *datalen 输入为data的大小，输出为数据的大小
// 参数  : int *txcode  交易码
// 参数  : int timeout 超时时间秒
// 描述  : 取推送或广播请求
int BFCGATE_GetReqData(void *data,unsigned int timeout)
{
	return g_pCgateApiClass.GetReqData((PCGATE_COMMSTRU)data,timeout);
}

// 函数名: GetAnsData
// 编程  : 王明松 2013-5-23 15:14:47
// 返回  : int <0连接出错 >0取到应答 =0超时无应答
// 参数  : PCGATE_COMMSTRU data   请求应答数据 
// 参数  : int timeout  超时时间秒
// 描述  : 取得指定流水的应答
int BFCGATE_GetAnsData(void *data,unsigned int timeout)
{
	return g_pCgateApiClass.GetAnsData((PCGATE_COMMSTRU)data,timeout);
}




//1成功  0未连接   2正在连接
int BFCGATE_TS_GetConnectStatus(void *pClass)
{
	CGateLink *pGl= (CGateLink *)pClass;
	return pGl->GetConnectStatus();
}

// 函数名: BFCGATE_TS_ConnectGate
// 编程  : 王明松 2013-5-29 15:27:27
// 返回  : int  成功  0未连接   2正在连接 
// 描述  : 连接网关
int BFCGATE_TS_ConnectGate(void *pClass)
{
	CGateLink *pGl= (CGateLink *)pClass;
	return pGl->ConnectGate();
}

// 函数名: BFCGATE_TS_Logout
// 编程  : 王明松 2013-5-23 15:20:54
// 返回  : void 
// 描述  : 关闭边接,退出线程
void BFCGATE_TS_Logout(void *pClass)
{
	CGateLink *pGl= (CGateLink *)pClass;
	pGl->Logout();
	delete pGl;
	pGl = NULL;
}

// 函数名: BFCGATE_TS_InitApi
// 编程  : 王明松 2013-5-23 14:49:22
// 返回  : int 1成功
// 参数  : const char *apixml xml配置文件
// 描述  : 初始化api，读取配置文件，得到证书目录及网关地址端口，并连接网关
int BFCGATE_TS_InitApi(const char *apixml,void **pClass)
{
	*pClass = new CGateLink();
	if (*pClass == NULL)
	{
		return -1;
	}
	return ((CGateLink *)(*pClass))->InitApi(apixml);
}

// 函数名: BFCGATE_TS_SendReqData
// 编程  : 王明松 2013-5-23 15:12:27
// 返回  : int -1未连接  -2发送出错  =0 超时未收到应答 1收到应答
// 参数  : PCGATE_COMMSTRU data   请求数据
// 参数  : unsigned int timeout    接收应答超时时间 <=0直接返回，不接收应答
// 描述  : 发送请求并接收应答
int BFCGATE_TS_SendReqData(void *pClass,void *data,unsigned int timeout)
{
	CGateLink *pGl= (CGateLink *)pClass;
	return pGl->SendReqData((PCGATE_COMMSTRU)data,timeout);
}


int BFCGATE_TS_Subscribe(void *pClass,void *data,unsigned int timeout)
{
	CGateLink *pGl= (CGateLink *)pClass;
	return pGl->Subscribe((PCGATE_COMMSTRU)data,timeout);
}

// 函数名: BFCGATE_TS_SendLogin
// 编程  : 王明松 2013-5-23 15:12:27
// 返回  : int -1未连接  -2发送出错  =0 超时未收到应答 1收到应答
// 参数  : PCGATE_COMMSTRU data   请求应答数据
// 参数  : unsigned int timeout    接收应答超时时间
// 描述  : 发送请求并接收应答
int BFCGATE_TS_SendLogin(void *pClass,void *data,unsigned int timeout)
{
	CGateLink *pGl= (CGateLink *)pClass;
	return pGl->SendLogin((PCGATE_COMMSTRU)data,timeout);
}

// 函数名: BFCGATE_TS_SendSms
// 编程  : 王明松 2013-5-23 15:12:27
// 返回  : int -1未连接  -2发送出错  =0 超时未收到应答 1收到应答
// 参数  : PCGATE_COMMSTRU data   请求应答数据
// 参数  : unsigned int timeout    接收应答超时时间
// 描述  : 发送短信请求并接收应答
int BFCGATE_TS_SendSms(void *pClass,void *data,unsigned int timeout)
{
	CGateLink *pGl= (CGateLink *)pClass;
	return pGl->SendSms((PCGATE_COMMSTRU)data,timeout);
}

// 函数名: BFCGATE_TS_GetReqData
// 编程  : 王明松 2013-5-23 15:12:42
// 返回  : int <0出错 =0超时无请求  >0请求交易码
// 参数  : char *data   请求数据
// 参数  : int *datalen 输入为data的大小，输出为数据的大小
// 参数  : int *txcode  交易码
// 参数  : int timeout 超时时间秒
// 描述  : 取推送或广播请求
int BFCGATE_TS_GetReqData(void *pClass,void *data,int timeout)
{
	CGateLink *pGl= (CGateLink *)pClass;
	return pGl->GetReqData((PCGATE_COMMSTRU)data,timeout);
}

// 函数名: BFCGATE_TS_GetAnsData
// 编程  : 王明松 2013-5-23 15:14:47
// 返回  : int <0连接出错 >0取到应答 =0超时无应答
// 参数  : PCGATE_COMMSTRU data   请求应答数据 
// 参数  : int timeout  超时时间秒
// 描述  : 取得指定流水的应答
int BFCGATE_TS_GetAnsData(void *pClass,void *data,int timeout)
{
	CGateLink *pGl= (CGateLink *)pClass;
	return pGl->GetAnsData((PCGATE_COMMSTRU)data,timeout);
}
