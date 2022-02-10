#include "BF_DesEncrypt.h"


int ENCRYPT_H_pc1[56]={
57,49,41,33,25,17,9,
1,58,50,42,34,26,18,
10,2,29,51,43,35,27,
19,11,3,60,52,44,36,
63,55,47,39,31,23,15,
7,62,54,46,38,30,22,
14,6,61,53,45,37,29,
21,13,5,28,20,12,4};

int ENCRYPT_H_pc2[48]={
14,17,11,24,1,5,
3,28,15,6,21,10,
23,19,12,4,26,8,
16,7,27,20,13,2,
41,52,31,37,47,55,
30,40,21,45,33,48,
44,49,39,56,34,53,
46,42,50,36,29,32};

int ENCRYPT_H_pc3[64]={
58,50,42,34,26,18,10,2,
60,52,44,36,28,20,12,4,
62,54,46,38,30,22,14,6,
64,56,48,40,32,24,16,8,
57,49,41,33,25,17,9,1,
59,51,43,35,27,19,11,3,
61,53,45,37,29,21,13,5,
63,55,47,39,31,23,15,7};

int ENCRYPT_H_e1[48]={
32,1,2,3,4,5,
4,5,6,7,8,9,
8,9,10,11,12,13,
12,13,14,15,16,17,
16,17,18,19,20,21,
20,21,22,23,24,25,
24,25,16,27,28,29,
28,29,30,31,32,1};

int ENCRYPT_H_s[8][4][16]={
{{14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7},
{0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8},
{4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0},
{15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13}},

{{15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10},
{3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5},
{0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15},
{13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9}},

{{10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8},
{13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1},
{13,6,4,2,8,15,3,0,11,1,2,12,5,10,14,7},
{1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12}},

{{7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15},
{13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9},
{10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4},
{3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14}},

{{2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9},
{14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6},
{4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14},
{11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3}},

{{12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11},
{10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8},
{9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6},
{4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13}},

{{4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1},
{13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6},
{1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2},
{6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12}},

{{13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7},
{1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2},
{7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8},
{2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11}}};

int ENCRYPT_H_p1[32]={
16,7,20,21,
29,12,28,17,
1,15,23,26,
5,18,31,10,
2,8,24,14,
32,27,3,9,
19,13,30,6,
22,11,4,25};

int ENCRYPT_H_ip[64]={
40,8,48,16,56,24,64,32,
39,7,47,15,55,23,63,31,
38,6,46,14,54,22,62,30,
37,5,45,13,53,21,61,29,
36,4,44,12,52,20,60,28,
35,3,43,11,51,19,59,27,
34,2,42,10,50,18,58,26,
33,1,41,9,49,17,57,25};

int ENCRYPT_H_kmv[16]={
1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1};

int e_r[48]={
	32,1,2,3,4,5,4,5,6,7,8,9,
	8,9,10,11,12,13,12,13,14,15,16,17,
	16,17,18,19,20,21,20,21,22,23,24,25,
	24,25,26,27,28,29,28,29,30,31,32,1};

int P[32]={
	16,7,20,21,29,12,28,17,
	1,15,23,26,5,18,31,10,
	2,8,24,14,32,27,3,9,
	19,13,30,6,22,11,4,25};

int SSS[8][4][16]={
	{{14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7},
	{0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8},/* err on */
	{4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0},
	{15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13}},

	{{15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10},
	{3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5},
	{0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15},
	{13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9}},

	{{10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8},
	{13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1},
	{13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7},
	{1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12}},

	{{7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15},
	{13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9},
	{10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4},
	{3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14}}, /* err on */

	{{2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9},
	{14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6}, /* err on */
	{4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14},
	{11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3}},

	{{12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11},
	{10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8},
	{9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6},
	{4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13}},

	{{4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1},
	{13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6},
	{1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2},
	{6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12}},

	{{13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7},
	{1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2},
	{7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8},
	{2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11}}};
