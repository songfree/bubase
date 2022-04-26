// BF_DrebServerApi.cpp: implementation of the CBF_DrebServerApi class.
//
//////////////////////////////////////////////////////////////////////

#include "BF_DrebServerApi.h"
#include "BF_Slist.h"
#include "BF_SimpleLog.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBF_DrebServerApi::CBF_DrebServerApi()
{
	bzero(m_errMsg,sizeof(m_errMsg));
	m_pLog = NULL;
#ifdef _WINDOWS
	log.SetLogPara(LOG_DEBUG,"","drebapi.log");
#else
	log.SetLogPara(LOG_DEBUG,"/tmp","drebapi.log");
#endif
}

CBF_DrebServerApi::~CBF_DrebServerApi()
{

}

bool CBF_DrebServerApi::Init(const char *conxmlfile,CDrebMsgProcBase* spi)
{
	m_pRes.log = &log;
//	log.LogMp(LOG_DEBUG,__FILE__,__LINE__,"��ʼInit res");
	if (!m_pRes.Init(conxmlfile))
	{
		sprintf(m_errMsg,"����������ʧ�� %s %s",conxmlfile,m_pRes.m_errMsg.c_str());
		log.LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
		return false;
	}
	log.LogMp(LOG_DEBUG,__FILE__,__LINE__,"���ӵ�DREB��Ŀ[%d]",m_pRes.g_vDrebLinkInfo.size());
	//�������߳�ʼ��
//	log.LogMp(LOG_DEBUG,__FILE__,__LINE__,"api init ");
	if (!m_pDrebApi.Init(&m_pRes,spi))
	{
		sprintf(m_errMsg,"DREBAPI��ʼ��ʧ��");
		log.LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
		return false;
	}
//	log.LogMp(LOG_DEBUG,__FILE__,__LINE__,"dreb init end ");
	m_pLog = m_pDrebApi.GetLogPoint();
	return true;
}

bool CBF_DrebServerApi::StartApi()
{
	if (!m_pDrebApi.Start())
	{
		sprintf(m_errMsg,"����ʧ�� %s",m_pDrebApi.GetErrMsg().c_str());
		return false;
	}
	return true;
}

void CBF_DrebServerApi::StopApi()
{
	m_pDrebApi.Stop();
}

void CBF_DrebServerApi::MonitorApi()
{
	m_pDrebApi.MonitorThread();
}

int CBF_DrebServerApi::SendMsg(DREBSAPI_COMMSTRU smsg)
{
	m_pLog->LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"��ʼ����SendMsg");
	S_BPC_RSMSG sdata;
	sdata.sMsgBuf = (PBPCCOMMSTRU )m_pDrebApi.PoolMalloc();
	if (sdata.sMsgBuf == NULL)
	{
		sprintf(m_errMsg,"��Դ���㣬PoolMallocʧ��");
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
		return -1;
	}
	//������Ϊ��������ã��������ߺͱ����ֽ���ͬ����JAVA����
	if (m_pRes.g_nEndianFlag == 1)
	{
		m_pLog->LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"��ʼ�����ֽ���ת��");
		//��������תΪ������
		m_pDrebEndian.SetCommEndian(false);
		m_pDrebEndian.Endian2LocalHost(&(smsg.sDBHead));
		m_pDrebEndian.Endian2Comm((unsigned char *)&(smsg.index),sizeof(smsg.index));
		m_pLog->LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"�ֽ���ת�����");
	}
	sdata.index = smsg.index;
	bzero(&(sdata.sMsgBuf->sBpcHead),BPCHEADLEN);
	memcpy(&(sdata.sMsgBuf->sDBHead),&(smsg.sDBHead),DREBHEADLEN);
	if (smsg.sDBHead.nLen>0 && smsg.sDBHead.nLen < BPUDATASIZE )
	{
		memcpy(sdata.sMsgBuf->sBuffer,smsg.sBuffer,smsg.sDBHead.nLen);
	}
	else if (smsg.sDBHead.nLen<0  || smsg.sDBHead.nLen >= BPUDATASIZE)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"Ҫ���͵����ݳ��Ȳ��� %d",smsg.sDBHead.nLen);
		m_pDrebApi.PoolFree(sdata.sMsgBuf);
		sdata.sMsgBuf = NULL;
		return -1;
	}
	
	sdata.sMsgBuf->sDBHead.nLen = smsg.sDBHead.nLen;
	sdata.sMsgBuf->sBpcHead.nBpcLen = sdata.sMsgBuf->sDBHead.nLen +DREBHEADLEN;
	m_pLog->LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"�������� %d",sdata.sMsgBuf->sDBHead.nLen);
	return m_pDrebApi.SendMsg(sdata);
}

