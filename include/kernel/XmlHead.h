// XmlHead.h: interface for the CXmlHead class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XMLHEAD_H__2D94E286_7858_4198_99AA_99ACCE6D73AF__INCLUDED_)
#define AFX_XMLHEAD_H__2D94E286_7858_4198_99AA_99ACCE6D73AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define  NODETYPE_UNKNOW   0   //δ֪�ڵ�
#define  NODETYPE_VALUE    1   //�����Ľڵ�  <XX></XX>
#define  NODETYPE_ATTR     2   //�����Ľڵ�  <XX/>
#define  NODETYPE_VER      3   //�汾        <?xml version="1.0" encoding="GB2312"?>
#define  NODETYPE_REMARK   4   //ע��        <!--ע�� -->
#define  NODETYPE_DOCTYPE  5   //�ĵ�����DOCTYPE       <!DOCTYPE items SYSTEM "items.dtd">
#define  NODETYPE_ROOT     6   //���ڵ� �̶�Ϊ�գ��ڲ�ʹ�� �汾�ڵ����ڸ��ڵ���ӽڵ�


#define NODEVALUELEN    40960      ///���Ľڵ�ֵ
#define NODENAMELEN    1024      ///���Ľڵ�����
#define NODENAMEATTRLEN    40960      ///�ڵ����Ƽ����Ե����ֵ

#define TAB "   "
#define SPACE 32
#define TABLESPACE 9
#define ENTER 10


#endif // !defined(AFX_XMLHEAD_H__2D94E286_7858_4198_99AA_99ACCE6D73AF__INCLUDED_)
