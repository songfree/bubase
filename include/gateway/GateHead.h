// GateHead.h: interface for the CGateHead class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GATEHEAD_H__1A490998_AE1D_4016_859A_2C1896E2D51B__INCLUDED_)
#define AFX_GATEHEAD_H__1A490998_AE1D_4016_859A_2C1896E2D51B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DrebPubDefine.h"

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
	
#define  MSG_GATEPING      1          //和网关的PING消息
#define  MSG_GATEREQ       2          //发给网关的请求数据，或网关发给客户端的推送数据
#define  MSG_GATELOGIN     3          //网关登录
#define  MSG_GATESMS       4          //网关短信
#define  MSG_PUSH          5          //api收到的推送数据流，公共流(行情、公告)，私有流(回报等)
#define  MSG_GATEPKISTEP1  6          //网关PKI认证第一步 网关返回签名串
#define  MSG_GATEPKISTEP2  7          //网关PKI认证第二步 API发给网关签名及证书公钥、签名字符串
#define  MSG_GATEPKISTEP3  8          //网关PKI认证第三步 网关发给API签名及证书公钥、会话密钥
#define  MSG_GATESUBSCRIBE 9          //网关订阅

typedef struct  //7
{
	char              cMsgType;        //消息类型  网关和API之间的消息类型 见上面的定义 
	char              cZip;            //压缩标志 0明文 1des加密密钥长度16 2压缩 3DES加密 4压缩并DES加密
	char              cCmd;            //数据总线节点请求命令字，应答时不做变换  
	unsigned short    nCrc;            //校验位
	char              cRaflag;         //请求应答标志  0请求 1应答
	char              cNextFlag;       //后续包标志  dreb之间服务路由使用时0首包 1后续包  请求时0首包 1后续包2bpc取后续包3bpu取后续包,应答时0无后续包 1有后续包发2bpc上有后续包要取3bpu上有后续包要取 
}__PACKED__ S_CGATE_COMMHEAD;

typedef struct //16
{
	unsigned short nNodeId;        //数据总线节点编号
	char           cNodePrivateId; //数据总线节点私有序号
	unsigned int   nSvrMainId;     //注册在数据总线节点上的主服务功能号   只有服务调用时才用填写  当为行情广播时填写合约编号
	char           cSvrPrivateId;  //注册在数据总线节点上的私有序号 0表示公共 >0为每个服务的序号，s_nSvrMainId+s_cSvrPrivateId 即为私有功能号   只有服务调用时才用填写
	unsigned int   nSerial;        //api标识唯一交易的流水;
	unsigned int   nServiceNo;     //要调用的服务功能号 交易码
}__PACKED__ S_CGATE_DESTINFO;

typedef struct  //长度 7+16+2+4+2 = 31
{
	S_CGATE_COMMHEAD  stComm;            //通讯定义
	S_CGATE_DESTINFO  stDest;            //目的信息
	unsigned short    nRetCode;          //通讯服务返回码 0成功
	NEXT_INFO         stNext;          //多包信息         应答填写或请求取后续包填写 当为行情广播时填写行情日期n_nNextOffset及行情序号n_nNextNo
	unsigned short    nLen;            //后面的业务数据长度
}__PACKED__ CGATE_HEAD,*PCGATE_HEAD;     //API和cgate通讯的头  



#define  CGATEHEADLEN sizeof(CGATE_HEAD)  

#define  CLIENTDATASIZE  (DREBBUFSIZE-DREBHEADLEN-CGATEHEADLEN)

typedef struct 
{
	CGATE_HEAD   head;                                   //通讯头
	char         buffer[CLIENTDATASIZE];                    //数据总线节点里传输的数据
}__PACKED__ CGATE_COMMSTRU,*PCGATE_COMMSTRU;             //API和网关通讯结构


typedef struct 
{
	int             rtime;                 //入队列时间
	unsigned short  d_nNodeId;        //目标数据总线节点编号
	char            d_cNodePrivateId; //数据总线节点私有序号
	unsigned int    s_nDrebSerial;    //总线流水标识
	unsigned short  isBC;             //是否广播
	int             index;            //连接索引
	int             timestamp;        //时间戳
	int             txcode;           //交易码
	int             prio;             //优先级
	int             nkey;             //  	 广播的key，对应总线的s_nHook字段
	CGATE_COMMSTRU  data;               //消息内容
}__PACKED__ S_CGATE_SMSG;               //发送数据的结构


typedef struct 
{
	int             rtime;                 //入队列时间
	unsigned int    sendnum;               //发送次数
	unsigned int    offset;     //发送位移 ，即当前发送的数据位置，主要是发送一部分数据的情况
	unsigned short  sendlen;    //发送数据长度
	unsigned short  totallen;   //总数据长度
	unsigned int    serial;    //流水
	CGATE_COMMSTRU  message;               //消息内容
}__PACKED__ S_CGATEAPI_SMSG;               //发送数据的结构


typedef struct 
{
	unsigned short  nSignBufferLen;           //签名数据长度
	char            sSignBuffer[501];         //签名数据缓冲
	unsigned short  nSignStrLen;        //要签名的字符串长度(给网关签名) 网关生成的会话密钥(api公钥加密)长度
	char            sSignStr[501];      //要签名的字符串(给网关签名)  网关生成的会话密钥(api公钥加密)
	unsigned short  nSignPubKeyLen;     //签名公钥长度
	char            sSignPubKey[2049];  //签名公钥
	unsigned short  nRsaPubKeyLen;      //非对称加密公钥长度
	char            sRsaPubKey[2049];   //非对称加密公钥
	char            sKeySerial[61];     //证书序列号
	unsigned short  nKeySerialLen;      //证书序列号长度
	char            sIndex[10];//连接序号
}__PACKED__ PKI_GATESTEP23,*PPKI_GATESTEP23;             //API和网关认证第二步通讯结构


//typedef struct 
//{
//	int             rtime;                 //入队列时间
//	unsigned int    sendnum;               //发送次数
//	unsigned int    offset;     //发送位移 ，即当前发送的数据位置，主要是发送一部分数据的情况
//	unsigned short  sendlen;    //发送数据长度
//	unsigned short  totallen;   //总数据长度
//	unsigned int    serial;    //dreb流水
//	unsigned int    nIndex;    //dreb连接索引
//	PCOMMSTRU       data; 
//}__PACKED__ S_GATEDREB_RSMSG,*PS_GATEDREB_RSMSG; //发送数据结构



typedef struct _S_SUBSCRIBE_
{
    unsigned int nServiceNo;   //要订阅的功能号 交易码
    unsigned int nKey;		   //功能信息的key    key根据业务情况定义，如行情为证券代码，委托回报成交回报为账户id
}S_SUBSCRIBE_;
typedef struct 
{
	char            flag;             //0 取消订阅   1 按功能号+key订阅
	unsigned int    datanum;          //订阅数量
	S_SUBSCRIBE_    subinfo;          //订阅信息
}__PACKED__ S_GATE_SUBSCRIBE,*PS_GATE_SUBSCRIBE;   //向网关订阅的报文结构

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

#endif // !defined(AFX_GATEHEAD_H__1A490998_AE1D_4016_859A_2C1896E2D51B__INCLUDED_)
