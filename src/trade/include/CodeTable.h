// CodeTable.h: interface for the CCodeTable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CODETABLE_H__CD822933_07A9_4A62_B9F2_0791BE0E5A56__INCLUDED_)
#define AFX_CODETABLE_H__CD822933_07A9_4A62_B9F2_0791BE0E5A56__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define   MAXDOUBLECOMPARE  0.00005    //DOUBLE比较的正值
#define   MINDOUBLECOMPARE  -0.00005   //DOUBLE比较的负值

#define   MINASCIINUM       48     //ASCII 码表最小值  '0'
#define   MAXASCIINUM       122    //ASCII 码表最大值  'z'
//交易所代码

#define  EX_CODE_SELF '0'	//EX_CODE_SELF	本系统
#define  EX_CODE_DCE '1'	//EX_CODE_DCE 	大商所
#define  EX_CODE_ZCE '2'	//EX_CODE_ZCE	郑商所
#define  EX_CODE_SFE  '3'	  	//上期所
#define  EX_CODE_CFFEX  '4'		//中金所
#define  EX_CODE_SFIT  '5'		//统一平台
#define  EX_CODE_SUCDEN  '6'		//sucden
#define  EX_CODE_GOLD  '7'		//上金所
#define  EX_CODE_SPTR  '8' 		//SPtrader
#define  EX_CODE_LTS  '9'	 	//证券
#define  EX_CODE_CME  'A'		//CME
#define  EX_CODE_LME  'B'		//LME
#define  EX_CODE_INE  'C'			//INE原油



//买卖标志
#define   BS_FLAG_BUY    '1'  //买 
#define   BS_FLAG_SELL   '3'  //卖 

//市场类型
#define EX_MARKET_TYPE_FUTURE   'F'		//期货市场
#define EX_MARKET_TYPE_SPOT     'G'		//现货市场
#define EX_MARKET_TYPE_XME      'X'		//外盘
#define EX_MARKET_TYPE_PATS     'P'		//PATS市场
#define EX_MARKET_TYPE_SEC      'S'     //证券市场


//交易类型
#define TX_TYPE_BASIC     '0'	 	//基本定单
#define TX_TYPE_ARBI      '1'	  	//套利定单
#define TX_TYPE_COMBO     '2'	 	//组合定单
#define TX_TYPE_STRIP     '3'	 	//批量定单
#define TX_TYPE_EXEC      '4' 	 	//期权执行申请定单
#define TX_TYPE_BOTH      '6'		//双边报价定单
#define TX_TYPE_EMERGENCY '7'		//应急
#define TX_TYPE_BLOCK     '8'		//大宗
#define TX_TYPE_IMPAWN    '9'		//质押


//出入金方向
#define  FUND_FLAG_OUT  '0'		//出金
#define  FUND_FLAG_IN   '1'		//入金


//发送状态
#define  SEND_STATUS_NO         '1'		//未发送
#define  SEND_STATUS_COMPLETE   '2'		//已发送
#define  SEND_STATUS_SUCCESS    '3'		//发送成功
#define  SEND_STATUS_FAIL       '4'		//发送失败
#define  SEND_STATUS_CANCEL     '5'		//已撤销

//合约状态
#define VARIETY_STATUS_NOINIT         '0'		//未就绪
#define VARIETY_STATUS_INIT           '1'		//初始化完成
#define VARIETY_STATUS_AUCTION_ORDER  '2' 		//集合竞价申报
#define VARIETY_STATUS_AUCTION_MATCH  '3'		//集合竞价撮合
#define VARIETY_STATUS_TRADE          '4' 	  	//开盘连续交易
#define VARIETY_STATUS_TRADE_PAUSE    '5' 	 	//连续交易暂停
#define VARIETY_STATUS_CLOSE          '6' 	 	//闭市
#define VARIETY_STATUS_DERY_APP       '7' 	 	//交割申报
#define VARIETY_STATUS_DERY_MATCH     '8'		//交割申报结束
#define VARIETY_STATUS_MID_APP        '9'	  	//中立仓申报
#define VARIETY_STATUS_MID_MATCH      'A' 	 	//中立仓申报撮合
#define VARIETY_STATUS_STOP           'C'		//合约停止

