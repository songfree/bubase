// bpc.cpp : Defines the entry point for the console application.
//



#include "public.h"
#include "GateApi.h"
#include "GateHead.h"
#include "BF_Tools.h"
#include "BF_Thread.h"
#include "BF_AIO_Head.h"
#include "BF_SimpleLog.h"

#ifdef WIN32
#ifdef _DEBUG
#pragma comment(lib, "bf_cgateapid.lib") 
#pragma comment(lib, "bf_dbpubd.lib") 
#pragma comment(lib, "bf_kerneld.lib") 
#pragma message("Automatically linking with bf_cgateapid.lib bf_dbpubd.lib  bf_kerneld.lib")
#else
#pragma comment(lib, "bf_cgateapi.lib") 
#pragma comment(lib, "bf_dbpub.lib") 
#pragma comment(lib, "bf_kernel.lib") 
#pragma message("Automatically linking with bf_cgateapi.lib bf_dbpub.lib  bf_kernel.lib")
#endif

#endif

#include "BF_Des.h"


CBF_SimpleLog m_pLog;

class TestApiReqThread : public CBF_Thread
{
public:
	TestApiReqThread()
	{

	}
	~TestApiReqThread()
	{

	}
private:
	virtual int Run()
	{
		int ret;
		int quotenum=0;
		CGATE_COMMSTRU data;
		int rtime = time(NULL);
		while(true)
		{
			ret = BFCGATE_GetReqData(&data,5);
			if (ret >0)
			{
				//����
				quotenum++;
				int etime = time(NULL);
				if (etime - rtime > 5)
				{
					rtime = etime;
					m_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,"ȡ���㲥����%d",quotenum);
					m_pLog.LogBin(LOG_PROMPT,__FILE__,__LINE__,"�յ�����",data.buffer,data.head.nLen);
				}
				printf("�յ����� ����node[%d %d] svr [%d %d]�Ĺ��ܺ�[%d]����Ϣ \n", data.head.stDest.nNodeId,data.head.stDest.cNodePrivateId, data.head.stDest.nSvrMainId, data.head.stDest.cSvrPrivateId,data.head.stDest.nServiceNo);
			}
		}
	}
};


bool UnzipBuf(PSOCKET_POOL_DATA info,CGATE_COMMSTRU &data)
{
	CBF_Des m_pDes;
	//0���� 1des������Կ����16 2ѹ�� 3DES���� 4DES���ܲ�ѹ��
	if (data.head.nLen == 0)
	{
		return true;
	}
	char zipDataBuf[CLIENTDATASIZE];
	bzero(zipDataBuf,sizeof(zipDataBuf));
	unsigned int dlen = CLIENTDATASIZE;
	int ddlen;
	unsigned short shortlen;
	switch (data.head.stComm.cZip)
	{
		case 1:
			ddlen = data.head.nLen; 
			if (m_pDes.UncryptString(data.buffer,ddlen,info->s_sSessionKey)<1)
			{
				m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ���");
				return false;
			}
			data.head.nLen = ddlen;
			data.head.stComm.cZip =0;
			break;
		case 2:	
			memcpy(zipDataBuf,data.buffer,data.head.nLen);
			if (!CBF_Tools::Uncompress((unsigned char *)(data.buffer),dlen,(unsigned char *)zipDataBuf,data.head.nLen))
			{
				m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"��ѹ������");
				return false;
			}
			data.head.nLen = dlen;
			data.head.stComm.cZip =0;
			break;
		case 3:
			ddlen = data.head.nLen; 
			if (m_pDes.UncryptString(data.buffer,ddlen,info->s_sSessionKey)<1)
			{
				m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ���");
				return false;
			}
			data.head.nLen = ddlen;
			data.head.stComm.cZip =0;
			break;
		case 4:
			m_pLog.LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"����ǰ����",data.buffer,data.head.nLen);
			ddlen = data.head.nLen; 
			if (m_pDes.UncryptString(data.buffer,ddlen,info->s_sSessionKey)<1)
			{
				m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ���");
				return false;
			}
			data.head.nLen = ddlen;
			memcpy(zipDataBuf,data.buffer,data.head.nLen);
			m_pLog.LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"���ܺ�����",zipDataBuf,data.head.nLen);
			if (!CBF_Tools::Uncompress((unsigned char *)(data.buffer),dlen,(unsigned char *)zipDataBuf,data.head.nLen))
			{
				m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"��ѹ������");
				return false;
			}
			data.head.nLen = dlen;
			m_pLog.LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"��ѹ��������",data.buffer,data.head.nLen);
			data.head.stComm.cZip =0;
			break;
