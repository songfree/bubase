// MoniHead.h: interface for the CMoniHead class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MONIHEAD_H__1A490998_AE1D_4016_859A_2C1896E2D51B__INCLUDED_)
#define AFX_MONIHEAD_H__1A490998_AE1D_4016_859A_2C1896E2D51B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

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
	
#define MONI_SOCK_UNKNOW   10      //未知
#define MONI_SOCK_DREB     13      //和数据总线节点的连接
#define MONI_SOCK_LISTEN	 14      //侦听端口


typedef struct 
{
	int             rtime;                 //入队列时间
	unsigned int    sendnum;               //发送次数
	unsigned int    offset;     //发送位移 ，即当前发送的数据位置，主要是发送一部分数据的情况
	unsigned short  sendlen;    //发送数据长度
	unsigned short  totallen;   //总数据长度
	unsigned int    serial;    //dreb流水
	unsigned int    nIndex;    //dreb连接索引
	COMMSTRU       data; 
}__PACKED__ S_GWDREB_RSMSG,*PS_GWDREB_RSMSG; //发送数据结构


typedef struct
{
	std::string sIp;       //IP
	int         nPort;     //端口
	int         nBandwidth;//带宽
}S_DREBLINK_INFO;

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

#endif // !defined(AFX_GATEHEAD_H__1A490998_AE1D_4016_859A_2C1896E2D51B__INCLUDED_)
