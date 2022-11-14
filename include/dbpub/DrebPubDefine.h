// PubDefine.h: interface for the PubDefine .
// ͨѶ����
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PUBDEFINE_H__F2DFC8C6_AAAD_4A6C_BAEC_249903098AF2__INCLUDED_)
#define AFX_PUBDEFINE_H__F2DFC8C6_AAAD_4A6C_BAEC_249903098AF2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "BF_SocketTcp.h"


//socket����

#define SOCK_UNKNOW   0      //δ֪
#define SOCK_CLI      2      //�ͻ���
#define SOCK_DREB     3      //�����������߽ڵ㵽���������߽ڵ������
#define SOCK_SRV      4      //ע��ķ����
#define SOCK_TODREB   5      //�������������߽ڵ�ڵ������
#define SOCK_SLOT	  6      //ע���ͨѶ��
#define SOCK_LISTEN	  7      //�����˿�

//�������߽ڵ�ģ��ȥ�����̵߳���Ϣ����

#define MSG_READ     2    //�����ݲ�����
#define MSG_CLOSE    3    //�ر�����
#define MSG_CONNECT  4    //��������,�������������߽ڵ������
#define MSG_ROUTE	 5    //·��
#define MSG_PING	 6    //����
#define MSG_SERVICE	 7    //�����·��֪ͨ
#define MSG_MONITOR	 8    //�����Ϣ

//�������߽ڵ�����������


#define  CMD_ROUTER      1  //�������߽ڵ㷢��·��֪ͨ��������
#define  CMD_DEL_NODE    2  //�������߽ڵ�֮��ȡ���������߽ڵ��������,ֻ�������������ӵ��������߽ڵ�
#define  CMD_DEL_SVR     3  //�����ȡ��ע������������
#define  CMD_PING        4  //���������������
#define  CMD_CONNECT     5  //����ע��
#define  CMD_SERVICE     6  //����·��
#define  CMD_REGSERVICE  7  //ע�����
#define  CMD_DPCALL      8  //�������߽ڵ�ͬ������ Ҫ�����մ�����ɺ�Ӧ��
#define  CMD_DPACALL     9  //�������߽ڵ��첽���� Ҫ����յ����������߽ڵ㷢���������ȷ��Ӧ��
#define  CMD_DPBC        10  //�������߽ڵ�㲥��������Ϣ����ָ���������߽ڵ�������ע��ķ���,Ҫ���������߽ڵ�Ӧ��
#define  CMD_DPABC       11  //�������߽ڵ�㲥��������Ϣ����ָ���������߽ڵ�������ע��ķ���,��Ҫ���������߽ڵ�Ӧ��
#define  CMD_DPPUSH      12 //�������߽ڵ����ͣ�����Ӧ��
#define  CMD_DPPOST      13 //�������߽ڵ�Ͷ�ݣ�Ҫ����յ����������߽ڵ�Ӧ��
#define  CMD_UNREGSERVICE 14  //ȡ������ע��
#define  CMD_MONITOR_DREB     15 //�������߽ڵ���
#define  CMD_MONITOR_BPC      16 //ҵ���������ڲ�����
#define  CMD_DPBC_UNREGTX     17 //����ȡ��ע�ύ�׹㲥  ���ĳһ������DREB_D_NODEINFO
#define  CMD_SUBSCRIBE         18     //���������߶��Ĺ㲥
#define  CMD_UNSUBSCRIBE         19     //����������ȡ�����Ĺ㲥

#define  BEATHEARTTICK        5  //������� 5��


#define  DREBMONI_INFO          1     //dreb��Ϣ ��ǰ�ڵ㡢��ǰ�ڵ����
#define  DREBMONI_HOST          2     //������Ϣ CPU ���� �ڴ�
#define  DREBMONI_CURTXLIST     3     //dreb��Ϣ ��ǰ�ڵ㽻���б�
#define  DREBMONI_ROUTEDREB     4     //dreb·����Ϣ �ڵ�·�ɱ�
#define  DREBMONI_ROUTESERVICE  5     //dreb·����Ϣ ������·�ɱ�
#define  DREBMONI_REPORT        6     //dreb����ͳ����Ϣ
#define  DREBMONI_CONNINFO      7     //dreb������Ϣ
#define  DREBMONI_REPORTSTART   8     //dreb��ʼͳ����Ϣ pack/s byte/s
#define  DREBMONI_REPORTSTOP    9     //drebֹͣͳ����Ϣ



