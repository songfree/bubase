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
	//ͨ���������ң�������������
	if (!m_pkey.Select(iset,quo.nVerietyId))
	{
		id = m_table.Add(quo);//���ӵ���
		m_pkey.Add(id,quo.nVerietyId);//��������
		return m_table.Size();  //����
	}
	iset.First(id);
	if (m_table.m_table[id].nDate < quo.nDate) //�������ڱ仯
	{
		//�������ڱ仯
		m_table.m_table[id].nDate = quo.nDate;
		m_table.m_table[id].nSerial = quo.nSerial;
		return m_table.Size();//����
	}
	else  if (m_table.m_table[id].nDate == quo.nDate) //����������ͬ
	{
		if (m_table.m_table[id].nSerial < quo.nSerial)
		{
			m_table.m_table[id].nSerial = quo.nSerial; //������ű仯
			return m_table.Size();
		}
	}
	return -1;
}