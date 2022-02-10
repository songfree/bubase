
#include "public.h"

#ifndef ENCRYPT_H 
#define ENCRYPT_H
#define ENCRYPT 0
#define DISCRYPT 1
#define DECRYPT  1
#define   STAND    0         /* MAC 标准 */
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

	// 函数名: Des
	// 编程  : 王明松 2013-6-21 10:20:11
	// 返回  : int 
	// 参数  : char *txt  文本
	// 参数  : char *obj  结果
	// 参数  : char *key  密钥8字节
	// 参数  : int mode   0加密1解密
	// 描述  : 加解密 
    int Des(char *txt,char *obj,char *key,int mode);  
	
	
	// 函数名: *des
	// 编程  : 王明松 2013-6-21 10:05:40
	// 返回  : char  *      密文8字节长度    
	// 参数  : char *d64    明文8字节长度
	// 参数  : char *s_key  密钥8字节长度
	// 描述  : 64位des加密
    char *des(char *d64 ,char *s_key);
	
	// 函数名: *undes
	// 编程  : 王明松 2013-6-21 10:05:49
	// 返回  : char  *       明文8字节长度
	// 参数  : char *d64     密文8字节长度
	// 参数  : char *s_key   密钥8字节长度
	// 描述  : 64位des解密
    char *undes(char *d64 ,char *s_key);

public: 
    CBF_DesEnctypt();
    ~CBF_DesEnctypt();     

	// 函数名: DES64
	// 编程  : 王明松 2013-6-21 10:05:53
	// 返回  : void 
	// 参数  : unsigned char *source  8字节长度
	// 参数  : unsigned char *dest    8字节长度
	// 参数  : unsigned char *key   密钥8字节长度
	// 参数  : unsigned char flag   0加密 1解密 
	// 描述  : 64位des加解密 长度不变
	void DES64(unsigned char *source,unsigned char *dest,unsigned char *key,unsigned char flag);

	// 函数名: DES
	// 编程  : 王明松 2013-6-21 10:11:24
	// 返回  : void 
	// 参数  : unsigned char *source  加密为8字节，解密为16字节(0-F)
	// 参数  : unsigned char *dest   加密后为16字节(0-F)，解密为8字节
	// 参数  : unsigned char *key  密钥8字节长度
	// 参数  : unsigned char flag  0加密 1解密 
	// 描述  : 64位des加解密 加密长度*2 
    void DES(unsigned char *source,unsigned char *dest,unsigned char *key,unsigned char flag);

	/// 函数名: encryptchar
	/// 编程  : 王明松 2007-3-25 10:19:52
	/// 返回  : int 0成功 <0失败
	/// 参数  : char *buf  加密前及加密后的buf
	/// 参数  : int buflen  加密前buf的长度
	/// 参数  : char *key   加密的key
	/// 描述  : 对字串进行加密
	int encryptchar(char *buf,int buflen,const char *key);

	/// 函数名: unencryptchar
	/// 编程  : 王明松 2007-3-25 10:19:56
	/// 返回  : int  0成功 <0失败
	/// 参数  : char *buf   加密及解密后的
	/// 参数  : int buflen  解密前的buf长度
	/// 参数  : char *key 解密的key
	/// 描述  : 对字串进行解密
	int unencryptchar(char *buf,int buflen ,const char *key);

	/// 函数名: encrypt64
	/// 编程  : 王明松 2007-3-25 10:19:52
	/// 返回  : int 0成功 <0失败
	/// 参数  : char *buf  加密前及加密后的buf
	/// 参数  : int buflen  加密前buf的长度
	/// 参数  : char *key   加密的key
	/// 描述  : 对字串进行加密
	int encrypt64(char *buf,int &buflen,const char *key);
	
	/// 函数名: unencrypt64
	/// 编程  : 王明松 2007-3-25 10:19:56
	/// 返回  : int  0成功 <0失败
	/// 参数  : char *buf   加密及解密后的
	/// 参数  : int buflen  解密前的buf长度
	/// 参数  : char *key 解密的key
	/// 描述  : 对字串进行解密
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
