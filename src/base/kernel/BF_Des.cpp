// BF_Des.cpp: implementation of the CBF_Des class.
//
//////////////////////////////////////////////////////////////////////

#include "BF_Des.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define DES_OK			0
#define DES_MAC_ERROR	-1

const unsigned char BF_DES_MASK[]={0x19,0xF2,0xC8,0x27,0x6A,0xD0,0x83,0x9B};

const char BF_DES_mfrxd_ip[]				/* initial permutation P	*/
= { 58, 50, 42, 34, 26, 18, 10,  2,
	60, 52, 44, 36, 28, 20, 12,  4,
	62, 54, 46, 38, 30, 22, 14,  6,
	64, 56, 48, 40, 32, 24, 16,  8,
	57, 49, 41, 33, 25, 17,  9,  1,
	59, 51, 43, 35, 27, 19, 11,  3,
	61, 53, 45, 37, 29, 21, 13,  5,
	63, 55, 47, 39, 31, 23, 15,  7 };

const char BF_DES_mfrxd_fp[]				/* final permutation F	  */
= {	40,  8, 48, 16, 56, 24, 64, 32,
	39,  7, 47, 15, 55, 23, 63, 31,
	38,  6, 46, 14, 54, 22, 62, 30,
	37,  5, 45, 13, 53, 21, 61, 29,
	36,  4, 44, 12, 52, 20, 60, 28,
	35,  3, 43, 11, 51, 19, 59, 27,
	34,  2, 42, 10, 50, 18, 58, 26,
	33,  1, 41,  9, 49, 17, 57, 25 };

const char BF_DES_pc1[]				/* permuted choice table (key)  */
= {	57, 49, 41, 33, 25, 17,  9,
	 1, 58, 50, 42, 34, 26, 18,
	10,  2, 59, 51, 43, 35, 27,
	19, 11,  3, 60, 52, 44, 36,

	63, 55, 47, 39, 31, 23, 15,
	 7, 62, 54, 46, 38, 30, 22,
	14,  6, 61, 53, 45, 37, 29,
	21, 13,  5, 28, 20, 12,  4	};

const char BF_DES_totrot[]			   /* number left rotations of pc1 */
= {	1,2,4,6,8,10,12,14,15,17,19,21,23,25,27,28	};

const char BF_DES_pc2[]				   /* permuted choice key (table)  */
= {	14, 17, 11, 24,  1,  5,
	 3, 28, 15,  6, 21, 10,
	23, 19, 12,  4, 26,  8,
	16,  7, 27, 20, 13,  2,
	41, 52, 31, 37, 47, 55,
	30, 40, 51, 45, 33, 48,
	44, 49, 39, 56, 34, 53,
	46, 42, 50, 36, 29, 32	};

const char BF_DES_si[8][64]			   /* 48->32 bit compression tables*/
= {					/* S[1]			 */
	14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7,
	 0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8,
	 4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0,
	15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13,
					/* S[2]			 */
	15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10,
	 3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5,
	 0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15,
	13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9,
					/* S[3]			 */
	10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8,
	13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1,
	13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7,
	 1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12,
					/* S[4]			 */
	 7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15,
	13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9,
	10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4,
	 3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14,
					/* S[5]			 */
	 2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9,
	14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6,
	 4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14,
	11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3,
					/* S[6]			 */
	12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11,
	10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8,
	 9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6,
	 4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13,
					/* S[7]			 */
	 4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1,
	13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6,
	 1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2,
	 6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12,
					/* S[8]			 */
	13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7,
	 1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2,
	 7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8,
	 2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11	};

const char BF_DES_p32i[]			   /* 32-bit permutation function  */
= {	16,  7, 20, 21,
	29, 12, 28, 17,
	 1, 15, 23, 26,
	 5, 18, 31, 10,
	 2,  8, 24, 14,
	32, 27,  3,  9,
	19, 13, 30,  6,
	22, 11,  4, 25	};

const int BF_DES_bytebit[]		   /* bit 0 is left-most in byte	*/
	= {	0200,0100,040,020,010,04,02,01 };

const int BF_DES_nibblebit[] = { 010,04,02,01 };


CBF_Des::CBF_Des()
{
	
}

CBF_Des::~CBF_Des()
{

}

