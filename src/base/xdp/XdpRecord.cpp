// XdpRecord.cpp: implementation of the CXdpRecord class.
//
//////////////////////////////////////////////////////////////////////

#include "XdpRecord.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXdpRecord::CXdpRecord()
{
	m_pXdpFmt = NULL;
	memset(m_sBitMap,0,XDP_BITMAPSIZE);
	m_nRealLen = XDP_BITMAPSIZE;
	m_pPackData = NULL;
	m_nPackDataMaxLen = 0;
	m_tField = NULL;
	
}

CXdpRecord::~CXdpRecord()
{
	m_pXdpFmt = NULL;
	if (m_pPackData != NULL)
	{
		free(m_pPackData);
		m_pPackData = NULL;
	}
	m_tField = NULL;
}

bool CXdpRecord::Init(CXdpFmt *pfmt)
{
	m_pXdpFmt = pfmt;
	m_nPackDataMaxLen = m_pXdpFmt->GetMaxLength()+1;
	if (m_pPackData != NULL)
	{
		free(m_pPackData);
		m_pPackData = NULL;
	}
	m_pPackData = (char *)malloc(m_nPackDataMaxLen);
	if (m_pPackData == NULL)
	{
		return false;
	}
	bzero(m_pPackData,m_nPackDataMaxLen);
	return true;
}

bool CXdpRecord::FromBuffer(const char *xdpdata,unsigned int datalen, char *errmsg)
{
	//基于性能，不用清除
//	bzero(m_pPackData,m_nPackDataMaxLen);
	char      *pPackData = m_pPackData;
	m_nRealLen = 0;
	if (datalen < XDP_BITMAPSIZE)
	{
		sprintf(errmsg,"报文长度不符[%d < %d]",datalen,XDP_BITMAPSIZE);
		return false;
	}
	//首先取得位图
	memcpy(m_sBitMap,xdpdata,XDP_BITMAPSIZE);
	
	char *pData = (char *)xdpdata+ XDP_BITMAPSIZE;
	m_nRealLen = XDP_BITMAPSIZE;
	for (i=0 ; i< m_pXdpFmt->GetFieldNum() ; i++)
	{
		if  (IsFieldSet(i))
		{
			m_tField = m_pXdpFmt->At(i);
			if (m_tField == NULL)
			{
				sprintf(errmsg,"无此字段index[%d]",i);
				return false;
			}
			//若是可变长字段，取得可变长部分的实际长度
			if (m_tField->f_extlength>0) //变长
			{
				if (1 == m_tField->f_extlength) //只有一个字节的长度
				{
					m_tvclen=(unsigned char )pData[0];
					if ((datalen-m_nRealLen) < (m_tField->f_extlength+m_tvclen))
					{
						sprintf(errmsg,"报文长度不符");
						return false;
					}
					m_nRealLen+=m_tField->f_extlength;
					m_nRealLen+=m_tvclen;
					memcpy(m_pPackData+m_tField->f_offset,pData,1+m_tvclen);
				}
				else  //字节序转换
				{
					//复制字节长度并转换为本机所用字节序
					memcpy(m_pPackData+m_tField->f_offset,pData,m_tField->f_extlength);
					CBF_Tools::Reverse((unsigned char *)(m_pPackData+m_tField->f_offset),m_tField->f_extlength);

					m_tus = (unsigned short *)((unsigned char *)(m_pPackData+m_tField->f_offset));
					if (*m_tus > m_tField->f_length)
					{
						sprintf(errmsg,"字段[%d %s]长度[%d]不符",m_tField->f_index,m_tField->f_name,*m_tus);
						return false;
					}
					if ((datalen-m_nRealLen) < (m_tField->f_extlength+*m_tus))
					{
						sprintf(errmsg,"报文长度不符");
						return false;
					}
					if (*m_tus>0)
					{
						memcpy(m_pPackData+m_tField->f_offset+m_tField->f_extlength,pData+m_tField->f_extlength,*m_tus);
					}
					m_nRealLen+=m_tField->f_extlength;
					m_nRealLen+=*m_tus;
				}
			}
			else  //定长
			{
				if ((datalen-m_nRealLen) < (m_tField->f_length+m_tField->f_extlength))
				{
					sprintf(errmsg,"报文长度不符");
					return false;
				}
				memcpy(m_pPackData+m_tField->f_offset,pData,m_tField->f_length);
				m_nRealLen+=m_tField->f_length;
			}
			pData= (char *)xdpdata+m_nRealLen;
		}
		
	}
	return true;
}

void CXdpRecord::ResetData()
{
	memset(m_sBitMap,0,XDP_BITMAPSIZE);
	m_sBitMap[XDP_BITMAPSIZE]=0;
	m_nRealLen = 0;
}

unsigned short CXdpRecord::GetRealLen()
{
	return m_nRealLen;
}

