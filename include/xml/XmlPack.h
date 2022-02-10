/// XmlPack.h: interface for the CXmlPack class.
///
/////////////////////////////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_XMLPACK_H__C509FC3C_B8B1_4911_A3F6_96C274BC9748__INCLUDED_)
#define AFX_XMLPACK_H__C509FC3C_B8B1_4911_A3F6_96C274BC9748__INCLUDED_

#include "public.h"
#include "attribute.h"
#include "node.h"
#include "xmlanalysis.h"
#include "xml.h"


/*xml包格式
<package>
	<head>
		<功能序列>302003|</功能序列>
        <应用序列>2|</应用序列>
		<DRTPID> 100  </DRTPID>
		<DRTPPORT> 9000  </DRTPPORT>
		<DRTPIP> 172.28.10.90  </DRTPIP>
		<SOURCENO> 0  </SOURCENO>
		<BCCFUNC> 10000  </BCCFUNC>
		<业务流水号> 0  </业务流水号>
		<业务步骤> 0  </业务步骤>
		<总记录数> 1  </总记录数>
		<本包记录数> 1  </本包记录数>
		<返回码> 0 </返回码>
		<NEXTFLAG> 0  </NEXTFLAG>
		<FIRSTFLAG> 0  </FIRSTFLAG>
		<BINFLAG>0</BINFLAG> ///二进制数据标志
		<BINLENGTH>0</BINLENGTH> ///二进制数据长度
	</head>
	<pack>
		<银行帐号> 9558801001  </银行帐号>
		<客户号> 9000000123  </客户号>
		<买卖方向> 1  </买卖方向>
		<手数> 10  </手数>
		<价格> 140  </价格>
		<合约名称> 99.99金  </合约名称>
		<合约代码> Au99.99  </合约代码>
	</pack>
</package>
*/
#if _MSC_VER > 1000
#pragma once
#endif /// _MSC_VER > 1000

#ifdef XML_EXPORTS
#define XML_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define XML_EXPORT __declspec(dllimport)
#else
#define XML_EXPORT 
#endif
#endif
#define MAXBUF 8192


class CXmlPackArray;
class XML_EXPORT CXmlPack  
{
public:   ///兼容bizinfo的方法

	/// 函数名: GetPackMuValueCount
	/// 编程  : 王明松 2008-7-3 14:33:53
	/// 返回  : int <0表示没有此节点 =0表示没下属节点
	/// 参数  : std::string name
	/// 描述  : 获取指定pack节点的节点名为name的下属节点数
	int GetPackMuValueCount(std::string name);

	/// 函数名: GetPackMuValue
	/// 编程  : 王明松 2008-7-3 14:24:35
	/// 返回  : int <0 失败 =0成功
	/// 参数  : std::string name pack里的字段名称
	/// 参数  : int recNo        记录数 从0开始
	/// 参数  : std::string &value   值
	/// 描述  : 从pack里的字段取出下属多个节点值，recNo标识为第几条记录
	int GetPackMuValue(std::string name,int recNo,std::string &value);


	/// 函数名: SetPackMuValue
	/// 编程  : 王明松 2008-7-3 14:52:50
	/// 返回  : CNode * NULL失败
	/// 输入参数  : std::string name
	/// 输入参数  : std::string value
	/// 输入参数  : bool istrans=true   若为false不进行转义符转换，否则进行转义符转换
	/// 描述  : 设置多记录节点  pack->name->record多个
	CNode *SetPackMuValue(std::string name,std::string value,bool istrans=true);


public: ///xmlpack方法

	/// 函数名: getXmlDataLen
	/// 编程  : 王明松 2008-7-3 14:21:26
	/// 返回  : int 数据长度
	/// 描述  : 获取xml的数据长度
	int getXmlDataLen();

	/// 函数名: tobuffer
	/// 编程  : 王明松 2008-6-19 19:00:34
	/// 返回  : bool 
	/// 参数  : char *buffer
	/// 参数  : int &bufsize 数据缓冲长度，若长度不够返回false,成功为数据长度
	/// 描述  : xml或二进制数据到数据缓冲
	bool tobuffer(char *buffer,int &bufsize);

	/// 函数名: XmlCopy
	/// 编程  : 王明松 2008-5-23 14:45:01
	/// 返回  : bool 
	/// 参数  : CXmlPack *xmlpack
	/// 描述  : xmlpack进行复制，和源xmlpack完全一致,xmlpack不变
	bool XmlCopy(CXmlPack *xmlpack);


