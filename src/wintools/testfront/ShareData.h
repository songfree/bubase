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
#include "XmlPack.h"
#include "IVarDefine.h"

struct SERVER_PARA
{
	char sip[40];
	unsigned int port;
	unsigned int headlen;
	unsigned int timeout;
	unsigned int timesep;
	unsigned int loglevel;
	unsigned int runtime;
};

class CShareData  
{
public:
	CShareData();
	virtual ~CShareData();

public:
	bool InitVar(char *varxml);
	void Clear();
	void Run(bool isstop,int sendnum,int sendbyte,int rcvnum,int rcvbyte,int errnum,int resptime);

	int m_nAllRunTime;//������ʱ��
	int m_nCurRunTime;//��ǰ����ʱ��
	int m_nTotalThreadNum;//���߳���
	int m_nSendNum;//���ͱ���
	int m_nRcvNum;//���ձ���
	int m_nErrNum;//�������
	int m_nMinRespTime;//��С��Ӧʱ��
	int m_nMaxRespTime;//�����Ӧʱ��
	int m_nAvRespTime;//ƽ����Ӧʱ��
	int m_nCurNumSec;//��ǰÿ�����
	int m_nAvNumSec;//ƽ��ÿ�����

	__int64 m_nSendByte;//�����ֽ���
	__int64 m_nRcvByte;//�����ֽ���

	int m_nTotalRespTIme;//����Ӧʱ��

	bool m_bIsExit;//�Ƿ��˳�
	bool m_bIsRun;//�Ƿ�ʼ����
	int  m_nRunTime;//��ʼ���͵�ʱ���

	int m_nConnNum;//���ӷ������Ĵ���

	SERVER_PARA m_pServerPara;//����������


	CBF_Mutex m_runMutex;//run�����Ļ������

	std::vector<CIVarDefine *>m_varList;//�����б�
protected:
	CXmlPack m_varxml;




};

#endif // !defined(AFX_SHAREDATA_H__1120841C_B2D8_4915_9C85_8F7B8B53711C__INCLUDED_)
