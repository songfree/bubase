//testclientapi.cpp : Defines the entry point for the console application.

#include <stdio.h>
#include "public.h"
#include "XmlPack.h"
#include "BF_Date_Time.h"


void testcopy(const char *xmlfile,int testnum)
{
	CXmlPack xmlpack;  
	CXmlPack xmldest;
	CBF_Date_Time publog;
	struct_stat statfile;
	int lasttime;
	int curtime;
	int usetime;
	int i,j;
	
	
	FSTAT(xmlfile,&statfile);
	if (xmlpack.m_xml.fromFile((char *)xmlfile)<0)
	{
		printf("从文件[%s]FromFile失败\n",xmlfile);
		return ;
	}
	printf("测试文件:%-20s %8d字节%8.2fK\n",xmlfile,statfile.st_size,1.00*statfile.st_size/1024);
	xmldest.m_xml.XmlCopy(&xmlpack.m_xml);
	xmldest.toFile("tmp3.xml");
	lasttime = publog.GetTickCount();
	for (i=0;i<testnum;i++)
	{
		xmldest.m_xml.XmlCopy(&xmlpack.m_xml);
	}
	curtime = publog.GetTickCount();
	usetime = curtime-lasttime;
	
	printf("复制XML   %8d次时间%8d毫秒,平均%8.4f毫秒\n",testnum,usetime,1.00*usetime/testnum);
}


void testxml(const char *xmlfile,char *xmlbuf,int xmlbuflen,int testnum)
{
	CXmlPack xmlpack;  
	CBF_Date_Time publog;
	struct_stat statfile;
	int lasttime;
	int curtime;
	int usetime;
	int i,j;
	int buflen=xmlbuflen;

	FSTAT(xmlfile,&statfile);
	lasttime = publog.GetTickCount();
	if (xmlpack.m_xml.fromFile((char *)xmlfile)<0)
	{
		printf("从文件[%s]FromFile失败\n",xmlfile);
		return ;
	}
	printf("测试文件:%-20s %8d字节%8.2fK  解析时间%8d毫秒\n",xmlfile,statfile.st_size,1.00*statfile.st_size/1024,publog.GetTickCount()-lasttime);
	buflen=xmlbuflen;
	if (!xmlpack.m_xml.toBuffer(xmlbuf,buflen))
	{
		printf("ToBuffer失败\n");
		return ;
	}
	//开始dom测试 
	lasttime = publog.GetTickCount();
	for (j=0;j<testnum;j++)
	{
		if (!xmlpack.m_xml.fromBuffer(xmlbuf))
		{
			printf("FromBuffer解析出错\n");
			return ;
		}
	}
	curtime = publog.GetTickCount();
	usetime = curtime-lasttime;
	printf("缓冲%7d字节%6.2fK DOM模式 %5d次解析时间%8d毫秒，平均%8.4f毫秒\n",buflen,1.00*buflen/1024,testnum,usetime,1.00*usetime/testnum);
	

}


int main(int argc, char *argv[])
{
	char *buf = (char *)malloc(1024*1024*10) ;
	int buflen;
	memset(buf,0,sizeof(1024*1024*10));
	buflen =  1024*1024*10;


	

	testcopy("测试3.xml",10000);


	testxml("测试1.xml",buf,buflen,10000);

	testxml("测试2.xml",buf,buflen,10000);

	testxml("测试3.xml",buf,buflen,10000);

	testxml("测试4.xml",buf,buflen,1000);

	testxml("测试5.xml",buf,buflen,100);

	testxml("测试6.xml",buf,buflen,10);

	free(buf);
	return 0;
}