// BuModule.h: interface for the CBuModule class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BUMODULE_H__0D5DA66A_18DD_4BA9_B754_79E1C9529474__INCLUDED_)
#define AFX_BUMODULE_H__0D5DA66A_18DD_4BA9_B754_79E1C9529474__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "IBuModule.h"
#include "BF_SimpleLog.h"
#include "FuncTbl.h"
#include "DbApi.h"
#include "BF_Xml.h"
#include "IPcLink.h"
#include "BpcHead.h"
#include "BF_Tools.h"
#include "XmlPackArray.h"
#include "dreberrcode.h"

#ifdef WIN32
#ifdef _DEBUG
#pragma comment(lib, "bf_oldxmld.lib") 
#pragma comment(lib, "bf_dbapid.lib") 
#pragma comment(lib, "bf_kerneld.lib") 
#pragma message("Automatically linking with  bf_kerneld.lib bf_dbapid.lib bf_oldxmld.lib ")
#else
#pragma comment(lib, "bf_oldxml.lib") 
#pragma comment(lib, "bf_dbapi.lib") 
#pragma comment(lib, "bf_kernel.lib") 
#pragma message("Automatically linking with  bf_kernel.lib  bf_dbapi.lib bf_oldxml.lib")
#endif
#endif

#ifdef BUMODULE_EXPORTS
#define BUMODULE_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define BUMODULE_EXPORT __declspec(dllimport)
#else
#define BUMODULE_EXPORT 
#endif
#endif

class BUMODULE_EXPORT CBuModule   : public CIBuModule
{
public:
	void BindOldFunc();

	// ������: BpuCall
	// ���  : ������ 2013-7-17 15:14:22
	// ����  : int  -1ʧ�� 0�ɹ�
	// ����  : PBPCCOMMSTRU data
	// ����  : LIST_BPC_RSMSG &ansdata
	// ����  : ���ñ�BPU�ڵĽ���
	int BpuCall(PBPCCOMMSTRU data,LIST_BPC_RSMSG &ansdata);

	// ������: BpcCall
	// ���  : ������ 2013-7-17 15:13:34
	// ����  : int -1ʧ�� 0�ɹ� 
	// ����  : PBPCCOMMSTRU data
	// ����  : LIST_BPC_RSMSG &ansdata
	// ����  : unsigned int ts
	// ����  : ����BPC��Ľ��ף���ȥ���ߡ�Ҫ��дdata���sDbHead���d_info���ֵ
	int BpcCall(PBPCCOMMSTRU data,LIST_BPC_RSMSG &ansdata,unsigned int ts);

	// ������: TransData
	// ���  : ������ 2013-7-17 15:12:20
	// ����  : int -1ʧ�� 0�ɹ� 
	// ����  : PBPCCOMMSTRU data
	// ����  : ��Ϣת�ƣ��������sDbHeadȫ���ƹ�����Ҫת�Ƶ�Ŀ����д��d_info��
	int TransData(PBPCCOMMSTRU data);

	// ������: ExtCall
	// ���  : ������ 2013-7-17 15:11:20
	// ����  : int -1ʧ�� 0�ɹ� 
	// ����  : PBPCCOMMSTRU data
	// ����  : LIST_BPC_RSMSG &ansdata
	// ����  : unsigned int ts
	// ����  : ���������Ľ���  Ҫ��дdata���sDbHead���d_info���ֵ
	int ExtCall(PBPCCOMMSTRU data,LIST_BPC_RSMSG &ansdata,unsigned int ts);

	// ������: AnsData
	// ���  : ������ 2013-7-17 14:37:46
	// ����  : int -1ʧ�� 0�ɹ� 
	// ����  : PBPCCOMMSTRU data
	// ����  : Ӧ�����ݣ�ֻ����Ӧ���־Ϊ1
	int AnsData(PBPCCOMMSTRU data);

	
	// ������: NextFuncInfo
	// ���  : ������ 2010-7-1 9:20:42
	// ����  : virtual bool 
	// ����  : S_TBL_FUNC &funcinfo
	// ����  : �õ���һ��������Ϣ��ǰ����Ҫ����FirstFuncInfo
	virtual bool NextFuncInfo(S_TBL_FUNC &funcinfo);
	
	// ������: FirstFuncInfo
	// ���  : ������ 2010-7-1 9:20:30
	// ����  : virtual bool 
	// ����  : S_TBL_FUNC &funcinfo
	// ����  : �õ���һ��������Ϣ
	virtual bool FirstFuncInfo(S_TBL_FUNC &funcinfo);

