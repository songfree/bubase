// CodeTable.h: interface for the CCodeTable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CODETABLE_H__CD822933_07A9_4A62_B9F2_0791BE0E5A56__INCLUDED_)
#define AFX_CODETABLE_H__CD822933_07A9_4A62_B9F2_0791BE0E5A56__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define   MAXDOUBLECOMPARE  0.00005    //DOUBLE�Ƚϵ���ֵ
#define   MINDOUBLECOMPARE  -0.00005   //DOUBLE�Ƚϵĸ�ֵ

#define   MINASCIINUM       48     //ASCII �����Сֵ  '0'
#define   MAXASCIINUM       122    //ASCII ������ֵ  'z'
//����������

#define  EX_CODE_SELF '0'	//EX_CODE_SELF	��ϵͳ
#define  EX_CODE_DCE '1'	//EX_CODE_DCE 	������
#define  EX_CODE_ZCE '2'	//EX_CODE_ZCE	֣����
#define  EX_CODE_SFE  '3'	  	//������
#define  EX_CODE_CFFEX  '4'		//�н���
#define  EX_CODE_SFIT  '5'		//ͳһƽ̨
#define  EX_CODE_SUCDEN  '6'		//sucden
#define  EX_CODE_GOLD  '7'		//�Ͻ���
#define  EX_CODE_SPTR  '8' 		//SPtrader
#define  EX_CODE_LTS  '9'	 	//֤ȯ
#define  EX_CODE_CME  'A'		//CME
#define  EX_CODE_LME  'B'		//LME
#define  EX_CODE_INE  'C'			//INEԭ��



//������־
#define   BS_FLAG_BUY    '1'  //�� 
#define   BS_FLAG_SELL   '3'  //�� 

//�г�����
#define EX_MARKET_TYPE_FUTURE   'F'		//�ڻ��г�
#define EX_MARKET_TYPE_SPOT     'G'		//�ֻ��г�
#define EX_MARKET_TYPE_XME      'X'		//����
#define EX_MARKET_TYPE_PATS     'P'		//PATS�г�
#define EX_MARKET_TYPE_SEC      'S'     //֤ȯ�г�


//��������
#define TX_TYPE_BASIC     '0'	 	//��������
#define TX_TYPE_ARBI      '1'	  	//��������
#define TX_TYPE_COMBO     '2'	 	//��϶���
#define TX_TYPE_STRIP     '3'	 	//��������
#define TX_TYPE_EXEC      '4' 	 	//��Ȩִ�����붨��
#define TX_TYPE_BOTH      '6'		//˫�߱��۶���
#define TX_TYPE_EMERGENCY '7'		//Ӧ��
#define TX_TYPE_BLOCK     '8'		//����
#define TX_TYPE_IMPAWN    '9'		//��Ѻ


//�������
#define  FUND_FLAG_OUT  '0'		//����
#define  FUND_FLAG_IN   '1'		//���


//����״̬
#define  SEND_STATUS_NO         '1'		//δ����
#define  SEND_STATUS_COMPLETE   '2'		//�ѷ���
#define  SEND_STATUS_SUCCESS    '3'		//���ͳɹ�
#define  SEND_STATUS_FAIL       '4'		//����ʧ��
#define  SEND_STATUS_CANCEL     '5'		//�ѳ���

//��Լ״̬
#define VARIETY_STATUS_NOINIT         '0'		//δ����
#define VARIETY_STATUS_INIT           '1'		//��ʼ�����
#define VARIETY_STATUS_AUCTION_ORDER  '2' 		//���Ͼ����걨
#define VARIETY_STATUS_AUCTION_MATCH  '3'		//���Ͼ��۴��
#define VARIETY_STATUS_TRADE          '4' 	  	//������������
#define VARIETY_STATUS_TRADE_PAUSE    '5' 	 	//����������ͣ
#define VARIETY_STATUS_CLOSE          '6' 	 	//����
#define VARIETY_STATUS_DERY_APP       '7' 	 	//�����걨
#define VARIETY_STATUS_DERY_MATCH     '8'		//�����걨����
#define VARIETY_STATUS_MID_APP        '9'	  	//�������걨
#define VARIETY_STATUS_MID_MATCH      'A' 	 	//�������걨���
#define VARIETY_STATUS_STOP           'C'		//��Լֹͣ

//Ʒ�����

