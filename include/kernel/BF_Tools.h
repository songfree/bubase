// BF_Tools.h: interface for the CBF_Tools class.
//
//////////////////////////////////////////////////////////////////////

/***********************************************************************************
  编码日志记录
  编码者: 王明松
  编码日期 2012-4-17 15:42:23 重构
  版本: V1.00 
  说明: 一些常用的函数
 ***********************************************************************************/


#if !defined(AFX_BF_TOOLS_H__42367B68_92BE_4995_83EA_475E113120FA__INCLUDED_)
#define AFX_BF_TOOLS_H__42367B68_92BE_4995_83EA_475E113120FA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "public.h"
#include "typedefine.h"

#ifdef KERNEL_EXPORTS
#define KERNEL_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define KERNEL_EXPORT __declspec(dllimport)
#else
#define KERNEL_EXPORT 
#endif
#endif

class KERNEL_EXPORT CBF_Tools
{
public:
	// 函数名: HEX_2_DSP
	// 编程  : 王明松 2012-4-11 13:31:26
	// 返回  : void 
	// 输入参数  : const char *hex  数据
	// 输出参数  : char *dsp  可显示的内容  1-9,A-F
	// 输入参数  : int count hex的长度
	// 描述  : 将数据转换为16进制显示，转换后的数据长度为count*2
	static void HEX_2_DSP(const char *hex,char *dsp,int count);
	
	// 函数名: DSP_2_HEX
	// 编程  : 王明松 2012-4-11 13:33:10
	// 返回  : void 
	// 输入参数  : const char *dsp  可显示的内容  1-9,A-F
	// 输出参数  : char *hex  实际数据
	// 输入参数  : int count  dsp长度
	// 描述  : 将16进制显示的数据转换为实际数据,转换后的数据长度为count/2
	static void DSP_2_HEX(const char *dsp,char *hex,int count);

	// 函数名: StringCopy
	// 编程  : 王明松 2012-2-27 11:29:36
	// 返回  : static bool 
	// 参数  : char *dest
	// 参数  : unsigned int destlen  存放字符的长度，不是sizeof长度
	// 参数  : const char *src
	// 描述  : 字符串copy
	static bool StringCopy(char *dest,unsigned int destlen,const char *src);

	// 函数名: IsNumber
	// 编程  : 王明松 2012-4-26 15:21:02
	// 返回  : static bool 
	// 参数  : const char *str
	// 描述  : 判断字符串是否全部由0-9数字组成
	static bool IsNumber(const char *str);

	// 函数名: RemoveOverdueFile
	// 编程  : 王明松 2012-4-25 10:33:27
	// 返回  : static int 删除的目录数或文件数
	// 参数  : const char *delpath  全路径，路径最后没有'/'和'\\'
	// 参数  : const char *fileattr  路径下的要删除文件属性
	// 参数  : int overduedays      过期天数
	// 参数  : bool isext=true      true文件属性是扩展名，false文件属性是开头的字符串
	// 描述  : 删除指定目录下过期的指定扩展名或以XX开头的文件
	static int RemoveOverdueFile(const char *delpath,const char *fileattr,int overduedays,bool isext=true);

	// 函数名: MakeDir
	// 编程  : 王明松 2012-4-25 9:56:05
	// 返回  : static bool 
	// 参数  : const char *path 目录
	// 描述  : 建立目录，如果前级目录没有也一起建
	static bool MakeDir(const char *path);

	// 函数名: Uncompress
	// 编程  : 王明松 2012-4-20 9:32:29
	// 返回  : static bool 
	// 输出参数  : unsigned char* destbuf 解开后的数据
	// 输入输出参数  : unsigned int * destlen 输入为destbuf的大小，输出为解开后的数据长度
	// 输入参数  : unsigned char* srbuf  压缩数据
	// 输入参数  : int srbuflen  压缩数据长度
	// 描述  : 解压缩数据
	static bool Uncompress(unsigned char *destbuf, unsigned int &destlen, unsigned char *srbuf, int srbuflen);

	// 函数名: Compress
	// 编程  : 王明松 2012-4-20 9:32:29
	// 返回  : static bool 
	// 输出参数  : unsigned char * destbuf  压缩后的数据
	// 输入输出参数  : unsigned int * destlen  输入为destbuf的大小，输出为压缩后的数据长度
	// 输入参数  : unsigned char * srbuf   要压缩的数据
	// 输入参数  : int srbuflen 要压缩的数据长度
	// 描述  : 压缩数据
	static bool Compress(unsigned char *destbuf,unsigned int &destlen,unsigned char *srbuf,int srbuflen);

	// 函数名: GetModuleNamePath
	// 编程  : 王明松 2012-4-20 9:32:29
	// 返回  : static void 
	// 参数  : char *argv  main函数传过来的argv
	// 参数  : char *curpath   当前目录(工作目录)
	// 参数  : char *prgpath    程序路径
	// 参数  : char *prgname     程序名称
	// 描述  : 获取主程序运行时的当前目录(工作目录),程序路径，程序名称
	static void GetModuleNamePath(char *argv[],char *curpath,char *prgpath,char *prgname);

	// 函数名: getopt
	// 编程  : 王明松 2012-4-19 14:34:27
	// 返回  : static int 
	// 参数  : int argc
	// 参数  : char *argv[]
	// 参数  : char *optstring
	// 描述  : 实现windows下的getopt函数
	static int Getopt(int argc, char *argv[], char *optstring);

	// 函数名: Reverse
	// 编程  : 王明松 2012-4-19 11:50:46
	// 返回  : static void  
	// 参数  : unsigned char *buf 
	// 参数  : int len
	// 描述  : 判断本机字节序是否为网络字节序BE，若是则进行相反处理为LE，否则不用处理
	static void Reverse(unsigned char *buf,int len);

