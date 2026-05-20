#Makefile for test
# AIX version

.SUFFIXES:
.SUFFIXES: .cpp .obj .o

CPP = g++


INCBASE = $(HOME)/include


KERNEL_INC= -I$(INCBASE) -I$(INCBASE)/kernel -I$(INCBASE)/xml -I$(INCBASE)/gateway -I$(INCBASE)/dbpub -I$(INCBASE)/dbapi 



CPPFLAGS = -Wall -g -rdynamic -fPIC -m64 -std=c++11
CPPDEFS = -DUNIX -DLINUX -DNDEBUG -D_THREAD_SAFE 

.cpp.o:
	$(CPP) $(CPPFLAGS) -c  $(CPPDEFS) $(KERNEL_INC) $< -o $@

OBJL =  test.o 
OBJL2 =  testqueue.o 
OBJL3 =  testkernel.o

ALL:   testmutex clean

BCCLIB = -L$(HOME)/lib   -lbf_kernel  -lbf_dbapi -lsqlapi -lpthread -ldl

testkernel: $(OBJL3)
	g++ $(CPPFLAGS) $(CPPDEFS) -o $@ $(OBJL3) $(KERNEL_INC) -L$(HOME)/lib    $(CPPDEFS) $(BCCLIB)
	mv $@ $(HOME)/bin
testqueue: $(OBJL2)
	g++ $(CPPFLAGS) $(CPPDEFS) -o $@ $(OBJL2) $(KERNEL_INC) -L$(HOME)/lib    $(CPPDEFS) -lbf_kernel -lpthread
	mv $@ $(HOME)/bin
testmutex: $(OBJL)
	g++ $(CPPFLAGS) $(CPPDEFS) -o $@ $(OBJL) $(KERNEL_INC) -L$(HOME)/lib    $(CPPDEFS) $(BCCLIB) -lbf_cgateapi
	mv $@ $(HOME)/bin
clean:
	rm -f *.o
