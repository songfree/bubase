// PortTbl.h: interface for the CPortTbl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PORTTBL_H__C88A811D_0EB0_4CB7_BA1F_A3FA529F9BE6__INCLUDED_)
#define AFX_PORTTBL_H__C88A811D_0EB0_4CB7_BA1F_A3FA529F9BE6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MdbBase.h"

typedef struct 
{
	std::string    sPortName; 
	unsigned short nPort;
} S_TIB_PORT;

#ifdef TIB_EXPORTS
#define TIB_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define TIB_EXPORT __declspec(dllimport)
#else
#define TIB_EXPORT 
#endif
#endif

class TIB_EXPORT CPortTbl  
{
public:

	// ������: At
	// ���  : ������ 2015-6-18 9:39:50
	// ����  : int <=0 ʧ��   >0�˿�
	// ����  : unsigned int index
	// ����  : ��������ȡ�ö˿�
	int At(unsigned int index);

	// ������: GetPortByName
	// ���  : ������ 2015-6-18 9:37:51
	// ����  : int <0ʧ�ܣ��޴�����  >=0Ϊ�˿�
	// ����  : const char *portname
	// ����  : ͨ���˿�����ȡ�ö˿�
	int GetPortByName(const char *portname);

	// ������: GetIndexByName
	// ���  : ������ 2015-6-18 9:35:15
	// ����  : int <0û�д�����   >=Ϊ��Ӧ������ֵ
	// ����  : const char *portname
	// ����  : ͨ���˿����Ƶõ��˿ڶ�Ӧ����������ͨ������ֱ��ȡ�ö˿�ֵ��������ȥ����
	int GetIndexByName(const char *portname);

	// ������: Add
	// ���  : ������ 2015-6-18 9:34:46
	// ����  : bool true�ɹ� falseʧ�ܣ��˿��ظ���˿������ظ�
	// ����  : S_TIB_PORT &portdata
	// ����  : ����һ���˿�
	bool Add(S_TIB_PORT &portdata);

	// ������: Clear
	// ���  : ������ 2015-6-18 9:34:36
	// ����  : void 
	// ����  : ����б�
	void Clear();
	CPortTbl();
	virtual ~CPortTbl();


private:
	std::vector<S_TIB_PORT>  m_vPortList;//�˿��б�

};

#endif // !defined(AFX_PORTTBL_H__C88A811D_0EB0_4CB7_BA1F_A3FA529F9BE6__INCLUDED_)
