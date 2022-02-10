#Makefile for test
# AIX version

.SUFFIXES:
.SUFFIXES: .cpp .obj .o

CPP = g++


INCBASE = $(HOME)/include
KERNEL_INC= -I$(INCBASE) -I$(INCBASE)/kernel -I$(INCBASE)/xml 

CPPFLAGS = -Wall -g -rdynamic -fPIC -m64 
CPPDEFS = -O2 -DUNIX -DLINUX -DNDEBUG -D_THREAD_SAFE 

.cpp.o:
	$(CPP) $(CPPFLAGS) -c  $(CPPDEFS) $(KERNEL_INC) $< -o $@

OBJL =  GResource.o  SocketInfo.o front.o  PoolData.o ProcessThread.o SocketMgr.o PoolModule.o SendQueue.o 

ALL:   bffpc  clean

BCCLIB = -L$(HOME)/lib   -lbf_kernel  -lbf_oldxml -lpthread -ldl

bffpc: $(OBJL)
	g++ $(CPPFLAGS) $(CPPDEFS) -o $@ $(OBJL) $(KERNEL_INC) -L$(HOME)/lib    $(CPPDEFS) $(BCCLIB)
	mv $@ $(HOME)/bin
clean:
	rm -f *.o