// 		case 14:
// 			m_pLog.LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"����ǰ����",data.buffer,data.head.nLen);
// 			ddlen = data.head.nLen; 
// 			if (m_pDes.UncryptString(data.buffer,ddlen,DESENCRYPTKEY3)<1)
// 			{
// 				m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ���");
// 				return false;
// 			}
// 			data.head.nLen = ddlen;
// 			memcpy(zipDataBuf,data.buffer,data.head.nLen);
// 			m_pLog.LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"���ܺ�����",zipDataBuf,data.head.nLen);
// 			if (!CBF_Tools::Uncompress((unsigned char *)(data.buffer),dlen,(unsigned char *)zipDataBuf,data.head.nLen))
// 			{
// 				m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"��ѹ������");
// 				return false;
// 			}
// 			data.head.nLen = dlen;
// 			m_pLog.LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"��ѹ��������",data.buffer,data.head.nLen);
// 			data.head.stComm.cZip =0;
// 			break;
// 		case 15:
// 			if (m_pRes->g_nUseCert == 1)
// 			{
// 				shortlen = data.head.nLen;
// 				if (!m_pRes->g_pCertClass->UnEncrypt(data.buffer,shortlen))
// 				{
// 					m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ���");
// 					return false;
// 				}
// 				data.head.nLen = shortlen;
// 			}
// 			
// 			memcpy(zipDataBuf,data.buffer,data.head.nLen);
// 			if (!CBF_Tools::Uncompress((unsigned char *)(data.buffer),dlen,(unsigned char *)zipDataBuf,data.head.nLen))
// 			{
// 				m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"��ѹ������");
// 				return false;
// 			}
// 			data.head.nLen = dlen;
// 			data.head.stComm.cZip =0;
// 			break;
		default:
			return true;
	}
	return true;
}
bool ZipBuf(PSOCKET_POOL_DATA info,PCGATE_COMMSTRU  data,int nZipFlag)
{
	CBF_Des m_pDesSend;
	//0���� 1des������Կ����16 2ѹ�� 3DES���� 4DES���ܲ�ѹ��
	if (data->head.stComm.cZip != 0)
	{
		return false;
	}
	if (data->head.nLen == 0)
	{
		return true;
	}
	char zipDataBuf[CLIENTDATASIZE];
	bzero(zipDataBuf,sizeof(zipDataBuf));
	unsigned int dlen = CLIENTDATASIZE;
	int ddlen;
	unsigned short shortlen;
	switch (nZipFlag)
	{
		case 1:
			ddlen = data->head.nLen ;
// 			if (ddlen*2 <CLIENTDATASIZE)
// 			{
// 				if (m_pDes.Encryptchar(data->buffer,ddlen,info->s_sSessionKey)<1)
// 				{
// 					m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ���");
// 					return false;
// 				}
// 				data->head.nLen = ddlen;
// 				data->head.stComm.cZip =1;
// 			}
			if (m_pDesSend.EncryptString(data->buffer,ddlen,info->s_sSessionKey)<1)
			{
				m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ���");
				return false;
			}
			data->head.nLen = ddlen;
			data->head.stComm.cZip =1;
			break;
		case 2:	
			memcpy(zipDataBuf,data->buffer,data->head.nLen);
			if (!CBF_Tools::Compress((unsigned char *)(data->buffer),dlen,(unsigned char *)(zipDataBuf),data->head.nLen))
			{
				m_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"����ѹ��ʧ��");
				return false;
			}
			data->head.nLen = dlen;
			data->head.stComm.cZip =2;
			break;
		case 3:
			ddlen = data->head.nLen ;
// 			if (ddlen*2 < CLIENTDATASIZE)
// 			{
// 				if (m_pDes.Encryptchar(data->buffer,ddlen,info->s_sSessionKey)<1)
// 				{
// 					m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ���");
// 					return false;
// 				}
// 				data->head.nLen = ddlen;
// 				data->head.stComm.cZip =3;
// 			}
			if (m_pDesSend.EncryptString(data->buffer,ddlen,info->s_sSessionKey)<1)
			{
				m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ���");
				return false;
			}
			data->head.nLen = ddlen;
			data->head.stComm.cZip =3;
			break;
		case 4:
			memcpy(zipDataBuf,data->buffer,data->head.nLen);
			m_pLog.LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"ѹ��ǰ����",data->buffer,data->head.nLen);
			if (!CBF_Tools::Compress((unsigned char *)data->buffer,dlen,(unsigned char *)(zipDataBuf),data->head.nLen))
			{
				m_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"����ѹ��ʧ��");
				return false;
			}
			data->head.nLen = dlen;
			ddlen = dlen;
			m_pLog.LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"����ǰ����",data->buffer,data->head.nLen);
			if (m_pDesSend.EncryptString(data->buffer,ddlen,info->s_sSessionKey)<1)
			{
				m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ���");
				return false;
			}
			data->head.nLen = ddlen;
			m_pLog.LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"���ܺ�����",data->buffer,data->head.nLen);
			data->head.stComm.cZip =4;
			break;
