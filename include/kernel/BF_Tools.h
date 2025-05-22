// BF_Tools.h: interface for the CBF_Tools class.
//
//////////////////////////////////////////////////////////////////////

/***********************************************************************************
  ������־��¼
  ������: ������
  �������� 2012-4-17 15:42:23 �ع�
  �汾: V1.00 
  ˵��: һЩ���õĺ���
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
	// ������: HEX_2_DSP
	// ���  : ������ 2012-4-11 13:31:26
	// ����  : void 
	// �������  : const char *hex  ����
	// �������  : char *dsp  ����ʾ������  1-9,A-F
	// �������  : int count hex�ĳ���
	// ����  : ������ת��Ϊ16������ʾ��ת��������ݳ���Ϊcount*2
	static void HEX_2_DSP(const char *hex,char *dsp,int count);
	
	// ������: DSP_2_HEX
	// ���  : ������ 2012-4-11 13:33:10
	// ����  : void 
	// �������  : const char *dsp  ����ʾ������  1-9,A-F
	// �������  : char *hex  ʵ������
	// �������  : int count  dsp����
	// ����  : ��16������ʾ������ת��Ϊʵ������,ת��������ݳ���Ϊcount/2
	static void DSP_2_HEX(const char *dsp,char *hex,int count);

	// ������: StringCopy
	// ���  : ������ 2012-2-27 11:29:36
	// ����  : static bool 
	// ����  : char *dest
	// ����  : unsigned int destlen  ����ַ��ĳ��ȣ�����sizeof����
	// ����  : const char *src
	// ����  : �ַ���copy
	static bool StringCopy(char *dest,unsigned int destlen,const char *src);

	// ������: IsNumber
	// ���  : ������ 2012-4-26 15:21:02
	// ����  : static bool 
	// ����  : const char *str
	// ����  : �ж��ַ����Ƿ�ȫ����0-9�������
	static bool IsNumber(const char *str);

	// ������: RemoveOverdueFile
	// ���  : ������ 2012-4-25 10:33:27
	// ����  : static int ɾ����Ŀ¼�����ļ���
	// ����  : const char *delpath  ȫ·����·�����û��'/'��'\\'
	// ����  : const char *fileattr  ·���µ�Ҫɾ���ļ�����
	// ����  : int overduedays      ��������
	// ����  : bool isext=true      true�ļ���������չ����false�ļ������ǿ�ͷ���ַ���
	// ����  : ɾ��ָ��Ŀ¼�¹��ڵ�ָ����չ������XX��ͷ���ļ�
	static int RemoveOverdueFile(const char *delpath,const char *fileattr,int overduedays,bool isext=true);

	// ������: MakeDir
	// ���  : ������ 2012-4-25 9:56:05
	// ����  : static bool 
	// ����  : const char *path Ŀ¼
	// ����  : ����Ŀ¼�����ǰ��Ŀ¼û��Ҳһ��
	static bool MakeDir(const char *path);

	// ������: Uncompress
	// ���  : ������ 2012-4-20 9:32:29
	// ����  : static bool 
	// �������  : unsigned char* destbuf �⿪�������
	// �����������  : unsigned int * destlen ����Ϊdestbuf�Ĵ�С�����Ϊ�⿪������ݳ���
	// �������  : unsigned char* srbuf  ѹ������
	// �������  : int srbuflen  ѹ�����ݳ���
	// ����  : ��ѹ������
	static bool Uncompress(unsigned char *destbuf, unsigned int &destlen, unsigned char *srbuf, int srbuflen);

	// ������: Compress
	// ���  : ������ 2012-4-20 9:32:29
	// ����  : static bool 
	// �������  : unsigned char * destbuf  ѹ���������
	// �����������  : unsigned int * destlen  ����Ϊdestbuf�Ĵ�С�����Ϊѹ��������ݳ���
	// �������  : unsigned char * srbuf   Ҫѹ��������
	// �������  : int srbuflen Ҫѹ�������ݳ���
	// ����  : ѹ������
	static bool Compress(unsigned char *destbuf,unsigned int &destlen,unsigned char *srbuf,int srbuflen);

	// ������: GetModuleNamePath
	// ���  : ������ 2012-4-20 9:32:29
	// ����  : static void 
	// ����  : char *argv  main������������argv
	// ����  : char *curpath   ��ǰĿ¼(����Ŀ¼)
	// ����  : char *prgpath    ����·��
	// ����  : char *prgname     ��������
	// ����  : ��ȡ����������ʱ�ĵ�ǰĿ¼(����Ŀ¼),����·������������
	static void GetModuleNamePath(char *argv[],char *curpath,char *prgpath,char *prgname);

	// ������: getopt
	// ���  : ������ 2012-4-19 14:34:27
	// ����  : static int 
	// ����  : int argc
	// ����  : char *argv[]
	// ����  : char *optstring
	// ����  : ʵ��windows�µ�getopt����
	static int Getopt(int argc, char *argv[], char *optstring);

	// ������: Reverse
	// ���  : ������ 2012-4-19 11:50:46
	// ����  : static void  
	// ����  : unsigned char *buf 
	// ����  : int len
	// ����  : �жϱ����ֽ����Ƿ�Ϊ�����ֽ���BE������������෴����ΪLE�������ô���
	static void Reverse(unsigned char *buf,int len);

	// ������: CRC
	// ���  : ������ 2012-4-19 11:26:05
	// ����  : unsigned short У��λ
	// ����  : void *pdata  ����
	// ����  : int datalen  ���ݳ���
	// ����  : �������ݵ�CRCУ��
	static unsigned short CRC(void *pdata,int datalen);

	// ������: GetConsoleEnter
	// ���  : ������ 2012-4-19 10:10:26
	// ����  : static int ������ַ�������
	// ����  : const char *prompt  ��ʾ��Ϣ
	// ����  : unsigned int getlen  Ҫ���յ��ַ�������(���)
	// ����  : char *getstr      ������ַ���
	// ����  : bool ispwd=false  ������Ļ�������
	// ����  : �ڿ���̨����ʾ������Ϣ����������ָ���ַ����ַ������Իس�����
	static int GetConsoleEnter(const char *prompt,unsigned int getlen,char *getstr,bool ispwd=false);

	// ������: getch
	// ���  : ������ 2012-4-19 10:05:07
	// ����  : static int >0������ַ�   <0����
	// ����  : �ӿ���̨����һ���ַ�������
	static int Getch();

	// ������: StringToInteger
	// ���  : ������ 2012-4-18 15:09:03
	// ����  : static INT64_ 
	// ����  : std::string data  ָ�����Ƶ������ַ���
	// ����  : unsigned int jz  ����
	// ����  : ��ָ�����Ƶ������ַ���ת��Ϊ10��������
	static INT64_ StringToInteger(std::string data,unsigned int jz);

	// ������: ToBinaryString
	// ���  : ������ 2012-4-18 14:57:42
	// ����  : void 
	// ����  : unsigned long data
	// ����  : char *str
	// ����  : ��10����������ת��Ϊ�������ַ���
	static void ToBinaryString(unsigned long data,char *str);

	// ������: Round
	// ���  : ������ 2012-4-17 16:57:47
	// ����  : static double 
	// ����  : double dd
	// ����  : unsigned int dec С��λ��
	// ����  : �������뷽��
	static double Round(double dd,unsigned int dec);

	// ������: & LRtrim
	// ���  : ������ 2012-4-17 16:29:13
	// ����  : static std::string 
	// ����  : std::string &str
	// ����  : ȥ�����ҿո�
	static std::string & LRtrim(std::string &str);

	// ������: & Rtrim
	// ���  : ������ 2012-4-17 16:29:08
	// ����  : static std::string 
	// ����  : std::string &str
	// ����  : ȥ���ҿո�
	static std::string & Rtrim(std::string &str);

	// ������: & Ltrim
	// ���  : ������ 2012-4-17 16:29:04
	// ����  : static std::string 
	// ����  : std::string &str
	// ����  : ȥ����ո�
	static std::string & Ltrim(std::string &str);

	// ������: * LRtrim
	// ���  : ������ 2012-4-17 15:48:57
	// ����  : static char 
	// ����  : char *str
	// ����  : ȥ�����ҿո�
	static char * LRtrim(char *str);

	// ������: * Rtrim
	// ���  : ������ 2012-4-17 15:48:05
	// ����  : static char 
	// ����  : char *str
	// ����  : ȥ���ұ߿ո�
	static char * Rtrim(char *str);

	// ������: * Ltrim
	// ���  : ������ 2012-4-17 15:47:51
	// ����  : static char 
	// ����  : char *str
	// ����  : ȥ����߿ո�
	static char * Ltrim(char *str);

	// ������: ToLower
	// ���  : ������ 2012-4-17 15:47:30
	// ����  : static void 
	// ����  : char *upperchar
	// ����  : ת��ΪСд�ַ�
	static void ToLower(char *upperchar);

	// ������: ToUpper
	// ���  : ������ 2012-4-17 15:47:13
	// ����  : static void 
	// ����  : char *lowerchar
	// ����  : ת��Ϊ��д�ַ�
	static void ToUpper(char *lowerchar);

	static int gbk2utf8(char* desStr, const char* srcStr, int desStrsize);

	CBF_Tools();
	virtual ~CBF_Tools();
	
private:

	// ������: IsNumberData
	// ���  : ������ 2012-4-18 15:14:55
	// ����  : static bool 
	// ����  : char *c  �ַ�
	// ����  : unsigned int jz ����  ������ 8���� 10����
	// ����  : �ж��ַ��Ƿ���ָ�����Ƶ��ַ�
	static bool IsNumberData(char *c,unsigned int jz);

	static unsigned int  m_sReverseMode;//�����ֽ��� 0δ֪ 1 BE   2LE
};

extern "C"
{
	//3des���� �������ģ���������	 0�ɹ� ����ʧ��
	KERNEL_EXPORT int bf_Des3Buffer(char* pwdbuffer, int pwdbuffersize, const char* des3key);
	//des���� �������ģ���������	 0�ɹ� ����ʧ��
	KERNEL_EXPORT int bf_DesBuffer(char* pwdbuffer, int pwdlen, const char* des3key);
	//3des���� �������ģ���������	 0�ɹ� ����ʧ��
	KERNEL_EXPORT int bf_Des3UncryptBuffer(char* pwdbuffer, int pwdlen, const char* des3key);
	//des���� �������ģ���������	 0�ɹ� ����ʧ��
	KERNEL_EXPORT int bf_DesUncryptBuffer(char* pwdbuffer, int pwdlen, const char* deskey);
}
#endif // !defined(AFX_BF_TOOLS_H__42367B68_92BE_4995_83EA_475E113120FA__INCLUDED_)
