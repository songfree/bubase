// Xdp.cpp: implementation of the CXdp class.
//
//////////////////////////////////////////////////////////////////////

#include "Xdp.h"
#include "XdpApi.h"


//��c#���õ�ȫ�ֱ���
std::vector<CXdp *> g_XdpArray;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXdp::CXdp()
{
	m_nRecCount = 1;	
}

CXdp::~CXdp()
{
	for (int i=0; i< m_vNextRecord.size() ; i++ )
	{
		CXdpRecord *ss = m_vNextRecord[i];
		delete  ss;
		ss = NULL;
		m_vNextRecord[i] = NULL;
	}
}

bool CXdp::InitXdp(const char *datafile,char *errmsg)
{
	ResetData();
	if (!m_pXdpFmt.Init(datafile,errmsg))
	{
		return false;
	}
	return	m_pFirstRecord.Init(&m_pXdpFmt);
}

bool CXdp::FromBuffer(const char *xdpbuf,unsigned int xdpbuflen,char *errmsg)
{
	if (xdpbuflen <3)
	{
		sprintf(errmsg,"���ĳ���%d̫С",xdpbuflen);
		return false;
	}
	ResetData();
	//ǰ�����ֽ�Ϊ��¼��+�汾����
	m_nRecCount = (unsigned char )xdpbuf[0];
	if (m_nRecCount <1)
	{
		sprintf(errmsg,"��¼��Ϊ0���Ĳ���");
		return false;
	}
	if (m_pXdpFmt.GetVersion() != (unsigned char )xdpbuf[1])
	{
		sprintf(errmsg,"�������XDP�汾[%d]������ӦΪ[%d]",(unsigned char )xdpbuf[1],m_pXdpFmt.GetVersion());
		m_nRecCount = 1;
		return false;	
	}

	if (!m_pFirstRecord.FromBuffer(xdpbuf+2,xdpbuflen-2,errmsg))
	{
		m_nRecCount = 1;
		return false;
	}
	int newnum = (int )m_nRecCount -m_vNextRecord.size();
	if (newnum >1) //û���㹻�ļ�¼��
	{
		for (int j=1 ; j < newnum ; j++)
		{
			CXdpRecord *record = new CXdpRecord();
			if (NULL == record)
			{
				return false;
			}
			if (!record->Init(&m_pXdpFmt))
			{
				return false;
			}
			m_vNextRecord.push_back(record);
		}
	}
	//�ӵڶ�����¼����ʼ
	int offset=2;
	offset+=m_pFirstRecord.GetRealLen();
	//
	for (unsigned short i=1 ; i<m_nRecCount ; i++)
	{
		if (!m_vNextRecord[i-1]->FromBuffer(xdpbuf+offset,xdpbuflen-offset,errmsg))
		{
			m_nRecCount = 1;
			return false;
		}
		offset +=m_vNextRecord[i-1]->GetRealLen();
	}
// 	//������Ķ��¼��λͼ��Ϊ��
// 	newnum = m_vNextRecord.size()-m_nRecCount+1;
// 	if (newnum>0)
// 	{
// 		for (int k=0;k<newnum;k++)
// 		{
// 			m_vNextRecord[m_nRecCount+k-1]->ResetData();
// 		}
// 	}
	return true;
}

bool CXdp::ToBuffer(char *xdpbuf,unsigned int &len,char *errmsg)
{
	xdpbuf[0] = m_nRecCount;
	xdpbuf[1] = m_pXdpFmt.GetVersion();
	if (m_vNextRecord.size()<m_nRecCount-1)
	{
		return false;
	}
	unsigned int datalen = 2;
	unsigned int tmplen= len-datalen;
	char *pData = xdpbuf+datalen;//ָ�򻺳���
	if (!m_pFirstRecord.ToBuffer(pData,tmplen,errmsg))
	{
		return false;
	}
	
	pData+=tmplen;//ָ��հ�
	
	datalen += tmplen;
	tmplen = len - datalen;
	for (int i=0 ; i< (int)m_nRecCount-1; i++)
	{
		if (!m_vNextRecord[i]->ToBuffer(pData,tmplen,errmsg))
		{
			return false;
		}
		pData+=tmplen;//ָ��հ�
		datalen += tmplen;
		tmplen = len - datalen;
	}
	len = datalen;
	return true;
}


void CXdp::ResetData()
{
	m_pFirstRecord.ResetData();
	m_nRecCount = 1;
	for (int i=0; i<m_vNextRecord.size(); i++)
	{
		m_vNextRecord[i]->ResetData();
	}
}

bool CXdp::SetCurRecCount(unsigned short x)
{
	if (x > m_vNextRecord.size()+1)
	{
		return false;
	}
	m_nRecCount = x;
	return true;
}
bool CXdp::GetFieldValue(unsigned short rec,unsigned short index,int &idata,char *errmsg)
{
	if (0 == rec)
	{
		return false;
	}
	if (1 == rec)
	{
		return GetFieldValue(index,idata,errmsg);
	}
	if (rec > m_nRecCount)
	{
		sprintf(errmsg,"û�д˼�¼");
		return false;
	}
	if (rec-1 > m_vNextRecord.size())
	{
		sprintf(errmsg,"û�д˼�¼��m_nRecCount���󣬱�����¼�ṹ��Ŀ����");
		return false;
	}
	return m_vNextRecord[rec-2]->GetFieldData(index,idata,errmsg);
}
bool CXdp::GetFieldValue(unsigned short rec,unsigned short index,unsigned int &idata,char *errmsg)
{
	if (0 == rec)
	{
		return false;
	}
	if (1 == rec)
	{
		return GetFieldValue(index,idata,errmsg);
	}
	if (rec > m_nRecCount)
	{
		sprintf(errmsg,"û�д˼�¼");
		return false;
	}
	if (rec-1 > m_vNextRecord.size())
	{
		sprintf(errmsg,"û�д˼�¼��m_nRecCount���󣬱�����¼�ṹ��Ŀ����");
		return false;
	}
	return m_vNextRecord[rec-2]->GetFieldData(index,idata,errmsg);
}
bool CXdp::GetFieldValue(unsigned short rec,unsigned short index,short &idata,char *errmsg)
{
	if (0 == rec)
	{
		return false;
	}
	if (1 == rec)
	{
		return GetFieldValue(index,idata,errmsg);
	}
	if (rec > m_nRecCount)
	{
		sprintf(errmsg,"û�д˼�¼");
		return false;
	}
	if (rec-1 > m_vNextRecord.size())
	{
		sprintf(errmsg,"û�д˼�¼��m_nRecCount���󣬱�����¼�ṹ��Ŀ����");
		return false;
	}
	return m_vNextRecord[rec-2]->GetFieldData(index,idata,errmsg);
}
bool CXdp::GetFieldValue(unsigned short rec,unsigned short index,unsigned short &idata,char *errmsg)
{
	if (0 == rec)
	{
		return false;
	}
	if (1 == rec)
	{
		return GetFieldValue(index,idata,errmsg);
	}
	if (rec > m_nRecCount)
	{
		sprintf(errmsg,"û�д˼�¼");
		return false;
	}
	if (rec-1 > m_vNextRecord.size())
	{
		sprintf(errmsg,"û�д˼�¼��m_nRecCount���󣬱�����¼�ṹ��Ŀ����");
		return false;
	}
	return m_vNextRecord[rec-2]->GetFieldData(index,idata,errmsg);
}
bool CXdp::GetFieldValue(unsigned short rec,unsigned short index,double &idata,char *errmsg)
{
	if (0 == rec)
	{
		return false;
	}
	if (1 == rec)
	{
		return GetFieldValue(index,idata,errmsg);
	}
	if (rec > m_nRecCount)
	{
		sprintf(errmsg,"û�д˼�¼");
		return false;
	}
	if (rec-1 > m_vNextRecord.size())
	{
		sprintf(errmsg,"û�д˼�¼��m_nRecCount���󣬱�����¼�ṹ��Ŀ����");
		return false;
	}
	return m_vNextRecord[rec-2]->GetFieldData(index,idata,errmsg);
}
bool CXdp::GetFieldValue(unsigned short rec, unsigned short index, INT64_& idata, char* errmsg)
{
    if (0 == rec)
    {
        return false;
    }
    if (1 == rec)
    {
        return GetFieldValue(index, idata, errmsg);
    }
    if (rec > m_nRecCount)
    {
        sprintf(errmsg, "û�д˼�¼");
        return false;
    }
    if (rec - 1 > m_vNextRecord.size())
    {
        sprintf(errmsg, "û�д˼�¼��m_nRecCount���󣬱�����¼�ṹ��Ŀ����");
        return false;
    }
    return m_vNextRecord[rec - 2]->GetFieldData(index, idata, errmsg);
}
bool CXdp::GetFieldValue(unsigned short rec, unsigned short index, UINT64_& idata, char* errmsg)
{
    if (0 == rec)
    {
        return false;
    }
    if (1 == rec)
    {
        return GetFieldValue(index, idata, errmsg);
    }
    if (rec > m_nRecCount)
    {
        sprintf(errmsg, "û�д˼�¼");
        return false;
    }
    if (rec - 1 > m_vNextRecord.size())
    {
        sprintf(errmsg, "û�д˼�¼��m_nRecCount���󣬱�����¼�ṹ��Ŀ����");
        return false;
    }
    return m_vNextRecord[rec - 2]->GetFieldData(index, idata, errmsg);
}
bool CXdp::GetFieldValue(unsigned short rec, unsigned short index, char *data, unsigned int &datalen,int &fieldtype, char *errmsg)
{
	if (0 == rec)
	{
		return false;
	}
	if (1 == rec)
	{
		return GetFieldValue(index,data,datalen,fieldtype,errmsg);
	}
	if (rec > m_nRecCount)
	{
		sprintf(errmsg,"û�д˼�¼");
		return false;
	}
	if (rec-1 > m_vNextRecord.size())
	{
		sprintf(errmsg,"û�д˼�¼��m_nRecCount���󣬱�����¼�ṹ��Ŀ����");
		return false;
	}
	return m_vNextRecord[rec-2]->GetFieldData(index,data,datalen,fieldtype,errmsg);
}
bool CXdp::GetFieldValue(unsigned short rec,const char *fieldname,int &idata,char *errmsg)
{
	if (0 == rec)
	{
		return false;
	}
	if (1 == rec)
	{
		return GetFieldValue(fieldname,idata,errmsg);
	}
	if (rec > m_nRecCount)
	{
		sprintf(errmsg,"û�д˼�¼");
		return false;
	}
	if (rec-1 > m_vNextRecord.size())
	{
		sprintf(errmsg,"û�д˼�¼��m_nRecCount���󣬱�����¼�ṹ��Ŀ����");
		return false;
	}
	return m_vNextRecord[rec-2]->GetFieldData(fieldname,idata,errmsg);
}
bool CXdp::GetFieldValue(unsigned short rec,const char *fieldname,unsigned int &idata,char *errmsg)
{
	if (0 == rec)
	{
		return false;
	}
	if (1 == rec)
	{
		return GetFieldValue(fieldname,idata,errmsg);
	}
	if (rec > m_nRecCount)
	{
		sprintf(errmsg,"û�д˼�¼");
		return false;
	}
	if (rec-1 > m_vNextRecord.size())
	{
		sprintf(errmsg,"û�д˼�¼��m_nRecCount���󣬱�����¼�ṹ��Ŀ����");
		return false;
	}
	return m_vNextRecord[rec-2]->GetFieldData(fieldname,idata,errmsg);
}
bool CXdp::GetFieldValue(unsigned short rec,const char *fieldname,short &idata,char *errmsg)
{
	if (0 == rec)
	{
		return false;
	}
	if (1 == rec)
	{
		return GetFieldValue(fieldname,idata,errmsg);
	}
	if (rec > m_nRecCount)
	{
		sprintf(errmsg,"û�д˼�¼");
		return false;
	}
	if (rec-1 > m_vNextRecord.size())
	{
		sprintf(errmsg,"û�д˼�¼��m_nRecCount���󣬱�����¼�ṹ��Ŀ����");
		return false;
	}
	return m_vNextRecord[rec-2]->GetFieldData(fieldname,idata,errmsg);
}
bool CXdp::GetFieldValue(unsigned short rec,const char *fieldname,unsigned short &idata,char *errmsg)
{
	if (0 == rec)
	{
		return false;
	}
	if (1 == rec)
	{
		return GetFieldValue(fieldname,idata,errmsg);
	}
	if (rec > m_nRecCount)
	{
		sprintf(errmsg,"û�д˼�¼");
		return false;
	}
	if (rec-1 > m_vNextRecord.size())
	{
		sprintf(errmsg,"û�д˼�¼��m_nRecCount���󣬱�����¼�ṹ��Ŀ����");
		return false;
	}
	return m_vNextRecord[rec-2]->GetFieldData(fieldname,idata,errmsg);
}
bool CXdp::GetFieldValue(unsigned short rec,const char *fieldname,double &idata,char *errmsg)
{
	if (0 == rec)
	{
		return false;
	}
	if (1 == rec)
	{
		return GetFieldValue(fieldname,idata,errmsg);
	}
	if (rec > m_nRecCount)
	{
		sprintf(errmsg,"û�д˼�¼");
		return false;
	}
	if (rec-1 > m_vNextRecord.size())
	{
		sprintf(errmsg,"û�д˼�¼��m_nRecCount���󣬱�����¼�ṹ��Ŀ����");
		return false;
	}
	return m_vNextRecord[rec-2]->GetFieldData(fieldname,idata,errmsg);
}

