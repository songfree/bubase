// ClientApi.cpp: implementation of the CClientApi class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "public.h"
#include "ClientApi.h"
#include "BF_Des.h"

#include "BF_Tools.h"


#ifdef TUXEDO
#include "atmi.h"
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define  BIZINFOSEG  ":"
#define  ENCRYPTKEY "sz123456"
#define  DESENCRYPTKEY "rd402wms"
#define  ENCODEKEY "sz123456SZ123456"



#define  MAXREQBUF 96*1024


CClientApi *getInstanceApi()
{
	return new CClientApi();
}

CClientApi::CClientApi()
{
	m_bIsInit = false;
	memset(&g_vars,0,sizeof(G_VARS_API));
	memset(m_errMsg,0,sizeof(m_errMsg));
	m_bIsTpInit = false;
	m_isServiceName = false;
#ifdef TUXEDO
	g_tpinitbuf = NULL;
#endif
	g_vars.g_bIsFileTrans = true;
	m_bIsShortConnect = true;
	m_TuxedoBufLen = MAXREQBUF;//���峤��
	m_tuxedoAlias = false;
	m_txCode = 0;
	m_bIsTcpInit = false;

}

CClientApi::~CClientApi()
{
	if (g_vars.g_bIsTuxedo)
	{
#ifdef TUXEDO
		if (g_tpinitbuf != NULL)
		{
			tpfree((char *)g_tpinitbuf);
			g_tpinitbuf = NULL;
		}
		tpterm();
#endif	
	}
	
}

bool CClientApi::Init(char *file)
{
	int ret;
	if (m_xmlconf.fromFile(file)<0)
	{
		m_bIsInit = false;
		sprintf(m_errMsg,"�ļ�[%s]�����ڻ��xml",file);
		return false;
	}
	if (m_xmlconf.getHeadValueChar("��־�ļ�",g_vars.g_logFileName)<0)
	{
		sprintf(m_errMsg,"�����ļ�[��־�ļ�]δ����");
		m_bIsInit = false;
		return false;
	}

	if (m_xmlconf.getHeadValue("��־����",g_vars.g_trace)<0)
	{
		sprintf(m_errMsg,"�����ļ�[��־����]δ����");
		m_bIsInit = false;
		return false;
	}
	m_log.SetLogPara(g_vars.g_trace,"",g_vars.g_logFileName);
	if (m_xmlconf.getHeadValue("CLIENTAPI",ret)<0)
	{
		sprintf(m_errMsg,"�����ļ�[CLIENTAPI]δ����");
		m_bIsInit = false;
		return false;
	}
	if (0 == ret)
	{
		g_vars.g_bIsTuxedo = true;
	}
	else
	{
		g_vars.g_bIsTuxedo = false;
	}

	if (m_xmlconf.getHeadValue("�Ƿ��ļ�����",ret)<0)
	{
		g_vars.g_bIsFileTrans = false;
	}
	else if(ret == 0)
	{
		g_vars.g_bIsFileTrans = false;
	}
	else
	{
		g_vars.g_bIsFileTrans = true;
	}

	//�Ƿ����
	if (m_xmlconf.getHeadValue("�Ƿ����ݰ�����",ret)<0)
	{
		sprintf(m_errMsg,"�����ļ�[�Ƿ����ݰ�����]δ����");
		m_bIsInit = false;
		return false;
	}
	if (1 == ret)
	{
		g_vars.g_bIsZipEncrypt = 1;
		
	}
	else if (2 == ret)
	{
		g_vars.g_bIsZipEncrypt = 2;
	}
	else
	{
		g_vars.g_bIsZipEncrypt = ret;
	}
	//�����Ƿ��������
	if (m_xmlconf.getHeadValue("�����Ƿ��������",ret)<0)
	{
		sprintf(m_errMsg,"�����ļ�[�����Ƿ��������]δ����");
		m_bIsInit = false;
		return false;
	}
	if (0 == ret)
	{
		g_vars.g_bIsPwdEncrypt = false;
	}
	else
	{
		g_vars.g_bIsPwdEncrypt = true;
	}
	if (m_xmlconf.getHeadValue("����TUXEDO�������",ret) == 0)
	{
		if (ret == 1)
		{
			m_tuxedoAlias = true;
		}
		else
		{
			m_tuxedoAlias = false;
		}
	}
	//��������
	if (m_xmlconf.getHeadValue("��������",ret)<0)
	{
		sprintf(m_errMsg,"�����ļ�[��������]δ����");
		m_bIsShortConnect = true;
	}
	else
	{
		if (0 == ret) //0������1������
		{
			m_bIsShortConnect = false;
			
		}
		else if (1 == ret)
		{
			m_bIsShortConnect = true;
		}
		else
		{
			m_bIsShortConnect = false;
		}
	}
	if (m_xmlconf.getHeadValueChar("SERVERIP",g_vars.g_sIp)<0)
	{
		sprintf(m_errMsg,"�����ļ�[SERVERIP]δ����");
		m_bIsInit = false;
		return false;
	}
	if (m_xmlconf.getHeadValue("LISTENPORT",g_vars.g_nPort )<0)
	{
		sprintf(m_errMsg,"�����ļ�[LISTENPORT]δ����");
		m_bIsInit = false;
		return false;
	}

	if (g_vars.g_bIsTuxedo)
	{
#ifdef TUXEDO
		if (m_xmlconf.getHeadValue("��ʱʱ��",g_vars.g_tpCallTime)<0)
		{
			sprintf(m_errMsg,"�����ļ�[��ʱʱ��]δ����");
			m_bIsInit = false;
			return false;
		}
// 		if (!TuxedoInit(m_bIsTpInit)) 
// 		{
// 			sprintf(m_errMsg,"TUXEDO tpinit ʧ��");
// 			m_log.Log(0,__FILE__,__LINE__,m_errMsg,tperrno,"Tpinit failed");
// 			return false;
// 		}
#else
		sprintf(m_errMsg,"��Ϊ��TUXEDO�汾!");
		m_log.LogMp(0,__FILE__,__LINE__,"�����ļ����ô� %s",m_errMsg);
		m_bIsInit = false;
		return false;	
#endif	
	}
	else
	{
		
		if (m_xmlconf.getHeadValue("socket���ճ�ʱʱ��",g_vars.g_timeOut)<0)
		{
			sprintf(m_errMsg,"�����ļ�[socket���ճ�ʱʱ��]δ����");
			m_bIsInit = false;
			return false;
		}
		if (m_xmlconf.getHeadValue("ͨѶ��ͷ����",g_vars.g_rcvHead)<0)
		{
			sprintf(m_errMsg,"�����ļ�[ͨѶ��ͷ����]δ����");
			m_bIsInit = false;
			return false;
		}
		if (m_xmlconf.getHeadValue("ͨѶ��ͷ�Ƿ��������",ret)<0)
		{
			sprintf(m_errMsg,"�����ļ�[ͨѶ��ͷ�Ƿ��������]δ����");
			m_bIsInit = false;
			return false;
		}
		if (ret==0)
		{
			g_vars.g_bIsInclHead = false;
		}
		else
		{
			g_vars.g_bIsInclHead = true;
		}
		
	}
	m_bIsInit = true;
	m_isServiceName = false;
	sprintf(m_wsnaddr,"WSNADDR=//%s:%d",g_vars.g_sIp,g_vars.g_nPort);
	return true;
}

