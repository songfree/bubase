// DrebEndian.cpp: implementation of the CDrebEndian class.
//
//////////////////////////////////////////////////////////////////////

#include "DrebEndian.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CDrebEndian::CDrebEndian()
{
	m_bUseLittleEndian = true;
	m_bIsLittle = true;   //主机序 linux windows
	if (htonl(1) == 1)
	{
		m_bIsLittle = false;//本机为网络序 AIX HP_UX
	}
}

CDrebEndian::~CDrebEndian()
{

}

void CDrebEndian::Endian2LocalHost(PDREB_HEAD data)
{
	if (m_bUseLittleEndian)
	{
		if (!m_bIsLittle)
		{

			Reverse((unsigned char *)&(data->a_Ainfo.a_nNodeId),sizeof(data->a_Ainfo.a_nNodeId));
			Reverse((unsigned char *)&(data->a_Ainfo.a_nRetCode),sizeof(data->a_Ainfo.a_nRetCode));
			Reverse((unsigned char *)&(data->b_Cinfo.b_nSerial),sizeof(data->b_Cinfo.b_nSerial));
			Reverse((unsigned char *)&(data->b_Cinfo.b_cIndex),sizeof(data->b_Cinfo.b_cIndex));
			Reverse((unsigned char *)&(data->d_Dinfo.d_nNodeId),sizeof(data->d_Dinfo.d_nNodeId));
			Reverse((unsigned char *)&(data->d_Dinfo.d_nServiceNo),sizeof(data->d_Dinfo.d_nServiceNo));
			Reverse((unsigned char *)&(data->d_Dinfo.d_nSvrMainId),sizeof(data->d_Dinfo.d_nSvrMainId));
			Reverse((unsigned char *)&(data->n_Ninfo.n_nNextNo),sizeof(data->n_Ninfo.n_nNextNo));
			Reverse((unsigned char *)&(data->n_Ninfo.n_nNextOffset),sizeof(data->n_Ninfo.n_nNextOffset));
			Reverse((unsigned char *)&(data->nCrc),sizeof(data->nCrc));
			Reverse((unsigned char *)&(data->nLen),sizeof(data->nLen));
			Reverse((unsigned char *)&(data->s_Sinfo.s_nDrebSerial),sizeof(data->s_Sinfo.s_nDrebSerial));
			Reverse((unsigned char *)&(data->s_Sinfo.s_nHook),sizeof(data->s_Sinfo.s_nHook));
			Reverse((unsigned char *)&(data->s_Sinfo.s_nIndex),sizeof(data->s_Sinfo.s_nIndex));
			Reverse((unsigned char *)&(data->s_Sinfo.s_nNodeId),sizeof(data->s_Sinfo.s_nNodeId));
			Reverse((unsigned char *)&(data->s_Sinfo.s_nSerial),sizeof(data->s_Sinfo.s_nSerial));
			Reverse((unsigned char *)&(data->s_Sinfo.s_nGateIndex),sizeof(data->s_Sinfo.s_nGateIndex));
			Reverse((unsigned char *)&(data->s_Sinfo.s_nSvrMainId),sizeof(data->s_Sinfo.s_nSvrMainId));
			Reverse((unsigned char *)&(data->s_Sinfo.s_nTimestamp),sizeof(data->s_Sinfo.s_nTimestamp));
		}
	}
	else
	{
		if (m_bIsLittle)
		{
			
			Reverse((unsigned char *)&(data->a_Ainfo.a_nNodeId),sizeof(data->a_Ainfo.a_nNodeId));
			Reverse((unsigned char *)&(data->a_Ainfo.a_nRetCode),sizeof(data->a_Ainfo.a_nRetCode));
			Reverse((unsigned char *)&(data->b_Cinfo.b_nSerial),sizeof(data->b_Cinfo.b_nSerial));
			Reverse((unsigned char *)&(data->b_Cinfo.b_cIndex),sizeof(data->b_Cinfo.b_cIndex));
			Reverse((unsigned char *)&(data->d_Dinfo.d_nNodeId),sizeof(data->d_Dinfo.d_nNodeId));
			Reverse((unsigned char *)&(data->d_Dinfo.d_nServiceNo),sizeof(data->d_Dinfo.d_nServiceNo));
			Reverse((unsigned char *)&(data->d_Dinfo.d_nSvrMainId),sizeof(data->d_Dinfo.d_nSvrMainId));
			Reverse((unsigned char *)&(data->n_Ninfo.n_nNextNo),sizeof(data->n_Ninfo.n_nNextNo));
			Reverse((unsigned char *)&(data->n_Ninfo.n_nNextOffset),sizeof(data->n_Ninfo.n_nNextOffset));
			Reverse((unsigned char *)&(data->nCrc),sizeof(data->nCrc));
			Reverse((unsigned char *)&(data->nLen),sizeof(data->nLen));
			Reverse((unsigned char *)&(data->s_Sinfo.s_nDrebSerial),sizeof(data->s_Sinfo.s_nDrebSerial));
			Reverse((unsigned char *)&(data->s_Sinfo.s_nHook),sizeof(data->s_Sinfo.s_nHook));
			Reverse((unsigned char *)&(data->s_Sinfo.s_nIndex),sizeof(data->s_Sinfo.s_nIndex));
			Reverse((unsigned char *)&(data->s_Sinfo.s_nNodeId),sizeof(data->s_Sinfo.s_nNodeId));
			Reverse((unsigned char *)&(data->s_Sinfo.s_nSerial),sizeof(data->s_Sinfo.s_nSerial));
			Reverse((unsigned char *)&(data->s_Sinfo.s_nGateIndex),sizeof(data->s_Sinfo.s_nGateIndex));
			Reverse((unsigned char *)&(data->s_Sinfo.s_nSvrMainId),sizeof(data->s_Sinfo.s_nSvrMainId));
			Reverse((unsigned char *)&(data->s_Sinfo.s_nTimestamp),sizeof(data->s_Sinfo.s_nTimestamp));
		}
	}

}

