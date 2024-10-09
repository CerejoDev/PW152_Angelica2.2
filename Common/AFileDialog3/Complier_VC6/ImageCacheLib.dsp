# Microsoft Developer Studio Project File - Name="ImageCacheLib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=ImageCacheLib - Win32 UnicodeDebug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ImageCacheLib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ImageCacheLib.mak" CFG="ImageCacheLib - Win32 UnicodeDebug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ImageCacheLib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "ImageCacheLib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "ImageCacheLib - Win32 UnicodeRelease" (based on "Win32 (x86) Static Library")
!MESSAGE "ImageCacheLib - Win32 UnicodeDebug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/AFileDialog3/Complier_VC6", ZDYAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ImageCacheLib - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Lib_VC6"
# PROP Intermediate_Dir "Build\ImageCacheLib\Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\ximageinclude" /I "..\..\LuaWrapper\Include" /I "..\..\..\Projects2\Angelica2\Output\3rdSDK\Include\Gfx" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_A_FORBID_NEWDELETE" /D "_A_FORBID_MALLOC" /D "_ANGELICA2" /Yu"ICLStdAfx.h" /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "ImageCacheLib - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Lib_VC6"
# PROP Intermediate_Dir "Build\ImageCacheLib\Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\ximageinclude" /I "..\..\LuaWrapper\Include" /I "..\..\..\Projects2\Angelica2\Output\3rdSDK\Include\Gfx" /D "_A_FORBID_NEWDELETE" /D "_A_FORBID_MALLOC" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_ANGELICA2" /Yu"ICLStdAfx.h" /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\Lib_VC6\ImageCacheLib_d.lib"

!ELSEIF  "$(CFG)" == "ImageCacheLib - Win32 UnicodeRelease"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ImageCacheLib___Win32_UnicodeRelease"
# PROP BASE Intermediate_Dir "ImageCacheLib___Win32_UnicodeRelease"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Lib_VC6"
# PROP Intermediate_Dir "Build\ImageCacheLib\UnicodeRelease"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "..\ximageinclude" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_A_FORBID_NEWDELETE" /D "_A_FORBID_MALLOC" /D "_PROJ_IN_ANGELICA_2" /Yu"ICLStdAfx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\ximageinclude" /I "..\..\LuaWrapper\Include" /I "..\..\..\Projects2\Angelica2\Output\3rdSDK\Include\Gfx" /D "_UNICODE" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_A_FORBID_NEWDELETE" /D "_A_FORBID_MALLOC" /D "_ANGELICA2" /Yu"ICLStdAfx.h" /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\Lib_VC6\ImageCacheLib_u.lib"

!ELSEIF  "$(CFG)" == "ImageCacheLib - Win32 UnicodeDebug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ImageCacheLib___Win32_UnicodeDebug"
# PROP BASE Intermediate_Dir "ImageCacheLib___Win32_UnicodeDebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Lib_VC6"
# PROP Intermediate_Dir "Build\ImageCacheLib\UnicodeDebug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\ximageinclude" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_A_FORBID_NEWDELETE" /D "_A_FORBID_MALLOC" /D "_PROJ_IN_ANGELICA_2" /Yu"ICLStdAfx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\ximageinclude" /I "..\..\LuaWrapper\Include" /I "..\..\..\Projects2\Angelica2\Output\3rdSDK\Include\Gfx" /D "_UNICODE" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_A_FORBID_NEWDELETE" /D "_A_FORBID_MALLOC" /D "_ANGELICA2" /Yu"ICLStdAfx.h" /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\Lib_VC6\ImageCacheLib_d.lib"
# ADD LIB32 /nologo /out:"..\Lib_VC6\ImageCacheLib_ud.lib"

!ENDIF 

# Begin Target

