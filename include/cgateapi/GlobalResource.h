// GlobalResource.h: interface for the CGlobalResource class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLOBALRESOURCE_H__90EA415C_F2CC_4710_B1B8_DC6B55202635__INCLUDED_)
#define AFX_GLOBALRESOURCE_H__90EA415C_F2CC_4710_B1B8_DC6B55202635__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BF_SocketTcp.h"
#include "BF_LogClient.h"
#include "BF_BufferPool.h"
#include "ICertApiBase.h"
#include "ApiFlowInfo.h"
#include "BF_Xml.h"
#include "BF_Tools.h"
#include "ICertApiBase.h"
#include "CgateLog.h"

typedef struct 
{
	std::string   sIp;
	unsigned int  nPort;
}GATE_IPPORT;

#ifdef CGATEAPI_EXPORTS
#define CGATEAPI_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define CGATEAPI_EXPORT __declspec(dllimport)
#else
#define CGATEAPI_EXPORT 
#endif
#endif

class CGATEAPI_EXPORT CGlobalResource  
{
public:

	// ������: GetMsg
	// ���  : ������ 2013-5-29 14:37:52
	// ����  : std::string 
	// ����  : ���س�����Ϣ
	std::string GetMsg();

	// ������: IsControlTx
	// ���  : ������ 2013-5-27 17:41:48
	// ����  : bool 
	// ����  : �Ƿ�ﵽ���ͽ���������
	bool IsControlTx();

	// ������: IsControlBytes
	// ���  : ������ 2013-5-27 17:41:30
	// ����  : bool 
	// ����  : �Ƿ�ﵽ�����ֽ�������
	bool IsControlBytes();

	// ������: Init
	// ���  : ������ 2013-5-27 17:17:38
	// ����  : bool 
	// ����  : const char *apixml
	// ����  : �������ļ���ʼ��
	bool Init(const char *apixml);


	// ������: OpenDll
	// ���  : ������ 2013-5-30 15:07:45
	// ����  : bool 
	// ����  : DLLHANDLE *pDll  ��̬���� 
	// ����  : const char *dllname
	// ����  : const char *dllpath
	// ����  : �򿪶�̬��
	bool OpenDll(DLLHANDLE *pDll, const char *dllname, const char *dllpath);

	CGlobalResource();
	virtual ~CGlobalResource();

	CBF_LogClient  g_pLog;
	CCgateLog      g_pCgateLog;
	CApiFlowInfo   g_pApiFlowInfo;        
	CICertApiBase  *g_pCertClass;          //֤��api������

	CBF_Xml       g_pXml;
	unsigned int  g_nFlowControlSendBytes;//�������Ʒ����ֽ���/��
	unsigned int  g_nFlowControlSendTxNum;//�������Ʒ��ͽ�����/��
	int           g_nHeartTime; //����ʱ��  ���೤ʱ��δ�յ���������������������
	int           g_nCloseTime;//����δ���Ͽ�ʱ��
	int           g_nLogLevel;//��־����
	std::string   g_sLogPath; //��־·��
	std::string   g_sLogName; //��־�ļ���
	int           g_nMaxLogSize;//��־��С
	int           g_nMaxDataLogSize;//data��־��С
	int           g_nDataLogLevel;//data��־����
	int           g_nCrcFlag; //crcУ���־  0��У�鲻����  1У�� ����

	int           g_nZipFlag; //�Ƿ�ѹ��  0��ѹ��1ѹ��

	int           g_nUseCert;//�Ƿ�ʹ��֤�� 0��ʹ�� 1ʹ��

	char          g_sCurPath[300];

	int           g_nConnGateTimeOut;//�������س�ʱʱ��

	unsigned int  g_nTimeOutSend; //�������ݳ�ʱʱ�� �� �������ڶ����ﳬ����ʱ��󲻷���
	unsigned int  g_nDataSendNum; //���ݷ��ʹ���
	
	vector<GATE_IPPORT>  g_lGateList; //����IP��ַ�˿��б�
	
private:
	char          m_sErrMsg[256];
	DLLHANDLE     m_pDll;

};

#endif // !defined(AFX_GLOBALRESOURCE_H__90EA415C_F2CC_4710_B1B8_DC6B55202635__INCLUDED_)
