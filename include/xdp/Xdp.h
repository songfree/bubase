// Xdp.h: interface for the CXdp class.
//
//////////////////////////////////////////////////////////////////////
//����Ϊһ��ͨ�õ�����Э���࣬����8583���ݰ�

#if !defined(AFX_XDP_H__00ABC1DA_A31B_4150_9238_C51CFCDF587F__INCLUDED_)
#define AFX_XDP_H__00ABC1DA_A31B_4150_9238_C51CFCDF587F__INCLUDED_



#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "XdpFmt.h"
#include "XdpRecord.h"


#ifdef XDP_EXPORTS
#define CXDP_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define CXDP_EXPORT __declspec(dllimport)
#else
#define CXDP_EXPORT 
#endif
#endif

//��¼����1��ʼ

class CXDP_EXPORT CXdp  
{
public:

	// ������: GetXdpRecord
	// ���  : ������ 2014-11-11 10:12:21
	// ����  : CXdpRecord *   NULLʧ��
	// ����  : unsigned short rec ��¼��1��ʼ
	// ����  : ȡ��һ����¼��
	CXdpRecord * GetXdpRecord(unsigned short rec);


	// ������: XdpCopy
	// ���  : ������ 2014-11-11 10:12:52
	// ����  : bool true�ɹ� falseʧ��
	// ����  : CXdp *src  Դ������
	// ����  : XDP��ʽ�����ĸ���
	bool XdpCopy(CXdp *src);

	// ������: GetRealPackSize
	// ���  : ������ 2014-10-30 11:31:20
	// ����  : unsigned short 
	// ����  : ȡ�õ�ǰʵ�ʱ��ĵĴ�С
	unsigned short GetRealPackSize();

	// ������: CancelField
	// ���  : ������ 2014-10-30 11:17:16
	// ����  : bool 
	// ����  : const char *fieldname
	// ����  : ȡ���ֶΣ������Ͳ������ɴ��ֶα��Ļ�ȡ��ֵ��
	bool CancelField(const char *fieldname);
	bool CancelField(unsigned short rec,const char *fieldname);

	// ������: CancelField
	// ���  : ������ 2014-10-30 11:16:28
	// ����  : bool 
	// ����  : unsigned short index
	// ����  : ȡ���ֶΣ������Ͳ������ɴ��ֶα��Ļ�ȡ��ֵ��
	bool CancelField(unsigned short index);
	bool CancelField(unsigned short rec,unsigned short index);

	// ������: PrintXdp
	// ���  : ������ 2014-10-30 11:15:33
	// ����  : void 
	// ����  : char *xdpbuf
	// ����  : ������������ʾ����������������תΪ16������ʾ
	void PrintXdp(char *xdpbuf);

	// ������: GetCurSavePackNum
	// ���  : ������ 2014-10-29 11:45:49
	// ����  : unsigned short 
	// ����  : ȡ�ñ�����¼�Ľṹ��Ŀ��ע�ⲻ������һ����¼
	unsigned short GetCurSavePackNum();

	// ������: ClearNextRecord
	// ���  : ������ 2014-10-29 11:34:21
	// ����  : void 
	// ����  : ��ձ�����¼�Ľṹ
	void ClearNextRecord();
	

	// ������: GetFieldType
	// ���  : ������ 2014-10-29 11:22:23
	// ����  : unsigned short ���ͣ���XdpFmt.h�궨��
	// ����  : const char *fieldname/unsigned short index �ֶ�����/����
	// ����  : ȡ���ֶ�����
	unsigned short GetFieldType(const char *fieldname);
	unsigned short GetFieldType(unsigned short index);

	// ������: GetRecCount
	// ���  : ������ 2014-10-29 11:10:29
	// ����  : unsigned short ��¼��
	// ����  : ���ص�ǰ�����й��м�����¼��ע���¼���ͱ�����¼�Ľṹ��Ŀ����һ������
	unsigned short GetRecCount();

