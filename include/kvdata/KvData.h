// KvData.h: interface for the CKvData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KVDATA_H__3A8993D3_2C8A_40EC_8B0C_27D1396CF119__INCLUDED_)
#define AFX_KVDATA_H__3A8993D3_2C8A_40EC_8B0C_27D1396CF119__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "KVNode.h"




#ifdef WIN32
#ifdef _DEBUG
#pragma comment(lib, "bf_kerneld.lib") 
#pragma message("Automatically linking with  bf_kerneld.lib ")
#else
#pragma comment(lib, "bf_kernel.lib") 
#pragma message("Automatically linking with  bf_kernel.lib ")
#endif
#endif


typedef struct _S_KV_HEAD
{
	char m_seq_no[8+1];  //流水号
	char m_msg_type[1+1];  //报文类型     
	char m_exch_code[10+1];  //交易代码 
	char m_msg_flag[1+1];   //报文标识    '1' 请求  '2' 应答
	char m_term_type[2+1];    //终端来源
	char m_user_type[2+1];   //用户类型
	char m_user_id[20+1];    //用户ID
	char m_area_code[4+1];    //地区代码
	char m_branch_id[20+1];  //代理机构
	char m_c_teller_id1[20+1]; //一级复核操作员
	char m_c_teller_id2[20+1]; //二级复核操作员
	char m_rsp_code[8+1];     //返回码
}S_KV_HEAD;
// typedef struct _S_KV_HEAD
// {
// 	char m_seq_no[8+1];  //流水号
// 	char m_msg_type[1+1];  //报文类型     
// 	char m_exch_code[4+1];  //交易代码 
// 	char m_msg_flag[1+1];   //报文标识    '1' 请求  '2' 应答
// 	char m_term_type[2+1];    //终端来源
// 	char m_user_type[2+1];   //用户类型
// 	char m_user_id[10+1];    //用户ID
// 	char m_area_code[4+1];    //地区代码
// 	char m_branch_id[12+1];  //代理机构
// 	char m_c_teller_id1[10+1]; //一级复核操作员
// 	char m_c_teller_id2[10+1]; //二级复核操作员
// 	char m_rsp_code[8+1];     //返回码
// }__PACKED__ S_KV_HEAD;

#define  FIXEDREQHEADLEN  108
#define  FIXEDRSPHEADLEN  76


#ifdef KVDATA_EXPORTS
#define KVDATA_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define KVDATA_EXPORT __declspec(dllimport)
#else
#define KVDATA_EXPORT 
#endif
#endif


class KVDATA_EXPORT CKvData  
{
public:

	// 函数名: SetNodeByName
	// 编程  : 王明松 2013-12-11 17:57:36
	// 返回  : CKVNode *  2层的节点
	// 参数  : std::string name   =分隔的名称
	// 参数  : int nodetype       =后面的值的类型，有哈希表、列表等
	// 描述  : 设置节点名及节点类型，后续可根据返回的节点进行设置值等操作
	CKVNode * SetNodeByName(std::string name,int nodetype);

	// 函数名: GetValueByNameOne
	// 编程  : 王明松 2013-12-11 15:14:28
	// 返回  : bool 
	// 参数  : std::string name  
	// 参数  : std::string &svalue
	// 参数  : int &col   列
	// 参数  : int &line  行
	// 描述  : 根据名称取值，统一用|分隔
	bool GetValueByNameOne(std::string name,std::string &svalue,int &col, int &line);


	// 函数名: GetValueByName
	// 编程  : 王明松 2013-12-11 14:24:00
	// 返回  : CKVNode* 2层的节点 NULL没找到
	// 参数  : std::string name =分隔的名称
	// 参数  : std::string &svalue    =分隔的值
	// 描述  : 根据名称取值 =分隔的值
	CKVNode* GetValueByName(std::string name,std::string &svalue);
	CKVNode* GetValueByName(std::string name,int &ivalue);
	CKVNode* GetValueByName(std::string name,short &ivalue);
	CKVNode* GetValueByName(std::string name,long &ivalue);
	CKVNode* GetValueByName(std::string name,double &ivalue);
	CKVNode* GetValueByName(std::string name,unsigned int &ivalue);
	CKVNode* GetValueByName(std::string name,unsigned short &ivalue);
	CKVNode* GetValueByName(std::string name,unsigned long &ivalue);


	// 函数名: GetNodeByName
	// 编程  : 王明松 2013-12-13 9:02:43
	// 返回  : CKVNode*   2层的节点 NULL没找到
	// 参数  : std::string name  =分隔的名称
	// 描述  : 根据名称获取节点
	CKVNode* GetNodeByName(std::string name);


	// 函数名: ToString
	// 编程  : 王明松 2013-12-11 13:56:43
	// 返回  : bool 
	// 参数  : std::string &data
	// 描述  : 将kv生成字符串，不包含固定头
	bool ToString(std::string &data);


	// 函数名: ParseNode
	// 编程  : 王明松 2013-12-11 11:24:31
	// 返回  : bool 
	// 参数  : std::string data
	// 描述  : 解析节点，除去固定报文头的解析
	bool ParseNode(std::string data);

	// 函数名: ParseHead
	// 编程  : 王明松 2013-12-10 20:53:19
	// 返回  : bool 
	// 参数  : std::string data
	// 描述  : 解析报文头
	bool ParseHead(std::string data);

	// 函数名: FromBuffer
	// 编程  : 王明松 2013-12-10 20:32:07
	// 返回  : bool 
	// 参数  : string data
	// 描述  : 解析值
	bool FromBuffer(std::string data);


