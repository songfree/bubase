// SqlDataResult.h: interface for the CSqlDataResult class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SQLDATARESULT_H__1C6FF3FA_7134_49AD_B1E4_6EEDEC1001D4__INCLUDED_)
#define AFX_SQLDATARESULT_H__1C6FF3FA_7134_49AD_B1E4_6EEDEC1001D4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"

#ifdef DBAPI_EXPORTS
#define DBAPI_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define DBAPI_EXPORT __declspec(dllimport)
#else
#define DBAPI_EXPORT 
#endif
#endif

typedef struct _RECORD_DATA
{
	std::string name;
	std::string value;
}S_RECORD;

/*****************************
*       数据记录类
*   每条记录作为一个类保存
******************************/
class DBAPI_EXPORT CSqlAPIData
{
private: 
	std::vector <S_RECORD> SqlRecordBuffer;
    int  SqlFieldCount;                                                   ///字段数
public: 
	int Add(S_RECORD sdata);
    int Clear();                                                          ///清空字段
    int GetCount();                                                       ///取字段数

	/// 函数名: SetAt
	/// 编程  : 王明松 2006-3-30 17:13:19
	/// 返回  : int 
	/// 参数  : char *buffer
	/// 参数  : int nIndex
	/// 描述  : update指定字段从0开始
    int SetAt(char *buffer,int nIndex);              

	/// 函数名: SetAt
	/// 编程  : 王明松 2006-3-30 17:13:32
	/// 返回  : int 
	/// 参数  : string buffer
	/// 参数  : int nIndex
	/// 描述  : update指定字段 从0开始
	int SetAt(std::string buffer,int nIndex);
	

	/// 函数名: GetAt
	/// 编程  : 王明松 2006-3-30 17:13:53
	/// 返回  : int 
	/// 参数  : int nIndex
	/// 参数  : char *FieldData
	/// 描述  : 取指定字段 从0开始
    int GetAt(int nIndex,char *FieldData);           

	/// 函数名: GetAt
	/// 编程  : 王明松 2006-3-30 17:13:56
	/// 返回  : int 
	/// 参数  : int nIndex
	/// 参数  : double &FieldData
	/// 描述  : 取指定字段 从0开始
    int GetAt(int nIndex,double &FieldData);          

	/// 函数名: GetAt
	/// 编程  : 王明松 2006-3-30 17:14:00
	/// 返回  : int 
	/// 参数  : int nIndex
	/// 参数  : long &FieldData
	/// 描述  : 取指定字段 从0开始
    int GetAt(int nIndex,long &FieldData);            

	/// 函数名: GetAt
	/// 编程  : 王明松 2006-3-30 17:14:04
	/// 返回  : int 
	/// 参数  : int nIndex
	/// 参数  : int &FieldData
	/// 描述  : 取指定字段 从0开始
    int GetAt(int nIndex,int &FieldData);             
    

	/// 函数名: *GetAt
	/// 编程  : 王明松 2006-3-30 17:14:28
	/// 返回  : char 
	/// 参数  : int nIndex
	/// 描述  : 取指定字段的char值 从0开始
	char *GetAt(int nIndex);           

	/// 函数名: GetAtD
	/// 编程  : 王明松 2006-3-30 17:14:31
	/// 返回  : double 
	/// 参数  : int nIndex
	/// 描述  : 取指定字段 从0开始
    double GetAtD(int nIndex);         

	/// 函数名: GetAtL
	/// 编程  : 王明松 2006-3-30 17:14:35
	/// 返回  : long 
	/// 参数  : int nIndex
	/// 描述  : 取指定字段 从0开始
    long GetAtL(int nIndex);            

	/// 函数名: GetAtI
	/// 编程  : 王明松 2006-3-30 17:14:38
	/// 返回  : int 
	/// 参数  : int nIndex
	/// 描述  : 取指定字段 从0开始
    int GetAtI(int nIndex);             

	/// 函数名: GetAtS
	/// 编程  : 王明松 2006-3-30 17:14:42
	/// 返回  : string 
	/// 参数  : int nIndex
	/// 描述  : 取指定字段 从0开始
	std::string GetAtS(int nIndex) ;


	/// 函数名: GetAt
	/// 编程  : 王明松 2006-3-30 17:13:53
	/// 返回  : int 
	/// 参数  : string fieldname 字段名
	/// 参数  : char *FieldData
	/// 描述  : 取指定字段
    int GetAt(std::string fieldname,char *FieldData);
	
	/// 函数名: GetAt
	/// 编程  : 王明松 2006-3-30 17:13:56
	/// 返回  : int 
	/// 参数  : string fieldname 字段名
	/// 参数  : double &FieldData
	/// 描述  : 取指定字段 
    int GetAt(std::string fieldname,double &FieldData);
	
	/// 函数名: GetAt
	/// 编程  : 王明松 2006-3-30 17:14:00
	/// 返回  : int 
	/// 参数  : string fieldname 字段名
	/// 参数  : long &FieldData
	/// 描述  : 取指定字段 
    int GetAt(std::string fieldname,long &FieldData);
	
