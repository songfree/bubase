// PubDefine.h: interface for the PubDefine .
// 通讯定义
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PUBDEFINE_H__F2DFC8C6_AAAD_4A6C_BAEC_249903098AF2__INCLUDED_)
#define AFX_PUBDEFINE_H__F2DFC8C6_AAAD_4A6C_BAEC_249903098AF2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "BF_SocketTcp.h"


//socket类型

#define SOCK_UNKNOW   0      //未知
#define SOCK_CLI      2      //客户端
#define SOCK_DREB     3      //其它数据总线节点到本数据总线节点的连接
#define SOCK_SRV      4      //注册的服务端
#define SOCK_TODREB   5      //和其它数据总线节点节点的连接
#define SOCK_SLOT	  6      //注册的通讯槽
#define SOCK_LISTEN	  7      //侦听端口

//数据总线节点模型去处理线程的消息类型

#define MSG_READ     2    //读数据并处理
#define MSG_CLOSE    3    //关闭连接
#define MSG_CONNECT  4    //主动连接,和其它数据总线节点的连接
#define MSG_ROUTE	 5    //路由
#define MSG_PING	 6    //心跳
#define MSG_SERVICE	 7    //服务号路由通知
#define MSG_MONITOR	 8    //监控消息

//数据总线节点请求命令字


#define  CMD_ROUTER      1  //数据总线节点发送路由通知的命令字
#define  CMD_DEL_NODE    2  //数据总线节点之间取消数据总线节点的命令字,只发送至主动连接的数据总线节点
#define  CMD_DEL_SVR     3  //服务端取消注册服务的命令字
#define  CMD_PING        4  //心跳请求的命令字
#define  CMD_CONNECT     5  //连接注册
#define  CMD_SERVICE     6  //服务路由
#define  CMD_REGSERVICE  7  //注册服务
#define  CMD_DPCALL      8  //数据总线节点同步调用 要求最终处理完成后应答
#define  CMD_DPACALL     9  //数据总线节点异步调用 要求接收到的数据总线节点发送至服务后确认应答
#define  CMD_DPBC        10  //数据总线节点广播，即将信息发给指定数据总线节点上所有注册的服务,要求数据总线节点应答
#define  CMD_DPABC       11  //数据总线节点广播，即将信息发给指定数据总线节点上所有注册的服务,不要求数据总线节点应答
#define  CMD_DPPUSH      12 //数据总线节点推送，无须应答
#define  CMD_DPPOST      13 //数据总线节点投递，要求接收到的数据总线节点应答
#define  CMD_UNREGSERVICE 14  //取消交易注册
#define  CMD_MONITOR_DREB     15 //数据总线节点监控
#define  CMD_MONITOR_BPC      16 //业务处理中心内部交易
#define  CMD_DPBC_UNREGTX     17 //发送取消注册交易广播  针对某一个服务，DREB_D_NODEINFO
#define  CMD_SUBSCRIBE         18     //服务向总线订阅广播
#define  CMD_UNSUBSCRIBE         19     //服务向总线取消订阅广播

#define  BEATHEARTTICK        5  //心跳间隔 5秒


#define  DREBMONI_INFO          1     //dreb信息 当前节点、当前节点服务
#define  DREBMONI_HOST          2     //主机信息 CPU 碰盘 内存
#define  DREBMONI_CURTXLIST     3     //dreb信息 当前节点交易列表
#define  DREBMONI_ROUTEDREB     4     //dreb路由信息 节点路由表
#define  DREBMONI_ROUTESERVICE  5     //dreb路由信息 服务交易路由表
#define  DREBMONI_REPORT        6     //dreb运行统计信息
#define  DREBMONI_CONNINFO      7     //dreb连接信息
#define  DREBMONI_REPORTSTART   8     //dreb开始统计信息 pack/s byte/s
#define  DREBMONI_REPORTSTOP    9     //dreb停止统计信息



