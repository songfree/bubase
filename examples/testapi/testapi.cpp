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
				//处理
				quotenum++;
				int etime = time(NULL);
				if (etime - rtime > 5)
				{
					rtime = etime;
					m_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,"取到广播总数%d",quotenum);
					m_pLog.LogBin(LOG_PROMPT,__FILE__,__LINE__,"收到报文",data.buffer,data.head.nLen);
				}
				printf("收到数据 来自node[%d %d] svr [%d %d]的功能号[%d]的消息 \n", data.head.stDest.nNodeId,data.head.stDest.cNodePrivateId, data.head.stDest.nSvrMainId, data.head.stDest.cSvrPrivateId,data.head.stDest.nServiceNo);
			}
		}
	}
};


bool UnzipBuf(PSOCKET_POOL_DATA info,CGATE_COMMSTRU &data)
{
	CBF_Des m_pDes;
	//0明文 1des加密密钥长度16 2压缩 3DES加密 4DES加密并压缩
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
				m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"解密出错");
				return false;
			}
			data.head.nLen = ddlen;
			data.head.stComm.cZip =0;
			break;
		case 2:	
			memcpy(zipDataBuf,data.buffer,data.head.nLen);
			if (!CBF_Tools::Uncompress((unsigned char *)(data.buffer),dlen,(unsigned char *)zipDataBuf,data.head.nLen))
			{
				m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"解压缩出错");
				return false;
			}
			data.head.nLen = dlen;
			data.head.stComm.cZip =0;
			break;
		case 3:
			ddlen = data.head.nLen; 
			if (m_pDes.UncryptString(data.buffer,ddlen,info->s_sSessionKey)<1)
			{
				m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"解密出错");
				return false;
			}
			data.head.nLen = ddlen;
			data.head.stComm.cZip =0;
			break;
		case 4:
			m_pLog.LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"解密前数据",data.buffer,data.head.nLen);
			ddlen = data.head.nLen; 
			if (m_pDes.UncryptString(data.buffer,ddlen,info->s_sSessionKey)<1)
			{
				m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"解密出错");
				return false;
			}
			data.head.nLen = ddlen;
			memcpy(zipDataBuf,data.buffer,data.head.nLen);
			m_pLog.LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"解密后数据",zipDataBuf,data.head.nLen);
			if (!CBF_Tools::Uncompress((unsigned char *)(data.buffer),dlen,(unsigned char *)zipDataBuf,data.head.nLen))
			{
				m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"解压缩出错");
				return false;
			}
			data.head.nLen = dlen;
			m_pLog.LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"解压缩后数据",data.buffer,data.head.nLen);
			data.head.stComm.cZip =0;
			break;
