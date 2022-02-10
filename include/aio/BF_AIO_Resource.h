// BF_AIO_Resource.h: interface for the CBF_AIO_Resource class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BF_AIO_RESOURCE_H__DAB2456F_EB32_4EBC_8D6A_FA6A6EE9F1EB__INCLUDED_)
#define AFX_BF_AIO_RESOURCE_H__DAB2456F_EB32_4EBC_8D6A_FA6A6EE9F1EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "BF_Socket.h"
#include "BF_Mutex.h"
#include "BF_AIO_Head.h"
#include "typedefine.h"
#include "BF_Date_Time.h"
#include "BF_LogClient.h"

//0-selectģ��  1-epollģ��(LINUX)   2-iocpģ��(WINDOWS)
#define  MODEIOCP    2     //iocpģ��
#define  MODEEPOLL   1     //epollģ��
#define  MODESELECT  0     //selectģ��

#ifdef AIO_EXPORTS
#define AIO_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define AIO_EXPORT __declspec(dllimport)
#else
#define AIO_EXPORT 
#endif
#endif

class AIO_EXPORT CBF_AIO_Resource  
{
public:

	// ������: ReSetPara
	// ���  : ������ 2013-6-5 22:09:26
	// ����  : void 
	// ����  : ��ʼ�������ĳ�ʼֵ,ͬ���캯��
	void ReSetPara();

	// ������: SetIoThreadNum
	// ���  : ������ 2013-6-4 19:49:54
	// ����  : void 
	// ����  : unsigned iothreadnum
	// ����  : �ڶ���������IO�߳���  epollֻҪһ���߳�  iocpΪ������������select������Ҫ����
	void SetIoThreadNum(unsigned iothreadnum);

	// ������: SetRunMode
	// ���  : ������ 2013-6-4 19:47:09
	// ����  : bool 
	// ����  : unsigned int mode
	// ����  : ��һ������������ģʽ 
	bool SetRunMode(unsigned int mode);

	CBF_AIO_Resource();
	virtual ~CBF_AIO_Resource();

	unsigned   int    g_nIoThreadNum;//IO�߳���
	unsigned   int    g_nMaxConnectNum;//���������
	bool              g_bIsExit;  //�Ƿ��˳�
	unsigned   int    g_nRunMode;//����ģʽ   0-selectģ��  1-epollģ��(LINUX)   2-iocpģ��(WINDOWS)
	unsigned   int    g_nPort;//IOģ�͵������˿�
#if defined(_WINDOWS)
	HANDLE            g_hIocp;  //IOCP��HANDLE
#endif
#if defined(LINUX)
	int               g_nEpoll;  //EPOlL
#endif
	unsigned  int     g_nListenIndex;
	UINT64_           g_nRecvByteLen;  //�յ������ݳ���
	UINT64_           g_nSendByteLen;  //���͵����ݳ���
    UINT64_           g_nStartTime;    //��ʼ����ʱ�� ����
	int               g_nIocpAcceptNum;//iocpһ�η�AcceptEx����Ŀ
	char              g_sCurPath[300];
	char              g_sModulePath[300];
	char              g_sModuleName[300];

	int               g_nQuoteQueueDeleteTime;      //������й���ʱ�� 
	int               g_nQuoteQueueNum;//������������

	
	  
	int               g_nSendOnceNum;//EPOLLһ�η�������

	int g_nHeartRun;//����ʱ��,���������ӳ�����ʱ��δ��ʱ��������������
	int g_nDisconnectTime;//δʹ�öϿ�ʱ�䣬���һ�������ڴ�ʱ����һֱû��ʹ�ã��򽫴����ӶϿ�����λ��Ĭ��600�뼴10����
	int g_nDispatchTime;//�������ʱ��


	int               g_nLoglevel;//��־���� Ĭ��Ϊ5
	std::string       g_sLogFileName;//��־�ļ���
	std::string       g_sLogFilePath;//��־�ļ�Ŀ¼
	int               g_nMaxLogSize;//��־��С��<1024������
	CBF_LogClient     g_pLog;

	// ������: Init
	// ���  : ������ 2013-6-4 19:18:47
	// ����  : virtual bool 
	// ����  : const char *confile
	// ����  : ��Ҫʵ�ֵĳ�ʼ����
	virtual bool Init(const char *confile)=0;

};

#endif // !defined(AFX_BF_AIO_RESOURCE_H__DAB2456F_EB32_4EBC_8D6A_FA6A6EE9F1EB__INCLUDED_)
