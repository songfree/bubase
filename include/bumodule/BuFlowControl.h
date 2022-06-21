// BuFlowControl.h: interface for the CBuFlowControl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BUFLOWCONTROL_H__0AA03C29_25AA_4558_B3F6_2B99F595A1B7__INCLUDED_)
#define AFX_BUFLOWCONTROL_H__0AA03C29_25AA_4558_B3F6_2B99F595A1B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "public.h"
#include "BF_Xml.h"
#include "IPcLink.h"
#include "BF_SimpleLog.h"
#include "DbApi.h"

#ifdef BUMODULE_EXPORTS
#define BUMODULE_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define BUMODULE_EXPORT __declspec(dllimport)
#else
#define BUMODULE_EXPORT 
#endif
#endif

typedef struct _S_TBL_FLOWTX
{
	int         nFuncNo; //���ܺ� 
	std::string sFuncName;///��������
	char        cPrio;//���ȼ�
	char        cServiceFlag;//�����־
}S_TBL_FLOWTX;

class BUMODULE_EXPORT CBuFlowControl  
{
public:
	CBuFlowControl();
	virtual ~CBuFlowControl();

	// ������: SetBuPara
	// ���  : ������ 2010-6-30 11:00:30
	// ����  : void 
	// ����  : CBuXml *conxml
	// ����  : CICallBu *callbu
	// ����  : CDbApi *sqldb
	// ����  : CIBuLog *bulog
	// ����  : ���ò���
	virtual void SetBuPara(CBF_Xml *conxml,CIPcLink *callbu,CDbApi *sqldb,CBF_SimpleLog *bulog);
	
	
	// ������: InitFlow
	// ���  : ������ 2010-7-2 10:53:20
	// ����  : virtual bool true�ɹ�falseʧ��
	// ����  : ��ʼ�����̿���
	virtual bool InitFlow();
	
	
	// ������: IsRunFlow
	// ���  : ������ 2010-7-2 11:00:40
	// ����  : virtual bool 
	// ����  : int nTxcode
	// ����  : �Ƿ���Ҫ��������
	virtual bool IsRunFlow(int nTxcode);
	
	// ������:  RunFlow
	// ���  : ������ 2010-7-2 10:54:38
	// ����  : virtual int   =0���óɹ�--�ύ  <0����ʧ��--�ع� Ҫ������������ύ��ع�
	// ����  : PBPCCOMMSTRU data
	// ����  : ��������
	virtual int  RunFlow(PBPCCOMMSTRU data);
	
	
	// ������:  GetFlowFunc
	// ���  : ������ 2010-7-5 15:08:44
	// ����  : virtual int 
	// ����  : vector<S_TBL_FLOWTX> &txlist
	// ����  : ȡ�����̽�����Ϣ
	virtual int  GetFlowFunc(std::vector<S_TBL_FLOWTX> &txlist);

protected:
	CDbApi         *m_pSqlCmd;//���ݿ�api
	CBF_SimpleLog  *m_pLog;             //��־��
	CIPcLink       *m_pCallBu;          //�ڲ����ܵ���
	CBF_Xml        *m_pConfXml;         //����xml
	bool           m_bIsSetPara;
};
#undef BUMODULE_EXPORT
#endif // !defined(AFX_BUFLOWCONTROL_H__0AA03C29_25AA_4558_B3F6_2B99F595A1B7__INCLUDED_)
