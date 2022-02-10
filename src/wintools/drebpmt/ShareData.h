// ShareData.h: interface for the CShareData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHAREDATA_H__1120841C_B2D8_4915_9C85_8F7B8B53711C__INCLUDED_)
#define AFX_SHAREDATA_H__1120841C_B2D8_4915_9C85_8F7B8B53711C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "public.h"
#include "BF_Mutex.h"
#include "BF_Xml.h"
#include "IVarDefine.h"

struct SERVER_PARA
{
	unsigned int timeout;//��ʱ
	unsigned int timesep;//���ͼ��
	unsigned int runtime;//����ʱ��
	unsigned int connectNum;//���Ӵ���
	unsigned int loglevel;//��־����
	unsigned int rrtime;//��¼��������Ӧʱ��ļ�¼
};

typedef struct _THREAD_DATA_PARA
{
	std::string sdrebip;//����ip
	unsigned int nPort;//�˿�
	unsigned short nNodeId; //dreb�����ڵ�
	unsigned int   cNodePrivate;//dreb˽�нڵ�
	unsigned int   nSvrId;//���������
	unsigned int   cSvrPrivate;//˽�з����
	unsigned int   nServiceNo;//�����ܺ�
	unsigned int   nConnectType;//��������
	unsigned int   nThreadNum;//�����߳���
	unsigned int   cZip;//���ݱ�־
	unsigned int   nStartTime;//����ʱ���
	int            nCmd;//����������
	unsigned int   nSendSep;//���ͼ��
};

class CShareData  
{
public:
	CShareData();
	virtual ~CShareData();

public:
	int GetSerial();

	// ������: InitVar
	// ���  : ������ 2010-9-30 15:25:08
	// ����  : bool 
	// ����  : char *varxml
	// ����  : ��ʼ����
	bool InitVar(char *varxml);
	void Clear();

	// ������: Run
	// ���  : ������ 2010-9-30 15:24:52
	// ����  : void 
	// ����  : bool isstop
	// ����  : int sendnum
	// ����  : int sendbyte
	// ����  : int rcvnum
	// ����  : int rcvbyte
	// ����  : int errnum
	// ����  : int resptime
	// ����  : ��������״��
	void Run(bool isstop,int sendnum,int sendbyte,int rcvnum,int rcvbyte,int errnum, INT64_ resptime);

	int m_nAllRunTime;//������ʱ��
	int m_nCurRunTime;//��ǰ����ʱ��
	int m_nTotalThreadNum;//���߳���
	int m_nSendNum;//���ͱ���
	int m_nRcvNum;//���ձ���
	int m_nErrNum;//�������
	INT64_ m_nMinRespTime;//��С��Ӧʱ��
	INT64_ m_nMaxRespTime;//�����Ӧʱ��
	INT64_ m_nAvRespTime;//ƽ����Ӧʱ��
	int m_nCurNumSec;//��ǰÿ�����
	int m_nAvNumSec;//ƽ��ÿ�����

	__int64 m_nSendByte;//�����ֽ���
	__int64 m_nRcvByte;//�����ֽ���

	int m_nTotalRespTIme;//����Ӧʱ��

	bool m_bIsExit;//�Ƿ��˳�
	bool m_bIsRun;//�Ƿ�ʼ����

 	int  m_nRunTime;//��ʼ���͵�ʱ���,����ʼ���Ե�ʱ���

	unsigned int m_nSerial;//������ˮ

	SERVER_PARA m_pServerPara;//����������


	CBF_Mutex m_runMutex;//run�����Ļ������

	CBF_Mutex m_serialMutex;//��ˮ�������
	std::vector<CIVarDefine *>m_varList;//�����б�
protected:
	
	CBF_Xml   m_varxml;




};

#endif // !defined(AFX_SHAREDATA_H__1120841C_B2D8_4915_9C85_8F7B8B53711C__INCLUDED_)