#define  BPCMONI_INFO       11     //bpc信息   节点 bpu组
#define  BPCMONI_HOST       12     //主机信息  CPU 碰盘 内存
#define  BPCMONI_BPU        13     //路由信息  服务、bpu数
#define  BPCMONI_REPORT     14     //运行统计信息  处理请求数 队列数  
#define  BPCMONI_SETBPU     15     //设置BPU参数
#define  BPCMONI_SETMODE    16     //设置bpc监控模式
#define  BPCMONI_STOPBPU    17     //停止BPU
#define  BPCMONI_STARTBPU   18     //启动BPU
#define  BPCMONI_DOWNLOAD   19     //下载文件
#define  BPCMONI_UPLOAD     20     //上传文件



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
	

#define DREBBUFSIZE  65534  //数据总线节点的最大缓冲大小
//#define DREBBUFSIZE  8192  //数据总线节点的最大缓冲大小
	
typedef struct //长度30
{
	unsigned short s_nNodeId;        //数据总线节点编号
	char           s_cNodePrivateId; //数据总线节点私有序号
	unsigned int   s_nSvrMainId;     //注册在数据总线节点上的主服务功能号   只有服务调用时才用填写
	char           s_cSvrPrivateId;  //注册在数据总线节点上的私有序号 0表示公共 >0为每个服务的序号，s_nSvrMainId+s_cSvrPrivateId 即为私有功能号   只有服务调用时才用填写
	int            s_nHook;          //客户端使用的字段，数据总线节点不用，原样返回;  网关cgate用来存客户端的连接时间戳	 若是广播时，保存广播的key，根据功能号+key进行订阅
	int            s_nSerial;        //异步时填写的流水，数据总线节点不用，原样返回;  客户端的请求序号 请求和应答匹配
	int            s_nGateIndex;     //网关连接索引，原样返回;                         网关cgate用来存客户端的连接序号
	unsigned int   s_nDrebSerial;    //数据总线节点产生的流水，标志唯一的请求数据包
	unsigned short s_nIndex;         //发送此请求的数据总线节点连接索引
	int            s_nTimestamp;     //连接总线的连接索引对应的时间戳，若应答时不同则认为连接已断。若是客户端应答可抛弃。
} __PACKED__ DREB_S_NODEINFO;        //源填写的信息


typedef struct   //长度12
{
	unsigned short d_nNodeId;        //目标数据总线节点编号
	char           d_cNodePrivateId; //数据总线节点私有序号
	unsigned int   d_nSvrMainId;     //目标服务功能号 ,必须填写
	char           d_cSvrPrivateId;  //目标私有序号 0表示公共 >0为每个服务的序号，必须填写
	unsigned int   d_nServiceNo;     //要调用的服务功能号
} __PACKED__ DREB_D_NODEINFO;        //目标信息

typedef struct  //长度5
{
	unsigned short a_nNodeId;        //应答的数据总线节点节点ID
	char           a_cNodePrivateId; //数据总线节点私有序号
	unsigned short a_nRetCode;       //应答的返回码
} __PACKED__ DREB_A_NODEINFO;        //应答的数据总线节点填写的信息

typedef struct  //长度4
{
	unsigned short n_nNextNo;           //后续包序号，后续包总数
	unsigned short n_nNextOffset;       //后续包位移, 第几个后续包
} __PACKED__ NEXT_INFO;          //应答多包填写或请求取后续包填写的信息

typedef struct  //长度6
{
	int              b_nSerial;           //bpc外调或内调使用的数据，存放请求连接的时间戳
	unsigned short   b_cIndex;            //bpc外调或内调使用的数据，存放请求连接索引，根据此索引返回给调用的bpu
} __PACKED__ BPC_CALLINFO;      //BPC调用使用的数据
	
