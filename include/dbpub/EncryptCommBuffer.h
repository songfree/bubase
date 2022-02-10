// EncryptCommBuffer.h: interface for the CEncryptCommBuffer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENCRYPTCOMMBUFFER_H__59BA9B16_7577_4848_AF20_DA8E0F696617__INCLUDED_)
#define AFX_ENCRYPTCOMMBUFFER_H__59BA9B16_7577_4848_AF20_DA8E0F696617__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BF_Des.h"
#include "BF_Tools.h"
#include "IErrLog.h"
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

class DBPUB_EXPORT CEncryptCommBuffer  
{
public:
	

	// ������: ZipDesProcess
	// ���  : ������ 2013-7-20 12:34:25
	// ����  : bool true�ɹ�falseʧ��
	// ����  : PCOMMSTRU data  dreb����
	// ����  : int destflag   ����Ŀ��  0���� 1DES���� 2ѹ�� 3DES���� 4��ѹ����1����  3��1ֻ����Կ������
	// ����  : ѹ�����ܴ���    
	bool DrebZipDesProcess(PCOMMSTRU data,int destflag);

	// ������: SetLogPara
	// ���  : ������ 2013-7-20 10:10:35
	// ����  : void 
	// ����  : CIErrlog *log
	// ����  : ������־����
	void SetLogPara(CIErrlog *log);


	CEncryptCommBuffer();
	virtual ~CEncryptCommBuffer();


	
protected:
	bool Zip(char *data,unsigned short &datalen);
	bool UnDesThree(char *data,unsigned short &datalen);
	bool DesThree(char *data,unsigned short &datalen);
	bool DesOne(char *data,unsigned short &datalen);
	bool UnZip(char *data,unsigned short  &datalen);
	bool UnDesOne(char *data,unsigned short &datalen);

	CBF_Des        m_pDes; //�ӽ�����
	
	CIErrlog      *m_pLog; //��־��

};

#endif // !defined(AFX_ENCRYPTCOMMBUFFER_H__59BA9B16_7577_4848_AF20_DA8E0F696617__INCLUDED_)
