// XdpRecord.h: interface for the CXdpRecord class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XDPRECORD_H__86FFC8E4_2683_43CF_AE23_D6442D392E9B__INCLUDED_)
#define AFX_XDPRECORD_H__86FFC8E4_2683_43CF_AE23_D6442D392E9B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "XdpFmt.h"
#include "BF_Tools.h"
#include "BF_Des.h"

//һ��xdp��¼   ��m_packdata�У��䳤���ֳ���Ϊ2��תΪ�����ֽ�������������ֵδת������ΪͨѶʱ��������
//ȡֵʱתΪ��������ֵʱתΪͨѶ������

//index��1��ʼ

#ifdef XDP_EXPORTS
#define CXDP_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define CXDP_EXPORT __declspec(dllimport)
#else
#define CXDP_EXPORT 
#endif
#endif

class CXDP_EXPORT CXdpRecord  
{
public:

	// ������: SetFieldMap
	// ���  : ������ 2017-6-30 13:45:54
	// ����  : void 
	// ����  : int index int index ��0��ʼ�����127  λͼֻ��16�ֽ�
	// ����  : bool is=true
	// ����  : ���ö�Ӧ��λͼ
	void SetFieldMap(int index,bool is=true);

	// ������: IsFieldSet
	// ���  : ������ 2017-6-30 13:45:07
	// ����  : bool true�� false��
	// ����  : int index ��0��ʼ�����127  λͼֻ��16�ֽ�
	// ����  : �ж��ֶ�λͼ�Ƿ���Ϊ1,���Ƿ������ô��ֶ�
	bool IsFieldSet(int index);

	// ������: XdpRecordCopy
	// ���  : ������ 2014-11-11 10:08:20
	// ����  : bool 
	// ����  : CXdpRecord *src
	// ����  : ����һ����¼���ڵ���ǰ����init
	bool XdpRecordCopy(CXdpRecord *src);

	// ������: GetBitMap
	// ���  : ������ 2014-11-11 10:06:47
	// ����  : char * 
	// ����  : ȡ��λͼ
	char * GetBitMap();

	// ������: GetPackDataBufferLen
	// ���  : ������ 2014-11-11 10:05:59
	// ����  : unsigned int 
	// ����  : ȡ�ô洢���ĵ����ռ䳤��
	unsigned int GetPackDataBufferLen();

	// ������: GetPackDataBuffer
	// ���  : ������ 2014-11-11 10:04:17
	// ����  : char * 
	// ����  : ȡ�ô洢���ĵ����ռ�ָ��
	char * GetPackDataBuffer();

	// ������: CancelField
	// ���  : ������ 2014-10-30 11:17:16
	// ����  : bool 
	// ����  : const char *fieldname
	// ����  : ȡ���ֶΣ������Ͳ������ɴ��ֶα��Ļ�ȡ��ֵ��
	bool CancelField(const char *fieldname);
	
	// ������: CancelField
	// ���  : ������ 2014-10-30 11:16:28
	// ����  : bool 
	// ����  : unsigned short index
	// ����  : ȡ���ֶΣ������Ͳ������ɴ��ֶα��Ļ�ȡ��ֵ��
	bool CancelField(unsigned short index);

	// ������: PrintXdpRec
	// ���  : ������ 2014-10-29 9:17:26
	// ����  : bool true�ɹ� falseʧ��
	// ����  : char *printbuf   ������  ע���СҪ���󣬷����Խ��
	// ����  : ��xdp�����������д���������У���д����־�鿴��
	bool PrintXdpRec(char *printbuf);

	// ������: GetFieldData
	// ���  : ������ 2014-10-28 16:15:27
	// ����  : bool true�ɹ�falseʧ��
	// ����  : unsigned short index  �ֶ�����
	// ����  : char *data   ֵ  ���ֶ�Ϊ������ʱ���������Զ������ֽ���ת��
	// �����������  : int &datalen   ����Ϊdata�����С�����Ϊ�ֶ�ֵ����
	// ����  : int &fieldtype         �ֶ�����
	// ����  : char *errmsg           ������Ϣ
	// ����  : ȡ���ֶ�ֵ
	bool GetFieldData(unsigned short index,char *data, unsigned int &datalen,int &fieldtype,char *errmsg);
	bool GetFieldData(unsigned short index,int &idata,char *errmsg);
	bool GetFieldData(unsigned short index,unsigned int &idata,char *errmsg);
	bool GetFieldData(unsigned short index,short &idata,char *errmsg);
	bool GetFieldData(unsigned short index,unsigned short &idata,char *errmsg);
	bool GetFieldData(unsigned short index,double &idata,char *errmsg);

	bool GetFieldData(const char *fieldname,char *data,unsigned int &datalen,int &fieldtype,char *errmsg);
	bool GetFieldData(const char *fieldname,int &idata,char *errmsg);
	bool GetFieldData(const char *fieldname,unsigned int &idata,char *errmsg);
	bool GetFieldData(const char *fieldname,short &idata,char *errmsg);
	bool GetFieldData(const char *fieldname,unsigned short &idata,char *errmsg);
	bool GetFieldData(const char *fieldname,double &idata,char *errmsg);