	/// 函数名: XmlPointCopy
	/// 编程  : 王明松 2009-1-20 9:34:08
	/// 返回  : bool 
	/// 参数  : CXmlPack *xmlpack
	/// 描述  : 将目标xmlpack的指针复制到本xml中，xmlpack置为空
	bool XmlPointCopy(CXmlPack *xmlpack);


	/// 函数名: readXmlPack
	/// 编程  : 王明松 2006-7-3 14:53:49
	/// 返回  : bool 
	/// 参数  : char *xmlBuf
	/// 描述  : 从xmlBuf解析xmlpack的数据包
	bool readXmlPack(char *xmlBuf);



	/// 函数名: getNextFlag
	/// 编程  : 王明松 2007-4-30 10:22:08
	/// 返回  : bool 
	/// 描述  : 获取后续包标志
	bool getNextFlag();

	/// 函数名: setNextFlag
	/// 编程  : 王明松 2007-4-30 10:21:56
	/// 返回  : bool 
	/// 参数  : bool isnext=false
	/// 描述  : 设置是否有后续包
	bool setNextFlag(bool isnext=false);

	/// 函数名: getFirstFlag
	/// 编程  : 王明松 2007-4-30 10:21:27
	/// 返回  : bool 
	/// 描述  : 设置第一个请求包标志
	bool getFirstFlag();

	/// 函数名: setFirstFlag
	/// 编程  : 王明松 2007-4-30 10:21:01
	/// 返回  : bool 
	/// 参数  : bool isfirst=true
	/// 描述  : 设置是否第一个请求包
	bool setFirstFlag(bool isfirst=true);

	/// 函数名: getTotalRec
	/// 编程  : 王明松 2007-4-30 10:20:50
	/// 返回  : bool 
	/// 参数  : int &rec
	/// 描述  : 获取总共记录数
	bool getTotalRec(int &rec);

	/// 函数名: setTotalRec
	/// 编程  : 王明松 2007-4-30 10:20:34
	/// 返回  : bool 
	/// 参数  : int rec
	/// 描述  : 设置总共记录数
	bool setTotalRec(int rec);

	/// 函数名: getRecCount
	/// 编程  : 王明松 2007-4-30 10:20:19
	/// 返回  : bool 
	/// 参数  : int &rec
	/// 描述  : 获取本包记录数
	bool getRecCount(int &rec);

	/// 函数名: setRecCount
	/// 编程  : 王明松 2007-4-30 10:19:54
	/// 返回  : bool 
	/// 参数  : int rec
	/// 描述  : 设置本包记录数
	bool setRecCount(int rec);

	/// 函数名: getRetMsg
	/// 编程  : 王明松 2007-4-30 10:19:44
	/// 返回  : bool 
	/// 参数  : char *msg
	/// 描述  : 获取返回信息
	bool getRetMsg(char *msg);

	/// 函数名: getRetCode
	/// 编程  : 王明松 2007-4-30 10:19:34
	/// 返回  : bool 
	/// 参数  : int &retCode
	/// 描述  : 获取返回码
	bool getRetCode(int &retCode);

	/// 函数名: getRetCode
	/// 编程  : 王明松 2007-4-30 10:19:34
	/// 返回  : bool 
	/// 参数  : char *retCode
	/// 描述  : 获取返回码
	bool getRetCode(char *retCode);

	/// 函数名: setReturn
	/// 编程  : 王明松 2007-4-30 9:57:57
	/// 返回  : bool 
	/// 参数  : int retCode
	/// 参数  : const char *msg
	/// 描述  : 设置返回码返回信息
	bool setReturn(int retCode,const char *msg);

	/// 函数名: setReturn
	/// 编程  : 王明松 2007-4-30 9:57:57
	/// 返回  : bool 
	/// 参数  : const char * retCode
	/// 参数  : const char *msg
	/// 描述  : 设置返回码返回信息
	bool setReturn(const char *retCode,const char *msg);

	/// 函数名: setRetMsg
	/// 编程  : 王明松 2007-4-30 9:57:45
	/// 返回  : bool 
	/// 参数  : const char *msg
	/// 描述  : 设置返回信息
	bool setRetMsg(const char *msg);