bool CXdpRecord::ToBuffer(char *xdpbuf,unsigned int &len, char *errmsg)
{
	if (len < XDP_BITMAPSIZE + m_nRealLen)
	{
		sprintf(errmsg,"缓冲长度不足[%d < %d]",len,XDP_BITMAPSIZE+m_nRealLen);
		return false;
	}
	//首先取得位图
	memcpy(xdpbuf,m_sBitMap,XDP_BITMAPSIZE);
	int datalen = XDP_BITMAPSIZE;

	char *poffset = xdpbuf+XDP_BITMAPSIZE;
	
	for (i=0 ; i< m_pXdpFmt->GetFieldNum() ; i++)
	{
		if (!IsFieldSet(i))
		{
			continue;
		}
		m_tField = m_pXdpFmt->At(i);
		if (m_tField == NULL)
		{
			sprintf(errmsg,"无此字段index[%d]",i);
			return false;
		}
		if (m_tField->f_extlength>0) //变长
		{
			if (1 == m_tField->f_extlength)
			{
				m_tvclen = (unsigned char )((m_pPackData+m_tField->f_offset)[0]);
				if (len < datalen+m_tField->f_extlength+m_tvclen)
				{
					sprintf(errmsg,"报文缓冲长度不足[%d < %d+index%d-%d]",len,datalen,i,m_tField->f_extlength+m_tvclen);
					return false;
				}
				
				memcpy(poffset,m_pPackData+m_tField->f_offset,m_tField->f_extlength+m_tvclen);
				poffset+=m_tField->f_extlength;
				poffset+=m_tvclen;
				datalen+=m_tField->f_extlength;
				datalen+=m_tvclen;
			}
			else  //两个字节长度
			{
				m_tus = (unsigned short *)((unsigned char *)(m_pPackData+m_tField->f_offset));
				if (len < datalen+m_tField->f_extlength+*m_tus)
				{
					sprintf(errmsg,"报文缓冲长度不足[%d < %d+index%d-%d]",len,datalen,i,m_tField->f_extlength+*m_tus);
					return false;
				}
				//复制长度，将长度转成通讯的字节序
				memcpy(poffset,m_pPackData+m_tField->f_offset,m_tField->f_extlength);
				CBF_Tools::Reverse((unsigned char *)poffset,m_tField->f_extlength);

				poffset+=m_tField->f_extlength;
				datalen+=m_tField->f_extlength;
				if ( *m_tus >0)
				{
					memcpy(poffset,m_pPackData+m_tField->f_offset+m_tField->f_extlength,*m_tus);
					poffset+=*m_tus;
					datalen+=*m_tus;
				}
			}
		}
		else  //定长
		{
			if (len < datalen+m_tField->f_length)
			{
				sprintf(errmsg,"报文缓冲长度不足[%d < %d+index%d-%d]",len,datalen,i,m_tField->f_length);
				return false;
			}
			memcpy(poffset,m_pPackData+m_tField->f_offset,m_tField->f_length);
			datalen+=m_tField->f_length;
			poffset+=m_tField->f_length;
			
		}
	}
	len = datalen;
	return true;
}
bool CXdpRecord::SetFieldData(S_XDP_FIELD_DEFINE *field,const char *data,unsigned int datalen,char *errmsg)
{
	int oldlen=0;
	unsigned short *olen;
	switch (field->f_type)
	{
		case XDP_USHORT:
		case XDP_SHORT:
			if (datalen != sizeof(short))
			{
				sprintf(errmsg,"字段index[%d]为短整型，但传入数据长度不为%d",field->f_index,sizeof(short));
				return false;
			}
			memcpy(m_pPackData+field->f_offset,data,datalen);
			CBF_Tools::Reverse((unsigned char *)(m_pPackData+field->f_offset),datalen);
			if (!IsFieldSet(field->f_index))
			{
				m_nRealLen +=datalen;
			}
			break;
		case XDP_UINT:
		case XDP_INT:
			if (datalen != sizeof(int))
			{
				sprintf(errmsg,"字段index[%d]为整型，但传入数据长度不为%d",field->f_index,sizeof(int));
				return false;
			}
			memcpy(m_pPackData+field->f_offset,data,datalen);
			CBF_Tools::Reverse((unsigned char *)(m_pPackData+field->f_offset),datalen);
			if (!IsFieldSet(field->f_index))
			{
				m_nRealLen +=datalen;
			}
			break;
		case XDP_DOUBLE:
			if (datalen != sizeof(double))
			{
				sprintf(errmsg,"字段index[%d]为浮点型，但传入数据长度不为%d",field->f_index,sizeof(double));
				return false;
			}
			memcpy(m_pPackData+field->f_offset,data,datalen);
			CBF_Tools::Reverse((unsigned char *)(m_pPackData+field->f_offset),datalen);
			if (!IsFieldSet(field->f_index))
			{
				m_nRealLen +=datalen;
			}
			break;
        case XDP_INT64:
            if (datalen != sizeof(INT64_))
            {
                sprintf(errmsg, "字段index[%d]为INT64_，但传入数据长度不为%d", field->f_index, sizeof(INT64_));
                return false;
            }
            memcpy(m_pPackData + field->f_offset, data, datalen);
            CBF_Tools::Reverse((unsigned char*)(m_pPackData + field->f_offset), datalen);
            if (!IsFieldSet(field->f_index))
            {
                m_nRealLen += datalen;
            }
            break;
        case XDP_UINT64:
            if (datalen != sizeof(UINT64_))
            {
                sprintf(errmsg, "字段index[%d]为UINT64_，但传入数据长度不为%d", field->f_index, sizeof(UINT64_));
                return false;
            }
            memcpy(m_pPackData + field->f_offset, data, datalen);
            CBF_Tools::Reverse((unsigned char*)(m_pPackData + field->f_offset), datalen);
            if (!IsFieldSet(field->f_index))
            {
                m_nRealLen += datalen;
            }
            break;
		case XDP_CHAR:
			if (datalen >= field->f_length)
			{
				sprintf(errmsg,"字段index[%d]为字符型，但传入数据长度超限%d",field->f_index,datalen);
				return false;
			}
			if (datalen >0)
			{
				memcpy(m_pPackData+field->f_offset,data,datalen);
				m_pPackData[field->f_offset+datalen] = 0;//加入结束符
			}
			if (!IsFieldSet(field->f_index))
			{
				m_nRealLen +=field->f_length;
			}
			break;
		case XDP_VARCHAR:
			if (datalen > field->f_length)
			{
				sprintf(errmsg,"字段index[%d]为变长字符型，但传入数据长度超限%d",field->f_index,datalen);
				return false;
			}
			oldlen = m_pPackData[field->f_offset];
			memcpy(m_pPackData+field->f_offset+field->f_extlength,data,datalen);
			m_pPackData[field->f_offset] = datalen;
			if (!IsFieldSet(field->f_index))
			{
				m_nRealLen = m_nRealLen+datalen+field->f_extlength;
			}
			else
			{
				m_nRealLen = m_nRealLen - oldlen +datalen;
			}
			break;
		case XDP_BINDATA:
			if (datalen > field->f_length)
			{
				sprintf(errmsg,"字段index[%d]为变长二进制型，但传入数据长度超限%d",field->f_index,datalen);
				return false;
			}
			
			if (!IsFieldSet(field->f_index))
			{
				memcpy(m_pPackData+field->f_offset+field->f_extlength,data,datalen);
				olen = (unsigned short *)(m_pPackData+field->f_offset);
				*olen = datalen;
				m_nRealLen = m_nRealLen+datalen+field->f_extlength;
			}
			else
			{
				olen = (unsigned short *)(m_pPackData+field->f_offset);
				oldlen = *olen;
				memcpy(m_pPackData+field->f_offset+field->f_extlength,data,datalen);
				*olen = datalen;
				m_nRealLen = m_nRealLen - oldlen +datalen;
			}
			break;
		default:
			sprintf(errmsg,"字段类型不符");
			return false;
	}
	SetFieldMap(field->f_index,true);
	
	return true;
}
bool CXdpRecord::SetFieldData(unsigned short index, INT64_ idata, char* errmsg)
{
    S_XDP_FIELD_DEFINE* field = m_pXdpFmt->At(index);
    if (field == NULL)
    {
        sprintf(errmsg, "无此字段index[%d]", index);
        return false;
    }
    if (field->f_type != XDP_INT64 && field->f_type != XDP_UINT64)
    {
        sprintf(errmsg, "字段index[%d] 非XDP_INT64或XDP_UINT64", index);
        return false;
    }
    char data[50];
    unsigned int datalen = sizeof(INT64_);
	INT64_* aa = (INT64_*)data;
    *aa = idata;
    return SetFieldData(field, data, datalen, errmsg);
}
bool CXdpRecord::SetFieldData(unsigned short index, UINT64_ idata, char* errmsg)
{
    S_XDP_FIELD_DEFINE* field = m_pXdpFmt->At(index);
    if (field == NULL)
    {
        sprintf(errmsg, "无此字段index[%d]", index);
        return false;
    }
    if (field->f_type != XDP_UINT64 && field->f_type != XDP_INT64)
    {
        sprintf(errmsg, "字段index[%d] 非XDP_UINT或XDP_INT64", index);
        return false;
    }
    char data[50];
    unsigned int datalen = sizeof(UINT64_);
	UINT64_* aa = (UINT64_*)data;
    *aa = idata;
    return SetFieldData(field, data, datalen, errmsg);
}