	// ������: GetFieldValue
	// ���  : ������ 2014-10-29 11:04:09
	// ����  : bool true�ɹ� falseʧ��
	// ����  : unsigned short rec    �ڼ�����¼ ��1��ʼ
	// ����  : unsigned short index/const char *fieldname  �ֶ�����/����
	// ����  : char *data      ���ݻ���  ��Ϊ��ֵ��ʱ��Ҫ��ǿ��ת��  �磺int *aa = (int *)data;
	// �����������  : unsigned int &datalen   ����Ϊdata�����С ���Ϊʵ�����ݳ���
	// ����  : int &fieldtype  �ֶ����ͣ���XdpFmt.h�к궨��
	// ����  : char *errmsg  ������Ϣ
	// ����  : ��xdp��ȡ�ֶ�ֵ
	bool GetFieldValue(unsigned short rec,const char *fieldname,char *data,unsigned int &datalen,int &fieldtype,char *errmsg);
	bool GetFieldValue(unsigned short rec,const char *fieldname,int &idata,char *errmsg);
	bool GetFieldValue(unsigned short rec,const char *fieldname,unsigned int &idata,char *errmsg);
	bool GetFieldValue(unsigned short rec,const char *fieldname,short &idata,char *errmsg);
	bool GetFieldValue(unsigned short rec,const char *fieldname,unsigned short &idata,char *errmsg);
	bool GetFieldValue(unsigned short rec,const char *fieldname,double &idata,char *errmsg);
	
	
	bool GetFieldValue(unsigned short rec,unsigned short index,char *data,unsigned int &datalen,int &fieldtype,char *errmsg);
	bool GetFieldValue(unsigned short rec,unsigned short index,int &idata,char *errmsg);
	bool GetFieldValue(unsigned short rec,unsigned short index,unsigned int &idata,char *errmsg);
	bool GetFieldValue(unsigned short rec,unsigned short index,short &idata,char *errmsg);
	bool GetFieldValue(unsigned short rec,unsigned short index,unsigned short &idata,char *errmsg);
	bool GetFieldValue(unsigned short rec,unsigned short index,double &idata,char *errmsg);

	// ������: GetFieldValue
	// ���  : ������ 2014-10-29 11:11:55
	// ����  : bool true�ɹ� falseʧ��
	// ����  : unsigned short index/const char *fieldname �ֶ�����/����
	// ����  : char *data       ���ݻ��� ��Ϊ��ֵ��ʱ��Ҫ��ǿ��ת����ȡֵ  �磺int *aa = (int *)data;
	// ����  : unsigned int &datalen     ����Ϊdata�����С ���Ϊʵ�����ݳ���
	// ����  : int &fieldtype  �ֶ����ͣ���XdpFmt.h�к궨��
	// ����  : char *errmsg   ������Ϣ
	// ����  : ��xdp��ȡ��һ����¼���ֶ�ֵ
	bool GetFieldValue(unsigned short index,char *data,unsigned int &datalen,int &fieldtype,char *errmsg);
	bool GetFieldValue(unsigned short index,int &idata,char *errmsg);
	bool GetFieldValue(unsigned short index,unsigned int &idata,char *errmsg);
	bool GetFieldValue(unsigned short index,short &idata,char *errmsg);
	bool GetFieldValue(unsigned short index,unsigned short &idata,char *errmsg);
	bool GetFieldValue(unsigned short index,double &idata,char *errmsg);

	bool GetFieldValue(const char *fieldname,char *data,unsigned int &datalen,int &fieldtype,char *errmsg);
	bool GetFieldValue(const char *fieldname,int &idata,char *errmsg);
	bool GetFieldValue(const char *fieldname,unsigned int &idata,char *errmsg);
	bool GetFieldValue(const char *fieldname,short &idata,char *errmsg);
	bool GetFieldValue(const char *fieldname,unsigned short &idata,char *errmsg);
	bool GetFieldValue(const char *fieldname,double &idata,char *errmsg);




	// ������: SetFieldValue
	// ���  : ������ 2014-10-29 11:40:30
	// ����  : bool 
	// ����  : unsigned short rec  1��ʼ  Ҫ����Ϊ��¼����ע����rec����ԭ��¼����ô��¼��Ҫ��С����˳������
	// ����  : unsigned short index/const char *fieldname �ֶ�����/����
	// ����  : const char *data   ���ݻ��� ��Ϊ��ֵ��ʱ��Ҫ��ǿ��ת������ֵ  �磺int *aa = (int *)data; *aa = 1200
	// ����  : int datalen     ���ݳ���
	// ����  : char *errmsg    ������Ϣ
	// ����  : ����ָ����¼�����ֶ�ֵ
	bool SetFieldValue(unsigned short rec,const char *fieldname,const char *data,unsigned int datalen,char *errmsg);
	
	bool SetFieldValueM(unsigned short rec,const char *fieldname,int idata,char *errmsg);
	bool SetFieldValueM(unsigned short rec,const char *fieldname,unsigned int idata,char *errmsg);
	bool SetFieldValueM(unsigned short rec,const char *fieldname,short idata,char *errmsg);
	bool SetFieldValueM(unsigned short rec,const char *fieldname,unsigned short idata,char *errmsg);
	bool SetFieldValueM(unsigned short rec,const char *fieldname,double idata,char *errmsg);