int ip_tab[64]={
	58,50,42,34,26,18,10,2,
	60,52,44,36,28,20,12,4,
	62,54,46,38,30,22,14,6,
	64,56,48,40,32,24,16,8,
	57,49,41,33,25,17,9,1,
	59,51,43,35,27,19,11,3,
	61,53,45,37,29,21,13,5,
	63,55,47,39,31,23,15,7};
int _ip_tab[64]={
	40,8,48,16,56,24,64,32,
	39,7,47,15,55,23,63,31,
	38,6,46,14,54,22,62,30,
	37,5,45,13,53,21,61,29,
	36,4,44,12,52,20,60,28,
	35,3,43,11,51,19,59,27,
	34,2,42,10,50,18,58,26,
	33,1,41,9,49,17,57,25};
int pc_1_c[28]={
	57,49,41,33,25,17,9
	,1,58,50,42,34,26,18
	,10,2,59,51,43,35,27
	,19,11,3,60,52,44,36};
int pc_1_d[28]={
	63,55,47,39,31,23,15
	,7,62,54,46,38,30,22
	,14,6,61,53,45,37,29
	,21,13,5,28,20,12,4};
int pc_2[48]={
	14,17,11,24,1,5,
	3,28,15,6,21,10,
	23,19,12,4,26,8,
	16,7,27,20,13,2,
	41,52,31,37,47,55,
	30,40,51,45,33,48,
	44,49,39,56,34,53,
	46,42,50,36,29,32};
static int ls_count[16]={
	1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1};
CBF_DesEnctypt :: CBF_DesEnctypt()
{
    
}
CBF_DesEnctypt::~CBF_DesEnctypt()
{
        
}
int CBF_DesEnctypt::get_bit(char *s_key,int bits)
{
	int i,j,key;
	
	i=(bits-1)/8;
	j=bits-i*8;
	key=*(s_key+i);
	key>>=(j-1);
	key&=1;
	return(key);
}

int CBF_DesEnctypt::put_bit(char *o_key,int bits,int v)
{
	int i,j;

	i=(bits-1)/8;
	j=bits-i*8;
	v<<=(j-1);
	*(o_key+i)|=v;
	return 0;
}

int CBF_DesEnctypt::transkey(char *skey,char *okey,int count,int *tab)
{
	int i;

	for(i=0;i<count;i++)
	{
		put_bit(okey,i+1,get_bit(skey,*(tab+i)));
	}
	return 0;	
}
	
int CBF_DesEnctypt::k64to56(char *skey,char *okey)
{
	int i;
	for(i=0;i<7;i++)
		*(okey+i)=0;
	transkey(skey,okey,56,ENCRYPT_H_pc1);
	return 0;
}

int CBF_DesEnctypt::k56to48(char *skey,char *okey)
{
	int i;
	for(i=0;i<6;i++)
		*(okey+i)=0;
	transkey(skey,okey,48,ENCRYPT_H_pc2);
	return 0;
}

int CBF_DesEnctypt::k64to64(char *skey,char *okey)
{
	int i;
	for(i=0;i<8;i++)
		*(okey+i)=0;
	transkey(skey,okey,64,ENCRYPT_H_pc3);
	return 0;
}