int CBF_DrebServerApi::GetMsg(DREBSAPI_COMMSTRU *gmsg)
{
	S_BPC_RSMSG rdata;
	
	if (m_pDrebApi.GetMsgData(rdata))
	{
		switch (rdata.sMsgBuf->sBpcHead.cMsgType)
		{
			case MSG_GETBPCNEXT://��bpc��ȡ������
				m_pDrebApi.PoolFree(rdata.sMsgBuf);
				rdata.sMsgBuf = NULL;
				m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"�յ�ȡ���������󣬶���");
				return 0;
			case MSG_GETNEXT://��BPU��ȡ��������Ҫ����BPU����
				m_pDrebApi.PoolFree(rdata.sMsgBuf);
				rdata.sMsgBuf = NULL;
				m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"�յ�ȡ���������󣬶���");
				return 0;
			case MSG_MONITOR_SAP://DREB���յ��ļ��ָ��
				if (OnMsgMonitor(rdata))
				{
					gmsg->cMsgType = rdata.sMsgBuf->sBpcHead.cMsgType;
					rdata.index = rdata.sMsgBuf->sBpcHead.nIndex;
					gmsg->index = rdata.sMsgBuf->sBpcHead.nIndex;
					m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"index [%d]",gmsg->index);
					memcpy(&(gmsg->sDBHead),&(rdata.sMsgBuf->sDBHead),DREBHEADLEN);
					if (rdata.sMsgBuf->sDBHead.nLen>0 && rdata.sMsgBuf->sDBHead.nLen < BPUDATASIZE )
					{
						memcpy(gmsg->sBuffer,rdata.sMsgBuf->sBuffer,rdata.sMsgBuf->sDBHead.nLen);
					}
					else if (rdata.sMsgBuf->sDBHead.nLen<0  || rdata.sMsgBuf->sDBHead.nLen >= BPUDATASIZE)
					{
						m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���յ������ݳ��Ȳ��� %d",rdata.sMsgBuf->sDBHead.nLen);
						m_pDrebApi.PoolFree(rdata.sMsgBuf);
						rdata.sMsgBuf = NULL;
						return -1;
					}
					gmsg->sDBHead.nLen = rdata.sMsgBuf->sDBHead.nLen;
					m_pDrebApi.PoolFree(rdata.sMsgBuf);
					rdata.sMsgBuf = NULL;
					//������Ϊ��������ã��������ߺͱ����ֽ���ͬ����JAVA����
					if (m_pRes.g_nEndianFlag == 1)
					{
						//��������תΪ������
						m_pDrebEndian.SetCommEndian(false);
						m_pDrebEndian.Endian2Comm(&(gmsg->sDBHead));
						m_pDrebEndian.Endian2Comm((unsigned char *)&(gmsg->index),sizeof(gmsg->index));
					}
					return 1;
				}
				return 0;
				break;
			case MSG_REQ://DREB���յ�������
			case MSG_CONNECTDREB://DREB�������ӳɹ����������������ע�ύ�׵�
			case MSG_DREBANS://DREB��������ȵ�Ӧ��
			case MSG_BPCMONITOR://�����ϱ������Ϣ
				if (!m_pDrebApi.UnzipBuf(*(rdata.sMsgBuf)))
				{
					m_pDrebApi.PoolFree(rdata.sMsgBuf);
					rdata.sMsgBuf = NULL;
					m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���ݽ�ѹ������");
					return -1;
				}
				gmsg->cMsgType = rdata.sMsgBuf->sBpcHead.cMsgType;
				rdata.index = rdata.sMsgBuf->sBpcHead.nIndex;
				gmsg->index = rdata.sMsgBuf->sBpcHead.nIndex;
				m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"index [%d]",gmsg->index);
				memcpy(&(gmsg->sDBHead),&(rdata.sMsgBuf->sDBHead),DREBHEADLEN);
				if (rdata.sMsgBuf->sDBHead.nLen>0 && rdata.sMsgBuf->sDBHead.nLen < BPUDATASIZE )
				{
					memcpy(gmsg->sBuffer,rdata.sMsgBuf->sBuffer,rdata.sMsgBuf->sDBHead.nLen);
				}
				else if (rdata.sMsgBuf->sDBHead.nLen<0  || rdata.sMsgBuf->sDBHead.nLen >= BPUDATASIZE)
				{
					m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���յ������ݳ��Ȳ��� %d",rdata.sMsgBuf->sDBHead.nLen);
					m_pDrebApi.PoolFree(rdata.sMsgBuf);
					rdata.sMsgBuf = NULL;
					return -1;
				}
				gmsg->sDBHead.nLen = rdata.sMsgBuf->sDBHead.nLen;
				m_pDrebApi.PoolFree(rdata.sMsgBuf);
				rdata.sMsgBuf = NULL;
				//������Ϊ��������ã��������ߺͱ����ֽ���ͬ����JAVA����
				if (m_pRes.g_nEndianFlag == 1)
				{
					//��������תΪ������
					m_pDrebEndian.SetCommEndian(false);
					m_pDrebEndian.Endian2Comm(&(gmsg->sDBHead));
					m_pDrebEndian.Endian2Comm((unsigned char *)&(gmsg->index),sizeof(gmsg->index));
				}
				return 1;
			default:
				m_pDrebApi.PoolFree(rdata.sMsgBuf);
				rdata.sMsgBuf = NULL;
				return 0;
		}
		return 0;
	}
	return 0;
}

