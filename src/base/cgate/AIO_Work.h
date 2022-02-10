// AIO_Work.h: interface for the CAIO_Work class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AIO_WORK_H__C839CA1C_4FBD_4A04_B366_345D413AF7EF__INCLUDED_)
#define AFX_AIO_WORK_H__C839CA1C_4FBD_4A04_B366_345D413AF7EF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BF_AIO_WorkBase.h"
#include "PoolData.h"
#include "BF_BufferPool.h"
#include "BF_Des.h"
#include "DrebEndian.h"
#include "BF_Tools.h"
#include "IErrLog.h"
#include "CgateLog.h"
#include "SendData.h"
//ע�⣬����ķ��������ɶ���̵߳��ã�����Ҫע�����ݷ�������

#define  CHECKFLAG_UNKNOW   0   //����δ֪
#define  CHECKFLAG_STEP1    1    //�ӽӿͻ��˷���ǩ������Կ
#define  CHECKFLAG_STEP2    2    //���տͻ�����֤����¼
#define  CHECKFLAG_NORMAL   5    //�������ӣ���¼����Ϊ��������

#define  FLAG_FIXEDKEY            94  //�̶�key����
#define  FLAG_SESSIONKEY          84   //�Ựkey����
#define  FLAG_CERTKEY             95  //֤��key����

#define  FLAG_ZIPFIXEDKEY         14  //ѹ�����̶�key����
#define  FLAG_ZIPSESSIONKEY       4   //ѹ�����Ựkey����
#define  FLAG_ZIPCERTKEY          15  //ѹ����֤��key����
#define  FLAG_ZIP                 2   //ѹ��


class CAIO_Work : public CBF_AIO_WorkBase  
{
public:

	// ������: GetSocketByIndex
	// ���  : ������ 2013-11-11 9:28:58
	// ����  : PSOCKET_POOL_DATA 
	// ����  : int index
	// ����  : ȡ��������Ϣ
	PSOCKET_POOL_DATA GetSocketByIndex(int index);

	// ������: SendBCData
	// ���  : ������ 2013-7-13 9:15:13
	// ����  : void 
	// ����  : CGATE_COMMSTRU &data
	// ����  : ���͹㲥����
	void SendBCData(CGATE_COMMSTRU &data);
	

	// ������: GenSignString
	// ���  : ������ 2013-6-9 14:45:26
	// ����  : std::string 
	// ����  : �������16λ�����ַ���
	std::string GenSignString();

	CAIO_Work();
	virtual ~CAIO_Work();

	// ������: Connect
	// ���  : ������ 2012-5-21 11:55:51
	// ����  : void 
	// ����  : PSOCKET_POOL_DATA *info
	// ����  : AIO֪ͨ�����ӶϿ���Ҫ������
	virtual void OnConnect(PSOCKET_POOL_DATA info);


	// ������: OnSendBack
	// ���  : ������ 2012-5-17 16:38:08
	// ����  : virtual void 
	// ����  : PSOCKET_SEND_DATA data   
	// ����  : AIO֪ͨ��д�����ݷ��أ��Է��ؽ�����д���
	virtual void OnSendBack(SOCKSENDQUEUE::iterator data,bool issuccess);
	
	virtual void OnSendBack(PSOCKET_SEND_DATA data,bool issuccess);
	
	// ������: OnPing
	// ���  : ������ 2012-5-21 11:54:23
	// ����  : virtual void 
	// ����  : AIO֪ͨ��Ҫ����PING��
	virtual void OnPing(PSOCKET_POOL_DATA info);

	// ������: OnRecvBack
	// ���  : ������ 2012-5-17 16:36:58
	// ����  : virtual void 
	// ����  : PSOCKET_POOL_DATA info
	// ����  : AIO֪ͨ�������յ����ݣ������ݽ��д���
	virtual void OnRecvBack(PSOCKET_POOL_DATA info);


	// ������: OnConnectBack
	// ���  : ������ 2012-5-17 16:35:34
	// ����  : virtual void 
	// ����  : PSOCKET_POOL_DATA info
	// ����  : int status ����״̬  0�ɹ� ����ʧ��
	// ����  : AIO֪ͨ�������������ӳɹ����Դ����ӽ��к�������
	virtual void OnConnectBack(PSOCKET_POOL_DATA info,int status);


	// ������: OnClose
	// ���  : ������ 2012-5-17 16:34:32
	// ����  : virtual void 
	// ����  : PSOCKET_POOL_DATA info ������Ϣ
	// ����  : std::string msg
	// ����  : AIO֪ͨ������һ�����ӹرգ�AIO��û�жԴ����ӽ��йرմ���Ҫ��������Դ����ӽ��к�������Ȼ�����Close����֪ͨAIO�رյ�
	virtual void OnClose(PSOCKET_POOL_DATA info,std::string msg);


