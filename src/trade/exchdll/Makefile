# Makefile for test
# AIX version

.SUFFIXES:
.SUFFIXES: .cpp .obj .o

CPP = g++

INCBASE = $(HOME)/include

INC= -I../include    -I$(INCBASE)/xdp  -I$(INCBASE) -I$(INCBASE)/kernel -I$(INCBASE)/dbpub  -I$(INCBASE)/dbapi -I$(INCBASE)/dreb  -I$(INCBASE)/xml

CPPFLAGS =  -g -Wall  -O2 -fpic -m64
CPPDEFS =  -DUNIX -DLINUX -DAIX -Wall -D_THREAD_SAFE
DLLLIB  =  -lbf_kernel -lbf_dbpub -lbf_dbapi -lsqlapi    -lpthread

.cpp.o:
	$(CPP) $(CPPFLAGS) -c   $(CPPDEFS)  $(INC)  $< -o $@

OBJL = ExchDll.o  

ALL: libExchDll.so clean

libExchDll.so: $(OBJL)
	$(CPP) $(CPPFLAGS) $(CPPDEFS) -shared $(OBJL) -L$(HOME)/lib -L$(HOME)/libapp $(DLLLIB) -o $@ 
	mv $@ $(HOME)/libapp
libExchDll.a: $(OBJL)
	ar -rv $@ $(OBJL) 
	mv $@ $(HOME)/libapp
clean:
	rm -f *.o
