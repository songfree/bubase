// BF_Des.h: interface for the CBF_Des class.
//
//////////////////////////////////////////////////////////////////////

/***********************************************************************************
  ������־��¼
  ������: ������
  �������� 2012-4-11 9:47:37 �ع�
  �汾: V1.00 
  ˵��: ��װc++��׼DES�㷨��MAC�㷨
 ***********************************************************************************/


#if !defined(AFX_BF_DES_H__20BD8B28_4A79_4970_A105_4ED81CAE96C0__INCLUDED_)
#define AFX_BF_DES_H__20BD8B28_4A79_4970_A105_4ED81CAE96C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "BF_DesEncrypt.h"

#ifdef KERNEL_EXPORTS
#define KERNEL_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define KERNEL_EXPORT __declspec(dllimport)
#else
#define KERNEL_EXPORT 
#endif
#endif

#define  DES_ONE     1
#define  DES_THREE   3


class KERNEL_EXPORT CBF_Des  
{
public:
	CBF_Des();
	virtual ~CBF_Des();

	// ������: Uncryptchar
	// ���  : ������ 2013-7-11 9:20:02
	// ����  : int  >0�ɹ�
	// ����  : char *buf    ����ǰ�������
	// ����  : int &buflen  �������ݳ���   ���Ϊ�����1/2
	// ����  : const char *key 8�ֽڳ���
	// ����  : �����ַ���
	bool Unencrypt(char *data,int &datalen,const char *key);

	// ������: Encryptchar
	// ���  : ������ 2013-7-11 9:19:58
	// ����  : int 
	// ����  : char *buf        ����ǰ�������
	// ����  : int &buflen      �������ݳ��� ���ܺ󳤶�Ϊ�����2��
	// ����  : const char *key  8�ֽڳ���
	// ����  : �����ַ���
	bool Encrypt(char *data,int &datalen,const char *key);

	// ������: Encryptchar
	// ���  : ������ 2013-7-11 9:19:58
	// ����  : int 
	// ����  : char *buf        ����ǰ�������
	// ����  : int &buflen      �������ݳ��� ���ܺ󳤶�Ϊ�����2��
	// ����  : const char *key  8�ֽڳ���
	// ����  : �����ַ���
	int Encryptchar(char *buf,int &buflen,const char *key);

	// ������: Uncryptchar
	// ���  : ������ 2013-7-11 9:20:02
	// ����  : int  >0�ɹ�
	// ����  : char *buf    ����ǰ�������
	// ����  : int &buflen  �������ݳ���   ���Ϊ�����1/2
	// ����  : const char *key 8�ֽڳ���
	// ����  : �����ַ���
	int Uncryptchar(char *buf,int &buflen ,const char *key);

	// ������: UncryptString
	// ���  : ������ 2012-4-11 13:45:41
	// ����  : int >0�ɹ�  
	// ����  : char *buf  �������� ע��buf��sizeӦ����8�ı���
	// ����  : int &buflen �������ݳ���   ���������ĳ��������8����
	// ����  : const char *key   8�ֽڳ���
	// ����  : �����ַ���
	int UncryptString(char *buf,int &buflen ,const char *key);


	// ������: EncryptString
	// ���  : ������ 2012-4-11 13:46:03
	// ����  : int >0�ɹ�  
	// ����  : char *buf ��������
	// ����  : int &buflen �������ݳ���   ���������ĳ��������8����
	// ����  : const char *key 8�ֽڳ���
	// ����  : �����ַ���
	int EncryptString(char *buf,int &buflen ,const char *key);

	// ������: GetPin
	// ���  : ������ 2012-4-11 11:23:33
	// ����  : void 
	// �������  : const char *zh �ʺš�����(���ȴ���13λ) ,����ʺŲ��μ���򣬴���16�� '0' 
	// �������  : char *pinblock ����PINBLOCK (16��ASCII�ַ�)
	// �������  : char *pin      ��������(4-6λ)
	// ����  : GetPinBlock�ķ��������ͨ��pinblock�õ���������
	void GetPin(const char *zh,char *pinblock,char *pin);

	// ������: GetPinBlock
	// ���  : ������ 2012-4-11 11:21:41
	// ����  : void 
	// �������  : const char *zh    �ʺš�����(���ȴ���13λ) ,����ʺŲ��μ���򣬴���16�� '0' 
	// �������  : const char *pin   ��������(4-6λ)
	// �������  : char *pinblock  ����������PINBLOCK (16��ASCII�ַ�)
	// ����  : �õ�ANSI9.8��׼������PINBLOCK	
	void GetPinBlock(const char *zh,const char *pin,char *pinblock );

	// ������: EncryptPin
	// ���  : ������ 2012-4-11 11:21:06
	// ����  : void 
	// ����  : char *pinblock       16��ASCII�ַ�
	// ����  : const char *pinkey   ��Կ 32λ����
	// ����  : ����pinblock  3DES�㷨
	void EncryptPin(char *pinblock,const char *pinkey);

	// ������: Change_Key
	// ���  : ������ 2012-4-11 11:26:16
	// ����  : void 
	// ����  : char *key
	// ����  : int len
	// ����  : ��������Կ����У�����ʹ�������У�飬������ԿΪ0-F�ַ���ʾ����������Ϊ��0000000000000000�����Ϊ��0101010101010101
	void Change_Key(char *key,int len);


