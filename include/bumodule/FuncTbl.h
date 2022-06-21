// FuncTbl.h: interface for the CFuncTbl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FUNCTBL_H__17F68A7D_AC29_4ACD_9FD9_31FC164D69A9__INCLUDED_)
#define AFX_FUNCTBL_H__17F68A7D_AC29_4ACD_9FD9_31FC164D69A9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MdbBase.h"
#include "public.h"
#include "IBuModule.h"


#ifdef BUMODULE_EXPORTS
#define BUMODULE_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define BUMODULE_EXPORT __declspec(dllimport)
#else
#define BUMODULE_EXPORT 
#endif
#endif

class BUMODULE_EXPORT CFuncTbl  
{
protected:
	
	CMemTableNew <S_TBL_FUNC> m_table;
	CPkeyIntUnordered<1>                 m_pkey;               //功能号的key
public:

	// 函数名: Clear
	// 编程  : 王明松 2010-7-1 14:54:35
	// 返回  : void 
	// 描述  : 清空表及索引
	void Clear();

	// 函数名: Next
	// 编程  : 王明松 2010-6-29 10:53:55
	// 返回  : bool 无则返回false
	// 参数  : S_TBL_FUNC &funcinfo
	// 描述  : 返回下一条功能信息，无则返回false
	bool Next(S_TBL_FUNC &funcinfo);

	// 函数名: First
	// 编程  : 王明松 2010-6-29 10:53:20
	// 返回  : bool 无则返回false
	// 参数  : S_TBL_FUNC &funcinfo
	// 描述  : 返回第一条功能信息
	bool First(S_TBL_FUNC &funcinfo);

	// 函数名: Select
	// 编程  : 王明松 2010-6-29 10:51:02
	// 返回  : bool 不存在返回false
	// 参数  : int nFuncNo  要查找的功能号
	// 参数  : S_TBL_FUNC &funcinfo
	// 描述  : 查找功能信息
	bool Select(int nFuncNo,S_TBL_FUNC &funcinfo);


	// 函数名: Find
	// 编程  : 王明松 2013-4-25 11:45:13
	// 返回  : S_TBL_FUNC &  
	// 参数  : int nFuncNo 要查找的功能号
	// 描述  : 查找功能信息
	S_TBL_FUNC & Find(int nFuncNo);

	// 函数名: Update
	// 编程  : 王明松 2010-6-29 10:50:32
	// 返回  : bool 若功能不存在则返回false
	// 参数  : S_TBL_FUNC funcinfo
	// 描述  : 更新一条功能信息
	bool Update(S_TBL_FUNC funcinfo);

	// 函数名: Insert
	// 编程  : 王明松 2010-6-29 10:49:58
	// 返回  : bool 若功能号存在则返回false
	// 参数  : S_TBL_FUNC funcinfo
	// 描述  : insert一条功能信息
	bool Insert(S_TBL_FUNC funcinfo);

	CFuncTbl();
	virtual ~CFuncTbl();
protected:

	S_TBL_FUNC  m_pNull;


};


class BUMODULE_EXPORT CFuncTblOld 
{
protected:
	
	CMemTable <S_TBL_FUNC_OLD> m_table;
	CIndexInt<1>                 m_pkey;               //功能号的key
public:
	
	// 函数名: Clear
	// 编程  : 王明松 2010-7-1 14:54:35
	// 返回  : void 
	// 描述  : 清空表及索引
	void Clear();
	
	// 函数名: Next
	// 编程  : 王明松 2010-6-29 10:53:55
	// 返回  : bool 无则返回false
	// 参数  : S_TBL_FUNC_OLD &funcinfo
	// 描述  : 返回下一条功能信息，无则返回false
	bool Next(S_TBL_FUNC_OLD &funcinfo);
	
	// 函数名: First
	// 编程  : 王明松 2010-6-29 10:53:20
	// 返回  : bool 无则返回false
	// 参数  : S_TBL_FUNC_OLD &funcinfo
	// 描述  : 返回第一条功能信息
	bool First(S_TBL_FUNC_OLD &funcinfo);
	
	// 函数名: Select
	// 编程  : 王明松 2010-6-29 10:51:02
	// 返回  : bool 不存在返回false
	// 参数  : int nFuncNo  要查找的功能号
	// 参数  : S_TBL_FUNC_OLD &funcinfo
	// 描述  : 查找功能信息
	bool Select(int nFuncNo,S_TBL_FUNC_OLD &funcinfo);
	
	
	// 函数名: Find
	// 编程  : 王明松 2013-4-25 11:45:13
	// 返回  : S_TBL_FUNC_OLD &  
	// 参数  : int nFuncNo 要查找的功能号
	// 描述  : 查找功能信息
	S_TBL_FUNC_OLD & Find(int nFuncNo);
	
	// 函数名: Update
	// 编程  : 王明松 2010-6-29 10:50:32
	// 返回  : bool 若功能不存在则返回false
	// 参数  : S_TBL_FUNC_OLD funcinfo
	// 描述  : 更新一条功能信息
	bool Update(S_TBL_FUNC_OLD funcinfo);
	
	// 函数名: Insert
	// 编程  : 王明松 2010-6-29 10:49:58
	// 返回  : bool 若功能号存在则返回false
	// 参数  : S_TBL_FUNC_OLD funcinfo
	// 描述  : insert一条功能信息
	bool Insert(S_TBL_FUNC_OLD funcinfo);
	
	CFuncTblOld();
	virtual ~CFuncTblOld();
protected:
	
	S_TBL_FUNC_OLD  m_pNull;
	
	
};

#undef  BUMODULE_EXPORT
#endif // !defined(AFX_FUNCTBL_H__17F68A7D_AC29_4ACD_9FD9_31FC164D69A9__INCLUDED_)