void CBF_Des::permute(char *inblock,char perm[16][16][8],char *outblock)		
{
	register int i,j;
	register char *ib, *ob;		/* ptr to input or output block */
	register char *p, *q;

	for (i=0, ob = outblock; i<8; i++)
	{
		*ob++ = 0;		/* clear output block		*/
	}
	ib = inblock;
	for (j = 0; j < 16; j += 2, ib++) /* for each input nibble	*/
	{	
		ob = outblock;
		p = perm[j][(*ib >> 4) & 017];
		q = perm[j + 1][*ib & 017];
		for (i = 0; i < 8; i++)   /* and each output byte	*/
			*ob++ |= *p++ | *q++;   /* OR the masks together*/
	}
}
/* 1 compression value for sinit*/
int CBF_Des::getcomp(int k,int v)	
{	
	register int i,j;		/* correspond to i and j in FIPS*/

	i=((v&040)>>4)|(v&1);		/* first and last bits make row */
	j=(v&037)>>1;			/* middle 4 bits are column	*/
	return (int) BF_DES_si[k][(i<<4)+j];   /* result is ith row, jth col   */
}
/* initialize s1-s8 arrays	*/
void CBF_Des::sinit()			 
{	
	register int i,j;

	for (i=0; i<4; i++)		/* each 12-bit position		*/
	{
		for (j=0; j<4096; j++)  /* each possible 12-bit value   */
		{
			m_s[i][j]=(getcomp(i*2,j>>6)<<4) |
				(017&getcomp(i*2+1,j&077));
					/* store 2 compressions per char*/
		}
	}
}
/* initialize key schedule array*/
void CBF_Des::kinit(char *key)		
{	
	register int i,j,l;
	int m;

	for (j=0; j<56; j++)		/* convert pc1 to bits of key   */
	{	
		l=BF_DES_pc1[j]-1;		/* integer bit location		*/
		m = l & 07;		/* find bit			*/
		m_pc1m[j]=(key[l>>3] &	/* find which key byte l is in  */
			BF_DES_bytebit[m])	/* and which bit of that byte   */
			? 1 : 0;	/* and store 1-bit result	*/
	}
	for (i=0; i<16; i++)		/* for each key sched section   */
	{
		for (j=0; j<6; j++)	/* and each byte of the kn	*/
		{
			m_kn[i][j]=0;	/* clear it for accumulation	*/
		}
	}
	for (i=0; i<16; i++)		/* key chunk for each iteration */
	{	
		for (j=0; j<56; j++)	/* rotate pc1 the right amount  */
		m_pcr[j] = m_pc1m[(l=j+BF_DES_totrot[i])<(j<28? 28 : 56) ? l: l-28];
		/* rotate left and right halves independently   */

		for (j=0; j<48; j++)	/* select bits individually	*/
		{
			if (m_pcr[BF_DES_pc2[j]-1])	/* check bit that goes to kn[j] */
			{	
				l= j & 07;
				m_kn[i][j>>3] |= BF_DES_bytebit[l];
			}		/* mask it in if it's there	*/
		}
		
	}
}
/* initialize 32-bit permutation*/
void CBF_Des::p32init()			
{	
	register int l, j, k;
	int i,m;

	for (i=0; i<4; i++)		/* each input byte position	*/
	{
		for (j=0; j<256; j++)	/* all possible input bytes	*/
		{
			for (k=0; k<4; k++)	/* each byte of the mask	*/
			{
				m_p32[i][j][k]=0;	/* clear permutation array	*/
			}
		}
	}
	for (i=0; i<4; i++)		/* each input byte position	*/
	{
		for (j=0; j<256; j++)	/* each possible input byte	*/
		{
			for (k=0; k<32; k++)	/* each output bit position	*/
			{   
				l=BF_DES_p32i[k]-1;	/* invert this bit (0-31)	*/
				if ((l>>3)!=i)	/* does it come from input posn?*/
					continue;	/* if not, bit k is 0		*/
				if (!(j & BF_DES_bytebit[l&07]))
					continue;	/* any such bit in input?	*/
				m = k & 07;	 /* which bit is it?		*/
				m_p32[i][j][k>>3] |= BF_DES_bytebit[m];
			}
		}
		
	}
}
/* initialize a perm array	*/
void CBF_Des::perminit(char perm[16][16][8],const char p[64])
{	
	register int l, j, k;
	int i,m;

	for (i=0; i<16; i++)		/* each input nibble position   */
	{
		for (j=0; j<16; j++)	/* all possible input nibbles   */
		{
			for (k=0; k<8; k++)	/* each byte of the mask	*/
			{
				perm[i][j][k]=0;/* clear permutation array	*/
			}
		}
	}
	for (i=0; i<16; i++)		/* each input nibble position   */
	{
		for (j = 0; j < 16; j++)/* each possible input nibble   */
		{
			for (k = 0; k < 64; k++)/* each output bit position	*/
			{   
				l = p[k] - 1;	/* where does this bit come from*/
				if ((l >> 2) != i)  /* does it come from input posn?*/
					continue;	/* if not, bit k is 0		*/
				if (!(j & BF_DES_nibblebit[l & 3]))
					continue;	/* any such bit in input?	*/
				m = k & 07;	/* which bit is this in the byte*/
				perm[i][j][k>>3] |= BF_DES_bytebit[m];
			}
		}
	}
}
/* 32 to 48 bits with E oper	*/
/* right is 32, bigright 48	*/
void CBF_Des::expand(char *right,char *bigright)	
{
	register char *bb, *r, r0, r1, r2, r3;

	bb = bigright;
	r = right; r0 = *r++; r1 = *r++; r2 = *r++; r3 = *r++;
	*bb++ = ((r3 & 0001) << 7) |	/* 32				*/
		((r0 & 0370) >> 1) |	/* 1 2 3 4 5			*/
		((r0 & 0030) >> 3);	/* 4 5				*/
	*bb++ = ((r0 & 0007) << 5) |	/* 6 7 8			*/
		((r1 & 0200) >> 3) |	/* 9				*/
		((r0 & 0001) << 3) |	/* 8				*/
		((r1 & 0340) >> 5);	/* 9 10 11			*/
	*bb++ = ((r1 & 0030) << 3) |	/* 12 13			*/
		((r1 & 0037) << 1) |	/* 12 13 14 15 16		*/
		((r2 & 0200) >> 7);	/* 17				*/
	*bb++ = ((r1 & 0001) << 7) |	/* 16				*/
		((r2 & 0370) >> 1) |	/* 17 18 19 20 21		*/
		((r2 & 0030) >> 3);	/* 20 21			*/
	*bb++ = ((r2 & 0007) << 5) |	/* 22 23 24			*/
		((r3 & 0200) >> 3) |	/* 25				*/
		((r2 & 0001) << 3) |	/* 24				*/
		((r3 & 0340) >> 5);	/* 25 26 27			*/
	*bb++ = ((r3 & 0030) << 3) |	/* 28 29			*/
		((r3 & 0037) << 1) |	/* 28 29 30 31 32		*/
		((r0 & 0200) >> 7);	/* 1				*/
}
/* contract f from 48 to 32 bits*/
/* using 12-bit pieces into bytes */
void CBF_Des::contract(char *in48,char *out32)	
{	
	register char *c;
	register char *i;
	register int i0, i1, i2, i3, i4, i5;

	i = in48;
	i0 = *i++; i1 = *i++; i2 = *i++; i3 = *i++; i4 = *i++; i5 = *i++;
	c = out32;			/* do output a byte at a time   */
	*c++ = m_s[0][07777 & ((i0 << 4) | ((i1 >> 4) & 017  ))];
	*c++ = m_s[1][07777 & ((i1 << 8) | ( i2	& 0377 ))];
	*c++ = m_s[2][07777 & ((i3 << 4) | ((i4 >> 4) & 017  ))];
	*c++ = m_s[3][07777 & ((i4 << 8) | ( i5	& 0377 ))];
}
/* 32-bit permutation at end	*/
/*of the f crypto function	*/
void CBF_Des::perm32ex(char *inblock,char *outblock)
{	
	register int j;
	register char *ib, *ob;
	register char *q;

	ob = outblock;			/* clear output block		*/
	*ob++ = 0; *ob++ = 0; *ob++ = 0; *ob++ = 0;
	ib=inblock;			/* ptr to 1st byte of input	*/
	for (j=0; j<4; j++, ib++)	/* for each input byte		*/
	{	
		q = m_p32[j][*ib & 0377];
		ob = outblock;		/* and each output byte		*/
		*ob++ |= *q++;		/* OR the 16 masks together	*/
		*ob++ |= *q++;
		*ob++ |= *q++;
		*ob++ |= *q++;
	}
}
/* critical cryptographic trans */
void CBF_Des::f(char *right,int num,char *fret)	
{	
	register char *kb, *rb, *bb;	/* ptr to key selection &c	*/
	char bigright[6];		/* right expanded to 48 bits	*/
	char result[6];			/* expand(R) XOR keyselect[num] */
	char preout[4];			/* result of 32-bit permutation */

	kb = m_kn[num];			/* fast version of iteration	*/
	bb = bigright;
	rb = result;
	expand(right,bb);		/* expand to 48 bits		*/
	*rb++ = *bb++ ^ *kb++;		/* expanded R XOR chunk of key  */
	*rb++ = *bb++ ^ *kb++;
	*rb++ = *bb++ ^ *kb++;
	*rb++ = *bb++ ^ *kb++;
	*rb++ = *bb++ ^ *kb++;
	*rb++ = *bb++ ^ *kb++;
	contract(result,preout);	/* use S fns to get 32 bits	*/
	perm32ex(preout,fret);		/* and do final 32-bit perm	*/
}
/* 1 churning operation		*/
void CBF_Des::iter(int num,char *inblock,char *outblock)
{
	char fret[4];			/* return from f(R[i-1],key)	*/
	register char *ib, *ob, *fb;
/*	register int i;	*/	/* rwo: unused	*/

	ob = outblock; ib = &inblock[4];
	f(ib, num, fret);		/* the primary transformation   */
	*ob++ = *ib++;			/* L[i] = R[i-1]		*/
	*ob++ = *ib++;
	*ob++ = *ib++;
	*ob++ = *ib++;
	ib = inblock; fb = fret;	/* R[i]=L[i] XOR f(R[i-1],key)  */
	*ob++ = *ib++ ^ *fb++;
	*ob++ = *ib++ ^ *fb++;
	*ob++ = *ib++ ^ *fb++;
	*ob++ = *ib++ ^ *fb++;
}
/* initialize all des arrays	*/
void CBF_Des::desinit(char *key)			
{
	perminit(m_iperm,BF_DES_mfrxd_ip);	/* initial permutation		*/
	perminit(m_fperm,BF_DES_mfrxd_fp);	/* final permutation		*/
	kinit(key);			/* key schedule			*/
	sinit();			/* compression functions	*/
	p32init();			/* 32-bit permutation in f	*/
}
/* encrypt 64-bit inblock	*/
void CBF_Des::endes(char *inblock,char *outblock)
{	
	char iters[17][8];		/* workspace for each iteration */
	char swap[8];			/* place to interchange L and R */
	register int i;
	register char *s, *t;

	permute(inblock,m_iperm,iters[0]);/* apply initial permutation	*/
	for (i=0; i<16; i++)		/* 16 churning operations	*/
		iter(i,iters[i],iters[i+1]);
					/* don't re-copy to save space  */
	s = swap; t = &iters[16][4];	/* interchange left		*/
	*s++ = *t++; *s++ = *t++; *s++ = *t++; *s++ = *t++;
	t = &iters[16][0];		/* and right			*/
	*s++ = *t++; *s++ = *t++; *s++ = *t++; *s++ = *t++;
	permute(swap,m_fperm,outblock);   /* apply final permutation	*/
}
/* decrypt 64-bit inblock	*/
void CBF_Des::dedes(char *inblock,char *outblock)
{	
	char iters[17][8];		/* workspace for each iteration */
	char swap[8];			/* place to interchange L and R */
	register int i;
	register char *s, *t;

	permute(inblock,m_iperm,iters[0]);/* apply initial permutation	*/
	for (i=0; i<16; i++)		/* 16 churning operations	*/
		iter(15-i,iters[i],iters[i+1]);
					/* reverse order from encrypting*/
	s = swap; t = &iters[16][4];	/* interchange left		*/
	*s++ = *t++; *s++ = *t++; *s++ = *t++; *s++ = *t++;
	t = &iters[16][0];		/* and right			*/
	*s++ = *t++; *s++ = *t++; *s++ = *t++; *s++ = *t++;
	permute(swap,m_fperm,outblock);   /* apply final permutation	*/
}

