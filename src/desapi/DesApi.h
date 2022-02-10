// DesApi.h: interface for the CDesApi class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DESAPI_H__BD08438C_A910_43CC_9CFA_1D0BAF156AFB__INCLUDED_)
#define AFX_DESAPI_H__BD08438C_A910_43CC_9CFA_1D0BAF156AFB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "TcpSocket.h"
#include "Errlog.h"

class CDesApi  
{
public:

	// ������: ConvertPinBlock
	// ���  : ������ 2011-3-1 19:46:58
	// ����  : int 0�ɹ� <0���� >0���ܻ����ش�
	// �������  : const char *srcpin ԴPIN �ɼ��ַ�  srcpinkey���ܵ�
	// �������  : const char *srcpinkey    �ɼ��ַ�  ����Կ���ܵ�
	// �������  : const char *destpinkey   �ɼ��ַ�  ����Կ���ܵ�
	// �������  : char *despin             �ɼ��ַ�  destpinkey���ܵ�
	// �������  : const char *bankacc      �˻�
	// ����  : ת���� �˷��������ʴ������PINתΪ���ܻ���PIN
	int ConvertPinBlock(const char *srcpin,const char *srcpinkey,const char *destpinkey,char *despin,const char *bankacc);

	// ������: CallSrv
	// ���  : ������ 2011-3-1 19:23:28
	// ����  : int -1ʧ�� 0�ɹ�  >0������
	// ����  : const char *data  ��������(��������Ϣͷ)
	// ����  : int datalen   ���ݳ���
	// ����  : char *rdata   ���ܻ�Ӧ������
	// ����  : int &rlen     ���ܻ�Ӧ�����ݳ���
	// ����  : ���ü��ܻ�
	int CallSrv(const char *data,int datalen,char *rdata,int &rlen);

	// ������: DisConnect
	// ���  : ������ 2011-3-1 19:23:16
	// ����  : void 
	// ����  : �Ͽ����ܻ�
	void DisConnect();

	// ������: Connect
	// ���  : ������ 2011-3-1 19:23:07
	// ����  : bool 
	// ����  : const char *ip
	// ����  : int port
	// ����  : int timeout
	// ����  : ���Ӽ��ܻ�
	bool Connect(const char *ip,int port,int timeout);

	// ������: Connect
	// ���  : ������ 2011-3-1 19:22:56
	// ����  : bool 
	// ����  : ���Ӽ��ܻ�
	bool Connect();

	

	// ������: SetHostPara
	// ���  : ������ 2011-3-1 19:22:41
	// ����  : void 
	// ����  : const char *ip
	// ����  : int port
	// ����  : int timeout
	// ����  : ���ü��ܻ�����
	void SetHostPara(const char *ip,int port,int timeout);
	CDesApi();
	virtual ~CDesApi();

	CErrlog m_pLog;

protected:
	CTcpSocket m_pSocket;
	
	char m_sRSBuffer[8192];
	std::string m_sIp;
	int         m_nPort;
	int         m_nTimeOut;

	bool        m_bIsConnect;
	bool        m_bIsSet;
};

#endif // !defined(AFX_DESAPI_H__BD08438C_A910_43CC_9CFA_1D0BAF156AFB__INCLUDED_)