void CBF_DrebServerApi::UnRegisterDreb()
{
	m_pDrebApi.UnRegisterDreb();
}

void CBF_DrebServerApi::RegisterDreb(int index, std::vector<int> &funclist)
{
	m_pDrebApi.RegisterDreb(index,&funclist);
}


int BFDREBAPI_InitApi(const char *apixml,void **pClass)
{
	CBF_DrebServerApi *tmp = new CBF_DrebServerApi();
	if (tmp == NULL)
	{
		*pClass= NULL;
		return -1;
	}
	if (!tmp->Init(apixml))
	{
		*pClass= NULL;
		delete tmp;
		tmp = NULL;
		return -1;
	}
	*pClass = tmp;
	return 0;
}
void BFDREBAPI_Free(void *pClass)
{
	CBF_DrebServerApi *tmp = (CBF_DrebServerApi *)pClass;
	delete tmp;
	tmp = NULL;
}
int BFDREBAPI_StartApi(void *pClass)
{
	CBF_DrebServerApi *tmp = (CBF_DrebServerApi *)pClass;
	if (!tmp->StartApi())
	{
		return -1;
	}
	return 0;
}
void BFDREBAPI_StopApi(void *pClass)
{
	CBF_DrebServerApi *tmp = (CBF_DrebServerApi *)pClass;
	tmp->StopApi();
}
void BFDREBAPI_MonitorApi(void *pClass)
{
	CBF_DrebServerApi *tmp = (CBF_DrebServerApi *)pClass;
	tmp->MonitorApi();
}
int BFDREBAPI_SendMsg(void *pClass,const char *data)
{
	CBF_DrebServerApi *tmp = (CBF_DrebServerApi *)pClass;
	DREBSAPI_COMMSTRU *sdata = (DREBSAPI_COMMSTRU *)data;
	return tmp->SendMsg(*sdata);
}
int BFDREBAPI_GetMsg(void *pClass,char *data)
{
	CBF_DrebServerApi *tmp = (CBF_DrebServerApi *)pClass;
	DREBSAPI_COMMSTRU *sdata = (DREBSAPI_COMMSTRU *)data;
	return tmp->GetMsg(sdata);
}
int BFDREBAPI_GetErrMsg(void *pClass,char *errmsg)
{
	CBF_DrebServerApi *tmp = (CBF_DrebServerApi *)pClass;
	strcpy(errmsg, tmp->m_errMsg);
	return 0;
}
void BFDREBAPI_UnRegisterDreb(void *pClass)
{
	CBF_DrebServerApi *tmp = (CBF_DrebServerApi *)pClass;
	tmp->UnRegisterDreb();
}