void CBF_Des::Desc(unsigned char *data, unsigned char *mkey, int tag)
{
	unsigned char key[16],cdi[48],l[32],r[32],e[48],lr[64];
	unsigned char cd[48] = {
		14,17,11,24, 1, 5, 3,28,15, 6,21,10,23,19,12, 4,
		26, 8,16, 7,27,20,13, 2,41,52,31,37,47,55,30,40,
		51,45,33,48,44,49,39,56,34,53,46,42,50,36,29,32
	};
	unsigned char pb[32]={
		16, 7,20,21,29,12,28,17, 1,15,23,26, 5,18,31,10,
		2, 8,24,14,32,27, 3, 9,19,13,30, 6,22,11, 4,25
	};
	unsigned char s[8][64]={
		14, 4,13, 1, 2,15,11, 8, 3,10, 6,12, 5, 9, 0, 7,
		0,15, 7, 4,14, 2,13, 1,10, 6,12,11, 9, 5, 3, 8,
		4, 1,14, 8,13, 6, 2,11,15,12, 9, 7, 3,10, 5, 0,
		15,12, 8, 2, 4, 9, 1, 7, 5,11, 3,14,10, 0, 6,13,
		15, 1, 8,14, 6,11, 3, 4, 9, 7, 2,13,12, 0, 5,10,
		3,13, 4, 7,15, 2, 8,14,12, 0, 1,10, 6, 9,11, 5,
		0,14, 7,11,10, 4,13, 1, 5, 8,12, 6, 9, 3, 2,15,
		13, 8,10, 1, 3,15, 4, 2,11, 6, 7,12, 0, 5,14, 9,
		10, 0, 9,14, 6, 3,15, 5, 1,13,12, 7,11, 4, 2, 8,
		13, 7, 0, 9, 3, 4, 6,10, 2, 8, 5,14,12,11,15, 1,
		13, 6, 4, 9, 8,15, 3, 0,11, 1, 2,12, 5,10,14, 7,
		1,10,13, 0, 6, 9, 8, 7, 4,15,14, 3,11, 5, 2,12,
		7,13,14, 3, 0, 6, 9,10, 1, 2, 8, 5,11,12, 4,15,
		13, 8,11, 5, 6,15, 0, 3, 4, 7, 2,12, 1,10,14, 9,
		10, 6, 9, 0,12,11, 7,13,15, 1, 3,14, 5, 2, 8, 4,
		3,15, 0, 6,10, 1,13, 8, 9, 4, 5,11,12, 7, 2,14,
		2,12, 4, 1, 7,10,11, 6, 8, 5, 3,15,13, 0,14, 9,
		14,11, 2,12, 4, 7,13, 1, 5, 0,15,10, 3, 9, 8, 6,
		4, 2, 1,11,10,13, 7, 8,15, 9,12, 5, 6, 3, 0,14,
		11, 8,12, 7, 1,14, 2,13, 6,15, 0, 9,10, 4, 5, 3,
		12, 1,10,15, 9, 2, 6, 8, 0,13, 3, 4,14, 7, 5,11,
		10,15, 4, 2, 7,12, 9, 5, 6, 1,13,14, 0,11, 3, 8,
		9,14,15, 5, 2, 8,12, 3, 7, 0, 4,10, 1,13,11, 6,
		4, 3, 2,12, 9, 5,15,10,11,14, 1, 7, 6, 0, 8,13,
		4,11, 2,14,15, 0, 8,13, 3,12, 9, 7, 5,10, 6, 1,
		13, 0,11, 7, 4, 9, 1,10,14, 3, 5,12, 2,15, 8, 6,
		1, 4,11,13,12, 3, 7,14,10,15, 6, 8, 0, 5, 9, 2,
		6,11,13, 8, 1, 4,10, 7, 9, 5, 0,15,14, 2, 3,12,
		13, 2, 8, 4, 6,15,11, 1,10, 9, 3,14, 5, 0,12, 7,
		1,15,13, 8,10, 3, 7, 4,12, 5, 6,11, 0,14, 9, 2,
		7,11, 4, 1, 9,12,14, 2, 0, 6,10,13,15, 3, 5, 8,
		2, 1,14, 7, 4,10, 8,13,15,12, 9, 0, 3, 5, 6,11
	};
	int i, j, k, h;
	if( tag != 0 && tag != 1 ) 
	{
		return;
	}
	for(i=0; i<16; i++) 
	{
		key[i] = (isdigit(mkey[i]))
		    ? mkey[i]-48: (isxdigit(mkey[i]))?(mkey[i])-55: mkey[i];
		data[i] = (isdigit(data[i]))
		    ? data[i]-48: (isxdigit(data[i]))?(data[i])-55: data[i];
	}
	for(i=0; i<32; i++) 
	{
		h = (255-8*i)%66;
		l[i] = (data[h/4]>>(3-(h%4)))%2;
		h--;
		r[i]=(data[h/4]>>(3-(h%4)))%2;
	}
	for(i=1; i<=16; i++) 
	{
		k = 2*(17*tag-2*i*tag+i)-(17*tag-2*i*tag+i+5)/7-1;
		for(j=0; j<24; j++) 
		{
			h = (454-k*8-cd[j]*8+((cd[j]>=29-k)? 29:0))%65;
			cdi[j] = (key[h/4]>>(3-(h%4)))%2;
			h = (672-k*8-cd[j+24]*8+((cd[j+24]>=53-k)
			    ? 31:0)+((81-cd[j+24]<=k)
			    ? 31:0)+((cd[j+24]>=57-k)? 67:0))%63;
			cdi[j+24] = (key[h/4]>>(3-(h%4)))%2;
		}
		for(k=0; k<8; k++) 
		{
			for(j=0; j<6; j++)
			{
				e[6*k+j] = (r[(4*k+j+31)%32]+cdi[6*k+j])%2;
			}
			for(h=0; h<4; h++)
			{
				lr[k*4+h] = (s[k][e[6*k]*32+e[6*k+1]*8
					+e[6*k+2]*4+e[6*k+3]*2+e[6*k+4]
					+e[6*k+5]*16]>>
				    (3-h))%2;
			}
		}
		for(j=0; j<32; j++) 
		{
			lr[j+32] = (lr[pb[j]-1]+l[j])%2;
			l[j]=r[j]; 
			r[j]=lr[j+32];
		}
	}
	for(i=0; i<64; i++) lr[i] = (1-(i%2))*l[i/2%4*8-i/8+7]
	+(i%2)*r[i/2%4*8-i/8+7];
	for(i=0; i<16; i++)  
	{
		data[i] = 8*lr[4*i]+4*lr[4*i+1]+2*lr[4*i+2]+lr[4*i+3];
		data[i] = data[i]+48+data[i]/10*7;
	}
}

