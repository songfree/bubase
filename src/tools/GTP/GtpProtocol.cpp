// GtpProtocol.cpp: implementation of the CGtpProtocol class.
//
//////////////////////////////////////////////////////////////////////

#include "GtpProtocol.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGtpProtocol::CGtpProtocol()
{
	bzero(&m_pGtpSessionHead,sizeof(S_GTP_SESSION));
	m_pLog.SetLogPara(LOG_DEBUG,"","parsegtp.log");
	//GTP通讯使用网络序
	m_pEndian.SetCommEndian(false);
}

CGtpProtocol::~CGtpProtocol()
{

}

void CGtpProtocol::NewGtp(unsigned int randnum, unsigned int logid,char cPackType)
{
	bzero(&m_pGtpSessionHead,sizeof(S_GTP_SESSION));
	m_pGtpSessionHead.head.cPackType = cPackType;
	m_pGtpSessionHead.head.cExtLength = 0;
	m_pGtpSessionHead.head.nLogId = logid;
	m_pGtpSessionHead.head.nRandNum = randnum;
	m_pGtpSessionHead.head.nBodyLen =0;
	m_pGtpData.Clear();
	

	//GTP消息头
	m_pGtpData.SetDataItem("X01","GTP1.0",false);
	m_pGtpData.SetDataItem("X02","0000",false);
	m_pGtpData.SetDataItem("X03",0);
	m_pGtpData.SetDataItem("X04",0);
	m_pGtpData.SetDataItem("X05",0);
	m_pGtpData.SetDataItem("X06","L",false);
	m_pGtpData.SetDataItem("X07",0);
	m_pGtpData.SetDataItem("X08",0);
	m_pGtpData.SetDataItem("X09",0);

}

void CGtpProtocol::SetBodyLen(unsigned short len)
{
	m_pGtpSessionHead.head.nLogId = len;
}

bool CGtpProtocol::SetExtHead(int extlen, char *head)
{
	if (extlen >SESSION_EXTHEAD_LEN)
	{
		return false;
	}
	m_pGtpSessionHead.head.cExtLength = extlen;
	memcpy(m_pGtpSessionHead.sExtInfo,head,extlen);
	return true;
}

void CGtpProtocol::SetHeadInfo(unsigned int randnum, unsigned int logid, char msgtype)
{
	m_pGtpSessionHead.head.cPackType = msgtype;
	m_pGtpSessionHead.head.nLogId = logid;
	m_pGtpSessionHead.head.nRandNum = randnum;
}

CGtpData * CGtpProtocol::GetGtpData()
{
	return &m_pGtpData;
}