#define PROD_TYPE_SPOT           '0'		//�ֻ�
#define PROD_TYPE_OPTION         '2'		//��Ȩ
#define PROD_TYPE_FUTURES        '4'		//�ڻ�
#define PROD_TYPE_SEC            '5'		//��Ʊ

//����״̬
#define  CHECK_STATUS_NO       '1'		//δ����
#define  CHECK_STATUS_PASS     '2'		//������ͨ��
#define  CHECK_STATUS_REFUSE   '3'		//���˲�ͨ��
#define  CHECK_STATUS_CANCEL   '4'		//�ѳ���

//ί��״̬
#define ORDER_STATUS_SENDOK        '1'		//�����걨
#define ORDER_STATUS_INVALID       '2'		//��Ч��
#define ORDER_STATUS_PARTCANCEL    '3'		//���ɲ���
#define ORDER_STATUS_OK            'o'	  	//�ѱ���
#define ORDER_STATUS_TRIG          'r' 		//ֹӯֹ���Ѵ���
#define ORDER_STATUS_COMPLETE      'c' 		//��ȫ�ɽ�
#define ORDER_STATUS_PART          'p' 		//���ֳɽ�
#define ORDER_STATUS_CANCEL        'd'	 	//����
#define ORDER_STATUS_SC            's'		//ϵͳ�ܾ�

//����״̬
#define FCS_NOCHECK     '0'		//δ����
#define FCS_SUCCESS     '1'		//ƥ��ɹ�
#define FCS_2           '2'		//������,�ҷ���
#define FCS_3           '3'		//�ҷ���,������
#define FCS_4           '4'		//������,�ҷ���¼�Ѵ���
#define FCS_5           '5'		//�ҷ���,������¼�Ѵ���
#define FCS_FAIL        '6'		//˫���ؼ���ƥ�䲻һ��

//����
#define CURRENCY_CNY    '0'		//�����
#define CURRENCY_USD    '1'		//��Ԫ
#define CURRENCY_EUR    '2'		//ŷԪ
#define CURRENCY_GBP    '3'		//Ӣ��
#define CURRENCY_HKD    '4'		//�۱�

//�˻�����
#define EX_ACC_TYPE_SELF    '0'		//��Ӫ
#define EX_ACC_TYPE_PERSON  '1'		//����
#define EX_ACC_TYPE_CORP    '2'		//����

//��ƽ��־
#define EO_FLAG_AUTO        '0'		//�Զ�
#define EO_FLAG_OPEN        '1'		//����
#define EO_FLAG_CLOSE       '2'		//ƽ��
#define EO_FLAG_CLOSETODAY  '3'		//ƽ��
#define EO_FLAG_YESTERDAY   '4'		//ƽ��
#define EO_FLAG_RULE        '5'     //ǿ��
#define EO_FLAG_FORCE       '6'     //ǿƽ
#define EO_FLAG_LOCALFORCE  '7'     //����ǿƽ


//Ͷ����־
#define SH_FLAG_SPEC     '1'		//Ͷ��
#define SH_FLAG_HEDGE    '3' 	 	//��ֵ
#define SH_FLAG_DERY     '4'		//���ӽ����걨
#define SH_FLAG_MID      '5'		//�������걨
#define SH_FLAG_INTER	 '6'	    //����interest arbitrage
#define SH_FLAG_SEC	     '7'	    //���ɱ���

//������������
#define  	ORDER_TYPE_LIMIT	     '0'    //�޼�
#define  	ORDER_TYPE_MARKET	     '1'    //�м�
#define  	ORDER_TYPE_MS_LOSS       '2'	//�м�ֹ��
#define  	ORDER_TYPE_MS_PROFIT	 '3'    //�м�ֹӯ
#define  	ORDER_TYPE_LS_LOSS	     '4'    //�޼�ֹ��
#define  	ORDER_TYPE_LS_PROFIT	 '5'    //�޼�ֹӯ
#define  	ORDER_TYPE_M2L	         '6'    //�м�ת�޼�
#define  	ORDER_TYPE_LEVEL1_M	     '7'    //1���м�
#define  	ORDER_TYPE_LEVEL5_M	     '8'    //5���м�

//��������
#define ORDER_ATTR_GFD   '0'		//������Ч
#define ORDER_ATTR_FOK   '1'		//ȫ���ɽ���ȫ������
#define ORDER_ATTR_FAK   '2'		//�����ɽ�ʣ�༴��������
#define ORDER_ATTR_GTD   '3'		//ָ��������Ч
#define ORDER_ATTR_GTC   '4'		//����ǰ��Ч

