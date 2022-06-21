#Makefile for test
# AIX version

.SUFFIXES:
.SUFFIXES: .cpp .obj .o

CPP = g++


INCBASE = $(HOME)/include

INC= -I$(INCBASE) -I$(INCBASE)/kernel  -I$(INCBASE)/dreb -I$(INCBASE)/dbpub


CPPFLAGS = -m64 -g -Wall  -O2 -fpic 
CPPDEFS =  -DLINUX -DUNIX  -D_THREAD_SAFE 

.cpp.o:
	$(CPP) $(CPPFLAGS) -c  $(CPPDEFS) $(INC) $< -o $@

OBJL =  bpc.o  DrebMsgThread.o   Bpc_Timer.o  GResource.o  LinkThread.o  NextPack.o  PoolData.o    PuThread.o   SocketInfo.o BpcRun.o  FuncTbl.o      MsgThread.o   NextTbl.o    SendQueue.o  SocketMgr.o  DispathThread.o


ALL:   bfsap clean



BPCLIB = -L$(HOME)/lib  -lbf_kernel  -lbf_drebapi  -lpthread -ldl -lbf_dbpub

bfsap: $(OBJL)
	g++ $(CPPFLAGS) $(CPPDEFS) -o $@ $(OBJL) $(INC) -L$(HOME)/lib    $(CPPDEFS) $(BPCLIB)
	mv $@ $(HOME)/bin
clean:
	rm -f *.o
