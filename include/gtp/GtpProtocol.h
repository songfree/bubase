// GtpProtocol.h: interface for the CGtpProtocol class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GTPPROTOCOL_H__90EBA403_F326_4D89_A996_9727CC0199BB__INCLUDED_)
#define AFX_GTPPROTOCOL_H__90EBA403_F326_4D89_A996_9727CC0199BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GtpData.h"
#include "BpcEndian.h"


#ifdef _WINDOWS
#ifdef _DEBUG
#pragma comment(lib, "bf_dbpubd.lib") 
#pragma comment(lib, "bf_kerneld.lib") 
#pragma message("Automatically linking with  bf_kerneld.lib bf_dbpubd.lib")
#else
#pragma comment(lib, "bf_kernel.lib") 
#pragma comment(lib, "bf_dbpub.lib") 
#pragma message("Automatically linking with  bf_kernel.lib bf_dbpub.lib")
#endif
#endif

//��Իƽ������ı���Э����

#define  GTPTypeNone                0     //�˱��Ĳ������κ����壬һ����������
#define  GTPTypeString              1     //������������������������
#define  GTPTypeBinary              2     //�������Ƕ���������
#define  GTPTypeCompressed          3     //��������ѹ��������ݣ���Ҫ��ѹ�����ٴ���
#define  GTPTypeEncrypt             4     //�������Ǽ��ܺ�����ݣ���Ҫ���ܺ��ٴ���

#define  SESSION_EXTHEAD_LEN        127   //��չ����ͷ����
#define  GTPMSGMAXLEN               16384  //GTP����ĳ���

#ifdef __cplusplus
extern "C" {
#endif
	
	//�ֽڶ���
#ifdef _WINDOWS
#define __PACKED__ 
#pragma pack(push,1)
#else
#ifdef __GNUC__
#define __PACKED__	__attribute__ ((packed))
	//#pragma pack(push,1)
#else
#define __PACKED__
#pragma options align=packed
#endif
#endif

//  ħ��(4)+LOGID(4)+������ͷ(4)+���䱨ͷ(0-127)+������(GTP)

typedef struct 
{
	unsigned int   nRandNum;//ħ��   �����ڻỰ���ȡ����ʱ������֤������������������ʱ�����ڵײ㼰ʱʶ��
	unsigned int   nLogId;//��־ID   ������־��ɸѡ���Ų�����  ���շ�������������Ӧ�ĺ�������ͨ��logID����ʶ���Ա������󣺶���ϵͳ���ͱ�������ʱ�����������һ��logID������������������������ñ����������Ӧ�����Ϣ������Ӧ�𡢱����ر����ɽ��ر���������ϸ�����ֵ
	unsigned char  cPackType;//��������   �������궨��
	unsigned char  cExtLength;//���䱨�ĳ���
	unsigned short nBodyLen;   //��Ϣ���ĳ���
}__PACKED__ S_GTP_SESSION_HEAD;

typedef struct 
{
	S_GTP_SESSION_HEAD  head;         //�Ựͷ
	char                sExtInfo[SESSION_EXTHEAD_LEN];//���䱨��ͷ
}__PACKED__ S_GTP_SESSION;


typedef struct 
{
	S_GTP_SESSION_HEAD  head;         //�Ựͷ
	char                sExtInfo[SESSION_EXTHEAD_LEN];//���䱨��ͷ
	char                sGtpMsg[GTPMSGMAXLEN];
}__PACKED__ S_GTP_MSG;

//ȡ���ֽڶ���
#ifdef _WINDOWS
#pragma pack(pop)
#else
#ifdef __GNUC__
//#pragma pack(pop)
#else
#pragma options align=reset
#endif
#endif
#undef __PACKED__

#ifdef __cplusplus
}
#endif




#ifdef GTP_EXPORTS
#define GTP_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define GTP_EXPORT __declspec(dllimport)
#else
#define GTP_EXPORT 
#endif
#endif

class GTP_EXPORT CGtpProtocol  
{
public:

	// ������: ToGtpDataString
	// ���  : ������ 2016-3-3 14:26:30
	// ����  : bool 
	// ����  : std::string gtpdata
	// ����  : ֻ������ı���
	bool ToGtpDataString(std::string gtpdata);