int CBF_Des::UnpackBCD(const unsigned char *InBuf,  char *OutBuf, int Len )
{
	int rc = 0;
	unsigned char ch;
	register int i, active = 0;
	unsigned char *pInBuf = (unsigned char *)InBuf;
	for ( i = 0; i < Len; i++ )
	{
		ch = *pInBuf;

		if ( active )
		{
			(*OutBuf=(ch&0xF))<10 ? (*OutBuf+='0') : (*OutBuf+=('A'-10));
			pInBuf++;
		}
		else
		{
			(*OutBuf=(ch&0xF0)>>4)<10 ? (*OutBuf+='0') : (*OutBuf+=('A'-10));
		}
		active ^= 1;
		if ( !isxdigit ( *OutBuf ) )	/* validate character */
		{
			rc = -1;
			break;
		}
		OutBuf++;
	}
	*OutBuf = 0;
	pInBuf=NULL;
	return ( rc );
}

int CBF_Des::PackBCD(const char *InBuf, unsigned char *OutBuf, int Len )
{
	int	    rc;		/* Return Value */
	register int     ActiveNibble;	/* Active Nibble Flag */
	char     CharIn;	/* Character from source buffer */
	unsigned char   CharOut;	/* Character from target buffer */
	char *pInBuf = (char *)InBuf;

	rc = 0;		/* Assume everything OK. */
	ActiveNibble = 0;	/* Set Most Sign Nibble (MSN) */
	/* to Active Nibble. */

	for ( ; (Len > 0); Len--, pInBuf++ )
	{
		CharIn = *pInBuf;

		if ( !isxdigit ( CharIn ) )	/* validate character */
		{
			rc = -1;
		}
		else
		{
			if ( CharIn > '9')
			{
				CharIn += 9;	/* Adjust Nibble for A-F */
			}
		}
		if ( rc == 0 )
		{
			CharOut = *OutBuf;
			if ( ActiveNibble )
			{
				*OutBuf++ = (unsigned char)( ( CharOut & 0xF0) |
				    ( CharIn  & 0x0F)   );
			}
			else
			{
				*OutBuf = (unsigned char)( ( CharOut & 0x0F)   |
				    ( (CharIn & 0x0F) << 4)   );
			}
			ActiveNibble ^= 1;	/* Change Active Nibble */
		}
	}
	pInBuf=NULL;
	return rc;
}


