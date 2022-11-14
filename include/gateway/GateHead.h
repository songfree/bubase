// GateHead.h: interface for the CGateHead class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GATEHEAD_H__1A490998_AE1D_4016_859A_2C1896E2D51B__INCLUDED_)
#define AFX_GATEHEAD_H__1A490998_AE1D_4016_859A_2C1896E2D51B__INCLUDED_

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
	
#define  MSG_GATEPING      1          //�����ص�PING��Ϣ
#define  MSG_GATEREQ       2          //�������ص��������ݣ������ط����ͻ��˵���������
#define  MSG_GATELOGIN     3          //���ص�¼
#define  MSG_GATESMS       4          //���ض���
#define  MSG_PUSH          5          //api�յ���������������������(���顢����)��˽����(�ر���)
#define  MSG_GATEPKISTEP1  6          //����PKI��֤��һ�� ���ط���ǩ����
#define  MSG_GATEPKISTEP2  7          //����PKI��֤�ڶ��� API��������ǩ����֤�鹫Կ��ǩ���ַ���
#define  MSG_GATEPKISTEP3  8          //����PKI��֤������ ���ط���APIǩ����֤�鹫Կ���Ự��Կ
#define  MSG_GATESUBSCRIBE 9          //���ض���

typedef struct  //7
{
	char              cMsgType;        //��Ϣ����  ���غ�API֮�����Ϣ���� ������Ķ��� 
	char              cZip;            //ѹ����־ 0���� 1des������Կ����16 2ѹ�� 3DES���� 4ѹ����DES����
	char              cCmd;            //�������߽ڵ����������֣�Ӧ��ʱ�����任  
	unsigned short    nCrc;            //У��λ
	char              cRaflag;         //����Ӧ���־  0���� 1Ӧ��
	char              cNextFlag;       //��������־  dreb֮�����·��ʹ��ʱ0�װ� 1������  ����ʱ0�װ� 1������2bpcȡ������3bpuȡ������,Ӧ��ʱ0�޺����� 1�к�������2bpc���к�����Ҫȡ3bpu���к�����Ҫȡ 
}__PACKED__ S_CGATE_COMMHEAD;

typedef struct //16
{
	unsigned short nNodeId;        //�������߽ڵ���
	char           cNodePrivateId; //�������߽ڵ�˽�����
	unsigned int   nSvrMainId;     //ע�����������߽ڵ��ϵ��������ܺ�   ֻ�з������ʱ������д  ��Ϊ����㲥ʱ��д��Լ���
	char           cSvrPrivateId;  //ע�����������߽ڵ��ϵ�˽����� 0��ʾ���� >0Ϊÿ���������ţ�s_nSvrMainId+s_cSvrPrivateId ��Ϊ˽�й��ܺ�   ֻ�з������ʱ������д
	unsigned int   nSerial;        //api��ʶΨһ���׵���ˮ;
	unsigned int   nServiceNo;     //Ҫ���õķ����ܺ� ������
}__PACKED__ S_CGATE_DESTINFO;

typedef struct  //���� 7+16+2+4+2 = 31
{
	S_CGATE_COMMHEAD  stComm;            //ͨѶ����
	S_CGATE_DESTINFO  stDest;            //Ŀ����Ϣ
	unsigned short    nRetCode;          //ͨѶ���񷵻��� 0�ɹ�
	NEXT_INFO         stNext;          //�����Ϣ         Ӧ����д������ȡ��������д ��Ϊ����㲥ʱ��д��������n_nNextOffset���������n_nNextNo
	unsigned short    nLen;            //�����ҵ�����ݳ���
}__PACKED__ CGATE_HEAD,*PCGATE_HEAD;     //API��cgateͨѶ��ͷ  



#define  CGATEHEADLEN sizeof(CGATE_HEAD)  

#define  CLIENTDATASIZE  (DREBBUFSIZE-DREBHEADLEN-CGATEHEADLEN)

