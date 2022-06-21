// GateLink.h: interface for the CGateLink class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GATELINK_H__3EB6AE6D_984A_42CE_8D97_DE65D0C9C3FE__INCLUDED_)
#define AFX_GATELINK_H__3EB6AE6D_984A_42CE_8D97_DE65D0C9C3FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "BF_Thread.h"
#include "GateHead.h"
#include "BF_Mutex.h"
#include "DrebEndian.h"
#include "BF_Xml.h"
#include "ICertApiBase.h"
#include "BF_Tools.h"
#include "PoolData.h"
#include "ApiSocket.h"
#include "GlobalResource.h"

#ifdef _WINDOWS
#ifdef _DEBUG
#pragma comment(lib, "bf_dbpubd.lib") 
#pragma comment(lib, "bf_kerneld.lib") 
#pragma message("Automatically linking with bf_dbpubd.lib bf_kerneld.lib ")
#else
#pragma comment(lib, "bf_kernel.lib") 
#pragma comment(lib, "bf_dbpub.lib") 
#pragma message("Automatically linking with bf_dbpub.lib bf_kernel.lib ")
#endif

#endif

// <?xml version="1.0" encoding="GB2312"?>
// <网关API>
// <日志>
// <日志级别 remark="日志跟踪级别0严重错误1重要错误2一般错误3警告4提示警告5调试信息">5</日志级别>
// <日志文件名 remark="记录日志信息的文件名,前加日期">cgateapi.log</日志文件名>
// <日志目录 remark="存放日志的目录"></日志目录>
// </日志>
// <网关  心跳时间="30" remark="通讯时发送心跳的间隔 秒">
// <gate1  ip="" port="" remark="网关服务器的IP及端口" />
// <gate2  ip="" port="" remark="网关服务器的IP及端口" />
// </网关>
// <证书 use="0" dllpath="/home/songfree/lib" remark="是否使用证书及证书动态库所在的目录">
// <shcaapi.dll  method="getInstance"  remark="证书功能动态库,method为实例化方法名" />
// </证书>
// </网关API>
#ifdef CGATEAPI_EXPORTS
#define CGATEAPI_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define CGATEAPI_EXPORT __declspec(dllimport)
#else
#define CGATEAPI_EXPORT 
#endif
#endif

class CGATEAPI_EXPORT CGateLink : public CBF_Thread  
{
public:
	void SetDatLogName(const char *name);

	// 函数名: SendLogin
	// 编程  : 王明松 2013-5-23 15:12:27
	// 返回  : int -1未连接  -2发送出错  =0 超时未收到应答 大于等于1收到应答 大于1为应答队列里有值为-1
	// 参数  : PCGATE_COMMSTRU data   请求应答数据
	// 参数  : unsigned int timeout    接收应答超时时间 <=0直接返回0，不接收应答
	// 描述  : 发送请求并接收应答
	int SendLogin(PCGATE_COMMSTRU data,unsigned int timeout);

	// 函数名: SendSms
	// 编程  : 王明松 2013-5-23 15:12:27
	// 返回  : int -1未连接  -2发送出错  =0 超时未收到应答 大于等于1收到应答 大于1为应答队列里有值为-1
	// 参数  : PCGATE_COMMSTRU data   请求应答数据
	// 参数  : unsigned int timeout    接收应答超时时间 <=0直接返回0，不接收应答
	// 描述  : 发送请求并接收应答
	int SendSms(PCGATE_COMMSTRU data,unsigned int timeout);

	// 函数名: GetConnectStatus
	// 编程  : 王明松 2013-5-30 17:36:08
	// 返回  : 0未连接  1已连接   2正在连接
	// 描述  : 取连接状态  前端调用此方法时，如果连接状态为2时，需循环sleep并检查是否连接好，否则直接调用connectgate会关闭当前连接
	int GetConnectStatus();

	// 函数名: ConnectGate
	// 编程  : 王明松 2013-5-29 15:27:27
	// 返回  : int 0未连接 1连接成功  -1连接失败
	// 描述  : 连接网关 关闭当前连接(如果连上的话)，重连网关
	int ConnectGate();

	// 函数名: Logout
	// 编程  : 王明松 2013-5-23 15:20:54
	// 返回  : void 
	// 描述  : 关闭连接,退出线程
	void Logout();

	// 函数名: InitApi
	// 编程  : 王明松 2013-5-23 14:49:22
	// 返回  : bool true成功
	// 参数  : const char *apixml xml配置文件
	// 描述  : 初始化api，读取配置文件，得到证书目录及网关地址端口
	bool InitApi(const char *apixml);