char *CBF_DesEnctypt::des(char *d64 ,char *s_key)
{
    char k[17][6];
    char k64[9];
	
    char *keyp;
    char r[17][4],l[17][4];
	int i,j,jj,m,n;
	char b[9];
	char kk[6],p[4],k56[7],e[17][6];
	
	k64to56(s_key,k56);

	for(i=0;i<16;i++)
	{
		for(j=0;j<7;j++)
			k56[j]>>=ENCRYPT_H_kmv[i]; 
		k56to48(k56,k[i]);
	}

	k64to64(d64,k64);
	
	for(i=0;i<4;i++)
		r[0][i]=k64[4+i];
	for(i=0;i<4;i++)
		l[0][i]=k64[i];

	for(i=1;i<=16;i++)
	{
		for(j=0;j<6;j++)
			e[i-1][j]=0;
		transkey(r[i-1],e[i-1],48,ENCRYPT_H_e1);
		
		for(j=0;j<6;j++)
			kk[j]=(e[i-1][j]^k[i-1][j]);
		
		for(jj=1;jj<=8;jj++)
		{
			b[jj]=0;
			for(j=1+(jj-1)*6;j<=6+(jj-1)*6;j++)
				put_bit(&b[jj],j-(jj-1)*6,get_bit(kk,j));

			m=(get_bit(&b[jj],6)<<1)+get_bit(&b[jj],1);
			n=(get_bit(&b[jj],3)<<1)+get_bit(&b[jj],2);
			n+=(get_bit(&b[jj],5)<<3)+(get_bit(&b[jj],4)<<2);
			b[jj]=ENCRYPT_H_s[jj][m][n];
		}

		kk[0]=(15&b[1])+((b[2]&15)<<4);
		kk[1]=(15&b[3])+((b[4]&15)<<4);
		kk[2]=(15&b[5])+((b[6]&15)<<4);
		kk[3]=(15&b[7])+((b[8]&15)<<4);

		for(jj=0;jj<4;jj++)
			p[jj]=0;
		transkey(kk,p,32,ENCRYPT_H_p1);

		for(jj=0;jj<4;jj++)
			r[i][jj]=p[jj]^l[i-1][jj];

		for(jj=0;jj<4;jj++)
			l[i][jj]=r[i-1][jj];
	}	
	
	for(i=0;i<4;i++)
	{
		k64[i]=r[16][i];
		k64[i+4]=l[16][i];
	}


	for(jj=0;jj<=8;jj++)
		m_keydes[jj]=0;
	transkey(k64,m_keydes,64,ENCRYPT_H_ip);
	keyp = m_keydes;
	return(keyp);

	
}
char *CBF_DesEnctypt::undes(char *d64 ,char *s_key)
{
    char k[17][6];
    char k64[9];
	
    char *keyp;
    char r[17][4],l[17][4];
	int i,j,jj,m,n;
	char b[9];
	char kk[6],p[4],k56[7],e[17][6];
	
	k64to56(s_key,k56);

	for(i=0;i<16;i++)
	{
		for(j=0;j<7;j++)
			k56[j]>>=ENCRYPT_H_kmv[i]; 
		k56to48(k56,k[i]);
	}

	k64to64(d64,k64);
	
	for(i=0;i<4;i++)
		r[0][i]=k64[4+i];
	for(i=0;i<4;i++)
		l[0][i]=k64[i];

	for(i=1;i<=16;i++)
	{
		for(j=0;j<6;j++)
			e[i-1][j]=0;
		transkey(r[i-1],e[i-1],48,ENCRYPT_H_e1);
		
		for(j=0;j<6;j++)
			kk[j]=(e[i-1][j]^k[16-i][j]);
		
		for(jj=1;jj<=8;jj++)
		{
			b[jj]=0;
			for(j=1+(jj-1)*6;j<=6+(jj-1)*6;j++)
				put_bit(&b[jj],j-(jj-1)*6,get_bit(kk,j));

			m=(get_bit(&b[jj],6)<<1)+get_bit(&b[jj],1);
			n=(get_bit(&b[jj],3)<<1)+get_bit(&b[jj],2);
			n+=(get_bit(&b[jj],5)<<3)+(get_bit(&b[jj],4)<<2);
			b[jj]=ENCRYPT_H_s[jj][m][n];
		}

		kk[0]=(15&b[1])+((b[2]&15)<<4);
		kk[1]=(15&b[3])+((b[4]&15)<<4);
		kk[2]=(15&b[5])+((b[6]&15)<<4);
		kk[3]=(15&b[7])+((b[8]&15)<<4);

		for(jj=0;jj<4;jj++)
			p[jj]=0;
		transkey(kk,p,32,ENCRYPT_H_p1);


		for(jj=0;jj<4;jj++)
			r[i][jj]=p[jj]^l[i-1][jj];

		for(jj=0;jj<4;jj++)
			l[i][jj]=r[i-1][jj];
	}	
	
	for(i=0;i<4;i++)
	{
		k64[i]=r[16][i];
		k64[i+4]=l[16][i];
	}


	for(jj=0;jj<=8;jj++)
		m_keyundes[jj]=0;
	transkey(k64,m_keyundes,64,ENCRYPT_H_ip);
	keyp = m_keyundes;
	return(keyp);

	
}


