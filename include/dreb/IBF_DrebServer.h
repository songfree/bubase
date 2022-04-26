// BF_DrebServer.h: interface for the CBF_DrebServer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IBF_DREBSERVER_H__8810FE91_B2B1_4312_9C27_4C97F9219A30__INCLUDED_)
#define AFX_IBF_DREBSERVER_H__8810FE91_B2B1_4312_9C27_4C97F9219A30__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "DrebPubDefine.h"
#include "BpcHead.h"
#include "BF_BufferPool.h"
#include "BF_LogClient.h"
#include "BF_HostInfo.h"
#include "dreberrcode.h"

class  CIBF_DrebServer 
{
public:
    virtual int GetDataLogLevel()=0;

    // ������: GetErrMsg
    // ���  : ������ 2015-7-21 10:28:00
    // ����  : std::string 
    // ����  : ȡ�ó�����Ϣ
    virtual std::string GetErrMsg()=0;

    // ������: SetDataLogLevel
    // ���  : ������ 2015-7-15 15:01:38
    // ����  : void 
    // ����  : int loglevel
    // ����  : ���ñ�����־���𣬼�ʱ��Ч
    virtual void SetDataLogLevel(int loglevel)=0;


    // ������: UnzipBuf
    // ���  : ������ 2014-12-15 8:47:49
    // ����  : bool 
    // ����  : BPCCOMMSTRU &data
    // ����  : �⿪�������ݣ���Լ��ָ��
    virtual bool UnzipBuf(BPCCOMMSTRU& data)=0;

    // ������: GetLogPoint
    // ���  : ������ 2014-12-12 10:31:28
    // ����  : CIErrlog * 
    // ����  : ������־��ָ�룬��ͨ����ָ��д����־��Ϣ
    virtual CIErrlog* GetLogPoint()=0;

    // ������: GetDrebInfo
    // ���  : ������ 2014-12-10 16:35:34
    // ����  : bool 
    // ����  : int index  ��������
    // ����  : char *sip  ip��ַ
    // ����  : int &port  �˿�
    // ����  : int &drebid   �����ڵ��
    // ����  : int &privateid  ˽�нڵ��
    // ����  : char *status  ״̬   "δ����"  "����"   "��������"
    // ����  : ��ȡDREB����������Ϣ
    virtual bool GetDrebInfo(int index, char* sip, int& port, int& drebid, int& privateid, char* status)=0;

    // ������: GetBufferPoolInfo
    // ���  : ������ 2014-12-10 16:30:51
    // ����  : void 
    // ����  : int &total    �ܹ������˶��ٿ�
    // ����  : int &unused   ���ж��ٿ�δʹ��
    // ����  : int &bufsize  ÿ���С
    // ����  : ��ȡ�ڴ滺�����Ϣ
    virtual void GetBufferPoolInfo(int& total, int& unused, int& bufsize)=0;

    // ������: RegisterDreb
    // ���  : ������ 2013-4-8 15:10:22
    // ����  : void 
    // ����  : int index        dreb��������
    // ����  : CInt *funclist   �����б�
    // ����  : �����ӵ�DREB�Ϸ�ע����Ϣ,��������ע�ᵽ������,�������԰�������·�ɣ����㲥���ܴ����ƣ��ɵ��ö��������ע��
    virtual void RegisterDreb(int index, vector<int>* funclist)=0;


    // ������: UnRegisterDreb
    // ���  : ������ 2014-12-12 10:07:43
    // ����  : void 
    // ����  : ȡ����dreb��ע��Ľ���
    virtual void UnRegisterDreb()=0;

    // ������: GetMsgData
    // ���  : ������ 2014-12-10 15:06:39
    // ����  : bool true�ɹ� 
    // ����  : S_BPC_RSMSG &rdata  ��Ϣ
    // ����  : unsigned int waitetms=1000  û����Ϣ�ĵȴ�ʱ��
    // ����  : bool get_front=true  true��ͷȡ,false��βȡ 
    // ����  : �Ӷ�����ȡ��Ϣ���д���ע��Ҫ����poolfree�ͷ�rdata��Ŀռ�
    virtual bool GetMsgData(S_BPC_RSMSG& rdata, unsigned int waitetms = 1000, bool get_front = true)=0;

    // ������: SendMsg
    // ���  : ������ 2014-12-10 15:00:22
    // ����  : int  -1����ʧ�� 0���ͳɹ�  >0�������
    // ����  : S_BPC_RSMSG &sdata
    // ����  : ��DREB�������� ע��bpchead���index��д������Ӧ��ԭ�����ɣ����������һ���Ƚϴ��ֵ�������ñ�apiѡ��һ�����ӷ���
    //          ע���ڱ�������������ͷ�sdata�ռ�
    //          a_cNodePrivateId����ʶ�Ƿ�ת�ƽ��ף��������յ��жϴ˱�ʶΪ100ʱ������s_info��������
    virtual int SendMsg(S_BPC_RSMSG& sdata)=0;

    // ������: Stop
    // ���  : ������ 2014-12-10 14:15:10
    // ����  : void 
    // ����  : ֹͣapi�߳�
    virtual void Stop()=0;

    // ������: Start
    // ���  : ������ 2014-12-10 14:14:58
    // ����  : bool 
    // ����  : ����api�߳�
    virtual bool Start()=0;


    // ������: MonitorThread
    // ���  : ������ 2014-12-15 8:48:34
    // ����  : void 
    // ����  : ����߳��Ƿ�ֹͣ����ͣ��������
    virtual void MonitorThread()=0;



    // ������: PoolFree
    // ���  : ������ 2014-12-5 11:32:37
    // ����  : void 
    // ����  : void *ppoint
    // ����  : �ͷſռ�
    virtual void PoolFree(void* ppoint)=0;

    // ������: PoolMalloc
    // ���  : ������ 2014-12-5 11:32:41
    // ����  : void * 
    // ����  : ����һ���ռ�
    virtual void* PoolMalloc()=0;

    virtual CBF_BufferPool* GetBufferPool()=0;

    // ������: GetHostInfo
    // ���  : ������ 2014-12-10 14:27:21
    // ����  : void 
    // ����  : S_MONITOR_HOST &host
    // ����  : vector<S_MONITOR_DISK>&disk
    // ����  : ��ȡ����cpu���ڴ桢������Ϣ��ǰ������Դ�ı�־g_nMonitorHostΪ1
    virtual void GetHostInfo(S_MONITOR_HOST& host, vector<S_MONITOR_DISK>& disk)=0;

};

#endif // !defined(AFX_IBF_DREBSERVER_H__8810FE91_B2B1_4312_9C27_4C97F9219A30__INCLUDED_)
