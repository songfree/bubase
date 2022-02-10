// EncryptCommBuffer.cpp: implementation of the CEncryptCommBuffer class.
//
//////////////////////////////////////////////////////////////////////

#include "EncryptCommBuffer.h"



#define  DESENCRYPTKEY    "rd402wms"
#define  DESENCRYPTKEY3   "wms13711songfree"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEncryptCommBuffer::CEncryptCommBuffer()
{
	m_pLog = NULL;
}

CEncryptCommBuffer::~CEncryptCommBuffer()
{
	m_pLog = NULL;
}

void CEncryptCommBuffer::SetLogPara(CIErrlog *log)
{
	m_pLog = log;
}

bool CEncryptCommBuffer::DrebZipDesProcess(PCOMMSTRU data, int destflag)
{
	unsigned short datalen = data->head.nLen;
	if (0 == destflag)
	{
		if ( 0 == data->head.cZip)
		{
			return true;
		}
		if (1 == data->head.cZip)
		{
			//解密
			if (!UnDesOne(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"原数据包标志[%d]，解为明文出错",data->head.cZip);
				return false;
			}
			data->head.nLen = datalen;
			data->head.cZip = destflag;
			return true;
		}
		if (2 == data->head.cZip)
		{
			//解压缩
			if (!UnZip(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"原数据包标志[%d]，解为明文出错",data->head.cZip);
				return false;
			}
			data->head.cZip = destflag;
			data->head.nLen = datalen;
			return true;
		}
		if (3 == data->head.cZip)
		{
			//解密
			if (!UnDesThree(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"原数据包标志[%d]，解为明文出错",data->head.cZip);
				return false;
			}
			data->head.cZip = destflag;
			data->head.nLen = datalen;
			return true;
		}
		if (4 == data->head.cZip)
		{
			//解密
			if (!UnDesOne(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"原数据包标志[%d]，解为压缩报文出错",data->head.cZip);
				return false;
			}
			data->head.cZip = 2;
			data->head.nLen = datalen;
			//解压
			if (!UnZip(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"数据包标志[%d]，解为明文出错",data->head.cZip);
				return false;
			}
			data->head.cZip = destflag;
			data->head.nLen = datalen;
			return true;
		}
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"原数据包标志[%d]不符",data->head.cZip);
		return false;
	}
	if (1 == destflag)
	{
		if ( 0 == data->head.cZip)
		{
			//加密
			if (!DesOne(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"数据包标志[%d]，加密出错",data->head.cZip);
				return false;
			}
			data->head.cZip = destflag;
			data->head.nLen = datalen;
			return true;
		}
		if (1 == data->head.cZip)
		{
			return true;
		}
		if (2 == data->head.cZip)
		{
			//解压缩
			if (!UnZip(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"数据包标志[%d]，解为明文出错",data->head.cZip);
				return false;
			}
			data->head.cZip = 0;
			data->head.nLen = datalen;
			//加密
			if (!DesOne(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"数据包标志[%d]，加密出错",data->head.cZip);
				return false;
			}
			data->head.cZip = destflag;
			data->head.nLen = datalen;
			return true;
		}
		if (3 == data->head.cZip)
		{
			//解密
			if (!UnDesThree(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"数据包标志[%d]，解为明文出错",data->head.cZip);
				return false;
			}
			data->head.cZip = 0;
			data->head.nLen = datalen;
			//加密
			if (!DesOne(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"数据包标志[%d]，加密出错",data->head.cZip);
				return false;
			}
			data->head.cZip = destflag;
			data->head.nLen = datalen;
			return true;
		}
		if (4 == data->head.cZip)
		{
			//解密
			if (!UnDesOne(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"数据包标志[%d]，解为压缩出错",data->head.cZip);
				return false;
			}
			data->head.cZip = 2;
			data->head.nLen = datalen;
			//解压
			if (!UnZip(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"数据包标志[%d]，解为明文出错",data->head.cZip);
				return false;
			}
			data->head.cZip = 0;
			data->head.nLen = datalen;
			//加密
			if (!DesOne(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"数据包标志[%d]，加密出错",data->head.cZip);
				return false;
			}
			data->head.cZip = destflag;
			data->head.nLen = datalen;
			return true;
		}
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"原数据包标志[%d]不符",data->head.cZip);
		return false;
	}
	if (2 == destflag)
	{
		if ( 0 == data->head.cZip)
		{
			//压缩
			if (!Zip(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"数据包标志[%d]，压缩出错",data->head.cZip);
				return false;
			}
			data->head.cZip = destflag;
			data->head.nLen = datalen;
			return true;
		}
		if (1 == data->head.cZip)
		{
			//解密
			if (!UnDesOne(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"数据包标志[%d]，解为明文出错",data->head.cZip);
				return false;
			}
			data->head.cZip = 0;
			data->head.nLen = datalen;
			//压缩
			if (!Zip(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"数据包标志[%d]，压缩出错",data->head.cZip);
				return false;
			}
			data->head.cZip = destflag;
			data->head.nLen = datalen;
			return true;
		}
		if (2 == data->head.cZip)
		{
			return true;
		}
		if (3 == data->head.cZip)
		{
			//解密
			if (!UnDesThree(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"数据包标志[%d]，解为明文出错",data->head.cZip);
				return false;
			}
			data->head.cZip = 0;
			data->head.nLen = datalen;
			//压缩
			if (!Zip(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"数据包标志[%d]，压缩出错",data->head.cZip);
				return false;
			}
			data->head.cZip = destflag;
			data->head.nLen = datalen;
			return true;
		}
		if (4 == data->head.cZip)
		{
			//解密
			if (!UnDesOne(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"数据包标志[%d]，解为压缩出错",data->head.cZip);
				return false;
			}
			data->head.cZip = destflag;
			data->head.nLen = datalen;
			return true;
		}
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"原数据包标志[%d]不符",data->head.cZip);
		return false;
	}
	if (3 == destflag)
	{
		if ( 0 == data->head.cZip)
		{
			//加密
			if (!DesThree(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"数据包标志[%d]，加密出错",data->head.cZip);
				return false;
			}
			data->head.cZip = destflag;
			data->head.nLen = datalen;
			return true;
		}
		if (1 == data->head.cZip)
		{
			//解密
			if (!UnDesOne(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"数据包标志[%d]，解为明文出错",data->head.cZip);
				return false;
			}
			data->head.cZip = 0;
			data->head.nLen = datalen;
			//加密
			if (!DesThree(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"数据包标志[%d]，加密出错",data->head.cZip);
				return false;
			}
			data->head.cZip = destflag;
			data->head.nLen = datalen;
			return true;
		}
		if (2 == data->head.cZip)
		{
			//解压
			if (!UnZip(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"数据包标志[%d]，解为明文出错",data->head.cZip);
				return false;
			}
			data->head.cZip = 0;
			data->head.nLen = datalen;
			//加密
			if (!DesThree(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"数据包标志[%d]，加密出错",data->head.cZip);
				return false;
			}
			data->head.cZip = destflag;
			data->head.nLen = datalen;
			return true;
		}
		if (3 == data->head.cZip)
		{
			return true;
		}
		if (4 == data->head.cZip)
		{
			//解密
			if (!UnDesOne(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"数据包标志[%d]，解为压缩出错",data->head.cZip);
				return false;
			}
			data->head.cZip = 2;
			data->head.nLen = datalen;
			//解压
			if (!UnZip(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"数据包标志[%d]，解为明文出错",data->head.cZip);
				return false;
			}
			data->head.cZip = 0;
			data->head.nLen = datalen;
			//加密
			if (!DesThree(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"数据包标志[%d]，加密出错",data->head.cZip);
				return false;
			}
			data->head.cZip = destflag;
			data->head.nLen = datalen;
			return true;
		}
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"原数据包标志[%d]不符",data->head.cZip);
		return false;
	}
	if (4 == destflag)
	{
		if ( 0 == data->head.cZip)
		{
			//压缩
			if (!Zip(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"数据包标志[%d]，压缩出错",data->head.cZip);
				return false;
			}
			data->head.cZip = 2;
			data->head.nLen = datalen;
			//加密
			if (!DesOne(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"数据包标志[%d]，加密出错",data->head.cZip);
				return false;
			}
			data->head.cZip = destflag;
			data->head.nLen = datalen;
			return true;
		}
		if (1 == data->head.cZip)
		{
			//解密
			if (!UnDesOne(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"数据包标志[%d]，解为明文出错",data->head.cZip);
				return false;
			}
			data->head.cZip = 0;
			data->head.nLen = datalen;
			//压缩
			if (!Zip(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"数据包标志[%d]，压缩出错",data->head.cZip);
				return false;
			}
			data->head.cZip = 2;
			data->head.nLen = datalen;
			//加密
			if (!DesOne(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"数据包标志[%d]，加密出错",data->head.cZip);
				return false;
			}
			data->head.cZip = destflag;
			data->head.nLen = datalen;
			return true;
		}
		if (2 == data->head.cZip)
		{
			//加密
			if (!DesOne(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"数据包标志[%d]，加密出错",data->head.cZip);
				return false;
			}
			data->head.cZip = destflag;
			data->head.nLen = datalen;
			return true;
		}
		if (3 == data->head.cZip)
		{
			//解密
			if (!UnDesThree(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"数据包标志[%d]，解为明文出错",data->head.cZip);
				return false;
			}
			data->head.cZip = 0;
			data->head.nLen = datalen;
			//压缩
			if (!Zip(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"数据包标志[%d]，压缩出错",data->head.cZip);
				return false;
			}
			data->head.cZip = 2;
			data->head.nLen = datalen;
			//加密
			if (!DesOne(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"数据包标志[%d]，加密出错",data->head.cZip);
				return false;
			}
			data->head.cZip = destflag;
			data->head.nLen = datalen;
			return true;
		}
		if (4 == data->head.cZip)
		{
			return true;
		}
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"原数据包标志[%d]不符",data->head.cZip);
		return false;
	}
	m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"目标标志[%d]不符",destflag);
	return false;
}

