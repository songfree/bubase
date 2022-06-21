// GtpData.cpp: implementation of the CGtpData class.
//
//////////////////////////////////////////////////////////////////////

#include "GtpData.h"
#include "BF_Tools.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#ifdef _WINDOWS
#define   INT64PFM  "%I64d"
#else
#define   INT64PFM  "%lld"
#endif

CGtpData::CGtpData()
{
	m_cDataType = GTP_DATATYPE_UNKNOW;
	m_pLog.SetMaxLogSize(0);
	m_pLog.SetLogPara(LOG_DEBUG,"","parsegtp.log");
}

CGtpData::~CGtpData()
{
	Clear();
}

bool CGtpData::FromBuffer(const char *gtpdata,int layer)
{
	Clear();
	if (!SetCurKeyValue("",gtpdata,GTP_DATATYPE_VALUE))
	{
		return false;
	}
	return ParseGtp(layer);
}

bool CGtpData::FromFile(const char *gtpfile, int layer)
{
	struct_stat st;
	FSTAT(gtpfile,&st);
	if (st.st_size <1)
	{
		m_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"GTP FILE[%s]大小不符或不存在",gtpfile);
		return false;
	}
	FILE *fp = fopen(gtpfile,"r");
	if (fp == NULL)
	{
		m_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"GTP FILE[%s]不存在",gtpfile);
		return false;
	}
	char *gtpbuf = (char *)malloc(st.st_size+1);
	if (gtpbuf == NULL)
	{
		m_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"读gtp文件分配空间出错");
		fclose(fp);
		return false;
	}
	//bzero(gtpbuf,st.st_size+1);
	int ret = fread(gtpbuf,1,st.st_size,fp);
	//if (ret != st.st_size)
	if (ret <1)
	{
		m_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"读取GTP FILE[%s]出错 read[%d] size[%d]",gtpfile,ret,st.st_size);
		free(gtpbuf);
		fclose(fp);
		return false;
	}
	gtpbuf[ret]=0;
	fclose(fp);
	bool bRet =  FromBuffer(gtpbuf,layer);
	free(gtpbuf);
	return bRet;
	
}

bool CGtpData::ToBuffer(char *gtpbuffer, int &buflen)
{
	int datalen=0;
	int curlen=0;
	int plen=0;
	bool bRet;
	S_GTP_DATA_ data;
	if (m_cDataType == GTP_DATATYPE_VALUE)  //kv格式
	{
		bRet = m_table.First(data);
		while (bRet )
		{
			if (data.bIsParseLocal && data.pGtpData != NULL) //HASH或数组
			{
				curlen=strlen(data.sFieldName)+1;
				if (datalen+curlen > buflen)
				{
					m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"缓冲空间大小[%d]不足",buflen);
					return false;
				}
				sprintf(gtpbuffer+datalen,"%s=",data.sFieldName);
				datalen+=curlen;
				plen = buflen-datalen;
				if (!data.pGtpData->ToBuffer(gtpbuffer+datalen,plen))
				{
					return false;
				}
				datalen+=plen;
			}
			else
			{
				curlen=strlen(data.sFieldName)+1;
				curlen+=data.sFieldValue.length();
				if (datalen+curlen > buflen)
				{
					m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"缓冲空间大小[%d]不足",buflen);
					return false;
				}
				sprintf(gtpbuffer+datalen,"%s=%s",data.sFieldName,data.sFieldValue.c_str());
				datalen+=curlen;
			}
			
			bRet = m_table.Next(data);
			if (bRet) //还有，要加,
			{
				if (datalen+1 > buflen)
				{
					m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"缓冲空间大小[%d]不足",buflen);
					return false;
				}
				sprintf(gtpbuffer+datalen,",");
				datalen++;
			}
		}
		buflen = datalen;
	}
	else if (m_cDataType == GTP_DATATYPE_HASH)  //哈希格式
	{
		if (datalen+2 > buflen)
		{
			m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"缓冲空间大小[%d]不足",buflen);
			return false;
		}
		sprintf(gtpbuffer+datalen,"{");
		datalen++;
		bRet = m_table.First(data);
		while (bRet )
		{
			if (data.bIsParseLocal && data.pGtpData != NULL) //HASH或数组
			{
				
				curlen=strlen(data.sFieldName)+1;
				if (datalen+curlen > buflen)
				{
					m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"缓冲空间大小[%d]不足",buflen);
					return false;
				}
				sprintf(gtpbuffer+datalen,"%s=",data.sFieldName);
				datalen+=curlen;
				
				
				plen = buflen-datalen;
				if (!data.pGtpData->ToBuffer(gtpbuffer+datalen,plen))
				{
					return false;
				}
				datalen+=plen;
			}
			else
			{
				curlen=strlen(data.sFieldName)+1;
				curlen+=data.sFieldValue.length();
				if (datalen+curlen > buflen)
				{
					m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"缓冲空间大小[%d]不足",buflen);
					return false;
				}
				sprintf(gtpbuffer+datalen,"%s=%s",data.sFieldName,data.sFieldValue.c_str());
				datalen+=curlen;
			}
			
			bRet = m_table.Next(data);
			if (bRet) //还有，要加,
			{
				if (datalen+1 > buflen)
				{
					m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"缓冲空间大小[%d]不足",buflen);
					return false;
				}
				sprintf(gtpbuffer+datalen,",");
				datalen++;
			}
		}
		sprintf(gtpbuffer+datalen,"}");
		datalen++;
		buflen = datalen;
	}
	else if (m_cDataType == GTP_DATATYPE_ARRAY)  //数组格式
	{
		if (datalen+2 > buflen)
		{
			m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"缓冲空间大小[%d]不足",buflen);
			return false;
		}
		sprintf(gtpbuffer+datalen,"[");
		datalen++;
		bRet = m_table.First(data);
		while (bRet )
		{
			if (data.bIsParseLocal && data.pGtpData != NULL) //HASH或数组
			{
				plen = buflen-datalen;
				if (!data.pGtpData->ToBuffer(gtpbuffer+datalen,plen))
				{
					return false;
				}
				datalen+=plen;
			}
			else  if (data.bIsParseLocal)
			{
				curlen=strlen(data.sFieldName)+1;
				curlen+=data.sFieldValue.length();
				if (datalen+curlen > buflen)
				{
					m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"缓冲空间大小[%d]不足",buflen);
					return false;
				}
				sprintf(gtpbuffer+datalen,"%s=%s",data.sFieldName,data.sFieldValue.c_str());
				datalen+=curlen;
			}
			else 
			{
				curlen=data.sFieldValue.length();
				if (datalen+curlen > buflen)
				{
					m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"缓冲空间大小[%d]不足",buflen);
					return false;
				}
				sprintf(gtpbuffer+datalen,"%s",data.sFieldValue.c_str());
				datalen+=curlen;
			}
			
			bRet = m_table.Next(data);
			if (bRet) //还有，要加,
			{
				if (datalen+1 > buflen)
				{
					m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"缓冲空间大小[%d]不足",buflen);
					return false;
				}
				sprintf(gtpbuffer+datalen,",");
				datalen++;
			}
		}
		sprintf(gtpbuffer+datalen,"]");
		datalen++;
		buflen = datalen;
	}
	return true;
}

