// BF_AIO_Base.h: interface for the CBF_AIO_Base class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BF_AIO_BASE_H__81C91698_796D_4220_84E1_4611D9C58EBE__INCLUDED_)
#define AFX_BF_AIO_BASE_H__81C91698_796D_4220_84E1_4611D9C58EBE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BF_AIO_Head.h"



#ifdef AIO_EXPORTS
#define AIO_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define AIO_EXPORT __declspec(dllimport)
#else
#define AIO_EXPORT 
#endif
#endif

class AIO_EXPORT CBF_AIO_Base  
{
public:
	// 函数名: GetSocketErrNo
	// 编程  : 王明松 2012-5-24 15:03:40
	// 返回  : int  
	// 描述  : 取得iocp发送出错时的ERRNO
	virtual int  GetSocketErrNo()=0;

	// 函数名: Send
	// 编程  : 王明松 2012-5-21 16:52:24
	// 返回  : bool 
	// 参数  : PSOCKET_SEND_DATA senddata
	// 描述  : 发送数据
	virtual bool Send(SOCKET_SEND_DATA senddata,bool isimmediate=false)=0;
	
	// 函数名: AddAio
	// 编程  : 王明松 2012-5-21 10:03:52
	// 返回  : bool 
	// 参数  : PSOCKET_POOL_DATA *info
	// 描述  : 将连接信息加入到AIO中
    virtual bool AddAio(PSOCKET_POOL_DATA info)=0;


	// 函数名: Close
	// 编程  : 王明松 2012-5-22 11:16:09
	// 返回  : virtual bool 
	// 参数  : PSOCKET_POOL_DATA info
	// 描述  : 应用主动关闭连接
	virtual bool Close(PSOCKET_POOL_DATA info)=0;
	
	// 函数名: Connect
	// 编程  : 王明松 2012-5-21 11:56:45
	// 返回  : bool 
	// 参数  : char *ip  连接的IP
	// 参数  : short port  端口
	// 参数  : unsigned  short  sockettype   连接类型
	// 参数  : unsigned int destindex        若是vc的话要指定关联的连接
	// 参数  : unsigned int &index           AIO返回此连接索引
	// 描述  : 应用要进行连接，OnConnectedBack返回是否成功连上
	virtual bool Connect(char *ip,short port,unsigned  short  sockettype, unsigned int destindex ,unsigned int &index)=0;

	CBF_AIO_Base()
	{

	}
	virtual ~CBF_AIO_Base()
	{

	}

};

#endif // !defined(AFX_BF_AIO_BASE_H__81C91698_796D_4220_84E1_4611D9C58EBE__INCLUDED_)
