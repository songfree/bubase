// IVarDefine.h: interface for the CIVarDefine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IVARDEFINE_H__3C286316_243A_47B6_BF95_4E1DF17BC483__INCLUDED_)
#define AFX_IVARDEFINE_H__3C286316_243A_47B6_BF95_4E1DF17BC483__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "BF_Mutex.h"
using namespace std;

class CIVarDefine  
{
public:
	int GetVarLen();

	/// ������: GetFmt
	/// ���  : ������ 2009-7-17 17:11:33
	/// ����  : string & 
	/// ����  : ��ȡ���ͱ����ĸ�ʽ�ִ�
	string & GetFmt();

	/// ������: SetVarPara
	/// ���  : ������ 2009-7-17 17:10:28
	/// ����  : void 
	/// ����  : string name
	/// ����  : int type
	/// ����  : int len
	/// ����  : ���ñ�������
	void SetVarPara(string name,int type,int len);

	/// ������: IntVarInit
	/// ���  : ������ 2009-7-17 17:10:36
	/// ����  : void 
	/// ����  : string fmt
	/// ����  : __int64 initvalue
	/// ����  : ���ͱ�����ʼ��
	void IntVarInit(string fmt, __int64 initvalue);

	/// ������: PushVarValue
	/// ���  : ������ 2009-7-17 17:03:48
	/// ����  : bool 
	/// ����  : string svalue
	/// ����  : ����һ���б����
	bool PushVarValue(string svalue);

	/// ������: GetVarType
	/// ���  : ������ 2009-7-17 17:04:01
	/// ����  : int 
	/// ����  : ��ȡ��������
	int GetVarType();

	/// ������: GetVarName
	/// ���  : ������ 2009-7-17 17:04:14
	/// ����  : string & 
	/// ����  : ��ȡ��������
	string & GetVarName();


	/// ������:     GetVarValue
	/// ���  : ������ 2009-7-17 17:04:24
	/// ����  : __int64  
	/// ����  : ȡ���ͱ���ֵ
	__int64      GetVarValue();

	/// ������: GetVarValueS
	/// ���  : ������ 2009-7-17 17:04:33
	/// ����  : std::string  
	/// ����  : ȡ�б����ֵ
	std::string  GetVarValueS();



	CIVarDefine();
	virtual ~CIVarDefine();
	
	

protected:
	int m_varType;//�������� 0��������   1�б����ѡ�� 2�б�˳��ѡ��
	int m_varLen; //�������� 
	__int64 m_maxVarValue;//���ı�����С
	std::string m_fmt;//������ʽ���ַ���
	__int64 m_lValue ; //�������ӱ��� ����Ϊ0ʱʹ��   _atoi64(value+1);//sprintf(value,"%I64d",lvalue+1); 
	CBF_Mutex  m_mutex;
	std::string m_sVarName;//��������
	vector<string>m_varValueList;//�����б�  ����Ϊ1 2ʱʹ��
	int m_varValueNo;//˳��ѡ��ʱ���±�
	

};

#endif // !defined(AFX_IVARDEFINE_H__3C286316_243A_47B6_BF95_4E1DF17BC483__INCLUDED_)
