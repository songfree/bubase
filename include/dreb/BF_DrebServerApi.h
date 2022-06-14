// BF_DrebServerApi.h: interface for the CBF_DrebServerApi class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BF_DREBSERVERAPI_H__C49125EC_277E_43F5_8EAA_7AE1400C4DE1__INCLUDED_)
#define AFX_BF_DREBSERVERAPI_H__C49125EC_277E_43F5_8EAA_7AE1400C4DE1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BF_DrebServer.h"
#include "BF_DrebResource.h"
#include "IErrLog.h"



#ifdef __cplusplus
extern "C" {
#endif
	
	//字节对齐
#ifdef _WINDOWS
#define __PACKED__ 
#pragma pack(push,1)
#else
#ifdef __GNUC__
#define __PACKED__	__attribute__ ((packed))
	//#pragma pack(push,1)
#else
#define __PACKED__
#pragma options align=packed
#endif
#endif
	

	typedef struct 
	{
		char          cMsgType;        //消息类型  从总线发过来的消息类型，发送时不用。
		unsigned int  index;           //从dreb总线过来的连接索引或指定要发送数据的连接索引(大于连接的总线数顺序来选择)
		DREB_HEAD     sDBHead;         //dreb头
		char   sBuffer[BPUDATASIZE];  
	}__PACKED__ DREBSAPI_COMMSTRU,*PDREBSAPI_COMMSTRU; //服务API报文

	//取消字节对齐
#ifdef _WINDOWS
#pragma pack(pop)
#else
#ifdef __GNUC__
	//#pragma pack(pop)
#else
#pragma options align=reset
#endif
#endif
#undef __PACKED__
	
#ifdef __cplusplus
}
#endif
#ifdef DREBAPI_EXPORTS
#define DREBAPI_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define DREBAPI_EXPORT __declspec(dllimport)
#else
#define DREBAPI_EXPORT 
#endif
#endif

class DREBAPI_EXPORT CBF_DrebServerApi  
{
public:
    // 函数名: Subscribe
    // 编程  : 王明松 2022-06-14 14:16:47
    // 返回  : void 
    // 参数  : int index
    // 参数  : std::vector<int> &funclist
    // 描述  : 在总线上订阅广播消息
    void Subscribe(int index, std::vector<int>& funclist);

    // 函数名: UnSubscribe
    // 编程  : 王明松 2022-06-14 14:16:49
    // 返回  : void 
    // 描述  : 取消在总线上的广播订阅
    void UnSubscribe();

	bool OnMsgMonitor(S_BPC_RSMSG &rcvdata);

	// 函数名: RegisterDreb
	// 编程  : 王明松 2015-7-21 11:08:47
	// 返回  : void 
	// 参数  : int index
	// 参数  : std::vector<int> &funclist
	// 描述  : 在总线上注册交易，一般在连接返回成功消息后注册
	void RegisterDreb(int index,std::vector<int> &funclist);

	// 函数名: UnRegisterDreb
	// 编程  : 王明松 2015-7-21 11:06:44
	// 返回  : void 
	// 描述  : 取消在总线上的交易注册
	void UnRegisterDreb();

	// 函数名: GetMsg
	// 编程  : 王明松 2015-7-21 10:08:48
	// 返回  : int 
	// 参数  : S_BPC_RSMSG *gmsg
	// 描述  : 从总线队列取消息
	int GetMsg(DREBSAPI_COMMSTRU *gmsg);

	// 函数名: SendMsg
	// 编程  : 王明松 2015-7-21 10:09:11
	// 返回  : int 
	// 参数  : S_BPC_RSMSG smsg
	// 描述  : 发送消息至总线  a_cNodePrivateId来标识是否转移交易，当总线收到判断此标识为100时，不对s_info进行重置
	int SendMsg(DREBSAPI_COMMSTRU smsg);


	// 函数名: MonitorApi
	// 编程  : 王明松 2015-7-21 10:09:28
	// 返回  : void 
	// 描述  : 监控总线API线程
	void MonitorApi();

	// 函数名: StopApi
	// 编程  : 王明松 2015-7-21 10:09:50
	// 返回  : void 
	// 描述  : 停止api线程
	void StopApi();

	// 函数名: StartApi
	// 编程  : 王明松 2015-7-21 10:10:04
	// 返回  : bool 
	// 描述  : 启动api线程
	bool StartApi();

	// 函数名: Init
	// 编程  : 王明松 2015-7-21 9:55:28
	// 返回  : bool 
	// 参数  : const char *conxmlfile
	// 参数  :,CDrebMsgProcBase* spi=NULL   当有回调时，api线程直接回调，否则放入队列
	// 描述  : 初始化API
	bool Init(const char *conxmlfile, CDrebMsgProcBase* spi=NULL);

	CBF_DrebServerApi();
	virtual ~CBF_DrebServerApi();


	CBF_DrebServer     m_pDrebApi;
	CBF_DrebResource   m_pRes;

	CDrebEndian   m_pDrebEndian;

	CIErrlog           *m_pLog;

	char               m_errMsg[256];

protected:
	void Upload(S_BPC_RSMSG &rcvdata);
	void Download(S_BPC_RSMSG &rcvdata);
	bool MonitorReport(S_BPC_RSMSG &rcvdata);
	bool MonitorBpu(S_BPC_RSMSG &rcvdata);
	bool MonitorHost(S_BPC_RSMSG &rcvdata);
	bool MonitorInfo(S_BPC_RSMSG &rcvdata);

