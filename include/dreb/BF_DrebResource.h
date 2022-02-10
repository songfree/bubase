// BF_DrebResource.h: interface for the CBF_DrebResource class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BF_DREBRESOURCE_H__D94AB550_73E8_47AE_92EB_FB23D818A824__INCLUDED_)
#define AFX_BF_DREBRESOURCE_H__D94AB550_73E8_47AE_92EB_FB23D818A824__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "typedefine.h"
#include "BF_Xml.h"
#include "BF_Tools.h"
#include "IErrLog.h"
#include "BF_SimpleLog.h"
#include "BF_Mutex.h"

typedef struct
{
	std::string sIp;       //IP
	int         nPort;     //�˿�
	int         nBandwidth;//����
	bool        bIsIpc;
}S_DREBLINK_INFO;

typedef struct 
{
	UINT64_  g_nRcvBytes;//�����ֽ���
	UINT64_  g_nRcvPackNum;//�������ݰ���
	UINT64_  g_nSendBytes;//�����ֽ���
	UINT64_  g_nSendPackNum;//�������ݰ���
	
}S_DREBSTAT_INFO;  //�������ߵ�ͳ������

#ifdef DREBAPI_EXPORTS
#define DREBAPI_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define DREBAPI_EXPORT __declspec(dllimport)
#else
#define DREBAPI_EXPORT 
#endif
#endif

class DREBAPI_EXPORT CBF_DrebResource 
{
public:
	

	CBF_DrebResource();
	virtual ~CBF_DrebResource();

	CBF_SimpleLog *log;

	//��־����
	int g_nLoglevel;//��־���� Ĭ��ΪLOG_DEBUG
	int g_nDrebLoglevel;//д��data�������־����   Ĭ��ΪLOG_PROMPT
	std::string g_sLogFileName;//��־�ļ���
	std::string g_sLogFilePath;//��־�ļ�Ŀ¼
	unsigned int  g_nMaxLogSize;//��־����С 0������

	int           g_nCrcFlag; //crcУ���־  0��У�鲻����  1У�� ����

	unsigned int  g_nEndianFlag; //��Ե��÷��ֽ������־,0���������ʺ�C/C++.  1ʹ��������(SNEDMSG��getmsg)ʱʹ�������������� �ʺ�JAVA����(java���ô����ֽ���)

	//������������
	int g_nDisconnectTime;//δʹ�öϿ�ʱ�䣬���һ�������ڴ�ʱ����һֱû��ʹ�ã��򽫴����ӶϿ�����λ��Ĭ��600�뼴10����
	int g_nHeartRun;//����ʱ��,���������ӳ�����ʱ��δ��ʱ����������������Ĭ��Ϊ5�� 

	//�������ͼ�ص���Ϣ
	int   g_nUseMonitor;       //�Ƿ��� Ĭ��Ϊ1
	int   g_nMonitorHost;      //�Ƿ񱨸�������Դ Ĭ��Ϊ1
	int   g_nMonitorDrebId;            //�����ڵ�� Ĭ��Ϊ0
	int   g_nMonitorDrebPrivateId;     //˽�нڵ�� Ĭ��Ϊ0
	int   g_nMonitorSvrId;            //��������ڵ�� Ĭ��Ϊ0
	int   g_nMonitorSvrPrivateId;     //˽�з���ڵ�� Ĭ��Ϊ0
	int   g_nMonitorTxCode;           //��ؽ�����     Ĭ��Ϊ99001

	//��������ע��Ĺ�������ź�˽�з����
	unsigned int   g_nSvrMainId;     //Ŀ������ܺ� ,������д Ĭ��Ϊ0
	unsigned int   g_nSvrPrivateId;  //Ŀ��˽����� 0��ʾ���� >0Ϊÿ���������ţ�������д Ĭ��Ϊ0
	
	//Ҫ���ӵ�dreb��Ϣ
	vector<S_DREBLINK_INFO> g_vDrebLinkInfo;  //Ҫ���ӵ�dreb��Ϣ
	
	
	

