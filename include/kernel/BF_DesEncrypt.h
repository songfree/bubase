
#include "public.h"

#ifndef ENCRYPT_H 
#define ENCRYPT_H
#define ENCRYPT 0
#define DISCRYPT 1
#define DECRYPT  1
#define   STAND    0         /* MAC ��׼ */
#define   BPI      1

#ifdef KERNEL_EXPORTS
#define KERNEL_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define KERNEL_EXPORT __declspec(dllimport)
#else
#define KERNEL_EXPORT 
#endif
#endif
class KERNEL_EXPORT CBF_DesEnctypt
{
private:
	int get_bit(char *s_key,int bits);
	int put_bit(char *o_key,int bits,int v);
	int transkey(char *skey,char *okey,int count,int *tab);
	int k64to56(char *skey,char *okey);
	int k56to48(char *skey,char *okey);
	int k64to64(char *skey,char *okey);

	void s_box(char *aa,char *bb);
	void F(int n,char *ll,char *rr,char *LL,char *RR);
	void _ip(unsigned char *text,char *ll,char *rr);
	void ip(unsigned char *text,char *ll,char *rr);
	void son(char *cc,char *dd,char *kk);
	void LS(char *bits,char *buffer,int count);
	void setkeystar(unsigned char bits[64]);
	void compress016(char *out,unsigned char *in);
	void compress0(char *out,unsigned char *in);
	void expand0(unsigned char *in,char *out);
	void discrypt0(unsigned char *mtext,unsigned char *text);
	void encrypt0(unsigned char *text,unsigned char *mtext);
	void HEX_2_DSP(char *hex,char *dsp,int count);
	void DSP_2_HEX(char *dsp,char *hex,int count);
	void  HostDes(unsigned char *card_no,unsigned char *work_key,unsigned char *pin,unsigned char *encrypt_pin,unsigned char flag);
	void MAC(unsigned char *packet,int packet_size,unsigned char * mac_value,unsigned char * key,unsigned char mode);
	void Do_XOR(unsigned char *dest,unsigned char *source,int size);

	// ������: Des
	// ���  : ������ 2013-6-21 10:20:11
	// ����  : int 
	// ����  : char *txt  �ı�
	// ����  : char *obj  ���
	// ����  : char *key  ��Կ8�ֽ�
	// ����  : int mode   0����1����
	// ����  : �ӽ��� 
    int Des(char *txt,char *obj,char *key,int mode);  
	
	
	// ������: *des
	// ���  : ������ 2013-6-21 10:05:40
	// ����  : char  *      ����8�ֽڳ���    
	// ����  : char *d64    ����8�ֽڳ���
	// ����  : char *s_key  ��Կ8�ֽڳ���
	// ����  : 64λdes����
    char *des(char *d64 ,char *s_key);
	
	// ������: *undes
	// ���  : ������ 2013-6-21 10:05:49
	// ����  : char  *       ����8�ֽڳ���
	// ����  : char *d64     ����8�ֽڳ���
	// ����  : char *s_key   ��Կ8�ֽڳ���
	// ����  : 64λdes����
    char *undes(char *d64 ,char *s_key);

public: 
    CBF_DesEnctypt();
    ~CBF_DesEnctypt();     

	// ������: DES64
	// ���  : ������ 2013-6-21 10:05:53
	// ����  : void 
	// ����  : unsigned char *source  8�ֽڳ���
	// ����  : unsigned char *dest    8�ֽڳ���
	// ����  : unsigned char *key   ��Կ8�ֽڳ���
	// ����  : unsigned char flag   0���� 1���� 
	// ����  : 64λdes�ӽ��� ���Ȳ���
	void DES64(unsigned char *source,unsigned char *dest,unsigned char *key,unsigned char flag);

	// ������: DES
	// ���  : ������ 2013-6-21 10:11:24
	// ����  : void 
	// ����  : unsigned char *source  ����Ϊ8�ֽڣ�����Ϊ16�ֽ�(0-F)
	// ����  : unsigned char *dest   ���ܺ�Ϊ16�ֽ�(0-F)������Ϊ8�ֽ�
	// ����  : unsigned char *key  ��Կ8�ֽڳ���
	// ����  : unsigned char flag  0���� 1���� 
	// ����  : 64λdes�ӽ��� ���ܳ���*2 
    void DES(unsigned char *source,unsigned char *dest,unsigned char *key,unsigned char flag);

	/// ������: encryptchar
	/// ���  : ������ 2007-3-25 10:19:52
	/// ����  : int 0�ɹ� <0ʧ��
	/// ����  : char *buf  ����ǰ�����ܺ��buf
	/// ����  : int buflen  ����ǰbuf�ĳ���
	/// ����  : char *key   ���ܵ�key
	/// ����  : ���ִ����м���
	int encryptchar(char *buf,int buflen,const char *key);

	/// ������: unencryptchar
	/// ���  : ������ 2007-3-25 10:19:56
	/// ����  : int  0�ɹ� <0ʧ��
	/// ����  : char *buf   ���ܼ����ܺ��
	/// ����  : int buflen  ����ǰ��buf����
	/// ����  : char *key ���ܵ�key
	/// ����  : ���ִ����н���
	int unencryptchar(char *buf,int buflen ,const char *key);

	/// ������: encrypt64
	/// ���  : ������ 2007-3-25 10:19:52
	/// ����  : int 0�ɹ� <0ʧ��
	/// ����  : char *buf  ����ǰ�����ܺ��buf
	/// ����  : int buflen  ����ǰbuf�ĳ���
	/// ����  : char *key   ���ܵ�key
	/// ����  : ���ִ����м���
	int encrypt64(char *buf,int &buflen,const char *key);
	
	/// ������: unencrypt64
	/// ���  : ������ 2007-3-25 10:19:56
	/// ����  : int  0�ɹ� <0ʧ��
	/// ����  : char *buf   ���ܼ����ܺ��
	/// ����  : int buflen  ����ǰ��buf����
	/// ����  : char *key ���ܵ�key
	/// ����  : ���ִ����н���
	int unencrypt64(char *buf,int &buflen ,const char *key);


	char m_keydes[9];
	char m_keyundes[9];

	unsigned char C[17][28];
	unsigned char D[17][28];
	unsigned char K[17][48];
	unsigned char c;
	unsigned char ch1;
};


#undef KERNEL_EXPORT
#endif
