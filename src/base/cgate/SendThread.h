// SendThread.h: interface for the CSendThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SENDTHREAD_H__434A6929_6C84_4F36_943B_65F1828DC99D__INCLUDED_)
#define AFX_SENDTHREAD_H__434A6929_6C84_4F36_943B_65F1828DC99D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "BF_Thread.h"
#include "BF_Des.h"
#include "BF_Tools.h"
#include "CgateLog.h"
#include "BF_SocketTcp.h"
#include "GateResource.h"
#include "SendData.h"
#include "AIO_Work.h"
#include "DrebEndian.h"
#include "IErrLog.h"

class CSendThread : public CBF_Thread  
{
public:

	// 函数名: Encrypt
	// 编程  : 王明松 2014-6-14 2:28:23
	// 返回  : bool 
	// 参数  : S_CGATE_SMSG *msg
	// 描述  : 对数据进行加密
	bool Encrypt(S_CGATE_SMSG *msg);
	
	CSendThread();
	virtual ~CSendThread();

	bool SetGlobalVar(CGateResource *res,CSendData *senddata,CAIO_Work *aiowork);

	int  m_nIndex;

private:
	
	// 函数名: Run
	// 编程  : 王明松 2010-3-12 13:00:15
	// 返回  : virtual int 
	// 描述  : 接收发送线程主方法
	virtual int Run();
	
	// 函数名: InitThreadInstance
	// 编程  : 王明松 2012-4-9 12:00:54
	// 返回  : virtual bool 
	// 描述  : 线程初始化，线程启动时调用一次
	virtual bool InitThreadInstance();
	
	// 函数名: ExitThreadInstance
	// 编程  : 王明松 2012-4-9 12:01:05
	// 返回  : virtual void 
	// 描述  : 退出线程要做的工作,线程退出时调用一次
	virtual void ExitThreadInstance();

	bool ZipBuf(PSOCKET_POOL_DATA info,PCGATE_COMMSTRU  data,int nZipFlag);
	
protected:
	CIErrlog      *m_pLog;
	CBF_Date_Time m_datetime;
	CBF_Des        m_pDes; //用于IO线程接收并来解压缩解密的
	CBF_Des        m_pDesSend;
	
	CGateResource  *m_pRes;
	CSendData   *m_pSendData;//交易处理数据队列

	CAIO_Work   *m_pAioWork;

	CCgateLog       *m_pGateLog;
	CDrebEndian    m_pEndian;
	S_CGATE_SMSG  m_pSmsg;

	char          m_sSrcBuffer[CLIENTDATASIZE+2];//针对客户端数据的解密解压缩的源
	char          m_sDestBuffer[CLIENTDATASIZE+2];//针对客户端数据的解密解压缩的目的

	void SendBCMsg(S_CGATE_SMSG *msg);
	void SendSingleBcMsg(S_CGATE_SMSG *msg);
	void SendMsg(S_CGATE_SMSG *msg);

	int  m_nSendQuoteNum;
	int  m_nSendCustNum;

	// 函数名: GetDrebCmdType
	// 编程  : 王明松 2013-5-28 10:40:57
	// 返回  : std::string 
	// 参数  : int cmdtype
	// 描述  : 
	std::string GetDrebCmdType(int cmdtype);
	
	// 函数名: GetMsgType
	// 编程  : 王明松 2013-5-28 10:40:52
	// 返回  : std::string 
	// 参数  : int type
	// 描述  : 取网关消息类型
	std::string GetMsgType(int type);

	// 函数名: Endian2CommCrc
	// 编程  : 王明松 2013-5-28 10:41:08
	// 返回  : void 
	// 参数  : CGATE_HEAD &head
	// 描述  : 字节序转成通讯格式并生成crc校验位
	void Endian2CommCrc(CGATE_HEAD &head);

};

#endif // !defined(AFX_SENDTHREAD_H__434A6929_6C84_4F36_943B_65F1828DC99D__INCLUDED_)
