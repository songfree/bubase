// PortTbl.h: interface for the CPortTbl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PORTTBL_H__C88A811D_0EB0_4CB7_BA1F_A3FA529F9BE6__INCLUDED_)
#define AFX_PORTTBL_H__C88A811D_0EB0_4CB7_BA1F_A3FA529F9BE6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MdbBase.h"

typedef struct 
{
	std::string    sPortName; 
	unsigned short nPort;
} S_TIB_PORT;

#ifdef TIB_EXPORTS
#define TIB_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define TIB_EXPORT __declspec(dllimport)
#else
#define TIB_EXPORT 
#endif
#endif

class TIB_EXPORT CPortTbl  
{
public:

	// 函数名: At
	// 编程  : 王明松 2015-6-18 9:39:50
	// 返回  : int <=0 失败   >0端口
	// 参数  : unsigned int index
	// 描述  : 根据索引取得端口
	int At(unsigned int index);

	// 函数名: GetPortByName
	// 编程  : 王明松 2015-6-18 9:37:51
	// 返回  : int <0失败，无此名称  >=0为端口
	// 参数  : const char *portname
	// 描述  : 通过端口名称取得端口
	int GetPortByName(const char *portname);

	// 函数名: GetIndexByName
	// 编程  : 王明松 2015-6-18 9:35:15
	// 返回  : int <0没有此名称   >=为对应的索引值
	// 参数  : const char *portname
	// 描述  : 通过端口名称得到端口对应的索引，可通过索引直接取得端口值，不用再去查找
	int GetIndexByName(const char *portname);

	// 函数名: Add
	// 编程  : 王明松 2015-6-18 9:34:46
	// 返回  : bool true成功 false失败，端口重复或端口名称重复
	// 参数  : S_TIB_PORT &portdata
	// 描述  : 增加一个端口
	bool Add(S_TIB_PORT &portdata);

	// 函数名: Clear
	// 编程  : 王明松 2015-6-18 9:34:36
	// 返回  : void 
	// 描述  : 清空列表
	void Clear();
	CPortTbl();
	virtual ~CPortTbl();


private:
	std::vector<S_TIB_PORT>  m_vPortList;//端口列表

};

#endif // !defined(AFX_PORTTBL_H__C88A811D_0EB0_4CB7_BA1F_A3FA529F9BE6__INCLUDED_)
