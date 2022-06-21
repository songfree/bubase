// BuLink.h: interface for the CBuLink class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BULINK_H__0DD4D8AF_9AA3_48A5_ADAF_5F6B93E465F3__INCLUDED_)
#define AFX_BULINK_H__0DD4D8AF_9AA3_48A5_ADAF_5F6B93E465F3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "BfDefine.h"
#include "BuRes.h"
#include "CallBu.h"
#include "BF_Date_Time.h"
#include "BF_SimpleLog.h"
#include "BF_Md5.h"


#ifdef WIN32
#ifdef _DEBUG

#pragma comment(lib, "bf_bumoduled.lib") 
#pragma message("Automatically linking with  bf_bumoduled.lib")
#else
#pragma comment(lib, "bf_bumodule.lib") 
#pragma message("Automatically linking with  bf_bumodule.lib")
#endif
#endif

class CBuLink  : public CIPcLink
{
public:
	CBuLink();
	virtual ~CBuLink();

public:
	CBuRes             m_pRes;
	CCallBu            m_pBuProc;
	CBF_SocketTcp      m_tcpSocket;
	CBF_SimpleLog      *m_pLog;
	CBF_Date_Time       m_publog;
	SOCKET_HANDLE       m_sock;                   //socket id

	char g_sCurPath[300];
	char g_sModulePath[300];
	char g_sModuleName[300];


	bool m_bIsConnect;

public:
	bool IsSocketEvent(int sec);
	int OnRecv(S_BF_RSMSG_FPC *data);
	int OnSend(S_BF_RSMSG_FPC *data);
	void CallService();
	void ResetData();
	void OnEvent();
	void OnNoEvent();
	void  OnSend();
	void OnRecv();


	// 函数名: Reverse
	// 编程  : 王明松 2010-3-19 9:42:37
	// 返回  : void 
	// 参数  : unsigned char *buf
	// 参数  : int len
	// 描述  : 字节序处理
	void Reverse(unsigned char *buf, int len);
	// 函数名: OnClose
	// 编程  : 王明松 2010-3-19 9:18:20
	// 返回  : void 
	// 参数  : const char *msg
	// 描述  : 关闭和bffcc的连接
	void OnClose(const char *msg);

	// 函数名: Run
	// 编程  : 王明松 2010-3-19 9:17:10
	// 返回  : int <0连接次数超限，退出 =0运行正常
	// 描述  : BU运行主方法
	int Run();

	// 函数名: Stop
	// 编程  : 王明松 2010-3-19 9:16:54
	// 返回  : void 
	// 描述  : 停止BU
	void Stop();


	// 函数名: Connect
	// 编程  : 王明松 2010-3-18 17:37:25
	// 返回  : int =0成功 <0连接超限 >0连接超过次数
	// 描述  : 连接bffcc
	int Connect();

public:
	/// 函数名: SetGPara1
	/// 编程  : 王明松 2008-12-30 9:42:48
	/// 返回  : virtual bool true成功false失败
	/// 参数  : bool setflag=true
	/// 描述  : 接口虚函数，这里要实现.设置全局变量1,若已设置则返回false
	virtual bool SetGPara1(bool setflag=true);
	
	
	/// 函数名: isSetGPara2
	/// 编程  : 王明松 2008-12-30 9:36:10
	/// 返回  : virtual bool true成功false失败
	/// 参数  : bool setflag=true
	/// 描述  : 设置全局变量2,若已设置则返回false
	virtual bool SetGPara2(bool setflag=true);
	
	
	/// 函数名: isSetGPara3
	/// 编程  : 王明松 2008-12-30 9:36:13
	/// 返回  : virtual bool true成功false失败
	/// 参数  : bool setflag=true
	/// 描述  : 设置全局变量3,若已设置则返回false
	virtual bool SetGPara3(bool setflag=true);
	
	/// 函数名: GetThreadShareSerial
	/// 编程  : 王明松 2008-12-30 9:36:13
	/// 返回  : virtual long 流水号
	/// 描述  : 取得唯一的流水号
	virtual long GetThreadShareSerial();
	
	
	// 函数名: AnsData
	// 编程  : 王明松 2013-4-23 15:16:43
	// 返回  : virtual int -1失败 0成功
	// 参数  : S_BPC_RSMSG *data
	// 描述  : 应答数据,由BPU业务里面的原子调用
	virtual int AnsData(PBPCCOMMSTRU data);
	
	// 函数名: DPTrans
	// 编程  : 王明松 2013-4-23 15:11:48
	// 返回  : virtual int -1失败 0成功
	// 参数  : PBPCCOMMSTRU calldata
	// 描述  : 转移至其它服务处理
	virtual int DPTrans(PBPCCOMMSTRU calldata);
	
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
	virtual int ExtCall(PBPCCOMMSTRU calldata,LIST_BPC_RSMSG &ansdata,unsigned int stimeout);
	
	
	// 函数名:  InitLink
	// 编程  : 王明松 2013-4-23 14:44:14
	// 返回  : virtual bool true成功 false失败
	// 参数  : const char *confile
	// 描述  : 初始化连接配置 
	virtual bool  InitLink(const char *confile);

protected:
	int  m_nConnNum;//连接次数

	std::vector <BPCCOMMSTRU> m_listAnsData;

	S_BF_RSMSG_FPC    m_pHeadData;

	char *m_sRcvBuffer;            //接收数据缓冲   
	unsigned int  m_nRcvBufLen;                //接收的数据长度  可多次接收一个数据包
	

	char *sRcvBuffer;            //接收数据缓冲   
	unsigned int  nRcvBufLen;                //接收的数据长度  可多次接收一个数据包

	char *m_sReqAnsData;            //发送数据缓冲   
	int  m_nDataLen;                //发送的数据长度  可多次接收一个数据包


	char *m_sSendBuffer;            //发送数据缓冲   
	unsigned int  m_nSendBufLen;                //发送的数据长度  可多次接收一个数据包


	int            m_nConntime;              //连接建立时间
	int            m_nReadtime;              //上次读数据时间
	int            m_nSendtime;              //上次发送数据时间 

	fd_set m_rset;
	fd_set m_wset;
#if defined(_WINDOWS)
	fd_set m_eset;
#endif
};

#endif // !defined(AFX_BULINK_H__0DD4D8AF_9AA3_48A5_ADAF_5F6B93E465F3__INCLUDED_)