bool CGtpData::ToFile(const char *gtpfilename)
{
	std::string tmpstr;
	if (!ToString(tmpstr))
	{
		return false;
	}
	FILE *fp = fopen(gtpfilename,"a");
	if (fp != NULL)
	{
		int ret = fwrite(tmpstr.c_str(),1,tmpstr.length(),fp);
		if (ret !=tmpstr.length())
		{
			m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"写文件出错 写入[%d] 大小[%d]",ret,tmpstr.length());
			fclose(fp);
			return false;
		}
		fclose(fp);
		return true;
	}
	m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"写打开文件[%s]出错 大小[%d]",gtpfilename,tmpstr.length());
	return false;
}

bool CGtpData::ToString(std::string &gtpstring)
{
	gtpstring= "";
	std::string tmpstr="";
	bool bRet;
	S_GTP_DATA_ data;
	if (m_cDataType == GTP_DATATYPE_VALUE)  //kv格式
	{
		bRet = m_table.First(data);
		while (bRet )
		{
			if (data.bIsParseLocal && data.pGtpData != NULL) //HASH或数组
			{
				gtpstring=gtpstring+data.sFieldName+"=";
				tmpstr="";
				if (!data.pGtpData->ToString(tmpstr))
				{
					return false;
				}
				gtpstring=gtpstring+tmpstr;
			}
			else
			{
				gtpstring=gtpstring+data.sFieldName+"="+data.sFieldValue;
			}
			
			bRet = m_table.Next(data);
			if (bRet) //还有，要加,
			{
				gtpstring+=",";
			}
		}
		
	}
	else if (m_cDataType == GTP_DATATYPE_HASH)  //哈希格式
	{
		gtpstring+="{";
		
		bRet = m_table.First(data);
		while (bRet )
		{
			if (data.bIsParseLocal && data.pGtpData != NULL) //HASH或数组
			{
				gtpstring=gtpstring+data.sFieldName+"=";
				tmpstr="";
				if (!data.pGtpData->ToString(tmpstr))
				{
					return false;
				}
				gtpstring+=tmpstr;
			}
			else
			{
				gtpstring=gtpstring+data.sFieldName+"="+data.sFieldValue;
			}
			
			bRet = m_table.Next(data);
			if (bRet) //还有，要加,
			{
				gtpstring+=",";
			}
		}
		gtpstring+="}";
	}
	else if (m_cDataType == GTP_DATATYPE_ARRAY)  //数组格式
	{
		gtpstring+="[";
		bRet = m_table.First(data);
		while (bRet )
		{
			if (data.bIsParseLocal && data.pGtpData != NULL) //HASH或数组
			{
				tmpstr="";
				if (!data.pGtpData->ToString(tmpstr))
				{
					return false;
				}
				gtpstring+=tmpstr;
			}
			else if (data.bIsParseLocal)
			{
				gtpstring=gtpstring+data.sFieldName+"="+data.sFieldValue;
			}
			else
			{
				gtpstring=gtpstring+data.sFieldValue;
			}
			bRet = m_table.Next(data);
			if (bRet) //还有，要加,
			{
				gtpstring+=",";
			}
		}
		gtpstring+="]";
	}
	return true;
}

int CGtpData::GetGtpDataLen()
{
	int datalen=0;
	int curlen=0;
	bool bRet;
	S_GTP_DATA_ data;
	if (m_cDataType == GTP_DATATYPE_VALUE)  //kv格式
	{
		bRet = m_table.First(data);
		while (bRet )
		{
			if (data.bIsParseLocal && data.pGtpData != NULL) //HASH或数组
			{
				datalen=datalen+strlen(data.sFieldName)+1;
				curlen = data.pGtpData->GetGtpDataLen();
				datalen+=curlen;
			}
			else
			{
				datalen=datalen+strlen(data.sFieldName)+1;
				datalen+=data.sFieldValue.length();
			}
			
			bRet = m_table.Next(data);
			if (bRet) //还有，要加,
			{
				datalen++;
			}
		}
		return datalen;
	}
	else if (m_cDataType == GTP_DATATYPE_HASH)  //哈希格式
	{
		datalen++;
		bRet = m_table.First(data);
		while (bRet )
		{
			if (data.bIsParseLocal && data.pGtpData != NULL) //HASH或数组
			{
				datalen=datalen+strlen(data.sFieldName)+1;
				curlen = data.pGtpData->GetGtpDataLen();
				datalen+=curlen;
			}
			else
			{
				datalen=datalen+strlen(data.sFieldName)+1;
				datalen+=data.sFieldValue.length();
			}
			
			bRet = m_table.Next(data);
			if (bRet) //还有，要加,
			{
				datalen++;
			}
		}
		datalen++;
		return datalen;
	}
	else if (m_cDataType == GTP_DATATYPE_ARRAY)  //数组格式
	{
		datalen++;
		bRet = m_table.First(data);
		while (bRet )
		{
			if (data.bIsParseLocal && data.pGtpData != NULL) //HASH或数组
			{
				curlen = data.pGtpData->GetGtpDataLen();
				
				datalen+=curlen;
			}
			else  if (data.bIsParseLocal)
			{
				datalen=datalen+strlen(data.sFieldName)+1;
				datalen+=data.sFieldValue.length();
			}
			else 
			{
				datalen+=data.sFieldValue.length();
			}
			
			bRet = m_table.Next(data);
			if (bRet) //还有，要加,
			{
				datalen++;
			}
		}
		datalen++;
		return datalen;
	}
	return datalen;
}

int CGtpData::GetValueByName(const char *fieldname, std::string &fieldvalue, int &fieldtype)
{
	CInt iSet;
	int rId;
	if (!m_keyIndex.Select(iSet,(char *)fieldname))
	{
		return -1;
	}
	bool bRet = iSet.First(rId);
	if (!bRet)
	{
		return -1;
	}
	fieldtype = m_table.m_table[rId].cDataType;
	if (fieldtype != GTP_DATATYPE_VALUE)
	{
		if (m_table.m_table[rId].pGtpData != NULL)
		{
			if (!m_table.m_table[rId].pGtpData->ToString(fieldvalue))
			{
				m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"GetValueByName ToString出错");
				return -3;
			}
		}
		else
		{
			fieldvalue = m_table.m_table[rId].sFieldValue;
		}
		
	}
	else
	{
		fieldvalue = m_table.m_table[rId].sFieldValue;
		GtpGetTransSign(fieldvalue);
	}
	return rId;
}

int CGtpData::GetValueByName(const char *fieldname, char *fieldvalue, int &fieldtype)
{
	std::string ss;
	int nRet= GetValueByName(fieldname,ss,fieldtype);
	if (nRet<0)
	{
		return nRet;
	}
	strcpy(fieldvalue,ss.c_str());
	return nRet;
}