	/// 函数名: setRetCode
	/// 编程  : 王明松 2007-4-30 9:57:34
	/// 返回  : bool 
	/// 参数  : int retCode
	/// 描述  : 设置返回码
	bool setRetCode(int retCode);
	
	/// 函数名: setRetCode
	/// 编程  : 王明松 2007-4-30 9:57:34
	/// 返回  : bool 
	/// 参数  : const char * retCode
	/// 描述  : 设置返回码
	bool setRetCode(const char *retCode);

	/// 函数名: getBinData
	/// 编程  : 王明松 2006-6-9 12:38:40
	/// 返回  : bool 
	/// 参数  : char *data
	/// 参数  : int &datalen
	/// 描述  : 取二进制数据
	bool getBinData(char *data,int &datalen);

	/// 函数名: setBinData
	/// 编程  : 王明松 2006-6-9 12:25:08
	/// 返回  : bool 
	/// 参数  : char *data
	/// 参数  : int datalen
	/// 描述  : 设置二进制数据
	bool setBinData(char *data,int datalen);

	/// 函数名: setBin
	/// 编程  : 王明松 2006-7-3 15:00:48
	/// 返回  : bool 
	/// 参数  : bool binflag
	/// 描述  : 设置是否二进制格式 true是二进制
	bool setBin(bool binflag);

	/// 函数名: isBin
	/// 编程  : 王明松 2006-7-3 15:00:35
	/// 返回  : bool 
	/// 描述  : 是否是二进制格式
	bool isBin();
	/// 函数名: modiPack
	/// 编程  : 王明松 2006-6-8 17:24:09
	/// 返回  : bool 
	/// 参数  : CXmlPack *xmlpack  输入
	/// 参数  : bool isUpdate  是否update原pack相同字段
	/// 描述  : 用参数xmlpack里的pack字段更新本包 isUpdate是否update原pack相同字段
	bool modiPack(CXmlPack *xmlpack,bool isUpdate);

	/// 函数名: xmlCopyNv
	/// 编程  : 王明松 2006-6-6 13:42:47
	/// 返回  : bool 
	/// 参数  : CXmlPack *xmlpack 输入
	/// 描述  : copy参数xmlpack里的head和pack节点的name及value到本包
	bool xmlCopyNV(CXmlPack *xmlpack);
	

	/// 函数名: deletePack
	/// 编程  : 王明松 2006-7-3 15:01:49
	/// 返回  : int 
	/// 描述  : 删除pack节点里的子节点，即清空pack里的数据
	int deletePack();

	/// 函数名: gotoPack
	/// 编程  : 王明松 2006-7-3 15:01:27
	/// 返回  : int 
	/// 描述  : 当前节点到pack节点
	int gotoPack();

	/// 函数名: gotoHead
	/// 编程  : 王明松 2006-7-3 15:01:12
	/// 返回  : int 
	/// 描述  : 当前节点到head节点
	int gotoHead();
	
	/// 函数名: newPackage
	/// 编程  : 王明松 2006-7-3 15:02:25
	/// 返回  : virtual int 
	/// 描述  : 生成一个全新的xml包
	virtual int newPackage();


	/// 函数名: trim
	/// 编程  : 王明松 2006-7-3 15:02:41
	/// 返回  : int 
	/// 参数  : std::string &s
	/// 描述  : 去掉空格
	int trim(std::string &s);

	/// 函数名: *rtrim
	/// 编程  : 王明松 2008-7-3 15:02:54
	/// 返回  : char *
	/// 参数  : char *str
	/// 描述  : 去掉右空格
	char *rtrim(char *str);

	/// 函数名: *ltrim
	/// 编程  : 王明松 2008-7-3 15:03:07
	/// 返回  : char 
	/// 参数  : char *str
	/// 描述  : 去掉左空格
	char *ltrim(char *str);

	/// 函数名: trim
	/// 编程  : 王明松 2008-7-3 15:03:17
	/// 返回  : int 
	/// 参数  : char *s
	/// 描述  : 去掉所有空格
	int trim(char *s);

	/// 函数名: getPackValueChar
	/// 编程  : 王明松 2006-7-3 15:03:34
	/// 返回  : int <0无此节点 =0成功
	/// 参数  : const char *name
	/// 参数  : char *value
	/// 描述  : 取pack里的name节点的值,去掉空格,当前节点至此节点
	int getPackValueChar(const char *name, char *value);
	///取head值,去掉空格