void CDrebEndian::Endian2LocalHost(unsigned char *data,int datalen)
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
void CDrebEndian::Endian2Comm(PDREB_HEAD data)
{
	if (m_bUseLittleEndian) //通讯用主机序
	{
		if (!m_bIsLittle) //本机为网络序
		{
			
			Reverse((unsigned char *)&(data->a_Ainfo.a_nNodeId),sizeof(data->a_Ainfo.a_nNodeId));
			Reverse((unsigned char *)&(data->a_Ainfo.a_nRetCode),sizeof(data->a_Ainfo.a_nRetCode));
			Reverse((unsigned char *)&(data->b_Cinfo.b_nSerial),sizeof(data->b_Cinfo.b_nSerial));
			Reverse((unsigned char *)&(data->b_Cinfo.b_cIndex),sizeof(data->b_Cinfo.b_cIndex));
			Reverse((unsigned char *)&(data->d_Dinfo.d_nNodeId),sizeof(data->d_Dinfo.d_nNodeId));
			Reverse((unsigned char *)&(data->d_Dinfo.d_nServiceNo),sizeof(data->d_Dinfo.d_nServiceNo));
			Reverse((unsigned char *)&(data->d_Dinfo.d_nSvrMainId),sizeof(data->d_Dinfo.d_nSvrMainId));
			Reverse((unsigned char *)&(data->n_Ninfo.n_nNextNo),sizeof(data->n_Ninfo.n_nNextNo));
			Reverse((unsigned char *)&(data->n_Ninfo.n_nNextOffset),sizeof(data->n_Ninfo.n_nNextOffset));
			Reverse((unsigned char *)&(data->nCrc),sizeof(data->nCrc));
			Reverse((unsigned char *)&(data->nLen),sizeof(data->nLen));
			Reverse((unsigned char *)&(data->s_Sinfo.s_nDrebSerial),sizeof(data->s_Sinfo.s_nDrebSerial));
			Reverse((unsigned char *)&(data->s_Sinfo.s_nHook),sizeof(data->s_Sinfo.s_nHook));
			Reverse((unsigned char *)&(data->s_Sinfo.s_nIndex),sizeof(data->s_Sinfo.s_nIndex));
			Reverse((unsigned char *)&(data->s_Sinfo.s_nNodeId),sizeof(data->s_Sinfo.s_nNodeId));
			Reverse((unsigned char *)&(data->s_Sinfo.s_nSerial),sizeof(data->s_Sinfo.s_nSerial));
			Reverse((unsigned char *)&(data->s_Sinfo.s_nGateIndex),sizeof(data->s_Sinfo.s_nGateIndex));
			Reverse((unsigned char *)&(data->s_Sinfo.s_nSvrMainId),sizeof(data->s_Sinfo.s_nSvrMainId));
			Reverse((unsigned char *)&(data->s_Sinfo.s_nTimestamp),sizeof(data->s_Sinfo.s_nTimestamp));
		}
		else //本机为主机序
		{

		}
	}
	else //通讯用网络序
	{
		if (m_bIsLittle) //本机为主机序
		{
	
			Reverse((unsigned char *)&(data->a_Ainfo.a_nNodeId),sizeof(data->a_Ainfo.a_nNodeId));
			Reverse((unsigned char *)&(data->a_Ainfo.a_nRetCode),sizeof(data->a_Ainfo.a_nRetCode));
			Reverse((unsigned char *)&(data->b_Cinfo.b_nSerial),sizeof(data->b_Cinfo.b_nSerial));
			Reverse((unsigned char *)&(data->b_Cinfo.b_cIndex),sizeof(data->b_Cinfo.b_cIndex));
			Reverse((unsigned char *)&(data->d_Dinfo.d_nNodeId),sizeof(data->d_Dinfo.d_nNodeId));
			Reverse((unsigned char *)&(data->d_Dinfo.d_nServiceNo),sizeof(data->d_Dinfo.d_nServiceNo));
			Reverse((unsigned char *)&(data->d_Dinfo.d_nSvrMainId),sizeof(data->d_Dinfo.d_nSvrMainId));
			Reverse((unsigned char *)&(data->n_Ninfo.n_nNextNo),sizeof(data->n_Ninfo.n_nNextNo));
			Reverse((unsigned char *)&(data->n_Ninfo.n_nNextOffset),sizeof(data->n_Ninfo.n_nNextOffset));
			Reverse((unsigned char *)&(data->nCrc),sizeof(data->nCrc));
			Reverse((unsigned char *)&(data->nLen),sizeof(data->nLen));
			Reverse((unsigned char *)&(data->s_Sinfo.s_nDrebSerial),sizeof(data->s_Sinfo.s_nDrebSerial));
			Reverse((unsigned char *)&(data->s_Sinfo.s_nHook),sizeof(data->s_Sinfo.s_nHook));
			Reverse((unsigned char *)&(data->s_Sinfo.s_nIndex),sizeof(data->s_Sinfo.s_nIndex));
			Reverse((unsigned char *)&(data->s_Sinfo.s_nNodeId),sizeof(data->s_Sinfo.s_nNodeId));
			Reverse((unsigned char *)&(data->s_Sinfo.s_nSerial),sizeof(data->s_Sinfo.s_nSerial));
			Reverse((unsigned char *)&(data->s_Sinfo.s_nGateIndex),sizeof(data->s_Sinfo.s_nGateIndex));
			Reverse((unsigned char *)&(data->s_Sinfo.s_nSvrMainId),sizeof(data->s_Sinfo.s_nSvrMainId));
			Reverse((unsigned char *)&(data->s_Sinfo.s_nTimestamp),sizeof(data->s_Sinfo.s_nTimestamp));
		}
		else //本机为网络序
		{

		}
	}
}
void CDrebEndian::Endian2Comm(unsigned char *data,int datalen)
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
void CDrebEndian::Reverse(unsigned char *buf, int len)
{
	int i;
	unsigned char buf1[10];
	for(i=0;i<len;i++)
	{
		buf1[i] = buf[len-i-1];
	}
	memcpy(buf,buf1,len);
}