int CBF_DesEnctypt::Des(char *txt,char *obj,char *key,int mode)
{
	unsigned char tt[9];
	char tm[9];
	int t[8];
	if(mode==ENCRYPT)
	{
		if(strlen(txt)<=8)
		{
			memset(tm,0,8);
			memcpy(tm,txt,strlen(txt));
		}
		else
		{
			memcpy(tm,txt,8);
			tm[8]=0;
		}
	
		memcpy((char *)tt,des(tm,key),8);
		sprintf(obj,"%02x%02x%02x%02x%02x%02x%02x%02x",\
			tt[0],tt[1],tt[2],tt[3],tt[4],tt[5],tt[6],tt[7]);
	}
	else if( mode== DISCRYPT)
	{
		sscanf(txt,"%02x%02x%02x%02x%02x%02x%02x%02x",\
			&t[0],&t[1],&t[2],&t[3],&t[4],&t[5],&t[6],&t[7]);
		tt[0]=t[0];tt[1]=t[1];tt[2]=t[2];tt[3]=t[3];
		tt[4]=t[4];tt[5]=t[5];tt[6]=t[6];tt[7]=t[7];
		memcpy(obj,undes((char *)tt,key),8);
	}
	return 0;
}



//static void expand0(unsigned char *in,char *out);
//static void setkeystar(unsigned char bits[64]);
//static void encrypt0(unsigned char *text,unsigned char *mtext);
//static void discrypt0(unsigned char *mtext,unsigned char *text);
//static void compress0(char *out,unsigned char *in);
//static void compress016(char *out,unsigned char *in);
//static void LS(char *bits,char *buffer,int count);
//static void son(char *cc,char *dd,char *kk);
//static void ip(unsigned char *text,char *ll,char *rr);
//static void _ip(unsigned char *text,char *ll,char *rr);
//static void F(int n,char *ll,char *rr,char *LL,char *RR);
//static void s_box(char *aa,char *bb);
//void DSP_2_HEX(char *dsp,char *hex,int count);
//void DSP_2_HEX(char *dsp,char *hex,int count);
//void HEX_2_DSP(char *hex,char *dsp,int count);


void CBF_DesEnctypt::DES(unsigned char *source,unsigned char *dest,unsigned char *key,unsigned char flag)
{
    unsigned char tmp[65];
	int tmpint[8];
	unsigned char tmpbuf[9];
	memset(tmpbuf,0,sizeof(tmpbuf));
    expand0(key, (char *)tmp);
    setkeystar(tmp);
	if( flag == ENCRYPT || flag == 'e' || flag == 'E' )
	{
		encrypt0(source, tmpbuf);
		sprintf((char *)dest,"%02x%02x%02x%02x%02x%02x%02x%02x",tmpbuf[0],tmpbuf[1],tmpbuf[2],tmpbuf[3],tmpbuf[4],tmpbuf[5],tmpbuf[6],tmpbuf[7]);
	}
    else
	{
		sscanf((char *)source,"%02x%02x%02x%02x%02x%02x%02x%02x",\
			&tmpint[0],&tmpint[1],&tmpint[2],&tmpint[3],&tmpint[4],&tmpint[5],&tmpint[6],&tmpint[7]);
		tmpbuf[0]=tmpint[0];
		tmpbuf[1]=tmpint[1];
		tmpbuf[2]=tmpint[2];
		tmpbuf[3]=tmpint[3];
		tmpbuf[4]=tmpint[4];
		tmpbuf[5]=tmpint[5];
		tmpbuf[6]=tmpint[6];
		tmpbuf[7]=tmpint[7];
		discrypt0(tmpbuf, dest);
	}
           
}

void CBF_DesEnctypt::Do_XOR(unsigned char *dest,unsigned char *source,int size)
{
   int i;
   for(i=0; i<size; i++)
       dest[i] ^= source[i];
}

