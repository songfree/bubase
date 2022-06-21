// CgateLog.cpp: implementation of the CCgateLog class.
//
//////////////////////////////////////////////////////////////////////

#include "CgateLog.h"
#include "BF_Tools.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



CCgateLog::CCgateLog()
{
	
}

CCgateLog::~CCgateLog()
{

}
// ������: LogDreb
// ���  : ������ 2013-5-6 16:00:38
// ����  : void 
// ����  : int level
// ����  : PCGATE_COMMSTRU data
// ����  : д��dreb����־
void CCgateLog::LogCgate(int level ,PCGATE_COMMSTRU data,int index,const char *key)
{
	if (level > m_nLogLevel)
	{
		return;
	}
	COMMSTRU  wdata;
	bzero(&wdata,sizeof(COMMSTRU));
	//����ͷ
	wdata.head.nLen = data->head.nLen;
	wdata.head.cCmd = data->head.stComm.cCmd;
	wdata.head.cZip = data->head.stComm.cZip;
	wdata.head.cDrebAffirm = 0;
	wdata.head.cNextFlag = data->head.stComm.cNextFlag; //�Ƿ���ȡ������
	wdata.head.cRaflag = data->head.stComm.cRaflag;//����Ӧ���־
	
	wdata.head.d_Dinfo.d_cNodePrivateId = data->head.stDest.cNodePrivateId;
	wdata.head.d_Dinfo.d_cSvrPrivateId = data->head.stDest.cSvrPrivateId;
	wdata.head.d_Dinfo.d_nNodeId = data->head.stDest.nNodeId;
	wdata.head.d_Dinfo.d_nServiceNo = data->head.stDest.nServiceNo;
	wdata.head.d_Dinfo.d_nSvrMainId = data->head.stDest.nSvrMainId;
	if (key != NULL)
	{
		//��s_nDrebSerial����Ự��Կ
		char tmpchar[17];
		bzero(tmpchar,sizeof(tmpchar));
		memcpy(tmpchar,key,8);
		wdata.head.s_Sinfo.s_nDrebSerial = atoi(tmpchar);
	}


	wdata.head.s_Sinfo.s_nSerial = data->head.stDest.nSerial;
	wdata.head.s_Sinfo.s_nGateIndex = index;
	

	//ȡ����������Ϣ
	wdata.head.n_Ninfo.n_nNextNo = data->head.stNext.n_nNextNo;
	wdata.head.n_Ninfo.n_nNextOffset = data->head.stNext.n_nNextOffset;

	if (wdata.head.nLen >0)
	{
		memcpy(wdata.buffer,data->buffer,wdata.head.nLen );
	}

	m_pLogGate.LogDreb(level,&wdata,false);
	return ;
}

void CCgateLog::SetLogPara(int logsize,int level ,const char *logpath,const char *cgatelogname)
{
	m_nLogLevel = level;
	m_pLogGate.SetLogPara(logsize,level,logpath,cgatelogname,"bpccgate.dat");
}

void CCgateLog::SetLogLevel(int loglevel)
{
	m_nLogLevel = loglevel;
	m_pLogGate.SetLogLevel(loglevel);
}

void CCgateLog::StopLog()
{
	m_pLogGate.StopLog();
}

bool CCgateLog::StartLog()
{
	return m_pLogGate.StartLog();
}

bool CCgateLog::IsStoped()
{
	return m_pLogGate.IsStoped();
}
