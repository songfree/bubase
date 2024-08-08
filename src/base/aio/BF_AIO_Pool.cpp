// BF_AIO_Pool.cpp: implementation of the CBF_AIO_Pool class.
//
//////////////////////////////////////////////////////////////////////

#include "BF_AIO_Pool.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBF_AIO_Pool::CBF_AIO_Pool()
{
	m_nMaxNum = 5000;//Ĭ�����1000
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
			fprintf(stderr,"�ڴ治�� new SOCKET_POOL_DATA ʧ��\n");
			return false;
		}
		pp->s_nIndex = m_nCurSocketIndex;
		m_nCurSocketIndex++;
		ResetData(pp);
		//����֮����
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
		{ //���������������
			return NULL;
		}
		m_pSocketData = new SOCKET_POOL_DATA();
		if (m_pSocketData == NULL)
		{
			fprintf(stderr,"�ڴ治�� new SOCKET_POOL_DATA ʧ��\n");
			return NULL;
		}
		m_pSocketData->s_nIndex = m_nCurSocketIndex;
		m_nCurSocketIndex++;
		ResetData(m_pSocketData);
		//��������
		m_vAllSocketInfo.push_back(m_pSocketData);
	}
	return m_pSocketData;
}

void CBF_AIO_Pool::ResetData(PSOCKET_POOL_DATA pp)
{
	bzero(&(pp->s_iocp),sizeof(pp->s_iocp)) ;                //IOCP�ص��ṹ
	pp->s_hSocket = INVALID_SOCKET      ;                    //SOCKET����
	bzero(&(pp->s_pSocketAddr),sizeof(SOCKADDR_IN)) ;        //�Է��ĵ�ַ�����ӹ����ĵ�ַ
	pp->s_nTimestamp = 0 ;                  //����ʱ���(��) Ӧ�÷�������ʱҪ�ʹ˽��бȽϣ�����������������Ӳ���ԭ����
	pp->s_nReadTime  = 0 ;                  //����������ʱ��(��)  AIO����
	pp->s_nWriteTime = 0 ;                  //���������ʱ��(��)  AIO����
	pp->s_pSendData = NULL;                 //IOCP�õķ���ָ��
//	pp->s_pSendQueue.clear();               //���Ͷ��� epoll��selectʹ�ã��ȷ��ͣ����ɹ���������
    pp->s_pIoBuffer = pp->s_sRealBuffer;          //IO��������ָ��  ָ��IO������ŵص�ָ��  s_pIoBuffer=s_sRealBuffer+s_nRealBufferLen
	pp->s_nIoBufferLen   = 0 ;              //iocp�����յ������ݳ���
	pp->s_nRealBufferLen = 0 ;              //�յ�����ʵ�ʵ����ݻ����е����ݳ���


//20151119�޸ĵ��ر�ʱIOCP��Ϊop_accept״̬	
	pp->s_cOperType = OP_ACCEPT ;             // IOCP��������

	

// #define     OP_NONE       0   //ʲô��������        ����δʹ��ʱ������
// #define     OP_READ       1   //��                  ��������ʱ�Ĳ���
// #define     OP_WRITE      2   //д                  
// #define     OP_ACCEPT     3   //��������            ���յ������Ӿ��Ǵ�������
// #define     OP_CONNECT    4   //�������ӱ�ķ���    ���������������ӳ�ȥ��
// #define     OP_LISTEN     5   //��������            ��������������
	pp->s_cSocketType = SOCK_UNKNOW;        //SOCKET��������   ��Ӧ�þ��嶨�� 
// #define     SOCK_UNKNOW     0                //δ֪  accept���Ǵ����ͣ���Ӧ����s_nSocketStatusΪ�����������Ȼ�����Ǻ������Ͳ����ô�����
// #define     SOCK_CLIENT     1                //��ͨ�ͻ���
// #define     SOCK_SERVER     2                //ע��ķ����
// #define     SOCK_TONODE     3                //���ӵ���Ľڵ�����
// #define     SOCK_FROMNODE   4                //�ӱ�Ľڵ��������ģ���Ӧ��ΪSOCK_TONODE
// #define     SOCK_TOVC       5                //����ͨ�� �������ӳ�ȥ��VC
// #define     SOCK_FROMVC     6                //����ͨ�� ��Ľڵ���������	
	pp->s_cSocketStatus = SOCKSTATUS_UNKNOW;      //����״̬ AIO EPOLL��selectģ�͸��ݴ�״̬���м�� IOCP����  
// #define     SOCKSTATUS_RW         0                //0�ɶ���д
// #define     SOCKSTATUS_R          1                //1��Ҫ����
// #define     SOCKSTATUS_W          2                //2��Ҫ���д
// #define     SOCKSTATUS_C          3                //3��Ҫ��д�����Ƿ�����
// #define     SOCKSTATUS_UNKNOW     4                //4ʲô��������
	pp->s_cHBFlag = HEARTBEAT_RECV;            //������־ 0������   1��Խ��շ�����  2��Է��ͷ�����
// #define     HEARTBEAT_NONE        0                //0������
// #define     HEARTBEAT_RECV        1                //1��Խ��շ�����
// #define     HEARTBEAT_SEND        2                //2��Է��ͷ�����
	pp->s_nHBTime  = 10 ;                //����ʱ��(��)  ��s_cHBFlagΪ1ʱ�������ڴ�ʱ��û�н������ݾͷ����� ��s_cHBFlagΪ2ʱ�������ڴ�ʱ��û�з������ݾͷ�����  
	pp->s_nHBCloseTime  = 20 ;           // (��)�������ӷ����������ڴ�ʱ����û���յ�������ӵ����ݾ͹ر�
	pp->s_nCloseTime   = 5 ;             //(��)��������accept�������ڴ�ʱ����û���յ�������ӵ����ݾ͹ر�
	pp->s_nPingTime    = 0  ;            //(��)�����PING����ʱ��(��) ���Ʋ�Ҫ�ظ�������
	
	pp->s_nVer = 1 ;                     //���Ӱ汾  ��Ӧ�þ��嶨��
	pp->s_nDestIndex = 0;                //Ŀ������  ��s_nSocketTypeΪSOCK_TOVC��SOCK_FROMVCʱ���൱������������ֱͨ�ģ�����Ҫ��ֱͨ����������
	pp->ptr =  NULL ;                    //�Զ���Ľṹָ�룬������չ
	pp->s_cCheckFlag = 0;
	bzero(pp->s_sSessionKey,sizeof(pp->s_sSessionKey));
	//��շ��Ͷ���
	pp->s_pSendQueue.clear();
	pp->s_pQuoteSendQueue.clear();
	pp->s_nSendQueueFlag = 0; //�޲������ݷ���
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
