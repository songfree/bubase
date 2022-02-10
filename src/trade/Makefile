#Makefile for test
# AIX version

.SUFFIXES:
.SUFFIXES: .cpp .obj .o

CPP = g++


INCBASE = $(HOME)/include

BU_INC= -I$(INCBASE) -I$(INCBASE)/kernel -I$(INCBASE)/dbpub -I$(INCBASE)/dreb -I$(INCBASE)/dbapi -I$(INCBASE)/drebapi -I$(INCBASE)/xdp -I./include 


CPPFLAGS = -Wall -g -rdynamic -fPIC -m64 
CPPDEFS = -DUNIX -DLINUX -DNDEBUG -D_THREAD_SAFE 

.cpp.o:
	$(CPP) $(CPPFLAGS) -c  $(CPPDEFS) $(BU_INC) $< -o $@

OBJL =   DrebMsgThread.o  PoolModule.o  SendQueue.o   SocketMgr.o  TradeResource.o PoolData.o       ProcThread.o  SocketInfo.o  TradeMain.o  main.o

ALL:   tradekernel clean

BCCLIB = -L$(HOME)/lib   -lbf_kernel -lbf_dbpub -lbf_fibapi -lbf_dbapi  -lbf_xdp   -lpthread -ldl

tradekernel: $(OBJL)
	g++ $(CPPFLAGS) $(CPPDEFS) -o $@ $(OBJL) $(KERNEL_INC) -L$(HOME)/lib    $(CPPDEFS) $(BCCLIB)
	mv $@ $(HOME)/bin
clean:
	rm -f *.o