bool CEncryptCommBuffer::UnDesOne(char *data, unsigned short &datalen)
{
	int deslen = datalen; 
	m_pLog->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"解密前数据",data,datalen);
	if (m_pDes.Uncryptchar(data,deslen,DESENCRYPTKEY)<1)
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"解密出错");
		return false;
	}
	datalen = deslen;
	data[datalen]=0;
	m_pLog->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"解密后数据",data,datalen);
	return true;
}

bool CEncryptCommBuffer::UnZip(char *data, unsigned short &datalen)
{
	m_pLog->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"解压前数据",data,datalen);
	char ZipDataBuf[65534];//压缩buf
	bzero(ZipDataBuf,sizeof(ZipDataBuf));
	
	unsigned int ziplen = 65534;
	if (!CBF_Tools::Uncompress((unsigned char *)(ZipDataBuf),ziplen,(unsigned char *)data,datalen))
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"解压缩出错");
		return false;
	}
	memcpy(data,ZipDataBuf,ziplen);
	datalen = ziplen;
	data[datalen]=0;
	m_pLog->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"解压后数据",data,datalen);
	return true;
}

bool CEncryptCommBuffer::DesOne(char *data, unsigned short &datalen)
{
	m_pLog->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"加密前数据",data,datalen);
	int deslen = datalen;
	if (m_pDes.Encryptchar(data,deslen,DESENCRYPTKEY) <1 )
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"加密出错");
		return false;
	}
	datalen = deslen;
	data[datalen]=0;
	m_pLog->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"加密后数据",data,datalen);
	return true;
}