void BFDREBAPI_RegisterDreb(void *pClass,int index,char *txlist)
{
	std::vector<int>funclist;
	CBF_Slist slist;
	slist.SetSeparateString("|");
	slist.FillSepString(txlist);
	int nRec = slist.GetCount();
	for (int i=0; i< nRec ; i++)
	{
		funclist.push_back(atoi(slist.GetAt(i,false).c_str()));
	}
	CBF_DrebServerApi *tmp = (CBF_DrebServerApi *)pClass;
	tmp->RegisterDreb(index,funclist);
}

bool  CBF_DrebServerApi::OnMsgMonitor(S_BPC_RSMSG &rcvdata)
{
	//DREB�����ĵ���
	switch (rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo)
	{
		case BPCMONI_INFO:
			return MonitorInfo(rcvdata);
		case BPCMONI_HOST:
			return MonitorHost(rcvdata);
		case BPCMONI_BPU:
			return MonitorBpu(rcvdata);
		case BPCMONI_REPORT:
			return MonitorReport(rcvdata);
		case BPCMONI_DOWNLOAD:
			Download(rcvdata);
			return false;//����Ҫ����
		case BPCMONI_UPLOAD:
			Upload(rcvdata);
			return false;//����Ҫ����
		default:
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_FUNCNOTFUND;
			rcvdata.sMsgBuf->sDBHead.nLen = 0;
			rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN;
			m_pDrebApi.SendMsg(rcvdata);
			return false;
	}
	return false;
}
bool CBF_DrebServerApi::MonitorInfo(S_BPC_RSMSG &rcvdata)
{
	int i;
	CBF_Xml xmlpack;
	xmlpack.SetNodeValueByPath("Monitor/BPC�����ڵ�",false,(int)m_pRes.g_nSvrMainId);
	xmlpack.SetNodeValueByPath("Monitor/BPC˽�нڵ�",false,(int)m_pRes.g_nSvrPrivateId);
	xmlpack.SetNodeValueByPath("Monitor/����ʱ��",false,m_pRes.g_sStartDateTime,false);
	char sip[40];
	char drebstatus[40];
	int  drebport;
	int  drebid;
	int  drebprivateid;
	
	//������Ϣ
	for (i=0 ; i < m_pRes.g_vDrebLinkInfo.size() ; i++)
	{
		CXmlNode *connnode = xmlpack.AddNodeByPath("Monitor/DREB������Ϣ",false,"����",false,"");
		if (connnode == NULL)
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"AddNodeByPath ����");
			continue;
		}
		bzero(sip,sizeof(sip));
		bzero(drebstatus,sizeof(drebstatus));
		m_pDrebApi.GetDrebInfo(i,sip,drebport,drebid,drebprivateid,drebstatus);
		connnode->SetAttribute("ip",false,sip);
		connnode->SetAttribute("port",false,drebport);
		connnode->SetAttribute("index",false,i);
		connnode->SetAttribute("�����ڵ�",false,drebid);
		connnode->SetAttribute("˽�нڵ�",false,drebprivateid);
		connnode->SetAttribute("status",false,drebstatus);
		
	}
		
	int len = DREBDATASIZE;
	if (!xmlpack.ToBuffer(rcvdata.sMsgBuf->sBuffer,len))
	{
		sprintf(rcvdata.sMsgBuf->sBuffer,"���岻�㣬������bpcinfo.txt�鿴");
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"MonitorInfo ���岻��");
		xmlpack.ToFile("bpcinfo.txt");
	}
	else
	{
		rcvdata.sMsgBuf->sDBHead.nLen = len;
	}
	
	rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
	rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
	rcvdata.sMsgBuf->sDBHead.cZip = 0;
	return true;
}
	
