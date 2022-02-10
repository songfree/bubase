// DrebApiML.h: interface for the CDrebApiML class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DREBAPIML_H__AD9480A6_F92E_4F4B_94A3_BDD457FDFA98__INCLUDED_)
#define AFX_DREBAPIML_H__AD9480A6_F92E_4F4B_94A3_BDD457FDFA98__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "MdbBase.h"

#ifdef DREBAPI_JAVA_EXPORTS
#define DREBAPI_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define DREBAPI_EXPORT __declspec(dllimport)
#else
#define DREBAPI_EXPORT 
#endif
#endif

class CBF_DrebServerApi;

class DREBAPI_EXPORT CDrebApiML  
{
public:
	CBF_DrebServerApi * Find(int id);
	bool Delete(int id);
	int  Insert(CBF_DrebServerApi *point);

	CDrebApiML();
	virtual ~CDrebApiML();

protected:

	CMemTable <CBF_DrebServerApi *> m_table;
	CPkeyInt<1>                     m_key;   //rowidË÷Òý

};

#endif // !defined(AFX_DREBAPIML_H__AD9480A6_F92E_4F4B_94A3_BDD457FDFA98__INCLUDED_)
