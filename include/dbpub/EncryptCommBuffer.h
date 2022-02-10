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
	

	// 函数名: ZipDesProcess
	// 编程  : 王明松 2013-7-20 12:34:25
	// 返回  : bool true成功false失败
	// 参数  : PCOMMSTRU data  dreb数据
	// 参数  : int destflag   处理目标  0明文 1DES加密 2压缩 3DES加密 4先压缩后1加密  3和1只是密钥的区别
	// 描述  : 压缩加密处理    
	bool DrebZipDesProcess(PCOMMSTRU data,int destflag);

	// 函数名: SetLogPara
	// 编程  : 王明松 2013-7-20 10:10:35
	// 返回  : void 
	// 参数  : CIErrlog *log
	// 描述  : 设置日志参数
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

	CBF_Des        m_pDes; //加解密类
	
	CIErrlog      *m_pLog; //日志类

};

#endif // !defined(AFX_ENCRYPTCOMMBUFFER_H__59BA9B16_7577_4848_AF20_DA8E0F696617__INCLUDED_)