void CBF_DesEnctypt::MAC(unsigned char *packet,int packet_size,unsigned char * mac_value,unsigned char * key,unsigned char mode)
{
   int size=0;

   memset(mac_value, 0, 8);

   while( packet_size > size)
   {
		if( (packet_size - size) <= 8 )
		{
			Do_XOR(mac_value, &packet[size], packet_size - size);
			DES(mac_value, mac_value, key, ENCRYPT);
			return;
		}
		Do_XOR(mac_value, &packet[size], 8);
		if( mode == STAND )
		{
			DES(mac_value, mac_value, key, ENCRYPT);
		}
		size += 8;
	}
}

void  CBF_DesEnctypt::HostDes(unsigned char *card_no,unsigned char *work_key,unsigned char *pin,unsigned char *encrypt_pin,unsigned char flag)
{
   unsigned char  card_buf[16], pin_buf[17], enpin_buf[8];
   unsigned char  key_buf[8];
   int   i, ii;

   DSP_2_HEX((char *)work_key,(char *)key_buf, 8);

   memset(card_buf, 'F', sizeof(card_buf));
   memcpy(card_buf, card_no+1, 15);
   DSP_2_HEX((char *)card_buf,(char *)card_buf, 8);
   card_buf[0] = 0;

   if(flag == 'e' || flag == 'E' || flag == ENCRYPT)
   {
       enpin_buf[0] = strlen((const char *)pin);
       memcpy(pin_buf, pin, strlen((const char *)pin));
       ii = strlen((const char *)pin);
       for(i = ii; i<17; i++)
       {
			pin_buf[i] = 'F';
       }
       DSP_2_HEX((char *)pin_buf, (char *)pin_buf, 8);

       Do_XOR(card_buf, pin_buf, 7);

       memcpy(enpin_buf+1, card_buf, 7);

       DES(enpin_buf, encrypt_pin, key_buf, ENCRYPT);
	   
       return;
   }

   if(flag == 'd' || flag == 'D' || flag == DECRYPT)
   {
       DES(encrypt_pin, pin_buf, key_buf, DECRYPT);

       Do_XOR(pin_buf+1, card_buf, 7);

       HEX_2_DSP((char *)(pin_buf+1), (char *)pin, 7);

       pin[pin_buf[0]&0x0f] = 0;

       return;
   }
}

void CBF_DesEnctypt::DSP_2_HEX(char *dsp,char *hex,int count)
{
    int i;
    for(i = 0; i < count; i++)
    {
		hex[i]=((dsp[i*2]<=0x39)?dsp[i*2]-0x30:dsp[i*2]-0x41+10);
        hex[i]=hex[i]<<4;
		hex[i]+=((dsp[i*2+1]<=0x39)?dsp[i*2+1]-0x30:dsp[i*2+1]-0x41+10);
    }
} 

void CBF_DesEnctypt::HEX_2_DSP(char *hex,char *dsp,int count)
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


void CBF_DesEnctypt::encrypt0(unsigned char *text,unsigned char *mtext)
{
	char ll[64],rr[64],LL[64],RR[64];
	char tmp[64];
	int i,j;
	ip(text,ll,rr);

	for (i=1;i<17;i++)
	{
		F(i,ll,rr,LL,RR);
		for (j=0;j<32;j++)
		{
			ll[j]=LL[j];
			rr[j]=RR[j];
		}
	}

	_ip((unsigned char *)tmp,rr,ll);

	compress0(tmp,mtext);
}

void CBF_DesEnctypt::discrypt0(unsigned char *mtext,unsigned char *text)
{
	char ll[64],rr[64],LL[64],RR[64];
	char tmp[64];
	int i,j;
	ip(mtext,ll,rr);

	for (i=16;i>0;i--)
	{
		F(i,ll,rr,LL,RR);
		for (j=0;j<32;j++)
		{
			ll[j]=LL[j];
			rr[j]=RR[j];
		}
	}

	_ip((unsigned char *)tmp,rr,ll);

	compress0(tmp,text);
}

