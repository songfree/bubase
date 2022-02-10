// changedbpwd.cpp : Defines the entry point for the console application.
//
#include "public.h"
#include "SrvThread.h"

int main(int argc, char* argv[])
{
	CSrvThread mythread;
	if (!mythread.Listen(9901))
	{
		return -1;
	}
	int nRet;
	while (1)
	{
		nRet = mythread.RunSelect();
		if (nRet != 0)
		{
			return -1;
		}
	}
	return 0;
}
