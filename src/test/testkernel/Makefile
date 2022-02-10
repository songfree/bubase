#Makefile for test
# AIX version

.SUFFIXES:
.SUFFIXES: .cpp .obj .o

CPP = g++


INCBASE = $(HOME)/include


KERNEL_INC= -I$(INCBASE) -I$(INCBASE)/kernel -I$(INCBASE)/xml -I$(INCBASE)/gateway -I$(INCBASE)/dbpub 



CPPFLAGS = -Wall -g -rdynamic -fPIC -m64 
CPPDEFS = -DUNIX -DLINUX -DNDEBUG -D_THREAD_SAFE 

.cpp.o:
	$(CPP) $(CPPFLAGS) -c  $(CPPDEFS) $(KERNEL_INC) $< -o $@

OBJL =  test.o 

ALL:   testkernel clean

BCCLIB = -L$(HOME)/lib   -lbf_kernel  -lpthread -ldl

testkernel: $(OBJL)
	g++ $(CPPFLAGS) $(CPPDEFS) -o $@ $(OBJL) $(KERNEL_INC) -L$(HOME)/lib    $(CPPDEFS) $(BCCLIB)
	mv $@ $(HOME)/bin
clean:
	rm -f *.o
