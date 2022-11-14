#Makefile for test
# AIX version

.SUFFIXES:
.SUFFIXES: .cpp .obj .o

CPP = g++

INCBASE = $(HOME)/include

KERNEL_INC= -I$(INCBASE) -I$(INCBASE)/kernel -I$(INCBASE)/dbpub -I$(INCBASE)/dreb -I$(INCBASE)/bumodule -I$(INCBASE)/dbapi -I$(INCBASE)/xml 

CPPFLAGS = -Wall -g -rdynamic -DLOGCURPATH -fPIC -m64 -std=c++11
CPPDEFS =  -O2 -DLINUX -DNDEBUG -D_THREAD_SAFE -D_MT -DSA_USE_PTHREAD -D_REENTRANT 

.cpp.o:
	$(CPP) $(CPPFLAGS) -c  $(CPPDEFS) $(KERNEL_INC) $< -o $@

OBJL =  BPCLink.o  BuFlowControl.o  BuFuncMgr.o  BuModule.o  CallBu.o  FuncTbl.o

ALL:   libbf_bumodule.so  libbf_bumodule.a clean

libbf_bumodule.a: $(OBJL)
	ar rv $@ $(OBJL)
	mv $@ $(HOME)/lib
libbf_bumodule.so: $(OBJL)
	g++ $(CPPFLAGS) $(CPPDEFS) -shared $(OBJL) -L$(HOME)/lib  -lpthread -ldl -lbf_kernel -lbf_dbpub -lbf_dbapi -o $@
	mv $@ $(HOME)/lib

clean:
	rm -f *.o