void CBF_DesEnctypt::expand0(unsigned char *in,char *out)
{
	int divide;
	int i,j;

	for (i=0;i<8;i++)
	{
		divide=0x80;
		for (j=0;j<8;j++)
		{
			*out++=(in[i]/divide)&1;
			divide/=2;
		}
	}
}


void CBF_DesEnctypt::compress0(char *out,unsigned char *in)
{
	int times;
	int i,j;

	for (i=0;i<8;i++)
	{
		times=0x80;
		in[i]=0;
		for (j=0;j<8;j++)
		{
			in[i]+=(*out++)*times;
			times/=2;
		}
	}
}

void CBF_DesEnctypt::compress016(char *out,unsigned char *in)
{
	int times;
	int i,j;

	for (i=0;i<16;i++)
	{
		times=0x8;
		in[i]='0';
		for (j=0;j<4;j++)
		{
			in[i]+=(*out++)*times;
			times/=2;
		}
	}
}

void CBF_DesEnctypt::setkeystar(unsigned char bits[64])
{
	int i,j;

	for (i=0;i<28;i++)
		C[0][i]=bits[pc_1_c[i]-1];
	for (i=0;i<28;i++)
		D[0][i]=bits[pc_1_d[i]-1];
	for (j=0;j<16;j++)
	{
		LS((char *)(C[j]),(char *)(C[j+1]),ls_count[j]);
		LS((char *)(D[j]),(char *)(D[j+1]),ls_count[j]);
		son((char *)(C[j+1]),(char *)(D[j+1]),(char *)(K[j+1]));
	}
}


void CBF_DesEnctypt::LS(char *bits,char *buffer,int count)
{
	int i;
	for (i=0;i<28;i++)
	{
		buffer[i]=bits[(i+count)%28];
	}
}

void CBF_DesEnctypt::son(char *cc,char *dd,char *kk)
{
	int i;
	char buffer[56];
	for (i=0;i<28;i++)
		buffer[i] = *cc++;

	for (i=28;i<56;i++)
		buffer[i] = *dd++;

	for (i=0;i<48;i++)
		*kk++=buffer[pc_2[i]-1];
}



void CBF_DesEnctypt::ip(unsigned char *text,char *ll,char *rr)
{
	int i;
	char buffer[64];
	expand0(text,buffer);

	for (i=0;i<32;i++)
		ll[i]=buffer[ip_tab[i]-1];

	for (i=0;i<32;i++)
		rr[i]=buffer[ip_tab[i+32]-1];
}

void CBF_DesEnctypt::_ip(unsigned char *text,char *ll,char *rr)
{
	int i;
	char tmp[64];
	for (i=0;i<32;i++)
		tmp[i]=ll[i];
	for (i=32;i<64;i++)
		tmp[i]=rr[i-32];
	for (i=0;i<64;i++)
		text[i]=tmp[_ip_tab[i]-1];
}




void CBF_DesEnctypt::F(int n,char *ll,char *rr,char *LL,char *RR)
{
	int i;
	char buffer[64],tmp[64];
	for (i=0;i<48;i++)
		buffer[i]=rr[e_r[i]-1];
	for (i=0;i<48;i++)
		buffer[i]=(buffer[i]+K[n][i])&1;

	s_box(buffer,tmp);

	for (i=0;i<32;i++)
		buffer[i]=tmp[P[i]-1];

	for (i=0;i<32;i++)
		RR[i]=(buffer[i]+ll[i])&1;

	for (i=0;i<32;i++)
		LL[i]=rr[i];
}

void CBF_DesEnctypt::s_box(char *aa,char *bb)
{
	int i,j,k,m;
	int y,z;
	char ss[8];
	m=0;
	for (i=0;i<8;i++)
	{
		j=6*i;
		y=aa[j]*2+aa[j+5];
		z=aa[j+1]*8+aa[j+2]*4+aa[j+3]*2+aa[j+4];
		ss[i]=SSS[i][y][z];
		y=0x08;
		for (k=0;k<4;k++)
		{
			bb[m++]=(ss[i]/y)&1;
			y/=2;
		}

	}
}