#define  BPCMONI_INFO       11     //bpc��Ϣ   �ڵ� bpu��
#define  BPCMONI_HOST       12     //������Ϣ  CPU ���� �ڴ�
#define  BPCMONI_BPU        13     //·����Ϣ  ����bpu��
#define  BPCMONI_REPORT     14     //����ͳ����Ϣ  ���������� ������  
#define  BPCMONI_SETBPU     15     //����BPU����
#define  BPCMONI_SETMODE    16     //����bpc���ģʽ
#define  BPCMONI_STOPBPU    17     //ֹͣBPU
#define  BPCMONI_STARTBPU   18     //����BPU
#define  BPCMONI_DOWNLOAD   19     //�����ļ�
#define  BPCMONI_UPLOAD     20     //�ϴ��ļ�



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
	

#define DREBBUFSIZE  65534  //�������߽ڵ����󻺳��С
//#define DREBBUFSIZE  8192  //�������߽ڵ����󻺳��С
	
typedef struct //����30
{
	unsigned short s_nNodeId;        //�������߽ڵ���
	char           s_cNodePrivateId; //�������߽ڵ�˽�����
	unsigned int   s_nSvrMainId;     //ע�����������߽ڵ��ϵ��������ܺ�   ֻ�з������ʱ������д
	char           s_cSvrPrivateId;  //ע�����������߽ڵ��ϵ�˽����� 0��ʾ���� >0Ϊÿ���������ţ�s_nSvrMainId+s_cSvrPrivateId ��Ϊ˽�й��ܺ�   ֻ�з������ʱ������д
	int            s_nHook;          //�ͻ���ʹ�õ��ֶΣ��������߽ڵ㲻�ã�ԭ������;  ����cgate������ͻ��˵�����ʱ���	 ���ǹ㲥ʱ������㲥��key�����ݹ��ܺ�+key���ж���
	int            s_nSerial;        //�첽ʱ��д����ˮ���������߽ڵ㲻�ã�ԭ������;  �ͻ��˵�������� �����Ӧ��ƥ��
	int            s_nGateIndex;     //��������������ԭ������;                         ����cgate������ͻ��˵��������
	unsigned int   s_nDrebSerial;    //�������߽ڵ��������ˮ����־Ψһ���������ݰ�
	unsigned short s_nIndex;         //���ʹ�������������߽ڵ���������
	int            s_nTimestamp;     //�������ߵ�����������Ӧ��ʱ�������Ӧ��ʱ��ͬ����Ϊ�����Ѷϡ����ǿͻ���Ӧ���������
} __PACKED__ DREB_S_NODEINFO;        //Դ��д����Ϣ


typedef struct   //����12
{
	unsigned short d_nNodeId;        //Ŀ���������߽ڵ���
	char           d_cNodePrivateId; //�������߽ڵ�˽�����
	unsigned int   d_nSvrMainId;     //Ŀ������ܺ� ,������д
	char           d_cSvrPrivateId;  //Ŀ��˽����� 0��ʾ���� >0Ϊÿ���������ţ�������д
	unsigned int   d_nServiceNo;     //Ҫ���õķ����ܺ�
} __PACKED__ DREB_D_NODEINFO;        //Ŀ����Ϣ

typedef struct  //����5
{
	unsigned short a_nNodeId;        //Ӧ����������߽ڵ�ڵ�ID
	char           a_cNodePrivateId; //�������߽ڵ�˽�����
	unsigned short a_nRetCode;       //Ӧ��ķ�����
} __PACKED__ DREB_A_NODEINFO;        //Ӧ����������߽ڵ���д����Ϣ

typedef struct  //����4
{
	unsigned short n_nNextNo;           //��������ţ�����������
	unsigned short n_nNextOffset;       //������λ��, �ڼ���������
} __PACKED__ NEXT_INFO;          //Ӧ������д������ȡ��������д����Ϣ

typedef struct  //����6
{
	int              b_nSerial;           //bpc������ڵ�ʹ�õ����ݣ�����������ӵ�ʱ���
	unsigned short   b_cIndex;            //bpc������ڵ�ʹ�õ����ݣ���������������������ݴ��������ظ����õ�bpu
} __PACKED__ BPC_CALLINFO;      //BPC����ʹ�õ�����
	
