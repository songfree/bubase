/**
* Copyright (c) 2008 songfree
* All rights reserved.
*
* �ļ�����:MoniSvr.cpp
* ժҪ:
*       
*        
*
* ��ǰ�汾��:1.0.0
*
* �汾��ʷ��Ϣ:
* �汾��            ����/�޸���       �������                 ԭ��
* 1.0.0                AUTO           20131127                  ����
**/
// MoniSvr.h: interface for the CMoniSvr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(BUBASE_AUTO_APPWIZARD_20131127_4261728_INCLUDED_)
#define BUBASE_AUTO_APPWIZARD_20131127_4261728_INCLUDED_

#include "BuModule.h"



#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class DLL_EXPORT CMoniSvr : public CBuModule  
{
public:
	
public:
	CMoniSvr();
	virtual ~CMoniSvr();

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
	bool GetSerialDb2(int &no);

	// ������: AddPcNode
	// ���  : ������ 2013-11-29 16:11:17
	// ����  : int 
	// ����  : PBPCCOMMSTRU data
	// ����  : ��������ڵ�
	int AddPcNode(PBPCCOMMSTRU data);

	// ������: NodeReport
	// ���  : ������ 2013-11-27 10:16:27
	// ����  : int 
	// ����  : PBPCCOMMSTRU data
	// ����  : �ڵ㱨�洦��
	int NodeReport(PBPCCOMMSTRU data);
	

	
protected:

	// ������: UpdateByLastTime
	// ���  : ������ 2013-11-29 16:09:47
	// ����  : int 
	// ����  : ����״̬������������ʱ��
	int UpdateByLastTime();

	// ������: Bpc2Report
	// ���  : ������ 2013-11-29 16:09:33
	// ����  : int 
	// ����  : CBF_Xml *xml
	// ����  : ��BPC���洦��
	int Bpc2Report(CBF_Xml *xml);

	// ������: BpcReport
	// ���  : ������ 2013-11-29 16:09:21
	// ����  : int 
	// ����  : CBF_Xml *xml
	// ����  : BPC���洦��
	int BpcReport(CBF_Xml *xml);

	// ������: DrebReport
	// ���  : ������ 2013-11-29 16:09:09
	// ����  : int 
	// ����  : CBF_Xml *xml
	// ����  : DREB�ڵ㱨�洦��
	int DrebReport(CBF_Xml *xml);

	// ������: GetSerial
	// ���  : ������ 2013-11-29 16:08:51
	// ����  : bool 
	// ����  : char *logserial
	// ����  : ȡ��ˮ��ɾ����־���ļ�¼
	bool GetSerial(char *logserial);

	int   m_nSaveLogNum;//�������־��
	int   m_nWriteBFMLog;
	int   m_nStatusTimeOut;
	int   m_nNodeType;//�ڵ�����
	int   m_nNodeId;//�����ڵ��
	int   m_nNodePrivateId;//˽�нڵ��
	int   m_nReportType;//��������
	int   m_nPcNodeId;  //����id
	std::string  m_sStartTime;//����ʱ��

	unsigned int m_nCurTime;

	char m_sSql[8*1024];

	CBF_Date_Time m_pDate;

};
//ע�⣺����Ķ�����ÿ�������涼Ҫ�У���������ݴ˷�����ȡ�����ʵ��
extern "C" 
{ 
    DLL_EXPORT CMoniSvr * getInstance() ; 
}

#endif // !BUBASE_AUTO_APPWIZARD_20131127_4261728_INCLUDED_)
