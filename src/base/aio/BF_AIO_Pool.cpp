// BF_AIO_Pool.cpp: implementation of the CBF_AIO_Pool class.
//
//////////////////////////////////////////////////////////////////////

#include "BF_AIO_Pool.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBF_AIO_Pool::CBF_AIO_Pool()
{
	m_nMaxNum = 5000;//默认最大1000
	m_nCurSocketIndex = 0;
	m_pSocketData = NULL;
}

CBF_AIO_Pool::~CBF_AIO_Pool()
{
	for (int i=0; i<m_vAllSocketInfo.size() ;i++)
	{
		if (m_vAllSocketInfo[i]->s_hSocket != INVALID_SOCKET)
		{
			closesocket(m_vAllSocketInfo[i]->s_hSocket);
			m_vAllSocketInfo[i]->s_hSocket = INVALID_SOCKET;
		}
		delete m_vAllSocketInfo[i];
		m_vAllSocketInfo[i] = NULL;
	}
}


void CBF_AIO_Pool::SetMaxNum(unsigned int max)
{
	m_nMaxNum = max;
}

bool CBF_AIO_Pool::Init(unsigned int num)
{
	int nNum;
	if (num>m_nMaxNum)
	{
		nNum = m_nMaxNum;
	}
	else
	{
		nNum = num;
	}
	for (int i=0 ; i<nNum ; i++)
	{
		PSOCKET_POOL_DATA pp = new SOCKET_POOL_DATA();
		if (pp == NULL)
		{
			fprintf(stderr,"内存不足 new SOCKET_POOL_DATA 失败\n");
			return false;
		}
		pp->s_nIndex = m_nCurSocketIndex;
		m_nCurSocketIndex++;
		ResetData(pp);
		//放入之容器
		m_vAllSocketInfo.push_back(pp);
		m_pFreeSocketInfo.push_back(pp);
	}
	return true;
}

PSOCKET_POOL_DATA CBF_AIO_Pool::PoolMalloc()
{
	CBF_PMutex mux(&m_mutex);
	if (m_pFreeSocketInfo.size() > 0)
	{
		m_pSocketData = m_pFreeSocketInfo.front();
		m_pFreeSocketInfo.pop_front();
//		ResetData(pp);
	}
	else
	{
		if (m_nCurSocketIndex > m_nMaxNum) 
		{ //超过最大连接数了
			return NULL;
		}
		m_pSocketData = new SOCKET_POOL_DATA();
		if (m_pSocketData == NULL)
		{
			fprintf(stderr,"内存不足 new SOCKET_POOL_DATA 失败\n");
			return NULL;
		}
		m_pSocketData->s_nIndex = m_nCurSocketIndex;
		m_nCurSocketIndex++;
		ResetData(m_pSocketData);
		//放入容器
		m_vAllSocketInfo.push_back(m_pSocketData);
	}
	return m_pSocketData;
}

