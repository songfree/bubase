// GateApi.h: interface for the CGateApi class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GATEAPI_H__38F43A80_B734_4386_A0E1_1A8E395F79EC__INCLUDED_)
#define AFX_GATEAPI_H__38F43A80_B734_4386_A0E1_1A8E395F79EC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

extern "C"
{	
	// 函数名: ConnectGate
	// 编程  : 王明松 2013-5-29 15:27:27
	// 返回  : int  1成功 0 未连接  2正在连接
	// 描述  : 连接网关
	int BFCGATE_GetConnectStatus();
	
	// 函数名: ConnectGate
	// 编程  : 王明松 2013-5-29 15:27:27
	// 返回  : int  1成功  -1连接失败  
	// 描述  : 连接网关
	int BFCGATE_ConnectGate();
	
	// 函数名: Logout
	// 编程  : 王明松 2013-5-23 15:20:54
	// 返回  : void 
	// 描述  : 关闭连接,退出线程
	void BFCGATE_Logout();
	
	// 函数名: InitApi
	// 编程  : 王明松 2013-5-23 14:49:22
	// 返回  : int 1成功
	// 参数  : const char *apixml xml配置文件
	// 描述  : 初始化api，读取配置文件，得到证书目录及网关地址端口
	int BFCGATE_InitApi(const char *apixml);
	
	
	// 函数名: SendReqData
	// 编程  : 王明松 2013-5-23 15:12:27
	// 返回  : int -1未连接  -2发送出错  =0 超时未收到应答 >=1收到应答 队列里的值为此值减1
	// 参数  : PCGATE_COMMSTRU data   请求应答数据
	// 参数  : unsigned int timeout    接收应答超时时间 <=0直接返回0，不接收应答
	// 描述  : 发送请求并接收应答
	int BFCGATE_SendReqData(void *data,unsigned int timeout);
	
		// 函数名: BFCGATE_Subscribe
	// 编程  : 王明松 2014-10-21 15:51:02
	// 返回  : int  -1未连接  -2发送出错  =0 超时未收到应答 >=1收到应答 队列里的值为此值减1
	// 参数  : void *pClass 网关连接类指针
	// 参数  : void *data 请求应答数据
	// 参数  : unsigned int timeout 接收应答超时时间 <=0直接返回0，不接收应答
	// 描述  : 发给网关订阅请求
	int BFCGATE_Subscribe(void *data,unsigned int timeout);
	
	// 函数名: GetReqData
	// 编程  : 王明松 2013-5-23 15:12:42
	// 返回  : int <0出错 =0超时无请求  >=1成功取到交易请求 队列里的值为此值减1
	// 参数  : PCGATE_COMMSTRU data   请求数据
	// 参数  : int timeout 超时时间秒
	// 描述  : 取推送或广播请求
	int BFCGATE_GetReqData(void *data,int timeout);
	
	// 函数名: GetAnsData
	// 编程  : 王明松 2013-5-23 15:14:47
	// 返回  : int <0出错 =0超时无请求  >1成功取到交易请求 队列里的值为此值减1
	// 参数  : PCGATE_COMMSTRU data   请求数据
	// 参数  : int timeout 超时时间秒 
	// 描述  : 取得指定流水的应答
	int BFCGATE_GetAnsData(void *data,int timeout);
	
	// 函数名: SendLogin
	// 编程  : 王明松 2013-5-23 15:12:27
	// 返回  : int -1未连接  -2发送出错  =0 超时未收到应答 >1收到应答 队列里的值为此值减1
	// 参数  : PCGATE_COMMSTRU data   请求应答数据
	// 参数  : unsigned int timeout    接收应答超时时间 <=0直接返回0，不接收应答
	// 描述  : 发送请求并接收应答
	int BFCGATE_SendLogin(void *data,unsigned int timeout);
	
	// 函数名: SendSms
	// 编程  : 王明松 2013-5-23 15:12:27
	// 返回  : int -1未连接  -2发送出错  =0 超时未收到应答 >1收到应答 队列里的值为此值减1
	// 参数  : PCGATE_COMMSTRU data   请求应答数据
	// 参数  : unsigned int timeout    接收应答超时时间 <=0直接返回0，不接收应答
	// 描述  : 发送请求并接收应答
	int BFCGATE_SendSms(void *data,unsigned int timeout);
	

	
	// 函数名: BFCGATE_TS_GetConnectStatus
	// 编程  : 王明松 2013-12-4 14:19:35
	// 返回  : int 
	// 参数  : void *pClass  网关连接类指针
	// 描述  : 取得连接网关状态
	int BFCGATE_TS_GetConnectStatus(void *pClass);
	
	// 函数名: BFCGATE_TS_ConnectGate
	// 编程  : 王明松 2013-12-4 14:20:06
	// 返回  : int 0成功
	// 参数  : void *pClass 网关连接类指针
	// 描述  : 连接网关
	int BFCGATE_TS_ConnectGate(void *pClass);
	
	// 函数名: BFCGATE_TS_Logout
	// 编程  : 王明松 2013-12-4 14:20:32
	// 返回  : void 
	// 参数  : void *pClass 网关连接类指针
	// 描述  : 关闭连接,退出线程，释放指针空间
	void BFCGATE_TS_Logout(void *pClass);
	
	// 函数名: BFCGATE_TS_InitApi
	// 编程  : 王明松 2013-12-4 14:20:53
	// 返回  : int 1成功
	// 参数  : const char *apixml
	// 参数  : void **pClass  返回的网关连接类指针
	// 描述  : 初始化api，读取配置文件，得到证书目录及网关地址端口
	int BFCGATE_TS_InitApi(const char *apixml,void **pClass);
	
	
	// 函数名: BFCGATE_TS_SendReqData
	// 编程  : 王明松 2013-5-23 15:12:27
	// 返回  : int -1未连接  -2发送出错  =0 超时未收到应答 >=1收到应答 队列里的值为此值减1
	// 参数  : void *pClass  网关连接类指针
	// 参数  : PCGATE_COMMSTRU data   请求应答数据
	// 参数  : unsigned int timeout    接收应答超时时间 <=0直接返回0，不接收应答
	// 描述  : 发送请求并接收应答
	int BFCGATE_TS_SendReqData(void *pClass,void *data,unsigned int timeout);
	
	// 函数名: BFCGATE_TS_Subscribe
	// 编程  : 王明松 2014-10-21 15:51:02
	// 返回  : int  -1未连接  -2发送出错  =0 超时未收到应答 >=1收到应答 队列里的值为此值减1
	// 参数  : void *pClass 网关连接类指针
	// 参数  : void *data 请求应答数据
	// 参数  : unsigned int timeout 接收应答超时时间 <=0直接返回0，不接收应答
	// 描述  : 发给网关订阅请求
	int BFCGATE_TS_Subscribe(void *pClass,void *data,unsigned int timeout);
	
	// 函数名: BFCGATE_TS_GetReqData
	// 编程  : 王明松 2013-5-23 15:12:42
	// 返回  : int <0出错 =0超时无请求  >=1成功取到交易请求 队列里的值为此值减1
	// 参数  : void *pClass          网关连接类指针
	// 参数  : PCGATE_COMMSTRU data   请求数据
	// 参数  : int timeout 超时时间秒
	// 描述  : 取推送或广播请求
	int BFCGATE_TS_GetReqData(void *pClass,void *data,int timeout);

	// 函数名: BFCGATE_TS_GetAnsData
	// 编程  : 王明松 2013-5-23 15:14:47
	// 返回  : int <0出错 =0超时无请求  >=1成功取到交易请求 队列里的值为此值减1
	// 参数  : void *pClass          网关连接类指针
	// 参数  : PCGATE_COMMSTRU data   请求数据
	// 参数  : int timeout 超时时间秒
	// 描述  : 取得指定流水的应答
	int BFCGATE_TS_GetAnsData(void *pClass,void *data,int timeout);
	
	// 函数名: BFCGATE_TS_SendLogin
	// 编程  : 王明松 2013-5-23 15:12:27
	// 返回  : int -1未连接  -2发送出错  =0 超时未收到应答 >=1收到应答 队列里的值为此值减1
	// 参数  : void *pClass          网关连接类指针
	// 参数  : PCGATE_COMMSTRU *data            请求应答数据
	// 参数  : unsigned int timeout    接收应答超时时间 <=0直接返回0，不接收应答
	// 描述  : 发送请求并接收应答
	int BFCGATE_TS_SendLogin(void *pClass,void *data,unsigned int timeout);
	
	// 函数名: BFCGATE_TS_SendSms
	// 编程  : 王明松 2013-5-23 15:12:27
	// 返回  : int -1未连接  -2发送出错  =0 超时未收到应答 >=1收到应答 队列里的值为此值减1
	// 参数  : void *pClass          网关连接类指针
	// 参数  : PCGATE_COMMSTRU data             请求应答数据
	// 参数  : unsigned int timeout    接收应答超时时间 <=0直接返回0，不接收应答
	// 描述  : 发送请求并接收应答
	int BFCGATE_TS_SendSms(void *pClass,void *data,unsigned int timeout);


	// 函数名: BFCGATE_TS_GetConnectStatus
	// 编程  : 王明松 2013-12-4 14:19:35
	// 返回  : int 
	// 参数  : void *pClass  网关连接类指针
	// 描述  : 取得连接网关状态
	int BFCGATE_TS_GetConnectStatus(void *pClass);
	
	// 函数名: BFCGATE_TS_ConnectGate
	// 编程  : 王明松 2013-12-4 14:20:06
	// 返回  : int 0成功
	// 参数  : void *pClass 网关连接类指针
	// 描述  : 连接网关
	int BFCGATE_TS_ConnectGate(void *pClass);
	
	// 函数名: BFCGATE_TS_Logout
	// 编程  : 王明松 2013-12-4 14:20:32
	// 返回  : void 
	// 参数  : void *pClass 网关连接类指针
	// 描述  : 关闭连接,退出线程，释放指针空间
	void BFCGATE_TS_Logout(void *pClass);
	
	// 函数名: BFCGATE_TS_InitApi
	// 编程  : 王明松 2013-12-4 14:20:53
	// 返回  : int 1成功
	// 参数  : const char *apixml
	// 参数  : void **pClass  返回的网关连接类指针
	// 描述  : 初始化api，读取配置文件，得到证书目录及网关地址端口
	int BFCGATE_TS_InitApi(const char *apixml,void **pClass);
	
	
	// 函数名: BFCGATE_TS_SendReqData
	// 编程  : 王明松 2013-5-23 15:12:27
	// 返回  : int -1未连接  -2发送出错  =0 超时未收到应答 >=1收到应答 队列里的值为此值减1
	// 参数  : void *pClass  网关连接类指针
	// 参数  : PCGATE_COMMSTRU data   请求应答数据
	// 参数  : unsigned int timeout    接收应答超时时间 <=0直接返回0，不接收应答
	// 描述  : 发送请求并接收应答
	int BFCGATE_TS_SendReqData(void *pClass,void *data,unsigned int timeout);
	
	// 函数名: BFCGATE_TS_Subscribe
	// 编程  : 王明松 2014-10-21 15:51:02
	// 返回  : int  -1未连接  -2发送出错  =0 超时未收到应答 >=1收到应答 队列里的值为此值减1
	// 参数  : void *pClass 网关连接类指针
	// 参数  : void *data 请求应答数据
	// 参数  : unsigned int timeout 接收应答超时时间 <=0直接返回0，不接收应答
	// 描述  : 发给网关订阅请求
	int BFCGATE_TS_Subscribe(void *pClass,void *data,unsigned int timeout);
	
	// 函数名: BFCGATE_TS_GetReqData
	// 编程  : 王明松 2013-5-23 15:12:42
	// 返回  : int <0出错 =0超时无请求  >=1成功取到交易请求 队列里的值为此值减1
	// 参数  : void *pClass          网关连接类指针
	// 参数  : PCGATE_COMMSTRU data   请求数据
	// 参数  : int timeout 超时时间秒
	// 描述  : 取推送或广播请求
	int BFCGATE_TS_GetReqData(void *pClass,void *data,int timeout);

	// 函数名: BFCGATE_TS_GetAnsData
	// 编程  : 王明松 2013-5-23 15:14:47
	// 返回  : int <0出错 =0超时无请求  >=1成功取到交易请求 队列里的值为此值减1
	// 参数  : void *pClass          网关连接类指针
	// 参数  : PCGATE_COMMSTRU data   请求数据
	// 参数  : int timeout 超时时间秒
	// 描述  : 取得指定流水的应答
	int BFCGATE_TS_GetAnsData(void *pClass,void *data,int timeout);
	
	// 函数名: BFCGATE_TS_SendLogin
	// 编程  : 王明松 2013-5-23 15:12:27
	// 返回  : int -1未连接  -2发送出错  =0 超时未收到应答 >=1收到应答 队列里的值为此值减1
	// 参数  : void *pClass          网关连接类指针
	// 参数  : PCGATE_COMMSTRU *data            请求应答数据
	// 参数  : unsigned int timeout    接收应答超时时间 <=0直接返回0，不接收应答
	// 描述  : 发送请求并接收应答
	int BFCGATE_TS_SendLogin(void *pClass,void *data,unsigned int timeout);
	
	// 函数名: BFCGATE_TS_SendSms
	// 编程  : 王明松 2013-5-23 15:12:27
	// 返回  : int -1未连接  -2发送出错  =0 超时未收到应答 >=1收到应答 队列里的值为此值减1
	// 参数  : void *pClass          网关连接类指针
	// 参数  : PCGATE_COMMSTRU data             请求应答数据
	// 参数  : unsigned int timeout    接收应答超时时间 <=0直接返回0，不接收应答
	// 描述  : 发送请求并接收应答
	int BFCGATE_TS_SendSms(void *pClass,void *data,unsigned int timeout);



	//给c#等用的方法

    // 函数名: BFCGATE_TSI_GetConnectStatus
    // 编程  : 王明松 2013-12-4 14:19:35
    // 返回  : int 
    // 参数  : unsigned int clientid  网关连接类下标
    // 描述  : 取得连接网关状态
    int BFCGATE_TSI_GetConnectStatus(unsigned int clientid);

    // 函数名: BFCGATE_TSI_ConnectGate
    // 编程  : 王明松 2013-12-4 14:20:06
    // 返回  : int 0成功
    // 参数  : unsigned int clientid  网关连接类下标
    // 描述  : 连接网关
    int BFCGATE_TSI_ConnectGate(unsigned int clientid);

    // 函数名: BFCGATE_TSI_Logout
    // 编程  : 王明松 2013-12-4 14:20:32
    // 返回  : void 
    // 参数  : unsigned int clientid  网关连接类下标
    // 描述  : 关闭连接,退出线程，释放指针空间
    void BFCGATE_TSI_Logout(unsigned int clientid);

    // 函数名: BFCGATE_TSI_InitApi
    // 编程  : 王明松 2013-12-4 14:20:53
    // 返回  : int >=0成功 网关连接类下标  <0失败
    // 参数  : const char *apixml
    // 描述  : 初始化api，读取配置文件，得到证书目录及网关地址端口
    int BFCGATE_TSI_InitApi(const char* apixml);


    // 函数名: BFCGATE_TSI_SendReqData
    // 编程  : 王明松 2013-5-23 15:12:27
    // 返回  : int -1未连接  -2发送出错  =0 超时未收到应答 >=1收到应答 队列里的值为此值减1
    // 参数  : unsigned int clientid  网关连接类下标
    // 参数  : PCGATE_COMMSTRU data   请求应答数据
    // 参数  : unsigned int timeout    接收应答超时时间 <=0直接返回0，不接收应答
    // 描述  : 发送请求并接收应答
    int BFCGATE_TSI_SendReqData(unsigned int clientid, void* data, unsigned int timeout);

    // 函数名: BFCGATE_TSI_Subscribe
    // 编程  : 王明松 2014-10-21 15:51:02
    // 返回  : int  -1未连接  -2发送出错  =0 超时未收到应答 >=1收到应答 队列里的值为此值减1
    // 参数  : unsigned int clientid  网关连接类下标
    // 参数  : void *data 请求应答数据
    // 参数  : unsigned int timeout 接收应答超时时间 <=0直接返回0，不接收应答
    // 描述  : 发给网关订阅请求
    int BFCGATE_TSI_Subscribe(unsigned int clientid, void* data, unsigned int timeout);

    // 函数名: BFCGATE_TSI_GetReqData
    // 编程  : 王明松 2013-5-23 15:12:42
    // 返回  : int <0出错 =0超时无请求  >=1成功取到交易请求 队列里的值为此值减1
    // 参数  : unsigned int clientid  网关连接类下标
    // 参数  : PCGATE_COMMSTRU data   请求数据
    // 参数  : int timeout 超时时间秒
    // 描述  : 取推送或广播请求
    int BFCGATE_TSI_GetReqData(unsigned int clientid, void* data, int timeout);

    // 函数名: BFCGATE_TSI_GetAnsData
    // 编程  : 王明松 2013-5-23 15:14:47
    // 返回  : int <0出错 =0超时无请求  >=1成功取到交易请求 队列里的值为此值减1
    // 参数  : unsigned int clientid  网关连接类下标
    // 参数  : PCGATE_COMMSTRU data   请求数据
    // 参数  : int timeout 超时时间秒
    // 描述  : 取得指定流水的应答
    int BFCGATE_TSI_GetAnsData(unsigned int clientid, void* data, int timeout);

    // 函数名: BFCGATE_TSI_SendLogin
    // 编程  : 王明松 2013-5-23 15:12:27
    // 返回  : int -1未连接  -2发送出错  =0 超时未收到应答 >=1收到应答 队列里的值为此值减1
    // 参数  : unsigned int clientid  网关连接类下标
    // 参数  : PCGATE_COMMSTRU *data            请求应答数据
    // 参数  : unsigned int timeout    接收应答超时时间 <=0直接返回0，不接收应答
    // 描述  : 发送请求并接收应答
    int BFCGATE_TSI_SendLogin(unsigned int clientid, void* data, unsigned int timeout);

    // 函数名: BFCGATE_TSI_SendSms
    // 编程  : 王明松 2013-5-23 15:12:27
    // 返回  : int -1未连接  -2发送出错  =0 超时未收到应答 >=1收到应答 队列里的值为此值减1
    // 参数  : unsigned int clientid  网关连接类下标
    // 参数  : PCGATE_COMMSTRU data             请求应答数据
    // 参数  : unsigned int timeout    接收应答超时时间 <=0直接返回0，不接收应答
    // 描述  : 发送请求并接收应答
    int BFCGATE_TSI_SendSms(unsigned int clientid, void* data, unsigned int timeout);
	
}

#endif // !defined(AFX_GATEAPI_H__38F43A80_B734_4386_A0E1_1A8E395F79EC__INCLUDED_)