void CBF_Des::StringXOR(unsigned char *str1, unsigned char *str2)
{
	int i;

	for (i=0; i<16; i++) 
	{
		if ( isdigit(str1[i]) ) 
		{ 
			str1[i]-='0'; 
			continue; 
		}
		if ( isxdigit(str1[i]) ) 
		{ 
			str1[i]-=('A'-10); 
			continue; 
		}
	}
	for (i=0; i<16; i++) 
	{
		if ( isdigit(str2[i]) ) 
		{ 
			str2[i]-='0'; 
			continue; 
		}
		if ( isxdigit(str2[i]) ) 
		{ 
			str2[i]-=('A'-10); 
			continue; 
		}
	}
	for (i=0; i<16; i++) 
	{
		str1[i]^=str2[i];
	}
	for (i=0; i<16; i++) 
	{
		if ( str1[i] < 10 ) 
		{ 
			str1[i]+='0'; 
			continue; 
		}
		if ( str1[i] >= 10 ) 
		{ 
			str1[i]+=('A'-10); 
			continue; 
		}
	}
}
void CBF_Des::EncryptPin(char *pinblock,const char *pinkey)
{
	char keypart1[17];
	char keypart2[17];
	memset(keypart1,0x00,17);
	memset(keypart2,0x00,17);
	if (strlen(pinkey)<32)
	{
		return;
	}
	memcpy(keypart1,pinkey,16);
	memcpy(keypart2,pinkey+16,16);
	
	Desc((unsigned char *)pinblock,(unsigned char *)keypart1,0);
	Desc((unsigned char *)pinblock,(unsigned char *)keypart2,1);
	Desc((unsigned char *)pinblock,(unsigned char *)keypart1,0);

}