	/// 函数名: getHeadValueChar
	/// 编程  : 王明松 2006-7-3 15:04:06
	/// 返回  : int <0无此节点 =0成功
	/// 参数  : const char *name
	/// 参数  : char *value
	/// 描述  : 取head值,去掉空格,当前节点至此节点
	int getHeadValueChar(const char *name, char *value);

	/// 函数名: getPackValue
	/// 编程  : 王明松 2006-7-3 15:04:58
	/// 返回  : int <0无此节点 =0成功
	/// 参数  : const std::string name
	/// 参数  : std::string &value
	/// 参数  : bool istrim=true
	/// 描述  : 取pack里的name节点的值,去掉空格,当前节点至此节点
	int getPackValue(const std::string name,std::string &value,bool istrim=true);
	int getPackValue(const std::string name,double &value);
	int getPackValue(const std::string name,int &value);
	int getPackValueI(const std::string name);
	double getPackValueD(const std::string name);

	
	std::string getPackValueS(const std::string name,bool istrim=false);


	/// 函数名: getHeadValue
	/// 编程  : 王明松 2006-7-3 15:05:47
	/// 返回  : int 
	/// 参数  : const std::string name
	/// 参数  : double &value
	/// 描述  : 取head里的name节点的值,去掉空格,当前节点至此节点
	int getHeadValue(const std::string name,double &value);
	int getHeadValue(const std::string name,int &value);
	int getHeadValue(const std::string name,std::string &value,bool istrim=true);
	int getHeadValueI(const std::string name);
	double getHeadValueD(const std::string name);
	
	std::string getHeadValueS(const std::string name,bool istrim=false);

	/// 函数名: getPackValue
	/// 编程  : 王明松 2006-6-27 9:37:12
	/// 返回  : char * 
	/// 参数  : const string name
	/// 描述  : 获取char *值，注意不能在sprintf()中使用多个此方法！ 当前节点至此节点
	char * getPackValue(const std::string name);


	/// 函数名: *getHeadValue
	/// 编程  : 王明松 2006-7-3 15:06:35
	/// 返回  : char 
	/// 参数  : const std::string name
	/// 描述  :  获取char *值，注意不能在sprintf()中使用多个此方法！ 当前节点至此节点
	char *getHeadValue(const std::string name);




public: ///多包pack1 pack2的方法

	/// 函数名: newPackFromMuPack
	/// 编程  : 王明松 2007-4-30 11:27:02
	/// 返回  : bool 
	/// 参数  : CXmlPackArray muxmlpack
	/// 描述  : 从多包类里生成多记录
	bool newPackFromMuPack(CXmlPackArray muxmlpack);
	

	/// 函数名: newPackFromMuPackPoint
	/// 编程  : 王明松 2009-1-20 16:09:29
	/// 返回  : bool 
	/// 参数  : CXmlPackArray muxmlpack
	/// 描述  : 从多包类里生成多记录,将muxmlpack里的指针复制到本包，muxmlpack原指针置空
	bool newPackFromMuPackPoint(CXmlPackArray muxmlpack);
	/// 函数名: addMuPack
	/// 编程  : 王明松 2007-4-30 11:23:21
	/// 返回  : bool 
	/// 参数  : CXmlPackArray muxmlpack
	/// 描述  : 增加多包类里的记录到本包  pack pack1 pack2等
	bool addMuPack(CXmlPackArray muxmlpack);


	/// 函数名: addMuPackPoint
	/// 编程  : 王明松 2009-1-20 16:09:58
	/// 返回  : bool 
	/// 参数  : CXmlPackArray muxmlpack
	/// 描述  : 增加多包类里的记录到本包(指针复制方式muxmlpack原指针置空) pack pack1 pack2等
	bool addMuPackPoint(CXmlPackArray muxmlpack);
	
	/// 函数名: getMuPack
	/// 编程  : 王明松 2007-4-30 11:19:21
	/// 返回  : bool 
	/// 参数  : CXmlPackArray *muxmlpack 输出
	/// 描述  : 取所有的记录到muxmlapck类中 pack pack1 pack2等
	bool getMuPack(CXmlPackArray *muxmlpack);