bool CXdpRecord::SetFieldData(unsigned short index,int idata,char *errmsg)
{
	S_XDP_FIELD_DEFINE *field = m_pXdpFmt->At(index);
	if (field == NULL)
	{
		sprintf(errmsg,"无此字段index[%d]",index);
		return false;
	}
	if (field->f_type != XDP_INT && field->f_type != XDP_UINT)
	{
		sprintf(errmsg,"字段index[%d] 非XDP_INT或XDP_UINT",index);
		return false;
	}
	char data[50];
	unsigned int datalen = sizeof(int);
	int *aa = (int *)data;
	*aa = idata;
	return SetFieldData(field,data,datalen,errmsg);
}
bool CXdpRecord::SetFieldData(unsigned short index,unsigned int idata,char *errmsg)
{
	S_XDP_FIELD_DEFINE *field = m_pXdpFmt->At(index);
	if (field == NULL)
	{
		sprintf(errmsg,"无此字段index[%d]",index);
		return false;
	}
	if (field->f_type != XDP_UINT && field->f_type != XDP_INT)
	{
		sprintf(errmsg,"字段index[%d] 非XDP_UINT或XDP_INT",index);
		return false;
	}
	char data[50];
	unsigned int datalen = sizeof(unsigned int);
	unsigned int *aa = (unsigned int *)data;
	*aa = idata;
	return SetFieldData(field,data,datalen,errmsg);
}
bool CXdpRecord::SetFieldData(unsigned short index,unsigned short idata,char *errmsg)
{
	S_XDP_FIELD_DEFINE *field = m_pXdpFmt->At(index);
	if (field == NULL)
	{
		sprintf(errmsg,"无此字段index[%d]",index);
		return false;
	}
	if (field->f_type != XDP_USHORT && field->f_type != XDP_SHORT)
	{
		sprintf(errmsg,"字段index[%d] 非XDP_USHORT或XDP_SHORT",index);
		return false;
	}
	char data[50];
	unsigned int datalen = sizeof(unsigned short);
	unsigned short *aa = (unsigned short *)data;
	*aa = idata;
	return SetFieldData(field,data,datalen,errmsg);
}
bool CXdpRecord::SetFieldData(unsigned short index,short idata,char *errmsg)
{
	S_XDP_FIELD_DEFINE *field = m_pXdpFmt->At(index);
	if (field == NULL)
	{
		sprintf(errmsg,"无此字段index[%d]",index);
		return false;
	}
	if (field->f_type != XDP_SHORT && field->f_type != XDP_USHORT)
	{
		sprintf(errmsg,"字段index[%d] 非XDP_SHORT或XDP_USHORT",index);
		return false;
	}
	char data[50];
	unsigned int datalen = sizeof(short);
	short *aa = (short *)data;
	*aa = idata;
	return SetFieldData(field,data,datalen,errmsg);
}
bool CXdpRecord::SetFieldData(unsigned short index,double idata,char *errmsg)
{
	S_XDP_FIELD_DEFINE *field = m_pXdpFmt->At(index);
	if (field == NULL)
	{
		sprintf(errmsg,"无此字段index[%d]",index);
		return false;
	}
	if (field->f_type != XDP_DOUBLE)
	{
		sprintf(errmsg,"字段index[%d] 非XDP_DOUBLE",index);
		return false;
	}
	char data[50];
	unsigned int datalen = sizeof(double);
	double *aa = (double *)data;
	*aa = idata;
	return SetFieldData(field,data,datalen,errmsg);
}
bool CXdpRecord::SetFieldData(unsigned short index, const char *data, unsigned int datalen,char *errmsg)
{
	S_XDP_FIELD_DEFINE *field = m_pXdpFmt->At(index);
	if (field == NULL)
	{
		sprintf(errmsg,"无此字段index[%d]",index);
		return false;
	}
	return SetFieldData(field,data,datalen,errmsg);
}
bool CXdpRecord::SetFieldData(const char* fieldname, INT64_ idata, char* errmsg)
{
    S_XDP_FIELD_DEFINE* field = m_pXdpFmt->GetField(fieldname);
    if (field == NULL)
    {
        sprintf(errmsg, "无此字段fieldname[%s]", fieldname);
        return false;
    }
    if (field->f_type != XDP_INT64 && field->f_type != XDP_UINT64)
    {
        sprintf(errmsg, "字段fieldname[%s] 非XDP_INT64或XDP_UINT64", fieldname);
        return false;
    }
    char data[50];
    unsigned int datalen = sizeof(INT64_);
	INT64_* aa = (INT64_*)data;
    *aa = idata;
    return SetFieldData(field, data, datalen, errmsg);
}
bool CXdpRecord::SetFieldData(const char* fieldname, UINT64_ idata, char* errmsg)
{
    S_XDP_FIELD_DEFINE* field = m_pXdpFmt->GetField(fieldname);
    if (field == NULL)
    {
        sprintf(errmsg, "无此字段fieldname[%s]", fieldname);
        return false;
    }
    if (field->f_type != XDP_UINT64 && field->f_type != XDP_INT64)
    {
        sprintf(errmsg, "字段fieldname[%s] 非XDP_UINT64或XDP_INT64", fieldname);
        return false;
    }
    char data[50];
    unsigned int datalen = sizeof(UINT64_);
	UINT64_* aa = (UINT64_*)data;
    *aa = idata;
    return SetFieldData(field, data, datalen, errmsg);
}
bool CXdpRecord::SetFieldData(const char *fieldname,int idata,char *errmsg)
{
	S_XDP_FIELD_DEFINE *field = m_pXdpFmt->GetField(fieldname);
	if (field == NULL)
	{
		sprintf(errmsg,"无此字段fieldname[%s]",fieldname);
		return false;
	}
	if (field->f_type != XDP_INT && field->f_type != XDP_UINT)
	{
		sprintf(errmsg,"字段fieldname[%s] 非XDP_INT或XDP_UINT",fieldname);
		return false;
	}
	char data[50];
	unsigned int datalen = sizeof(int);
	int *aa = (int *)data;
	*aa = idata;
	return SetFieldData(field,data,datalen,errmsg);
}
bool CXdpRecord::SetFieldData(const char *fieldname,unsigned int idata,char *errmsg)
{
	S_XDP_FIELD_DEFINE *field = m_pXdpFmt->GetField(fieldname);
	if (field == NULL)
	{
		sprintf(errmsg,"无此字段fieldname[%s]",fieldname);
		return false;
	}
	if (field->f_type != XDP_UINT && field->f_type != XDP_INT)
	{
		sprintf(errmsg,"字段fieldname[%s] 非XDP_UINT或XDP_INT",fieldname);
		return false;
	}
	char data[50];
	unsigned int datalen = sizeof(unsigned int);
	unsigned int *aa = (unsigned int *)data;
	*aa = idata;
	return SetFieldData(field,data,datalen,errmsg);
}
bool CXdpRecord::SetFieldData(const char *fieldname,unsigned short idata,char *errmsg)
{
	S_XDP_FIELD_DEFINE *field = m_pXdpFmt->GetField(fieldname);
	if (field == NULL)
	{
		sprintf(errmsg,"无此字段fieldname[%s]",fieldname);
		return false;
	}
	if (field->f_type != XDP_USHORT && field->f_type != XDP_SHORT)
	{
		sprintf(errmsg,"字段fieldname[%s] 非XDP_USHORT或XDP_SHORT",fieldname);
		return false;
	}
	char data[50];
	unsigned int datalen = sizeof(unsigned short);
	unsigned short *aa = (unsigned short *)data;
	*aa = idata;
	return SetFieldData(field,data,datalen,errmsg);
}
bool CXdpRecord::SetFieldData(const char *fieldname,short idata,char *errmsg)
{
	S_XDP_FIELD_DEFINE *field = m_pXdpFmt->GetField(fieldname);
	if (field == NULL)
	{
		sprintf(errmsg,"无此字段fieldname[%s]",fieldname);
		return false;
	}
	if (field->f_type != XDP_SHORT && field->f_type != XDP_USHORT)
	{
		sprintf(errmsg,"字段fieldname[%s] 非XDP_SHORT或XDP_USHORT",fieldname);
		return false;
	}
	char data[50];
	unsigned int datalen = sizeof(short);
	short *aa = (short *)data;
	*aa = idata;
	return SetFieldData(field,data,datalen,errmsg);
}
bool CXdpRecord::SetFieldData(const char *fieldname,double idata,char *errmsg)
{
	S_XDP_FIELD_DEFINE *field = m_pXdpFmt->GetField(fieldname);
	if (field == NULL)
	{
		sprintf(errmsg,"无此字段fieldname[%s]",fieldname);
		return false;
	}
	if (field->f_type != XDP_DOUBLE)
	{
		sprintf(errmsg,"字段fieldname[%s] 非XDP_DOUBLE",fieldname);
		return false;
	}
	char data[50];
	unsigned int datalen = sizeof(double);
	double *aa = (double *)data;
	*aa = idata;
	return SetFieldData(field,data,datalen,errmsg);
}
bool CXdpRecord::SetFieldData(const char *fieldname,const char *data,unsigned int datalen,char *errmsg)
{
	S_XDP_FIELD_DEFINE *field = m_pXdpFmt->GetField(fieldname);
	if (field == NULL)
	{
		sprintf(errmsg,"无此字段fieldname[%s]",fieldname);
		return false;
	}
	return SetFieldData(field,data,datalen,errmsg);
}
bool CXdpRecord::GetFieldData(unsigned short index,int &idata,char *errmsg)
{
	S_XDP_FIELD_DEFINE *field = m_pXdpFmt->At(index);
	if (field == NULL)
	{
		sprintf(errmsg,"无此字段 index[%d]",index);
		return false;
	}
	if (field->f_type != XDP_INT && field->f_type != XDP_UINT)
	{
		sprintf(errmsg,"字段index[%d] 非 XDP_INT或XDP_UINT",index);
		return false;
	}
	char data[50];
	unsigned int datalen = sizeof(data);
	if (!GetFieldData(field,data,datalen,errmsg))
	{
		return false;
	}
	idata = *((int *)data);
	return true;
}
bool CXdpRecord::GetFieldData(unsigned short index,unsigned int &idata,char *errmsg)
{
	S_XDP_FIELD_DEFINE *field = m_pXdpFmt->At(index);
	if (field == NULL)
	{
		sprintf(errmsg,"无此字段 index[%d]",index);
		return false;
	}
	if (field->f_type != XDP_UINT && field->f_type != XDP_INT)
	{
		sprintf(errmsg,"字段index[%d] 非 XDP_UINT或XDP_INT",index);
		return false;
	}
	char data[50];
	unsigned int datalen = sizeof(data);
	if (!GetFieldData(field,data,datalen,errmsg))
	{
		return false;
	}
	idata = *((unsigned int *)data);
	return true;
}
bool CXdpRecord::GetFieldData(unsigned short index, INT64_& idata, char* errmsg)
{
    S_XDP_FIELD_DEFINE* field = m_pXdpFmt->At(index);
    if (field == NULL)
    {
        sprintf(errmsg, "无此字段 index[%d]", index);
        return false;
    }
    if (field->f_type != XDP_INT64 && field->f_type != XDP_UINT64)
    {
        sprintf(errmsg, "字段index[%d] 非 XDP_INT64或XDP_UINT64", index);
        return false;
    }
    char data[50];
    unsigned int datalen = sizeof(data);
    if (!GetFieldData(field, data, datalen, errmsg))
    {
        return false;
    }
    idata = *((INT64_ *)data);
    return true;
}
bool CXdpRecord::GetFieldData(unsigned short index, UINT64_& idata, char* errmsg)
{
    S_XDP_FIELD_DEFINE* field = m_pXdpFmt->At(index);
    if (field == NULL)
    {
        sprintf(errmsg, "无此字段 index[%d]", index);
        return false;
    }
    if (field->f_type != XDP_UINT64 && field->f_type != XDP_INT64)
    {
        sprintf(errmsg, "字段index[%d] 非 XDP_UINT64或XDP_INT64", index);
        return false;
    }
    char data[50];
    unsigned int datalen = sizeof(data);
    if (!GetFieldData(field, data, datalen, errmsg))
    {
        return false;
    }
    idata = *((UINT64_*)data);
    return true;
}
bool CXdpRecord::GetFieldData(unsigned short index,short &idata,char *errmsg)
{
	S_XDP_FIELD_DEFINE *field = m_pXdpFmt->At(index);
	if (field == NULL)
	{
		sprintf(errmsg,"无此字段 index[%d]",index);
		return false;
	}
	if (field->f_type != XDP_SHORT && field->f_type != XDP_USHORT)
	{
		sprintf(errmsg,"字段index[%d] 非 XDP_SHORT或XDP_USHORT",index);
		return false;
	}
	char data[50];
	unsigned int datalen = sizeof(data);
	if (!GetFieldData(field,data,datalen,errmsg))
	{
		return false;
	}
	idata = *((short *)data);
	return true;
}
bool CXdpRecord::GetFieldData(unsigned short index,unsigned short &idata,char *errmsg)
{
	S_XDP_FIELD_DEFINE *field = m_pXdpFmt->At(index);
	if (field == NULL)
	{
		sprintf(errmsg,"无此字段 index[%d]",index);
		return false;
	}
	if (field->f_type != XDP_USHORT && field->f_type != XDP_SHORT)
	{
		sprintf(errmsg,"字段index[%d] 非 XDP_USHORT或XDP_SHORT",index);
		return false;
	}
	char data[50];
	unsigned int datalen = sizeof(data);
	if (!GetFieldData(field,data,datalen,errmsg))
	{
		return false;
	}
	idata = *((unsigned short *)data);
	return true;
}
bool CXdpRecord::GetFieldData(unsigned short index,double &idata,char *errmsg)
{
	S_XDP_FIELD_DEFINE *field = m_pXdpFmt->At(index);
	if (field == NULL)
	{
		sprintf(errmsg,"无此字段 index[%d]",index);
		return false;
	}
	if (field->f_type != XDP_DOUBLE)
	{
		sprintf(errmsg,"字段index[%d] 非 XDP_DOUBLE",index);
		return false;
	}
	char data[50];
	unsigned int datalen = sizeof(data);
	if (!GetFieldData(field,data,datalen,errmsg))
	{
		return false;
	}
	idata = *((double *)data);
	return true;
}
bool CXdpRecord::GetFieldData(unsigned short index, char *data,unsigned int &datalen, int &fieldtype, char *errmsg)
{
	S_XDP_FIELD_DEFINE *field = m_pXdpFmt->At(index);
	if (field == NULL)
	{
		sprintf(errmsg,"无此字段 index[%d]",index);
		return false;
	}
	fieldtype = field->f_type;
	if (!GetFieldData(field,data,datalen,errmsg))
	{
		return false;
	}
	return true;
}

