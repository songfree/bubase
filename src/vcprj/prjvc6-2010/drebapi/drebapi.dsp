# Microsoft Developer Studio Project File - Name="drebapi" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=drebapi - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "drebapi.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "drebapi.mak" CFG="drebapi - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "drebapi - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "drebapi - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "drebapi - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DREBAPI_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "D:\����\BUBASE�������\include\dreb" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DREBAPI_EXPORTS" /D "_ENGLISH_" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"../../../../vc6/lib/bf_drebapi.dll" /implib:"../../../../vc6/lib/bf_drebapi.lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "drebapi - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DREBAPI_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "D:\����\BUBASE�������\include\dreb" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DREBAPI_EXPORTS" /D "_ENGLISH_" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"../../../../vc6/lib/bf_drebapid.dll" /implib:"../../../../vc6/lib/bf_drebapid.lib" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "drebapi - Win32 Release"
# Name "drebapi - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=../../../../src/base/drebapi/BF_DrebResource.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/drebapi/BF_DrebSendQueue.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/drebapi/BF_DrebServer.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/drebapi/BF_DrebServerApi.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/drebapi/BF_DrebSocketInfo.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/drebapi/BF_DrebSocketMgr.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/drebapi/DrebClient.cpp
# End Source File
# Begin Source File

SOURCE=\BUBASE\src\base\drebapi\DrebClientApi.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=../../../../include/dreb/BF_DrebResource.h
# End Source File
# Begin Source File

SOURCE=../../../../include/dreb/BF_DrebSendQueue.h
# End Source File
# Begin Source File

SOURCE=../../../../include/dreb/BF_DrebServer.h
# End Source File
# Begin Source File

SOURCE=../../../../include/dreb/BF_DrebServerApi.h
# End Source File
# Begin Source File

SOURCE=../../../../include/dreb/BF_DrebSocketInfo.h
# End Source File
# Begin Source File

SOURCE=../../../../include/dreb/BF_DrebSocketMgr.h
# End Source File
# Begin Source File

SOURCE=../../../../include/dreb/DrebApiHead.h
# End Source File
# Begin Source File

SOURCE=../../../../include/dreb/DrebClient.h
# End Source File
# Begin Source File

SOURCE=\BUBASE\include\dreb\DrebClientApi.h
# End Source File
# Begin Source File

SOURCE=../../../../include/dreb/DrebMsgProcBase.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
