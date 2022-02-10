// Xdp.h: interface for the CXdp class.
//
//////////////////////////////////////////////////////////////////////
//此类为一个通用的数据协议类，类似8583数据包

#if !defined(AFX_XDP_H__00ABC1DA_A31B_4150_9238_C51CFCDF587F__INCLUDED_)
#define AFX_XDP_H__00ABC1DA_A31B_4150_9238_C51CFCDF587F__INCLUDED_



#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "XdpFmt.h"
#include "XdpRecord.h"


#ifdef XDP_EXPORTS
#define CXDP_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define CXDP_EXPORT __declspec(dllimport)
#else
#define CXDP_EXPORT 
#endif
#endif

//记录数从1开始

class CXDP_EXPORT CXdp  
{
public:

	// 函数名: GetXdpRecord
	// 编程  : 王明松 2014-11-11 10:12:21
	// 返回  : CXdpRecord *   NULL失败
	// 参数  : unsigned short rec 记录从1开始
	// 描述  : 取得一条记录类
	CXdpRecord * GetXdpRecord(unsigned short rec);


	// 函数名: XdpCopy
	// 编程  : 王明松 2014-11-11 10:12:52
	// 返回  : bool true成功 false失败
	// 参数  : CXdp *src  源报文类
	// 描述  : XDP格式及报文复制
	bool XdpCopy(CXdp *src);

	// 函数名: GetRealPackSize
	// 编程  : 王明松 2014-10-30 11:31:20
	// 返回  : unsigned short 
	// 描述  : 取得当前实际报文的大小
	unsigned short GetRealPackSize();

	// 函数名: CancelField
	// 编程  : 王明松 2014-10-30 11:17:16
	// 返回  : bool 
	// 参数  : const char *fieldname
	// 描述  : 取消字段，这样就不会生成此字段报文或取出值了
	bool CancelField(const char *fieldname);
	bool CancelField(unsigned short rec,const char *fieldname);

	// 函数名: CancelField
	// 编程  : 王明松 2014-10-30 11:16:28
	// 返回  : bool 
	// 参数  : unsigned short index
	// 描述  : 取消字段，这样就不会生成此字段报文或取出值了
	bool CancelField(unsigned short index);
	bool CancelField(unsigned short rec,unsigned short index);

	// 函数名: PrintXdp
	// 编程  : 王明松 2014-10-30 11:15:33
	// 返回  : void 
	// 参数  : char *xdpbuf
	// 描述  : 将报文内容显示出来，二进制数据转为16进制显示
	void PrintXdp(char *xdpbuf);

	// 函数名: GetCurSavePackNum
	// 编程  : 王明松 2014-10-29 11:45:49
	// 返回  : unsigned short 
	// 描述  : 取得保存多记录的结构数目，注意不包括第一条记录
	unsigned short GetCurSavePackNum();

	// 函数名: ClearNextRecord
	// 编程  : 王明松 2014-10-29 11:34:21
	// 返回  : void 
	// 描述  : 清空保存多记录的结构
	void ClearNextRecord();
	

	// 函数名: GetFieldType
	// 编程  : 王明松 2014-10-29 11:22:23
	// 返回  : unsigned short 类型，见XdpFmt.h宏定义
	// 参数  : const char *fieldname/unsigned short index 字段名称/索引
	// 描述  : 取得字段类型
	unsigned short GetFieldType(const char *fieldname);
	unsigned short GetFieldType(unsigned short index);

	// 函数名: GetRecCount
	// 编程  : 王明松 2014-10-29 11:10:29
	// 返回  : unsigned short 记录数
	// 描述  : 返回当前报文中共有几条记录，注意记录数和保存多记录的结构数目不是一个概念
	unsigned short GetRecCount();

