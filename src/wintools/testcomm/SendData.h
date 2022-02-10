// SendData.h: interface for the CSendData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SENDDATA_H__5A84790E_4380_4DE9_BF1D_12915CA0C3EF__INCLUDED_)
#define AFX_SENDDATA_H__5A84790E_4380_4DE9_BF1D_12915CA0C3EF__INCLUDED_

#include "XmlPack.h"
#include "BF_Mutex.h"
#include "BF_SimpleLog.h"
#include "BF_Tools.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define  MAXREQBUF 8*1024

typedef struct 
{
	int  threadNo;//�̺߳�
	int  raflag;//����Ӧ���־ 0���� 1Ӧ�� 2������Ϣ
	char cutime[9];
	char buffer[MAXREQBUF];
	int  bufferlen;
	int  usetime; //ʹ��ʱ��
}S_REQANS;

typedef struct 
{
	int length;
	int type; //0�ַ�1����2����
	int dec;
	int align;//0�����1�Ҷ���
}S_FIELD_ATTRIBUTE;

class CSendData  
{
public:
	void putResult(bool result);

	// ������: PushReqAns
	// ���  : ������ 2007-2-6 17:01:47
	// ����  : bool 
	// ����  : S_REQANS s_reqans
	// ����  : �������Ӧ������Ϣ����
	bool PushReqAns(S_REQANS s_reqans);

	

	// ������: SetPackType
	// ���  : ������ 2007-2-5 15:56:14
	// ����  : void 
	// ����  : int type
	// ����  : char *seg
	// ����  : ���ð����ͣ��ָ���
	void SetPackType(int type,char *seg);


	// ������: SetParameter
	// ���  : ������ 2007-2-5 14:58:38
	// ����  : void 
	// ����  : int headlen ��ͷ����
	// ����  : bool isinclude �Ƿ������ͷ
	// ����  : bool fill �Ƿ񲹿ո�
	// ����  : int type ������ 0�ֳ�1�����ָ�2�ָ�
	// ����  : char *seg �ָ���
	// ����  : ���ò���
	void SetParameter(int headlen, bool isinclude,bool fill,int type, char *seg);

	// ������: InitSendData
	// ���  : ������ 2007-2-5 13:23:56
	// ����  : bool 
	// ����  : int headlen
	// ����  : bool isinclude
	// ����  : bool fill
	// ����  : char *headfile
	// ����  : char *packfile
	// ����  : ��ʼ��ͷ��������
	bool InitSendData(int headlen,bool isinclude,bool fill,char *headfile,char *packfile);

	// ������: GetSendBuf
	// ���  : ������ 2007-2-5 12:36:40
	// ����  : bool 
	// ����  : char *buf  ��ŵĻ���
	// ����  : int &buflen  ���峤�ȼ����غ�ĳ���
	// ����  : ȡҪ���͵�����
	bool GetSendBuf(char *buf,unsigned int &buflen);

	bool GetSendBuf(char *buf,unsigned int &buflen,CXmlPack &xmlpack);
	CSendData();
	virtual ~CSendData();
	
	CXmlPack m_headxml;
	CXmlPack m_packxml;

	unsigned int m_headLen;

	int m_packType;//����ʽ 0����1�������ָ� 2�ָ�
	bool m_isIncludeHead;
	bool m_isFillBlank; //�Ƿ񲹿ո�
	char m_errMsg[256];
	char m_seg[10];//�ָ���
	
	vector <S_REQANS> m_reqans;
	int m_nSuccessNum;
	int m_nFaildNum;

	bool m_bIsExit;
	bool m_bIsBegin;
	
//	Cpubstrtime m_publog;
	CBF_SimpleLog m_log;
	CBF_Date_Time m_pDate;
	CBF_Tools     m_pTool;

	int m_nLogLevel;

private:
	CBF_Mutex m_sendDataMutex;
	CBF_Mutex m_putDataMutex;
	// ������: GetFieldBufferByType2
	// ���  : ������ 2007-2-6 17:01:31
	// ����  : bool 
	// ����  : char *value
	// ����  : S_FIELD_ATTRIBUTE s_field
	// ����  : char *buf
	// ����  : int &buflen
	// ����  : ���ָ���ȡ�ֶλ���
	bool GetFieldBufferByType2(char *value, S_FIELD_ATTRIBUTE s_field, char *buf, int &buflen);
	
	// ������: GetFieldBufferByType1
	// ���  : ������ 2007-2-6 17:01:10
	// ����  : bool 
	// ����  : char *value
	// ����  : S_FIELD_ATTRIBUTE s_field
	// ����  : char *buf
	// ����  : int &buflen
	// ����  : �������ָ�ȡ�ֶλ���
	bool GetFieldBufferByType1(char *value,S_FIELD_ATTRIBUTE s_field,char *buf,int &buflen);
	
	// ������: GetFieldBufferByType0
	// ���  : ������ 2007-2-6 17:00:47
	// ����  : bool 
	// ����  : char *value
	// ����  : S_FIELD_ATTRIBUTE s_field
	// ����  : char *buf
	// ����  : int &buflen
	// ����  : ��������ȡ�ֶλ���
	bool GetFieldBufferByType0(char *value,S_FIELD_ATTRIBUTE s_field,char *buf,int &buflen);
	
	// ������: GetValue
	// ���  : ������ 2007-2-6 17:00:36
	// ����  : bool 
	// ����  : CNode *node
	// ����  : char *value
	// ����  : ȡ�ڵ���ֶ�ֵ
	bool GetValue(CNode *node,char *value);
	
	// ������: GetFieldBuffer
	// ���  : ������ 2007-2-6 16:59:56
	// ����  : bool 
	// ����  : CNode *node
	// ����  : char *buf
	// ����  : int &buflen
	// ����  : ȡ�ֶλ���
	bool GetFieldBuffer(CNode *node,char *buf,int &buflen);
	
	// ������: GetAttributes
	// ���  : ������ 2007-2-6 16:59:44
	// ����  : bool 
	// ����  : CNode *node
	// ����  : S_FIELD_ATTRIBUTE *s_attr
	// ����  : �ӽڵ���ȡ����
	bool GetAttributes(CNode *node,S_FIELD_ATTRIBUTE *s_attr);
	
	// ������: GetPackBuffer
	// ���  : ������ 2007-2-6 16:59:13
	// ����  : bool 
	// ����  : char *buf
	// ����  : int &buflen
	// ����  : ȡ�����İ���
	bool GetPackBuffer(char *buf, int &buflen);
	
	// ������: GetHeadBuffer
	// ���  : ������ 2007-2-6 16:58:56
	// ����  : bool 
	// ����  : char *buf
	// ����  : int &buflen
	// ����  : ȡ�����İ�ͷ
	bool GetHeadBuffer(char *buf,int &buflen);
	

};

#endif // !defined(AFX_SENDDATA_H__5A84790E_4380_4DE9_BF1D_12915CA0C3EF__INCLUDED_)