//品种类别

#define PROD_TYPE_SPOT           '0'		//现货
#define PROD_TYPE_OPTION         '2'		//期权
#define PROD_TYPE_FUTURES        '4'		//期货
#define PROD_TYPE_SEC            '5'		//股票

//复核状态
#define  CHECK_STATUS_NO       '1'		//未复核
#define  CHECK_STATUS_PASS     '2'		//复核已通过
#define  CHECK_STATUS_REFUSE   '3'		//复核不通过
#define  CHECK_STATUS_CANCEL   '4'		//已撤销

//委托状态
#define ORDER_STATUS_SENDOK        '1'		//正在申报
#define ORDER_STATUS_INVALID       '2'		//无效单
#define ORDER_STATUS_PARTCANCEL    '3'		//部成部撤
#define ORDER_STATUS_OK            'o'	  	//已报入
#define ORDER_STATUS_TRIG          'r' 		//止盈止损已触发
#define ORDER_STATUS_COMPLETE      'c' 		//完全成交
#define ORDER_STATUS_PART          'p' 		//部分成交
#define ORDER_STATUS_CANCEL        'd'	 	//撤单
#define ORDER_STATUS_SC            's'		//系统拒绝

//对账状态
#define FCS_NOCHECK     '0'		//未处理
#define FCS_SUCCESS     '1'		//匹配成功
#define FCS_2           '2'		//他方有,我方无
#define FCS_3           '3'		//我方有,他方无
#define FCS_4           '4'		//他方有,我方记录已处理
#define FCS_5           '5'		//我方有,他方记录已处理
#define FCS_FAIL        '6'		//双方关键域匹配不一致

//币种
#define CURRENCY_CNY    '0'		//人民币
#define CURRENCY_USD    '1'		//美元
#define CURRENCY_EUR    '2'		//欧元
#define CURRENCY_GBP    '3'		//英磅
#define CURRENCY_HKD    '4'		//港币

//账户类型
#define EX_ACC_TYPE_SELF    '0'		//自营
#define EX_ACC_TYPE_PERSON  '1'		//个人
#define EX_ACC_TYPE_CORP    '2'		//法人

//开平标志
#define EO_FLAG_AUTO        '0'		//自动
#define EO_FLAG_OPEN        '1'		//开仓
#define EO_FLAG_CLOSE       '2'		//平仓
#define EO_FLAG_CLOSETODAY  '3'		//平今
#define EO_FLAG_YESTERDAY   '4'		//平昨
#define EO_FLAG_RULE        '5'     //强减
#define EO_FLAG_FORCE       '6'     //强平
#define EO_FLAG_LOCALFORCE  '7'     //本地强平


//投保标志
#define SH_FLAG_SPEC     '1'		//投机
#define SH_FLAG_HEDGE    '3' 	 	//保值
#define SH_FLAG_DERY     '4'		//递延交割申报
#define SH_FLAG_MID      '5'		//中立仓申报
#define SH_FLAG_INTER	 '6'	    //套利interest arbitrage
#define SH_FLAG_SEC	     '7'	    //个股备兑

//报单订单类型
#define  	ORDER_TYPE_LIMIT	     '0'    //限价
#define  	ORDER_TYPE_MARKET	     '1'    //市价
#define  	ORDER_TYPE_MS_LOSS       '2'	//市价止损
#define  	ORDER_TYPE_MS_PROFIT	 '3'    //市价止盈
#define  	ORDER_TYPE_LS_LOSS	     '4'    //限价止损
#define  	ORDER_TYPE_LS_PROFIT	 '5'    //限价止盈
#define  	ORDER_TYPE_M2L	         '6'    //市价转限价
#define  	ORDER_TYPE_LEVEL1_M	     '7'    //1档市价
#define  	ORDER_TYPE_LEVEL5_M	     '8'    //5档市价

//报单属性
#define ORDER_ATTR_GFD   '0'		//当日有效
#define ORDER_ATTR_FOK   '1'		//全部成交或全撤定单
#define ORDER_ATTR_FAK   '2'		//立即成交剩余即撤销定单
#define ORDER_ATTR_GTD   '3'		//指定日期有效
#define ORDER_ATTR_GTC   '4'		//撤销前有效

