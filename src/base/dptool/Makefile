# Makefile for test
# AIX version

.SUFFIXES:
.SUFFIXES: .cpp .obj .o

CPP = g++

INCBASE = $(HOME)/include

INC= -I$(INCBASE) -I$(INCBASE)/kernel -I$(INCBASE)/xml  -I$(INCBASE)/dptool 

CPPDEFS = -O2 -DUNIX -D_MT -DLINUX  -Wall -D_THREAD_SAFE -fPIC -m64

.cpp.o:
	$(CPP) -c   $(CPPDEFS) -g -rdynamic $(INC)  $< -o $@

OBJL =  RtfGenerate.o  InterfacePack.o PackConvert.o TransPackNew.o

ALL:  libbf_dptool.so libbf_dptool.a clean

libbf_dptool.so: $(OBJL)
	g++ $(CPPFLAGS) $(CPPDEFS) -shared $(OBJL) -L$(HOME)/lib -lbf_kernel -lbf_oldxml -lpthread  -o $@ 
	mv $@ $(HOME)/lib
libbf_dptool.a: $(OBJL)
	ar -rv $@ $(OBJL) 
	mv $@ $(HOME)/lib
clean:
	rm -f *.o