typedef struct 
{
	CGATE_HEAD   head;                                   //ͨѶͷ
	char         buffer[CLIENTDATASIZE];                    //�������߽ڵ��ﴫ�������
}__PACKED__ CGATE_COMMSTRU,*PCGATE_COMMSTRU;             //API������ͨѶ�ṹ


typedef struct 
{
	int             rtime;                 //�����ʱ��
	unsigned short  d_nNodeId;        //Ŀ���������߽ڵ���
	char            d_cNodePrivateId; //�������߽ڵ�˽�����
	unsigned int    s_nDrebSerial;    //������ˮ��ʶ
	unsigned short  isBC;             //�Ƿ�㲥
	int             index;            //��������
	int             timestamp;        //ʱ���
	int             txcode;           //������
	int             prio;             //���ȼ�
	int             nkey;             //  	 �㲥��key����Ӧ���ߵ�s_nHook�ֶ�
	CGATE_COMMSTRU  data;               //��Ϣ����
}__PACKED__ S_CGATE_SMSG;               //�������ݵĽṹ


typedef struct 
{
	int             rtime;                 //�����ʱ��
	unsigned int    sendnum;               //���ʹ���
	unsigned int    offset;     //����λ�� ������ǰ���͵�����λ�ã���Ҫ�Ƿ���һ�������ݵ����
	unsigned short  sendlen;    //�������ݳ���
	unsigned short  totallen;   //�����ݳ���
	unsigned int    serial;    //��ˮ
	CGATE_COMMSTRU  message;               //��Ϣ����
}__PACKED__ S_CGATEAPI_SMSG;               //�������ݵĽṹ


typedef struct 
{
	unsigned short  nSignBufferLen;           //ǩ�����ݳ���
	char            sSignBuffer[501];         //ǩ�����ݻ���
	unsigned short  nSignStrLen;        //Ҫǩ�����ַ�������(������ǩ��) �������ɵĻỰ��Կ(api��Կ����)����
	char            sSignStr[501];      //Ҫǩ�����ַ���(������ǩ��)  �������ɵĻỰ��Կ(api��Կ����)
	unsigned short  nSignPubKeyLen;     //ǩ����Կ����
	char            sSignPubKey[2049];  //ǩ����Կ
	unsigned short  nRsaPubKeyLen;      //�ǶԳƼ��ܹ�Կ����
	char            sRsaPubKey[2049];   //�ǶԳƼ��ܹ�Կ
	char            sKeySerial[61];     //֤�����к�
	unsigned short  nKeySerialLen;      //֤�����кų���
	char            sIndex[10];//�������
}__PACKED__ PKI_GATESTEP23,*PPKI_GATESTEP23;             //API��������֤�ڶ���ͨѶ�ṹ


//typedef struct 
//{
//	int             rtime;                 //�����ʱ��
//	unsigned int    sendnum;               //���ʹ���
//	unsigned int    offset;     //����λ�� ������ǰ���͵�����λ�ã���Ҫ�Ƿ���һ�������ݵ����
//	unsigned short  sendlen;    //�������ݳ���
//	unsigned short  totallen;   //�����ݳ���
//	unsigned int    serial;    //dreb��ˮ
//	unsigned int    nIndex;    //dreb��������
//	PCOMMSTRU       data; 
//}__PACKED__ S_GATEDREB_RSMSG,*PS_GATEDREB_RSMSG; //�������ݽṹ



typedef struct _S_SUBSCRIBE_
{
    unsigned int nServiceNo;   //Ҫ���ĵĹ��ܺ� ������
    unsigned int nKey;		   //������Ϣ��key    key����ҵ��������壬������Ϊ֤ȯ���룬ί�лر��ɽ��ر�Ϊ�˻�id
}S_SUBSCRIBE_;
typedef struct 
{
	char            flag;             //0 ȡ������   1 �����ܺ�+key����
	unsigned int    datanum;          //��������
	S_SUBSCRIBE_    subinfo;          //������Ϣ
}__PACKED__ S_GATE_SUBSCRIBE,*PS_GATE_SUBSCRIBE;   //�����ض��ĵı��Ľṹ

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
