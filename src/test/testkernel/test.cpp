#include "public.h"
#include "BF_Mutex.h"
#include "GateHead.h"
#include "MdbBase.h"
#include "BF_Date_Time.h"
#include "GateApi.h"


#ifdef _WINDOWS
#ifdef _DEBUG
#pragma comment(lib, "bf_kerneld.lib") 
#pragma comment(lib, "bf_cgateapid.lib") 
#pragma message("Automatically linking with   bf_kerneld.lib bf_cgateapid.lib")
#else
#pragma comment(lib, "bf_kernel.lib") 
#pragma comment(lib, "bf_cgateapi.lib") 
#pragma message("Automatically linking with   bf_kernel.lib bf_cgateapi.lib ")
#endif

#endif



int main(int argc,char *argv[])
{
    BFCGATE_TSI_InitApi("cgateapi.xml");

    BFCGATE_TSI_ConnectGate(0);

    printf("gettickcount[%d]\n",CBF_Date_Time::GetTickCount());
    //CBF_MutexEvent envent;
    CIndexInt<1> aa;
    for (int i = 0; i < 10; i++)
    {
        aa.Add(i,i);
    }
    for (int i = 0; i < 10; i++)
    {
        aa.Add(i+10, i);
    }
    int rid;
    CInt iset;
    bool bret = aa.Select(iset,8);
    bret = iset.First(rid);
    while (bret)
    {
         bret = iset.Next(rid);
    }
    SLEEP_SECONDS(5);
    return 0;
}
