// DrebApi.h: interface for the CDrebApi class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DREBAPI_H__E74BA6E7_E5A3_4C93_8FE1_DACDB0AB6D28__INCLUDED_)
#define AFX_DREBAPI_H__E74BA6E7_E5A3_4C93_8FE1_DACDB0AB6D28__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



extern "C"
{	
	
	// 函数名: BFDREBAPI_InitApi
	// 编程  : 王明松 2013-12-4 14:20:53
	// 返回  : int 0成功 <0失败
	// 参数  : const char *apixml
	// 参数  : void **pClass  返回的总线连接类指针
	// 描述  : 初始化api，读取配置文件，得到证书目录及网关地址端口
	int BFDREBAPI_InitApi(const char *apixml,void **pClass);
	

	// 函数名: BFDREBAPI_FREE
	// 编程  : 王明松 2015-7-23 10:07:49
	// 返回  : void 
	// 参数  : void *pClass
	// 描述  : 程序退出释放资源
	void BFDREBAPI_Free(void *pClass);
	
	// 函数名: BFDREBAPI_StartApi
	// 编程  : 王明松 2015-7-21 11:41:58
	// 返回  : int 0成功 <0失败
	// 参数  : void *pClass
	// 描述  : 启动API
	int BFDREBAPI_StartApi(void *pClass);
	
	
	// 函数名: BFDREBAPI_StopApi
	// 编程  : 王明松 2015-7-21 11:42:20
	// 返回  : void 
	// 参数  : void *pClass
	// 描述  : 停止API
	void BFDREBAPI_StopApi(void *pClass);
	
	
	// 函数名: BFDREBAPI_MonitorApi
	// 编程  : 王明松 2015-7-21 11:42:29
	// 返回  : void 
	// 参数  : void *pClass
	// 描述  : 监控API，若线程停止则启动
	void BFDREBAPI_MonitorApi(void *pClass);
	
	
	// 函数名: int BFDREBAPI_SendMsg
	// 编程  : 王明松 2015-7-21 11:42:49
	// 返回  : int  0成功  >0 放入队列再发  <0失败   
	// 参数  : void *pClass  初始化时返回的指针
	// 参数  : const char *data   数据为 DREBSAPI_COMMSTRU的结构
	// 描述  : 向总线上发送消息
	 int BFDREBAPI_SendMsg(void *pClass,const char *data);
	
	
	// 函数名: int BFDREBAPI_GetMsg
	// 编程  : 王明松 2015-7-21 11:43:51
	// 返回  : int    0无数据要处理 1有数据   <0出错了
	// 参数  : void *pClass 初始化时返回的指针
	// 参数  : char *data  数据为 DREBSAPI_COMMSTRU的结构
	// 描述  : 从总线API队列里取出一条消息
	int BFDREBAPI_GetMsg(void *pClass, char *data);
	
	
	// 函数名: BFDREBAPI_GetErrMsg
	// 编程  : 王明松 2015-7-21 11:46:01
	// 返回  : int
	// 参数  : void *pClass 初始化时返回的指针
	// 参数  : char *errmsg
	// 描述  : 取得出错信息
	 int BFDREBAPI_GetErrMsg(void *pClass,char *errmsg);
	
	
	// 函数名: BFDREBAPI_UnRegisterDreb
	// 编程  : 王明松 2015-7-21 11:46:18
	// 返回  : void 
	// 参数  : void *pClass
	// 描述  : 取消在总线上注册的交易
	 void BFDREBAPI_UnRegisterDreb(void *pClass);
	
	
	// 函数名: BFDREBAPI_RegisterDreb
	// 编程  : 王明松 2015-7-21 11:46:30
	// 返回  : 
	// 参数  : void *pClass    初始化时返回的指针
	// 参数  : int index       在注册的总线连接索引
	// 参数  : char *txlist    以|分隔的交易码，形如  1|2|3|4|  
	// 描述  : 在总线上注册交易
	 void BFDREBAPI_RegisterDreb(void *pClass,int index,char *txlist);

     // 函数名: BFDREBAPI_UnSubscribe
    // 编程  : 王明松 2022-06-14 14:20:18
    // 返回  : void 
    // 参数  : void *pClass
    // 描述  : 取消在总线上的广播订阅
     void BFDREBAPI_UnSubscribe(void* pClass);


     // 函数名: BFDREBAPI_Subscribe
     // 编程  : 王明松 2022-06-14 14:20:25
     // 返回  : 
     // 参数  : void *pClass    初始化时返回的指针
     // 参数  : int index       在注册的总线连接索引
     // 参数  : char *txlist    以|分隔的交易码，形如  1|2|3|4|  
     // 描述  : 在总线上订阅广播消息
     void BFDREBAPI_Subscribe(void* pClass, int index, char* txlist);
	
}


#endif // !defined(AFX_DREBAPI_H__E74BA6E7_E5A3_4C93_8FE1_DACDB0AB6D28__INCLUDED_)
