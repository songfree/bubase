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
*       ���ݼ�¼��
*   ÿ����¼��Ϊһ���ౣ��
******************************/
class DBAPI_EXPORT CSqlAPIData
{
private: 
	std::vector <S_RECORD> SqlRecordBuffer;
    int  SqlFieldCount;                                                   ///�ֶ���
public: 
	int Add(S_RECORD sdata);
    int Clear();                                                          ///����ֶ�
    int GetCount();                                                       ///ȡ�ֶ���

	/// ������: SetAt
	/// ���  : ������ 2006-3-30 17:13:19
	/// ����  : int 
	/// ����  : char *buffer
	/// ����  : int nIndex
	/// ����  : updateָ���ֶδ�0��ʼ
    int SetAt(char *buffer,int nIndex);              

	/// ������: SetAt
	/// ���  : ������ 2006-3-30 17:13:32
	/// ����  : int 
	/// ����  : string buffer
	/// ����  : int nIndex
	/// ����  : updateָ���ֶ� ��0��ʼ
	int SetAt(std::string buffer,int nIndex);
	

	/// ������: GetAt
	/// ���  : ������ 2006-3-30 17:13:53
	/// ����  : int 
	/// ����  : int nIndex
	/// ����  : char *FieldData
	/// ����  : ȡָ���ֶ� ��0��ʼ
    int GetAt(int nIndex,char *FieldData);           

	/// ������: GetAt
	/// ���  : ������ 2006-3-30 17:13:56
	/// ����  : int 
	/// ����  : int nIndex
	/// ����  : double &FieldData
	/// ����  : ȡָ���ֶ� ��0��ʼ
    int GetAt(int nIndex,double &FieldData);          

	/// ������: GetAt
	/// ���  : ������ 2006-3-30 17:14:00
	/// ����  : int 
	/// ����  : int nIndex
	/// ����  : long &FieldData
	/// ����  : ȡָ���ֶ� ��0��ʼ
    int GetAt(int nIndex,long &FieldData);            

	/// ������: GetAt
	/// ���  : ������ 2006-3-30 17:14:04
	/// ����  : int 
	/// ����  : int nIndex
	/// ����  : int &FieldData
	/// ����  : ȡָ���ֶ� ��0��ʼ
    int GetAt(int nIndex,int &FieldData);             
    

	/// ������: *GetAt
	/// ���  : ������ 2006-3-30 17:14:28
	/// ����  : char 
	/// ����  : int nIndex
	/// ����  : ȡָ���ֶε�charֵ ��0��ʼ
	char *GetAt(int nIndex);           

	/// ������: GetAtD
	/// ���  : ������ 2006-3-30 17:14:31
	/// ����  : double 
	/// ����  : int nIndex
	/// ����  : ȡָ���ֶ� ��0��ʼ
    double GetAtD(int nIndex);         

	/// ������: GetAtL
	/// ���  : ������ 2006-3-30 17:14:35
	/// ����  : long 
	/// ����  : int nIndex
	/// ����  : ȡָ���ֶ� ��0��ʼ
    long GetAtL(int nIndex);            

	/// ������: GetAtI
	/// ���  : ������ 2006-3-30 17:14:38
	/// ����  : int 
	/// ����  : int nIndex
	/// ����  : ȡָ���ֶ� ��0��ʼ
    int GetAtI(int nIndex);             

	/// ������: GetAtS
	/// ���  : ������ 2006-3-30 17:14:42
	/// ����  : string 
	/// ����  : int nIndex
	/// ����  : ȡָ���ֶ� ��0��ʼ
	std::string GetAtS(int nIndex) ;


	/// ������: GetAt
	/// ���  : ������ 2006-3-30 17:13:53
	/// ����  : int 
	/// ����  : string fieldname �ֶ���
	/// ����  : char *FieldData
	/// ����  : ȡָ���ֶ�
    int GetAt(std::string fieldname,char *FieldData);
	
	/// ������: GetAt
	/// ���  : ������ 2006-3-30 17:13:56
	/// ����  : int 
	/// ����  : string fieldname �ֶ���
	/// ����  : double &FieldData
	/// ����  : ȡָ���ֶ� 
    int GetAt(std::string fieldname,double &FieldData);
	
	/// ������: GetAt
	/// ���  : ������ 2006-3-30 17:14:00
	/// ����  : int 
	/// ����  : string fieldname �ֶ���
	/// ����  : long &FieldData
	/// ����  : ȡָ���ֶ� 
    int GetAt(std::string fieldname,long &FieldData);
	
	/// ������: GetAt
	/// ���  : ������ 2006-3-30 17:14:04
	/// ����  : int 
	/// ����  : string fieldname �ֶ���
	/// ����  : int &FieldData
	/// ����  : ȡָ���ֶ� ��0��ʼ
    int GetAt(std::string fieldname,int &FieldData);
    
	
	/// ������: *GetAt
	/// ���  : ������ 2006-3-30 17:14:28
	/// ����  : char 
	/// ����  : string fieldname �ֶ���
	/// ����  : ȡָ���ֶε�charֵ
	char *GetAt(std::string fieldname);
	