int CGtpData::GetValueByName(const char *fieldname, int &fieldvalue, int &fieldtype)
{
	std::string ss;
	int nRet= GetValueByName(fieldname,ss,fieldtype);
	if (nRet<0)
	{
		return nRet;
	}
	if (fieldtype != GTP_DATATYPE_VALUE)
	{
		return -2;
	}
	fieldvalue = atoi(ss.c_str());
	return nRet;
}
int CGtpData::GetValueByName(const char *fieldname, unsigned char &fieldvalue, int &fieldtype)
{
	std::string ss;
	int nRet= GetValueByName(fieldname,ss,fieldtype);
	if (nRet<0)
	{
		return nRet;
	}
	if (fieldtype != GTP_DATATYPE_VALUE)
	{
		return -2;
	}
	fieldvalue = ss.c_str()[0];
	return nRet;
}
int CGtpData::GetValueByName(const char *fieldname, double &fieldvalue, int &fieldtype)
{
	std::string ss;
	int nRet= GetValueByName(fieldname,ss,fieldtype);
	if (nRet<0)
	{
		return nRet;
	}
	if (fieldtype != GTP_DATATYPE_VALUE)
	{
		return -2;
	}
	fieldvalue = atof(ss.c_str());
	return nRet;
}

int CGtpData::GetValueByName(const char *fieldname, unsigned int &fieldvalue, int &fieldtype)
{
	std::string ss;
	int nRet= GetValueByName(fieldname,ss,fieldtype);
	if (nRet<0)
	{
		return nRet;
	}
	if (fieldtype != GTP_DATATYPE_VALUE)
	{
		return -2;
	}
	fieldvalue = atol(ss.c_str());
	return nRet;
}

int CGtpData::GetValueByName(const char *fieldname, unsigned long &fieldvalue, int &fieldtype)
{
	std::string ss;
	int nRet= GetValueByName(fieldname,ss,fieldtype);
	if (nRet<0)
	{
		return nRet;
	}
	if (fieldtype != GTP_DATATYPE_VALUE)
	{
		return -2;
	}
	fieldvalue = ATOI64(ss.c_str());
	return nRet;
}

int CGtpData::GetValueByName(const char *fieldname, unsigned short &fieldvalue, int &fieldtype)
{
	std::string ss;
	int nRet= GetValueByName(fieldname,ss,fieldtype);
	if (nRet<0)
	{
		return nRet;
	}
	if (fieldtype != GTP_DATATYPE_VALUE)
	{
		return -2;
	}
	fieldvalue = atoi(ss.c_str());
	return nRet;
}

int CGtpData::GetValueByName(const char *fieldname, short &fieldvalue, int &fieldtype)
{
	std::string ss;
	int nRet= GetValueByName(fieldname,ss,fieldtype);
	if (nRet<0)
	{
		return nRet;
	}
	if (fieldtype != GTP_DATATYPE_VALUE)
	{
		return -2;
	}
	fieldvalue = atoi(ss.c_str());
	return nRet;
}

int CGtpData::GetValueByName(const char *fieldname, long &fieldvalue, int &fieldtype)
{
	std::string ss;
	int nRet= GetValueByName(fieldname,ss,fieldtype);
	if (nRet<0)
	{
		return nRet;
	}
	if (fieldtype != GTP_DATATYPE_VALUE)
	{
		return -2;
	}
	fieldvalue = atol(ss.c_str());
	return nRet;
}

int CGtpData::GetDataType()
{
	return m_cDataType;
}

int CGtpData::GetRowId(const char *fieldname, int &fieldtype)
{
	CInt iSet;
	int rId;
	if (!m_keyIndex.Select(iSet,(char *)fieldname))
	{
		return -1;
	}
	bool bRet = iSet.First(rId);
	if (!bRet)
	{
		return -1;
	}
	fieldtype = m_table.m_table[rId].cDataType;
	return rId;
}