typedef struct  //����9+30+12+5+4+6+2  = 68
{
	char              cZip;         //ѹ����־ 0���� 1des������Կ����16 2ѹ�� 3DES���� 4DES���ܲ�ѹ��
	char              cCmd;         //�������߽ڵ����������֣�Ӧ��ʱ�����任   
	int               nCrc;         //У��λ
	char              cRaflag;      //����Ӧ���־  0���� 1Ӧ��
	char              cNextFlag;    //��������־  dreb֮�����·��ʹ��ʱ0�װ� 1������  ����ʱ0�װ� 1������2bpcȡ������3bpuȡ������,Ӧ��ʱ0�޺����� 1�к�������2bpc���к�����Ҫȡ3bpu���к�����Ҫȡ
	char              cDrebAffirm;  //�������߽ڵ�ȷ�ϱ�־ 0�� 1�������߽ڵ�ȷ��
	DREB_S_NODEINFO   s_Sinfo;      //Դ��Ϣ           �����󷽼������������߽ڵ���д
    DREB_D_NODEINFO   d_Dinfo;      //Ŀ����Ϣ         Ӧ��ʱ��Դ��Ϣȡ��������
	DREB_A_NODEINFO   a_Ainfo;      //Ӧ����Ϣ         Ӧ���������߽ڵ���д
	NEXT_INFO         n_Ninfo;      //�����Ϣ         Ӧ����д������ȡ��������д
	BPC_CALLINFO      b_Cinfo;      //BPC������Ϣ
	unsigned short     nLen;        //��������ݳ���
}__PACKED__ DREB_HEAD,*PDREB_HEAD;  //�������߽ڵ�ͨѶͷ  
	
#define DREBHEADLEN   sizeof(DREB_HEAD)        //�������߽ڵ��ͨѶͷ��С
#define DREBDATASIZE  (DREBBUFSIZE-DREBHEADLEN)  //�������߽ڵ��������ݴ�С

typedef struct 
{
	DREB_HEAD    head;                                   //ͨѶͷ
	char         buffer[DREBDATASIZE];                   //�������߽ڵ��ﴫ�������
}__PACKED__ COMMSTRU,*PCOMMSTRU; //�������߽ڵ�ͨѶ�ṹ

typedef struct 
{
	SOCKET_HANDLE socket;      //socketid
	char msgtype;               //��Ϣ����
	int  index;                 //���ӵ���ţ���ʶ��������ĸ������Ϲ�����
	int  connecttime;           //����Ҫʹ�õ�socket��ʱ������ڷ���ʱ��ԭ���ӽ��бȽϣ�����ͬ��ʾ�����Ѷϲ����ã�ȡ������
	char sockettype;            //socket type SOCK_UNKNOW SOCK_CLI SOCK_DREB SOCK_SRV SOCK_TODREB
	DREB_D_NODEINFO closeinfo; //�ر�����ʱ��ԭ������Ϣ����˽ṹ�������̴߳���·�ɱ�
    int rtime;                 //�����ʱ��
	int sendnum;               //���ʹ���
//	int nTotalLen;             //�����ܳ���
	unsigned int nTotalLen;     //�ܹ�Ҫ�������ݳ���
	unsigned int nSendLen;      //��ǰҪ�������ݳ���
	int serial;                //dreb������ˮ
}__PACKED__ DREBQUEUEMSG_HEAD;       //dreb�ڲ�����ʹ�õ�ͷ

typedef struct 
{
	DREBQUEUEMSG_HEAD msghead;  //dreb�ڲ�����ʹ�õ�ͷ
	COMMSTRU          message;  //��Ϣ����
}__PACKED__ S_DREB_RSMSG;       //�������߽ڵ��ڲ����ͽ������ݵĽṹ



typedef struct //����33
{
	char           cConnectType;   //�������� 0�ͻ��� 1����� 2�������߽ڵ� 
	unsigned short nNodeId;        //Ŀ���������߽ڵ���
	char           cNodePrivateId; //�������߽ڵ�˽�����
	unsigned int   nSvrMainId;     //Ŀ������ܺ� ,������д
	char           cSvrPrivateId;  //Ŀ��˽����� 0��ʾ���� >0Ϊÿ���������ţ�������д
	unsigned int   nBandWidth;     //�������߽ڵ����
	char           sVersion[20];   //���Ӱ汾   
}__PACKED__ S_CONNECT_DATA;
typedef struct //����9
{
	unsigned short nNodeId;        //Ŀ���������߽ڵ���
	char           cNodePrivateId; //�������߽ڵ�˽�����
	char           nStep;          //0����1ֱ�����������߽ڵ�>1Ϊ�������߽ڵ��Ĳ���
	unsigned int    nBandWidth;     //������λk
}__PACKED__ S_ROUTE_MSG;    //�������߽ڵ�·�ɽṹ

