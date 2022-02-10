// GtpData.h: interface for the CGtpData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GTPDATA_H__35334B97_ADCF_41EE_8157_0537A846AC7F__INCLUDED_)
#define AFX_GTPDATA_H__35334B97_ADCF_41EE_8157_0537A846AC7F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "public.h"
#include "MdbBase.h"
#include "BF_SimpleLog.h"



#define   GTP_DATATYPE_UNKNOW   0            //未知
#define   GTP_DATATYPE_VALUE    1            //值的类型 
#define   GTP_DATATYPE_HASH     2            //hash表  {xy=1,xy2=2}
#define   GTP_DATATYPE_ARRAY    3            //数组    [{xy=1,xy2=2},{xy3=1,xy4=2}


#define   GTP_KEY_LEN           10           //KEY的长度

//此类为x=1,x1=2,x3={xy=1,xy2=2},x4=[{xy=1,xy2=2},{xy3=1,xy4=2}] 这种类型的数据
//本类是一个数据节点类，包含key，值、hash表、数组。
//hash表和数据也用一个本类来表示。
//本类有三种类型，见上面类型定义

class CGtpData;

typedef struct 
{
	char             sFieldName[GTP_KEY_LEN+1];  //key值
	std::string      sFieldValue;     //值
	unsigned  char   cDataType;       //类型
	bool             bIsParseLocal;   //本节点是否解析
	CGtpData        *pGtpData;        //子类型指针
}S_GTP_DATA_;




#ifdef GTP_EXPORTS
#define GTP_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define GTP_EXPORT __declspec(dllimport)
#else
#define GTP_EXPORT 
#endif
#endif

class GTP_EXPORT CGtpData  
{
public:

	// 函数名: UpdateCurValue
	// 编程  : 王明松 2016-4-20 8:55:08
	// 返回  : bool 
	// 参数  : CGtpData *src  从这个节点只取值去更新
	// 参数  : bool isUpdate=false  当本节点有此字段时，是否更新 false不更新 true更新
	// 描述  : 更新当前的值
	bool UpdateCurValue(CGtpData *src,bool isUpdate=false);
	
	// 函数名: ToStringDelRec
	// 编程  : 王明松 2016-3-3 17:50:09
	// 返回  : bool 
	// 参数  : std::string &gtpstring
	// 参数  : int delnum
	// 描述  : 生成gtp时去掉前面几个字段
	bool ToStringDelRec(std::string &gtpstring,int delnum);

	// 函数名: AddDataItem
	// 编程  : 王明松 2016-3-2 15:34:38
	// 返回  : CGtpData * 
	// 参数  : const char *fieldname  数据项的KEY
	// 参数  : char datatype  数据项类型
	// 描述  : 增加一个空的数据项  若为数组类型，数组里面只能是hash，所以注意增加hash时的fieldname最好从"0"开始，会按这个排序，否则可能生成的顺序不一致
	CGtpData * AddDataItem(const char *fieldname,char datatype);

	// 函数名: SetDataItem
	// 编程  : 王明松 2016-3-2 15:31:45
	// 返回  : bool 
	// 参数  : const char *fieldname  key
	// 参数  : const char *fieldvalue value
	// 参数  : bool istrans=true    是否做转义检查处理
	// 描述  : 设置数据项，即本节点下面的KV值  若本节点为数组直接返回出错
	bool SetDataItem(const char *fieldname,const char *fieldvalue,bool istrans=true);
	bool SetDataItem(const char *fieldname,const std::string &fieldvalue,bool istrans=true);
	bool SetDataItem(const char *fieldname,int fieldvalue);
	bool SetDataItem(const char *fieldname,unsigned int fieldvalue);
	bool SetDataItem(const char *fieldname,short fieldvalue);
	bool SetDataItem(const char *fieldname,unsigned short fieldvalue);
	bool SetDataItem(const char *fieldname,long fieldvalue);
	bool SetDataItem(const char *fieldname,unsigned long fieldvalue);
	bool SetDataItem(const char *fieldname,double fieldvalue,int dec=2);
	bool SetDataItem(const char *fieldname,char fieldvalue);

