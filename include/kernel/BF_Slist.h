// BF_Slist.h: interface for the CBF_Slist class.
//
//////////////////////////////////////////////////////////////////////

/***********************************************************************************
  编码日志记录
  编码者: 王明松
  编码日期 2012-4-17 11:00:31 重构
  版本: V1.00 
  说明: 处理分隔符的数据类
 ***********************************************************************************/


#if !defined(AFX_BF_SLIST_H__34A62F79_4DB7_495B_9479_09886538A78D__INCLUDED_)
#define AFX_BF_SLIST_H__34A62F79_4DB7_495B_9479_09886538A78D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "public.h"
#include "BF_Tools.h"

#ifdef KERNEL_EXPORTS
#define KERNEL_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define KERNEL_EXPORT __declspec(dllimport)
#else
#define KERNEL_EXPORT 
#endif
#endif

#define  SLIST_BUFFER_SIZE   8192   //分隔符里分隔的数据最大长度


class KERNEL_EXPORT CBF_Slist  
{
public:
	CBF_Slist(const char *sep="|");

	virtual ~CBF_Slist();
	
public:

	// 函数名: ToStringException
	// 编程  : 王明松 2012-4-18 14:38:20
	// 返回  : std::string 
	// 参数  : unsigned int index  要去除的数据
	// 参数  : unsigned int flag=1   标志  1 a|b|c| 0 a|b|c 3 |a|b|c| 2 |a|b|c
	// 描述  : 将数据组成分隔符分成的字符串 去除指定的数据
	std::string ToStringException(unsigned int index,unsigned int flag=1);

	// 函数名: ToString
	// 编程  : 王明松 2012-4-18 12:27:31
	// 返回  : std::string 
	// 参数  : unsigned int flag 标志  1 a|b|c| 0 a|b|c 3 |a|b|c| 2 |a|b|c
	// 描述  : 将数据组成分隔符分成的字符串
	std::string ToString(unsigned int flag=1);

	// 函数名: FillSepString
	// 编程  : 王明松 2012-4-18 9:34:18
	// 返回  : int 数据个数
	// 参数  : string datastring  以分隔符分隔的数据 1|2|3| 1|2|3
	// 描述  : 解析分隔数据
	int FillSepString(std::string datastring);

	// 函数名: SetSeparateString
	// 编程  : 王明松 2012-4-18 9:22:51
	// 返回  : void 
	// 参数  : char *sepstr
	// 描述  : 设置分隔符
	void SetSeparateString(const char *sepstr);
	
	/// 函数名: Clear
	/// 编程  : 王明松 2007-3-30 10:30:53
	/// 返回  : void 
	/// 描述  : 清除已经装载完的数据
	void Clear();				

	/// 函数名: GetCount
	/// 编程  : 王明松 2007-3-30 10:31:14
	/// 返回  : int 
	/// 描述  : 获取分隔符分隔的数据个数
	int GetCount(); 			

	/// 函数名: SetAt
	/// 编程  : 王明松 2007-3-30 10:31:34
	/// 返回  : bool 
	/// 参数  : std::string buffer
	/// 参数  : int nIndex
	/// 描述  : 修改第nIndex个数据项
	bool SetAt(std::string buffer,int nIndex);	

	/// 函数名: GetAt
	/// 编程  : 王明松 2008-4-21 10:24:21
	/// 返回  : int 
	/// 参数  : int nIndex
	/// 参数  : std::string &data
	/// 参数  : bool flag 去除空格标志
	/// 描述  : 得到某数据项
	bool GetAt(int nIndex,std::string &data,bool flag=false); 

	/// 函数名: GetAt
	/// 编程  : 王明松 2008-4-21 10:26:02
	/// 返回  : string 
	/// 参数  : int nIndex
	/// 参数  : bool flag 去除空格标志
	/// 描述  : 得到某个数据项
	std::string GetAt(int nIndex,bool flag=false);		

	/// 函数名: Add
	/// 编程  : 王明松 2007-3-30 10:37:27
	/// 返回  : int 
	/// 参数  : string data
	/// 描述  : 增加数据项
	void Add(std::string data);		

	/// 函数名: Add
	/// 编程  : 王明松 2007-3-30 10:37:35
	/// 返回  : void 
	/// 参数  : double data
	/// 参数  : int dec  保留小数位
	/// 描述  : 增加数据项
	void Add(double data,int dec=2);

	/// 函数名: Add
	/// 编程  : 王明松 2007-3-30 10:38:06
	/// 返回  : void 
	/// 参数  : int data
	/// 描述  : 增加数据项
	void Add(int data);

	/// 函数名: Add
	/// 编程  : 王明松 2007-3-30 10:38:10
	/// 返回  : void 
	/// 参数  : long data
	/// 描述  : 增加数据项
	void Add(long data);

public:

	bool   m_bNotSepString;

	// 函数名: FillSepString
	// 编程  : 王明松 2012-4-18 9:34:18
	// 返回  : int 数据个数
	// 参数  : string datastring  以汉字或全角分隔符分隔的数据
	// 描述  : 解析分隔数据
	int FillChnSepString(std::string datastring);

	// 函数名: fastDectGB
	// 编程  : 王明松 2013-12-9 9:02:27
	// 返回  : bool 
	// 参数  : const char * src
	// 描述  : 检查字符串是否有汉字
	bool fastDectGB(const char * src);


	// 函数名: bufFillSP
	// 编程  : 王明松 2013-12-9 9:10:08
	// 返回  : void 
	// 参数  : const char* databuf
	// 参数  : char *destbuf
	// 描述  : 非全角、汉字后添加SPEP  调用者必须保证destbuf有足够的空间去保存数据
	void bufFillSP(const char* databuf,  char *destbuf);


	// 函数名: bufRemoveSP
	// 编程  : 王明松 2013-12-9 9:11:22
	// 返回  : void  
	// 参数  : const char* databuf
	// 参数  : char *destbuf
	// 描述  : 去掉SPEP 调用者必须保证destbuf有足够的空间去保存数据
	void  bufRemoveSP(const char* databuf,  char *destbuf);

private:
	std::string  m_sSepstr;///分隔符

	typedef std::list <std::string> STRINGSLIST;

	STRINGSLIST m_buffer_list;
    int m_count; 
	
};

#endif // !defined(AFX_BF_SLIST_H__34A62F79_4DB7_495B_9479_09886538A78D__INCLUDED_)