bool CXdpRecord::GetFieldData(const char *fieldname,int &idata,char *errmsg)
{
	S_XDP_FIELD_DEFINE *field = m_pXdpFmt->GetField(fieldname);
	if (field == NULL)
	{
		sprintf(errmsg,"无此字段fieldname[%s]",fieldname);
		return false;
	}
	if (field->f_type != XDP_INT && field->f_type != XDP_UINT)
	{
		sprintf(errmsg,"字段fieldname[%s] 非XDP_INT或XDP_UINT",fieldname);
		return false;
	}
	char data[50];
	unsigned int datalen = sizeof(data);
	if (!GetFieldData(field,data,datalen,errmsg))
	{
		return false;
	}
	idata = *((int *)data);
	return true;
}

bool CXdpRecord::GetFieldData(const char* fieldname, INT64_& idata, char* errmsg)
{
    S_XDP_FIELD_DEFINE* field = m_pXdpFmt->GetField(fieldname);
    if (field == NULL)
    {
        sprintf(errmsg, "无此字段fieldname[%s]", fieldname);
        return false;
    }
    if (field->f_type != XDP_INT64 && field->f_type != XDP_UINT64)
    {
        sprintf(errmsg, "字段fieldname[%s] 非XDP_INT64或XDP_UINT64", fieldname);
        return false;
    }
    char data[50];
    unsigned int datalen = sizeof(data);
    if (!GetFieldData(field, data, datalen, errmsg))
    {
        return false;
    }
    idata = *((INT64_*)data);
    return true;
}

