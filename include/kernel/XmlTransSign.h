// XmlTransSign.h: interface for the CXmlTransSign class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XMLTRANSSIGN_H__CC509A7F_87F0_4D9A_9F38_9D627F4E7231__INCLUDED_)
#define AFX_XMLTRANSSIGN_H__CC509A7F_87F0_4D9A_9F38_9D627F4E7231__INCLUDED_



#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



#include "public.h"
#ifdef KERNEL_EXPORTS
#define KERNEL_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define KERNEL_EXPORT __declspec(dllimport)
#else
#define KERNEL_EXPORT 
#endif
#endif
extern "C" 
{ 
	/// 函数名: XmlgetTransSign
	/// 编程  : 王明松 2008-7-11 9:54:03
	/// 返回  : int 
	/// 输入输出参数  : std::string &value
	/// 描述  : 去除转义符
	KERNEL_EXPORT int XmlgetTransSign(std::string &value);

	/// 函数名: XmlputTransSign
	/// 编程  : 王明松 2008-7-11 9:54:07
	/// 返回  : int 
	/// 输入输出参数  : std::string &value
	/// 描述  : 加入转议符
	KERNEL_EXPORT int XmlputTransSign(std::string &value);
}
#undef KERNEL_EXPORT
#endif // !defined(AFX_XMLTRANSSIGN_H__CC509A7F_87F0_4D9A_9F38_9D627F4E7231__INCLUDED_)
