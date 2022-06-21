// SqlDataResult.cpp: implementation of the CSqlDataResult class.
//
//////////////////////////////////////////////////////////////////////

#include "SqlDataResult.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/*****************************
*       数据记录类
*   每条记录作为一个类保存
******************************/
CSqlAPIData::CSqlAPIData()
{
   SqlFieldCount=0;
   SqlRecordBuffer.clear();
}
CSqlAPIData::~CSqlAPIData()
{
	SqlRecordBuffer.clear();
}
int CSqlAPIData::Add(char *buffer)
{
	S_RECORD ss;
    ss.value = std::string(buffer);
    SqlRecordBuffer.push_back(ss);
    SqlFieldCount++;
    return 0;
}
int CSqlAPIData::Add(std::string buffer)
{
	S_RECORD ss;
    ss.value = buffer;
    SqlRecordBuffer.push_back(ss);
    SqlFieldCount++;
    return 0;
}

int CSqlAPIData::Add(S_RECORD sdata)
{
	SqlRecordBuffer.push_back(sdata);
    SqlFieldCount++;
    return 0;
}
int CSqlAPIData::GetAt(int nIndex,char *FieldData)
{
	char buf[4096];
	memset(buf,0,sizeof(buf));
    if(nIndex>=0 && nIndex<SqlFieldCount)
	{
		strcpy(buf,SqlRecordBuffer[nIndex].value.c_str());
   		strcpy(FieldData,buf);
		return 0;
	}
    else
		return -1;
}
int CSqlAPIData::GetAt(int nIndex,double &FieldData)
{
   if(nIndex>=0 && nIndex<SqlFieldCount)
   {
     	FieldData= atof(SqlRecordBuffer[nIndex].value.c_str());
     	return 0;
   }
   else
		return -1;
}
int CSqlAPIData::GetAt(int nIndex,int &FieldData)
{
   if(nIndex>=0 && nIndex<SqlFieldCount)
   {
		FieldData= atoi(SqlRecordBuffer[nIndex].value.c_str());
     	return 0;
   }
   else
		return -1;
}
int CSqlAPIData::GetAt(int nIndex,long &FieldData)
{
   if(nIndex>=0 && nIndex<SqlFieldCount)
   {
	    FieldData = atol(SqlRecordBuffer[nIndex].value.c_str());
		return 0;
   }
   else
		return -1;
}

int CSqlAPIData::SetAt(char *buffer,int nIndex)
{
	if (nIndex>=0 && nIndex<SqlFieldCount )
	{
		int i;
		std::vector <S_RECORD> ss;
		for (i=0;i<SqlFieldCount;i++)
		{
			if (i == nIndex)
			{
				S_RECORD rc;
				rc.value = std::string(buffer);
				ss.push_back(rc);
			}
			else
			{
				ss.push_back(SqlRecordBuffer[i]);
			}
		}
		SqlRecordBuffer.clear();
		for (i=0;i<SqlFieldCount;i++)
		{
			SqlRecordBuffer.push_back(ss[i]);
		}
	}
	return 0;
}
int CSqlAPIData::SetAt(std::string buffer,int nIndex)
{
	if (nIndex>=0 && nIndex<SqlFieldCount )
	{
		int i;
		std::vector <S_RECORD> ss;
		for (i=0;i<SqlFieldCount;i++)
		{
			if (i == nIndex)
			{
				S_RECORD rc;
				rc.value = buffer;
				ss.push_back(rc);
			}
			else
			{
				ss.push_back(SqlRecordBuffer[i]);
			}
		}
		SqlRecordBuffer.clear();
		for (i=0;i<SqlFieldCount;i++)
		{
			SqlRecordBuffer.push_back(ss[i]);
		}
	}
	return 0;
}
int CSqlAPIData::GetCount()
{
    return SqlFieldCount;
}

int CSqlAPIData::Clear()
{
	SqlFieldCount=0;
	SqlRecordBuffer.clear();
	/*
	for(i=0;i<SQLAPI_FIELD_MAXCOUNT;i++)
		 memset(&SqlRecordBuffer[i],0,sizeof(SqlRecordBuffer[i]));
	*/
	return 0;
}
char *CSqlAPIData::GetAt(int nIndex)  //取指定字段的char值 从0开始
{
	static char FieldData[4096];
	memset(FieldData,0,sizeof(FieldData));
    if(nIndex>=0 && nIndex<SqlFieldCount)
	{
		strcpy(FieldData,SqlRecordBuffer[nIndex].value.c_str());
	}
	return FieldData;
}
double CSqlAPIData::GetAtD(int nIndex)          //取指定字段 从0开始
{
	if(nIndex>=0 && nIndex<SqlFieldCount)
	{
	    return atof(SqlRecordBuffer[nIndex].value.c_str());
	}
	return 0;
}

long CSqlAPIData::GetAtL(int nIndex)            //取指定字段 从0开始
{
	if(nIndex>=0 && nIndex<SqlFieldCount)
	{
	    return atol(SqlRecordBuffer[nIndex].value.c_str());
	}
	return 0;
}

