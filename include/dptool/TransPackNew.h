/// TransPackNew.h: interface for the CTransPack class.
///
/////////////////////////////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRANSPACKNEW_H__C279BF91_1D35_4A2B_B76E_F9BF45B56C62__INCLUDED_)
#define AFX_TRANSPACKNEW_H__C279BF91_1D35_4A2B_B76E_F9BF45B56C62__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif /// _MSC_VER > 1000
#include "public.h"
#include "PackConvert.h"

#ifdef DPTOOL_EXPORTS
#define DPTOOL_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define DPTOOL_EXPORT __declspec(dllimport)
#else
#define DPTOOL_EXPORT 
#endif
#endif


typedef struct _S_SERVER_INFO_
{
	std::string sip;///����IP
	int    nport;///����˿�
	std::string sid;///����ID
	std::string sname;///������
	std::string stransname;///ת����������
	bool    bisTrans;///�Ƿ��ת��
};
typedef struct _S_TRANS_INFO_
{
	std::string stransname;///ת����������
	std::string sfilename; ///ת�������ļ�
	CPackConvert *packTrans;///��ת����
};
class DPTOOL_EXPORT CTransPackNew  
{
public:
	CTransPackNew();
	virtual ~CTransPackNew();
	bool m_bisUse;
	char m_errMsg[256];
public:

	/// ������: IsPut
	/// ���  : ������ 2008-6-27 15:13:07
	/// ����  : bool 
	/// ����  : _S_SERVER_INFO sinfo
	/// ����  : �����Ƿ�ɷ���
	bool IsPut(_S_SERVER_INFO_ sinfo);

	/// ������: GetServerId
	/// ���  : ������ 2008-4-27 15:12:32
	/// ����  : int <0ʧ�� =0�ɹ�
	/// ����  : string ip
	/// ����  : string &id
	/// ����  : ����IP��ַ��ȡ����ID
	int GetServerId(std::string ip, std::string &id);

	/// ������: GetServerPara
	/// ���  : ������ 2008-4-27 15:11:46
	/// ����  : int <0ʧ�� =0�ɹ�
	/// ����  : string sid
	/// ����  : string &ip
	/// ����  : int &port
	/// ����  : ���ݷ���ID��ȡIP���˿�
	int GetServerPara(std::string sid, std::string &ip,int &port);

	/// ������: GetTransByServerId
	/// ���  : ������ 2008-4-27 15:10:35
	/// ����  : CPackConvert * 
	/// ����  : string sid
	/// ����  : int &istrans �Ƿ�Ҫת����1Ҫ0��-1δ�ҵ�����
	/// ����  : ͨ������ID����ȡת����ʵ�� 
	CPackConvert * GetTransByServerId(std::string sid,int &istrans);

	/// ������: GetTransByIp
	/// ���  : ������ 2008-4-27 15:10:31
	/// ����  : CPackConvert * 
	/// ����  : string sip
	/// ����  : int &istrans �Ƿ�Ҫת����1Ҫ0��-1δ�ҵ�����
	/// ����  : ͨ�����ӵ�IP��ȡת����ʵ��
	CPackConvert * GetTransByIp(std::string sip,int &istrans);

	/// ������: Init
	/// ���  : ������ 2008-4-27 15:10:12
	/// ����  : int <0ʧ�� =0�ɹ�
	/// ����  : CXmlPack *xmlpack
	/// ����  : ��ʼ��ת��
	int Init(CXmlPack *xmlpack);

protected:

	std::vector <_S_SERVER_INFO_> m_serverList;//��������
	std::vector <_S_TRANS_INFO_> m_transList;//ת������
};
#undef DPTOOL_EXPORT
#endif /// !defined(AFX_TRANSPACKNEW_H__C279BF91_1D35_4A2B_B76E_F9BF45B56C62__INCLUDED_)
