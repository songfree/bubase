# Microsoft Developer Studio Project File - Name="cgate" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=cgate - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "cgate.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "cgate.mak" CFG="cgate - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "cgate - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "cgate - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "cgate - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "_WINDOWS" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /out:"../../../../vc6/bin/bfcgate.exe"

!ELSEIF  "$(CFG)" == "cgate - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_WINDOWS" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /out:"../../../../vc6/bin/bfcgated.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "cgate - Win32 Release"
# Name "cgate - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=../../../../src/base/cgate/AIO_Work.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/cgate/Bpc_Timer.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/cgate/cgate.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/cgate/CgateLog.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/cgate/GateResource.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/cgate/GateWay.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/cgate/HostInfo.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/cgate/LinkThread.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/cgate/MdbQuotation.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/cgate/MsgThread.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/cgate/PoolData.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/cgate/SendData.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/cgate/SendQueue.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/cgate/SendThread.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/cgate/SocketInfo.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/cgate/SocketMgr.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/cgate/SubScribeInfo.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=../../../../src/base/cgate/AIO_Work.h
# End Source File
# Begin Source File

SOURCE=../../../../src/base/cgate/Bpc_Timer.h
# End Source File
# Begin Source File

SOURCE=../../../../src/base/cgate/CgateLog.h
# End Source File
# Begin Source File

SOURCE=../../../../src/base/cgate/GateResource.h
# End Source File
# Begin Source File

SOURCE=../../../../src/base/cgate/GateWay.h
# End Source File
# Begin Source File

SOURCE=../../../../src/base/cgate/HostInfo.h
# End Source File
# Begin Source File

SOURCE=../../../../src/base/cgate/LinkThread.h
# End Source File
# Begin Source File

SOURCE=../../../../src/base/cgate/MdbQuotation.h
# End Source File
# Begin Source File

SOURCE=../../../../src/base/cgate/MsgThread.h
# End Source File
# Begin Source File

SOURCE=../../../../src/base/cgate/PoolData.h
# End Source File
# Begin Source File

SOURCE=../../../../src/base/cgate/SendData.h
# End Source File
# Begin Source File

SOURCE=../../../../src/base/cgate/SendQueue.h
# End Source File
# Begin Source File

SOURCE=../../../../src/base/cgate/SendThread.h
# End Source File
# Begin Source File

SOURCE=../../../../src/base/cgate/SocketInfo.h
# End Source File
# Begin Source File

SOURCE=../../../../src/base/cgate/SocketMgr.h
# End Source File
# Begin Source File

SOURCE=../../../../src/base/cgate/SubScribeInfo.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