bool CXdp::GetFieldValue(unsigned short rec, const char* fieldname, INT64_& idata, char* errmsg)
{
    if (0 == rec)
    {
        return false;
    }
    if (1 == rec)
    {
        return GetFieldValue(fieldname, idata, errmsg);
    }
    if (rec > m_nRecCount)
    {
        sprintf(errmsg, "û�д˼�¼");
        return false;
    }
    if (rec - 1 > m_vNextRecord.size())
    {
        sprintf(errmsg, "û�д˼�¼��m_nRecCount���󣬱�����¼�ṹ��Ŀ����");
        return false;
    }
    return m_vNextRecord[rec - 2]->GetFieldData(fieldname, idata, errmsg);
}
bool CXdp::GetFieldValue(unsigned short rec, const char* fieldname, UINT64_& idata, char* errmsg)
{
    if (0 == rec)
    {
        return false;
    }
    if (1 == rec)
    {
        return GetFieldValue(fieldname, idata, errmsg);
    }
    if (rec > m_nRecCount)
    {
        sprintf(errmsg, "û�д˼�¼");
        return false;
    }
    if (rec - 1 > m_vNextRecord.size())
    {
        sprintf(errmsg, "û�д˼�¼��m_nRecCount���󣬱�����¼�ṹ��Ŀ����");
        return false;
    }
    return m_vNextRecord[rec - 2]->GetFieldData(fieldname, idata, errmsg);
}
bool CXdp::GetFieldValue(unsigned short rec, const char *fieldname, char *data,unsigned int &datalen,int &fieldtype, char *errmsg)
{
	if (0 == rec)
	{
		return false;
	}
	if (1 == rec)
	{
		return GetFieldValue(fieldname,data,datalen,fieldtype,errmsg);
	}
	if (rec > m_nRecCount)
	{
		sprintf(errmsg,"û�д˼�¼");
		return false;
	}
	if (rec-1 > m_vNextRecord.size())
	{
		sprintf(errmsg,"û�д˼�¼��m_nRecCount���󣬱�����¼�ṹ��Ŀ����");
		return false;
	}
	return m_vNextRecord[rec-2]->GetFieldData(fieldname,data,datalen,fieldtype,errmsg);
}
bool CXdp::GetFieldValue(unsigned short index,int &idata,char *errmsg)
{
	return m_pFirstRecord.GetFieldData(index,idata,errmsg);
}
bool CXdp::GetFieldValue(unsigned short index,unsigned int &idata,char *errmsg)
{
	return m_pFirstRecord.GetFieldData(index,idata,errmsg);
}
bool CXdp::GetFieldValue(unsigned short index,short &idata,char *errmsg)
{
	return m_pFirstRecord.GetFieldData(index,idata,errmsg);
}
bool CXdp::GetFieldValue(unsigned short index,unsigned short &idata,char *errmsg)
{
	return m_pFirstRecord.GetFieldData(index,idata,errmsg);
}
bool CXdp::GetFieldValue(unsigned short index,double &idata,char *errmsg)
{
	return m_pFirstRecord.GetFieldData(index,idata,errmsg);
}
bool CXdp::GetFieldValue(unsigned short index, INT64_& idata, char* errmsg)
{
    return m_pFirstRecord.GetFieldData(index, idata, errmsg);
}
bool CXdp::GetFieldValue(unsigned short index, UINT64_& idata, char* errmsg)
{
    return m_pFirstRecord.GetFieldData(index, idata, errmsg);
}
bool CXdp::GetFieldValue(unsigned short index, char *data,unsigned int &datalen,int &fieldtype, char *errmsg)
{
	return m_pFirstRecord.GetFieldData(index,data,datalen,fieldtype,errmsg);
}


bool CXdp::GetFieldValue(const char *fieldname,int &idata,char *errmsg)
{
	return m_pFirstRecord.GetFieldData(fieldname,idata,errmsg);
}
bool CXdp::GetFieldValue(const char *fieldname,unsigned int &idata,char *errmsg)
{
	return m_pFirstRecord.GetFieldData(fieldname,idata,errmsg);
}
bool CXdp::GetFieldValue(const char *fieldname,short &idata,char *errmsg)
{
	return m_pFirstRecord.GetFieldData(fieldname,idata,errmsg);
}
bool CXdp::GetFieldValue(const char *fieldname,unsigned short &idata,char *errmsg)
{
	return m_pFirstRecord.GetFieldData(fieldname,idata,errmsg);
}
bool CXdp::GetFieldValue(const char *fieldname,double &idata,char *errmsg)
{
	return m_pFirstRecord.GetFieldData(fieldname,idata,errmsg);
}
bool CXdp::GetFieldValue(const char* fieldname, INT64_& idata, char* errmsg)
{
    return m_pFirstRecord.GetFieldData(fieldname, idata, errmsg);
}
bool CXdp::GetFieldValue(const char* fieldname, UINT64_& idata, char* errmsg)
{
    return m_pFirstRecord.GetFieldData(fieldname, idata, errmsg);
}
bool CXdp::GetFieldValue(const char *fieldname, char *data,unsigned int &datalen,int &fieldtype, char *errmsg)
{
	return m_pFirstRecord.GetFieldData(fieldname,data,datalen,fieldtype,errmsg);
}

unsigned short CXdp::GetRecCount()
{
	return m_nRecCount;
}

