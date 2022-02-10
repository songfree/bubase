// BF_AIO_Base.h: interface for the CBF_AIO_Base class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BF_AIO_BASE_H__81C91698_796D_4220_84E1_4611D9C58EBE__INCLUDED_)
#define AFX_BF_AIO_BASE_H__81C91698_796D_4220_84E1_4611D9C58EBE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BF_AIO_Head.h"



#ifdef AIO_EXPORTS
#define AIO_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define AIO_EXPORT __declspec(dllimport)
#else
#define AIO_EXPORT 
#endif
#endif

class AIO_EXPORT CBF_AIO_Base  
{
public:
	// ������: GetSocketErrNo
	// ���  : ������ 2012-5-24 15:03:40
	// ����  : int  
	// ����  : ȡ��iocp���ͳ���ʱ��ERRNO
	virtual int  GetSocketErrNo()=0;

	// ������: Send
	// ���  : ������ 2012-5-21 16:52:24
	// ����  : bool 
	// ����  : PSOCKET_SEND_DATA senddata
	// ����  : ��������
	virtual bool Send(SOCKET_SEND_DATA senddata,bool isimmediate=false)=0;
	
	// ������: AddAio
	// ���  : ������ 2012-5-21 10:03:52
	// ����  : bool 
	// ����  : PSOCKET_POOL_DATA *info
	// ����  : ��������Ϣ���뵽AIO��
    virtual bool AddAio(PSOCKET_POOL_DATA info)=0;


	// ������: Close
	// ���  : ������ 2012-5-22 11:16:09
	// ����  : virtual bool 
	// ����  : PSOCKET_POOL_DATA info
	// ����  : Ӧ�������ر�����
	virtual bool Close(PSOCKET_POOL_DATA info)=0;
	
	// ������: Connect
	// ���  : ������ 2012-5-21 11:56:45
	// ����  : bool 
	// ����  : char *ip  ���ӵ�IP
	// ����  : short port  �˿�
	// ����  : unsigned  short  sockettype   ��������
	// ����  : unsigned int destindex        ����vc�Ļ�Ҫָ������������
	// ����  : unsigned int &index           AIO���ش���������
	// ����  : Ӧ��Ҫ�������ӣ�OnConnectedBack�����Ƿ�ɹ�����
	virtual bool Connect(char *ip,short port,unsigned  short  sockettype, unsigned int destindex ,unsigned int &index)=0;

	CBF_AIO_Base()
	{

	}
	virtual ~CBF_AIO_Base()
	{

	}

};

#endif // !defined(AFX_BF_AIO_BASE_H__81C91698_796D_4220_84E1_4611D9C58EBE__INCLUDED_)