//����Ա���
#define  OPER_TYPE_MGR    '0'		//����Ա
#define  OPER_TYPE_TRADER '1'		//����Ա

//�շ�ģʽ
#define  FEE_MODEL_DONERATE   '1'		        //�ɽ�������
#define  FEE_MODEL_WEIGHT     '2'		        //����(Ԫ/��)
#define  FEE_MODEL_HAND       '3'		        //����(Ԫ/��)
#define  FEE_MODEL_EXCH_RATE   '4'		        //�����������ѱ���(�ڽ����������ϳ��ԣ�����������)
#define  FEE_MODEL_EXCH_RATE_INCLUDE   '5'		//�����������ѱ���(�ڽ����������ϳ��ԣ�����������)

//�Ƿ��ֶ�
#define  BOOL_FALSE     '0'		//��
#define  BOOL_TRUE      '1'		//��
#define  BOOL_ASC_FALSE  48		//��
#define  BOOL_ASC_TRUE   49		//��
//��������
#define ORG_TYPE_SELF   '0'		//��Ӫ����
#define ORG_TYPE_AGENT  '1'		//��������
#define ORG_TYPE_COOP   '2'		//�������� cooperation
#define ORG_TYPE_MGR    '3'		//�������(������)
#define ORG_TYPE_OTHER  '4'		//����

//��������
#define  ORG_LEVEL_STATE     '1'		//ȫ��
#define  ORG_LEVEL_PROV      '2'		//ʡ���� province
#define  ORG_LEVEL_CITY      '3'		//�������� city
#define  ORG_LEVEL_COUNTY    '4'		//�������� county
#define  ORG_LEVEL_POINT     '5'		//����  point

//��¼״̬
#define  LOGIN_STATUS_NONE     '0'		//δ����δ��¼
#define  LOGIN_STATUS_OK       '1'		//�������ѵ�¼
#define  LOGIN_STATUS_CONNECT  '2'		//������δ��¼

//�ն�����
#define  TERM_TYPE_MGR        "1"		//�����
#define  TERM_TYPE_BANKCOUNT  "2"		//���й���
#define  TERM_TYPE_TRADER     "3"		//���׶�
#define  TERM_TYPE_AUTO       "4"		//Ӫҵ������
#define  TERM_TYPE_CYBERBANK  "5"		//��������
#define  TERM_TYPE_PHONE      "6"		//�绰����
#define  TERM_TYPE_MOBILE     "7"		//�ֻ�����
#define  TERM_TYPE_SYSTEM     "8"		//ϵͳ
#define  TERM_TYPE_CLEAR      "9"		//����
#define  TERM_TYPE_GATEWAY   "10"		//����
#define  TERM_TYPE_OTHER     "11"		//����
#define  TERM_TYPE_RISK      "12"		//���
#define  TERM_TYPE_MANUAL    "14"		//�˹���ϯ
#define  TERM_TYPE_CCB       "15"		//��ҵ����
#define  TERM_TYPE_RPCAPI       "16"		//�ڲ�SOCKET API


//֤������
#define   CERT_TYPE_IDCARD    '0'		//���֤
#define   CERT_TYPE_OFFICER   '1'		//����֤
#define   CERT_TYPE_PASSPORT  '2'		//����
#define   CERT_TYPE_RESIDENCE '3'		//���ڱ�
#define   CERT_TYPE_OTHER     '4'	 	//����
#define   CERT_TYPE_LP_HM     '5'		//�۰�ͨ��֤
#define   CERT_TYPE_RT        '6'		//����֤
#define   CERT_TYPE_BL        '7'		//Ӫҵִ��

//�˻�״̬
#define   ACC_STATUS_VALID    '0' 		//����
#define   ACC_STATUS_FROZEN   '1'		//����
#define   ACC_STATUS_LOGOUT   '2'		//ע��

//�˻�����
#define ACC_TYPE_SELF     '0'		//��Ӫ
#define ACC_TYPE_PERSON   '1'		//����
#define ACC_TYPE_CORP     '2'		//����

//����ģ������
#define  FEE_MODEL_TYPE_MEMBER    '1'		//  ��Ա����ģ��
#define  FEE_MODEL_TYPE_EXCHANGE  '2'		//  ����������ģ��

