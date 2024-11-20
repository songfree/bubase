// RtfGenerate.cpp: implementation of the CRtfGenerate class.
//
//////////////////////////////////////////////////////////////////////

#include "RtfGenerate.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRtfGenerate::CRtfGenerate()
{
	m_bIsInit = false;
	m_bIsFile = false;
	m_rtffp = NULL;
	m_curTableFieldCount = 0;
	memset(m_errMsg,0,sizeof(m_errMsg));
}

CRtfGenerate::~CRtfGenerate()
{
	if (m_rtffp != NULL)
	{
		EndRtfFile();
	}
}

bool CRtfGenerate::InitRtf(const char *confile)
{
	if (m_rtfconf.fromFile((char *)confile)<0)
	{
		sprintf(m_errMsg,"配置文件[%s]非xmlpack",confile);
		m_bIsInit = false;
		return false;
	}
	std::string ss;
	if (m_rtfconf.getHeadValue("header",ss,false)<0)
	{
		sprintf(m_errMsg,"配置文件无header节定义");
		m_bIsInit = false;
		return false;
	}
	if (m_rtfconf.getHeadValue("firstpage",ss,false)<0)
	{
		sprintf(m_errMsg,"配置文件无firstpage节定义");
		m_bIsInit = false;
		return false;
	}
	if (m_rtfconf.getHeadValue("context",ss,false)<0)
	{
		sprintf(m_errMsg,"配置文件无context节");
		m_bIsInit = false;
		return -1;
	}
	m_bIsInit = true;
	return true;
}

int CRtfGenerate::SetRtfFileName(const char *filename)
{
	
	if (strlen(filename)<1)
	{
		sprintf(m_errMsg,"文件名不能为空");
		m_bIsFile = false;
		return -1;
	}
	if (m_rtffp != NULL)
	{
		fclose(m_rtffp);
		m_rtffp = NULL;
	}
	m_rtffp = fopen(filename,"w");
	if (m_rtffp == NULL)
	{
		sprintf(m_errMsg,"生成文件[%s]失败",filename);
		m_bIsFile = false;
		return -1;
	}
	m_rtfFile = filename;
	m_bIsFile = true;
	return 0;
}

int CRtfGenerate::SetRtfHead(const char *leftheader, const char *rightheader, const char *footer,const char *centerfooter)
{
	if (!m_bIsInit)
	{
		sprintf(m_errMsg,"未初始化");
		return -1;
	}
	int ret;
	std::string headfmt;
	if (m_rtfconf.getHeadValue("header",headfmt,false)<0)
	{
		sprintf(m_errMsg,"配置文件无header节");
		return -1;
	}
	if (m_rtffp == NULL)
	{
		sprintf(m_errMsg,"RTF文件未打开");
		return -1;
	}
	fprintf(m_rtffp,headfmt.c_str(),leftheader,rightheader,footer,centerfooter);
	fprintf(m_rtffp,"\n");

	return 0;
}

void CRtfGenerate::EndRtfFile()
{
	fprintf(m_rtffp,"}");
	fclose(m_rtffp);
	m_rtffp = NULL;
}

int CRtfGenerate::SetFirstPage(const char *docname, const char *docauthor, const char *docversion)
{
	if (!m_bIsInit)
	{
		sprintf(m_errMsg,"未初始化");
		return -1;
	}
	int ret;
	std::string firstfmt;
	if (m_rtfconf.getHeadValue("firstpage",firstfmt,false)<0)
	{
		sprintf(m_errMsg,"配置文件无firstpage节");
		return -1;
	}
	if (m_rtffp == NULL)
	{
		sprintf(m_errMsg,"RTF文件未打开");
		return -1;
	}
	fprintf(m_rtffp,firstfmt.c_str(),docname,docauthor,docversion);
	fprintf(m_rtffp,"\n");
	
	return 0;
}

int CRtfGenerate::SetContext(const char *context,int font,int fontsize,int align)
{
	if (!m_bIsInit)
	{
		sprintf(m_errMsg,"未初始化");
		return -1;
	}
	int ret;
	std::string firstfmt;
	if (m_rtfconf.getHeadValue("context",firstfmt,false)<0)
	{
		sprintf(m_errMsg,"配置文件无context节");
		return -1;
	}
	if (m_rtffp == NULL)
	{
		sprintf(m_errMsg,"RTF文件未打开");
		return -1;
	}
	if (align == 0)
	{
		fprintf(m_rtffp,firstfmt.c_str(),"l",font,fontsize,context);
	}
	else if (align == 1)
	{
		fprintf(m_rtffp,firstfmt.c_str(),"c",font,fontsize,context);
	}
	else if (align == 2)
	{
		fprintf(m_rtffp,firstfmt.c_str(),"r",font,fontsize,context);
	}
	fprintf(m_rtffp,"\n");
	return 0;
}