int CClientApi::Send2Server(char *sendbuf,int buflen,char *rcvbuf,int &rcvlen)
{
	int ret;
	char *tsendbuf=NULL;
	long sendlen;
	sendlen = buflen;
	char tmpchar[100];

	if (g_vars.g_bIsTuxedo)
	{
#ifdef TUXEDO
		if (buflen >= m_TuxedoBufLen)
		{
			sprintf(m_errMsg,"���ݳ��Ȳ�С�ڻ��峤��[%d-%d]",buflen,m_TuxedoBufLen);
			m_log.LogMp(0,__FILE__,__LINE__,"���ݳ��Ȳ�С�ڻ��峤�� %s",m_errMsg);
			return -1;
		}
		if((tsendbuf = (char *) tpalloc("CARRAY", NULL, m_TuxedoBufLen)) == NULL) 
		{
			sprintf(m_errMsg,"Error allocating send buffer");
			m_log.LogMp(0,__FILE__,__LINE__,"����tuxedo�ռ� %s","Error allocating send buffer");
			return -1;
		}
		if (!m_bIsShortConnect) //tuxedo�Ƿ������
		{
			if (!TuxedoInit(m_bIsTpInit))//�����Ӹ������ӳɹ������Ƿ�����
			{
				sprintf(m_errMsg,"TUXEDO tpinit ʧ��");
				m_log.LogMp(0,__FILE__,__LINE__,"Tpinit failed %s %d",m_errMsg,tperrno);
				tpfree(tsendbuf);
				return -1;
			}
		}
		else
		{
			//������ǿ������
			if (!TuxedoInit(false))
			{
				sprintf(m_errMsg,"TUXEDO tpinit ʧ��");
				m_log.LogMp(0,__FILE__,__LINE__,"%s %d %s %s",m_errMsg,tperrno,"Tpinit failed",tpstrerror(tperrno));
				tpfree(tsendbuf);
				return -1;
			}
		}
		memcpy(tsendbuf, sendbuf,buflen);
		m_log.LogBin(5,__FILE__,__LINE__,"��סTUXEDO��̨����",sendbuf,sendlen);
		if (m_tuxedoAlias)
		{
			if (m_txCode>0)
			{
				sprintf(tmpchar,"%s%d",m_service,m_txCode);
			}
			else
			{
				strcpy(tmpchar,m_service);
			}
			
		}
		else
		{
			strcpy(tmpchar,m_service);
		}
		tuxputenv(m_wsnaddr);
		m_log.LogMp(LOG_DEBUG,__FILE__,__LINE__,"���÷��� %s",tmpchar);
		ret = tpcall(tmpchar, (char *)tsendbuf, buflen, (char **)&tsendbuf, &sendlen, (long)0);
		if(ret == -1) 
		{
			ret = tperrno;
			if (ret == TPESYSTEM) //ϵͳ���ٴε���һ��
			{
				m_log.LogMp(LOG_DEBUG,__FILE__,__LINE__,"���÷��� %s",tmpchar);
				ret = tpcall(tmpchar, (char *)tsendbuf, buflen, (char **)&tsendbuf, &sendlen, (long)0);
			}
			else if (ret == TPENOENT )
			{
				if (m_tuxedoAlias)
				{
					m_log.LogMp(LOG_DEBUG,__FILE__,__LINE__,"���÷��� %s",m_service);
					ret = tpcall(m_service, (char *)tsendbuf, buflen, (char **)&tsendbuf, &sendlen, (long)0);
				}
				else
				{
					ret = -1;
				}
			}
			else
			{
				sprintf(m_errMsg,"Can't send request to service %s %d %s",m_service,ret,tpstrerror(tperrno));
				m_log.LogMp(0,__FILE__,__LINE__,"����������� %d %s %s",ret,"Can't send request to service ",m_service);
				tpfree(tsendbuf);
				//��tpcallʧ�ܣ������ǳ����Ƕ����ӣ�Ҫ��������ֹ���ӶϿ���һֱ�����ϡ�
				tpfree((char *)g_tpinitbuf);
				g_tpinitbuf = NULL;
				tpterm();
				m_bIsTpInit = false;
				return -1;
			}
			if (ret == -1)
			{
				sprintf(m_errMsg,"Can't send request to service %s %d %s",m_service,ret,tpstrerror(tperrno));
				m_log.Log(0,__FILE__,__LINE__,"�����������",ret,"Can't send request to service ",m_service);
				tpfree(tsendbuf);
				tpfree((char *)g_tpinitbuf);
				g_tpinitbuf = NULL;
				//��tpcallʧ�ܣ������ǳ����Ƕ����ӣ�Ҫ��������ֹ���ӶϿ���һֱ�����ϡ�
				tpterm();
				m_bIsTpInit = false;
				return -1;
			}
			
		}
		
		m_log.LogBin(5,__FILE__,__LINE__,"TUXEDO��̨����",tsendbuf,sendlen);
		memcpy(rcvbuf,tsendbuf,sendlen);
		rcvbuf[sendlen]=0;
		rcvlen = sendlen;
		tpfree(tsendbuf);
		if (m_bIsShortConnect) //tuxedo�Ƿ������
		{
			tpfree((char *)g_tpinitbuf);
			g_tpinitbuf = NULL;
			tpterm();
			m_bIsTpInit = false;
		}
#else
		sprintf(m_errMsg,"��Ϊ��tuxedo�汾!");
		m_log.LogMp(0,__FILE__,__LINE__,"����������� %d %s",ret,m_errMsg);
		return -1;
#endif	
	}
	else
	{
		if (m_bIsShortConnect)
		{
			m_bIsTcpInit = false;
			if (!TcpInit())
			{
				return -101;
			}
		}
		else
		{
			if (!m_bIsTcpInit)
			{
				if (!TcpInit())
				{
					return -101;
				}
			}
		}
		
		int lasttime = m_publog.GetTickCount();
		ret = TcpSend(sendbuf, sendlen);
		if (ret<0)
		{
			m_log.LogMp(0,__FILE__,__LINE__,"��������ʧ�� %s",m_errMsg);
			m_tcpSocket.Close();
			m_bIsTcpInit = false;
			return -102;
		}
		ret = TcpReceive(rcvbuf, rcvlen);
		if (ret<0)
		{
			m_log.LogMp(0,__FILE__,__LINE__,"����Ӧ��ʧ�� %d %s",m_publog.GetTickCount()-lasttime,m_errMsg);
			if (m_bIsShortConnect)
			{
				m_tcpSocket.Close();
				m_bIsTcpInit = false;
			}
			return -103;
		}
		if (m_bIsShortConnect)
		{
			m_tcpSocket.Close();
			m_bIsTcpInit = false;
		}
	}
	return 0;
}

