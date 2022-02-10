
#include "GtpData.h"
#include "GtpProtocol.h"


bool GetGtpValue(const char *gtpbuffer,const char *key,char *cvalue)
{
	int ngtplen = strlen(gtpbuffer);
	int kelen = strlen(key);
	bool isfind=false;
	bool isfindkey=false;
	int  valuebegin=0;
	
	int keybegin=0;
	int keyend=0;
	int keyoffset=0;
	for (int i=0 ; i< ngtplen; i++)
	{
		if (!isfind) //���ҵ�һ���ַ�
		{
			if (gtpbuffer[i] == key[keyoffset])
			{
				isfind = true;
				keyoffset++;
				keybegin = i;
				if (keyoffset == kelen) //�ҵ�key�ˣ�����ֻ��һ���ַ���KEY
				{
					if (gtpbuffer[i+1] != '=') //����Ĳ���=������key
					{
						isfind = false;
						keyoffset =0;
					}
					else
					{
						isfindkey = true;
						valuebegin = i+2; //ֵ�����￪ʼ
						break;
					}
					
				}
			}
			else
			{
				isfind = false;
				keyoffset =0;
			}
		}
		else  //���Һ���
		{
			if (gtpbuffer[i] == key[keyoffset])
			{
				isfind = true;
				keyoffset++;
				if (keyoffset == kelen) //�ҵ�key��
				{
					if (gtpbuffer[i+1] != '=') //����Ĳ���=������key
					{
						isfind = false;
						keyoffset =0;
					}
					else
					{
						isfindkey = true;
						valuebegin = i+2; //ֵ�����￪ʼ
						break;
					}
					
				}
			}
			else
			{
				isfind = false;
				keyoffset =0;
			}
		}
	}
	if (!isfind || !isfindkey) //û���ҵ�
	{
		return false;
	}
	int valueend=valuebegin;
	bool isend=false;
	for (int j=valuebegin; j<ngtplen ; j++)
	{
		if (gtpbuffer[j] == ',')
		{
			valueend = j;
			isend = true;
			break;
		}
	}
	if (!isend)  //û�ҵ�','��˵�������һ��
	{
		valueend = ngtplen;
	}
	memcpy(cvalue,gtpbuffer+valuebegin,valueend -valuebegin);
	cvalue[valueend -valuebegin]=0;
	return true;
}

