// BF_Md5.h: interface for the CBF_Md5 class.
//
//////////////////////////////////////////////////////////////////////

/***********************************************************************************
  ������־��¼
  ������: ������
  �������� 2012-4-17 14:18:22 �ع�
  �汾: V1.00 
  ˵��: MD5�㷨
 ***********************************************************************************/


#if !defined(AFX_BF_MD5_H__5D56F139_BA63_4CAA_B29F_04B893858170__INCLUDED_)
#define AFX_BF_MD5_H__5D56F139_BA63_4CAA_B29F_04B893858170__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "typedefine.h"



#ifdef KERNEL_EXPORTS
#define KERNEL_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define KERNEL_EXPORT __declspec(dllimport)
#else
#define KERNEL_EXPORT 
#endif
#endif

#define MD5_BUFFER_SIZE 1024   //����MD5����󳤶�

class KERNEL_EXPORT CBF_Md5  
{

public:
	CBF_Md5();
	CBF_Md5(const void *input, size_t length);
	CBF_Md5(const string &str);
	CBF_Md5(ifstream &in);
	virtual ~CBF_Md5();


	// ������: Update
	// ���  : ������ 2012-4-17 14:39:13
	// ����  : void 
	// ����  : const void *input
	// ����  : size_t length
	// ����  : ����Ҫ��MD5������
	void Update(const void *input, size_t length);

	// ������: Update
	// ���  : ������ 2012-4-17 14:39:31
	// ����  : void 
	// ����  : const string &str
	// ����  : ����Ҫ��MD5������
	void Update(const string &str);

	// ������: Update
	// ���  : ������ 2012-4-17 14:39:39
	// ����  : void 
	// ����  : ifstream &in
	// ����  : ����Ҫ��MD5������
	void Update(ifstream &in);

	// ������: digest
	// ���  : ������ 2012-4-17 14:36:53
	// ����  : const byte*  16λ����char ��\0����
	// ����  : �õ�Update������ݵ�MD5,16λ����
	const byte* Digest();

	// ������: ToString
	// ���  : ������ 2012-4-17 14:34:32
	// ����  : string 
	// ����  : �õ�Update������ݵ�MD5��ת��Ϊ����ʾ�ַ�
	std::string ToString();

	// ������: Reset
	// ���  : ������ 2012-4-17 14:34:17
	// ����  : void 
	// ����  : ����֮ǰ��Update����
	void Reset();
private:
	void Update(const byte *input, size_t length);
	void Final();
	void Transform(const byte block[64]);
	void Encode(const UINT32 *input, byte *output, size_t length);
	void Decode(const byte *input, UINT32 *output, size_t length);
	std::string BytesToHexString(const byte *input, size_t length);
	

private:
	UINT32 m_state[4];	/* state (ABCD) */
	UINT32 m_count[2];	/* number of bits, modulo 2^64 (low-order word first) */
	byte m_buffer[64];	/* input buffer */
	byte m_digest[17];	/* message digest */
	bool m_finished;		/* calculate finished ? */
	
	static const byte PADDING[64];	/* padding for calculate */
	static const char HEX[16];

};

#endif // !defined(AFX_BF_MD5_H__5D56F139_BA63_4CAA_B29F_04B893858170__INCLUDED_)
