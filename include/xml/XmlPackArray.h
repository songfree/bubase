/// XmlPackArray.h: interface for the CXmlPackArray class.
///
/////////////////////////////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_XMLPACKARRAY_H__E6A80E21_BFF2_47A5_9B73_394EDB28C7D8__INCLUDED_)
#define AFX_XMLPACKARRAY_H__E6A80E21_BFF2_47A5_9B73_394EDB28C7D8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif /// _MSC_VER > 1000

#include "XmlPack.h"


#ifdef XML_EXPORTS
#define XML_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define XML_EXPORT __declspec(dllimport)
#else
#define XML_EXPORT 
#endif
#endif

class XML_EXPORT CXmlPackArray 
{
public:
	CXmlPackArray();
	virtual ~CXmlPackArray();
public:

	/// 函数名: ClearPoint
	/// 编程  : 王明松 2009-1-20 11:17:58
	/// 返回  : void 
	/// 描述  : 清除保存的xmlpack类指针，不做销毁
	void ClearPoint();

	/// 函数名: PackArrayCopy
	/// 编程  : 王明松 2009-1-20 11:18:23
	/// 返回  : bool 
	/// 参数  : CXmlPackArray *muxmlpack
	/// 描述  : 将muxmlpack的数据指针复制到本类中，muxmlpack的数据只做指针清除，实际数据不销毁
	bool PackArrayCopy(CXmlPackArray *muxmlpack);

	/// 函数名: size
	/// 编程  : 王明松 2006-6-2 8:50:48
	/// 返回  : int 
	/// 描述  : 得到多包记录数
	int size();

	/// 函数名: clear
	/// 编程  : 王明松 2006-6-2 8:50:48
	/// 返回  : void 
	/// 描述  : 清空多包
	void clear();

	/// 函数名: Push_back
	/// 编程  : 王明松 2006-6-2 8:50:58
	/// 返回  : bool 
	/// 参数  : CXmlPack *xmlpack
	/// 描述  : 放入一个包，建议使用后面的pust_back方法配合pop_back。
	bool push_back(CXmlPack *xmlpack);

	/// 函数名: GetAt
	/// 编程  : 王明松 2006-6-2 8:51:08
	/// 返回  : bool 
	/// 参数  : int rec
	/// 参数  : CXmlPack *xmlpack
	/// 描述  : 取出一个包
	bool GetAt(int rec,CXmlPack *xmlpack);
	CXmlPack *GetAt(int rec);
	CXmlPack *at(int rec);


	/// 函数名: *push_back
	/// 编程  : 王明松 2009-1-20 9:04:40
	/// 返回  : CXmlPack * 放入列表的指针
	/// 描述  : 放入一个新的记录，返回记录的指针，可以用此指针赋值，相对原方法加快处理速度。
	CXmlPack *push_back();


	/// 函数名: pop_back
	/// 编程  : 王明松 2009-1-20 9:15:48
	/// 返回  : void 
	/// 描述  : 删除最后一条记录，并销毁
	void pop_back();
protected:
	std::vector <CXmlPack *> m_xmlarray;
};
#undef XML_EXPORT
#endif /// !defined(AFX_XMLPACKARRAY_H__E6A80E21_BFF2_47A5_9B73_394EDB28C7D8__INCLUDED_)
