// DrebClient_Java.h: interface for the CDrebClient_Java class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DREBCLIENT_JAVA_H__3A4F4813_2022_4BF3_AB23_0CB8ED0AE339__INCLUDED_)
#define AFX_DREBCLIENT_JAVA_H__3A4F4813_2022_4BF3_AB23_0CB8ED0AE339__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DrebPubDefine.h"
#include "DrebEndian.h"
#include "MdbBase.h"
#include "DrebClient.h"

#ifdef DREBAPI_JAVA_EXPORTS
#define DREBAPI_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define DREBAPI_EXPORT __declspec(dllimport)
#else
#define DREBAPI_EXPORT 
#endif
#endif

#define    MAXINSTANCE_NUM    120  //最大200个实例
#define    INSTANCENUM_ERR    121  //出错返回码
#define    INSTANCENEW_ERR    122  //NEW类出错
#define    INSTANCEINIT_ERR   123  //类init出错

//java程序采用网络序，传入字节数组，JAVA程序不用考虑字节序问题，由本类进行判断是否需要转换

class DREBAPI_EXPORT CDrebClient_Java
{
public:

	// 函数名: GetErrMsg
	// 编程  : 王明松 2016-9-6 16:18:50
	// 返回  : char  0失败 1成功 
	// 参数  : char index 实例对应的索引
	// 参数  : char *errmsg 出错信息
	// 描述  : 取得出错信息
	char GetErrMsg(char index,char *errmsg);

	// 函数名: SetPara
	// 编程  : 王明松 2016-9-6 10:20:41
	// 返回  : void 
	// 参数  : char index 实例对应的索引
	// 参数  : char *ip   ip地址
	// 参数  : char *port      4字节的整型   为了处理方便 java统一使用bytes字节数组传入整型值
	// 参数  : char *timeout   4字节的整型   为了处理方便 java统一使用bytes字节数组传入整型值
	// 描述  : 设置通讯参数
	void SetPara(char index,char *ip,char *port ,char *timeoutms);

	// 函数名: SetCallerEndian
	// 编程  : 王明松 2016-9-6 9:49:37
	// 返回  : void 
	// 参数  : char bigendian  1网络序:JAVA C/C++(hp_ux aix)   2主机序 C/C++(windows/linux)
	// 描述  : 设置调用者字节序   默认为1网络序
	void SetCallerEndian(char bigendian);

	// 函数名: RecvData
	// 编程  : 王明松 2016-9-6 9:12:55
	// 返回  : char 0在指定时间内未收到数据  100成功 102接收失败 101未连接 103接收长度超限 1连接断开 2通讯平台应答数据不符 3CRC错误
	// 参数  : char index 实例对应的索引
	// 参数  : char *drebheaddata   对应总线的COMMSTRU结构
	// 参数  : char *timeoutms  接收超时 毫秒
	// 描述  : 接收应答
	char RecvData(char index,char *drebheaddata,char *timeoutms);

	// 函数名: SendData
	// 编程  : 王明松 2016-9-6 9:11:29
	// 返回  : char 0失败  1发送出错 2数据太大  100发送成功
	// 参数  : char index 实例对应的索引
	// 参数  : char *drebheaddata  对应总线的COMMSTRU结构  
	// 描述  : 发送请求，注意是同步阻塞连接，发送成功后确定要收应答就要马上调用RecvData
	char SendData(char index,char *drebheaddata);

	// 函数名: GetDrebHead
	// 编程  : 王明松 2016-9-5 16:23:25
	// 返回  : char  0失败  1成功
	// 参数  : char index   实例对应的索引
	// 参数  : char *head   DREB_HEAD的结构内容，已转为网络序 注意head空间要大于等于DREBHEADLEN
	// 描述  : 取得连接上总线的报文头信息
	char GetDrebHead(char index,char *head);

	// 函数名: IsConnect
	// 编程  : 王明松 2016-9-5 16:10:46
	// 返回  : char 0没连上，1成功连上
	// 参数  : char index 实例对应的索引
	// 描述  : 是否连接上总线
	char IsConnect(char index);

	// 函数名: Close
	// 编程  : 王明松 2016-9-5 15:44:09
	// 返回  : void 
	// 参数  : char index 实例对应的索引
	// 描述  : 关闭总线连接
	void Close(char index);

	// 函数名: Connect
	// 编程  : 王明松 2016-9-5 15:42:57
	// 返回  : char 0失败 1成功
	// 参数  : char index 实例对应的索引
	// 描述  : 连接总线
	char Connect(char index);

	// 函数名: FreeInstance
	// 编程  : 王明松 2016-9-5 15:19:56
	// 返回  : void 
	// 参数  : char index  实例对应的索引
	// 描述  : 释放一个客户端实例
	void FreeInstance(char index);

	// 函数名: NewInstance
	// 编程  : 王明松 2016-9-5 14:56:53
	// 返回  : char  返回的为实例对应的索引，每次调用要传入此索引  大于120为失败，即同时只能有120个实例。
	// 参数  : char *ip   ip地址
	// 参数  : char *port      4字节的整型   为了处理方便 java统一使用bytes字节数组传入整型值
	// 参数  : char *timeout   4字节的整型   为了处理方便 java统一使用bytes字节数组传入整型值
	// 描述  : 生成一个新的客户端实例  
	char NewInstance(char *ip,char *port,char *timeout);

	