bool CBF_DrebServerApi::MonitorHost(S_BPC_RSMSG &rcvdata)
{
	CBF_Xml xmlpack;
	xmlpack.SetNodeValueByPath("Monitor/BPC�����ڵ�",false,(int)m_pRes.g_nSvrMainId);
	xmlpack.SetNodeValueByPath("Monitor/BPC˽�нڵ�",false,(int)m_pRes.g_nSvrPrivateId);
	xmlpack.SetNodeValueByPath("Monitor/����ʱ��",false,m_pRes.g_sStartDateTime,false);
	
	int total;
	int unused;
	int bufsize;
	m_pDrebApi.GetBufferPoolInfo(total,unused,bufsize);
	xmlpack.SetNodeAttribute("Monitor/�ڴ滺���",false,"�ܷ���",false,total );
	xmlpack.SetNodeAttribute("Monitor/�ڴ滺���",false,"δʹ��",false,unused );
	xmlpack.SetNodeAttribute("Monitor/�ڴ滺���",false,"ÿ���С",false,bufsize );
	//ȡ������Ϣ
	S_MONITOR_HOST hf;
	vector<S_MONITOR_DISK> di;
	m_pDrebApi.GetHostInfo(hf,di);
	
	xmlpack.SetNodeValueByPath("Monitor/������Դ/CPU",false,(int)hf.nCpuRate);
	
	
	xmlpack.SetNodeValueByPath("Monitor/������Դ/�ڴ�",false,"",false);
	xmlpack.SetNodeAttribute("Monitor/������Դ/�ڴ�",false,"�ܴ�СM",false,(int)hf.nTotalMemory);
	xmlpack.SetNodeAttribute("Monitor/������Դ/�ڴ�",false,"ʹ��M",false,(int)hf.nUsedMemory);
	
	for (int i=0 ; i<di.size() ; i++)
	{
		CXmlNode *disknode = xmlpack.AddNodeByPath("Monitor/������Դ/�����б�",false,"����",false,"");
		if (disknode != NULL)
		{
			disknode->SetAttribute("����",false,di[i].sDiskName);
			disknode->SetAttribute("�ܴ�СM",false,(int)di[i].nTotalSpace);
			disknode->SetAttribute("ʹ�ô�СM",false,(int)di[i].nUsedSpace);
		}
	}
	
	int len = DREBDATASIZE;
	if (!xmlpack.ToBuffer(rcvdata.sMsgBuf->sBuffer,len))
	{
		sprintf(rcvdata.sMsgBuf->sBuffer,"���岻�㣬������bpchost.txt�鿴");
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"MonitorHost ���岻��");
		xmlpack.ToFile("bpchost.txt");
	}
	else
	{
		rcvdata.sMsgBuf->sDBHead.nLen = len;
	}
	
	rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
	rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
	rcvdata.sMsgBuf->sDBHead.cZip = 0;
	return true;
}

bool CBF_DrebServerApi::MonitorBpu(S_BPC_RSMSG &rcvdata)
{
	
	int connNum=0;
	CBF_Xml xmlpack;
	xmlpack.SetNodeValueByPath("Monitor/BPC�����ڵ�",false,(int)m_pRes.g_nSvrMainId);
	xmlpack.SetNodeValueByPath("Monitor/BPC˽�нڵ�",false,(int)m_pRes.g_nSvrPrivateId);
	xmlpack.SetNodeValueByPath("Monitor/����ʱ��",false,m_pRes.g_sStartDateTime,false);
	//BPU��Ϣ
	xmlpack.AddNodeByPath("Monitor",false,"BPU�б�",false,"");

	int len = DREBDATASIZE;
	if (!xmlpack.ToBuffer(rcvdata.sMsgBuf->sBuffer,len))
	{
		sprintf(rcvdata.sMsgBuf->sBuffer,"���岻�㣬������bpcbpu.txt�鿴");
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"MonitorBpu ���岻��");
		xmlpack.ToFile("bpcbpu.txt");
	}
	else
	{
		rcvdata.sMsgBuf->sDBHead.nLen = len;
	}
	
	rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
	rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
	rcvdata.sMsgBuf->sDBHead.cZip = 0;
	return true;
	
}