typedef struct  //长度9+30+12+5+4+6+2  = 68
{
	char              cZip;         //压缩标志 0明文 1des加密密钥长度16 2压缩 3DES加密 4DES加密并压缩
	char              cCmd;         //数据总线节点请求命令字，应答时不做变换   
	int               nCrc;         //校验位
	char              cRaflag;      //请求应答标志  0请求 1应答
	char              cNextFlag;    //后续包标志  dreb之间服务路由使用时0首包 1后续包  请求时0首包 1后续包2bpc取后续包3bpu取后续包,应答时0无后续包 1有后续包发2bpc上有后续包要取3bpu上有后续包要取
	char              cDrebAffirm;  //数据总线节点确认标志 0否 1数据总线节点确认
	DREB_S_NODEINFO   s_Sinfo;      //源信息           由请求方及请求数据总线节点填写
    DREB_D_NODEINFO   d_Dinfo;      //目的信息         应答时从源信息取过来更新
	DREB_A_NODEINFO   a_Ainfo;      //应答信息         应答数据总线节点填写
	NEXT_INFO         n_Ninfo;      //多包信息         应答填写或请求取后续包填写
	BPC_CALLINFO      b_Cinfo;      //BPC调用信息
	unsigned short     nLen;        //后面的数据长度
}__PACKED__ DREB_HEAD,*PDREB_HEAD;  //数据总线节点通讯头  
	
#define DREBHEADLEN   sizeof(DREB_HEAD)        //数据总线节点的通讯头大小
#define DREBDATASIZE  (DREBBUFSIZE-DREBHEADLEN)  //数据总线节点的最大数据大小

typedef struct 
{
	DREB_HEAD    head;                                   //通讯头
	char         buffer[DREBDATASIZE];                   //数据总线节点里传输的数据
}__PACKED__ COMMSTRU,*PCOMMSTRU; //数据总线节点通讯结构

typedef struct 
{
	SOCKET_HANDLE socket;      //socketid
	char msgtype;               //消息类型
	int  index;                 //连接的序号，标识此请求从哪个连接上过来的
	int  connecttime;           //发送要使用的socket的时间戳，在发送时和原连接进行比较，若不同表示连接已断并重用，取消发送
	char sockettype;            //socket type SOCK_UNKNOW SOCK_CLI SOCK_DREB SOCK_SRV SOCK_TODREB
	DREB_D_NODEINFO closeinfo; //关闭连接时将原连接信息放入此结构，处理线程处理路由表
    int rtime;                 //入队列时间
	int sendnum;               //发送次数
//	int nTotalLen;             //发送总长度
	unsigned int nTotalLen;     //总共要发送数据长度
	unsigned int nSendLen;      //当前要发送数据长度
	int serial;                //dreb总线流水
}__PACKED__ DREBQUEUEMSG_HEAD;       //dreb内部队列使用的头

typedef struct 
{
	DREBQUEUEMSG_HEAD msghead;  //dreb内部队列使用的头
	COMMSTRU          message;  //消息内容
}__PACKED__ S_DREB_RSMSG;       //数据总线节点内部发送接收数据的结构



typedef struct //长度33
{
	char           cConnectType;   //连接类型 0客户端 1服务端 2数据总线节点 
	unsigned short nNodeId;        //目标数据总线节点编号
	char           cNodePrivateId; //数据总线节点私有序号
	unsigned int   nSvrMainId;     //目标服务功能号 ,必须填写
	char           cSvrPrivateId;  //目标私有序号 0表示公共 >0为每个服务的序号，必须填写
	unsigned int   nBandWidth;     //数据总线节点带宽
	char           sVersion[20];   //连接版本   
}__PACKED__ S_CONNECT_DATA;
typedef struct //长度9
{
	unsigned short nNodeId;        //目标数据总线节点编号
	char           cNodePrivateId; //数据总线节点私有序号
	char           nStep;          //0本地1直连的数据总线节点>1为数据总线节点间的步进
	unsigned int    nBandWidth;     //带宽，单位k
}__PACKED__ S_ROUTE_MSG;    //数据总线节点路由结构

