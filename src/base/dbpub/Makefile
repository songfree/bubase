#Makefile for test
# AIX version

.SUFFIXES:
.SUFFIXES: .cpp .obj .o

CPP = g++

INCBASE = $(HOME)/include

KERNEL_INC= -I$(INCBASE) -I$(INCBASE)/kernel -I$(INCBASE)/dbpub

CPPFLAGS = -Wall -g -rdynamic -DLOGCURPATH -fPIC -m64 -std=c++11
CPPDEFS =  -O2 -DLINUX -DNDEBUG -D_THREAD_SAFE -D_MT -DSA_USE_PTHREAD -D_REENTRANT 

.cpp.o:
	$(CPP) $(CPPFLAGS) -c  $(CPPDEFS) $(KERNEL_INC) $< -o $@

OBJL =  BpcEndian.o  DrebBpcLog.o  DrebEndian.o EncryptCommBuffer.o BF_CommDataLog.o

ALL:   libbf_dbpub.so libbf_dbpub.a clean

libbf_dbpub.a: $(OBJL)
	ar rv $@ $(OBJL)
	mv $@ $(HOME)/lib
libbf_dbpub.so: $(OBJL)
	g++ $(CPPFLAGS) $(CPPDEFS) -shared $(OBJL) -L$(HOME)/lib  -lpthread -ldl -lbf_kernel -o $@
	mv $@ $(HOME)/lib

clean:
	rm -f *.o