	// 函数名: ParseGtp
	// 编程  : 王明松 2016-2-29 19:53:34
	// 返回  : bool 
	// 参数  : int layer=1
	// 描述  : 解析节点数据  >0为解析层级 1只解析当前
	bool ParseGtp(int layer=1);

	// 函数名: Clear
	// 编程  : 王明松 2016-2-29 19:43:04
	// 返回  : void 
	// 描述  : 清空数据
	void Clear();

	
	

	// 函数名: GetDataPoint
	// 编程  : 王明松 2016-2-29 14:53:21
	// 返回  : CGtpData * =NULL无此数组或hash值
	// 参数  : const char *fieldname
	// 描述  : 根据key来取得对应的数据类型，然后针对数组指针或hash指定进行取值或赋值操作
	CGtpData * GetDataPoint(const char *fieldname);


	// 函数名: GetDataPoint
	// 编程  : 王明松 2016-3-2 10:27:53
	// 返回  : CGtpData * =NULL无此数组或hash值
	// 参数  : unsigned int recnum   记录数  从0开始  注意记录是按从小到大排序的
	// 描述  : 根据记录数来取得对应的数据类型，不根据名称来取，因为数组里的hash表没有名称，和FieldCount配合使用
	CGtpData * GetDataPoint(unsigned int recnum);

	// 函数名: GetArraySize
	// 编程  : 王明松 2016-2-29 15:06:56
	// 返回  : int  <0无此数据  >0为数据大小
	// 输入参数  : unsigned int arrayrowid   数组对应的rowid，此rowid可GetRowId得到
	// 描述  : 取得指定数组里的数据个数  
	int GetArraySize(unsigned int arrayrowid);
	

	// 函数名: FieldCount
	// 编程  : 王明松 2016-3-2 9:58:00
	// 返回  : int 
	// 描述  : 取得当前类的字段个数，若为hash和value则为字段数，若为array则为数组个数
	int FieldCount();

	// 函数名: GetHashValue
	// 编程  : 王明松 2016-2-29 15:10:41
	// 返回  : int =0成功 <0无此字段key   >0字段类型不符
	// 输入参数  : unsigned int hashrowid  哈希表的rowid，可调GetRowId得到
	// 输入参数  : const char *fieldname   字段的key 
	// 输出参数  : unsigned  short &fieldvalue   字段值
	// 输出参数  : int &fieldtype                有此字段为数据类型
	// 描述  : 取得当前节点下哈希表里的值
	int GetHashValue(unsigned int hashrowid,const char *fieldname, unsigned  short &fieldvalue, int &fieldtype);
	int GetHashValue(unsigned int hashrowid,const char *fieldname, unsigned long &fieldvalue, int &fieldtype);
	int GetHashValue(unsigned int hashrowid,const char *fieldname, unsigned int  &fieldvalue, int &fieldtype);
	int GetHashValue(unsigned int hashrowid,const char *fieldname, double &fieldvalue, int &fieldtype);
	int GetHashValue(unsigned int hashrowid,const char *fieldname, int &fieldvalue, int &fieldtype);
	int GetHashValue(unsigned int hashrowid,const char *fieldname, long &fieldvalue, int &fieldtype);
	int GetHashValue(unsigned int hashrowid,const char *fieldname, short &fieldvalue, int &fieldtype);
	int GetHashValue(unsigned int hashrowid,const char *fieldname ,char *fieldvalue,int &fieldtype);
	int GetHashValue(unsigned int hashrowid,const char *fieldname ,std::string &fieldvalue,int &fieldtype);


	// 函数名: GetRowId
	// 编程  : 王明松 2016-2-29 15:07:37
	// 返回  : int >=0成功  其它失失败
	// 输入参数  : const char *fieldname   字段key
	// 输出参数  : int &fieldtype   数据类型
	// 描述  : 取得key对应的rowid，然后可根据row来取数据值，只有哈希和数组或重复取值才用
	int GetRowId(const char *fieldname,int &fieldtype);


