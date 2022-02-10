// changedbpwd.cpp : Defines the entry point for the console application.
//
#include "public.h"
#include "BF_Thread.h"
#ifdef _WINDOWS
#ifdef _DEBUG
#pragma comment(lib, "bf_kerneld.lib") 
#pragma message("Automatically linking with   bf_kerneld.lib ")
#else

#pragma comment(lib, "bf_kernel.lib") 
#pragma message("Automatically linking with    bf_kernel.lib ")
#endif
#endif

bool g_bExit = false;
vector<int> g_vTestVector;

#define  MAXNUM  1000000000

class test_Safe : public CBF_Thread
{
public:
	int  m_nType;
private:
	virtual int Run()
	{
		int st;
		while (!g_bExit)
		{
			if (m_nType == 0) //²éÑ¯
			{
				st= g_vTestVector.size();
				if (st>0)
				{
					g_vTestVector.at(st-1);
				}
				
			}
			else if (m_nType == 1) //Ôö¼Ó
			{
				st= g_vTestVector.size();
				if (st <= MAXNUM)
				{
					g_vTestVector.push_back(time(NULL));
				}
			}
			
		}
		return 0;
	}
};

int main(int argc, char* argv[])
{

	char errmsg[256];
	test_Safe tt[20];
	for (int i=0; i< 20 ; i++)
	{
		tt[i].m_nType=0;
	}
	tt[19].m_nType=1;
	tt[18].m_nType=1;

	g_bExit = false;
	for (i=0; i< 18 ; i++)
	{
		sprintf(errmsg,"test_%d",i);
		tt[i].m_sThreadName  = errmsg;
		tt[i].CreateThread();
	}
	tt[18].m_sThreadName  = "test_18";
	tt[18].CreateThread();
//	tt[19].m_sThreadName  = "test_19";
//	tt[19].CreateThread();
	
	
		
	int st;
	while (!g_bExit)
	{
		st= g_vTestVector.size() ;
		if (st >= MAXNUM)
		{
			g_bExit = true;
			break;
		}
		SLEEP(1);
	}
	SLEEP(1000);
	return 0;
}
