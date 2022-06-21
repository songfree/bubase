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
#include "BF_Xml.h"

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
    int               g_nCrcFlag; //crcУ���־  0��У�鲻����  1У�� ����
	
	  
	int               g_nSendOnceNum;//EPOLLһ�η�������

	int g_nHeartRun;//����ʱ��,���������ӳ�����ʱ��δ��ʱ��������������
	int g_nDisconnectTime;//δʹ�öϿ�ʱ�䣬���һ�������ڴ�ʱ����һֱû��ʹ�ã��򽫴����ӶϿ�����λ��Ĭ��600�뼴10����
	int g_nDispatchTime;//�������ʱ��

    bool g_bIsLocalLog;


	//int               g_nLoglevel;//��־���� Ĭ��Ϊ5
	//std::string       g_sLogFileName;//��־�ļ���
	//std::string       g_sLogFilePath;//��־�ļ�Ŀ¼
	//int               g_nMaxLogSize;//��־��С��<1024������
	CIErrlog         *g_pLog;

	// ������: Init
	// ���  : ������ 2013-6-4 19:18:47
	// ����  : virtual bool 
	// ����  : const char *confile
	// ����  : CBF_LogClient* log ��־��ָ�룬��ΪNULLʱnew���̳д˷���Ҫnew����
	// ����  : ��Ҫʵ�ֵĳ�ʼ����
	virtual bool Init(const char* confile, CIErrlog* log = NULL)
	{
        CBF_Xml pXml;
        if (!pXml.FromFile(confile))
        {
            printf("����[%s]�ļ������ڻ��xml��ʽ \n", confile);
            return false;
        }
        g_pLog = log;
        if (g_pLog == NULL)
        {
            int               nLoglevel;//��־���� Ĭ��Ϊ5
            std::string       sLogFileName;//��־�ļ���
            std::string       sLogFilePath;//��־�ļ�Ŀ¼
            int               nMaxLogSize;//��־��С��<1024������
            g_pLog = new CBF_LogClient();
            if (pXml.GetNodeValueByPath("/package/head/public/loglevel", false, nLoglevel) == NULL)
            {
                nLoglevel = 5;
            }
            if (pXml.GetNodeValueByPath("/package/head/public/logfile", false, sLogFileName, false) == NULL)
            {
                printf("xml�����ļ��ڵ�[/package/head/public/logfile]δ����\n");
                return false;
            }
            if (pXml.GetNodeValueByPath("/package/head/public/logdir", false, sLogFilePath, false) == NULL)
            {
                printf("xml�����ļ��ڵ�[/package/head/public/logdir]δ����\n");
                return false;
            }
            if (pXml.GetNodeValueByPath("/package/head/public/logsize", false, nMaxLogSize) == NULL)
            {
                printf("xml�����ļ��ڵ�[/package/head/public/logsize]δ����\n");
                return false;
            }
            if (nMaxLogSize < 1024)
            {
                nMaxLogSize = 0;
            }
            g_pLog->SetMaxLogSize(nMaxLogSize);
            g_pLog->SetLogPara(nLoglevel, sLogFilePath.c_str(), sLogFileName.c_str());
            //������־�߳�
            ((CBF_LogClient *)g_pLog)->StartLog();
            g_bIsLocalLog = true;
        }
        if (pXml.GetNodeValueByPath("/package/head/public/CRC", false, g_nCrcFlag) == NULL)
        {
            g_nCrcFlag = 0;
        }
  
        if (pXml.GetNodeValueByPath("/package/head/public/quotduetime", false, g_nQuoteQueueDeleteTime) == NULL)
        {
             g_pLog->LogMp(LOG_ERROR, __FILE__, __LINE__, "xml�����ļ��ڵ�[/package/head/public/quotduetime]δ���� Ĭ��10�� ��С2��");
            g_nQuoteQueueDeleteTime = 10;
        }
        if (g_nQuoteQueueDeleteTime < 2)
        {
            g_nQuoteQueueDeleteTime = 2;
        }

        if (pXml.GetNodeValueByPath("/package/head/public/quotnum", false, g_nQuoteQueueNum) == NULL)
        {
            g_pLog->LogMp(LOG_ERROR, __FILE__, __LINE__, "xml�����ļ��ڵ�[/package/head/public/quotnum]δ���� Ĭ��30������С10��");
            g_nQuoteQueueDeleteTime = 30;
        }
        if (g_nQuoteQueueNum < 10)
        {
            g_nQuoteQueueNum = 10;
        }
        if (pXml.GetNodeValueByPath("/package/head/public/sendwindow", false, g_nSendOnceNum) == NULL)
        {
            g_pLog->LogMp(LOG_ERROR, __FILE__, __LINE__, "xml�����ļ��ڵ�[/package/head/public/sendwindow]δ���� Ĭ��16�������20��");
            g_nSendOnceNum = 16;
        }
        if (g_nSendOnceNum > 20)
        {
            g_nSendOnceNum = 20;
        }
        if (pXml.GetNodeValueByPath("/package/head/public/dispatchtime", false, g_nDispatchTime) == NULL)
        {
            g_nDispatchTime = 30;
            g_pLog->LogMp(LOG_ERROR, __FILE__, __LINE__, "��Ϣ����ʱ��Ĭ��30��");
        }
        if (pXml.GetNodeValueByPath("/package/head/public/hearttime", false, g_nHeartRun) == NULL)
        {
            g_pLog->LogMp(LOG_ERROR, __FILE__, __LINE__, "xml�����ļ��ڵ�[/package/head/public/hearttime]δ����");
            return false;
        }
        if (pXml.GetNodeValueByPath("/package/head/public/disconnecttime", false, g_nDisconnectTime) == NULL)
        {
            g_pLog->LogMp(LOG_ERROR, __FILE__, __LINE__, "xml�����ļ��ڵ�[/package/head/public/disconnecttime]δ����");
            return false;
        }
        int tmpint;

        if (pXml.GetNodeValueByPath("/package/head/iomode/runmode", false, tmpint) == NULL)
        {
            g_pLog->LogMp(LOG_ERROR, __FILE__, __LINE__, "xml�����ļ��ڵ�[/package/head/iomode/runmode]δ����");
            return false;
        }
        g_nRunMode = tmpint;
        if (pXml.GetNodeValueByPath("/package/head/iomode/listenport", false, tmpint) == NULL)
        {
            g_pLog->LogMp(LOG_ERROR, __FILE__, __LINE__, "xml�����ļ��ڵ�[/package/head/iomode/listenport]δ����");
            return false;
        }
        g_nPort = tmpint;
        if (pXml.GetNodeValueByPath("/package/head/iomode/maxconnectnum", false, tmpint) == NULL)
        {
            g_pLog->LogMp(LOG_ERROR, __FILE__, __LINE__, "xml�����ļ��ڵ�[/package/head/iomode/maxconnectnum]δ����");
            return false;
        }
        g_nMaxConnectNum = tmpint;
        if (pXml.GetNodeValueByPath("/package/head/iomode/iothread", false, tmpint) == NULL)
        {
            g_pLog->LogMp(LOG_ERROR, __FILE__, __LINE__, "xml�����ļ��ڵ�[/package/head/iomode/iothread]δ����");
            return false;
        }
        g_nIoThreadNum = tmpint;
        if (pXml.GetNodeValueByPath("/package/head/iomode/iocpacceptnum", false, tmpint) == NULL)
        {
            g_pLog->LogMp(LOG_ERROR, __FILE__, __LINE__, "xml�����ļ��ڵ�[/package/head/iomode/iocpacceptnum��]δ����");
            g_nIocpAcceptNum = 5;
        }
        g_nIocpAcceptNum = tmpint;
        

        
        if (!SetRunMode(g_nRunMode))
        {
            g_pLog->LogMp(LOG_ERROR, __FILE__, __LINE__, "����ģʽ���� [%d]", g_nRunMode);
            return false;
        }
        SetIoThreadNum(g_nIoThreadNum);
        return true;
	}

};

#endif // !defined(AFX_BF_AIO_RESOURCE_H__DAB2456F_EB32_4EBC_8D6A_FA6A6EE9F1EB__INCLUDED_)
