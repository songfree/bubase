// BF_DrebResource.cpp: implementation of the CBF_DrebResource class.
//
//////////////////////////////////////////////////////////////////////

#include "BF_DrebResource.h"
#include "IErrLog.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBF_DrebResource::CBF_DrebResource()
{
	//��־����
	g_nLoglevel = LOG_DEBUG;//��־���� Ĭ��Ϊ5
	g_nDrebLoglevel = LOG_PROMPT;//д��data�������־����
	g_nMaxLogSize = 0;

	g_nEndianFlag = 0;
	
	//������������
	g_nDisconnectTime = 600;//δʹ�öϿ�ʱ�䣬���һ�������ڴ�ʱ����һֱû��ʹ�ã��򽫴����ӶϿ�����λ��Ĭ��600�뼴10����
	g_nHeartRun = 5;//����ʱ��,���������ӳ�����ʱ��δ��ʱ����������������Ĭ��Ϊ5�� 
	
	//�������ͼ�ص���Ϣ
	g_nUseMonitor = 1;       //�Ƿ���
	g_nMonitorHost=1;      //�Ƿ񱨸�������Դ
	g_nMonitorDrebId=0;            //�����ڵ��
	g_nMonitorDrebPrivateId=0;     //˽�нڵ��
	g_nMonitorSvrId=0;            //��������ڵ��
	g_nMonitorSvrPrivateId=0;     //˽�з���ڵ��
	g_nMonitorTxCode=99001;           //��ؽ�����
	g_nQueueSize=1000;

	g_nCpuCore=0;
	//��������ע��Ĺ�������ź�˽�з����
	g_nSvrMainId=0;     //Ŀ������ܺ� ,������д
	g_nSvrPrivateId=0;  //Ŀ��˽����� 0��ʾ���� >0Ϊÿ���������ţ�������д
	
	g_nBcSerialDeleteTime = 10;//�ظ��Ĺ㲥��10��ɾ��
	bzero(g_sStartDateTime,sizeof(g_sStartDateTime));  //������д���ɳ������е���
	
	//��������Ϊ��������main�����GetModuleNamePathȡ�����г������Ϣ
	bzero(g_sCurPath,sizeof(g_sCurPath));
	bzero(g_sModulePath,sizeof(g_sModulePath));
	bzero(g_sModuleName,sizeof(g_sModuleName));

	bzero(&g_DrebRcvSendStat,sizeof(g_DrebRcvSendStat));
	bzero(&g_DrebRcvSendSec,sizeof(g_DrebRcvSendSec));
	

}

CBF_DrebResource::~CBF_DrebResource()
{

}

void CBF_DrebResource::ReportRcv(unsigned int rcvbyte, unsigned int packnum)
{
	CBF_PMutex pp(&m_reportMutex);
	g_DrebRcvSendStat.g_nRcvBytes+=rcvbyte;
	g_DrebRcvSendStat.g_nRcvPackNum+=packnum;
}

