// BuDefine.h: interface for the PubDefine .
// ͨѶ����
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BFDEFINE_H__F2DFC8C6_AAAD_4A6C_BAEC_249903098AF2__INCLUDED_)
#define AFX_BFDEFINE_H__F2DFC8C6_AAAD_4A6C_BAEC_249903098AF2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "BF_SocketTcp.h"




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
	

#define BFBUFSIZE  8192  //ǰ�õ���󻺳��С
#define MAXBUNUM   100   //���bu������
#define BEATHEARTTICK  5  //�������

#define SOCK_UNKNOW   0      //δ֪
#define SOCK_CLI      2      //�ͻ���
#define SOCK_BU       3      //BU�ͻ���
#define SOCK_LISTEN	  7      //�����˿�

#define MSG_REQ       0      //��������
#define MSG_FREE      1      //BU���Ϳ�������
#define MSG_BUANS     2      //BUӦ��
#define MSG_BUANSFREE 3      //BUӦ�𲢿�������
#define MSG_GETSERIAL 4      //BUȡ����Ψһ��ˮ


typedef struct 
{
	char   buffer[BFBUFSIZE];  
}__PACKED__ COMMSTRU_FPC,*PCOMMSTRU_FPC; //�������ݽṹ

typedef struct 
{
	short msgtype;             //��Ϣ����
	unsigned short index;      //���ӵ���ţ���ʶ��������ĸ������Ϲ�����
	unsigned short buindex;    //bu���ӵ����
	int connecttime;           //����Ҫʹ�õ�socket��ʱ������ڷ���ʱ��ԭ���ӽ��бȽϣ�����ͬ��ʾ�����Ѷϲ����ã�ȡ������
	int rtime;                 //���������ʱ��
	unsigned int len;        // ����Ϣ�������ݰ����ȣ��������������ݳ���
}__PACKED__ S_MSG_HEAD_FPC;
typedef struct 
{
	S_MSG_HEAD_FPC head;	
	PCOMMSTRU_FPC  message;         //��Ϣ����
}__PACKED__ S_MSG_BUF_FPC;
typedef struct 
{
	unsigned int nWantlen;       //Ҫ�������ݳ���
	unsigned int nSendlen;       //�ѷ������ݳ���
	S_MSG_BUF_FPC msgbuf;	
}__PACKED__ S_BF_RSMSG_FPC;      //ͨѶƽ̨�ڲ����ͽ������ݵĽṹ



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


#endif // !defined(AFX_BFDEFINE_H__F2DFC8C6_AAAD_4A6C_BAEC_249903098AF2__INCLUDED_)
