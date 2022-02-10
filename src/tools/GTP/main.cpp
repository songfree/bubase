
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
		if (!isfind) //查找第一个字符
		{
			if (gtpbuffer[i] == key[keyoffset])
			{
				isfind = true;
				keyoffset++;
				keybegin = i;
				if (keyoffset == kelen) //找到key了，这里只有一个字符的KEY
				{
					if (gtpbuffer[i+1] != '=') //后面的不是=，不是key
					{
						isfind = false;
						keyoffset =0;
					}
					else
					{
						isfindkey = true;
						valuebegin = i+2; //值从这里开始
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
		else  //查找后续
		{
			if (gtpbuffer[i] == key[keyoffset])
			{
				isfind = true;
				keyoffset++;
				if (keyoffset == kelen) //找到key了
				{
					if (gtpbuffer[i+1] != '=') //后面的不是=，不是key
					{
						isfind = false;
						keyoffset =0;
					}
					else
					{
						isfindkey = true;
						valuebegin = i+2; //值从这里开始
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
	if (!isfind || !isfindkey) //没有找到
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
	if (!isend)  //没找到','，说明是最后一个
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
	printf("市场[%s] ",sname.c_str());
	CGtpData *pgtp = gtp.GetDataPoint("Q52");
	int ret = pgtp->FieldCount();
	if (ret <1)
	{
		printf("Q52不是arraylist\n");
		return -1;
	}
	std::string sqdate;
	std::string sqtime;
	for (int i=0; i<ret ;i++)
	{
		//直接用取hash方式取值
		//pgtp->GetHashValue(i,"I10",sname,fieldtype);
		//pgtp->GetHashValue(i,"I11",svalue,fieldtype);
		//printf(" %s %s \n",sname.c_str(),svalue.c_str());
		//用此hash的序号对应的节点来取值
		CGtpData *pgtp2 = pgtp->GetDataPoint(i);
		pgtp2->GetValueByName("I10",sname,fieldtype);
		pgtp2->GetValueByName("I11",svalue,fieldtype);
		pgtp2->GetValueByName("Q11",sqdate,fieldtype);
		pgtp2->GetValueByName("Q12",sqtime,fieldtype);
		printf("合约[%s] 名称[%s] 行情日期[%s] 行情时间[%s]",sname.c_str(),svalue.c_str(),sqdate.c_str(),sqtime.c_str());

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
			printf(" 买价%d %.2f 买量%d %d 卖价%d %.2f 卖量%d %d\n",j,dv1,j,nv1,j,dv2,j,nv2);
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
	printf("原始数据 data:%s\n",ss.c_str());
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
			printf("GetGtpDataLen %d和 ToBuffer的长度%d不相同",getlen,datalen);
			return -1 ;
		}
		if (getlen != tmpstr.length())
		{
			printf("GetGtpDataLen %d和 ToString的长度%d不相同",getlen,tmpstr.length());
			return -1 ;
		}
	}


	//取array节点的rowid
	int rowid = data->GetRowId("array",fieldtype);
	printf("节点array的rowid[%d] 类型[%d]\n",rowid,fieldtype);
	//取array的子节点个数
	int arraynum = data->GetArraySize(rowid);
	printf("节点array的子节点个数[%d]\n",arraynum);
	//取hash节点的值和类型
	data->GetValueByName("hash",ss,fieldtype);
	printf("节点hash的值[%s]  类型[%d]\n",ss.c_str(),fieldtype);
	////取array节点的rowid
	rowid = data->GetRowId("array",fieldtype);
	printf("节点array的rowid[%d] 类型[%d]\n",rowid,fieldtype);


	CGtpData *tmphashnode=NULL;
	
	//取array节点的对象
	CGtpData *tmpg = data->GetDataPoint("array");
	//array对象有几个字段(子节点)
	int hashnum = tmpg->FieldCount();

	//循环取每个子节点的值
	for (int i=0; i< hashnum ; i++)
	{
		//取排序最小的节点
		tmphashnode = tmpg->GetDataPoint(i);
		//从节点里取x3名称的值
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
