// BPCLink.h: interface for the CBPCLink class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BPCLINK_H__9E11A8A6_6781_4BCA_AEB6_0AF8CFE0BE4E__INCLUDED_)
#define AFX_BPCLINK_H__9E11A8A6_6781_4BCA_AEB6_0AF8CFE0BE4E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



#include "IPcLink.h"
#include "CallBu.h"
#include "BpcHead.h"
#include "BpcEndian.h"
#include "BF_SocketTcp.h"
#include "BF_Date_Time.h"
#include "BF_Des.h"
#include "BF_Tools.h"
#include "DrebBpcLog.h"

#ifdef BUMODULE_EXPORTS
#define BUMODULE_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define BUMODULE_EXPORT __declspec(dllimport)
#else
#define BUMODULE_EXPORT 
#endif
#endif




#ifdef _WINDOWS
#ifdef _DEBUG
#pragma comment(lib, "bf_dbpubd.lib") 
#pragma message("Automatically linking with   bf_dbpubd.lib ")
#else
#pragma comment(lib, "bf_dbpub.lib") 
#pragma message("Automatically linking with   bf_dbpub.lib ")
#endif

#endif


class BUMODULE_EXPORT CBPCLink : public CIPcLink  
{
public:
	
	// ������: GetBpcMsgType
	// ���  : ������ 2013-4-27 14:51:12
	// ����  : std::string 
	// ����  : int msgtype
	// ����  : ȡ��BPC��Ϣ����
	std::string GetBpcMsgType(int msgtype);

	// ������: GetDrebCmdType
	// ���  : ������ 2013-4-27 14:51:38
	// ����  : std::string 
	// ����  : int cmdtype
	// ����  : ȡ��DREB������
	std::string GetDrebCmdType(int cmdtype);

	// ������: GetFpcPort
	// ���  : ������ 2010-7-6 17:18:44
	// ����  : int  bpc�˿�
	// ����  : ȡ��BPC�˿�
	int GetBpcPort();

	// ������: GetFpcIp
	// ���  : ������ 2010-7-6 17:18:30
	// ����  : std::string 
	// ����  : ȡ��BPC��ַ
	std::string GetBpcIp();

	// ������: Run
	// ���  : ������ 2010-7-6 17:17:52
	// ����  : int 0��ʾ������Ҫ��������  ������ʾ�����ˣ��˳�
	// ����  : ���У�������Ҫ��ͣ�ĵ��ô˷���
	int Run();

	CBPCLink();
	virtual ~CBPCLink();
	
	char g_sCurPath[300];
	char g_sModulePath[300];
	char g_sModuleName[300];

public:
	void Stop();

	// ������: LogFile
	// ���  : ������ 2013-4-28 10:05:14
	// ����  : int 
	// ����  : int level ��־����
	// ����  : bool flag  ��־��־   false��������Ӧ���   true�������ݰ�
	// ����  : PBPCCOMMSTRU data
	// ����  : �����ݰ�д����־�ļ�
	int LogFile(int level,bool flag,PBPCCOMMSTRU data);

	// ������: OnFree
	// ���  : ������ 2013-4-28 9:53:57
	// ����  : bool true�ɹ� falseʧ��
	// ����  : ֪ͨBPC���ǿ���
	bool OnFree();

	// ������: Init����ͨ����
	// ���  : ������ 2010-7-6 17:18:12
	// ����  : bool true�ɹ� falseʧ��
	// ����  : const char *filepathname
	// ����  : ��ʼ��BPc����
	virtual bool InitLink(const char *confile);

	/// ������: SetGPara1
	/// ���  : ������ 2008-12-30 9:42:48
	/// ����  : virtual bool true�ɹ� falseʧ��
	/// ����  : bool setflag=true
	/// ����  : �ӿ��麯��������Ҫʵ��.����ȫ�ֱ���1,���������򷵻�false
	virtual bool SetGPara1(bool setflag=true);
	
	
	/// ������: isSetGPara2
	/// ���  : ������ 2008-12-30 9:36:10
	/// ����  : virtual bool true�ɹ� falseʧ��
	/// ����  : bool setflag=true
	/// ����  : ����ȫ�ֱ���2,���������򷵻�false
	virtual bool SetGPara2(bool setflag=true);
	
	
	/// ������: isSetGPara3
	/// ���  : ������ 2008-12-30 9:36:13
	/// ����  : virtual bool true�ɹ� falseʧ��
	/// ����  : bool setflag=true
	/// ����  : ����ȫ�ֱ���3,���������򷵻�false
	virtual bool SetGPara3(bool setflag=true);
	