	// 函数名: GetFieldValue
	// 编程  : 王明松 2014-10-29 11:04:09
	// 返回  : bool true成功 false失败
	// 参数  : unsigned short rec    第几条记录 从1开始
	// 参数  : unsigned short index/const char *fieldname  字段索引/名称
	// 参数  : char *data      数据缓冲  当为数值型时，要用强制转换  如：int *aa = (int *)data;
	// 输入输出参数  : unsigned int &datalen   输入为data缓冲大小 输出为实际数据长度
	// 参数  : int &fieldtype  字段类型，见XdpFmt.h中宏定义
	// 参数  : char *errmsg  出错信息
	// 描述  : 从xdp中取字段值
	bool GetFieldValue(unsigned short rec,const char *fieldname,char *data,unsigned int &datalen,int &fieldtype,char *errmsg);
	bool GetFieldValue(unsigned short rec,const char *fieldname,int &idata,char *errmsg);
	bool GetFieldValue(unsigned short rec,const char *fieldname,unsigned int &idata,char *errmsg);
	bool GetFieldValue(unsigned short rec,const char *fieldname,short &idata,char *errmsg);
	bool GetFieldValue(unsigned short rec,const char *fieldname,unsigned short &idata,char *errmsg);
	bool GetFieldValue(unsigned short rec,const char *fieldname,double &idata,char *errmsg);
	
	
	bool GetFieldValue(unsigned short rec,unsigned short index,char *data,unsigned int &datalen,int &fieldtype,char *errmsg);
	bool GetFieldValue(unsigned short rec,unsigned short index,int &idata,char *errmsg);
	bool GetFieldValue(unsigned short rec,unsigned short index,unsigned int &idata,char *errmsg);
	bool GetFieldValue(unsigned short rec,unsigned short index,short &idata,char *errmsg);
	bool GetFieldValue(unsigned short rec,unsigned short index,unsigned short &idata,char *errmsg);
	bool GetFieldValue(unsigned short rec,unsigned short index,double &idata,char *errmsg);

	// 函数名: GetFieldValue
	// 编程  : 王明松 2014-10-29 11:11:55
	// 返回  : bool true成功 false失败
	// 参数  : unsigned short index/const char *fieldname 字段索引/名称
	// 参数  : char *data       数据缓冲 当为数值型时，要用强制转换来取值  如：int *aa = (int *)data;
	// 参数  : unsigned int &datalen     输入为data缓冲大小 输出为实际数据长度
	// 参数  : int &fieldtype  字段类型，见XdpFmt.h中宏定义
	// 参数  : char *errmsg   出错信息
	// 描述  : 从xdp中取第一个记录的字段值
	bool GetFieldValue(unsigned short index,char *data,unsigned int &datalen,int &fieldtype,char *errmsg);
	bool GetFieldValue(unsigned short index,int &idata,char *errmsg);
	bool GetFieldValue(unsigned short index,unsigned int &idata,char *errmsg);
	bool GetFieldValue(unsigned short index,short &idata,char *errmsg);
	bool GetFieldValue(unsigned short index,unsigned short &idata,char *errmsg);
	bool GetFieldValue(unsigned short index,double &idata,char *errmsg);

	bool GetFieldValue(const char *fieldname,char *data,unsigned int &datalen,int &fieldtype,char *errmsg);
	bool GetFieldValue(const char *fieldname,int &idata,char *errmsg);
	bool GetFieldValue(const char *fieldname,unsigned int &idata,char *errmsg);
	bool GetFieldValue(const char *fieldname,short &idata,char *errmsg);
	bool GetFieldValue(const char *fieldname,unsigned short &idata,char *errmsg);
	bool GetFieldValue(const char *fieldname,double &idata,char *errmsg);




	// 函数名: SetFieldValue
	// 编程  : 王明松 2014-10-29 11:40:30
	// 返回  : bool 
	// 参数  : unsigned short rec  1开始  要设置为记录数，注意若rec大于原记录数那么记录数要从小到大按顺序设置
	// 参数  : unsigned short index/const char *fieldname 字段索引/名称
	// 参数  : const char *data   数据缓冲 当为数值型时，要用强制转换来置值  如：int *aa = (int *)data; *aa = 1200
	// 参数  : int datalen     数据长度
	// 参数  : char *errmsg    出错信息
	// 描述  : 设置指定记录数的字段值
	bool SetFieldValue(unsigned short rec,const char *fieldname,const char *data,unsigned int datalen,char *errmsg);
	
	bool SetFieldValueM(unsigned short rec,const char *fieldname,int idata,char *errmsg);
	bool SetFieldValueM(unsigned short rec,const char *fieldname,unsigned int idata,char *errmsg);
	bool SetFieldValueM(unsigned short rec,const char *fieldname,short idata,char *errmsg);
	bool SetFieldValueM(unsigned short rec,const char *fieldname,unsigned short idata,char *errmsg);
	bool SetFieldValueM(unsigned short rec,const char *fieldname,double idata,char *errmsg);

