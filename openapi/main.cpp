#include "public.h"
#include "OpenApi.h"

int main(int argc,char *argv[])
{
	printf("在当前目录下要有cfca目录，证书文件放在此目录下，证书密码固定为8个1\n");
	COpenApi  api;

	if (!api.InitCertApi("./cfca","123456"))
	{
		printf("初始化失败：%s \n",api.GetErrMsg().c_str());
		return -1;
	}
	char tmpchar[4096];
	char reschar[8192];
	char reschar2[8192];
	bzero(tmpchar,sizeof(tmpchar));
	bzero(reschar,sizeof(reschar));
	bzero(reschar2,sizeof(reschar2));
	strcpy(tmpchar,"123456789");
	unsigned short len = strlen(tmpchar);
	unsigned short dlen = sizeof(reschar);
	if (!api.Sign(tmpchar,len,reschar,dlen))
	{
		printf("签名失败 %s \n",api.GetErrMsg().c_str());
		return -1;
	}
	
	char pubkey[8192];
	char pubkey2[8192];
	bzero(pubkey,sizeof(pubkey));
	bzero(pubkey2,sizeof(pubkey2));
	unsigned short  pubkeylen = sizeof(pubkey);
	unsigned short  pubkeylen2 = sizeof(pubkey2);
	if (!api.GetPubicKey(pubkey,pubkeylen,pubkey2,pubkeylen2))
	{
		printf("取公钥失败 %s \n",api.GetErrMsg().c_str());
		return -1;
	}
	
	memcpy(reschar2,reschar,dlen);
	unsigned short dlen2 = dlen;

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