unsigned short CXdp::GetFieldType(unsigned short index)
{
	S_XDP_FIELD_DEFINE *field = m_pXdpFmt.At(index);
	if (field == NULL)
	{
		return XDP_UNKNOW;
	}
	return field->f_type;
}
unsigned short CXdp::GetFieldType(const char *fieldname)
{
	S_XDP_FIELD_DEFINE *field = m_pXdpFmt.GetField(fieldname);
	if (field == NULL)
	{
		return XDP_UNKNOW;
	}
	return field->f_type;
}
bool CXdp::SetFieldValueM(unsigned short rec,const char *fieldname,int idata,char *errmsg)
{
	if (0 == rec)
	{
		return false;
	}
	if (1 == rec)
	{
		return SetFieldValue(fieldname,idata,errmsg);
	}
	if (rec > 255)
	{
		sprintf(errmsg,"���255����¼");
		return false;
	}
	if (rec > m_vNextRecord.size()+2)
	{
		sprintf(errmsg,"���Ӽ�¼ʱҪ��˳������");
		return false;
	}
	if (m_vNextRecord.size() < rec-1)
	{
		//����һ��
		CXdpRecord *record = new CXdpRecord();
		if (NULL == record)
		{
			return false;
		}
		if (!record->Init(&m_pXdpFmt))
		{
			return false;
		}
		m_vNextRecord.push_back(record);
		m_nRecCount = rec;
	}
	if (rec > m_nRecCount)
	{
		m_nRecCount = rec;
	}
	return m_vNextRecord[rec-2]->SetFieldData(fieldname,idata,errmsg);
}
bool CXdp::SetFieldValueM(unsigned short rec,const char *fieldname,unsigned int idata,char *errmsg)
{
	if (0 == rec)
	{
		return false;
	}
	if (1 == rec)
	{
		return SetFieldValue(fieldname,idata,errmsg);
	}
	if (rec > 255)
	{
		sprintf(errmsg,"���255����¼");
		return false;
	}
	if (rec > m_vNextRecord.size()+2)
	{
		sprintf(errmsg,"���Ӽ�¼ʱҪ��˳������");
		return false;
	}
	if (m_vNextRecord.size() < rec-1)
	{
		//����һ��
		CXdpRecord *record = new CXdpRecord();
		if (NULL == record)
		{
			return false;
		}
		if (!record->Init(&m_pXdpFmt))
		{
			return false;
		}
		m_vNextRecord.push_back(record);
		m_nRecCount = rec;
	}
	if (rec > m_nRecCount)
	{
		m_nRecCount = rec;
	}
	return m_vNextRecord[rec-2]->SetFieldData(fieldname,idata,errmsg);
}
bool CXdp::SetFieldValueM(unsigned short rec,const char *fieldname,short idata,char *errmsg)
{
	if (0 == rec)
	{
		return false;
	}
	if (1 == rec)
	{
		return SetFieldValue(fieldname,idata,errmsg);
	}
	if (rec > 255)
	{
		sprintf(errmsg,"���255����¼");
		return false;
	}
	if (rec > m_vNextRecord.size()+2)
	{
		sprintf(errmsg,"���Ӽ�¼ʱҪ��˳������");
		return false;
	}
	if (m_vNextRecord.size() < rec-1)
	{
		//����һ��
		CXdpRecord *record = new CXdpRecord();
		if (NULL == record)
		{
			return false;
		}
		if (!record->Init(&m_pXdpFmt))
		{
			return false;
		}
		m_vNextRecord.push_back(record);
		m_nRecCount = rec;
	}
	if (rec > m_nRecCount)
	{
		m_nRecCount = rec;
	}
	return m_vNextRecord[rec-2]->SetFieldData(fieldname,idata,errmsg);
}
bool CXdp::SetFieldValueM(unsigned short rec,const char *fieldname,unsigned short idata,char *errmsg)
{
	if (0 == rec)
	{
		return false;
	}
	if (1 == rec)
	{
		return SetFieldValue(fieldname,idata,errmsg);
	}
	if (rec > 255)
	{
		sprintf(errmsg,"���255����¼");
		return false;
	}
	if (rec > m_vNextRecord.size()+2)
	{
		sprintf(errmsg,"���Ӽ�¼ʱҪ��˳������");
		return false;
	}
	if (m_vNextRecord.size() < rec-1)
	{
		//����һ��
		CXdpRecord *record = new CXdpRecord();
		if (NULL == record)
		{
			return false;
		}
		if (!record->Init(&m_pXdpFmt))
		{
			return false;
		}
		m_vNextRecord.push_back(record);
		m_nRecCount = rec;
	}
	if (rec > m_nRecCount)
	{
		m_nRecCount = rec;
	}
	return m_vNextRecord[rec-2]->SetFieldData(fieldname,idata,errmsg);
}
bool CXdp::SetFieldValueM(unsigned short rec,const char *fieldname,double idata,char *errmsg)
{
	if (0 == rec)
	{
		return false;
	}
	if (1 == rec)
	{
		return SetFieldValue(fieldname,idata,errmsg);
	}
	if (rec > 255)
	{
		sprintf(errmsg,"���255����¼");
		return false;
	}
	if (rec > m_vNextRecord.size()+2)
	{
		sprintf(errmsg,"���Ӽ�¼ʱҪ��˳������");
		return false;
	}
	if (m_vNextRecord.size() < rec-1)
	{
		//����һ��
		CXdpRecord *record = new CXdpRecord();
		if (NULL == record)
		{
			return false;
		}
		if (!record->Init(&m_pXdpFmt))
		{
			return false;
		}
		m_vNextRecord.push_back(record);
		m_nRecCount = rec;
	}
	if (rec > m_nRecCount)
	{
		m_nRecCount = rec;
	}
	return m_vNextRecord[rec-2]->SetFieldData(fieldname,idata,errmsg);
}
bool CXdp::SetFieldValueM(unsigned short rec, const char* fieldname, INT64_ idata, char* errmsg)
{
    if (0 == rec)
    {
        return false;
    }
    if (1 == rec)
    {
        return SetFieldValue(fieldname, idata, errmsg);
    }
    if (rec > 255)
    {
        sprintf(errmsg, "���255����¼");
        return false;
    }
    if (rec > m_vNextRecord.size() + 2)
    {
        sprintf(errmsg, "���Ӽ�¼ʱҪ��˳������");
        return false;
    }
    if (m_vNextRecord.size() < rec - 1)
    {
        //����һ��
        CXdpRecord* record = new CXdpRecord();
        if (NULL == record)
        {
            return false;
        }
        if (!record->Init(&m_pXdpFmt))
        {
            return false;
        }
        m_vNextRecord.push_back(record);
        m_nRecCount = rec;
    }
    if (rec > m_nRecCount)
    {
        m_nRecCount = rec;
    }
    return m_vNextRecord[rec - 2]->SetFieldData(fieldname, idata, errmsg);
}
bool CXdp::SetFieldValueM(unsigned short rec, const char* fieldname, UINT64_ idata, char* errmsg)
{
    if (0 == rec)
    {
        return false;
    }
    if (1 == rec)
    {
        return SetFieldValue(fieldname, idata, errmsg);
    }
    if (rec > 255)
    {
        sprintf(errmsg, "���255����¼");
        return false;
    }
    if (rec > m_vNextRecord.size() + 2)
    {
        sprintf(errmsg, "���Ӽ�¼ʱҪ��˳������");
        return false;
    }
    if (m_vNextRecord.size() < rec - 1)
    {
        //����һ��
        CXdpRecord* record = new CXdpRecord();
        if (NULL == record)
        {
            return false;
        }
        if (!record->Init(&m_pXdpFmt))
        {
            return false;
        }
        m_vNextRecord.push_back(record);
        m_nRecCount = rec;
    }
    if (rec > m_nRecCount)
    {
        m_nRecCount = rec;
    }
    return m_vNextRecord[rec - 2]->SetFieldData(fieldname, idata, errmsg);
}
bool CXdp::SetFieldValue(unsigned short rec,const char *fieldname,const char *data,unsigned int datalen,char *errmsg)
{
	if (0 == rec)
	{
		return false;
	}
	if (1 == rec)
	{
		return SetFieldValue(fieldname,data,datalen,errmsg);
	}
	if (rec > 255)
	{
		sprintf(errmsg,"���255����¼");
		return false;
	}
	if (rec > m_vNextRecord.size()+2)
	{
		sprintf(errmsg,"���Ӽ�¼ʱҪ��˳������");
		return false;
	}
	if (m_vNextRecord.size() < rec-1)
	{
		//����һ��
		CXdpRecord *record = new CXdpRecord();
		if (NULL == record)
		{
			return false;
		}
		if (!record->Init(&m_pXdpFmt))
		{
			return false;
		}
		m_vNextRecord.push_back(record);
		m_nRecCount = rec;
	}
	if (rec > m_nRecCount)
	{
		m_nRecCount = rec;
	}
	return m_vNextRecord[rec-2]->SetFieldData(fieldname,data,datalen,errmsg);
	
}
bool CXdp::SetFieldValue(unsigned short rec,unsigned short index,int idata,char *errmsg)
{
	if (0 == rec)
	{
		return false;
	}
	if (1 == rec)
	{
		return SetFieldValue(index,idata,errmsg);
	}
	if (rec > 255)
	{
		sprintf(errmsg,"���255����¼");
		return false;
	}
	if (rec > m_vNextRecord.size()+2)
	{
		sprintf(errmsg,"���Ӽ�¼ʱҪ��˳������");
		return false;
	}
	if (m_vNextRecord.size() < rec-1)
	{
		//����һ��
		CXdpRecord *record = new CXdpRecord();
		if (NULL == record)
		{
			return false;
		}
		if (!record->Init(&m_pXdpFmt))
		{
			return false;
		}
		m_vNextRecord.push_back(record);
		m_nRecCount = rec;
	}
	if (rec > m_nRecCount)
	{
		m_nRecCount = rec;
	}
	//����ֵ
	return m_vNextRecord[rec-2]->SetFieldData(index,idata,errmsg);
}
bool CXdp::SetFieldValue(unsigned short rec,unsigned short index,unsigned int idata,char *errmsg)
{
	if (0 == rec)
	{
		return false;
	}
	if (1 == rec)
	{
		return SetFieldValue(index,idata,errmsg);
	}
	if (rec > 255)
	{
		sprintf(errmsg,"���255����¼");
		return false;
	}
	if (rec > m_vNextRecord.size()+2)
	{
		sprintf(errmsg,"���Ӽ�¼ʱҪ��˳������");
		return false;
	}
	if (m_vNextRecord.size() < rec-1)
	{
		//����һ��
		CXdpRecord *record = new CXdpRecord();
		if (NULL == record)
		{
			return false;
		}
		if (!record->Init(&m_pXdpFmt))
		{
			return false;
		}
		m_vNextRecord.push_back(record);
		m_nRecCount = rec;
	}
	if (rec > m_nRecCount)
	{
		m_nRecCount = rec;
	}
	//����ֵ
	return m_vNextRecord[rec-2]->SetFieldData(index,idata,errmsg);
}
bool CXdp::SetFieldValue(unsigned short rec,unsigned short index,short idata,char *errmsg)
{
	if (0 == rec)
	{
		return false;
	}
	if (1 == rec)
	{
		return SetFieldValue(index,idata,errmsg);
	}
	if (rec > 255)
	{
		sprintf(errmsg,"���255����¼");
		return false;
	}
	if (rec > m_vNextRecord.size()+2)
	{
		sprintf(errmsg,"���Ӽ�¼ʱҪ��˳������");
		return false;
	}
	if (m_vNextRecord.size() < rec-1)
	{
		//����һ��
		CXdpRecord *record = new CXdpRecord();
		if (NULL == record)
		{
			return false;
		}
		if (!record->Init(&m_pXdpFmt))
		{
			return false;
		}
		m_vNextRecord.push_back(record);
		m_nRecCount = rec;
	}
	if (rec > m_nRecCount)
	{
		m_nRecCount = rec;
	}
	//����ֵ
	return m_vNextRecord[rec-2]->SetFieldData(index,idata,errmsg);
}
bool CXdp::SetFieldValue(unsigned short rec,unsigned short index,unsigned short idata,char *errmsg)
{
	if (0 == rec)
	{
		return false;
	}
	if (1 == rec)
	{
		return SetFieldValue(index,idata,errmsg);
	}
	if (rec > 255)
	{
		sprintf(errmsg,"���255����¼");
		return false;
	}
	if (rec > m_vNextRecord.size()+2)
	{
		sprintf(errmsg,"���Ӽ�¼ʱҪ��˳������");
		return false;
	}
	if (m_vNextRecord.size() < rec-1)
	{
		//����һ��
		CXdpRecord *record = new CXdpRecord();
		if (NULL == record)
		{
			return false;
		}
		if (!record->Init(&m_pXdpFmt))
		{
			return false;
		}
		m_vNextRecord.push_back(record);
		m_nRecCount = rec;
	}
	if (rec > m_nRecCount)
	{
		m_nRecCount = rec;
	}
	//����ֵ
	return m_vNextRecord[rec-2]->SetFieldData(index,idata,errmsg);
}
bool CXdp::SetFieldValue(unsigned short rec,unsigned short index,double idata,char *errmsg)
{
	if (0 == rec)
	{
		return false;
	}
	if (1 == rec)
	{
		return SetFieldValue(index,idata,errmsg);
	}
	if (rec > 255)
	{
		sprintf(errmsg,"���255����¼");
		return false;
	}
	if (rec > m_vNextRecord.size()+2)
	{
		sprintf(errmsg,"���Ӽ�¼ʱҪ��˳������");
		return false;
	}
	if (m_vNextRecord.size() < rec-1)
	{
		//����һ��
		CXdpRecord *record = new CXdpRecord();
		if (NULL == record)
		{
			return false;
		}
		if (!record->Init(&m_pXdpFmt))
		{
			return false;
		}
		m_vNextRecord.push_back(record);
		m_nRecCount = rec;
	}
	if (rec > m_nRecCount)
	{
		m_nRecCount = rec;
	}
	//����ֵ
	return m_vNextRecord[rec-2]->SetFieldData(index,idata,errmsg);
}
bool CXdp::SetFieldValue(unsigned short rec, unsigned short index, INT64_ idata, char* errmsg)
{
    if (0 == rec)
    {
        return false;
    }
    if (1 == rec)
    {
        return SetFieldValue(index, idata, errmsg);
    }
    if (rec > 255)
    {
        sprintf(errmsg, "���255����¼");
        return false;
    }
    if (rec > m_vNextRecord.size() + 2)
    {
        sprintf(errmsg, "���Ӽ�¼ʱҪ��˳������");
        return false;
    }
    if (m_vNextRecord.size() < rec - 1)
    {
        //����һ��
        CXdpRecord* record = new CXdpRecord();
        if (NULL == record)
        {
            return false;
        }
        if (!record->Init(&m_pXdpFmt))
        {
            return false;
        }
        m_vNextRecord.push_back(record);
        m_nRecCount = rec;
    }
    if (rec > m_nRecCount)
    {
        m_nRecCount = rec;
    }
    //����ֵ
    return m_vNextRecord[rec - 2]->SetFieldData(index, idata, errmsg);
}
bool CXdp::SetFieldValue(unsigned short rec, unsigned short index, UINT64_ idata, char* errmsg)
{
    if (0 == rec)
    {
        return false;
    }
    if (1 == rec)
    {
        return SetFieldValue(index, idata, errmsg);
    }
    if (rec > 255)
    {
        sprintf(errmsg, "���255����¼");
        return false;
    }
    if (rec > m_vNextRecord.size() + 2)
    {
        sprintf(errmsg, "���Ӽ�¼ʱҪ��˳������");
        return false;
    }
    if (m_vNextRecord.size() < rec - 1)
    {
        //����һ��
        CXdpRecord* record = new CXdpRecord();
        if (NULL == record)
        {
            return false;
        }
        if (!record->Init(&m_pXdpFmt))
        {
            return false;
        }
        m_vNextRecord.push_back(record);
        m_nRecCount = rec;
    }
    if (rec > m_nRecCount)
    {
        m_nRecCount = rec;
    }
    //����ֵ
    return m_vNextRecord[rec - 2]->SetFieldData(index, idata, errmsg);
}
bool CXdp::SetFieldValue(unsigned short rec,unsigned short index,const char *data,unsigned int datalen,char *errmsg)
{
	if (0 == rec)
	{
		return false;
	}
	if (1 == rec)
	{
		return SetFieldValue(index,data,datalen,errmsg);
	}
	if (rec > 255)
	{
		sprintf(errmsg,"���255����¼");
		return false;
	}
	if (rec > m_vNextRecord.size()+2)
	{
		sprintf(errmsg,"���Ӽ�¼ʱҪ��˳������");
		return false;
	}
	if (m_vNextRecord.size() < rec-1)
	{
		//����һ��
		CXdpRecord *record = new CXdpRecord();
		if (NULL == record)
		{
			return false;
		}
		if (!record->Init(&m_pXdpFmt))
		{
			return false;
		}
		m_vNextRecord.push_back(record);
		m_nRecCount = rec;
	}
	if (rec > m_nRecCount)
	{
		m_nRecCount = rec;
	}
	//����ֵ
	return m_vNextRecord[rec-2]->SetFieldData(index,data,datalen,errmsg);
}

