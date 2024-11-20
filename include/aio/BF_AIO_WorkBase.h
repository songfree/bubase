// BF_AIO_WorkBase.h: interface for the CBF_AIO_WorkBase class.
//
//////////////////////////////////////////////////////////////////////

/***********************************************************************************
  ������־��¼
  ������: ������
  �������� 2012-5-17 16:08:02
  �汾: V1.00 
  ˵��: �첽IO�Ŀͻ�������࣬���������ݡ����ӶϿ��Ⱥ����ñ���ķ�����ʵ��ʹ��ʱ�̳д˻������ʵ��.
 ***********************************************************************************/


#if !defined(AFX_BF_AIO_WORKBASE_H__1C6E4AF0_8B2C_4C35_A00F_C8293BED35B4__INCLUDED_)
#define AFX_BF_AIO_WORKBASE_H__1C6E4AF0_8B2C_4C35_A00F_C8293BED35B4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "BF_AIO_Base.h"
#include "BF_AIO_Pool.h"
#include "BF_AIO_Resource.h"


#ifdef AIO_EXPORTS
#define AIO_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define AIO_EXPORT __declspec(dllimport)
#else
#define AIO_EXPORT 
#endif
#endif

class AIO_EXPORT CBF_AIO_WorkBase  
{
public:


	// ������: Close
	// ���  : ������ 2012-5-24 11:33:38
	// ����  : bool 
	// ����  : PSOCKET_POOL_DATA info  ������Ϣ  s_pSendQueue�������Ҫ���
	// ����  : ֪ͨAIO�ر����ӣ�����֮ǰ�����Ͷ���Ӧ��Ҫ���,������ڴ�й¶��AIO�յ�֪ͨ����ͷ����ӵ����ӳ�
	//         ,״̬Ϊ���У��´����ӹ����ɼ���ʹ��
	bool CloseClient(PSOCKET_POOL_DATA info)
	{
		if (m_pAioMethod == NULL)
		{
			return false;
		}
		return m_pAioMethod->Close(info);
	}

	// ������: GetConnectPoolInfo
	// ���  : ������ 2013-6-14 20:32:01
	// ����  : int 
	// ����  : int &total
	// ����  : ȡ���ӳ������Ŵ�Ϣ
	int GetConnectPoolInfo(int &total)
	{
		int use;
		m_pAioPool->PoolInfo(total,use);
		return use;
	}

	// ������: Send2Client
	// ���  : ������ 2012-5-21 16:52:24
	// ����  : bool 
	// ����  : PSOCKET_SEND_DATA senddata
	// ����  : ��������
	bool Send2Client(SOCKET_SEND_DATA senddata,bool isimmediate=false, bool islock=true)
	{
		if (m_pAioMethod == NULL)
		{
			return false;
		}
		return m_pAioMethod->Send(senddata,isimmediate, islock);
	}

	// ������: AddAio
	// ���  : ������ 2012-5-21 10:03:52
	// ����  : bool 
	// ����  : PSOCKET_POOL_DATA info
	// ����  : ��������Ϣ���뵽AIO��
    bool AddAio(PSOCKET_POOL_DATA info)
	{
		if (m_pAioMethod == NULL)
		{
			return false;
		}
		return m_pAioMethod->AddAio(info);
	}

	// ������: Connect
	// ���  : ������ 2012-5-21 11:56:45
	// ����  : bool 
	// ����  : char *ip  ���ӵ�IP
	// ����  : short port  �˿�
	// ����  : unsigned  short  sockettype   ��������
	// ����  : unsigned int destindex        ����vc�Ļ�Ҫָ������������
	// ����  : unsigned int &index           AIO���ش���������
	// ����  : Ӧ��Ҫ�������ӣ�OnConnectedBack�����Ƿ�ɹ�����
	bool Connect(char *ip,short port,unsigned  short  sockettype, unsigned int destindex ,unsigned int &index)
	{
		if (m_pAioMethod == NULL)
		{
			return false;
		}
		return m_pAioMethod->Connect(ip,port,sockettype,destindex,index);
	}

	// ������: Connect
	// ���  : ������ 2012-5-21 11:55:51
	// ����  : void 
	// ����  : PSOCKET_POOL_DATA *info
	// ����  : AIO֪ͨ�����ӶϿ���Ҫ������
	virtual void OnConnect(PSOCKET_POOL_DATA info)
	{
		
	}