bool CBF_DrebServerApi::MonitorReport(S_BPC_RSMSG &rcvdata)
{
	CBF_Xml xmlpack;
	xmlpack.SetNodeValueByPath("Monitor/BPC�����ڵ�",false,(int)m_pRes.g_nSvrMainId);
	xmlpack.SetNodeValueByPath("Monitor/BPC˽�нڵ�",false,(int)m_pRes.g_nSvrPrivateId);
	xmlpack.SetNodeValueByPath("Monitor/����ʱ��",false,m_pRes.g_sStartDateTime,false);
	char tmpchar[100];
	char nodename[200];
	int len = DREBDATASIZE;
	if (!xmlpack.ToBuffer(rcvdata.sMsgBuf->sBuffer,len))
	{
		sprintf(rcvdata.sMsgBuf->sBuffer,"���岻�㣬������bpureport.txt�鿴");
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"MonitorReport ���岻��");
		xmlpack.ToFile("bpureport.txt");
	}
	else
	{
		rcvdata.sMsgBuf->sDBHead.nLen = len;
	}
	rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
	rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
	rcvdata.sMsgBuf->sDBHead.cZip = 0;
	return true;
}


	
void CBF_DrebServerApi::Download(S_BPC_RSMSG &rcvdata)
{
	if (rcvdata.sMsgBuf->sDBHead.cZip != 0)
	{
		if (!m_pDrebApi.UnzipBuf(*rcvdata.sMsgBuf))
		{
			sprintf(rcvdata.sMsgBuf->sBuffer,"���ݽ�ѹ������");
			rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			m_pDrebApi.SendMsg(rcvdata);
			return ;
		}
	}
	S_BPC_DUFILE sfile;
	if (rcvdata.sMsgBuf->sDBHead.nLen  < sizeof(S_BPC_DUFILE))
	{
		m_pLog->LogBin(LOG_ERROR,__FILE__,__LINE__,"�������ݷ���������",\
			rcvdata.sMsgBuf->sBuffer,rcvdata.sMsgBuf->sDBHead.nLen);
		sprintf(rcvdata.sMsgBuf->sBuffer,"�������ݷ���������");
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_MONITOR_DOWNLOAD;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		m_pDrebApi.SendMsg(rcvdata);
		return ;
	}
	memcpy(&sfile,rcvdata.sMsgBuf->sBuffer,sizeof(S_BPC_DUFILE));
	struct_stat fstat;
	FSTAT(sfile.sFileName,&fstat);
	if (fstat.st_size <= 0)
	{
		sprintf(rcvdata.sMsgBuf->sBuffer,"�ļ�[%s]������",sfile.sFileName);
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_MONITOR_DOWNLOAD;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		m_pDrebApi.SendMsg(rcvdata);
		return ;
	}
	int offset = atol(sfile.sFileOffset);
	if (offset >= fstat.st_size)
	{
		sprintf(rcvdata.sMsgBuf->sBuffer,"�ļ�ƫ����[%d]�Ѵ��ڵ����ļ���С",offset);
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_MONITOR_DOWNLOAD;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		m_pDrebApi.SendMsg(rcvdata);
		return ;
	}
	sprintf(sfile.sFileSize,"%011ld",fstat.st_size);
	FILE *fp = fopen(sfile.sFileName,"rb");
	if (fp == NULL)
	{
		sprintf(rcvdata.sMsgBuf->sBuffer,"�ļ�[%s]������",sfile.sFileName);
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_MONITOR_DOWNLOAD;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		m_pDrebApi.SendMsg(rcvdata);
		return ;
	}
	fseek(fp,offset,1);
	int getlen = fread(rcvdata.sMsgBuf->sBuffer+sizeof(S_BPC_DUFILE),1,7*1024,fp);
	fclose(fp);
	sprintf(sfile.sFileOffset,"%011ld",getlen + offset);
	if ((getlen + offset) >= fstat.st_size)
	{
		sfile.cEndFlag = '1';
	}
	else
	{
		sfile.cEndFlag = '0';
	}
	memcpy(rcvdata.sMsgBuf->sBuffer,&sfile,sizeof(S_BPC_DUFILE));
	rcvdata.sMsgBuf->sDBHead.nLen = sizeof(S_BPC_DUFILE)+getlen;
	rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
	rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
	rcvdata.sMsgBuf->sDBHead.cZip = 0;
	m_pDrebApi.SendMsg(rcvdata);
	return;

}