//操作员类别
#define  OPER_TYPE_MGR    '0'		//管理员
#define  OPER_TYPE_TRADER '1'		//交易员

//收费模式
#define  FEE_MODEL_DONERATE   '1'		        //成交金额比率
#define  FEE_MODEL_WEIGHT     '2'		        //重量(元/克)
#define  FEE_MODEL_HAND       '3'		        //手数(元/手)
#define  FEE_MODEL_EXCH_RATE   '4'		        //交易所手续费比率(在交易所费率上乘以，不含交易所)
#define  FEE_MODEL_EXCH_RATE_INCLUDE   '5'		//交易所手续费比率(在交易所费率上乘以，包含交易所)

//是否字段
#define  BOOL_FALSE     '0'		//否
#define  BOOL_TRUE      '1'		//是
#define  BOOL_ASC_FALSE  48		//否
#define  BOOL_ASC_TRUE   49		//是
//机构类型
#define ORG_TYPE_SELF   '0'		//自营网点
#define ORG_TYPE_AGENT  '1'		//代理网点
#define ORG_TYPE_COOP   '2'		//合作机构 cooperation
#define ORG_TYPE_MGR    '3'		//管理机构(非网点)
#define ORG_TYPE_OTHER  '4'		//其他

//机构级别
#define  ORG_LEVEL_STATE     '1'		//全国
#define  ORG_LEVEL_PROV      '2'		//省中心 province
#define  ORG_LEVEL_CITY      '3'		//地市中心 city
#define  ORG_LEVEL_COUNTY    '4'		//市县中心 county
#define  ORG_LEVEL_POINT     '5'		//网点  point

//登录状态
#define  LOGIN_STATUS_NONE     '0'		//未连接未登录
#define  LOGIN_STATUS_OK       '1'		//已连接已登录
#define  LOGIN_STATUS_CONNECT  '2'		//已连接未登录

//终端类型
#define  TERM_TYPE_MGR        "1"		//管理端
#define  TERM_TYPE_BANKCOUNT  "2"		//银行柜面
#define  TERM_TYPE_TRADER     "3"		//交易端
#define  TERM_TYPE_AUTO       "4"		//营业厅自助
#define  TERM_TYPE_CYBERBANK  "5"		//网上银行
#define  TERM_TYPE_PHONE      "6"		//电话银行
#define  TERM_TYPE_MOBILE     "7"		//手机银行
#define  TERM_TYPE_SYSTEM     "8"		//系统
#define  TERM_TYPE_CLEAR      "9"		//清算
#define  TERM_TYPE_GATEWAY   "10"		//网关
#define  TERM_TYPE_OTHER     "11"		//其它
#define  TERM_TYPE_RISK      "12"		//风控
#define  TERM_TYPE_MANUAL    "14"		//人工座席
#define  TERM_TYPE_CCB       "15"		//企业网银
#define  TERM_TYPE_RPCAPI       "16"		//内部SOCKET API


//证件类型
#define   CERT_TYPE_IDCARD    '0'		//身份证
#define   CERT_TYPE_OFFICER   '1'		//军官证
#define   CERT_TYPE_PASSPORT  '2'		//护照
#define   CERT_TYPE_RESIDENCE '3'		//户口薄
#define   CERT_TYPE_OTHER     '4'	 	//其它
#define   CERT_TYPE_LP_HM     '5'		//港澳通行证
#define   CERT_TYPE_RT        '6'		//回乡证
#define   CERT_TYPE_BL        '7'		//营业执照

//账户状态
#define   ACC_STATUS_VALID    '0' 		//正常
#define   ACC_STATUS_FROZEN   '1'		//冻结
#define   ACC_STATUS_LOGOUT   '2'		//注销

//账户类型
#define ACC_TYPE_SELF     '0'		//自营
#define ACC_TYPE_PERSON   '1'		//个人
#define ACC_TYPE_CORP     '2'		//法人

//费用模板类型
#define  FEE_MODEL_TYPE_MEMBER    '1'		//  会员费用模板
#define  FEE_MODEL_TYPE_EXCHANGE  '2'		//  交易所费用模板

