// XdpFmt.h: interface for the CXdpFmt class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XDPFMT_H__9EEAE1E1_1E78_48AE_8715_9BB6553A1BD2__INCLUDED_)
#define AFX_XDPFMT_H__9EEAE1E1_1E78_48AE_8715_9BB6553A1BD2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BF_Xml.h"
#include "MdbBase.h"

#ifdef _WINDOWS
#ifdef _DEBUG
#pragma comment(lib, "bf_kerneld.lib") 
#pragma message("Automatically linking with   bf_kerneld.lib ")
#else
#pragma comment(lib, "bf_kernel.lib") 
#pragma message("Automatically linking with   bf_kernel.lib ")
#endif
#endif

//��ȡxml���ã����ɵ���¼����ֽڳ���


#define  XDP_UNKNOW    0    //δ֪
#define  XDP_USHORT    1    //�޷��Ŷ�����
#define  XDP_SHORT     2    //������
#define  XDP_UINT      3    //�޷�������
#define  XDP_INT       4    //����
#define  XDP_DOUBLE    5    //������
#define  XDP_CHAR      6    //�̶������ַ��� ���255
#define  XDP_VARCHAR   7    //�䳤�ַ��� ���255   ��ǰ����һ���ֽڳ���  1+data
#define  XDP_BINDATA   8    //���������� ���4096  ��ǰ���������ֽڳ���  2+data 


#define      XDP_BITMAPSIZE     16      //λͼ��С  16�ֽ�
#define      XDP_FIELD_NAMELEN  20       //�ֶ����Ƴ���
#define      XDP_FIELD_CHARLEN  255      //�ַ�����󳤶�
#define      XDP_FIELD_BINLEN   4096     //������������󳤶�



typedef struct 
{
	unsigned short  f_index;      //�ֶ�����  ��1��ʼ
	std::string     f_name ;      //�ֶ�����
	char            f_type;       //�ֶ�����
	unsigned  short f_length;     //�ֶγ���
	unsigned  short f_extlength;  //��չ���ȣ����XDP_VARCHAR��XDP_BINDATA�ֱ�Ϊ1��2
	unsigned  int   f_offset;     //���ֶ���ȫֵ��ƫ��λ��  ����ֵ�ͽ���ʱ���ݴ�λ����ֵ��ȡֵ
} S_XDP_FIELD_DEFINE;

#ifdef XDP_EXPORTS
#define CXDP_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define CXDP_EXPORT __declspec(dllimport)
#else
#define CXDP_EXPORT 
#endif
#endif

class CXDP_EXPORT CXdpFmt  
{
public:

	// ������: XdpFmtCopy
	// ���  : ������ 2014-11-11 9:27:41
	// ����  : bool true�ɹ� falseʧ��
	// ����  : CXdpFmt *src  Դfmt
	// ����  : ��һ������fmt����xdp����
	bool XdpFmtCopy(CXdpFmt *src);

	// ������: ResetFmt
	// ���  : ������ 2014-11-11 9:17:51
	// ����  : void 
	// ����  : ����XDP��ʽ��֮�����init��XdpFmtCopy
	void ResetFmt();

	// ������: char GetVersion
	// ���  : ������ 2014-11-11 9:06:49
	// ����  : unsigned  
	// ����  : ȡ��XDP�汾
	unsigned  char GetVersion();

	// ������: GetFieldNum
	// ���  : ������ 2014-10-27 16:35:45
	// ����  : int 
	// ����  : ȡ���ֶ���
	int GetFieldNum();

	// ������: GetField
	// ���  : ������ 2014-10-27 16:12:17
	// ����  : bool true�ҵ�    false�޴��ֶ�
	// ����  : const char *fieldname   �ֶ�����
	// ����  : S_XDP_FIELD_DEFINE &field
	// ����  : ȡ�ö����ֶ�
//	bool GetField(const char *fieldname,S_XDP_FIELD_DEFINE &field);

	// ������: GetField
	// ���  : ������ 2014-10-27 16:12:21
	// ����  : bool true�ҵ�    false�޴��ֶ�
	// ����  : unsigned short index  �ֶ�����
	// ����  : S_XDP_FIELD_DEFINE &field
	// ����  : ȡ�ö����ֶ�
//	bool GetField(unsigned short index,S_XDP_FIELD_DEFINE &field);

	// ������: GetField
	// ���  : ������ 2014-10-27 16:12:21
	// ����  : NULL�޴��ֶ� 
	// ����  : unsigned short index  �ֶ�����
	// ����  : ȡ�ö����ֶ�
	S_XDP_FIELD_DEFINE *GetField(const char *fieldname);
	S_XDP_FIELD_DEFINE *GetField(unsigned short index);

	S_XDP_FIELD_DEFINE *At(int index);

	// ������: GetMaxLength
	// ���  : ������ 2014-10-27 16:07:05
	// ����  : unsigned int 
	// ����  : ȡ�ñ��ĵ���󳤶ȣ��Դ˷���ռ�
	unsigned int GetMaxLength();

	// ������: Init
	// ���  : ������ 2014-10-23 14:24:13
	// ����  : bool true�ɹ� falseʧ��
	// ����  : const char *xmlfilepathname
	// ����  : ��ʼ�����Ķ���
	bool Init(const char *xmlfilepathname,char *errmsg);

	CXdpFmt();
	virtual ~CXdpFmt();

protected:

	// ������: AddFmt
	// ���  : ������ 2014-10-24 12:57:12
	// ����  : bool 
	// ����  : S_XDP_FIELD_DEFINE field
	// ����  : ��������������ݱ�
	bool AddFmt(S_XDP_FIELD_DEFINE field);

	// ������: GetType
	// ���  : ������ 2014-10-24 12:56:39
	// ����  : unsigned short ����
	// ����  : const char *type
	// ����  : �����ֶ��ַ���ȡ���ֶ�����
	unsigned short GetType(const char *type);

	
	unsigned short  m_nMaxLength;      //�������������������ĳ���
	unsigned short  m_nFieldNum;       //�ֶ���

	CMemTable <S_XDP_FIELD_DEFINE> m_table;        //�ֶ����ñ�
	CPkeyInt<1>             m_index;              //�ֶ�����
 	CPkeyCharF<20,1>        m_indexname;          //�ֶ���������

	unsigned  char          m_cVersion;          //XDP�汾

};

#endif // !defined(AFX_XDPFMT_H__9EEAE1E1_1E78_48AE_8715_9BB6553A1BD2__INCLUDED_)
