// BuFuncMgr.h: interface for the CBuFuncMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BUFUNCMGR_H__B8471207_2203_46BC_BAA1_B2D787065AE2__INCLUDED_)
#define AFX_BUFUNCMGR_H__B8471207_2203_46BC_BAA1_B2D787065AE2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BuModule.h"
#include "BF_Xml.h"




#ifdef BUMODULE_EXPORTS
#define BUMODULE_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define BUMODULE_EXPORT __declspec(dllimport)
#else
#define BUMODULE_EXPORT 
#endif
#endif

class BUMODULE_EXPORT CBuFuncMgr  
{
public:
	char * GetMsg();


	// ������: Clear
	// ���  : ������ 2010-7-1 14:59:35
	// ����  : void 
	// ����  : ��չ��ܼ�ԭ�ӽ��ױ�ʵ������̬����
	void Clear();

	// ������: OpenDll
	// ���  : ������ 2010-7-1 14:58:05
	// ����  : bool 
	// ����  : DLLHANDLE *pDll  ���صĶ�̬����
	// ����  : const char *dllname  ��̬������
	// ����  : const char *dllpath  ��̬������Ŀ¼
	// ����  : �򿪶�̬�⣬���ض�̬��ľ��
	bool OpenDll(DLLHANDLE *pDll, const char *dllname,const char *dllpath);

	// ������: SetBuPara
	// ���  : ������ 2010-7-1 17:07:49
	// ����  : void 
	// ����  : CBFXml *conxml
	// ����  : CIPcLink *callbu
	// ����  : CDbApi *sqldb
	// ����  : CIBuLog *bulog
	// ����  : ����BU���в���
	void SetBuPara(CBF_Xml *conxml,CIPcLink *callbu,CDbApi *sqldb,CBF_SimpleLog *bulog);

	// ������: InitFuncInfo
	// ���  : ������ 2010-7-1 14:57:10
	// ����  : bool 
	// ����  : ��ʼ�����ܼ�ԭ�ӽ��ף�ע��ʹ��ǰ�ȵ���SetPara,���ظ�����
	bool InitFuncInfo();

	CBuFuncMgr();
	virtual ~CBuFuncMgr();

	CFuncTbl  m_pFuncTbl;     //����ע��Ĺ���(����)��
	CFuncTbl  m_pTxUnitTbl;   //����ע���ԭ�ӹ��ܱ�
	
protected:


	// ������: RegisterFunc
	// ���  : ������ 2010-7-1 14:59:07
	// ����  : bool 
	// ����  : S_TBL_FUNC funcinfo
	// ����  : ע�Ṧ�ܻ�ԭ�ӽ���
	bool RegisterFunc(S_TBL_FUNC funcinfo);

	
	CBF_SimpleLog  *m_pLog;
	CBF_Xml    *m_pConfigXml;//�����ļ�
	CIPcLink   *m_pExtCall;//�ⲿ����
	CDbApi    *m_pSqlCmd; //���ݿ�����

	
	char m_errMsg[256];
	
	vector <CBuModule *> m_alldllclass;
	vector <DLLHANDLE> m_allhandle;

};
#undef BUMODULE_EXPORT
#endif // !defined(AFX_BUFUNCMGR_H__B8471207_2203_46BC_BAA1_B2D787065AE2__INCLUDED_)
