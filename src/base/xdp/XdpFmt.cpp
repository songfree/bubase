// XdpFmt.cpp: implementation of the CXdpFmt class.
//
//////////////////////////////////////////////////////////////////////

#include "XdpFmt.h"
#include "BF_Tools.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXdpFmt::CXdpFmt()
{
	m_nMaxLength = 0;
	m_nFieldNum = 0;
}

CXdpFmt::~CXdpFmt()
{

}

bool CXdpFmt::Init(const char *xmlfilepathname,char *errmsg)
{
	CBF_Xml         xml;
	if (!xml.FromFile(xmlfilepathname))
	{
		sprintf(errmsg,"配置文件[%s]不存在或格式非法",xmlfilepathname);
		return false;
	}
	m_nMaxLength = 0;
	m_table.Clear();
	m_index.Clear();
	m_indexname.Clear();

	CXmlNode *node =  xml.GetNodeByPath("/xdp",false);
	if (NULL == node)
	{
		sprintf(errmsg,"配置文件[%s]格式非法，无xdp节点",xmlfilepathname);
		return false;
	}
	int version;
	if (xml.GetNodeAttribute("/xdp",false,"version",false,version) == NULL)
	{
		sprintf(errmsg,"配置文件[%s]格式非法，无xdp节点下无version属性",xmlfilepathname);
		return false;
	}
	if (version < 0 || version > 255)
	{
		sprintf(errmsg,"XDP版本为0-255之间的值");
		return false;
	}
	m_cVersion = version;
	CXmlNode *fieldnode = (CXmlNode *)node->GetFirstChild();
	if (NULL == fieldnode)
	{
		sprintf(errmsg,"配置文件[%s]格式非法，xdp节点下无配置项",xmlfilepathname);
		return false;
	}
	S_XDP_FIELD_DEFINE  sfield;
	CXmlAttribute *attr = NULL;
	int intvalue=0;
	int index=0;
	while (fieldnode != NULL)
	{
		if (fieldnode->GetNodeName(false).length() > XDP_FIELD_NAMELEN)
		{
			sprintf(errmsg,"配置文件[%s]格式非法，节点[/xdp/%s]名称长度大于20",xmlfilepathname, fieldnode->GetNodeName(false).c_str());
			return false;
		}
		CBF_Tools::StringCopy(sfield.f_name, sizeof(sfield.f_name) - 1, fieldnode->GetNodeName(false).c_str());
		attr = fieldnode->GetAttribute("index",false);
		if (NULL == attr )
		{
			sprintf(errmsg,"配置文件[%s]格式非法，节点[/xdp/%s]无index属性",xmlfilepathname, sfield.f_name);
			return false;
		}
		intvalue = atoi(attr->GetAttrValue(false).c_str());
		if  (intvalue<0 || intvalue>XDP_BITMAPSIZE*8)
		{
			sprintf(errmsg,"配置文件[%s]格式非法，节点[/xdp/%s]的属性index[%s]不符",xmlfilepathname,\
				sfield.f_name,attr->GetAttrValue(false).c_str());
			return false;
		}
		sfield.f_index = intvalue;
		if (sfield.f_index != index)
		{
			sprintf(errmsg,"配置文件[%s]格式非法，节点[/xdp/%s]的属性index[%s]不是按顺序生成",xmlfilepathname,\
				sfield.f_name,attr->GetAttrValue(false).c_str());
			return false;
		}
		
		attr = fieldnode->GetAttribute("type",false);
		if (NULL == attr )
		{
			sprintf(errmsg,"配置文件[%s]格式非法，节点[/xdp/%s]无type属性",xmlfilepathname, sfield.f_name);
			return false;
		}
		sfield.f_type = GetType(attr->GetAttrValue(false).c_str());
		if (XDP_UNKNOW == sfield.f_type)
		{
			sprintf(errmsg,"配置文件[%s]格式非法，节点[/xdp/%s]type属性[%s]非法",xmlfilepathname,\
				sfield.f_name,attr->GetAttrValue(false).c_str());
			return false;
		}
		switch (sfield.f_type)
		{
			case  XDP_USHORT:
				sfield.f_length = sizeof(short);
				sfield.f_extlength= 0;
				break;
			case  XDP_SHORT:
				sfield.f_length = sizeof(short);
				sfield.f_extlength= 0;
				break;
			case  XDP_UINT:
				sfield.f_length = sizeof(int);
				sfield.f_extlength= 0;
				break;
			case  XDP_INT:
				sfield.f_length = sizeof(int);
				sfield.f_extlength= 0;
				break;
			case  XDP_DOUBLE:
				sfield.f_length = sizeof(double);
				sfield.f_extlength= 0;
				break;
            case  XDP_UINT64:
			case  XDP_INT64:
                sfield.f_length = sizeof(INT64_);
                sfield.f_extlength = 0;
                break;
			case  XDP_CHAR:
				attr = fieldnode->GetAttribute("length",false);
				if (NULL == attr )
				{
					sprintf(errmsg,"配置文件[%s]格式非法，节点[/xdp/%s]无length属性",xmlfilepathname, sfield.f_name);
					return false;
				}
				intvalue = atoi(attr->GetAttrValue(false).c_str());
				if (intvalue <1 || intvalue>XDP_FIELD_CHARLEN)
				{
					sprintf(errmsg,"配置文件[%s]格式非法，节点[/xdp/%s]length属性[%s]非法，应小于255",xmlfilepathname,\
						sfield.f_name,attr->GetAttrValue(false).c_str());
					return false;
				}
				sfield.f_length = intvalue;
				sfield.f_extlength= 0;
				break;
			case  XDP_VARCHAR:
				attr = fieldnode->GetAttribute("length",false);
				if (NULL == attr )
				{
					sprintf(errmsg,"配置文件[%s]格式非法，节点[/xdp/%s]无length属性",xmlfilepathname, sfield.f_name);
					return false;
				}
				intvalue = atoi(attr->GetAttrValue(false).c_str());
				if (intvalue <1 || intvalue>XDP_FIELD_CHARLEN)
				{
					sprintf(errmsg,"配置文件[%s]格式非法，节点[/xdp/%s]length属性[%s]非法，应小于255",xmlfilepathname,\
						sfield.f_name,attr->GetAttrValue(false).c_str());
					return false;
				}
				sfield.f_length = intvalue;
				sfield.f_extlength= 1;
				break;
			case  XDP_BINDATA:
				attr = fieldnode->GetAttribute("length",false);
				if (NULL == attr )
				{
					sprintf(errmsg,"配置文件[%s]格式非法，节点[/xdp/%s]无length属性",xmlfilepathname, sfield.f_name);
					return false;
				}
				intvalue = atoi(attr->GetAttrValue(false).c_str());
				if (intvalue <1 || intvalue >XDP_FIELD_BINLEN)
				{
					sprintf(errmsg,"配置文件[%s]格式非法，节点[/xdp/%s]length属性[%s]非法，应小于4096",xmlfilepathname,\
						sfield.f_name,attr->GetAttrValue(false).c_str());
					return false;
				}
				sfield.f_length = intvalue;
				sfield.f_extlength= sizeof(short);
				break;
			default:
                sprintf(errmsg, "配置文件[%s]格式非法，节点[/xdp/%s]未知的字段类型", xmlfilepathname, \
                    sfield.f_name);
				return false;

		}
		if (!AddFmt(sfield))
		{
			sprintf(errmsg,"配置文件[%s]格式非法，节点[/xdp/%s]配置项可能和其他的重复",xmlfilepathname,\
				sfield.f_name);
			return false;
		}
		fieldnode = (CXmlNode *)fieldnode->getNextSibling();
		index++;
	}
	//计算偏移位置
	m_nFieldNum = m_index.Size();
	unsigned short offset = 0;
	for (unsigned short k=0 ; k<m_nFieldNum ; k++)
	{
		m_table.m_table[k].f_offset = offset;
		//移到下一个字段开始处
		offset += m_table.m_table[k].f_extlength;
		offset += m_table.m_table[k].f_length;
	}
	return true;
}

