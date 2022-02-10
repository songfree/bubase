// IBuModule.h: interface for the CIBuModule class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IBUMODULE_H__F6A715DF_DB67_4547_B40B_1259BA78EE15__INCLUDED_)
#define AFX_IBUMODULE_H__F6A715DF_DB67_4547_B40B_1259BA78EE15__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "BpcHead.h"
#include "XmlPack.h"

class CIBuModule;

typedef  int(CIBuModule::*FuncPointer)(PBPCCOMMSTRU data);

typedef  int(CIBuModule::*FuncPointerOld)(CXmlPack *xmlpack,CXmlPackArray *muxmlpack);


typedef struct _S_TBL_FUNC
{   
	int         nFuncNo; //���ܺ� 
	FuncPointer pFunc;///����ָ��
	char        sFuncName[81];///��������
	double      dVersion;///�汾
	char        sAuthor[41];///����
	int         nLevel;///���ȼ�
	bool        bRepealFlag;///������־
	CIBuModule *pBuInstance;///������ʵ��
	char        sDllName[201];///��̬������
	bool        bIsTxUnit;///�Ƿ��ǽ��׵�λ(����ǽ��׵�λ������bcc��ע�ᣬ�����̿��������ã�����Ҫ��bccע��)
	bool        bIsStop;///�˹����Ƿ�ֹͣ
	unsigned int nCallNum;//���ô���
}S_TBL_FUNC;
typedef struct _S_TBL_FUNC_OLD
{   
	int         nFuncNo; //���ܺ� 
	FuncPointerOld pFunc;///����ָ��
	char        sFuncName[81];///��������
	double      dVersion;///�汾
	char        sAuthor[41];///����
	int         nLevel;///���ȼ�
	bool        bRepealFlag;///������־
	CIBuModule *pBuInstance;///������ʵ��
	char        sDllName[201];///��̬������
	bool        bIsTxUnit;///�Ƿ��ǽ��׵�λ(����ǽ��׵�λ������bcc��ע�ᣬ�����̿��������ã�����Ҫ��bccע��)
	bool        bIsStop;///�˹����Ƿ�ֹͣ
	unsigned int nCallNum;//���ô���
}S_TBL_FUNC_OLD;  //Ϊ�˼������ڵĿ��xmlpack 

class CIBuModule  
{
public:
	
	CIBuModule()
	{

	}
	virtual ~CIBuModule()
	{

	}
	// ������: NextFuncInfo
	// ���  : ������ 2010-7-1 9:20:42
	// ����  : virtual bool 
	// ����  : S_TBL_FUNC &funcinfo
	// ����  : �õ���һ��������Ϣ��ǰ����Ҫ����FirstFuncInfo
	virtual bool NextFuncInfo(S_TBL_FUNC &funcinfo)=0;

	// ������: FirstFuncInfo
	// ���  : ������ 2010-7-1 9:20:30
	// ����  : virtual bool 
	// ����  : S_TBL_FUNC &funcinfo
	// ����  : �õ���һ��������Ϣ
	virtual bool FirstFuncInfo(S_TBL_FUNC &funcinfo)=0;

	// ������: Run
	// ���  : ������ 2010-6-29 16:17:00
	// ����  : virtual int 
	// ����  : PBPCCOMMSTRU data
	// ����  : ����ԭ�ӹ���
	virtual int Run(PBPCCOMMSTRU data) =0;

	// ������: BuInit
	// ���  : ������ 2010-6-29 16:17:11
	// ����  : virtual bool 
	// ����  : ��ʼ�������ڴ˷�������һЩ���⴦����Ҫ��m_bIsInitΪtrue
	virtual bool BuInit()=0;


};

#endif // !defined(AFX_IBUMODULE_H__F6A715DF_DB67_4547_B40B_1259BA78EE15__INCLUDED_)
