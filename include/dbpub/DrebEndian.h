// DrebEndian.h: interface for the CDrebEndian class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DREBENDIAN_H__86E9E605_0216_4F05_80FB_D0BBA4A5F9AE__INCLUDED_)
#define AFX_DREBENDIAN_H__86E9E605_0216_4F05_80FB_D0BBA4A5F9AE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "public.h"
#include "DrebPubDefine.h"

#ifdef DBPUB_EXPORTS
#define DBPUB_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define DBPUB_EXPORT __declspec(dllimport)
#else
#define DBPUB_EXPORT 
#endif
#endif

class DBPUB_EXPORT CDrebEndian  
{

public:
	// ������: CRC
	// ���  : ������ 2013-5-6 14:20:10
	// ����  : unsigned int 
	// ����  : void *pdata
	// ����  : int datalen
	// ����  : ����CRCУ��λ
	unsigned int CRC(void *pdata,int datalen);

	// ������: SetCommEndian
	// ���  : ������ 2013-4-25 11:13:08
	// ����  : void 
	// ����  : bool islittle true��windows linux��ʹ�õ�������  falseΪ������
	// ����  : ����ͨѶʱ���ֽ��� 
	void SetCommEndian(bool islittle);
	
	// ������: Endian2Comm
	// ���  : ������ 2013-4-25 10:41:58
	// ����  : void 
	// ����  : PDREB_HEAD data
	// ����  : ͨѶʱ�����ֽ���תΪͨѶʱ���õ��ֽ���
	void Endian2Comm(PDREB_HEAD data);
	

	// ������: Endian2Comm
	// ���  : ������ 2013-5-6 14:42:15
	// ����  : void 
	// ����  : unsigned char *data
	// ����  : int datalen
	// ����  : ͨѶʱ�����ֽ���תΪͨѶʱ���õ��ֽ���
	void Endian2Comm(unsigned char *data,int datalen);


	// ������: Endian2CommGenCrc
	// ���  : ������ 2013-5-6 14:31:04
	// ����  : void 
	// ����  : PDREB_HEAD data
	// ����  : ͨѶʱ�����ֽ���תΪͨѶʱ���õ��ֽ��򣬲�����CRCУ��λ
	void Endian2CommGenCrc(PDREB_HEAD data);
	
	// ������: Endian2LocalHost
	// ���  : ������ 2013-4-25 10:42:03
	// ����  : void 
	// ����  : PDREB_HEAD data
	// ����  : ��ͨѶ�������ֽ���תΪ������ʹ�õ��ֽ���
	void Endian2LocalHost(PDREB_HEAD data);


	// ������: Endian2LocalHostChkCrc
	// ���  : ������ 2013-5-6 14:21:46
	// ����  : bool 
	// ����  : PDREB_HEAD data
	// ����  : ��ͨѶ�������ֽ���תΪ������ʹ�õ��ֽ��򣬲����CRC�ֶ�
	bool Endian2LocalHostChkCrc(PDREB_HEAD data);
	

	// ������: Endian2LocalHost
	// ���  : ������ 2013-5-6 14:42:02
	// ����  : void 
	// ����  : unsigned char *data
	// ����  : int datalen
	// ����  : ��ͨѶ�������ֽ���תΪ������ʹ�õ��ֽ���
	void Endian2LocalHost(unsigned char *data,int datalen);
	

	// ������: Reverse
	// ���  : ������ 2013-5-6 14:41:45
	// ����  : void 
	// ����  : unsigned char *buf
	// ����  : int len
	// ����  : ���з�ת
	void Reverse(unsigned char *buf, int len);

	CDrebEndian();
	virtual ~CDrebEndian();
	
protected:
	
	bool m_bIsLittle;//�Ƿ������� falseΪ������ 
	
	bool m_bUseLittleEndian;//ͨѶ�Ƿ�ʹ��linux,windows�����ֽ��� false��Ϊ������
	
	

	
};

#endif // !defined(AFX_DREBENDIAN_H__86E9E605_0216_4F05_80FB_D0BBA4A5F9AE__INCLUDED_)
