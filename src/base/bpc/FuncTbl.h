// FuncTbl.h: interface for the CFuncTbl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FUNCTBL_H__1BB79147_6A71_4944_AA41_5F4BBC375F08__INCLUDED_)
#define AFX_FUNCTBL_H__1BB79147_6A71_4944_AA41_5F4BBC375F08__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MdbBase.h"
#include "BF_Mutex.h"
#include "BpcHead.h"


typedef struct 
{
	unsigned int    nFuncNo;           //交易码
	char     sFuncName[41];         //交易名称
	char     sBpuGroupName[21];      //bpu组名称
	char            cPrio;             //交易优先级
	char            cServiceFlag;      //服务标志 0 BPC内部功能不用在DREB上注册2BPU内部功能不用在DREB上注册  1要在DREB上注册
	unsigned int    nCallNum;          //调用次数
	unsigned int    nBpuGroupIndex;    //bpu组对应的请求队列索引
}S_FUNCINFO_TBL;     //功能信息

class CFuncTbl  
{
public:

	// 函数名: GetAllFunc
	// 编程  : 王明松 2013-10-25 10:16:58
	// 返回  : int 
	// 参数  : vector<S_FUNCINFO_TBL> &funclist
	// 描述  : 获取所有的功能列表，按交易码排序
	int GetAllFunc(vector<S_FUNCINFO_TBL> &funclist);


	// 函数名: GetAllFuncByGroup
	// 编程  : 王明松 2013-11-2 15:32:11
	// 返回  : int 
	// 参数  : vector<S_FUNCINFO_TBL> &funclist
	// 描述  : 获取所有的功能列表，按bpu组排序
	int GetAllFuncByGroup(vector<S_FUNCINFO_TBL> &funclist);

	// 函数名: FindFunc
	// 编程  : 王明松 2013-4-8 11:40:16
	// 返回  : S_FUNCINFO_TBL & 
	// 参数  : unsigned int func
	// 参数  : int &findflag  标志 0没找到，无此功能 1找到有些功能
	// 描述  : 查找一个功能，并返回功能地址，置功能的调用计数加1
	S_FUNCINFO_TBL & FindFunc(unsigned int func,int &findflag);

	// 函数名: SelectByPk
	// 编程  : 王明松 2010-7-27 11:26:15
	// 返回  : bool 
	// 参数  : unsigned int func
	// 参数  : S_FUNCINFO_TBL &fn
	// 描述  : 获取功能,功能的调用计数不变
	bool Select(unsigned int func,S_FUNCINFO_TBL &fn);

	CFuncTbl();
	virtual ~CFuncTbl();
	
	// 函数名: Insert
	// 编程  : 王明松 2010-7-27 11:25:25
	// 返回  : bool 
	// 参数  : S_FUNCINFO_TBL fn
	// 描述  : insert一条功能
	bool Insert(S_FUNCINFO_TBL fn);

	int Size()
	{
		return m_pkey.Size();
	}

protected:
	// 函数名: Next
	// 编程  : 王明松 2010-7-27 11:26:34
	// 返回  : bool 
	// 参数  : S_FUNCINFO_TBL &fn
	// 描述  : 下一个功能
	bool Next(S_FUNCINFO_TBL &fn);
	
	// 函数名: First
	// 编程  : 王明松 2010-7-27 11:26:23
	// 返回  : bool 
	// 参数  : S_FUNCINFO_TBL &fn
	// 描述  : 第一个功能
	bool First(S_FUNCINFO_TBL &fn);

	
	
	// 函数名: Delete
	// 编程  : 王明松 2010-7-27 11:26:06
	// 返回  : bool 
	// 参数  : unsigned int func
	// 描述  : 删除一条功能
	bool Delete(unsigned int func);
	
	// 函数名: Update
	// 编程  : 王明松 2010-7-27 11:25:38
	// 返回  : bool 
	// 参数  : S_FUNCINFO_TBL fn
	// 描述  : 更新功能
	bool Update(S_FUNCINFO_TBL fn);


	CMemTable <S_FUNCINFO_TBL> m_table;        //功能 内存表
	CIndexInt<1>               m_pkey;           //功能号，唯一索引
	CIndexCharF<21,1>          m_indexgroup;     //以bpu组为索引
	S_FUNCINFO_TBL             m_null;
	CBF_Mutex m_mutex;	
};

#endif // !defined(AFX_FUNCTBL_H__1BB79147_6A71_4944_AA41_5F4BBC375F08__INCLUDED_)
