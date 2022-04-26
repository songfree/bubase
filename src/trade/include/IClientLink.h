// IClientLink.h: interface for the CIClientLink class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ICLIENTLINK_H__F212B770_97AA_433C_80A0_483518F5F65D__INCLUDED_)
#define AFX_ICLIENTLINK_H__F212B770_97AA_433C_80A0_483518F5F65D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <functional>
#include "BpcHead.h"
//客户端连接接口类，主程序继承实现，供业务模块调用


typedef struct 
{
	char         src;        //0总线    1scoket 
	unsigned int index;      //连接索引  
	unsigned int nWantlen;       //要发送或接收数据长度  不变
	unsigned int nSendlen;       //已发送数据长度   发送改变
	unsigned int rTime;          //接收数据的时间点
	PBPCCOMMSTRU pData; //数据
}S_TRADE_DATA;


typedef  int (*CallBackFunc)(S_TRADE_DATA &reqdata, S_TRADE_DATA &ansdata);	  //注意要释放ansdata里面的pdata，和业务模块一样释放 reqdata也可以释放，但要注意reqdata.pdata要置为NULL，否则框架会释放

class CIClientLink  
{
public:
	CIClientLink()
	{

	}
	virtual ~CIClientLink()
	{

	}

public:
	// 函数名: Send2Client
	// 编程  : 王明松 2017-7-20 9:28:49
	// 返回  : virtual int 
	// 参数  : S_TRADE_DATA &data  src=1时data.pData->sBpcHead.nIndex为socket连接序号
	// 描述  : 发送给客户端数据  数据格式为总线头+数据  src=0表示总线  src=1表示socket直连
	virtual int Send2Client(S_TRADE_DATA &data)=0;

    // 函数名: AnsData
    // 编程  : 王明松 2017-7-20 9:28:49
    // 返回  : virtual int 
    // 参数  : S_TRADE_DATA &data  src=1时data.pData->sBpcHead.nIndex为socket连接序号
	// 参数  : char nextflag  0无后续包  1 有后续包  10最后一个数据包
    // 描述  : 应答数据  数据格式为总线头+数据  src=0表示总线  src=1表示socket直连  如果要求每个应答报文严格有序，要求，填pData->sDBHead.NEXT_INFO里的2个值
	virtual int AnsData(S_TRADE_DATA& data,char nextflag) = 0;

    // 函数名: RpcRequest
    // 编程  : 王明松 2017-7-20 9:28:49
    // 返回  : virtual int 
    // 参数  : S_TRADE_DATA &data  src=1时data.pData->sBpcHead.nIndex为socket连接序号
    // 参数  : char nextflag  0无后续包  1 有后续包  10最后一个数据包
    // 描述  : 应答数据  数据格式为总线头+数据  src=0表示总线  src=1表示socket直连
    virtual int RpcRequest(S_TRADE_DATA& data, int requestid, std::function<int(S_TRADE_DATA& reqdata, S_TRADE_DATA& ansdata)> func) = 0;

	// 函数名: PushRcvQueue
	// 编程  : 王明松 2017-8-14 9:37:13
	// 返回  : virtual int 
	// 参数  : S_TRADE_DATA &data
	// 描述  : 将请求或推送消息放入接收队列，供各处理线程取出处理
	virtual int PushRcvQueue(S_TRADE_DATA &data)=0;


	// 函数名: PushAnsQueue
	// 编程  : 王明松 2017-8-14 9:38:13
	// 返回  : virtual int 
	// 参数  : S_TRADE_DATA &data
	// 描述  : 将应答放入各处理线程的应答队列
	virtual int PushAnsQueue(S_TRADE_DATA &data)=0;


	// 函数名: GetAnsData
	// 编程  : 王明松 2017-8-14 14:00:51
	// 返回  : virtual int 
	// 参数  : int index  线程的索引，每个线程一个应答队列，每个线程同步等待取应答。
	// 参数  : S_TRADE_DATA &data
	// 参数  : int tms  超时ms
	// 描述  : 取应答
	virtual int GetAnsData(int index,S_TRADE_DATA &data,int tms)=0;


	// 函数名:  PoolMalloc
	// 编程  : 王明松 2018-4-13 15:50:23
	// 返回  : virtual int 
	// 参数  : S_TRADE_DATA &data
	// 描述  : 分配空间
	virtual int  PoolMalloc(S_TRADE_DATA &data)=0;

	// 函数名: PoolFree
	// 编程  : 王明松 2018-4-13 15:50:27
	// 返回  : virtual void 
	// 参数  : S_TRADE_DATA &data
	// 描述  : 释放空间
	virtual void PoolFree(S_TRADE_DATA &data)=0;

	//取递增的流水号
	virtual int  GetSerial()=0;
	//取递增的本地号 报单号
	virtual UINT64_ GetLocalNo()=0;

};

#endif // !defined(AFX_ICLIENTLINK_H__F212B770_97AA_433C_80A0_483518F5F65D__INCLUDED_)
