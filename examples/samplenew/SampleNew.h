/**
* Copyright (c) 2008 songfree
* All rights reserved.
*
* �ļ�����:SampleNew.cpp
* ժҪ:
*       
*        
*
* ��ǰ�汾��:1.0.0
*
* �汾��ʷ��Ϣ:
* �汾��            ����/�޸���       �������                 ԭ��
* 1.0.0                AUTO           20100708                  ����
**/
// SampleNew.h: interface for the CSampleNew class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(BUBASE_AUTO_APPWIZARD_20100708_33383875_INCLUDED_)
#define BUBASE_AUTO_APPWIZARD_20100708_33383875_INCLUDED_

#include "BuModule.h"
#include "DrebBpcLog.h"


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class DLL_EXPORT CSampleNew : public CBuModule  
{
public:
	
public:
	CSampleNew();
	virtual ~CSampleNew();

public:
	// ������: BuInit
	// ���  : ������ 2010-6-29 16:17:11
	// ����  : virtual bool 
	// ����  : ��ʼ�������ڴ˷�������һЩ���⴦����Ҫ��m_bIsInitΪtrue
	virtual bool BuInit();
	
//      �ڴ˴������¹���
public:
	int Test1001(PBPCCOMMSTRU data);
	int Test1002(PBPCCOMMSTRU data);
	int Test1003(PBPCCOMMSTRU data);
	int Test1004(PBPCCOMMSTRU data);
	int Test1005(PBPCCOMMSTRU data);
	int Test1006(PBPCCOMMSTRU data);
	int Test1007(PBPCCOMMSTRU data);
	int Test1008(PBPCCOMMSTRU data);
	int Test1009(PBPCCOMMSTRU data);
	int Test1010(PBPCCOMMSTRU data);
	int Test99001(PBPCCOMMSTRU data);
	CDrebBpcLog m_pLogDat;

};
//ע�⣺����Ķ�����ÿ�������涼Ҫ�У���������ݴ˷�����ȡ�����ʵ��
extern "C" 
{ 
    DLL_EXPORT CSampleNew * getInstance() ; 
}

#endif // !BUBASE_AUTO_APPWIZARD_20100708_33383875_INCLUDED_)
