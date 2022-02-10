// termkey.cpp : Defines the entry point for the console application.
//


#include "public.h"
#include "Errlog.h"
#include "pubstrtime.h"
//#include "DesApi.h"
#include "PtDesPub.h"

void DSP_2_HEX(char *dsp,char *hex,int count)
{
    int i;
    for(i = 0; i < count; i++)
    {
		hex[i]=((dsp[i*2]<=0x39)?dsp[i*2]-0x30:dsp[i*2]-0x41+10);
        hex[i]=hex[i]<<4;
		hex[i]+=((dsp[i*2+1]<=0x39)?dsp[i*2+1]-0x30:dsp[i*2+1]-0x41+10);
    }
} 

void HEX_2_DSP(char *hex,char *dsp,int count)
{
    int i;
    char ch;
    for(i = 0; i < count; i++)
    {
        ch=(hex[i]&0xf0)>>4;
        dsp[i*2]=(ch>9)?ch+0x41-10:ch+0x30;
        ch=hex[i]&0xf;
        dsp[i*2+1]=(ch>9)?ch+0x41-10:ch+0x30;
    }
} 

int main(int argc, char* argv[])
{

	int ret;
//	CDesApi api;

//	api.SetHostPara("172.29.22.132",8,2000);
	char sPin[8192];
	memset(sPin,0,sizeof(sPin));
	CErrlog pLog;
	pLog.SetLogPara(LOG_DEBUG,"","hsmapi.log");

	char pinkey[17];
	char pwd[17];
	char pinblock[33];
	memset(pinkey,0,sizeof(pinkey));
	memset(pwd,0,sizeof(pwd));
	memset(pinblock,0,sizeof(pinblock));

	strcpy(pinkey,"1234567890987654");
	strcpy(pwd,"000000");
	getpinblock("0000000000000000",pwd,pinblock);
//	strcpy(pinblock,"06123456FFFFFFFF");
	softendesc((char *)pinkey,( char *)pinblock,sPin);

	pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"密钥明文[%s]密码明文[%s]PINBLOCK[%s]密码密文[%s]",pinkey,pwd,pinblock,sPin);
 	printf("%s\n",sPin);
// 	ret = api.ConvertPinBlock("8F59ADC85C995E72D0A7A5E80D49055D","70E8685B13E372CA9B8CC3700382E098","2C7B3559EBB9E6A4",sPin);
// 	printf("%s\n",sPin);
	char transkey[33];
	sprintf(pinkey,"9EA444CF70BF3BED");
	sprintf(transkey,"33333333333333333333333333333333");
	softdedesc((unsigned char *)transkey,(unsigned char *)pinkey,sPin);//解开PINKEY
	strcpy(pinkey,sPin);
	pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"PINKEY【%s】",pinkey);
	
	//转加//9E5E0157D3942172
	
	//      E64D61800742D563

	strcpy(pwd,"000000");
	getpinblock("6210983000021820618",pwd,pinblock);
	pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"pinblock【%s】",pinblock);

	softendesc((char *)pinkey,( char *)pinblock,sPin);
	pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"密码【%s】",sPin);

	memset(pinblock,0,sizeof(pinblock));
	softdedesc((unsigned char *)pinkey,(unsigned char *)sPin,pinblock);

	pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"pinblock【%s】",pinblock);


	return 0;
}