int CClientApi::CallService(CXmlPack &xmlpack, CXmlPackArray &muxmlpack)
{
	int ret;
	char sendbuf[MAXREQBUF];
	char rcvbuf[MAXREQBUF];
	char tmpchar[400];
	int buflen;
	int rcvlen;
	CXmlPack tmpxmlpack;

	char tmpfilename[400];
	FILE *fp = NULL;
	int mupackans=0;
	memset(sendbuf,0,sizeof(sendbuf));
	memset(rcvbuf,0,sizeof(rcvbuf));
	memset(tmpchar,0,sizeof(tmpchar));
	memset(tmpfilename,0,sizeof(tmpfilename));
	if (xmlpack.getHeadValue("������",ret)<0)
	{
		sprintf(m_errMsg,"xmlpack������δ����");
		m_log.LogMp(1,__FILE__,__LINE__,m_errMsg);
		return -1;
	}
	m_txCode = ret;
	if (!g_vars.g_bIsPwdEncrypt) //δ��������
	{
		

	}
	buflen = sizeof(sendbuf);
	if (!xmlpack.tobuffer(sendbuf,buflen))
	{
		sprintf(m_errMsg,"xml���ݰ�����");
		m_log.LogMp(1,__FILE__,__LINE__,m_errMsg);
		return -1;
	}
	m_log.LogBin(5,__FILE__,__LINE__,"���͵�������������",sendbuf,buflen);


	if (2 == g_vars.g_bIsZipEncrypt )
	{
		memcpy(rcvbuf,sendbuf,buflen);
		unsigned int dlen = sizeof(sendbuf)-1;
		CBF_Tools::Compress((unsigned char *)sendbuf,dlen,(unsigned char *)rcvbuf,buflen);
		buflen = dlen;
	}
	else if (1 == g_vars.g_bIsZipEncrypt )
	{
		//�����ݰ����м���
//		encode.encryptchar(sendbuf,buflen,ENCODEKEY);
	}
	else if (3 == g_vars.g_bIsZipEncrypt )
	{
//		encrypt.encrypt64(sendbuf,buflen,DESENCRYPTKEY);
	}
	else if (4 == g_vars.g_bIsZipEncrypt )
	{
// 		encrypt.encrypt64(sendbuf,buflen,DESENCRYPTKEY);
// 		memcpy(rcvbuf,sendbuf,buflen);
// 		unsigned int dlen = sizeof(sendbuf)-1;
// 		Compress((unsigned char *)sendbuf,&dlen,(unsigned char *)rcvbuf,buflen);
// 		buflen = dlen;
	}

	if (g_vars.g_bIsTuxedo)
	{
		if (!GetService(ret))
		{
			sprintf(m_errMsg,"�����ļ�TUXEDO���������ò���");
			m_log.LogMp(1,__FILE__,__LINE__,m_errMsg);
			return -1;
		}
	}
	ret = Send2Server(sendbuf,buflen,rcvbuf,buflen);
	if (ret<0)
	{
		xmlpack.setReturn(-1,m_errMsg);
		m_log.LogMp(1,__FILE__,__LINE__,"%d %s",ret,m_errMsg);
		return ret;
	}
	if (1 == g_vars.g_bIsZipEncrypt)
	{
		//��Ӧ������н���
//		encode.unencryptchar(rcvbuf,buflen,ENCODEKEY);
	}
	else if (2 == g_vars.g_bIsZipEncrypt)
	{
		unsigned int dlen = sizeof(sendbuf)-1;
		CBF_Tools::Uncompress((unsigned char *)sendbuf,dlen,(unsigned char *)rcvbuf,buflen);
		memcpy(rcvbuf,sendbuf,dlen);
		buflen = dlen;
		rcvbuf[buflen]=0;
	}
	else if (3 == g_vars.g_bIsZipEncrypt )
	{
//		encrypt.unencrypt64(rcvbuf,buflen,DESENCRYPTKEY);
	}
	else if (4 == g_vars.g_bIsZipEncrypt )
	{
// 		unsigned int dlen = sizeof(sendbuf)-1;
// 		Uncompress((unsigned char *)sendbuf,&dlen,(unsigned char *)rcvbuf,buflen);
// 		memcpy(rcvbuf,sendbuf,dlen);
// 		buflen = dlen;
// 		rcvbuf[buflen]=0;
// 		encrypt.unencrypt64(rcvbuf,buflen,DESENCRYPTKEY);
	}
/* *** Updated by songfree at 2009-5-13 15:02:27 ****** 
	else if (5 == g_vars.g_bIsZipEncrypt)
	{
		unsigned long dlen = sizeof(sendbuf)-1;
		ret = uncompress((unsigned char *)sendbuf,&dlen,(unsigned char *)rcvbuf,buflen);
		if (ret != Z_OK)
		{
			buflen = 0;
			m_log.LogBin(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ݰ���zlibѹ��",rcvbuf,buflen);
			return -1;
		}
		memcpy(rcvbuf,sendbuf,dlen);
		buflen = dlen;
		rcvbuf[buflen]=0;
	}
*******************************************************/

	m_log.LogBin(LOG_DEBUG,__FILE__,__LINE__,"�յ���Ӧ����������",rcvbuf,buflen);
	if (!xmlpack.fromBuf(rcvbuf))
	{
		sprintf(m_errMsg,"��̨���ذ���xmlpack");
		m_log.LogBin(LOG_ERROR_GENERAL,__FILE__,__LINE__,m_errMsg,rcvbuf,buflen);
		xmlpack.setReturn(-1,m_errMsg);
		return -1;
	}
	muxmlpack.clear();
	

	//���û�к������Ұ�ͷ������ΪGETXMLFILE,��ȥȡxml�ļ�
	if (!xmlpack.getNextFlag() && xmlpack.getHeadValueI("GETXMLFILE") == 1)
	{
		GetFileSerial(tmpfilename);
		ret = GetFile(xmlpack.getPackValueS("XML�ļ���").c_str(),tmpfilename,m_errMsg);
		if (ret <0)
		{
			xmlpack.setReturn("-1",m_errMsg);
			m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"ȡ������ʧ�� %s",m_errMsg);
			return -1;
		}
		if (xmlpack.fromFile(tmpfilename)<0)
		{
			xmlpack.setReturn("-1","����xml�ļ���xml");
			m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"ȡ������ʧ�� %s",tmpfilename);
			UNLINK(tmpfilename);
			return -1;
		}
		UNLINK(tmpfilename);
		xmlpack.setNextFlag(false);
		muxmlpack.push_back(&xmlpack);
		return 0;
	}

	tmpxmlpack.XmlCopy(&xmlpack);

	//���Ƕ�packӦ����packȡ��muxmpack��
	mupackans = xmlpack.getHeadValueI("MUPACKANS");
	if ( mupackans == 1)
	{
		tmpxmlpack.getMuPackPoint(&muxmlpack);
		muxmlpack.at(0)->XmlCopyHead(&tmpxmlpack);
	}
	else
	{
		muxmlpack.push_back(&xmlpack);
	}
	
	

	//�Ƿ���ͨ�����ļ�ȡ������
	if (tmpxmlpack.getNextFlag() && g_vars.g_bIsFileTrans)
	{
		GetFileSerial(tmpfilename);
		ret = GetFile(tmpxmlpack.getHeadValueS("�ļ���").c_str(),tmpfilename,m_errMsg);
		if (ret <0)
		{
			xmlpack.setReturn("-1",m_errMsg);
			m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"ȡ������ʧ�� %s",m_errMsg);
			return -1;
		}
		ret = GetMuPack(tmpfilename,muxmlpack,false,mupackans);	
		if (ret <0)
		{
			xmlpack.setReturn("-1","ȡ������ʧ��");
			m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"ȡ������ʧ�� %s",tmpfilename);
			return -1;
		}
		return 0;
	}
	
	//ͨ��һ��xml��һ��ͨѶȡ������
	while (tmpxmlpack.getNextFlag())
	{
		tmpxmlpack.setFirstFlag(false);
		tmpxmlpack.setBin(false);
		tmpxmlpack.deletePack();
		buflen = sizeof(sendbuf);
		if (!tmpxmlpack.tobuffer(sendbuf,buflen))
		{
			sprintf(m_errMsg,"xml���ݰ�����");
			m_log.LogMp(1,__FILE__,__LINE__,m_errMsg);
			xmlpack.setReturn(-1,m_errMsg);
			return -1;
		}
		if (1 == g_vars.g_bIsZipEncrypt)
		{
			//����������м���
//			encode.encryptchar(sendbuf,buflen,ENCODEKEY);
		}
		else if (2 == g_vars.g_bIsZipEncrypt)
		{
			memcpy(rcvbuf,sendbuf,buflen);
			unsigned int dlen = sizeof(sendbuf)-1;
			CBF_Tools::Compress((unsigned char *)sendbuf,dlen,(unsigned char *)rcvbuf,buflen);
			buflen = dlen;
		}
		else if (3 == g_vars.g_bIsZipEncrypt )
		{
//			encrypt.encrypt64(sendbuf,buflen,DESENCRYPTKEY);
		}
		else if (4 == g_vars.g_bIsZipEncrypt )
		{
// 			encrypt.encrypt64(sendbuf,buflen,DESENCRYPTKEY);
// 			memcpy(rcvbuf,sendbuf,buflen);
// 			unsigned int dlen = sizeof(sendbuf)-1;
// 			Compress((unsigned char *)sendbuf,&dlen,(unsigned char *)rcvbuf,buflen);
// 			buflen = dlen;
		}
/* *** Updated by songfree at 2009-5-13 15:02:50 ****** 
		else if (5 == g_vars.g_bIsZipEncrypt )
		{
			memcpy(rcvbuf,sendbuf,buflen);
			unsigned long dlen = sizeof(sendbuf)-1;
			ret = compress((unsigned char *)sendbuf,&dlen,(unsigned char *)rcvbuf,buflen);
			if (ret!=Z_OK)
			{
				m_log.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ݰ�ѹ��ʧ��");
				return -1;
			}
			buflen = dlen;
		}
*******************************************************/

		for (int kk=0;kk<2;kk++)
		{
			ret = Send2Server(sendbuf,buflen,rcvbuf,rcvlen);
			if (ret<0)
			{
				if (kk ==1 )
				{
					return ret;
				}
				else
				{
					continue;
				}
			}
			break;
		}
		buflen = rcvlen;
// 		if (Send2Server(sendbuf,buflen,rcvbuf,buflen)<0)
// 		{
// 			return -1;
// 		}
		if (1 == g_vars.g_bIsZipEncrypt)
		{
			//��Ӧ������н���
//			encode.unencryptchar(rcvbuf,buflen,ENCODEKEY);
		}
		else if (2 == g_vars.g_bIsZipEncrypt)
		{
			unsigned int dlen = sizeof(sendbuf)-1;
			CBF_Tools::Uncompress((unsigned char *)sendbuf,dlen,(unsigned char *)rcvbuf,buflen);
			memcpy(rcvbuf,sendbuf,dlen);
			buflen = dlen;
			rcvbuf[buflen]=0;
		}
		else if (3 == g_vars.g_bIsZipEncrypt )
		{
//			encrypt.unencrypt64(rcvbuf,buflen,DESENCRYPTKEY);
		}
		else if (4 == g_vars.g_bIsZipEncrypt )
		{
// 			unsigned int dlen = sizeof(sendbuf)-1;
// 			Uncompress((unsigned char *)sendbuf,&dlen,(unsigned char *)rcvbuf,buflen);
// 			memcpy(rcvbuf,sendbuf,dlen);
// 			buflen = dlen;
// 			rcvbuf[buflen]=0;
// 			encrypt.unencrypt64(rcvbuf,buflen,DESENCRYPTKEY);
		}
/* *** Updated by songfree at 2009-5-13 15:02:57 ****** 
		else if (5 == g_vars.g_bIsZipEncrypt)
		{
			unsigned long dlen = sizeof(sendbuf)-1;
			ret = uncompress((unsigned char *)sendbuf,&dlen,(unsigned char *)rcvbuf,buflen);
			if (ret != Z_OK)
			{
				buflen = 0;
				m_log.LogBin(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ݰ���zlibѹ��",rcvbuf,buflen);
				return -1;
			}
			memcpy(rcvbuf,sendbuf,dlen);
			buflen = dlen;
			rcvbuf[buflen]=0;
		}
*******************************************************/

		m_log.LogBin(5,__FILE__,__LINE__,"�յ���Ӧ����������",rcvbuf,buflen);
		if (!tmpxmlpack.fromBuf(rcvbuf))
		{
			sprintf(m_errMsg,"��̨���ذ���xmlpack");
			m_log.LogBin(1,__FILE__,__LINE__,m_errMsg,rcvbuf,buflen);
			xmlpack.setReturn(-1,m_errMsg);
			return -1;
		}
		if (mupackans == 1)
		{
			tmpxmlpack.getMuPackPoint(&muxmlpack);
		}
		else
		{
			CXmlPack *tmpp = muxmlpack.push_back();
			tmpp->XmlPointCopy(&tmpxmlpack);
			tmpxmlpack.XmlCopyHead(tmpp);
		}
		//muxmlpack.push_back(&tmpxmlpack);
	}
	if (muxmlpack.size()>0)
	{
		xmlpack.setTotalRec(muxmlpack.size());
	}
	return 0;
}
bool CClientApi::GetService(int txcode)
{
	if (!m_isServiceName)
	{
		int ret;
		char tmpchar[30];
		sprintf(tmpchar,"%d",txcode);
		ret = m_xmlconf.getPackValueChar(tmpchar,m_service);
		if (ret<0)
		{
			ret = m_xmlconf.getPackValueChar("default",m_service);
			if (ret<0)
			{
				sprintf(m_errMsg,"ȡTUXEDO�����");
				m_log.LogMp(0,__FILE__,__LINE__,"%s %s",m_errMsg,tmpchar);
				return false;
			}
		}
	}
	return true;
}