int CGtpProtocol::FromGtpBuffer(const char *gtpbuffer, int &buflen)
{
	if (buflen < sizeof(S_GTP_SESSION_HEAD))
	{
		m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"GTP数据非法，长度小于会话头");
		return -1;
	}
	int curgtplen = sizeof(S_GTP_SESSION_HEAD);//会话头长度
	memcpy(&m_pGtpSessionHead.head,gtpbuffer,sizeof(S_GTP_SESSION_HEAD));
	m_pEndian.Endian2LocalHost((unsigned char *)&(m_pGtpSessionHead.head.nRandNum),sizeof(m_pGtpSessionHead.head.nRandNum));
	m_pEndian.Endian2LocalHost((unsigned char *)&(m_pGtpSessionHead.head.nLogId),sizeof(m_pGtpSessionHead.head.nLogId));
	m_pEndian.Endian2LocalHost((unsigned char *)&(m_pGtpSessionHead.head.nBodyLen),sizeof(m_pGtpSessionHead.head.nBodyLen));
	if (m_pGtpSessionHead.head.cExtLength >127)
	{
		m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"GTP数据非法，扩展报文头长度[%d]大于127",m_pGtpSessionHead.head.cExtLength);
		return -1;
	}
	if (m_pGtpSessionHead.head.cExtLength >0)
	{
		memcpy(m_pGtpSessionHead.sExtInfo,gtpbuffer+curgtplen,m_pGtpSessionHead.head.cExtLength);
		curgtplen+=m_pGtpSessionHead.head.cExtLength;//会话头+扩展头长度
	}
	if (m_pGtpSessionHead.head.nBodyLen + curgtplen > buflen)
	{
		m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"GTP数据非法，长度[%d]小于会话头+报文长度[%d]",buflen,m_pGtpSessionHead.head.nBodyLen + curgtplen);
		return -1;
	}
	if (m_pGtpSessionHead.head.nBodyLen <1)
	{
		m_pGtpData.Clear();
		buflen = m_pGtpSessionHead.head.nBodyLen + curgtplen;
		return 0;
	}
	if (m_pGtpSessionHead.head.cPackType == 1)
	{
		char gtpdata[8192];
		bzero(gtpdata,sizeof(gtpdata));
		memcpy(gtpdata,gtpbuffer,m_pGtpSessionHead.head.nBodyLen);
		if (!FromGtpData(gtpdata))
		{
			m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"GTP报文非法 [%s]",gtpdata);
			return -1;
		}
		buflen = m_pGtpSessionHead.head.nBodyLen + curgtplen;
		return 1;
	}
	buflen = m_pGtpSessionHead.head.nBodyLen + curgtplen;
	return m_pGtpSessionHead.head.cPackType;
}



bool CGtpProtocol::FromGtpData(const char *gtpdata)
{
	return m_pGtpData.FromBuffer(gtpdata);
}

bool CGtpProtocol::ToBuffer(char *gtpbuffer, int &buflen)
{
	int datalen = buflen;
	std::string ss;
	if (!m_pGtpData.ToStringDelRec(ss,9))  
	{
		return false;
	}
	char tmpchar[10];
	sprintf(tmpchar,"%04d",ss.length());
	m_pGtpData.SetDataItem("X02",tmpchar,false);
	m_pGtpSessionHead.head.nBodyLen = ss.length();
	if (!m_pGtpData.ToString(ss))  
	{
		return false;
	}
	if (buflen <ss.length()+sizeof(S_GTP_SESSION_HEAD)+m_pGtpSessionHead.head.cExtLength)
	{
		return false;
	}
	if (!ToGtpHeadBuffer(gtpbuffer,datalen))
	{
		return false;
	}
	memcpy(gtpbuffer+datalen,ss.c_str(),ss.length());
	buflen = datalen+ss.length();
	return true;
}
bool CGtpProtocol::ToGtpHeadBuffer(char *gtpbuffer, int &buflen)
{
	if (buflen < sizeof(S_GTP_SESSION_HEAD)+m_pGtpSessionHead.head.cExtLength)
	{
		return false;
	}
	memcpy(gtpbuffer,&m_pGtpSessionHead.head,sizeof(S_GTP_SESSION_HEAD));
	if (m_pGtpSessionHead.head.cExtLength>0)
	{
		memcpy(gtpbuffer+sizeof(S_GTP_SESSION_HEAD),m_pGtpSessionHead.sExtInfo,m_pGtpSessionHead.head.cExtLength);
	}
	S_GTP_SESSION_HEAD *aa = (S_GTP_SESSION_HEAD *)gtpbuffer;
	m_pEndian.Endian2Comm((unsigned char *)&(aa->nRandNum),sizeof(aa->nRandNum));
	m_pEndian.Endian2Comm((unsigned char *)&(aa->nLogId),sizeof(aa->nLogId));
	m_pEndian.Endian2Comm((unsigned char *)&(aa->nBodyLen),sizeof(aa->nBodyLen));
	buflen = sizeof(S_GTP_SESSION_HEAD)+m_pGtpSessionHead.head.cExtLength;
	return true;
}

bool CGtpProtocol::ToGtpDataString(std::string gtpdata)
{
	return m_pGtpData.ToString(gtpdata);
}
