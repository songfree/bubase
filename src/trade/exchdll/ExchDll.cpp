// ExchDll.cpp: implementation of the CExchDll class.
//
//////////////////////////////////////////////////////////////////////

#include "ExchDll.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//ע�⣺����Ķ�����ÿ�������涼Ҫ�У���������ݴ˷�����ȡ�����ʵ��
CExchDll * getInstance() 
{ 
    return new CExchDll(); 
}
CExchDll::CExchDll()
{
	m_pLog = NULL;
	m_pMemPool = NULL;
	bzero(m_errMsg,sizeof(m_errMsg));
}

CExchDll::~CExchDll()
{

}
// ������: Send2Exch
// ���  : ������ 2017-7-20 9:33:01
// ����  : virtual int 
// ����  : S_TRADE_DATA &data  
// ����  : CXdp *xdp   xdp����
// ����  : ���������� 
int CExchDll::Send2Exch(S_TRADE_DATA &data,CXdp *xdp)
{
	//��������
	data.src = 0;
	unsigned int len ;
	if (!xdp->ToBuffer(data.pData->sBuffer,len,m_errMsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"Send2Exch XDP ToBuffer ERROR txcode[%d]",data.pData->sDBHead.d_Dinfo.d_nServiceNo);
		m_pMemPool->PoolFree(data.pData);
		data.pData = NULL;
		return -1;
	}
	return m_pBusLink->Send2Client(data);
	
}


// ������: Init
// ���  : ������ 2017-8-10 15:41:08
// ����  : virtual bool 
// ����  : CBF_Xml *xml
// ����  : ��ʼ��
bool CExchDll::Init(CBF_Xml *xml,CIClientLink *pBusLink,CMemDb *pMemDb,	CXdp *pXdp,	CIErrlog *pLog,CBF_BufferPool *pMemPool)
{
	m_pBusLink = pBusLink;
	m_pXdp.XdpCopy(pXdp);
	m_pMemDb= pMemDb;
	m_pLog = pLog;
	m_pMemPool = pMemPool;
	return true;
}

// ������: Start
// ���  : ������ 2017-8-10 15:39:45
// ����  : virtual bool 
// ����  : �������������̨����
bool CExchDll::Start()
{
	return true;
}


// ������: Stop
// ���  : ������ 2017-8-10 15:40:09
// ����  : virtual bool 
// ����  : ֹͣ
bool CExchDll::Stop()
{
	return true;
}


// ������: Join
// ���  : ������ 2017-8-10 15:40:21
// ����  : virtual bool 
// ����  : �ȴ��߳�ֹͣ
bool CExchDll::Join()
{
	return true;
}
