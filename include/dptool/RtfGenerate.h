/// RtfGenerate.h: interface for the CRtfGenerate class.
///
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RTFGENERATE_H__15D52AC5_2E78_4803_89E2_989FB9CE890F__INCLUDED_)
#define AFX_RTFGENERATE_H__15D52AC5_2E78_4803_89E2_989FB9CE890F__INCLUDED_

#include "public.h"
//#include "pubstrtime.h"
#include "XmlPack.h"

#if _MSC_VER > 1000
#pragma once
#endif /// _MSC_VER > 1000
#ifdef DPTOOL_EXPORTS
#define DPTOOL_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define DPTOOL_EXPORT __declspec(dllimport)
#else
#define DPTOOL_EXPORT 
#endif
#endif
/* 配置文件
<?xml version="1.0" encoding="GB2312"?>
<package>
<head>
<header>{\rtf1\ansi\deff0
{\fonttbl
{\f0\cpg936\fswiss Microsoft Sans Serif;}
{\f1\cpg936\fswiss Arial Unicode MS;}
{\f2\froman\fcharset0\fprq2{\*\panose 02020603050405020304}Times New Roman;}
{\f13\fnil\fcharset134\fprq2{\*\panose 02010600030101010101}\'cb\'ce\'cc\'e5{\*\falt SimSun};}
{\f17\fnil\fcharset134\fprq2{\*\panose 02010600030101010101}\'ba\'da\'cc\'e5{\*\falt SimHei};}
{\f109\froman\fcharset238\fprq2 Times New Roman CE;}
{\f110\froman\fcharset204\fprq2 Times New Roman Cyr;}
{\f55\fmodern\fcharset134\fprq1{\*\panose 02010609030101010101}\'bf\'ac\'cc\'e5_GB2312;}
{\f621\fnil\fcharset0\fprq2 @\'cb\'ce\'cc\'e5 Western;}
}
{\stylesheet
{\widctlpar\adjustright\fs20\cgrid\snext0 Normal;}
{\*\cs10\additive Default Paragraph Font;}
}
{\*\listtable
{\list\listtemplateid1
{\listlevel\levelnfc0\leveljc0\levelstartat1\levelfollow1{\leveltext \'01\'00;}{\levelnumbers\'01;}\fi-283\li283}
{\listlevel\levelnfc0\leveljc0\levelstartat1\levelfollow1{\leveltext \'03\'00.\'01;}{\levelnumbers\'01\'03;}\fi-283\li566}
{\listlevel\levelnfc0\leveljc0\levelstartat1\levelfollow1{\leveltext \'05\'00.\'01.\'02;}{\levelnumbers\'01\'03\'05;}\fi-283\li849}
{\listlevel\levelnfc0\leveljc0\levelstartat1\levelfollow1{\leveltext \'07\'00.\'01.\'02.\'03;}{\levelnumbers\'01\'03\'05\'07;}\fi-283\li1132}
{\listlevel\levelnfc0\leveljc0\levelstartat1\levelfollow1{\leveltext \'09\'00.\'01.\'02.\'03.\'04;}{\levelnumbers\'01\'03\'05\'07\'09;}\fi-283\li1415}
{\listlevel\levelnfc0\leveljc0\levelstartat1\levelfollow1{\leveltext \'0b\'00.\'01.\'02.\'03.\'04.\'05;}{\levelnumbers\'01\'03\'05\'07\'09\'0b;}\fi-283\li1698}
{\listlevel\levelnfc0\leveljc0\levelstartat1\levelfollow1{\leveltext \'0d\'00.\'01.\'02.\'03.\'04.\'05.\'06;}{\levelnumbers\'01\'03\'05\'07\'09\'0b\'0d;}\fi-283\li1981}
{\listlevel\levelnfc0\leveljc0\levelstartat1\levelfollow1{\leveltext \'0f\'00.\'01.\'02.\'03.\'04.\'05.\'06.\'07;}{\levelnumbers\'01\'03\'05\'07\'09\'0b\'0d\'0f;}\fi-283\li2264}
{\listlevel\levelnfc0\leveljc0\levelstartat1\levelfollow1{\leveltext \'11\'00.\'01.\'02.\'03.\'04.\'05.\'06.\'07.\'08;}{\levelnumbers\'01\'03\'05\'07\'09\'0b\'0d\'0f\'11;}\fi-283\li2547}
\listid1}}
{\*\listoverridetable{\listoverride\listid1\listoverridecount0\ls1}}
\paperw11906\paperh16838\margl454\margt510\margr624\margb510
{\header 
\pard\plain
\s0\ql\sl240\slmult1\brdrb\brdrs\brdrw15 \sb20\sa20\li80\ri80\tqr\tx10828
\f0\fs20
%s\tab %s
\par\pard}{\footer 
\pard\plain 
\s0\ql\sl240\slmult1\brdrt\brdrs\brdrw15 \sb20\sa20\li80\ri80\tqc\tx5414\tqr\tx10828
\f0\fs20
%s\tab %s\tab 页数 \chpgn 
\par\pard}
</header>
<firstpage>\par
\par
\par
\par
\par
\par
\par
\par
\par\pard\plain
\s0\qc\sb240\sa60\sl240\slmult1\brdrt\brdrs\brdrw15 \brdrb\brdrs\brdrw15 \brdrl\brdrs\brdrw15 \brdrr\brdrs\brdrw15 \sb20\sa20\li80\ri80
\b\f0\fs50
%s
\par\pard\pard\plain\par
\pard\plain\par
\pard\plain\par
\pard\plain 
\s0\qc\sl240\slmult1\sb20\sa20\li80\ri80
\f0\fs40
%s
\par\pard
\pard\plain
\s0\qc\sl240\slmult1\sb20\sa20\li80\ri80
\f0\fs30
%s
\par\pard
\pard\plain
\s0\qc\sl240\slmult1\sb20\sa20\li80\ri80
\f0\fs20
\par\pard
\pard\plain
\s0\qc\sl240\slmult1\sb20\sa20\li80\ri80
\f0\fs20
\par\pard
\page</firstpage>
<context>\pard\plain 
\s0\q%s\sl240\slmult1\sb20\sa20\li80\ri80
\f%d\fs%d
%s
\par\pard</context>
<title>{\listtext\plain %s \tab}\pard\ls1\ilvl%d\plain
\s0\ql\sb200\sa60\sl240\slmult1\sb20\sa20\li80\ri80
\b\f1\fs20
{\pard\plain \sb200\sa60\nowidctlpar\adjustright }\f%d\fs%d %s
\par\pard</title>
<tabletitle></tabletitle>
</head>
<pack>
<default>ACCMGR</default>
</pack>
</package>
*/
typedef struct _TABLE_DEFINE_
{
	std::string name;///表格表头名称 设置表头时使用
	std::string data;///表格数据  设置表格数据时使用
	int length;  ///表头所占一行的比例，加起来要等于100  设置表头时使用
	int offset;///在设置时计算的相对偏移,不用设置
};
typedef  std::vector<_TABLE_DEFINE_> _V_TABLE_DEFINE;

