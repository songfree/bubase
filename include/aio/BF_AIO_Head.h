// BF_AIO_Head.h: interface for the CBF_AIO_Head class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BF_AIO_HEAD_H__EECCCB23_9E01_470F_8989_C9DB81E473EA__INCLUDED_)
#define AFX_BF_AIO_HEAD_H__EECCCB23_9E01_470F_8989_C9DB81E473EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "public.h"
#include "BF_Socket.h"
#include "BF_Mutex.h"
#include "DrebPubDefine.h"


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

#ifndef _WINDOWS
typedef struct  
{
	char  ol;
// 	unsigned long Internal;
// 	unsigned long InternalHigh;
// 	union 
// 	{
// 		struct 
// 		{
// 			unsigned long Offset;
// 			unsigned long OffsetHigh;
// 		} DUMMYSTRUCTNAME;
// 		void * Pointer;
// 	} DUMMYUNIONNAME;
// 	void *  hEvent;
} __PACKED__ OVERLAPPED;  //为IOCP特定义
#endif


typedef struct 
{
	unsigned  int    s_nIndex;                        //要发送数据的连接索引
	unsigned  int    s_nTimestamp;                    //要发送数据的连接的连接时间戳
	unsigned  int    s_nSendNum;                      //发送次数
	unsigned  int    s_nSendLen;                      //已发送的数据长度
	unsigned  int    s_nHook;                          //发送数据的标识
	unsigned  int    s_nTotalLen;                     //发送数据总长度
	unsigned  int    s_nTime;                         //入队列时间  
	unsigned  int    s_nTxCode;                       //交易码
	int              nQuoCode;                        //行情代码
	int              nQuoSerial;                      //行情序号
	int              nQuoDate;                        //行情日期
	int              s_serial;                        //总线流水
	int              s_nRet;                          //发送返回标识  -1不成功 >=0发送的数据长度
	char             s_sSendBuffer[DREBBUFSIZE];   //发送数据缓冲
}__PACKED__ SOCKET_SEND_DATA,*PSOCKET_SEND_DATA;

typedef std::list<SOCKET_SEND_DATA> SOCKSENDQUEUE; //对应每个连接的发送队列


#define  EPOLLMAXFD     100000  //最大的连接数