// 		case 14:
// 			memcpy(zipDataBuf,data->buffer,data->head.nLen);
// 			m_pLog.LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"ѹ��ǰ����",data->buffer,data->head.nLen);
// 			if (!CBF_Tools::Compress((unsigned char *)data->buffer,dlen,(unsigned char *)(zipDataBuf),data->head.nLen))
// 			{
// 				m_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"����ѹ��ʧ��");
// 				return false;
// 			}
// 			data->head.nLen = dlen;
// 			ddlen = dlen;
// 			m_pLog.LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"����ǰ����",data->buffer,data->head.nLen);
// 			if (m_pDesSend.EncryptString(data->buffer,ddlen,DESENCRYPTKEY3)<1)
// 			{
// 				m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ���");
// 				return false;
// 			}
// 			data->head.nLen = ddlen;
// 			m_pLog.LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"���ܺ�����",data->buffer,data->head.nLen);
// 			data->head.stComm.cZip =14;
// 			break;
// 		case 15:
// 			memcpy(zipDataBuf,data->buffer,data->head.nLen);
// 			if (!CBF_Tools::Compress((unsigned char *)data->buffer,dlen,(unsigned char *)(zipDataBuf),data->head.nLen))
// 			{
// 				m_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"����ѹ��ʧ��");
// 				return false;
// 			}
// 			data->head.nLen = dlen;
// 			if (m_pRes->g_nUseCert == 1)
// 			{
// 				shortlen = data->head.nLen;
// 				if (m_pRes->g_pCertClass->Encrypt(info->s_sEncryptPubKey,info->s_nEncryptPubKeyLen,data->buffer,shortlen)<1)
// 				{
// 					m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ���");
// 					return false;
// 				}
// 				data->head.nLen = shortlen;
// 			}
// 			
// 			data->head.stComm.cZip =15;
// 			break;
		default:
			return true;
	}
	return true;
}


