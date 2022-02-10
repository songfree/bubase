/**
* Copyright (c) 2008 songfree
* All rights reserved.
*
* �ļ�����:Sample.cpp
* ժҪ:
*       
*        
*
* ��ǰ�汾��:1.0.0
*
* �汾��ʷ��Ϣ:
* �汾��            ����/�޸���       �������                 ԭ��
* 1.0.0                AUTO           20130514                  ����
**/
// Sample.h: interface for the CSample class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(BUBASE_AUTO_APPWIZARD_20130514_24199015_INCLUDED_)
#define BUBASE_AUTO_APPWIZARD_20130514_24199015_INCLUDED_

#include "BuModule.h"
#include "DrebBpcLog.h"


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class DLL_EXPORT CSample : public CBuModule  
{
public:
	
public:
	CSample();
	virtual ~CSample();

public:
	// ������: BuInit
	// ���  : ������ 2010-6-29 16:17:11
	// ����  : virtual bool 
	// ����  : ��ʼ�������ڴ˷�������һЩ���⴦����Ҫ��m_bIsInitΪtrue
	virtual bool BuInit();
	

	// ������: SetBuPara
	// ���  : ������ 2010-6-30 11:00:30
	// ����  : void 
	// ����  : CBuXml *conxml
	// ����  : CICallBu *callbu
	// ����  : CDbApi *sqldb
	// ����  : CIBuLog *bulog
	// ����  : ���ò���
	virtual void SetBuPara(CBF_Xml *conxml,CIPcLink *pclink,CDbApi *sqldb,CBF_SimpleLog *bulog);
//      �ڴ˴������¹���
public:
	int Test2010(PBPCCOMMSTRU data);
	int Test2009(PBPCCOMMSTRU data);
	int Test2008(PBPCCOMMSTRU data);
	int Test2007(PBPCCOMMSTRU data);
	int Test2006(PBPCCOMMSTRU data);
	int Test2005(PBPCCOMMSTRU data);
	int Test2004(PBPCCOMMSTRU data);
	int Test2003(PBPCCOMMSTRU data);
	int Test2002(PBPCCOMMSTRU data);
	int Test2001(PBPCCOMMSTRU data);

	int Test3001(CXmlPack *xmlpack,CXmlPackArray *muxmlpack);
	
	
	CDrebBpcLog  m_pLogDat;
};
//ע�⣺����Ķ�����ÿ�������涼Ҫ�У���������ݴ˷�����ȡ�����ʵ��
extern "C" 
{ 
    DLL_EXPORT CSample * getInstance() ; 
}

#endif // !BUBASE_AUTO_APPWIZARD_20130514_24199015_INCLUDED_)