	// ������: SoftGenMac
	// ���  : ������ 2012-4-11 11:27:01
	// ����  : int 0�ɹ� -1��mackey -2mackey���Ȳ�Ϊ16 -3blocklenΪ0
	// �������  : const char *mackey     mackey��Կ 16λ����
	// �������  : const char *macblock   Ҫ����mac���������� <8k
	// �������  : char *returnmac  ���ɵ�MAC���� 16λ����
	// �������  : int blocklen     Ҫ����mac���������ݳ���
	// ����  : ����MAC
	int SoftGenMac(const char *mackey,const char *macblock,char *returnmac,int blocklen);


	// ������: SoftEndesc
	// ���  : ������ 2012-6-20 22:39:22
	// ����  : int 0 �ɹ� ����ʧ��
	// ����  : unsigned char *mkey ��Կ ����16/32/48  des/2des/3des
	// ����  : unsigned char *pin   ����  ����16
	// ����  : char *outpin         ����  ����16
	// ����  : ����
	int SoftEndesc(unsigned char *mkey, unsigned char *pin, char *outpin);


	// ������: SoftDedesc
	// ���  : ������ 2012-6-20 22:39:27
	// ����  : int  0 �ɹ� ����ʧ��
	// ����  : unsigned char *mkey  ��Կ ����16/32/48  des/2des/3des
	// ����  : unsigned char *pin   ����  ����16
	// ����  : char *outpin         ����  ����16
	// ����  : ����
	int SoftDedesc(unsigned char *mkey, unsigned char *pin, char *outpin);
	

	// ������: HEX_2_DSP
	// ���  : ������ 2012-4-11 13:31:26
	// ����  : void 
	// �������  : const char *hex  ����
	// �������  : char *dsp  ����ʾ������  1-9,A-F
	// �������  : int count hex�ĳ���
	// ����  : ������ת��Ϊ16������ʾ��ת��������ݳ���Ϊcount*2
	void HEX_2_DSP(const char *hex,char *dsp,int count);

	// ������: DSP_2_HEX
	// ���  : ������ 2012-4-11 13:33:10
	// ����  : void 
	// �������  : const char *dsp  ����ʾ������  1-9,A-F
	// �������  : char *hex  ʵ������
	// �������  : int count  dsp����
	// ����  : ��16������ʾ������ת��Ϊʵ������,ת��������ݳ���Ϊcount/2
	void DSP_2_HEX(const char *dsp,char *hex,int count);

	CBF_DesEnctypt m_pDesEncrypt;
	
private:

	// ������: UnpackBCD
	// ���  : ������ 2012-4-11 11:20:38
	// ����  : int =0�ɹ�
	// ����  : const unsigned char *InBuf
	// ����  : char *OutBuf
	// ����  : int Len InBuf����
	// ����  : ת��Ϊ10������
	int UnpackBCD(const unsigned char *InBuf,  char *OutBuf, int Len );

	// ������: PackBCD
	// ���  : ������ 2012-4-11 11:10:55
	// ����  : int 0�ɹ�
	// ����  : const char *InBuf
	// ����  : unsigned char *OutBuf
	// ����  : int Len  InBuf����
	// ����  : ת��ΪBCD��
	int PackBCD(const char *InBuf, unsigned char *OutBuf, int Len );

	// ������: StringXOR
	// ���  : ������ 2012-4-11 11:10:26
	// ����  : void 
	// �����������  : unsigned char *str1  16λ����
	// ����  : unsigned char *str2  16λ����
	// ����  : �ַ�����򣬽������str1
	void StringXOR(unsigned char *str1, unsigned char *str2);
	
	void permute(char *inblock,char perm[16][16][8],char *outblock);
	// 1 compression value for sinit
	int getcomp(int k,int v);	
	// initialize s1-s8 arrays	
	void sinit();
	// initialize key schedule array	 
	void kinit(char *key);	
	// initialize 32-bit permutation
	void p32init();		
	// initialize a perm array	
	void perminit(char perm[16][16][8],const char p[64]);
	//32 to 48 bits with E oper
	// right is 32, bigright 48
	void expand(char *right,char *bigright);
	//contract f from 48 to 32 bits
	// using 12-bit pieces into bytes 
	void contract(char *in48,char *out32);
	// 32-bit permutation at end
	// of the f crypto function	
	void perm32ex(char *inblock,char *outblock);
	//critical cryptographic trans
	void f(char *right,int num,char *fret);
	// 1 churning operation		
	void iter(int num,char *inblock,char *outblock);
	// initialize all des arrays	
	void desinit(char *key);
	// encrypt 64-bit inblock	
	void endes(char *inblock,char *outblock);
	// decrypt 64-bit inblock	
	void dedes(char *inblock,char *outblock);


	// ������: Desc
	// ���  : ������ 2012-4-11 11:37:29
	// ����  : void 
	// ����  : unsigned char *data 16λ��������
	// ����  : unsigned char *mkey 16λ������Կ
	// ����  : int tag           0���� 1����
	// ����  : ��׼DES�㷨
 	void Desc(unsigned char *data, unsigned char *mkey, int tag);

	char m_iperm[16][16][8]; /* inital and final permutations*/
	char m_fperm[16][16][8]; /* inital and final permutations*/
	char m_s[4][4096];			/* S1 thru S8 precomputed	*/
	char m_p32[4][256][4];			/* for permuting 32-bit f output*/
	char m_kn[16][6];				/* key selections		*/
	char m_pc1m[56];			   /* place to modify pc1 into	*/
	char m_pcr[56];			   /* place to rotate pc1 into	*/
};

#endif // !defined(AFX_BF_DES_H__20BD8B28_4A79_4970_A105_4ED81CAE96C0__INCLUDED_)
