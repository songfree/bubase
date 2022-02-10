// XdpRecord.h: interface for the CXdpRecord class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XDPRECORD_H__86FFC8E4_2683_43CF_AE23_D6442D392E9B__INCLUDED_)
#define AFX_XDPRECORD_H__86FFC8E4_2683_43CF_AE23_D6442D392E9B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "XdpFmt.h"
#include "BF_Tools.h"
#include "BF_Des.h"

//一条xdp记录   在m_packdata中，变长部分长度为2已转为本机字节序，其它的整型值未转换，即为通讯时的主机序
//取值时转为本机序，置值时转为通讯主机序

//index从1开始

#ifdef XDP_EXPORTS
#define CXDP_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define CXDP_EXPORT __declspec(dllimport)
#else
#define CXDP_EXPORT 
#endif
#endif

class CXDP_EXPORT CXdpRecord  
{
public:

	// 函数名: SetFieldMap
	// 编程  : 王明松 2017-6-30 13:45:54
	// 返回  : void 
	// 参数  : int index int index 从0开始，最大127  位图只有16字节
	// 参数  : bool is=true
	// 描述  : 设置对应的位图
	void SetFieldMap(int index,bool is=true);

	// 函数名: IsFieldSet
	// 编程  : 王明松 2017-6-30 13:45:07
	// 返回  : bool true是 false否
	// 参数  : int index 从0开始，最大127  位图只有16字节
	// 描述  : 判断字段位图是否置为1,即是否已设置此字段
	bool IsFieldSet(int index);

	// 函数名: XdpRecordCopy
	// 编程  : 王明松 2014-11-11 10:08:20
	// 返回  : bool 
	// 参数  : CXdpRecord *src
	// 描述  : 复制一条记录，在调用前必须init
	bool XdpRecordCopy(CXdpRecord *src);

	// 函数名: GetBitMap
	// 编程  : 王明松 2014-11-11 10:06:47
	// 返回  : char * 
	// 描述  : 取得位图
	char * GetBitMap();

	// 函数名: GetPackDataBufferLen
	// 编程  : 王明松 2014-11-11 10:05:59
	// 返回  : unsigned int 
	// 描述  : 取得存储报文的最大空间长度
	unsigned int GetPackDataBufferLen();

	// 函数名: GetPackDataBuffer
	// 编程  : 王明松 2014-11-11 10:04:17
	// 返回  : char * 
	// 描述  : 取得存储报文的最大空间指针
	char * GetPackDataBuffer();

	// 函数名: CancelField
	// 编程  : 王明松 2014-10-30 11:17:16
	// 返回  : bool 
	// 参数  : const char *fieldname
	// 描述  : 取消字段，这样就不会生成此字段报文或取出值了
	bool CancelField(const char *fieldname);
	
	// 函数名: CancelField
	// 编程  : 王明松 2014-10-30 11:16:28
	// 返回  : bool 
	// 参数  : unsigned short index
	// 描述  : 取消字段，这样就不会生成此字段报文或取出值了
	bool CancelField(unsigned short index);

	// 函数名: PrintXdpRec
	// 编程  : 王明松 2014-10-29 9:17:26
	// 返回  : bool true成功 false失败
	// 参数  : char *printbuf   缓冲区  注意大小要够大，否则会越界
	// 描述  : 将xdp里的内容整理写到缓冲区中，供写入日志查看等
	bool PrintXdpRec(char *printbuf);

	// 函数名: GetFieldData
	// 编程  : 王明松 2014-10-28 16:15:27
	// 返回  : bool true成功false失败
	// 参数  : unsigned short index  字段索引
	// 参数  : char *data   值  当字段为数据型时，本方法自动进行字节序转换
	// 输入输出参数  : int &datalen   输入为data缓冲大小，输出为字段值长度
	// 参数  : int &fieldtype         字段类型
	// 参数  : char *errmsg           出错信息
	// 描述  : 取得字段值
	bool GetFieldData(unsigned short index,char *data, unsigned int &datalen,int &fieldtype,char *errmsg);
	bool GetFieldData(unsigned short index,int &idata,char *errmsg);
	bool GetFieldData(unsigned short index,unsigned int &idata,char *errmsg);
	bool GetFieldData(unsigned short index,short &idata,char *errmsg);
	bool GetFieldData(unsigned short index,unsigned short &idata,char *errmsg);
	bool GetFieldData(unsigned short index,double &idata,char *errmsg);

	bool GetFieldData(const char *fieldname,char *data,unsigned int &datalen,int &fieldtype,char *errmsg);
	bool GetFieldData(const char *fieldname,int &idata,char *errmsg);
	bool GetFieldData(const char *fieldname,unsigned int &idata,char *errmsg);
	bool GetFieldData(const char *fieldname,short &idata,char *errmsg);
	bool GetFieldData(const char *fieldname,unsigned short &idata,char *errmsg);
	bool GetFieldData(const char *fieldname,double &idata,char *errmsg);

