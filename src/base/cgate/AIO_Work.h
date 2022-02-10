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
//注意，本类的方法可能由多个线程调用，所以要注意数据发生混乱

#define  CHECKFLAG_UNKNOW   0   //连接未知
#define  CHECKFLAG_STEP1    1    //接接客户端发送签名及公钥
#define  CHECKFLAG_STEP2    2    //接收客户端验证及登录
#define  CHECKFLAG_NORMAL   5    //正常连接，登录后置为正常连接

#define  FLAG_FIXEDKEY            94  //固定key加密
#define  FLAG_SESSIONKEY          84   //会话key加密
#define  FLAG_CERTKEY             95  //证书key加密

#define  FLAG_ZIPFIXEDKEY         14  //压缩并固定key加密
#define  FLAG_ZIPSESSIONKEY       4   //压缩并会话key加密
#define  FLAG_ZIPCERTKEY          15  //压缩并证书key加密
#define  FLAG_ZIP                 2   //压缩


class CAIO_Work : public CBF_AIO_WorkBase  
{
public:

	// 函数名: GetSocketByIndex
	// 编程  : 王明松 2013-11-11 9:28:58
	// 返回  : PSOCKET_POOL_DATA 
	// 参数  : int index
	// 描述  : 取得连接信息
	PSOCKET_POOL_DATA GetSocketByIndex(int index);

	// 函数名: SendBCData
	// 编程  : 王明松 2013-7-13 9:15:13
	// 返回  : void 
	// 参数  : CGATE_COMMSTRU &data
	// 描述  : 发送广播数据
	void SendBCData(CGATE_COMMSTRU &data);
	

	// 函数名: GenSignString
	// 编程  : 王明松 2013-6-9 14:45:26
	// 返回  : std::string 
	// 描述  : 随机生成16位数字字符串
	std::string GenSignString();

	CAIO_Work();
	virtual ~CAIO_Work();

	// 函数名: Connect
	// 编程  : 王明松 2012-5-21 11:55:51
	// 返回  : void 
	// 参数  : PSOCKET_POOL_DATA *info
	// 描述  : AIO通知，连接断开，要求连接
	virtual void OnConnect(PSOCKET_POOL_DATA info);


	// 函数名: OnSendBack
	// 编程  : 王明松 2012-5-17 16:38:08
	// 返回  : virtual void 
	// 参数  : PSOCKET_SEND_DATA data   
	// 描述  : AIO通知，写入数据返回，对返回结果进行处理
	virtual void OnSendBack(SOCKSENDQUEUE::iterator data,bool issuccess);
	
	virtual void OnSendBack(PSOCKET_SEND_DATA data,bool issuccess);
	
	// 函数名: OnPing
	// 编程  : 王明松 2012-5-21 11:54:23
	// 返回  : virtual void 
	// 描述  : AIO通知，要发送PING包
	virtual void OnPing(PSOCKET_POOL_DATA info);

	// 函数名: OnRecvBack
	// 编程  : 王明松 2012-5-17 16:36:58
	// 返回  : virtual void 
	// 参数  : PSOCKET_POOL_DATA info
	// 描述  : AIO通知，连接收到数据，对数据进行处理
	virtual void OnRecvBack(PSOCKET_POOL_DATA info);


	// 函数名: OnConnectBack
	// 编程  : 王明松 2012-5-17 16:35:34
	// 返回  : virtual void 
	// 参数  : PSOCKET_POOL_DATA info
	// 参数  : int status 连接状态  0成功 其他失败
	// 描述  : AIO通知，发现主动连接成功，对此连接进行后续处理
	virtual void OnConnectBack(PSOCKET_POOL_DATA info,int status);


	// 函数名: OnClose
	// 编程  : 王明松 2012-5-17 16:34:32
	// 返回  : virtual void 
	// 参数  : PSOCKET_POOL_DATA info 连接信息
	// 参数  : std::string msg
	// 描述  : AIO通知，发现一个连接关闭，AIO并没有对此连接进行关闭处理，要求在这里对此连接进行后续处理，然后调用Close方法通知AIO关闭等
	virtual void OnClose(PSOCKET_POOL_DATA info,std::string msg);