/***************************************************************************************************************/
/* 功能：得到ANSI9.8标准的明文PINBLOCK	                                                                   */
/* 参数：zh：帐号、卡号(长度大于13位) ,如果帐号不参加异或，传入16个 '0'                                       */
/*       pin: 密码明文(4-6位)                                                                                */
/*       pinblock：计算后的明文PINBLOCK (16个ASCII字符)                                                       */
/***************************************************************************************************************/

void CBF_Des::GetPinBlock(const char *zh,const char *pin,char *pinblock )
{
	int i = 0;
	char x1[17];
	char x2[17];
	int pinlen=0;
	int start = 0;

	memset(x1,'0',17);
	memset(x2,'0',17);
	x1[16]=0;
	pinlen=strlen(pin);

	if (pinlen==4)
	{
		strcpy(x2,"040000FFFFFFFFFF");
		memcpy(x2+2,pin,4);
	}
	else if (pinlen==6)
	{
		strcpy(x2,"06000000FFFFFFFF");
		memcpy(x2+2,pin,6);
	}
	else
	{
		strncpy(x2, pin, sizeof(x2)-1);
	}
	
	i = strlen(zh);
	if (i<13)
	{
		memcpy(x1+5,zh,11);
	}
	else
	{
		start = i - 13;
		memcpy(x1+4,zh+start,12);
	}
	strcpy(pinblock,x1);
	StringXOR((unsigned char *)pinblock,(unsigned char *)x2);
}
/***************************************************************************************************************/
/* 功能：根据明文PINBLOCK得到密码明文	                                                                   */
/* 参数：zh：帐号、卡号(长度大于13位) ,如果帐号不参加异或，传入16个 '0'                                       */
/*       pinblock: 明文pinblock (16个ASCII字符)                                                                               */
/*       pin：计算后的明文pin                                                        */
/***************************************************************************************************************/
void CBF_Des::GetPin(const char *zh,char *pinblock,char *pin)
{
	int i = 0;
	char x1[17];
	char x2[17];
	int pinlen=0;
	int start = 0;

	memset(x1,'0',17);
	memset(x2,'0',17);
	x1[16]=0;

	memcpy(x2,pinblock,16);
	
	i = strlen(zh);
	if (i<13)
	{
		memcpy(x1+5,zh,11);
	}
	else
	{
		start = i - 13;
		memcpy(x1+4,zh+start,12);
	}
	
	StringXOR((unsigned char *)x1,(unsigned char *)x2);
	/*异或结果在x1中*/
	
	memset(x2,0x00,sizeof(x2));
	pinlen=x1[1]-'0';
	memcpy(x2,x1+2,pinlen);
	strcpy(pin,x2);
	return;
}


