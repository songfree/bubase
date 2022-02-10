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



#define   GTP_DATATYPE_UNKNOW   0            //δ֪
#define   GTP_DATATYPE_VALUE    1            //ֵ������ 
#define   GTP_DATATYPE_HASH     2            //hash��  {xy=1,xy2=2}
#define   GTP_DATATYPE_ARRAY    3            //����    [{xy=1,xy2=2},{xy3=1,xy4=2}


#define   GTP_KEY_LEN           10           //KEY�ĳ���

//����Ϊx=1,x1=2,x3={xy=1,xy2=2},x4=[{xy=1,xy2=2},{xy3=1,xy4=2}] �������͵�����
//������һ�����ݽڵ��࣬����key��ֵ��hash�����顣
//hash�������Ҳ��һ����������ʾ��
//�������������ͣ����������Ͷ���

class CGtpData;

typedef struct 
{
	char             sFieldName[GTP_KEY_LEN+1];  //keyֵ
	std::string      sFieldValue;     //ֵ
	unsigned  char   cDataType;       //����
	bool             bIsParseLocal;   //���ڵ��Ƿ����
	CGtpData        *pGtpData;        //������ָ��
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

	// ������: UpdateCurValue
	// ���  : ������ 2016-4-20 8:55:08
	// ����  : bool 
	// ����  : CGtpData *src  ������ڵ�ֻȡֵȥ����
	// ����  : bool isUpdate=false  �����ڵ��д��ֶ�ʱ���Ƿ���� false������ true����
	// ����  : ���µ�ǰ��ֵ
	bool UpdateCurValue(CGtpData *src,bool isUpdate=false);
	
	// ������: ToStringDelRec
	// ���  : ������ 2016-3-3 17:50:09
	// ����  : bool 
	// ����  : std::string &gtpstring
	// ����  : int delnum
	// ����  : ����gtpʱȥ��ǰ�漸���ֶ�
	bool ToStringDelRec(std::string &gtpstring,int delnum);

	// ������: AddDataItem
	// ���  : ������ 2016-3-2 15:34:38
	// ����  : CGtpData * 
	// ����  : const char *fieldname  �������KEY
	// ����  : char datatype  ����������
	// ����  : ����һ���յ�������  ��Ϊ�������ͣ���������ֻ����hash������ע������hashʱ��fieldname��ô�"0"��ʼ���ᰴ������򣬷���������ɵ�˳��һ��
	CGtpData * AddDataItem(const char *fieldname,char datatype);

	// ������: SetDataItem
	// ���  : ������ 2016-3-2 15:31:45
	// ����  : bool 
	// ����  : const char *fieldname  key
	// ����  : const char *fieldvalue value
	// ����  : bool istrans=true    �Ƿ���ת���鴦��
	// ����  : ��������������ڵ������KVֵ  �����ڵ�Ϊ����ֱ�ӷ��س���
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

	// ������: ParseGtp
	// ���  : ������ 2016-2-29 19:53:34
	// ����  : bool 
	// ����  : int layer=1
	// ����  : �����ڵ�����  >0Ϊ�����㼶 1ֻ������ǰ
	bool ParseGtp(int layer=1);

	// ������: Clear
	// ���  : ������ 2016-2-29 19:43:04
	// ����  : void 
	// ����  : �������
	void Clear();

	
	

	// ������: GetDataPoint
	// ���  : ������ 2016-2-29 14:53:21
	// ����  : CGtpData * =NULL�޴������hashֵ
	// ����  : const char *fieldname
	// ����  : ����key��ȡ�ö�Ӧ���������ͣ�Ȼ���������ָ���hashָ������ȡֵ��ֵ����
	CGtpData * GetDataPoint(const char *fieldname);


	// ������: GetDataPoint
	// ���  : ������ 2016-3-2 10:27:53
	// ����  : CGtpData * =NULL�޴������hashֵ
	// ����  : unsigned int recnum   ��¼��  ��0��ʼ  ע���¼�ǰ���С���������
	// ����  : ���ݼ�¼����ȡ�ö�Ӧ���������ͣ�������������ȡ����Ϊ�������hash��û�����ƣ���FieldCount���ʹ��
	CGtpData * GetDataPoint(unsigned int recnum);

	// ������: GetArraySize
	// ���  : ������ 2016-2-29 15:06:56
	// ����  : int  <0�޴�����  >0Ϊ���ݴ�С
	// �������  : unsigned int arrayrowid   �����Ӧ��rowid����rowid��GetRowId�õ�
	// ����  : ȡ��ָ������������ݸ���  
	int GetArraySize(unsigned int arrayrowid);
	

	// ������: FieldCount
	// ���  : ������ 2016-3-2 9:58:00
	// ����  : int 
	// ����  : ȡ�õ�ǰ����ֶθ�������Ϊhash��value��Ϊ�ֶ�������Ϊarray��Ϊ�������
	int FieldCount();

	// ������: GetHashValue
	// ���  : ������ 2016-2-29 15:10:41
	// ����  : int =0�ɹ� <0�޴��ֶ�key   >0�ֶ����Ͳ���
	// �������  : unsigned int hashrowid  ��ϣ���rowid���ɵ�GetRowId�õ�
	// �������  : const char *fieldname   �ֶε�key 
	// �������  : unsigned  short &fieldvalue   �ֶ�ֵ
	// �������  : int &fieldtype                �д��ֶ�Ϊ��������
	// ����  : ȡ�õ�ǰ�ڵ��¹�ϣ�����ֵ
	int GetHashValue(unsigned int hashrowid,const char *fieldname, unsigned  short &fieldvalue, int &fieldtype);
	int GetHashValue(unsigned int hashrowid,const char *fieldname, unsigned long &fieldvalue, int &fieldtype);
	int GetHashValue(unsigned int hashrowid,const char *fieldname, unsigned int  &fieldvalue, int &fieldtype);
	int GetHashValue(unsigned int hashrowid,const char *fieldname, double &fieldvalue, int &fieldtype);
	int GetHashValue(unsigned int hashrowid,const char *fieldname, int &fieldvalue, int &fieldtype);
	int GetHashValue(unsigned int hashrowid,const char *fieldname, long &fieldvalue, int &fieldtype);
	int GetHashValue(unsigned int hashrowid,const char *fieldname, short &fieldvalue, int &fieldtype);
	int GetHashValue(unsigned int hashrowid,const char *fieldname ,char *fieldvalue,int &fieldtype);
	int GetHashValue(unsigned int hashrowid,const char *fieldname ,std::string &fieldvalue,int &fieldtype);


	// ������: GetRowId
	// ���  : ������ 2016-2-29 15:07:37
	// ����  : int >=0�ɹ�  ����ʧʧ��
	// �������  : const char *fieldname   �ֶ�key
	// �������  : int &fieldtype   ��������
	// ����  : ȡ��key��Ӧ��rowid��Ȼ��ɸ���row��ȡ����ֵ��ֻ�й�ϣ��������ظ�ȡֵ����
	int GetRowId(const char *fieldname,int &fieldtype);


	// ������: GetDataType
	// ���  : ������ 2016-2-29 15:15:56
	// ����  : int 
	// ����  : ȡ���ڵ����������
	int GetDataType();
	

	// ������: GetValueByName
	// ���  : ������ 2016-2-29 15:16:11
	// ����  : int  >=0 �ɹ�Ϊ���ֶε�rowid    <0  �޴��ֶλ��ֶ����Ͳ��� -1�޴��ֶ�  -2���Ͳ���
	// �������  : const char *fieldname    �ֶε�key
	// �������  : unsigned  short &fieldvalue   �ֶ�ֵ
	// �������  : int &fieldtype            �ֶ�����
	// ����  : �����ֶ�keyȡֵ
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


	// ������: GetValueByName
	// ���  : ������ 2016-4-20 9:09:36
	// ����  : int >=0 �ɹ�Ϊ���ֶε�rowid    <0  �޴��ֶλ��ֶ����Ͳ��� -1�޴��ֶ�  -2���Ͳ���
	// ����  : unsigned int recnum  ͨ��FieldCount���õ���������¼��Ȼ��ѭ����ȡ
	// �������  : unsigned  short &fieldvalue   �ֶ�ֵ
	// �������  : int &fieldtype
	// ����  : ���ݼ�¼����ȡ�ֶ�ֵ  ֵ������hash������
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

	// ������: GetGtpDataLen
	// ���  : ������ 2016-2-29 15:18:40
	// ����  : int 
	// ����  : ȡ�ñ��ڵ�����ݳ���
	int GetGtpDataLen();


	// ������: ToString
	// ���  : ������ 2016-2-29 15:18:53
	// ����  : bool 
	// ����  : std::string &gtpstring
	// ����  : �����ı�����
	bool ToString(std::string &gtpstring);

	// ������: ToFile
	// ���  : ������ 2016-2-29 15:19:05
	// ����  : bool 
	// ����  : const char *gtpfilename
	// ����  : ���ı�����������ļ�
	bool ToFile(const char *gtpfilename);

	// ������: ToBuffer
	// ���  : ������ 2016-2-29 15:19:23
	// ����  : bool 
	// ����  : char *gtpbuffer
	// ����  : int &buflen
	// ����  : �����ı�����
	bool ToBuffer(char *gtpbuffer ,int &buflen);

	// ������: FromFile
	// ���  : ������ 2016-2-29 15:19:39
	// ����  : bool 
	// ����  : const char *gtpfile
	// ����  : int layer=0   0Ϊֻ�������㣬��������ȡ���ݽ���   1Ϊ������һ�㣬������ϣ������
	// ����  : ���ļ����ȡgtp���ģ�������
	bool FromFile(const char *gtpfile,int layer=1);

	// ������: FromBuffer
	// ���  : ������ 2016-2-29 15:20:03
	// ����  : bool 
	// ����  : const char *gtpdata  
	// ����  : int layer=0    0Ϊֻ�������㣬��������ȡ���ݽ���   1Ϊ������һ�㣬������ϣ������
	// ����  : �ӻ������ȡgtp���ģ�������
	bool FromBuffer(const char *gtpdata,int layer=1);


	int GtpGetTransSign(std::string &value);

	int GtpPutTransSign(std::string &value);

	CGtpData();
	virtual ~CGtpData();

protected:

	// ������: SetKeyValue
	// ���  : ������ 2016-2-29 19:43:15
	// ����  : bool 
	// ����  : const std::string &key
	// ����  : const std::string &svalue
	// ����  : char datatype  
	// ����  : ���õ�ǰ�ڵ�key��ֵ������ 
	bool SetCurKeyValue(const std::string &key,const std::string &svalue,char datatype);

	// ������: Insert
	// ���  : ������ 2016-3-2 9:12:31
	// ����  : int <0ʧ��  >0 rowid
	// ����  : S_GTP_DATA_ data
	// ����  : ����һ����¼kv��hash��array
	int Insert(S_GTP_DATA_ data);

	void bzeroData(S_GTP_DATA_ &data);


	// ������: ParseGtpArray
	// ���  : ������ 2016-3-2 9:11:47
	// ����  : bool 
	// ����  : int layer
	// ����  : ��������
	bool ParseGtpArray(int layer);

	// ������: ParseGtpHash
	// ���  : ������ 2016-3-2 9:12:03
	// ����  : bool 
	// ����  : int layer
	// ����  : ������ϣ��
	bool ParseGtpHash(int layer);

	// ������: ParseGtpValue
	// ���  : ������ 2016-3-2 9:12:15
	// ����  : bool 
	// ����  : int layer
	// ����  : ����kv
	bool ParseGtpValue(int layer);


	// ������: FindArrayOffset
	// ���  : ������ 2016-3-1 10:56:01
	// ����  : int 
	// ����  : int begin
	// ����  : int end
	// ����  : const char *gtpdata
	// ����  : ��ѯ����Ľ���λ��
	int FindArrayOffset(int begin,int end ,const char *gtpdata);

	// ������: FindHashOffset
	// ���  : ������ 2016-3-1 10:55:45
	// ����  : int 
	// ����  : int begin
	// ����  : int end
	// ����  : const char *gtpdata
	// ����  : ���ҹ�ϣ��Ľ���λ��
	int FindHashOffset(int begin,int end,const char *gtpdata);

	std::string  m_sGtpKey;  //keyֵ
	std::string  m_sGtpValue; //����    ����[{}]

	unsigned   char   m_cDataType;//�����ݽڵ������   GTP_DATATYPE_VALUE ��ϣ��  ����
	CMemTableNew <S_GTP_DATA_>     m_table;   
	CPkeyCharV<1>           m_keyIndex;     //���Ƶ�����

	CBF_SimpleLog  m_pLog;
	
};

#endif // !defined(AFX_GTPDATA_H__35334B97_ADCF_41EE_8157_0537A846AC7F__INCLUDED_)
