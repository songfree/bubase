
#include "KvData.h"


int main(int argc,char *argv[])
{


	CKvData ckv;
	char buf[1000];
	memset(buf, 0, 1000);
	sprintf(buf, "%s", "        189008403  2183 0179                0                       00000000#rsp_msg=处理成功#Exch_bal=0#al_result_set=20160602｜     20000000.00｜10040271｜入金｜是｜所有渠道｜成功｜(复兴)买卖黄金保证金｜∧20160602｜     20000000.00｜10040267｜入金｜是｜所有渠道｜成功｜(复兴)买卖黄金保证金｜∧20160601｜      5000000.00｜10060395｜入金｜是｜所有渠道｜成功｜(复兴)买卖黄金保证金｜∧20160523｜       170750.55｜10050365｜出金｜否｜交易终端｜失败｜RSP017004:该客户资金密码不存在｜∧20160523｜       170750.55｜10050403｜出金｜否｜交易终端｜失败｜RSP017004:该客户资金密码不存在｜∧20160523｜       170750.55｜10050388｜出金｜否｜交易终端｜失败｜RSP017004:该客户资金密码不存在｜∧20160523｜       170750.55｜10050377｜出金｜否｜交易终端｜失败｜RSP017004:该客户资金密码不存在｜∧20160523｜      4891565.60｜10050591｜出金｜是｜交易终端｜成功｜｜∧20160523｜       170750.55｜10050421｜出金｜是｜交易终端｜成功｜｜∧20160523｜      1000000.00｜10050374｜入金｜是｜所有渠道｜成功｜(复兴)买卖黄金保证金｜∧#account_no=8110701014000013622#cust_name=复兴门#protocol_no=1.0#h_start_num=1#h_query_num=100#h_return_count=0#h_all_page_num=0#h_all_count=0#std400pgtn=13#std400acur=10#");
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
// 	pdata.setPack("rspcode","江苏省南京市");
// 	pdata.ToString(svs);
// 	printf("[%s]len[%d]\n",svs.c_str(),svs.length());
// 
// 
// 
// 
// 	CBF_Tools::GetConsoleEnter("请输入kv文件名",65534,databuf);
// 	fp = fopen(databuf,"r");
// 	if (fp == NULL)
// 	{
// 		printf("文件[%s]不存在\n",databuf);
// 		return -1;
// 	}
// 	ret = fread(databuf,1,65534,fp);
// 	databuf[ret]=0;
// 	fclose(fp);
// 	
// 	int curtime = CBF_Date_Time::GetTickCount();
// 	bool isbool = pdata.FromBuffer(databuf);
// 	printf("解析时间[%d]ms\n",CBF_Date_Time::GetTickCount() -  curtime);
// 	if (!isbool)
// 	{
// 		printf("解析出错\n");
// 		return -1;
// 	}
// 	std::string svalue;
// 	int ncol,nline;
// 	CBF_Tools::GetConsoleEnter("请输入名称(#分隔的名称)",40,databuf);
// 	bool bRet = pdata.getPackX(databuf,svalue,nline,ncol);
// 	if (!bRet)
// 	{
// 		printf("取值[%s]出错\n",databuf);
// 	}
// 	printf("长度[%d] 行[%d] 列[%d]\n",svalue.length(),nline,ncol);
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
// 	pdata.setPack("txname","登录交易");
// 	pdata.setPackX("交易列表",svalue,nline,ncol);
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
// 	pdata.getPackH("交易列表",txslist,0,1);
// 	printf("交易列表 行0列1=%s\n",txslist.c_str());
// 	if (!pdata.getPackH("交易列表",txslist,2,3))
// 	{
// 		printf("出错[%s]\n",pdata.m_sErrMsg);	
// 	}
// 	printf("交易列表 行2列3=%s\n",txslist.c_str());
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
//	CBF_Tools::GetConsoleEnter("按任意键退出",1,databuf);
	return 0;

}