unsigned short CXdpFmt::GetType(const char *type)
{
	char tmpchar[40];
	bzero(tmpchar,sizeof(tmpchar));
	CBF_Tools::StringCopy(tmpchar,39,type);
	CBF_Tools::ToUpper(tmpchar);
	if (strcmp(tmpchar,"USHORT") == 0)
	{
		return XDP_USHORT;
	}
	else if (strcmp(tmpchar,"SHORT") == 0)
	{
		return XDP_SHORT;
	}
	else if (strcmp(tmpchar,"UINT") == 0)
	{
		return XDP_UINT;
	}
	else if (strcmp(tmpchar,"INT") == 0)
	{
		return XDP_INT;
	}
    else if (strcmp(tmpchar, "UINT64") == 0)
    {
        return XDP_UINT64;
    }
    else if (strcmp(tmpchar, "INT64") == 0)
    {
        return XDP_INT64;
    }
	else if (strcmp(tmpchar,"DOUBLE") == 0)
	{
		return XDP_DOUBLE;
	}
	else if (strcmp(tmpchar,"CHAR") == 0)
	{
		return XDP_CHAR;
	}
	else if (strcmp(tmpchar,"VARCHAR") == 0)
	{
		return XDP_VARCHAR;
	}
	else if (strcmp(tmpchar,"BINDATA") == 0)
	{
		return XDP_BINDATA;
	}
	else 
	{
		return XDP_UNKNOW;
	}

}

