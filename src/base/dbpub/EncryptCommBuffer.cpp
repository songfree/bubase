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
			//����
			if (!UnDesOne(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"ԭ���ݰ���־[%d]����Ϊ���ĳ���",data->head.cZip);
				return false;
			}
			data->head.nLen = datalen;
			data->head.cZip = destflag;
			return true;
		}
		if (2 == data->head.cZip)
		{
			//��ѹ��
			if (!UnZip(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"ԭ���ݰ���־[%d]����Ϊ���ĳ���",data->head.cZip);
				return false;
			}
			data->head.cZip = destflag;
			data->head.nLen = datalen;
			return true;
		}
		if (3 == data->head.cZip)
		{
			//����
			if (!UnDesThree(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"ԭ���ݰ���־[%d]����Ϊ���ĳ���",data->head.cZip);
				return false;
			}
			data->head.cZip = destflag;
			data->head.nLen = datalen;
			return true;
		}
		if (4 == data->head.cZip)
		{
			//����
			if (!UnDesOne(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"ԭ���ݰ���־[%d]����Ϊѹ�����ĳ���",data->head.cZip);
				return false;
			}
			data->head.cZip = 2;
			data->head.nLen = datalen;
			//��ѹ
			if (!UnZip(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���ݰ���־[%d]����Ϊ���ĳ���",data->head.cZip);
				return false;
			}
			data->head.cZip = destflag;
			data->head.nLen = datalen;
			return true;
		}
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"ԭ���ݰ���־[%d]����",data->head.cZip);
		return false;
	}
	if (1 == destflag)
	{
		if ( 0 == data->head.cZip)
		{
			//����
			if (!DesOne(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���ݰ���־[%d]�����ܳ���",data->head.cZip);
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
			//��ѹ��
			if (!UnZip(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���ݰ���־[%d]����Ϊ���ĳ���",data->head.cZip);
				return false;
			}
			data->head.cZip = 0;
			data->head.nLen = datalen;
			//����
			if (!DesOne(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���ݰ���־[%d]�����ܳ���",data->head.cZip);
				return false;
			}
			data->head.cZip = destflag;
			data->head.nLen = datalen;
			return true;
		}
		if (3 == data->head.cZip)
		{
			//����
			if (!UnDesThree(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���ݰ���־[%d]����Ϊ���ĳ���",data->head.cZip);
				return false;
			}
			data->head.cZip = 0;
			data->head.nLen = datalen;
			//����
			if (!DesOne(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���ݰ���־[%d]�����ܳ���",data->head.cZip);
				return false;
			}
			data->head.cZip = destflag;
			data->head.nLen = datalen;
			return true;
		}
		if (4 == data->head.cZip)
		{
			//����
			if (!UnDesOne(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���ݰ���־[%d]����Ϊѹ������",data->head.cZip);
				return false;
			}
			data->head.cZip = 2;
			data->head.nLen = datalen;
			//��ѹ
			if (!UnZip(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���ݰ���־[%d]����Ϊ���ĳ���",data->head.cZip);
				return false;
			}
			data->head.cZip = 0;
			data->head.nLen = datalen;
			//����
			if (!DesOne(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���ݰ���־[%d]�����ܳ���",data->head.cZip);
				return false;
			}
			data->head.cZip = destflag;
			data->head.nLen = datalen;
			return true;
		}
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"ԭ���ݰ���־[%d]����",data->head.cZip);
		return false;
	}
	if (2 == destflag)
	{
		if ( 0 == data->head.cZip)
		{
			//ѹ��
			if (!Zip(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���ݰ���־[%d]��ѹ������",data->head.cZip);
				return false;
			}
			data->head.cZip = destflag;
			data->head.nLen = datalen;
			return true;
		}
		if (1 == data->head.cZip)
		{
			//����
			if (!UnDesOne(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���ݰ���־[%d]����Ϊ���ĳ���",data->head.cZip);
				return false;
			}
			data->head.cZip = 0;
			data->head.nLen = datalen;
			//ѹ��
			if (!Zip(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���ݰ���־[%d]��ѹ������",data->head.cZip);
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
			//����
			if (!UnDesThree(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���ݰ���־[%d]����Ϊ���ĳ���",data->head.cZip);
				return false;
			}
			data->head.cZip = 0;
			data->head.nLen = datalen;
			//ѹ��
			if (!Zip(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���ݰ���־[%d]��ѹ������",data->head.cZip);
				return false;
			}
			data->head.cZip = destflag;
			data->head.nLen = datalen;
			return true;
		}
		if (4 == data->head.cZip)
		{
			//����
			if (!UnDesOne(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���ݰ���־[%d]����Ϊѹ������",data->head.cZip);
				return false;
			}
			data->head.cZip = destflag;
			data->head.nLen = datalen;
			return true;
		}
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"ԭ���ݰ���־[%d]����",data->head.cZip);
		return false;
	}
	if (3 == destflag)
	{
		if ( 0 == data->head.cZip)
		{
			//����
			if (!DesThree(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���ݰ���־[%d]�����ܳ���",data->head.cZip);
				return false;
			}
			data->head.cZip = destflag;
			data->head.nLen = datalen;
			return true;
		}
		if (1 == data->head.cZip)
		{
			//����
			if (!UnDesOne(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���ݰ���־[%d]����Ϊ���ĳ���",data->head.cZip);
				return false;
			}
			data->head.cZip = 0;
			data->head.nLen = datalen;
			//����
			if (!DesThree(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���ݰ���־[%d]�����ܳ���",data->head.cZip);
				return false;
			}
			data->head.cZip = destflag;
			data->head.nLen = datalen;
			return true;
		}
		if (2 == data->head.cZip)
		{
			//��ѹ
			if (!UnZip(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���ݰ���־[%d]����Ϊ���ĳ���",data->head.cZip);
				return false;
			}
			data->head.cZip = 0;
			data->head.nLen = datalen;
			//����
			if (!DesThree(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���ݰ���־[%d]�����ܳ���",data->head.cZip);
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
			//����
			if (!UnDesOne(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���ݰ���־[%d]����Ϊѹ������",data->head.cZip);
				return false;
			}
			data->head.cZip = 2;
			data->head.nLen = datalen;
			//��ѹ
			if (!UnZip(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���ݰ���־[%d]����Ϊ���ĳ���",data->head.cZip);
				return false;
			}
			data->head.cZip = 0;
			data->head.nLen = datalen;
			//����
			if (!DesThree(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���ݰ���־[%d]�����ܳ���",data->head.cZip);
				return false;
			}
			data->head.cZip = destflag;
			data->head.nLen = datalen;
			return true;
		}
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"ԭ���ݰ���־[%d]����",data->head.cZip);
		return false;
	}
	if (4 == destflag)
	{
		if ( 0 == data->head.cZip)
		{
			//ѹ��
			if (!Zip(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���ݰ���־[%d]��ѹ������",data->head.cZip);
				return false;
			}
			data->head.cZip = 2;
			data->head.nLen = datalen;
			//����
			if (!DesOne(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���ݰ���־[%d]�����ܳ���",data->head.cZip);
				return false;
			}
			data->head.cZip = destflag;
			data->head.nLen = datalen;
			return true;
		}
		if (1 == data->head.cZip)
		{
			//����
			if (!UnDesOne(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���ݰ���־[%d]����Ϊ���ĳ���",data->head.cZip);
				return false;
			}
			data->head.cZip = 0;
			data->head.nLen = datalen;
			//ѹ��
			if (!Zip(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���ݰ���־[%d]��ѹ������",data->head.cZip);
				return false;
			}
			data->head.cZip = 2;
			data->head.nLen = datalen;
			//����
			if (!DesOne(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���ݰ���־[%d]�����ܳ���",data->head.cZip);
				return false;
			}
			data->head.cZip = destflag;
			data->head.nLen = datalen;
			return true;
		}
		if (2 == data->head.cZip)
		{
			//����
			if (!DesOne(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���ݰ���־[%d]�����ܳ���",data->head.cZip);
				return false;
			}
			data->head.cZip = destflag;
			data->head.nLen = datalen;
			return true;
		}
		if (3 == data->head.cZip)
		{
			//����
			if (!UnDesThree(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���ݰ���־[%d]����Ϊ���ĳ���",data->head.cZip);
				return false;
			}
			data->head.cZip = 0;
			data->head.nLen = datalen;
			//ѹ��
			if (!Zip(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���ݰ���־[%d]��ѹ������",data->head.cZip);
				return false;
			}
			data->head.cZip = 2;
			data->head.nLen = datalen;
			//����
			if (!DesOne(data->buffer,datalen))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���ݰ���־[%d]�����ܳ���",data->head.cZip);
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
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"ԭ���ݰ���־[%d]����",data->head.cZip);
		return false;
	}
	m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"Ŀ���־[%d]����",destflag);
	return false;
}

bool CEncryptCommBuffer::UnDesOne(char *data, unsigned short &datalen)
{
	int deslen = datalen; 
	m_pLog->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"����ǰ����",data,datalen);
	if (m_pDes.Uncryptchar(data,deslen,DESENCRYPTKEY)<1)
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ���");
		return false;
	}
	datalen = deslen;
	data[datalen]=0;
	m_pLog->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"���ܺ�����",data,datalen);
	return true;
}

bool CEncryptCommBuffer::UnZip(char *data, unsigned short &datalen)
{
	m_pLog->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"��ѹǰ����",data,datalen);
	char ZipDataBuf[65534];//ѹ��buf
	bzero(ZipDataBuf,sizeof(ZipDataBuf));
	
	unsigned int ziplen = 65534;
	if (!CBF_Tools::Uncompress((unsigned char *)(ZipDataBuf),ziplen,(unsigned char *)data,datalen))
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"��ѹ������");
		return false;
	}
	memcpy(data,ZipDataBuf,ziplen);
	datalen = ziplen;
	data[datalen]=0;
	m_pLog->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"��ѹ������",data,datalen);
	return true;
}

bool CEncryptCommBuffer::DesOne(char *data, unsigned short &datalen)
{
	m_pLog->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"����ǰ����",data,datalen);
	int deslen = datalen;
	if (m_pDes.Encryptchar(data,deslen,DESENCRYPTKEY) <1 )
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ���");
		return false;
	}
	datalen = deslen;
	data[datalen]=0;
	m_pLog->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"���ܺ�����",data,datalen);
	return true;
}

bool CEncryptCommBuffer::DesThree(char *data, unsigned short &datalen)
{
	m_pLog->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"����ǰ����",data,datalen);
	int deslen = datalen;
	if (m_pDes.Encryptchar(data,deslen,DESENCRYPTKEY3) <1 )
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ���");
		return false;
	}
	datalen = deslen;
	data[datalen]=0;
	m_pLog->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"���ܺ�����",data,datalen);
	return true;
}

bool CEncryptCommBuffer::UnDesThree(char *data, unsigned short &datalen)
{
	m_pLog->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"����ǰ����",data,datalen);
	int deslen = datalen;
	if (m_pDes.Uncryptchar(data,deslen,DESENCRYPTKEY3)<1)
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ���");
		return false;
	}
	datalen = deslen;
	data[datalen]=0;
	m_pLog->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"���ܺ�����",data,datalen);
	return true;
}

bool CEncryptCommBuffer::Zip(char *data, unsigned short &datalen)
{
	m_pLog->LogBin(LOG_DEBUG,__FILE__,__LINE__,"ѹ��ǰ����",data,datalen);
	char ZipDataBuf[65536];//ѹ��buf
	bzero(ZipDataBuf,sizeof(ZipDataBuf));
	unsigned int ziplen = 65534;
	if (!CBF_Tools::Compress((unsigned char *)(ZipDataBuf),ziplen,(unsigned char *)(data),datalen))
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"����ѹ��ʧ��");
		return false;
	}
	memcpy(data,ZipDataBuf,ziplen);
	datalen = ziplen;
	data[datalen]=0;
	m_pLog->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"ѹ��������",data,datalen);
	return true;
}