	// 函数名: CRC
	// 编程  : 王明松 2012-4-19 11:26:05
	// 返回  : unsigned short 校验位
	// 参数  : void *pdata  数据
	// 参数  : int datalen  数据长度
	// 描述  : 生成数据的CRC校验
	static unsigned short CRC(void *pdata,int datalen);

	// 函数名: GetConsoleEnter
	// 编程  : 王明松 2012-4-19 10:10:26
	// 返回  : static int 输入的字符串长度
	// 参数  : const char *prompt  提示信息
	// 参数  : unsigned int getlen  要接收的字符串长度(最大)
	// 参数  : char *getstr      输入的字符串
	// 参数  : bool ispwd=false  是密码的话不回显
	// 描述  : 在控制台上提示输入信息并接收输入指定字符的字符串，以回车结束
	static int GetConsoleEnter(const char *prompt,unsigned int getlen,char *getstr,bool ispwd=false);

	// 函数名: getch
	// 编程  : 王明松 2012-4-19 10:05:07
	// 返回  : static int >0输入的字符   <0出错
	// 描述  : 从控制台接受一个字符的输入
	static int Getch();

	// 函数名: StringToInteger
	// 编程  : 王明松 2012-4-18 15:09:03
	// 返回  : static INT64_ 
	// 参数  : std::string data  指定进制的整型字符串
	// 参数  : unsigned int jz  进制
	// 描述  : 将指定进制的整型字符串转换为10进制整数
	static INT64_ StringToInteger(std::string data,unsigned int jz);

	// 函数名: ToBinaryString
	// 编程  : 王明松 2012-4-18 14:57:42
	// 返回  : void 
	// 参数  : unsigned long data
	// 参数  : char *str
	// 描述  : 将10进制正整数转换为二进制字符串
	static void ToBinaryString(unsigned long data,char *str);

	// 函数名: Round
	// 编程  : 王明松 2012-4-17 16:57:47
	// 返回  : static double 
	// 参数  : double dd
	// 参数  : unsigned int dec 小数位数
	// 描述  : 四舍五入方法
	static double Round(double dd,unsigned int dec);

	// 函数名: & LRtrim
	// 编程  : 王明松 2012-4-17 16:29:13
	// 返回  : static std::string 
	// 参数  : std::string &str
	// 描述  : 去除左右空格
	static std::string & LRtrim(std::string &str);

	// 函数名: & Rtrim
	// 编程  : 王明松 2012-4-17 16:29:08
	// 返回  : static std::string 
	// 参数  : std::string &str
	// 描述  : 去除右空格
	static std::string & Rtrim(std::string &str);

	// 函数名: & Ltrim
	// 编程  : 王明松 2012-4-17 16:29:04
	// 返回  : static std::string 
	// 参数  : std::string &str
	// 描述  : 去除左空格
	static std::string & Ltrim(std::string &str);

	// 函数名: * LRtrim
	// 编程  : 王明松 2012-4-17 15:48:57
	// 返回  : static char 
	// 参数  : char *str
	// 描述  : 去除左右空格
	static char * LRtrim(char *str);

	// 函数名: * Rtrim
	// 编程  : 王明松 2012-4-17 15:48:05
	// 返回  : static char 
	// 参数  : char *str
	// 描述  : 去除右边空格
	static char * Rtrim(char *str);

	// 函数名: * Ltrim
	// 编程  : 王明松 2012-4-17 15:47:51
	// 返回  : static char 
	// 参数  : char *str
	// 描述  : 去除左边空格
	static char * Ltrim(char *str);

	// 函数名: ToLower
	// 编程  : 王明松 2012-4-17 15:47:30
	// 返回  : static void 
	// 参数  : char *upperchar
	// 描述  : 转换为小写字符
	static void ToLower(char *upperchar);

	// 函数名: ToUpper
	// 编程  : 王明松 2012-4-17 15:47:13
	// 返回  : static void 
	// 参数  : char *lowerchar
	// 描述  : 转换为大写字符
	static void ToUpper(char *lowerchar);

	static int gbk2utf8(char* desStr, const char* srcStr, int desStrsize);

	CBF_Tools();
	virtual ~CBF_Tools();
	
private:

	// 函数名: IsNumberData
	// 编程  : 王明松 2012-4-18 15:14:55
	// 返回  : static bool 
	// 参数  : char *c  字符
	// 参数  : unsigned int jz 进制  二进制 8进制 10进制
	// 描述  : 判断字符是否是指定进制的字符
	static bool IsNumberData(char *c,unsigned int jz);

	static unsigned int  m_sReverseMode;//本机字节序 0未知 1 BE   2LE
};

extern "C"
{
	//3des加密 密码明文，返回密文	 0成功 其它失败
	KERNEL_EXPORT int bf_Des3Buffer(char* pwdbuffer, int pwdbuffersize, const char* des3key);
	//des加密 密码明文，返回密文	 0成功 其它失败
	KERNEL_EXPORT int bf_DesBuffer(char* pwdbuffer, int pwdlen, const char* des3key);
	//3des解密 密码密文，返回明文	 0成功 其它失败
	KERNEL_EXPORT int bf_Des3UncryptBuffer(char* pwdbuffer, int pwdlen, const char* des3key);
	//des解密 密码密文，返回明文	 0成功 其它失败
	KERNEL_EXPORT int bf_DesUncryptBuffer(char* pwdbuffer, int pwdlen, const char* deskey);
}
#endif // !defined(AFX_BF_TOOLS_H__42367B68_92BE_4995_83EA_475E113120FA__INCLUDED_)