class DPTOOL_EXPORT CRtfGenerate  
{
public:
	/// 函数名: SetTableTextData
	/// 编程  : 王明松 2008-8-19 14:47:45
	/// 返回  : int =0成功
	/// 参数  : _V_TABLE_DEFINE &td 字段值
	/// 参数  : int font=0 
	/// 参数  : int fontsize=20
	/// 参数  : int fontsty=0 0无 1粗体 2 斜体 3 下划线
	/// 参数  : int align=0 对齐方式 0左 1中 2右
	/// 参数  : int line=0 是否数据之间划线，0不划，1划
	/// 参数  : bool dataend=false 表格是否结束
	/// 描述  : 设置表格文本数据
	int SetTableTextData(_V_TABLE_DEFINE &td,int font=0,int fontsize=20,int fontsty=0,int align=0,int line=0,bool dataend=false);

	/// 函数名: SetTableTitle
	/// 编程  : 王明松 2008-8-19 14:47:45
	/// 返回  : int =0成功
	/// 参数  : _V_TABLE_DEFINE &td 字段数,length加起来要是100
	/// 参数  : int font=0 
	/// 参数  : int fontsize=20
	/// 参数  : int fontsty=0 0无 1粗体 2 斜体 3 下划线
	/// 参数  : int align=0 对齐方式 0左 1中 2右
	/// 描述  : 设置表格头
	int SetTableTitle(_V_TABLE_DEFINE &td,int font=0,int fontsize=20,int fontsty=0,int align=0);