int CGtpData::GetHashValue(unsigned int hashrowid, const char *fieldname, char *fieldvalue, int &fieldtype)
{
	std::string ss;
	int nRet = GetHashValue(hashrowid,fieldname,ss,fieldtype);
	if (nRet != 0)
	{
		return nRet;
	}
	strcpy(fieldvalue,ss.c_str());
	return 0;
}
int CGtpData::GetHashValue(unsigned int hashrowid, const char *fieldname, std::string &fieldvalue, int &fieldtype)
{
	if (hashrowid > m_table.Size())
	{
		return -1;
	}
	if (m_table.m_table[hashrowid].cDataType != GTP_DATATYPE_HASH)
	{
		return 1;
	}
	//未解析则去解析
	if (!m_table.m_table[hashrowid].bIsParseLocal)
	{
		m_table.m_table[hashrowid].pGtpData = new CGtpData();
		if (m_table.m_table[hashrowid].pGtpData == NULL)
		{
			m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"hash字段[%s]未解析去解析new CGtpData出错",m_table.m_table[hashrowid].sFieldName);
			return -2;
		}
		m_table.m_table[hashrowid].pGtpData->SetCurKeyValue(\
			m_table.m_table[hashrowid].sFieldName,\
			m_table.m_table[hashrowid].sFieldValue,m_table.m_table[hashrowid].cDataType);
		m_table.m_table[hashrowid].bIsParseLocal = m_table.m_table[hashrowid].pGtpData->ParseGtp(1);
		if (!m_table.m_table[hashrowid].bIsParseLocal)
		{
			m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"hash字段[%s]未解析去解析出错",m_table.m_table[hashrowid].sFieldName);
			delete m_table.m_table[hashrowid].pGtpData;
			m_table.m_table[hashrowid].pGtpData = NULL;
			return -3;
		}
	}
	if (m_table.m_table[hashrowid].pGtpData->GetValueByName(fieldname,fieldvalue,fieldtype)<0)
	{
		return -1;
	}
	return 0;
}
int CGtpData::GetHashValue(unsigned int hashrowid, const char *fieldname, int &fieldvalue, int &fieldtype)
{
	std::string ss;
	int nRet = GetHashValue(hashrowid,fieldname,ss,fieldtype);
	if (nRet != 0)
	{
		return nRet;
	}
	if (fieldtype != GTP_DATATYPE_VALUE)
	{
		return 2;
	}
	fieldvalue = atoi(ss.c_str());
	return 0;
}
int CGtpData::GetHashValue(unsigned int hashrowid, const char *fieldname, short &fieldvalue, int &fieldtype)
{
	std::string ss;
	int nRet = GetHashValue(hashrowid,fieldname,ss,fieldtype);
	if (nRet != 0)
	{
		return nRet;
	}
	if (fieldtype != GTP_DATATYPE_VALUE)
	{
		return 2;
	}
	fieldvalue = atoi(ss.c_str());
	return 0;
}
int CGtpData::GetHashValue(unsigned int hashrowid, const char *fieldname, long &fieldvalue, int &fieldtype)
{
	std::string ss;
	int nRet = GetHashValue(hashrowid,fieldname,ss,fieldtype);
	if (nRet != 0)
	{
		return nRet;
	}
	if (fieldtype != GTP_DATATYPE_VALUE)
	{
		return 2;
	}
	fieldvalue = atol(ss.c_str());
	return 0;
}
int CGtpData::GetHashValue(unsigned int hashrowid, const char *fieldname, unsigned int &fieldvalue, int &fieldtype)
{
	std::string ss;
	int nRet = GetHashValue(hashrowid,fieldname,ss,fieldtype);
	if (nRet != 0)
	{
		return nRet;
	}
	if (fieldtype != GTP_DATATYPE_VALUE)
	{
		return 2;
	}
	fieldvalue = atol(ss.c_str());
	return 0;
}
int CGtpData::GetHashValue(unsigned int hashrowid, const char *fieldname, unsigned short &fieldvalue, int &fieldtype)
{
	std::string ss;
	int nRet = GetHashValue(hashrowid,fieldname,ss,fieldtype);
	if (nRet != 0)
	{
		return nRet;
	}
	if (fieldtype != GTP_DATATYPE_VALUE)
	{
		return 2;
	}
	fieldvalue = atoi(ss.c_str());
	return 0;
}
int CGtpData::GetHashValue(unsigned int hashrowid, const char *fieldname, unsigned long &fieldvalue, int &fieldtype)
{
	std::string ss;
	int nRet = GetHashValue(hashrowid,fieldname,ss,fieldtype);
	if (nRet != 0)
	{
		return nRet;
	}
	if (fieldtype != GTP_DATATYPE_VALUE)
	{
		return 2;
	}
	fieldvalue = ATOI64(ss.c_str());
	return 0;
}
int CGtpData::GetHashValue(unsigned int hashrowid, const char *fieldname, double &fieldvalue, int &fieldtype)
{
	std::string ss;
	int nRet = GetHashValue(hashrowid,fieldname,ss,fieldtype);
	if (nRet != 0)
	{
		return nRet;
	}
	if (fieldtype != GTP_DATATYPE_VALUE)
	{
		return 2;
	}
	fieldvalue = atof(ss.c_str());
	return 0;
}
int CGtpData::FieldCount()
{
	return m_table.Size();
}
int CGtpData::GetArraySize(unsigned int arrayrowid)
{
	if (arrayrowid > m_table.Size())
	{
		return -1;
	}
	if (m_table.m_table[arrayrowid].cDataType != GTP_DATATYPE_ARRAY)
	{
		return -1;
	}
	//未解析则去解析
	if (!m_table.m_table[arrayrowid].bIsParseLocal)
	{
		m_table.m_table[arrayrowid].pGtpData = new CGtpData();
		if (m_table.m_table[arrayrowid].pGtpData == NULL)
		{
			m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"ARRAY字段[%s]未解析去解析new CGtpData出错",m_table.m_table[arrayrowid].sFieldName);
			return -2;
		}
		m_table.m_table[arrayrowid].pGtpData->SetCurKeyValue(\
			m_table.m_table[arrayrowid].sFieldName,\
			m_table.m_table[arrayrowid].sFieldValue,m_table.m_table[arrayrowid].cDataType);
		m_table.m_table[arrayrowid].bIsParseLocal = m_table.m_table[arrayrowid].pGtpData->ParseGtp(1);
		if (!m_table.m_table[arrayrowid].bIsParseLocal)
		{
			m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"array字段[%s]未解析去解析出错",m_table.m_table[arrayrowid].sFieldName);
			delete m_table.m_table[arrayrowid].pGtpData;
			m_table.m_table[arrayrowid].pGtpData = NULL;
			return -3;
		}
	}
	if (m_table.m_table[arrayrowid].pGtpData != NULL)
	{
		return m_table.m_table[arrayrowid].pGtpData->m_table.Size();
	}
	
	return -1;
}


CGtpData * CGtpData::GetDataPoint(const char *fieldname)
{
	CInt iSet;
	int rId;
	if (!m_keyIndex.Select(iSet,(char *)fieldname))
	{
		return NULL;
	}
	bool bRet = iSet.First(rId);
	if (!bRet)
	{
		return NULL;
	}
	if (!m_table.m_table[rId].bIsParseLocal)
	{
		m_table.m_table[rId].pGtpData = new CGtpData();
		if (m_table.m_table[rId].pGtpData == NULL)
		{
			m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"ARRAY字段[%s]未解析去解析new CGtpData出错",m_table.m_table[rId].sFieldName);
			return NULL;
		}
		m_table.m_table[rId].pGtpData->SetCurKeyValue(\
			m_table.m_table[rId].sFieldName,\
			m_table.m_table[rId].sFieldValue,m_table.m_table[rId].cDataType);
		m_table.m_table[rId].bIsParseLocal = m_table.m_table[rId].pGtpData->ParseGtp(1);
		if (!m_table.m_table[rId].bIsParseLocal)
		{
			m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"array字段[%s]未解析去解析出错",m_table.m_table[rId].sFieldName);
			delete m_table.m_table[rId].pGtpData;
			m_table.m_table[rId].pGtpData = NULL;
			return NULL;
		}
	}
	return m_table.m_table[rId].pGtpData ;
}
CGtpData * CGtpData::GetDataPoint(unsigned int recnum)
{
	if (recnum > m_keyIndex.Size())
	{
		return NULL;
	}
	int rowid;
	int currec =0;
	bool bRet = m_keyIndex.First(rowid);
	while (bRet)
	{
		if (recnum == currec)
		{
			if (!m_table.m_table[rowid].bIsParseLocal)
			{
				m_table.m_table[rowid].pGtpData = new CGtpData();
				if (m_table.m_table[rowid].pGtpData == NULL)
				{
					m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"ARRAY字段[%s]未解析去解析new CGtpData出错",m_table.m_table[rowid].sFieldName);
					return NULL;
				}
				m_table.m_table[rowid].pGtpData->SetCurKeyValue(\
					m_table.m_table[rowid].sFieldName,\
					m_table.m_table[rowid].sFieldValue,m_table.m_table[rowid].cDataType);
				m_table.m_table[rowid].bIsParseLocal = m_table.m_table[rowid].pGtpData->ParseGtp(1);
				if (!m_table.m_table[rowid].bIsParseLocal)
				{
					m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"array字段[%s]未解析去解析出错",m_table.m_table[rowid].sFieldName);
					delete m_table.m_table[rowid].pGtpData;
					m_table.m_table[rowid].pGtpData = NULL;
					return NULL;
				}
			}
			return m_table.m_table[rowid].pGtpData;
		}
		currec++;
		bRet = m_keyIndex.Next(rowid);
	}
	return NULL;
	 
}


void CGtpData::Clear()
{
	int rowid;
	bool bRet = m_keyIndex.First(rowid);
	while (bRet)
	{
		if (m_table.m_table[rowid].pGtpData != NULL)
		{
			m_table.m_table[rowid].pGtpData->Clear();
			delete m_table.m_table[rowid].pGtpData;
			m_table.m_table[rowid].pGtpData = NULL;
		}
		bRet = m_keyIndex.Next(rowid);
	}
	m_table.Clear();
	m_keyIndex.Clear();
}

