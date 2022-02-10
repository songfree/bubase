// SendData.h: interface for the CSendData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SENDDATA_H__5A84790E_4380_4DE9_BF1D_12915CA0C3EF__INCLUDED_)
#define AFX_SENDDATA_H__5A84790E_4380_4DE9_BF1D_12915CA0C3EF__INCLUDED_

#include "XmlPack.h"
#include "BF_Mutex.h"
#include "BF_SimpleLog.h"
#include "BF_Tools.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define  MAXREQBUF 8*1024

typedef struct 
{
	int  threadNo;//线程号
	int  raflag;//请求应答标志 0请求 1应答 2出错信息
	char cutime[9];
	char buffer[MAXREQBUF];
	int  bufferlen;
	int  usetime; //使用时间
}S_REQANS;

typedef struct 
{
	int length;
	int type; //0字符1浮点2整型
	int dec;
	int align;//0左对齐1右对齐
}S_FIELD_ATTRIBUTE;

class CSendData  
{
public:
	void putResult(bool result);

	// 函数名: PushReqAns
	// 编程  : 王明松 2007-2-6 17:01:47
	// 返回  : bool 
	// 参数  : S_REQANS s_reqans
	// 描述  : 放请求或应答至消息队列
	bool PushReqAns(S_REQANS s_reqans);

	

	// 函数名: SetPackType
	// 编程  : 王明松 2007-2-5 15:56:14
	// 返回  : void 
	// 参数  : int type
	// 参数  : char *seg
	// 描述  : 设置包类型，分隔符
	void SetPackType(int type,char *seg);


	// 函数名: SetParameter
	// 编程  : 王明松 2007-2-5 14:58:38
	// 返回  : void 
	// 参数  : int headlen 包头长度
	// 参数  : bool isinclude 是否包含包头
	// 参数  : bool fill 是否补空格
	// 参数  : int type 包类型 0字长1定长分隔2分隔
	// 参数  : char *seg 分隔符
	// 描述  : 设置参数
	void SetParameter(int headlen, bool isinclude,bool fill,int type, char *seg);

	// 函数名: InitSendData
	// 编程  : 王明松 2007-2-5 13:23:56
	// 返回  : bool 
	// 参数  : int headlen
	// 参数  : bool isinclude
	// 参数  : bool fill
	// 参数  : char *headfile
	// 参数  : char *packfile
	// 描述  : 初始包头包体配置
	bool InitSendData(int headlen,bool isinclude,bool fill,char *headfile,char *packfile);

	// 函数名: GetSendBuf
	// 编程  : 王明松 2007-2-5 12:36:40
	// 返回  : bool 
	// 参数  : char *buf  存放的缓冲
	// 参数  : int &buflen  缓冲长度及返回后的长度
	// 描述  : 取要发送的数据
	bool GetSendBuf(char *buf,unsigned int &buflen);

	bool GetSendBuf(char *buf,unsigned int &buflen,CXmlPack &xmlpack);
	CSendData();
	virtual ~CSendData();
	
	CXmlPack m_headxml;
	CXmlPack m_packxml;

	unsigned int m_headLen;

	int m_packType;//包格式 0定长1定长带分隔 2分隔
	bool m_isIncludeHead;
	bool m_isFillBlank; //是否补空格
	char m_errMsg[256];
	char m_seg[10];//分隔符
	
	vector <S_REQANS> m_reqans;
	int m_nSuccessNum;
	int m_nFaildNum;

	bool m_bIsExit;
	bool m_bIsBegin;
	
//	Cpubstrtime m_publog;
	CBF_SimpleLog m_log;
	CBF_Date_Time m_pDate;
	CBF_Tools     m_pTool;

	int m_nLogLevel;

private:
	CBF_Mutex m_sendDataMutex;
	CBF_Mutex m_putDataMutex;
	// 函数名: GetFieldBufferByType2
	// 编程  : 王明松 2007-2-6 17:01:31
	// 返回  : bool 
	// 参数  : char *value
	// 参数  : S_FIELD_ATTRIBUTE s_field
	// 参数  : char *buf
	// 参数  : int &buflen
	// 描述  : 按分隔符取字段缓冲
	bool GetFieldBufferByType2(char *value, S_FIELD_ATTRIBUTE s_field, char *buf, int &buflen);
	
	// 函数名: GetFieldBufferByType1
	// 编程  : 王明松 2007-2-6 17:01:10
	// 返回  : bool 
	// 参数  : char *value
	// 参数  : S_FIELD_ATTRIBUTE s_field
	// 参数  : char *buf
	// 参数  : int &buflen
	// 描述  : 按定长分隔取字段缓冲
	bool GetFieldBufferByType1(char *value,S_FIELD_ATTRIBUTE s_field,char *buf,int &buflen);
	
	// 函数名: GetFieldBufferByType0
	// 编程  : 王明松 2007-2-6 17:00:47
	// 返回  : bool 
	// 参数  : char *value
	// 参数  : S_FIELD_ATTRIBUTE s_field
	// 参数  : char *buf
	// 参数  : int &buflen
	// 描述  : 按定长包取字段缓冲
	bool GetFieldBufferByType0(char *value,S_FIELD_ATTRIBUTE s_field,char *buf,int &buflen);
	
	// 函数名: GetValue
	// 编程  : 王明松 2007-2-6 17:00:36
	// 返回  : bool 
	// 参数  : CNode *node
	// 参数  : char *value
	// 描述  : 取节点的字段值
	bool GetValue(CNode *node,char *value);
	
	// 函数名: GetFieldBuffer
	// 编程  : 王明松 2007-2-6 16:59:56
	// 返回  : bool 
	// 参数  : CNode *node
	// 参数  : char *buf
	// 参数  : int &buflen
	// 描述  : 取字段缓冲
	bool GetFieldBuffer(CNode *node,char *buf,int &buflen);
	
	// 函数名: GetAttributes
	// 编程  : 王明松 2007-2-6 16:59:44
	// 返回  : bool 
	// 参数  : CNode *node
	// 参数  : S_FIELD_ATTRIBUTE *s_attr
	// 描述  : 从节点中取属性
	bool GetAttributes(CNode *node,S_FIELD_ATTRIBUTE *s_attr);
	
	// 函数名: GetPackBuffer
	// 编程  : 王明松 2007-2-6 16:59:13
	// 返回  : bool 
	// 参数  : char *buf
	// 参数  : int &buflen
	// 描述  : 取组包后的包体
	bool GetPackBuffer(char *buf, int &buflen);
	
	// 函数名: GetHeadBuffer
	// 编程  : 王明松 2007-2-6 16:58:56
	// 返回  : bool 
	// 参数  : char *buf
	// 参数  : int &buflen
	// 描述  : 取组包后的包头
	bool GetHeadBuffer(char *buf,int &buflen);
	

};

#endif // !defined(AFX_SENDDATA_H__5A84790E_4380_4DE9_BF1D_12915CA0C3EF__INCLUDED_)