int CSqlAPIData::GetAtI(int nIndex) 
{
	if(nIndex>=0 && nIndex<SqlFieldCount)
	{
	    return atoi(SqlRecordBuffer[nIndex].value.c_str());
	}
	return 0;
}
std::string CSqlAPIData::GetAtS(int nIndex) 
{
	if(nIndex>=0 && nIndex<SqlFieldCount)
	{
	    return SqlRecordBuffer[nIndex].value;
	}
	return NULL;
}
char *CSqlAPIData::GetAt(std::string fieldname)  //取指定字段的char值 
{
	static char FieldData[4096];
	memset(FieldData,0,sizeof(FieldData));
	for (int i=0;i<SqlFieldCount;i++)
	{
		if (SqlRecordBuffer[i].name.compare(fieldname) == 0)
		{
			strcpy(FieldData,SqlRecordBuffer[i].value.c_str());
			return FieldData;
		}
	}
 	return FieldData;
}
double CSqlAPIData::GetAtD(std::string fieldname)          //取指定字段 从0开始
{
	for (int i=0;i<SqlFieldCount;i++)
	{
		if (SqlRecordBuffer[i].name.compare(fieldname) == 0)
		{
			return atof(SqlRecordBuffer[i].value.c_str());
		}
	}
	return 0;
}

long CSqlAPIData::GetAtL(std::string fieldname)            //取指定字段 从0开始
{
	for (int i=0;i<SqlFieldCount;i++)
	{
		if (SqlRecordBuffer[i].name.compare(fieldname) == 0)
		{
			return atol(SqlRecordBuffer[i].value.c_str());
		}
	}
	return 0;
}

int CSqlAPIData::GetAtI(std::string fieldname) 
{
	for (int i=0;i<SqlFieldCount;i++)
	{
		if (SqlRecordBuffer[i].name.compare(fieldname) == 0)
		{
			return atoi(SqlRecordBuffer[i].value.c_str());
		}
	}
	return 0;
}
std::string CSqlAPIData::GetAtS(std::string fieldname)
{
	for (int i=0;i<SqlFieldCount;i++)
	{
		if (SqlRecordBuffer[i].name.compare(fieldname) == 0)
		{
			return SqlRecordBuffer[i].value;
		}
	}
	return NULL;
}
int CSqlAPIData::GetAt(std::string fieldname,char *FieldData)
{
	char buf[4096];
	memset(buf,0,sizeof(buf));
	for (int i=0;i<SqlFieldCount;i++)
	{
		if (SqlRecordBuffer[i].name.compare(fieldname) == 0)
		{
			strcpy(buf,SqlRecordBuffer[i].value.c_str());
			strcpy(FieldData,buf);
			return 0;
		}
	}
    return -1;
}
int CSqlAPIData::GetAt(std::string fieldname,double &FieldData)
{
	for (int i=0;i<SqlFieldCount;i++)
	{
		if (SqlRecordBuffer[i].name.compare(fieldname) == 0)
		{
			FieldData= atof(SqlRecordBuffer[i].value.c_str());
			return 0;
		}
	}
	return -1;
}
int CSqlAPIData::GetAt(std::string fieldname,int &FieldData)
{
	for (int i=0;i<SqlFieldCount;i++)
	{
		if (SqlRecordBuffer[i].name.compare(fieldname) == 0)
		{
			FieldData= atoi(SqlRecordBuffer[i].value.c_str());
			return 0;
		}
	}
	return -1;
}
int CSqlAPIData::GetAt(std::string fieldname,long &FieldData)
{
	for (int i=0;i<SqlFieldCount;i++)
	{
		if (SqlRecordBuffer[i].name.compare(fieldname) == 0)
		{
			FieldData= atol(SqlRecordBuffer[i].value.c_str());
			return 0;
		}
	}
	return -1;
}

/********************************
*     数据集合类
*  提供方法给调用者取数据
*********************************/