/*对字串进行加密*/
int CBF_DesEnctypt::encrypt64(char *buf,int &buflen,const char *key)
{
	if (buflen <=0)
	{
		return -1;
	}
	int ret,ret1;
	char *p=NULL;
	unsigned char *pp=NULL;
	unsigned char tmp[9];
	unsigned char tmpout[17];
	unsigned char key2[9];
	memset(tmp,0,sizeof(tmp));
	memset(key2,0,sizeof(key2));
	memcpy(key2,key,8);
	p = buf;
	char *pdata = (char *)malloc(buflen+20);
	if (pdata == NULL)
	{
		return -1;
	}
	bzero(pdata,buflen+20);
	sprintf(pdata,"%08d",buflen);//前面8字节存原来数据的长度
	pp = (unsigned char *)pdata ;
	ret=0;//指向buf数据长度
	ret1=8;//指向pdata的数据长度后面
	pp = pp + 8; //指针移向长度后面
	while (ret<buflen)
	{
		memset(tmp,0,sizeof(tmp));//存放每次8个字节长度的数据
		if (ret +8 <= buflen)
		{
			memcpy(tmp,p,8);
		}
		else
		{
			memcpy(tmp,p,buflen-ret);
		}
		memset(tmpout,0,sizeof(tmpout));//存放每次8字节加密的结果
		DES64(tmp,tmpout,key2,0);//调用nsdes
		tmpout[8]=0;
		memcpy(pp,tmpout,8);
		pp = pp + 8;
		p = p+8;
		ret = ret+8;
		ret1 = ret1 +8;
	}
	memcpy(buf,pdata,ret1);
	free(pdata);
	pdata = NULL;
	buf[ret1]=0;
	buflen = ret1;
	p = NULL;
	pdata = NULL;
	return ret1;
	
}
/*对字串进行解密*/
int CBF_DesEnctypt::unencrypt64(char *buf,int &buflen ,const char *key)
{
	if (buflen <=8)
	{
		return -1;
	}
	int ret;
	char *p=NULL;
	unsigned char *pp=NULL;
	unsigned char tmp[17];
	unsigned char tmpout[17];
	unsigned char key2[9];
	char pHead[9];//加密的头 前8字节为原加密前的数据长度
	memset(key2,0,sizeof(key2));
	memcpy(key2,key,8);
	memset(pHead,0,sizeof(pHead));
	memcpy(pHead,buf,8);
	pHead[8]=0;
	int datalen = atoi(pHead);
	if (datalen <=0)
	{
		fprintf(stderr,"要解密的数据长度不符[%d]",buflen);
		return -102;
	}
	if (buflen%8 != 0)
	{
		fprintf(stderr,"要解密的数据长度不符[%d]",buflen);
		return -100;
	}
	if (datalen>buflen)
	{
		fprintf(stderr,"要解密的数据头[%d]和数据长度[%d]不符",datalen,buflen);
		return -101;
	}
	buflen = buflen -8;//加密的数据长度
	p = buf;
	p = p+8;//指向加密数据
	pp = (unsigned char *)buf ;//指向解密数据
	ret=0;
	while (ret<buflen)
	{
		memset(tmp,0,sizeof(tmp));
		memset(tmpout,0,sizeof(tmpout));
		if (ret +8 <= buflen)
		{
			memcpy(tmp,p,8);
		}
		else
		{
			memcpy(tmp,p,buflen-ret);
		}
		DES64(tmp,tmpout,key2,1);//调用nsdes
		tmp[8]=0;
		tmpout[8]=0;
		memcpy(pp,tmpout,8);
		pp = pp + 8;
		p = p+8;
		ret = ret+8;
		
	} 
	buflen = datalen ;
	buf[datalen]=0;
	p = NULL;
	pp = NULL;
	return datalen;
}