	/// 函数名: getMuPackPoint
	/// 编程  : 王明松 2009-1-20 16:03:32
	/// 返回  : bool 
	/// 参数  : CXmlPackArray *muxmlpack
	/// 描述  : 将本包的多pack取到muxmlpack中，指针的方式，原指针从链表删除
	bool getMuPackPoint(CXmlPackArray *muxmlpack);
	/// 函数名: getPack
	/// 编程  : 王明松 2007-4-30 10:32:10
	/// 返回  : bool 
	/// 参数  : int recNo  记录id  从1开始
	/// 参数  : CXmlPack *xmlpack  输出
	/// 描述  : 从本包获取多包记录到xmlpack中，xmlpack重新new一个，并从源里取head返回码
	bool getPack(int recNo,CXmlPack *xmlpack);


	/// 函数名: getPackPoint
	/// 编程  : 王明松 2009-1-20 15:56:23
	/// 返回  : bool 
	/// 参数  : int recNo
	/// 参数  : CXmlPack *xmlpack
	/// 描述  : 从本包获取多包记录到xmlpack中(指针方式，本包记录删除)，xmlpack要求是xmlpack的格式
	bool getPackPoint(int recNo,CXmlPack *xmlpack);
	
	/// 函数名: addPack
	/// 编程  : 王明松 2007-4-30 10:29:35
	/// 返回  : int  0成功
	/// 参数  : CXmlPack xmlpack
	/// 描述  : 将xmlpack的pack内容作为多记录增加到本包中 pack%d
	int addPack(CXmlPack *xmlpack);


	/// 函数名: addPackPoint
	/// 编程  : 王明松 2009-1-20 10:19:54
	/// 返回  : int 0成功
	/// 参数  : CXmlPack *xmlpack
	/// 描述  : 将xmlpack的pack指针作为多记录增加到本包中 pack%d，原xmlpack的pack内容销毁
	int addPackPoint(CXmlPack *xmlpack);

	/// 函数名: addPack
	/// 编程  : 王明松 2007-4-30 10:24:58
	/// 返回  : int  返回本包记录数
	/// 参数  : string &packname 返回的记录节点名
	/// 描述  : 增加一个空的pack记录,pack%d本包记录数
	int addPack(std::string &packname);

	///设置多记录包体内容 

	/// 函数名: setPackNameValueI
	/// 编程  : 王明松 2009-2-4 12:36:21
	/// 返回  : int 
	/// 输入参数  : int packno
	/// 输入参数  : std::string name
	/// 输入参数  : int value
	/// 输入参数  : bool isupdate=true  若为false直接增加节点，否则进行查找更新或增加
	/// 描述  : 设置多pack的字段内容
	int setPackNameValueI(int packno, std::string name, int value,bool isupdate=true);

	/// 函数名: setPackNameValueI
	/// 编程  : 王明松 2009-2-4 12:36:25
	/// 返回  : int 
	/// 输入参数  : int packno
	/// 输入参数  : std::string name
	/// 输入参数  : long value
	/// 输入参数  : bool isupdate=true  若为false直接增加节点，否则进行查找更新或增加
	/// 描述  : 设置多pack的字段内容
	int setPackNameValueI(int packno, std::string name, long value,bool isupdate=true);

	/// 函数名: setPackNameValueI
	/// 编程  : 王明松 2009-2-4 12:36:28
	/// 返回  : int 
	/// 输入参数  : int packno
	/// 输入参数  : std::string name
	/// 输入参数  : double value
	/// 输入参数  : bool isupdate=true  若为false直接增加节点，否则进行查找更新或增加
	/// 描述  : 设置多pack的字段内容
	int setPackNameValueI(int packno, std::string name, double value,bool isupdate=true);
	

	/// 函数名: setPackNameValueI
	/// 编程  : 王明松 2009-2-4 12:36:33
	/// 返回  : int 
	/// 输入参数  : int packno
	/// 输入参数  : std::string name
	/// 输入参数  : std::string value
	/// 输入参数  : bool isupdate=true  若为false直接增加节点，否则进行查找更新或增加
	/// 输入参数  : bool istrans=true   若为false不进行转义符转换，否则进行转义符转换
	/// 描述  : 设置多pack的字段内容
	int setPackNameValueI(int packno, std::string name, std::string value,bool isupdate=true,bool istrans=true);
	/// 函数名: setPackNameValue
	/// 编程  : 王明松 2006-7-3 15:07:46
	/// 返回  : int 
	/// 输入参数  : const std::string item
	/// 输入参数  : std::string name
	/// 输入参数  : std::string value
	/// 输入参数  : bool isupdate=true  若为false直接增加节点，否则进行查找更新或增加
	/// 输入参数  : bool istrans=true   若为false不进行转义符转换，否则进行转义符转换
	/// 描述  : 设置多pack的字段内容
	int setPackNameValue(const std::string item, std::string name, std::string value,bool isupdate=true,bool istrans=true);

public: ///设置xmlpack内容