int CClientApi::TcpSend(char *buf, int buflen)
{
	int ret;
	char tmpchar[20];
	char buffer[MAXREQBUF];
	int sendlen;
	if (MAXREQBUF - buflen < g_vars.g_rcvHead)
	{
		sprintf(m_errMsg,"���ݳ���[%d]̫��",buflen);
		return -1;
	}
	sprintf(tmpchar,"%s0%d%s","%",g_vars.g_rcvHead,"d");
	if (g_vars.g_bIsInclHead)
	{
		sprintf(buffer,tmpchar,buflen+g_vars.g_rcvHead);
	}
	else
	{
		sprintf(buffer,tmpchar,buflen);
	}
	memcpy(buffer+g_vars.g_rcvHead,buf,buflen);
	sendlen = buflen + g_vars.g_rcvHead;
	ret = m_tcpSocket.Send(buffer,sendlen);
//	m_log.LogBin(LOG_DEBUG,__FILE__,__LINE__,"���͵�������������",buffer,sendlen);
	if (ret!= sendlen)
	{
		sprintf(m_errMsg,"����������� ");
		m_bIsTcpInit = false;
		return -1;
	}
	return 0;
}

int CClientApi::TcpReceive(char *buf, int &buflen)
{
	int ret;
	
	ret = m_tcpSocket.Receive(buf,g_vars.g_rcvHead,g_vars.g_timeOut);
	if (ret != g_vars.g_rcvHead)
	{
		if (ret>0)
		{
			sprintf(m_errMsg,"����ͨѶ��ͷ����,���Ȳ���[%d-%d]",g_vars.g_rcvHead,g_vars.g_timeOut);
//			m_log.LogBin(LOG_ERROR_GENERAL,__FILE__,__LINE__,m_errMsg,buf,ret);
		}
		else if (ret <0)
		{
			sprintf(m_errMsg,"����ͨѶ��ͷ����,�����ѹر�");
			m_bIsTcpInit = false;
//			m_log.Log(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ѹر�",ret,m_errMsg);
		}
		else
		{
			m_bIsTcpInit = false;//�����ָ��ʱ��δ�յ����ݣ����ǳ����ӵĻ����ǹرյĺ�
			m_tcpSocket.Close();
			sprintf(m_errMsg,"����ͨѶ��ͷ����,ָ����ʱ����δ�յ�����[%d-%d]",g_vars.g_rcvHead,g_vars.g_timeOut);
//			m_log.Log(LOG_ERROR_GENERAL,__FILE__,__LINE__,"��ָ����ʱ����δ�յ�����",ret,m_errMsg);
		}
		return -1;
	}
	buf[ret]=0;
	m_publog.trim(buf);
	if (!m_publog.IsNumber(buf))
	{
		sprintf(m_errMsg,"ͨѶ��ͷ����,������");
		m_log.LogBin(LOG_ERROR_GENERAL,__FILE__,__LINE__,m_errMsg,buf,ret);
		return -1;
	}
	buflen = atoi(buf);
	if (g_vars.g_bIsInclHead)
	{
		buflen = buflen - g_vars.g_rcvHead;
	}
	ret = m_tcpSocket.Receive(buf,buflen,g_vars.g_timeOut);
//	m_log.LogBin(5,__FILE__,__LINE__,"�յ���Ӧ����������",buf,ret);
	if (ret != buflen)
	{
		if (ret>0)
		{
			sprintf(m_errMsg,"����ͨѶ��ͷ����,���Ȳ���[%d-%d]",g_vars.g_rcvHead,g_vars.g_timeOut);
//			m_log.LogBin(LOG_ERROR_GENERAL,__FILE__,__LINE__,m_errMsg,buf,ret);
		}
		else if (ret <0)
		{
			sprintf(m_errMsg,"����ͨѶ��ͷ����,�����ѹر�");
			m_bIsTcpInit = false;
//			m_log.Log(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ѹر�",ret,m_errMsg);
		}
		else
		{
			sprintf(m_errMsg,"����ͨѶ��ͷ����,ָ����ʱ����δ�յ�����[%d-%d]",g_vars.g_rcvHead,g_vars.g_timeOut);
//			m_log.Log(LOG_ERROR_GENERAL,__FILE__,__LINE__,"��ָ����ʱ����δ�յ�����",ret,m_errMsg);
		}
		return -1;
	}
	buf[buflen]=0;
	return 0;
}
bool CClientApi::TuxedoInit(bool isinit)
{
	if (!isinit)
	{
#ifdef TUXEDO	
		tuxputenv(m_wsnaddr);

		if (g_tpinitbuf != NULL)
		{
			tpfree((char *)g_tpinitbuf);
			g_tpinitbuf = NULL;
		}
		g_tpinitbuf = (TPINIT *)tpalloc("TPINIT", NULL, TPINITNEED(0));
		g_tpinitbuf->flags = TPMULTICONTEXTS;	
		
		if (tpinit(g_tpinitbuf) == -1) 
		{
			m_log.LogMp(0,__FILE__,__LINE__,"�߳�TUXEDO��ʼ��ʧ�� %d %s",tperrno,"Tpinit failed");
			if (m_isServiceName)
			{
				m_log.LogMp(0,__FILE__,__LINE__,"�߳�TUXEDO��ʼ��ʧ�� %d %s",tperrno,m_wsnaddr);
			}
			m_bIsTpInit = false;
			if (g_tpinitbuf != NULL)
			{
				tpfree((char *)g_tpinitbuf);
				g_tpinitbuf = NULL;
			}
			tpterm();
			return false;
		}
		//tpgetctxt(&m_ctxt, 0);
		//tpsetctxt(m_ctxt,0);
/* *** Updated by songfree at 2008-6-26 11:37:50 ****** 
		if (tpinit((TPINIT *) NULL) == -1) 
		{
			m_log.Log(0,__FILE__,__LINE__,"�߳�TUXEDO��ʼ��ʧ��",tperrno,"Tpinit failed");
			m_bIsTpInit = false;
			tpterm();
			return false;
		}

*******************************************************/

		

#else
		sprintf(m_errMsg,"��Ϊ��TUXEDO�汾!");
#endif
	}
	m_bIsTpInit = true;
	return true;
}