bool CGtpData::ParseGtp(int layer)
{
	if (m_cDataType == GTP_DATATYPE_UNKNOW)
	{
		m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"数据类型不符，无数据");
		return false;
	}
	if (layer >0) //先解析本层
	{
		switch (m_cDataType)
		{
			case GTP_DATATYPE_VALUE:
				return ParseGtpValue(layer);
				break;
			case GTP_DATATYPE_HASH:
				return ParseGtpHash(layer);
				break;
			case GTP_DATATYPE_ARRAY:
				return ParseGtpArray(layer);
				break;
			default:
				m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"数据类型不符，无数据");
				return false;
		}
	}
	return true;
}
int CGtpData::FindHashOffset(int begin, int end, const char *gtpdata)
{
	int i;
	int hashnum = 1;
	for (i=begin+1; i< end; i++)
	{
		//查找到对应的}
		if ('}' == (gtpdata+i)[0])
		{
			if ((gtpdata+i-1)[0] == '\\')  //前一个字符为转义
			{
				continue;
			}
			hashnum--;
			if (0 == hashnum ) //找到对应的{}了
			{
				return i;
			}
		}
		if ('{' == (gtpdata+i)[0])
		{
			if ((gtpdata+i-1)[0] == '\\')  //前一个字符为转义
			{
				continue;
			}
			hashnum++;
		}
	}
	return -1;
}

int CGtpData::FindArrayOffset(int begin, int end, const char *gtpdata)
{
	int i;
	int arraynum = 1;
	for (i=begin+1; i< end; i++)
	{
		//查找到对应的}
		if (']' == (gtpdata+i)[0])
		{
			if ((gtpdata+i-1)[0] == '\\')  //前一个字符为转义
			{
				continue;
			}
			arraynum--;
			if (0 == arraynum ) //找到对应的{}了
			{
				return i;
			}
		}
		if ('[' == (gtpdata+i)[0])
		{
			if ((gtpdata+i-1)[0] == '\\')  //前一个字符为转义
			{
				continue;
			}
			arraynum++;
		}
	}
	return -1;
}

bool CGtpData::ParseGtpValue(int layer)
{
	S_GTP_DATA_  gdata;
	char *gtpdata= (char *)m_sGtpValue.c_str();
	int datalen = m_sGtpValue.length();
	int keybegin = 0;
	int valuebegin=0;
	int valueend=0;
	bool isfindkey=true;
	for (int i=0; i< datalen ; )
	{
		if (isfindkey) //查找key
		{
			//一个一个查找=
			if (gtpdata[i] == '=' && gtpdata[i-1] != '\\') //找到一个key
			{
				if (i-keybegin >GTP_KEY_LEN)
				{
					m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"KEY[%s]值大于%d",m_sGtpValue.substr(keybegin,i-keybegin).c_str(),GTP_KEY_LEN);
					return false;
				}
				bzeroData(gdata);
				memcpy(gdata.sFieldName,gtpdata+keybegin,i-keybegin);
				valuebegin = i+1;
				i=valuebegin;
				if (i>=datalen)
				{
					m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"KEY[%s]无值 %d>%d  %s",gdata.sFieldName,i,datalen,m_sGtpValue.c_str());
					return false;
				}
				isfindkey = false;
				if (gtpdata[i] == '{' ) //哈希表
				{
					gdata.cDataType = GTP_DATATYPE_HASH;
				}
				else if (gtpdata[i] == '[' ) //数组
				{
					gdata.cDataType = GTP_DATATYPE_ARRAY;
				}
				else
				{
					gdata.cDataType = GTP_DATATYPE_VALUE;
				}
				continue;
			}
			i++;
		}
		else  //查找value
		{
			if (gdata.cDataType == GTP_DATATYPE_HASH ) //哈希表
			{
				valueend = FindHashOffset(i,datalen,gtpdata);
				if (valueend - valuebegin<=0)
				{
					m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"KEY[%s]无值 valueend[%d]<=valuebegin[%d] %s",gdata.sFieldName,valueend,valuebegin,m_sGtpValue.c_str());
					return false;
				}
				gdata.sFieldValue = m_sGtpValue.substr(valuebegin,valueend+1-valuebegin);
				//若layer大于则再解析
				if (layer-1>0)
				{
					gdata.pGtpData = new CGtpData();
					gdata.pGtpData->SetCurKeyValue(gdata.sFieldName,gdata.sFieldValue,GTP_DATATYPE_HASH);
					gdata.bIsParseLocal = gdata.pGtpData->ParseGtp(layer-1);
					if (!gdata.bIsParseLocal)
					{
						m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"解析KEY[%s]节点出错 %s",gdata.sFieldName,gdata.sFieldValue.c_str());
						delete gdata.pGtpData;
						gdata.pGtpData = NULL;
						return false;
					}
				}
				else
				{
					gdata.bIsParseLocal = false;
				}
				if (Insert(gdata)<0)
				{
					m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"KEY[%s]重复",gdata.sFieldName);
					return false;
				}

				keybegin = valueend+1;
				if (datalen>keybegin)
				{
					if (gtpdata[keybegin] != ',')
					{
						m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"KEY[%s]的值后无,分隔符 %d %s",gdata.sFieldName,keybegin,gtpdata+keybegin );
						return false;
					}
					else
					{
						keybegin++;
					}
				}
				i=keybegin;
				isfindkey = true;
				continue;
				
			}
			else if (gdata.cDataType == GTP_DATATYPE_ARRAY ) //数组
			{
				valueend = FindArrayOffset(i,datalen,gtpdata);
				if (valueend - valuebegin<=0)
				{
					m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"KEY[%s]无值 valueend[%d]<=valuebegin[%d] %s",gdata.sFieldName,valueend,valuebegin,m_sGtpValue.c_str());
					return false;
				}
				gdata.sFieldValue = m_sGtpValue.substr(valuebegin,valueend+1-valuebegin);
				//若layer大于1则再解析
				if (layer-1>0)
				{
					gdata.pGtpData = new CGtpData();
					gdata.pGtpData->SetCurKeyValue(gdata.sFieldName,gdata.sFieldValue ,GTP_DATATYPE_ARRAY);
					gdata.bIsParseLocal = gdata.pGtpData->ParseGtp(layer-1);
					if (!gdata.bIsParseLocal)
					{
						m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"解析KEY[%s]节点出错 %s",gdata.sFieldName,gdata.sFieldValue.c_str());
						delete gdata.pGtpData;
						gdata.pGtpData = NULL;
						return false;
					}
				}
				else
				{
				    gdata.bIsParseLocal = false;
				}
				if (Insert(gdata)<0)
				{
					m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"KEY[%s]重复",gdata.sFieldName);
					return false;
				}

				keybegin = valueend+1;
				if (datalen>keybegin)
				{
					if (gtpdata[keybegin] != ',')
					{
						m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"KEY[%s]的值后无,分隔符 %s",gdata.sFieldName,gtpdata+keybegin);
						return false;
					}
					else
					{
						keybegin++;
					}
				}
				i=keybegin;
				isfindkey = true;
				continue;
			}
			else //查找最后一个,  GTP_DATATYPE_DATA
			{
				if (gtpdata[i] == ',' && gtpdata[i-1] != '\\') //找到一个,分隔
				{
					valueend = i;
					gdata.sFieldValue = m_sGtpValue.substr(valuebegin,valueend-valuebegin);
					gdata.bIsParseLocal = true;
					if (Insert(gdata)<0)
					{
						m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"KEY[%s]重复",gdata.sFieldName);
						return false;
					}
					keybegin = valueend+1;
					i=keybegin;
					isfindkey = true;
					continue;
				}
				if (i == datalen-1) //最后一个了，没有,分隔
				{
					valueend = i;
					gdata.sFieldValue = m_sGtpValue.substr(valuebegin,valueend+1-valuebegin);
					gdata.bIsParseLocal = true;
					if (Insert(gdata)<0)
					{
						m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"KEY[%s]重复",gdata.sFieldName);
						return false;
					}
					return true;
				}
				i++;
			}
		}
	}
	return true;
}

