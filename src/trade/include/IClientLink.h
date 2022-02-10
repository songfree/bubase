// IClientLink.h: interface for the CIClientLink class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ICLIENTLINK_H__F212B770_97AA_433C_80A0_483518F5F65D__INCLUDED_)
#define AFX_ICLIENTLINK_H__F212B770_97AA_433C_80A0_483518F5F65D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BpcHead.h"
//�ͻ������ӽӿ��࣬������̳�ʵ�֣���ҵ��ģ�����


typedef struct 
{
	char         src;        //0����    1scoket 
	unsigned int index;      //��������  
	unsigned int nWantlen;       //Ҫ���ͻ�������ݳ���  ����
	unsigned int nSendlen;       //�ѷ������ݳ���   ���͸ı�
	unsigned int rTime;          //�������ݵ�ʱ���
	PBPCCOMMSTRU pData; //����
}S_TRADE_DATA;

class CIClientLink  
{
public:
	CIClientLink()
	{

	}
	virtual ~CIClientLink()
	{

	}

public:
	// ������: Send2Client
	// ���  : ������ 2017-7-20 9:28:49
	// ����  : virtual int 
	// ����  : S_TRADE_DATA &data  src=1ʱdata.pData->sBpcHead.nIndexΪsocket�������
	// ����  : ���͸��ͻ�������  ���ݸ�ʽΪ����ͷ+����  src=0��ʾ����  src=1��ʾsocketֱ��
	virtual int Send2Client(S_TRADE_DATA &data)=0;


	// ������: PushRcvQueue
	// ���  : ������ 2017-8-14 9:37:13
	// ����  : virtual int 
	// ����  : S_TRADE_DATA &data
	// ����  : �������������Ϣ������ն��У����������߳�ȡ������
	virtual int PushRcvQueue(S_TRADE_DATA &data)=0;


	// ������: PushAnsQueue
	// ���  : ������ 2017-8-14 9:38:13
	// ����  : virtual int 
	// ����  : S_TRADE_DATA &data
	// ����  : ��Ӧ�����������̵߳�Ӧ�����
	virtual int PushAnsQueue(S_TRADE_DATA &data)=0;


	// ������: GetAnsData
	// ���  : ������ 2017-8-14 14:00:51
	// ����  : virtual int 
	// ����  : int index  �̵߳�������ÿ���߳�һ��Ӧ����У�ÿ���߳�ͬ���ȴ�ȡӦ��
	// ����  : S_TRADE_DATA &data
	// ����  : int tms  ��ʱms
	// ����  : ȡӦ��
	virtual int GetAnsData(int index,S_TRADE_DATA &data,int tms)=0;


	// ������:  PoolMalloc
	// ���  : ������ 2018-4-13 15:50:23
	// ����  : virtual int 
	// ����  : S_TRADE_DATA &data
	// ����  : ����ռ�
	virtual int  PoolMalloc(S_TRADE_DATA &data)=0;

	// ������: PoolFree
	// ���  : ������ 2018-4-13 15:50:27
	// ����  : virtual void 
	// ����  : S_TRADE_DATA &data
	// ����  : �ͷſռ�
	virtual void PoolFree(S_TRADE_DATA &data)=0;

};

#endif // !defined(AFX_ICLIENTLINK_H__F212B770_97AA_433C_80A0_483518F5F65D__INCLUDED_)
