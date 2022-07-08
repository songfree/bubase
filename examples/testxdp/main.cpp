// changedbpwd.cpp : Defines the entry point for the console application.
//
#include "public.h"
#include "Xdp.h"

#ifdef _WINDOWS
#ifdef _DEBUG
#pragma comment(lib, "bf_xdpd.lib") 
#pragma comment(lib, "bf_dbpubd.lib") 
#pragma comment(lib, "bf_kerneld.lib") 
#pragma message("Automatically linking with   bf_kerneld.lib bf_dbpubd.lib bf_xdpd.lib")
#else
#pragma comment(lib, "bf_xdp.lib") 
#pragma comment(lib, "bf_dbpubd.lib") 
#pragma comment(lib, "bf_kernel.lib") 
#pragma message("Automatically linking with   bf_kernel.lib bf_dbpub.lib bf_xdp.lib")
#endif
#endif

int main(int argc, char* argv[])
{

	char errmsg[256];

	errmsg[0] = 2;
	
	CXdp  xdp;
	if (!xdp.InitXdp("xdp.xml",errmsg))
	{
		printf("��ʼ������%s\n",errmsg);
		return -1;
	}
	int i,j;
	char databuf[1000];
	bzero(databuf,sizeof(databuf));
	xdp.ResetData();

	char xdpdata[8192];
	bzero(xdpdata,sizeof(xdpdata));
	xdp.PrintXdp(xdpdata);
	printf("xpddata: %s\n",xdpdata);
	if (!xdp.SetFieldValue(1,(unsigned short)12,errmsg))
	{
		return -1;
	}
	if (!xdp.SetFieldValue("uint4",(unsigned int)1,errmsg))
	{
		return -1;
	}
	if (!xdp.SetFieldValue("uint9",(unsigned int)2,errmsg))
	{
		return -1;
	}
	if (!xdp.SetFieldValue("uint1",(unsigned int)602,errmsg))
	{
		return -1;
	}
	if (!xdp.SetFieldValue("uint2",(unsigned int)1,errmsg))
	{
		return -1;
	}
	if (!xdp.SetFieldValue("uint5",(unsigned int)2,errmsg))
	{
		return -1;
	}
	if (!xdp.SetFieldValue("uint6",(unsigned int)2,errmsg))
	{
		return -1;
	}
	if (!xdp.SetFieldValue("uint7",(unsigned int)2,errmsg))
	{
		return -1;
	}
	if (!xdp.SetFieldValue("uint8",(unsigned int)2,errmsg))
	{
		return -1;
	}
	if (!xdp.SetFieldValue("uint9",(unsigned int)2,errmsg))
	{
		return -1;
	}
	if (!xdp.SetFieldValue("uint10",(unsigned int)2,errmsg))
	{
		return -1;
	}
	
	if (!xdp.SetFieldValue("int1",(int)2,errmsg))
	{
		return -1;
	}
	if (!xdp.SetFieldValue("int2",(int)2,errmsg))
	{
		return -1;
	}
	if (!xdp.SetFieldValue("int3",(int)2,errmsg))
	{
		return -1;
	}
	if (!xdp.SetFieldValue("int4",(int)2,errmsg))
	{
		return -1;
	}
	if (!xdp.SetFieldValue("int5",(int)2,errmsg))
	{
		return -1;
	}
	if (!xdp.SetFieldValue("int6",(int)2,errmsg))
	{
		return -1;
	}
	if (!xdp.SetFieldValue("int7",(int)2,errmsg))
	{
		return -1;
	}
	if (!xdp.SetFieldValue("int8",(int)2,errmsg))
	{
		return -1;
	}
	if (!xdp.SetFieldValue("int9",(int)2,errmsg))
	{
		return -1;
	}
	if (!xdp.SetFieldValue("int10",(int)2,errmsg))
	{
		return -1;
	}
    if (!xdp.SetFieldValue("int641", (UINT64_)2, errmsg))
    {
        return -1;
    }
    if (!xdp.SetFieldValue("int642", (UINT64_)2, errmsg))
    {
        return -1;
    }
    if (!xdp.SetFieldValue("int643", (UINT64_)2, errmsg))
    {
        return -1;
    }
    if (!xdp.SetFieldValue("int644", (UINT64_)2, errmsg))
    {
        return -1;
    }
    if (!xdp.SetFieldValue("int645", (UINT64_)2, errmsg))
    {
        return -1;
    }
    if (!xdp.SetFieldValue("int646", (UINT64_)2, errmsg))
    {
        return -1;
    }
    if (!xdp.SetFieldValue("int647", (UINT64_)2, errmsg))
    {
        return -1;
    }

	unsigned int len = sizeof(databuf);
	
	if (!xdp.ToBuffer(databuf,len,errmsg))
	{
		return -1;
	}

	for (i=1;i<10; i++)
	{
		if (!xdp.SetFieldValue((unsigned short)i,22,(unsigned int)602,errmsg))
		{
			return -1;
		}
		if (!xdp.SetFieldValue((unsigned short)i,23,(unsigned int)1,errmsg))
		{
			return -1;
		}
		if (!xdp.SetFieldValue((unsigned short)i,24,(unsigned int)2,errmsg))
		{
			return -1;
		}
		if (!xdp.SetFieldValue((unsigned short)i,20,(unsigned int)602,errmsg))
		{
			return -1;
		}
		if (!xdp.SetFieldValue((unsigned short)i,21,(unsigned int)1,errmsg))
		{
			return -1;
		}
		if (!xdp.SetFieldValue((unsigned short)i,24,(unsigned int)2,errmsg))
		{
			return -1;
		}
		if (!xdp.SetFieldValue((unsigned short)i,25,(unsigned int)2,errmsg))
		{
			return -1;
		}


	}
	if (!xdp.FromBuffer(databuf,len,errmsg))
	{
		printf("parse xdp error %s",errmsg);
		return -1;
	}
	

	
	
	int loopnum=1000000;
	INT64_ tbtime=CBF_Date_Time::GetTickUS();

	for (j=0; j < loopnum; j++)
	{
		len = sizeof(databuf);
		if (!xdp.ToBuffer(databuf,len,errmsg))
		{
			return -1;
		}
	}
	INT64_ tetime=CBF_Date_Time::GetTickUS();

	int totaltime= tetime-tbtime;
	printf("XDP ToBuffer �ֶ�31�����ݳ���[%d]  [%d]�κ�ʱ[%d]us��ƽ��[%.4f]us\n",len,loopnum,totaltime,1.00*totaltime/loopnum);


	SLEEP_SECONDS(5);
	if (!xdp.FromBuffer(databuf,len,errmsg))
	{
		printf("parse xdp error %s",errmsg);
		return -1;
	}
 	

	tbtime=CBF_Date_Time::GetTickUS();
	for (j=0; j < loopnum; j++)
	{
		
		if (!xdp.FromBuffer(databuf,len,errmsg))
		{
			return -1;
		}
	}
	tetime=CBF_Date_Time::GetTickUS();
	totaltime= tetime-tbtime;
	printf("XDP FromBuffer �ֶ�31�����ݳ���[%d] [%d]�κ�ʱ[%d]us��ƽ��[%.4f]us\n",len,loopnum,totaltime, 1.00*totaltime/loopnum);


	//
	//
	//FILE *fp1 = fopen("arbtest.dat","wb");
	//if (fp1 != NULL)
	//{
	//	fwrite(databuf,1,len,fp1);
	//	fclose(fp1);
	//}


	//
	//FILE *fp = fopen("b.txt","rb");
	//if (fp != NULL)
	//{
	//	int nr = fread(xdpdata,1,8000,fp);
	//	if (nr <1)
	//	{
	//		return -1;
	//	}
	//	fclose(fp);
	//	xdpdata[nr]=0;
	//	unsigned int len = nr;
	//	if (!xdp.FromBuffer(xdpdata,len,errmsg))
	//	{
	//		printf("parse xdp error %s",errmsg);
	//		return -1;
	//	}
	//	xdp.PrintXdp(xdpdata);
	//	printf("xpddata: %s",xdpdata);

	//}
	//



	
//����SetFieldValue
	char cdata[300];
	unsigned int datalen = 2;
	unsigned short *ushort = (unsigned short *)cdata;
	*ushort = 245;
	if (!xdp.SetFieldValue("ushort1",cdata,datalen,errmsg))
	{
		printf("������ֵ����\n");
		return -1;
	}
	
	unsigned int *uint = (unsigned int *)cdata;
	datalen = 4;
	*uint = 99988;
	if (!xdp.SetFieldValue("uint1",cdata,datalen,errmsg))
	{
		printf("������ֵ����\n");
		return -1;
	}

	double *dv = (double *)cdata;
	datalen = sizeof(double);
	*dv = 99.972;
	if (!xdp.SetFieldValue("double1",cdata,datalen,errmsg))
	{
		printf("������ֵ����\n");
		return -1;
	}
	strcpy(cdata,"88888666");
	datalen = strlen(cdata);
	if (!xdp.SetFieldValue("varchar1",cdata,datalen,errmsg))
	{
		printf("������ֵ����\n");
		return -1;
	}

	strcpy(cdata,"99999999999");
	datalen = strlen(cdata);
	if (!xdp.SetFieldValue("varchar2",cdata,datalen,errmsg))
	{
		printf("������ֵ����\n");
		return -1;
	}

	strcpy(cdata,"1111111111111111111111111");
	datalen = 20;
	if (!xdp.SetFieldValue("bindata1",cdata,datalen,errmsg))
	{
		printf("������ֵ����\n");
		return -1;
	}
	
//����SetFieldValue���¼
	ushort = (unsigned short *)cdata;
	*ushort = 2245;
	datalen = 2;
	if (!xdp.SetFieldValue(2,6,cdata,datalen,errmsg))
	{
		printf("������ֵ����\n");
		return -1;
	}
	*ushort = 2499;
	if (!xdp.SetFieldValue(2,"ushort1",cdata,datalen,errmsg))
	{
		printf("������ֵ����\n");
		return -1;
	}
	uint = (unsigned int *)cdata;
	datalen = 4;
	*uint = 299988;
	if (!xdp.SetFieldValue(2,"uint1",cdata,datalen,errmsg))
	{
		printf("������ֵ����\n");
		return -1;
	}
	
	
	dv = (double *)cdata;
	datalen = sizeof(double);
	*dv = 299.972;
	if (!xdp.SetFieldValue(2,"double1",cdata,datalen,errmsg))
	{
		printf("������ֵ����\n");
		return -1;
	}

	
	strcpy(cdata,"288888666");
	datalen = strlen(cdata);
	if (!xdp.SetFieldValue(2,"varchar1",cdata,datalen,errmsg))
	{
		printf("������ֵ����\n");
		return -1;
	}
	
	
	strcpy(cdata,"299999999999");
	datalen = strlen(cdata);
	if (!xdp.SetFieldValue(2,"varchar2",cdata,datalen,errmsg))
	{
		printf("������ֵ����\n");
		return -1;
	}
	
	strcpy(cdata,"21111111111111111111111111");
	datalen = 20;
	if (!xdp.SetFieldValue(2,"bindata1",cdata,datalen,errmsg))
	{
		printf("������ֵ����\n");
		return -1;
	}
	



	char xdpbuf[8192];
	bzero(xdpbuf,sizeof(xdpbuf));

//���Դ�ӡ��������
	xdp.PrintXdp(xdpbuf);
	printf("����������¼���xdp��������:\n%s",xdpbuf);
//���Ի�ȡ���Ĵ�С
	datalen = xdp.GetRealPackSize();
	printf("GetRealPackSize�õ��ı��Ĵ�С[%d]\n",datalen);
	
//�������ɱ���
	datalen = sizeof(xdpbuf);
	if (!xdp.ToBuffer(xdpbuf,datalen,errmsg))
	{
		printf("ToBuffer����\n");
		return -1;
	}
	printf("ToBuffer�õ��ı��Ĵ�С[%d]\n",datalen);
//���Դӻ����������
	unsigned int xdpbuflen = datalen; 
	if (!xdp.FromBuffer(xdpbuf,datalen,errmsg))
	{
		printf("FromBuffer���� %s\n",errmsg);
		return false;
	}
	xdp.PrintXdp(xdpbuf);
	printf("FromBuffer���xdp��������:\n%s",xdpbuf);
	datalen = xdp.GetRealPackSize();
	printf("FromBuffer�� GetRealPackSize�õ��ı��Ĵ�С[%d]\n",datalen);

//����GetFieldValue
	printf("����GetFieldValue:\n");
	datalen = sizeof(cdata);
	int ftype=XDP_UNKNOW;
	if (!xdp.GetFieldValue("ushort1",cdata,datalen,ftype,errmsg))
	{
		printf("ȡ�ֶ�ushort1���� %s\n",errmsg);
		return false;
	}
	unsigned short *us = (unsigned short *)cdata;
	printf("rec=1  ushort1=%d\n",*us);

	datalen = sizeof(cdata);
	if (!xdp.GetFieldValue("uint1",cdata,datalen,ftype,errmsg))
	{
		printf("ȡ�ֶ�uint1���� %s\n",errmsg);
		return false;
	}
	unsigned int *ui = (unsigned int *)cdata;
	printf("rec=1 uint1=%d\n",*ui);

	datalen = sizeof(cdata);
	if (!xdp.GetFieldValue("double1",cdata,datalen,ftype,errmsg))
	{
		printf("ȡ�ֶ�double1���� %s\n",errmsg);
		return false;
	}
	dv = (double *)cdata;
	printf("rec=1 double1=%f\n",*dv);


	datalen = sizeof(cdata);
	if (!xdp.GetFieldValue("varchar1",cdata,datalen,ftype,errmsg))
	{
		printf("ȡ�ֶ�varchar1���� %s\n",errmsg);
		return false;
	}
	cdata[datalen]=0;
	printf("rec=1 varchar1=%s\n",cdata);

	datalen = sizeof(cdata);
	if (!xdp.GetFieldValue("varchar2",cdata,datalen,ftype,errmsg))
	{
		printf("ȡ�ֶ�varchar2���� %s\n",errmsg);
		return false;
	}
	cdata[datalen]=0;
	printf("rec=1 varchar2=%s\n",cdata);

	datalen = sizeof(cdata);
	if (!xdp.GetFieldValue("bindata1",cdata,datalen,ftype,errmsg))
	{
		printf("ȡ�ֶ�bindata1���� %s\n",errmsg);
		return false;
	}
	cdata[datalen]=0;
	//���ǲ��ɼ�����򲻳���
	printf("rec=1 bindata1=%s\n",cdata);
	
	for (int k=1; k<xdp.GetRecCount(); k++)
	{
		datalen = sizeof(cdata);
		int ftype=XDP_UNKNOW;
		if (!xdp.GetFieldValue(k+1,"ushort1",cdata,datalen,ftype,errmsg))
		{
			printf("ȡ�ֶ�ushort1���� %s\n",errmsg);
			return false;
		}
		unsigned short *us = (unsigned short *)cdata;
		printf("rec=%d ushort1=%d\n",k+1,*us);
		
		datalen = sizeof(cdata);
		if (!xdp.GetFieldValue(k+1,"uint1",cdata,datalen,ftype,errmsg))
		{
			printf("ȡ�ֶ�uint1���� %s\n",errmsg);
			return false;
		}
		unsigned int *ui = (unsigned int *)cdata;
		printf("rec=%d uint1=%d\n",k+1,*ui);
		
		datalen = sizeof(cdata);
		if (!xdp.GetFieldValue(k+1,"double1",cdata,datalen,ftype,errmsg))
		{
			printf("ȡ�ֶ�double1���� %s\n",errmsg);
			return false;
		}
		dv = (double *)cdata;
		printf("rec=%d  double1=%f\n",k+1,*dv);
		
		
		datalen = sizeof(cdata);
		if (!xdp.GetFieldValue(k+1,"varchar1",cdata,datalen,ftype,errmsg))
		{
			printf("ȡ�ֶ�varchar1���� %s\n",errmsg);
			return false;
		}
		cdata[datalen]=0;
		printf("rec=%d  varchar1=%s\n",k+1,cdata);
		
		datalen = sizeof(cdata);
		if (!xdp.GetFieldValue(k+1,"varchar2",cdata,datalen,ftype,errmsg))
		{
			printf("ȡ�ֶ�varchar2���� %s\n",errmsg);
			return false;
		}
		cdata[datalen]=0;
		printf("rec=%d  varchar2=%s\n",k+1,cdata);
		
		datalen = sizeof(cdata);
		if (!xdp.GetFieldValue(k+1,"bindata1",cdata,datalen,ftype,errmsg))
		{
			printf("ȡ�ֶ�bindata1���� %s\n",errmsg);
			return false;
		}
		cdata[datalen]=0;
		//���ǲ��ɼ�����򲻳���
		printf("rec=%d  bindata1=%s\n",k+1,cdata);
	}
	printf("����GetFieldValue����\n");

	datalen = sizeof(xdpbuf);
	bzero(xdpbuf,datalen);
	//�������ɱ���
	if (!xdp.ToBuffer(xdpbuf,datalen,errmsg))
	{
		printf("ToBuffer����\n");
		return -1;
	}
	printf("xdp ToBuffer�õ��ı��Ĵ�С[%d]\n",datalen);
	char xdp2buf[8192];
	memcpy(xdp2buf,xdpbuf,datalen);
	xdp2buf[datalen]=0;
	unsigned int xdp2len = datalen;

	xdp.PrintXdp(xdpbuf);
	printf("ToBuffer��xdp2ǰ��xdp1��������:\n%s",xdpbuf);

	//����һ����������
	CXdp  xdp2;
	if (!xdp2.InitXdp("xdp.xml",errmsg))
	{
		printf("��ʼ������%s\n");
		return -1;
	}
	

	if (!xdp2.FromBuffer(xdp2buf,xdp2len,errmsg))
	{
		printf("FromBuffer���� %s\n",errmsg);
		return false;
	}
	//�������ɱ���

	xdp.PrintXdp(xdpbuf);
	printf("xdp2 FromBuffer���xdp1��������:\n%s",xdpbuf);



	datalen = sizeof(cdata);
	ftype=XDP_UNKNOW;
	if (!xdp2.GetFieldValue("ushort1",cdata,datalen,ftype,errmsg))
	{
		printf("ȡ�ֶ�ushort1���� %s\n",errmsg);
		return false;
	}
	us = (unsigned short *)cdata;
	printf("ushort1=%d\n",*us);
	
	datalen = sizeof(cdata);
	if (!xdp2.GetFieldValue("uint1",cdata,datalen,ftype,errmsg))
	{
		printf("ȡ�ֶ�uint1���� %s\n",errmsg);
		return false;
	}
	ui = (unsigned int *)cdata;
	printf("uint1=%d\n",*ui);
	
	datalen = sizeof(cdata);
	if (!xdp2.GetFieldValue("double1",cdata,datalen,ftype,errmsg))
	{
		printf("ȡ�ֶ�double1���� %s\n",errmsg);
		return false;
	}
	dv = (double *)cdata;
	printf("double1=%f\n",*dv);
	
	
	datalen = sizeof(cdata);
	if (!xdp2.GetFieldValue("varchar1",cdata,datalen,ftype,errmsg))
	{
		printf("ȡ�ֶ�varchar1���� %s\n",errmsg);
		return false;
	}
	cdata[datalen]=0;
	printf("varchar1=%s\n",cdata);
	
	datalen = sizeof(cdata);
	if (!xdp2.GetFieldValue("varchar2",cdata,datalen,ftype,errmsg))
	{
		printf("ȡ�ֶ�varchar2���� %s\n",errmsg);
		return false;
	}
	cdata[datalen]=0;
	printf("varchar2=%s\n",cdata);
	
	datalen = sizeof(cdata);
	if (!xdp2.GetFieldValue("bindata1",cdata,datalen,ftype,errmsg))
	{
		printf("ȡ�ֶ�bindata1���� %s\n",errmsg);
		return false;
	}
	cdata[datalen]=0;
	//���ǲ��ɼ�����򲻳���
	printf("bindata1=%s\n",cdata);


	xdp.PrintXdp(xdpbuf);
	printf("xdp1��������:\n%s",xdpbuf);

	xdp2.PrintXdp(xdpbuf);
	printf("xdp2��������:\n%s",xdpbuf);


	uint = (unsigned int *)cdata;
	datalen = 4;
	*uint = 123456789;
	if (!xdp.SetFieldValue("uint10",cdata,datalen,errmsg))
	{
		printf("������ֵ����\n");
		return -1;
	}

	if (!xdp2.XdpCopy(&xdp))
	{
		printf("XDP���Ƴ���\n");
		return -1;
	}

	xdp2.PrintXdp(xdpbuf);
	printf("xdp2��������:\n%s",xdpbuf);
	return 0;
}
