// BF_3Des.h: interface for the CBF_3Des class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BF_3DES_H__1CC4AC38_7C5E_4D11_9515_6C76DBFE3769__INCLUDED_)
#define AFX_BF_3DES_H__1CC4AC38_7C5E_4D11_9515_6C76DBFE3769__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BF_Des.h"


#ifdef KERNEL_EXPORTS
#define KERNEL_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define KERNEL_EXPORT __declspec(dllimport)
#else
#define KERNEL_EXPORT 
#endif
#endif

//�Ǳ�׼��3DES�㷨 ������KEY

class KERNEL_EXPORT CBF_3Des  
{
public:

	// ������: UnDes3Buffer
	// ���  : ������ 2017-4-27 9:37:38
	// ����  : bool 
	// �����������  : char *buffer   Ҫ���ܵ��ַ��� 16����    ���ܺ���ַ��� 
	// ����  : int buflen     Ҫ���ܵ��ַ�������   16�ı���
	// ����  : const char *key   24���ַ�
	// ����  : char *errmsg  ������Ϣ
	// ����  : 3DES����
	bool UnDes3Buffer(char *buffer,int buflen,const char *key,char *errmsg);

	// ������: Des3Buffer
	// ���  : ������ 2017-4-27 9:37:43
	// ����  : bool 
	// �����������  : char *buffer  Ҫ���ܵ��ַ���   3des����16����
	// ����  : int bufsize     ��������С
	// ����  : const char *key  24���ַ�
	// ����  : char *errmsg  ������Ϣ
	// ����  : 3DES����
	bool Des3Buffer(char *buffer,int bufsize,const char *key,char *errmsg);

	CBF_3Des();
	virtual ~CBF_3Des();


	CBF_DesEnctypt  m_pDes;

};

#endif // !defined(AFX_BF_3DES_H__1CC4AC38_7C5E_4D11_9515_6C76DBFE3769__INCLUDED_)