typedef struct //长度15
{
	unsigned short nNodeId;        //目标数据总线节点编号
	char           cNodePrivateId; //数据总线节点私有序号
	unsigned int   nSvrMainId;     //公共服务号
	char           cSvrPrivateId;  //私有服务号
	char           cStep;          //dreb步进
	unsigned short nFuncNum;       //功能号个数
	unsigned int   nFuncNo;        //功能号
}__PACKED__ S_ROUTESVR_MSG;         //数据总线节点服务路由结构

typedef struct //长度11
{
	unsigned int   nSvrMainId;     //公共服务号
	char           cSvrPrivateId;  //私有服务号 
	unsigned short nFuncNum;       //功能号个数
	unsigned int   nFuncNo;        //功能号
}__PACKED__ S_SERVICEREG_MSG;         //数据总线节点服务功能注册结构	    广播订阅结构


//以下为监控使用结构

//DREBMONI_INFO使用结构 begin
typedef struct //长度 6
{
	unsigned int   nSvrMainId;     //公共服务号
	char           cSvrPrivateId;  //私有服务号
	char           cClose;         //是否关闭
}__PACKED__ S_MONITOR_SVR;         //DREB监控服务结构

typedef struct //长度 5
{
	unsigned short nNodeId;        //目标数据总线节点编号
	char           cNodePrivateId; //数据总线节点私有序号
	unsigned short nSvrNum;         //服务个数
}__PACKED__ S_MONITOR_INFO;         //DREB监控信息结构

typedef struct //长度 11
{
	S_MONITOR_INFO s_nodeInfo;       //当前节点信息
	S_MONITOR_SVR  s_svrinfo;        //当前节点服务信息，可包含多个此节点
}__PACKED__ S_MONITOR_CURDREBINFO;         //DREB当前节点信息结构
//DREBMONI_INFO使用结构 end 


//DREBMONI_HOST BPCMONI_HOST使用结构 begin
typedef struct //长度 208
{
	char     sDiskName[200];        //磁盘名
	unsigned int nTotalSpace;         //总大小 m
	unsigned int nUsedSpace;          //使用大小 m
}__PACKED__ S_MONITOR_DISK;         //主机监控信息结构
typedef struct //长度 14
{
	unsigned int nTotalMemory;        //内存总数   m
	unsigned int nUsedMemory;         //使用内存数 m
	int          EmsUseRadio;
	unsigned int nCpuRate;            //CPU使用率 %
	unsigned short nDiskNum;          //磁盘数
}__PACKED__ S_MONITOR_HOST;         //主机监控信息结构
typedef struct //长度 14
{
	S_MONITOR_HOST  s_hostInfo;         //cpu内存信息
	S_MONITOR_DISK  s_diskInfo;         //磁盘信息，可以有多个
}__PACKED__ S_MONITOR_HOSTINFO;         //主机监控信息结构
//DREBMONI_HOST BPCMONI_HOST使用结构 end

//DREBMONI_CURTXLIST 使用结构 begin
typedef struct  //长度15
{
	unsigned int   nSvrMainId;     //注册在DREB的主服务功能号   只有服务调用时才用填写
	char           cSvrPrivateId;  //注册在DREB的私有序号 0表示公共 >0为每个服务的序号，s_nSvrMainId+s_cSvrPrivateId 即为私有功能号   只有服务调用时才用填写
	unsigned int   nFuncNo;        //服务功能号
	unsigned int   nStep;          //0本地1直连的数据总线节点>1为数据总线节点间的步进
	unsigned int   nIndex;         //连接索引  通过此连接索引可得到数据总线节点的连接
}__PACKED__ S_MONITOR_SERVICEINFO;  
typedef struct  //长度 20
{
	unsigned short nNodeId;        //DREB编号
	char           cNodePrivateId; //DREB私有序号	
	unsigned short nTxNum;         //交易码个数
	S_MONITOR_SERVICEINFO  s_txList;  //交易码信息 可以有多个
}__PACKED__ S_MONITOR_TXLIST;
//DREBMONI_CURTXLIST 使用结构 end