void CDrebEndian::SetCommEndian(bool islittle)
{
	m_bUseLittleEndian = islittle;
}

unsigned int CDrebEndian::CRC(void *pdata, int datalen)
{
	unsigned short acc = 0, i;
	unsigned char* Data = (unsigned char*) pdata;
	while(datalen--)
	{
		acc = acc ^ (*Data++ << 8);
		for(i = 0; i++ < 8; )
		{
			if(acc & 0x8000)
			{
				acc = (acc << 1) ^ 0x1021;
			}
			else
			{
				acc <<= 1;
			}
		}
	}
	i = ( (unsigned short) ( ((unsigned char)acc) << 8) ) | ((unsigned short)(acc >> 8));
	return i;
}
void CDrebEndian::Endian2CommGenCrc(PDREB_HEAD data)
{
	Endian2Comm(data);
	data->nCrc = 0;
	data->nCrc  = CRC(data,DREBHEADLEN);
	Endian2Comm((unsigned char *)&(data->nCrc),sizeof(data->nCrc));
	
}
bool CDrebEndian::Endian2LocalHostChkCrc(PDREB_HEAD data)
{
	int crc,crc2;
	crc=data->nCrc;
	Endian2LocalHost((unsigned char *)&crc,sizeof(crc));
	data->nCrc=0;
	crc2=CRC(data,DREBHEADLEN);
	if (crc == crc2)
	{
		Endian2LocalHost(data);
		return true;
	}
	return false;
}
// unsigned long long ntohll(unsigned long long val)
// {
//     if (__BYTE_ORDER == __LITTLE_ENDIAN)
//     {
//         return (((unsigned long long )htonl((int)((val << 32) >> 32))) << 32) | (unsigned int)htonl((int)(val >> 32));
//     }
//     else if (__BYTE_ORDER == __BIG_ENDIAN)
//     {
//         return val;
//     }
// }
// 
// unsigned long long htonll(unsigned long long val)
// {
//     if (__BYTE_ORDER == __LITTLE_ENDIAN)
//     {
//         return (((unsigned long long )htonl((int)((val << 32) >> 32))) << 32) | (unsigned int)htonl((int)(val >> 32));
//     }
//     else if (__BYTE_ORDER == __BIG_ENDIAN)
//     {
//         return val;
//     }
// }