int CClientApi::CallServiceFile(CXmlPack &apack,const char *reqfilename,char *ansfilename)
{
	int ret;
	char rcvbuf[MAXREQBUF];
	int buflen;

	CXmlPackArray muxmlpack;

	char *bufpoint = NULL;

	FILE *fp=NULL;

	memset(rcvbuf,0,sizeof(rcvbuf));


	if (apack.fromFile((char *)reqfilename)<0)
	{
		sprintf(m_errMsg,"�ļ�[%s]��xml",reqfilename);
		return -1;	
	}
	ret = CallService(apack,muxmlpack);
	if (ret <0)
	{
		apack.setReturn("-1",m_errMsg);
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"ȡ������ʧ�� %s",m_errMsg);
		return -1;
	}
	fp = fopen(ansfilename,"wb");
	if (fp == NULL)
	{
		apack.setReturn(-1,"�����ļ�ʧ��");
		return -1;
	}
	//д���ļ�
	for (int i=0;i<muxmlpack.size();i++)
	{
		memset(rcvbuf,0,sizeof(rcvbuf));
		CXmlPack *tmpp = muxmlpack.at(i);
		if (i==0)
		{
			tmpp->XmlCopyHead(&apack);
		}
		buflen = tmpp->getXmlDataLen();
		if (buflen >= MAXREQBUF)
		{
			bufpoint = (char *)malloc(buflen+1);
			memset(bufpoint,0,buflen+1);
			tmpp->toBuf(bufpoint,buflen);
			WriteToFile(fp,bufpoint,buflen+1);
			free(bufpoint);
			bufpoint = NULL;
		}
		else
		{
			tmpp->toBuf(rcvbuf,buflen);
			WriteToFile(fp,rcvbuf,buflen+1);
		}
		
	}
	fclose(fp);
	return 0;
}

int CClientApi::GetFileName(char *filename)
{
	char tmpchar[200];
	char tmpchar2[210];
	memset(tmpchar,0,sizeof(tmpchar));
	WORKPATH(tmpchar);
	sprintf(tmpchar2,"%s/tmp",tmpchar);
	m_publog.makedir(tmpchar2);
	sprintf(filename,"%s/%06d%010ld.nxt",tmpchar2,m_publog.GetPid(),m_publog.getTickCount());
	return 0;
}