//DREBMONI_ROUTEDREB 使有结构 begin
typedef struct //长度15
{
	unsigned short nNodeId;        //目标数据总线节点编号
	char           cNodePrivateId; //数据总线节点私有序号
	unsigned int            nStep;          //0本地1直连的数据总线节点>1为数据总线节点间的步进
	unsigned int            nIndex;         //连接索引  通过此连接索引可得到数据总线节点的连接
	unsigned int            nbandwidth;     //带宽
}__PACKED__ S_MONITOR_DREB;
typedef struct //长度17
{
	unsigned short     nDrebNum;         //DREB个数
	S_MONITOR_DREB  s_drebInfo;       //dreb路由信息 可有多个
}__PACKED__ S_MONITOR_ROUTEDREB;
//DREBMONI_ROUTEDREB 使有结构 end

//DREBMONI_ROUTESERVICE 使用结构  begin
typedef struct //长度20
{
	unsigned short nNodeId;        //DREB编号
	char           cNodePrivateId; //DREB私有序号	
	unsigned int   nSvrMainId;     //注册在DREB的主服务功能号   只有服务调用时才用填写
	char           cSvrPrivateId;  //注册在DREB的私有序号 0表示公共 >0为每个服务的序号，s_nSvrMainId+s_cSvrPrivateId 即为私有功能号   只有服务调用时才用填写
	unsigned int   nFuncNo;        //服务功能号
	unsigned int   nStep;          //0本地1直连的数据总线节点>1为数据总线节点间的步进
	unsigned int   nIndex;         //连接索引  通过此连接索引可得到数据总线节点的连接
}__PACKED__ S_MONITOR_SERVICE;
typedef struct //长度22
{
	unsigned short     nServiceNum;         //服务交易个数
	S_MONITOR_SERVICE  s_service;           //服务交易信息
}__PACKED__ S_MONITOR_ROUTESERVICE;
//DREBMONI_ROUTESERVICE 使用结构  end

//DREBMONI_REPORT 使用结构  begin
typedef struct //长度22
{
	char    sStartDate[18];    //启动日期
	char    sRcvPackClient[21];//接收客户端包数
	char    sRcvByteClient[21];//接收客户端字节数
	char    sAnsPackClient[21];//应答客户端包数 
	char    sAnsByteClient[21];//应答客户端字节数
	
	char    sTransOutNum[21]; //数据转发包数
	char    sTransOutByte[21];//数据转发字节数 
	char    sTransInNum[21];  //数据转发包数
	char    sTransInByte[21]; //数据转发字节数 
	
	char    sCallSrvNum[21];   //调用服务次数
    char    sCallSrvByte[21];  //调用服务字节数 
	char    sSrvAnsNum[21];    //服务应答次数
    char    sSrvAnsByte[21];   //服务应答字节数 
}__PACKED__ S_MONITOR_DREBREPORT;
//DREBMONI_REPORT 使用结构  end

//DREBMONI_CONNINFO 使用结构  begin
typedef struct //长度44
{
	char           sIp[33];            //数据总线IP
	unsigned int   nPort;              //数据总线端口
	char           cStatus;            //连接状态 0正常 1为连接
	unsigned short nNodeId;              //数据总线编号
	char           cNodePrivateId;       //数据总线私有序号
	int            nbandwidth;         //带宽
}__PACKED__ S_MONITOR_CONNINFO;
//DREBMONI_CONNINFO 使用结构  end



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


#endif // !defined(AFX_PUBDEFINE_H__F2DFC8C6_AAAD_4A6C_BAEC_249903098AF2__INCLUDED_)
