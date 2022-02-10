// changedbpwd.cpp : Defines the entry point for the console application.
//
#include "public.h"
#include "BF_LogClient.h"

#ifdef _WINDOWS
#ifdef _DEBUG

#pragma comment(lib, "bf_kerneld.lib") 
#pragma message("Automatically linking with   bf_kerneld.lib ")
#else

#pragma comment(lib, "bf_kernel.lib") 
#pragma message("Automatically linking with   bf_kernel.lib ")
#endif
#endif

class CTestfile
{
public:
	CTestfile()
	{
		m_fp=NULL;
	}
	virtual ~CTestfile()
	{
		if (m_fp!=NULL)
		{
			fclose(m_fp);
			m_fp = NULL;
		}
	}
	bool Open()
	{
		m_fp = fopen("test.txt","a+");
		if (m_fp!=NULL)
		{
			fprintf(m_fp,"%s","test aaaaaaaaaaaaaaaa");
			return true;
		}
		return false;
	}
	FILE *m_fp;

};

int main(int argc, char* argv[])
{

	char errmsg[256];
//	CTestfile aa;
//	aa.Open();

	CBF_LogClient log;
	log.SetMaxLogSize(2024);
	log.SetLogPara(LOG_DEBUG,"","testlogclient.log");
	
	log.StartLog();


	for (int i=0; i< 100 ; i++)
	{
		log.LogMp(LOG_DEBUG,__FILE__,__LINE__,"²âÊÔÐ´ÈÕÖ¾      %10d",i);
		SLEEP(100);

	}
	log.StopLog();
	
	return 0;
}