int main(int argc, char* argv[])
{
// 	char key[33];
// 	char testdata[8192];
// 	strcpy(testdata,"00000800#MsgType=2#CheckFlag=0##ApiName=onRecvForwardQuotation#DataType=TForwardQuotation#Flag=L#NodeID=6020#NodeType=6#Posi=0#RootID=#RspCode=RSP000000#RspMsg=���׳ɹ�#ask1=0.00000000#ask2=0.00000000#ask3=0.00000000#ask4=0.00000000#ask5=0.00000000#askLot1=0#askLot2=0#askLot3=0#askLot4=0#askLot5=0#average=0.00000000#bid1=0.00000000#bid2=0.00000000#bid3=0.00000000#bid4=0.00000000#bid5=0.00000000#bidLot1=0#bidLot2=0#bidLot3=0#bidLot4=0#bidLot5=0#close=0.00000000#high=0.00000000#highLimit=8250.00000000#instID=Ag99.99#last=0.00000000#lastClose=7500.00000000#lastSettle=7500.00000000#low=0.00000000#lowLimit=6750.00000000#name=Ag99.99#open=0.00000000#quoteDate=20130306#quoteTime=16:50:53#sequenceNo=0#settle=0.00000000#turnOver=0.00000000#upDown=0.00000000#upDownRate=0.00000000#volume=0#weight=0.00000000#");
// 	strcpy(key,"7844817331733094");
// 
// 	CBF_Des m_pDesSend;
// 
// 	char zipDataBuf[CLIENTDATASIZE];
// 	bzero(zipDataBuf,sizeof(zipDataBuf));
// 	char zipDataBufdst[CLIENTDATASIZE];
// 	bzero(zipDataBufdst,sizeof(zipDataBufdst));
// 	unsigned int dlen = CLIENTDATASIZE;
// 	int ddlen;
// 	unsigned short shortlen;
// 	ddlen = strlen(testdata);
// 	memcpy(zipDataBuf,testdata,ddlen);
// 	m_pLog.LogBin(LOG_DEBUG,__FILE__,__LINE__,"ѹ��ǰ����",zipDataBuf,ddlen);
// 	if (!CBF_Tools::Compress((unsigned char *)zipDataBufdst,dlen,(unsigned char *)(zipDataBuf),ddlen))
// 	{
// 		m_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"����ѹ��ʧ��");
// 		return false;
// 	}
// 	
// 	m_pLog.LogBin(LOG_DEBUG,__FILE__,__LINE__,"ѹ�������ǰ����",zipDataBufdst,dlen);
// 	ddlen = dlen;
// 	if (m_pDesSend.EncryptString(zipDataBufdst,ddlen,key)<1)
// 	{
// 		m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ���");
// 		return false;
// 	}
// 	m_pLog.LogBin(LOG_DEBUG,__FILE__,__LINE__,"ѹ�����ܺ�����",zipDataBufdst,ddlen);
// 
// 	
// 	m_pLog.LogBin(LOG_DEBUG,__FILE__,__LINE__,"���ܽ�ѹǰ����",zipDataBufdst,ddlen);
// 	
// 	if (m_pDesSend.UncryptString(zipDataBufdst,ddlen,key)<1)
// 	{
// 		m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ���");
// 		return false;
// 	}
// 	m_pLog.LogBin(LOG_DEBUG,__FILE__,__LINE__,"���ܺ��ѹǰ����",zipDataBufdst,ddlen);
// 	dlen = CLIENTDATASIZE;
// 	if (!CBF_Tools::Uncompress((unsigned char *)(zipDataBuf),dlen,(unsigned char *)zipDataBufdst,ddlen))
// 	{
// 		m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"��ѹ������");
// 		return false;
// 	}
// 	
// 	m_pLog.LogBin(LOG_DEBUG,__FILE__,__LINE__,"��ѹ��������",zipDataBuf,dlen);
// 		


	if (!BFCGATE_InitApi("cgateapi.xml"))
	{
		printf("api��ʼ��ʧ��\n");
		return -1;
	}

	if (!BFCGATE_ConnectGate())
	{
		printf("��������ʧ��\n");
		return -1;
	}
	int ret;
	TestApiReqThread reqthread;
	m_pLog.SetLogPara(LOG_DEBUG,"","req.log");
	reqthread.CreateThread();

	CGATE_COMMSTRU  data;
	bzero(&data,sizeof(CGATE_COMMSTRU));
	char tmpchar[30];
	int serial =1;
	int timeout;
	CGATE_COMMSTRU  globaldata;
	char txfilename[200];
	bzero(txfilename,sizeof(txfilename));
	int filelen= sizeof(txfilename);
	
	ret = CBF_Tools::GetConsoleEnter("�����������ļ����س�����",filelen-1,txfilename);
	txfilename[ret]=0;
	FILE *fp =fopen(txfilename,"r");
	if (fp == NULL)
	{
		printf("%s ������\n",txfilename);
		return -1;
	}
	bzero(&globaldata,sizeof(CGATE_COMMSTRU));
	globaldata.head.nLen = fread(globaldata.buffer,1,8192,fp);
	fclose(fp);
	ret = CBF_Tools::GetConsoleEnter("�����뽻���룬�س�����",8,tmpchar);
	tmpchar[ret] =0;
	globaldata.head.stDest.nServiceNo = atoi(tmpchar);
	data.head.nRetCode = 0;

	ret = CBF_Tools::GetConsoleEnter("��������ճ�ʱʱ��(��)���س�����",2,tmpchar);
	tmpchar[ret] =0;
	timeout = atoi(tmpchar);

	int sendtime;


	ret = CBF_Tools::GetConsoleEnter("�����뷢�ͼ��(����)���س�����",4,tmpchar);
	tmpchar[ret] =0;
	sendtime = atoi(tmpchar);
	
	bzero(&data,sizeof(CGATE_COMMSTRU));

	globaldata.head.stDest.nNodeId =0;
	
	globaldata.head.stDest.nSvrMainId = 0;

	

	globaldata.head.stDest.cNodePrivateId = 0;
	globaldata.head.stDest.cSvrPrivateId = 0;
	globaldata.head.stComm.cRaflag = 0;
	globaldata.head.stDest.nSerial = serial;
	serial++;
	globaldata.head.stNext.n_nNextNo = 0;
	globaldata.head.stNext.n_nNextOffset = 0;

	

// 	while (true)
// 	{
// 		ret = CBF_Tools::GetConsoleEnter("��������Ϣ����8:CMD_DPCALL 9:CMD_DPACALL 10:CMD_DPBC 11:CMD_DPABC 12:CMD_DPPUSH 13:CMD_DPPOST���س�����",12,tmpchar);
// 		tmpchar[ret] =0;
// 		globaldata.head.stComm.cCmd = atoi(tmpchar);
// 		if (globaldata.head.stComm.cCmd < 8 || globaldata.head.stComm.cCmd > 13)
// 		{
// 			continue;	
// 		}
// 		break;
// 	}
// 	ret = CBF_Tools::GetConsoleEnter("����������ȷ��1:��ȷ 2:�˳����س�����",12,tmpchar);
// 	tmpchar[ret] =0;

	
// 	ret = atoi(tmpchar);
// 	if (ret == 2)
// 	{
// 		BFCGATE_Logout();
// 		return -1;
// 	}

	globaldata.head.stComm.cCmd = CMD_DPCALL; 
	

	int packnum=0;
	int sendnum=0;
	while (true)
	{
		if (BFCGATE_GetConnectStatus()!=1)
		{
			if (!BFCGATE_ConnectGate())
			{
				printf("��������ʧ��\n");
				SLEEP(sendtime);
				continue;
			}
		}
		packnum = 0;
		globaldata.head.stDest.nSerial++;
		memcpy(&data,&globaldata,sizeof(CGATE_COMMSTRU));
		printf("����%d����\n",globaldata.head.stDest.nServiceNo);
		
		ret = BFCGATE_SendLogin(&data,timeout);
		sendnum++;
		if (ret > 0)
		{
			printf("�յ���̨Ӧ�� nRet[%d] nLen[%d]\n",data.head.nRetCode,data.head.nLen);
			if (data.head.stComm.cNextFlag == 0)
			{
				printf("�޺�����\n");
				//�����ض���	 keyΪ9021 9022 9023�����ݣ�9021����Ϊ��̨�ر���Ľ����˻� 
                CGATE_COMMSTRU data;
				bzero(&data.head,CGATEHEADLEN);
                S_GATE_SUBSCRIBE* subscribe = (S_GATE_SUBSCRIBE*)(data.buffer);
                subscribe->flag = 2;
                subscribe->datanum = 3;
                subscribe->variety = 9021;
				int *key1 = (int *)(data.buffer+ sizeof(S_GATE_SUBSCRIBE));
				*key1 = 9022;
				key1 = (int*)(data.buffer + sizeof(S_GATE_SUBSCRIBE)+4);
				*key1 = 9023;
				data.head.nLen = sizeof(S_GATE_SUBSCRIBE)+4+4;
                BFCGATE_Subscribe(&data, timeout);
				//�����ض������� 10011 20012 10013��Լ������
                subscribe = (S_GATE_SUBSCRIBE*)(data.buffer);
                subscribe->flag = 1;
                subscribe->datanum = 3;
                subscribe->variety = 10011;
                key1 = (int*)(data.buffer + sizeof(S_GATE_SUBSCRIBE));
                *key1 = 10012;
                key1 = (int*)(data.buffer + sizeof(S_GATE_SUBSCRIBE) + 4);
                *key1 = 10013;
                data.head.nLen = sizeof(S_GATE_SUBSCRIBE) + 4 + 4;
                BFCGATE_Subscribe(&data, timeout);
				SLEEP(sendtime);
				continue;
			}
			
			//��̨�������ͣ���ͣ����
			if (data.head.stComm.cNextFlag == 1 || data.head.stComm.cNextFlag == 10)
			{
				packnum++;
				while (true)
				{
					printf("�յ�Ӧ������ݰ��� %d",packnum);
					if (packnum >= data.head.stNext.n_nNextNo) //�ܰ���
					{
						printf("Ӧ�������ݰ���Ŀ %d\n",packnum);
						break;
					}
					ret = BFCGATE_GetAnsData(&data,timeout);
					if (ret != 1)
					{
						printf("����Ӧ����� %d",ret);
						break;
					}
					packnum++;
				}
			}
			else
			{
				packnum++;
				while (data.head.stComm.cNextFlag <10)
				{
					printf("�յ�Ӧ������ݰ��� %d",packnum);
					ret = BFCGATE_GetAnsData(&data,timeout);
					if (ret != 1)
					{
						printf("����Ӧ����� %d",ret);
						break;
					}
					packnum++;
				}
				printf("�յ�Ӧ������ݰ��� %d",packnum);
			}
			////����
			//CGATE_COMMSTRU data;
			//S_GATE_SUBSCRIBE   *subscribe=(S_GATE_SUBSCRIBE *)(data.buffer);
			//subscribe->flag = 2;
			//subscribe->datanum = 1;
			//subscribe->variety = 9021;
			//BFCGATE_Subscribe(&data, timeout);
		}
		else if (ret == 0)
		{
			printf("��ʱδ�յ�Ӧ��\n");
		}
		else if (ret == -2)
		{
			printf("���ͳ���\n");
			if (!BFCGATE_ConnectGate())
			{
				printf("��������ʧ��\n");
				return -1;
			}
		}
		else
		{
			printf("���ӶϿ�\n");
			if (!BFCGATE_ConnectGate())
			{
				printf("��������ʧ��\n");
				return -1;
			}
		}
		SLEEP(sendtime);
		SLEEP_SECONDS(60);
	}
	return 0;
}

