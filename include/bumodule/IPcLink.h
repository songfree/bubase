// IPcLink.h: interface for the CIPcLink class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPCLINK_H__189DF427_9657_4008_9915_8DA339C13457__INCLUDED_)
#define AFX_IPCLINK_H__189DF427_9657_4008_9915_8DA339C13457__INCLUDED_

//BPC��FPC���ӵĻ���
#include "public.h"
#include "BpcEndian.h"
#include "BpcHead.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



class CIPcLink  
{
public:
	/// ������: SetGPara1
	/// ���  : ������ 2008-12-30 9:42:48
	/// ����  : virtual bool true�ɹ�falseʧ��
	/// ����  : bool setflag=true
	/// ����  : �ӿ��麯��������Ҫʵ��.����ȫ�ֱ���1,���������򷵻�false
	virtual bool SetGPara1(bool setflag=true)=0;
	
	
	/// ������: isSetGPara2
	/// ���  : ������ 2008-12-30 9:36:10
	/// ����  : virtual bool true�ɹ�falseʧ��
	/// ����  : bool setflag=true
	/// ����  : ����ȫ�ֱ���2,���������򷵻�false
	virtual bool SetGPara2(bool setflag=true)=0;
	
	
	/// ������: isSetGPara3
	/// ���  : ������ 2008-12-30 9:36:13
	/// ����  : virtual bool true�ɹ�falseʧ��
	/// ����  : bool setflag=true
	/// ����  : ����ȫ�ֱ���3,���������򷵻�false
	virtual bool SetGPara3(bool setflag=true)=0;
	
	/// ������: GetThreadShareSerial
	/// ���  : ������ 2008-12-30 9:36:13
	/// ����  : virtual long ��ˮ��
	/// ����  : ȡ��Ψһ����ˮ��
	virtual long GetThreadShareSerial()=0;


	// ������: AnsData
	// ���  : ������ 2013-4-23 15:16:43
	// ����  : virtual int -1ʧ�� 0�ɹ�
	// ����  : S_BPC_RSMSG *data
	// ����  : Ӧ������,��BPUҵ�������ԭ�ӵ���
	virtual int AnsData(PBPCCOMMSTRU data) =0;

	// ������: DPTrans
	// ���  : ������ 2013-4-23 15:11:48
	// ����  : virtual int -1ʧ�� 0�ɹ�
	// ����  : PBPCCOMMSTRU calldata
	// ����  : ת��������������
	virtual int DPTrans(PBPCCOMMSTRU calldata) =0;

	// ������: BpuCall
	// ���  : ������ 2013-4-23 15:09:13
	// ����  : virtual int -1ʧ�� 0�ɹ�
	// ����  : PBPCCOMMSTRU calldata
	// ����  : LIST_BPC_RSMSG &ansdata
	// ����  : ����BPU�ڵĹ��ܣ������̿���ʹ��
	virtual int BpuCall(PBPCCOMMSTRU calldata,LIST_BPC_RSMSG &ansdata) =0;

	// ������: BpcCall
	// ���  : ������ 2013-4-23 15:08:18
	// ����  : virtual int -1ʧ�� 0�ɹ�
	// ����  : PBPCCOMMSTRU calldata
	// ����  : LIST_BPC_RSMSG &ansdata
	// ����  : ����bpc����
	virtual int BpcCall(PBPCCOMMSTRU calldata,LIST_BPC_RSMSG &ansdata,unsigned int stimeout) =0;

	// ������: ExtCall
	// ���  : ������ 2013-4-23 15:01:25
	// ����  : virtual int -1ʧ�� 0�ɹ�
	// ����  : PBPCCOMMSTRU calldata
	// ����  : vector<S_BPC_RSMSG> &ansdata
	// ����  : ���BPC
	virtual int ExtCall(PBPCCOMMSTRU calldata,LIST_BPC_RSMSG &ansdata,unsigned int stimeout) =0;
	

	// ������:  InitLink
	// ���  : ������ 2013-4-23 14:44:14
	// ����  : virtual bool true�ɹ� falseʧ��
	// ����  : const char *confile
	// ����  : ��ʼ���������� 
	virtual bool  InitLink(const char *confile) =0;
	CIPcLink()
	{

	}
	virtual ~CIPcLink()
	{

	}



};

#endif // !defined(AFX_IPCLINK_H__189DF427_9657_4008_9915_8DA339C13457__INCLUDED_)