bool CXdpFmt::AddFmt(S_XDP_FIELD_DEFINE field)
{
	//根据索引去找，是否重复
	if (m_index.Find(field.f_index))
	{
		return false;
	}
	if (m_indexname.Find(field.f_name ))
	{
		return false;
	}
	int rowid = m_table.Add(field);
	m_index.Add(rowid, m_table.m_table[rowid].f_index);
	m_indexname.Add(rowid,m_table.m_table[rowid].f_name);
	m_nMaxLength+=field.f_extlength;
	m_nMaxLength+=field.f_length;
	return true;
}

unsigned int CXdpFmt::GetMaxLength()
{
	return m_nMaxLength;
}
S_XDP_FIELD_DEFINE *CXdpFmt::At(int index)
{
	return m_table.At(index);
}
S_XDP_FIELD_DEFINE *CXdpFmt::GetField(unsigned short index)
{
	return m_table.At(index);
}
// bool CXdpFmt::GetField(unsigned short index, S_XDP_FIELD_DEFINE &field)
// {
// 	CInt  iSet;
// 	if (!m_index.Select(iSet,index))
// 	{
// 		return false;
// 	}
// 	int id;
// 	if (!iSet.First(id))
// 	{
// 		return false;
// 	}
// 	field = m_table.m_table[id];
// 	return true;
// }

S_XDP_FIELD_DEFINE *CXdpFmt::GetField(const char *fieldname)
{
	int id;
	if (!m_indexname.Select(id,(char *)fieldname))
	{
		return NULL;
	}
	return  &(m_table.m_table[id]);
}
// bool CXdpFmt::GetField(const char *fieldname, S_XDP_FIELD_DEFINE &field)
// {
// 	CInt  iSet;
// 	if (!m_indexname.Select(iSet,(char *)fieldname))
// 	{
// 		return false;
// 	}
// 	int id;
// 	if (!iSet.First(id))
// 	{
// 		return false;
// 	}
// 	field = m_table.m_table[id];
// 	return true;
// }

int CXdpFmt::GetFieldNum()
{
	return m_nFieldNum;
}

unsigned  char CXdpFmt::GetVersion()
{
	return m_cVersion;
}

void CXdpFmt::ResetFmt()
{
	m_index.Clear();
	m_indexname.Clear();
	m_table.Clear();
	m_nFieldNum = 0;
	m_nMaxLength = 0;
}

bool CXdpFmt::XdpFmtCopy(CXdpFmt *src)
{
	int fieldnum = src->GetFieldNum();
	S_XDP_FIELD_DEFINE  *field;
	//重置FMT
	ResetFmt();
	m_cVersion = src->GetVersion();
	for (int i=0; i< fieldnum ; i++)
	{
		field = src->GetField(i);
		if (field == NULL)
		{
			return false;
		}
		if (!AddFmt(*field))
		{
			return false;
		}
	}
	m_nFieldNum = m_index.Size();
	return true;
}