//��������
#define  FEE_TYPE_TRADE     '1'		//���׷���
#define  FEE_TYPE_INTEREST  '2'		//����
#define  FEE_TYPE_STORE     '3'		//�ִ�


//���۵�λ
#define  PRICE_UNIT_MG       '1'		//mg
#define  PRICE_UNIT_G        '2'		//g
#define  PRICE_UNIT_KG       '3'		//kg
#define  PRICE_UNIT_TON      '4'		//��
#define  PRICE_UNIT_PORTION  '5'		//��
#define  PRICE_UNIT_OZ       '6'		//��˾
#define  PRICE_UNIT_PIECE    '7'		//��

//��������
#define  DT_HOLI                  '1'		//1 �����ڼ���
#define  DT_INTEREST_PERSON       '2'		//2 ��˽��Ϣ��
#define  DT_INTEREST_CORP         '3'		//3 �Թ���Ϣ��
#define  DT_STOR_CLEAR            '4'		//4 �ִ��ѽ�����
#define  DT_STOR_TAKE             '5'		//5 �ִ������
#define  DT_TRADE_NIGHT           '6'		//6 ҹ�н�����
#define  DT_INTEREST_CALC_CORP    '7'		//7 �Թ���Ϣ������
#define  DT_INTEREST_CALC_PERSON  '8'		//8 ��˽��Ϣ������
#define  DT_FAIR_VALUE            '9'		//9 ���ʼ�ֵ������

//����Ȩ��
#define   TR_NONE     '0' 		//���ɽ���
#define   TR_CLOSE    '1'		//ֻ��ƽ��
#define   TR_NORMAL   '2'       //����

//�ֲּ��ָ��
#define   PM_MARGINRATE        '0' 		//��֤��ռ��
#define   PM_POSIVALUE_RATE    '1'		//�ֲּ�ֵ��
#define   PM_POSI_OFFSET_RATE  '2'      //�ֲ������ֵ��

//�ʲ��ʽ���ָ��
#define   PFM_NETVALUE  '0' 		//��λ��ֵ
#define   PFM_RISK      '1'		    //���ն�

//�ʽ���ָ��
#define  FMM_RIGHT    '0' 		//�ͻ�Ȩ��
#define  FMM_RISK     '1'		//���ն�

//����
#define  EF_LARGE       '0'		//����
#define  EF_SMALLEQUAL  '1'		//С�ڵ���
#define  EF_EQUAL       '2'		//����
#define  EF_LARGEEQUAL  '3'		//���ڵ���

//�ʽ�����
#define  FUND_TYPE_INOUT            '1'   //��֧
#define  FUND_TYPE_FROZE            '2'   //����
#define  FUND_TYPE_MARGIN           '3'   //��֤��
#define  FUND_TYPE_FEE              '4'   //����
#define  FUND_TYPE_RESERVE          '5'   //������
#define  FUND_TYPE_SURPLUS          '6'   //ӯ��
#define  FUND_TYPE_FLOATSURPLUS     '7'   //����ӯ��
#define  FUND_TYPE_ARAP             '8'   //Ӧ��Ӧ��
#define  FUND_TYPE_INCR_INTE        '9'   //��Ϣ

#define CLEAR_STATUS_0     '0'		//0δ����
#define CLEAR_STATUS_1     '1'		//1���������
#define CLEAR_STATUS_OK    '2'		//2�������
#define CLEAR_STATUS_CHG    '3'		//��������
#define CLEAR_STATUS_DATEEND    '4'		//�������

