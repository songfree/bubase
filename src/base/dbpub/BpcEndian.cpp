// BpcEndian.cpp: implementation of the CBpcEndian class.
//
//////////////////////////////////////////////////////////////////////

#include "BpcEndian.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBpcEndian::CBpcEndian()
{
	m_bUseLittleEndian = true;
	m_bIsLittle = true;   //主机序 linux windows
	if (htonl(1) == 1)
	{
		m_bIsLittle = false;//本机为网络序 AIX HP_UX
	}
}

CBpcEndian::~CBpcEndian()
{

}

void CBpcEndian::Endian2LocalHost(PBPCCOMMSTRU data)
{
	if (m_bUseLittleEndian)
	{
		if (!m_bIsLittle)
		{
			Reverse((unsigned char *)&(data->sBpcHead.nBpcLen),sizeof(data->sBpcHead.nBpcLen));
			Reverse((unsigned char *)&(data->sBpcHead.nBpuIndex),sizeof(data->sBpcHead.nBpuIndex));
			Reverse((unsigned char *)&(data->sBpcHead.nConnectTime),sizeof(data->sBpcHead.nConnectTime));
			Reverse((unsigned char *)&(data->sBpcHead.nIndex),sizeof(data->sBpcHead.nIndex));
			Reverse((unsigned char *)&(data->sBpcHead.nRtime),sizeof(data->sBpcHead.nRtime));
			
//			Reverse((unsigned char *)&(data->sBpcHead.nMsgFlag),sizeof(data->sBpcHead.nMsgFlag)); //已经约定好为LMSDID
			
			Reverse((unsigned char *)&(data->sDBHead.a_Ainfo.a_nNodeId),sizeof(data->sDBHead.a_Ainfo.a_nNodeId));
			Reverse((unsigned char *)&(data->sDBHead.a_Ainfo.a_nRetCode),sizeof(data->sDBHead.a_Ainfo.a_nRetCode));
			Reverse((unsigned char *)&(data->sDBHead.b_Cinfo.b_nSerial),sizeof(data->sDBHead.b_Cinfo.b_nSerial));
			Reverse((unsigned char *)&(data->sDBHead.b_Cinfo.b_cIndex),sizeof(data->sDBHead.b_Cinfo.b_cIndex));
			Reverse((unsigned char *)&(data->sDBHead.d_Dinfo.d_nNodeId),sizeof(data->sDBHead.d_Dinfo.d_nNodeId));
			Reverse((unsigned char *)&(data->sDBHead.d_Dinfo.d_nServiceNo),sizeof(data->sDBHead.d_Dinfo.d_nServiceNo));
			Reverse((unsigned char *)&(data->sDBHead.d_Dinfo.d_nSvrMainId),sizeof(data->sDBHead.d_Dinfo.d_nSvrMainId));
			Reverse((unsigned char *)&(data->sDBHead.n_Ninfo.n_nNextNo),sizeof(data->sDBHead.n_Ninfo.n_nNextNo));
			Reverse((unsigned char *)&(data->sDBHead.n_Ninfo.n_nNextOffset),sizeof(data->sDBHead.n_Ninfo.n_nNextOffset));
			Reverse((unsigned char *)&(data->sDBHead.nCrc),sizeof(data->sDBHead.nCrc));
			Reverse((unsigned char *)&(data->sDBHead.nLen),sizeof(data->sDBHead.nLen));
			Reverse((unsigned char *)&(data->sDBHead.s_Sinfo.s_nDrebSerial),sizeof(data->sDBHead.s_Sinfo.s_nDrebSerial));
			Reverse((unsigned char *)&(data->sDBHead.s_Sinfo.s_nHook),sizeof(data->sDBHead.s_Sinfo.s_nHook));
			Reverse((unsigned char *)&(data->sDBHead.s_Sinfo.s_nIndex),sizeof(data->sDBHead.s_Sinfo.s_nIndex));
			Reverse((unsigned char *)&(data->sDBHead.s_Sinfo.s_nNodeId),sizeof(data->sDBHead.s_Sinfo.s_nNodeId));
			Reverse((unsigned char *)&(data->sDBHead.s_Sinfo.s_nSerial),sizeof(data->sDBHead.s_Sinfo.s_nSerial));
			Reverse((unsigned char *)&(data->sDBHead.s_Sinfo.s_nGateIndex),sizeof(data->sDBHead.s_Sinfo.s_nGateIndex));
			Reverse((unsigned char *)&(data->sDBHead.s_Sinfo.s_nSvrMainId),sizeof(data->sDBHead.s_Sinfo.s_nSvrMainId));
			Reverse((unsigned char *)&(data->sDBHead.s_Sinfo.s_nTimestamp),sizeof(data->sDBHead.s_Sinfo.s_nTimestamp));
		}
	}
	else
	{
		if (m_bIsLittle)
		{
			Reverse((unsigned char *)&(data->sBpcHead.nBpcLen),sizeof(data->sBpcHead.nBpcLen));
			Reverse((unsigned char *)&(data->sBpcHead.nBpuIndex),sizeof(data->sBpcHead.nBpuIndex));
			Reverse((unsigned char *)&(data->sBpcHead.nConnectTime),sizeof(data->sBpcHead.nConnectTime));
			Reverse((unsigned char *)&(data->sBpcHead.nIndex),sizeof(data->sBpcHead.nIndex));
			Reverse((unsigned char *)&(data->sBpcHead.nRtime),sizeof(data->sBpcHead.nRtime));
			
//			Reverse((unsigned char *)&(data->sBpcHead.nMsgFlag),sizeof(data->sBpcHead.nMsgFlag)); //已经约定好为LMSDID
			
			Reverse((unsigned char *)&(data->sDBHead.a_Ainfo.a_nNodeId),sizeof(data->sDBHead.a_Ainfo.a_nNodeId));
			Reverse((unsigned char *)&(data->sDBHead.a_Ainfo.a_nRetCode),sizeof(data->sDBHead.a_Ainfo.a_nRetCode));
			Reverse((unsigned char *)&(data->sDBHead.b_Cinfo.b_nSerial),sizeof(data->sDBHead.b_Cinfo.b_nSerial));
			Reverse((unsigned char *)&(data->sDBHead.b_Cinfo.b_cIndex),sizeof(data->sDBHead.b_Cinfo.b_cIndex));
			Reverse((unsigned char *)&(data->sDBHead.d_Dinfo.d_nNodeId),sizeof(data->sDBHead.d_Dinfo.d_nNodeId));
			Reverse((unsigned char *)&(data->sDBHead.d_Dinfo.d_nServiceNo),sizeof(data->sDBHead.d_Dinfo.d_nServiceNo));
			Reverse((unsigned char *)&(data->sDBHead.d_Dinfo.d_nSvrMainId),sizeof(data->sDBHead.d_Dinfo.d_nSvrMainId));
			Reverse((unsigned char *)&(data->sDBHead.n_Ninfo.n_nNextNo),sizeof(data->sDBHead.n_Ninfo.n_nNextNo));
			Reverse((unsigned char *)&(data->sDBHead.n_Ninfo.n_nNextOffset),sizeof(data->sDBHead.n_Ninfo.n_nNextOffset));
			Reverse((unsigned char *)&(data->sDBHead.nCrc),sizeof(data->sDBHead.nCrc));
			Reverse((unsigned char *)&(data->sDBHead.nLen),sizeof(data->sDBHead.nLen));
			Reverse((unsigned char *)&(data->sDBHead.s_Sinfo.s_nDrebSerial),sizeof(data->sDBHead.s_Sinfo.s_nDrebSerial));
			Reverse((unsigned char *)&(data->sDBHead.s_Sinfo.s_nHook),sizeof(data->sDBHead.s_Sinfo.s_nHook));
			Reverse((unsigned char *)&(data->sDBHead.s_Sinfo.s_nIndex),sizeof(data->sDBHead.s_Sinfo.s_nIndex));
			Reverse((unsigned char *)&(data->sDBHead.s_Sinfo.s_nNodeId),sizeof(data->sDBHead.s_Sinfo.s_nNodeId));
			Reverse((unsigned char *)&(data->sDBHead.s_Sinfo.s_nSerial),sizeof(data->sDBHead.s_Sinfo.s_nSerial));
			Reverse((unsigned char *)&(data->sDBHead.s_Sinfo.s_nGateIndex),sizeof(data->sDBHead.s_Sinfo.s_nGateIndex));
			Reverse((unsigned char *)&(data->sDBHead.s_Sinfo.s_nSvrMainId),sizeof(data->sDBHead.s_Sinfo.s_nSvrMainId));
			Reverse((unsigned char *)&(data->sDBHead.s_Sinfo.s_nTimestamp),sizeof(data->sDBHead.s_Sinfo.s_nTimestamp));
		}
	}

}

