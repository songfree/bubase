// NextTbl.h: interface for the CNextTbl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NEXTTBL_H__786AAE62_7EC4_42CD_AFFA_3902DA17B08A__INCLUDED_)
#define AFX_NEXTTBL_H__786AAE62_7EC4_42CD_AFFA_3902DA17B08A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MdbBase.h"
#include "BF_Mutex.h"

#include "GResource.h"

typedef struct 
{
	int      nNodeId;        //数据总线节点编号
	int      nNodePrivateId; //数据总线节点私有序号
	int      nDrebSerial;    //数据总线节点产生的流水，标志唯一的请求数据包
	unsigned int  nFileSerial;//文件流水
	int      nTime;//生成时间
}S_NEXT_TBL;     //后续包信息表

class CNextTbl  
{
public:
	CGResource *m_pRes;

	// 函数名: DeleteTime
	// 编程  : 王明松 2013-4-16 15:39:39
	// 返回  : int 
	// 参数  : unsigned int stime  超时秒
	// 描述  : 删除时间超时的记录
	int DeleteTime(unsigned int stime);
	
	CNextTbl();
	virtual ~CNextTbl();

	// 函数名: Next
	// 编程  : 王明松 2013-4-16 15:40:31
	// 返回  : bool 
	// 参数  : S_NEXT_TBL &fn
	// 描述  : 下一条记录
	bool Next(S_NEXT_TBL &fn);
	
	// 函数名: First
	// 编程  : 王明松 2013-4-16 15:40:19
	// 返回  : bool 
	// 参数  : S_NEXT_TBL &fn
	// 描述  : 第一条记录
	bool First(S_NEXT_TBL &fn);

	// 函数名: Select
	// 编程  : 王明松 2013-4-16 15:40:45
	// 返回  : bool 
	// 参数  : int nodeid
	// 参数  : int privateid
	// 参数  : int serial
	// 参数  : S_NEXT_TBL &fn
	// 描述  : 查找记录
	bool Select(int nodeid,int privateid,int serial,S_NEXT_TBL &fn);


	// 函数名: Delete
	// 编程  : 王明松 2013-4-16 15:41:05
	// 返回  : bool 
	// 参数  : int nodeid
	// 参数  : int privateid
	// 参数  : int serial
	// 描述  : 删除记录
	bool Delete(int nodeid,int privateid,int serial);
	

	// 函数名: Update
	// 编程  : 王明松 2013-4-16 15:41:16
	// 返回  : bool 
	// 参数  : S_NEXT_TBL fn
	// 描述  : 更新记录
	bool Update(S_NEXT_TBL fn);


	// 函数名: Insert
	// 编程  : 王明松 2013-4-16 15:41:27
	// 返回  : bool 
	// 参数  : S_NEXT_TBL fn
	// 描述  : 插入记录
	bool Insert(S_NEXT_TBL fn);
protected:
	
	CMemTable <S_NEXT_TBL> m_table;               //功能 内存表
	CIndexInt<3>           m_pkey;                //节点ID，节点私有ID，节点流水，唯一索引
	CIndexInt<1>           m_indextime;           //时间索引
	CBF_Mutex m_mutex;	
};



#endif // !defined(AFX_NEXTTBL_H__786AAE62_7EC4_42CD_AFFA_3902DA17B08A__INCLUDED_)
