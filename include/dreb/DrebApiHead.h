// DrebApiHead.h: interface for the CDrebApiHead class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DREBAPIHEAD_H__FA633950_A8B1_487B_B35A_8F9E55D048DB__INCLUDED_)
#define AFX_DREBAPIHEAD_H__FA633950_A8B1_487B_B35A_8F9E55D048DB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "dreberrcode.h"
#include "DrebClient.h"
#include "DrebServer.h"


#ifdef _WINDOWS
#ifdef _DEBUG
#pragma comment(lib, "bf_kerneld.lib") 
#pragma comment(lib, "bf_drebapid.lib") 
#pragma message("Automatically linking with   bf_kerneld.lib bf_drebapid.lib")
#else
#pragma comment(lib, "bf_kernel.lib") 
#pragma comment(lib, "bf_drebapi.lib") 
#pragma message("Automatically linking with   bf_kernel.lib bf_drebapi.lib")
#endif

#endif

#endif // !defined(AFX_DREBAPIHEAD_H__FA633950_A8B1_487B_B35A_8F9E55D048DB__INCLUDED_)