	// 函数名: GetDataType
	// 编程  : 王明松 2016-2-29 15:15:56
	// 返回  : int 
	// 描述  : 取本节点的数据类型
	int GetDataType();
	

	// 函数名: GetValueByName
	// 编程  : 王明松 2016-2-29 15:16:11
	// 返回  : int  >=0 成功为该字段的rowid    <0  无此字段或字段类型不符 -1无此字段  -2类型不符
	// 输入参数  : const char *fieldname    字段的key
	// 输出参数  : unsigned  short &fieldvalue   字段值
	// 输出参数  : int &fieldtype            字段类型
	// 描述  : 根据字段key取值
	int GetValueByName(const char *fieldname, unsigned  short &fieldvalue, int &fieldtype);
	int GetValueByName(const char *fieldname, unsigned long &fieldvalue, int &fieldtype);
	int GetValueByName(const char *fieldname, unsigned int  &fieldvalue, int &fieldtype);
	int GetValueByName(const char *fieldname, double &fieldvalue, int &fieldtype);
	int GetValueByName(const char *fieldname, int &fieldvalue, int &fieldtype);
	int GetValueByName(const char *fieldname, long &fieldvalue, int &fieldtype);
	int GetValueByName(const char *fieldname, short &fieldvalue, int &fieldtype);
	int GetValueByName(const char *fieldname ,char *fieldvalue,int &fieldtype);
	int GetValueByName(const char *fieldname ,std::string &fieldvalue,int &fieldtype);
	int GetValueByName(const char *fieldname, unsigned  char &fieldvalue, int &fieldtype);


	// 函数名: GetValueByName
	// 编程  : 王明松 2016-4-20 9:09:36
	// 返回  : int >=0 成功为该字段的rowid    <0  无此字段或字段类型不符 -1无此字段  -2类型不符
	// 参数  : unsigned int recnum  通过FieldCount来得到多少条记录，然后循环来取
	// 输出参数  : unsigned  short &fieldvalue   字段值
	// 输出参数  : int &fieldtype
	// 描述  : 根据记录数来取字段值  值不能是hash和数组
	int GetValueByName(unsigned int recnum, std::string &fieldname,unsigned  short &fieldvalue, int &fieldtype);
	int GetValueByName(unsigned int recnum, std::string &fieldname,unsigned long &fieldvalue, int &fieldtype);
	int GetValueByName(unsigned int recnum, std::string &fieldname,unsigned int  &fieldvalue, int &fieldtype);
	int GetValueByName(unsigned int recnum, std::string &fieldname,double &fieldvalue, int &fieldtype);
	int GetValueByName(unsigned int recnum, std::string &fieldname,int &fieldvalue, int &fieldtype);
	int GetValueByName(unsigned int recnum, std::string &fieldname,long &fieldvalue, int &fieldtype);
	int GetValueByName(unsigned int recnum, std::string &fieldname,short &fieldvalue, int &fieldtype);
	int GetValueByName(unsigned int recnum, std::string &fieldname,char *fieldvalue,int &fieldtype);
	int GetValueByName(unsigned int recnum, std::string &fieldname,std::string &fieldvalue,int &fieldtype);
	int GetValueByName(unsigned int recnum,std::string &fieldname, unsigned char &fieldvalue, int &fieldtype);

	// 函数名: GetGtpDataLen
	// 编程  : 王明松 2016-2-29 15:18:40
	// 返回  : int 
	// 描述  : 取得本节点的数据长度
	int GetGtpDataLen();


	// 函数名: ToString
	// 编程  : 王明松 2016-2-29 15:18:53
	// 返回  : bool 
	// 参数  : std::string &gtpstring
	// 描述  : 生成文本报文
	bool ToString(std::string &gtpstring);

	// 函数名: ToFile
	// 编程  : 王明松 2016-2-29 15:19:05
	// 返回  : bool 
	// 参数  : const char *gtpfilename
	// 描述  : 将文本报文输出至文件
	bool ToFile(const char *gtpfilename);