int main(int argc,char *argv[])
{

	CGtpData  gtp;
	bool bret = gtp.FromFile("q012.txt");
	int fieldtype;
	std::string sname;
	std::string svalue;
	gtp.GetValueByName("I00",sname,fieldtype);
	printf("�г�[%s] ",sname.c_str());
	CGtpData *pgtp = gtp.GetDataPoint("Q52");
	int ret = pgtp->FieldCount();
	if (ret <1)
	{
		printf("Q52����arraylist\n");
		return -1;
	}
	std::string sqdate;
	std::string sqtime;
	for (int i=0; i<ret ;i++)
	{
		//ֱ����ȡhash��ʽȡֵ
		//pgtp->GetHashValue(i,"I10",sname,fieldtype);
		//pgtp->GetHashValue(i,"I11",svalue,fieldtype);
		//printf(" %s %s \n",sname.c_str(),svalue.c_str());
		//�ô�hash����Ŷ�Ӧ�Ľڵ���ȡֵ
		CGtpData *pgtp2 = pgtp->GetDataPoint(i);
		pgtp2->GetValueByName("I10",sname,fieldtype);
		pgtp2->GetValueByName("I11",svalue,fieldtype);
		pgtp2->GetValueByName("Q11",sqdate,fieldtype);
		pgtp2->GetValueByName("Q12",sqtime,fieldtype);
		printf("��Լ[%s] ����[%s] ��������[%s] ����ʱ��[%s]",sname.c_str(),svalue.c_str(),sqdate.c_str(),sqtime.c_str());

		CGtpData *pgtp3 = pgtp2->GetDataPoint("Q00");
		int ret2 = pgtp3->FieldCount();
		double dv1,dv2;
		int nv1,nv2;
		for (int j=0; j<ret2 ;j++)
		{
			pgtp3->GetHashValue(j,"Q21",dv1,fieldtype);
			pgtp3->GetHashValue(j,"Q22",nv1,fieldtype);
			pgtp3->GetHashValue(j,"Q23",dv2,fieldtype);
			pgtp3->GetHashValue(j,"Q24",nv2,fieldtype);
			printf(" ���%d %.2f ����%d %d ����%d %.2f ����%d %d\n",j,dv1,j,nv1,j,dv2,j,nv2);
		}
	}
	

	char buffer[8192];
	int buflen = 8192;
	gtp.ToBuffer(buffer,buflen);
	buffer[buflen]=0;

	char cvalue[1000];
	bzero(cvalue,sizeof(cvalue));
	GetGtpValue(buffer,"X05",cvalue);

	GetGtpValue(buffer,"O63",cvalue);

	for (int ii=0; ii<20;ii++)
	{
		
		
		gtp.FromFile("B062_00.txt");
	}

	return 0;


	CGtpProtocol pData;

	pData.NewGtp(1,1);


	CGtpData *data = pData.GetGtpData();
	std::string tmpstr;
	char gtpdata[8192];
	bzero(gtpdata,sizeof(gtpdata));
	int datalen = sizeof(gtpdata);
	//std::string  ss = "a=1,b=2,c=3,hash={x1=[{p1=2,p2=2,p3=3},{p1=2,p2=2,p3=3},{p1=2,p2=2,p3=3}],x2=3,x3=4},d=4,array=[{x1=1,x2=2,x3=3},{x11=1,x21=2,x31=3}],e=5";
	std::string  ss = "a=1,b=2,c=3,hash={x1=[{p1=2,p2=2,p3=3},{p1=2,p2=2,p3=3},{p1=2,p2=2,p3=3}],x2=3,x3=4},d=4,array=[{x1=1,x2=2,x3=3},{x1=1,x2=2,x3=[{xp=1,xp2=2},{xp=10,xp2=20}]}],e=5";
	bool bRet = data->FromBuffer(ss.c_str(),1);
	printf("ԭʼ���� data:%s\n",ss.c_str());
	if (bRet)
	{
		bRet = data->ToBuffer(gtpdata,datalen);
		if (!bRet)
		{
			printf("ToBuffer error");
		}
		else
		{
			printf("ToBuffer data:%s\n",gtpdata);
		}
		
		bRet = data->ToString(tmpstr);
		if (!bRet)
		{
			printf("ToString error");
		}
		else
		{
			printf("ToString data:%s\n",tmpstr.c_str());
		}
		bRet = data->ToFile("gtptest.txt");
		if (!bRet)
		{
			printf("ToFile error");
		}
		else
		{
			printf("ToFile : gtptest.txt\n");
		}
		int getlen = data->GetGtpDataLen();
		if (getlen !=datalen)
		{
			printf("GetGtpDataLen %d�� ToBuffer�ĳ���%d����ͬ",getlen,datalen);
			return -1 ;
		}
		if (getlen != tmpstr.length())
		{
			printf("GetGtpDataLen %d�� ToString�ĳ���%d����ͬ",getlen,tmpstr.length());
			return -1 ;
		}
	}


	//ȡarray�ڵ��rowid
	int rowid = data->GetRowId("array",fieldtype);
	printf("�ڵ�array��rowid[%d] ����[%d]\n",rowid,fieldtype);
	//ȡarray���ӽڵ����
	int arraynum = data->GetArraySize(rowid);
	printf("�ڵ�array���ӽڵ����[%d]\n",arraynum);
	//ȡhash�ڵ��ֵ������
	data->GetValueByName("hash",ss,fieldtype);
	printf("�ڵ�hash��ֵ[%s]  ����[%d]\n",ss.c_str(),fieldtype);
	////ȡarray�ڵ��rowid
	rowid = data->GetRowId("array",fieldtype);
	printf("�ڵ�array��rowid[%d] ����[%d]\n",rowid,fieldtype);


	CGtpData *tmphashnode=NULL;
	
	//ȡarray�ڵ�Ķ���
	CGtpData *tmpg = data->GetDataPoint("array");
	//array�����м����ֶ�(�ӽڵ�)
	int hashnum = tmpg->FieldCount();

	//ѭ��ȡÿ���ӽڵ��ֵ
	for (int i=0; i< hashnum ; i++)
	{
		//ȡ������С�Ľڵ�
		tmphashnode = tmpg->GetDataPoint(i);
		//�ӽڵ���ȡx3���Ƶ�ֵ
		if (tmphashnode->GetValueByName("x3",ss,fieldtype)>=0)
		{
			printf("x3 [%s] type[%d]\n",ss.c_str(),fieldtype);
		}
	}

	
	data->SetDataItem("array","123456789");
	bRet = data->ToString(tmpstr);
	if (!bRet)
	{
		printf("ToString error");
	}
	else
	{
		printf("ToString data:%s\n",tmpstr.c_str());
	}
	
	CGtpData *addgtp = data->AddDataItem("array2",GTP_DATATYPE_ARRAY);
	if (addgtp != NULL)
	{
		addgtp->SetDataItem("x1","2");
	}
	CGtpData *addhash = addgtp->AddDataItem("0",GTP_DATATYPE_HASH);
	addhash->SetDataItem("x1","");
	addhash->SetDataItem("x2","2{");
	addhash->SetDataItem("x3","3}");
	addhash->SetDataItem("x4","4[");
	addhash->SetDataItem("x5","5]");


	addhash = addgtp->AddDataItem("3",GTP_DATATYPE_HASH);
	addhash->SetDataItem("x11","1,");
	addhash->SetDataItem("x12","2{");
	addhash->SetDataItem("x13","3}");
	addhash->SetDataItem("x14","4[");
	addhash->SetDataItem("x15","5]");

	addhash = addgtp->AddDataItem("2",GTP_DATATYPE_HASH);
	addhash->SetDataItem("x21","1,");
	addhash->SetDataItem("x22","2{");
	addhash->SetDataItem("x23","3}");
	addhash->SetDataItem("x24","4[");
	addhash->SetDataItem("x25","5]");

	bRet = data->ToString(tmpstr);
	if (!bRet)
	{
		printf("ToString error");
	}
	else
	{
		printf("ToString data:%s\n",tmpstr.c_str());
	}
	bRet = data->FromBuffer(tmpstr.c_str(),4);

	pData.NewGtp(2,2);
	pData.SetBodyLen(data->GetGtpDataLen());
	data->SetDataItem("U01","1111111111");
	
	datalen = sizeof(gtpdata);
	pData.ToBuffer(gtpdata,datalen);

	return 0;

}
