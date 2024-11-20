// BF_AIO_WorkBase.h: interface for the CBF_AIO_WorkBase class.
//
//////////////////////////////////////////////////////////////////////

/***********************************************************************************
  编码日志记录
  编码者: 王明松
  编码日期 2012-5-17 16:08:02
  版本: V1.00 
  说明: 异步IO的客户处理基类，当接收数据、连接断开等后会调用本类的方法，实际使用时继承此基类进行实现.
 ***********************************************************************************/


#if !defined(AFX_BF_AIO_WORKBASE_H__1C6E4AF0_8B2C_4C35_A00F_C8293BED35B4__INCLUDED_)
#define AFX_BF_AIO_WORKBASE_H__1C6E4AF0_8B2C_4C35_A00F_C8293BED35B4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "BF_AIO_Base.h"
#include "BF_AIO_Pool.h"
#include "BF_AIO_Resource.h"


#ifdef AIO_EXPORTS
#define AIO_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define AIO_EXPORT __declspec(dllimport)
#else
#define AIO_EXPORT 
#endif
#endif

class AIO_EXPORT CBF_AIO_WorkBase  
{
public:


	// 函数名: Close
	// 编程  : 王明松 2012-5-24 11:33:38
	// 返回  : bool 
	// 参数  : PSOCKET_POOL_DATA info  连接信息  s_pSendQueue里的内容要清空
	// 描述  : 通知AIO关闭连接，在这之前，发送队列应该要清空,否则会内存泄露，AIO收到通知后会释放连接到连接池
	//         ,状态为空闲，下次连接过来可继续使用
	bool CloseClient(PSOCKET_POOL_DATA info)
	{
		if (m_pAioMethod == NULL)
		{
			return false;
		}
		return m_pAioMethod->Close(info);
	}

	// 函数名: GetConnectPoolInfo
	// 编程  : 王明松 2013-6-14 20:32:01
	// 返回  : int 
	// 参数  : int &total
	// 描述  : 取连接池连接信处息
	int GetConnectPoolInfo(int &total)
	{
		int use;
		m_pAioPool->PoolInfo(total,use);
		return use;
	}

	// 函数名: Send2Client
	// 编程  : 王明松 2012-5-21 16:52:24
	// 返回  : bool 
	// 参数  : PSOCKET_SEND_DATA senddata
	// 描述  : 发送数据
	bool Send2Client(SOCKET_SEND_DATA senddata,bool isimmediate=false, bool islock=true)
	{
		if (m_pAioMethod == NULL)
		{
			return false;
		}
		return m_pAioMethod->Send(senddata,isimmediate, islock);
	}

	// 函数名: AddAio
	// 编程  : 王明松 2012-5-21 10:03:52
	// 返回  : bool 
	// 参数  : PSOCKET_POOL_DATA info
	// 描述  : 将连接信息加入到AIO中
    bool AddAio(PSOCKET_POOL_DATA info)
	{
		if (m_pAioMethod == NULL)
		{
			return false;
		}
		return m_pAioMethod->AddAio(info);
	}

	// 函数名: Connect
	// 编程  : 王明松 2012-5-21 11:56:45
	// 返回  : bool 
	// 参数  : char *ip  连接的IP
	// 参数  : short port  端口
	// 参数  : unsigned  short  sockettype   连接类型
	// 参数  : unsigned int destindex        若是vc的话要指定关联的连接
	// 参数  : unsigned int &index           AIO返回此连接索引
	// 描述  : 应用要进行连接，OnConnectedBack返回是否成功连上
	bool Connect(char *ip,short port,unsigned  short  sockettype, unsigned int destindex ,unsigned int &index)
	{
		if (m_pAioMethod == NULL)
		{
			return false;
		}
		return m_pAioMethod->Connect(ip,port,sockettype,destindex,index);
	}

	// 函数名: Connect
	// 编程  : 王明松 2012-5-21 11:55:51
	// 返回  : void 
	// 参数  : PSOCKET_POOL_DATA *info
	// 描述  : AIO通知，连接断开，要求连接
	virtual void OnConnect(PSOCKET_POOL_DATA info)
	{
		
	}


