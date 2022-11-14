#Makefile for test
# AIX version

.SUFFIXES:
.SUFFIXES: .cpp .obj .o

CPP = g++

INCBASE = $(HOME)/include

KERNEL_INC= -I$(INCBASE) -I$(INCBASE)/kernel -I$(INCBASE)/dbpub -I$(INCBASE)/dreb -I$(INCBASE)/bumodule -I$(INCBASE)/cgateapi -I$(INCBASE)/gateway

CPPFLAGS = -Wall -g -rdynamic -DLOGCURPATH -fPIC -m64 -std=c++11
CPPDEFS =  -O2 -DLINUX -DNDEBUG -D_THREAD_SAFE -D_MT -DSA_USE_PTHREAD -D_REENTRANT 

.cpp.o:
	$(CPP) $(CPPFLAGS) -c  $(CPPDEFS) $(KERNEL_INC) $< -o $@

OBJL = CgateLog.o ApiFlowInfo.o  ApiSocket.o  GateApi.o  GateLink.o  GlobalResource.o  PoolData.o  SendQueue.o

ALL:   libbf_cgateapi.so libbf_cgateapi.a clean

libbf_cgateapi.a: $(OBJL)
	ar rv $@ $(OBJL)
	mv $@ $(HOME)/lib
libbf_cgateapi.so: $(OBJL)
	g++ $(CPPFLAGS) $(CPPDEFS) -shared $(OBJL) -L$(HOME)/lib  -lpthread -ldl -lbf_kernel -lbf_dbpub  -o $@
	mv $@ $(HOME)/lib

clean:
	rm -f *.o