	/// 函数名: SetPackNode
	/// 编程  : 王明松 2009-1-24 10:57:31
	/// 返回  : bool 
	/// 参数  : CNode *pnode  若空则m_packnode也为空
	/// 描述  : 将pnode置为pack节点或pack的临近节点，节点名称为pnode.m_name 如果节点名为pack则m_packnode为此节点
	bool SetPackNode(CNode *pnode);

	/// 函数名: XmlCopyPack
	/// 编程  : 王明松 2009-1-16 10:11:56
	/// 返回  : bool 
	/// 参数  : CXmlPack *xmlpack
	/// 描述  : 复制xmlpack的pack节点到本包的pack节点,本pack节点删除后再复制
	bool XmlCopyPack(CXmlPack *xmlpack);

	/// 函数名: XmlCopyHead
	/// 编程  : 王明松 2009-1-16 10:11:52
	/// 返回  : bool 
	/// 参数  : CXmlPack *xmlpack
	/// 描述  : 复制xmlpack的head节点到本包的head节点,本head节点删除后再复制
	bool XmlCopyHead(CXmlPack *xmlpack);

	
	/// 函数名: setPackNameValue
	/// 编程  : 王明松 2008-7-3 15:08:55
	/// 返回  : int =0成功
	/// 输入参数  : std::string name
	/// 输入参数  : int value
	/// 输入参数  : bool isupdate=true  若为false直接增加节点，否则进行查找更新或增加
	/// 描述  : 设置包体内容 当前节点至此节点
	int setPackNameValue(std::string name, int value,bool isupdate=true);

	/// 函数名: setPackNameValue
	/// 编程  : 王明松 2008-7-3 15:08:55
	/// 返回  : int =0成功
	/// 输入参数  : std::string name
	/// 输入参数  : long value
	/// 输入参数  : bool isupdate=true  若为false直接增加节点，否则进行查找更新或增加
	/// 描述  : 设置包体内容 当前节点至此节点
	int setPackNameValue(std::string name, long value,bool isupdate=true);

	/// 函数名: setPackNameValue
	/// 编程  : 王明松 2008-7-3 15:08:55
	/// 返回  : int =0成功
	/// 输入参数  : std::string name
	/// 输入参数  : double value
	/// 输入参数  : bool isupdate=true  若为false直接增加节点，否则进行查找更新或增加
	/// 描述  : 设置包体内容 当前节点至此节点
	int setPackNameValue(std::string name, double value,bool isupdate=true);
	int setPackNameValue(std::string name, double value,int dec,bool isupdate=true);

	/// 函数名: setPackNameValue
	/// 编程  : 王明松 2008-7-3 15:08:55
	/// 返回  : int =0成功
	/// 输入参数  : std::string name
	/// 输入参数  : string value
	/// 输入参数  : bool isupdate=true  若为false直接增加节点，否则进行查找更新或增加
	/// 输入参数  : bool istrans=true   若为false不进行转义符转换，否则进行转义符转换
	/// 描述  : 设置包体内容 当前节点至此节点
	int setPackNameValue(std::string name ,std::string value,bool isupdate=true,bool istrans=true);


	/// 函数名: setHeadNameValue
	/// 编程  : 王明松 2006-7-3 15:09:09
	/// 返回  : int =0成功
	/// 输入参数  : std::string name
	/// 输入参数  : long value
	/// 输入参数  : bool isupdate=true  若为false直接增加节点，否则进行查找更新或增加
	/// 描述  : 设置包头内容 当前节点至此节点
	int setHeadNameValue(std::string name, long value,bool isupdate=true);