	// ������: OnAccept
	// ���  : ������ 2012-5-17 16:33:33
	// ����  : virtual void 
	// ����  : PSOCKET_POOL_DATA info
	// ����  : AIO֪ͨ���յ�һ�����ӣ��Դ������ӽ��д���,AIO��ܲ��������ӳأ���Ӧ�ü��ͨ��������ʾ����AddAio
	virtual void OnAccept(PSOCKET_POOL_DATA info);


	void SetGlobalVar(CGateResource *res,CPoolData *pool,CBF_BufferPool *mempool,CSendData *senddata);


	// ������: SendData
	// ���  : ������ 2013-6-9 10:28:31
	// ����  : int 
	// ����  : PCGATE_COMMSTRU data
	// ����  : int index Ҫ���͵���������
	// ����  : int timestamp=0  ��������0��ΪӦ��ʱԭ���ӵ�ʱ���������ʱ�ж�ʱ����Ƿ���ͬ������ͬ�����͡�
	// ����  : ��������
	int SendData(PCGATE_COMMSTRU data,int index,bool isimmediate,int timestamp=0);


	// ������: SendData
	// ���  : ������ 2014-4-15 14:11:47
	// ����  : int 
	// ����  : PCGATE_COMMSTRU data
	// ����  : int index
	// ����  : ֱ�ӷ��ͣ����÷����������һ�̷߳���
	int SendData(PCGATE_COMMSTRU data,int index);

	// ������: ZipBuf
	// ���  : ������ 2013-5-27 21:45:15
	// ����  : bool 
	// ����  : PCGATE_COMMSTRU  data
	// ����  : int nZipFlag=4
	// ����  : ����ѹ������
	bool ZipBuf(PSOCKET_POOL_DATA info,PCGATE_COMMSTRU  data,int nZipFlag=4);

private:
	CPoolData   *m_pDataPool;     //�ͻ��������  �ӿͻ������յ����������ݣ�����˶��У�����Ϣ�����߳�������
	CGateResource  *m_pRes;       //
	CBF_BufferPool *m_pMemPool;
	CSendData    *m_pSendData;

	CGATE_COMMSTRU  m_pRcvData;
	CDrebEndian    m_pEndian;
	CBF_Des        m_pDes; //����IO�߳̽��ղ�����ѹ�����ܵ�
	CBF_Des        m_pDesSend;
	CIErrlog      *m_pLog;

	CCgateLog       *m_pGateLog;

	CBF_Mutex     m_pDesMutex;

	CBF_Mutex     m_pSendMutex;
	char          m_sSrcBuffer[CLIENTDATASIZE+2];//��Կͻ������ݵĽ��ܽ�ѹ����Դ
	char          m_sDestBuffer[CLIENTDATASIZE+2];//��Կͻ������ݵĽ��ܽ�ѹ����Ŀ��

	// ������: ProcessRcvMsg
	// ���  : ������ 2013-6-9 19:41:58
	// ����  : void 
	// ����  : PSOCKET_POOL_DATA info
	// ����  : CGATE_COMMSTRU &data
	// ����  : ������յ�����Ϣ
	void ProcessRcvMsg(PSOCKET_POOL_DATA info,CGATE_COMMSTRU &data);

	// ������: Endian2CommCrc
	// ���  : ������ 2013-5-28 10:41:08
	// ����  : void 
	// ����  : CGATE_HEAD &head
	// ����  : �ֽ���ת��ͨѶ��ʽ������crcУ��λ
	void Endian2CommCrc(CGATE_HEAD &head);
	
	
	// ������: Endian2HostCrc
	// ���  : ������ 2013-5-28 10:41:02
	// ����  : bool 
	// ����  : CGATE_HEAD &head
	// ����  : ת��Ϊ�����ֽ���У��crc
	bool Endian2HostCrc(CGATE_HEAD &head);
	
	
	
	// ������: UnzipBuf
	// ���  : ������ 2013-5-27 21:44:59
	// ����  : bool 
	// ����  : CGATE_COMMSTRU &data
	// ����  : ��ѹ����
	bool UnzipBuf(PSOCKET_POOL_DATA info,CGATE_COMMSTRU &data);
	
	// ������: GetDrebCmdType
	// ���  : ������ 2013-5-28 10:40:57
	// ����  : std::string 
	// ����  : int cmdtype
	// ����  : 
	std::string GetDrebCmdType(int cmdtype);

	// ������: GetMsgType
	// ���  : ������ 2013-5-28 10:40:52
	// ����  : std::string 
	// ����  : int type
	// ����  : ȡ������Ϣ����
	std::string GetMsgType(int type);


};

#endif // !defined(AFX_AIO_WORK_H__C839CA1C_4FBD_4A04_B366_345D413AF7EF__INCLUDED_)