	/// 函数名: GetAt
	/// 编程  : 王明松 2006-3-30 17:14:04
	/// 返回  : int 
	/// 参数  : string fieldname 字段名
	/// 参数  : int &FieldData
	/// 描述  : 取指定字段 从0开始
    int GetAt(std::string fieldname,int &FieldData);
    
	
	/// 函数名: *GetAt
	/// 编程  : 王明松 2006-3-30 17:14:28
	/// 返回  : char 
	/// 参数  : string fieldname 字段名
	/// 描述  : 取指定字段的char值
	char *GetAt(std::string fieldname);
	
	/// 函数名: GetAtD
	/// 编程  : 王明松 2006-3-30 17:14:31
	/// 返回  : double 
	/// 参数  : string fieldname 字段名
	/// 描述  : 取指定字段 
    double GetAtD(std::string fieldname);
	
	/// 函数名: GetAtL
	/// 编程  : 王明松 2006-3-30 17:14:35
	/// 返回  : long 
	/// 参数  : string fieldname 字段名
	/// 描述  : 取指定字段 
    long GetAtL(std::string fieldname);
	
	/// 函数名: GetAtI
	/// 编程  : 王明松 2006-3-30 17:14:38
	/// 返回  : int 
	/// 参数  : string fieldname 字段名
	/// 描述  : 取指定字段 
    int GetAtI(std::string fieldname);
	
	/// 函数名: GetAtS
	/// 编程  : 王明松 2006-3-30 17:14:42
	/// 返回  : string 
	/// 参数  : string fieldname 字段名
	/// 描述  : 取指定字段 
	std::string GetAtS(std::string fieldname) ;

	/// 函数名: Add
	/// 编程  : 王明松 2006-3-30 17:15:23
	/// 返回  : int 
	/// 参数  : char *buffer
	/// 描述  : 加一个字段到最后
    int Add(char *buffer);                           

	/// 函数名: Add
	/// 编程  : 王明松 2006-3-30 17:15:27
	/// 返回  : int 
	/// 参数  : string buffer
	/// 描述  : 加一个字段到最后
	int Add(std::string buffer);

    ~CSqlAPIData();
    CSqlAPIData();
};
/********************************
*     数据集合类
*  提供方法给调用者取数据
*********************************/
class DBAPI_EXPORT CSqlData
{
private: 
	std::vector <CSqlAPIData> SqlData;  ///数据集合
    int  SqlCount;                     ///记录数
public: 

	/// 函数名: GetSqlCount
	/// 编程  : 王明松 2006-3-30 17:16:30
	/// 返回  : int 
	/// 描述  : 取记录数
    int GetSqlCount();                 

	/// 函数名: clear
	/// 编程  : 王明松 2006-3-30 17:16:39
	/// 返回  : int 
	/// 描述  : 清空记录
    int clear();                       

	/// 函数名: GetAt
	/// 编程  : 王明松 2006-3-30 17:16:47
	/// 返回  : int 
	/// 参数  : int Record
	/// 参数  : int FieldNo
	/// 参数  : char *FieldData
	/// 描述  : 取某一记录的某一字段的char值
    int GetAt(int Record,int FieldNo,char *FieldData);     

	/// 函数名: GetAt
	/// 编程  : 王明松 2006-3-30 17:16:56
	/// 返回  : int 
	/// 参数  : int Record
	/// 参数  : int FieldNo
	/// 参数  : double &FieldData
	/// 描述  : 取某一记录的某一字段的double值
    int GetAt(int Record,int FieldNo,double &FieldData);    

	/// 函数名: GetAt
	/// 编程  : 王明松 2006-3-30 17:17:00
	/// 返回  : int 
	/// 参数  : int Record
	/// 参数  : int FieldNo
	/// 参数  : long &FieldData
	/// 描述  : 取某一记录的某一字段的long值
    int GetAt(int Record,int FieldNo,long &FieldData);      

	/// 函数名: GetAt
	/// 编程  : 王明松 2006-3-30 17:17:04
	/// 返回  : int 
	/// 参数  : int Record
	/// 参数  : int FieldNo
	/// 参数  : int &FieldData
	/// 描述  : 取某一记录的某一字段的int值
    int GetAt(int Record,int FieldNo,int &FieldData);      


	/// 函数名: GetAt
	/// 编程  : 王明松 2006-3-30 17:17:08
	/// 返回  : char * 
	/// 参数  : int Record
	/// 参数  : int FieldNo
	/// 描述  : 取某一记录的某一字段的char值
	char * GetAt(int Record,int FieldNo);     

	/// 函数名: GetAtD
	/// 编程  : 王明松 2006-3-30 17:17:12
	/// 返回  : double 
	/// 参数  : int Record
	/// 参数  : int FieldNo
	/// 描述  : 取某一记录的某一字段的double值
	double GetAtD(int Record,int FieldNo);    