// 		case 14:
// 			m_pLog.LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"解密前数据",data.buffer,data.head.nLen);
// 			ddlen = data.head.nLen; 
// 			if (m_pDes.UncryptString(data.buffer,ddlen,DESENCRYPTKEY3)<1)
// 			{
// 				m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"解密出错");
// 				return false;
// 			}
// 			data.head.nLen = ddlen;
// 			memcpy(zipDataBuf,data.buffer,data.head.nLen);
// 			m_pLog.LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"解密后数据",zipDataBuf,data.head.nLen);
// 			if (!CBF_Tools::Uncompress((unsigned char *)(data.buffer),dlen,(unsigned char *)zipDataBuf,data.head.nLen))
// 			{
// 				m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"解压缩出错");
// 				return false;
// 			}
// 			data.head.nLen = dlen;
// 			m_pLog.LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"解压缩后数据",data.buffer,data.head.nLen);
// 			data.head.stComm.cZip =0;
// 			break;
// 		case 15:
// 			if (m_pRes->g_nUseCert == 1)
// 			{
// 				shortlen = data.head.nLen;
// 				if (!m_pRes->g_pCertClass->UnEncrypt(data.buffer,shortlen))
// 				{
// 					m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"解密出错");
// 					return false;
// 				}
// 				data.head.nLen = shortlen;
// 			}
// 			
// 			memcpy(zipDataBuf,data.buffer,data.head.nLen);
// 			if (!CBF_Tools::Uncompress((unsigned char *)(data.buffer),dlen,(unsigned char *)zipDataBuf,data.head.nLen))
// 			{
// 				m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"解压缩出错");
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
	//0明文 1des加密密钥长度16 2压缩 3DES加密 4DES加密并压缩
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
// 					m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"加密出错");
// 					return false;
// 				}
// 				data->head.nLen = ddlen;
// 				data->head.stComm.cZip =1;
// 			}
			if (m_pDesSend.EncryptString(data->buffer,ddlen,info->s_sSessionKey)<1)
			{
				m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"加密出错");
				return false;
			}
			data->head.nLen = ddlen;
			data->head.stComm.cZip =1;
			break;
		case 2:	
			memcpy(zipDataBuf,data->buffer,data->head.nLen);
			if (!CBF_Tools::Compress((unsigned char *)(data->buffer),dlen,(unsigned char *)(zipDataBuf),data->head.nLen))
			{
				m_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"数据压缩失败");
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
// 					m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"加密出错");
// 					return false;
// 				}
// 				data->head.nLen = ddlen;
// 				data->head.stComm.cZip =3;
// 			}
			if (m_pDesSend.EncryptString(data->buffer,ddlen,info->s_sSessionKey)<1)
			{
				m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"加密出错");
				return false;
			}
			data->head.nLen = ddlen;
			data->head.stComm.cZip =3;
			break;
		case 4:
			memcpy(zipDataBuf,data->buffer,data->head.nLen);
			m_pLog.LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"压缩前数据",data->buffer,data->head.nLen);
			if (!CBF_Tools::Compress((unsigned char *)data->buffer,dlen,(unsigned char *)(zipDataBuf),data->head.nLen))
			{
				m_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"数据压缩失败");
				return false;
			}
			data->head.nLen = dlen;
			ddlen = dlen;
			m_pLog.LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"加密前数据",data->buffer,data->head.nLen);
			if (m_pDesSend.EncryptString(data->buffer,ddlen,info->s_sSessionKey)<1)
			{
				m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"加密出错");
				return false;
			}
			data->head.nLen = ddlen;
			m_pLog.LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"加密后数据",data->buffer,data->head.nLen);
			data->head.stComm.cZip =4;
			break;
