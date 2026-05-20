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

int CMdbQuotation::IsNewQuotation(int funcno,int ndate, int nkey, unsigned int ntime)
{
	int id;
	//通过主键查找，不存在则增加
	if (!m_pkey.Select(id, funcno, nkey))
	{
		S_CGATE_QUOTATION quo;
		quo.nDate = ndate;
		quo.nSerial = ntime;
		quo.nVerietyId = nkey;
		quo.nServiceNo = funcno;
		quo.nLastUpdateTime	=time(NULL);
		id = m_table.Add(quo);//增加到表
		m_pkey.Add(id, m_table.m_table[id].nServiceNo,m_table.m_table[id].nVerietyId);//增加主键
		return m_table.Size();  //更新
	}
	if (m_table.m_table[id].nDate < ndate) //行情日期变化
	{
		//行情日期变化
		m_table.m_table[id].nDate = ndate;
		m_table.m_table[id].nSerial = ntime;
		m_table.m_table[id].nLastUpdateTime = time(NULL);
		return m_table.Size();//更新
	}
	else  if (m_table.m_table[id].nDate == ndate) //行情日期相同
	{
		if (m_table.m_table[id].nSerial <= ntime)
		{
			m_table.m_table[id].nSerial = ntime; //行情序号变化
			m_table.m_table[id].nLastUpdateTime = time(NULL);
			return m_table.Size();
		}
	}
	if (time(NULL) - m_table.m_table[id].nLastUpdateTime > 3) //大于3秒没有变化，说明有问题了，用最新的更新
	{
        //行情日期变化
        m_table.m_table[id].nDate = ndate;
        m_table.m_table[id].nSerial = ntime;
        m_table.m_table[id].nLastUpdateTime = time(NULL);
        return m_table.Size();//更新
	}
	return -1;
}