bool CGtpData::ParseGtpHash(int layer)
{
	if (m_cDataType != GTP_DATATYPE_HASH)
	{
		m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"类型不符,不为GTP_DATATYPE_HASH");
		return false;
	}
	char *pgtpdata = (char *)m_sGtpValue.c_str();
	if (pgtpdata[0]  != '{' && pgtpdata[m_sGtpValue.length()-1] != '}')
	{
		m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"哈希表[%s]不符，不为{开始或不为}结束",m_sGtpKey.c_str());
		return false;
	}
	m_sGtpValue = m_sGtpValue.substr(1,m_sGtpValue.length()-2);
	return ParseGtpValue(layer);

}

bool CGtpData::ParseGtpArray(int layer)
{
	
	if (m_cDataType != GTP_DATATYPE_ARRAY)
	{
		m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"类型不符,不为GTP_DATATYPE_ARRAY");
		return false;
	}
	char *pgtpdata = (char *)m_sGtpValue.c_str();
	if (pgtpdata[0]  != '[' && pgtpdata[m_sGtpValue.length()-1] != ']')
	{
		m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"数组[%s]不符，不为[开始或不为]结束",m_sGtpKey.c_str());
		return false;
	}
	m_sGtpValue = m_sGtpValue.substr(1,m_sGtpValue.length()-2);
	S_GTP_DATA_  gdata;
	pgtpdata= (char *)m_sGtpValue.c_str();
	int datalen = m_sGtpValue.length();
	
	
	int nHashNum=0;
	//一个一个查找{
	if (pgtpdata[0] != '{') //找到一个{，hash开始
	{
		m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"数组[%s]中的哈希表不为{开始",m_sGtpKey.c_str());
		return false;
	}
	int hashbegin=0;
	int hashend=0;
	for (int i=0; i< datalen ; )
	{
		//查找hash表的结束位置
		hashend = FindHashOffset(i,datalen,pgtpdata);
		if (hashend - hashbegin<=0)
		{
			m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"数组[%s]中的哈希表[%d]不为}结束",\
				m_sGtpKey.c_str(),nHashNum);
			return false;
		}
		bzeroData(gdata);
		sprintf(gdata.sFieldName,"%d",nHashNum);
		gdata.sFieldValue = m_sGtpValue.substr(hashbegin,hashend+1-hashbegin);
		gdata.cDataType = GTP_DATATYPE_HASH;
		//若layer大于1则再解析
		if (layer-1>0)
		{
			gdata.pGtpData = new CGtpData();
			gdata.pGtpData->SetCurKeyValue(gdata.sFieldName,gdata.sFieldValue ,GTP_DATATYPE_HASH);
			gdata.bIsParseLocal = gdata.pGtpData->ParseGtp(layer-1);
			if (!gdata.bIsParseLocal)
			{
				m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"解析KEY[%s]节点出错",gdata.sFieldName);
				delete gdata.pGtpData;
				gdata.pGtpData = NULL;
				return false;
			}
		}
		else
		{
			gdata.bIsParseLocal = false;
		}
		if (Insert(gdata)<0)
		{
			m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"KEY[%s]重复",gdata.sFieldName);
			return false;
		}
		
		i=	 hashend+1;
		nHashNum++;
		
		if (i >= datalen-1) //最后一个了，没有,分隔
		{
			return true;
		}
		if (pgtpdata[i] != ',') //多个没有用,分隔
		{
			m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"数组[%s]中的哈希表[%d]之前无,分隔",\
				m_sGtpKey.c_str(),nHashNum);
			return false;
		}
		i++;
		hashbegin=i;
	}

	return true;
}

void CGtpData::bzeroData(S_GTP_DATA_ &data)
{
	data.cDataType= GTP_DATATYPE_UNKNOW;
	data.bIsParseLocal = false;
	data.pGtpData = NULL;
	bzero(data.sFieldName,sizeof(data.sFieldName));
	data.sFieldValue = "";
}

int CGtpData::Insert(S_GTP_DATA_ data)
{
	if (!m_keyIndex.Find(data.sFieldName))
	{
		int rowid = m_table.Add(data);
		m_keyIndex.Add(rowid,m_table.m_table[rowid].sFieldName);
		return rowid;
	}
	else
	{
		return -1;
	}
}
bool CGtpData::SetCurKeyValue(const std::string &key,const std::string &svalue, char datatype)
{
	m_sGtpKey = key;
	m_sGtpValue = svalue;
	m_cDataType = datatype;
	return true;
}

