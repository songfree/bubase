#Makefile for test
# AIX version

.SUFFIXES:
.SUFFIXES: .cpp .obj .o

CPP = g++


INCBASE = $(HOME)/include

BU_INC= -I$(INCBASE) -I$(INCBASE)/kernel  -I$(INCBASE)/gtp -I$(INCBASE)/dbpub 


CPPFLAGS = -Wall -g -rdynamic -fPIC -m64 
CPPDEFS = -DUNIX -DLINUX -DNDEBUG -D_THREAD_SAFE 

.cpp.o:
	$(CPP) $(CPPFLAGS) -c  $(CPPDEFS) $(BU_INC) $< -o $@

OBJL =   GtpData.o  GtpProtocol.o  
OBJL2 =  main.o 

ALL:  libbf_gtp.so libbf_gtp.a testgtp clean

BCCLIB = -L$(HOME)/lib   -lbf_kernel -lpthread -ldl -lbf_gtp -lbf_dbpub

libbf_gtp.a: $(OBJL)
	ar rv $@ $(OBJL)
	mv $@ $(HOME)/lib
libbf_gtp.so: $(OBJL)
	g++ $(CPPFLAGS) $(CPPDEFS) -shared $(OBJL) -L$(HOME)/lib  -lpthread -ldl -lbf_kernel -lbf_dbpub -o $@
	mv $@ $(HOME)/lib
testgtp: $(OBJL2)
	g++ $(CPPFLAGS) $(CPPDEFS) -o $@ $(OBJL2) $(BU_INC) -L$(HOME)/lib    $(CPPDEFS) $(BCCLIB)
	mv $@ $(HOME)/bin
clean:
	rm -f *.o