bool CXdp::SetFieldValue(unsigned short index,int idata,char *errmsg)
{
	return m_pFirstRecord.SetFieldData(index,idata,errmsg);
}
bool CXdp::SetFieldValue(unsigned short index,unsigned int idata,char *errmsg)
{
	return m_pFirstRecord.SetFieldData(index,idata,errmsg);
}
bool CXdp::SetFieldValue(unsigned short index,short idata,char *errmsg)
{
	return m_pFirstRecord.SetFieldData(index,idata,errmsg);
}
bool CXdp::SetFieldValue(unsigned short index,unsigned short idata,char *errmsg)
{
	return m_pFirstRecord.SetFieldData(index,idata,errmsg);
}
bool CXdp::SetFieldValue(unsigned short index,double idata,char *errmsg)
{
	return m_pFirstRecord.SetFieldData(index,idata,errmsg);
}
bool CXdp::SetFieldValue(unsigned short index, INT64_ idata, char* errmsg)
{
    return m_pFirstRecord.SetFieldData(index, idata, errmsg);
}
bool CXdp::SetFieldValue(unsigned short index, UINT64_ idata, char* errmsg)
{
    return m_pFirstRecord.SetFieldData(index, idata, errmsg);
}
bool CXdp::SetFieldValue(unsigned short index,const char *data,unsigned int datalen,char *errmsg)
{
	return m_pFirstRecord.SetFieldData(index,data,datalen,errmsg);
}
bool CXdp::SetFieldValue(const char *fieldname,int idata,char *errmsg)
{
	return m_pFirstRecord.SetFieldData(fieldname,idata,errmsg);
}
bool CXdp::SetFieldValue(const char *fieldname,unsigned int idata,char *errmsg)
{
	return m_pFirstRecord.SetFieldData(fieldname,idata,errmsg);
}
bool CXdp::SetFieldValue(const char *fieldname,short idata,char *errmsg)
{
	return m_pFirstRecord.SetFieldData(fieldname,idata,errmsg);
}
bool CXdp::SetFieldValue(const char *fieldname,unsigned short idata,char *errmsg)
{
	return m_pFirstRecord.SetFieldData(fieldname,idata,errmsg);
}
bool CXdp::SetFieldValue(const char *fieldname,double idata,char *errmsg)
{
	return m_pFirstRecord.SetFieldData(fieldname,idata,errmsg);
}
bool CXdp::SetFieldValue(const char* fieldname, INT64_ idata, char* errmsg)
{
    return m_pFirstRecord.SetFieldData(fieldname, idata, errmsg);
}
bool CXdp::SetFieldValue(const char* fieldname, UINT64_ idata, char* errmsg)
{
    return m_pFirstRecord.SetFieldData(fieldname, idata, errmsg);
}
bool CXdp::SetFieldValue(const char *fieldname,const char *data,unsigned int datalen,char *errmsg)
{
	return m_pFirstRecord.SetFieldData(fieldname,data,datalen,errmsg);
}