/***************************************************************************************************************/
/* 功能：生成mac			                                                                                         */
/* 参数：makkey：MAKKEY明文                                                                                    */
/*       macblock: 需要进行计算的mak块。(大小<8k)                                                              */
/*       returnmac：计算的mac结果 (16个ASCII字符)                                                              */
/*       blocklen：macblock的长度                                                                              */
/***************************************************************************************************************/
int CBF_Des::SoftGenMac(const char *mackey,const char *macblock,char *returnmac,int blocklen)
{
	int  i, rc, k;
	char tmp_dta[8192];
	char tmp[17];
	char mac[32];
	char macdata[8192];
	int j;

	if ( mackey[0]==0x00 )
	{
		return -1;
	}
	if ( strlen( mackey ) !=  16 )
	{
		return -2;
	}

	if ( blocklen==0 )
	{
		return -3;
	}

	PackBCD(mackey,(unsigned char *)tmp,16);
	desinit(tmp);
	i=blocklen/8;
	k=blocklen%8 ;
	if ( k == 0 )
		i -= 1;

	memset(macdata, 0, sizeof(macdata));
	memcpy(macdata, macblock, blocklen);

	memset(tmp_dta, 0, sizeof(tmp_dta));
	memset(tmp_dta, 0, (i+1)*8);
	memcpy(tmp_dta, macblock, blocklen);

	memcpy(macdata, tmp_dta, (i+1)*8 );
	memset(mac,0x00,sizeof(mac));
	memcpy(mac, macdata, 8);

	endes(mac,tmp);

	memcpy(mac,tmp,8);
	for (rc=0; rc<i; rc++) 
	{
		memcpy(tmp, macdata+(rc+1)*8, 8);
		for (j=0;j<8;j++)
		{
			mac[j]=mac[j]^tmp[j];
		}
		mac[8]=0x00;
		endes(mac, tmp);
		memcpy(mac,tmp,8);
	}

	UnpackBCD((unsigned char *)mac,tmp,16);
	memcpy(mac,tmp,16);
	memcpy( returnmac, mac , 16 );
	returnmac[16]=0x00;

	return 0;
}
/***************************************************************************************************************/
/* 功能：加密函数，支持单des，2des和3des			                                                                                         */
/* 参数：mkey：加密密钥                                                                                    */
/*       pin: 需要进行加密的数据(16个ASCII字符，一般为ANSI9.8标准的PINBLOCK)                                                              */
/*       outpin：加密后的密文                                                              */
/***************************************************************************************************************/
int CBF_Des::SoftEndesc(unsigned char *mkey, unsigned char *pin, char *outpin)
{
	int  ilpinlen  = 0;
	int  ilmkeylen = 0;
	char keypart1[17];
	char keypart2[17];
	char keypart3[17];
	char tmpkey[48];
	
	ilpinlen  = strlen((char *)pin);
	ilmkeylen = strlen((char *)mkey);
	
	if(strlen((char *)mkey) < 16 || strlen((char *)pin) < 16 || (ilmkeylen%ilpinlen) != 0)
	{
		fprintf(stderr,"函数[softendesc] mkey[%s]mkeylen[%d]或者pin[%s]pinlen[%d]的长度不合法[-1]", mkey, ilmkeylen, pin, ilpinlen);
		return -1;
	}
	
	if(!mkey || !pin || !outpin)
	{
		fprintf(stderr,"函数[softendesc] mkey[%s]或者pin[%s]或者outpin[%s]的值不合法[-2]", mkey, pin, outpin);
		return -2;
	}
	
	memset(tmpkey, 0x00, sizeof(tmpkey));
	memset(keypart1, 0x00, sizeof(keypart1));
	memset(keypart2, 0x00, sizeof(keypart2));
	memset(keypart3, 0x00, sizeof(keypart3));
	
	memcpy(tmpkey, pin, sizeof(tmpkey));		
	
	if((ilmkeylen/ilpinlen) == 1)
	{
		if(ilmkeylen == 16)
		{ 
			memcpy(keypart1, mkey, 16);
			Desc((unsigned char *)tmpkey, (unsigned char *)keypart1, 0);
		}
		
		if(ilmkeylen == 32)
		{
			memcpy(keypart1, mkey, 16);
			Desc((unsigned char *)tmpkey, (unsigned char *)keypart1, 0);
			
			memcpy(keypart2, mkey+16, 16);
			Desc((unsigned char *)tmpkey+16, (unsigned char *)keypart2, 0);
		}
		
		if(ilmkeylen == 48)
		{
			memcpy(keypart1, mkey, 16);
			Desc((unsigned char *)tmpkey, (unsigned char *)keypart1, 0);
			
			memcpy(keypart2, mkey+16, 16);
			Desc((unsigned char *)tmpkey+16, (unsigned char *)keypart2, 1);
		
			memcpy(keypart3, mkey+32, 16);
			Desc((unsigned char *)tmpkey+32,(unsigned char *) keypart3, 0);
		}
	}
	else if((ilmkeylen/ilpinlen) == 2)
	{		
		memcpy(keypart1, mkey, 16);
		memcpy(keypart2, mkey+16, 16);
		Desc((unsigned char *)tmpkey, (unsigned char *)keypart1, 0);
		Desc((unsigned char *)tmpkey, (unsigned char *)keypart2, 1);
		Desc((unsigned char *)tmpkey, (unsigned char *)keypart1, 0);
	}
	else if((ilmkeylen/ilpinlen) == 3)
	{
		memcpy(keypart1, mkey, 16);
		memcpy(keypart2, mkey+16, 16);
		memcpy(keypart3, mkey+32, 16);
		Desc((unsigned char *)tmpkey, (unsigned char *)keypart1, 0);
		Desc((unsigned char *)tmpkey, (unsigned char *)keypart2, 1);
		Desc((unsigned char *)tmpkey, (unsigned char *)keypart3, 0);
	}
	else
	{
		fprintf(stderr,"函数[softendesc] mkey[%s]mkeylen[%d]或者pin[%s]pinlen[%d]的长度不合法[-3]", mkey, ilmkeylen, pin, ilpinlen);
		return -3;
	}
	memcpy(outpin, tmpkey, ilpinlen);
	outpin[ilpinlen] = '\0';
	
	/* fprintf(stderr,"函数[softendesc] ilpinlen[%d] ilmkeylen[%d] 用mkey[%s]对pin[%s]加密成功,结果[%s]", ilpinlen, ilmkeylen, mkey, pin, outpin); */
	
	return 0;
}
/***************************************************************************************************************/
/* 功能：解密函数，支持单des，2des和3des			                                                                                         */
/* 参数：mkey：解密密钥                                                                                    */
/*       pin: 需要进行解密的数据(16个ASCII字符，一般为ANSI9.8标准的PINBLOCK)                                                              */
/*       outpin：解密后的明文                                                              */
/***************************************************************************************************************/
int CBF_Des::SoftDedesc(unsigned char *mkey, unsigned char *pin, char *outpin)
{
	int  ilpinlen  = 0;
	int  ilmkeylen = 0;
	char keypart1[17];
	char keypart2[17];
	char keypart3[17];
	char tmpkey[48];
	
	ilpinlen   = strlen((char *)pin);
	ilmkeylen = strlen((char *)mkey);
	
	if(strlen((char *)mkey) < 16 || strlen((char *)pin) < 16 || (ilmkeylen%ilpinlen) != 0)
	{
		fprintf(stderr,"函数[softdedesc] mkey[%s]mkeylen[%d]或者pin[%s]pinlen[%d]的长度不合法[-1]", mkey, ilmkeylen, pin, ilpinlen);
		return -1;
	}
		
	if(!mkey || !pin || !outpin)
	{
		fprintf(stderr,"函数[softdedesc] mkey[%s]或者pin[%s]或者outpin[%s]的值不合法[-2]", mkey, pin, outpin);
		return -2;
	}
		
	memset(tmpkey, 0x00, sizeof(tmpkey));
	memset(keypart1, 0x00, sizeof(keypart1));
	memset(keypart2, 0x00, sizeof(keypart2));
	memset(keypart3, 0x00, sizeof(keypart3));
	
	memcpy(tmpkey, pin, sizeof(tmpkey));
	
	if((ilmkeylen/ilpinlen) == 1)
	{
		if(ilmkeylen == 16)
		{ 
			memcpy(keypart1, mkey, 16);
			Desc((unsigned char *)tmpkey, (unsigned char *)keypart1, 1);
		}
		
		if(ilmkeylen == 32)
		{
			memcpy(keypart1, mkey, 16);
			Desc((unsigned char *)tmpkey, (unsigned char *)keypart1, 1);
			
			memcpy(keypart2, mkey+16, 16);
			Desc((unsigned char *)tmpkey+16, (unsigned char *)keypart2, 1);
		}
		
		if(ilmkeylen == 48)
		{
			memcpy(keypart1, mkey, 16);
			Desc((unsigned char *)tmpkey, (unsigned char *)keypart1, 1);
			
			memcpy(keypart2, mkey+16, 16);
			Desc((unsigned char *)tmpkey+16, (unsigned char *)keypart2, 1);
		
			memcpy(keypart3, mkey+32, 16);
			Desc((unsigned char *)tmpkey+32, (unsigned char *)keypart3, 1);
		}
	}
	else if((ilmkeylen/ilpinlen) == 2)
	{		
		memcpy(keypart1, mkey, 16);
		memcpy(keypart2, mkey+16, 16);
		Desc((unsigned char *)tmpkey, (unsigned char *)keypart1, 1);
		Desc((unsigned char *)tmpkey, (unsigned char *)keypart2, 0);
		Desc((unsigned char *)tmpkey, (unsigned char *)keypart1, 1);
	}else if((ilmkeylen/ilpinlen) == 3)
	{
		memcpy(keypart1, mkey, 16);
		memcpy(keypart2, mkey+16, 16);
		memcpy(keypart3, mkey+32, 16);
		Desc((unsigned char *)tmpkey, (unsigned char *)keypart1, 1);
		Desc((unsigned char *)tmpkey, (unsigned char *)keypart2, 0);
		Desc((unsigned char *)tmpkey, (unsigned char *)keypart3, 1);
	}
	else
	{
		fprintf(stderr,"函数[softdedesc] mkey[%s]mkeylen[%d]或者pin[%s]pinlen[%d]的长度不合法[-3]", mkey, ilmkeylen, pin, ilpinlen);
		return -3;
	}
	memcpy(outpin, tmpkey, ilpinlen);
	outpin[ilpinlen] = '\0';

	
	/* fprintf(stderr,"函数[softdedesc] ilpinlen[%d] ilmkeylen[%d] 用mkey[%s]对pin[%s]解密成功,结果[%s]", ilpinlen, ilmkeylen, mkey, pin, outpin); */
	
	return 0;
}



