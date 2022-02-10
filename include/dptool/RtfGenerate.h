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
/* �����ļ�
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
%s\tab %s\tab ҳ�� \chpgn 
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
	std::string name;///����ͷ���� ���ñ�ͷʱʹ��
	std::string data;///�������  ���ñ������ʱʹ��
	int length;  ///��ͷ��ռһ�еı�����������Ҫ����100  ���ñ�ͷʱʹ��
	int offset;///������ʱ��������ƫ��,��������
};
typedef  std::vector<_TABLE_DEFINE_> _V_TABLE_DEFINE;

class DPTOOL_EXPORT CRtfGenerate  
{
public:
	/// ������: SetTableTextData
	/// ���  : ������ 2008-8-19 14:47:45
	/// ����  : int =0�ɹ�
	/// ����  : _V_TABLE_DEFINE &td �ֶ�ֵ
	/// ����  : int font=0 
	/// ����  : int fontsize=20
	/// ����  : int fontsty=0 0�� 1���� 2 б�� 3 �»���
	/// ����  : int align=0 ���뷽ʽ 0�� 1�� 2��
	/// ����  : int line=0 �Ƿ�����֮�仮�ߣ�0������1��
	/// ����  : bool dataend=false ����Ƿ����
	/// ����  : ���ñ���ı�����
	int SetTableTextData(_V_TABLE_DEFINE &td,int font=0,int fontsize=20,int fontsty=0,int align=0,int line=0,bool dataend=false);

	/// ������: SetTableTitle
	/// ���  : ������ 2008-8-19 14:47:45
	/// ����  : int =0�ɹ�
	/// ����  : _V_TABLE_DEFINE &td �ֶ���,length������Ҫ��100
	/// ����  : int font=0 
	/// ����  : int fontsize=20
	/// ����  : int fontsty=0 0�� 1���� 2 б�� 3 �»���
	/// ����  : int align=0 ���뷽ʽ 0�� 1�� 2��
	/// ����  : ���ñ��ͷ
	int SetTableTitle(_V_TABLE_DEFINE &td,int font=0,int fontsize=20,int fontsty=0,int align=0);

	/// ������: SetTitle
	/// ���  : ������ 2008-8-19 14:14:46
	/// ����  : int =0�ɹ�
	/// ����  : const char *title ����
	/// ����  : int ntitle ���ű��� 1-7 
	/// ����  : int font ���� 
	/// ����  : int fontsize �����С
	/// ����  : ���ñ�������
	int SetTitle(const char *title,int ntitle,int font,int fontsize);

	/// ������: SetContext
	/// ���  : ������ 2008-8-19 11:14:08
	/// ����  : int =0�ɹ�
	/// ����  : const char *context ����
	/// ����  : int font=0 ����
	/// ����  : int fontsize=20 �����С
	/// ����  : int align=0 ���뷽ʽ 0�� 1�� 2��
	/// ����  : �����ĵ�����
	int SetContext(const char *context,int font=0,int fontsize=20,int align=0);

	/// ������: SetFirstPage
	/// ���  : ������ 2008-8-19 11:00:21
	/// ����  : int =0�ɹ�
	/// ����  : const char *docname �ĵ�����
	/// ����  : const char *docauthor  �ĵ�����
	/// ����  : const char *docversion �ĵ��汾
	/// ����  : �����ĵ���һҳ
	int SetFirstPage(const char *docname,const char *docauthor,const char *docversion);

	/// ������: EndRtfFile
	/// ���  : ������ 2008-8-19 11:00:11
	/// ����  : void 
	/// ����  : ����rtf�ĵ�
	void EndRtfFile();

	/// ������: SetRtfHead
	/// ���  : ������ 2008-8-19 9:56:45
	/// ����  : int =0�ɹ�
	/// ����  : const char *leftheader ���ҳü
	/// ����  : const char *rightheader �ұ�ҳü
	/// ����  : const char *footer  ���ҳ��
	/// ����  : const char *centerfooter �м�ҳ��
	/// ����  : ����rtf�ļ���ҳüҳ�� ҳ���ұ߹̶�Ϊҳ��
	int SetRtfHead(const char *leftheader,const char *rightheader,const char *footer,const char *centerfooter);

	/// ������: SetRtfFileName
	/// ���  : ������ 2008-8-19 9:24:10
	/// ����  : int =0�ɹ�
	/// ����  : const char *filename
	/// ����  : ����rtfĿ���ļ���
	int SetRtfFileName(const char *filename);

	/// ������: InitRtf
	/// ���  : ������ 2008-8-19 9:12:24
	/// ����  : bool 
	/// ����  : const char *confile
	/// ����  : rtf��ʼ������ȡ������rtf���ã����ļ���
	bool InitRtf(const char *confile);

	char m_errMsg[256];
	CRtfGenerate();
	virtual ~CRtfGenerate();

protected:
	std::string m_rtfFile;///Ҫ���ɵ�rtf�ļ�
	bool m_bIsInit;///�Ƿ��ʼ����
	CXmlPack m_rtfconf;///rft����ģ���ļ�
	bool m_bIsFile;///�Ƿ������ļ���

	FILE *m_rtffp;///�ļ�ָ��
	int m_curTableFieldCount;///��ǰ�������
	_V_TABLE_DEFINE m_vTabelOffsetList;///��ǰ�������λ��


};
#undef DPTOOL_EXPORT
#endif /// !defined(AFX_RTFGENERATE_H__15D52AC5_2E78_4803_89E2_989FB9CE890F__INCLUDED_)
