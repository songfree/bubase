#include "public.h"
#include "ShcaApi.h"

int main(int argc,char *argv[])
{
	printf("�ڵ�ǰĿ¼��Ҫ��shcaĿ¼��֤���ļ����ڴ�Ŀ¼�� ˽ԿUserKey.key  ��ԿUserCert.der ֤����RootCert.der��֤������̶�Ϊ123412\n");

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
			printf("��ʼ��ʧ�ܣ�%s \n",errmsg);
		}
		else
		{
			printf("��ʼ��ʧ�ܣ�ʵ������ \n");
		}
		return -1;
	}
	strcpy(tmpchar,"123456789");
	unsigned short len = strlen(tmpchar);
	unsigned short dlen = sizeof(reschar);
	if (SHCAAPI_Sign(apiindex,tmpchar,len,reschar,&dlen) != 0)
	{
		SHCAAPI_GetErrMsg(apiindex,errmsg);
		printf("ǩ��ʧ�� %s \n",errmsg);
		return -1;
	}

	unsigned short  pubkeylen = sizeof(pubkey);
	unsigned short  pubkeylen2 = sizeof(pubkey2);
	if (SHCAAPI_GetPubicKey(apiindex,pubkey,&pubkeylen,pubkey2,&pubkeylen2) != 0)
	{
		SHCAAPI_GetErrMsg(apiindex,errmsg);
		printf("ȡ��Կʧ�� %s \n",errmsg);
		return -1;
	}
	memcpy(reschar2,reschar,dlen);
	unsigned short dlen2 = dlen;
	
	if (SHCAAPI_Encrypt(apiindex,pubkey,pubkeylen,reschar2,&dlen2) != 0)
	{
		SHCAAPI_GetErrMsg(apiindex,errmsg);
		printf("����ʧ�� %s \n",errmsg);
		return -1;
	}
	
	if (SHCAAPI_UnEncrypt(apiindex,reschar2,&dlen2) !=0 )
	{
		SHCAAPI_GetErrMsg(apiindex,errmsg);
		printf("����ʧ�� %s \n",errmsg);
		return -1;
	}
	if (dlen2 != dlen ||  memcmp(reschar,reschar2,dlen2) !=0 )
	{
		printf("���ܽ������ \n");
		return -1;
	}
	reschar2[dlen2] = 0;
	if (SHCAAPI_CheckSign(apiindex,pubkey,pubkeylen,tmpchar,len,reschar2,dlen2) !=0 )
	{
		SHCAAPI_GetErrMsg(apiindex,errmsg);
		printf("ǩ����֤ʧ�� %s \n",errmsg);
		return -1;
	}
	printf("����C API�ɹ�!\n");




	CShcaApi  api;
	if (!api.InitCertApi("./shca","123412"))
//	if (!api.InitCertApi("./cfca","111111"))
	{
		printf("��ʼ��ʧ�ܣ�%s \n",api.GetErrMsg().c_str());
		return -1;
	}
	
	strcpy(tmpchar,"123456789");
	len = strlen(tmpchar);
	dlen = sizeof(reschar);
	if (!api.Sign(tmpchar,len,reschar,dlen))
	{
		printf("ǩ��ʧ�� %s \n",api.GetErrMsg().c_str());
		return -1;
	}
	
	
	pubkeylen = sizeof(pubkey);
	pubkeylen2 = sizeof(pubkey2);
	if (!api.GetPubicKey(pubkey,pubkeylen,pubkey2,pubkeylen2))
	{
		printf("ȡ��Կʧ�� %s \n",api.GetErrMsg().c_str());
		return -1;
	}
	
	memcpy(reschar2,reschar,dlen);
	dlen2 = dlen;

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