/// InterfacePack.h: interface for the CInterfacePack class.
///
/////////////////////////////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_INTERFACEPACK_H__ACF796B0_9EC2_47FD_B214_7438ADC630FA__INCLUDED_)
#define AFX_INTERFACEPACK_H__ACF796B0_9EC2_47FD_B214_7438ADC630FA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif /// _MSC_VER > 1000

#include "public.h"
#include "attribute.h"
#include "node.h"
#include "xmlanalysis.h"
#include "xml.h"
//#include "pubstrtime.h"
#include "BF_Tools.h"
#include "BF_SimpleLog.h"
#include "BF_Slist.h"



#ifdef _WINDOWS
#ifdef _DEBUG
#pragma comment(lib, "bf_kerneld.lib") 
#pragma comment(lib, "bf_oldxmld.lib") 
#pragma message("Automatically linking with  bf_kerneld.lib bf_oldxmld.lib")
#else
#pragma comment(lib, "bf_kernel.lib") 
#pragma comment(lib, "bf_oldxml.lib") 
#pragma message("Automatically linking with  bf_kernel.lib bf_oldxml.lib")
#endif
#endif

/*接口包格式
<?xml version="1.0" encoding="GB2312"?>
<!-- edited with XML Spy v4.2 U (http:///www.xmlspy.com) by lock (AIsys) -->
<interface>
	<dict>
		<密码验证标志>0验1不验不等于1验</密码验证标志>
		<证件类型>0:身份证1:护照2:军官证3:士兵证4:回乡证6:户口簿7:其他8:无9:警官证</证件类型>
		<成功标志>0成功1失败9未知</成功标志>
		<币种>000人民币001港币002美元003日元004欧元</币种>
		<账户类型>0:对私账号1:对私卡2:对公账号3:对公卡</账户类型>
	</dict>
	<conv>
		<separator>分隔符</separator>
		<xpack>xpack包</xpack>
		<xml>xml包</xml>
		<standard>
			<no1 name="定长包" type="char" length="16" offset="100" left="yes" trans="1" var="~1" update="yes"/>
			<no2 name="定长包" type="integer" length="10" offset="10" left="no" trans="0" var="@11" update="no"/>
			<no3 name="定长包" type="double" length="2" left="no"/>
		</standard>
	</conv>
	<type>
		<char/>
		<integer/>
		<double/>
	</type>
	<trans>
	    <不作翻译>0</不作翻译>
		<要作翻译>1</要作翻译>
	</trans>
	<var>
	    <翻译的常量>~</翻译的常量>
		<翻译的变量或方法>@</翻译的变量或方法>
	</var>
	<update>
	    <更新请求xml内容>yes</更新请求xml内容>
	    <不更新请求xml内容>no</不更新请求xml内容>
	</update>
	<交易码 conv="standard" separator="|" offset="20">10</交易码>
	<翻译方法>
	    <transBUYFLAG conv="0">
	        <b>0</b>
	        <s>1</s>
	        <default>0</default>
	    </transBUYFLAG>
	    <transBUYFLAGNAME conv="0">
	        <b>买入</b>
	        <s>卖出</s>
	        <default>买入</default>
	    </transBUYFLAGNAME>
	    <transEOFLAG conv="0">
	        <0>0</0>
	        <1>0</1>
	        <default>0</default>
	    </transEOFLAG>
	    <transEOFLAGNAME conv="0">
	        <0>开仓</0>
	        <1>平仓</1>
	        <default>无</default>
	    </transEOFLAGNAME >
		<transDONEQTY conv="1">
			<v1>lvol0</v1>
			<v2>-</v2>
			<v3>lvol2</v3>
		</transDONEQTY>
	<翻译方法/>
	<包长度>5</包长度>
	<包头>
		<包头0 name="定长包配置范例">
		    <报文标识 type="char" offset="0" length="5" left="no" trans="0" var="" trim="1"></报文标识>
		    <交易码 type="char"   offset="5" length="6" left="yes" trans="1" var="@transtxcode"></交易码>
		    <日期 type="char" offset="11" length="8" left="no" trans="0" var=""></日期>
		    <时间 type="char" offset="19" length="8" left="no" trans="0" var=""></时间>
		    <银行地区 type="char" offset="27" length="8" left="no" trans="0" var=""></银行地区>
		    <银行网点 type="char" offset="35" length="8" left="no" trans="0" var=""></银行网点>
		    <银行柜员 type="char" offset="43" length="8" left="no" trans="0" var=""></银行柜员>
		    <交易序号 type="char" offset="51" length="8" left="no" trans="0" var=""></交易序号>
		    <终端号 type="char" offset="59" length="6" left="no" trans="0" var=""></终端号>
		    <请求来源 type="char" offset="65" length="8" left="no" trans="0" var=""></请求来源>
		    <终端类型 type="char" offset="73" length="8" left="no" trans="0" var=""></终端类型>
		    <后续标志 type="char" offset="81" length="8" left="no" trans="0" var=""></后续标志>
		    <原报文时间 type="char" offset="89" length="8" left="no" trans="0" var=""></原报文时间>
		</包头0>
		    
		<包头1 name="分隔符配置范例">
			<报文标识 type="char" offset="0" length="5" left="no" trans="0" var="" ></报文标识>
			<交易码 type="char"   offset="1" length="6" left="yes" trans="1" var="@transtxcode"></交易码>
			<日期 type="char" offset="2" length="8" left="no" trans="0" var=""></日期>
			<时间 type="char" offset="3" length="8" left="no" trans="0" var=""></时间>
			<银行地区 type="char" offset="4" length="8" left="no" trans="0" var=""></银行地区>
			<银行网点 type="char" offset="5" length="8" left="no" trans="0" var=""></银行网点>
			<银行柜员 type="char" offset="6" length="8" left="no" trans="0" var=""></银行柜员>
			<交易序号 type="char" offset="7" length="8" left="no" trans="0" var=""></交易序号>
			<终端号 type="char" offset="8" length="8" left="no" trans="0" var=""></终端号>
			<请求来源 type="char" offset="9" length="8" left="no" trans="0" var=""></请求来源>
			<终端类型 type="char" offset="10" length="8" left="no" trans="0" var=""></终端类型>
			<后续标志 type="char" offset="11" length="8" left="no" trans="0" var=""></后续标志>
			<原报文时间 type="char" offset="12" length="8" left="no" trans="0" var=""></原报文时间>
		</包头1>
	</包头>
	<公共错误返回>
		<返回信息 type="char" trans="0">vsmess</返回信息>
	<公共错误返回/>
	<func_list>
		<f8612 name="资金冻结解冻" conv="separator" separator="|" author="songfree" oldfunc="">
			<请求 ret="1">
			    <include>包头1</include>
				<银行账号 type="char" offset="13" length="23" left="yes" trans="0" var="" trim="1"/>
				<密码检查标志 type="char" offset="14" length="1" left="yes" trans="0" var=""/>
				<转账金额 type="double" offset="15" length="16" dec="2" left="no" trans="0" var=""/>
			</请求>
			<返回码0 code="0" conv="separator" separator="|" file="1">
			    <include>包头1</include>
			</返回码0>
		</f8612>
		<f8613 name="保证金出金" conv="standard"  author="songfree" oldfunc="">
			<请求 ret="1">
				<include>包头0</include>
				<银行账号 type="char" offset="97" length="23" left="yes" trans="0" var="" trim="1"/>
				<密码检查标志 type="char" offset="120" length="1" left="yes" trans="0" var=""/>
				<转账金额 type="double" offset="121" length="16" dec="2" left="no" trans="0" var=""/>
			</请求>
			<返回码0 code="0" conv="separator" separator="|" file="1">
				<include>包头1</include>
			</返回码0>
		</f8613>
		<f690613 name="日终对账-对账" conv="xpack" author="songfree" oldfunc="330312">
			<请求 ret="1">
				<日期 type="char" trans="0" >sdate0</日期>
				<应用代码 type="char" trans="0" >vsmess</应用代码>
				<操作员 type="char" trans="0">sorder0</操作员>
			</请求>
			<返回码0 code="0" conv="xpack" >
				<本地流水号 type="char" trans="0">sserial0</本地流水号>
				<银行账号 type="char" trans="0">sbank_acc</银行账号>
				<应用客户号 type="char" trans="0">sholder_ac_no2</应用客户号>
				<应用代码 type="char" trans="0">vsmess</应用代码>
				<日期 type="char" trans="0">sdate0</日期>
				<时间 type="char" trans="0">stime0</时间>
				<转账金额 type="double" trans="0">damt0</转账金额>
				<币种 type="char" trans="0">sbranch_code2</币种>
				<出入金状态 type="char" trans="0">sstatus0</出入金状态>
				<本地柜台标志 type="char" trans="0">smain_flag</本地柜台标志>
				<资金操作类型 type="char" trans="0">smain_flag2</资金操作类型>
				<应用流水号 type="char" trans="0">sserial4</应用流水号>
				<发起方 type="char" trans="0">sstatus1</发起方>
				<返回信息 type="char" trans="0">vsmess</返回信息>
			</返回码0>
		</f690613>
	</func_list>
</interface>
*/

