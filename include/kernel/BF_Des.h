// BF_Des.h: interface for the CBF_Des class.
//
//////////////////////////////////////////////////////////////////////

/***********************************************************************************
  编码日志记录
  编码者: 王明松
  编码日期 2012-4-11 9:47:37 重构
  版本: V1.00 
  说明: 封装c++标准DES算法及MAC算法
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

	// 函数名: Uncryptchar
	// 编程  : 王明松 2013-7-11 9:20:02
	// 返回  : int  >0成功
	// 参数  : char *buf    解密前后的数据
	// 参数  : int &buflen  数据内容长度   输出为输入的1/2
	// 参数  : const char *key 8字节长度
	// 描述  : 解密字符串
	bool Unencrypt(char *data,int &datalen,const char *key);

	// 函数名: Encryptchar
	// 编程  : 王明松 2013-7-11 9:19:58
	// 返回  : int 
	// 参数  : char *buf        加密前后的数据
	// 参数  : int &buflen      数据内容长度 加密后长度为输入的2倍
	// 参数  : const char *key  8字节长度
	// 描述  : 加密字符串
	bool Encrypt(char *data,int &datalen,const char *key);

	// 函数名: Encryptchar
	// 编程  : 王明松 2013-7-11 9:19:58
	// 返回  : int 
	// 参数  : char *buf        加密前后的数据
	// 参数  : int &buflen      数据内容长度 加密后长度为输入的2倍
	// 参数  : const char *key  8字节长度
	// 描述  : 加密字符串
	int Encryptchar(char *buf,int &buflen,const char *key);

	// 函数名: Uncryptchar
	// 编程  : 王明松 2013-7-11 9:20:02
	// 返回  : int  >0成功
	// 参数  : char *buf    解密前后的数据
	// 参数  : int &buflen  数据内容长度   输出为输入的1/2
	// 参数  : const char *key 8字节长度
	// 描述  : 解密字符串
	int Uncryptchar(char *buf,int &buflen ,const char *key);

	// 函数名: UncryptString
	// 编程  : 王明松 2012-4-11 13:45:41
	// 返回  : int >0成功  
	// 参数  : char *buf  数据内容 注意buf的size应该是8的倍数
	// 参数  : int &buflen 数据内容长度   输出和输入的长度相差在8左右
	// 参数  : const char *key   8字节长度
	// 描述  : 解密字符串
	int UncryptString(char *buf,int &buflen ,const char *key);


	// 函数名: EncryptString
	// 编程  : 王明松 2012-4-11 13:46:03
	// 返回  : int >0成功  
	// 参数  : char *buf 数据内容
	// 参数  : int &buflen 数据内容长度   输出和输入的长度相差在8左右
	// 参数  : const char *key 8字节长度
	// 描述  : 加密字符串
	int EncryptString(char *buf,int &buflen ,const char *key);

	// 函数名: GetPin
	// 编程  : 王明松 2012-4-11 11:23:33
	// 返回  : void 
	// 输入参数  : const char *zh 帐号、卡号(长度大于13位) ,如果帐号不参加异或，传入16个 '0' 
	// 输入参数  : char *pinblock 明文PINBLOCK (16个ASCII字符)
	// 输出参数  : char *pin      密码明文(4-6位)
	// 描述  : GetPinBlock的反向操作，通过pinblock得到密码明文
	void GetPin(const char *zh,char *pinblock,char *pin);

	// 函数名: GetPinBlock
	// 编程  : 王明松 2012-4-11 11:21:41
	// 返回  : void 
	// 输入参数  : const char *zh    帐号、卡号(长度大于13位) ,如果帐号不参加异或，传入16个 '0' 
	// 输入参数  : const char *pin   密码明文(4-6位)
	// 输出参数  : char *pinblock  计算后的明文PINBLOCK (16个ASCII字符)
	// 描述  : 得到ANSI9.8标准的明文PINBLOCK	
	void GetPinBlock(const char *zh,const char *pin,char *pinblock );

	// 函数名: EncryptPin
	// 编程  : 王明松 2012-4-11 11:21:06
	// 返回  : void 
	// 参数  : char *pinblock       16个ASCII字符
	// 参数  : const char *pinkey   密钥 32位长度
	// 描述  : 加密pinblock  3DES算法
	void EncryptPin(char *pinblock,const char *pinkey);

	// 函数名: Change_Key
	// 编程  : 王明松 2012-4-11 11:26:16
	// 返回  : void 
	// 参数  : char *key
	// 参数  : int len
	// 描述  : 对输入密钥进行校验调整使其符合奇校验，输入密钥为0-F字符表示，例如输入为“0000000000000000”输出为“0101010101010101
	void Change_Key(char *key,int len);


	// 函数名: SoftGenMac
	// 编程  : 王明松 2012-4-11 11:27:01
	// 返回  : int 0成功 -1无mackey -2mackey长度不为16 -3blocklen为0
	// 输入参数  : const char *mackey     mackey密钥 16位长度
	// 输入参数  : const char *macblock   要生成mac的数据内容 <8k
	// 输出参数  : char *returnmac  生成的MAC内容 16位长度
	// 输入参数  : int blocklen     要生成mac的数据内容长度
	// 描述  : 生成MAC
	int SoftGenMac(const char *mackey,const char *macblock,char *returnmac,int blocklen);


	// 函数名: SoftEndesc
	// 编程  : 王明松 2012-6-20 22:39:22
	// 返回  : int 0 成功 其它失败
	// 参数  : unsigned char *mkey 密钥 长度16/32/48  des/2des/3des
	// 参数  : unsigned char *pin   明文  长度16
	// 参数  : char *outpin         密文  长度16
	// 描述  : 加密
	int SoftEndesc(unsigned char *mkey, unsigned char *pin, char *outpin);


	// 函数名: SoftDedesc
	// 编程  : 王明松 2012-6-20 22:39:27
	// 返回  : int  0 成功 其它失败
	// 参数  : unsigned char *mkey  密钥 长度16/32/48  des/2des/3des
	// 参数  : unsigned char *pin   密文  长度16
	// 参数  : char *outpin         明文  长度16
	// 描述  : 解密
	int SoftDedesc(unsigned char *mkey, unsigned char *pin, char *outpin);
	

	// 函数名: HEX_2_DSP
	// 编程  : 王明松 2012-4-11 13:31:26
	// 返回  : void 
	// 输入参数  : const char *hex  数据
	// 输出参数  : char *dsp  可显示的内容  1-9,A-F
	// 输入参数  : int count hex的长度
	// 描述  : 将数据转换为16进制显示，转换后的数据长度为count*2
	void HEX_2_DSP(const char *hex,char *dsp,int count);

	// 函数名: DSP_2_HEX
	// 编程  : 王明松 2012-4-11 13:33:10
	// 返回  : void 
	// 输入参数  : const char *dsp  可显示的内容  1-9,A-F
	// 输出参数  : char *hex  实际数据
	// 输入参数  : int count  dsp长度
	// 描述  : 将16进制显示的数据转换为实际数据,转换后的数据长度为count/2
	void DSP_2_HEX(const char *dsp,char *hex,int count);

	CBF_DesEnctypt m_pDesEncrypt;
	
private:

	// 函数名: UnpackBCD
	// 编程  : 王明松 2012-4-11 11:20:38
	// 返回  : int =0成功
	// 参数  : const unsigned char *InBuf
	// 参数  : char *OutBuf
	// 参数  : int Len InBuf长度
	// 描述  : 转换为10进制码
	int UnpackBCD(const unsigned char *InBuf,  char *OutBuf, int Len );

	// 函数名: PackBCD
	// 编程  : 王明松 2012-4-11 11:10:55
	// 返回  : int 0成功
	// 参数  : const char *InBuf
	// 参数  : unsigned char *OutBuf
	// 参数  : int Len  InBuf长度
	// 描述  : 转换为BCD码
	int PackBCD(const char *InBuf, unsigned char *OutBuf, int Len );

	// 函数名: StringXOR
	// 编程  : 王明松 2012-4-11 11:10:26
	// 返回  : void 
	// 输入输出参数  : unsigned char *str1  16位长度
	// 参数  : unsigned char *str2  16位长度
	// 描述  : 字符串异或，结果放在str1
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


	// 函数名: Desc
	// 编程  : 王明松 2012-4-11 11:37:29
	// 返回  : void 
	// 参数  : unsigned char *data 16位长度数据
	// 参数  : unsigned char *mkey 16位长度密钥
	// 参数  : int tag           0加密 1解密
	// 描述  : 标准DES算法
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