typedef struct  _SOCKET_POOL_DATA
{
	OVERLAPPED         s_iocp;               //IOCP重叠结构
	SOCKET_HANDLE      s_hSocket;            //SOCKET连接
	SOCKADDR_IN        s_pSocketAddr;        //对方的地址或连接过来的地址
	unsigned  int      s_nIndex;             //索引  应用不能改变   
	unsigned  int      s_nTimestamp;         //连接时间戳(秒) 应用发送数据时要和此进行比较，如果不相等则表明连接不是原连接
	unsigned  int      s_nReadTime;          //最后读到数据时间(秒)  AIO设置
	unsigned  int      s_nWriteTime;         //最后发送数据时间(秒)  AIO设置
	PSOCKET_SEND_DATA  s_pSendData;          //IOCP用的发送指针
	SOCKSENDQUEUE      s_pSendQueue;         //发送队列 epoll和select使用，先发送，不成功则放入队列 
	SOCKSENDQUEUE      s_pQuoteSendQueue;    //行情发送队列
	int                s_nSendQueueFlag;     //部分发送队列标志    0无部分发送  1发送队列有部分发送  2行情队列有部分发送
	CBF_Mutex          s_mutex;              //发送队列互斥量，可能会多线程发送，epoll和select用
	char  *            s_pIoBuffer;          //IO操作缓冲指针  指明IO操作存放地的指针  s_pIoBuffer=s_sRealBuffer+s_nRealBufferLen
	unsigned  int      s_nIoBufferLen;       //iocp操作收到的数据长度
	char               s_sRealBuffer[DREBBUFSIZE];  //实际的数据缓冲，收下来后由应用取走后重置s_nRealBufferLen来再次接收
	unsigned  int      s_nRealBufferLen;     //收到放在实际的数据缓冲中的数据长度
	_SOCKET_POOL_DATA*  s_pEpollPoint;          //EPOOL发送时用到的指针

	char               s_cOperType;          // IOCP操作类型
#define     OP_NONE       0   //什么都不用做        连接未使用时的类型
#define     OP_READ       1   //读                  连接正常时的操作
#define     OP_WRITE      2   //写
#define     OP_ACCEPT     3   //接收连接
#define     OP_CONNECT    4   //检查读写并检查连接是否成功
#define     OP_LISTEN     5   //侦听操作
	char               s_cSocketType;        //SOCKET连接类型   由应用具体定义 

#ifndef SOCK_UNKNOW 
#define     SOCK_UNKNOW     0                //未知 
#endif
#define     SOCK_CLIENT     1                //普通客户端
#define     SOCK_SERVER     2                //注册的服务端
#define     SOCK_TONODE     3                //连接到别的节点连接
#define     SOCK_FROMNODE   4                //从别的节点连过来的，对应的为SOCK_TONODE
#define     SOCK_TOVC       5                //虚拟通道 主动连接出去的VC
#define     SOCK_FROMVC     6                //虚拟通道 别的节点连过来的	
#define     SOCK_LISTEN     7                //侦听端口
#define     SOCK_ACCEPT     8                //刚连接过来的，还没有经过认证的连接,认证后改为其它类型，此为中间状态
	char               s_cSocketStatus;      //连接状态 AIO EPOLL及select模型根据此状态进行检查 IOCP不用  
#define     SOCKSTATUS_RW         0                //0可读可写
#define     SOCKSTATUS_R          1                //1需要检查读
#define     SOCKSTATUS_W          2                //2需要检查写
#define     SOCKSTATUS_C          3                //3需要检写连接是否正常
#define     SOCKSTATUS_UNKNOW     4                //4什么都不用做
	char             s_cHBFlag;            //心跳标志 0无心跳   1针对接收发心跳  2针对发送发心跳
#define     HEARTBEAT_NONE        0                //0无心跳
#define     HEARTBEAT_RECV        1                //1针对接收发心跳
#define     HEARTBEAT_SEND        2                //2针对发送发心跳
	char             s_cCheckFlag;                 //针对socket认证检查的字段，应用定义
	unsigned  int    s_nHBTime;            //心跳时间(秒)  当s_cHBFlag为1时此连接在此时间没有接收数据就发心跳 当s_cHBFlag为2时此连接在此时间没有发送数据就发心跳  
	unsigned  int    s_nHBCloseTime;       //(秒)当此连接发送心跳后，在此时间内没有收到这个连接的数据就关闭
	unsigned  int    s_nCloseTime;         //(秒)当此连接accept上来后，在此时间内没有收到这个连接的数据就关闭
	unsigned  int    s_nPingTime;          //最后发送PING数据时间(秒) 控制不要重复发心跳
	
	unsigned  int    s_nVer;               //连接版本  由应用具体定义
	unsigned  int    s_nDestIndex;         //目的索引  当s_nSocketType为SOCK_TOVC或SOCK_FROMVC时，相当于两个连接是直通的，这里要填直通的连接索引
	char             s_sSessionKey[49];    //会话密钥
	char             s_sSignString[49];    //要签名的字符串
	unsigned short   s_nSignPubKeyLen;     //签名公钥长度
	char             s_sSignPubKey[2049];  //签名公钥
	unsigned short   s_nEncryptPubKeyLen;  //加密公钥长度
	char             s_sEncryptPubKey[2049]; //加密公钥
	void  *          ptr;                  //自定义的结构指针，方便扩展
}SOCKET_POOL_DATA,*PSOCKET_POOL_DATA;



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

#endif // !defined(AFX_BF_AIO_HEAD_H__EECCCB23_9E01_470F_8989_C9DB81E473EA__INCLUDED_)