typedef struct _func_attr
{
	int  funcNo;///功能号
	int  conv_type;///类型 0 xml 1xpack 2 分隔符 3定长
	char separator[3];///分隔符
}S_FUNC_ATTR;///功能转换属性

typedef struct _funcfieldinfo
{
	std::string name;///名称
	int fieldtype;///类型 0-char 1-integer 2-double
	int fieldlength;///长度
	int fieldoffset;///此字段所在buf位移(定长为位移、分隔为序号)
	bool isTrans;///是否翻译
	bool isVar;///是否变量
	bool isTrim;///是否去除空格
	std::string transValue;///翻译字段定义
	bool isupdate;///是否更新xml请求字段
	int left;///左右对齐 0-左对 1右对
	int  conv_type;///类型 0 xml 1xpack 2 分隔符 3定长
	int  dec;///小数位数
	char separator[3];///分隔符
	std::string value;///值
	std::string include;///包含的包头
}S_FUNC_FIELD_INFO; ///字段属性

typedef struct 
{
	int retNum;///返回包数目
	std::vector<S_FUNC_FIELD_INFO> funcReq;///请求包字段列表
}S_FUNC_REQ_FIELD_LIST; ///请求包内容

typedef struct 
{
	int retCode;///返回码
	int  conv_type;///返回包类型 0 xml 1xpack 2 分隔符 3定长
	char separator[3];///返回包分隔符
	int  fileFlag;///返回包是否写到文件
	char include[100];///应答是否单独生成包头定长缓冲
	std::vector<S_FUNC_FIELD_INFO> funcRes;///返回包字段列表
}S_FUNC_RES_FIELD_LIST; ///应答包内容