typedef struct //����15
{
	unsigned short nNodeId;        //Ŀ���������߽ڵ���
	char           cNodePrivateId; //�������߽ڵ�˽�����
	unsigned int   nSvrMainId;     //���������
	char           cSvrPrivateId;  //˽�з����
	char           cStep;          //dreb����
	unsigned short nFuncNum;       //���ܺŸ���
	unsigned int   nFuncNo;        //���ܺ�
}__PACKED__ S_ROUTESVR_MSG;         //�������߽ڵ����·�ɽṹ

typedef struct //����11
{
	unsigned int   nSvrMainId;     //���������
	char           cSvrPrivateId;  //˽�з���� 
	unsigned short nFuncNum;       //���ܺŸ���
	unsigned int   nFuncNo;        //���ܺ�
}__PACKED__ S_SERVICEREG_MSG;         //�������߽ڵ������ע��ṹ	    �㲥���Ľṹ


//����Ϊ���ʹ�ýṹ

//DREBMONI_INFOʹ�ýṹ begin
typedef struct //���� 6
{
	unsigned int   nSvrMainId;     //���������
	char           cSvrPrivateId;  //˽�з����
	char           cClose;         //�Ƿ�ر�
}__PACKED__ S_MONITOR_SVR;         //DREB��ط���ṹ

typedef struct //���� 5
{
	unsigned short nNodeId;        //Ŀ���������߽ڵ���
	char           cNodePrivateId; //�������߽ڵ�˽�����
	unsigned short nSvrNum;         //�������
}__PACKED__ S_MONITOR_INFO;         //DREB�����Ϣ�ṹ

typedef struct //���� 11
{
	S_MONITOR_INFO s_nodeInfo;       //��ǰ�ڵ���Ϣ
	S_MONITOR_SVR  s_svrinfo;        //��ǰ�ڵ������Ϣ���ɰ�������˽ڵ�
}__PACKED__ S_MONITOR_CURDREBINFO;         //DREB��ǰ�ڵ���Ϣ�ṹ
//DREBMONI_INFOʹ�ýṹ end 


//DREBMONI_HOST BPCMONI_HOSTʹ�ýṹ begin
typedef struct //���� 208
{
	char     sDiskName[200];        //������
	unsigned int nTotalSpace;         //�ܴ�С m
	unsigned int nUsedSpace;          //ʹ�ô�С m
}__PACKED__ S_MONITOR_DISK;         //���������Ϣ�ṹ
typedef struct //���� 14
{
	unsigned int nTotalMemory;        //�ڴ�����   m
	unsigned int nUsedMemory;         //ʹ���ڴ��� m
	int          EmsUseRadio;
	unsigned int nCpuRate;            //CPUʹ���� %
	unsigned short nDiskNum;          //������
}__PACKED__ S_MONITOR_HOST;         //���������Ϣ�ṹ
typedef struct //���� 14
{
	S_MONITOR_HOST  s_hostInfo;         //cpu�ڴ���Ϣ
	S_MONITOR_DISK  s_diskInfo;         //������Ϣ�������ж��
}__PACKED__ S_MONITOR_HOSTINFO;         //���������Ϣ�ṹ
//DREBMONI_HOST BPCMONI_HOSTʹ�ýṹ end

//DREBMONI_CURTXLIST ʹ�ýṹ begin
typedef struct  //����15
{
	unsigned int   nSvrMainId;     //ע����DREB���������ܺ�   ֻ�з������ʱ������д
	char           cSvrPrivateId;  //ע����DREB��˽����� 0��ʾ���� >0Ϊÿ���������ţ�s_nSvrMainId+s_cSvrPrivateId ��Ϊ˽�й��ܺ�   ֻ�з������ʱ������д
	unsigned int   nFuncNo;        //�����ܺ�
	unsigned int   nStep;          //0����1ֱ�����������߽ڵ�>1Ϊ�������߽ڵ��Ĳ���
	unsigned int   nIndex;         //��������  ͨ�������������ɵõ��������߽ڵ������
}__PACKED__ S_MONITOR_SERVICEINFO;  
typedef struct  //���� 20
{
	unsigned short nNodeId;        //DREB���
	char           cNodePrivateId; //DREB˽�����	
	unsigned short nTxNum;         //���������
	S_MONITOR_SERVICEINFO  s_txList;  //��������Ϣ �����ж��
}__PACKED__ S_MONITOR_TXLIST;
//DREBMONI_CURTXLIST ʹ�ýṹ end

