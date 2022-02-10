#include "public.h"
#include "minilzo.h"
#include <stdlib.h>
#include "BF_Zip.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define HEAP_ALLOC(var,size) long __LZO_MMODEL var [ ((size) + (sizeof(long) - 1)) / sizeof(long) ]

static HEAP_ALLOC(wrkmem,LZO1X_1_MEM_COMPRESS);


	// ������: BF_Compress
	// ���  : ������ 2012-4-20 16:18:10
	// ����  : bool  true���ɹ���falseѹ��ʧ�ܡ�
	// ����  : unsigned char * destbuf ѹ��������ݣ�
	// ����  : unsigned int * destlen  ѹ��ǰΪdestbuf�Ĵ�С���������srbuflen��ѹ����Ϊѹ����ĳ���
	// ����  : unsigned char * srbuf   ѹ��ǰ������  
	// ����  : int srbuflen            srbuf�����ݵĳ���
	// ����  : ѹ������
bool BF_Compress(unsigned char * destbuf,unsigned int * destlen,unsigned char * srbuf,int srbuflen)
{
	if(lzo_init() == LZO_E_OK) 
	{
		lzo_memset(destbuf,0,srbuflen);
		if(lzo1x_1_compress(srbuf,srbuflen,destbuf,destlen,wrkmem)==LZO_E_OK)
		{
			return true;
		}
	}
	return false;
}

// ������: BF_Uncompress
// ���  : ������ 2012-4-20 16:18:15
// ����  : bool  true���ɹ���false��ѹ��ʧ�ܡ�
// ����  : unsigned char* destbuf   ��ѹ���������
// ����  : unsigned int * destlen   ��ѹ��ǰΪdestbuf�Ĵ�С���������srbuflen����ѹ����Ϊ��ѹ���ĳ���
// ����  : unsigned char* srbuf     ��ѹ��ǰ������
// ����  : int srbuflen             srbuf�����ݵĳ���
// ����  : ��ѹ������
bool  BF_Uncompress(unsigned char* destbuf, unsigned int * destlen, unsigned char* srbuf, int srbuflen)
{
	if (lzo_init() == LZO_E_OK)
	{
		lzo_memset(destbuf,0,srbuflen);
		if(lzo1x_decompress_safe(srbuf,srbuflen,destbuf,destlen,NULL)==LZO_E_OK)
		{
			return true;
		}
	}
	return false;
}

