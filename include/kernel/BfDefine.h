// BuDefine.h: interface for the PubDefine .
// 通讯定义
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BFDEFINE_H__F2DFC8C6_AAAD_4A6C_BAEC_249903098AF2__INCLUDED_)
#define AFX_BFDEFINE_H__F2DFC8C6_AAAD_4A6C_BAEC_249903098AF2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "BF_SocketTcp.h"




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
	

#define BFBUFSIZE  8192  //前置的最大缓冲大小
#define MAXBUNUM   100   //最大bu处理数
#define BEATHEARTTICK  5  //心跳间隔

#define SOCK_UNKNOW   0      //未知
#define SOCK_CLI      2      //客户端
#define SOCK_BU       3      //BU客户端
#define SOCK_LISTEN	  7      //侦听端口

#define MSG_REQ       0      //接收请求
#define MSG_FREE      1      //BU发送空闲请求
#define MSG_BUANS     2      //BU应答
#define MSG_BUANSFREE 3      //BU应答并空闲请求
#define MSG_GETSERIAL 4      //BU取共享唯一流水


typedef struct 
{
	char   buffer[BFBUFSIZE];  
}__PACKED__ COMMSTRU_FPC,*PCOMMSTRU_FPC; //发送数据结构

typedef struct 
{
	short msgtype;             //消息类型
	unsigned short index;      //连接的序号，标识此请求从哪个连接上过来的
	unsigned short buindex;    //bu连接的序号
	int connecttime;           //发送要使用的socket的时间戳，在发送时和原连接进行比较，若不同表示连接已断并重用，取消发送
	int rtime;                 //请求入队列时间
	unsigned int len;        // 本消息后续数据包长度，不包括本段数据长度
}__PACKED__ S_MSG_HEAD_FPC;
typedef struct 
{
	S_MSG_HEAD_FPC head;	
	PCOMMSTRU_FPC  message;         //消息内容
}__PACKED__ S_MSG_BUF_FPC;
typedef struct 
{
	unsigned int nWantlen;       //要发送数据长度
	unsigned int nSendlen;       //已发送数据长度
	S_MSG_BUF_FPC msgbuf;	
}__PACKED__ S_BF_RSMSG_FPC;      //通讯平台内部发送接收数据的结构



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


#endif // !defined(AFX_BFDEFINE_H__F2DFC8C6_AAAD_4A6C_BAEC_249903098AF2__INCLUDED_)
