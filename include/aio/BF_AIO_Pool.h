// BF_AIO_Pool.h: interface for the CBF_AIO_Pool class.
//
//////////////////////////////////////////////////////////////////////
/***********************************************************************************
  编码日志记录
  编码者: 王明松
  编码日期 2012-5-15 15:02:11
  版本: V1.00 
  说明: 本类是一个连接池类，存放所有的连接，初始化的时候先建立指定大小的连接池数据区，
         当连接池不够时重新申请增加，当达到指定的数量时，返回失败
 ***********************************************************************************/
#if !defined(AFX_BF_AIO_POOL_H__DB10BB1F_05E0_48DE_87A3_01439B3B52D0__INCLUDED_)
#define AFX_BF_AIO_POOL_H__DB10BB1F_05E0_48DE_87A3_01439B3B52D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "BF_Socket.h"
#include "BF_Mutex.h"
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





class AIO_EXPORT CBF_AIO_Pool  
{
public:
	
	// 函数名: GetDataByIndex
	// 编程  : 王明松 2012-5-18 10:56:01
	// 返回  : PSOCKET_POOL_DATA index不存在返回NULL
	// 参数  : unsigned int index
	// 描述  : 根据索引取得连接信息
	PSOCKET_POOL_DATA GetDataByIndex(unsigned int index);
	
	// 函数名: PoolInfo
	// 编程  : 王明松 2012-5-15 16:21:59
	// 返回  : void 
	// 参数  : int &total   总的连接池数目
	// 参数  : int &unused  空闲数目
	// 描述  : 连接池信息 
	void PoolInfo(int &total,int &unused);
	
	// 函数名: CloseFree
	// 编程  : 王明松 2012-5-15 16:21:28
	// 返回  : void 
	// 参数  : PSOCKET_POOL_DATA pp
	// 描述  : 关闭连接，将此连接放入空闲的池中
	void CloseFree(PSOCKET_POOL_DATA pp);
	
	// 函数名: PoolMalloc
	// 编程  : 王明松 2012-5-15 16:13:50
	// 返回  : PSOCKET_POOL_DATA 
	// 描述  : 取得一个未操作的socket连接信息，如果连接池里有空则返回，否则NEW返回
	PSOCKET_POOL_DATA PoolMalloc();
	
	// 函数名: Init
	// 编程  : 王明松 2012-5-15 15:51:51
	// 返回  : bool 
	// 参数  : unsigned int num  连接数
	// 描述  : 初始化连接
	bool Init(unsigned int num);
	
	// 函数名: SetMaxNum
	// 编程  : 王明松 2012-5-15 15:41:53
	// 返回  : void 
	// 参数  : unsigned int max
	// 描述  : 设置最大的连接数
	void SetMaxNum(unsigned int max);

	// 函数名: RestData
	// 编程  : 王明松 2012-5-15 15:55:22
	// 返回  : void 
	// 参数  : PSOCKET_POOL_DATA pp
	// 描述  : 重置连接信息为初始值
	void ResetData(PSOCKET_POOL_DATA pp);
	
	CBF_AIO_Pool();
	virtual ~CBF_AIO_Pool();
	
private:
	
	CBF_Mutex    m_mutex;
	unsigned int m_nMaxNum;         //最大socket数
	unsigned int m_nCurSocketIndex; //当前的socket索引数
	
	std::vector<PSOCKET_POOL_DATA> m_vAllSocketInfo;  //存放所有的信息指针
	std::deque<PSOCKET_POOL_DATA>  m_pFreeSocketInfo; //存放空闲的socket信息


	PSOCKET_POOL_DATA m_pSocketData;
};

#endif // !defined(AFX_BF_AIO_POOL_H__DB10BB1F_05E0_48DE_87A3_01439B3B52D0__INCLUDED_)
