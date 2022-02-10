// BF_Zip.h: interface for the CBF_Zip class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BF_ZIP_H__949008A6_9D40_493C_A3FE_CEF528ADBF23__INCLUDED_)
#define AFX_BF_ZIP_H__949008A6_9D40_493C_A3FE_CEF528ADBF23__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef KERNEL_EXPORTS
#define KERNEL_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define KERNEL_EXPORT __declspec(dllimport)
#else
#define KERNEL_EXPORT 
#endif
#endif

extern "C" 
{ 
	// ������: BF_Compress
	// ���  : ������ 2012-4-20 16:18:10
	// ����  : bool  true���ɹ���falseѹ��ʧ�ܡ�
	// ����  : unsigned char * destbuf ѹ��������ݣ�
	// ����  : unsigned int * destlen  ѹ��ǰΪdestbuf�Ĵ�С���������srbuflen��ѹ����Ϊѹ����ĳ���
	// ����  : unsigned char * srbuf   ѹ��ǰ������  
	// ����  : int srbuflen            srbuf�����ݵĳ���
	// ����  : ѹ������
	KERNEL_EXPORT bool  BF_Compress(unsigned char * destbuf,unsigned int * destlen,unsigned char * srbuf,int srbuflen);
	

	// ������: BF_Uncompress
	// ���  : ������ 2012-4-20 16:18:15
	// ����  : bool  true���ɹ���false��ѹ��ʧ�ܡ�
	// ����  : unsigned char* destbuf   ��ѹ���������
	// ����  : unsigned int * destlen   ��ѹ��ǰΪdestbuf�Ĵ�С���������srbuflen����ѹ����Ϊ��ѹ���ĳ���
	// ����  : unsigned char* srbuf     ��ѹ��ǰ������
	// ����  : int srbuflen             srbuf�����ݵĳ���
	// ����  : ��ѹ������
	KERNEL_EXPORT bool  BF_Uncompress(unsigned char* destbuf, unsigned int * destlen, unsigned char* srbuf, int srbuflen);
}

#endif // !defined(AFX_BF_ZIP_H__949008A6_9D40_493C_A3FE_CEF528ADBF23__INCLUDED_)
