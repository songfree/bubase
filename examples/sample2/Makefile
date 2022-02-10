# Makefile for test
# AIX version

.SUFFIXES:
.SUFFIXES: .cpp .obj .o

CPP = g++

INCBASE = $(HOME)/include

INC= -I$(INCBASE) -I$(INCBASE)/xml -I$(INCBASE)/kernel -I$(INCBASE)/xml  -I$(INCBASE)/bumodule -I$(INCBASE)/dbapi -I$(INCBASE)/dptool -I$(INCBASE)/dbpub

CPPFLAGS =  -g -Wall  -O2 -fpic 
CPPDEFS =  -DUNIX -DLINUX -DAIX -Wall -D_THREAD_SAFE
DLLLIB  =   -lbf_kernel -lbf_dbpub -lbf_dbapi -lsqlapi  -lbf_bumodule   -lpthread

.cpp.o:
	$(CPP) $(CPPFLAGS) -c   $(CPPDEFS)  $(INC)  $< -o $@

OBJL = Sample.o 

ALL: libSample.so clean

libSample.so: $(OBJL)
	$(CPP) $(CPPFLAGS) $(CPPDEFS) -shared $(OBJL) -L$(HOME)/lib $(DLLLIB) -o $@ 
	mv $@ $(HOME)/libapp
libSample.a: $(OBJL)
	ar -rv $@ $(OBJL) 
	mv $@ $(HOME)/libapp
clean:
	rm -f *.o