bool CXdpRecord::GetFieldData(const char* fieldname, UINT64_& idata, char* errmsg)
{
    S_XDP_FIELD_DEFINE* field = m_pXdpFmt->GetField(fieldname);
    if (field == NULL)
    {
        sprintf(errmsg, "无此字段fieldname[%s]", fieldname);
        return false;
    }
    if (field->f_type != XDP_UINT64 && field->f_type != XDP_INT64)
    {
        sprintf(errmsg, "字段fieldname[%s] 非XDP_UINT64或XDP_INT64", fieldname);
        return false;
    }
    char data[50];
    unsigned int datalen = sizeof(data);
    if (!GetFieldData(field, data, datalen, errmsg))
    {
        return false;
    }
    idata = *((UINT64_*)data);
    return true;
}
bool CXdpRecord::GetFieldData(const char *fieldname,unsigned int &idata,char *errmsg)
{
	S_XDP_FIELD_DEFINE *field = m_pXdpFmt->GetField(fieldname);
	if (field == NULL)
	{
		sprintf(errmsg,"无此字段fieldname[%s]",fieldname);
		return false;
	}
	if (field->f_type != XDP_UINT && field->f_type != XDP_INT)
	{
		sprintf(errmsg,"字段fieldname[%s] 非XDP_UINT或XDP_INT",fieldname);
		return false;
	}
	char data[50];
	unsigned int datalen = sizeof(data);
	if (!GetFieldData(field,data,datalen,errmsg))
	{
		return false;
	}
	idata = *((unsigned int *)data);
	return true;
}
bool CXdpRecord::GetFieldData(const char *fieldname,short &idata,char *errmsg)
{
	S_XDP_FIELD_DEFINE *field = m_pXdpFmt->GetField(fieldname);
	if (field == NULL)
	{
		sprintf(errmsg,"无此字段fieldname[%s]",fieldname);
		return false;
	}
	if (field->f_type != XDP_SHORT && field->f_type != XDP_USHORT)
	{
		sprintf(errmsg,"字段fieldname[%s] 非XDP_SHORT或XDP_USHORT",fieldname);
		return false;
	}
	char data[50];
	unsigned int datalen = sizeof(data);
	if (!GetFieldData(field,data,datalen,errmsg))
	{
		return false;
	}
	idata = *((short *)data);
	return true;
}
bool CXdpRecord::GetFieldData(const char *fieldname,unsigned short &idata,char *errmsg)
{
	S_XDP_FIELD_DEFINE *field = m_pXdpFmt->GetField(fieldname);
	if (field == NULL)
	{
		sprintf(errmsg,"无此字段fieldname[%s]",fieldname);
		return false;
	}
	if (field->f_type != XDP_USHORT && field->f_type != XDP_SHORT)
	{
		sprintf(errmsg,"字段fieldname[%s] 非XDP_USHORT或XDP_SHORT",fieldname);
		return false;
	}
	char data[50];
	unsigned int datalen = sizeof(data);
	if (!GetFieldData(field,data,datalen,errmsg))
	{
		return false;
	}
	idata = *((unsigned short *)data);
	return true;
}
bool CXdpRecord::GetFieldData(const char *fieldname,double &idata,char *errmsg)
{
	S_XDP_FIELD_DEFINE *field = m_pXdpFmt->GetField(fieldname);
	if (field == NULL)
	{
		sprintf(errmsg,"无此字段fieldname[%s]",fieldname);
		return false;
	}
	if (field->f_type != XDP_DOUBLE)
	{
		sprintf(errmsg,"字段fieldname[%s] 非XDP_DOUBLE",fieldname);
		return false;
	}
	char data[50];
	unsigned int datalen = sizeof(data);
	if (!GetFieldData(field,data,datalen,errmsg))
	{
		return false;
	}
	idata = *((double *)data);
	return true;
}

