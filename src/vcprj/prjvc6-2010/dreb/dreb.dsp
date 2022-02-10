# Microsoft Developer Studio Project File - Name="dreb" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=dreb - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "dreb.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dreb.mak" CFG="dreb - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dreb - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "dreb - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "dreb - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../../../include/kernel" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "_WINDOWS" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /out:"../../../../vc6/bin/bfdreb.exe"

!ELSEIF  "$(CFG)" == "dreb - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../../include/kernel" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_WINDOWS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /out:"../../../../vc6/bin/bfdrebd.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "dreb - Win32 Release"
# Name "dreb - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=../../../../src/base/dreb/BFDreb.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/dreb/dreb.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/dreb/Dreb_Timer.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/dreb/DrebStatInfo.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/dreb/DrebTbl.cpp
# End Source File
# Begin Source File

SOURCE=.\FuncRoute.cpp
# End Source File
# Begin Source File

SOURCE=.\FuncTbl.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/dreb/GlobalVar.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/dreb/MemDB.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/dreb/MsgProcThread.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/dreb/PoolData.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/dreb/PoolModule.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/dreb/RouteTbl.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/dreb/SendQueue.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/dreb/ServiceTbl.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/dreb/SocketInfo.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/dreb/SocketMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\SvrRoute.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/dreb/SvrTbl.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=../../../../src/base/dreb/BFDreb.h
# End Source File
# Begin Source File

SOURCE=../../../../src/base/dreb/Dreb_Timer.h
# End Source File
# Begin Source File

SOURCE=../../../../include/dreb/dreberrcode.h
# End Source File
# Begin Source File

SOURCE=../../../../src/base/dreb/DrebStatInfo.h
# End Source File
# Begin Source File

SOURCE=../../../../src/base/dreb/DrebTbl.h
# End Source File
# Begin Source File

SOURCE=.\FuncRoute.h
# End Source File
# Begin Source File

SOURCE=.\FuncTbl.h
# End Source File
# Begin Source File

SOURCE=../../../../src/base/dreb/GlobalVar.h
# End Source File
# Begin Source File

SOURCE=../../../../src/base/dreb/MemDB.h
# End Source File
# Begin Source File

SOURCE=../../../../src/base/dreb/MsgProcThread.h
# End Source File
# Begin Source File

SOURCE=../../../../src/base/dreb/PoolData.h
# End Source File
# Begin Source File

SOURCE=../../../../src/base/dreb/PoolModule.h
# End Source File
# Begin Source File

SOURCE=../../../../src/base/dreb/RouteTbl.h
# End Source File
# Begin Source File

SOURCE=../../../../src/base/dreb/SendQueue.h
# End Source File
# Begin Source File

SOURCE=../../../../src/base/dreb/ServiceTbl.h
# End Source File
# Begin Source File

SOURCE=../../../../src/base/dreb/SocketInfo.h
# End Source File
# Begin Source File

SOURCE=../../../../src/base/dreb/SocketMgr.h
# End Source File
# Begin Source File

SOURCE=.\SvrRoute.h
# End Source File
# Begin Source File

SOURCE=../../../../src/base/dreb/SvrTbl.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
