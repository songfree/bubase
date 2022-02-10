#Makefile for test
# AIX version

.SUFFIXES:
.SUFFIXES: .cpp .obj .o

CPP = g++


INCBASE = $(HOME)/include

INC= -I$(INCBASE) -I$(INCBASE)/kernel -I$(INCBASE)/dbpub -I$(INCBASE)/dreb -I$(INCBASE)/cgateapi -I$(INCBASE)/gateway  -I$(INCBASE)/aio 



CPPFLAGS = -Wall -g -m64 
CPPDEFS = -DUNIX  -DLINUX -DNDEBUG -D_THREAD_SAFE 

.cpp.o:
	$(CPP) $(CPPFLAGS) -c  $(CPPDEFS) $(INC) $< -o $@

OBJL =  testapi.o  


ALL:   testcgate clean

DREBLIB = -L$(HOME)/lib   -lbf_kernel -lbf_dbpub -lbf_cgateapi -ldl

testcgate: $(OBJL)
	g++ $(CPPFLAGS) $(CPPDEFS) -o $@ $(OBJL) $(INC) -L$(HOME)/lib    $(CPPDEFS) $(DREBLIB)
	mv $@ $(HOME)/bin

clean:
	rm -f *.o