/*对输入密钥进行校验调整使其符合奇校验，输入密钥为0-F字符表示，
例如输入为“0000000000000000”输出为“0101010101010101*/
void CBF_Des::Change_Key(char *key,int len)
{
	int i,j,k,flag;
	unsigned char a,b;
	unsigned char key_bcd[25];

	memset(key_bcd,0x00,sizeof(key_bcd));
	PackBCD(key,key_bcd,len);
	
	for(i=0;i<len/2;i++)
	{
		j=0;
		a=key_bcd[i];
		for(k=0;k<8;k++)
		{
			j=j+a%2;
			a=a>>1;
		}
	    
		flag=j%2;		
		if(flag)
			b=key_bcd[i];		
		else
			b=key_bcd[i]^0x01;	
		UnpackBCD(&b,key+2*i,2);
	}
	*(key+len)='\0';
	return;
}

void CBF_Des::DSP_2_HEX(const char *dsp,char *hex,int count)
{
    int i;
    for(i = 0; i < count; i++)
    {
		hex[i]=((dsp[i*2]<=0x39)?dsp[i*2]-0x30:dsp[i*2]-0x41+10);
        hex[i]=hex[i]<<4;
		hex[i]+=((dsp[i*2+1]<=0x39)?dsp[i*2+1]-0x30:dsp[i*2+1]-0x41+10);
    }
} 

void CBF_Des::HEX_2_DSP(const char *hex,char *dsp,int count)
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

/*对字串进行加密*/
int CBF_Des::EncryptString(char *buf,int &buflen,const char *key)
{
	return m_pDesEncrypt.encrypt64(buf,buflen,key);
}
/*对字串进行解密*/
int CBF_Des::UncryptString(char *buf,int &buflen ,const char *key)
{
	return m_pDesEncrypt.unencrypt64(buf,buflen,key);
}
/*对字串进行加密*/
int CBF_Des::Encryptchar(char *buf,int &buflen,const char *key)
{
	int ret = m_pDesEncrypt.encryptchar(buf,buflen,key);
	buflen = ret;
	return ret;
}
	
/*对字串进行解密*/
int CBF_Des::Uncryptchar(char *buf,int &buflen ,const char *key)
{
	int ret = m_pDesEncrypt.unencryptchar(buf,buflen,key);
	buflen = ret;
	return ret;
}
bool CBF_Des::Encrypt(char *data, int &datalen, const char *key)
{
	char *dspdata = (char *)malloc(datalen*3+8);
	bzero(dspdata,sizeof(datalen*3+8));
	HEX_2_DSP(data,dspdata,datalen);
	datalen = datalen *2;
	
	if (EncryptString(dspdata,datalen,key)<=0)
	{
		free(dspdata);
		dspdata = NULL;
		return false;
	}
	memcpy(data,dspdata,datalen);
	free(dspdata);
	dspdata = NULL;
	return true;
}

bool CBF_Des::Unencrypt(char *data, int &datalen, const char *key)
{
	if (UncryptString(data,datalen,key)<=0)
	{
		return false;
	}
	char *hexdata = (char *)malloc(datalen+8);
	bzero(hexdata,datalen+8);
	DSP_2_HEX(data,hexdata,datalen);
	datalen = datalen /2;
	memcpy(data,hexdata,datalen);
	free(hexdata);
	hexdata = NULL;
	return true;
}