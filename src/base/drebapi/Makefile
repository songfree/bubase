#Makefile for test
# AIX version

.SUFFIXES:
.SUFFIXES: .cpp .obj .o

CPP = g++

INCBASE = $(HOME)/include

KERNEL_INC= -I$(INCBASE) -I$(INCBASE)/kernel -I$(INCBASE)/dreb -I$(INCBASE)/dbpub

CPPFLAGS = -Wall -g -rdynamic -DLOGCURPATH -fPIC -m64
CPPDEFS =  -O2 -DLINUX -DNDEBUG -D_THREAD_SAFE -D_MT -DSA_USE_PTHREAD -D_REENTRANT 

.cpp.o:
	$(CPP) $(CPPFLAGS) -c  $(CPPDEFS) $(KERNEL_INC) $< -o $@

OBJL = BF_DrebResource.o   BF_DrebServer.o     BF_DrebSocketInfo.o  DrebClient.o   BF_DrebSendQueue.o  BF_DrebServerApi.o  BF_DrebSocketMgr.o DrebClientApi.o

ALL:   libbf_drebapi.so libbf_drebapi.a clean

libbf_drebapi.a: $(OBJL)
	ar rv $@ $(OBJL)
	mv $@ $(HOME)/lib
libbf_drebapi.so: $(OBJL)
	g++ $(CPPFLAGS) $(CPPDEFS) -shared $(OBJL) -L$(HOME)/lib  -lpthread -ldl -lbf_kernel -lbf_dbpub -o $@
	mv $@ $(HOME)/lib

clean:
	rm -f *.o