int CRtfGenerate::SetTitle(const char *title, int ntitle, int font, int fontsize)
{
	if (!m_bIsInit)
	{
		sprintf(m_errMsg,"未初始化");
		return -1;
	}
	int ret;
	std::string ss;
	if (m_rtfconf.getHeadValue("title",ss,false)<0)
	{
		sprintf(m_errMsg,"配置文件无context节");
		return -1;
	}
	if (m_rtffp == NULL)
	{
		sprintf(m_errMsg,"RTF文件未打开");
		return -1;
	}
	std::string stitle;
	switch (ntitle)
	{
		case 0:
			stitle="";
			ntitle = 1;
			break;
		case 1:
			stitle= "1";
			break;
		case 2:
			stitle="1.1";
			break;
		case 3:
			stitle="1.1.1";
			break;
		case 4:
			stitle="1.1.1.1";
			break;
		case 5:
			stitle="1.1.1.1.1";
			break;	
		case 6:
			stitle="1.1.1.1.1.1";
			break;	
		default:
			stitle="";
			ntitle = 1;
			break;
	}
	fprintf(m_rtffp,ss.c_str(),stitle.c_str(),ntitle-1,font,fontsize,title);
	fprintf(m_rtffp,"\n");
	return 0;
}

int CRtfGenerate::SetTableTitle(_V_TABLE_DEFINE &td, int font, int fontsize, int fontsty, int align)
{
	if (!m_bIsInit)
	{
		sprintf(m_errMsg,"未初始化");
		m_curTableFieldCount = 0;
		return -1;
	}
	int ret;
	std::string ss;
	if (m_rtfconf.getHeadValue("title",ss,false)<0)
	{
		sprintf(m_errMsg,"配置文件无context节");
		m_curTableFieldCount = 0;
		return -1;
	}
	if (m_rtffp == NULL)
	{
		sprintf(m_errMsg,"RTF文件未打开");
		m_curTableFieldCount = 0;
		return -1;
	}
	fprintf(m_rtffp,"\n\\trhdr\n\n");
	fprintf(m_rtffp,"\\trowd\\trgaph113\\trleft113\n");
	int rec = td.size();
	int alloffset=0;
	int offset=0;
	int i;
	for (i=0;i<rec;i++)
	{
		td[i].offset = (td[i].length+alloffset)*(10800/100);
		alloffset +=td[i].length;
	}
	if (alloffset != 100)
	{
		sprintf(m_errMsg,"表格字段的宽度比例不为100%");
		m_curTableFieldCount = 0;
		return -1;
	}
	//将表格信息保存在成员变量中
	m_vTabelOffsetList.clear();
	for (i=0;i<rec;i++)
	{
		m_vTabelOffsetList.push_back(td[i]);
	}
	m_curTableFieldCount = rec;
	for (i=0;i<rec;i++)
	{
		if (i == rec-1)
		{
			fprintf(m_rtffp,"\\clbrdrt\\brdrs\\brdrw15 \\clbrdrb\\brdrs\\brdrw15 \\clbrdrl\\brdrs\\brdrw15 \\clbrdrr\\brdrs\\brdrw15 \\cellx%d\n\n",td[i].offset);
		}
		else
		{
			fprintf(m_rtffp,"\\clbrdrt\\brdrs\\brdrw15 \\clbrdrb\\brdrs\\brdrw15 \\clbrdrl\\brdrs\\brdrw15  \\cellx%d\n",td[i].offset);
		}
	}
	std::string salign;
	if (align == 0)
	{
		salign = "l";
	}
	else if (align == 1)
	{
		salign = "c";
	}
	else if (align == 2)
	{
		salign = "r";
	}
	else
	{
		salign = "l";
	}
	//1粗体 2 斜体 3 下划线
	std::string sfontsty;
	if (fontsty == 1)
	{
		sfontsty = "\\b";
	}
	else if (fontsty == 2)
	{
		sfontsty = "\\i";
	}
	else if (fontsty == 3)
	{
		sfontsty = "\\u";
	}
	for (i=0;i<rec;i++)
	{
		fprintf(m_rtffp,"\\pard\\plain\n");
		fprintf(m_rtffp,"\\s0\\q%s\\sl240\\slmult1\\intbl\\sb20\\sa20\\li80\\ri80\n",salign.c_str());
		fprintf(m_rtffp,"%s\\f%d\\fs%d\n",sfontsty.c_str(),font,fontsize);
		if (i == rec-1)
		{
			fprintf(m_rtffp,"%s\\cell\\pard\\intbl\\row\n",td[i].name.c_str());
		}
		else
		{
			fprintf(m_rtffp,"%s\\cell\n",td[i].name.c_str());
		}
		
	}
	

	return 0;
}

