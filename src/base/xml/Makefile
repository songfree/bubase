#Makefile for test
# AIX version

.SUFFIXES:
.SUFFIXES: .cpp .obj .o

CPP = g++

INCBASE = $(HOME)/include

KERNEL_INC= -I$(INCBASE) -I$(INCBASE)/kernel -I$(INCBASE)/xml 

CPPFLAGS = -Wall -g -rdynamic -DLOGCURPATH -fPIC -m64 -std=c++11
CPPDEFS = -O2 -DUNIX -DLINUX -DNDEBUG -D_THREAD_SAFE -D_MT 

.cpp.o:
	$(CPP) $(CPPFLAGS) -c  $(CPPDEFS) $(KERNEL_INC) $< -o $@

OBJL =  attribute.o node.o xml.o xmlanalysis.o XmlPack.o XmlPackArray.o 

ALL:   libbf_oldxml.so libbf_oldxml.a  clean

libbf_oldxml.a: $(OBJL)
	ar rv $@ $(OBJL)
	mv $@ $(HOME)/lib
libbf_oldxml.so: $(OBJL)
	g++ $(CPPFLAGS) $(CPPDEFS) -shared $(OBJL) -L$(HOME)/lib   -lpthread -ldl -o $@
	mv $@ $(HOME)/lib
clean:
	rm -f *.o