void CBF_DrebServerApi::Upload(S_BPC_RSMSG &rcvdata)
{
	if (rcvdata.sMsgBuf->sDBHead.cZip != 0)
	{
		if (!m_pDrebApi.UnzipBuf(*rcvdata.sMsgBuf))
		{
			sprintf(rcvdata.sMsgBuf->sBuffer,"���ݽ�ѹ������");
			rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			m_pDrebApi.SendMsg(rcvdata);
			return ;
		}
	}
	S_BPC_DUFILE sfile;
	if (rcvdata.sMsgBuf->sDBHead.nLen  <= sizeof(S_BPC_DUFILE))
	{
		m_pLog->LogBin(LOG_ERROR,__FILE__,__LINE__,"�������ݷ��ϴ�����",\
			rcvdata.sMsgBuf->sBuffer,rcvdata.sMsgBuf->sDBHead.nLen);
		sprintf(rcvdata.sMsgBuf->sBuffer,"�������ݷ��ϴ�����");
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_MONITOR_UPLOAD;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		m_pDrebApi.SendMsg(rcvdata);
		return ;
	}
	memcpy(&sfile,rcvdata.sMsgBuf->sBuffer,sizeof(S_BPC_DUFILE));
	m_pLog->LogBin(LOG_DEBUG,__FILE__,__LINE__,"�ϴ��ļ�",rcvdata.sMsgBuf->sBuffer,rcvdata.sMsgBuf->sDBHead.nLen );
	FILE *fp=NULL;
	if (sfile.cBeginFlag == '1')
	{
		char filenamepath[301];
		sprintf(filenamepath,"%s",sfile.sFileName);
		int j= strlen(filenamepath)-1;
		for (;j>0;j--)
		{
			if ((filenamepath[j] == '/') ||  (filenamepath[j] == '\\'))
			{
				filenamepath[j] =0;
				CBF_Tools::MakeDir(filenamepath);
				break;
			}
		}
		fp = fopen(sfile.sFileName,"wb");
	}
	else
	{
		fp = fopen(sfile.sFileName,"ab");
	}
	if (fp == NULL)
	{
		sprintf(rcvdata.sMsgBuf->sBuffer,"�ļ�[%s]�򿪳���",sfile.sFileName);
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_MONITOR_UPLOAD;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		m_pDrebApi.SendMsg(rcvdata);
		return ;
	}
	fwrite(rcvdata.sMsgBuf->sBuffer+sizeof(S_BPC_DUFILE),1,rcvdata.sMsgBuf->sDBHead.nLen-sizeof(S_BPC_DUFILE),fp);
	fclose(fp);
	if (sfile.cEndFlag == '1')
	{
		struct_stat fstat;
		FSTAT(sfile.sFileName,&fstat);
		if (atol(sfile.sFileSize) != fstat.st_size)
		{
			sprintf(rcvdata.sMsgBuf->sBuffer,"�ϴ��ļ�[%s]ʧ�ܣ���С[%d]������[%s]",sfile.sFileName,fstat.st_size,sfile.sFileSize);
			rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_MONITOR_UPLOAD;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			m_pDrebApi.SendMsg(rcvdata);
			return ;
		}
		sprintf(rcvdata.sMsgBuf->sBuffer,"�ϴ��ļ�[%s]�ɹ�",sfile.sFileName);
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		m_pDrebApi.SendMsg(rcvdata);
		return ;
	}
	else
	{
		sprintf(rcvdata.sMsgBuf->sBuffer,"д���ļ�[%s]�ɹ�",sfile.sFileName);
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		m_pDrebApi.SendMsg(rcvdata);
		return ;
	}
}