	bool SetFieldValue(unsigned short rec,unsigned short index,const char *data,unsigned int datalen,char *errmsg);
	bool SetFieldValue(unsigned short rec,unsigned short index,int idata,char *errmsg);
	bool SetFieldValue(unsigned short rec,unsigned short index,unsigned int idata,char *errmsg);
	bool SetFieldValue(unsigned short rec,unsigned short index,short idata,char *errmsg);
	bool SetFieldValue(unsigned short rec,unsigned short index,unsigned short idata,char *errmsg);
	bool SetFieldValue(unsigned short rec,unsigned short index,double idata,char *errmsg);



	// 函数名: SetFieldValue
	// 编程  : 王明松 2014-10-29 11:40:35
	// 返回  : bool 
	// 参数  : unsigned short index/const char *fieldname 字段索引/名称  
	// 参数  : const char *data   数据缓冲 当为数值型时，要用强制转换来置值  如：int *aa = (int *)data; *aa = 1200
	// 参数  : int datalen    数据长度
	// 参数  : char *errmsg  出错信息
	// 描述  : 设置第一条记录的字段值
	bool SetFieldValue(unsigned short index,const char *data,unsigned int datalen,char *errmsg);
	bool SetFieldValue(unsigned short index,int idata,char *errmsg);
	bool SetFieldValue(unsigned short index,unsigned int idata,char *errmsg);
	bool SetFieldValue(unsigned short index,short idata,char *errmsg);
	bool SetFieldValue(unsigned short index,unsigned short idata,char *errmsg);
	bool SetFieldValue(unsigned short index,double idata,char *errmsg);

	bool SetFieldValue(const char *fieldname,const char *data,unsigned int datalen,char *errmsg);
	bool SetFieldValue(const char *fieldname,int idata,char *errmsg);
	bool SetFieldValue(const char *fieldname,unsigned int idata,char *errmsg);
	bool SetFieldValue(const char *fieldname,short idata,char *errmsg);
	bool SetFieldValue(const char *fieldname,unsigned short idata,char *errmsg);
	bool SetFieldValue(const char *fieldname,double idata,char *errmsg);


	// 函数名: SetCurRecCount
	// 编程  : 王明松 2014-10-29 10:55:12
	// 返回  : bool true成功  false失败，当原报文的记录Y<X时失败
	// 参数  : unsigned short rec 
	// 描述  : 设置当前的记录数，当原报文中有Y条记录时，本次只返回X(X<Y)条记录时，必须重置记录数为X,否则返回的仍为Y条
	bool SetCurRecCount(unsigned short x);

	// 函数名: ResetData
	// 编程  : 王明松 2014-10-27 14:24:43
	// 返回  : void 
	// 描述  : 清除多记录，重置位图
	void ResetData();

	// 函数名: ToBuffer
	// 编程  : 王明松 2014-10-27 14:18:52
	// 返回  : bool true成功 false失败
	// 参数  : char *xdpbuf   xdp报文
	// 输入输出参数  : int &len    输入为xdpbuf大小,输出为xdp报文大小
	// 参数  : char *errmsg   出错信息
	// 描述  : 生成xdp报文
	bool ToBuffer(char *xdpbuf,unsigned int &len,char *errmsg);

	// 函数名: FromBuffer
	// 编程  : 王明松 2014-10-27 14:17:34
	// 返回  : bool true成功 false失败
	// 参数  : const char *xdpbuf   xdp数据缓冲
	// 参数  : int xdpbuflen   xdp数据长度
	// 参数  : char *errmsg   出错信息
	// 描述  : 从数据缓冲解析报文
	bool FromBuffer(const char *xdpbuf,unsigned int xdpbuflen,char *errmsg);

	// 函数名: InitXdp
	// 编程  : 王明松 2014-10-27 14:14:55
	// 返回  : bool true成功 false失败
	// 参数  : const char *datafile   xml配置文件
	// 参数  : char *errmsg      出错信息
	// 描述  : 初始化报文，
	bool InitXdp(const char *datafile,char *errmsg);

	CXdp();
	virtual ~CXdp();

	CXdpFmt          m_pXdpFmt;//xdp配置

protected:

	unsigned short   m_nRecCount;//记录数

	CXdpRecord       m_pFirstRecord;//第一条记录

	std::vector<CXdpRecord *>  m_vNextRecord;//保存后续多记灵的容器 

};

#endif // !defined(AFX_XDP_H__00ABC1DA_A31B_4150_9238_C51CFCDF587F__INCLUDED_)