	/// ������: GetAtD
	/// ���  : ������ 2006-3-30 17:14:31
	/// ����  : double 
	/// ����  : string fieldname �ֶ���
	/// ����  : ȡָ���ֶ� 
    double GetAtD(std::string fieldname);
	
	/// ������: GetAtL
	/// ���  : ������ 2006-3-30 17:14:35
	/// ����  : long 
	/// ����  : string fieldname �ֶ���
	/// ����  : ȡָ���ֶ� 
    long GetAtL(std::string fieldname);
	
	/// ������: GetAtI
	/// ���  : ������ 2006-3-30 17:14:38
	/// ����  : int 
	/// ����  : string fieldname �ֶ���
	/// ����  : ȡָ���ֶ� 
    int GetAtI(std::string fieldname);
	
	/// ������: GetAtS
	/// ���  : ������ 2006-3-30 17:14:42
	/// ����  : string 
	/// ����  : string fieldname �ֶ���
	/// ����  : ȡָ���ֶ� 
	std::string GetAtS(std::string fieldname) ;

	/// ������: Add
	/// ���  : ������ 2006-3-30 17:15:23
	/// ����  : int 
	/// ����  : char *buffer
	/// ����  : ��һ���ֶε����
    int Add(char *buffer);                           

	/// ������: Add
	/// ���  : ������ 2006-3-30 17:15:27
	/// ����  : int 
	/// ����  : string buffer
	/// ����  : ��һ���ֶε����
	int Add(std::string buffer);

    ~CSqlAPIData();
    CSqlAPIData();
};
/********************************
*     ���ݼ�����
*  �ṩ������������ȡ����
*********************************/
class DBAPI_EXPORT CSqlData
{
private: 
	std::vector <CSqlAPIData> SqlData;  ///���ݼ���
    int  SqlCount;                     ///��¼��
public: 

	/// ������: GetSqlCount
	/// ���  : ������ 2006-3-30 17:16:30
	/// ����  : int 
	/// ����  : ȡ��¼��
    int GetSqlCount();                 

	/// ������: clear
	/// ���  : ������ 2006-3-30 17:16:39
	/// ����  : int 
	/// ����  : ��ռ�¼
    int clear();                       

	/// ������: GetAt
	/// ���  : ������ 2006-3-30 17:16:47
	/// ����  : int 
	/// ����  : int Record
	/// ����  : int FieldNo
	/// ����  : char *FieldData
	/// ����  : ȡĳһ��¼��ĳһ�ֶε�charֵ
    int GetAt(int Record,int FieldNo,char *FieldData);     

	/// ������: GetAt
	/// ���  : ������ 2006-3-30 17:16:56
	/// ����  : int 
	/// ����  : int Record
	/// ����  : int FieldNo
	/// ����  : double &FieldData
	/// ����  : ȡĳһ��¼��ĳһ�ֶε�doubleֵ
    int GetAt(int Record,int FieldNo,double &FieldData);    

	/// ������: GetAt
	/// ���  : ������ 2006-3-30 17:17:00
	/// ����  : int 
	/// ����  : int Record
	/// ����  : int FieldNo
	/// ����  : long &FieldData
	/// ����  : ȡĳһ��¼��ĳһ�ֶε�longֵ
    int GetAt(int Record,int FieldNo,long &FieldData);      

	/// ������: GetAt
	/// ���  : ������ 2006-3-30 17:17:04
	/// ����  : int 
	/// ����  : int Record
	/// ����  : int FieldNo
	/// ����  : int &FieldData
	/// ����  : ȡĳһ��¼��ĳһ�ֶε�intֵ
    int GetAt(int Record,int FieldNo,int &FieldData);      


	/// ������: GetAt
	/// ���  : ������ 2006-3-30 17:17:08
	/// ����  : char * 
	/// ����  : int Record
	/// ����  : int FieldNo
	/// ����  : ȡĳһ��¼��ĳһ�ֶε�charֵ
	char * GetAt(int Record,int FieldNo);     

	/// ������: GetAtD
	/// ���  : ������ 2006-3-30 17:17:12
	/// ����  : double 
	/// ����  : int Record
	/// ����  : int FieldNo
	/// ����  : ȡĳһ��¼��ĳһ�ֶε�doubleֵ
	double GetAtD(int Record,int FieldNo);    

	/// ������:  GetAtL
	/// ���  : ������ 2006-3-30 17:17:15
	/// ����  : long  
	/// ����  : int Record
	/// ����  : int FieldNo
	/// ����  : ȡĳһ��¼��ĳһ�ֶε�longֵ
    long   GetAtL(int Record,int FieldNo);      

