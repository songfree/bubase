#Makefile for test
# AIX version

.SUFFIXES:
.SUFFIXES: .cpp .obj .o

CPP = g++


INCBASE = $(HOME)/include

BU_INC= -I$(INCBASE) -I$(INCBASE)/kernel -I$(INCBASE)/dbapi -I$(INCBASE)/xml -I$(INCBASE)/bumodule -I$(INCBASE)/dbpub 


CPPFLAGS = -Wall -g -rdynamic -fPIC -m64 -std=c++11
CPPDEFS = -O2 -DUNIX -DLINUX -DNDEBUG -D_THREAD_SAFE 

.cpp.o:
	$(CPP) $(CPPFLAGS) -c  $(CPPDEFS) $(BU_INC) $< -o $@

OBJL = BuLink.o  BuRes.o bfbu.o 

ALL:   bffpu clean

BULIB = -L$(HOME)/lib  -lbf_bumodule  -lbf_kernel -lbf_oldxml -lbf_dbapi -lsqlapi -lbf_dbpub -lpthread  -ldl


bffpu: $(OBJL)
	g++ $(CPPFLAGS) $(CPPDEFS) -o $@ $(OBJL) $(BU_INC) -L$(HOME)/lib    $(CPPDEFS) $(BULIB)
	mv $@ $(HOME)/bin
clean:
	rm -f *.o
