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
// %s.cpp: implementation of the C%s class.
//
//////////////////////////////////////////////////////////////////////



#include "%s.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


C%s::~C%s()
{

}
bool C%s::BuInit()
{
	m_bIsInit = true;
	return m_bIsInit;
}


void C%s::SetBuPara(CBF_Xml *conxml,CIPcLink *pclink,CDbApi *sqldb,CBF_SimpleLog *bulog)
{
	if (conxml == NULL || pclink == NULL || bulog == NULL)
	{
		m_bIsSetPara = false;
		return;
	}
	m_pLog = bulog;
	m_pConfigXml = conxml;
	m_pPcLink = pclink;
	m_pSqlCmd = sqldb;
	m_bIsSetPara = true;
}
//ע�⣺����Ķ�����ÿ�������涼Ҫ�У���������ݴ˷�����ȡ�����ʵ��
C%s * getInstance() 
{ 
    return new C%s(); 
}
C%s::C%s()
{
	
	//���ܺ� ����ָ��  �������� �汾 ����  ���ȼ�  ������־ �Ƿ��׵�λ  �Ƿ�ͣ��
	//InitFuncInfo(1000,(FuncPointer )&CSample::test01,"����",1.0,"����",1,false,true,false);
	//�����ӹ���Ҫ�ڴ˽�������
}