	// 函数名: OnSendBack
	// 编程  : 王明松 2012-5-17 16:38:08
	// 返回  : virtual void 
	// 参数  : PSOCKET_SEND_DATA data   
	// 描述  : AIO通知，写入数据返回，对返回结果进行处理
	virtual void OnSendBack(SOCKSENDQUEUE::iterator data,bool issuccess)
	{

	}
	virtual void OnSendBack(PSOCKET_SEND_DATA data,bool issuccess)
	{
		
	}

	// 函数名: OnPing
	// 编程  : 王明松 2012-5-21 11:54:23
	// 返回  : virtual void 
	// 描述  : AIO通知，要发送PING包
	virtual void OnPing(PSOCKET_POOL_DATA info)
	{
		
	}

	// 函数名: OnRecvBack
	// 编程  : 王明松 2012-5-17 16:36:58
	// 返回  : virtual void 
	// 参数  : PSOCKET_POOL_DATA info
	// 描述  : AIO通知，连接收到数据，对数据进行处理
	virtual void OnRecvBack(PSOCKET_POOL_DATA info)
	{
		//收到数据了，取数据，放入处理队列
		//框架在此函数返回后再去读数据,除非这里置状态通知框架读
	}

	// 函数名: OnConnectBack
	// 编程  : 王明松 2012-5-17 16:35:34
	// 返回  : virtual void 
	// 参数  : PSOCKET_POOL_DATA info
	// 参数  : int status 连接状态  0成功 其他失败
	// 描述  : AIO通知，发现主动连接成功，对此连接进行后续处理
	virtual void OnConnectBack(PSOCKET_POOL_DATA info,int status)
	{
		//置连接状态，发送连接验证信息等等
	}

	// 函数名: OnClose
	// 编程  : 王明松 2012-5-17 16:34:32
	// 返回  : virtual void 
	// 参数  : PSOCKET_POOL_DATA info 连接信息
	// 参数  : std::string msg
	// 描述  : AIO通知，发现一个连接关闭，AIO并没有对此连接进行关闭处理，要求在这里对此连接进行后续处理，然后调用Close方法通知AIO关闭等
	virtual void OnClose(PSOCKET_POOL_DATA info,std::string msg)
	{
		//清空发送队列 不能是简单的清空，要对队列的数据进行释放
//		info->s_pSendQueue.clear();
		CloseClient(info);
		
	}

	// 函数名: GetPoolData
	// 编程  : 王明松 2013-6-9 10:57:39
	// 返回  : PSOCKET_POOL_DATA 
	// 参数  : int index
	// 描述  : 根据连接索引取得连接信息
	PSOCKET_POOL_DATA GetPoolData(int index)
	{
		return m_pAioPool->GetDataByIndex(index);
	}

	// 函数名: OnAccept
	// 编程  : 王明松 2012-5-17 16:33:33
	// 返回  : virtual void 
	// 参数  : PSOCKET_POOL_DATA info
	// 描述  : AIO通知，收到一条连接，对此新连接进行处理,AIO框架不放入连接池，由应用检查通过后再显示调用AddAio
	virtual void OnAccept(PSOCKET_POOL_DATA info)
	{
		//检查连接是否合法

		//置SOCKET状态等等，是只读还是只写还是读写,主要是SELECT模型用 
		info->s_cSocketStatus = SOCKSTATUS_RW;
		AddAio(info);
	}

	// 函数名: SetParameter
	// 编程  : 王明松 2012-5-18 11:32:46
	// 返回  : void 
	// 参数  : CBF_AIO_Base *p
	// 描述  : 设置AIO参数，框架初始化时调用
	void SetParameter(CBF_AIO_Base *p,CBF_AIO_Pool   *pool);

	CBF_AIO_WorkBase();
	virtual ~CBF_AIO_WorkBase();

	int  GetConnInfo(std::vector<PSOCKET_POOL_DATA>& connlist)
	{
		return m_pAioPool->GetConnInfo(connlist);
	}

protected:
	CBF_AIO_Base   *m_pAioMethod;  //通过此基类指针调用发送数据、主动连接、将连接加入异步IO集合等
	CBF_AIO_Pool   *m_pAioPool;

};

#endif // !defined(AFX_BF_AIO_WORKBASE_H__1C6E4AF0_8B2C_4C35_A00F_C8293BED35B4__INCLUDED_)
