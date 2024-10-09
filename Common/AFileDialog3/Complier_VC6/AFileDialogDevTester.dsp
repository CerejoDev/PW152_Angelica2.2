# Microsoft Developer Studio Project File - Name="AFileDialogDevTester" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=AFileDialogDevTester - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AFileDialogDevTester.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AFileDialogDevTester.mak" CFG="AFileDialogDevTester - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AFileDialogDevTester - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "AFileDialogDevTester - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AFileDialogDevTester - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Build\AFileDialogTester\Release"
# PROP Intermediate_Dir "Build\AFileDialogTester\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\AFileDialogDll" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 AFileDialog2.lib Mpg123Lib.lib dxguid.lib dsound.lib ImmWrapper.lib d3d8.lib d3dx8.lib ZLibLib.lib Angelica3D.lib AngelicaCommon.lib AngelicaFile.lib AngelicaMedia.lib ogg_static.lib vorbis_static.lib vorbisfile_static.lib /nologo /subsystem:windows /machine:I386 /out:"../最新编辑器/AFileDialogDevTester.exe" /libpath:"..\AFileDialogDll\Release\\"

!ELSEIF  "$(CFG)" == "AFileDialogDevTester - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Build\AFileDialogDevTester\Debug"
# PROP Intermediate_Dir "Build\AFileDialogDevTester\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\AFileDialogDll" /D "_PROJ_IN_ANGELICA_2" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 AFileDialog2_d.lib Mpg123Lib_d.lib dxguid.lib dsound.lib ImmWrapper_d.lib d3d8.lib d3dx8.lib ZLibLib_d.lib Angelica3D_d.lib AngelicaCommon_d.lib AngelicaFile_d.lib AngelicaMedia_d.lib ogg_static_d.lib vorbis_static_d.lib vorbisfile_static_d.lib /nologo /subsystem:windows /pdb:"../Bin_VC6/AFileDialogDevTester_d.pdb" /debug /machine:I386 /out:"../Bin_VC6/AFileDialogDevTester_d.exe" /pdbtype:sept /libpath:"..\AFileDialogDll\Debug\\"
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy ..\bin_VC6\AFileDialogDevTester_d.exe ..\..\..\最新编辑器
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "AFileDialogDevTester - Win32 Release"
# Name "AFileDialogDevTester - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\AFileDialogTester\AFileDialogDevTester.cpp
# End Source File
# Begin Source File

SOURCE=..\AFileDialogTester\AFileDialogDevTester.rc
# End Source File
# Begin Source File

SOURCE=..\AFileDialogTester\AFileDialogDevTesterDoc.cpp
# End Source File
# Begin Source File

SOURCE=..\AFileDialogTester\AFileDialogDevTesterView.cpp
# End Source File
# Begin Source File

SOURCE=..\AFileDialogTester\Global.cpp
# End Source File
# Begin Source File

SOURCE=..\AFileDialogTester\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=..\AFileDialogTester\PreviewWnd.cpp
# End Source File
# Begin Source File

SOURCE=..\AFileDialogTester\Render.cpp
# End Source File
# Begin Source File

SOURCE=..\AFileDialogTester\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\AFileDialogTester\AFileDialogDevTester.h
# End Source File
# Begin Source File

SOURCE=..\AFileDialogTester\AFileDialogDevTesterDoc.h
# End Source File
# Begin Source File

SOURCE=..\AFileDialogTester\AFileDialogDevTesterView.h
# End Source File
# Begin Source File

SOURCE=..\AFileDialogTester\Global.h
# End Source File
# Begin Source File

SOURCE=..\AFileDialogTester\MainFrm.h
# End Source File
# Begin Source File

SOURCE=..\AFileDialogTester\PreviewWnd.h
# End Source File
# Begin Source File

SOURCE=..\AFileDialogTester\Render.h
# End Source File
# Begin Source File

SOURCE=..\AFileDialogTester\resource.h
# End Source File
# Begin Source File

SOURCE=..\AFileDialogTester\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\AFileDialogTester\res\AFileDialogDevTester.ico
# End Source File
# Begin Source File

SOURCE=..\AFileDialogTester\res\AFileDialogDevTester.rc2
# End Source File
# Begin Source File

SOURCE=..\AFileDialogTester\res\AFileDialogDevTesterDoc.ico
# End Source File
# Begin Source File

SOURCE=..\AFileDialogTester\res\Toolbar.bmp
# End Source File
# End Group
# End Target
# End Project
