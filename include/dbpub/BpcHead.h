// BpcHead.h: interface for the CBpcHead class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BPCHEAD_H__D8EDEF1B_7EE3_41D6_B92E_E030C704C1E4__INCLUDED_)
#define AFX_BPCHEAD_H__D8EDEF1B_7EE3_41D6_B92E_E030C704C1E4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "DrebPubDefine.h"
#include "BF_SocketTcp.h"




#ifdef __cplusplus
extern "C" {
#endif
	
	//字节对齐
#ifdef _WINDOWS
#define __PACKED__ 
#pragma pack(push,1)
#else
#ifdef __GNUC__
#define __PACKED__	__attribute__ ((packed))
	//#pragma pack(push,1)
#else
#define __PACKED__
#pragma options align=packed
#endif
#endif
	
#define LMSGID 0x0000   // 主机序
#define RMSGID 0x0101   // 网络序
	
#define BPCBUFSIZE  65534   //bpc的最大缓冲大小
//#define BPCBUFSIZE  8192   //bpc的最大缓冲大小


#define MAXBPUNUM   100    //最大bpu数
#define BEATHEARTTICK  5   //心跳间隔
	
#define BPCSOCK_UNKNOW   0      //未知
#define BPCSOCK_CLI      2      //客户端
#define BPCSOCK_BPU      3      //BPU客户端
#define BPCSOCK_DREB     4      //DREB连接
#define BPCSOCK_LISTEN	  7      //侦听端口
	
#define MSG_REQ         0      //接收请求,bpc发给BPU的请求
#define MSG_FREE        1      //BPU发送空闲请求
#define MSG_GETSERIAL   4      //BPU取共享唯一流水
#define MSG_SETGPARA1   5      //BPU发送SETGPARA1
#define MSG_SETGPARA2   6      //BPU发送SETGPARA2
#define MSG_SETGPARA3   7      //BPU发送SETGPARA3
#define MSG_TRANS        9       //BPU发送消息传递
#define MSG_EXTCALL      10      //BPU发送外调请求	
#define MSG_BPCCALL      11      //BPU发送PU组调用请求，dreb命令字为	
#define MSG_BPUREG       12      //BPU发送注册请求
#define MSG_BPUISREG     13      //BPU发送是否注册请求
#define MSG_GETNEXT      15      //dreb过来的取在BPU缓冲的后续包的消息
#define MSG_BPCMONITOR   16      //主动的监控报告，定时发送

#define MSG_GETBPCNEXT   17      //dreb过来的取在BPC缓存的后续包的消息
#define MSG_CONNECTDREB  18      //dreb过来的已连接DREB成功的消息，可在这消息里进行注册等(主备询问，注册)
#define MSG_MONITOR_SAP  19      //dreb过来的请求监控消息，DREB命令字为CMD_MONITOR_BPC,具体的监控项在service_no里，详见DrebPubDefine.h里的BPCMONI_INFO那里
#define MSG_DREBANS      20      //DREB过来的应答信息


	typedef struct //长度 17
	{
		char            cMsgType;         //消息类型
		short           nMsgFlag;         //LMSGID和RMSGID 表示通讯时报文的字节序
		unsigned short  nIndex;           //接收dreb的数据时的连接的序号，标识此数据从哪个dreb连接上过来的
		unsigned short  nBpuIndex;        //接收bpu请求时bpu连接的序号
		int             nConnectTime;     //发送要使用的socket的时间戳，在发送时和原连接进行比较，若不同表示连接已断并重用，取消发送
		int             nRtime;           //请求入队列时间，BPU注册时BPU的PID进程ID
		unsigned short  nBpcLen;          // 本消息后续数据包长度，不包括本段数据长度
	}__PACKED__ S_BPC_HEAD;

	
	#define BPUDATASIZE  (BPCBUFSIZE-DREBHEADLEN-sizeof(S_BPC_HEAD))   //bpu的最大缓冲大小
	#define BPCHEADLEN   sizeof(S_BPC_HEAD)

	typedef struct 
	{
		S_BPC_HEAD    sBpcHead;        //bpchead
		DREB_HEAD     sDBHead;         //dreb头
		char   sBuffer[BPUDATASIZE];  
	}__PACKED__ BPCCOMMSTRU,*PBPCCOMMSTRU; //发送数据结构
	
	

	typedef struct 
	{
		char         sBpuGroupName[21]; //bpu组名称
		unsigned int nBpuGroupIndex;    //bpu组的序号 可据此直接操作bpu组的队列
		unsigned int nRtime;        //入队列时间
		unsigned int nTotalLen;     //总共要发送数据长度
		unsigned int nSendLen;      //当前要发送数据长度
		unsigned int   s_nDrebSerial;    //数据总线节点产生的流水，标志唯一的请求数据包
		unsigned short s_nNodeId;        //数据总线节点编号
		char           s_cNodePrivateId; //数据总线节点私有序号
		unsigned int index;              //连接序号
		PBPCCOMMSTRU sMsgBuf;	
	}__PACKED__ S_BPC_RSMSG;    
	

	typedef vector<BPCCOMMSTRU> LIST_BPC_RSMSG;

	typedef struct //长度 47
	{
		unsigned int    nFuncNo;           //交易码
		char            sFuncName[41];     //交易名称
		char            cPrio;             //交易优先级
		char            cServiceFlag;      //服务标志 0:BPC内部功能不用在DREB上注册 2:BPU内部功能,不用在DREB上注册 1:要在DREB上注册
	}__PACKED__ S_BPU_FUNCINFO;     //功能信息

	typedef struct 
	{
		char            sBpuGroupName[21]; //bpu组名称
		unsigned int    nFuncNum;          //功能数目
		S_BPU_FUNCINFO  funcinfo;          //功能信息
	}__PACKED__ S_BPU_FUNC_REG;     //功能信息
	
	
typedef struct //长度 6
{
	unsigned int   nSvrMainId;     //公共服务号
	char           cSvrPrivateId;  //私有服务号
	unsigned int   nBpuGroupNum;   //bpu组数目

}__PACKED__ S_MONITORBPC_INFO;      
	


typedef struct 
{
	unsigned int nTime;//日期时间  hhmmss
	unsigned int nUs;  //微秒
	unsigned int nId;  //进程ID
	BPCCOMMSTRU  data;
}__PACKED__ S_LOG_BPCHEAD;

typedef struct 
{
	unsigned int nTime;//日期时间  hhmmss
	unsigned int nUs;  //微秒
	unsigned int nId;  //进程ID
	COMMSTRU  data;
}__PACKED__ S_LOG_DREBHEAD;
	
typedef struct //长度 47
{
	char            sFileName[201];           //文件名 带目录
	char            sFileSize[12];            //文件大小
	char            cBeginFlag;               //文件开始标志 '1'是 '0'否
	char            cEndFlag;                 //文件结束标志 '1'是 '0'否
	char            sFileOffset[12];          //文件偏移量  
}__PACKED__ S_BPC_DUFILE;     //上传下载文件结构

	//取消字节对齐
#ifdef _WINDOWS
#pragma pack(pop)
#else
#ifdef __GNUC__
	//#pragma pack(pop)
#else
#pragma options align=reset
#endif
#endif
#undef __PACKED__
	
#ifdef __cplusplus
}
#endif

#endif // !defined(AFX_BPCHEAD_H__D8EDEF1B_7EE3_41D6_B92E_E030C704C1E4__INCLUDED_)
