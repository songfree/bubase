// GateResource.h: interface for the CGateResource class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GATERESOURCE_H__B9E4A73C_7ADB_404E_A678_7919376804D6__INCLUDED_)
#define AFX_GATERESOURCE_H__B9E4A73C_7ADB_404E_A678_7919376804D6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BF_AIO_Resource.h"
#include "BF_Xml.h"
#include "ICertApiBase.h"
#include "CgateLog.h"
#include "BF_CommDataLog.h"


typedef struct
{
	std::string sIp;       //IP
	int         nPort;     //�˿�
	int         nBandwidth;//����
}S_DREBLINK_INFO;

class CGateResource : public CBF_AIO_Resource  
{
public:
	CGateResource();
	virtual ~CGateResource();
	
	unsigned int   g_nSvrMainId;     //Ŀ������ܺ� ,������д
	unsigned int   g_nSvrPrivateId;  //Ŀ��˽����� 0��ʾ���� >0Ϊÿ���������ţ�������д
	
	int   g_nUnCommpress;   //�ǲ��ǶԿͻ��˵����ݽ�ѹ��������
	int   g_nZipFlag;  //�����ͻ��˵������Ƿ�ѹ��
	int   g_nEncryptFlag;  //�����ͻ��˵������Ƿ����

	int   g_nUseMonitor;       //�Ƿ���
	int   g_nMonitorHost;      //�Ƿ񱨸�������Դ
	int   g_nMonitorDrebId;            //�����ڵ��
	int   g_nMonitorDrebPrivateId;     //˽�нڵ��
	int   g_nMonitorSvrId;            //��������ڵ��
	int   g_nMonitorSvrPrivateId;     //˽�з���ڵ��
	int   g_nMonitorTxCode;           //��ؽ�����

	int   g_nSendQueThreadNum;        //���鷢���߳���

	int   g_nMaxDataLogSize;          //data��־��С
	int   g_nDataLoglevel;            //data��־����
	int g_nCrcFlag; //crcУ���־  0��У�鲻����  1У�� ����

	int   g_nQuoSubScribe;//�Ƿ����鶩��   0���ö���  1�趩��


	int   g_nBroadCastMode;//�㲥ģʽ 0һֱ��  1����    2ֻ��

	CUInt  g_lBCFuncList;//�㲥���� �������б�  ��ֹ���ͻ�ֻ���͵�
	// ������: Init
	// ���  : ������ 2013-6-7 20:25:58
	// ����  : virtual bool 
	// ����  : const char *confile
	// ����  : ��ʼ�����������ļ�
	virtual bool Init(const char *confile);

	vector<S_DREBLINK_INFO> g_vDrebLinkInfo;
	CICertApiBase  *g_pCertClass;          //֤��api������
	int           g_nUseCert;//�Ƿ�ʹ��֤�� 0��ʹ�� 1ʹ��

	CBF_Xml  g_pXml;
	CCgateLog          g_pGateLog;//дcgate��־���߳���
	CBF_CommDataLog    g_pDrebLog;//дdreb��־���߳���

	// ������: GetMsg
	// ���  : ������ 2013-5-29 14:37:52
	// ����  : std::string 
	// ����  : ���س�����Ϣ
	std::string GetMsg();

	char  m_sStartDate[20];

private:
	// ������: OpenDll
	// ���  : ������ 2013-5-30 15:07:45
	// ����  : bool 
	// ����  : DLLHANDLE *pDll  ��̬���� 
	// ����  : const char *dllname
	// ����  : const char *dllpath
	// ����  : �򿪶�̬��
	bool OpenDll(DLLHANDLE *pDll, const char *dllname, const char *dllpath);

	char          m_sErrMsg[256];

	DLLHANDLE     m_pDll;
};

#endif // !defined(AFX_GATERESOURCE_H__B9E4A73C_7ADB_404E_A678_7919376804D6__INCLUDED_)
