// XmlTransSign.cpp: implementation of the CXmlTransSign class.
//
//////////////////////////////////////////////////////////////////////

#include "XmlTransSign.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//转义符
//&quot; "
//&lt; <
//&gt; >
//&amp; &
//&apos; '
int XmlgetTransSign(std::string &value)
{
	int nLength;
	char *tmpchar = (char *)malloc(value.length()+1);
	memset(tmpchar,0,value.length()+1);
	nLength = value.length();
	char *p = tmpchar;
	bool isbegin=false;
	int  isbeginoffset=0;
	bool isquot;
	bool islt;
	bool isgt;
	bool isamp;
	bool isapos;
	for (int i=0;i<nLength;)
	{
	
		if (!isbegin) //前面不是&开始的
		{
			if (value.c_str()[i] == '&' )
			{
				isbegin = true;
				isbeginoffset=1;
				isquot= true;
				islt= true;
				isgt= true;
				isamp= true;
				isapos= true;
			}
		}
		else if (isbeginoffset == 1)   //l g a q
		{
			isbeginoffset++;
			if (value.c_str()[i] == 'l' )
			{
				isquot= false;
				islt= true;
				isgt= false;
				isamp= false;
				isapos= false;
			}
			else if (value.c_str()[i] == 'g' )
			{
				isquot= false;
				islt= false;
				isgt= true;
				isamp= false;
				isapos= false;
			}
			else if (value.c_str()[i] == 'a' )
			{
				isquot= false;
				islt= false;
				isgt= false;
				isamp= true;
				isapos= true;
			}
			else if (value.c_str()[i] == 'q' )
			{
				isquot= true;
				islt= false;
				isgt= false;
				isamp= false;
				isapos= false;
			}
			else
			{
				isbegin = false;
				isbeginoffset=0;
			}

		}
		else if (isbeginoffset == 2)   //t m u p
		{
			if (islt || isgt)
			{
				if (value.c_str()[i] == 't' )
				{
					isbeginoffset++;
				}
				else
				{
					isbegin = false;
					isbeginoffset=0;
				}
			}
			else if (isquot)
			{
				if (value.c_str()[i] == 'u' )
				{
					isbeginoffset++;
				}
				else
				{
					isbegin = false;
					isbeginoffset=0;
				}
			}
			else if (isamp || isapos)
			{
				if (value.c_str()[i] == 'm' )
				{
					isapos = false;
					isbeginoffset++;
				}
				else if (value.c_str()[i] == 'p' )
				{
					isamp = false;
					isbeginoffset++;
				}
				else
				{
					isbegin = false;
					isbeginoffset=0;
				}
			}
			
		}
		else if (isbeginoffset == 3)   //; p o
		{
			if (islt)
			{
				if (value.c_str()[i] == ';' )
				{
					//替换 <
					p=p-isbeginoffset;
					*p= '<' ;
					p++;
					i++;
					isbegin = false;
					isbeginoffset=0;
				}
				else
				{
					isbegin = false;
					isbeginoffset=0;
				}
			}
			else if (isgt)
			{
				if (value.c_str()[i] == ';' )
				{
					//替换 >
					p=p-isbeginoffset;
					*p= '>' ;
					p++;
					i++;
					isbegin = false;
					isbeginoffset=0;
				}
				else
				{
					isbegin = false;
					isbeginoffset=0;
				}
			}
			else if (isquot)
			{
				if (value.c_str()[i] == 'o' )
				{
					isbeginoffset++;
					isapos = false;
				}
				else
				{
					isbegin = false;
					isbeginoffset=0;
				}
			}
			else if (isapos)
			{
				if (value.c_str()[i] == 'o' )
				{
					isquot = false;
					isbeginoffset++;
				}
				else
				{
					isbegin = false;
					isbeginoffset=0;
				}
			}
			else if (isamp)
			{
				if (value.c_str()[i] == 'p' )
				{
					isbeginoffset++;
				}
				else
				{
					isbegin = false;
					isbeginoffset=0;
				}
			}
			else
			{
				isbegin = false;
				isbeginoffset=0;
			}
		}
		else if (isbeginoffset == 4)  //; t s
		{
			if (isamp)
			{
				if (value.c_str()[i] == ';' )
				{
					//替换 &
					p=p-isbeginoffset;
					*p= '&' ;
					p++;
					i++;
					isbegin = false;
					isbeginoffset=0;
				}
				else
				{
					isbegin = false;
					isbeginoffset=0;
				}
			}
			else if (isapos)
			{
				if (value.c_str()[i] == 's' )
				{
					isbeginoffset++;
				}
				else
				{
					isbegin = false;
					isbeginoffset=0;
				}
			}
			else if (isquot)
			{
				if (value.c_str()[i] == 't' )
				{
					isbeginoffset++;
				}
				else
				{
					isbegin = false;
					isbeginoffset=0;
				}
			}
		}
		else if (isbeginoffset == 5)  //; 
		{
			if (isapos)
			{
				if (value.c_str()[i] == ';' )
				{
					//替换 '
					p=p-isbeginoffset;
					*p= 39 ;
					p++;
					i++;
					isbegin = false;
					isbeginoffset=0;
				}
				else
				{
					isbegin = false;
					isbeginoffset=0;
				}
			}
			else if (isquot)
			{
				if (value.c_str()[i] == ';' )
				{
					//替换 "
					p=p-isbeginoffset;
					*p= '"' ;
					p++;
					i++;
					isbegin = false;
					isbeginoffset=0;
				}
				else
				{
					isbegin = false;
					isbeginoffset=0;
				}
			}
			else
			{
				isbegin = false;
				isbeginoffset=0;
			}
		}
		else
		{
			isbegin = false;
			isbeginoffset=0;
		}
		*p = value.c_str()[i];
		p++;
		i++;
// 		if (nLength >= i+4)
// 		{
// 			if (strncmp((char *)value.c_str()+i,"&lt;",4)==0)
// 			{
// 				*p= '<' ;
// 				p++;
// 				i+=4;
// 				continue;
// 			}
// 			if (strncmp((char *)value.c_str()+i,"&gt;",4)==0)
// 			{
// 				*p= '>' ;
// 				p++;
// 				i+=4;
// 				continue;
// 			}
// 			if (nLength >= i+5)
// 			{
// 				if (strncmp((char *)value.c_str()+i,"&amp;",5)==0)
// 				{
// 					*p= '&' ;
// 					p++;
// 					i+=5;
// 					continue;
// 				}
// 			}
// 			if (nLength >= i+6)
// 			{
// 				if (strncmp((char *)value.c_str()+i,"&quot;",6)==0)
// 				{
// 					*p= '"' ;
// 					p++;
// 					i+=6;
// 					continue;
// 				}
// 				if (strncmp((char *)value.c_str()+i,"&apos;",6)==0)
// 				{
// 					*p= 39 ;
// 					p++;
// 					i+=6;
// 					continue;
// 				}
// 			}
// 
// 			*p = value.c_str()[i];
// 			p++;
// 			i++;
// 		}
// 		else
// 		{
// 			*p = value.c_str()[i];
// 			p++;
// 			i++;
// 		}
	}
	value = tmpchar;
	p=NULL;
	free(tmpchar);
	return 0;
}
//转义符
//&quot; "
//&lt; <
//&gt; >
//&amp; &
//&apos; '
int XmlputTransSign(std::string &value)
{
	if (value.length()<1)
	{
		return 0;
	}
	char *tmpchar = (char *)malloc(value.length()*3);
	memset(tmpchar,0,value.length()*3);
	
	char *p = tmpchar;
	for (int i=0;i<(int)value.length();i++)
	{
		if (value.c_str()[i] == '"' )
		{
			strcpy(p,"&quot;");
			p+=6;
		}
		else if (value.c_str()[i] == '<' )
		{
			strcpy(p,"&lt;");
			p+=4;
		}
		else if (value.c_str()[i] == '>' )
		{
			strcpy(p,"&gt;");
			p+=4;
		}
		else if (value.c_str()[i] == '&' )
		{
			strcpy(p,"&amp;");
			p+=5;
		}
		else if (value.c_str()[i] == 39 )
		{
			strcpy(p,"&apos;");
			p+=6;
		}
		else
		{
			*p = value.c_str()[i];
			p++;
		}
	}
	*(p+1)= 0;
	value = tmpchar;
	p = NULL;
	free(tmpchar);
	return 0;
}
