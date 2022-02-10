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
	
	// 函数名: GetBpcMsgType
	// 编程  : 王明松 2013-4-27 14:51:12
	// 返回  : std::string 
	// 参数  : int msgtype
	// 描述  : 取得BPC消息类型
	std::string GetBpcMsgType(int msgtype);

	// 函数名: GetDrebCmdType
	// 编程  : 王明松 2013-4-27 14:51:38
	// 返回  : std::string 
	// 参数  : int cmdtype
	// 描述  : 取得DREB命令字
	std::string GetDrebCmdType(int cmdtype);

	// 函数名: GetFpcPort
	// 编程  : 王明松 2010-7-6 17:18:44
	// 返回  : int  bpc端口
	// 描述  : 取得BPC端口
	int GetBpcPort();

	// 函数名: GetFpcIp
	// 编程  : 王明松 2010-7-6 17:18:30
	// 返回  : std::string 
	// 描述  : 取得BPC地址
	std::string GetBpcIp();

	// 函数名: Run
	// 编程  : 王明松 2010-7-6 17:17:52
	// 返回  : int 0表示主程序要继续调用  其它表示出错了，退出
	// 描述  : 运行，主程序要不停的调用此方法
	int Run();

	CBPCLink();
	virtual ~CBPCLink();
	
	char g_sCurPath[300];
	char g_sModulePath[300];
	char g_sModuleName[300];

public:
	void Stop();

	// 函数名: LogFile
	// 编程  : 王明松 2013-4-28 10:05:14
	// 返回  : int 
	// 参数  : int level 日志级别
	// 参数  : bool flag  日志标志   false正常请求应答包   true出错数据包
	// 参数  : PBPCCOMMSTRU data
	// 描述  : 将数据包写入日志文件
	int LogFile(int level,bool flag,PBPCCOMMSTRU data);

	// 函数名: OnFree
	// 编程  : 王明松 2013-4-28 9:53:57
	// 返回  : bool true成功 false失败
	// 描述  : 通知BPC我是空闲
	bool OnFree();

	// 函数名: Init测评通道口
	// 编程  : 王明松 2010-7-6 17:18:12
	// 返回  : bool true成功 false失败
	// 参数  : const char *filepathname
	// 描述  : 初始化BPc连接
	virtual bool InitLink(const char *confile);

	/// 函数名: SetGPara1
	/// 编程  : 王明松 2008-12-30 9:42:48
	/// 返回  : virtual bool true成功 false失败
	/// 参数  : bool setflag=true
	/// 描述  : 接口虚函数，这里要实现.设置全局变量1,若已设置则返回false
	virtual bool SetGPara1(bool setflag=true);
	
	
	/// 函数名: isSetGPara2
	/// 编程  : 王明松 2008-12-30 9:36:10
	/// 返回  : virtual bool true成功 false失败
	/// 参数  : bool setflag=true
	/// 描述  : 设置全局变量2,若已设置则返回false
	virtual bool SetGPara2(bool setflag=true);
	
	
	/// 函数名: isSetGPara3
	/// 编程  : 王明松 2008-12-30 9:36:13
	/// 返回  : virtual bool true成功 false失败
	/// 参数  : bool setflag=true
	/// 描述  : 设置全局变量3,若已设置则返回false
	virtual bool SetGPara3(bool setflag=true);
	
	/// 函数名: GetThreadShareSerial
	/// 编程  : 王明松 2008-12-30 9:36:13
	/// 返回  : virtual long  流水号
	/// 描述  : 取得唯一的流水号
	virtual long GetThreadShareSerial();


	// 函数名: AnsData
	// 编程  : 王明松 2013-4-23 15:16:43
	// 返回  : virtual int -1失败 0成功
	// 参数  : PBPCCOMMSTRU data
	// 描述  : 应答数据,由BPU业务里面的原子调用
	virtual int AnsData(PBPCCOMMSTRU data) ;
	
	// 函数名: DPTrans
	// 编程  : 王明松 2013-4-23 15:11:48
	// 返回  : virtual int -1失败 0成功
	// 参数  : PBPCCOMMSTRU calldata
	// 描述  : 转移至其它服务处理
	virtual int DPTrans(PBPCCOMMSTRU calldata) ;
	
	// 函数名: BpuCall
	// 编程  : 王明松 2013-4-23 15:09:13
	// 返回  : virtual int -1失败 0成功
	// 参数  : PBPCCOMMSTRU calldata
	// 参数  : LIST_BPC_RSMSG &ansdata
	// 描述  : 调用BPU内的功能，给流程控制使用
	virtual int BpuCall(PBPCCOMMSTRU calldata,LIST_BPC_RSMSG &ansdata);
	
	// 函数名: BpcCall
	// 编程  : 王明松 2013-4-23 15:08:18
	// 返回  : virtual int -1失败 0成功
	// 参数  : PBPCCOMMSTRU calldata
	// 参数  : LIST_BPC_RSMSG &ansdata
	// 描述  : 调用bpc功能
	virtual int BpcCall(PBPCCOMMSTRU calldata,LIST_BPC_RSMSG &ansdata,unsigned int stimeout);
	
	// 函数名: ExtCall
	// 编程  : 王明松 2013-4-23 15:01:25
	// 返回  : virtual int -1失败 0成功
	// 参数  : PBPCCOMMSTRU calldata
	// 参数  : vector<S_BPC_RSMSG> &ansdata
	// 描述  : 外调BPC
	virtual int ExtCall(PBPCCOMMSTRU calldata,LIST_BPC_RSMSG &ansdata,unsigned int stimeout) ;
	
	CBF_SimpleLog     *m_pLog;