	// ������: SetFieldData
	// ���  : ������ 2014-10-28 16:13:26
	// ����  : bool true�ɹ�falseʧ��
	// ����  : unsigned short index  �ֶ�����
	// ����  : const char *data    ֵ     ���ֶ�Ϊ������ʱ���������Զ������ֽ���ת��
	// ����  : int datalen         ����
	// ����  : char *errmsg        ������Ϣ
	// ����  : �����ֶ�ֵ
	bool SetFieldData(unsigned short index,const char *data,unsigned int datalen,char *errmsg);
	bool SetFieldData(unsigned short index,int idata,char *errmsg);
	bool SetFieldData(unsigned short index,unsigned int idata,char *errmsg);
	bool SetFieldData(unsigned short index,short idata,char *errmsg);
	bool SetFieldData(unsigned short index,unsigned short idata,char *errmsg);
	bool SetFieldData(unsigned short index,double idata,char *errmsg);

	bool SetFieldData(const char *fieldname,const char *data,unsigned int datalen,char *errmsg);
	bool SetFieldData(const char *fieldname,int idata,char *errmsg);
	bool SetFieldData(const char *fieldname,unsigned int idata,char *errmsg);
	bool SetFieldData(const char *fieldname,short idata,char *errmsg);
	bool SetFieldData(const char *fieldname,unsigned short idata,char *errmsg);
	bool SetFieldData(const char *fieldname,double idata,char *errmsg);

	// ������: ToBuffer
	// ���  : ������ 2014-10-28 15:16:25
	// ����  : bool true�ɹ�falseʧ��
	// ����  : unsigned char *xdpbuf  
	// �����������  : unsigned int &len    ����Ϊ�����С�����Ϊ���ĳ���
	// ����  : char *errmsg  ������Ϣ
	// ����  : ����ͨѶʱ�ı���
	bool ToBuffer(char *xdpbuf,unsigned int &len,char *errmsg);

	// ������: GetRealLen
	// ���  : ������ 2014-10-28 15:16:07
	// ����  : unsigned short 
	// ����  : ȡ��ʵ��������ռ�ռ��С��������λͼ
	unsigned short GetRealLen();

	// ������: ResetData
	// ���  : ������ 2014-10-27 14:23:34
	// ����  : void 
	// ����  : ���λͼ������xdpʵ��ռ�ÿռ�Ϊ0
	void ResetData();

	// ������: FromBuffer
	// ���  : ������ 2014-10-24 16:46:29
	// ����  : bool 
	// ����  : const char *xdpdata
	// ����  : int datalen
	// ����  : char *errmsg
	// ����  : ����һ�����ݣ��������ݷ���m_pPackData�й���ȡ���޸ģ���λͼ������ʵ��ռ�ÿռ�
	bool FromBuffer(const char *xdpdata,unsigned int datalen,char *errmsg);

	// ������: Init
	// ���  : ������ 2014-10-24 16:46:17
	// ����  : bool 
	// ����  : CXdpFmt *pfmt
	// ����  : ��ʼ���ಢ����ռ�
	bool Init(CXdpFmt *pfmt);
	CXdpRecord();
	virtual ~CXdpRecord();

protected:

	// ������: SetFieldData
	// ���  : ������ 2014-10-28 16:13:26
	// ����  : bool true�ɹ�falseʧ��
	// ����  : S_XDP_FIELD_DEFINE  *field  �ֶ�
	// ����  : const char *data    ֵ     ���ֶ�Ϊ������ʱ���������Զ������ֽ���ת��
	// ����  : int datalen         ����
	// ����  : char *errmsg        ������Ϣ
	// ����  : �����ֶ�ֵ
	bool SetFieldData(S_XDP_FIELD_DEFINE *field,const char *data,unsigned int datalen,char *errmsg);

	bool GetFieldData(S_XDP_FIELD_DEFINE *field,char *data,unsigned int &datalen,char *errmsg);

	CXdpFmt   *m_pXdpFmt;   //���Ķ���
	char      m_sBitMap[XDP_BITMAPSIZE+1];//λͼ
	char      *    m_pPackData;  //�洢���ĵ����ռ䣬������λͼ
	unsigned int   m_nPackDataMaxLen;//�洢���ĵĿռ䳤��
	unsigned short  m_nRealLen;//������ʵ��ռ�ÿռ�
	CBF_Des         m_pDes;//ֻ�õ����ɼ�ת�ɼ�

private:
	unsigned short  i,j;//��ʱѭ������

	S_XDP_FIELD_DEFINE *m_tField;//��ʱ������ֶ�˵��
	unsigned short m_tvclen;//��ʱ����
	unsigned short *m_tus; ////��ʱ����
};

#endif // !defined(AFX_XDPRECORD_H__86FFC8E4_2683_43CF_AE23_D6442D392E9B__INCLUDED_)
