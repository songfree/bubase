// IVarDefine.h: interface for the CIVarDefine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IVARDEFINE_H__3C286316_243A_47B6_BF95_4E1DF17BC483__INCLUDED_)
#define AFX_IVARDEFINE_H__3C286316_243A_47B6_BF95_4E1DF17BC483__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "BF_Mutex.h"
using namespace std;

class CIVarDefine  
{
public:
	int GetVarLen();

	/// 函数名: GetFmt
	/// 编程  : 王明松 2009-7-17 17:11:33
	/// 返回  : string & 
	/// 描述  : 获取整型变量的格式字串
	string & GetFmt();

	/// 函数名: SetVarPara
	/// 编程  : 王明松 2009-7-17 17:10:28
	/// 返回  : void 
	/// 参数  : string name
	/// 参数  : int type
	/// 参数  : int len
	/// 描述  : 设置变量参数
	void SetVarPara(string name,int type,int len);

	/// 函数名: IntVarInit
	/// 编程  : 王明松 2009-7-17 17:10:36
	/// 返回  : void 
	/// 参数  : string fmt
	/// 参数  : __int64 initvalue
	/// 描述  : 整型变量初始化
	void IntVarInit(string fmt, __int64 initvalue);

	/// 函数名: PushVarValue
	/// 编程  : 王明松 2009-7-17 17:03:48
	/// 返回  : bool 
	/// 参数  : string svalue
	/// 描述  : 放入一个列表变量
	bool PushVarValue(string svalue);

	/// 函数名: GetVarType
	/// 编程  : 王明松 2009-7-17 17:04:01
	/// 返回  : int 
	/// 描述  : 获取变量类型
	int GetVarType();

	/// 函数名: GetVarName
	/// 编程  : 王明松 2009-7-17 17:04:14
	/// 返回  : string & 
	/// 描述  : 获取变量名称
	string & GetVarName();


	/// 函数名:     GetVarValue
	/// 编程  : 王明松 2009-7-17 17:04:24
	/// 返回  : __int64  
	/// 描述  : 取整型变量值
	__int64      GetVarValue();

	/// 函数名: GetVarValueS
	/// 编程  : 王明松 2009-7-17 17:04:33
	/// 返回  : std::string  
	/// 描述  : 取列表变量值
	std::string  GetVarValueS();



	CIVarDefine();
	virtual ~CIVarDefine();
	
	

protected:
	int m_varType;//变量类型 0整型增加   1列表随机选择 2列表顺序选择
	int m_varLen; //变量长度 
	__int64 m_maxVarValue;//最大的变量大小
	std::string m_fmt;//变量格式化字符串
	__int64 m_lValue ; //整型增加变量 类型为0时使用   _atoi64(value+1);//sprintf(value,"%I64d",lvalue+1); 
	CBF_Mutex  m_mutex;
	std::string m_sVarName;//变量名称
	vector<string>m_varValueList;//变量列表  类型为1 2时使用
	int m_varValueNo;//顺序选择时的下标
	

};

#endif // !defined(AFX_IVARDEFINE_H__3C286316_243A_47B6_BF95_4E1DF17BC483__INCLUDED_)
