// BF_DrebSocketInfo.h: interface for the CBF_DrebSocketInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BF_DREBSOCKETINFO_H__E9295B9F_3BA5_4361_B883_30412F15E49C__INCLUDED_)
#define AFX_BF_DREBSOCKETINFO_H__E9295B9F_3BA5_4361_B883_30412F15E49C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "public.h"
#include "BF_SocketTcp.h"
#include "BF_Mutex.h"
#include "BF_SimpleLog.h"
#include "BF_BufferPool.h"
#include "BF_DrebResource.h"
#include "BpcHead.h"
#include "BF_DrebSendQueue.h"
#include "DrebEndian.h"
#include "BpcEndian.h"
#include "BF_Des.h"
#include "BF_CommDataLog.h"
#include "IErrLog.h"

#ifdef DREBAPI_EXPORTS
#define DREBAPI_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define DREBAPI_EXPORT __declspec(dllimport)
#else
#define DREBAPI_EXPORT 
#endif
#endif

class DREBAPI_EXPORT CBF_DrebSocketInfo  
{
public:
	CBF_DrebSocketInfo();
	virtual ~CBF_DrebSocketInfo();


	std::string GetBpcMsgType(int msgtype);
	std::string GetDrebCmdType(int cmdtype);

	// 函数名: NeedRead
	// 编程  : 王明松 2010-7-22 9:28:09
	// 返回  : bool 
	// 描述  : 连接是否需要读
	bool NeedRead();
	
	// 函数名: NeedConnect
	// 编程  : 王明松 2010-7-22 9:24:20
	// 返回  : bool 
	// 描述  : 是否需要主动进行连接，只有连接类型为SOCK_TODREB时才需要连接
	bool NeedConnect();
	
	// 函数名: NeedSend
	// 编程  : 王明松 2010-7-22 9:23:48
	// 返回  : bool 
	// 描述  : 是否需要发送数据，当队列有数据时才需要发送
	bool NeedSend();

	// 函数名: GetRecvData
	// 编程  : 王明松 2009-7-3 15:53:27
	// 返回  : int 数据长度 小于0连接关闭 等于0数据不完整，大于0接收到的数据长度
	// 参数  : S_BPC_RSMSG *msg
	// 描述  : 获取接收到的数据
	int GetRecvData(S_BPC_RSMSG *msg);
	

	// 函数名: SendMsg
	// 编程  : 王明松 2014-12-10 14:47:56
	// 返回  : int 
	// 参数  : S_BPC_RSMSG &msg
	// 参数  : bool sendimmediate=false
	// 描述  : 发送数据
	int SendMsg(S_BPC_RSMSG &msg,bool sendimmediate=false);
	
	// 函数名: OnSend
	// 编程  : 王明松 2009-7-3 15:35:30
	// 返回  : int 小于0发送失败 >0发送的数据条数
	// 描述  : 从队列里发送数据
	int OnSend();
	
	// 函数名: OnRecv
	// 编程  : 王明松 2009-7-3 15:35:23
	// 返回  : int 若返回小于0则表示连接关闭，小于通讯头则还要继续收，大于通讯头可以取数据进行处理了
	// 描述  : 接收数据
	int OnRecv();

	// 函数名: OnShutdown
	// 编程  : 王明松 2009-7-3 10:17:25
	// 返回  : void 
	// 描述  : 强行关闭连接，置各项参数为初始值
	void OnShutdown();
	
	// 函数名: OnClose
	// 编程  : 王明松 2009-7-3 10:16:06
	// 返回  : void 
	// 参数  : const char *msg
	// 描述  : 关闭连接，根据连接类型置各项参数
	void OnClose(const char *msg,const char *filename,int fileline);
	
	// 函数名: IsCanSelect
	// 编程  : 王明松 2009-7-3 10:14:49
	// 返回  : bool 
	// 描述  : 此连接是否可以select操作
	bool IsCanSelect();

	// 函数名: Init
	// 编程  : 王明松 2014-12-10 14:41:34
	// 返回  : void 
	// 参数  : CBF_DrebResource *res
	// 参数  : CBF_BufferPool *pool
	// 参数  : CIErrlog *perrlog
	// 参数  : CBF_CommDataLog *pdatalog
	// 描述  : 初始化
	void Init(CBF_DrebResource *res,CBF_BufferPool *pool,CIErrlog *perrlog,CBF_CommDataLog *pdreb2bpclog,CBF_CommDataLog *pbpc2dreblog);