void CXdp::ClearNextRecord()
{
	for (int i=0; i< m_vNextRecord.size() ; i++ )
	{
		CXdpRecord *ss = m_vNextRecord[i];
		delete  ss;
		ss = NULL;
		m_vNextRecord[i] = NULL;
	}
	m_vNextRecord.clear();
	m_nRecCount = 1;
}

unsigned short CXdp::GetCurSavePackNum()
{
	return m_vNextRecord.size();
}

void CXdp::PrintXdp(char *xdpbuf)
{
	int len =0;
	if (!m_pFirstRecord.PrintXdpRec(xdpbuf+len))
	{
		return;
	}
	len = strlen(xdpbuf);
	for (int i=0;i< (int)m_nRecCount-1 ; i++)
	{
		m_vNextRecord[i]->PrintXdpRec(xdpbuf+len);
		len = strlen(xdpbuf);
	}
	return;
}

bool CXdp::CancelField(unsigned short index)
{
	return m_pFirstRecord.CancelField(index);
}

bool CXdp::CancelField(const char *fieldname)
{
	return m_pFirstRecord.CancelField(fieldname);
}

bool CXdp::CancelField(unsigned short rec,unsigned short index)
{
	if (0 == rec)
	{
		return false;
	}
	if (1 == rec)
	{
		return m_pFirstRecord.CancelField(index);
	}
	if (rec > m_vNextRecord.size()+1)
	{
		return false;
	}
	
	return m_vNextRecord[rec-2]->CancelField(index);
}

bool CXdp::CancelField(unsigned short rec,const char *fieldname)
{
	if (0 == rec)
	{
		return false;
	}
	if (1 == rec)
	{
		return m_pFirstRecord.CancelField(fieldname);
	}
	if (rec > m_vNextRecord.size()+1)
	{
		return false;
	}
	
	return m_vNextRecord[rec-2]->CancelField(fieldname);
}

unsigned short CXdp::GetRealPackSize()
{
	unsigned short packsize =0 ;
	if (m_nRecCount >0)
	{
		packsize+=sizeof(short);//��¼��
		packsize += m_pFirstRecord.GetRealLen();
		for (int i=1 ; i<m_nRecCount ; i++)
		{
			packsize += m_vNextRecord[i-1]->GetRealLen();
		}
	}
	return packsize;
}

bool CXdp::XdpCopy(CXdp *src)
{
	if (!m_pXdpFmt.XdpFmtCopy(&(src->m_pXdpFmt)))
	{
		return false;
	}
	if (!m_pFirstRecord.Init(&m_pXdpFmt))
	{
		return false;
	}

	m_nRecCount = src->GetRecCount();
	CXdpRecord *xdprec = NULL;
	for (int i=0; i< m_nRecCount ; i++)
	{
		xdprec = src->GetXdpRecord(i+1);
		if ( NULL ==  xdprec )
		{
			return false;
		}
		if (0 == i)
		{
			if (!m_pFirstRecord.XdpRecordCopy(xdprec))
			{
				return false;
			}
		}
		else
		{
			if (i  > m_vNextRecord.size() )
			{
				//����һ��
				CXdpRecord *record = new CXdpRecord();
				if (NULL == record)
				{
					return false;
				}
				if (!record->Init(&m_pXdpFmt))
				{
					return false;
				}
				m_vNextRecord.push_back(record);
			}
			if (!m_vNextRecord[i-1]->XdpRecordCopy(xdprec))
			{
				return false;
			}
		}
	}
	
	return true;
}

CXdpRecord * CXdp::GetXdpRecord(unsigned short rec)
{
	if (0 == rec )
	{
		return NULL;
	}
	if (1 == rec)
	{
		return &m_pFirstRecord;
	}
	if (rec > m_nRecCount)
	{
		return NULL;
	}
	if (rec  > m_vNextRecord.size()+1)
	{
		return NULL;
	}
	return m_vNextRecord[rec-2];
}






// ������: GetRealPackSize
// ���  : ������ 2014-10-30 11:31:20
// ����  : unsigned short 
// ����  : void *CXdpClass   xdp���ʵ��ָ�룬��InitXdp���
// ����  : ȡ�õ�ǰʵ�ʱ��ĵĴ�С
unsigned short XDP_GetRealPackSize(void *CXdpClass)
{
	if (NULL == CXdpClass)
	{
		return 0;
	}
	CXdp *xdp = (CXdp *) CXdpClass;
	return xdp->GetRealPackSize();
}

// ������: CancelField
// ���  : ������ 2014-10-30 11:17:16
// ����  : int 0�ɹ� ��0�˰�
// ����  : void *CXdpClass   xdp���ʵ��ָ�룬��InitXdp���
// ����  : unsigned short rec  ��1��ʼ
// ����  : const char *fieldname
// ����  : ȡ���ֶΣ������Ͳ������ɴ��ֶα��Ļ�ȡ��ֵ��
int XDP_CancelFieldByName(void *CXdpClass,unsigned short rec,const char *fieldname)
{
	if (NULL == CXdpClass)
	{
		return 0;
	}
	CXdp *xdp = (CXdp *) CXdpClass;
	if (!xdp->CancelField(rec,fieldname))
	{
		return -1;
	}
	return 0;
}

// ������: CancelField
// ���  : ������ 2014-10-30 11:16:28
// ����  : int 0�ɹ� ��0�˰�
// ����  : void *CXdpClass   xdp���ʵ��ָ�룬��InitXdp���
// ����  : unsigned short rec   ��1��ʼ
// ����  : unsigned short index
// ����  : ȡ���ֶΣ������Ͳ������ɴ��ֶα��Ļ�ȡ��ֵ��
int XDP_CancelFieldByIndex(void *CXdpClass,unsigned short rec,unsigned short index)
{
	if (NULL == CXdpClass)
	{
		return 0;
	}
	CXdp *xdp = (CXdp *) CXdpClass;
	if (!xdp->CancelField(rec,index))
	{
		return -1;
	}
	return 0;
}

// ������: PrintXdp
// ���  : ������ 2014-10-30 11:15:33
// ����  : void 
// ����  : void *CXdpClass   xdp���ʵ��ָ�룬��InitXdp���
// ����  : char *xdpbuf
// ����  : ������������ʾ����������������תΪ16������ʾ
void XDP_PrintXdp(void *CXdpClass,char *xdpbuf)
{
	if (NULL == CXdpClass)
	{
		return;
	}
	CXdp *xdp = (CXdp *) CXdpClass;
	xdp->PrintXdp(xdpbuf);
	return;
}

// ������: GetCurSavePackNum
// ���  : ������ 2014-10-29 11:45:49
// ����  : unsigned short 
// ����  : void *CXdpClass   xdp���ʵ��ָ�룬��InitXdp���
// ����  : ȡ�ñ�����¼�Ľṹ��Ŀ��ע�ⲻ������һ����¼
unsigned short XDP_GetCurSavePackNum(void *CXdpClass)
{
	if (NULL == CXdpClass)
	{
		return 0;
	}
	CXdp *xdp = (CXdp *) CXdpClass;
	return xdp->GetCurSavePackNum();
}

// ������: ClearNextRecord
// ���  : ������ 2014-10-29 11:34:21
// ����  : void 
// ����  : void *CXdpClass   xdp���ʵ��ָ�룬��InitXdp���
// ����  : ��ձ�����¼�Ľṹ
void XDP_ClearNextRecord(void *CXdpClass)
{
	if (NULL == CXdpClass)
	{
		return;
	}
	CXdp *xdp = (CXdp *) CXdpClass;
	xdp->ClearNextRecord();
}


// ������: GetFieldType
// ���  : ������ 2014-10-29 11:22:23
// ����  : unsigned short ���ͣ���XdpFmt.h�궨��
// ����  : void *CXdpClass   xdp���ʵ��ָ�룬��InitXdp���
// ����  : const char *fieldname/unsigned short index �ֶ�����/����
// ����  : ȡ���ֶ�����
unsigned short XDP_GetFieldTypeByName(void *CXdpClass,const char *fieldname)
{
	if (NULL == CXdpClass)
	{
		return XDP_UNKNOW;
	}
	CXdp *xdp = (CXdp *) CXdpClass;	
	return xdp->GetFieldType(fieldname);
}
unsigned short XDP_GetFieldTypeByIndex(void *CXdpClass,unsigned short index)
{
	if (NULL == CXdpClass)
	{
		return XDP_UNKNOW;
	}
	CXdp *xdp = (CXdp *) CXdpClass;	
	return xdp->GetFieldType(index);
}

// ������: GetRecCount
// ���  : ������ 2014-10-29 11:10:29
// ����  : unsigned short ��¼��
// ����  : void *CXdpClass   xdp���ʵ��ָ�룬��InitXdp���
// ����  : ���ص�ǰ�����й��м�����¼��ע���¼���ͱ�����¼�Ľṹ��Ŀ����һ������
unsigned short XDP_GetRecCount(void *CXdpClass)
{
	if (NULL == CXdpClass)
	{
		return 0;
	}
	CXdp *xdp = (CXdp *) CXdpClass;	
	return xdp->GetRecCount();
}

