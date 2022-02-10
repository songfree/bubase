// MdbQuotation.cpp: implementation of the CMdbQuotation class.
//
//////////////////////////////////////////////////////////////////////

#include "MdbQuotation.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMdbQuotation::CMdbQuotation()
{

}

CMdbQuotation::~CMdbQuotation()
{

}
int CMdbQuotation::IsNewQuotation(S_CGATE_QUOTATION &quo)
{
	CInt iset;
	int id;
	//通过主键查找，不存在则增加
	if (!m_pkey.Select(iset,quo.nVerietyId))
	{
		id = m_table.Add(quo);//增加到表
		m_pkey.Add(id,quo.nVerietyId);//增加主键
		return m_table.Size();  //更新
	}
	iset.First(id);
	if (m_table.m_table[id].nDate < quo.nDate) //行情日期变化
	{
		//行情日期变化
		m_table.m_table[id].nDate = quo.nDate;
		m_table.m_table[id].nSerial = quo.nSerial;
		return m_table.Size();//更新
	}
	else  if (m_table.m_table[id].nDate == quo.nDate) //行情日期相同
	{
		if (m_table.m_table[id].nSerial < quo.nSerial)
		{
			m_table.m_table[id].nSerial = quo.nSerial; //行情序号变化
			return m_table.Size();
		}
	}
	return -1;
}