bool CXdpRecord::GetFieldData(const char *fieldname, char *data,unsigned int &datalen, int &fieldtype, char *errmsg)
{
	S_XDP_FIELD_DEFINE *field = m_pXdpFmt->GetField(fieldname);
	if (field == NULL)
	{
		sprintf(errmsg,"无此字段fieldname[%s]",fieldname);
		return false;
	}
	fieldtype = field->f_type;
	if (!GetFieldData(field,data,datalen,errmsg))
	{
		return false;
	}
	return true;
}

bool CXdpRecord::GetFieldData(S_XDP_FIELD_DEFINE *field,char *data,unsigned int &datalen,char *errmsg)
{
	if (!IsFieldSet(field->f_index))
	{
		datalen = 0;
		sprintf(errmsg,"此字段[%d]没有置数据",field->f_index);
		return false;
	}
	unsigned short *olen;
	switch (field->f_type)
	{
		case XDP_USHORT:
		case XDP_SHORT:
			if (datalen < sizeof(short))
			{
				sprintf(errmsg,"字段index[%d]为短整型，但传入缓冲长度不足",field->f_index);
				return false;
			}
			datalen = sizeof(short);
			memcpy(data,m_pPackData+field->f_offset,datalen);
			CBF_Tools::Reverse((unsigned char *)data,datalen);
			break;
		case XDP_UINT:
		case XDP_INT:
			if (datalen < sizeof(int))
			{
				sprintf(errmsg,"字段index[%d]为整型，但传入缓冲长度不足",field->f_index);
				return false;
			}
			datalen = sizeof(int);
			memcpy(data,m_pPackData+field->f_offset,datalen);
			CBF_Tools::Reverse((unsigned char *)data,datalen);
			break;
        case XDP_UINT64:
        case XDP_INT64:
            if (datalen < sizeof(INT64_))
            {
                sprintf(errmsg, "字段index[%d]为64位整型，但传入缓冲长度不足", field->f_index);
                return false;
            }
            datalen = sizeof(int);
            memcpy(data, m_pPackData + field->f_offset, datalen);
            CBF_Tools::Reverse((unsigned char*)data, datalen);
            break;
		case XDP_DOUBLE:
			if (datalen < sizeof(double))
			{
				sprintf(errmsg,"字段index[%d]为浮点型，但传入缓冲长度不足",field->f_index);
				return false;
			}
			datalen = sizeof(double);
			memcpy(data,m_pPackData+field->f_offset,datalen);
			CBF_Tools::Reverse((unsigned char *)data,datalen);
			break;
		case XDP_CHAR:
			if (datalen < field->f_length)
			{
				sprintf(errmsg,"字段index[%d]为字符型，但传入缓冲长度不足%d",field->f_index,datalen);
				return false;
			}
			if (datalen > field->f_length)
			{
				datalen = field->f_length;
				memcpy(data,m_pPackData+field->f_offset,datalen);
				data[datalen] = 0;//加一个结束符
			}
			else
			{
				datalen = field->f_length;
				memcpy(data,m_pPackData+field->f_offset,datalen);
			}
			break;
		case XDP_VARCHAR:
			if (datalen < m_pPackData[field->f_offset])
			{
				sprintf(errmsg,"字段index[%d]为变长字符型，但传入缓冲不足%d",field->f_index,datalen);
				return false;
			}
			
			if (datalen > m_pPackData[field->f_offset])
			{
				datalen = m_pPackData[field->f_offset];
				if (datalen>0)
				{
					memcpy(data,m_pPackData+field->f_offset+field->f_extlength,datalen);
				}
				data[datalen] =0; //加一个结束符
			}
			else
			{
				datalen = m_pPackData[field->f_offset];
				memcpy(data,m_pPackData+field->f_offset+field->f_extlength,datalen);
				
			}
			break;
		case XDP_BINDATA:
			
			olen = (unsigned short *)(m_pPackData+field->f_offset);
			if (datalen < *olen)
			{
				sprintf(errmsg,"字段index[%d]为变长二进制型，但传入缓冲长度不足%d",field->f_index,datalen);
				return false;
			}
			datalen = *olen;
			if (datalen>0)
			{
				memcpy(data,m_pPackData+field->f_offset+field->f_extlength,datalen);
			}
			break;
		default:
			sprintf(errmsg,"字段类型不符");
			return false;
	}
	return true;
}

