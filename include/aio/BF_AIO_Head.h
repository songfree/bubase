// BF_AIO_Head.h: interface for the CBF_AIO_Head class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BF_AIO_HEAD_H__EECCCB23_9E01_470F_8989_C9DB81E473EA__INCLUDED_)
#define AFX_BF_AIO_HEAD_H__EECCCB23_9E01_470F_8989_C9DB81E473EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "public.h"
#include "BF_Socket.h"
#include "BF_Mutex.h"
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

#ifndef _WINDOWS
typedef struct  
{
	char  ol;
// 	unsigned long Internal;
// 	unsigned long InternalHigh;
// 	union 
// 	{
// 		struct 
// 		{
// 			unsigned long Offset;
// 			unsigned long OffsetHigh;
// 		} DUMMYSTRUCTNAME;
// 		void * Pointer;
// 	} DUMMYUNIONNAME;
// 	void *  hEvent;
} __PACKED__ OVERLAPPED;  //ΪIOCP�ض���
#endif


typedef struct 
{
	unsigned  int    s_nIndex;                        //Ҫ�������ݵ���������
	unsigned  int    s_nTimestamp;                    //Ҫ�������ݵ����ӵ�����ʱ���
	unsigned  int    s_nSendNum;                      //���ʹ���
	unsigned  int    s_nSendLen;                      //�ѷ��͵����ݳ���
	unsigned  int    s_nHook;                          //�������ݵı�ʶ
	unsigned  int    s_nTotalLen;                     //���������ܳ���
	unsigned  int    s_nTime;                         //�����ʱ��  
	unsigned  int    s_nTxCode;                       //������
	int              nQuoCode;                        //�������
	int              nQuoSerial;                      //�������
	int              nQuoDate;                        //��������
	int              s_serial;                        //������ˮ
	int              s_nRet;                          //���ͷ��ر�ʶ  -1���ɹ� >=0���͵����ݳ���
	char             s_sSendBuffer[DREBBUFSIZE];   //�������ݻ���
}__PACKED__ SOCKET_SEND_DATA,*PSOCKET_SEND_DATA;

typedef std::list<SOCKET_SEND_DATA> SOCKSENDQUEUE; //��Ӧÿ�����ӵķ��Ͷ���


#define  EPOLLMAXFD     100000  //����������

