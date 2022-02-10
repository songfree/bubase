// ExchDll.cpp: implementation of the CExchDll class.
//
//////////////////////////////////////////////////////////////////////

#include "ExchDll.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//注意：下面的定义在每个类里面都要有，主程序根据此方法获取本类的实例
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
// 函数名: Send2Exch
// 编程  : 王明松 2017-7-20 9:33:01
// 返回  : virtual int 
// 参数  : S_TRADE_DATA &data  
// 参数  : CXdp *xdp   xdp报文
// 描述  : 发给交易所 
int CExchDll::Send2Exch(S_TRADE_DATA &data,CXdp *xdp)
{
	//发给总线
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


// 函数名: Init
// 编程  : 王明松 2017-8-10 15:41:08
// 返回  : virtual bool 
// 参数  : CBF_Xml *xml
// 描述  : 初始化
bool CExchDll::Init(CBF_Xml *xml,CIClientLink *pBusLink,CMemDb *pMemDb,	CXdp *pXdp,	CIErrlog *pLog,CBF_BufferPool *pMemPool)
{
	m_pBusLink = pBusLink;
	m_pXdp.XdpCopy(pXdp);
	m_pMemDb= pMemDb;
	m_pLog = pLog;
	m_pMemPool = pMemPool;
	return true;
}

// 函数名: Start
// 编程  : 王明松 2017-8-10 15:39:45
// 返回  : virtual bool 
// 描述  : 启动交易所或柜台连接
bool CExchDll::Start()
{
	return true;
}


// 函数名: Stop
// 编程  : 王明松 2017-8-10 15:40:09
// 返回  : virtual bool 
// 描述  : 停止
bool CExchDll::Stop()
{
	return true;
}


// 函数名: Join
// 编程  : 王明松 2017-8-10 15:40:21
// 返回  : virtual bool 
// 描述  : 等待线程停止
bool CExchDll::Join()
{
	return true;
}