bool CXdpRecord::PrintXdpRec(char *printbuf)
{
	
	int datalen = 0;
	int curdatalen=0;
	char errmsg[400];
	char fielddata[8192];
	unsigned int fdatalen;

	char fieldprintdata[9000];
	unsigned short *pushort=NULL;
	short *pshort=NULL;
	unsigned int  *puint=NULL;
	int  *pint=NULL;
	INT64_ *pint64=NULL;
	UINT64_ *puint64=NULL;
	double *pdvalue=NULL;
	sprintf(printbuf,"\n---   XDP record begin  ---");
	curdatalen = strlen(printbuf);
	for (int i=0 ; i< m_pXdpFmt->GetFieldNum() ; i++)
	{
		if (!IsFieldSet(i))
		{
			continue;
		}
		S_XDP_FIELD_DEFINE *field = m_pXdpFmt->At(i);
		if (field == NULL)
		{
			return false;
		}
		fdatalen = sizeof(fielddata);
		bzero(fielddata,sizeof(fielddata));
		if (!GetFieldData(field,fielddata,fdatalen,errmsg))
		{
			return false;
		}
		
		switch (field->f_type)
		{
			case XDP_USHORT:
				pushort = (unsigned short *)fielddata;
				sprintf(fieldprintdata,"\n%s=%d",field->f_name,*pushort);
				datalen = strlen(fieldprintdata);
				break;
			case XDP_SHORT:
				pshort = (short *)fielddata;
				sprintf(fieldprintdata,"\n%s=%d",field->f_name,*pshort);
				datalen = strlen(fieldprintdata);
				break;
			case XDP_UINT:
				puint = (unsigned int *)fielddata;
				sprintf(fieldprintdata,"\n%s=%d",field->f_name,*puint);
				datalen = strlen(fieldprintdata);
				break;
			case XDP_INT:
				pint = (int *)fielddata;
				sprintf(fieldprintdata,"\n%s=%d",field->f_name,*pint);
				datalen = strlen(fieldprintdata);
				break;
            case XDP_INT64:
				pint64 = (INT64_*)fielddata;
#ifdef _WINDOWS
                sprintf(fieldprintdata, "\n%s=%I64d", field->f_name, *pint64);
#else
				sprintf(fieldprintdata, "\n%s=%lld", field->f_name, *pint64);
#endif
                datalen = strlen(fieldprintdata);
                break;
            case XDP_UINT64:
                puint64 = (UINT64_*)fielddata;
#ifdef _WINDOWS
                sprintf(fieldprintdata, "\n%s=%I64d", field->f_name, *puint64);
#else
                sprintf(fieldprintdata, "\n%s=%lld", field->f_name, *puint64);
#endif
                datalen = strlen(fieldprintdata);
                break;
			case XDP_DOUBLE:
				pdvalue = (double *)fielddata;
				sprintf(fieldprintdata,"\n%s=%f",field->f_name,*pdvalue);
				datalen = strlen(fieldprintdata);
				break;
			case XDP_CHAR:
				sprintf(fieldprintdata,"\n%s=%s",field->f_name,fielddata);
				datalen = strlen(fieldprintdata);
				break;
			case XDP_VARCHAR:
				sprintf(fieldprintdata,"\n%s=%s",field->f_name,fielddata);
				datalen = strlen(fieldprintdata);
				break;
			case XDP_BINDATA:
				//转成16进制
				bzero(fieldprintdata,sizeof(fieldprintdata));
				sprintf(fieldprintdata,"\n%s=",field->f_name);
				m_pDes.HEX_2_DSP(fielddata,fieldprintdata+strlen(field->f_name)+2,fdatalen);
				datalen = strlen(fieldprintdata);
				break;
			default:
				return false;
		}
		sprintf(printbuf+curdatalen,"%s",fieldprintdata);
		curdatalen += datalen;
	}
	sprintf(printbuf+curdatalen,"\n---  XDP record end! ---\n");
	return true;
}
bool CXdpRecord::CancelField(const char *fieldname)
{
	S_XDP_FIELD_DEFINE *field = m_pXdpFmt->GetField(fieldname);
	if (field == NULL)
	{
		return false;
	}
	return CancelField(field->f_index);
}
bool CXdpRecord::CancelField(unsigned short index)
{
	S_XDP_FIELD_DEFINE *field = m_pXdpFmt->At(index);
	if (field == NULL)
	{
		return false;
	}
	if (IsFieldSet(index))
	{
		unsigned short *sv=NULL;
		switch (field->f_type)
		{
		case XDP_USHORT:
		case XDP_SHORT:
			m_nRealLen -= sizeof(short);
			break;
		case XDP_INT:
		case XDP_UINT:
			m_nRealLen -= sizeof(int);
			break;
        case XDP_INT64:
        case XDP_UINT64:
            m_nRealLen -= sizeof(INT64_);
            break;
		case XDP_DOUBLE:
			m_nRealLen -= sizeof(double);
			break;
		case XDP_CHAR:
			m_nRealLen -= field->f_length;
			break;
		case XDP_VARCHAR:
			m_nRealLen -= (unsigned char)(m_pPackData+field->f_offset)[0];
			break;
		case XDP_BINDATA:
			sv = (unsigned short *)(m_pPackData+field->f_offset);
			m_nRealLen -= *sv;
			break;
		default:
			break;
		}
		SetFieldMap(index,false);
	}
	
	return true;
}

