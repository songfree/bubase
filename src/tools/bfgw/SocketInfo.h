// SocketInfo.h: interface for the CSocketInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOCKETINFO_H__836AC69E_8563_408B_BFBD_E4195C73718A__INCLUDED_)
#define AFX_SOCKETINFO_H__836AC69E_8563_408B_BFBD_E4195C73718A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "BF_SocketTcp.h"
#include "BF_Mutex.h"
#include "BF_SimpleLog.h"
#include "Resource.h"
#include "GwHead.h"
#include "DrebEndian.h"


#ifdef _WINDOWS
#ifdef _DEBUG
#pragma comment(lib, "bf_dbpubd.lib") 
#pragma message("Automatically linking with   bf_dbpubd.lib ")
#else
#pragma comment(lib, "bf_dbpub.lib") 
#pragma message("Automatically linking with   bf_dbpub.lib ")
#endif

#endif




class CSocketInfo  
{
public:
	
	CSocketInfo();
	virtual ~CSocketInfo();


	// 函数名: Send
	// 编程  : 王明松 2013-12-10 12:24:11
	// 返回  : int 发送数据长度  >0实际发的长度 =0对端缓冲区满 <0发送出错
	// 参数  : const char *data 数据
	// 参数  : int len 数据长度
	// 描述  : 发送数据
	int Send(const char *data,int len);


	// 函数名: GetDrebCmdType
	// 编程  : 王明松 2013-12-10 12:25:26
	// 返回  : std::string 
	// 参数  : int cmdtype
	// 描述  : 取得DREB的命令类型
	std::string GetDrebCmdType(int cmdtype);


	// 函数名: NeedSend
	// 编程  : 王明松 2013-12-10 12:25:42
	// 返回  : bool 
	// 描述  : 是否需要发送
	bool NeedSend();
	

	// 函数名: SetAccept
	// 编程  : 王明松 2013-12-10 12:25:56
	// 返回  : void 
	// 参数  : SOCKET_HANDLE sock
	// 参数  : const char *address
	// 描述  : 设置为新连接
	void SetAccept(SOCKET_HANDLE sock,const char *address);
	
	/// 函数名: SetInput
	/// 编程  : 王明松 2009-7-13 15:44:09
	/// 返回  : void 
	/// 描述  : 设置连接为接入端
	void SetInput();
	
	// 函数名: SetOutput
	// 编程  : 王明松 2010-3-18 11:53:23
	// 返回  : void 
	// 描述  : 设置为转出端
	void SetOutput(const char *ip,int port);

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

	/// 函数名: GetRecvData
	/// 编程  : 王明松 2009-7-3 15:53:27
	/// 返回  : int 数据长度 小于0连接关闭 等于0数据不完整，大于0接收到的数据长度
	/// 参数  : S_GATEDREB_RSMSG *msg
	/// 描述  : 获取接收到的数据
	int GetRecvData(S_GWDREB_RSMSG *msg);
	
	// 函数名: Send
	// 编程  : 王明松 2013-6-6 19:57:31
	// 返回  : int 
	// 参数  : S_GATEDREB_RSMSG &msg
	// 描述  : 
	int Send(S_GWDREB_RSMSG &msg,bool isimmediate=false);
	
	
	/// 函数名: OnRecv
	/// 编程  : 王明松 2009-7-3 15:35:23
	/// 返回  : int 若返回小于0则表示连接关闭，小于通讯头则还要继续收，大于通讯头可以取数据进行处理了
	/// 描述  : 接收数据
	int OnRecv();
	


	/// 函数名: OnShutdown
	/// 编程  : 王明松 2009-7-3 10:17:25
	/// 返回  : void 
	/// 描述  : 强行关闭连接，置各项参数为初始值
	void OnShutdown();
	
	/// 函数名: OnClose
	/// 编程  : 王明松 2009-7-3 10:16:06
	/// 返回  : void 
	/// 参数  : const char *msg
	/// 描述  : 关闭连接，根据连接类型置各项参数
	void OnClose(const char *msg,const char *filename,int fileline);
	
	/// 函数名: IsCanSelect
	/// 编程  : 王明松 2009-7-3 10:14:49
	/// 返回  : bool 
	/// 描述  : 此连接是否可以select操作
	bool IsCanSelect();

	void Init(CResource *res);

	/// 函数名: Listen
	/// 编程  : 王明松 2009-7-3 12:48:10
	/// 返回  : bool 
	/// 参数  : bool blockflag=false  阻塞方式 true为阻塞
	/// 参数  : int queuenum=5
	/// 描述  : 开始侦听，并置内部参数
	bool Listen(bool blockflag=false,int queuenum=5);
	
	/// 函数名: IsFree
	/// 编程  : 王明松 2009-7-3 10:18:17
	/// 返回  : bool 
	/// 描述  : accept时取此连接是否可以accept
	bool IsFree();


	std::string    m_sDrebIp; //dreb ip    转出IP
	int            m_nDrebPort; //dreb port   转出端口
	int            m_nDrebId;//公共节点号
	int            m_nDrebPrivateId;//私有节点号

	char           m_sAddress[33]; //客户端IP,连接过来的IP
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

	int            m_nNoBufferTime;          //缓冲区满时间
	int            m_nNoWriteTime;          //不可写时间

	char m_sRcvBuffer[DREBBUFSIZE];            //接收数据缓冲   
	unsigned int  m_nRcvBufLen;                //接收的数据长度  可多次接收一个数据包
	char m_sSendBuffer[DREBBUFSIZE];            //发送数据缓冲   
	unsigned int  m_nSendBufLen;                //发送的数据长度  可多次接收一个数据包
	
	int  m_index;
	int  m_destIndex;                        //转发目标index
	
	DREB_HEAD m_head;//dreb的头信息

protected:

	void ResetInfo();

	char   m_errMsg[256];
	CBF_SocketTcp m_tcpSocket;
	
	CBF_SimpleLog m_log;	
	CBF_Date_Time m_datetime;
	
	CResource  *m_gRes;
	CBF_Mutex  m_mutex;
	int m_sendNum;
	
	CDrebEndian   m_pDrebEndian;
	

};

#endif // !defined(AFX_SOCKETINFO_H__836AC69E_8563_408B_BFBD_E4195C73718A__INCLUDED_)