typedef struct 
{
	S_FUNC_ATTR funcAttr;
	S_FUNC_REQ_FIELD_LIST funcReq;///请求段内容
	std::vector<S_FUNC_RES_FIELD_LIST> funcRes;///返回段内容
}S_FUNC_CONF_INFO;  ///功能交易配置信息

typedef struct 
{
	std::string headName;///包头名称
	int    headLength;///包头长度
	std::vector<S_FUNC_FIELD_INFO> headinfo;///包头信息
}S_PACKHEAD_INFO; ///include包头信息

typedef struct 
{
	std::string transName;///翻译名称
	std::string conv;///翻译标志0文字翻译1运算
	std::vector<S_FUNC_FIELD_INFO> transinfo;///翻译信息
}S_TRANS_INFO; ///翻译包结构信息


#ifdef DPTOOL_EXPORTS
#define DPTOOL_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define DPTOOL_EXPORT __declspec(dllimport)
#else
#define DPTOOL_EXPORT 
#endif
#endif
class DPTOOL_EXPORT CInterfacePack  
{
public:
	CInterfacePack();
	virtual ~CInterfacePack();
	std::vector <S_TRANS_INFO> m_trans_info;///所有的翻译信息
	std::vector <S_PACKHEAD_INFO> m_include_head;///所有的include头
	std::vector <S_FUNC_CONF_INFO> m_func_conf_info;///所有功能配置信息
	S_FUNC_FIELD_INFO m_txcode_info;///取交易码节点信息
	
	CXML m_xml;        /*xml解析数据类*/
	char m_errMsg[1025];
	CBF_SimpleLog m_publog;
protected:
	/// 函数名: readPack
	/// 编程  : 王明松 2006-6-8 13:00:12
	/// 返回  : bool 
	/// 描述  : 从xml中读取接口配置信息
	bool readPack();
	/// 函数名: getField
	/// 编程  : 王明松 2006-6-8 12:55:56
	/// 返回  : int 
	/// 参数  : CNode *fieldnode 节点
	/// 参数  : string &name     节点名称
	/// 参数  : int &type        节点类型
	/// 参数  : int &length      节点长度
	/// 参数  : int &left        对齐标志
	/// 参数  : string &value    节点值
	/// 描述  : 取一个配置节点的值及属性
	bool getField(CNode *fieldnode,S_FUNC_FIELD_INFO *fieldpack);

