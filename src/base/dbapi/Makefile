# Makefile for test
# AIX version

.SUFFIXES:
.SUFFIXES: .cpp .obj .o

CPP = g++

INCBASE = $(HOME)/include

INC= -I$(INCBASE) -I$(INCBASE)/kernel -I$(INCBASE)/dbapi 


CPPFLAGS = -Wall -g -rdynamic -fPIC -m64 -std=c++11
CPPDEFS = -O2 -DUNIX -DLINUX -DNDEBUG -D_THREAD_SAFE -D_MT 


.cpp.o:
	$(CPP) $(CPPFLAGS) -c   $(CPPDEFS)  $(INC)  $< -o $@

OBJL =  DbApi.o SqlDataResult.o 

ALL: libbf_dbapi.so libbf_dbapi.a clean

libbf_dbapi.so: $(OBJL)
	g++ $(CPPFLAGS) $(CPPDEFS)  -shared $(OBJL) -L$(HOME)/lib   -lsqlapi  -o $@ 
	mv $@ $(HOME)/lib
libbf_dbapi.a: $(OBJL)
	ar -rv $@ $(OBJL) 
	mv $@ $(HOME)/lib
clean:
	rm -f *.o
