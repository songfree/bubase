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

	/// ������: ClearPoint
	/// ���  : ������ 2009-1-20 11:17:58
	/// ����  : void 
	/// ����  : ��������xmlpack��ָ�룬��������
	void ClearPoint();

	/// ������: PackArrayCopy
	/// ���  : ������ 2009-1-20 11:18:23
	/// ����  : bool 
	/// ����  : CXmlPackArray *muxmlpack
	/// ����  : ��muxmlpack������ָ�븴�Ƶ������У�muxmlpack������ֻ��ָ�������ʵ�����ݲ�����
	bool PackArrayCopy(CXmlPackArray *muxmlpack);

	/// ������: size
	/// ���  : ������ 2006-6-2 8:50:48
	/// ����  : int 
	/// ����  : �õ������¼��
	int size();

	/// ������: clear
	/// ���  : ������ 2006-6-2 8:50:48
	/// ����  : void 
	/// ����  : ��ն��
	void clear();

	/// ������: Push_back
	/// ���  : ������ 2006-6-2 8:50:58
	/// ����  : bool 
	/// ����  : CXmlPack *xmlpack
	/// ����  : ����һ����������ʹ�ú����pust_back�������pop_back��
	bool push_back(CXmlPack *xmlpack);

	/// ������: GetAt
	/// ���  : ������ 2006-6-2 8:51:08
	/// ����  : bool 
	/// ����  : int rec
	/// ����  : CXmlPack *xmlpack
	/// ����  : ȡ��һ����
	bool GetAt(int rec,CXmlPack *xmlpack);
	CXmlPack *GetAt(int rec);
	CXmlPack *at(int rec);


	/// ������: *push_back
	/// ���  : ������ 2009-1-20 9:04:40
	/// ����  : CXmlPack * �����б��ָ��
	/// ����  : ����һ���µļ�¼�����ؼ�¼��ָ�룬�����ô�ָ�븳ֵ�����ԭ�����ӿ촦���ٶȡ�
	CXmlPack *push_back();


	/// ������: pop_back
	/// ���  : ������ 2009-1-20 9:15:48
	/// ����  : void 
	/// ����  : ɾ�����һ����¼��������
	void pop_back();
protected:
	std::vector <CXmlPack *> m_xmlarray;
};
#undef XML_EXPORT
#endif /// !defined(AFX_XMLPACKARRAY_H__E6A80E21_BFF2_47A5_9B73_394EDB28C7D8__INCLUDED_)