	/// ������: GetThreadShareSerial
	/// ���  : ������ 2008-12-30 9:36:13
	/// ����  : virtual long  ��ˮ��
	/// ����  : ȡ��Ψһ����ˮ��
	virtual long GetThreadShareSerial();


	// ������: AnsData
	// ���  : ������ 2013-4-23 15:16:43
	// ����  : virtual int -1ʧ�� 0�ɹ�
	// ����  : PBPCCOMMSTRU data
	// ����  : Ӧ������,��BPUҵ�������ԭ�ӵ���
	virtual int AnsData(PBPCCOMMSTRU data) ;
	
	// ������: DPTrans
	// ���  : ������ 2013-4-23 15:11:48
	// ����  : virtual int -1ʧ�� 0�ɹ�
	// ����  : PBPCCOMMSTRU calldata
	// ����  : ת��������������
	virtual int DPTrans(PBPCCOMMSTRU calldata) ;
	
	// ������: BpuCall
	// ���  : ������ 2013-4-23 15:09:13
	// ����  : virtual int -1ʧ�� 0�ɹ�
	// ����  : PBPCCOMMSTRU calldata
	// ����  : LIST_BPC_RSMSG &ansdata
	// ����  : ����BPU�ڵĹ��ܣ������̿���ʹ��
	virtual int BpuCall(PBPCCOMMSTRU calldata,LIST_BPC_RSMSG &ansdata);
	
	// ������: BpcCall
	// ���  : ������ 2013-4-23 15:08:18
	// ����  : virtual int -1ʧ�� 0�ɹ�
	// ����  : PBPCCOMMSTRU calldata
	// ����  : LIST_BPC_RSMSG &ansdata
	// ����  : ����bpc����
	virtual int BpcCall(PBPCCOMMSTRU calldata,LIST_BPC_RSMSG &ansdata,unsigned int stimeout);
	
	// ������: ExtCall
	// ���  : ������ 2013-4-23 15:01:25
	// ����  : virtual int -1ʧ�� 0�ɹ�
	// ����  : PBPCCOMMSTRU calldata
	// ����  : vector<S_BPC_RSMSG> &ansdata
	// ����  : ���BPC
	virtual int ExtCall(PBPCCOMMSTRU calldata,LIST_BPC_RSMSG &ansdata,unsigned int stimeout) ;
	
	CBF_SimpleLog     *m_pLog;
protected:
	
	
	// ������: GetRcvData
	// ���  : ������ 2013-4-23 14:57:54
	// ����  : virtual int 1������������
	// ����  : BPCCOMMSTRU &data
	// ����  : �첽���պ�ȡ��������
	virtual int GetRcvData(BPCCOMMSTRU &data) ;
	
	// ������: Receive
	// ���  : ������ 2013-4-23 14:47:49
	// ����  : virtual int >0�յ�����  <=0δ�յ�����
	// ����  : ��������
	virtual int OnReceive() ;
	
	
	// ������: OnSend
	// ���  : ������ 2013-4-23 14:57:08
	// ����  : virtual int <0���ӳ�����,�Զ��ر�����   =0�Է���������,����δ��   >0���ͳɹ�
	// ����  : PBPCCOMMSTRU data
	// ����  : �������ݸ�BPC��FPC
	virtual int OnSend(PBPCCOMMSTRU data);

protected:

	// ������: GetAns
	// ���  : ������ 2013-5-2 15:19:05
	// ����  : int -1ʧ�� 0�ɹ�
	// ����  : char msgtype DPCALL DPACALL DPBC DPABC DPPUSH DPPOST
	// ����  : LIST_BPC_RSMSG &ansdata  Ӧ������
	// ����  : unsigned int stimeout  ��ʱʱ�� ��
	// ����  : ����dreb�������ͣ�ȡ��Ӧ����
	int GetAns(char msgtype,LIST_BPC_RSMSG &ansdata,unsigned int stimeout);

	// ������: RegisterBuGroup
	// ���  : ������ 2010-7-15 10:55:26
	// ����  : bool true�ɹ�falseʧ��
	// ����  : ��BPC��ע��bpu����Ϣ
	bool RegisterBuGroup();