//费用类型
#define  FEE_TYPE_TRADE     '1'		//交易费用
#define  FEE_TYPE_INTEREST  '2'		//利率
#define  FEE_TYPE_STORE     '3'		//仓储


//报价单位
#define  PRICE_UNIT_MG       '1'		//mg
#define  PRICE_UNIT_G        '2'		//g
#define  PRICE_UNIT_KG       '3'		//kg
#define  PRICE_UNIT_TON      '4'		//吨
#define  PRICE_UNIT_PORTION  '5'		//股
#define  PRICE_UNIT_OZ       '6'		//盎司
#define  PRICE_UNIT_PIECE    '7'		//张

//日期类型
#define  DT_HOLI                  '1'		//1 法定节假日
#define  DT_INTEREST_PERSON       '2'		//2 对私结息日
#define  DT_INTEREST_CORP         '3'		//3 对公结息日
#define  DT_STOR_CLEAR            '4'		//4 仓储费结算日
#define  DT_STOR_TAKE             '5'		//5 仓储提货日
#define  DT_TRADE_NIGHT           '6'		//6 夜市交易日
#define  DT_INTEREST_CALC_CORP    '7'		//7 对公利息计提日
#define  DT_INTEREST_CALC_PERSON  '8'		//8 对私利息计提日
#define  DT_FAIR_VALUE            '9'		//9 公允价值结算日

//交易权限
#define   TR_NONE     '0' 		//不可交易
#define   TR_CLOSE    '1'		//只可平仓
#define   TR_NORMAL   '2'       //正常

//持仓监控指标
#define   PM_MARGINRATE        '0' 		//保证金占比
#define   PM_POSIVALUE_RATE    '1'		//持仓价值比
#define   PM_POSI_OFFSET_RATE  '2'      //持仓轧差价值比

//资产资金监控指标
#define   PFM_NETVALUE  '0' 		//单位净值
#define   PFM_RISK      '1'		    //风险度

//资金监控指标
#define  FMM_RIGHT    '0' 		//客户权益
#define  FMM_RISK     '1'		//风险度

//条件
#define  EF_LARGE       '0'		//大于
#define  EF_SMALLEQUAL  '1'		//小于等于
#define  EF_EQUAL       '2'		//等于
#define  EF_LARGEEQUAL  '3'		//大于等于

//资金类型
#define  FUND_TYPE_INOUT            '1'   //收支
#define  FUND_TYPE_FROZE            '2'   //冻结
#define  FUND_TYPE_MARGIN           '3'   //保证金
#define  FUND_TYPE_FEE              '4'   //费用
#define  FUND_TYPE_RESERVE          '5'   //备付金
#define  FUND_TYPE_SURPLUS          '6'   //盈亏
#define  FUND_TYPE_FLOATSURPLUS     '7'   //浮动盈亏
#define  FUND_TYPE_ARAP             '8'   //应收应付
#define  FUND_TYPE_INCR_INTE        '9'   //利息

#define CLEAR_STATUS_0     '0'		//0未清算
#define CLEAR_STATUS_1     '1'		//1清算进行中
#define CLEAR_STATUS_OK    '2'		//2清算完成
#define CLEAR_STATUS_CHG    '3'		//正在日切
#define CLEAR_STATUS_DATEEND    '4'		//日切完成