	// 函数名: SendReqData
	// 编程  : 王明松 2013-5-23 15:12:27
	// 返回  : int -1未连接  -2发送出错  =0 超时未收到应答 大于等于1收到应答 大于1为应答队列里有值为-1
	// 参数  : PCGATE_COMMSTRU data   请求应答数据  data->head.stDest.nSerial为指定序号
	// 参数  : unsigned int timeout    接收应答超时时间 <=0直接返回0，不接收应答
	// 描述  : 发送请求并接收应答
	int SendReqData(PCGATE_COMMSTRU data,unsigned int timeout);

	// 函数名: GetReqData
	// 编程  : 王明松 2013-5-23 15:12:42
	// 返回  : int <0出错 =0超时无请求  >1成功取到交易请求  剩余请求数为返回-1
	// 参数  : PCGATE_COMMSTRU data   请求数据
	// 参数  : int timeout 超时时间秒
	// 描述  : 取推送或广播请求
	int GetReqData(PCGATE_COMMSTRU data,int timeout);

	// 函数名: GetAnsData
	// 编程  : 王明松 2013-5-23 15:14:47
	// 返回  : int <0出错 =0超时无请求  1成功取到交易请求
	// 参数  : PCGATE_COMMSTRU data   请求数据  data->head.stDest.nSerial >0为指定序号的应答 =0为取应答
	// 参数  : int timeout 超时时间秒
	// 描述  : 取得指定流水的应答
	int GetAnsData(PCGATE_COMMSTRU data,int timeout);


	// 函数名: Subscribe
	// 编程  : 王明松 2014-10-21 15:29:00
	// 返回  : int -1未连接  -2发送出错  =0 超时未收到应答 大于等于1收到应答 大于1为应答队列里有值
	// 参数  : PCGATE_COMMSTRU data
	// 参数  : unsigned int timeout <=0直接返回0，不接收应答
	// 描述  : 向网关发送订阅请求，注意数据是一个结构，里面使用LE 主机序
	int	Subscribe(PCGATE_COMMSTRU data,unsigned int timeout);

	int Subscribe(int flag,std::vector<int> &keylist);

	CGateLink();
	virtual ~CGateLink();

    //消息处理回调，用户可继承实现，否则就调用 GetReqData和GetAnsData
	virtual void ProcessMsg(CGATE_COMMSTRU& data);


	CGlobalResource m_pRes;
private:

	

	// 函数名: ExitThreadInstance
	// 编程  : 王明松 2013-5-27 10:01:19
	// 返回  : virtual void 
	// 描述  : 退出线程
	virtual void ExitThreadInstance();

	// 函数名: InitThreadInstance
	// 编程  : 王明松 2013-5-27 10:01:23
	// 返回  : virtual bool 
	// 描述  : 初始线程
	virtual bool InitThreadInstance();

	
	// 函数名: Run
	// 编程  : 王明松 2013-5-23 15:26:51
	// 返回  : virtual int 
	// 描述  : 线程主方法
	virtual int Run();

	CApiSocket      m_pApiSocket;
	

	int m_nIsConnect; //连接状态 0未连接  1连接成功   2正在连接

	char          m_sErrMsg[256];

	unsigned short m_nSerial;//客户端发送的流水

	CBF_Mutex     m_mutex;

	bool          m_bIsRunThread;

	fd_set m_rset;
	fd_set m_wset;
#if defined(_WINDOWS)
	fd_set m_eset;
#endif
	
	

protected:
	

	void OnRecv();

	// 函数名: OnNoEvent
	// 编程  : 王明松 2013-5-30 14:48:37
	// 返回  : void 
	// 描述  : 没有事件，检查连接读时间，发送心跳及超时断开
	void OnNoEvent();


	// 函数名: OnEvent
	// 编程  : 王明松 2013-5-30 14:47:19
	// 返回  : void 
	// 描述  : 有事件，进行读写检查
	void OnEvent();


	// 函数名: OnPing
	// 编程  : 王明松 2013-5-30 14:47:51
	// 返回  : void 
	// 描述  : 发送ping
	void OnPing();
};