	// ������: UnzipBuf
	// ���  : ������ 2013-5-2 16:07:55
	// ����  : bool true�ɹ�falseʧ��
	// ����  : BPCCOMMSTRU &data
	// ����  : ��cZipΪ1-4�Ľ��н��
	bool UnzipBuf(BPCCOMMSTRU &data);

	// ������: ZipBuf
	// ���  : ������ 2013-5-2 16:08:18
	// ����  : bool true�ɹ�falseʧ��
	// ����  : PBPCCOMMSTRU data
	// ����  : ��czipΪ0�ĸ������ý��м��ܻ�ѹ������
	bool ZipBuf(PBPCCOMMSTRU data);


	// ������: OnClose
	// ���  : ������ 2010-7-7 10:17:14
	// ����  : void 
	// ����  : char *msg
	// ����  : �ر�����
	void OnClose(const char *file,int fileline,const char *msg);

	// ������: OnNoReadEvent
	// ���  : ������ 2010-7-7 10:08:56
	// ����  : void 
	// ����  : �޶��¼�����
	void OnNoReadEvent();

	// ������: OnNoEvent
	// ���  : ������ 2010-7-7 10:08:16
	// ����  : void 
	// ����  : ���¼�����
	void OnNoEvent();


	// ������: ConnectFPC
	// ���  : ������ 2010-7-7 9:37:59
	// ����  : int =0���ӳɹ�  ����:ʧ��
	// ����  : ����bPC
	int ConnectBPC();

	// ������: IsSocketEvent
	// ���  : ������ 2010-7-7 9:35:32
	// ����  : bool true�ɹ�falseʧ��
	// ����  : int sec   ����
	// ����  : ��ָ����ʱ����socket�Ƿ��ж��¼�
	bool IsSocketEvent(int sec);

	

	std::string m_sConfigXmlFile;
	
	CBpcEndian  m_pHeadCheck; //dreb�ֽ�����
	CBF_Xml           *m_pConfXml;
	
	CCallBu           m_pCallBu;
	
	CBF_SocketTcp      m_tcpSocket;
	CBF_Date_Time  m_pDate;
	CBF_Des        m_pDes;
	CDrebBpcLog    m_pLogFile;

	SOCKET_HANDLE  m_sock;                   //socket id
	bool            m_bIsConnect;//�Ƿ�������BPC
	int            m_nConnNum;//���Ӵ���
	int            m_nConntime;              //���ӽ���ʱ��
	int            m_nReadtime;              //�ϴζ�����ʱ��
	int            m_nSendtime;              //�ϴη�������ʱ�� 
	int            m_nDisconnectTime;        //���ӶϿ�ʱ��
	int            m_nHeartTime;             //����ʱ��
	std::string    m_sBpuGroupName;     //bpu������

	BPCCOMMSTRU    m_pHeadData;   //��������
	char m_sRcvBuffer[BPCBUFSIZE];            //�������ݻ���   
	unsigned int  m_nRcvBufLen;                //���յ����ݳ���  �ɶ�ν���һ�����ݰ�
	char m_sSendBuffer[BPCBUFSIZE];            //�������ݻ���   
	unsigned int  m_nSendBufLen;                //���͵����ݳ���  �ɶ�ν���һ�����ݰ�
	fd_set m_rset;
	fd_set m_wset;
#if defined(_WINDOWS)
	fd_set m_eset;
#endif

	unsigned int m_nBpcHeadLen;  //DREBͷ+BPCͷ����
	bool         m_bIsFree;   //��bpu�Ƿ����
	bool         m_bIsBpuCall;//�Ƿ�BPU����(���̿��Ƶ���)
	LIST_BPC_RSMSG   *m_listCall;  //bpu���õĽ����
	int m_nLogLevel;
	std::string  m_sLogFile;    //  ��־�ļ�
	std::string  m_sLogPath;   //���ݰ���־Ŀ¼
	int          m_nZipLen;//�������ѹ�����ݳ���
	int          m_nZipFlag;//����ѹ����־

	int          m_nUseIpc;

	std::string  m_sBpcIp;
	int          m_nBpcPort;

};

#undef BUMODULE_EXPORT
#endif // !defined(AFX_BPCLINK_H__9E11A8A6_6781_4BCA_AEB6_0AF8CFE0BE4E__INCLUDED_)