	//封装

	// 函数名: fromBuffer
	// 编程  : 王明松 2013-12-13 9:12:52
	// 返回  : bool 
	// 参数  : char *buf
	// 描述  : 将kv报文数据转换成kv报文对象
	bool fromBuffer(char *buf); 
	
	// 函数名: toBuffer
	// 编程  : 王明松 2013-12-13 9:13:10
	// 返回  : bool 
	// 参数  : char *buf
	// 参数  : unsigned int size  buf的大小
	// 描述  : 将kv报文对象转换成kv报文数据
	bool toBuffer(char *buf, unsigned int size);
	
	/* 设置kv报文对象的报文头的值*/
	int setHead(std::string key, std::string svalue);
	
	// 函数名: setPack
	// 编程  : 王明松 2013-12-13 9:13:31
	// 返回  : int 
	// 参数  : std::string key
	// 参数  : std::string value   普通的值，无分隔
	// 参数  : bool flag= true
	// 描述  : 设置kv报文对象报文体的值,flag为true时更新或新增key 为flase时直接新增key
	int setPack(std::string key, std::string svalue, bool flag= true);
	int setPack(std::string key, short svalue, bool flag= true);
	int setPack(std::string key, int svalue, bool flag= true);
	int setPack(std::string key, long svalue, bool flag= true);
	int setPack(std::string key, double svalue, bool flag= true);
	int setPack(std::string key, unsigned short svalue, bool flag= true);
	int setPack(std::string key, unsigned int  svalue, bool flag= true);
	int setPack(std::string key, unsigned long svalue, bool flag= true);
	
	
	// 函数名: setPackX
	// 编程  : 王明松 2013-12-12 20:39:13
	// 返回  : int 
	// 参数  : std::string key    名称
	// 参数  : std::string value  "｜"线分隔的的值
	// 参数  : int row= 0         行  value里记录有多少行 (若是哈希表，少输入一行，即去掉表头)
	// 参数  : int col= 0         列  每条记录里的列数
	// 参数  : bool flag= true
	// 描述  : 设置kv报文对象报文体的值
	int setPackX(std::string key, std::string svalue, int line= 0, int col= 0, bool flag= true);
	
	// 函数名: getHead
	// 编程  : 王明松 2013-12-13 9:14:49
	// 返回  : bool 
	// 参数  : std::string key   定长报文头里的名称  serial_no等
	// 参数  : std::string &value  值
	// 描述  : 获取定长报文头里的名称对应的值
	bool getHead(std::string key, std::string &svalue);
	

	// 函数名: getPack
	// 编程  : 王明松 2013-12-13 9:16:19
	// 返回  : bool 
	// 参数  : std::string key   =分隔的名称
	// 参数  : std::string &value  值
	// 参数  : int row= 0    对应的行
	// 参数  : int col= 0    对应的列
	// 描述  : 获取指定行和列的值，当名称所在的层是kvtype_datalist即为此层的值，当为哈希表或列表时取对应的数据记录的行和列的值 
	bool getPack(std::string key, std::string &svalue, int row= 0, int col= 0);
	
	// 函数名: getPackH
	// 编程  : 王明松 2013-12-13 9:19:11
	// 返回  : bool 
	// 参数  : std::string key  =分隔的名称
	// 参数  : std::string &value  值
	// 参数  : int row= 0    对应的行
	// 参数  : int col= 0    对应的列
	// 描述  : 获取指定行和列的值，当名称所在的层是kvtype_datalist即为此层的值，当为哈希表时取包括表头对应行和列的值，当为列表时为数据记录的行和列的值 
	bool getPackH(std::string key, std::string &svalue, int row= 0, int col= 0);
	
	// 函数名: getPackX
	// 编程  : 王明松 2013-12-13 9:22:27
	// 返回  : bool 
	// 参数  : std::string key   =分隔的名称
	// 参数  : std::string &value  普通的无"｜"分隔,哈希表和列表时值用"｜"分隔
	// 参数  : int &row       返回的行
	// 参数  : int &col       返回的列
	// 描述  : 取指定名称的值
	bool getPackX(std::string key, std::string &svalue, int &row, int &col);
				
	
	// 函数名: clear
	// 编程  : 王明松 2013-12-13 9:24:45
	// 返回  : void 
	// 描述  : 清空定找报文头结构，并删除所有节点
	void clear();

	CKvData();
	virtual ~CKvData();
	
	char    m_sErrMsg[256];

	CKVNode     m_pDataNode;
protected:
	
	bool        m_bIsRequestPack; //是否请求数据包
	S_KV_HEAD   m_kvHead;

private:
	CBF_SimpleLog  m_pLog;
	// 函数名: stringConvertX2K
	// 编程  : 王明松 2013-12-13 9:25:20
	// 返回  : int 
	// 参数  : CKVNode *namenode    =分隔的层  2层
	// 参数  : std::string &value   "｜"分隔的报文内容
	// 参数  : int line              报文内容有多少行(哈希表时不包含表头那行)
	// 参数  : int col              报文内容有多少列
	// 描述  : 把"｜"分隔的报文内容一个个增加到namenode(=分隔的层)的树下面
	int stringConvertX2K(CKVNode *namenode,std::string &svalue, int line, int col);

};

#endif // !defined(AFX_KVDATA_H__3A8993D3_2C8A_40EC_8B0C_27D1396CF119__INCLUDED_)
