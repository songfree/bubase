#Makefile for test
# AIX version

.SUFFIXES:
.SUFFIXES: .cpp .obj .o

CPP = g++

INCBASE = $(HOME)/include

KERNEL_INC= -I$(INCBASE) -I$(INCBASE)/kernel -I$(INCBASE)/gateway 

CPPFLAGS = -Wall -g -rdynamic -DLOGCURPATH -fPIC -m64
CPPDEFS = -DUNIX -DLINUX -DNDEBUG -D_THREAD_SAFE -D_MT 

.cpp.o:
	$(CPP) $(CPPFLAGS) -c  $(CPPDEFS) $(KERNEL_INC) $< -o $@

OBJL =  ShcaApi.o ShcaApiML.o  
OBJL2 =  ShaApi.o main.o

ALL:   libshcaapi.so libshcaapi.a  clean

testshcaapi: $(OBJL2)
	g++ $(CPPFLAGS) $(CPPDEFS) -o $@ $(OBJL2) $(INC) -L$(HOME)/lib   -lpthread -ldl $(CPPDEFS) 
	mv $@ $(HOME)/bin
libshcaapi.a: $(OBJL)
	ar rv $@ $(OBJL)
	mv $@ $(HOME)/lib
libshcaapi.so: $(OBJL)
	g++ $(CPPFLAGS) $(CPPDEFS) -shared $(OBJL) -L$(HOME)/lib   -lpthread -ldl -o $@
	mv $@ $(HOME)/lib
clean:
	rm -f *.o
