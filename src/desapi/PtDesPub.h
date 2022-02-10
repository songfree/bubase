#if !defined(AFX_PTDES_PUB_H__)
#define AFX_PTDES_PUB_H__


#ifdef MYKERNEL_EXPORTS
#define KERNEL_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define KERNEL_EXPORT __declspec(dllimport)
#else
#define KERNEL_EXPORT 
#endif
#endif


extern "C"
{

	// 函数名: softgenmac
	// 编程  : 王明松 2013-1-27 8:37:16
	// 返回  : int  0成功
	// 输入参数  : char *makkey   mackey 明文 
	// 输入参数  : char *macblock   要生成MAC的报文块
	// 输出参数  : char *returnmac  返回的MAC内容
	// 输入参数  : int blocklen  要生成MAC的报文块长度
	// 描述  : 生成MAC
	 KERNEL_EXPORT  int  softgenmac(char *makkey,char *macblock,char *returnmac,int blocklen);


	// 函数名: softendesc
	// 编程  : 王明松 2013-1-27 8:39:33
	// 返回  : int  0成功
	// 参数  : char *mkey   密钥   长度为16倍数   16为des  32为2des 48为3des
	// 参数  : char *pin    要加密的密码块 (一般先调用getpinblock和账号异或得到密码块) 长度为16
	// 参数  : char *outpin  加密的密文 长度不变仍为16
	// 描述  : 软加密
	 KERNEL_EXPORT  int  softendesc(char *mkey, char *pin, char *outpin);

	// 函数名: softdedesc
	// 编程  : 王明松 2013-1-27 8:50:44
	// 返回  : int  0成功
	// 输入参数  : char *mkey 密钥   长度为16倍数   16为des  32为2des 48为3des
	// 输入参数  : char *pin   要解密的密码块  长度为16
	// 输出参数  : char *outpin  密码块明文 调用getbin得到密码明文
	// 描述  : 解密
	 KERNEL_EXPORT  int  softdedesc(char *mkey, char *pin, char *outpin);
	 

	// 函数名: getpinblock
	// 编程  : 王明松 2016-1-27 8:52:29
	// 返回  : void 
	// 输入参数  : char *zh  账号
	// 输入参数  : char *pin  密码明文
	// 输出参数  : char *pinblock  密码块
	// 描述  : 密码和账号异或得到密码块
	 KERNEL_EXPORT  void getpinblock(char *zh,char *pin,char *pinblock );

	// 函数名: getpin
	// 编程  : 王明松 2013-1-27 8:53:40
	// 返回  : void 
	// 输入参数  : char *zh  账号
	// 输入参数  : char *pinblock  密码块
	// 输出参数  : char *pin  密码明文
	// 描述  : 得到密码明文
	 KERNEL_EXPORT  void getpin(char *zh,char *pinblock,char *pin);

	 KERNEL_EXPORT  void change_key(char *key,int len);



	// 函数名: softencrypt
	// 编程  : 王明松 2016-1-27 8:55:36
	// 返回  : int  
	// 输入参数  : const char *pin  要加密的密码块 (一般先调用getpinblock和账号异或得到密码块) 长度为16
	// 输出参数  : char *outpin     加密的密文 长度不变仍为16
	// 描述  : 给交易客户端调用的软加密方法，密钥固定
	 KERNEL_EXPORT  int  softencrypt(const char *pin, char *outpin);


	// 函数名: softdecrypt
	// 编程  : 王明松 2016-1-27 8:57:37
	// 返回  : int  
	// 输入参数  : const char *pin 要解密的密码块  长度为16
	// 输出参数  : char *outpin    密码块明文 调用getbin得到密码明文
	// 描述  : 给交易客户端调用的软解密方法，密钥固定
	 KERNEL_EXPORT  int  softdecrypt(const char *pin, char *outpin);


	// 函数名: softde2pwd
	// 编程  : 王明松 2016-1-27 9:00:22
	// 返回  : int  
	// 输入参数  : const char *zh   账号
	// 输入参数  : const char *pin  要解密的密码块 长度为16
	// 输出参数  : char *outpin  密码明文
	// 描述  : 给交易客户端调用的软解密方法，密钥固定,这里直接调用getbin得到了密码明文
	 KERNEL_EXPORT  int  softde2pwd(const char *zh,const char *pin, char *outpin);
};



#endif /* !defined(AFX_PTDES_PUB_H__E3B5E504_19B5_11D3_8A76_00A00C12E824__INCLUDED_)*/