int CClientApi::WriteToFile(FILE *fp, char *buf, int buflen)
{
	char tmpchar[MAXREQBUF];
	memset(tmpchar,0,sizeof(tmpchar));
	sprintf(tmpchar,"%09d",buflen);
	fwrite(tmpchar,1,9,fp);
	fwrite(buf,1,buflen,fp);
	return 0;
}

int CClientApi::SendFile(const char *srcfilenamepath,const char * destfilenamepath,char *szmsg)
{
	int ret;
	FILE *fp;
	char xmlbuf[MAXREQBUF];
	char retcode[10];
	memset(xmlbuf,0,sizeof(xmlbuf));
	memset(retcode,0,sizeof(retcode));
	CXmlPack xmlpack;
	CXmlPackArray muxmlpack;

	struct_stat f_stat;
	FSTAT(srcfilenamepath,&f_stat);
	
	fp = fopen(srcfilenamepath,"rb");
	if (fp == NULL)
	{
		sprintf(szmsg,"���ļ�ʧ��");
		return -1;
	}
	ret = fread(xmlbuf,1,6000,fp);
	bool isbegin = true;
	while (ret>0)
	{
		xmlpack.newPackage();
		xmlpack.setHeadNameValue("������","999999006");
		xmlpack.setBinData(xmlbuf,ret);
		xmlpack.setPackNameValue("�ļ���",destfilenamepath);
		if (isbegin)
		{
			xmlpack.setPackNameValue("�ļ���ʼ��־","1");
			isbegin = false;
		}
		else
		{
			xmlpack.setPackNameValue("�ļ���ʼ��־","0");
		}
		if (ret<6000)
		{
			xmlpack.setPackNameValue("�ļ�������־","1");
			xmlpack.setPackNameValue("�ļ���С",f_stat.st_size);
			ret = -1;
		}

		if (CallService(xmlpack,muxmlpack)<0)
		{
			sprintf(szmsg,"����ʧ��%s",m_errMsg);
			fclose(fp);
			return -1;
		}
		xmlpack.getRetCode(retcode);
		if (strcmp(retcode,"0")!=0)
		{
			sprintf(szmsg,"����ʧ��[%s]",xmlpack.getPackValueS("������Ϣ").c_str());
			fclose(fp);
			return -1;
		}
		ret = fread(xmlbuf,1,6000,fp);
	}
	fclose(fp);
	sprintf(szmsg,"�ϴ��ļ��ɹ�");
	return 0;
}

int CClientApi::GetFile(const char *srcfilenamepath,const char * destfilenamepath,char *szmsg)
{
	int ret;
	FILE *fp;
	char xmlbuf[MAXREQBUF];
	char retcode[10];
	memset(xmlbuf,0,sizeof(xmlbuf));
	memset(retcode,0,sizeof(retcode));
	CXmlPack xmlpack;
	CXmlPackArray muxmlpack;
	
	xmlpack.newPackage();
	xmlpack.setHeadNameValue("������","999999003");
	xmlpack.setPackNameValue("�ļ���",srcfilenamepath);
	xmlpack.setPackNameValue("ƫ����","0");
    if (CallService(xmlpack,muxmlpack)<0)
	{
		sprintf(szmsg,"����ʧ��%s",m_errMsg);
		return -1;
	}
	xmlpack.getHeadValueChar("������",retcode);
	if (strcmp(retcode,"0")!=0)
	{
		sprintf(szmsg,"��̨����:%s",xmlpack.getPackValueS("������Ϣ").c_str());
		return -1;
	}
	fp = fopen(destfilenamepath,"wb");
	if (fp == NULL)
	{
		sprintf(szmsg,"д���ļ�ʧ��");
		return -1;
	}
	if (!xmlpack.getBinData(xmlbuf,ret))
	{
		sprintf(szmsg,"��̨�����޶���������");
		fclose(fp);
		UNLINK(destfilenamepath);
		return -1;
	}
	fwrite(xmlbuf,1,ret,fp);
	while (xmlpack.getPackValueI("NEXTFLAG") == 1)
	{
		xmlpack.setFirstFlag(true);
		xmlpack.setBin(false);
		xmlpack.setPackNameValue("�ļ���",srcfilenamepath);
		xmlpack.setPackNameValue("NEXTFLAG","0");
		if (CallService(xmlpack,muxmlpack)<0)
		{
			sprintf(szmsg,"����ʧ��%s",m_errMsg);
			fclose(fp);
			UNLINK(destfilenamepath);
			return -1;
		}
		xmlpack.getHeadValueChar("������",retcode);
		if (strcmp(retcode,"0")!=0)
		{
			sprintf(szmsg,"��̨����:%s",xmlpack.getPackValueS("������Ϣ").c_str());
			fclose(fp);
			UNLINK(destfilenamepath);
			return -1;
		}
		if (!xmlpack.getBinData(xmlbuf,ret))
		{
			sprintf(szmsg,"��̨�����޶���������");
			fclose(fp);
			UNLINK(destfilenamepath);
			return -1;
		}
		fwrite(xmlbuf,1,ret,fp);
	}
	fclose(fp);
	struct_stat f_stat;
	FSTAT(destfilenamepath,&f_stat);
	ret = xmlpack.getPackValueI("�ļ���С");
	if (ret != f_stat.st_size)
	{
		UNLINK(destfilenamepath);
		sprintf(szmsg,"�ļ���С����%ld-%ld",ret,f_stat.st_size);
		return -1;
	}
	sprintf(szmsg,"ȡ�ļ��ɹ�");
	
	return 0;
}
int CClientApi::SetCommPara(const char *ip, int port, int zip, int pwdflag, int timeout, int headlen, int inchead,const char *logfile,int loglevel,bool istransfile,bool shortconnect)
{
	
	strcpy(g_vars.g_sIp,ip);
	g_vars.g_nPort = port;
	g_vars.g_bIsZipEncrypt = zip;
	g_vars.g_bIsPwdEncrypt = pwdflag;
	g_vars.g_timeOut = timeout;
	g_vars.g_rcvHead = headlen;
	g_vars.g_bIsInclHead = inchead;
	strcpy(g_vars.g_logFileName,logfile);
	g_vars.g_trace = loglevel;
	m_log.SetLogPara(loglevel,"",logfile);
	
	g_vars.g_bIsTuxedo = false;
	g_vars.g_bIsFileTrans = istransfile;
	m_isServiceName = false;
	m_bIsShortConnect = shortconnect;
	
	return 0;
}

int CClientApi::SetTuxedoPara(const char *wsnaddr,const char *serviceName, int zip, int pwdflag,char *logfile,int loglevel,bool istransfile)
{
	
	g_vars.g_bIsZipEncrypt = zip;
	g_vars.g_bIsPwdEncrypt = pwdflag;
	
	strcpy(g_vars.g_logFileName,logfile);
	g_vars.g_trace = loglevel;
	m_log.SetLogPara(loglevel,"",logfile);
	g_vars.g_bIsTuxedo = true;
	m_publog.Strncopy(m_service,(char *)serviceName,59);
	m_isServiceName = true;
	g_vars.g_bIsFileTrans = istransfile;

#ifdef TUXEDO
	sprintf(m_wsnaddr,"WSNADDR=%s",wsnaddr);
//	tuxputenv(m_wsnaddr);
	m_bIsTpInit = false;
#endif
	return 0;
}