	bool SetFieldValue(unsigned short rec,unsigned short index,const char *data,unsigned int datalen,char *errmsg);
	bool SetFieldValue(unsigned short rec,unsigned short index,int idata,char *errmsg);
	bool SetFieldValue(unsigned short rec,unsigned short index,unsigned int idata,char *errmsg);
	bool SetFieldValue(unsigned short rec,unsigned short index,short idata,char *errmsg);
	bool SetFieldValue(unsigned short rec,unsigned short index,unsigned short idata,char *errmsg);
	bool SetFieldValue(unsigned short rec,unsigned short index,double idata,char *errmsg);



	// ������: SetFieldValue
	// ���  : ������ 2014-10-29 11:40:35
	// ����  : bool 
	// ����  : unsigned short index/const char *fieldname �ֶ�����/����  
	// ����  : const char *data   ���ݻ��� ��Ϊ��ֵ��ʱ��Ҫ��ǿ��ת������ֵ  �磺int *aa = (int *)data; *aa = 1200
	// ����  : int datalen    ���ݳ���
	// ����  : char *errmsg  ������Ϣ
	// ����  : ���õ�һ����¼���ֶ�ֵ
	bool SetFieldValue(unsigned short index,const char *data,unsigned int datalen,char *errmsg);
	bool SetFieldValue(unsigned short index,int idata,char *errmsg);
	bool SetFieldValue(unsigned short index,unsigned int idata,char *errmsg);
	bool SetFieldValue(unsigned short index,short idata,char *errmsg);
	bool SetFieldValue(unsigned short index,unsigned short idata,char *errmsg);
	bool SetFieldValue(unsigned short index,double idata,char *errmsg);

	bool SetFieldValue(const char *fieldname,const char *data,unsigned int datalen,char *errmsg);
	bool SetFieldValue(const char *fieldname,int idata,char *errmsg);
	bool SetFieldValue(const char *fieldname,unsigned int idata,char *errmsg);
	bool SetFieldValue(const char *fieldname,short idata,char *errmsg);
	bool SetFieldValue(const char *fieldname,unsigned short idata,char *errmsg);
	bool SetFieldValue(const char *fieldname,double idata,char *errmsg);


	// ������: SetCurRecCount
	// ���  : ������ 2014-10-29 10:55:12
	// ����  : bool true�ɹ�  falseʧ�ܣ���ԭ���ĵļ�¼Y<Xʱʧ��
	// ����  : unsigned short rec 
	// ����  : ���õ�ǰ�ļ�¼������ԭ��������Y����¼ʱ������ֻ����X(X<Y)����¼ʱ���������ü�¼��ΪX,���򷵻ص���ΪY��
	bool SetCurRecCount(unsigned short x);

	// ������: ResetData
	// ���  : ������ 2014-10-27 14:24:43
	// ����  : void 
	// ����  : ������¼������λͼ
	void ResetData();

	// ������: ToBuffer
	// ���  : ������ 2014-10-27 14:18:52
	// ����  : bool true�ɹ� falseʧ��
	// ����  : char *xdpbuf   xdp����
	// �����������  : int &len    ����Ϊxdpbuf��С,���Ϊxdp���Ĵ�С
	// ����  : char *errmsg   ������Ϣ
	// ����  : ����xdp����
	bool ToBuffer(char *xdpbuf,unsigned int &len,char *errmsg);

	// ������: FromBuffer
	// ���  : ������ 2014-10-27 14:17:34
	// ����  : bool true�ɹ� falseʧ��
	// ����  : const char *xdpbuf   xdp���ݻ���
	// ����  : int xdpbuflen   xdp���ݳ���
	// ����  : char *errmsg   ������Ϣ
	// ����  : �����ݻ����������
	bool FromBuffer(const char *xdpbuf,unsigned int xdpbuflen,char *errmsg);

	// ������: InitXdp
	// ���  : ������ 2014-10-27 14:14:55
	// ����  : bool true�ɹ� falseʧ��
	// ����  : const char *datafile   xml�����ļ�
	// ����  : char *errmsg      ������Ϣ
	// ����  : ��ʼ�����ģ�
	bool InitXdp(const char *datafile,char *errmsg);

	CXdp();
	virtual ~CXdp();

	CXdpFmt          m_pXdpFmt;//xdp����

protected:

	unsigned short   m_nRecCount;//��¼��

	CXdpRecord       m_pFirstRecord;//��һ����¼

	std::vector<CXdpRecord *>  m_vNextRecord;//����������������� 

};

#endif // !defined(AFX_XDP_H__00ABC1DA_A31B_4150_9238_C51CFCDF587F__INCLUDED_)
