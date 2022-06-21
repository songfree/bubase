// Resource.h: interface for the CResource class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RESOURCE_H__FF782DDA_47F8_4B57_961C_BF9F48B676FC__INCLUDED_)
#define AFX_RESOURCE_H__FF782DDA_47F8_4B57_961C_BF9F48B676FC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BF_Xml.h"
#include "BF_SimpleLog.h"
#include "BF_Date_Time.h"
#include "BF_Tools.h"
#include "MdbBase.h"
#include "MoniHead.h"
#include "BF_ProcessMgr.h"

#define  HEARTHEAD    8   //����������ͨѶͷ����

class CResource  
{
public:
	CResource();
	virtual ~CResource();

	bool g_bIsExit;

	unsigned int   g_nSvrMainId;     //Ŀ������ܺ� ,������д
	unsigned int   g_nSvrPrivateId;  //Ŀ��˽����� 0��ʾ���� >0Ϊÿ���������ţ�������д

	int   g_nUseMonitor;       //�Ƿ���
	int   g_nMonitorHost;      //�Ƿ񱨸�������Դ
	int   g_nMonitorDrebId;            //�����ڵ��
	int   g_nMonitorDrebPrivateId;     //˽�нڵ��
	int   g_nMonitorSvrId;            //��������ڵ��
	int   g_nMonitorSvrPrivateId;     //˽�з���ڵ��
	int   g_nMonitorTxCode;           //��ؽ�����
	
	char              g_sCurPath[300]; //��ǰ����Ŀ¼
	char              g_sModulePath[300];//��������Ŀ¼
	char              g_sModuleName[300];//������

	char              m_sStartDate[20];//��������
	
	int      g_nCrcFlag;//�Ƿ�У��CRC

	int g_nLoglevel;//��־���� Ĭ��Ϊ5
	std::string g_sLogFileName;//��־�ļ���
	std::string g_sLogFilePath;//��־�ļ�Ŀ¼
	int g_nHeartRun;//����ʱ��,���������ӳ�����ʱ��δ��ʱ��������������
	int g_nDisconnectTime;//δʹ�öϿ�ʱ�䣬���һ�������ڴ�ʱ����һֱû��ʹ�ã��򽫴����ӶϿ�����λ��Ĭ��600�뼴10����
	int g_nGwDisTime;  //ת������δʹ�öϿ�ʱ��

	int g_nPort;   //�����˿�

	CBF_Xml  g_pXml; //xml������
	CBF_SimpleLog  g_pLog;
	CBF_Date_Time  m_pDate;

	CBF_Xml  g_pRunXml; //�������������Ϣ����PID��xml�ļ�
	char     m_sErrMsg[256]; //������Ϣ

	std::vector<S_DREBLINK_INFO> g_vDrebLinkInfo;//dreb�����б�
	std::vector<S_PROCESS_NODE>  g_vProcessInfo; //����Ľ�����Ϣ
	
public:
	bool UpdateLastTimeById(std::string nodeid);

	// ������: Init
	// ���  : ������ 2013-12-10 9:46:25
	// ����  : bool 
	// ����  : const char *confile
	// ����  : �������ļ�
	bool Init(const char *confile);


protected:
	
};

#endif // !defined(AFX_RESOURCE_H__FF782DDA_47F8_4B57_961C_BF9F48B676FC__INCLUDED_)