//资金编号
#define  FUND_CODE_IN          "101"     //入金
#define  FUND_CODE_OUT         "102"     //出金
#define  FUND_CODE_BUY         "103"     //买货款
#define  FUND_CODE_SELL        "104"     //卖货款
#define  FUND_CODE_PAYRIGHT    "105"     //支出权利金
#define  FUND_CODE_GETRIGHT    "106"     //得到权利金
#define  FUND_CODE_GETBREAK    "107"     //收到违约金
#define  FUND_CODE_PAYBREAK    "108"    //支出违约金
#define  FUND_CODE_BUYFROZE    "201"     //买入冻结
#define  FUND_CODE_BUYFROZEMARGIN     "202"     //买入保证金冻结
#define  FUND_CODE_SELLFROZEMARGIN     "203"    //卖出保证金冻结
#define  FUND_CODE_FEEFROZE            "204"    //手续费冻结
#define  FUND_CODE_TAKEFROZE           "205"    //提货冻结
#define  FUND_CODE_MARGIN_POSI           "301"     //持仓保证金
#define  FUND_CODE_MARGIN_POSI_BUY        "303"     //买入持仓保证金
#define  FUND_CODE_MARGIN_POSI_SELL        "304"    // 卖出持仓保证金
#define  FUND_CODE_MARGIN_OPTION_SELL  "305"     //期权卖出保证金
#define  FUND_CODE_MARGIN_DELI            "306"     //交割保证金
#define  FUND_CODE_FEE_EXCNANGE            "401"    //手续费
#define  FUND_CODE_FEE_TRANS           "402"    //运保费
#define  FUND_CODE_FEE_STORAGE         "403"   //仓储费
#define  FUND_CODE_FEE_UPDIFF         "404"    //升水
#define  FUND_CODE_FEE_DOWNDIFF         "405"    //贴水
#define  FUND_CODE_RESERVE_DIFF  "501"   //溢短备付金
#define  FUND_CODE_RESERVE_DELI  "502"   //交割准备金
#define  FUND_CODE_SURPLUS_POSI    "601"   //持仓盈亏  结算
#define  FUND_CODE_SURPLUS_CLOSE   "602"   //平仓盈亏
#define  FUND_CODE_FLOATSURPLUS_POSI    "701"   //持仓浮动盈亏
#define  FUND_CODE_FLOATSURPLUS          "702"  //持有浮动盈亏
#define  FUND_CODE_AR                  "801"    //应收账款
#define  FUND_CODE_AP                  "802"    //应付账款
#define  FUND_CODE_INTE_IN        "901"    //利息收入
#define  FUND_CODE_INTE_TAX      "902"    //利息税支出
#define  FUND_CODE_PUNI_INTE      "903"    //罚息支出
#define  FUND_CODE_INTE_OUT      "904"    //利息支出


#define  WARN_ITEM_UPDOWN      '0' 		//涨跌
#define  WARN_ITEM_UPDOWNRATE  '1'		//涨跌幅
#define  WARN_ITEM_DONENUM     '2'		//成交量
#define  WARN_ITEM_DUEDAYS     '3'		//临近 到期天数
#define  WARN_ITEM_IV           '4'		//IV
#define  WARN_ITEM_DELTA        '5'		//DELTA
#define  WARN_ITEM_GAMMA        '6'		//GAMMA
#define  WARN_ITEM_THETA        '7'		//THETA
#define  WARN_ITEM_VEGA         '8'		//VEGA


#define  BAIL_MODE_BOTH	  '0'   //双边
#define	 BAIL_MODE_NET	  '1'   //净边
#define	 BAIL_MODE_LARGE  '2'   //大边
#define	 BAIL_MODE_NONE	  '3'   //无，非保证金模式

#define  POSI_MODE_BOTH	      '0'   //双边
#define	 POSI_MODE_SINGLE	  '1'   //单边