	/// 函数名: SetTitle
	/// 编程  : 王明松 2008-8-19 14:14:46
	/// 返回  : int =0成功
	/// 参数  : const char *title 标题
	/// 参数  : int ntitle 几号标题 1-7 
	/// 参数  : int font 字体 
	/// 参数  : int fontsize 字体大小
	/// 描述  : 设置标题内容
	int SetTitle(const char *title,int ntitle,int font,int fontsize);

	/// 函数名: SetContext
	/// 编程  : 王明松 2008-8-19 11:14:08
	/// 返回  : int =0成功
	/// 参数  : const char *context 正文
	/// 参数  : int font=0 字体
	/// 参数  : int fontsize=20 字体大小
	/// 参数  : int align=0 对齐方式 0左 1中 2右
	/// 描述  : 设置文档正文
	int SetContext(const char *context,int font=0,int fontsize=20,int align=0);

	/// 函数名: SetFirstPage
	/// 编程  : 王明松 2008-8-19 11:00:21
	/// 返回  : int =0成功
	/// 参数  : const char *docname 文档名称
	/// 参数  : const char *docauthor  文档作者
	/// 参数  : const char *docversion 文档版本
	/// 描述  : 设置文档第一页
	int SetFirstPage(const char *docname,const char *docauthor,const char *docversion);

	/// 函数名: EndRtfFile
	/// 编程  : 王明松 2008-8-19 11:00:11
	/// 返回  : void 
	/// 描述  : 结束rtf文档
	void EndRtfFile();

	/// 函数名: SetRtfHead
	/// 编程  : 王明松 2008-8-19 9:56:45
	/// 返回  : int =0成功
	/// 参数  : const char *leftheader 左边页眉
	/// 参数  : const char *rightheader 右边页眉
	/// 参数  : const char *footer  左边页脚
	/// 参数  : const char *centerfooter 中间页脚
	/// 描述  : 设置rtf文件的页眉页脚 页脚右边固定为页数
	int SetRtfHead(const char *leftheader,const char *rightheader,const char *footer,const char *centerfooter);

	/// 函数名: SetRtfFileName
	/// 编程  : 王明松 2008-8-19 9:24:10
	/// 返回  : int =0成功
	/// 参数  : const char *filename
	/// 描述  : 设置rtf目标文件名
	int SetRtfFileName(const char *filename);

	/// 函数名: InitRtf
	/// 编程  : 王明松 2008-8-19 9:12:24
	/// 返回  : bool 
	/// 参数  : const char *confile
	/// 描述  : rtf初始化，读取基本的rtf配置，从文件中
	bool InitRtf(const char *confile);

	char m_errMsg[256];
	CRtfGenerate();
	virtual ~CRtfGenerate();

protected:
	std::string m_rtfFile;///要生成的rtf文件
	bool m_bIsInit;///是否初始化过
	CXmlPack m_rtfconf;///rft配置模板文件
	bool m_bIsFile;///是否设置文件名

	FILE *m_rtffp;///文件指针
	int m_curTableFieldCount;///当前表格列数
	_V_TABLE_DEFINE m_vTabelOffsetList;///当前表格数及位置


};
#undef DPTOOL_EXPORT
#endif /// !defined(AFX_RTFGENERATE_H__15D52AC5_2E78_4803_89E2_989FB9CE890F__INCLUDED_)