//转义符
//\, ,
//\{ {
//\} }
//\[ [
//\] ]
//\= =
int CGtpData::GtpGetTransSign(std::string &value)
{
	int nLength;
	char *tmpchar = (char *)malloc(value.length()+1);
	memset(tmpchar,0,value.length()+1);
	nLength = value.length();
	char *p = tmpchar;
	for (int i=0;i<nLength;)
	{
		if (value.c_str()[i] == '\\' && 
			( value.c_str()[i+1] == ',' || \
			  value.c_str()[i+1] == '{' || \
			  value.c_str()[i+1] == '}' || \
			  value.c_str()[i+1] == '[' || \
			  value.c_str()[i+1] == ']' ||  \
			  value.c_str()[i+1] == '=') ) 
		{
			//是转义
			i++;
			continue;
		}
		*p = value.c_str()[i];
		p++;
		i++;
	}
	value = tmpchar;
	p=NULL;
	free(tmpchar);
	return 0;
}
//转义符
//\, ,
//\{ {
//\} }
//\[ [
//\] ]
//\= =
int CGtpData::GtpPutTransSign(std::string &value)
{
	if (value.length()<1)
	{
		return 0;
	}
	char *tmpchar = (char *)malloc(value.length()+100);
	memset(tmpchar,0,value.length()+100);
	
	char *p = tmpchar;
	for (int i=0;i<(int)value.length();i++)
	{
		if (value.c_str()[i] == ',' )
		{
			strcpy(p,"\\,");
			p+=2;
		}
		else if (value.c_str()[i] == '{' )
		{
			strcpy(p,"\\{");
			p+=2;
		}
		else if (value.c_str()[i] == '}' )
		{
			strcpy(p,"\\}");
			p+=2;
		}
		else if (value.c_str()[i] == '[' )
		{
			strcpy(p,"\\[");
			p+=2;
		}
		else if (value.c_str()[i] == ']' )
		{
			strcpy(p,"\\]");
			p+=2;
		}
		else if (value.c_str()[i] == '=' )
		{
			strcpy(p,"\\=");
			p+=2;
		}
		else
		{
			*p = value.c_str()[i];
			p++;
		}
	}
	*(p+1)= 0;
	value = tmpchar;
	p = NULL;
	free(tmpchar);
	return 0;
}
bool CGtpData::SetDataItem(const char *fieldname,const std::string &fieldvalue,bool istrans)
{
	return SetDataItem(fieldname,fieldvalue.c_str(),istrans);
}
bool CGtpData::SetDataItem(const char *fieldname, const char *fieldvalue, bool istrans)
{
	CInt iSet;
	std::string ss;
	ss = fieldvalue;
	if (m_cDataType == GTP_DATATYPE_ARRAY)
	{
		//数组下面不能直接设置值
		m_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"数组下面不能直接设置值,要通过AddDataItem增加哈希表再设置值");
		return false;
	}
	if (m_cDataType == GTP_DATATYPE_UNKNOW)
	{
		if (!SetCurKeyValue("","",GTP_DATATYPE_VALUE))
		{
			return false;
		}
	}
	if (istrans)
	{
		GtpPutTransSign(ss);
	}
	bool bRet = m_keyIndex.Select(iSet,(char *)fieldname);
	if (bRet) //有此数据项，更新
	{
		int rowid;
		iSet.First(rowid);
		m_table.m_table[rowid].sFieldValue = ss;
		//如果下面有数据，则清除
		if (m_table.m_table[rowid].pGtpData != NULL)
		{
			delete m_table.m_table[rowid].pGtpData;
			m_table.m_table[rowid].pGtpData = NULL;
		}
		m_table.m_table[rowid].cDataType = GTP_DATATYPE_VALUE;
		m_table.m_table[rowid].bIsParseLocal = true;
	}
	else
	{
		S_GTP_DATA_ data;
		data.bIsParseLocal = true;
		data.cDataType = GTP_DATATYPE_VALUE;
		data.pGtpData = NULL;
		CBF_Tools::StringCopy(data.sFieldName,GTP_KEY_LEN,fieldname);
		data.sFieldValue = ss;
		if (Insert(data)<0)
		{
			return false;
		}
	}
	return true;
}
bool CGtpData::SetDataItem(const char *fieldname,int fieldvalue)
{
	char tmpchar[40];
	sprintf(tmpchar,"%d",fieldvalue);
	return SetDataItem(fieldname,tmpchar,false);
}
bool CGtpData::SetDataItem(const char *fieldname,char fieldvalue)
{
	char tmpchar[2];
	tmpchar[0]=fieldvalue;
	tmpchar[1]=0;
	//sprintf(tmpchar,"%c",fieldvalue);
	return SetDataItem(fieldname,tmpchar,false);
}
bool CGtpData::SetDataItem(const char *fieldname,unsigned int fieldvalue)
{
	char tmpchar[40];
	sprintf(tmpchar,"%ld",fieldvalue);
	return SetDataItem(fieldname,tmpchar,false);
}
bool CGtpData::SetDataItem(const char *fieldname,short fieldvalue)
{
	char tmpchar[40];
	sprintf(tmpchar,"%d",fieldvalue);
	return SetDataItem(fieldname,tmpchar,false);
}
bool CGtpData::SetDataItem(const char *fieldname,unsigned short fieldvalue)
{
	char tmpchar[40];
	sprintf(tmpchar,"%d",fieldvalue);
	return SetDataItem(fieldname,tmpchar,false);
}
bool CGtpData::SetDataItem(const char *fieldname,long fieldvalue)
{
	char tmpchar[40];
 	sprintf(tmpchar,INT64PFM,fieldvalue);
	return SetDataItem(fieldname,tmpchar,false);
}
bool CGtpData::SetDataItem(const char *fieldname,unsigned long fieldvalue)
{
	char tmpchar[40];
	sprintf(tmpchar,INT64PFM,fieldvalue);
	return SetDataItem(fieldname,tmpchar,false);
}
bool CGtpData::SetDataItem(const char *fieldname,double fieldvalue,int dec)
{
	char tmpchar[40];
	switch (dec)
	{
	case 1:
		sprintf(tmpchar,"%.1f",fieldvalue);
		break;
	case 2:
		sprintf(tmpchar,"%.2f",fieldvalue);
		break;
	case 3:
		sprintf(tmpchar,"%.3f",fieldvalue);
		break;
	case 4:
		sprintf(tmpchar,"%.4f",fieldvalue);
		break;
	case 5:
		sprintf(tmpchar,"%.5f",fieldvalue);
		break;
	case 6:
		sprintf(tmpchar,"%.6f",fieldvalue);
		break;
	default:
		sprintf(tmpchar,"%f",fieldvalue);
		break;
	}
	
	return SetDataItem(fieldname,tmpchar,false);
}

CGtpData * CGtpData::AddDataItem(const char *fieldname, char datatype)
{
	CInt iSet;
	bool bRet = m_keyIndex.Select(iSet,(char *)fieldname);
	if (bRet) //有此数据项，退出
	{
		return NULL;
	}
	S_GTP_DATA_ data;
	bzeroData(data);
	data.bIsParseLocal = true;
	data.cDataType = datatype;
	CBF_Tools::StringCopy(data.sFieldName,GTP_KEY_LEN,fieldname);
	data.pGtpData = new CGtpData();
	if (data.pGtpData == NULL)
	{
		return NULL;
	}
	data.pGtpData->SetCurKeyValue(fieldname,"",datatype);
	if (Insert(data)<0)
	{
		delete data.pGtpData;
		data.pGtpData = NULL;
		return NULL;
	}
	return data.pGtpData;
}

bool CGtpData::ToStringDelRec(std::string &gtpstring, int delnum)
{
	gtpstring= "";
	std::string tmpstr="";
	bool bRet;
	S_GTP_DATA_ data;
	int recnum=0;
	if (m_cDataType == GTP_DATATYPE_VALUE)  //kv格式
	{
		bRet = m_table.First(data);
		while (bRet )
		{
			if (recnum < delnum)
			{
				bRet = m_table.Next(data);
				recnum++;
				continue;
			}
			recnum++;
			if (data.bIsParseLocal && data.pGtpData != NULL) //HASH或数组
			{
				gtpstring=gtpstring+data.sFieldName+"=";
				tmpstr="";
				if (!data.pGtpData->ToString(tmpstr))
				{
					return false;
				}
				gtpstring=gtpstring+tmpstr;
			}
			else
			{
				gtpstring=gtpstring+data.sFieldName+"="+data.sFieldValue;
			}
			
			bRet = m_table.Next(data);
			if (bRet) //还有，要加,
			{
				gtpstring+=",";
			}
		}
		
	}
	else if (m_cDataType == GTP_DATATYPE_HASH)  //哈希格式
	{
		gtpstring+="{";
		
		bRet = m_table.First(data);
		while (bRet )
		{
			if (data.bIsParseLocal && data.pGtpData != NULL) //HASH或数组
			{
				gtpstring=gtpstring+data.sFieldName+"=";
				tmpstr="";
				if (!data.pGtpData->ToString(tmpstr))
				{
					return false;
				}
				gtpstring+=tmpstr;
			}
			else
			{
				gtpstring=gtpstring+data.sFieldName+"="+data.sFieldValue;
			}
			
			bRet = m_table.Next(data);
			if (bRet) //还有，要加,
			{
				gtpstring+=",";
			}
		}
		gtpstring+="}";
	}
	else if (m_cDataType == GTP_DATATYPE_ARRAY)  //数组格式
	{
		gtpstring+="[";
		bRet = m_table.First(data);
		while (bRet )
		{
			if (data.bIsParseLocal && data.pGtpData != NULL) //HASH或数组
			{
				tmpstr="";
				if (!data.pGtpData->ToString(tmpstr))
				{
					return false;
				}
				gtpstring+=tmpstr;
			}
			else if (data.bIsParseLocal)
			{
				gtpstring=gtpstring+data.sFieldName+"="+data.sFieldValue;
			}
			else
			{
				gtpstring=gtpstring+data.sFieldValue;
			}
			bRet = m_table.Next(data);
			if (bRet) //还有，要加,
			{
				gtpstring+=",";
			}
		}
		gtpstring+="]";
	}
	return true;

}