	// ������: BuInit
	// ���  : ������ 2010-6-29 16:17:11
	// ����  : virtual bool 
	// ����  : ��ʼ�������ڴ˷�������һЩ���⴦����Ҫ��m_bIsInitΪtrue
	virtual bool BuInit();
	
	// ������: Run
	// ���  : ������ 2010-6-29 16:17:00
	// ����  : virtual int 
	// ����  : PBPCCOMMSTRU data
	// ����  : ����ԭ�ӹ���
	virtual int Run(PBPCCOMMSTRU data);

	// ������: RunXml
	// ���  : ������ 2010-6-29 16:17:00
	// ����  : virtual int 
	// ����  : PBPCCOMMSTRU data
	// ����  : ͳһ���ü����Ͽ�����ԭ�ӷ��ӣ�ע��ʱ���е��Ͽ�ܵĽ��׶��󶨵��˷���
	virtual int RunXml(PBPCCOMMSTRU data);

	// ������: SetPuPara
	// ���  : ������ 2010-6-30 11:00:30
	// ����  : void 
	// ����  : CBuXml *conxml
	// ����  : CICallBu *callbu
	// ����  : CDbApi *sqldb
	// ����  : InitFuncInfo *bulog
	// ����  : ���ò���
	virtual void SetPuPara(CBF_Xml *conxml,CIPcLink *pclink,CDbApi *sqldb,CBF_SimpleLog *bulog);

	// ������: InitFuncInfo
	// ���  : ������ 2010-6-29 16:16:28
	// ����  : void 
	// ����  : int funcNo
	// ����  : FuncPointer pfunc
	// ����  : const char *funcName
	// ����  : double version
	// ����  : const char *author
	// ����  : int level
	// ����  : bool repealFlag
	// ����  : bool isTxUnit
	// ����  : bool isStop
	// ����  : ���ù�����Ϣ���ڹ������������
	void InitFuncInfo(int funcNo,FuncPointer pfunc,const char *funcName,double version,const char *author,int level,bool repealFlag,bool isTxUnit,bool isStop);

	/// ������: setFuncInfo
	/// ���  : ������ 2007-4-28 15:08:26
	/// ����  : void 
	/// ����  : int funcNo  ���ܺ�
	/// ����  : FuncPointer pfunc ����ָ��
	/// ����  : char *funcName  ��������
	/// ����  : double version  �汾�����ܺ���ͬ�Ļ�ֻʹ�����µİ汾
	/// ����  : char *author  ����
	/// ����  : int level     ����
	/// ����  : bool repealFlag   ������־
	/// ����  : bool isTxUnit     �Ƿ��׵�λ
	/// ����  : bool isStop       �����Ƿ�ֹͣ
	/// ����  : int appType       Ӧ������  �ɸ��ݴ����������Ӳ�ͬ�����ݿ�
	/// ����  : ���ù��ܵĲ����������Ͽ�ܵ�xmlpack�ķ���
	void setFuncInfo(int funcNo,FuncPointerOld pfunc,char *funcName,double version,char *author,int level,bool repealFlag,bool isTxUnit,bool isStop,int appType);
	
	
	CBuModule();
	virtual ~CBuModule();

	

protected:
	CBF_Xml       *m_pConfigXml;//�����ļ�
	CIPcLink      *m_pPcLink;//�ⲿ����
	CBF_SimpleLog *m_pLog;  //��־
	CDbApi        *m_pSqlCmd; //���ݿ�����
	bool           m_bIsInit;  //Init�������óɹ�����Ϊtrue
	bool           m_bIsSetPara;//�Ƿ������ò���
	CFuncTbl       m_pFuncTbl; //�����б�
	CFuncTblOld    m_pFuncTblOld;//��ż����Ͽ�ܵĽ�����Ϣ

	BPCCOMMSTRU    m_pAnsData; //������Ӧ�������ط���Ҫ�ã�������AnsData���������ֽ�����ᷢ���仯������ÿ��Ҫ������ֵ

	CXmlPack       m_xmlpack;
	CXmlPackArray  m_muxmlpack;
};
#undef  BUMODULE_EXPORT
#endif // !defined(AFX_BUMODULE_H__0D5DA66A_18DD_4BA9_B754_79E1C9529474__INCLUDED_)