	// ������: OnSendBack
	// ���  : ������ 2012-5-17 16:38:08
	// ����  : virtual void 
	// ����  : PSOCKET_SEND_DATA data   
	// ����  : AIO֪ͨ��д�����ݷ��أ��Է��ؽ�����д���
	virtual void OnSendBack(SOCKSENDQUEUE::iterator data,bool issuccess)
	{

	}
	virtual void OnSendBack(PSOCKET_SEND_DATA data,bool issuccess)
	{
		
	}

	// ������: OnPing
	// ���  : ������ 2012-5-21 11:54:23
	// ����  : virtual void 
	// ����  : AIO֪ͨ��Ҫ����PING��
	virtual void OnPing(PSOCKET_POOL_DATA info)
	{
		
	}

	// ������: OnRecvBack
	// ���  : ������ 2012-5-17 16:36:58
	// ����  : virtual void 
	// ����  : PSOCKET_POOL_DATA info
	// ����  : AIO֪ͨ�������յ����ݣ������ݽ��д���
	virtual void OnRecvBack(PSOCKET_POOL_DATA info)
	{
		//�յ������ˣ�ȡ���ݣ����봦�����
		//����ڴ˺������غ���ȥ������,����������״̬֪ͨ��ܶ�
	}

	// ������: OnConnectBack
	// ���  : ������ 2012-5-17 16:35:34
	// ����  : virtual void 
	// ����  : PSOCKET_POOL_DATA info
	// ����  : int status ����״̬  0�ɹ� ����ʧ��
	// ����  : AIO֪ͨ�������������ӳɹ����Դ����ӽ��к�������
	virtual void OnConnectBack(PSOCKET_POOL_DATA info,int status)
	{
		//������״̬������������֤��Ϣ�ȵ�
	}

	// ������: OnClose
	// ���  : ������ 2012-5-17 16:34:32
	// ����  : virtual void 
	// ����  : PSOCKET_POOL_DATA info ������Ϣ
	// ����  : std::string msg
	// ����  : AIO֪ͨ������һ�����ӹرգ�AIO��û�жԴ����ӽ��йرմ���Ҫ��������Դ����ӽ��к�������Ȼ�����Close����֪ͨAIO�رյ�
	virtual void OnClose(PSOCKET_POOL_DATA info,std::string msg)
	{
		//��շ��Ͷ��� �����Ǽ򵥵���գ�Ҫ�Զ��е����ݽ����ͷ�
//		info->s_pSendQueue.clear();
		CloseClient(info);
		
	}

	// ������: GetPoolData
	// ���  : ������ 2013-6-9 10:57:39
	// ����  : PSOCKET_POOL_DATA 
	// ����  : int index
	// ����  : ������������ȡ��������Ϣ
	PSOCKET_POOL_DATA GetPoolData(int index)
	{
		return m_pAioPool->GetDataByIndex(index);
	}

	// ������: OnAccept
	// ���  : ������ 2012-5-17 16:33:33
	// ����  : virtual void 
	// ����  : PSOCKET_POOL_DATA info
	// ����  : AIO֪ͨ���յ�һ�����ӣ��Դ������ӽ��д���,AIO��ܲ��������ӳأ���Ӧ�ü��ͨ��������ʾ����AddAio
	virtual void OnAccept(PSOCKET_POOL_DATA info)
	{
		//��������Ƿ�Ϸ�

		//��SOCKET״̬�ȵȣ���ֻ������ֻд���Ƕ�д,��Ҫ��SELECTģ���� 
		info->s_cSocketStatus = SOCKSTATUS_RW;
		AddAio(info);
	}

	// ������: SetParameter
	// ���  : ������ 2012-5-18 11:32:46
	// ����  : void 
	// ����  : CBF_AIO_Base *p
	// ����  : ����AIO��������ܳ�ʼ��ʱ����
	void SetParameter(CBF_AIO_Base *p,CBF_AIO_Pool   *pool);

	CBF_AIO_WorkBase();
	virtual ~CBF_AIO_WorkBase();

	int  GetConnInfo(std::vector<PSOCKET_POOL_DATA>& connlist)
	{
		return m_pAioPool->GetConnInfo(connlist);
	}

protected:
	CBF_AIO_Base   *m_pAioMethod;  //ͨ���˻���ָ����÷������ݡ��������ӡ������Ӽ����첽IO���ϵ�
	CBF_AIO_Pool   *m_pAioPool;

};

#endif // !defined(AFX_BF_AIO_WORKBASE_H__1C6E4AF0_8B2C_4C35_A00F_C8293BED35B4__INCLUDED_)
