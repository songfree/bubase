// ApiSocket.h: interface for the CApiSocket class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_APISOCKET_H__9C4C1318_7B57_4F18_A6C4_E3090A0DEEE6__INCLUDED_)
#define AFX_APISOCKET_H__9C4C1318_7B57_4F18_A6C4_E3090A0DEEE6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GateHead.h"
#include "SendQueue.h"
#include "GlobalResource.h"
#include "DrebEndian.h"
#include "BF_Tools.h"
#include "BF_Des.h"
#include "PoolData.h"


#ifdef CGATEAPI_EXPORTS
#define CGATEAPI_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define CGATEAPI_EXPORT __declspec(dllimport)
#else
#define CGATEAPI_EXPORT 
#endif
#endif

class CGATEAPI_EXPORT CApiSocket  
{
public:

	// 函数名: GetRcvDataLen
	// 编程  : 王明松 2013-6-15 14:19:21
	// 返回  : int 
	// 描述  : 取收到的数据长度
	int GetRcvDataLen();

	// 函数名: Pki
	// 编程  : 王明松 2013-5-30 14:49:29
	// 返回  : bool 
	// 描述  : PKI连接认证
	bool Pki();

	SOCKET_HANDLE GetSocket();

	// 函数名: ConnectGate
	// 编程  : 王明松 2013-5-29 14:42:13
	// 返回  : bool 
	// 描述  : 连接网关
	bool ConnectGate();

	// 函数名: OnRcv
	// 编程  : 王明松 2013-5-28 10:49:16
	// 返回  : int 若返回小于0则表示连接关闭，小于通讯头则还要继续收，大于通讯头可以取数据进行处理了
	// 描述  : 接收数据，由select线程调用
	int OnRcv();


	// 函数名: GetRecvData
	// 编程  : 王明松 2013-5-28 11:50:47
	// 返回  : int 若返回小于0则表示连接关闭，=0未接收完整，大于0收到数据包
	// 参数  : CGATE_COMMSTRU *msg
	// 描述  : 取得接收的交易数据
	int GetRecvData(CGATE_COMMSTRU *msg);
	
	// 函数名: SetPara
	// 编程  : 王明松 2013-5-27 15:59:44
	// 返回  : void 
	// 参数  : CGlobalResource *res
	// 描述  : 设置参数
	void SetPara(CGlobalResource *res);

	// 函数名: OnClose
	// 编程  : 王明松 2013-5-27 15:45:50
	// 返回  : void 
	// 参数  : const char *filename
	// 参数  : int fileline
	// 参数  : const char *msg
	// 参数  : int loglevel
	// 描述  : 关闭连接
	void OnClose(const char *filename,int fileline,const char *msg,int loglevel);

	// 函数名: OnSend
	// 编程  : 王明松 2013-5-27 14:45:15
	// 返回  : int 
	// 描述  : 当socket可写时，主动发送队列里的数据，由select线程调用
//	int OnSend();

	// 函数名: Send
	// 编程  : 王明松 2013-5-27 14:38:52
	// 返回  : int  -1 未连接网关 -2发送出错  1发送部分数据，关闭连接 2发送完成
	// 参数  : PCGATE_COMMSTRU data
	// 描述  : 发送队列，若未完全发送或发送失败则放入队列，当socket可写时再发送  由用户调用
	int Send2Gate(PCGATE_COMMSTRU data);

	
	CPoolData       m_pAnsQueue;//应答队列
	
	CPoolData       m_pReqQueue;//推送队列


	CApiSocket();
	virtual ~CApiSocket();
	
	int    m_nSendTime; //发送时间 秒
	int    m_nRcvTime;  //接收时间 秒
	int    m_nPingTime; //发送ping时间

	unsigned int m_nIndex;//cgate返回的连接索引

	int          m_nSocketStatus;//连接状态 0未连接 1已连接  