int CClientApi::GetFileSerial(char *serial)
{
#ifdef WIN32
	sprintf(serial,"%ld.nxt",abs(m_publog.getTickCount()));
#else
	sprintf(serial,"%s/tmp/%ld.nxt",(char *)getenv("HOME"),std::abs((long)m_publog.getTickCount()));
#endif
	return 0;
}

int CClientApi::GetMuPack(const char *filename, CXmlPackArray &muxmlpack,int mupackans,bool first)
{
	int ret;
	char buffer[MAXREQBUF];
	int buflen;
	int readcount=0;
	CXmlPack xmlpack;
	FILE *fp = NULL;
	struct_stat f_stat;
	FSTAT(filename,&f_stat);
	fp = fopen(filename,"rb");
	if (fp == NULL)
	{
		sprintf(m_errMsg,"�ļ�%s��ʧ��",filename);
		UNLINK(filename);
		return -1;
	}
	ret = fread(buffer,1,9,fp);
	if (ret != 9 )
	{
		sprintf(m_errMsg,"�ļ�%s�Ƿ�",filename);
		fclose(fp);
		UNLINK(filename);
		return -1;
	}
	readcount =+ret;
	buffer[ret]=0;
	if (!m_publog.IsNumber(buffer))
	{
		sprintf(m_errMsg,"�ļ�%s�Ƿ�",filename);
		fclose(fp);
		UNLINK(filename);
		return -1;
	}
	buflen = atoi(buffer);
	if (buflen>=MAXREQBUF)
	{
		sprintf(m_errMsg,"�ļ�%s�Ƿ�",filename);
		fclose(fp);
		UNLINK(filename);
		return -1;
	}
	ret = fread(buffer,1,buflen,fp);
	if (ret != buflen)
	{
		sprintf(m_errMsg,"�ļ�%s�Ƿ�",filename);
		fclose(fp);
		UNLINK(filename);
		return -1;
	}
	buffer[ret] = 0;
	readcount +=ret;
	if (first)
	{
		if (!xmlpack.fromBuf(buffer))
		{
			sprintf(m_errMsg,"�ļ�%s�Ƿ�",filename);
			fclose(fp);
			UNLINK(filename);
			return -1;
		}
		mupackans = xmlpack.getHeadValueI("MUPACKANS");
		if (mupackans == 1)
		{
			xmlpack.getMuPackPoint(&muxmlpack);
		}
		else
		{
			CXmlPack *tmpp = muxmlpack.push_back();
			tmpp->XmlPointCopy(&xmlpack);
		}
		//muxmlpack.push_back(&xmlpack);
	}
	while (!feof(fp) && readcount < f_stat.st_size )
	{
		ret = fread(buffer,1,9,fp);
		if (ret != 9 )
		{
			muxmlpack.clear();
			sprintf(m_errMsg,"�ļ�%s�Ƿ�",filename);
			fclose(fp);
			UNLINK(filename);
			return -1;
		}
		readcount +=ret;
		buffer[ret]=0;
		if (!m_publog.IsNumber(buffer))
		{
			muxmlpack.clear();
			sprintf(m_errMsg,"�ļ�%s�Ƿ�",filename);
			fclose(fp);
			UNLINK(filename);
			return -1;
		}
		buflen = atoi(buffer);
		if (buflen >= MAXREQBUF)
		{
			sprintf(m_errMsg,"�ļ�%s�Ƿ�",filename);
			fclose(fp);
			UNLINK(filename);
			return -1;
		}
		ret = fread(buffer,1,buflen,fp);
		if (ret != buflen)
		{
			sprintf(m_errMsg,"�ļ�%s�Ƿ�",filename);
			fclose(fp);
			UNLINK(filename);
			return -1;
		}
		buffer[ret] = 0;
		readcount +=ret;
		if (!xmlpack.fromBuf(buffer))
		{
			sprintf(m_errMsg,"�ļ�%s�Ƿ�",filename);
			fclose(fp);
			UNLINK(filename);
			return -1;
		}
		if (mupackans == 1)
		{
			xmlpack.getMuPackPoint(&muxmlpack);
		}
		else
		{
			CXmlPack *tmpp = muxmlpack.push_back();
			tmpp->XmlPointCopy(&xmlpack);
		}
		//muxmlpack.push_back(&xmlpack);
	}
	fclose(fp);
	UNLINK(filename);
	return 0;
}

int CClientApi::SetTuxedoPara(bool shortconnect,int tuxedobuflen,bool tuxedoalias)
{
	m_bIsShortConnect = shortconnect;
	if (tuxedobuflen < 8192)
	{
		m_TuxedoBufLen = 8192;
	}
	else
	{
		m_TuxedoBufLen = tuxedobuflen;
	}
	m_tuxedoAlias = tuxedoalias;
	return 0;
}

int CClientApi::CallService(const char *bizinfobuf, std::string &bizinfoansbuf)
{
	
	int ret;
	int i;
	int offset=0;
	int offsetend =0;
	int beginLen = 4;
	char bizhead[5];
	char bizend[5];
	char bizinfoseg[10];
	memset(bizhead,0,sizeof(bizhead));
	memset(bizend,0,sizeof(bizend));
	memset(bizinfoseg,0,sizeof(bizinfoseg));
	strcpy(bizinfoseg,BIZINFOSEG);
	CXmlPack xmlpack;
	CXmlPackArray muxmlpack;


	if (Biz2Xml(bizinfobuf,xmlpack)<0)
	{
		return -1;
	}
	ret = CallService(xmlpack,muxmlpack);	
	if (ret <0)
	{
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"���ú�̨���� %d %s",ret,m_errMsg);
		return -1;	
	}
	if (muxmlpack.size()>1)
	{
		sprintf(m_errMsg,"��̨���ض��xml���ݰ����޷�ת��ΪBIZINFO");
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"���ݷǷ� %d %s",muxmlpack.size(),m_errMsg);
		return -1;	
	}
	if (Xml2Biz(&xmlpack, bizinfoansbuf)<0)
	{
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"ת��Ϊbizinfoʧ�� %d %s",muxmlpack.size(),m_errMsg);
		return -1;	
	}
	return 0;
}