void CBF_DrebResource::ReportSend(unsigned int sendbytes, unsigned int sendpack)
{
	CBF_PMutex pp(&m_reportMutex);
	g_DrebRcvSendStat.g_nSendBytes+=sendbytes;
	g_DrebRcvSendStat.g_nSendPackNum+=sendpack;
}
// bool CBF_DrebResource::Init(const char *confile)
// {
// 	if (confile == NULL)
// 	{
// 		printf("�����ļ�Ϊ��\n");
// 		m_errMsg = "�����ļ�Ϊ��";
// 		return false;
// 	}
// 	
// 	if (!m_pXml.FromFile(confile))
// 	{
// 		printf("�����ļ��Ƿ�,�����ļ�[%s]\n",confile);
// 		m_errMsg = "���������ļ��Ƿ�";
// 		return false;
// 	}
// 	CBF_Date_Time pdata;
// 	CBF_Tools::StringCopy(g_sStartDateTime,17,pdata.ToStringDT17().c_str());
// 
// 	int ret=0;
// 	std::string stmp;
// 	
// 	if (m_pXml.GetNodeValueByPath("package/head/��������/��־����",false,g_nLoglevel) == NULL)
// 	{
// 		m_errMsg = "�����ļ���,��[package/head/��������/��־����]�ڵ�";
// 		return false;
// 	}
// 	if (g_nLoglevel <0 )
// 	{
// 		g_nLoglevel = LOG_DEBUG;//���ô���Ϊ5
// 	}
// 	if (m_pXml.GetNodeValueByPath("package/head/��������/DATA��־����",false,g_nDrebLoglevel) == NULL)
// 	{
// 		m_errMsg = "�����ļ���,��[package/head/��������/DATA��־����]�ڵ�";
// 		return false;
// 	}
// 	if (g_nDrebLoglevel <0 )
// 	{
// 		g_nDrebLoglevel = LOG_DEBUG;//���ô���Ϊ5
// 	}
// 
// 	if (m_pXml.GetNodeValueByPath("package/head/��������/��־�ļ�",false,g_sLogFileName) == NULL)
// 	{
// 		m_errMsg = "�����ļ���,��[package/head/��������/��־�ļ�]�ڵ�";
// 		return false;
// 	}
// 	if (m_pXml.GetNodeValueByPath("package/head/��������/��־Ŀ¼",false,g_sLogFilePath) == NULL)
// 	{
// 		m_errMsg = "�����ļ���,��[package/head/��������/��־Ŀ¼]�ڵ�";
// 		g_sLogFilePath="";
// 	}
// 	
// 	if (m_pXml.GetNodeValueByPath("package/head/��������/��־��С",false,ret) == NULL)
// 	{
// 		m_errMsg = "�����ļ���,��[package/head/��������/��־��С]�ڵ�";
// 		g_nMaxLogSize=0;
// 	}
// 	else
// 	{
// 		g_nMaxLogSize = ret;
// 	}
// 
// 	if (m_pXml.GetNodeValueByPath("package/head/��������/�ֽ�����",false,ret) == NULL)
// 	{
// 		m_errMsg = "�����ļ���,��[package/head/��������/�ֽ�����]�ڵ�";
// 		g_nEndianFlag=0;
// 	}
// 	else
// 	{
// 		g_nEndianFlag = ret;
// 	}
// 	if (g_nEndianFlag != 1)
// 	{
// 		g_nEndianFlag = 0;
// 	}
// 	
// 	if (m_pXml.GetNodeValueByPath("package/head/BPC����/���������",false,ret) == NULL)
// 	{
// 		m_errMsg = "�����ļ���,��[package/head/BPC����/���������]�ڵ�";
// 		return false;
// 	}
// 	if (ret<1)
// 	{
// 		m_errMsg = "�����ļ���,��[package/head/BPC����/���������]�ڵ����ò���";
// 		return false;
// 	}
// 	g_nSvrMainId = ret;
// 	if (m_pXml.GetNodeValueByPath("package/head/BPC����/˽�з����",false,ret) == NULL)
// 	{
// 		m_errMsg = "�����ļ���,��[package/head/BPC����/˽�з����]�ڵ�";
// 		return false;
// 	}
// 	if (ret<1 || ret>254)
// 	{
// 		m_errMsg = "�����ļ���,��[package/head/BPC����/˽�з����]�ڵ����ò���";
// 		return false;
// 	}
// 	g_nSvrPrivateId = ret;
// 
// 
// 	if (m_pXml.GetNodeValueByPath("package/head/BPC����/DREB����ʱ��",false,g_nHeartRun) == NULL)
// 	{
// 		m_errMsg = "�����ļ�,��[package/head/BPC����/DREB����ʱ��]�ڵ�";
// 		g_nHeartRun = 5;
// 	}
// 	if (m_pXml.GetNodeValueByPath("package/head/BPC����/���ӹ���ʱ��",false,g_nDisconnectTime) == NULL)
// 	{
// 		m_errMsg = "�����ļ�,��[package/head/BPC����/���ӹ���ʱ��]�ڵ�";
// 		g_nDisconnectTime = 600;
// 	}
// 	g_nUseMonitor=0;
// 	if (m_pXml.GetNodeAttribute("package/head/�������",false,"use",false,g_nUseMonitor) == NULL)
// 	{
// 		m_errMsg = "��package/head/�������";
// 		g_nUseMonitor = 0;
// 	}
// 	if (g_nUseMonitor == 1)
// 	{
// 		if (m_pXml.GetNodeAttribute("package/head/�������",false,"host",false,g_nMonitorHost) == NULL)
// 		{
// 			m_errMsg = "��package/head/�������";
// 			g_nMonitorHost = 1;
// 		}
// 		if (m_pXml.GetNodeValueByPath("package/head/�������/��ع����ڵ��",false,g_nMonitorDrebId) == NULL)
// 		{
// 			m_errMsg = "�ڵ�[package/head/�������/��ع����ڵ��]û������";
// 			g_nMonitorDrebId = 0;
// 		}
// 		if (m_pXml.GetNodeValueByPath("package/head/�������/���˽�нڵ��",false,g_nMonitorDrebPrivateId) == NULL)
// 		{
// 			m_errMsg = "�ڵ�[package/head/�������/���˽�нڵ��]û������";
// 			g_nMonitorDrebPrivateId = 0;
// 		}
// 		if (m_pXml.GetNodeValueByPath("package/head/�������/��ع��������",false,g_nMonitorSvrId) == NULL)
// 		{
// 			m_errMsg = "�ڵ�[package/head/�������/��ع��������]û������";
// 			g_nMonitorSvrId = 0;
// 		}
// 		if (m_pXml.GetNodeValueByPath("package/head/�������/���˽�з����",false,g_nMonitorSvrPrivateId) == NULL)
// 		{
// 			m_errMsg = "�ڵ�[package/head/�������/���˽�з����]û������";
// 			g_nMonitorSvrPrivateId = 0;
// 		}
// 		if (m_pXml.GetNodeValueByPath("package/head/�������/��ر��潻����",false,g_nMonitorTxCode) == NULL)
// 		{
// 			m_errMsg = "�ڵ�[package/head/�������/��ر��潻����]û������";
// 			return false;
// 		}
// 	}
// 	CXmlNode *drebnode = m_pXml.GetNodeByPath("package/head/DREB����",false);
// 	if (drebnode == NULL)
// 	{
// 		m_errMsg = "�����ļ���,��[package/head/DREB����]�ڵ�";
// 		return false;
// 	}
// 	CXmlNode *tmpnode = (CXmlNode *)drebnode->GetFirstChild();
// 	S_DREBLINK_INFO dreblinkinfo;
// 	while (tmpnode != NULL)
// 	{
// 		tmpnode->GetAttribute("ip",false,dreblinkinfo.sIp);
// 		tmpnode->GetAttribute("port",false,stmp);
// 		dreblinkinfo.nPort = atoi(stmp.c_str());
// 		tmpnode->GetAttribute("bandwidth",false,stmp);
// 		dreblinkinfo.nBandwidth = atoi(stmp.c_str());
// 		g_vDrebLinkInfo.push_back(dreblinkinfo);
// 		tmpnode = (CXmlNode *)tmpnode->getNextSibling();
// 	}
// 	return true;
// }

void CBF_DrebResource::ComputeStat()
{
	CBF_PMutex pp(&m_reportMutex);
	//����
	g_DrebRcvSendSec.g_nRcvBytes = g_DrebRcvSendStat.g_nRcvBytes  - m_oldDrebStatInfo.g_nRcvBytes;
	g_DrebRcvSendSec.g_nRcvPackNum =g_DrebRcvSendStat.g_nRcvPackNum  - m_oldDrebStatInfo.g_nRcvPackNum;
	g_DrebRcvSendSec.g_nSendBytes =g_DrebRcvSendStat.g_nSendBytes  - m_oldDrebStatInfo.g_nSendBytes;
	g_DrebRcvSendSec.g_nSendPackNum =g_DrebRcvSendStat.g_nSendPackNum  - m_oldDrebStatInfo.g_nSendPackNum;
	//���浽�ɵĽṹ
	memcpy(&m_oldDrebStatInfo,&(g_DrebRcvSendStat),sizeof(S_DREBSTAT_INFO));
}
