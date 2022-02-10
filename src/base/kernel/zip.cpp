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


	// 函数名: BF_Compress
	// 编程  : 王明松 2012-4-20 16:18:10
	// 返回  : bool  true：成功，false压缩失败。
	// 参数  : unsigned char * destbuf 压缩后的数据，
	// 参数  : unsigned int * destlen  压缩前为destbuf的大小，必须大于srbuflen，压缩后为压缩后的长度
	// 参数  : unsigned char * srbuf   压缩前的数据  
	// 参数  : int srbuflen            srbuf中数据的长度
	// 描述  : 压缩数据
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

// 函数名: BF_Uncompress
// 编程  : 王明松 2012-4-20 16:18:15
// 返回  : bool  true：成功，false解压缩失败。
// 参数  : unsigned char* destbuf   解压缩后的数据
// 参数  : unsigned int * destlen   解压缩前为destbuf的大小，必须大于srbuflen，解压缩后为解压缩的长度
// 参数  : unsigned char* srbuf     解压缩前的数据
// 参数  : int srbuflen             srbuf中数据的长度
// 描述  : 解压缩数据
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