# Name "ImageCacheLib - Win32 Release"
# Name "ImageCacheLib - Win32 Debug"
# Name "ImageCacheLib - Win32 UnicodeRelease"
# Name "ImageCacheLib - Win32 UnicodeDebug"
# Begin Group "ICLThreadPool"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ImageCacheLib\ICLThreadPool.cpp
# End Source File
# Begin Source File

SOURCE=..\ImageCacheLib\ICLThreadPool.h
# End Source File
# Begin Source File

SOURCE=..\ImageCacheLib\ICLThreadTask.cpp
# End Source File
# Begin Source File

SOURCE=..\ImageCacheLib\ICLThreadTask.h
# End Source File
# Begin Source File

SOURCE=..\ImageCacheLib\ICLThreadWrapper.cpp
# End Source File
# Begin Source File

SOURCE=..\ImageCacheLib\ICLThreadWrapper.h
# End Source File
# End Group
# Begin Group "ICLImg"

# PROP Default_Filter ""
# Begin Group "ICLImg_Type"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ImageCacheLib\ICLImgImplDDS.cpp
# End Source File
# Begin Source File

SOURCE=..\ImageCacheLib\ICLImgImplDDS.h
# End Source File
# Begin Source File

SOURCE=..\ImageCacheLib\ICLImgImplECM.cpp
# End Source File
# Begin Source File

SOURCE=..\ImageCacheLib\ICLImgImplECM.h
# End Source File
# Begin Source File

SOURCE=..\ImageCacheLib\ICLImgImplGfx.cpp
# End Source File
# Begin Source File

SOURCE=..\ImageCacheLib\ICLImgImplGfx.h
# End Source File
# Begin Source File

SOURCE=..\ImageCacheLib\ICLImgImplSMD.cpp
# End Source File
# Begin Source File

SOURCE=..\ImageCacheLib\ICLImgImplSMD.h
# End Source File
# Begin Source File

SOURCE=..\ImageCacheLib\ICLImgImplUMD.cpp
# End Source File
# Begin Source File

SOURCE=..\ImageCacheLib\ICLImgImplUMD.h
# End Source File
# Begin Source File

SOURCE=..\ImageCacheLib\ICLImgImplUMDS.cpp
# End Source File
# Begin Source File

SOURCE=..\ImageCacheLib\ICLImgImplUMDS.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\ImageCacheLib\ICLImg.cpp
# End Source File
# Begin Source File

SOURCE=..\ImageCacheLib\ICLImg.h
# End Source File
# Begin Source File

SOURCE=..\ImageCacheLib\ICLImgImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\ImageCacheLib\ICLImgImpl.h
# End Source File
# End Group
# Begin Group "ICLTasks"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ImageCacheLib\ICLTaskProcFolder.cpp
# End Source File
# Begin Source File

SOURCE=..\ImageCacheLib\ICLTaskProcFolder.h
# End Source File
# End Group
# Begin Group "ICLFile"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ImageCacheLib\ICLFile.cpp
# End Source File
# Begin Source File

SOURCE=..\ImageCacheLib\ICLFile.h
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ImageCacheLib\ICLCommon.h
# End Source File
# Begin Source File

SOURCE=..\ImageCacheLib\ICLNotifier.h
# End Source File
# Begin Source File

SOURCE=..\ImageCacheLib\ICLRequireHeaders.h
# End Source File
# Begin Source File

SOURCE=..\ImageCacheLib\ICLUnicodeCommon.cpp
# End Source File
# Begin Source File

SOURCE=..\ImageCacheLib\ICLUnicodeCommon.h
# End Source File
# End Group
# Begin Group "Precompiled Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ImageCacheLib\ICLStdAfx.cpp
# ADD CPP /Yc"ICLStdAfx.h"
# End Source File
# Begin Source File

SOURCE=..\ImageCacheLib\ICLStdAfx.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\ImageCacheLib\PlatformIndependFuncs.cpp
# End Source File
# Begin Source File

SOURCE=..\ImageCacheLib\PlatformIndependFuncs.h
# End Source File
# End Target
# End Project