	CDrebClient_Java();
	virtual ~CDrebClient_Java();

	

protected:
// 	typedef struct 
// 	{
// 		CDrebClient *client;
// 		int         id;
// 	}S_DREB_CLIENT;
	CDrebEndian  m_pEndian;//字节序转换
	CInstanceList<CDrebClient> m_pInstanceList;  //保存java程序new生成的实例，delete由java程序主动调用

};

//如下是针对上述类提供给java调用的c方法
extern "C"
{	
	// 函数名: GetErrMsg
	// 编程  : 王明松 2016-9-6 16:18:50
	// 返回  : char  0失败 1成功 
	// 参数  : char index 实例对应的索引
	// 参数  : char *errmsg 出错信息
	// 描述  : 取得出错信息
	DREBAPI_EXPORT char BFDREB_CLIENT_GetErrMsg(char index,char *errmsg);

	// 函数名: SetCallerEndian
	// 编程  : 王明松 2016-9-6 9:49:37
	// 返回  : void 
	// 参数  : char bigendian  1网络序:JAVA C/C++(hp_ux aix)   2主机序 C/C++(windows/linux)
	// 描述  : 设置调用者字节序   默认为1网络序
	DREBAPI_EXPORT	void BFDREB_CLIENT_SetCallerEndian(char bigendian);
	
	// 函数名: RecvData
	// 编程  : 王明松 2016-9-6 9:12:55
	// 返回  : char 0在指定时间内未收到数据  100成功 102接收失败 101未连接 103接收长度超限 1连接断开 2通讯平台应答数据不符 3CRC错误
	// 参数  : char index 实例对应的索引
	// 参数  : char *drebheaddata   对应总线的COMMSTRU结构
	// 参数  : char *timeoutms  接收超时 毫秒
	// 描述  : 接收应答
	DREBAPI_EXPORT char BFDREB_CLIENT_RecvData(char index,char *drebheaddata,char *timeoutms);
	
	// 函数名: SendData
	// 编程  : 王明松 2016-9-6 9:11:29
	// 返回  : char 0失败  1发送出错 2数据太大  100发送成功
	// 参数  : char index 实例对应的索引
	// 参数  : char *drebheaddata  对应总线的COMMSTRU结构  
	// 描述  : 发送请求，注意是同步阻塞连接，发送成功后确定要收应答就要马上调用RecvData
	DREBAPI_EXPORT char BFDREB_CLIENT_SendData(char index,char *drebheaddata);
	
	// 函数名: GetDrebHead
	// 编程  : 王明松 2016-9-5 16:23:25
	// 返回  : char  0失败  1成功
	// 参数  : char index   实例对应的索引
	// 参数  : char *head   DREB_HEAD的结构内容，已转为网络序 注意head空间要大于等于DREBHEADLEN
	// 描述  : 取得连接上总线的报文头信息
	DREBAPI_EXPORT char BFDREB_CLIENT_GetDrebHead(char index,char *head);
	
	// 函数名: IsConnect
	// 编程  : 王明松 2016-9-5 16:10:46
	// 返回  : char 0没连上，1成功连上
	// 参数  : char index 实例对应的索引
	// 描述  : 是否连接上总线
	DREBAPI_EXPORT char BFDREB_CLIENT_IsConnect(char index);
	
	// 函数名: Close
	// 编程  : 王明松 2016-9-5 15:44:09
	// 返回  : void 
	// 参数  : char index 实例对应的索引
	// 描述  : 关闭总线连接
	DREBAPI_EXPORT void BFDREB_CLIENT_Close(char index);
	
	// 函数名: Connect
	// 编程  : 王明松 2016-9-5 15:42:57
	// 返回  : char 0失败 1成功
	// 参数  : char index 实例对应的索引
	// 描述  : 连接总线
	DREBAPI_EXPORT char BFDREB_CLIENT_Connect(char index);
	
	// 函数名: FreeInstance
	// 编程  : 王明松 2016-9-5 15:19:56
	// 返回  : void 
	// 参数  : char index  实例对应的索引
	// 描述  : 释放一个客户端实例
	DREBAPI_EXPORT void BFDREB_CLIENT_FreeInstance(char index);
	
	// 函数名: NewInstance
	// 编程  : 王明松 2016-9-5 14:56:53
	// 返回  : char  返回的为实例对应的索引，每次调用要传入此索引  大于120为失败，即同时只能有120个实例。
	// 参数  : char *ip   ip地址
	// 参数  : char *port      4字节的整型   为了处理方便 java统一使用bytes字节数组传入整型值
	// 参数  : char *timeout   4字节的整型   为了处理方便 java统一使用bytes字节数组传入整型值
	// 描述  : 生成一个新的客户端实例  
	DREBAPI_EXPORT char BFDREB_CLIENT_NewInstance(char *ip,char *port,char *timeout);

	// 函数名: SetPara
	// 编程  : 王明松 2016-9-6 10:20:41
	// 返回  : void 
	// 参数  : char index 实例对应的索引
	// 参数  : char *ip   ip地址
	// 参数  : char *port      4字节的整型   为了处理方便 java统一使用bytes字节数组传入整型值
	// 参数  : char *timeout   4字节的整型   为了处理方便 java统一使用bytes字节数组传入整型值
	// 描述  : 设置通讯参数
	DREBAPI_EXPORT void BFDREB_CLIENT_SetPara(char index,char *ip,char *port ,char *timeoutms);

}



#endif // !defined(AFX_DREBCLIENT_JAVA_H__3A4F4813_2022_4BF3_AB23_0CB8ED0AE339__INCLUDED_)