	// 函数名: OnAccept
	// 编程  : 王明松 2012-5-17 16:33:33
	// 返回  : virtual void 
	// 参数  : PSOCKET_POOL_DATA info
	// 描述  : AIO通知，收到一条连接，对此新连接进行处理,AIO框架不放入连接池，由应用检查通过后再显示调用AddAio
	virtual void OnAccept(PSOCKET_POOL_DATA info);


	void SetGlobalVar(CGateResource *res,CPoolData *pool,CBF_BufferPool *mempool,CSendData *senddata);


	// 函数名: SendData
	// 编程  : 王明松 2013-6-9 10:28:31
	// 返回  : int 
	// 参数  : PCGATE_COMMSTRU data
	// 参数  : int index 要发送的连接索引
	// 参数  : int timestamp=0  若不等于0则为应答时原连接的时间戳，发送时判断时间戳是否相同，不相同不发送。
	// 描述  : 发送数据
	int SendData(PCGATE_COMMSTRU data,int index,bool isimmediate,int timestamp=0);


	// 函数名: SendData
	// 编程  : 王明松 2014-4-15 14:11:47
	// 返回  : int 
	// 参数  : PCGATE_COMMSTRU data
	// 参数  : int index
	// 描述  : 直接发送，不用放入队列由另一线程发送
	int SendData(PCGATE_COMMSTRU data,int index);

	// 函数名: ZipBuf
	// 编程  : 王明松 2013-5-27 21:45:15
	// 返回  : bool 
	// 参数  : PCGATE_COMMSTRU  data
	// 参数  : int nZipFlag=4
	// 描述  : 加密压缩数据
	bool ZipBuf(PSOCKET_POOL_DATA info,PCGATE_COMMSTRU  data,int nZipFlag=4);

private:
	CPoolData   *m_pDataPool;     //客户请求队列  从客户连接收到的请求数据，放入此队列，供消息处理线程来处理
	CGateResource  *m_pRes;       //
	CBF_BufferPool *m_pMemPool;
	CSendData    *m_pSendData;

	CGATE_COMMSTRU  m_pRcvData;
	CDrebEndian    m_pEndian;
	CBF_Des        m_pDes; //用于IO线程接收并来解压缩解密的
	CBF_Des        m_pDesSend;
	CIErrlog      *m_pLog;

	CCgateLog       *m_pGateLog;

	CBF_Mutex     m_pDesMutex;

	CBF_Mutex     m_pSendMutex;
	char          m_sSrcBuffer[CLIENTDATASIZE+2];//针对客户端数据的解密解压缩的源
	char          m_sDestBuffer[CLIENTDATASIZE+2];//针对客户端数据的解密解压缩的目的

	// 函数名: ProcessRcvMsg
	// 编程  : 王明松 2013-6-9 19:41:58
	// 返回  : void 
	// 参数  : PSOCKET_POOL_DATA info
	// 参数  : CGATE_COMMSTRU &data
	// 描述  : 处理接收到的消息
	void ProcessRcvMsg(PSOCKET_POOL_DATA info,CGATE_COMMSTRU &data);

	// 函数名: Endian2CommCrc
	// 编程  : 王明松 2013-5-28 10:41:08
	// 返回  : void 
	// 参数  : CGATE_HEAD &head
	// 描述  : 字节序转成通讯格式并生成crc校验位
	void Endian2CommCrc(CGATE_HEAD &head);
	
	
	// 函数名: Endian2HostCrc
	// 编程  : 王明松 2013-5-28 10:41:02
	// 返回  : bool 
	// 参数  : CGATE_HEAD &head
	// 描述  : 转换为本机字节序并校验crc
	bool Endian2HostCrc(CGATE_HEAD &head);
	
	
	
	// 函数名: UnzipBuf
	// 编程  : 王明松 2013-5-27 21:44:59
	// 返回  : bool 
	// 参数  : CGATE_COMMSTRU &data
	// 描述  : 解压数据
	bool UnzipBuf(PSOCKET_POOL_DATA info,CGATE_COMMSTRU &data);
	
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


};

#endif // !defined(AFX_AIO_WORK_H__C839CA1C_4FBD_4A04_B366_345D413AF7EF__INCLUDED_)