int CRtfGenerate::SetTableTextData(_V_TABLE_DEFINE &td, int font, int fontsize, int fontsty, int align,int line,bool dataend)
{
	if (m_curTableFieldCount <0)
	{
		sprintf(m_errMsg,"您没有设置表格头");
		return -1;
	}
	if (td.size() != m_curTableFieldCount)
	{
		sprintf(m_errMsg,"传入参数的列数和设置表格列数不符");
		return -1;
	}
	if (!m_bIsInit)
	{
		sprintf(m_errMsg,"未初始化");
		m_curTableFieldCount = 0;
		return -1;
	}
	int ret;
	std::string ss;
	if (m_rtfconf.getHeadValue("title",ss,false)<0)
	{
		sprintf(m_errMsg,"配置文件无context节");
		m_curTableFieldCount = 0;
		return -1;
	}
	if (m_rtffp == NULL)
	{
		sprintf(m_errMsg,"RTF文件未打开");
		m_curTableFieldCount = 0;
		return -1;
	}
	fprintf(m_rtffp,"\n\\trowd\\trgaph113\\trleft113\n");
	int rec = td.size();
	int alloffset=0;
	int offset=0;
	int i;
	
	//\brdrnil无边框 \clbrdrb下边框 \clbrdrt上边框 \clbrdrl左边框 \clbrdrr右边框。
	rec = m_vTabelOffsetList.size();
	for (i=0;i<rec;i++)
	{

		if (i == rec-1)
		{
			if (line == 1)
			{
				fprintf(m_rtffp,"\\clbrdrt\\brdrs\\brdrw15 \\clbrdrb\\brdrs\\brdrw15 \\clbrdrl\\brdrs\\brdrw15 \\clbrdrr\\brdrs\\brdrw15 \\cellx%d\n\n",m_vTabelOffsetList[i].offset);
			}
			else
			{
				if (dataend)
				{
					fprintf(m_rtffp,"\\clbrdrb\\brdrs\\brdrw15 \\clbrdrl\\brdrs\\brdrw15 \\clbrdrr\\brdrs\\brdrw15 \\cellx%d\n\n",m_vTabelOffsetList[i].offset);
				}
				else
				{
					fprintf(m_rtffp,"\\clbrdrl\\brdrs\\brdrw15 \\clbrdrr\\brdrs\\brdrw15 \\cellx%d\n\n",m_vTabelOffsetList[i].offset);
				}
			}
		}
		else
		{
			if (line == 1)
			{
				fprintf(m_rtffp,"\\clbrdrt\\brdrs\\brdrw15 \\clbrdrb\\brdrs\\brdrw15 \\clbrdrl\\brdrs\\brdrw15 \\cellx%d\n",m_vTabelOffsetList[i].offset);
			}
			else
			{
				if (dataend)
				{
					fprintf(m_rtffp,"\\clbrdrb\\brdrs\\brdrw15 \\clbrdrl\\brdrs\\brdrw15 \\clbrdrr\\brdrs\\brdrw15 \\cellx%d\n",m_vTabelOffsetList[i].offset);
				}
				else
				{
					fprintf(m_rtffp,"\\clbrdrl\\brdrs\\brdrw15  \\cellx%d\n",m_vTabelOffsetList[i].offset);
				}
			}
		}
	}
	std::string salign;
	if (align == 0)
	{
		salign = "l";
	}
	else if (align == 1)
	{
		salign = "c";
	}
	else if (align == 2)
	{
		salign = "r";
	}
	else
	{
		salign = "l";
	}
	//1粗体 2 斜体 3 下划线
	std::string sfontsty;
	if (fontsty == 1)
	{
		sfontsty = "\\b";
	}
	else if (fontsty == 2)
	{
		sfontsty = "\\i";
	}
	else if (fontsty == 3)
	{
		sfontsty = "\\u";
	}
	for (i=0;i<rec;i++)
	{
		fprintf(m_rtffp,"\\pard\\plain\n");
		fprintf(m_rtffp,"\\s0\\q%s\\sl240\\slmult1\\intbl\\sb20\\sa20\\li80\\ri80\n",salign.c_str());
		fprintf(m_rtffp,"%s\\f%d\\fs%d\n",sfontsty.c_str(),font,fontsize);
		if (i == rec-1)
		{
			fprintf(m_rtffp,"%s\\cell\\pard\\intbl\\row\n\n",td[i].data.c_str());
		}
		else
		{
			fprintf(m_rtffp,"%s\\cell\n",td[i].data.c_str());
		}
		
	}
	//fprintf(m_rtffp,"\\%s\n","sect");
	return 0;
}