	/// 函数名: setHeadNameValue
	/// 编程  : 王明松 2006-7-3 15:09:09
	/// 返回  : int =0成功
	/// 输入参数  : std::string name
	/// 输入参数  : double value
	/// 输入参数  : bool isupdate=true  若为false直接增加节点，否则进行查找更新或增加
	/// 描述  : 设置包头内容 当前节点至此节点
	int setHeadNameValue(std::string name, double value,bool isupdate=true);
	int setHeadNameValue(std::string name, double value,int dec,bool isupdate=true);
	/// 函数名: setHeadNameValue
	/// 编程  : 王明松 2006-7-3 15:09:09
	/// 返回  : int =0成功
	/// 输入参数  : std::string name
	/// 输入参数  : int value
	/// 输入参数  : bool isupdate=true  若为false直接增加节点，否则进行查找更新或增加
	/// 描述  : 设置包头内容 当前节点至此节点
	int setHeadNameValue(std::string name, int value,bool isupdate=true);

	/// 函数名: setHeadNameValue
	/// 编程  : 王明松 2006-7-3 15:09:09
	/// 返回  : int =0成功
	/// 输入参数  : std::string name
	/// 输入参数  : string value
	/// 输入参数  : bool isupdate=true  若为false直接增加节点，否则进行查找更新或增加
	/// 输入参数  : bool istrans=true   若为false不进行转义符转换，否则进行转义符转换
	/// 描述  : 设置包头内容 当前节点至此节点
	int setHeadNameValue(std::string name,std::string value,bool isupdate=true,bool istrans=true);


	/// 函数名: readPack
	/// 编程  : 王明松 2006-7-3 15:09:27
	/// 返回  : bool 
	/// 描述  : 读xml数据是否符合定义的xml包格式
	bool readPack();

	/// 函数名: toXmlBuf
	/// 编程  : 王明松 2008-6-19 19:02:57
	/// 返回  : bool 
	/// 参数  : char *xmlBuf
	/// 描述  : 把xml数据生成buffer,不做长度检查，要求输入的缓冲够大,建议使用toString
	bool toXmlBuf(char *xmlBuf);


	/// 函数名: toBuf
	/// 编程  : 王明松 2008-6-19 19:02:16
	/// 返回  : bool 
	/// 参数  : char *Buf
	/// 参数  : int &buflen
	/// 描述  : 把xml或二进制数据生成buffer,不做长度检查，要求输入的缓冲够大,建议使用tobuffer
	bool toBuf(char *Buf,int &buflen);

	///

	/// 函数名: toString
	/// 编程  : 王明松 2008-7-3 15:09:50
	/// 返回  : bool 
	/// 参数  : std::string &buffer
	/// 描述  : 把xml格式生成string类型buffer
	bool toString(std::string &buffer);


	/// 函数名: fromBuf
	/// 编程  : 王明松 2008-7-3 15:10:08
	/// 返回  : bool 
	/// 参数  : char *xmlBuf
	/// 描述  : 从buffer生成xml数据
	bool fromBuf(char *xmlBuf);

	/// 函数名: toFile
	/// 编程  : 王明松 2008-7-3 15:10:17
	/// 返回  : int 
	/// 参数  : char *filename
	/// 描述  : 把xml数据写到文件
	int toFile(char *filename);

	/// 函数名: fromFile
	/// 编程  : 王明松 2008-7-3 15:10:26
	/// 返回  : int 
	/// 参数  : char *filename
	/// 描述  : 从文件生成xml数据
	int fromFile(char *filename);

	/// 函数名: *getMsg
	/// 编程  : 王明松 2008-7-3 15:10:35
	/// 返回  : char 
	/// 描述  : 获取错误信息
	char *getMsg();



	/// 函数名: toBufOld
	/// 编程  : 王明松 2008-11-25 16:21:02
	/// 返回  : bool 
	/// 参数  : char *Buf
	/// 参数  : int &buflen
	/// 描述  : 无</>的形式的xml
	bool toBufOld(char *Buf,int &buflen);

	CXmlPack();
	///从buffer生成xml类
	CXmlPack(char *xmlBuf);
	virtual ~CXmlPack();
	CXML m_xml;        /*xml解析数据类*/
	CNode *m_rootnode;/*xml根节点*/
	char m_errMsg[1025];
	
	
protected:
	CNode *m_headnode;/*xml包头节点*/
	CNode *m_packnode;/*xml包体节点*/
	int m_rowbuflen;///存放二进制数据的缓冲长度
	char *m_rowbuf; ///存放二进制数据的缓冲

	CNode *m_pRecNode;//本包记录数节点
};
#undef XML_EXPORT
#endif /// !defined(AFX_XMLPACK_H__C509FC3C_B8B1_4911_A3F6_96C274BC9748__INCLUDED_)
