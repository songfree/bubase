#Makefile for test
# AIX version

.SUFFIXES:
.SUFFIXES: .cpp .obj .o

CPP = g++


INCBASE = $(HOME)/include

BU_INC= -I$(INCBASE) -I$(INCBASE)/kernel  -I$(INCBASE)/kvdata 


CPPFLAGS = -Wall -g -rdynamic -fPIC -m64 -std=c++11 
CPPDEFS = -DUNIX -DLINUX -DNDEBUG -D_THREAD_SAFE 

.cpp.o:
	$(CPP) $(CPPFLAGS) -c  $(CPPDEFS) $(BU_INC) $< -o $@

OBJL =   KvData.o  KVNode.o  
OBJL2 =  main.o 

ALL:  libbf_kvdata.so libbf_kvdata.a  clean

BCCLIB = -L$(HOME)/lib   -lbf_kernel -lpthread -ldl -lbf_kvdata

libbf_kvdata.a: $(OBJL)
	ar rv $@ $(OBJL)
	mv $@ $(HOME)/lib
libbf_kvdata.so: $(OBJL)
	g++ $(CPPFLAGS) $(CPPDEFS) -shared $(OBJL) -L$(HOME)/lib  -lpthread -ldl -lbf_kernel -o $@
	mv $@ $(HOME)/lib
testkvdata: $(OBJL2)
	g++ $(CPPFLAGS) $(CPPDEFS) -o $@ $(OBJL2) $(BU_INC) -L$(HOME)/lib    $(CPPDEFS) $(BCCLIB)
	mv $@ $(HOME)/bin
clean:
	rm -f *.o