// ������: GetFieldValue
// ���  : ������ 2014-10-29 11:04:09
// ����  : int 0�ɹ� ��0�˰�
// ����  : void *CXdpClass   xdp���ʵ��ָ�룬��InitXdp���
// ����  : unsigned short rec    �ڼ�����¼
// ����  : unsigned short index/const char *fieldname  �ֶ�����/����
// ����  : char *data      ���ݻ���  ��Ϊ��ֵ��ʱ��Ҫ��ǿ��ת��  �磺int *aa = (int *)data;
// �����������  : unsigned int &datalen   ����Ϊdata�����С ���Ϊʵ�����ݳ���
// ����  : int &fieldtype  �ֶ����ͣ���XdpFmt.h�к궨��
// ����  : char *errmsg  ������Ϣ
// ����  : ��xdp��ȡ�ֶ�ֵ
int XDP_GetFieldValueByName(void *CXdpClass,unsigned short rec,const char *fieldname,char *data,unsigned int *datalen,int *fieldtype,char *errmsg)
{
	if (NULL == CXdpClass)
	{
		sprintf(errmsg,"CXdpClassΪNULL");
		return -1;
	}
	CXdp *xdp = (CXdp *) CXdpClass;	
	unsigned int dlen = *datalen;
	int ftype;
	if (!xdp->GetFieldValue(rec,fieldname,data,dlen,ftype,errmsg))
	{
		return -1;
	}
	*datalen = dlen;
	*fieldtype= ftype;
	return 0;
}
int XDP_GetFieldValueByIndex(void *CXdpClass,unsigned short rec,unsigned short index,char *data,unsigned int *datalen,int *fieldtype,char *errmsg)
{
	if (NULL == CXdpClass)
	{
		sprintf(errmsg,"CXdpClassΪNULL");
		return -1;
	}
	CXdp *xdp = (CXdp *) CXdpClass;	
	unsigned int dlen = *datalen;
	int ftype;
	if (!xdp->GetFieldValue(rec,index,data,dlen,ftype,errmsg))
	{
		return -1;
	}
	*datalen = dlen;
	*fieldtype= ftype;
	return 0;
}

// ������: GetFieldValue
// ���  : ������ 2014-10-29 11:11:55
// ����  : int 0�ɹ� ��0�˰�
// ����  : void *CXdpClass   xdp���ʵ��ָ�룬��InitXdp���
// ����  : unsigned short index/const char *fieldname �ֶ�����/����
// ����  : char *data       ���ݻ��� ��Ϊ��ֵ��ʱ��Ҫ��ǿ��ת����ȡֵ  �磺int *aa = (int *)data;
// ����  : unsigned int &datalen     ����Ϊdata�����С ���Ϊʵ�����ݳ���
// ����  : int &fieldtype  �ֶ����ͣ���XdpFmt.h�к궨��
// ����  : char *errmsg   ������Ϣ
// ����  : ��xdp��ȡ��һ����¼���ֶ�ֵ
int XDP_GetFirstFieldValueByIndex(void *CXdpClass,unsigned short index,char *data,unsigned int *datalen,int *fieldtype,char *errmsg)
{
	if (NULL == CXdpClass)
	{
		sprintf(errmsg,"CXdpClassΪNULL");
		return -1;
	}
	CXdp *xdp = (CXdp *) CXdpClass;	
	unsigned int dlen = *datalen;
	int ftype;
	if (!xdp->GetFieldValue(index,data,dlen,ftype,errmsg))
	{
		return -1;
	}
	*datalen = dlen;
	*fieldtype= ftype;
	return 0;
}
int XDP_GetFirstFieldValueByName(void *CXdpClass,const char *fieldname,char *data,unsigned int *datalen,int *fieldtype,char *errmsg)
{
	if (NULL == CXdpClass)
	{
		sprintf(errmsg,"CXdpClassΪNULL");
		return -1;
	}
	CXdp *xdp = (CXdp *) CXdpClass;	
	unsigned int dlen = *datalen;
	int ftype;
	if (!xdp->GetFieldValue(fieldname,data,dlen,ftype,errmsg))
	{
		return -1;
	}
	*datalen = dlen;
	*fieldtype= ftype;
	return 0;
}



// ������: SetFieldValue
// ���  : ������ 2014-10-29 11:40:30
// ����  : int 0�ɹ� ��0�˰�
// ����  : void *CXdpClass   xdp���ʵ��ָ�룬��InitXdp���
// ����  : unsigned short rec    Ҫ����Ϊ��¼����ע����rec����ԭ��¼����ô��¼��Ҫ��С����˳������
// ����  : unsigned short index/const char *fieldname �ֶ�����/����
// ����  : const char *data   ���ݻ��� ��Ϊ��ֵ��ʱ��Ҫ��ǿ��ת������ֵ  �磺int *aa = (int *)data; *aa = 1200
// ����  : int datalen     ���ݳ���
// ����  : char *errmsg    ������Ϣ
// ����  : ����ָ����¼�����ֶ�ֵ
int XDP_SetFieldValueByName(void *CXdpClass,unsigned short rec,const char *fieldname,const char *data,unsigned int datalen,char *errmsg)
{
	if (NULL == CXdpClass)
	{
		sprintf(errmsg,"CXdpClassΪNULL");
		return -1;
	}
	CXdp *xdp = (CXdp *) CXdpClass;	
	if (!xdp->SetFieldValue(rec,fieldname,data,datalen,errmsg))
	{
		return -1;
	}
	return 0;
}
int XDP_SetFieldValueByIndex(void *CXdpClass,unsigned short rec,unsigned short index,const char *data,unsigned int datalen,char *errmsg)
{
	if (NULL == CXdpClass)
	{
		sprintf(errmsg,"CXdpClassΪNULL");
		return -1;
	}
	CXdp *xdp = (CXdp *) CXdpClass;	
	if (!xdp->SetFieldValue(rec,index,data,datalen,errmsg))
	{
		return -1;
	}
	return 0;
}



// ������: SetFieldValue
// ���  : ������ 2014-10-29 11:40:35
// ����  : int 0�ɹ� ��0�˰�
// ����  : void *CXdpClass   xdp���ʵ��ָ�룬��InitXdp���
// ����  : unsigned short index/const char *fieldname �ֶ�����/����  
// ����  : const char *data   ���ݻ��� ��Ϊ��ֵ��ʱ��Ҫ��ǿ��ת������ֵ  �磺int *aa = (int *)data; *aa = 1200
// ����  : int datalen    ���ݳ���
// ����  : char *errmsg  ������Ϣ
// ����  : ���õ�һ����¼���ֶ�ֵ
int XDP_SetFirstFieldValueByIndex(void *CXdpClass,unsigned short index,const char *data,unsigned int datalen,char *errmsg)
{
	if (NULL == CXdpClass)
	{
		sprintf(errmsg,"CXdpClassΪNULL");
		return -1;
	}
	CXdp *xdp = (CXdp *) CXdpClass;	
	if (!xdp->SetFieldValue(index,data,datalen,errmsg))
	{
		return -1;
	}
	return 0;
}
int XDP_SetFirstFieldValueByName(void *CXdpClass,const char *fieldname,const char *data,unsigned int datalen,char *errmsg)
{
	if (NULL == CXdpClass)
	{
		sprintf(errmsg,"CXdpClassΪNULL");
		return -1;
	}
	CXdp *xdp = (CXdp *) CXdpClass;	
	if (!xdp->SetFieldValue(fieldname,data,datalen,errmsg))
	{
		return -1;
	}
	return 0;
}


// ������: SetCurRecCount
// ���  : ������ 2014-10-29 10:55:12
// ����  : int 0�ɹ� ��0�˰ܣ���ԭ���ĵļ�¼Y<Xʱʧ��
// ����  : void *CXdpClass   xdp���ʵ��ָ�룬��InitXdp���
// ����  : unsigned short rec 
// ����  : ���õ�ǰ�ļ�¼������ԭ��������Y����¼ʱ������ֻ����X(X<Y)����¼ʱ���������ü�¼��ΪX,���򷵻ص���ΪY��
int XDP_SetCurRecCount(void *CXdpClass,unsigned short x)
{
	if (NULL == CXdpClass)
	{
		return -1;
	}
	CXdp *xdp = (CXdp *) CXdpClass;	
	if (!xdp->SetCurRecCount(x))
	{
		return -1;
	}
	return 0;
}

// ������: ResetData
// ���  : ������ 2014-10-27 14:24:43
// ����  : void 
// ����  : void *CXdpClass   xdp���ʵ��ָ�룬��InitXdp���
// ����  : ������¼������λͼ
void XDP_ResetData(void *CXdpClass)
{
	if (NULL == CXdpClass)
	{
		return ;
	}
	CXdp *xdp = (CXdp *) CXdpClass;	
	xdp->ResetData();
	return;
}

// ������: ToBuffer
// ���  : ������ 2014-10-27 14:18:52
// ����  : int 0�ɹ� ��0�˰�
// ����  : void *CXdpClass   xdp���ʵ��ָ�룬��InitXdp���
// ����  : char *xdpbuf   xdp����
// �����������  : int *len    ����Ϊxdpbuf��С,���Ϊxdp���Ĵ�С
// ����  : char *errmsg   ������Ϣ
// ����  : ����xdp����
int XDP_ToBuffer(void *CXdpClass,char *xdpbuf,unsigned int *len,char *errmsg)
{
	if (NULL == CXdpClass)
	{
		sprintf(errmsg,"CXdpClassΪNULL");
		return -1;
	}
	CXdp *xdp = (CXdp *) CXdpClass;	
	unsigned int dlen = *len;
	if (!xdp->ToBuffer(xdpbuf,dlen,errmsg))
	{
		*len =0;
		return -1;
	}
	*len = dlen;
	return 0;
}

// ������: FromBuffer
// ���  : ������ 2014-10-27 14:17:34
// ����  : int 0�ɹ� ��0�˰�
// ����  : void *CXdpClass   xdp���ʵ��ָ�룬��InitXdp���
// ����  : const char *xdpbuf   xdp���ݻ���
// ����  : int xdpbuflen   xdp���ݳ���
// ����  : char *errmsg   ������Ϣ
// ����  : �����ݻ����������
int XDP_FromBuffer(void *CXdpClass,const char *xdpbuf,unsigned int xdpbuflen,char *errmsg)
{
	if (NULL == CXdpClass)
	{
		sprintf(errmsg,"CXdpClassΪNULL");
		return -1;
	}
	CXdp *xdp = (CXdp *) CXdpClass;	
	if (!xdp->FromBuffer(xdpbuf,xdpbuflen,errmsg))
	{
		return -1;
	}
	return 0;
}