	CBF_SimpleLog      log;
	
};




extern "C"
{	
	
	// 函数名: BFDREBAPI_InitApi
	// 编程  : 王明松 2013-12-4 14:20:53
	// 返回  : int 0成功 <0失败
	// 参数  : const char *apixml
	// 参数  : void **pClass  返回的总线连接类指针
	// 描述  : 初始化api，读取配置文件，得到证书目录及网关地址端口
	DREBAPI_EXPORT	int BFDREBAPI_InitApi(const char *apixml,void **pClass);
	
	// 函数名: BFDREBAPI_FREE
	// 编程  : 王明松 2015-7-23 10:07:49
	// 返回  : void 
	// 参数  : void *pClass
	// 描述  : 程序退出释放资源
	DREBAPI_EXPORT void BFDREBAPI_Free(void *pClass);

	// 函数名: BFDREBAPI_StartApi
	// 编程  : 王明松 2015-7-21 11:41:58
	// 返回  : int 0成功 <0失败
	// 参数  : void *pClass
	// 描述  : 启动API
	DREBAPI_EXPORT	int BFDREBAPI_StartApi(void *pClass);


	// 函数名: BFDREBAPI_StopApi
	// 编程  : 王明松 2015-7-21 11:42:20
	// 返回  : void 
	// 参数  : void *pClass
	// 描述  : 停止API
	DREBAPI_EXPORT	void BFDREBAPI_StopApi(void *pClass);


	// 函数名: BFDREBAPI_MonitorApi
	// 编程  : 王明松 2015-7-21 11:42:29
	// 返回  : void 
	// 参数  : void *pClass
	// 描述  : 监控API，若线程停止则启动
	DREBAPI_EXPORT	void BFDREBAPI_MonitorApi(void *pClass);


	// 函数名: int BFDREBAPI_SendMsg
	// 编程  : 王明松 2015-7-21 11:42:49
	// 返回  : int  0成功  >0 放入队列再发  <0失败   
	// 参数  : void *pClass  初始化时返回的指针
	// 参数  : const char *data   数据为 DREBSAPI_COMMSTRU的结构
	// 描述  : 向总线上发送消息  a_cNodePrivateId来标识是否转移交易，当总线收到判断此标识为100时，不对s_info进行重置
	DREBAPI_EXPORT  int BFDREBAPI_SendMsg(void *pClass,const char *data);


	// 函数名: int BFDREBAPI_GetMsg
	// 编程  : 王明松 2015-7-21 11:43:51
	// 返回  : int    0无数据要处理 1有数据   <0出错了
	// 参数  : void *pClass 初始化时返回的指针
	// 参数  : char *data  数据为 DREBSAPI_COMMSTRU的结构
	// 描述  : 从总线API队列里取出一条消息
	DREBAPI_EXPORT  int BFDREBAPI_GetMsg(void *pClass, char *data);


	// 函数名: BFDREBAPI_GetErrMsg
	// 编程  : 王明松 2015-7-21 11:46:01
	// 返回  : int 
	// 参数  : void *pClass 初始化时返回的指针
	// 参数  : char *errmsg 
	// 描述  : 取得出错信息
	DREBAPI_EXPORT  int BFDREBAPI_GetErrMsg(void *pClass,char *errmsg);


	// 函数名: BFDREBAPI_UnRegisterDreb
	// 编程  : 王明松 2015-7-21 11:46:18
	// 返回  : void 
	// 参数  : void *pClass
	// 描述  : 取消在总线上注册的交易
	DREBAPI_EXPORT  void BFDREBAPI_UnRegisterDreb(void *pClass);


	// 函数名: BFDREBAPI_RegisterDreb
	// 编程  : 王明松 2015-7-21 11:46:30
	// 返回  : 
	// 参数  : void *pClass    初始化时返回的指针
	// 参数  : int index       在注册的总线连接索引
	// 参数  : char *txlist    以|分隔的交易码，形如  1|2|3|4|  
	// 描述  : 在总线上注册交易
	DREBAPI_EXPORT  void BFDREBAPI_RegisterDreb(void *pClass,int index,char *txlist);

    // 函数名: BFDREBAPI_UnSubscribe
	// 编程  : 王明松 2022-06-14 14:38:30
    // 返回  : void 
    // 参数  : void *pClass
    // 描述  : 取消在总线上的广播订阅
    DREBAPI_EXPORT  void BFDREBAPI_UnSubscribe(void* pClass);


    // 函数名: BFDREBAPI_Subscribe
    // 编程  : 王明松 2022-06-14 14:38:30
    // 返回  : 
    // 参数  : void *pClass    初始化时返回的指针
    // 参数  : int index       在注册的总线连接索引
    // 参数  : char *txlist    以|分隔的交易码，形如  1|2|3|4|  
    // 描述  : 在总线上订阅广播
    DREBAPI_EXPORT  void BFDREBAPI_Subscribe(void* pClass, int index, char* txlist);

}


#endif // !defined(AFX_BF_DREBSERVERAPI_H__C49125EC_277E_43F5_8EAA_7AE1400C4DE1__INCLUDED_)
