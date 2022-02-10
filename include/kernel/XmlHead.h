// XmlHead.h: interface for the CXmlHead class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XMLHEAD_H__2D94E286_7858_4198_99AA_99ACCE6D73AF__INCLUDED_)
#define AFX_XMLHEAD_H__2D94E286_7858_4198_99AA_99ACCE6D73AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define  NODETYPE_UNKNOW   0   //未知节点
#define  NODETYPE_VALUE    1   //正常的节点  <XX></XX>
#define  NODETYPE_ATTR     2   //正常的节点  <XX/>
#define  NODETYPE_VER      3   //版本        <?xml version="1.0" encoding="GB2312"?>
#define  NODETYPE_REMARK   4   //注释        <!--注释 -->
#define  NODETYPE_DOCTYPE  5   //文档类型DOCTYPE       <!DOCTYPE items SYSTEM "items.dtd">
#define  NODETYPE_ROOT     6   //根节点 固定为空，内部使用 版本节点属于根节点的子节点


#define NODEVALUELEN    40960      ///最大的节点值
#define NODENAMELEN    1024      ///最大的节点名称
#define NODENAMEATTRLEN    40960      ///节点名称及属性的最大值

#define TAB "   "
#define SPACE 32
#define TABLESPACE 9
#define ENTER 10


#endif // !defined(AFX_XMLHEAD_H__2D94E286_7858_4198_99AA_99ACCE6D73AF__INCLUDED_)
