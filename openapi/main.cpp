#include "public.h"
#include "OpenApi.h"

int main(int argc,char *argv[])
{
	printf("�ڵ�ǰĿ¼��Ҫ��cfcaĿ¼��֤���ļ����ڴ�Ŀ¼�£�֤������̶�Ϊ8��1\n");
	COpenApi  api;

	if (!api.InitCertApi("./cfca","123456"))
	{
		printf("��ʼ��ʧ�ܣ�%s \n",api.GetErrMsg().c_str());
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
		printf("ǩ��ʧ�� %s \n",api.GetErrMsg().c_str());
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
		printf("ȡ��Կʧ�� %s \n",api.GetErrMsg().c_str());
		return -1;
	}
	
	memcpy(reschar2,reschar,dlen);
	unsigned short dlen2 = dlen;

	if (!api.Encrypt(pubkey,pubkeylen,reschar2,dlen2))
	{
		printf("����ʧ�� %s \n",api.GetErrMsg().c_str());
		return -1;
	}

	if (!api.UnEncrypt(reschar2,dlen2))
	{
		printf("����ʧ�� %s \n",api.GetErrMsg().c_str());
		return -1;
	}
	if (dlen2 != dlen ||  memcmp(reschar,reschar2,dlen2) !=0 )
	{
		printf("���ܽ������ \n");
		return -1;
	}
	reschar2[dlen2] = 0;
	if (!api.CheckSign(pubkey,pubkeylen,tmpchar,len,reschar2,dlen2))
	{
		printf("ǩ����֤ʧ�� %s \n",api.GetErrMsg().c_str());
		return -1;
	}
	printf("���Գɹ�!\n");
	return 0;
}