
#include "KvData.h"


int main(int argc,char *argv[])
{


	CKvData ckv;
	char buf[1000];
	memset(buf, 0, 1000);
	sprintf(buf, "%s", "        189008403  2183 0179                0                       00000000#rsp_msg=����ɹ�#Exch_bal=0#al_result_set=20160602��     20000000.00��10040271�������ǣ������������ɹ���(����)�����ƽ�֤�����20160602��     20000000.00��10040267�������ǣ������������ɹ���(����)�����ƽ�֤�����20160601��      5000000.00��10060395�������ǣ������������ɹ���(����)�����ƽ�֤�����20160523��       170750.55��10050365���������������նˣ�ʧ�ܣ�RSP017004:�ÿͻ��ʽ����벻���ڣ���20160523��       170750.55��10050403���������������նˣ�ʧ�ܣ�RSP017004:�ÿͻ��ʽ����벻���ڣ���20160523��       170750.55��10050388���������������նˣ�ʧ�ܣ�RSP017004:�ÿͻ��ʽ����벻���ڣ���20160523��       170750.55��10050377���������������նˣ�ʧ�ܣ�RSP017004:�ÿͻ��ʽ����벻���ڣ���20160523��      4891565.60��10050591��������ǣ������նˣ��ɹ�������20160523��       170750.55��10050421��������ǣ������նˣ��ɹ�������20160523��      1000000.00��10050374�������ǣ������������ɹ���(����)�����ƽ�֤�����#account_no=8110701014000013622#cust_name=������#protocol_no=1.0#h_start_num=1#h_query_num=100#h_return_count=0#h_all_page_num=0#h_all_count=0#std400pgtn=13#std400acur=10#");
	ckv.fromBuffer(buf);
	
	int c=0,r=0;
	string svalue="";
	ckv.getPackX("al_result_set", svalue, r, c);
	
	printf("%d %d ",r,c);

	return 0;
	


// 	int ret,len;
// 	char databuf[65535];
// 	bzero(databuf,sizeof(databuf));
// 	CKvData  pdata;
// 	FILE *fp = NULL;
// 
// 	std::string svs;
// 	pdata.setPack("rspcode","����ʡ�Ͼ���");
// 	pdata.ToString(svs);
// 	printf("[%s]len[%d]\n",svs.c_str(),svs.length());
// 
// 
// 
// 
// 	CBF_Tools::GetConsoleEnter("������kv�ļ���",65534,databuf);
// 	fp = fopen(databuf,"r");
// 	if (fp == NULL)
// 	{
// 		printf("�ļ�[%s]������\n",databuf);
// 		return -1;
// 	}
// 	ret = fread(databuf,1,65534,fp);
// 	databuf[ret]=0;
// 	fclose(fp);
// 	
// 	int curtime = CBF_Date_Time::GetTickCount();
// 	bool isbool = pdata.FromBuffer(databuf);
// 	printf("����ʱ��[%d]ms\n",CBF_Date_Time::GetTickCount() -  curtime);
// 	if (!isbool)
// 	{
// 		printf("��������\n");
// 		return -1;
// 	}
// 	std::string svalue;
// 	int ncol,nline;
// 	CBF_Tools::GetConsoleEnter("����������(#�ָ�������)",40,databuf);
// 	bool bRet = pdata.getPackX(databuf,svalue,nline,ncol);
// 	if (!bRet)
// 	{
// 		printf("ȡֵ[%s]����\n",databuf);
// 	}
// 	printf("����[%d] ��[%d] ��[%d]\n",svalue.length(),nline,ncol);
// 	len = 65534;
// 	pdata.toBuffer(databuf,len);
// 	fp = fopen("2.txt","w");
// 	if (fp == NULL)
// 	{
// 		return -1;
// 	}
// 	ret = fwrite(databuf,1,strlen(databuf),fp);
// 	fclose(fp);
// 	

// 	std::string data;
// 	pdata.ToString(data);
// 	fp = fopen("c:/log/19.test","w");
// 	if (fp == NULL)
// 	{
// 		return -1;
// 	}
// 	ret = fwrite(data.c_str(),1,data.length(),fp);
// 	fclose(fp);
// 	pdata.toBuffer(databuf,sizeof(databuf)-1);
// 	fp = fopen("c:/log/sep31.test","w");
// 	if (fp == NULL)
// 	{
// 		return -1;
// 	}
// 	ret = fwrite(databuf,1,strlen(databuf),fp);
// 	fclose(fp);
// 	int ncol,nline;
// 	data = "";
// 	pdata.GetValueByNameOne("rest",data,ncol,nline);
// 	fp = fopen("c:/log/sep32.test","w");
// 	if (fp == NULL)
// 	{
// 		return -1;
// 	}
// 	ret = fwrite(data.c_str(),1,data.length(),fp);
// 	fclose(fp);
// 	std::string svalue;
// 	pdata.getPackX("rest",svalue,nline,ncol);
// 	printf("value[%s]  line[%d] col[%d]\n",svalue.c_str(),nline,ncol);
// 	fp = fopen("c:/log/sep33.test","w");
// 	if (fp == NULL)
// 	{
// 		return -1;
// 	}
// 	ret = fwrite(svalue.c_str(),1,svalue.length(),fp);
// 	fclose(fp);
// 	std::string arraylist;
// 	int arrayline;
// 	int arraycol;
// 	pdata.getPackX("alm_protocol_prod",arraylist,arrayline,arraycol);
// 	printf("arraylist[%s]  line[%d] col[%d]\n",arraylist.c_str(),arrayline,arraycol);
// 	fp = fopen("c:/log/sep34.test","w");
// 	if (fp == NULL)
// 	{
// 		return -1;
// 	}
// 	ret = fwrite(arraylist.c_str(),1,arraylist.length(),fp);
// 	fclose(fp);
// 
// 	pdata.m_pDataNode.DeleteAllChild();
// 
// 	
// 	pdata.setPack("rspcode","000000");
// 	pdata.setPackX("rest",svalue,nline,ncol);
// 	pdata.setPack("txcode","8004001");
// 	pdata.setPack("txname","��¼����");
// 	pdata.setPackX("�����б�",svalue,nline,ncol);
// 	pdata.setPack("aaaaaaa","2222222");
// 
// 	pdata.setPackX("alm_protocol_prod",arraylist,arrayline,arraycol);
// 
// 	pdata.toBuffer(databuf,sizeof(databuf)-1);
// 	fp = fopen("c:/log/sep35.test","w");
// 	if (fp == NULL)
// 	{
// 		return -1;
// 	}
// 	ret = fwrite(databuf,1,strlen(databuf),fp);
// 	fclose(fp);
// 
// 	std::string txcode;
// 	pdata.getPackH("txcode",txcode);
// 	printf("txcode=%s\n",txcode.c_str());
// 	std::string txslist;
// 	pdata.getPackH("�����б�",txslist,0,1);
// 	printf("�����б� ��0��1=%s\n",txslist.c_str());
// 	if (!pdata.getPackH("�����б�",txslist,2,3))
// 	{
// 		printf("����[%s]\n",pdata.m_sErrMsg);	
// 	}
// 	printf("�����б� ��2��3=%s\n",txslist.c_str());
// 
// 	//"seq_no", "msg_type", "exch_code", "msg_flag", "term_type", "user_type", "user_id", "area_code", "branch_id", "c_teller_id1", "c_teller_id2", "rsp_code"
// 	pdata.setHead("msg_flag","1");
// 
// 	pdata.setHead("seq_no","12345678");
// 	pdata.setHead("msg_type","0");
// 	pdata.setHead("exch_code","800400100");
// 	pdata.setHead("term_type","0");
// 	pdata.setHead("user_type","0");
// 	pdata.setHead("user_id","lbh");
// 	pdata.setHead("area_code","0000");
// 	pdata.setHead("branch_id","10010010");
// 	pdata.setHead("c_teller_id1","lbh1");
// 	pdata.setHead("c_teller_id2","lbh2");
// 	pdata.setHead("rsp_code","000000");
// 	
// 	pdata.toBuffer(databuf,sizeof(databuf)-1);
// 	fp = fopen("c:/log/sep36.test","w");
// 	if (fp == NULL)
// 	{
// 		return -1;
// 	}
// 	ret = fwrite(databuf,1,strlen(databuf),fp);
// 	fclose(fp);
//	CBF_Tools::GetConsoleEnter("��������˳�",1,databuf);
	return 0;

}