void CBF_DesEnctypt::DES64(unsigned char *source,unsigned char *dest,unsigned char *key,unsigned char flag)
{
    unsigned char tmp[65];
//	int tmpint[8];
	unsigned char tmpbuf[9];
	memset(tmpbuf,0,sizeof(tmpbuf));
    expand0(key, (char *)tmp);
    setkeystar(tmp);
	if( flag == ENCRYPT || flag == 'e' || flag == 'E' )
	{
		encrypt0(source, dest);
	}
    else
	{
		discrypt0(source, dest);
	}
	
}
/*对字串进行加密*/
int CBF_DesEnctypt::encryptchar(char *buf,int buflen,const char *key)
{
    int ret,ret1;
    char *p;
    unsigned char *pp;
    unsigned char tmp[9];
    unsigned char tmpout[17];
    char *outbuf;
    unsigned char key2[9];
    memset(tmp,0,sizeof(tmp));
	memset(key2,0,sizeof(key2));

	outbuf = (char *)malloc(buflen*2 +20);
	if (outbuf == NULL)
	{
		return -1;
	}
    memset(outbuf,0,buflen*2 +20);
	sprintf(outbuf,"%08d",buflen);
	pp = (unsigned char *)outbuf ;
	pp = pp+8;
    memcpy(key2,key,8);
    p = buf;
    
    ret=0;
    ret1=8;//置结果长度初始为8
    //errlog("comm",__FILE__,__LINE__,0,buf);
    while (ret<buflen)
    {
		memset(tmp,0,sizeof(tmp));
     	if (ret +8 <buflen)
     	{
     		memcpy(tmp,p,8);
     	}
     	else
     	{
     		memcpy(tmp,p,buflen-ret);
     	}
     	//errlog("comm",__FILE__,__LINE__,0,tmp);
     	memset(tmpout,0,sizeof(tmpout));
     	DES(tmp,tmpout,key2,0);//调用nsdes
     	//Des((char *)tmp,(char *)tmpout,key,0);//调用wxpdes
     	tmpout[16]=0;
     	//publog.errlog("comm",__FILE__,__LINE__,0,(char *)tmpout);
     	memcpy(pp,tmpout,16);
     	pp = pp + 16;
     	p = p+8;
     	ret = ret+8;
     	ret1 = ret1 +16;
	}
	memcpy(buf,outbuf,ret1);
	buf[ret1]=0;
	free(outbuf);
	outbuf = NULL;
	return ret1;
     
}
/*对字串进行解密*/
int CBF_DesEnctypt::unencryptchar(char *buf,int buflen ,const char *key)
{
	int ret;
	int dlen;
    char *p;
    unsigned char *pp;
    unsigned char tmp[17];
    unsigned char tmpout[17];
    unsigned char *outbuf;
    unsigned char key2[9];
	int datalen = buflen -8;
    memset(key2,0,sizeof(key2));

	outbuf = (unsigned char *)malloc(buflen);
	if (outbuf == NULL)
	{
		return -1;
	}
    memset(outbuf,0,buflen);
    memcpy(key2,key,8);
    p = buf;
	memset(tmpout,0,sizeof(tmpout));
	memcpy(tmpout,p,8);
	tmpout[8]=0;//原数据的长度
	dlen = atoi((char *)tmpout);
	p= p+8;
    pp = outbuf ;
    ret=0;
    //errlog("comm",__FILE__,__LINE__,0,buf);
    while (ret<datalen)
    {
     	memset(tmp,0,sizeof(tmp));
     	memset(tmpout,0,sizeof(tmpout));
     	if (ret +16 <datalen)
     	{
     		memcpy(tmp,p,16);
     	}
     	else
     	{
     		memcpy(tmp,p,datalen-ret);
     	}
     	DES(tmp,tmpout,key2,1);//调用nsdes
     	//Des((char *)tmp,(char *)tmpout,key,1);//调用wxpdes
     	tmp[16]=0;
     	tmpout[8]=0;
     	//errlog("comm",__FILE__,__LINE__,0,tmp);
     	//errlog("comm",__FILE__,__LINE__,0,tmpout);
     	memcpy(pp,tmpout,8);
     	pp = pp + 8;
     	p = p+16;
     	ret = ret+16;
     		
	}   
	memcpy(buf,(char *)outbuf,dlen);
	buf[dlen]=0;
	free(outbuf);
	outbuf = NULL;
	return dlen;
}