int CClientApi::Biz2Xml(const char *bizinfobuf, CXmlPack &xmlpack)
{
	int ret;
	int i;
	int reccount;
	char tmpName[4000];
	char tmpValue[8193];


	int offset=0;
	int offsetend =0;
	int beginLen = 4;
	char bizhead[10];
	char bizend[10];
	char bizinfoseg[10];
	memset(bizhead,0,sizeof(bizhead));
	memset(bizend,0,sizeof(bizend));
	memset(bizinfoseg,0,sizeof(bizinfoseg));
	strcpy(bizinfoseg,BIZINFOSEG);

	memset(tmpName,0,sizeof(tmpName));
	memset(tmpValue,0,sizeof(tmpValue));
	
	strcpy(bizhead,"{D::");//bizinfo�Ŀ�ʼΪ{D::,����Ϊ}
	strcpy(bizend,"}");
	
	ret = strlen(bizinfobuf);
	m_log.LogBin(LOG_DEBUG,__FILE__,__LINE__,"bizinfo��������",(char *)bizinfobuf,ret);


	for (i=0; i< ret; i++)
	{
		if (strncmp(bizinfobuf+i,bizhead,strlen(bizhead)) == 0)
		{
			offset = i+strlen(bizhead);
			break;
		}
	}
	if (offset <1)
	{
		sprintf(m_errMsg,"����biz���ݲ���[%s]��ʼ",bizhead);
		return -1;
	}
	for (i= ret -1; i >1; i--)
	{
		if (strncmp(bizinfobuf+i,bizend,strlen(bizend)) == 0)
		{
			offsetend = i;
			break;
		}
	}
	if (offsetend <1)
	{
		sprintf(m_errMsg,"����biz���ݲ���[%s]����",bizend);
		return -1;
	}
	ret = offsetend - offset;
	if (ret <1)
	{
		sprintf(m_errMsg,"����biz����[%s-%s]�м�������",bizhead,bizend);
		return -1;
	}
	char *filldata = (char *)malloc(ret+2);
	memset(filldata,0,ret+2);
	memcpy(filldata,bizinfobuf+offset,ret);


	m_log.LogBin(LOG_DEBUG,__FILE__,__LINE__,"ת��ǰbizinfo��������",filldata,ret);
	filldata[ret] = bizinfoseg[0];//���һ��Ҫ��:����
	m_log.LogBin(LOG_DEBUG,__FILE__,__LINE__,"ת��ǰbizinfo��������",filldata,ret+1);
	if (m_publog.FillStrings(filldata,bizinfoseg[0])<0)
	{
		sprintf(m_errMsg,"����:�ָ�����");
		free(filldata);
		filldata = NULL;
		return -1;
	}
	free(filldata);
	filldata = NULL;
	ret = m_publog.GetCount();
	if (ret <1)
	{
		sprintf(m_errMsg,"bizinfo����������");
		return -1;
	}
	if (ret%2 != 0)
	{
		sprintf(m_errMsg,"bizinfo����NAME:VALUEG��ƥ��-�ܹ����ݸ���%d",ret);
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"���ݷǷ� %d %s",ret,m_errMsg);
		return -1;	
	}

	xmlpack.newPackage();
	reccount = m_publog.GetCount();
	std::string tmpstring;
	std::string ssegvalue;
	for (i=0; i<reccount; )
	{
		memset(tmpName,0,sizeof(tmpName));
		memset(tmpValue,0,sizeof(tmpValue));
		m_publog.GetAt(i,tmpName,true);
		m_publog.GetAt(i+1,tmpValue,false);//value��ȥ���ո񣬴���������
		if (strcmp(tmpName,"TXN") == 0) //������
		{
			xmlpack.setHeadNameValue("������",tmpValue);
		}
		ret = xmlpack.getPackValue(tmpName,tmpstring);
		if (ret <0) //û�д��ֶΣ�����
		{
			xmlpack.setPackNameValue(tmpName,tmpValue);
		}
		else  //�д��ֶΣ����¼
		{
			
			if (tmpstring.length()>0)
			{
				CNode *tmpnode = xmlpack.m_xml.getCurrentNode();
				if (tmpnode == NULL)
				{
					sprintf(m_errMsg,"���ɳ��ֵ�xml�ڵ��");
					return -1;
				}
				tmpnode->m_value = "";//��ԭ����ֵ��Ϊ��
				xmlpack.m_xml.addItem("RECORD",tmpstring);
				xmlpack.m_xml.addItem("RECORD",tmpValue);
			}
			else
			{
				xmlpack.m_xml.addItem("RECORD",tmpValue);
			}
			
		}
		i +=2;
	}
	return 0;
}

int CClientApi::Xml2Biz(CXmlPack *xmlpack,std::string &bizbuf)
{
	int ret;
	char bizhead[10];
	char bizend[10];
	char bizinfoseg[10];
	char tmpchar[400];
	int buflen;
	Cpubstrtime publog;
	memset(bizhead,0,sizeof(bizhead));
	memset(bizend,0,sizeof(bizend));
	memset(tmpchar,0,sizeof(tmpchar));
	memset(bizinfoseg,0,sizeof(bizinfoseg));

	strcpy(bizinfoseg,BIZINFOSEG);
	strcpy(bizhead,"{D::");//bizinfo�Ŀ�ʼΪ{D::,����Ϊ}
	strcpy(bizend,"}");
	

	sprintf(tmpchar,"11%s",bizinfoseg);
	publog.FillStrings(tmpchar,bizinfoseg[0]);
	publog.Clear();
	publog.Add("RET_CODE");
	if (xmlpack->getHeadValueI("������") == 0)
	{
		publog.Add("0000");
	}
	else
	{
		publog.Add((char *)xmlpack->getHeadValueS("������").c_str());
	}
	publog.Add("ERR_MSG");
	publog.Add((char *)xmlpack->getPackValueS("������Ϣ").c_str());
	xmlpack->gotoPack();
	CNode *packnode = xmlpack->m_xml.getFirstChildItem();
	while (packnode != NULL)
	{
		
		xmlpack->m_xml.setCurrentNode(packnode);
		CNode *tmpnode = xmlpack->m_xml.getEndChildItem();
		if (tmpnode == NULL)//Ϊ��˵�����Ǽ�¼
		{
			if (strcmp(packnode->getName().c_str(),"������Ϣ") != 0 )
			{
				publog.Add((char *)packnode->getName().c_str());
				publog.Add((char *)packnode->getValue().c_str());
			}
		}
		while (tmpnode != NULL)
		{
			publog.Add((char *)packnode->getName().c_str());
			publog.Add((char *)tmpnode->getValue().c_str());
			tmpnode = xmlpack->m_xml.getLastSibling();
		}
		xmlpack->m_xml.setCurrentNode(packnode);
		packnode = xmlpack->m_xml.getNextSibling();
	}
	xmlpack->gotoPack();
	try
	{
		bizbuf = bizhead;
		ret = publog.GetCount();
		for (int i=0 ; i< ret ; i++)
		{
			bizbuf = bizbuf + publog.GetAt(i);
			if (i<ret-1)
			{
				bizbuf = bizbuf +BIZINFOSEG;
			}
		}
		bizbuf = bizbuf +bizend;
		m_log.LogBin(LOG_DEBUG,__FILE__,__LINE__,"ת��Ӧ����bizinfo��������",(char *)bizbuf.c_str(),bizbuf.length());
	}
	catch (...)
	{
		return -1;
	}
	
	return 0;
}

bool CClientApi::TcpInit()
{
	if (m_bIsTcpInit)
	{
		return true;
	}
	m_tcpSocket.Close();
	if (!m_tcpSocket.Create())
	{
		sprintf(m_errMsg,"����socketʧ��");
		m_log.LogMp(0,__FILE__,__LINE__,"����socketʧ��");
		return false;
	}
	if (!m_tcpSocket.ConnectTcp(g_vars.g_sIp, g_vars.g_nPort))
	{
		sprintf(m_errMsg,"����Զ�����ʧ�� %d-%s",g_vars.g_nPort,g_vars.g_sIp);
		m_log.LogMp(0,__FILE__,__LINE__,"����Զ�����ʧ�� %d %s",g_vars.g_nPort,g_vars.g_sIp);
		return false;
	}
	m_bIsTcpInit = true;
	return true;
}

void CClientApi::Close()
{
	m_bIsTcpInit = false;
	m_tcpSocket.Close();
}
