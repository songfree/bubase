// GateApi.h: interface for the CGateApi class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GATEAPI_H__38F43A80_B734_4386_A0E1_1A8E395F79EC__INCLUDED_)
#define AFX_GATEAPI_H__38F43A80_B734_4386_A0E1_1A8E395F79EC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef XDP_EXPORTS
#define CXDP_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define CXDP_EXPORT __declspec(dllimport)
#else
#define CXDP_EXPORT 
#endif
#endif


extern "C"
{	
	// ������: GetRealPackSize
	// ���  : ������ 2014-10-30 11:31:20
	// ����  : unsigned short 
	// ����  : void *CXdpClass   xdp���ʵ��ָ�룬��InitXdp���
	// ����  : ȡ�õ�ǰʵ�ʱ��ĵĴ�С
	CXDP_EXPORT unsigned short XDP_GetRealPackSize(void *CXdpClass);

	// ������: CancelField
	// ���  : ������ 2014-10-30 11:17:16
	// ����  : int 0�ɹ� ��0ʧ��
	// ����  : void *CXdpClass   xdp���ʵ��ָ�룬��InitXdp���
	// ����  : unsigned short rec  ��1��ʼ
	// ����  : const char *fieldname
	// ����  : ȡ���ֶΣ������Ͳ������ɴ��ֶα��Ļ�ȡ��ֵ��
	CXDP_EXPORT int XDP_CancelFieldByName(void *CXdpClass,unsigned short rec,const char *fieldname);

	// ������: CancelField
	// ���  : ������ 2014-10-30 11:16:28
	// ����  : int 0�ɹ� ��0ʧ��
	// ����  : void *CXdpClass   xdp���ʵ��ָ�룬��InitXdp���
	// ����  : unsigned short rec   ��1��ʼ
	// ����  : unsigned short index
	// ����  : ȡ���ֶΣ������Ͳ������ɴ��ֶα��Ļ�ȡ��ֵ��
	CXDP_EXPORT int XDP_CancelFieldByIndex(void *CXdpClass,unsigned short rec,unsigned short index);

	// ������: PrintXdp
	// ���  : ������ 2014-10-30 11:15:33
	// ����  : void 
	// ����  : void *CXdpClass   xdp���ʵ��ָ�룬��InitXdp���
	// ����  : char *xdpbuf
	// ����  : ������������ʾ����������������תΪ16������ʾ
	CXDP_EXPORT void XDP_PrintXdp(void *CXdpClass,char *xdpbuf);

	// ������: GetCurSavePackNum
	// ���  : ������ 2014-10-29 11:45:49
	// ����  : unsigned short 
	// ����  : void *CXdpClass   xdp���ʵ��ָ�룬��InitXdp���
	// ����  : ȡ�ñ�����¼�Ľṹ��Ŀ��ע�ⲻ������һ����¼
	CXDP_EXPORT unsigned short XDP_GetCurSavePackNum(void *CXdpClass);

	// ������: ClearNextRecord
	// ���  : ������ 2014-10-29 11:34:21
	// ����  : void 
	// ����  : void *CXdpClass   xdp���ʵ��ָ�룬��InitXdp���
	// ����  : ��ձ�����¼�Ľṹ
	CXDP_EXPORT void XDP_ClearNextRecord(void *CXdpClass);
	

	// ������: GetFieldType
	// ���  : ������ 2014-10-29 11:22:23
	// ����  : unsigned short ���ͣ���XdpFmt.h�궨��
	// ����  : void *CXdpClass   xdp���ʵ��ָ�룬��InitXdp���
	// ����  : const char *fieldname/unsigned short index �ֶ�����/����
	// ����  : ȡ���ֶ�����
	CXDP_EXPORT unsigned short XDP_GetFieldTypeByName(void *CXdpClass,const char *fieldname);
	CXDP_EXPORT unsigned short XDP_GetFieldTypeByIndex(void *CXdpClass,unsigned short index);

	// ������: GetRecCount
	// ���  : ������ 2014-10-29 11:10:29
	// ����  : unsigned short ��¼��
	// ����  : void *CXdpClass   xdp���ʵ��ָ�룬��InitXdp���
	// ����  : ���ص�ǰ�����й��м�����¼��ע���¼���ͱ�����¼�Ľṹ��Ŀ����һ������
	CXDP_EXPORT unsigned short XDP_GetRecCount(void *CXdpClass);

	// ������: GetFieldValue
	// ���  : ������ 2014-10-29 11:04:09
	// ����  : int 0�ɹ� ��0ʧ��
	// ����  : void *CXdpClass   xdp���ʵ��ָ�룬��InitXdp���
	// ����  : unsigned short rec    �ڼ�����¼
	// ����  : unsigned short index/const char *fieldname  �ֶ�����/����
	// ����  : char *data      ���ݻ���  ��Ϊ��ֵ��ʱ��Ҫ��ǿ��ת��  �磺int *aa = (int *)data;
	// �����������  : unsigned int &datalen   ����Ϊdata�����С ���Ϊʵ�����ݳ���
	// ����  : int &fieldtype  �ֶ����ͣ���XdpFmt.h�к궨��
	// ����  : char *errmsg  ������Ϣ
	// ����  : ��xdp��ȡ�ֶ�ֵ
	CXDP_EXPORT int XDP_GetFieldValueByName(void *CXdpClass,unsigned short rec,const char *fieldname,char *data,unsigned int *datalen,int *fieldtype,char *errmsg);
	CXDP_EXPORT int XDP_GetFieldValueByIndex(void *CXdpClass,unsigned short rec,unsigned short index,char *data,unsigned int *datalen,int *fieldtype,char *errmsg);

	// ������: GetFieldValue
	// ���  : ������ 2014-10-29 11:11:55
	// ����  : int 0�ɹ� ��0ʧ��
	// ����  : void *CXdpClass   xdp���ʵ��ָ�룬��InitXdp���
	// ����  : unsigned short index/const char *fieldname �ֶ�����/����
	// ����  : char *data       ���ݻ��� ��Ϊ��ֵ��ʱ��Ҫ��ǿ��ת����ȡֵ  �磺int *aa = (int *)data;
	// ����  : unsigned int &datalen     ����Ϊdata�����С ���Ϊʵ�����ݳ���
	// ����  : int &fieldtype  �ֶ����ͣ���XdpFmt.h�к궨��
	// ����  : char *errmsg   ������Ϣ
	// ����  : ��xdp��ȡ��һ����¼���ֶ�ֵ
	CXDP_EXPORT int XDP_GetFirstFieldValueByIndex(void *CXdpClass,unsigned short index,char *data,unsigned int *datalen,int *fieldtype,char *errmsg);
	CXDP_EXPORT int XDP_GetFirstFieldValueByName(void *CXdpClass,const char *fieldname,char *data,unsigned int *datalen,int *fieldtype,char *errmsg);



	// ������: SetFieldValue
	// ���  : ������ 2014-10-29 11:40:30
	// ����  : int 0�ɹ� ��0ʧ��
	// ����  : void *CXdpClass   xdp���ʵ��ָ�룬��InitXdp���
	// ����  : unsigned short rec    Ҫ����Ϊ��¼����ע����rec����ԭ��¼����ô��¼��Ҫ��С����˳������
	// ����  : unsigned short index/const char *fieldname �ֶ�����/����
	// ����  : const char *data   ���ݻ��� ��Ϊ��ֵ��ʱ��Ҫ��ǿ��ת������ֵ  �磺int *aa = (int *)data; *aa = 1200
	// ����  : int datalen     ���ݳ���
	// ����  : char *errmsg    ������Ϣ
	// ����  : ����ָ����¼�����ֶ�ֵ
	CXDP_EXPORT int XDP_SetFieldValueByName(void *CXdpClass,unsigned short rec,const char *fieldname,const char *data,unsigned int datalen,char *errmsg);
	CXDP_EXPORT int XDP_SetFieldValueByIndex(void *CXdpClass,unsigned short rec,unsigned short index,const char *data,unsigned int datalen,char *errmsg);



	// ������: SetFieldValue
	// ���  : ������ 2014-10-29 11:40:35
	// ����  : int 0�ɹ� ��0ʧ��
	// ����  : void *CXdpClass   xdp���ʵ��ָ�룬��InitXdp���
	// ����  : unsigned short index/const char *fieldname �ֶ�����/����  
	// ����  : const char *data   ���ݻ��� ��Ϊ��ֵ��ʱ��Ҫ��ǿ��ת������ֵ  �磺int *aa = (int *)data; *aa = 1200
	// ����  : int datalen    ���ݳ���
	// ����  : char *errmsg  ������Ϣ
	// ����  : ���õ�һ����¼���ֶ�ֵ
	CXDP_EXPORT int XDP_SetFirstFieldValueByIndex(void *CXdpClass,unsigned short index,const char *data,unsigned int datalen,char *errmsg);
	CXDP_EXPORT int XDP_SetFirstFieldValueByName(void *CXdpClass,const char *fieldname,const char *data,unsigned int datalen,char *errmsg);


	// ������: SetCurRecCount
	// ���  : ������ 2014-10-29 10:55:12
	// ����  : int 0�ɹ� ��0ʧ�ܣ���ԭ���ĵļ�¼Y<Xʱʧ��
	// ����  : void *CXdpClass   xdp���ʵ��ָ�룬��InitXdp���
	// ����  : unsigned short rec 
	// ����  : ���õ�ǰ�ļ�¼������ԭ��������Y����¼ʱ������ֻ����X(X<Y)����¼ʱ���������ü�¼��ΪX,���򷵻ص���ΪY��
	CXDP_EXPORT int XDP_SetCurRecCount(void *CXdpClass,unsigned short x);

	// ������: ResetData
	// ���  : ������ 2014-10-27 14:24:43
	// ����  : void 
	// ����  : void *CXdpClass   xdp���ʵ��ָ�룬��InitXdp���
	// ����  : ������¼������λͼ
	CXDP_EXPORT void XDP_ResetData(void *CXdpClass);

	// ������: ToBuffer
	// ���  : ������ 2014-10-27 14:18:52
	// ����  : int 0�ɹ� ��0ʧ��
	// ����  : void *CXdpClass   xdp���ʵ��ָ�룬��InitXdp���
	// ����  : char *xdpbuf   xdp����
	// �����������  : int *len    ����Ϊxdpbuf��С,���Ϊxdp���Ĵ�С
	// ����  : char *errmsg   ������Ϣ
	// ����  : ����xdp����
	CXDP_EXPORT int XDP_ToBuffer(void *CXdpClass,char *xdpbuf,unsigned int *len,char *errmsg);

	// ������: FromBuffer
	// ���  : ������ 2014-10-27 14:17:34
	// ����  : int 0�ɹ� ��0ʧ��
	// ����  : void *CXdpClass   xdp���ʵ��ָ�룬��InitXdp���
	// ����  : const char *xdpbuf   xdp���ݻ���
	// ����  : int xdpbuflen   xdp���ݳ���
	// ����  : char *errmsg   ������Ϣ
	// ����  : �����ݻ����������
	CXDP_EXPORT int XDP_FromBuffer(void *CXdpClass,const char *xdpbuf,unsigned int xdpbuflen,char *errmsg);

	// ������: InitXdp
	// ���  : ������ 2014-10-27 14:14:55
	// ����  : int 0�ɹ� ��0ʧ��
	// �����������  : void **CXdpClass   ����Ϊָ���ַ�����Ϊxdp���ʵ��ָ��
	// ����  : const char *datafile   xml�����ļ�
	// ����  : char *errmsg      ������Ϣ
	// ����  : ��ʼ�����ģ�
	CXDP_EXPORT int XDP_InitXdp(void **CXdpClass,const char *datafile,char *errmsg);


	// ������: FreeXdp
	// ���  : ������ 2014-11-7 11:19:43
	// ����  : void
	// ����  : void *CXdpClass
	// ����  : �ͷ�xdp���ʵ��
	CXDP_EXPORT void XDP_FreeXdp(void *CXdpClass);


	// ������:  XDP_Copy
	// ���  : ������ 2014-11-11 10:22:21
	// ����  : CXDP_EXPORT int 0�ɹ� ��0ʧ��
	// ����  : void *CXdpClassDst  Ŀ��
	// ����  : void *CXdpClassSrc   Դ
	// ����  : xdp����
	CXDP_EXPORT int  XDP_Copy(void *CXdpClassDst,void *CXdpClassSrc);
}

#endif // !defined(AFX_GATEAPI_H__38F43A80_B734_4386_A0E1_1A8E395F79EC__INCLUDED_)