#ifndef SUCCESS
#define  SUCCESS              0   //成功
#endif
//返回码
#define  RET_ERR_DATABASE        2000     //数据库错误
#define  RET_ERR_INPUT           2001     //输入数据非法
#define  RET_ERR_NOTFOUND        2002     //无此数据，找不到
#define  RET_ERR_SYSTEM          2003     //系统错误   如ToBuffer错等等
#define  RET_ERR_EXISTS          2004     //已存在
#define  RET_ERR_PAGE_NOTFOUND   2005     //分页查询无记录
#define  RET_ERR_PAGESIZE        2006     //分页查询，当前页记录超限
#define  RET_ERR_GENCONTRACT_NO  2007     //生成合约编号失败
#define  RET_ERR_NO_DELETE       2008     //禁止删除
#define  RET_ERR_NO_RIGHT        2009     //没有权限操作
#define  RET_ERR_PWD             2010     //密码错误或操作员不存在
#define  RET_ERR_PWDLOCK         2011     //密码锁定
#define  RET_ERR_CERTNO          2012     //数字证书不符
#define  RET_ERR_CLEARDATE       2013     //清算日期不符
#define  RET_ERR_REPEATIMPORT    2014     //重复导入
#define  RET_ERR_CONNECTDB       2015     //连接数据库出错
#define  RET_ERR_DELETECLEARDATA  2016     //删除清算数据出错
#define  RET_ERR_IMPORT           2017     //导入出错
#define  RET_ERR_TRADEDATE        2018     //交易日期不符
#define  RET_ERR_TXACC            2019     //资金账号非法
#define  RET_ERR_CLEARSTATUS      2020     //清算状态不符
#define  RET_ERR_CLEARPRICE       2021     //更新结算价出错
#define  RET_ERR_DATEINPUT        2022     //日期输入不符
#define  RET_ERR_CONTRACTINPUT    2023     //合约输入不符
#define  RET_ERR_CUSTTXACC        2024     //客户号资金账号不匹配
#define  RET_ERR_MARKET           2025     //市场非法
#define  RET_ERR_UPORG            2026     //上级机构非法
#define  RET_ERR_PRODACC          2027     //产品交易账号不存在
#define  RET_ERR_SYSTEMPARA       2028     //系统参数配置错
#define  RET_ERR_OPTIONS          2029     //计算期权出错
#define  RET_ERR_POSITION_NOTFOUND   2030     //无持仓/持仓不足 无库存/库存不足
#define  RET_ERR_GETRISKLEVEL    2031     //取风险级别出错
#define  RET_ERR_CALCRISK        2032     //计算风险出错
#define  RET_ERR_FEESET          2033     //手续费设置出错
#define  RET_ERR_MARGINSET          2034     //保证金设置出错
#define  RET_ERR_POSITION_AVAIL      2035     //无足够持仓
#define  RET_ERR_CUST                 2036     //客户证件、手机号不一致  即按证件和手机号找到多个客户
#define  RET_ERR_FEETEMPLET_NOTFOUND  2037     //费率模板不存在
#define  RET_ERR_ORG_NOTFOUND            2038     //机构不存在
#define  RET_ERR_TRANSHISDATA            2039     //转历史表出错
#define  RET_ERR_SHFLAG                  2040     //投保标志错误
#define  RET_ERR_EOFLAG                  2041     //开平标志错误
#define  RET_ERR_BSFLAG                  2042     //买卖标志错误
#define  RET_ERR_TXTYPE                  2043     //交易类型错误
#define  RET_ERR_ORDERTYPE               2044     //报单类型错误
#define  RET_ERR_ORDERATTR               2045     //报单属性错误/成交属性错误
#define  RET_ERR_ORDERPRICE               2046     //委托价错误
#define  RET_ERR_EXCHANGE                 2047     //交易所信息不存在
#define  RET_ERR_CHANNEL                  2048     //渠道错误
#define  RET_ERR_CUSTOPER                 2049     //客户/交易员错误
#define  RET_ERR_CUSTFUNDACC              2050     //资金账号非法
#define  RET_ERR_CUSTNO                   2051     //客户号非法，不存在
#define  RET_ERR_CHANNELOPEN               2052     //未开通此渠道
#define  RET_ERR_TXORG                     2053     //交易网点不存在

#define  RET_ERR_ORDERTYPE_SUPPORT              2054     //报单类型不支持
#define  RET_ERR_ORDERATTR_SUPPORT              2055     //报单类型+成交属性不支持
#define  RET_ERR_CUSTFUND                       2056     //无资金数据
#define  RET_ERR_TRADECODE                       2057     //无交易编码
#define  RET_ERR_GETFEE                         2058     //取手续费出错
#define  RET_ERR_GETMARGIN                      2059     //出保证金出错
#define  RET_ERR_AVAILFUND                      2060     //可用资金不足
#define  RET_ERR_CLEARFLAG                      2061     //清算标志不符，配置为清算不可数据导入
#define  RET_ERR_CLEARFLAG                      2061     //清算标志不符，配置为清算不可数据导入
#define  RET_ERR_TRADERIGHT                     2062     //交易权限不符
#endif // !defined(AFX_CODETABLE_H__CD822933_07A9_4A62_B9F2_0791BE0E5A56__INCLUDED_)
