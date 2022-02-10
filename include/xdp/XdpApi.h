// GateApi.h: interface for the CGateApi class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GATEAPI_H__38F43A80_B734_4386_A0E1_1A8E395F79EC__INCLUDED_)
#define AFX_GATEAPI_H__38F43A80_B734_4386_A0E1_1A8E395F79EC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef XDP_EXPORTS
#define CXDP_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define CXDP_EXPORT __declspec(dllimport)
#else
#define CXDP_EXPORT 
#endif
#endif


extern "C"
{	
	// 函数名: GetRealPackSize
	// 编程  : 王明松 2014-10-30 11:31:20
	// 返回  : unsigned short 
	// 参数  : void *CXdpClass   xdp类的实例指针，由InitXdp获得
	// 描述  : 取得当前实际报文的大小
	CXDP_EXPORT unsigned short XDP_GetRealPackSize(void *CXdpClass);

	// 函数名: CancelField
	// 编程  : 王明松 2014-10-30 11:17:16
	// 返回  : int 0成功 非0失败
	// 参数  : void *CXdpClass   xdp类的实例指针，由InitXdp获得
	// 参数  : unsigned short rec  从1开始
	// 参数  : const char *fieldname
	// 描述  : 取消字段，这样就不会生成此字段报文或取出值了
	CXDP_EXPORT int XDP_CancelFieldByName(void *CXdpClass,unsigned short rec,const char *fieldname);

	// 函数名: CancelField
	// 编程  : 王明松 2014-10-30 11:16:28
	// 返回  : int 0成功 非0失败
	// 参数  : void *CXdpClass   xdp类的实例指针，由InitXdp获得
	// 参数  : unsigned short rec   从1开始
	// 参数  : unsigned short index
	// 描述  : 取消字段，这样就不会生成此字段报文或取出值了
	CXDP_EXPORT int XDP_CancelFieldByIndex(void *CXdpClass,unsigned short rec,unsigned short index);

	// 函数名: PrintXdp
	// 编程  : 王明松 2014-10-30 11:15:33
	// 返回  : void 
	// 参数  : void *CXdpClass   xdp类的实例指针，由InitXdp获得
	// 参数  : char *xdpbuf
	// 描述  : 将报文内容显示出来，二进制数据转为16进制显示
	CXDP_EXPORT void XDP_PrintXdp(void *CXdpClass,char *xdpbuf);

	// 函数名: GetCurSavePackNum
	// 编程  : 王明松 2014-10-29 11:45:49
	// 返回  : unsigned short 
	// 参数  : void *CXdpClass   xdp类的实例指针，由InitXdp获得
	// 描述  : 取得保存多记录的结构数目，注意不包括第一条记录
	CXDP_EXPORT unsigned short XDP_GetCurSavePackNum(void *CXdpClass);

	// 函数名: ClearNextRecord
	// 编程  : 王明松 2014-10-29 11:34:21
	// 返回  : void 
	// 参数  : void *CXdpClass   xdp类的实例指针，由InitXdp获得
	// 描述  : 清空保存多记录的结构
	CXDP_EXPORT void XDP_ClearNextRecord(void *CXdpClass);
	

	// 函数名: GetFieldType
	// 编程  : 王明松 2014-10-29 11:22:23
	// 返回  : unsigned short 类型，见XdpFmt.h宏定义
	// 参数  : void *CXdpClass   xdp类的实例指针，由InitXdp获得
	// 参数  : const char *fieldname/unsigned short index 字段名称/索引
	// 描述  : 取得字段类型
	CXDP_EXPORT unsigned short XDP_GetFieldTypeByName(void *CXdpClass,const char *fieldname);
	CXDP_EXPORT unsigned short XDP_GetFieldTypeByIndex(void *CXdpClass,unsigned short index);

	// 函数名: GetRecCount
	// 编程  : 王明松 2014-10-29 11:10:29
	// 返回  : unsigned short 记录数
	// 参数  : void *CXdpClass   xdp类的实例指针，由InitXdp获得
	// 描述  : 返回当前报文中共有几条记录，注意记录数和保存多记录的结构数目不是一个概念
	CXDP_EXPORT unsigned short XDP_GetRecCount(void *CXdpClass);

	// 函数名: GetFieldValue
	// 编程  : 王明松 2014-10-29 11:04:09
	// 返回  : int 0成功 非0失败
	// 参数  : void *CXdpClass   xdp类的实例指针，由InitXdp获得
	// 参数  : unsigned short rec    第几条记录
	// 参数  : unsigned short index/const char *fieldname  字段索引/名称
	// 参数  : char *data      数据缓冲  当为数值型时，要用强制转换  如：int *aa = (int *)data;
	// 输入输出参数  : unsigned int &datalen   输入为data缓冲大小 输出为实际数据长度
	// 参数  : int &fieldtype  字段类型，见XdpFmt.h中宏定义
	// 参数  : char *errmsg  出错信息
	// 描述  : 从xdp中取字段值
	CXDP_EXPORT int XDP_GetFieldValueByName(void *CXdpClass,unsigned short rec,const char *fieldname,char *data,unsigned int *datalen,int *fieldtype,char *errmsg);
	CXDP_EXPORT int XDP_GetFieldValueByIndex(void *CXdpClass,unsigned short rec,unsigned short index,char *data,unsigned int *datalen,int *fieldtype,char *errmsg);

	// 函数名: GetFieldValue
	// 编程  : 王明松 2014-10-29 11:11:55
	// 返回  : int 0成功 非0失败
	// 参数  : void *CXdpClass   xdp类的实例指针，由InitXdp获得
	// 参数  : unsigned short index/const char *fieldname 字段索引/名称
	// 参数  : char *data       数据缓冲 当为数值型时，要用强制转换来取值  如：int *aa = (int *)data;
	// 参数  : unsigned int &datalen     输入为data缓冲大小 输出为实际数据长度
	// 参数  : int &fieldtype  字段类型，见XdpFmt.h中宏定义
	// 参数  : char *errmsg   出错信息
	// 描述  : 从xdp中取第一个记录的字段值
	CXDP_EXPORT int XDP_GetFirstFieldValueByIndex(void *CXdpClass,unsigned short index,char *data,unsigned int *datalen,int *fieldtype,char *errmsg);
	CXDP_EXPORT int XDP_GetFirstFieldValueByName(void *CXdpClass,const char *fieldname,char *data,unsigned int *datalen,int *fieldtype,char *errmsg);



	// 函数名: SetFieldValue
	// 编程  : 王明松 2014-10-29 11:40:30
	// 返回  : int 0成功 非0失败
	// 参数  : void *CXdpClass   xdp类的实例指针，由InitXdp获得
	// 参数  : unsigned short rec    要设置为记录数，注意若rec大于原记录数那么记录数要从小到大按顺序设置
	// 参数  : unsigned short index/const char *fieldname 字段索引/名称
	// 参数  : const char *data   数据缓冲 当为数值型时，要用强制转换来置值  如：int *aa = (int *)data; *aa = 1200
	// 参数  : int datalen     数据长度
	// 参数  : char *errmsg    出错信息
	// 描述  : 设置指定记录数的字段值
	CXDP_EXPORT int XDP_SetFieldValueByName(void *CXdpClass,unsigned short rec,const char *fieldname,const char *data,unsigned int datalen,char *errmsg);
	CXDP_EXPORT int XDP_SetFieldValueByIndex(void *CXdpClass,unsigned short rec,unsigned short index,const char *data,unsigned int datalen,char *errmsg);



	// 函数名: SetFieldValue
	// 编程  : 王明松 2014-10-29 11:40:35
	// 返回  : int 0成功 非0失败
	// 参数  : void *CXdpClass   xdp类的实例指针，由InitXdp获得
	// 参数  : unsigned short index/const char *fieldname 字段索引/名称  
	// 参数  : const char *data   数据缓冲 当为数值型时，要用强制转换来置值  如：int *aa = (int *)data; *aa = 1200
	// 参数  : int datalen    数据长度
	// 参数  : char *errmsg  出错信息
	// 描述  : 设置第一条记录的字段值
	CXDP_EXPORT int XDP_SetFirstFieldValueByIndex(void *CXdpClass,unsigned short index,const char *data,unsigned int datalen,char *errmsg);
	CXDP_EXPORT int XDP_SetFirstFieldValueByName(void *CXdpClass,const char *fieldname,const char *data,unsigned int datalen,char *errmsg);


	// 函数名: SetCurRecCount
	// 编程  : 王明松 2014-10-29 10:55:12
	// 返回  : int 0成功 非0失败，当原报文的记录Y<X时失败
	// 参数  : void *CXdpClass   xdp类的实例指针，由InitXdp获得
	// 参数  : unsigned short rec 
	// 描述  : 设置当前的记录数，当原报文中有Y条记录时，本次只返回X(X<Y)条记录时，必须重置记录数为X,否则返回的仍为Y条
	CXDP_EXPORT int XDP_SetCurRecCount(void *CXdpClass,unsigned short x);

	// 函数名: ResetData
	// 编程  : 王明松 2014-10-27 14:24:43
	// 返回  : void 
	// 参数  : void *CXdpClass   xdp类的实例指针，由InitXdp获得
	// 描述  : 清除多记录，重置位图
	CXDP_EXPORT void XDP_ResetData(void *CXdpClass);

	// 函数名: ToBuffer
	// 编程  : 王明松 2014-10-27 14:18:52
	// 返回  : int 0成功 非0失败
	// 参数  : void *CXdpClass   xdp类的实例指针，由InitXdp获得
	// 参数  : char *xdpbuf   xdp报文
	// 输入输出参数  : int *len    输入为xdpbuf大小,输出为xdp报文大小
	// 参数  : char *errmsg   出错信息
	// 描述  : 生成xdp报文
	CXDP_EXPORT int XDP_ToBuffer(void *CXdpClass,char *xdpbuf,unsigned int *len,char *errmsg);

	// 函数名: FromBuffer
	// 编程  : 王明松 2014-10-27 14:17:34
	// 返回  : int 0成功 非0失败
	// 参数  : void *CXdpClass   xdp类的实例指针，由InitXdp获得
	// 参数  : const char *xdpbuf   xdp数据缓冲
	// 参数  : int xdpbuflen   xdp数据长度
	// 参数  : char *errmsg   出错信息
	// 描述  : 从数据缓冲解析报文
	CXDP_EXPORT int XDP_FromBuffer(void *CXdpClass,const char *xdpbuf,unsigned int xdpbuflen,char *errmsg);

	// 函数名: InitXdp
	// 编程  : 王明松 2014-10-27 14:14:55
	// 返回  : int 0成功 非0失败
	// 输入输出参数  : void **CXdpClass   输入为指针地址，输出为xdp类的实例指针
	// 参数  : const char *datafile   xml配置文件
	// 参数  : char *errmsg      出错信息
	// 描述  : 初始化报文，
	CXDP_EXPORT int XDP_InitXdp(void **CXdpClass,const char *datafile,char *errmsg);


	// 函数名: FreeXdp
	// 编程  : 王明松 2014-11-7 11:19:43
	// 返回  : void
	// 参数  : void *CXdpClass
	// 描述  : 释放xdp类的实例
	CXDP_EXPORT void XDP_FreeXdp(void *CXdpClass);


	// 函数名:  XDP_Copy
	// 编程  : 王明松 2014-11-11 10:22:21
	// 返回  : CXDP_EXPORT int 0成功 非0失败
	// 参数  : void *CXdpClassDst  目标
	// 参数  : void *CXdpClassSrc   源
	// 描述  : xdp复制
	CXDP_EXPORT int  XDP_Copy(void *CXdpClassDst,void *CXdpClassSrc);
}

#endif // !defined(AFX_GATEAPI_H__38F43A80_B734_4386_A0E1_1A8E395F79EC__INCLUDED_)
