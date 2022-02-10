# Microsoft Developer Studio Project File - Name="BPC" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=BPC - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "BPC.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "BPC.mak" CFG="BPC - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "BPC - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "BPC - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "BPC - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../../../include/bfxml" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "_WINDOWS" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 psapi.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /out:"../../../../vc6/bin/bfbpc.exe"

!ELSEIF  "$(CFG)" == "BPC - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../../include/bfxml" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_WINDOWS" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 psapi.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /out:"../../../../vc6/bin/bfbpcd.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "BPC - Win32 Release"
# Name "BPC - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=../../../../src/base/bpc/bpc.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/bpc/Bpc_Timer.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/bpc/BpcRun.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/bpc/DispathThread.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/bpc/FuncTbl.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/bpc/GResource.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/bpc/HostInfo.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/bpc/LinkThread.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/bpc/MsgThread.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/bpc/NextTbl.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/bpc/PoolData.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/bpc/ProcessMgr.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/bpc/PuThread.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/bpc/SendQueue.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/bpc/SocketInfo.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/bpc/SocketMgr.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=../../../../src/base/bpc/Bpc_Timer.h
# End Source File
# Begin Source File

SOURCE=../../../../src/base/bpc/BpcRun.h
# End Source File
# Begin Source File

SOURCE=../../../../src/base/bpc/DispathThread.h
# End Source File
# Begin Source File

SOURCE=../../../../src/base/bpc/FuncTbl.h
# End Source File
# Begin Source File

SOURCE=../../../../src/base/bpc/GResource.h
# End Source File
# Begin Source File

SOURCE=../../../../src/base/bpc/HostInfo.h
# End Source File
# Begin Source File

SOURCE=../../../../src/base/bpc/IGRes.h
# End Source File
# Begin Source File

SOURCE=../../../../src/base/bpc/LinkThread.h
# End Source File
# Begin Source File

SOURCE=../../../../src/base/bpc/MsgThread.h
# End Source File
# Begin Source File

SOURCE=../../../../src/base/bpc/NextTbl.h
# End Source File
# Begin Source File

SOURCE=../../../../src/base/bpc/PoolData.h
# End Source File
# Begin Source File

SOURCE=../../../../src/base/bpc/ProcessMgr.h
# End Source File
# Begin Source File

SOURCE=../../../../src/base/bpc/PSAPI.H
# End Source File
# Begin Source File

SOURCE=../../../../src/base/bpc/PuThread.h
# End Source File
# Begin Source File

SOURCE=../../../../src/base/bpc/SendQueue.h
# End Source File
# Begin Source File

SOURCE=../../../../src/base/bpc/SocketInfo.h
# End Source File
# Begin Source File

SOURCE=../../../../src/base/bpc/SocketMgr.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