// ������: InitXdp
// ���  : ������ 2014-10-27 14:14:55
// ����  : int 0�ɹ� ��0�˰�
// �����������  : void **CXdpClass   ����Ϊָ���ַ�����Ϊxdp���ʵ��ָ��
// ����  : const char *datafile   xml�����ļ�
// ����  : char *errmsg      ������Ϣ
// ����  : ��ʼ�����ģ�
int XDP_InitXdp(void **CXdpClass,const char *datafile,char *errmsg)
{
	CXdp *xdp = new CXdp();
	if (NULL == xdp)
	{
		sprintf(errmsg,"new CXdpʧ��");
		return -1;
	}
	if (!xdp->InitXdp(datafile,errmsg))
	{
		*CXdpClass = xdp;
		return -1;
	}
	*CXdpClass = xdp;
	return 0;
}

// ������: FreeXdp
// ���  : ������ 2014-11-7 11:19:43
// ����  : void
// ����  : void *CXdpClass
// ����  : �ͷ�xdp���ʵ��
void XDP_FreeXdp(void *CXdpClass)
{
	if (NULL == CXdpClass)
	{
		return ;
	}
	CXdp *xdp = (CXdp *) CXdpClass;	
	delete xdp;
	xdp = NULL;
	return;
}
int  XDP_Copy(void *CXdpClassDst,void *CXdpClassSrc)
{
	if (NULL == CXdpClassDst)
	{
		return -1;
	}
	if (NULL == CXdpClassSrc)
	{
		return -1;
	}
	CXdp *xdpsrc = (CXdp *) CXdpClassSrc;
	CXdp *xdpdst = (CXdp *) CXdpClassDst;
	if (!xdpdst->XdpCopy(xdpsrc))
	{
		return -1;
	}
	return 0;
}

//��װ��c#�õ�c����



// ������: GetRealPackSize
// ���  : ������ 2014-10-30 11:31:20
// ����  : unsigned short 
// ����  : unsigned int xdpindex   xdp���ʵ���±꣬��InitXdp���
// ����  : ȡ�õ�ǰʵ�ʱ��ĵĴ�С
unsigned short XDPI_GetRealPackSize(unsigned int xdpindex)
{
    if (xdpindex > g_XdpArray.size() - 1)
    {
        return 0;
    }
    CXdp* xdp = g_XdpArray[xdpindex];
    return xdp->GetRealPackSize();
}

// ������: CancelField
// ���  : ������ 2014-10-30 11:17:16
// ����  : int 0�ɹ� ��0�˰�
// ����  : unsigned int xdpindex   xdp���ʵ���±꣬��InitXdp���
// ����  : unsigned short rec  ��1��ʼ
// ����  : const char *fieldname
// ����  : ȡ���ֶΣ������Ͳ������ɴ��ֶα��Ļ�ȡ��ֵ��
int XDPI_CancelFieldByName(unsigned int xdpindex, unsigned short rec, const char* fieldname)
{
    if (xdpindex > g_XdpArray.size() - 1)
    {
        return -1;
    }
    CXdp* xdp = g_XdpArray[xdpindex];
    if (!xdp->CancelField(rec, fieldname))
    {
        return -1;
    }
    return 0;
}

// ������: CancelField
// ���  : ������ 2014-10-30 11:16:28
// ����  : int 0�ɹ� ��0�˰�
// ����  : unsigned int xdpindex   xdp���ʵ���±꣬��InitXdp���
// ����  : unsigned short rec   ��1��ʼ
// ����  : unsigned short index
// ����  : ȡ���ֶΣ������Ͳ������ɴ��ֶα��Ļ�ȡ��ֵ��
int XDPI_CancelFieldByIndex(unsigned int xdpindex, unsigned short rec, unsigned short index)
{
    if (xdpindex > g_XdpArray.size() - 1)
    {
        return -1;
    }
    CXdp* xdp = g_XdpArray[xdpindex];
    if (!xdp->CancelField(rec, index))
    {
        return -1;
    }
    return 0;
}

// ������: PrintXdp
// ���  : ������ 2014-10-30 11:15:33
// ����  : void 
// ����  : unsigned int xdpindex   xdp���ʵ���±꣬��InitXdp���
// ����  : char *xdpbuf
// ����  : ������������ʾ����������������תΪ16������ʾ
void XDPI_PrintXdp(unsigned int xdpindex, char* xdpbuf)
{
    if (xdpindex > g_XdpArray.size() - 1)
    {
        return ;
    }
    CXdp* xdp = g_XdpArray[xdpindex];
    xdp->PrintXdp(xdpbuf);
    return;
}

// ������: GetCurSavePackNum
// ���  : ������ 2014-10-29 11:45:49
// ����  : unsigned short 
// ����  : unsigned int xdpindex   xdp���ʵ���±꣬��InitXdp���
// ����  : ȡ�ñ�����¼�Ľṹ��Ŀ��ע�ⲻ������һ����¼
unsigned short XDPI_GetCurSavePackNum(unsigned int xdpindex)
{
    if (xdpindex > g_XdpArray.size() - 1)
    {
        return 0;
    }
    CXdp* xdp = g_XdpArray[xdpindex];
    return xdp->GetCurSavePackNum();
}

// ������: ClearNextRecord
// ���  : ������ 2014-10-29 11:34:21
// ����  : void 
// ����  : unsigned int xdpindex   xdp���ʵ���±꣬��InitXdp���
// ����  : ��ձ�����¼�Ľṹ
void XDPI_ClearNextRecord(unsigned int xdpindex)
{
    if (xdpindex > g_XdpArray.size() - 1)
    {
        return;
    }
    CXdp* xdp = g_XdpArray[xdpindex];
    xdp->ClearNextRecord();
	return;
}


// ������: GetFieldType
// ���  : ������ 2014-10-29 11:22:23
// ����  : unsigned short ���ͣ���XdpFmt.h�궨��
// ����  : unsigned int xdpindex   xdp���ʵ���±꣬��InitXdp���
// ����  : const char *fieldname/unsigned short index �ֶ�����/����
// ����  : ȡ���ֶ�����
unsigned short XDPI_GetFieldTypeByName(unsigned int xdpindex, const char* fieldname)
{
    if (xdpindex > g_XdpArray.size() - 1)
    {
        return XDP_UNKNOW;
    }
    CXdp* xdp = g_XdpArray[xdpindex];
    return xdp->GetFieldType(fieldname);
}
unsigned short XDPI_GetFieldTypeByIndex(unsigned int xdpindex, unsigned short index)
{
    if (xdpindex > g_XdpArray.size() - 1)
    {
		return XDP_UNKNOW;
    }
    CXdp* xdp = g_XdpArray[xdpindex];
    return xdp->GetFieldType(index);
}

// ������: GetRecCount
// ���  : ������ 2014-10-29 11:10:29
// ����  : unsigned short ��¼��
// ����  : unsigned int xdpindex   xdp���ʵ���±꣬��InitXdp���
// ����  : ���ص�ǰ�����й��м�����¼��ע���¼���ͱ�����¼�Ľṹ��Ŀ����һ������
unsigned short XDPI_GetRecCount(unsigned int xdpindex)
{
    if (xdpindex > g_XdpArray.size() - 1)
    {
        return 0;
    }
    CXdp* xdp = g_XdpArray[xdpindex];
    return xdp->GetRecCount();
}

// ������: GetFieldValue
// ���  : ������ 2014-10-29 11:04:09
// ����  : int 0�ɹ� ��0�˰�
// ����  : unsigned int xdpindex   xdp���ʵ���±꣬��InitXdp���
// ����  : unsigned short rec    �ڼ�����¼
// ����  : unsigned short index/const char *fieldname  �ֶ�����/����
// ����  : char *data      ���ݻ���  ��Ϊ��ֵ��ʱ��Ҫ��ǿ��ת��  �磺int *aa = (int *)data;
// �����������  : unsigned int &datalen   ����Ϊdata�����С ���Ϊʵ�����ݳ���
// ����  : int &fieldtype  �ֶ����ͣ���XdpFmt.h�к궨��
// ����  : char *errmsg  ������Ϣ
// ����  : ��xdp��ȡ�ֶ�ֵ
int XDPI_GetFieldValueByName(unsigned int xdpindex, unsigned short rec, const char* fieldname, char* data, unsigned int* datalen, int* fieldtype, char* errmsg)
{
    if (xdpindex > g_XdpArray.size() - 1)
    {
        return -1;
    }
    CXdp* xdp = g_XdpArray[xdpindex];
    unsigned int dlen = *datalen;
    int ftype;
    if (!xdp->GetFieldValue(rec, fieldname, data, dlen, ftype, errmsg))
    {
        return -1;
    }
    *datalen = dlen;
    *fieldtype = ftype;
    return 0;
}
int XDPI_GetFieldValueByIndex(unsigned int xdpindex, unsigned short rec, unsigned short index, char* data, unsigned int* datalen, int* fieldtype, char* errmsg)
{
    if (xdpindex > g_XdpArray.size() - 1)
    {
        return -1;
    }
    CXdp* xdp = g_XdpArray[xdpindex];
    unsigned int dlen = *datalen;
    int ftype;
    if (!xdp->GetFieldValue(rec, index, data, dlen, ftype, errmsg))
    {
        return -1;
    }
    *datalen = dlen;
    *fieldtype = ftype;
    return 0;
}

