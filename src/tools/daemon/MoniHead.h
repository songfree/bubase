// MoniHead.h: interface for the CMoniHead class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MONIHEAD_H__1A490998_AE1D_4016_859A_2C1896E2D51B__INCLUDED_)
#define AFX_MONIHEAD_H__1A490998_AE1D_4016_859A_2C1896E2D51B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DrebPubDefine.h"

#ifdef __cplusplus
extern "C" {
#endif
	
	//�ֽڶ���
#ifdef _WINDOWS
#define __PACKED__ 
#pragma pack(push,1)
#else
#ifdef __GNUC__
#define __PACKED__	__attribute__ ((packed))
	//#pragma pack(push,1)
#else
#define __PACKED__
#pragma options align=packed
#endif
#endif
	
#define MONI_SOCK_UNKNOW   10      //δ֪
#define MONI_SOCK_DREB     13      //���������߽ڵ������
#define MONI_SOCK_LISTEN	 14      //�����˿�


typedef struct 
{
	int             rtime;                 //�����ʱ��
	unsigned int    sendnum;               //���ʹ���
	unsigned int    offset;     //����λ�� ������ǰ���͵�����λ�ã���Ҫ�Ƿ���һ�������ݵ����
	unsigned short  sendlen;    //�������ݳ���
	unsigned short  totallen;   //�����ݳ���
	unsigned int    serial;    //dreb��ˮ
	unsigned int    nIndex;    //dreb��������
	COMMSTRU       data; 
}__PACKED__ S_GWDREB_RSMSG,*PS_GWDREB_RSMSG; //�������ݽṹ


typedef struct
{
	std::string sIp;       //IP
	int         nPort;     //�˿�
	int         nBandwidth;//����
}S_DREBLINK_INFO;

	//ȡ���ֽڶ���
#ifdef _WINDOWS
#pragma pack(pop)
#else
#ifdef __GNUC__
	//#pragma pack(pop)
#else
#pragma options align=reset
#endif
#endif
#undef __PACKED__
	
#ifdef __cplusplus
}
#endif

#endif // !defined(AFX_GATEHEAD_H__1A490998_AE1D_4016_859A_2C1896E2D51B__INCLUDED_)