extern "C"
{	
	// 函数名: ConnectGate
	// 编程  : 王明松 2013-5-29 15:27:27
	// 返回  : int  1成功 0为连接 2正在连接
	// 描述  : 连接网关
	CGATEAPI_EXPORT	int BFCGATE_GetConnectStatus();

	// 函数名: ConnectGate
	// 编程  : 王明松 2013-5-29 15:27:27
	// 返回  : int  0未连接   1连接成功  -1连接未成功  
	// 描述  : 连接网关
	CGATEAPI_EXPORT	int BFCGATE_ConnectGate();
	
	// 函数名: Logout
	// 编程  : 王明松 2013-5-23 15:20:54
	// 返回  : void 
	// 描述  : 关闭边接,退出线程
	CGATEAPI_EXPORT	void BFCGATE_Logout();
	
	// 函数名: InitApi
	// 编程  : 王明松 2013-5-23 14:49:22
	// 返回  : int 1成功
	// 参数  : const char *apixml xml配置文件
	// 描述  : 初始化api，读取配置文件，得到证书目录及网关地址端口
	CGATEAPI_EXPORT	int BFCGATE_InitApi(const char *apixml);
	
	
	// 函数名: SendReqData
	// 编程  : 王明松 2013-5-23 15:12:27
	// 返回  : int -1未连接  -2发送出错  =0 超时未收到应答 大于等于1收到应答 大于1为应答队列里有值为-1
	// 参数  : PCGATE_COMMSTRU data   请求应答数据
	// 参数  : unsigned int timeout    接收应答超时时间
	// 描述  : 发送请求并接收应答
	CGATEAPI_EXPORT int BFCGATE_SendReqData(void *data,unsigned int timeout);
	

	// 函数名: BFCGATE_Subscribe
	// 编程  : 王明松 2014-10-21 15:51:02
	// 返回  : int  -1未连接  -2发送出错  =0 超时未收到应答 >=1收到应答 队列里的值为此值减1
	// 参数  : void *pClass 网关连接类指针
	// 参数  : void *data 请求应答数据
	// 参数  : unsigned int timeout 接收应答超时时间
	// 描述  : 发给网关订阅请求
	CGATEAPI_EXPORT int BFCGATE_Subscribe(void *data,unsigned int timeout);

	// 函数名: GetReqData
	// 编程  : 王明松 2013-5-23 15:12:42
	// 返回  : int <0出错 =0超时无请求  >=1成功取到交易请求 广播队列里的值为此值减1
	// 参数  : PCGATE_COMMSTRU data   请求数据
	// 参数  : unsigned int timeout 超时时间秒
	// 描述  : 取推送或广播请求
	CGATEAPI_EXPORT int BFCGATE_GetReqData(void *data,unsigned int timeout);
	
	// 函数名: GetAnsData
	// 编程  : 王明松 2013-5-23 15:14:47
	// 返回  : int <0出错 =0超时无请求  >1成功取到交易请求 队列里的值为此值减1
	// 参数  : PCGATE_COMMSTRU data   请求数据 data->head.stDest.nSerial >0为指定序号的应答 =0为取应答
	// 参数  : unsigned int timeout 超时时间秒
	// 描述  : 取得指定流水的应答
	CGATEAPI_EXPORT int BFCGATE_GetAnsData(void *data,unsigned int timeout);
	
	// 函数名: SendLogin
	// 编程  : 王明松 2013-5-23 15:12:27
	// 返回  : int -1未连接  -2发送出错  =0 超时未收到应答 >=1收到应答 队列里的值为此值减1
	// 参数  : PCGATE_COMMSTRU data   请求应答数据
	// 参数  : unsigned int timeout    接收应答超时时间
	// 描述  : 发送请求并接收应答
	CGATEAPI_EXPORT int BFCGATE_SendLogin(void *data,unsigned int timeout);
	
	// 函数名: SendSms
	// 编程  : 王明松 2013-5-23 15:12:27
	// 返回  : int -1未连接  -2发送出错  =0 超时未收到应答 >=1收到应答 队列里的值为此值减1
	// 参数  : PCGATE_COMMSTRU data   请求应答数据
	// 参数  : unsigned int timeout    接收应答超时时间
	// 描述  : 发送请求并接收应答
	CGATEAPI_EXPORT int BFCGATE_SendSms(void *data,unsigned int timeout);

	// 函数名: BFCGATE_TS_GetConnectStatus
	// 编程  : 王明松 2013-12-4 14:19:35
	// 返回  : int 
	// 参数  : void *pClass  网关连接类指针
	// 描述  : 取得连接网关状态
	CGATEAPI_EXPORT	int BFCGATE_TS_GetConnectStatus(void *pClass);
	
	// 函数名: BFCGATE_TS_ConnectGate
	// 编程  : 王明松 2013-12-4 14:20:06
	// 返回  : int 0成功
	// 参数  : void *pClass 网关连接类指针
	// 描述  : 连接网关
	CGATEAPI_EXPORT	int BFCGATE_TS_ConnectGate(void *pClass);
	
	// 函数名: BFCGATE_TS_Logout
	// 编程  : 王明松 2013-12-4 14:20:32
	// 返回  : void 
	// 参数  : void *pClass 网关连接类指针
	// 描述  : 关闭连接,退出线程，释放指针空间
	CGATEAPI_EXPORT	void BFCGATE_TS_Logout(void *pClass);
	
	// 函数名: BFCGATE_TS_InitApi
	// 编程  : 王明松 2013-12-4 14:20:53
	// 返回  : int 1成功
	// 参数  : const char *apixml
	// 参数  : void **pClass  返回的网关连接类指针
	// 描述  : 初始化api，读取配置文件，得到证书目录及网关地址端口
	CGATEAPI_EXPORT	int BFCGATE_TS_InitApi(const char *apixml,void **pClass);
	
	
	// 函数名: BFCGATE_TS_SendReqData
	// 编程  : 王明松 2013-5-23 15:12:27
	// 返回  : int -1未连接  -2发送出错  =0 超时未收到应答 >=1收到应答 队列里的值为此值减1
	// 参数  : void *pClass  网关连接类指针
	// 参数  : PCGATE_COMMSTRU data   请求应答数据
	// 参数  : unsigned int timeout    接收应答超时时间
	// 描述  : 发送请求并接收应答
	CGATEAPI_EXPORT int BFCGATE_TS_SendReqData(void *pClass,void *data,unsigned int timeout);

	// 函数名: BFCGATE_TS_Subscribe
	// 编程  : 王明松 2014-10-21 15:51:02
	// 返回  : int  -1未连接  -2发送出错  =0 超时未收到应答 >=1收到应答 队列里的值为此值减1
	// 参数  : void *pClass 网关连接类指针
	// 参数  : void *data 请求应答数据
	// 参数  : unsigned int timeout 接收应答超时时间
	// 描述  : 发给网关订阅请求
	CGATEAPI_EXPORT int BFCGATE_TS_Subscribe(void *pClass,void *data,unsigned int timeout);

	// 函数名: BFCGATE_TS_SendLogin
	// 编程  : 王明松 2013-5-23 15:12:27
	// 返回  : int -1未连接  -2发送出错  =0 超时未收到应答 >=1收到应答 队列里的值为此值减1
	// 参数  : void *pClass          网关连接类指针
	// 参数  : PCGATE_COMMSTRU *data            请求应答数据
	// 参数  : unsigned int timeout    接收应答超时时间
	// 描述  : 发送请求并接收应答
	CGATEAPI_EXPORT int BFCGATE_TS_SendLogin(void *pClass,void *data,unsigned int timeout);
	
	// 函数名: BFCGATE_TS_SendSms
	// 编程  : 王明松 2013-5-23 15:12:27
	// 返回  : int -1未连接  -2发送出错  =0 超时未收到应答 >=1收到应答 队列里的值为此值减1
	// 参数  : void *pClass          网关连接类指针
	// 参数  : PCGATE_COMMSTRU data             请求应答数据
	// 参数  : unsigned int timeout    接收应答超时时间
	// 描述  : 发送请求并接收应答
	CGATEAPI_EXPORT int BFCGATE_TS_SendSms(void *pClass,void *data,unsigned int timeout);

	// 函数名: BFCGATE_TS_GetAnsData
	// 编程  : 王明松 2013-5-23 15:14:47
	// 返回  : int <0出错 =0超时无请求  >=1成功取到交易请求 队列里的值为此值减1
	// 参数  : void *pClass          网关连接类指针
	// 参数  : PCGATE_COMMSTRU data   请求数据 data->head.stDest.nSerial >0为指定序号的应答 =0为取应答
	// 参数  : int timeout 超时时间秒
	// 描述  : 取得指定流水的应答
	CGATEAPI_EXPORT int BFCGATE_TS_GetAnsData(void *pClass,void *data,int timeout);

	// 函数名: BFCGATE_TS_GetReqData
	// 编程  : 王明松 2013-5-23 15:12:42
	// 返回  : int <0出错 =0超时无请求  >=1成功取到交易请求 队列里的值为此值减1
	// 参数  : void *pClass          网关连接类指针
	// 参数  : PCGATE_COMMSTRU data   请求数据
	// 参数  : int timeout 超时时间秒
	// 描述  : 取推送或广播请求
	CGATEAPI_EXPORT int BFCGATE_TS_GetReqData(void *pClass,void *data,int timeout);
	
}


#endif // !defined(AFX_GATELINK_H__3EB6AE6D_984A_42CE_8D97_DE65D0C9C3FE__INCLUDED_)
