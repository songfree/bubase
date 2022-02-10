// BpcEndian.h: interface for the CBpcEndian class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BPCENDIANLITTLE_H__C11F5AA8_5EDB_48EC_9B30_07C176F7B137__INCLUDED_)
#define AFX_BPCENDIANLITTLE_H__C11F5AA8_5EDB_48EC_9B30_07C176F7B137__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BpcHead.h"

#ifdef DBPUB_EXPORTS
#define DBPUB_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define DBPUB_EXPORT __declspec(dllimport)
#else
#define DBPUB_EXPORT 
#endif
#endif


class DBPUB_EXPORT CBpcEndian  
{
public:

	// ������: SetCommEndian
	// ���  : ������ 2013-4-25 11:13:08
	// ����  : void 
	// ����  : bool islittle true��windows linux��ʹ�õ�������  falseΪ������
	// ����  : ����ͨѶʱ���ֽ��� 
	void SetCommEndian(bool islittle);

	// ������: Endian2Comm
	// ���  : ������ 2013-4-25 10:41:58
	// ����  : void 
	// ����  : PBPCCOMMSTRU data
	// ����  : ͨѶʱ�����ֽ���תΪͨѶʱ���õ��ֽ���
	void Endian2Comm(PBPCCOMMSTRU data);

	void Endian2Comm(unsigned char *data,int datalen);

	// ������: Endian2LocalHost
	// ���  : ������ 2013-4-25 10:42:03
	// ����  : void 
	// ����  : PBPCCOMMSTRU data
	// ����  : ��ͨѶ�������ֽ���תΪ������ʹ�õ��ֽ���
	void Endian2LocalHost(PBPCCOMMSTRU data);

	void Endian2LocalHost(unsigned char *data,int datalen);

	CBpcEndian();
	virtual ~CBpcEndian();
	
protected:

	bool m_bIsLittle;//�Ƿ������� falseΪ������ 

	bool m_bUseLittleEndian;//ͨѶ�Ƿ�ʹ��linux,windows�����ֽ��� false��Ϊ������

	void Reverse(unsigned char *buf, int len);

};

#endif // !defined(AFX_BPCENDIANLITTLE_H__C11F5AA8_5EDB_48EC_9B30_07C176F7B137__INCLUDED_)
