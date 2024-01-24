// Xdp.cpp: implementation of the CXdp class.
//
//////////////////////////////////////////////////////////////////////

#include "Xdp.h"
#include "XdpApi.h"


//给c#等用的全局变量
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
		sprintf(errmsg,"报文长度%d太小",xdpbuflen);
		return false;
	}
	ResetData();
	//前两个字节为记录数+版本备用
	m_nRecCount = (unsigned char )xdpbuf[0];
	if (m_nRecCount <1)
	{
		sprintf(errmsg,"记录数为0或报文不符");
		return false;
	}
	if (m_pXdpFmt.GetVersion() != (unsigned char )xdpbuf[1])
	{
		sprintf(errmsg,"缓冲里的XDP版本[%d]不符，应为[%d]",(unsigned char )xdpbuf[1],m_pXdpFmt.GetVersion());
		m_nRecCount = 1;
		return false;	
	}

	if (!m_pFirstRecord.FromBuffer(xdpbuf+2,xdpbuflen-2,errmsg))
	{
		m_nRecCount = 1;
		return false;
	}
	int newnum = (int )m_nRecCount -m_vNextRecord.size();
	if (newnum >1) //没有足够的记录类
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
	//从第二条记录数开始
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
// 	//将后面的多记录的位图置为空
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
	char *pData = xdpbuf+datalen;//指向缓冲区
	if (!m_pFirstRecord.ToBuffer(pData,tmplen,errmsg))
	{
		return false;
	}
	
	pData+=tmplen;//指向空白
	
	datalen += tmplen;
	tmplen = len - datalen;
	for (int i=0 ; i< (int)m_nRecCount-1; i++)
	{
		if (!m_vNextRecord[i]->ToBuffer(pData,tmplen,errmsg))
		{
			return false;
		}
		pData+=tmplen;//指向空白
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
		sprintf(errmsg,"没有此记录");
		return false;
	}
	if (rec-1 > m_vNextRecord.size())
	{
		sprintf(errmsg,"没有此记录，m_nRecCount错误，保存多记录结构数目不符");
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
		sprintf(errmsg,"没有此记录");
		return false;
	}
	if (rec-1 > m_vNextRecord.size())
	{
		sprintf(errmsg,"没有此记录，m_nRecCount错误，保存多记录结构数目不符");
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
		sprintf(errmsg,"没有此记录");
		return false;
	}
	if (rec-1 > m_vNextRecord.size())
	{
		sprintf(errmsg,"没有此记录，m_nRecCount错误，保存多记录结构数目不符");
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
		sprintf(errmsg,"没有此记录");
		return false;
	}
	if (rec-1 > m_vNextRecord.size())
	{
		sprintf(errmsg,"没有此记录，m_nRecCount错误，保存多记录结构数目不符");
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
		sprintf(errmsg,"没有此记录");
		return false;
	}
	if (rec-1 > m_vNextRecord.size())
	{
		sprintf(errmsg,"没有此记录，m_nRecCount错误，保存多记录结构数目不符");
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
        sprintf(errmsg, "没有此记录");
        return false;
    }
    if (rec - 1 > m_vNextRecord.size())
    {
        sprintf(errmsg, "没有此记录，m_nRecCount错误，保存多记录结构数目不符");
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
        sprintf(errmsg, "没有此记录");
        return false;
    }
    if (rec - 1 > m_vNextRecord.size())
    {
        sprintf(errmsg, "没有此记录，m_nRecCount错误，保存多记录结构数目不符");
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
		sprintf(errmsg,"没有此记录");
		return false;
	}
	if (rec-1 > m_vNextRecord.size())
	{
		sprintf(errmsg,"没有此记录，m_nRecCount错误，保存多记录结构数目不符");
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
		sprintf(errmsg,"没有此记录");
		return false;
	}
	if (rec-1 > m_vNextRecord.size())
	{
		sprintf(errmsg,"没有此记录，m_nRecCount错误，保存多记录结构数目不符");
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
		sprintf(errmsg,"没有此记录");
		return false;
	}
	if (rec-1 > m_vNextRecord.size())
	{
		sprintf(errmsg,"没有此记录，m_nRecCount错误，保存多记录结构数目不符");
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
		sprintf(errmsg,"没有此记录");
		return false;
	}
	if (rec-1 > m_vNextRecord.size())
	{
		sprintf(errmsg,"没有此记录，m_nRecCount错误，保存多记录结构数目不符");
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
		sprintf(errmsg,"没有此记录");
		return false;
	}
	if (rec-1 > m_vNextRecord.size())
	{
		sprintf(errmsg,"没有此记录，m_nRecCount错误，保存多记录结构数目不符");
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
		sprintf(errmsg,"没有此记录");
		return false;
	}
	if (rec-1 > m_vNextRecord.size())
	{
		sprintf(errmsg,"没有此记录，m_nRecCount错误，保存多记录结构数目不符");
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
        sprintf(errmsg, "没有此记录");
        return false;
    }
    if (rec - 1 > m_vNextRecord.size())
    {
        sprintf(errmsg, "没有此记录，m_nRecCount错误，保存多记录结构数目不符");
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
        sprintf(errmsg, "没有此记录");
        return false;
    }
    if (rec - 1 > m_vNextRecord.size())
    {
        sprintf(errmsg, "没有此记录，m_nRecCount错误，保存多记录结构数目不符");
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
		sprintf(errmsg,"没有此记录");
		return false;
	}
	if (rec-1 > m_vNextRecord.size())
	{
		sprintf(errmsg,"没有此记录，m_nRecCount错误，保存多记录结构数目不符");
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
		sprintf(errmsg,"最多255条记录");
		return false;
	}
	if (rec > m_vNextRecord.size()+2)
	{
		sprintf(errmsg,"增加记录时要按顺序增加");
		return false;
	}
	if (m_vNextRecord.size() < rec-1)
	{
		//增加一条
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
		sprintf(errmsg,"最多255条记录");
		return false;
	}
	if (rec > m_vNextRecord.size()+2)
	{
		sprintf(errmsg,"增加记录时要按顺序增加");
		return false;
	}
	if (m_vNextRecord.size() < rec-1)
	{
		//增加一条
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
		sprintf(errmsg,"最多255条记录");
		return false;
	}
	if (rec > m_vNextRecord.size()+2)
	{
		sprintf(errmsg,"增加记录时要按顺序增加");
		return false;
	}
	if (m_vNextRecord.size() < rec-1)
	{
		//增加一条
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
		sprintf(errmsg,"最多255条记录");
		return false;
	}
	if (rec > m_vNextRecord.size()+2)
	{
		sprintf(errmsg,"增加记录时要按顺序增加");
		return false;
	}
	if (m_vNextRecord.size() < rec-1)
	{
		//增加一条
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
		sprintf(errmsg,"最多255条记录");
		return false;
	}
	if (rec > m_vNextRecord.size()+2)
	{
		sprintf(errmsg,"增加记录时要按顺序增加");
		return false;
	}
	if (m_vNextRecord.size() < rec-1)
	{
		//增加一条
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
        sprintf(errmsg, "最多255条记录");
        return false;
    }
    if (rec > m_vNextRecord.size() + 2)
    {
        sprintf(errmsg, "增加记录时要按顺序增加");
        return false;
    }
    if (m_vNextRecord.size() < rec - 1)
    {
        //增加一条
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
        sprintf(errmsg, "最多255条记录");
        return false;
    }
    if (rec > m_vNextRecord.size() + 2)
    {
        sprintf(errmsg, "增加记录时要按顺序增加");
        return false;
    }
    if (m_vNextRecord.size() < rec - 1)
    {
        //增加一条
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
		sprintf(errmsg,"最多255条记录");
		return false;
	}
	if (rec > m_vNextRecord.size()+2)
	{
		sprintf(errmsg,"增加记录时要按顺序增加");
		return false;
	}
	if (m_vNextRecord.size() < rec-1)
	{
		//增加一条
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
		sprintf(errmsg,"最多255条记录");
		return false;
	}
	if (rec > m_vNextRecord.size()+2)
	{
		sprintf(errmsg,"增加记录时要按顺序增加");
		return false;
	}
	if (m_vNextRecord.size() < rec-1)
	{
		//增加一条
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
	//设置值
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
		sprintf(errmsg,"最多255条记录");
		return false;
	}
	if (rec > m_vNextRecord.size()+2)
	{
		sprintf(errmsg,"增加记录时要按顺序增加");
		return false;
	}
	if (m_vNextRecord.size() < rec-1)
	{
		//增加一条
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
	//设置值
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
		sprintf(errmsg,"最多255条记录");
		return false;
	}
	if (rec > m_vNextRecord.size()+2)
	{
		sprintf(errmsg,"增加记录时要按顺序增加");
		return false;
	}
	if (m_vNextRecord.size() < rec-1)
	{
		//增加一条
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
	//设置值
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
		sprintf(errmsg,"最多255条记录");
		return false;
	}
	if (rec > m_vNextRecord.size()+2)
	{
		sprintf(errmsg,"增加记录时要按顺序增加");
		return false;
	}
	if (m_vNextRecord.size() < rec-1)
	{
		//增加一条
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
	//设置值
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
		sprintf(errmsg,"最多255条记录");
		return false;
	}
	if (rec > m_vNextRecord.size()+2)
	{
		sprintf(errmsg,"增加记录时要按顺序增加");
		return false;
	}
	if (m_vNextRecord.size() < rec-1)
	{
		//增加一条
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
	//设置值
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
        sprintf(errmsg, "最多255条记录");
        return false;
    }
    if (rec > m_vNextRecord.size() + 2)
    {
        sprintf(errmsg, "增加记录时要按顺序增加");
        return false;
    }
    if (m_vNextRecord.size() < rec - 1)
    {
        //增加一条
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
    //设置值
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
        sprintf(errmsg, "最多255条记录");
        return false;
    }
    if (rec > m_vNextRecord.size() + 2)
    {
        sprintf(errmsg, "增加记录时要按顺序增加");
        return false;
    }
    if (m_vNextRecord.size() < rec - 1)
    {
        //增加一条
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
    //设置值
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
		sprintf(errmsg,"最多255条记录");
		return false;
	}
	if (rec > m_vNextRecord.size()+2)
	{
		sprintf(errmsg,"增加记录时要按顺序增加");
		return false;
	}
	if (m_vNextRecord.size() < rec-1)
	{
		//增加一条
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
	//设置值
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
		packsize+=sizeof(short);//记录数
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
				//增加一条
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






// 函数名: GetRealPackSize
// 编程  : 王明松 2014-10-30 11:31:20
// 返回  : unsigned short 
// 参数  : void *CXdpClass   xdp类的实例指针，由InitXdp获得
// 描述  : 取得当前实际报文的大小
unsigned short XDP_GetRealPackSize(void *CXdpClass)
{
	if (NULL == CXdpClass)
	{
		return 0;
	}
	CXdp *xdp = (CXdp *) CXdpClass;
	return xdp->GetRealPackSize();
}

// 函数名: CancelField
// 编程  : 王明松 2014-10-30 11:17:16
// 返回  : int 0成功 非0人败
// 参数  : void *CXdpClass   xdp类的实例指针，由InitXdp获得
// 参数  : unsigned short rec  从1开始
// 参数  : const char *fieldname
// 描述  : 取消字段，这样就不会生成此字段报文或取出值了
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

// 函数名: CancelField
// 编程  : 王明松 2014-10-30 11:16:28
// 返回  : int 0成功 非0人败
// 参数  : void *CXdpClass   xdp类的实例指针，由InitXdp获得
// 参数  : unsigned short rec   从1开始
// 参数  : unsigned short index
// 描述  : 取消字段，这样就不会生成此字段报文或取出值了
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

// 函数名: PrintXdp
// 编程  : 王明松 2014-10-30 11:15:33
// 返回  : void 
// 参数  : void *CXdpClass   xdp类的实例指针，由InitXdp获得
// 参数  : char *xdpbuf
// 描述  : 将报文内容显示出来，二进制数据转为16进制显示
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

// 函数名: GetCurSavePackNum
// 编程  : 王明松 2014-10-29 11:45:49
// 返回  : unsigned short 
// 参数  : void *CXdpClass   xdp类的实例指针，由InitXdp获得
// 描述  : 取得保存多记录的结构数目，注意不包括第一条记录
unsigned short XDP_GetCurSavePackNum(void *CXdpClass)
{
	if (NULL == CXdpClass)
	{
		return 0;
	}
	CXdp *xdp = (CXdp *) CXdpClass;
	return xdp->GetCurSavePackNum();
}

// 函数名: ClearNextRecord
// 编程  : 王明松 2014-10-29 11:34:21
// 返回  : void 
// 参数  : void *CXdpClass   xdp类的实例指针，由InitXdp获得
// 描述  : 清空保存多记录的结构
void XDP_ClearNextRecord(void *CXdpClass)
{
	if (NULL == CXdpClass)
	{
		return;
	}
	CXdp *xdp = (CXdp *) CXdpClass;
	xdp->ClearNextRecord();
}


// 函数名: GetFieldType
// 编程  : 王明松 2014-10-29 11:22:23
// 返回  : unsigned short 类型，见XdpFmt.h宏定义
// 参数  : void *CXdpClass   xdp类的实例指针，由InitXdp获得
// 参数  : const char *fieldname/unsigned short index 字段名称/索引
// 描述  : 取得字段类型
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

// 函数名: GetRecCount
// 编程  : 王明松 2014-10-29 11:10:29
// 返回  : unsigned short 记录数
// 参数  : void *CXdpClass   xdp类的实例指针，由InitXdp获得
// 描述  : 返回当前报文中共有几条记录，注意记录数和保存多记录的结构数目不是一个概念
unsigned short XDP_GetRecCount(void *CXdpClass)
{
	if (NULL == CXdpClass)
	{
		return 0;
	}
	CXdp *xdp = (CXdp *) CXdpClass;	
	return xdp->GetRecCount();
}

// 函数名: GetFieldValue
// 编程  : 王明松 2014-10-29 11:04:09
// 返回  : int 0成功 非0人败
// 参数  : void *CXdpClass   xdp类的实例指针，由InitXdp获得
// 参数  : unsigned short rec    第几条记录
// 参数  : unsigned short index/const char *fieldname  字段索引/名称
// 参数  : char *data      数据缓冲  当为数值型时，要用强制转换  如：int *aa = (int *)data;
// 输入输出参数  : unsigned int &datalen   输入为data缓冲大小 输出为实际数据长度
// 参数  : int &fieldtype  字段类型，见XdpFmt.h中宏定义
// 参数  : char *errmsg  出错信息
// 描述  : 从xdp中取字段值
int XDP_GetFieldValueByName(void *CXdpClass,unsigned short rec,const char *fieldname,char *data,unsigned int *datalen,int *fieldtype,char *errmsg)
{
	if (NULL == CXdpClass)
	{
		sprintf(errmsg,"CXdpClass为NULL");
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
		sprintf(errmsg,"CXdpClass为NULL");
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

// 函数名: GetFieldValue
// 编程  : 王明松 2014-10-29 11:11:55
// 返回  : int 0成功 非0人败
// 参数  : void *CXdpClass   xdp类的实例指针，由InitXdp获得
// 参数  : unsigned short index/const char *fieldname 字段索引/名称
// 参数  : char *data       数据缓冲 当为数值型时，要用强制转换来取值  如：int *aa = (int *)data;
// 参数  : unsigned int &datalen     输入为data缓冲大小 输出为实际数据长度
// 参数  : int &fieldtype  字段类型，见XdpFmt.h中宏定义
// 参数  : char *errmsg   出错信息
// 描述  : 从xdp中取第一个记录的字段值
int XDP_GetFirstFieldValueByIndex(void *CXdpClass,unsigned short index,char *data,unsigned int *datalen,int *fieldtype,char *errmsg)
{
	if (NULL == CXdpClass)
	{
		sprintf(errmsg,"CXdpClass为NULL");
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
		sprintf(errmsg,"CXdpClass为NULL");
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



// 函数名: SetFieldValue
// 编程  : 王明松 2014-10-29 11:40:30
// 返回  : int 0成功 非0人败
// 参数  : void *CXdpClass   xdp类的实例指针，由InitXdp获得
// 参数  : unsigned short rec    要设置为记录数，注意若rec大于原记录数那么记录数要从小到大按顺序设置
// 参数  : unsigned short index/const char *fieldname 字段索引/名称
// 参数  : const char *data   数据缓冲 当为数值型时，要用强制转换来置值  如：int *aa = (int *)data; *aa = 1200
// 参数  : int datalen     数据长度
// 参数  : char *errmsg    出错信息
// 描述  : 设置指定记录数的字段值
int XDP_SetFieldValueByName(void *CXdpClass,unsigned short rec,const char *fieldname,const char *data,unsigned int datalen,char *errmsg)
{
	if (NULL == CXdpClass)
	{
		sprintf(errmsg,"CXdpClass为NULL");
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
		sprintf(errmsg,"CXdpClass为NULL");
		return -1;
	}
	CXdp *xdp = (CXdp *) CXdpClass;	
	if (!xdp->SetFieldValue(rec,index,data,datalen,errmsg))
	{
		return -1;
	}
	return 0;
}



// 函数名: SetFieldValue
// 编程  : 王明松 2014-10-29 11:40:35
// 返回  : int 0成功 非0人败
// 参数  : void *CXdpClass   xdp类的实例指针，由InitXdp获得
// 参数  : unsigned short index/const char *fieldname 字段索引/名称  
// 参数  : const char *data   数据缓冲 当为数值型时，要用强制转换来置值  如：int *aa = (int *)data; *aa = 1200
// 参数  : int datalen    数据长度
// 参数  : char *errmsg  出错信息
// 描述  : 设置第一条记录的字段值
int XDP_SetFirstFieldValueByIndex(void *CXdpClass,unsigned short index,const char *data,unsigned int datalen,char *errmsg)
{
	if (NULL == CXdpClass)
	{
		sprintf(errmsg,"CXdpClass为NULL");
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
		sprintf(errmsg,"CXdpClass为NULL");
		return -1;
	}
	CXdp *xdp = (CXdp *) CXdpClass;	
	if (!xdp->SetFieldValue(fieldname,data,datalen,errmsg))
	{
		return -1;
	}
	return 0;
}


// 函数名: SetCurRecCount
// 编程  : 王明松 2014-10-29 10:55:12
// 返回  : int 0成功 非0人败，当原报文的记录Y<X时失败
// 参数  : void *CXdpClass   xdp类的实例指针，由InitXdp获得
// 参数  : unsigned short rec 
// 描述  : 设置当前的记录数，当原报文中有Y条记录时，本次只返回X(X<Y)条记录时，必须重置记录数为X,否则返回的仍为Y条
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

// 函数名: ResetData
// 编程  : 王明松 2014-10-27 14:24:43
// 返回  : void 
// 参数  : void *CXdpClass   xdp类的实例指针，由InitXdp获得
// 描述  : 清除多记录，重置位图
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

// 函数名: ToBuffer
// 编程  : 王明松 2014-10-27 14:18:52
// 返回  : int 0成功 非0人败
// 参数  : void *CXdpClass   xdp类的实例指针，由InitXdp获得
// 参数  : char *xdpbuf   xdp报文
// 输入输出参数  : int *len    输入为xdpbuf大小,输出为xdp报文大小
// 参数  : char *errmsg   出错信息
// 描述  : 生成xdp报文
int XDP_ToBuffer(void *CXdpClass,char *xdpbuf,unsigned int *len,char *errmsg)
{
	if (NULL == CXdpClass)
	{
		sprintf(errmsg,"CXdpClass为NULL");
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

// 函数名: FromBuffer
// 编程  : 王明松 2014-10-27 14:17:34
// 返回  : int 0成功 非0人败
// 参数  : void *CXdpClass   xdp类的实例指针，由InitXdp获得
// 参数  : const char *xdpbuf   xdp数据缓冲
// 参数  : int xdpbuflen   xdp数据长度
// 参数  : char *errmsg   出错信息
// 描述  : 从数据缓冲解析报文
int XDP_FromBuffer(void *CXdpClass,const char *xdpbuf,unsigned int xdpbuflen,char *errmsg)
{
	if (NULL == CXdpClass)
	{
		sprintf(errmsg,"CXdpClass为NULL");
		return -1;
	}
	CXdp *xdp = (CXdp *) CXdpClass;	
	if (!xdp->FromBuffer(xdpbuf,xdpbuflen,errmsg))
	{
		return -1;
	}
	return 0;
}

// 函数名: InitXdp
// 编程  : 王明松 2014-10-27 14:14:55
// 返回  : int 0成功 非0人败
// 输入输出参数  : void **CXdpClass   输入为指针地址，输出为xdp类的实例指针
// 参数  : const char *datafile   xml配置文件
// 参数  : char *errmsg      出错信息
// 描述  : 初始化报文，
int XDP_InitXdp(void **CXdpClass,const char *datafile,char *errmsg)
{
	CXdp *xdp = new CXdp();
	if (NULL == xdp)
	{
		sprintf(errmsg,"new CXdp失败");
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

// 函数名: FreeXdp
// 编程  : 王明松 2014-11-7 11:19:43
// 返回  : void
// 参数  : void *CXdpClass
// 描述  : 释放xdp类的实例
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

//封装给c#用的c方法



// 函数名: GetRealPackSize
// 编程  : 王明松 2014-10-30 11:31:20
// 返回  : unsigned short 
// 参数  : unsigned int xdpindex   xdp类的实例下标，由InitXdp获得
// 描述  : 取得当前实际报文的大小
unsigned short XDPI_GetRealPackSize(unsigned int xdpindex)
{
    if (xdpindex > g_XdpArray.size() - 1)
    {
        return 0;
    }
    CXdp* xdp = g_XdpArray[xdpindex];
    return xdp->GetRealPackSize();
}

// 函数名: CancelField
// 编程  : 王明松 2014-10-30 11:17:16
// 返回  : int 0成功 非0人败
// 参数  : unsigned int xdpindex   xdp类的实例下标，由InitXdp获得
// 参数  : unsigned short rec  从1开始
// 参数  : const char *fieldname
// 描述  : 取消字段，这样就不会生成此字段报文或取出值了
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

// 函数名: CancelField
// 编程  : 王明松 2014-10-30 11:16:28
// 返回  : int 0成功 非0人败
// 参数  : unsigned int xdpindex   xdp类的实例下标，由InitXdp获得
// 参数  : unsigned short rec   从1开始
// 参数  : unsigned short index
// 描述  : 取消字段，这样就不会生成此字段报文或取出值了
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

// 函数名: PrintXdp
// 编程  : 王明松 2014-10-30 11:15:33
// 返回  : void 
// 参数  : unsigned int xdpindex   xdp类的实例下标，由InitXdp获得
// 参数  : char *xdpbuf
// 描述  : 将报文内容显示出来，二进制数据转为16进制显示
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

// 函数名: GetCurSavePackNum
// 编程  : 王明松 2014-10-29 11:45:49
// 返回  : unsigned short 
// 参数  : unsigned int xdpindex   xdp类的实例下标，由InitXdp获得
// 描述  : 取得保存多记录的结构数目，注意不包括第一条记录
unsigned short XDPI_GetCurSavePackNum(unsigned int xdpindex)
{
    if (xdpindex > g_XdpArray.size() - 1)
    {
        return 0;
    }
    CXdp* xdp = g_XdpArray[xdpindex];
    return xdp->GetCurSavePackNum();
}

// 函数名: ClearNextRecord
// 编程  : 王明松 2014-10-29 11:34:21
// 返回  : void 
// 参数  : unsigned int xdpindex   xdp类的实例下标，由InitXdp获得
// 描述  : 清空保存多记录的结构
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


// 函数名: GetFieldType
// 编程  : 王明松 2014-10-29 11:22:23
// 返回  : unsigned short 类型，见XdpFmt.h宏定义
// 参数  : unsigned int xdpindex   xdp类的实例下标，由InitXdp获得
// 参数  : const char *fieldname/unsigned short index 字段名称/索引
// 描述  : 取得字段类型
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

// 函数名: GetRecCount
// 编程  : 王明松 2014-10-29 11:10:29
// 返回  : unsigned short 记录数
// 参数  : unsigned int xdpindex   xdp类的实例下标，由InitXdp获得
// 描述  : 返回当前报文中共有几条记录，注意记录数和保存多记录的结构数目不是一个概念
unsigned short XDPI_GetRecCount(unsigned int xdpindex)
{
    if (xdpindex > g_XdpArray.size() - 1)
    {
        return 0;
    }
    CXdp* xdp = g_XdpArray[xdpindex];
    return xdp->GetRecCount();
}

// 函数名: GetFieldValue
// 编程  : 王明松 2014-10-29 11:04:09
// 返回  : int 0成功 非0人败
// 参数  : unsigned int xdpindex   xdp类的实例下标，由InitXdp获得
// 参数  : unsigned short rec    第几条记录
// 参数  : unsigned short index/const char *fieldname  字段索引/名称
// 参数  : char *data      数据缓冲  当为数值型时，要用强制转换  如：int *aa = (int *)data;
// 输入输出参数  : unsigned int &datalen   输入为data缓冲大小 输出为实际数据长度
// 参数  : int &fieldtype  字段类型，见XdpFmt.h中宏定义
// 参数  : char *errmsg  出错信息
// 描述  : 从xdp中取字段值
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

// 函数名: GetFieldValue
// 编程  : 王明松 2014-10-29 11:11:55
// 返回  : int 0成功 非0人败
// 参数  : unsigned int xdpindex   xdp类的实例下标，由InitXdp获得
// 参数  : unsigned short index/const char *fieldname 字段索引/名称
// 参数  : char *data       数据缓冲 当为数值型时，要用强制转换来取值  如：int *aa = (int *)data;
// 参数  : unsigned int &datalen     输入为data缓冲大小 输出为实际数据长度
// 参数  : int &fieldtype  字段类型，见XdpFmt.h中宏定义
// 参数  : char *errmsg   出错信息
// 描述  : 从xdp中取第一个记录的字段值
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



// 函数名: SetFieldValue
// 编程  : 王明松 2014-10-29 11:40:30
// 返回  : int 0成功 非0人败
// 参数  : unsigned int xdpindex   xdp类的实例下标，由InitXdp获得
// 参数  : unsigned short rec    要设置为记录数，注意若rec大于原记录数那么记录数要从小到大按顺序设置
// 参数  : unsigned short index/const char *fieldname 字段索引/名称
// 参数  : const char *data   数据缓冲 当为数值型时，要用强制转换来置值  如：int *aa = (int *)data; *aa = 1200
// 参数  : int datalen     数据长度
// 参数  : char *errmsg    出错信息
// 描述  : 设置指定记录数的字段值
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



// 函数名: SetFieldValue
// 编程  : 王明松 2014-10-29 11:40:35
// 返回  : int 0成功 非0人败
// 参数  : unsigned int xdpindex   xdp类的实例下标，由InitXdp获得
// 参数  : unsigned short index/const char *fieldname 字段索引/名称  
// 参数  : const char *data   数据缓冲 当为数值型时，要用强制转换来置值  如：int *aa = (int *)data; *aa = 1200
// 参数  : int datalen    数据长度
// 参数  : char *errmsg  出错信息
// 描述  : 设置第一条记录的字段值
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


// 函数名: SetCurRecCount
// 编程  : 王明松 2014-10-29 10:55:12
// 返回  : int 0成功 非0人败，当原报文的记录Y<X时失败
// 参数  : unsigned int xdpindex   xdp类的实例下标，由InitXdp获得
// 参数  : unsigned short rec 
// 描述  : 设置当前的记录数，当原报文中有Y条记录时，本次只返回X(X<Y)条记录时，必须重置记录数为X,否则返回的仍为Y条
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

// 函数名: ResetData
// 编程  : 王明松 2014-10-27 14:24:43
// 返回  : void 
// 参数  : unsigned int xdpindex   xdp类的实例下标，由InitXdp获得
// 描述  : 清除多记录，重置位图
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

// 函数名: ToBuffer
// 编程  : 王明松 2014-10-27 14:18:52
// 返回  : int 0成功 非0人败
// 参数  : unsigned int xdpindex   xdp类的实例下标，由InitXdp获得
// 参数  : char *xdpbuf   xdp报文
// 输入输出参数  : int *len    输入为xdpbuf大小,输出为xdp报文大小
// 参数  : char *errmsg   出错信息
// 描述  : 生成xdp报文
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

// 函数名: FromBuffer
// 编程  : 王明松 2014-10-27 14:17:34
// 返回  : int 0成功 非0人败
// 参数  : unsigned int xdpindex   xdp类的实例下标，由InitXdp获得
// 参数  : const char *xdpbuf   xdp数据缓冲
// 参数  : int xdpbuflen   xdp数据长度
// 参数  : char *errmsg   出错信息
// 描述  : 从数据缓冲解析报文
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

// 函数名: InitXdp
// 编程  : 王明松 2014-10-27 14:14:55
// 返回  : int >=0成功 xdp类的实例下标   <0失败
// 参数  : const char *datafile   xml配置文件
// 参数  : char *errmsg      出错信息
// 描述  : 初始化报文，
int XDPI_InitXdp(const char* datafile, char* errmsg)
{
    CXdp* xdp = new CXdp();
    if (NULL == xdp)
    {
        sprintf(errmsg, "new CXdp失败");
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

// 函数名: FreeXdp
// 编程  : 王明松 2014-11-7 11:19:43
// 返回  : void
// 参数  :  unsigned int xdpindex   xdp类的实例下标，由InitXdp获得
// 描述  : 释放xdp类的实例
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