#Makefile for test
# AIX version

.SUFFIXES:
.SUFFIXES: .cpp .obj .o

CPP = g++

INCBASE = $(HOME)/include

KERNEL_INC= -I$(INCBASE) -I$(INCBASE)/kernel -I$(INCBASE)/dbpub -I$(INCBASE)/xdp

CPPFLAGS = -Wall -g -rdynamic -DLOGCURPATH -fPIC -m64
CPPDEFS = -O2 -DLINUX -DNDEBUG -D_THREAD_SAFE -D_MT -DSA_USE_PTHREAD -D_REENTRANT 

.cpp.o:
	$(CPP) $(CPPFLAGS) -c  $(CPPDEFS) $(KERNEL_INC) $< -o $@

OBJL = Xdp.o XdpFmt.o  XdpRecord.o  

ALL:   libbf_xdp.so libbf_xdp.a clean

libbf_xdp.a: $(OBJL)
	ar rv $@ $(OBJL)
	mv $@ $(HOME)/lib
libbf_xdp.so: $(OBJL)
	g++ $(CPPFLAGS) $(CPPDEFS) -shared $(OBJL) -L$(HOME)/lib  -lpthread -ldl -lbf_kernel -lbf_dbpub  -o $@
	mv $@ $(HOME)/lib

clean:
	rm -f *.o
