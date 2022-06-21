// DrebApiML.h: interface for the CDrebApiML class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHCAAPIML_H__AD9480A6_F92E_4F4B_94A3_BDD457FDFA98__INCLUDED_)
#define AFX_SHCAAPIML_H__AD9480A6_F92E_4F4B_94A3_BDD457FDFA98__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "MdbBase.h"

class CShcaApi;

class CShcaApiML  
{
public:
	CShcaApi * Find(int id);
	bool Delete(int id);
	int  Insert(CShcaApi *point);

	CShcaApiML();
	virtual ~CShcaApiML();

protected:

	CMemTable <CShcaApi *> m_table;
	CPkeyInt<1>                     m_key;   //rowidË÷Òý

};

#endif // !defined(AFX_SHCAAPIML_H__AD9480A6_F92E_4F4B_94A3_BDD457FDFA98__INCLUDED_)
