#Makefile for test
# AIX version

.SUFFIXES:
.SUFFIXES: .cpp .obj .o

CPP = g++


INCBASE = $(HOME)/include

INC= -I$(INCBASE) -I$(INCBASE)/kernel  -I$(INCBASE)/xdp -I$(FICC_DEPS)/include



CPPFLAGS = -Wall -g -m64 
CPPDEFS = -DUNIX  -DLINUX -DNDEBUG -D_THREAD_SAFE 

.cpp.o:
	$(CPP) $(CPPFLAGS) -c  $(CPPDEFS) $(INC) $< -o $@

OBJL =  main.o  ficc.pb.o   message_head.pb.o  


ALL:   testxdppb clean

DREBLIB = -L$(HOME)/lib   -lbf_kernel  -lbf_xdp  -lpthread -ldl -L$(FICC_DEPS)/lib -lprotobuf

testxdppb: $(OBJL)
	g++ $(CPPFLAGS) $(CPPDEFS) -o $@ $(OBJL) $(INC) -L$(HOME)/lib    $(CPPDEFS) $(DREBLIB)
	mv $@ $(HOME)/bin

clean:
	rm -f *.o