void CBpcEndian::Endian2LocalHost(unsigned char *data,int datalen)
{
	if (m_bUseLittleEndian)
	{
		if (!m_bIsLittle)
		{
			Reverse(data,datalen);
		}
	}
	else
	{
		if (m_bIsLittle)
		{
			Reverse(data,datalen);
		}
	}
	
}
void CBpcEndian::Endian2Comm(PBPCCOMMSTRU data)
{
	if (m_bUseLittleEndian) //通讯用主机序
	{
		if (!m_bIsLittle) //本机为网络序
		{
			Reverse((unsigned char *)&(data->sBpcHead.nBpcLen),sizeof(data->sBpcHead.nBpcLen));
			Reverse((unsigned char *)&(data->sBpcHead.nBpuIndex),sizeof(data->sBpcHead.nBpuIndex));
			Reverse((unsigned char *)&(data->sBpcHead.nConnectTime),sizeof(data->sBpcHead.nConnectTime));
			Reverse((unsigned char *)&(data->sBpcHead.nIndex),sizeof(data->sBpcHead.nIndex));
			Reverse((unsigned char *)&(data->sBpcHead.nRtime),sizeof(data->sBpcHead.nRtime));

			data->sBpcHead.nMsgFlag = LMSGID;//通讯用主机序
			
			Reverse((unsigned char *)&(data->sDBHead.a_Ainfo.a_nNodeId),sizeof(data->sDBHead.a_Ainfo.a_nNodeId));
			Reverse((unsigned char *)&(data->sDBHead.a_Ainfo.a_nRetCode),sizeof(data->sDBHead.a_Ainfo.a_nRetCode));
			Reverse((unsigned char *)&(data->sDBHead.b_Cinfo.b_nSerial),sizeof(data->sDBHead.b_Cinfo.b_nSerial));
			Reverse((unsigned char *)&(data->sDBHead.b_Cinfo.b_cIndex),sizeof(data->sDBHead.b_Cinfo.b_cIndex));
			Reverse((unsigned char *)&(data->sDBHead.d_Dinfo.d_nNodeId),sizeof(data->sDBHead.d_Dinfo.d_nNodeId));
			Reverse((unsigned char *)&(data->sDBHead.d_Dinfo.d_nServiceNo),sizeof(data->sDBHead.d_Dinfo.d_nServiceNo));
			Reverse((unsigned char *)&(data->sDBHead.d_Dinfo.d_nSvrMainId),sizeof(data->sDBHead.d_Dinfo.d_nSvrMainId));
			Reverse((unsigned char *)&(data->sDBHead.n_Ninfo.n_nNextNo),sizeof(data->sDBHead.n_Ninfo.n_nNextNo));
			Reverse((unsigned char *)&(data->sDBHead.n_Ninfo.n_nNextOffset),sizeof(data->sDBHead.n_Ninfo.n_nNextOffset));
			Reverse((unsigned char *)&(data->sDBHead.nCrc),sizeof(data->sDBHead.nCrc));
			Reverse((unsigned char *)&(data->sDBHead.nLen),sizeof(data->sDBHead.nLen));
			Reverse((unsigned char *)&(data->sDBHead.s_Sinfo.s_nDrebSerial),sizeof(data->sDBHead.s_Sinfo.s_nDrebSerial));
			Reverse((unsigned char *)&(data->sDBHead.s_Sinfo.s_nHook),sizeof(data->sDBHead.s_Sinfo.s_nHook));
			Reverse((unsigned char *)&(data->sDBHead.s_Sinfo.s_nIndex),sizeof(data->sDBHead.s_Sinfo.s_nIndex));
			Reverse((unsigned char *)&(data->sDBHead.s_Sinfo.s_nNodeId),sizeof(data->sDBHead.s_Sinfo.s_nNodeId));
			Reverse((unsigned char *)&(data->sDBHead.s_Sinfo.s_nSerial),sizeof(data->sDBHead.s_Sinfo.s_nSerial));
			Reverse((unsigned char *)&(data->sDBHead.s_Sinfo.s_nGateIndex),sizeof(data->sDBHead.s_Sinfo.s_nGateIndex));
			Reverse((unsigned char *)&(data->sDBHead.s_Sinfo.s_nSvrMainId),sizeof(data->sDBHead.s_Sinfo.s_nSvrMainId));
			Reverse((unsigned char *)&(data->sDBHead.s_Sinfo.s_nTimestamp),sizeof(data->sDBHead.s_Sinfo.s_nTimestamp));
		}
		else //本机为主机序
		{
			data->sBpcHead.nMsgFlag = LMSGID;
		}
	}
	else //通讯用网络序
	{
		if (m_bIsLittle) //本机为主机序
		{
			Reverse((unsigned char *)&(data->sBpcHead.nBpcLen),sizeof(data->sBpcHead.nBpcLen));
			Reverse((unsigned char *)&(data->sBpcHead.nBpuIndex),sizeof(data->sBpcHead.nBpuIndex));
			Reverse((unsigned char *)&(data->sBpcHead.nConnectTime),sizeof(data->sBpcHead.nConnectTime));
			Reverse((unsigned char *)&(data->sBpcHead.nIndex),sizeof(data->sBpcHead.nIndex));
			Reverse((unsigned char *)&(data->sBpcHead.nRtime),sizeof(data->sBpcHead.nRtime));
			
			data->sBpcHead.nMsgFlag = RMSGID;   //通讯用网络序
		
			Reverse((unsigned char *)&(data->sDBHead.a_Ainfo.a_nNodeId),sizeof(data->sDBHead.a_Ainfo.a_nNodeId));
			Reverse((unsigned char *)&(data->sDBHead.a_Ainfo.a_nRetCode),sizeof(data->sDBHead.a_Ainfo.a_nRetCode));
			Reverse((unsigned char *)&(data->sDBHead.b_Cinfo.b_nSerial),sizeof(data->sDBHead.b_Cinfo.b_nSerial));
			Reverse((unsigned char *)&(data->sDBHead.b_Cinfo.b_cIndex),sizeof(data->sDBHead.b_Cinfo.b_cIndex));
			Reverse((unsigned char *)&(data->sDBHead.d_Dinfo.d_nNodeId),sizeof(data->sDBHead.d_Dinfo.d_nNodeId));
			Reverse((unsigned char *)&(data->sDBHead.d_Dinfo.d_nServiceNo),sizeof(data->sDBHead.d_Dinfo.d_nServiceNo));
			Reverse((unsigned char *)&(data->sDBHead.d_Dinfo.d_nSvrMainId),sizeof(data->sDBHead.d_Dinfo.d_nSvrMainId));
			Reverse((unsigned char *)&(data->sDBHead.n_Ninfo.n_nNextNo),sizeof(data->sDBHead.n_Ninfo.n_nNextNo));
			Reverse((unsigned char *)&(data->sDBHead.n_Ninfo.n_nNextOffset),sizeof(data->sDBHead.n_Ninfo.n_nNextOffset));
			Reverse((unsigned char *)&(data->sDBHead.nCrc),sizeof(data->sDBHead.nCrc));
			Reverse((unsigned char *)&(data->sDBHead.nLen),sizeof(data->sDBHead.nLen));
			Reverse((unsigned char *)&(data->sDBHead.s_Sinfo.s_nDrebSerial),sizeof(data->sDBHead.s_Sinfo.s_nDrebSerial));
			Reverse((unsigned char *)&(data->sDBHead.s_Sinfo.s_nHook),sizeof(data->sDBHead.s_Sinfo.s_nHook));
			Reverse((unsigned char *)&(data->sDBHead.s_Sinfo.s_nIndex),sizeof(data->sDBHead.s_Sinfo.s_nIndex));
			Reverse((unsigned char *)&(data->sDBHead.s_Sinfo.s_nNodeId),sizeof(data->sDBHead.s_Sinfo.s_nNodeId));
			Reverse((unsigned char *)&(data->sDBHead.s_Sinfo.s_nSerial),sizeof(data->sDBHead.s_Sinfo.s_nSerial));
			Reverse((unsigned char *)&(data->sDBHead.s_Sinfo.s_nGateIndex),sizeof(data->sDBHead.s_Sinfo.s_nGateIndex));
			Reverse((unsigned char *)&(data->sDBHead.s_Sinfo.s_nSvrMainId),sizeof(data->sDBHead.s_Sinfo.s_nSvrMainId));
			Reverse((unsigned char *)&(data->sDBHead.s_Sinfo.s_nTimestamp),sizeof(data->sDBHead.s_Sinfo.s_nTimestamp));
		}
		else //本机为网络序
		{
			data->sBpcHead.nMsgFlag = RMSGID;
		}
	}
}
void CBpcEndian::Endian2Comm(unsigned char *data,int datalen)
{
	if (m_bUseLittleEndian) //通讯用主机序
	{
		if (!m_bIsLittle) //本机为网络序
		{
			Reverse(data,datalen);
		}
	}
	else //通讯用网络序
	{
		if (m_bIsLittle) //本机为主机序
		{
			Reverse(data,datalen);
		}
	}
}
void CBpcEndian::Reverse(unsigned char *buf, int len)
{
	int i;
	unsigned char buf1[10];
	for(i=0;i<len;i++)
	{
		buf1[i] = buf[len-i-1];
	}
	memcpy(buf,buf1,len);
}

void CBpcEndian::SetCommEndian(bool islittle)
{
	m_bUseLittleEndian = islittle;
}