	// ������: ToGtpHeadBuffer
	// ���  : ������ 2016-3-3 14:26:52
	// ����  : bool 
	// ����  : char *gtpbuffer
	// ����  : int &buflen
	// ����  : ����GTP�Ự����ͷ    
	bool ToGtpHeadBuffer(char *gtpbuffer,int &buflen);

	// ������: FromGtpData
	// ���  : ������ 2016-3-3 14:12:54
	// ����  : bool 
	// ����  : const char *gtpdata   kv����ı�����
	// ����  : �����ı����� �ر��ǵ�gtpЭ��������ı����Ǽ��ܻ�ѹ��ʱ���ȵ���FromGtpBuffer�õ��Ựͷ�ͼ���ѹ�����ģ�ȥ���ܽ�ѹ���ٵ��ñ�����
	bool FromGtpData(const char *gtpdata);

	// ������: ToBuffer
	// ���  : ������ 2016-3-3 10:03:11
	// ����  : bool true�ɹ�
	// �������  : char *gtpbuffer  ����  
	// �����������  : int &buflen  ����Ϊ�����С ���Ϊ���Ĵ�С
	// ����  : ����gtpЭ�鱨�ģ����������ɵı�������ΪGTPTypeString
	bool ToBuffer(char *gtpbuffer, int &buflen);

	// ������: FromBuffer
	// ���  : ������ 2016-3-3 10:02:07
	// ����  : int  <0��������  0Ϊ��GTP����   1Ϊ���ı�������  >1 GTP���ݷ����ı�������Ϊѹ������ܱ�־������������2 3 4
	// �������  : const char *gtpbuffer   GTPЭ�鱨��  
	// �����������  : int buflen          ����ΪGTP�������ݳ��ȣ����Ϊ��ǰ��GTP���ĳ���
	// ����  : �ӻ������GTPЭ�鱨�ģ�������������GTPTypeString�����������ֱ�ӷ��ر������ͣ��������buflenȡ�����Ľ��д���
	int FromGtpBuffer(const char *gtpbuffer,int &buflen);

	// ������: GetGtpData
	// ���  : ������ 2016-3-3 10:00:44
	// ����  : CGtpData * 
	// ����  : ȡGTP������ָ�룬Ȼ���������ֵȡֵ�ȵ�
	CGtpData * GetGtpData();

	// ������: SetHeadInfo
	// ���  : ������ 2016-3-3 9:38:32
	// ����  : void 
	// ����  : unsigned int randnum   ħ��
	// ����  : unsigned int logid     ��־ID
	// ����  : char msgtype           ��������
	// ����  : ���ûỰͷ��Ϣ
	void SetHeadInfo(unsigned int randnum, unsigned int logid,char  msgtype);

	// ������: SetExtHead
	// ���  : ������ 2016-3-3 9:40:28
	// ����  : bool true�ɹ�
	// ����  : int extlen   ��չ����ͷ���ȣ����ܴ���127
	// ����  : char *head   ��չ����ͷ��Ϣ
	// ����  : ������չ����ͷ��Ϣ
	bool SetExtHead(int extlen,char *head);

	// ������: SetBodyLen
	// ���  : ������ 2016-3-3 9:39:45
	// ����  : void 
	// ����  : unsigned short len  gtp����(k=v,k1=v1��)
	// ����  : ����gtp��Ϣ����
	void SetBodyLen(unsigned short len);

	// ������: NewGtp
	// ���  : ������ 2016-3-3 9:29:58
	// ����  : void 
	// ����  : unsigned int randnum   ħ��
	// ����  : unsigned int logid     ��־ID
	// ����  : char cPackType=GTPTypeString  ��������
	// ����  : ����һ���µ�GTP���ݰ�  ��չͷĬ��û��
	void NewGtp(unsigned int randnum,unsigned int logid,char cPackType=GTPTypeString);

	CGtpProtocol();
	virtual ~CGtpProtocol();

protected:

	S_GTP_SESSION   m_pGtpSessionHead;//�Ự����ͷ

	CGtpData  m_pGtpData;//gtp������

	CBF_SimpleLog  m_pLog;
	CBpcEndian     m_pEndian;//�ֽ�����

};

#endif // !defined(AFX_GTPPROTOCOL_H__90EBA403_F326_4D89_A996_9727CC0199BB__INCLUDED_)
