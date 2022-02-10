# Microsoft Developer Studio Project File - Name="kernel" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=kernel - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "kernel.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "kernel.mak" CFG="kernel - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "kernel - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "kernel - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "kernel - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "KERNEL_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "KERNEL_EXPORTS" /D "_ENGLISH_" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"../../../../vc6/lib/bf_kernel.dll" /implib:"../../../../vc6/lib/bf_kernel.lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "kernel - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "KERNEL_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "KERNEL_EXPORTS" /D "_ENGLISH_" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"../../../../vc6/lib/bf_kerneld.dll" /implib:"../../../../vc6/lib/bf_kerneld.lib" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "kernel - Win32 Release"
# Name "kernel - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=\BUBASE\src\base\kernel\BF_3Des.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/kernel/BF_BufferPool.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/kernel/BF_Date_Time.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/kernel/BF_Des.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/kernel/BF_DesEncrypt.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/kernel/BF_HostInfo.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/kernel/BF_LogClient.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/kernel/BF_Md5.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/kernel/BF_Mutex.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/kernel/BF_ProcessMgr.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/kernel/BF_SimpleLog.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/kernel/BF_Slist.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/kernel/BF_SocketTcp.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/kernel/BF_SocketUdp.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/kernel/BF_Thread.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/kernel/BF_Time_Value.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/kernel/BF_Timer.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/kernel/BF_Tools.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/kernel/BF_Xml.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/kernel/minilzo.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/kernel/ShareMemory.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/kernel/XmlAttribute.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/kernel/XmlAttributes.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/kernel/XmlAttrIndex.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/kernel/XmlNode.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/kernel/XmlNodeIndex.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/kernel/XmlParse.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/kernel/XmlTransSign.cpp
# End Source File
# Begin Source File

SOURCE=../../../../src/base/kernel/zip.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=\BUBASE\include\kernel\BF_3Des.h
# End Source File
# Begin Source File

SOURCE=../../../../include/kernel/BF_BufferPool.h
# End Source File
# Begin Source File

SOURCE=../../../../include/kernel/BF_Date_Time.h
# End Source File
# Begin Source File

SOURCE=../../../../include/kernel/BF_Des.h
# End Source File
# Begin Source File

SOURCE=../../../../include/kernel/BF_DesEncrypt.h
# End Source File
# Begin Source File

SOURCE=../../../../include/kernel/BF_HostInfo.h
# End Source File
# Begin Source File

SOURCE=../../../../include/kernel/BF_LogClient.h
# End Source File
# Begin Source File

SOURCE=../../../../include/kernel/BF_Md5.h
# End Source File
# Begin Source File

SOURCE=../../../../include/kernel/BF_Mutex.h
# End Source File
# Begin Source File

SOURCE=../../../../include/kernel/BF_ProcessMgr.h
# End Source File
# Begin Source File

SOURCE=../../../../include/kernel/BF_Queue.h
# End Source File
# Begin Source File

SOURCE=../../../../include/kernel/BF_SimpleLog.h
# End Source File
# Begin Source File

SOURCE=../../../../include/kernel/BF_Slist.h
# End Source File
# Begin Source File

SOURCE=../../../../include/kernel/BF_Socket.h
# End Source File
# Begin Source File

SOURCE=../../../../include/kernel/BF_SocketTcp.h
# End Source File
# Begin Source File

SOURCE=../../../../include/kernel/BF_SocketUdp.h
# End Source File
# Begin Source File

SOURCE=../../../../include/kernel/BF_Thread.h
# End Source File
# Begin Source File

SOURCE=../../../../include/kernel/BF_Time_Value.h
# End Source File
# Begin Source File

SOURCE=../../../../include/kernel/BF_Timer.h
# End Source File
# Begin Source File

SOURCE=../../../../include/kernel/BF_Tools.h
# End Source File
# Begin Source File

SOURCE=../../../../include/kernel/BF_Xml.h
# End Source File
# Begin Source File

SOURCE=../../../../include/kernel/BF_Zip.h
# End Source File
# Begin Source File

SOURCE=../../../../include/kernel/IErrLog.h
# End Source File
# Begin Source File

SOURCE=../../../../include/kernel/ISqlApi.h
# End Source File
# Begin Source File

SOURCE=../../../../include/kernel/IXmlNode.h
# End Source File
# Begin Source File

SOURCE=../../../../include/kernel/IXmlParse.h
# End Source File
# Begin Source File

SOURCE=../../../../include/kernel/MdbBase.h
# End Source File
# Begin Source File

SOURCE=../../../../include/kernel/public.h
# End Source File
# Begin Source File

SOURCE=../../../../include/kernel/ShareMemory.h
# End Source File
# Begin Source File

SOURCE=../../../../include/kernel/typedefine.h
# End Source File
# Begin Source File

SOURCE=../../../../include/kernel/wgetopt.h
# End Source File
# Begin Source File

SOURCE=../../../../include/kernel/XmlAttribute.h
# End Source File
# Begin Source File

SOURCE=../../../../include/kernel/XmlAttributes.h
# End Source File
# Begin Source File

SOURCE=../../../../include/kernel/XmlAttrIndex.h
# End Source File
# Begin Source File

SOURCE=../../../../include/kernel/XmlHead.h
# End Source File
# Begin Source File

SOURCE=../../../../include/kernel/XmlNode.h
# End Source File
# Begin Source File

SOURCE=../../../../include/kernel/XmlNodeIndex.h
# End Source File
# Begin Source File

SOURCE=../../../../include/kernel/XmlParse.h
# End Source File
# Begin Source File

SOURCE=../../../../include/kernel/XmlTransSign.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
