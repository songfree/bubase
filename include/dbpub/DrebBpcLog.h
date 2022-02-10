// BpcLog.h: interface for the CBpcLog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BPCLOG_H__46E1F84A_FC45_417C_BDF5_2AAB80089AC3__INCLUDED_)
#define AFX_BPCLOG_H__46E1F84A_FC45_417C_BDF5_2AAB80089AC3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BF_Date_Time.h"
#include "BpcHead.h"



#ifdef _WINDOWS
#ifdef _DEBUG
#pragma comment(lib, "bf_kerneld.lib") 
#pragma message("Automatically linking with   bf_kerneld.lib ")
#else
#pragma comment(lib, "bf_kernel.lib") 
#pragma message("Automatically linking with   bf_kernel.lib ")
#endif

#endif

#ifdef DBPUB_EXPORTS
#define DBPUB_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define DBPUB_EXPORT __declspec(dllimport)
#else
#define DBPUB_EXPORT 
#endif
#endif

//��־�ļ���ʽ
//�ļ���ǰ2�ֽڱ�ʾ�ֽ������־����
//��һ�ֽ�   ȡֵ��B��L   B��ʾBIG_ENDIAN�ֽ���  �������aix hp_ux��ʹ�õ��ֽ���  L��ʾLITTLE_ENDIAN�ֽ��� linux��windowsʹ�õ��ֽ���
//�ڶ��ֽ�   ȡֵ��0��1   0��ʾdreb������־   1��ʾbpc������־
//�������ݼ����½ṹ
// 
// typedef struct   bpc������־
// {
// 	unsigned int nTime;//����ʱ��  hhmmss
// 	unsigned int nUs;  //΢��
// 	unsigned int nId;  //����ID
// 	BPCCOMMSTRU  data;
// }S_LOG_BPCDATA;
// 
// typedef struct   dreb������־
// {
// 	unsigned int nTime;//����ʱ��  hhmmss
// 	unsigned int nUs;  //΢��
// 	unsigned int nId;  //����ID
// 	COMMSTRU  data;
// }S_LOG_DREBDATA;



class DBPUB_EXPORT CDrebBpcLog  
{
public:

	// ������: LogDreb
	// ���  : ������ 2013-5-6 16:00:38
	// ����  : void 
	// ����  : int level
	// ����  : PBPCCOMMSTRU data
	// ����  : bool flag false������־  true������־
	// ����  : д��dreb����־
	void LogDreb(int level ,PBPCCOMMSTRU data,bool flag=false);

	// ������: LogDreb
	// ���  : ������ 2013-5-6 16:00:38
	// ����  : void 
	// ����  : int level
	// ����  : PCOMMSTRU data
	// ����  : bool flag false������־  true������־
	// ����  : д��dreb����־
	void LogDreb(int level ,PCOMMSTRU data,bool flag=false);

	// ������: SetLogPara
	// ���  : ������ 2013-5-6 16:00:11
	// ����  : void 
	// ����  : int level
	// ����  : const char *logpath
	// ����  : const char *iologname ������־��
	// ����  : const char *errlogname ������־��
	// ����  : ���ò���
	void SetLogPara(int level ,const char *logpath,const char *dreblogname,const char *dreberrlogname,const char *bpclogname,const char *bpcerrlogname);


	// ������: LogBpc
	// ���  : ������ 2013-5-6 16:00:20
	// ����  : void 
	// ����  : int level
	// ����  : PBPCCOMMSTRU data
	// ����  : bool flag  false������־  true������־
	// ����  : д��BPC����־
	void LogBpc(int level,PBPCCOMMSTRU data,bool flag=false);

	CDrebBpcLog();
	virtual ~CDrebBpcLog();

protected:
	std::string m_sLogPath;
	std::string m_sDrebLogName;
	std::string m_sDrebLogErrName;
	std::string m_sBpcLogName;
	std::string m_sBpcLogErrName;
	int         m_nLogLevel;
	CBF_Date_Time  m_pDate;
	int         m_nBpcHeadLen;

};

#endif // !defined(AFX_BPCLOG_H__46E1F84A_FC45_417C_BDF5_2AAB80089AC3__INCLUDED_)