void CBF_AIO_Pool::ResetData(PSOCKET_POOL_DATA pp)
{
	bzero(&(pp->s_iocp),sizeof(pp->s_iocp)) ;                //IOCP重叠结构
	pp->s_hSocket = INVALID_SOCKET      ;                    //SOCKET连接
	bzero(&(pp->s_pSocketAddr),sizeof(SOCKADDR_IN)) ;        //对方的地址或连接过来的地址
	pp->s_nTimestamp = 0 ;                  //连接时间戳(秒) 应用发送数据时要和此进行比较，如果不相等则表明连接不是原连接
	pp->s_nReadTime  = 0 ;                  //最后读到数据时间(秒)  AIO设置
	pp->s_nWriteTime = 0 ;                  //最后发送数据时间(秒)  AIO设置
	pp->s_pSendData = NULL;                 //IOCP用的发送指针
//	pp->s_pSendQueue.clear();               //发送队列 epoll和select使用，先发送，不成功则放入队列
    pp->s_pIoBuffer = pp->s_sRealBuffer;          //IO操作缓冲指针  指明IO操作存放地的指针  s_pIoBuffer=s_sRealBuffer+s_nRealBufferLen
	pp->s_nIoBufferLen   = 0 ;              //iocp操作收到的数据长度
	pp->s_nRealBufferLen = 0 ;              //收到放在实际的数据缓冲中的数据长度


//20151119修改当关闭时IOCP置为op_accept状态	
	pp->s_cOperType = OP_ACCEPT ;             // IOCP操作类型

	

// #define     OP_NONE       0   //什么都不用做        连接未使用时的类型
// #define     OP_READ       1   //读                  连接正常时的操作
// #define     OP_WRITE      2   //写                  
// #define     OP_ACCEPT     3   //接收连接            刚收到的连接就是此种类型
// #define     OP_CONNECT    4   //主动连接别的服务    此连接是主动连接出去的
// #define     OP_LISTEN     5   //侦听连接            此连接是侦听的
	pp->s_cSocketType = SOCK_UNKNOW;        //SOCKET连接类型   由应用具体定义 
// #define     SOCK_UNKNOW     0                //未知  accept后是此类型，由应用置s_nSocketStatus为读后接收数据然后检查是何种类型并重置此类型
// #define     SOCK_CLIENT     1                //普通客户端
// #define     SOCK_SERVER     2                //注册的服务端
// #define     SOCK_TONODE     3                //连接到别的节点连接
// #define     SOCK_FROMNODE   4                //从别的节点连过来的，对应的为SOCK_TONODE
// #define     SOCK_TOVC       5                //虚拟通道 主动连接出去的VC
// #define     SOCK_FROMVC     6                //虚拟通道 别的节点连过来的	
	pp->s_cSocketStatus = SOCKSTATUS_UNKNOW;      //连接状态 AIO EPOLL及select模型根据此状态进行检查 IOCP不用  
// #define     SOCKSTATUS_RW         0                //0可读可写
// #define     SOCKSTATUS_R          1                //1需要检查读
// #define     SOCKSTATUS_W          2                //2需要检查写
// #define     SOCKSTATUS_C          3                //3需要检写连接是否正常
// #define     SOCKSTATUS_UNKNOW     4                //4什么都不用做
	pp->s_cHBFlag = HEARTBEAT_RECV;            //心跳标志 0无心跳   1针对接收发心跳  2针对发送发心跳
// #define     HEARTBEAT_NONE        0                //0无心跳
// #define     HEARTBEAT_RECV        1                //1针对接收发心跳
// #define     HEARTBEAT_SEND        2                //2针对发送发心跳
	pp->s_nHBTime  = 10 ;                //心跳时间(秒)  当s_cHBFlag为1时此连接在此时间没有接收数据就发心跳 当s_cHBFlag为2时此连接在此时间没有发送数据就发心跳  
	pp->s_nHBCloseTime  = 20 ;           // (秒)当此连接发送心跳后，在此时间内没有收到这个连接的数据就关闭
	pp->s_nCloseTime   = 5 ;             //(秒)当此连接accept上来后，在此时间内没有收到这个连接的数据就关闭
	pp->s_nPingTime    = 0  ;            //(秒)最后发送PING数据时间(秒) 控制不要重复发心跳
	
	pp->s_nVer = 1 ;                     //连接版本  由应用具体定义
	pp->s_nDestIndex = 0;                //目的索引  当s_nSocketType为SOCK_TOVC或SOCK_FROMVC时，相当于两个连接是直通的，这里要填直通的连接索引
	pp->ptr =  NULL ;                    //自定义的结构指针，方便扩展
	pp->s_cCheckFlag = 0;
	bzero(pp->s_sSessionKey,sizeof(pp->s_sSessionKey));
	//清空发送队列
	pp->s_pSendQueue.clear();
	pp->s_pQuoteSendQueue.clear();
	pp->s_nSendQueueFlag = 0; //无部分数据发送
}

void CBF_AIO_Pool::CloseFree(PSOCKET_POOL_DATA pp)
{
	if (pp->s_hSocket != INVALID_SOCKET)
	{
		closesocket(pp->s_hSocket);
		pp->s_hSocket = INVALID_SOCKET;
	}
	ResetData(pp);
	CBF_PMutex mux(&m_mutex);
	m_pFreeSocketInfo.push_back(pp);
}
void CBF_AIO_Pool::PoolInfo(int &total,int &unused)
{
	CBF_PMutex mux(&m_mutex);
	total=m_vAllSocketInfo.size();
	unused=m_pFreeSocketInfo.size();
}
int CBF_AIO_Pool::GetConnInfo(std::vector<PSOCKET_POOL_DATA>& connlist)
{
	CBF_PMutex mux(&m_mutex);
	for (unsigned int i = 0; i < m_vAllSocketInfo.size(); i++)
	{
		if (m_vAllSocketInfo[i]->s_hSocket != INVALID_SOCKET)
		{
			connlist.push_back(m_vAllSocketInfo[i]);
		}
	}
	return connlist.size();
}


PSOCKET_POOL_DATA CBF_AIO_Pool::GetDataByIndex(unsigned int index)
{
	if (index >= m_nCurSocketIndex)
	{
		return NULL;
	}
	return m_vAllSocketInfo[index];
}