	// 函数名: ToBuffer
	// 编程  : 王明松 2016-2-29 15:19:23
	// 返回  : bool 
	// 参数  : char *gtpbuffer
	// 参数  : int &buflen
	// 描述  : 生成文本报文
	bool ToBuffer(char *gtpbuffer ,int &buflen);

	// 函数名: FromFile
	// 编程  : 王明松 2016-2-29 15:19:39
	// 返回  : bool 
	// 参数  : const char *gtpfile
	// 参数  : int layer=0   0为只解析顶层，后续可由取数据解析   1为解析下一层，包括哈希和数组
	// 描述  : 从文件里读取gtp报文，并解析
	bool FromFile(const char *gtpfile,int layer=1);

	// 函数名: FromBuffer
	// 编程  : 王明松 2016-2-29 15:20:03
	// 返回  : bool 
	// 参数  : const char *gtpdata  
	// 参数  : int layer=0    0为只解析顶层，后续可由取数据解析   1为解析下一层，包括哈希和数组
	// 描述  : 从缓冲里读取gtp报文，并解析
	bool FromBuffer(const char *gtpdata,int layer=1);


	int GtpGetTransSign(std::string &value);

	int GtpPutTransSign(std::string &value);

	CGtpData();
	virtual ~CGtpData();

protected:

	// 函数名: SetKeyValue
	// 编程  : 王明松 2016-2-29 19:43:15
	// 返回  : bool 
	// 参数  : const std::string &key
	// 参数  : const std::string &svalue
	// 参数  : char datatype  
	// 描述  : 设置当前节点key，值，类型 
	bool SetCurKeyValue(const std::string &key,const std::string &svalue,char datatype);

	// 函数名: Insert
	// 编程  : 王明松 2016-3-2 9:12:31
	// 返回  : int <0失败  >0 rowid
	// 参数  : S_GTP_DATA_ data
	// 描述  : 插入一条记录kv或hash或array
	int Insert(S_GTP_DATA_ data);

	void bzeroData(S_GTP_DATA_ &data);


	// 函数名: ParseGtpArray
	// 编程  : 王明松 2016-3-2 9:11:47
	// 返回  : bool 
	// 参数  : int layer
	// 描述  : 解析数组
	bool ParseGtpArray(int layer);

	// 函数名: ParseGtpHash
	// 编程  : 王明松 2016-3-2 9:12:03
	// 返回  : bool 
	// 参数  : int layer
	// 描述  : 解析哈希表
	bool ParseGtpHash(int layer);

	// 函数名: ParseGtpValue
	// 编程  : 王明松 2016-3-2 9:12:15
	// 返回  : bool 
	// 参数  : int layer
	// 描述  : 解析kv
	bool ParseGtpValue(int layer);


	// 函数名: FindArrayOffset
	// 编程  : 王明松 2016-3-1 10:56:01
	// 返回  : int 
	// 参数  : int begin
	// 参数  : int end
	// 参数  : const char *gtpdata
	// 描述  : 查询数组的结束位置
	int FindArrayOffset(int begin,int end ,const char *gtpdata);

	// 函数名: FindHashOffset
	// 编程  : 王明松 2016-3-1 10:55:45
	// 返回  : int 
	// 参数  : int begin
	// 参数  : int end
	// 参数  : const char *gtpdata
	// 描述  : 查找哈希表的结束位置
	int FindHashOffset(int begin,int end,const char *gtpdata);

	std::string  m_sGtpKey;  //key值
	std::string  m_sGtpValue; //数据    包含[{}]

	unsigned   char   m_cDataType;//本数据节点的类型   GTP_DATATYPE_VALUE 哈希表  数组
	CMemTableNew <S_GTP_DATA_>     m_table;   
	CPkeyCharV<1>           m_keyIndex;     //名称的索引

	CBF_SimpleLog  m_pLog;
	
};

#endif // !defined(AFX_GTPDATA_H__35334B97_ADCF_41EE_8157_0537A846AC7F__INCLUDED_)