	// 函数名: GetMsgType
	// 编程  : 王明松 2013-5-28 10:40:52
	// 返回  : std::string 
	// 参数  : int type
	// 描述  : 取网关消息类型
	std::string GetMsgType(int type);

	CCgateLog      *m_pGateLog;

private:



	// 函数名: Uncompress
	// 编程  : 王明松 2012-4-20 9:32:29
	// 返回  : bool 
	// 输出参数  : unsigned char* destbuf 解开后的数据
	// 输入输出参数  : unsigned int * destlen 输入为destbuf的大小，输出为解开后的数据长度
	// 输入参数  : unsigned char* srbuf  压缩数据
	// 输入参数  : int srbuflen  压缩数据长度
	// 描述  : 解压缩数据
	bool Uncompress(unsigned char *destbuf, unsigned int &destlen, unsigned char *srbuf, int srbuflen);
	
	// 函数名: Compress
	// 编程  : 王明松 2012-4-20 9:32:29
	// 返回  : bool 
	// 输出参数  : unsigned char * destbuf  压缩后的数据
	// 输入输出参数  : unsigned int * destlen  输入为destbuf的大小，输出为压缩后的数据长度
	// 输入参数  : unsigned char * srbuf   要压缩的数据
	// 输入参数  : int srbuflen 要压缩的数据长度
	// 描述  : 压缩数据
	bool Compress(unsigned char *destbuf,unsigned int &destlen,unsigned char *srbuf,int srbuflen);

	std::string GenSignString();

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

	// 函数名: ZipBuf
	// 编程  : 王明松 2013-5-27 21:45:15
	// 返回  : bool 
	// 参数  : PCGATE_COMMSTRU  data
	// 参数  : int nZipFlag=4
	// 描述  : 加密压缩数据
	bool ZipBuf(PCGATE_COMMSTRU  data,int nZipFlag=4);
	
	// 函数名: UnzipBuf
	// 编程  : 王明松 2013-5-27 21:44:59
	// 返回  : bool 
	// 参数  : CGATE_COMMSTRU &data
	// 描述  : 解压数据
	bool UnzipBuf(CGATE_COMMSTRU &data);

	// 函数名: GetDrebCmdType
	// 编程  : 王明松 2013-5-28 10:40:57
	// 返回  : std::string 
	// 参数  : int cmdtype
	// 描述  : 
	std::string GetDrebCmdType(int cmdtype);

	char           m_sRcvBuffer[DREBBUFSIZE];  //接收数据缓冲
	int            m_nRcvBufferLen;//接收数据缓冲大小
	unsigned char  m_sSendBuffer[DREBBUFSIZE];  //接收数据缓冲
	CGATE_HEAD     m_head; //接收报文头

	char           m_sSessionKey[49];//会话密钥
	char           m_sGatePubSignKey[2049]; //网关签名公钥
	unsigned short m_nGatePubSignKeyLen;   //网关签名公钥长度
	char           m_sGatePubRsaKey[2049]; //网关加密公钥
	unsigned short m_nGatePubRsaKeyLen;    //网关加密公钥长度

	char           m_sLocalPubSignKey[2049]; //本地签名公钥
	unsigned short m_nLocalPubSignKeyLen;   //本地签名公钥长度
	char           m_sLocalPubRsaKey[2049]; //本地加密公钥
	unsigned short m_nLocalPubRsaKeyLen;    //本地加密公钥长度


	
	

	CBF_SocketTcp  m_pSocketTcp;//socket处理类
	
	CDrebEndian    m_pEndian;
	CBF_Des        m_pDes;

	CBF_Des        m_pDesZip;

	CGlobalResource  *m_pRes;
	CIErrlog      *m_pLog;
	

	CBF_Mutex      m_mutex;
	CBF_Mutex      m_mutexCompress;

};

#endif // !defined(AFX_APISOCKET_H__9C4C1318_7B57_4F18_A6C4_E3090A0DEEE6__INCLUDED_)