protected:
	
	
	// 函数名: GetRcvData
	// 编程  : 王明松 2013-4-23 14:57:54
	// 返回  : virtual int 1有请求，其它无
	// 参数  : BPCCOMMSTRU &data
	// 描述  : 异步接收后取请求数据
	virtual int GetRcvData(BPCCOMMSTRU &data) ;
	
	// 函数名: Receive
	// 编程  : 王明松 2013-4-23 14:47:49
	// 返回  : virtual int >0收到数据  <=0未收到数据
	// 描述  : 接收数据
	virtual int OnReceive() ;
	
	
	// 函数名: OnSend
	// 编程  : 王明松 2013-4-23 14:57:08
	// 返回  : virtual int <0连接出错了,自动关闭连接   =0对方缓冲区满,连接未关   >0发送成功
	// 参数  : PBPCCOMMSTRU data
	// 描述  : 发送数据给BPC或FPC
	virtual int OnSend(PBPCCOMMSTRU data);

protected:

	// 函数名: GetAns
	// 编程  : 王明松 2013-5-2 15:19:05
	// 返回  : int -1失败 0成功
	// 参数  : char msgtype DPCALL DPACALL DPBC DPABC DPPUSH DPPOST
	// 参数  : LIST_BPC_RSMSG &ansdata  应答数据
	// 参数  : unsigned int stimeout  超时时间 秒
	// 描述  : 根据dreb命令类型，取得应答结果
	int GetAns(char msgtype,LIST_BPC_RSMSG &ansdata,unsigned int stimeout);

	// 函数名: RegisterBuGroup
	// 编程  : 王明松 2010-7-15 10:55:26
	// 返回  : bool true成功false失败
	// 描述  : 在BPC上注册bpu组信息
	bool RegisterBuGroup();

	// 函数名: UnzipBuf
	// 编程  : 王明松 2013-5-2 16:07:55
	// 返回  : bool true成功false失败
	// 参数  : BPCCOMMSTRU &data
	// 描述  : 对cZip为1-4的进行解包
	bool UnzipBuf(BPCCOMMSTRU &data);

	// 函数名: ZipBuf
	// 编程  : 王明松 2013-5-2 16:08:18
	// 返回  : bool true成功false失败
	// 参数  : PBPCCOMMSTRU data
	// 描述  : 对czip为0的根据配置进行加密或压缩处理
	bool ZipBuf(PBPCCOMMSTRU data);


	// 函数名: OnClose
	// 编程  : 王明松 2010-7-7 10:17:14
	// 返回  : void 
	// 参数  : char *msg
	// 描述  : 关闭连接
	void OnClose(const char *file,int fileline,const char *msg);

	// 函数名: OnNoReadEvent
	// 编程  : 王明松 2010-7-7 10:08:56
	// 返回  : void 
	// 描述  : 无读事件处理
	void OnNoReadEvent();

	// 函数名: OnNoEvent
	// 编程  : 王明松 2010-7-7 10:08:16
	// 返回  : void 
	// 描述  : 无事件处理
	void OnNoEvent();


	// 函数名: ConnectFPC
	// 编程  : 王明松 2010-7-7 9:37:59
	// 返回  : int =0连接成功  其他:失败
	// 描述  : 连接bPC
	int ConnectBPC();

	// 函数名: IsSocketEvent
	// 编程  : 王明松 2010-7-7 9:35:32
	// 返回  : bool true成功false失败
	// 参数  : int sec   秒数
	// 描述  : 在指定的时间内socket是否有读事件
	bool IsSocketEvent(int sec);

	

	std::string m_sConfigXmlFile;
	
	CBpcEndian  m_pHeadCheck; //dreb字节序处理
	CBF_Xml           *m_pConfXml;
	
	CCallBu           m_pCallBu;
	
	CBF_SocketTcp      m_tcpSocket;
	CBF_Date_Time  m_pDate;
	CBF_Des        m_pDes;
	CDrebBpcLog    m_pLogFile;

	SOCKET_HANDLE  m_sock;                   //socket id
	bool            m_bIsConnect;//是否连接上BPC
	int            m_nConnNum;//连接次数
	int            m_nConntime;              //连接建立时间
	int            m_nReadtime;              //上次读数据时间
	int            m_nSendtime;              //上次发送数据时间 
	int            m_nDisconnectTime;        //连接断开时间
	int            m_nHeartTime;             //心跳时间
	std::string    m_sBpuGroupName;     //bpu组名称

	BPCCOMMSTRU    m_pHeadData;   //请求数据
	char m_sRcvBuffer[BPCBUFSIZE];            //接收数据缓冲   
	unsigned int  m_nRcvBufLen;                //接收的数据长度  可多次接收一个数据包
	char m_sSendBuffer[BPCBUFSIZE];            //发送数据缓冲   
	unsigned int  m_nSendBufLen;                //发送的数据长度  可多次接收一个数据包
	fd_set m_rset;
	fd_set m_wset;
#if defined(_WINDOWS)
	fd_set m_eset;
#endif

	unsigned int m_nBpcHeadLen;  //DREB头+BPC头长度
	bool         m_bIsFree;   //本bpu是否空闲
	bool         m_bIsBpuCall;//是否BPU调用(流程控制调用)
	LIST_BPC_RSMSG   *m_listCall;  //bpu调用的结果集
	int m_nLogLevel;
	std::string  m_sLogFile;    //  日志文件
	std::string  m_sLogPath;   //数据包日志目录
	int          m_nZipLen;//处理加密压缩数据长度
	int          m_nZipFlag;//加密压缩标志

	int          m_nUseIpc;

	std::string  m_sBpcIp;
	int          m_nBpcPort;

};

#undef BUMODULE_EXPORT
#endif // !defined(AFX_BPCLINK_H__9E11A8A6_6781_4BCA_AEB6_0AF8CFE0BE4E__INCLUDED_)
