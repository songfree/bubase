#include "public.h"
#include "ShcaApi.h"

int main(int argc,char *argv[])
{
	printf("在当前目录下要有shca目录，证书文件放在此目录下 私钥UserKey.key  公钥UserCert.der 证书链RootCert.der，证书密码固定为123412\n");

	int apiindex;
	char errmsg[256];
	char tmpchar[4096];
	char reschar[8192];
	char reschar2[8192];
	char pubkey[8192];
	char pubkey2[8192];
	bzero(pubkey,sizeof(pubkey));
	bzero(pubkey2,sizeof(pubkey2));
	bzero(tmpchar,sizeof(tmpchar));
	bzero(reschar,sizeof(reschar));
	bzero(reschar2,sizeof(reschar2));

	bzero(errmsg,sizeof(errmsg));
	int ret = SHCAAPI_InitCertApi("./shca","123412",&apiindex);
	if (ret <0)
	{
		if (ret == -2)
		{
			SHCAAPI_GetErrMsg(apiindex,errmsg);
			printf("初始化失败：%s \n",errmsg);
		}
		else
		{
			printf("初始化失败：实例出错 \n");
		}
		return -1;
	}
	strcpy(tmpchar,"123456789");
	unsigned short len = strlen(tmpchar);
	unsigned short dlen = sizeof(reschar);
	if (SHCAAPI_Sign(apiindex,tmpchar,len,reschar,&dlen) != 0)
	{
		SHCAAPI_GetErrMsg(apiindex,errmsg);
		printf("签名失败 %s \n",errmsg);
		return -1;
	}

	unsigned short  pubkeylen = sizeof(pubkey);
	unsigned short  pubkeylen2 = sizeof(pubkey2);
	if (SHCAAPI_GetPubicKey(apiindex,pubkey,&pubkeylen,pubkey2,&pubkeylen2) != 0)
	{
		SHCAAPI_GetErrMsg(apiindex,errmsg);
		printf("取公钥失败 %s \n",errmsg);
		return -1;
	}
	memcpy(reschar2,reschar,dlen);
	unsigned short dlen2 = dlen;
	
	if (SHCAAPI_Encrypt(apiindex,pubkey,pubkeylen,reschar2,&dlen2) != 0)
	{
		SHCAAPI_GetErrMsg(apiindex,errmsg);
		printf("加密失败 %s \n",errmsg);
		return -1;
	}
	
	if (SHCAAPI_UnEncrypt(apiindex,reschar2,&dlen2) !=0 )
	{
		SHCAAPI_GetErrMsg(apiindex,errmsg);
		printf("解密失败 %s \n",errmsg);
		return -1;
	}
	if (dlen2 != dlen ||  memcmp(reschar,reschar2,dlen2) !=0 )
	{
		printf("解密结果不符 \n");
		return -1;
	}
	reschar2[dlen2] = 0;
	if (SHCAAPI_CheckSign(apiindex,pubkey,pubkeylen,tmpchar,len,reschar2,dlen2) !=0 )
	{
		SHCAAPI_GetErrMsg(apiindex,errmsg);
		printf("签名验证失败 %s \n",errmsg);
		return -1;
	}
	printf("测试C API成功!\n");




	CShcaApi  api;
	if (!api.InitCertApi("./shca","123412"))
//	if (!api.InitCertApi("./cfca","111111"))
	{
		printf("初始化失败：%s \n",api.GetErrMsg().c_str());
		return -1;
	}
	
	strcpy(tmpchar,"123456789");
	len = strlen(tmpchar);
	dlen = sizeof(reschar);
	if (!api.Sign(tmpchar,len,reschar,dlen))
	{
		printf("签名失败 %s \n",api.GetErrMsg().c_str());
		return -1;
	}
	
	
	pubkeylen = sizeof(pubkey);
	pubkeylen2 = sizeof(pubkey2);
	if (!api.GetPubicKey(pubkey,pubkeylen,pubkey2,pubkeylen2))
	{
		printf("取公钥失败 %s \n",api.GetErrMsg().c_str());
		return -1;
	}
	
	memcpy(reschar2,reschar,dlen);
	dlen2 = dlen;

	if (!api.Encrypt(pubkey,pubkeylen,reschar2,dlen2))
	{
		printf("加密失败 %s \n",api.GetErrMsg().c_str());
		return -1;
	}

	if (!api.UnEncrypt(reschar2,dlen2))
	{
		printf("解密失败 %s \n",api.GetErrMsg().c_str());
		return -1;
	}
	if (dlen2 != dlen ||  memcmp(reschar,reschar2,dlen2) !=0 )
	{
		printf("解密结果不符 \n");
		return -1;
	}
	reschar2[dlen2] = 0;
	if (!api.CheckSign(pubkey,pubkeylen,tmpchar,len,reschar2,dlen2))
	{
		printf("签名验证失败 %s \n",api.GetErrMsg().c_str());
		return -1;
	}
	printf("测试成功!\n");
	return 0;
}