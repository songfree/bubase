// BF_3Des.cpp: implementation of the CBF_3Des class.
//
//////////////////////////////////////////////////////////////////////

#include "BF_3Des.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBF_3Des::CBF_3Des()
{

}

CBF_3Des::~CBF_3Des()
{

}

bool CBF_3Des::Des3Buffer(char *buffer, int bufsize, const char *key,char *errmsg)
{
	if (strlen(key)!=24)
	{
		sprintf(errmsg,"��Կ����[%d]����,Ӧ�̶�Ϊ24����",strlen(key));
		return false;
	}
	unsigned char key1[9];
	unsigned char key2[9];
	unsigned char key3[9];
	bzero(key1,sizeof(key1));
	bzero(key2,sizeof(key2));
	bzero(key3,sizeof(key3));
	//24�����ȣ��ֳ�3��key
	memcpy(key1,key,8);
	memcpy(key2,key+8,8);
	memcpy(key3,key+16,8);


	int buflen  = strlen(buffer);
	int nRet = buflen % 8;
	int addlen = 8-nRet;
	if (nRet != 0)
	{
		nRet = buflen/8+1;
		buflen = buflen+addlen;
		if (bufsize < buflen)
		{
			sprintf(errmsg,"�����С[%d]����,Ӧ����Ϊ%d����",bufsize,buflen);
			return false;
		}
		bzero(buffer+buflen,addlen);
		
	}
	else
	{
		nRet = buflen/8;
	}
	
	if (bufsize < nRet*16+1)
	{
		sprintf(errmsg,"�����С[%d]����,Ӧ����Ϊ%d����",bufsize,nRet*16+1);
		return false;
	}
	char *destbuffer= (char *)malloc(nRet*16+1);
	if (destbuffer == NULL)
	{
		sprintf(errmsg,"����ռ����");
		return false;
	}
	bzero(destbuffer,nRet*16+1);
	//��8�ı����ַ����Ƶ�dest����
	memcpy(destbuffer,buffer,buflen);


	unsigned char tmpchar[17];
	unsigned char tmpchar2[17];
	bzero(tmpchar,sizeof(tmpchar));
	bzero(tmpchar2,sizeof(tmpchar2));
	for (int i=0; i< nRet ;i++)
	{
		//�õ�һ��key����
		memcpy(tmpchar,destbuffer+i*8,8);
		m_pDes.DES(tmpchar,tmpchar2,key1,0);
//		printf("DES1 [%s]\n",tmpchar2);
		//�õڶ���key����
		bzero(tmpchar,sizeof(tmpchar));
		m_pDes.DES(tmpchar2,tmpchar,key2,1);
//		printf("DES2 [%s]\n",tmpchar);
		//�õ�����key����
		bzero(buffer+i*16,16);
		m_pDes.DES(tmpchar,(unsigned char *)(buffer+i*16),key3,0);
//		printf("DES3 [%s]",buffer+i*16);
	}
	buffer[nRet*16]=0;
	free(destbuffer);
	destbuffer = NULL;
	return true;
}

bool CBF_3Des::UnDes3Buffer(char *buffer, int buflen, const char *key,char *errmsg)
{
	if (strlen(key)!=24)
	{
		sprintf(errmsg,"��Կ����[%d]����,Ӧ�̶�Ϊ24����",strlen(key));
		return false;
	}
	if (buflen % 16 !=0)
	{
		sprintf(errmsg,"���ĳ��Ȳ�Ϊ16����");
		return false;
	}
	unsigned char key1[9];
	unsigned char key2[9];
	unsigned char key3[9];
	bzero(key1,sizeof(key1));
	bzero(key2,sizeof(key2));
	bzero(key3,sizeof(key3));
	//24�����ȣ��ֳ�3��key
	memcpy(key1,key,8);
	memcpy(key2,key+8,8);
	memcpy(key3,key+16,8);

	int nRet = buflen /16;
	char *destbuffer= (char *)malloc(buflen+1);
	if (destbuffer == NULL)
	{
		sprintf(errmsg,"����ռ����");
		return false;
	}
	bzero(destbuffer,buflen+1);

	unsigned char tmpchar[17];
	unsigned char tmpchar2[17];
	bzero(tmpchar,sizeof(tmpchar));
	bzero(tmpchar2,sizeof(tmpchar2));
	for (int i=0 ; i<nRet ; i++)
	{
		memcpy(tmpchar,buffer+i*16,16);
		bzero(tmpchar2,sizeof(tmpchar2));
		//����
		m_pDes.DES(tmpchar,tmpchar2,key3,1);
		//����
		m_pDes.DES(tmpchar2,tmpchar,key2,0);
		//����
		m_pDes.DES(tmpchar,(unsigned char *)(buffer+i*8),key1,1);
	}
	buffer[nRet*8]=0;
	free(destbuffer);
	destbuffer = NULL;
	return true;
}
