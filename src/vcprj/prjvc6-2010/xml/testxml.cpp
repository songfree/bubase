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
		printf("���ļ�[%s]FromFileʧ��\n",xmlfile);
		return ;
	}
	printf("�����ļ�:%-20s %8d�ֽ�%8.2fK\n",xmlfile,statfile.st_size,1.00*statfile.st_size/1024);
	xmldest.m_xml.XmlCopy(&xmlpack.m_xml);
	xmldest.toFile("tmp3.xml");
	lasttime = publog.GetTickCount();
	for (i=0;i<testnum;i++)
	{
		xmldest.m_xml.XmlCopy(&xmlpack.m_xml);
	}
	curtime = publog.GetTickCount();
	usetime = curtime-lasttime;
	
	printf("����XML   %8d��ʱ��%8d����,ƽ��%8.4f����\n",testnum,usetime,1.00*usetime/testnum);
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
		printf("���ļ�[%s]FromFileʧ��\n",xmlfile);
		return ;
	}
	printf("�����ļ�:%-20s %8d�ֽ�%8.2fK  ����ʱ��%8d����\n",xmlfile,statfile.st_size,1.00*statfile.st_size/1024,publog.GetTickCount()-lasttime);
	buflen=xmlbuflen;
	if (!xmlpack.m_xml.toBuffer(xmlbuf,buflen))
	{
		printf("ToBufferʧ��\n");
		return ;
	}
	//��ʼdom���� 
	lasttime = publog.GetTickCount();
	for (j=0;j<testnum;j++)
	{
		if (!xmlpack.m_xml.fromBuffer(xmlbuf))
		{
			printf("FromBuffer��������\n");
			return ;
		}
	}
	curtime = publog.GetTickCount();
	usetime = curtime-lasttime;
	printf("����%7d�ֽ�%6.2fK DOMģʽ %5d�ν���ʱ��%8d���룬ƽ��%8.4f����\n",buflen,1.00*buflen/1024,testnum,usetime,1.00*usetime/testnum);
	

}


int main(int argc, char *argv[])
{
	char *buf = (char *)malloc(1024*1024*10) ;
	int buflen;
	memset(buf,0,sizeof(1024*1024*10));
	buflen =  1024*1024*10;


	

	testcopy("����3.xml",10000);


	testxml("����1.xml",buf,buflen,10000);

	testxml("����2.xml",buf,buflen,10000);

	testxml("����3.xml",buf,buflen,10000);

	testxml("����4.xml",buf,buflen,1000);

	testxml("����5.xml",buf,buflen,100);

	testxml("����6.xml",buf,buflen,10);

	free(buf);
	return 0;
}