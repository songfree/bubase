// changedbpwd.cpp : Defines the entry point for the console application.
//
#include "public.h"
#include "Xdp.h"
#include "ficc.pb.h"

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

typedef struct ORDER_REQ_
{
    char trade_acct[21];
	char secu_code[21];
	char order_price_type[2];
	char direction[2];
	char offset_flag[2];
	char hedge_flag[2];
	double price=100.2;
	int vol=100;
	char time_cond[2];
	int strategy_code=999001;
	int user_id=100;
	char purchase_way[2];
	char text[201];
	double ytm=0.98;
	double strike_yeild=0.97;
	int quote_type=1;
	int max_floor=100;
	int min_tick_size=1;
	int settle_type=2;
	int match_type=1;
	int delivery_type=1;
	int clearing_method=2;
	char valid_until_time[17];
	char mdentry_time[17];
	char client_ref_id[21];
	int routing_type=1;
	int	market_indicator=2;
	int realtime_undertake_flag=1;
	int subject_party_type=1;
	char reserver1[21];
	char quote_id[21];
}S_ORDER_REQ_;
//// 交易报单数据
//message ficc_Order_Req{
//    message_head.head  head = 1;                 //报文头
//    string trade_acct = 2;               // 交易账户
//    string secu_code = 3;                // 证券代码
//    string order_price_type = 4;        // 报单价格条件
//    string direction = 5;               // 买入卖出
//    string offset_flag = 6;             // 开仓平仓
//    string hedge_flag = 7;              // 投机保值
//    float price = 8;                    // 委托价格
//    uint64 vol = 9;                     // 报单数量
//    string time_cond = 10;               // 有效期类型
//    uint64 strategy_code = 11;           // 策略代码
//    uint32 user_id = 12;                 // 用户ID   交易员id
//    string purchase_way = 13;            // 申赎方式
//    string text = 14;                    // 报单说明
//    float ytm = 15;                      // 收益率
//    float strike_yeild = 16;             // 行权收益率
//    uint64 quote_type = 17;              // 报价类型
//    uint64 max_floor = 18;               // 最大显示量
//    uint64 min_tick_size = 19;           // 最小变动量
//    uint64 settle_type = 20;             // 清算速度
//    uint64 match_type = 21;              // 报价方式
//    uint64 delivery_type = 22;           // 结算方式
//    uint64 clearing_method = 23;         // 清算类型
//    string valid_until_time = 24;        // 报价有效时间
//    string mdentry_time = 25;            // 行情发生时间
//    string client_ref_id = 26;           // 客户参考编号
//    uint64 routing_type = 27;            // 发送对象
//    uint64 market_indicator = 28;        // 市场类型
//    uint64 realtime_undertake_flag = 29; // 实时承接标识
//    uint64 subject_party_type = 30;      // 本方主体类型
//    string reserver1 = 31;               // 系统保留字段1  发起报单请求的报单来源   前端'1'   策略服务'2'
//    string quote_id = 32;                // 报价编号
//}
int main(int argc, char* argv[])
{
	char errmsg[256];
	errmsg[0] = 2;
	
	CXdp  xdp;
	if (!xdp.InitXdp("xdp.xml",errmsg))
	{
		printf("初始化出错：%s\n",errmsg);
		return -1;
	}
	int i,j;
	char databuf[1000];
	bzero(databuf,sizeof(databuf));
	xdp.ResetData();
	//准备测试数据
	S_ORDER_REQ_ testdata;
	strcpy(testdata.trade_acct,"123456789");
	strcpy(testdata.secu_code, "ib00755");
	strcpy(testdata.order_price_type, "1");
	strcpy(testdata.direction, "1");
	strcpy(testdata.offset_flag, "1");
	strcpy(testdata.hedge_flag, "0");
	strcpy(testdata.time_cond, "0");
	strcpy(testdata.purchase_way, "0");
	strcpy(testdata.text, "test order test protobuf and xdp");
	strcpy(testdata.valid_until_time, "20220629 23:59:59");
	strcpy(testdata.mdentry_time, "20220629 23:59:59");
	strcpy(testdata.client_ref_id, "20220629000001");
	strcpy(testdata.reserver1, "1111111111");
	strcpy(testdata.quote_id, "22222222222");
	
	ficc_message::ficc_Order_Req  req;
	char destbuffer[8192];
	int destbuflen = sizeof(destbuffer);
	int testnum=100000;
	INT64_ btime = CBF_Date_Time::GetTickUS();
	for (i = 0; i < testnum; i++)
	{
		req.set_trade_acct(testdata.trade_acct);
		req.set_secu_code(testdata.secu_code);
		req.set_order_price_type(testdata.order_price_type);
		req.set_direction(testdata.direction);
		req.set_offset_flag(testdata.offset_flag);;
		req.set_hedge_flag(testdata.hedge_flag);
		req.set_price(testdata.price);
		req.set_vol(testdata.vol);
		req.set_time_cond(testdata.time_cond);
		req.set_strategy_code(testdata.strategy_code);
		req.set_user_id(testdata.user_id);
		req.set_purchase_way(testdata.purchase_way);
		req.set_text(testdata.text);
		req.set_ytm(testdata.ytm);
		req.set_strike_yeild(testdata.strike_yeild);
		req.set_quote_type(testdata.quote_type);
		req.set_max_floor(testdata.max_floor);
		req.set_min_tick_size(testdata.min_tick_size);
		req.set_settle_type(testdata.settle_type);
		req.set_match_type(testdata.match_type);
		req.set_delivery_type(testdata.delivery_type);;
		req.set_clearing_method(testdata.clearing_method);
		req.set_valid_until_time(testdata.valid_until_time);
		req.set_mdentry_time(testdata.mdentry_time);
		req.set_client_ref_id(testdata.client_ref_id);
		req.set_routing_type(testdata.routing_type);
		req.set_market_indicator(testdata.market_indicator);
		req.set_realtime_undertake_flag(testdata.realtime_undertake_flag);
		req.set_subject_party_type(testdata.subject_party_type);
		req.set_reserver1(testdata.reserver1);
		req.set_quote_id(testdata.quote_id);
        
       
        if (!req.SerializeToArray(destbuffer, destbuflen))
        {
			printf("SerializeToArray error!\n");
            return -1;
        }
	}
	j= req.ByteSize();
	INT64_ etime = CBF_Date_Time::GetTickUS();
	printf("protobuf set and SerializeToArray datasize %d totaltime %d us avg %.4f us \n",j,etime-btime,1.00*(etime - btime)/testnum);

    btime = CBF_Date_Time::GetTickUS();
    for (i = 0; i < testnum; i++)
    {
		req.ParseFromArray(destbuffer,destbuflen);
    }
	etime = CBF_Date_Time::GetTickUS();
    j = req.ByteSize();
    printf("protobuf ParseFromArray datasize %d totaltime %d us avg %.4f us \n", j, etime - btime, 1.00 * (etime - btime) / testnum);

	unsigned  int len = destbuflen;
    btime = CBF_Date_Time::GetTickUS();
    for (i = 0; i < testnum; i++)
    {
        xdp.SetFieldValue((unsigned short)80,testdata.trade_acct,strlen(testdata.trade_acct),errmsg);
		xdp.SetFieldValue((unsigned short)81, testdata.secu_code, strlen(testdata.secu_code), errmsg);
		xdp.SetFieldValue((unsigned short)82, testdata.order_price_type, strlen(testdata.order_price_type), errmsg);
		xdp.SetFieldValue((unsigned short)83, testdata.direction, strlen(testdata.direction), errmsg);
		xdp.SetFieldValue((unsigned short)84, testdata.offset_flag, strlen(testdata.offset_flag), errmsg);
		xdp.SetFieldValue((unsigned short)85, testdata.hedge_flag, strlen(testdata.hedge_flag), errmsg);
		xdp.SetFieldValue((unsigned short)50, testdata.price, errmsg);
		xdp.SetFieldValue((unsigned short)20, testdata.vol, errmsg);
		xdp.SetFieldValue((unsigned short)86, testdata.time_cond, strlen(testdata.time_cond), errmsg);
		xdp.SetFieldValue((unsigned short)21, testdata.strategy_code, errmsg);
		xdp.SetFieldValue((unsigned short)22, testdata.user_id, errmsg);
		xdp.SetFieldValue((unsigned short)87, testdata.purchase_way, strlen(testdata.purchase_way), errmsg);
		xdp.SetFieldValue((unsigned short)88, testdata.text, strlen(testdata.text), errmsg);
		xdp.SetFieldValue((unsigned short)51, testdata.ytm, errmsg);
		xdp.SetFieldValue((unsigned short)52, testdata.strike_yeild, errmsg);
		xdp.SetFieldValue((unsigned short)23, testdata.quote_type, errmsg);
		xdp.SetFieldValue((unsigned short)24, testdata.max_floor, errmsg);
		xdp.SetFieldValue((unsigned short)25, testdata.min_tick_size, errmsg);
		xdp.SetFieldValue((unsigned short)26, testdata.settle_type, errmsg);
		xdp.SetFieldValue((unsigned short)27, testdata.match_type, errmsg);
		xdp.SetFieldValue((unsigned short)28, testdata.delivery_type, errmsg);
		xdp.SetFieldValue((unsigned short)29, testdata.clearing_method, errmsg);
		xdp.SetFieldValue((unsigned short)89, testdata.valid_until_time, strlen(testdata.valid_until_time), errmsg);
		xdp.SetFieldValue((unsigned short)90, testdata.mdentry_time, strlen(testdata.mdentry_time), errmsg);
		xdp.SetFieldValue((unsigned short)91, testdata.client_ref_id, strlen(testdata.client_ref_id), errmsg);
		xdp.SetFieldValue((unsigned short)10, testdata.routing_type, errmsg);
		xdp.SetFieldValue((unsigned short)11, testdata.market_indicator, errmsg);
		xdp.SetFieldValue((unsigned short)12, testdata.realtime_undertake_flag, errmsg);
		xdp.SetFieldValue((unsigned short)13, testdata.subject_party_type, errmsg);
		xdp.SetFieldValue((unsigned short)13, testdata.subject_party_type, errmsg);
		xdp.SetFieldValue((unsigned short)92, testdata.reserver1, strlen(testdata.reserver1), errmsg);
		xdp.SetFieldValue((unsigned short)93, testdata.quote_id, strlen(testdata.quote_id), errmsg);
		len = destbuflen;
        if (!xdp.ToBuffer(destbuffer, len,errmsg))
        {
            printf("ToBuffer error!\n");
            return -1;
        }
    }
    etime = CBF_Date_Time::GetTickUS();
    printf("xdp set and ToBuffer datasize %d totaltime %d us avg %.4f us \n",len, etime - btime, 1.00 * (etime - btime) / testnum);
    btime = CBF_Date_Time::GetTickUS();
    for (i = 0; i < testnum; i++)
    {
        if (!xdp.FromBuffer(destbuffer, len, errmsg))
        {
            printf("FromBuffer error!\n");
            return -1;
        }
    }
    etime = CBF_Date_Time::GetTickUS();
    printf("xdp FromBuffer datasize %d totaltime %d us avg %.4f us \n", len, etime - btime, 1.00 * (etime - btime) / testnum);
	return 0;
}
