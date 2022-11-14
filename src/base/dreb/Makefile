#Makefile for test
# AIX version

.SUFFIXES:
.SUFFIXES: .cpp .obj .o

CPP = g++


INCBASE = $(HOME)/include

INC= -I$(INCBASE) -I$(INCBASE)/kernel -I$(INCBASE)/dreb -I$(INCBASE)/dbpub



CPPFLAGS = -Wall -O2 -g -m64 -std=c++11 
CPPDEFS = -DUNIX  -DLINUX -DNDEBUG -D_THREAD_SAFE 

.cpp.o:
	$(CPP) $(CPPFLAGS) -c  $(CPPDEFS) $(INC) $< -o $@

OBJL =  BFDreb.o  DrebStatInfo.o  Dreb_Timer.o    MsgProcThread.o  RouteTbl.o   ServiceTbl.o  SocketMgr.o dreb.o    DrebTbl.o       GlobalVar.o   MemDB.o     PoolModule.o     SendQueue.o  SocketInfo.o  SvrTbl.o PoolData.o


ALL:   bfdreb clean

DREBLIB = -L$(HOME)/lib   -lbf_kernel -lbf_dbpub  -lpthread -ldl

bfdreb: $(OBJL)
	g++ $(CPPFLAGS) $(CPPDEFS) -o $@ $(OBJL) $(INC) -L$(HOME)/lib    $(CPPDEFS) $(DREBLIB)
	mv $@ $(HOME)/bin

clean:
	rm -f *.o