//�ʽ���
#define  FUND_CODE_IN          "101"     //���
#define  FUND_CODE_OUT         "102"     //����
#define  FUND_CODE_BUY         "103"     //�����
#define  FUND_CODE_SELL        "104"     //������
#define  FUND_CODE_PAYRIGHT    "105"     //֧��Ȩ����
#define  FUND_CODE_GETRIGHT    "106"     //�õ�Ȩ����
#define  FUND_CODE_GETBREAK    "107"     //�յ�ΥԼ��
#define  FUND_CODE_PAYBREAK    "108"    //֧��ΥԼ��
#define  FUND_CODE_BUYFROZE    "201"     //���붳��
#define  FUND_CODE_BUYFROZEMARGIN     "202"     //���뱣֤�𶳽�
#define  FUND_CODE_SELLFROZEMARGIN     "203"    //������֤�𶳽�
#define  FUND_CODE_FEEFROZE            "204"    //�����Ѷ���
#define  FUND_CODE_TAKEFROZE           "205"    //�������
#define  FUND_CODE_MARGIN_POSI           "301"     //�ֱֲ�֤��
#define  FUND_CODE_MARGIN_POSI_BUY        "303"     //����ֱֲ�֤��
#define  FUND_CODE_MARGIN_POSI_SELL        "304"    // �����ֱֲ�֤��
#define  FUND_CODE_MARGIN_OPTION_SELL  "305"     //��Ȩ������֤��
#define  FUND_CODE_MARGIN_DELI            "306"     //���֤��
#define  FUND_CODE_FEE_EXCNANGE            "401"    //������
#define  FUND_CODE_FEE_TRANS           "402"    //�˱���
#define  FUND_CODE_FEE_STORAGE         "403"   //�ִ���
#define  FUND_CODE_FEE_UPDIFF         "404"    //��ˮ
#define  FUND_CODE_FEE_DOWNDIFF         "405"    //��ˮ
#define  FUND_CODE_RESERVE_DIFF  "501"   //��̱�����
#define  FUND_CODE_RESERVE_DELI  "502"   //����׼����
#define  FUND_CODE_SURPLUS_POSI    "601"   //�ֲ�ӯ��  ����
#define  FUND_CODE_SURPLUS_CLOSE   "602"   //ƽ��ӯ��
#define  FUND_CODE_FLOATSURPLUS_POSI    "701"   //�ֲָ���ӯ��
#define  FUND_CODE_FLOATSURPLUS          "702"  //���и���ӯ��
#define  FUND_CODE_AR                  "801"    //Ӧ���˿�
#define  FUND_CODE_AP                  "802"    //Ӧ���˿�
#define  FUND_CODE_INTE_IN        "901"    //��Ϣ����
#define  FUND_CODE_INTE_TAX      "902"    //��Ϣ˰֧��
#define  FUND_CODE_PUNI_INTE      "903"    //��Ϣ֧��
#define  FUND_CODE_INTE_OUT      "904"    //��Ϣ֧��


#define  WARN_ITEM_UPDOWN      '0' 		//�ǵ�
#define  WARN_ITEM_UPDOWNRATE  '1'		//�ǵ���
#define  WARN_ITEM_DONENUM     '2'		//�ɽ���
#define  WARN_ITEM_DUEDAYS     '3'		//�ٽ� ��������
#define  WARN_ITEM_IV           '4'		//IV
#define  WARN_ITEM_DELTA        '5'		//DELTA
#define  WARN_ITEM_GAMMA        '6'		//GAMMA
#define  WARN_ITEM_THETA        '7'		//THETA
#define  WARN_ITEM_VEGA         '8'		//VEGA


#define  BAIL_MODE_BOTH	  '0'   //˫��
#define	 BAIL_MODE_NET	  '1'   //����
#define	 BAIL_MODE_LARGE  '2'   //���
#define	 BAIL_MODE_NONE	  '3'   //�ޣ��Ǳ�֤��ģʽ

#define  POSI_MODE_BOTH	      '0'   //˫��
#define	 POSI_MODE_SINGLE	  '1'   //����

