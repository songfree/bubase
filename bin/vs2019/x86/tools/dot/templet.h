/**
* Copyright (c) 2008 songfree
* All rights reserved.
*
* �ļ�����:%s.cpp
* ժҪ:
*       
*        
*
* ��ǰ�汾��:1.0.0
*
* �汾��ʷ��Ϣ:
* �汾��            ����/�޸���       �������                 ԭ��
* 1.0.0                AUTO           %s                  ����
**/
// %s.h: interface for the C%s class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(%s)
#define %s

#include "BuModule.h"



#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class DLL_EXPORT C%s : public CBuModule  
{
public:
	
public:
	C%s();
	virtual ~C%s();

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
	
	

};
//ע�⣺����Ķ�����ÿ�������涼Ҫ�У���������ݴ˷�����ȡ�����ʵ��
extern "C" 
{ 
    DLL_EXPORT C%s * getInstance() ; 
}

#endif // !%s)