	/// 函数名: readHead
	/// 编程  : 王明松 2006-6-8 12:56:59
	/// 返回  : bool 
	/// 参数  : CNode *m_readnode
	/// 描述  : 读取包头节点数据至m_include_head
	bool readHead(CNode *readnode);

	/// 函数名: readTrans
	/// 编程  : 王明松 2006-6-12 17:02:27
	/// 返回  : bool 
	/// 参数  : CNode *transnode
	/// 描述  : 读取翻译段信息
	bool readTrans(CNode *transnode);

	/// 函数名: readFunc
	/// 编程  : 王明松 2006-6-8 12:57:33
	/// 返回  : int 
	/// 参数  : CNode *funclistnode
	/// 描述  : 读取功能配置信息至m_func_conf_info
	bool readFunc(CNode *funclistnode);

	/// 函数名: *rtrim
	/// 编程  : 王明松 2006-6-8 13:01:32
	/// 返回  : char 
	/// 参数  : char *str
	/// 描述  : 去掉右空格
	char *rtrim(char *str);

	/// 函数名: *ltrim
	/// 编程  : 王明松 2006-6-8 13:01:35
	/// 返回  : char 
	/// 参数  : char *str
	/// 描述  : 去掉左空格
	char *ltrim(char *str);

	/// 函数名: trim
	/// 编程  : 王明松 2006-6-8 13:01:39
	/// 返回  : int 
	/// 参数  : char *s
	/// 描述  : 去掉空格
	int trim(char *s);

	/// 函数名: trim
	/// 编程  : 王明松 2006-6-8 13:01:43
	/// 返回  : int 
	/// 参数  : string &s
	/// 描述  : 去掉空格
	int trim(std::string &s);
	
protected:
	CNode *m_rootnode;/*xml根节点*/
	CNode *m_headnode;/*包头节点*/
	CNode *m_funclistnode;/*功能列表节点*/
	CNode *m_transnode;///翻译节点
	CNode *m_txcodenode;///取交易码的节点
public:
	bool resetField(S_FUNC_FIELD_INFO *field);

	/// 函数名: getTxCode
	/// 编程  : 王明松 2006-8-2 17:16:40
	/// 返回  : int 
	/// 参数  : char *buf
	/// 参数  : int buflen
	/// 参数  : int &txcode
	/// 描述  : 根据配置信息从数据缓冲中取出交易码
	int getTxCode(char *buf,int buflen,int &txcode);


	/// 函数名: getTransConv
	/// 编程  : 王明松 2006-6-12 17:33:54
	/// 返回  : string 
	/// 参数  : string transname
	/// 描述  : 获取翻译名称的属性   0转换 1加减乘除计算
	std::string getTransConv(std::string transname);

	/// 函数名: getTransString
	/// 编程  : 王明松 2006-6-12 17:27:06
	/// 返回  : string  翻译后的值
	/// 参数  : string transname 翻译名称
	/// 参数  : string sourceValue 要翻译的字段值
	/// 描述  : 获取翻译后的值
	std::string getTransString(std::string transname,std::string sourceValue);

	/// 函数名: *getHeadInfo
	/// 编程  : 王明松 2006-8-2 17:13:12
	/// 返回  : S_PACKHEAD_INFO  
	/// 参数  : string headname
	/// 描述  : 根据包头名字取包头信息
	S_PACKHEAD_INFO  *getHeadInfo(std::string headname);

	/// 函数名: *getFuncInfo
	/// 编程  : 王明松 2006-8-2 17:13:37
	/// 返回  : S_FUNC_CONF_INFO 
	/// 参数  : int txcode
	/// 描述  : 根据交易码取交易配置信息
	S_FUNC_CONF_INFO *getFuncInfo(int txcode);
	/// 函数名: fromFile
	/// 编程  : 王明松 2006-6-8 12:58:32
	/// 返回  : int 
	/// 参数  : char *filename
	/// 描述  : 从接口配置文件读取接口配置信息
	int fromFile(char *filename);
	///获取错误信息
	char *getMsg();

	/// 函数名: fromBuf
	/// 编程  : 王明松 2006-6-8 12:59:50
	/// 返回  : bool 
	/// 参数  : char *xmlBuf
	/// 描述  : 从数据缓冲读取接口配置信息
	bool fromBuf(char *xmlBuf);
};

#endif /// !defined(AFX_INTERFACEPACK_H__ACF796B0_9EC2_47FD_B214_7438ADC630FA__INCLUDED_)