char * CXdpRecord::GetPackDataBuffer()
{
	return m_pPackData;
}

unsigned int CXdpRecord::GetPackDataBufferLen()
{
	return m_nPackDataMaxLen;
}

char * CXdpRecord::GetBitMap()
{
	return m_sBitMap;
}

bool CXdpRecord::XdpRecordCopy(CXdpRecord *src)
{
	//复制位图
	memcpy(m_sBitMap,src->GetBitMap(),XDP_BITMAPSIZE);
	//复制数据内容
	m_nPackDataMaxLen = src->GetPackDataBufferLen();
	if (m_nPackDataMaxLen <1)
	{
		return false;
	}
	memcpy(m_pPackData,src->GetPackDataBuffer(),m_nPackDataMaxLen);
	//复制报文实际占用空间
	m_nRealLen = src->GetRealLen();
	return true;
}

bool CXdpRecord::IsFieldSet(int index)
{
	int bitnum=index/8;
	int offset= index%8;
	char tmpchar = m_sBitMap[bitnum];
	int ret=0;
	switch (offset)
	{
		case 0:
			ret = (tmpchar & 128);
			break;
		case 1:
			ret = (tmpchar & 64);
			break;
		case 2:
			ret = (tmpchar & 32);
			break;
		case 3:
			ret = (tmpchar & 16);
			break;
		case 4:
			ret = (tmpchar & 8);
			break;
		case 5:
			ret = (tmpchar & 4);
			break;
		case 6:
			ret = (tmpchar & 2);
			break;
		case 7:
			ret = (tmpchar & 1);
			break;
		default:
			return false;

	}
	return ret;
}

void CXdpRecord::SetFieldMap(int index, bool is)
{
	int bitnum=index/8;
	int offset= index%8;

	char tmpchar[20];
	bzero(tmpchar,sizeof(tmpchar));
	bool bret=IsFieldSet(index);
	if (bret) //已设
	{
		if (!is)
		{
			//使反转
			switch (offset)
			{
			case 0:
				m_sBitMap[bitnum]= m_sBitMap[bitnum] ^128;
				break;
			case 1:
				m_sBitMap[bitnum]= m_sBitMap[bitnum] ^ 64;
				break;
			case 2:
				m_sBitMap[bitnum]= m_sBitMap[bitnum] ^32;
				break;
			case 3:
				m_sBitMap[bitnum]= m_sBitMap[bitnum] ^ 16;
				break;
			case 4:
				m_sBitMap[bitnum]= m_sBitMap[bitnum] ^ 8;
				break;
			case 5:
				m_sBitMap[bitnum]= m_sBitMap[bitnum] ^ 4;
				break;
			case 6:
				m_sBitMap[bitnum]= m_sBitMap[bitnum] ^2;
				break;
			case 7:
				m_sBitMap[bitnum]= m_sBitMap[bitnum] ^1;
				break;
			default:
				break;
			}
		}
	}
	else //未设
	{
		if (is)
		{
			//使反转
			switch (offset)
			{
			case 0:
				m_sBitMap[bitnum]= m_sBitMap[bitnum] ^128;
				break;
			case 1:
				m_sBitMap[bitnum]= m_sBitMap[bitnum] ^ 64;
				break;
			case 2:
				
				m_sBitMap[bitnum]= m_sBitMap[bitnum] ^32;
				
				break;
			case 3:
				
				m_sBitMap[bitnum]= m_sBitMap[bitnum] ^ 16;
				
				break;
			case 4:
				m_sBitMap[bitnum]= m_sBitMap[bitnum] ^ 8;
				break;
			case 5:
				
				m_sBitMap[bitnum]= m_sBitMap[bitnum] ^ 4;
				
				break;
			case 6:
				
				m_sBitMap[bitnum]= m_sBitMap[bitnum] ^2;
				
				break;
			case 7:
				m_sBitMap[bitnum]= m_sBitMap[bitnum] ^1;
				break;
			default:
				break;
			}

		}
	}
	
}