//DREBMONI_ROUTEDREB ʹ�нṹ begin
typedef struct //����15
{
	unsigned short nNodeId;        //Ŀ���������߽ڵ���
	char           cNodePrivateId; //�������߽ڵ�˽�����
	unsigned int            nStep;          //0����1ֱ�����������߽ڵ�>1Ϊ�������߽ڵ��Ĳ���
	unsigned int            nIndex;         //��������  ͨ�������������ɵõ��������߽ڵ������
	unsigned int            nbandwidth;     //����
}__PACKED__ S_MONITOR_DREB;
typedef struct //����17
{
	unsigned short     nDrebNum;         //DREB����
	S_MONITOR_DREB  s_drebInfo;       //dreb·����Ϣ ���ж��
}__PACKED__ S_MONITOR_ROUTEDREB;
//DREBMONI_ROUTEDREB ʹ�нṹ end

//DREBMONI_ROUTESERVICE ʹ�ýṹ  begin
typedef struct //����20
{
	unsigned short nNodeId;        //DREB���
	char           cNodePrivateId; //DREB˽�����	
	unsigned int   nSvrMainId;     //ע����DREB���������ܺ�   ֻ�з������ʱ������д
	char           cSvrPrivateId;  //ע����DREB��˽����� 0��ʾ���� >0Ϊÿ���������ţ�s_nSvrMainId+s_cSvrPrivateId ��Ϊ˽�й��ܺ�   ֻ�з������ʱ������д
	unsigned int   nFuncNo;        //�����ܺ�
	unsigned int   nStep;          //0����1ֱ�����������߽ڵ�>1Ϊ�������߽ڵ��Ĳ���
	unsigned int   nIndex;         //��������  ͨ�������������ɵõ��������߽ڵ������
}__PACKED__ S_MONITOR_SERVICE;
typedef struct //����22
{
	unsigned short     nServiceNum;         //�����׸���
	S_MONITOR_SERVICE  s_service;           //��������Ϣ
}__PACKED__ S_MONITOR_ROUTESERVICE;
//DREBMONI_ROUTESERVICE ʹ�ýṹ  end

//DREBMONI_REPORT ʹ�ýṹ  begin
typedef struct //����22
{
	char    sStartDate[18];    //��������
	char    sRcvPackClient[21];//���տͻ��˰���
	char    sRcvByteClient[21];//���տͻ����ֽ���
	char    sAnsPackClient[21];//Ӧ��ͻ��˰��� 
	char    sAnsByteClient[21];//Ӧ��ͻ����ֽ���
	
	char    sTransOutNum[21]; //����ת������
	char    sTransOutByte[21];//����ת���ֽ��� 
	char    sTransInNum[21];  //����ת������
	char    sTransInByte[21]; //����ת���ֽ��� 
	
	char    sCallSrvNum[21];   //���÷������
    char    sCallSrvByte[21];  //���÷����ֽ��� 
	char    sSrvAnsNum[21];    //����Ӧ�����
    char    sSrvAnsByte[21];   //����Ӧ���ֽ��� 
}__PACKED__ S_MONITOR_DREBREPORT;
//DREBMONI_REPORT ʹ�ýṹ  end

//DREBMONI_CONNINFO ʹ�ýṹ  begin
typedef struct //����44
{
	char           sIp[33];            //��������IP
	unsigned int   nPort;              //�������߶˿�
	char           cStatus;            //����״̬ 0���� 1Ϊ����
	unsigned short nNodeId;              //�������߱��
	char           cNodePrivateId;       //��������˽�����
	int            nbandwidth;         //����
}__PACKED__ S_MONITOR_CONNINFO;
//DREBMONI_CONNINFO ʹ�ýṹ  end



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


#endif // !defined(AFX_PUBDEFINE_H__F2DFC8C6_AAAD_4A6C_BAEC_249903098AF2__INCLUDED_)