// 		case 14:
// 			memcpy(zipDataBuf,data->buffer,data->head.nLen);
// 			m_pLog.LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"压缩前数据",data->buffer,data->head.nLen);
// 			if (!CBF_Tools::Compress((unsigned char *)data->buffer,dlen,(unsigned char *)(zipDataBuf),data->head.nLen))
// 			{
// 				m_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"数据压缩失败");
// 				return false;
// 			}
// 			data->head.nLen = dlen;
// 			ddlen = dlen;
// 			m_pLog.LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"加密前数据",data->buffer,data->head.nLen);
// 			if (m_pDesSend.EncryptString(data->buffer,ddlen,DESENCRYPTKEY3)<1)
// 			{
// 				m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"加密出错");
// 				return false;
// 			}
// 			data->head.nLen = ddlen;
// 			m_pLog.LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"加密后数据",data->buffer,data->head.nLen);
// 			data->head.stComm.cZip =14;
// 			break;
// 		case 15:
// 			memcpy(zipDataBuf,data->buffer,data->head.nLen);
// 			if (!CBF_Tools::Compress((unsigned char *)data->buffer,dlen,(unsigned char *)(zipDataBuf),data->head.nLen))
// 			{
// 				m_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"数据压缩失败");
// 				return false;
// 			}
// 			data->head.nLen = dlen;
// 			if (m_pRes->g_nUseCert == 1)
// 			{
// 				shortlen = data->head.nLen;
// 				if (m_pRes->g_pCertClass->Encrypt(info->s_sEncryptPubKey,info->s_nEncryptPubKeyLen,data->buffer,shortlen)<1)
// 				{
// 					m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"加密出错");
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
// 	strcpy(testdata,"00000800#MsgType=2#CheckFlag=0##ApiName=onRecvForwardQuotation#DataType=TForwardQuotation#Flag=L#NodeID=6020#NodeType=6#Posi=0#RootID=#RspCode=RSP000000#RspMsg=交易成功#ask1=0.00000000#ask2=0.00000000#ask3=0.00000000#ask4=0.00000000#ask5=0.00000000#askLot1=0#askLot2=0#askLot3=0#askLot4=0#askLot5=0#average=0.00000000#bid1=0.00000000#bid2=0.00000000#bid3=0.00000000#bid4=0.00000000#bid5=0.00000000#bidLot1=0#bidLot2=0#bidLot3=0#bidLot4=0#bidLot5=0#close=0.00000000#high=0.00000000#highLimit=8250.00000000#instID=Ag99.99#last=0.00000000#lastClose=7500.00000000#lastSettle=7500.00000000#low=0.00000000#lowLimit=6750.00000000#name=Ag99.99#open=0.00000000#quoteDate=20130306#quoteTime=16:50:53#sequenceNo=0#settle=0.00000000#turnOver=0.00000000#upDown=0.00000000#upDownRate=0.00000000#volume=0#weight=0.00000000#");
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
// 	m_pLog.LogBin(LOG_DEBUG,__FILE__,__LINE__,"压缩前数据",zipDataBuf,ddlen);
// 	if (!CBF_Tools::Compress((unsigned char *)zipDataBufdst,dlen,(unsigned char *)(zipDataBuf),ddlen))
// 	{
// 		m_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"数据压缩失败");
// 		return false;
// 	}
// 	
// 	m_pLog.LogBin(LOG_DEBUG,__FILE__,__LINE__,"压缩后加密前数据",zipDataBufdst,dlen);
// 	ddlen = dlen;
// 	if (m_pDesSend.EncryptString(zipDataBufdst,ddlen,key)<1)
// 	{
// 		m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"加密出错");
// 		return false;
// 	}
// 	m_pLog.LogBin(LOG_DEBUG,__FILE__,__LINE__,"压缩加密后数据",zipDataBufdst,ddlen);
// 
// 	
// 	m_pLog.LogBin(LOG_DEBUG,__FILE__,__LINE__,"解密解压前数据",zipDataBufdst,ddlen);
// 	
// 	if (m_pDesSend.UncryptString(zipDataBufdst,ddlen,key)<1)
// 	{
// 		m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"解密出错");
// 		return false;
// 	}
// 	m_pLog.LogBin(LOG_DEBUG,__FILE__,__LINE__,"解密后解压前数据",zipDataBufdst,ddlen);
// 	dlen = CLIENTDATASIZE;
// 	if (!CBF_Tools::Uncompress((unsigned char *)(zipDataBuf),dlen,(unsigned char *)zipDataBufdst,ddlen))
// 	{
// 		m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"解压缩出错");
// 		return false;
// 	}
// 	
// 	m_pLog.LogBin(LOG_DEBUG,__FILE__,__LINE__,"解压缩后数据",zipDataBuf,dlen);
// 		


	if (!BFCGATE_InitApi("cgateapi.xml"))
	{
		printf("api初始化失败\n");
		return -1;
	}

	if (!BFCGATE_ConnectGate())
	{
		printf("连接网关失败\n");
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
	
	ret = CBF_Tools::GetConsoleEnter("请输入数据文件，回车结束",filelen-1,txfilename);
	txfilename[ret]=0;
	FILE *fp =fopen(txfilename,"r");
	if (fp == NULL)
	{
		printf("%s 不存在\n",txfilename);
		return -1;
	}
	bzero(&globaldata,sizeof(CGATE_COMMSTRU));
	globaldata.head.nLen = fread(globaldata.buffer,1,8192,fp);
	fclose(fp);
	ret = CBF_Tools::GetConsoleEnter("请输入交易码，回车结束",8,tmpchar);
	tmpchar[ret] =0;
	globaldata.head.stDest.nServiceNo = atoi(tmpchar);
	data.head.nRetCode = 0;

	ret = CBF_Tools::GetConsoleEnter("请输入接收超时时间(秒)，回车结束",2,tmpchar);
	tmpchar[ret] =0;
	timeout = atoi(tmpchar);

	int sendtime;


	ret = CBF_Tools::GetConsoleEnter("请输入发送间隔(毫秒)，回车结束",4,tmpchar);
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
// 		ret = CBF_Tools::GetConsoleEnter("请输入消息类型8:CMD_DPCALL 9:CMD_DPACALL 10:CMD_DPBC 11:CMD_DPABC 12:CMD_DPPUSH 13:CMD_DPPOST，回车结束",12,tmpchar);
// 		tmpchar[ret] =0;
// 		globaldata.head.stComm.cCmd = atoi(tmpchar);
// 		if (globaldata.head.stComm.cCmd < 8 || globaldata.head.stComm.cCmd > 13)
// 		{
// 			continue;	
// 		}
// 		break;
// 	}
// 	ret = CBF_Tools::GetConsoleEnter("上述输入正确否？1:正确 2:退出，回车结束",12,tmpchar);
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
				printf("连接网关失败\n");
				SLEEP(sendtime);
				continue;
			}
		}
		packnum = 0;
		globaldata.head.stDest.nSerial++;
		memcpy(&data,&globaldata,sizeof(CGATE_COMMSTRU));
		printf("发送%d交易\n",globaldata.head.stDest.nServiceNo);
		
		ret = BFCGATE_SendLogin(&data,timeout);
		sendnum++;
		if (ret > 0)
		{
			printf("收到后台应答 nRet[%d] nLen[%d]\n",data.head.nRetCode,data.head.nLen);
			if (data.head.stComm.cNextFlag == 0)
			{
				printf("无后续包\n");
				//向网关订阅	 key为9021 9022 9023的数据，9021可以为后台回报类的交易账户 
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
				//向网关订阅行情 10011 20012 10013合约的行情
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
			
			//后台主动推送，不停的收
			if (data.head.stComm.cNextFlag == 1 || data.head.stComm.cNextFlag == 10)
			{
				packnum++;
				while (true)
				{
					printf("收到应答的数据包数 %d",packnum);
					if (packnum >= data.head.stNext.n_nNextNo) //总包数
					{
						printf("应答总数据包数目 %d\n",packnum);
						break;
					}
					ret = BFCGATE_GetAnsData(&data,timeout);
					if (ret != 1)
					{
						printf("接收应答出错 %d",ret);
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
					printf("收到应答的数据包数 %d",packnum);
					ret = BFCGATE_GetAnsData(&data,timeout);
					if (ret != 1)
					{
						printf("接收应答出错 %d",ret);
						break;
					}
					packnum++;
				}
				printf("收到应答的数据包数 %d",packnum);
			}
			////订阅
			//CGATE_COMMSTRU data;
			//S_GATE_SUBSCRIBE   *subscribe=(S_GATE_SUBSCRIBE *)(data.buffer);
			//subscribe->flag = 2;
			//subscribe->datanum = 1;
			//subscribe->variety = 9021;
			//BFCGATE_Subscribe(&data, timeout);
		}
		else if (ret == 0)
		{
			printf("超时未收到应答\n");
		}
		else if (ret == -2)
		{
			printf("发送出错\n");
			if (!BFCGATE_ConnectGate())
			{
				printf("连接网关失败\n");
				return -1;
			}
		}
		else
		{
			printf("连接断开\n");
			if (!BFCGATE_ConnectGate())
			{
				printf("连接网关失败\n");
				return -1;
			}
		}
		SLEEP(sendtime);
		SLEEP_SECONDS(60);
	}
	return 0;
}

