// ExchDll.h: interface for the CExchDll class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXCHDLL_H__7100EC87_64FF_419A_ACB3_1AD52392DAE0__INCLUDED_)
#define AFX_EXCHDLL_H__7100EC87_64FF_419A_ACB3_1AD52392DAE0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef _WINDOWS
#ifdef _DEBUG
#pragma comment(lib, "bf_kerneld.lib") 
#pragma comment(lib, "bf_xdpd.lib") 
#pragma message("Automatically linking with   bf_kerneld.lib  bf_xdpd.lib")
#else
#pragma comment(lib, "bf_kernel.lib") 
#pragma comment(lib, "bf_xdp.lib") 
#pragma message("Automatically linking with   bf_kernel.lib  bf_xdp.lib")
#endif

#endif


#include "IExchLink.h"


class DLL_EXPORT CExchDll : public CIExchLink  
{
public:
	CExchDll();
	virtual ~CExchDll();

	// ������: Send2Exch
	// ���  : ������ 2017-7-20 9:33:01
	// ����  : virtual int 
	// ����  : S_TRADE_DATA &data  
	// ����  : CXdp *xdp   xdp����
	// ����  : ���������� 
	virtual int Send2Exch(S_TRADE_DATA &data,CXdp *xdp);
	
	
	// ������: Init
	// ���  : ������ 2017-8-10 15:41:08
	// ����  : virtual bool 
	// ����  : CBF_Xml *xml
	// ����  : ��ʼ��
	virtual bool Init(CBF_Xml *xml,CIClientLink *pBusLink,CMemDb *pMemDb,	CXdp *pXdp,	CIErrlog *pLog);
	
	// ������: Start
	// ���  : ������ 2017-8-10 15:39:45
	// ����  : virtual bool 
	// ����  : �������������̨����
	virtual bool Start();
	
	
	// ������: Stop
	// ���  : ������ 2017-8-10 15:40:09
	// ����  : virtual bool 
	// ����  : ֹͣ
	virtual bool Stop();
	
	
	// ������: Join
	// ���  : ������ 2017-8-10 15:40:21
	// ����  : virtual bool 
	// ����  : �ȴ��߳�ֹͣ
	virtual bool Join();



	char m_errMsg[256];

};
extern "C" 
{ 
    DLL_EXPORT CExchDll * getInstance() ; 
}
#endif // !defined(AFX_EXCHDLL_H__7100EC87_64FF_419A_ACB3_1AD52392DAE0__INCLUDED_)