CSqlData::~CSqlData()
{
	SqlData.clear();
}
CSqlData::CSqlData()
{
    SqlCount=0;
    SqlData.clear();
}
int CSqlData::put(CSqlAPIData sqldata)
{
    SqlData.push_back(sqldata);
    SqlCount++;
    return 0;
}
int CSqlData::GetAt(int Record,int FieldNo,int &FieldData)
{
    if (SqlCount<1)
    {
        return -1;
    }
    CSqlAPIData data = SqlData[Record];
    int ret =  data.GetAt(FieldNo,FieldData);
    if (ret <0) return -1;
    return 0;
}
int CSqlData::GetAt(int Record,int FieldNo,long &FieldData)
{
    if (SqlCount<1)
    {
        return -1;
    }
    CSqlAPIData data = SqlData[Record];
    int ret =  data.GetAt(FieldNo,FieldData);
    if (ret <0) return -1;
    return 0;
}
int CSqlData::GetAt(int Record,int FieldNo,double &FieldData)
{
    if (SqlCount<1)
    {
        return -1;
    }
    CSqlAPIData data = SqlData[Record];
    int ret =  data.GetAt(FieldNo,FieldData);
    if (ret <0) return -1;
    return 0;
}
int CSqlData::GetAt(int Record,int FieldNo,char * FieldData)
{
    if (SqlCount<1)
    {
        return -1;
    }
    CSqlAPIData data = (CSqlAPIData )SqlData[Record];
    int ret = data.GetAt(FieldNo,FieldData);
    if (ret <0) return -1;
    return 0;
}
int CSqlData::clear()
{
    SqlData.clear();
    return 0;
}
int CSqlData::GetSqlCount()
{
    return SqlCount;
}
CSqlAPIData CSqlData::GetAt(int index)
{
    CSqlAPIData data;
    if (SqlCount<1 || index>=SqlCount)
    {
        return data;
    }
    data = (CSqlAPIData )SqlData[index];
    return data;
}
void CSqlData::setSqlCount(int count)
{
    SqlCount = count;
    return;
}
char *CSqlData::GetAt(int Record,int FieldNo)
{
	static char FieldData[4096];
	memset(FieldData,0,sizeof(FieldData));
    if (SqlCount<1)
    {
        return FieldData;
    }
    CSqlAPIData data = SqlData[Record];
    int ret =  data.GetAt(FieldNo,FieldData);
    return FieldData;
}
double CSqlData::GetAtD(int Record,int FieldNo)
{
    if (SqlCount<1)
    {
        return 0;
    }
    CSqlAPIData data = SqlData[Record];
    return data.GetAtD(FieldNo);
}
long CSqlData::GetAtL(int Record,int FieldNo)
{
    if (SqlCount<1)
    {
        return 0;
    }
    CSqlAPIData data = SqlData[Record];
    return data.GetAtL(FieldNo);
}
int CSqlData::GetAtI(int Record,int FieldNo)
{
    if (SqlCount<1)
    {
        return 0;
    }
    CSqlAPIData data = SqlData[Record];
    return data.GetAtI(FieldNo);
}
std::string CSqlData::GetAtS(int Record,int FieldNo)
{
    if (SqlCount<1)
    {
        return NULL;
    }
    CSqlAPIData data = SqlData[Record];
    return data.GetAtS(FieldNo);
}

int CSqlData::GetAt(int Record,std::string fieldname,int &FieldData)
{
    if (SqlCount<1)
    {
        return -1;
    }
    CSqlAPIData data = SqlData[Record];
    int ret =  data.GetAt(fieldname,FieldData);
    if (ret <0) return -1;
    return 0;
}
int CSqlData::GetAt(int Record,std::string fieldname,long &FieldData)
{
    if (SqlCount<1)
    {
        return -1;
    }
    CSqlAPIData data = SqlData[Record];
    int ret =  data.GetAt(fieldname,FieldData);
    if (ret <0) return -1;
    return 0;
}
int CSqlData::GetAt(int Record,std::string fieldname,double &FieldData)
{
    if (SqlCount<1)
    {
        return -1;
    }
    CSqlAPIData data = SqlData[Record];
    int ret =  data.GetAt(fieldname,FieldData);
    if (ret <0) return -1;
    return 0;
}
int CSqlData::GetAt(int Record,std::string fieldname,char * FieldData)
{
    if (SqlCount<1)
    {
        return -1;
    }

    CSqlAPIData data = (CSqlAPIData )SqlData[Record];
    int ret = data.GetAt(fieldname,FieldData);
    if (ret <0) return -1;
    return 0;
}
char *CSqlData::GetAt(int Record,std::string fieldname)
{
	static char FieldData[4096];
	memset(FieldData,0,sizeof(FieldData));
    if (SqlCount<1)
    {
        return FieldData;
    }
    CSqlAPIData data = SqlData[Record];
    int ret =  data.GetAt(fieldname,FieldData);
    return FieldData;
}
double CSqlData::GetAtD(int Record,std::string fieldname)
{
    if (SqlCount<1)
    {
        return 0;
    }
    CSqlAPIData data = SqlData[Record];
    return data.GetAtD(fieldname);
}
long CSqlData::GetAtL(int Record,std::string fieldname)
{
    if (SqlCount<1)
    {
        return 0;
    }
    CSqlAPIData data = SqlData[Record];
    return data.GetAtL(fieldname);
}
int CSqlData::GetAtI(int Record,std::string fieldname)
{
    if (SqlCount<1)
    {
        return 0;
    }
    CSqlAPIData data = SqlData[Record];
    return data.GetAtI(fieldname);
}
std::string CSqlData::GetAtS(int Record,std::string fieldname)
{
    if (SqlCount<1)
    {
        return NULL;
    }
    CSqlAPIData data = SqlData[Record];
    return data.GetAtS(fieldname);
}