	// 函数名: Listen
	// 编程  : 王明松 2009-7-3 12:48:10
	// 返回  : bool 
	// 参数  : int port
	// 参数  : bool blockflag=false  阻塞方式 true为阻塞
	// 参数  : int queuenum=5
	// 描述  : 开始侦听，并置内部参数
	bool Listen(int port,bool blockflag=false,int queuenum=5);
	
	// 函数名: IsFree
	// 编程  : 王明松 2009-7-3 10:18:17
	// 返回  : bool 
	// 描述  : accept时取此连接是否可以accept
	bool IsFree();


	std::string    m_sDrebIp; //dreb ip
	int            m_nDrebPort; //dreb port
	int            m_nDrebId;//公共节点号
	int            m_nDrebPrivateId;//私有节点号

	SOCKET_HANDLE  m_sock;                   //socket id
	int            m_nType;                  //SOCKET类型
	int            m_nPort;                  //侦听端口
	bool           m_bChecked;
	bool           m_bNeedread;              //是否需要读数据
	bool           m_bNeedConnect;//是否需要连接
    char           m_cCreateDate[9];         //连接建立日期
	int            m_nConntime;              //连接建立时间
	int            m_nReadtime;              //上次读数据时间
	int            m_nSendtime;              //上次发送数据时间 
	char           m_cCloseDate[9];         //连接关闭日期
	int            m_nCloseTime;             //关闭时间
	int            m_nPingTime;              //onping时间
	int            m_nPid;                   //连接上来的bpu的进程ID
	char m_sRcvBuffer[BPCBUFSIZE];            //接收数据缓冲   
	unsigned int  m_nRcvBufLen;                //接收的数据长度  可多次接收一个数据包
	char m_sSendBuffer[BPCBUFSIZE];            //发送数据缓冲   
	unsigned int  m_nSendBufLen;                //发送的数据长度  可多次接收一个数据包
	int  m_index;

	CBF_DrebSendQueue       m_pSendQueue;//发送队列

	DREB_HEAD m_head;//dreb的头信息

protected:

	// 函数名: UnzipBuf
	// 编程  : 王明松 2013-5-2 16:07:55
	// 返回  : bool 
	// 参数  : BPCCOMMSTRU &data
	// 描述  : 对cZip为1-4的进行解包
	bool UnzipBuf(BPCCOMMSTRU &data);
	
	// 函数名: ZipBuf
	// 编程  : 王明松 2013-5-2 16:08:18
	// 返回  : bool 
	// 参数  : PBPCCOMMSTRU data
	// 参数  : int zipflag   1-4 
	// 描述  : 对czip为0的根据配置进行加密或压缩处理
	bool ZipBuf(PBPCCOMMSTRU data,int zipflag);

	// 函数名: PutSendMsg
	// 编程  : 王明松 2009-7-3 15:35:48
	// 返回  : int 
	// 参数  : S_BPC_RSMSG msg
	// 描述  : 放入一个要发送的数据至队列并置需发送标志为true
	int PutSendMsg(S_BPC_RSMSG msg);

	void ResetInfo();

	void CheckFirstData();

	char   m_errMsg[256];
	CBF_SocketTcp m_tcpSocket;
	CBF_Date_Time m_datetime;
	CBF_Des        m_pDes;

	//传入的几个类指针
	CBF_CommDataLog *m_pDreb2BpcDataLog;
	CBF_CommDataLog *m_pBpc2DrebDataLog;
	CBF_BufferPool *m_pMemPool;
	CBF_DrebResource  *m_gRes;
	CIErrlog          *m_log;


	CBF_Mutex  m_mutex;
	int m_sendNum;
	unsigned int  m_nBpcHeadLen;
	unsigned int  m_nDrebHeadLen;
	CDrebEndian   m_pDrebEndian;
	CBpcEndian    m_pBpcEndian;
};

#endif // !defined(AFX_BF_DREBSOCKETINFO_H__E9295B9F_3BA5_4361_B883_30412F15E49C__INCLUDED_)
