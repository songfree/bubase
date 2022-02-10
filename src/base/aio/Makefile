#Makefile for test
# AIX version

.SUFFIXES:
.SUFFIXES: .cpp .obj .o

CPP = g++

INCBASE = $(HOME)/include

KERNEL_INC= -I$(INCBASE) -I$(INCBASE)/kernel -I$(INCBASE)/dbpub -I$(INCBASE)/aio  -I$(INCBASE)/dreb 

CPPFLAGS = -Wall -g -rdynamic -DLOGCURPATH -fPIC -m64
CPPDEFS = -O2 -DLINUX -DNDEBUG -D_THREAD_SAFE -D_MT -DSA_USE_PTHREAD -D_REENTRANT 

.cpp.o:
	$(CPP) $(CPPFLAGS) -c  $(CPPDEFS) $(KERNEL_INC) $< -o $@

OBJL = BF_AIO.o  BF_AIO_Pool.o  BF_AIO_Resource.o  BF_AIO_Thread.o  BF_AIO_WorkBase.o 

ALL:   libbf_aio.so libbf_aio.a clean

libbf_aio.a: $(OBJL)
	ar rv $@ $(OBJL)
	mv $@ $(HOME)/lib
libbf_aio.so: $(OBJL)
	g++ $(CPPFLAGS) $(CPPDEFS) -shared $(OBJL) -L$(HOME)/lib  -lpthread -ldl -lbf_kernel -lbf_dbpub  -o $@
	mv $@ $(HOME)/lib

clean:
	rm -f *.o
