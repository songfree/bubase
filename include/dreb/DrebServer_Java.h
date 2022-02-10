// DrebServer_Java.h: interface for the CDrebServer_Java class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DREBSERVER_JAVA_H__FD8439D9_4F77_44A4_8A79_44513F2549F3__INCLUDED_)
#define AFX_DREBSERVER_JAVA_H__FD8439D9_4F77_44A4_8A79_44513F2549F3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "DrebPubDefine.h"
#include "DrebEndian.h"
#include "MdbBase.h"
#include "BF_DrebServerApi.h"

#ifdef DREBAPI_JAVA_EXPORTS
#define DREBAPI_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define DREBAPI_EXPORT __declspec(dllimport)
#else
#define DREBAPI_EXPORT 
#endif
#endif

class DREBAPI_EXPORT CDrebServer_Java  
{
public:
	CDrebServer_Java();
	virtual ~CDrebServer_Java();
	

protected:
	CDrebEndian  m_pEndian;//字节序转换
	CInstanceList<CBF_DrebServerApi> m_pInstanceList;  //保存java程序new生成的实例，delete由java程序主动调用
};

//如下是针对上述类提供给java调用的c方法
extern "C"
{
	
}
#endif // !defined(AFX_DREBSERVER_JAVA_H__FD8439D9_4F77_44A4_8A79_44513F2549F3__INCLUDED_)
