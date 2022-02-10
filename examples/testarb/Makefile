#Makefile for test
# AIX version

.SUFFIXES:
.SUFFIXES: .cpp .obj .o

CPP = g++


INCBASE = $(HOME)/include

INC= -I$(INCBASE) -I$(INCBASE)/kernel  -I$(INCBASE)/dreb -I$(INCBASE)/dbpub -I$(INCBASE)/xdp 



CPPFLAGS = -Wall -g -m64 
CPPDEFS = -DUNIX  -DLINUX -DNDEBUG -D_THREAD_SAFE 

.cpp.o:
	$(CPP) $(CPPFLAGS) -c  $(CPPDEFS) $(INC) $< -o $@

OBJL =  ArbMain.o  ArbResource.o   DrebMsgThread.o  arb.o  


ALL:   testarb clean

DREBLIB = -L$(HOME)/lib  -lbf_kernel  -lbf_xdp -lbf_drebapi -lpthread -ldl -lbf_dbpub

testarb: $(OBJL)
	g++ $(CPPFLAGS) $(CPPDEFS) -o $@ $(OBJL) $(INC) -L$(HOME)/lib    $(CPPDEFS) $(DREBLIB)
	mv $@ $(HOME)/bin

clean:
	rm -f *.o
