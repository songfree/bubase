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
	//ͨ���������ң�������������
	if (!m_pkey.Select(id, funcno, nkey))
	{
		S_CGATE_QUOTATION quo;
		quo.nDate = ndate;
		quo.nSerial = ntime;
		quo.nVerietyId = nkey;
		quo.nServiceNo = funcno;
		id = m_table.Add(quo);//���ӵ���
		m_pkey.Add(id, m_table.m_table[id].nServiceNo,m_table.m_table[id].nVerietyId);//��������
		return m_table.Size();  //����
	}
	if (m_table.m_table[id].nDate < ndate) //�������ڱ仯
	{
		//�������ڱ仯
		m_table.m_table[id].nDate = ndate;
		m_table.m_table[id].nSerial = ntime;
		return m_table.Size();//����
	}
	else  if (m_table.m_table[id].nDate == ndate) //����������ͬ
	{
		if (m_table.m_table[id].nSerial <= ntime)
		{
			m_table.m_table[id].nSerial = ntime; //������ű仯
			return m_table.Size();
		}
	}
	return -1;
}