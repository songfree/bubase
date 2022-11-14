#Makefile for test
# AIX version

.SUFFIXES:
.SUFFIXES: .cpp .obj .o

CPP = g++


INCBASE = $(HOME)/include

BU_INC= -I$(INCBASE) -I$(INCBASE)/kernel -I$(INCBASE)/dbpub 


CPPFLAGS = -Wall -g -rdynamic -fPIC -m64 -std=c++11 
CPPDEFS = -DUNIX -DLINUX -DNDEBUG -D_THREAD_SAFE 

.cpp.o:
	$(CPP) $(CPPFLAGS) -c  $(CPPDEFS) $(BU_INC) $< -o $@

OBJL =   GwRun.o  HostInfo.o  LinkThread.o  Resource.o  SocketInfo.o  SocketMgr.o  gw.o 

ALL:   bfgw clean

BCCLIB = -L$(HOME)/lib   -lbf_kernel -lbf_dbpub -lpthread -ldl

bfgw: $(OBJL)
	g++ $(CPPFLAGS) $(CPPDEFS) -o $@ $(OBJL) $(KERNEL_INC) -L$(HOME)/lib    $(CPPDEFS) $(BCCLIB)
	mv $@ $(HOME)/bin
clean:
	rm -f *.o