typedef struct  _SOCKET_POOL_DATA
{
	OVERLAPPED         s_iocp;               //IOCP�ص��ṹ
	SOCKET_HANDLE      s_hSocket;            //SOCKET����
	SOCKADDR_IN        s_pSocketAddr;        //�Է��ĵ�ַ�����ӹ����ĵ�ַ
	unsigned  int      s_nIndex;             //����  Ӧ�ò��ܸı�   
	unsigned  int      s_nTimestamp;         //����ʱ���(��) Ӧ�÷�������ʱҪ�ʹ˽��бȽϣ�����������������Ӳ���ԭ����
	unsigned  int      s_nReadTime;          //����������ʱ��(��)  AIO����
	unsigned  int      s_nWriteTime;         //���������ʱ��(��)  AIO����
	PSOCKET_SEND_DATA  s_pSendData;          //IOCP�õķ���ָ��
	SOCKSENDQUEUE      s_pSendQueue;         //���Ͷ��� epoll��selectʹ�ã��ȷ��ͣ����ɹ��������� 
	SOCKSENDQUEUE      s_pQuoteSendQueue;    //���鷢�Ͷ���
	int                s_nSendQueueFlag;     //���ַ��Ͷ��б�־    0�޲��ַ���  1���Ͷ����в��ַ���  2��������в��ַ���
	CBF_Mutex          s_mutex;              //���Ͷ��л����������ܻ���̷߳��ͣ�epoll��select��
	char  *            s_pIoBuffer;          //IO��������ָ��  ָ��IO������ŵص�ָ��  s_pIoBuffer=s_sRealBuffer+s_nRealBufferLen
	unsigned  int      s_nIoBufferLen;       //iocp�����յ������ݳ���
	char               s_sRealBuffer[DREBBUFSIZE];  //ʵ�ʵ����ݻ��壬����������Ӧ��ȡ�ߺ�����s_nRealBufferLen���ٴν���
	unsigned  int      s_nRealBufferLen;     //�յ�����ʵ�ʵ����ݻ����е����ݳ���
	_SOCKET_POOL_DATA*  s_pEpollPoint;          //EPOOL����ʱ�õ���ָ��

	char               s_cOperType;          // IOCP��������
#define     OP_NONE       0   //ʲô��������        ����δʹ��ʱ������
#define     OP_READ       1   //��                  ��������ʱ�Ĳ���
#define     OP_WRITE      2   //д
#define     OP_ACCEPT     3   //��������
#define     OP_CONNECT    4   //����д����������Ƿ�ɹ�
#define     OP_LISTEN     5   //��������
	char               s_cSocketType;        //SOCKET��������   ��Ӧ�þ��嶨�� 

#ifndef SOCK_UNKNOW 
#define     SOCK_UNKNOW     0                //δ֪ 
#endif
#define     SOCK_CLIENT     1                //��ͨ�ͻ���
#define     SOCK_SERVER     2                //ע��ķ����
#define     SOCK_TONODE     3                //���ӵ���Ľڵ�����
#define     SOCK_FROMNODE   4                //�ӱ�Ľڵ��������ģ���Ӧ��ΪSOCK_TONODE
#define     SOCK_TOVC       5                //����ͨ�� �������ӳ�ȥ��VC
#define     SOCK_FROMVC     6                //����ͨ�� ��Ľڵ���������	
#define     SOCK_LISTEN     7                //�����˿�
#define     SOCK_ACCEPT     8                //�����ӹ����ģ���û�о�����֤������,��֤���Ϊ�������ͣ���Ϊ�м�״̬
	char               s_cSocketStatus;      //����״̬ AIO EPOLL��selectģ�͸��ݴ�״̬���м�� IOCP����  
#define     SOCKSTATUS_RW         0                //0�ɶ���д
#define     SOCKSTATUS_R          1                //1��Ҫ����
#define     SOCKSTATUS_W          2                //2��Ҫ���д
#define     SOCKSTATUS_C          3                //3��Ҫ��д�����Ƿ�����
#define     SOCKSTATUS_UNKNOW     4                //4ʲô��������
	char             s_cHBFlag;            //������־ 0������   1��Խ��շ�����  2��Է��ͷ�����
#define     HEARTBEAT_NONE        0                //0������
#define     HEARTBEAT_RECV        1                //1��Խ��շ�����
#define     HEARTBEAT_SEND        2                //2��Է��ͷ�����
	char             s_cCheckFlag;                 //���socket��֤�����ֶΣ�Ӧ�ö���
	unsigned  int    s_nHBTime;            //����ʱ��(��)  ��s_cHBFlagΪ1ʱ�������ڴ�ʱ��û�н������ݾͷ����� ��s_cHBFlagΪ2ʱ�������ڴ�ʱ��û�з������ݾͷ�����  
	unsigned  int    s_nHBCloseTime;       //(��)�������ӷ����������ڴ�ʱ����û���յ�������ӵ����ݾ͹ر�
	unsigned  int    s_nCloseTime;         //(��)��������accept�������ڴ�ʱ����û���յ�������ӵ����ݾ͹ر�
	unsigned  int    s_nPingTime;          //�����PING����ʱ��(��) ���Ʋ�Ҫ�ظ�������
	
	unsigned  int    s_nVer;               //���Ӱ汾  ��Ӧ�þ��嶨��
	unsigned  int    s_nDestIndex;         //Ŀ������  ��s_nSocketTypeΪSOCK_TOVC��SOCK_FROMVCʱ���൱������������ֱͨ�ģ�����Ҫ��ֱͨ����������
	char             s_sSessionKey[49];    //�Ự��Կ
	char             s_sSignString[49];    //Ҫǩ�����ַ���
	unsigned short   s_nSignPubKeyLen;     //ǩ����Կ����
	char             s_sSignPubKey[2049];  //ǩ����Կ
	unsigned short   s_nEncryptPubKeyLen;  //���ܹ�Կ����
	char             s_sEncryptPubKey[2049]; //���ܹ�Կ
	void  *          ptr;                  //�Զ���Ľṹָ�룬������չ
}SOCKET_POOL_DATA,*PSOCKET_POOL_DATA;



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

#endif // !defined(AFX_BF_AIO_HEAD_H__EECCCB23_9E01_470F_8989_C9DB81E473EA__INCLUDED_)