	// 函数名: SetFieldData
	// 编程  : 王明松 2014-10-28 16:13:26
	// 返回  : bool true成功false失败
	// 参数  : unsigned short index  字段索引
	// 参数  : const char *data    值     当字段为数据型时，本方法自动进行字节序转换
	// 参数  : int datalen         长度
	// 参数  : char *errmsg        出错信息
	// 描述  : 设置字段值
	bool SetFieldData(unsigned short index,const char *data,unsigned int datalen,char *errmsg);
	bool SetFieldData(unsigned short index,int idata,char *errmsg);
	bool SetFieldData(unsigned short index,unsigned int idata,char *errmsg);
	bool SetFieldData(unsigned short index,short idata,char *errmsg);
	bool SetFieldData(unsigned short index,unsigned short idata,char *errmsg);
	bool SetFieldData(unsigned short index,double idata,char *errmsg);

	bool SetFieldData(const char *fieldname,const char *data,unsigned int datalen,char *errmsg);
	bool SetFieldData(const char *fieldname,int idata,char *errmsg);
	bool SetFieldData(const char *fieldname,unsigned int idata,char *errmsg);
	bool SetFieldData(const char *fieldname,short idata,char *errmsg);
	bool SetFieldData(const char *fieldname,unsigned short idata,char *errmsg);
	bool SetFieldData(const char *fieldname,double idata,char *errmsg);

	// 函数名: ToBuffer
	// 编程  : 王明松 2014-10-28 15:16:25
	// 返回  : bool true成功false失败
	// 参数  : unsigned char *xdpbuf  
	// 输入输出参数  : unsigned int &len    输入为缓冲大小，输出为报文长度
	// 参数  : char *errmsg  出错信息
	// 描述  : 生成通讯时的报文
	bool ToBuffer(char *xdpbuf,unsigned int &len,char *errmsg);

	// 函数名: GetRealLen
	// 编程  : 王明松 2014-10-28 15:16:07
	// 返回  : unsigned short 
	// 描述  : 取得实际数据所占空间大小，不包含位图
	unsigned short GetRealLen();

	// 函数名: ResetData
	// 编程  : 王明松 2014-10-27 14:23:34
	// 返回  : void 
	// 描述  : 清空位图，重置xdp实际占用空间为0
	void ResetData();

	// 函数名: FromBuffer
	// 编程  : 王明松 2014-10-24 16:46:29
	// 返回  : bool 
	// 参数  : const char *xdpdata
	// 参数  : int datalen
	// 参数  : char *errmsg
	// 描述  : 解析一条数据，并将数据放入m_pPackData中供获取或修改，置位图及报文实际占用空间
	bool FromBuffer(const char *xdpdata,unsigned int datalen,char *errmsg);

	// 函数名: Init
	// 编程  : 王明松 2014-10-24 16:46:17
	// 返回  : bool 
	// 参数  : CXdpFmt *pfmt
	// 描述  : 初始化类并分配空间
	bool Init(CXdpFmt *pfmt);
	CXdpRecord();
	virtual ~CXdpRecord();

protected:

	// 函数名: SetFieldData
	// 编程  : 王明松 2014-10-28 16:13:26
	// 返回  : bool true成功false失败
	// 参数  : S_XDP_FIELD_DEFINE  *field  字段
	// 参数  : const char *data    值     当字段为数据型时，本方法自动进行字节序转换
	// 参数  : int datalen         长度
	// 参数  : char *errmsg        出错信息
	// 描述  : 设置字段值
	bool SetFieldData(S_XDP_FIELD_DEFINE *field,const char *data,unsigned int datalen,char *errmsg);

	bool GetFieldData(S_XDP_FIELD_DEFINE *field,char *data,unsigned int &datalen,char *errmsg);

	CXdpFmt   *m_pXdpFmt;   //报文定义
	char      m_sBitMap[XDP_BITMAPSIZE+1];//位图
	char      *    m_pPackData;  //存储报文的最大空间，不包含位图
	unsigned int   m_nPackDataMaxLen;//存储报文的空间长度
	unsigned short  m_nRealLen;//本报文实际占用空间
	CBF_Des         m_pDes;//只用到不可见转可见

private:
	unsigned short  i,j;//临时循环变量

	S_XDP_FIELD_DEFINE *m_tField;//临时定义的字段说明
	unsigned short m_tvclen;//临时变量
	unsigned short *m_tus; ////临时变量
};

#endif // !defined(AFX_XDPRECORD_H__86FFC8E4_2683_43CF_AE23_D6442D392E9B__INCLUDED_)
