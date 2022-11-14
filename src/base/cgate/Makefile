#Makefile for test
# AIX version

.SUFFIXES:
.SUFFIXES: .cpp .obj .o

CPP = g++


INCBASE = $(HOME)/include

INC= -I$(INCBASE) -I$(INCBASE)/kernel  -I$(INCBASE)/dreb -I$(INCBASE)/dbpub -I$(INCBASE)/aio -I$(INCBASE)/gateway


CPPFLAGS = -m64 -g -Wall  -O2 -fpic -std=c++11 
CPPDEFS = -m64 -DLINUX -DUNIX  -D_THREAD_SAFE 

.cpp.o:
	$(CPP) $(CPPFLAGS) -c  $(CPPDEFS) $(INC) $< -o $@

OBJL = DrebMsgThread.o  AIO_Work.o  GateResource.o  GateWay.o  MsgThread.o  PoolData.o    cgate.o CgateLog.o SendData.o SendThread.o MdbQuotation.o SubScribeInfo.o


ALL:   bfcgate clean



BPCLIB = -L$(HOME)/lib  -lbf_kernel  -lbf_drebapi  -lpthread -ldl -lbf_dbpub -lbf_aio

bfcgate: $(OBJL)
	g++ $(CPPFLAGS) $(CPPDEFS) -o $@ $(OBJL) $(INC) -L$(HOME)/lib    $(CPPDEFS) $(BPCLIB)
	mv $@ $(HOME)/bin
clean:
	rm -f *.o