// ������: GetFieldValue
// ���  : ������ 2014-10-29 11:11:55
// ����  : int 0�ɹ� ��0�˰�
// ����  : unsigned int xdpindex   xdp���ʵ���±꣬��InitXdp���
// ����  : unsigned short index/const char *fieldname �ֶ�����/����
// ����  : char *data       ���ݻ��� ��Ϊ��ֵ��ʱ��Ҫ��ǿ��ת����ȡֵ  �磺int *aa = (int *)data;
// ����  : unsigned int &datalen     ����Ϊdata�����С ���Ϊʵ�����ݳ���
// ����  : int &fieldtype  �ֶ����ͣ���XdpFmt.h�к궨��
// ����  : char *errmsg   ������Ϣ
// ����  : ��xdp��ȡ��һ����¼���ֶ�ֵ
int XDPI_GetFirstFieldValueByIndex(unsigned int xdpindex, unsigned short index, char* data, unsigned int* datalen, int* fieldtype, char* errmsg)
{
    if (xdpindex > g_XdpArray.size() - 1)
    {
        return -1;
    }
    CXdp* xdp = g_XdpArray[xdpindex];
    unsigned int dlen = *datalen;
    int ftype;
    if (!xdp->GetFieldValue(index, data, dlen, ftype, errmsg))
    {
        return -1;
    }
    *datalen = dlen;
    *fieldtype = ftype;
    return 0;
}
int XDPI_GetFirstFieldValueByName(unsigned int xdpindex, const char* fieldname, char* data, unsigned int* datalen, int* fieldtype, char* errmsg)
{
    if (xdpindex > g_XdpArray.size() - 1)
    {
        return -1;
    }
    CXdp* xdp = g_XdpArray[xdpindex];
    unsigned int dlen = *datalen;
    int ftype;
    if (!xdp->GetFieldValue(fieldname, data, dlen, ftype, errmsg))
    {
        return -1;
    }
    *datalen = dlen;
    *fieldtype = ftype;
    return 0;
}



// ������: SetFieldValue
// ���  : ������ 2014-10-29 11:40:30
// ����  : int 0�ɹ� ��0�˰�
// ����  : unsigned int xdpindex   xdp���ʵ���±꣬��InitXdp���
// ����  : unsigned short rec    Ҫ����Ϊ��¼����ע����rec����ԭ��¼����ô��¼��Ҫ��С����˳������
// ����  : unsigned short index/const char *fieldname �ֶ�����/����
// ����  : const char *data   ���ݻ��� ��Ϊ��ֵ��ʱ��Ҫ��ǿ��ת������ֵ  �磺int *aa = (int *)data; *aa = 1200
// ����  : int datalen     ���ݳ���
// ����  : char *errmsg    ������Ϣ
// ����  : ����ָ����¼�����ֶ�ֵ
int XDPI_SetFieldValueByName(unsigned int xdpindex, unsigned short rec, const char* fieldname, const char* data, unsigned int datalen, char* errmsg)
{
    if (xdpindex > g_XdpArray.size() - 1)
    {
        return -1;
    }
    CXdp* xdp = g_XdpArray[xdpindex];
    if (!xdp->SetFieldValue(rec, fieldname, data, datalen, errmsg))
    {
        return -1;
    }
    return 0;
}
int XDPI_SetFieldValueByIndex(unsigned int xdpindex, unsigned short rec, unsigned short index, const char* data, unsigned int datalen, char* errmsg)
{
    if (xdpindex > g_XdpArray.size() - 1)
    {
        return -1;
    }
    CXdp* xdp = g_XdpArray[xdpindex];
    if (!xdp->SetFieldValue(rec, index, data, datalen, errmsg))
    {
        return -1;
    }
    return 0;
}



// ������: SetFieldValue
// ���  : ������ 2014-10-29 11:40:35
// ����  : int 0�ɹ� ��0�˰�
// ����  : unsigned int xdpindex   xdp���ʵ���±꣬��InitXdp���
// ����  : unsigned short index/const char *fieldname �ֶ�����/����  
// ����  : const char *data   ���ݻ��� ��Ϊ��ֵ��ʱ��Ҫ��ǿ��ת������ֵ  �磺int *aa = (int *)data; *aa = 1200
// ����  : int datalen    ���ݳ���
// ����  : char *errmsg  ������Ϣ
// ����  : ���õ�һ����¼���ֶ�ֵ
int XDPI_SetFirstFieldValueByIndex(unsigned int xdpindex, unsigned short index, const char* data, unsigned int datalen, char* errmsg)
{
    if (xdpindex > g_XdpArray.size() - 1)
    {
        return -1;
    }
    CXdp* xdp = g_XdpArray[xdpindex];
    if (!xdp->SetFieldValue(index, data, datalen, errmsg))
    {
        return -1;
    }
    return 0;
}
int XDPI_SetFirstFieldValueByName(unsigned int xdpindex, const char* fieldname, const char* data, unsigned int datalen, char* errmsg)
{
    if (xdpindex > g_XdpArray.size() - 1)
    {
        return -1;
    }
    CXdp* xdp = g_XdpArray[xdpindex];
    if (!xdp->SetFieldValue(fieldname, data, datalen, errmsg))
    {
        return -1;
    }
    return 0;
}


// ������: SetCurRecCount
// ���  : ������ 2014-10-29 10:55:12
// ����  : int 0�ɹ� ��0�˰ܣ���ԭ���ĵļ�¼Y<Xʱʧ��
// ����  : unsigned int xdpindex   xdp���ʵ���±꣬��InitXdp���
// ����  : unsigned short rec 
// ����  : ���õ�ǰ�ļ�¼������ԭ��������Y����¼ʱ������ֻ����X(X<Y)����¼ʱ���������ü�¼��ΪX,���򷵻ص���ΪY��
int XDPI_SetCurRecCount(unsigned int xdpindex, unsigned short x)
{
    if (xdpindex > g_XdpArray.size() - 1)
    {
        return -1;
    }
    CXdp* xdp = g_XdpArray[xdpindex];
    if (!xdp->SetCurRecCount(x))
    {
        return -1;
    }
    return 0;
}

// ������: ResetData
// ���  : ������ 2014-10-27 14:24:43
// ����  : void 
// ����  : unsigned int xdpindex   xdp���ʵ���±꣬��InitXdp���
// ����  : ������¼������λͼ
void XDPI_ResetData(unsigned int xdpindex)
{
    if (xdpindex > g_XdpArray.size() - 1)
    {
        return ;
    }
    CXdp* xdp = g_XdpArray[xdpindex];
    xdp->ResetData();
    return;
}

// ������: ToBuffer
// ���  : ������ 2014-10-27 14:18:52
// ����  : int 0�ɹ� ��0�˰�
// ����  : unsigned int xdpindex   xdp���ʵ���±꣬��InitXdp���
// ����  : char *xdpbuf   xdp����
// �����������  : int *len    ����Ϊxdpbuf��С,���Ϊxdp���Ĵ�С
// ����  : char *errmsg   ������Ϣ
// ����  : ����xdp����
int XDPI_ToBuffer(unsigned int xdpindex, char* xdpbuf, unsigned int* len, char* errmsg)
{
    if (xdpindex > g_XdpArray.size() - 1)
    {
        return -1;
    }
    CXdp* xdp = g_XdpArray[xdpindex];
    unsigned int dlen = *len;
    if (!xdp->ToBuffer(xdpbuf, dlen, errmsg))
    {
        *len = 0;
        return -1;
    }
    *len = dlen;
    return 0;
}

// ������: FromBuffer
// ���  : ������ 2014-10-27 14:17:34
// ����  : int 0�ɹ� ��0�˰�
// ����  : unsigned int xdpindex   xdp���ʵ���±꣬��InitXdp���
// ����  : const char *xdpbuf   xdp���ݻ���
// ����  : int xdpbuflen   xdp���ݳ���
// ����  : char *errmsg   ������Ϣ
// ����  : �����ݻ����������
int XDPI_FromBuffer(unsigned int xdpindex, const char* xdpbuf, unsigned int xdpbuflen, char* errmsg)
{
    if (xdpindex > g_XdpArray.size() - 1)
    {
        return -1;
    }
    CXdp* xdp = g_XdpArray[xdpindex];
    if (!xdp->FromBuffer(xdpbuf, xdpbuflen, errmsg))
    {
        return -1;
    }
    return 0;
}

// ������: InitXdp
// ���  : ������ 2014-10-27 14:14:55
// ����  : int >=0�ɹ� xdp���ʵ���±�   <0ʧ��
// ����  : const char *datafile   xml�����ļ�
// ����  : char *errmsg      ������Ϣ
// ����  : ��ʼ�����ģ�
int XDPI_InitXdp(const char* datafile, char* errmsg)
{
    CXdp* xdp = new CXdp();
    if (NULL == xdp)
    {
        sprintf(errmsg, "new CXdpʧ��");
        return -1;
    }
    if (!xdp->InitXdp(datafile, errmsg))
    {
		delete xdp;
		xdp = NULL;
        return -1;
    }
    g_XdpArray.push_back(xdp);
    return g_XdpArray.size()-1;
}

// ������: FreeXdp
// ���  : ������ 2014-11-7 11:19:43
// ����  : void
// ����  :  unsigned int xdpindex   xdp���ʵ���±꣬��InitXdp���
// ����  : �ͷ�xdp���ʵ��
void XDPI_FreeXdp(unsigned int xdpindex)
{
    if (xdpindex > g_XdpArray.size() - 1)
    {
        return ;
    }
    CXdp* xdp = g_XdpArray[xdpindex];
    delete xdp;
	g_XdpArray[xdpindex] = NULL;
    return;
}
int  XDPI_Copy(unsigned int destindex, unsigned int srcindex)
{
	if (destindex == srcindex)
	{
		return -1;
	}
	if (destindex > g_XdpArray.size() - 1)
	{
		return -1;
	}
	CXdp* xdpsrc = g_XdpArray[srcindex];
    if (srcindex > g_XdpArray.size() - 1)
    {
        return -1;
    }
    CXdp* xdpdst = (CXdp*)g_XdpArray[destindex];
    if (!xdpdst->XdpCopy(xdpsrc))
    {
        return -1;
    }
    return 0;
}