bool CGtpData::UpdateCurValue(CGtpData *src,bool isUpdate)
{
	int fieldcount = src->FieldCount();
	std::string skey;
	std::string svalue;
	int fieldtype;
	int nRet;
	CInt iSet;
	for (int i=0; i<fieldcount; i++)
	{
		nRet = src->GetValueByName(i,skey,svalue,fieldtype);
		if (nRet<0) //类型不是value
		{
			continue;
		}
		if (isUpdate) //更新
		{
			SetDataItem(skey.c_str(),svalue.c_str());
		}
		else
		{
			iSet.Clear();			
			if (!m_keyIndex.Select(iSet,(char *)skey.c_str()))
			{
				//没有找到
				SetDataItem(skey.c_str(),svalue.c_str());
			}
			//找到了不用更新
		}
	}
	return true;
}

int CGtpData::GetValueByName(unsigned int recnum,std::string &fieldname, std::string &fieldvalue, int &fieldtype)
{
	if (recnum > m_keyIndex.Size())
	{
		return -3;
	}
	int rId;
	int currec =0;
	bool isfind=false;
	bool bRet = m_keyIndex.First(rId);
	while (bRet)
	{
		if (recnum == currec)
		{
			isfind = true;
			break;
		}
		currec++;
		bRet = m_keyIndex.Next(rId);
	}
	if (!isfind)
	{
		return -3;
	}

	CInt iSet;
	fieldtype = m_table.m_table[rId].cDataType;
	if (fieldtype != GTP_DATATYPE_VALUE)
	{
		return -2;//类型不符
	}
	else
	{
		fieldvalue = m_table.m_table[rId].sFieldValue;
		GtpGetTransSign(fieldvalue);
		fieldname = m_table.m_table[rId].sFieldName;
		GtpGetTransSign(fieldname);
	}
	return rId;
}

int CGtpData::GetValueByName(unsigned int recnum,std::string &fieldname, char *fieldvalue, int &fieldtype)
{
	std::string ss;
	int nRet= GetValueByName(recnum,fieldname,ss,fieldtype);
	if (nRet<0)
	{
		return nRet;
	}
	strcpy(fieldvalue,ss.c_str());
	return nRet;
}

int CGtpData::GetValueByName(unsigned int recnum,std::string &fieldname, int &fieldvalue, int &fieldtype)
{
	std::string ss;
	int nRet= GetValueByName(recnum,fieldname,ss,fieldtype);
	if (nRet<0)
	{
		return nRet;
	}
	if (fieldtype != GTP_DATATYPE_VALUE)
	{
		return -2;
	}
	fieldvalue = atoi(ss.c_str());
	return nRet;
}

int CGtpData::GetValueByName(unsigned int recnum,std::string &fieldname, double &fieldvalue, int &fieldtype)
{
	std::string ss;
	int nRet= GetValueByName(recnum,fieldname,ss,fieldtype);
	if (nRet<0)
	{
		return nRet;
	}
	if (fieldtype != GTP_DATATYPE_VALUE)
	{
		return -2;
	}
	fieldvalue = atof(ss.c_str());
	return nRet;
}

int CGtpData::GetValueByName(unsigned int recnum,std::string &fieldname, unsigned int &fieldvalue, int &fieldtype)
{
	std::string ss;
	int nRet= GetValueByName(recnum,fieldname,ss,fieldtype);
	if (nRet<0)
	{
		return nRet;
	}
	if (fieldtype != GTP_DATATYPE_VALUE)
	{
		return -2;
	}
	fieldvalue = atol(ss.c_str());
	return nRet;
}

int CGtpData::GetValueByName(unsigned int recnum,std::string &fieldname, unsigned long &fieldvalue, int &fieldtype)
{
	std::string ss;
	int nRet= GetValueByName(recnum,fieldname,ss,fieldtype);
	if (nRet<0)
	{
		return nRet;
	}
	if (fieldtype != GTP_DATATYPE_VALUE)
	{
		return -2;
	}
	fieldvalue = ATOI64(ss.c_str());
	return nRet;
}

int CGtpData::GetValueByName(unsigned int recnum,std::string &fieldname, unsigned short &fieldvalue, int &fieldtype)
{
	std::string ss;
	int nRet= GetValueByName(recnum,fieldname,ss,fieldtype);
	if (nRet<0)
	{
		return nRet;
	}
	if (fieldtype != GTP_DATATYPE_VALUE)
	{
		return -2;
	}
	fieldvalue = atoi(ss.c_str());
	return nRet;
}

int CGtpData::GetValueByName(unsigned int recnum,std::string &fieldname, short &fieldvalue, int &fieldtype)
{
	std::string ss;
	int nRet= GetValueByName(recnum,fieldname,ss,fieldtype);
	if (nRet<0)
	{
		return nRet;
	}
	if (fieldtype != GTP_DATATYPE_VALUE)
	{
		return -2;
	}
	fieldvalue = atoi(ss.c_str());
	return nRet;
}

int CGtpData::GetValueByName(unsigned int recnum,std::string &fieldname, long &fieldvalue, int &fieldtype)
{
	std::string ss;
	int nRet= GetValueByName(recnum,fieldname,ss,fieldtype);
	if (nRet<0)
	{
		return nRet;
	}
	if (fieldtype != GTP_DATATYPE_VALUE)
	{
		return -2;
	}
	fieldvalue = atol(ss.c_str());
	return nRet;
}
int CGtpData::GetValueByName(unsigned int recnum,std::string &fieldname, unsigned char &fieldvalue, int &fieldtype)
{
	std::string ss;
	int nRet= GetValueByName(recnum,fieldname,ss,fieldtype);
	if (nRet<0)
	{
		return nRet;
	}
	if (fieldtype != GTP_DATATYPE_VALUE)
	{
		return -2;
	}
	fieldvalue = ss.c_str()[0];
	return nRet;
}