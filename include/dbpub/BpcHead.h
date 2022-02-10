// BpcHead.h: interface for the CBpcHead class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BPCHEAD_H__D8EDEF1B_7EE3_41D6_B92E_E030C704C1E4__INCLUDED_)
#define AFX_BPCHEAD_H__D8EDEF1B_7EE3_41D6_B92E_E030C704C1E4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "DrebPubDefine.h"
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
	
#define LMSGID 0x0000   // ������
#define RMSGID 0x0101   // ������
	
#define BPCBUFSIZE  65534   //bpc����󻺳��С
//#define BPCBUFSIZE  8192   //bpc����󻺳��С


#define MAXBPUNUM   100    //���bpu��
#define BEATHEARTTICK  5   //�������
	
#define BPCSOCK_UNKNOW   0      //δ֪
#define BPCSOCK_CLI      2      //�ͻ���
#define BPCSOCK_BPU      3      //BPU�ͻ���
#define BPCSOCK_DREB     4      //DREB����
#define BPCSOCK_LISTEN	  7      //�����˿�
	
#define MSG_REQ         0      //��������,bpc����BPU������
#define MSG_FREE        1      //BPU���Ϳ�������
#define MSG_GETSERIAL   4      //BPUȡ����Ψһ��ˮ
#define MSG_SETGPARA1   5      //BPU����SETGPARA1
#define MSG_SETGPARA2   6      //BPU����SETGPARA2
#define MSG_SETGPARA3   7      //BPU����SETGPARA3
#define MSG_TRANS        9       //BPU������Ϣ����
#define MSG_EXTCALL      10      //BPU�����������	
#define MSG_BPCCALL      11      //BPU����PU���������dreb������Ϊ	
#define MSG_BPUREG       12      //BPU����ע������
#define MSG_BPUISREG     13      //BPU�����Ƿ�ע������
#define MSG_GETNEXT      15      //dreb������ȡ��BPU����ĺ���������Ϣ
#define MSG_BPCMONITOR   16      //�����ļ�ر��棬��ʱ����

#define MSG_GETBPCNEXT   17      //dreb������ȡ��BPC����ĺ���������Ϣ
#define MSG_CONNECTDREB  18      //dreb������������DREB�ɹ�����Ϣ����������Ϣ�����ע���(����ѯ�ʣ�ע��)
#define MSG_MONITOR_SAP  19      //dreb��������������Ϣ��DREB������ΪCMD_MONITOR_BPC,����ļ������service_no����DrebPubDefine.h���BPCMONI_INFO����
#define MSG_DREBANS      20      //DREB������Ӧ����Ϣ


	typedef struct //���� 17
	{
		char            cMsgType;         //��Ϣ����
		short           nMsgFlag;         //LMSGID��RMSGID ��ʾͨѶʱ���ĵ��ֽ���
		unsigned short  nIndex;           //����dreb������ʱ�����ӵ���ţ���ʶ�����ݴ��ĸ�dreb�����Ϲ�����
		unsigned short  nBpuIndex;        //����bpu����ʱbpu���ӵ����
		int             nConnectTime;     //����Ҫʹ�õ�socket��ʱ������ڷ���ʱ��ԭ���ӽ��бȽϣ�����ͬ��ʾ�����Ѷϲ����ã�ȡ������
		int             nRtime;           //���������ʱ�䣬BPUע��ʱBPU��PID����ID
		unsigned short  nBpcLen;          // ����Ϣ�������ݰ����ȣ��������������ݳ���
	}__PACKED__ S_BPC_HEAD;

	
	#define BPUDATASIZE  (BPCBUFSIZE-DREBHEADLEN-sizeof(S_BPC_HEAD))   //bpu����󻺳��С
	#define BPCHEADLEN   sizeof(S_BPC_HEAD)

	typedef struct 
	{
		S_BPC_HEAD    sBpcHead;        //bpchead
		DREB_HEAD     sDBHead;         //drebͷ
		char   sBuffer[BPUDATASIZE];  
	}__PACKED__ BPCCOMMSTRU,*PBPCCOMMSTRU; //�������ݽṹ
	
	

	typedef struct 
	{
		char         sBpuGroupName[21]; //bpu������
		unsigned int nBpuGroupIndex;    //bpu������ �ɾݴ�ֱ�Ӳ���bpu��Ķ���
		unsigned int nRtime;        //�����ʱ��
		unsigned int nTotalLen;     //�ܹ�Ҫ�������ݳ���
		unsigned int nSendLen;      //��ǰҪ�������ݳ���
		unsigned int   s_nDrebSerial;    //�������߽ڵ��������ˮ����־Ψһ���������ݰ�
		unsigned short s_nNodeId;        //�������߽ڵ���
		char           s_cNodePrivateId; //�������߽ڵ�˽�����
		unsigned int index;              //�������
		PBPCCOMMSTRU sMsgBuf;	
	}__PACKED__ S_BPC_RSMSG;    
	

	typedef vector<BPCCOMMSTRU> LIST_BPC_RSMSG;

	typedef struct //���� 47
	{
		unsigned int    nFuncNo;           //������
		char            sFuncName[41];     //��������
		char            cPrio;             //�������ȼ�
		char            cServiceFlag;      //�����־ 0:BPC�ڲ����ܲ�����DREB��ע�� 2:BPU�ڲ�����,������DREB��ע�� 1:Ҫ��DREB��ע��
	}__PACKED__ S_BPU_FUNCINFO;     //������Ϣ

	typedef struct 
	{
		char            sBpuGroupName[21]; //bpu������
		unsigned int    nFuncNum;          //������Ŀ
		S_BPU_FUNCINFO  funcinfo;          //������Ϣ
	}__PACKED__ S_BPU_FUNC_REG;     //������Ϣ
	
	
typedef struct //���� 6
{
	unsigned int   nSvrMainId;     //���������
	char           cSvrPrivateId;  //˽�з����
	unsigned int   nBpuGroupNum;   //bpu����Ŀ

}__PACKED__ S_MONITORBPC_INFO;      
	


typedef struct 
{
	unsigned int nTime;//����ʱ��  hhmmss
	unsigned int nUs;  //΢��
	unsigned int nId;  //����ID
	BPCCOMMSTRU  data;
}__PACKED__ S_LOG_BPCHEAD;

typedef struct 
{
	unsigned int nTime;//����ʱ��  hhmmss
	unsigned int nUs;  //΢��
	unsigned int nId;  //����ID
	COMMSTRU  data;
}__PACKED__ S_LOG_DREBHEAD;
	
typedef struct //���� 47
{
	char            sFileName[201];           //�ļ��� ��Ŀ¼
	char            sFileSize[12];            //�ļ���С
	char            cBeginFlag;               //�ļ���ʼ��־ '1'�� '0'��
	char            cEndFlag;                 //�ļ�������־ '1'�� '0'��
	char            sFileOffset[12];          //�ļ�ƫ����  
}__PACKED__ S_BPC_DUFILE;     //�ϴ������ļ��ṹ

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

#endif // !defined(AFX_BPCHEAD_H__D8EDEF1B_7EE3_41D6_B92E_E030C704C1E4__INCLUDED_)