#ifndef SUCCESS
#define  SUCCESS              0   //�ɹ�
#endif
//������
#define  RET_ERR_DATABASE        2000     //���ݿ����
#define  RET_ERR_INPUT           2001     //�������ݷǷ�
#define  RET_ERR_NOTFOUND        2002     //�޴����ݣ��Ҳ���
#define  RET_ERR_SYSTEM          2003     //ϵͳ����   ��ToBuffer��ȵ�
#define  RET_ERR_EXISTS          2004     //�Ѵ���
#define  RET_ERR_PAGE_NOTFOUND   2005     //��ҳ��ѯ�޼�¼
#define  RET_ERR_PAGESIZE        2006     //��ҳ��ѯ����ǰҳ��¼����
#define  RET_ERR_GENCONTRACT_NO  2007     //���ɺ�Լ���ʧ��
#define  RET_ERR_NO_DELETE       2008     //��ֹɾ��
#define  RET_ERR_NO_RIGHT        2009     //û��Ȩ�޲���
#define  RET_ERR_PWD             2010     //�����������Ա������
#define  RET_ERR_PWDLOCK         2011     //��������
#define  RET_ERR_CERTNO          2012     //����֤�鲻��
#define  RET_ERR_CLEARDATE       2013     //�������ڲ���
#define  RET_ERR_REPEATIMPORT    2014     //�ظ�����
#define  RET_ERR_CONNECTDB       2015     //�������ݿ����
#define  RET_ERR_DELETECLEARDATA  2016     //ɾ���������ݳ���
#define  RET_ERR_IMPORT           2017     //�������
#define  RET_ERR_TRADEDATE        2018     //�������ڲ���
#define  RET_ERR_TXACC            2019     //�ʽ��˺ŷǷ�
#define  RET_ERR_CLEARSTATUS      2020     //����״̬����
#define  RET_ERR_CLEARPRICE       2021     //���½���۳���
#define  RET_ERR_DATEINPUT        2022     //�������벻��
#define  RET_ERR_CONTRACTINPUT    2023     //��Լ���벻��
#define  RET_ERR_CUSTTXACC        2024     //�ͻ����ʽ��˺Ų�ƥ��
#define  RET_ERR_MARKET           2025     //�г��Ƿ�
#define  RET_ERR_UPORG            2026     //�ϼ������Ƿ�
#define  RET_ERR_PRODACC          2027     //��Ʒ�����˺Ų�����
#define  RET_ERR_SYSTEMPARA       2028     //ϵͳ�������ô�
#define  RET_ERR_OPTIONS          2029     //������Ȩ����
#define  RET_ERR_POSITION_NOTFOUND   2030     //�޳ֲ�/�ֲֲ��� �޿��/��治��
#define  RET_ERR_GETRISKLEVEL    2031     //ȡ���ռ������
#define  RET_ERR_CALCRISK        2032     //������ճ���
#define  RET_ERR_FEESET          2033     //���������ó���
#define  RET_ERR_MARGINSET          2034     //��֤�����ó���
#define  RET_ERR_POSITION_AVAIL      2035     //���㹻�ֲ�
#define  RET_ERR_CUST                 2036     //�ͻ�֤�����ֻ��Ų�һ��  ����֤�����ֻ����ҵ�����ͻ�
#define  RET_ERR_FEETEMPLET_NOTFOUND  2037     //����ģ�岻����
#define  RET_ERR_ORG_NOTFOUND            2038     //����������
#define  RET_ERR_TRANSHISDATA            2039     //ת��ʷ�����
#define  RET_ERR_SHFLAG                  2040     //Ͷ����־����
#define  RET_ERR_EOFLAG                  2041     //��ƽ��־����
#define  RET_ERR_BSFLAG                  2042     //������־����
#define  RET_ERR_TXTYPE                  2043     //�������ʹ���
#define  RET_ERR_ORDERTYPE               2044     //�������ʹ���
#define  RET_ERR_ORDERATTR               2045     //�������Դ���/�ɽ����Դ���
#define  RET_ERR_ORDERPRICE               2046     //ί�м۴���
#define  RET_ERR_EXCHANGE                 2047     //��������Ϣ������
#define  RET_ERR_CHANNEL                  2048     //��������
#define  RET_ERR_CUSTOPER                 2049     //�ͻ�/����Ա����
#define  RET_ERR_CUSTFUNDACC              2050     //�ʽ��˺ŷǷ�
#define  RET_ERR_CUSTNO                   2051     //�ͻ��ŷǷ���������
#define  RET_ERR_CHANNELOPEN               2052     //δ��ͨ������
#define  RET_ERR_TXORG                     2053     //�������㲻����

#define  RET_ERR_ORDERTYPE_SUPPORT              2054     //�������Ͳ�֧��
#define  RET_ERR_ORDERATTR_SUPPORT              2055     //��������+�ɽ����Բ�֧��
#define  RET_ERR_CUSTFUND                       2056     //���ʽ�����
#define  RET_ERR_TRADECODE                       2057     //�޽��ױ���
#define  RET_ERR_GETFEE                         2058     //ȡ�����ѳ���
#define  RET_ERR_GETMARGIN                      2059     //����֤�����
#define  RET_ERR_AVAILFUND                      2060     //�����ʽ���
#define  RET_ERR_CLEARFLAG                      2061     //�����־����������Ϊ���㲻�����ݵ���
#define  RET_ERR_CLEARFLAG                      2061     //�����־����������Ϊ���㲻�����ݵ���
#define  RET_ERR_TRADERIGHT                     2062     //����Ȩ�޲���
#endif // !defined(AFX_CODETABLE_H__CD822933_07A9_4A62_B9F2_0791BE0E5A56__INCLUDED_)