	/// 函数名:  GetAtL
	/// 编程  : 王明松 2006-3-30 17:17:15
	/// 返回  : long  
	/// 参数  : int Record
	/// 参数  : int FieldNo
	/// 描述  : 取某一记录的某一字段的long值
    long   GetAtL(int Record,int FieldNo);      

	/// 函数名:   GetAtI
	/// 编程  : 王明松 2006-3-30 17:17:19
	/// 返回  : int  
	/// 参数  : int Record
	/// 参数  : int FieldNo
	/// 描述  : 取某一记录的某一字段的int值
    int    GetAtI(int Record,int FieldNo);      

	/// 函数名: GetAtS
	/// 编程  : 王明松 2006-3-30 17:17:22
	/// 返回  : string 
	/// 参数  : int Record
	/// 参数  : int FieldNo
	/// 描述  : 取某一记录的某一字段的string值
	std::string GetAtS(int Record,int FieldNo);
	
	/// 函数名: GetAt
	/// 编程  : 王明松 2006-3-30 17:16:47
	/// 返回  : int 
	/// 参数  : int Record
	/// 参数  : string fieldname 字段名
	/// 参数  : char *FieldData
	/// 描述  : 取某一记录的某一字段的char值
    int GetAt(int Record, std::string fieldname,char *FieldData);
	
	/// 函数名: GetAt
	/// 编程  : 王明松 2006-3-30 17:16:56
	/// 返回  : int 
	/// 参数  : int Record
	/// 参数  : string fieldname 字段名
	/// 参数  : double &FieldData
	/// 描述  : 取某一记录的某一字段的double值
    int GetAt(int Record, std::string fieldname,double &FieldData);
	
	/// 函数名: GetAt
	/// 编程  : 王明松 2006-3-30 17:17:00
	/// 返回  : int 
	/// 参数  : int Record
	/// 参数  : string fieldname 字段名
	/// 参数  : long &FieldData
	/// 描述  : 取某一记录的某一字段的long值
    int GetAt(int Record, std::string fieldname,long &FieldData);
	
	/// 函数名: GetAt
	/// 编程  : 王明松 2006-3-30 17:17:04
	/// 返回  : int 
	/// 参数  : int Record
	/// 参数  : string fieldname 字段名
	/// 参数  : int &FieldData
	/// 描述  : 取某一记录的某一字段的int值
    int GetAt(int Record, std::string fieldname,int &FieldData);
	
	
	/// 函数名: GetAt
	/// 编程  : 王明松 2006-3-30 17:17:08
	/// 返回  : char * 
	/// 参数  : int Record
	/// 参数  : string fieldname 字段名
	/// 描述  : 取某一记录的某一字段的char值
	char * GetAt(int Record, std::string fieldname);
	
	/// 函数名: GetAtD
	/// 编程  : 王明松 2006-3-30 17:17:12
	/// 返回  : double 
	/// 参数  : int Record
	/// 参数  : string fieldname 字段名
	/// 描述  : 取某一记录的某一字段的double值
	double GetAtD(int Record, std::string fieldname);
	
	/// 函数名:  GetAtL
	/// 编程  : 王明松 2006-3-30 17:17:15
	/// 返回  : long  
	/// 参数  : int Record
	/// 参数  : string fieldname 字段名
	/// 描述  : 取某一记录的某一字段的long值
    long   GetAtL(int Record, std::string fieldname);
	
	/// 函数名:   GetAtI
	/// 编程  : 王明松 2006-3-30 17:17:19
	/// 返回  : int  
	/// 参数  : int Record
	/// 参数  : string fieldname 字段名
	/// 描述  : 取某一记录的某一字段的int值
    int    GetAtI(int Record, std::string fieldname);
	
	/// 函数名: GetAtS
	/// 编程  : 王明松 2006-3-30 17:17:22
	/// 返回  : string 
	/// 参数  : int Record
	/// 参数  : string fieldname 字段名
	/// 描述  : 取某一记录的某一字段的string值
	std::string GetAtS(int Record, std::string fieldname);

public: 
    int          DB_ErrorCode;                            ///出错代码
    char         DB_ErrorInfo[300];                       ///出错信息

	/// 函数名: setSqlCount
	/// 编程  : 王明松 2006-3-30 17:16:18
	/// 返回  : void 
	/// 参数  : int count
	/// 描述  : 设置记录数
    void setSqlCount(int count);                          

	/// 函数名: put
	/// 编程  : 王明松 2006-3-30 17:15:58
	/// 返回  : int 
	/// 参数  : CSqlAPIData sqldata
	/// 描述  : 放入一条记录
    int put(CSqlAPIData sqldata);                     

	/// 函数名: GetAt
	/// 编程  : 王明松 2006-3-30 17:16:02
	/// 返回  : CSqlAPIData 
	/// 参数  : int index
	/// 描述  : 取一条记录
    CSqlAPIData GetAt(int index);   
	
    ~CSqlData();
    CSqlData();
};

#endif // !defined(AFX_SQLDATARESULT_H__1C6FF3FA_7134_49AD_B1E4_6EEDEC1001D4__INCLUDED_)