	//��������Ϊ��������main�����GetModuleNamePathȡ�����г������Ϣ
	char g_sCurPath[300];     //��ǰĿ¼
	char g_sModulePath[300];  //����Ŀ¼
	char g_sModuleName[300];  //��������

	//����Ϊ��api����ʱ��д����ʹ��
	char g_sStartDateTime[20];  //������д���ɳ���������д
	
	S_DREBSTAT_INFO  g_DrebRcvSendSec;//ÿ�뷢�ͽ�������
	

	void ReportSend(unsigned int sendbytes,unsigned int sendpack=1);
	void ReportRcv(unsigned int rcvbyte,unsigned int packnum=1);
	void ComputeStat();

	std::string m_errMsg;//������Ϣ
	CBF_Xml     m_pXml;//xml����


	virtual bool Init(const char *confile)
	{
		if (confile == NULL)
		{
			printf("file is null\n");
			m_errMsg = "file is null";
			return false;
		}
		
		if (!m_pXml.FromFile(confile))
		{
			printf("file [%s] error\n",confile);
			m_errMsg = "file error";
			return false;
		}
		CBF_Date_Time pdata;
		CBF_Tools::StringCopy(g_sStartDateTime,17,pdata.ToStringDT17().c_str());

		int ret=0;
		std::string stmp;
		
		if (m_pXml.GetNodeValueByPath("package/head/public/loglevel",false,g_nLoglevel) == NULL)
		{
			m_errMsg = "file error,no [package/head/public/loglevel] node";
			return false;
		}
		if (g_nLoglevel <0 )
		{
			g_nLoglevel = LOG_DEBUG;//���ô���Ϊ5
		}
		if (m_pXml.GetNodeValueByPath("package/head/public/dataloglevel",false,g_nDrebLoglevel) == NULL)
		{
			m_errMsg = "file error,no [package/head/public/dataloglevel] node";
			return false;
		}
		if (g_nDrebLoglevel <0 )
		{
			g_nDrebLoglevel = LOG_DEBUG;//���ô���Ϊ5
		}

		if (m_pXml.GetNodeValueByPath("package/head/public/logfile",false,g_sLogFileName) == NULL)
		{
			m_errMsg = "file error,no [package/head/public/logfile] node";
			return false;
		}
		if (m_pXml.GetNodeValueByPath("package/head/public/logdir",false,g_sLogFilePath) == NULL)
		{
			m_errMsg = "file error,no [package/head/public/logdir] node";
			g_sLogFilePath="";
		}
		if (m_pXml.GetNodeValueByPath("package/head/public/CRC",false,g_nCrcFlag) == NULL)
		{
			g_nCrcFlag=0;
		}
		if (m_pXml.GetNodeValueByPath("package/head/public/logsize",false,ret) == NULL)
		{
			m_errMsg = "file error,no [package/head/public/logsize] node";
			g_nMaxLogSize=0;
		}
		else
		{
			g_nMaxLogSize = ret;
		}

		if (m_pXml.GetNodeValueByPath("package/head/public/endian",false,ret) == NULL)
		{
			m_errMsg = "file error,no [package/head/public/endian] node";
			g_nEndianFlag=0;
		}
		else
		{
			g_nEndianFlag = ret;
		}
		if (g_nEndianFlag != 1)
		{
			g_nEndianFlag = 0;
		}
		
		if (m_pXml.GetNodeValueByPath("package/head/bpc/svrid",false,ret) == NULL)
		{
			m_errMsg = "file error,no [package/head/bpc/svrid] node";
			return false;
		}
		if (ret<1)
		{
			m_errMsg = "file error,[package/head/bpc/svrid] is illegal";
			return false;
		}
		g_nSvrMainId = ret;
		if (m_pXml.GetNodeValueByPath("package/head/bpc/svrpid",false,ret) == NULL)
		{
			m_errMsg = "file error,no [package/head/bpc/svrpid] node";
			return false;
		}
		if (ret<1 || ret>254)
		{
			m_errMsg = "file error [package/head/bpc/svrpid] is illegal";
			return false;
		}
		g_nSvrPrivateId = ret;


		if (m_pXml.GetNodeValueByPath("package/head/bpc/hearttime",false,g_nHeartRun) == NULL)
		{
			m_errMsg = "file error,no [package/head/bpc/hearttime] node";
			g_nHeartRun = 5;
		}
		if (m_pXml.GetNodeValueByPath("package/head/bpc/due_times",false,g_nDisconnectTime) == NULL)
		{
			m_errMsg = "file error,no [package/head/bpc/due_times] node";
			g_nDisconnectTime = 600;
		}
		g_nUseMonitor=0;
		if (m_pXml.GetNodeAttribute("package/head/monitor",false,"use",false,g_nUseMonitor) == NULL)
		{
			m_errMsg = "file error,no [package/head/monitor] node";
			g_nUseMonitor = 0;
		}
		if (g_nUseMonitor == 1)
		{
			if (m_pXml.GetNodeAttribute("package/head/monitor",false,"host",false,g_nMonitorHost) == NULL)
			{
				m_errMsg = "file error,no [package/head/monitor] host attribute";
				g_nMonitorHost = 1;
			}
			if (m_pXml.GetNodeValueByPath("package/head/monitor/drebid",false,g_nMonitorDrebId) == NULL)
			{
				m_errMsg = "file error,no [package/head/monitor/drebid] node";
				g_nMonitorDrebId = 0;
			}
			if (m_pXml.GetNodeValueByPath("package/head/monitor/drebpid",false,g_nMonitorDrebPrivateId) == NULL)
			{
				m_errMsg = "file error,no [package/head/monitor/drebpid] node";
				g_nMonitorDrebPrivateId = 0;
			}
			if (m_pXml.GetNodeValueByPath("package/head/monitor/svrid",false,g_nMonitorSvrId) == NULL)
			{
				m_errMsg = "file error,no [package/head/monitor/svrid] node";
				g_nMonitorSvrId = 0;
			}
			if (m_pXml.GetNodeValueByPath("package/head/monitor/svrpid",false,g_nMonitorSvrPrivateId) == NULL)
			{
				m_errMsg = "file error,no [package/head/monitor/svrpid] node";
				g_nMonitorSvrPrivateId = 0;
			}
			if (m_pXml.GetNodeValueByPath("package/head/monitor/txcode",false,g_nMonitorTxCode) == NULL)
			{
				m_errMsg = "file error,no [package/head/monitor/txcode] node";
				return false;
			}
		}
		CXmlNode *drebnode = m_pXml.GetNodeByPath("package/head/dreb",false);
		if (drebnode == NULL)
		{
			m_errMsg = "file error,no [package/head/dreb] node";
			return false;
		}
		CXmlNode *tmpnode = (CXmlNode *)drebnode->GetFirstChild();
		S_DREBLINK_INFO dreblinkinfo;
		while (tmpnode != NULL)
		{
			tmpnode->GetAttribute("ip",false,dreblinkinfo.sIp);
			tmpnode->GetAttribute("port",false,stmp);
			dreblinkinfo.nPort = atoi(stmp.c_str());
			tmpnode->GetAttribute("bandwidth",false,stmp);
			dreblinkinfo.nBandwidth = atoi(stmp.c_str());
			g_vDrebLinkInfo.push_back(dreblinkinfo);
			tmpnode = (CXmlNode *)tmpnode->getNextSibling();
		}
		return true;
	}
protected:
	CBF_Mutex        m_reportMutex;
	S_DREBSTAT_INFO  m_oldDrebStatInfo;//����֮ǰ�Ľ��շ�����Ϣ
	S_DREBSTAT_INFO  g_DrebRcvSendStat;//���ͽ����ۼ�����
	
};

#endif // !defined(AFX_BF_DREBRESOURCE_H__D94AB550_73E8_47AE_92EB_FB23D818A824__INCLUDED_)