	/// ������:   GetAtI
	/// ���  : ������ 2006-3-30 17:17:19
	/// ����  : int  
	/// ����  : int Record
	/// ����  : int FieldNo
	/// ����  : ȡĳһ��¼��ĳһ�ֶε�intֵ
    int    GetAtI(int Record,int FieldNo);      

	/// ������: GetAtS
	/// ���  : ������ 2006-3-30 17:17:22
	/// ����  : string 
	/// ����  : int Record
	/// ����  : int FieldNo
	/// ����  : ȡĳһ��¼��ĳһ�ֶε�stringֵ
	std::string GetAtS(int Record,int FieldNo);
	
	/// ������: GetAt
	/// ���  : ������ 2006-3-30 17:16:47
	/// ����  : int 
	/// ����  : int Record
	/// ����  : string fieldname �ֶ���
	/// ����  : char *FieldData
	/// ����  : ȡĳһ��¼��ĳһ�ֶε�charֵ
    int GetAt(int Record, std::string fieldname,char *FieldData);
	
	/// ������: GetAt
	/// ���  : ������ 2006-3-30 17:16:56
	/// ����  : int 
	/// ����  : int Record
	/// ����  : string fieldname �ֶ���
	/// ����  : double &FieldData
	/// ����  : ȡĳһ��¼��ĳһ�ֶε�doubleֵ
    int GetAt(int Record, std::string fieldname,double &FieldData);
	
	/// ������: GetAt
	/// ���  : ������ 2006-3-30 17:17:00
	/// ����  : int 
	/// ����  : int Record
	/// ����  : string fieldname �ֶ���
	/// ����  : long &FieldData
	/// ����  : ȡĳһ��¼��ĳһ�ֶε�longֵ
    int GetAt(int Record, std::string fieldname,long &FieldData);
	
	/// ������: GetAt
	/// ���  : ������ 2006-3-30 17:17:04
	/// ����  : int 
	/// ����  : int Record
	/// ����  : string fieldname �ֶ���
	/// ����  : int &FieldData
	/// ����  : ȡĳһ��¼��ĳһ�ֶε�intֵ
    int GetAt(int Record, std::string fieldname,int &FieldData);
	
	
	/// ������: GetAt
	/// ���  : ������ 2006-3-30 17:17:08
	/// ����  : char * 
	/// ����  : int Record
	/// ����  : string fieldname �ֶ���
	/// ����  : ȡĳһ��¼��ĳһ�ֶε�charֵ
	char * GetAt(int Record, std::string fieldname);
	
	/// ������: GetAtD
	/// ���  : ������ 2006-3-30 17:17:12
	/// ����  : double 
	/// ����  : int Record
	/// ����  : string fieldname �ֶ���
	/// ����  : ȡĳһ��¼��ĳһ�ֶε�doubleֵ
	double GetAtD(int Record, std::string fieldname);
	
	/// ������:  GetAtL
	/// ���  : ������ 2006-3-30 17:17:15
	/// ����  : long  
	/// ����  : int Record
	/// ����  : string fieldname �ֶ���
	/// ����  : ȡĳһ��¼��ĳһ�ֶε�longֵ
    long   GetAtL(int Record, std::string fieldname);
	
	/// ������:   GetAtI
	/// ���  : ������ 2006-3-30 17:17:19
	/// ����  : int  
	/// ����  : int Record
	/// ����  : string fieldname �ֶ���
	/// ����  : ȡĳһ��¼��ĳһ�ֶε�intֵ
    int    GetAtI(int Record, std::string fieldname);
	
	/// ������: GetAtS
	/// ���  : ������ 2006-3-30 17:17:22
	/// ����  : string 
	/// ����  : int Record
	/// ����  : string fieldname �ֶ���
	/// ����  : ȡĳһ��¼��ĳһ�ֶε�stringֵ
	std::string GetAtS(int Record, std::string fieldname);

public: 
    int          DB_ErrorCode;                            ///�������
    char         DB_ErrorInfo[300];                       ///������Ϣ

	/// ������: setSqlCount
	/// ���  : ������ 2006-3-30 17:16:18
	/// ����  : void 
	/// ����  : int count
	/// ����  : ���ü�¼��
    void setSqlCount(int count);                          

	/// ������: put
	/// ���  : ������ 2006-3-30 17:15:58
	/// ����  : int 
	/// ����  : CSqlAPIData sqldata
	/// ����  : ����һ����¼
    int put(CSqlAPIData sqldata);                     

	/// ������: GetAt
	/// ���  : ������ 2006-3-30 17:16:02
	/// ����  : CSqlAPIData 
	/// ����  : int index
	/// ����  : ȡһ����¼
    CSqlAPIData GetAt(int index);   
	
    ~CSqlData();
    CSqlData();
};

#endif // !defined(AFX_SQLDATARESULT_H__1C6FF3FA_7134_49AD_B1E4_6EEDEC1001D4__INCLUDED_)