bool CEncryptCommBuffer::DesThree(char *data, unsigned short &datalen)
{
	m_pLog->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"加密前数据",data,datalen);
	int deslen = datalen;
	if (m_pDes.Encryptchar(data,deslen,DESENCRYPTKEY3) <1 )
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"加密出错");
		return false;
	}
	datalen = deslen;
	data[datalen]=0;
	m_pLog->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"加密后数据",data,datalen);
	return true;
}

bool CEncryptCommBuffer::UnDesThree(char *data, unsigned short &datalen)
{
	m_pLog->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"解密前数据",data,datalen);
	int deslen = datalen;
	if (m_pDes.Uncryptchar(data,deslen,DESENCRYPTKEY3)<1)
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"解密出错");
		return false;
	}
	datalen = deslen;
	data[datalen]=0;
	m_pLog->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"解密后数据",data,datalen);
	return true;
}

bool CEncryptCommBuffer::Zip(char *data, unsigned short &datalen)
{
	m_pLog->LogBin(LOG_DEBUG,__FILE__,__LINE__,"压缩前数据",data,datalen);
	char ZipDataBuf[65536];//压缩buf
	bzero(ZipDataBuf,sizeof(ZipDataBuf));
	unsigned int ziplen = 65534;
	if (!CBF_Tools::Compress((unsigned char *)(ZipDataBuf),ziplen,(unsigned char *)(data),datalen))
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"数据压缩失败");
		return false;
	}
	memcpy(data,ZipDataBuf,ziplen);
	datalen = ziplen;
	data[datalen]=0;
	m_pLog->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"压缩后数据",data,datalen);
	return true;
}
