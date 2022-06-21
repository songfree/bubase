#Makefile for test
# AIX version

.SUFFIXES:
.SUFFIXES: .cpp .obj .o

CPP = g++

INCBASE = $(HOME)/include

KERNEL_INC= -I$(INCBASE) -I$(INCBASE)/kernel 

CPPFLAGS = -Wall -g -rdynamic -DLOGCURPATH -fPIC -m64 -O2
CPPDEFS =  -DLINUX -DNDEBUG -D_THREAD_SAFE -D_MT -DSA_USE_PTHREAD -D_REENTRANT  -D_ENGLISH_

.cpp.o:
	$(CPP) $(CPPFLAGS) -c  $(CPPDEFS) $(KERNEL_INC) $< -o $@

OBJL =  BF_BufferPool.o  BF_LogClient.o  BF_SimpleLog.o  BF_SocketUdp.o  BF_Time_Value.o  minilzo.o      XmlAttribute.o   XmlNode.o       XmlTransSign.o BF_Date_Time.o   BF_Md5.o        BF_Slist.o      BF_Thread.o     BF_Tools.o       ShareMemory.o  XmlAttributes.o  XmlNodeIndex.o  zip.o BF_Des.o  BF_3Des.o       BF_Mutex.o      BF_SocketTcp.o  BF_Timer.o      BF_Xml.o         wgetopt.o      XmlAttrIndex.o   XmlParse.o BF_DesEncrypt.o  BF_HostInfo.o BF_ProcessMgr.o

ALL:   libbf_kernel.so libbf_kernel.a clean

libbf_kernel.a: $(OBJL)
	ar rv $@ $(OBJL)
	mv $@ $(HOME)/lib
libbf_kernel.so: $(OBJL)
	g++ $(CPPFLAGS) $(CPPDEFS) -shared $(OBJL) -L$(HOME)/lib  -lpthread -ldl -o $@
	mv $@ $(HOME)/lib

clean:
	rm -f *.o
