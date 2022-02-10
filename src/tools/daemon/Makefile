#Makefile for test
# AIX version

.SUFFIXES:
.SUFFIXES: .cpp .obj .o

CPP = g++


INCBASE = $(HOME)/include

BU_INC= -I$(INCBASE) -I$(INCBASE)/kernel -I$(INCBASE)/dbpub -I$(INCBASE)/kvdata


CPPFLAGS = -Wall -g -rdynamic -fPIC -m64 
CPPDEFS = -DUNIX -DLINUX -DNDEBUG -D_THREAD_SAFE 

.cpp.o:
	$(CPP) $(CPPFLAGS) -c  $(CPPDEFS) $(BU_INC) $< -o $@

OBJL =     LinkThread.o  MoniRun.o    Resource.o  SocketInfo.o  SocketMgr.o  main.o

OBJL2 =   tkpid.o

OBJL3 =    daemonprocess.o 

ALL:   bfdaemon testdaemon clean

BCCLIB = -L$(HOME)/lib   -lbf_kernel -lbf_oldxml -lbf_dbpub -lbf_kvdata -lpthread -ldl

bfdaemon: $(OBJL)
	g++ $(CPPFLAGS) $(CPPDEFS) -o $@ $(OBJL) $(BU_INC) -L$(HOME)/lib    $(CPPDEFS) $(BCCLIB)
	mv $@ $(HOME)/bin
testdaemon: $(OBJL3)
	g++ $(CPPFLAGS) $(CPPDEFS) -o $@ $(OBJL3) $(BU_INC) -L$(HOME)/lib    $(CPPDEFS) $(BCCLIB)
	mv $@ $(HOME)/bin
clean:
	rm -f *.o
testkill: $(OBJL2)
	g++ $(CPPFLAGS) $(CPPDEFS) -o $@ $(OBJL2) $(BU_INC) -L$(HOME)/lib    $(CPPDEFS) $(BCCLIB)
