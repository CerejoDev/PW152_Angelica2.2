# Microsoft Developer Studio Project File - Name="AFileDialogDll" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=AFileDialogDll - Win32 UnicodeDebug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AFileDialogDll.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AFileDialogDll.mak" CFG="AFileDialogDll - Win32 UnicodeDebug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AFileDialogDll - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "AFileDialogDll - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "AFileDialogDll - Win32 UnicodeDebug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "AFileDialogDll - Win32 UnicodeRelease" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/AFileDialog3/Complier_VC6", ZDYAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AFileDialogDll - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Lib_VC6"
# PROP Intermediate_Dir "Build\AFileDialogDll\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\ximageinclude" /I "..\ImageCacheLib" /I "..\..\LuaWrapper\Include" /I "..\..\..\Projects2\Angelica2\Output\3rdSDK\Include\Gfx" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /D "_A_FORBID_NEWDELETE" /D "_A_FORBID_MALLOC" /D "_ANGELICA2" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 Mpg123Lib.lib ogg_static.lib vorbisfile_static.lib ImmWrapper.lib d3dx8.lib png.lib jpeg_d.lib tiff.lib cximage.lib ImageCacheLib.lib ZLibLib.lib AngelicaCommon.lib AngelicaFile.lib AngelicaMedia.lib GfxEditor.lib APhysXEngineStatic.lib Angelica3D.lib LuaWrapper.lib lua5.1.mt.lib dxguid.lib d3d8.lib dsound.lib dbghelp.lib /nologo /subsystem:windows /dll /machine:I386 /out:"..\Lib_VC6/AFileDialog2.dll" /libpath:"..\Lib_VC6" /libpath:"..\ximagelib_vc6" /libpath:"..\..\LuaWrapper\lib" /libpath:"..\..\..\Projects2\Angelica2\Output\3rdsdk\lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy ..\Lib_VC6\AFileDialog2.dll ..\..\最新编辑器
# End Special Build Tool

!ELSEIF  "$(CFG)" == "AFileDialogDll - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Lib_VC6"
# PROP Intermediate_Dir "Build\AFileDialogDll\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\DepInclude\Include2" /I "..\DepInclude\Include2\luawrapper" /I "..\ximageinclude" /I "..\ImageCacheLib" /I "..\..\LuaWrapper\Include" /I "..\..\..\Projects2\Angelica2\Output\3rdSDK\Include\Gfx" /D "_ANGELICA2" /D "_USRDLL" /D "_A_FORBID_NEWDELETE" /D "_A_FORBID_MALLOC" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_WINDLL" /D "_AFXDLL" /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 D3dx8.lib AngelicaCommon_d.lib AngelicaFile_d.lib ZLibLib_d.lib AngelicaMedia_d.lib Mpg123Lib_d.lib strmbase_d.lib ImmWrapper_d.lib Angelica3D_d.lib shlwapi.lib strmbase_u.lib vorbisfile_static_d.lib vorbis_static_d.lib ogg_static_d.lib gfxeditor_d.lib APhysXEngineStatic_d.lib PhysXLoader.lib NxCooking.lib NxCharacter.lib luawrapper_d.lib lua5.1.mtd.lib mfcs42d.lib msvcrtd.lib cximage_d.lib Jpeg_d.lib png_d.lib Tiff_d.lib ImageCacheLib_d.lib dxguid.lib d3d8.lib dsound.lib dbghelp.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\Lib_VC6\AFileDialog2_d.dll" /pdbtype:sept /libpath:"D:\CBin\stlport" /libpath:"..\DepLib\Lib2" /libpath:"..\Lib_VC6" /libpath:"..\ximagelib_vc6" /libpath:"..\..\LuaWrapper\lib" /libpath:"..\..\..\Projects2\Angelica2\Output\3rdsdk\lib"
# SUBTRACT LINK32 /pdb:none /nodefaultlib
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy ..\Lib_VC6\AFileDialog2_d.dll ..\..\最新编辑器
# End Special Build Tool

!ELSEIF  "$(CFG)" == "AFileDialogDll - Win32 UnicodeDebug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "UnicodeDebug"
# PROP BASE Intermediate_Dir "UnicodeDebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Lib_VC6"
# PROP Intermediate_Dir "Build\ImageCacheLib\UnicodeDebug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\ximageinclude" /I "..\ImageCacheLib" /I "..\..\LuaWrapper\Include" /I "..\..\..\Projects2\Angelica2\Output\3rdSDK\Include\Gfx" /D "UNICODE" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /D "_A_FORBID_NEWDELETE" /D "_A_FORBID_MALLOC" /D "_ANGELICA2" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 ZLibLib_d.lib AngelicaCommon_sd.lib AngelicaFile_sd.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 Mpg123Lib_d.lib ogg_static_d.lib vorbisfile_static_d.lib ImmWrapper_d.lib ImageCacheLib_ud.lib png_ud.lib jpeg_ud.lib tiff_ud.lib d3dx8.lib cximage_ud.lib ZLibLib_d.lib AngelicaCommon_sud.lib AngelicaFile_sud.lib AngelicaMedia_sud.lib GfxEditor_ud.lib LuaWrapper_du.lib Angelica3D_ud.lib lua5.1.mtd.lib dxguid.lib d3d8.lib dsound.lib dbghelp.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../Lib_VC6/AFileDialog2_ud.dll" /pdbtype:sept /libpath:"..\Lib_VC6" /libpath:"..\ximagelib_vc6" /libpath:"..\..\LuaWrapper\lib" /libpath:"..\..\..\Projects2\Angelica2\Output\3rdsdk\lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy ..\Lib_VC6\AFileDialog2_ud.dll ..\..\最新编辑器
# End Special Build Tool

!ELSEIF  "$(CFG)" == "AFileDialogDll - Win32 UnicodeRelease"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "UnicodeRelease"
# PROP BASE Intermediate_Dir "UnicodeRelease"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Lib_VC6"
# PROP Intermediate_Dir "Build\ImageCacheLib\UnicodeRelease"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\ximageinclude" /I "..\ImageCacheLib" /I "..\..\LuaWrapper\Include" /I "..\..\..\Projects2\Angelica2\Output\3rdSDK\Include\Gfx" /D "_UNICODE" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /D "_A_FORBID_NEWDELETE" /D "_A_FORBID_MALLOC" /D "_ANGELICA2" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 Mpg123Lib.lib ogg_static.lib vorbisfile_static.lib ImmWrapper.lib ImageCacheLib_u.lib png_u.lib jpeg_u.lib tiff_u.lib d3dx8.lib cximage_u.lib ZLibLib.lib AngelicaCommon_su.lib AngelicaFile_su.lib AngelicaMedia_su.lib GfxEditor_u.lib Angelica3D_u.lib LuaWrapper_u.lib lua5.1.mtd.lib dxguid.lib d3d8.lib dsound.lib dbghelp.lib /nologo /subsystem:windows /dll /machine:I386 /out:"../Lib_VC6/AFileDialog2_u.dll" /libpath:"..\Lib_VC6" /libpath:"..\ximagelib_vc6" /libpath:"..\..\LuaWrapper\lib" /libpath:"..\..\..\Projects2\Angelica2\Output\3rdsdk\lib" /libpath:"..\..\..\Projects2\Angelica2\Output\a3dsdk\lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy ..\Lib_VC6\AFileDialog2_u.dll ..\..\最新编辑器
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "AFileDialogDll - Win32 Release"
# Name "AFileDialogDll - Win32 Debug"
# Name "AFileDialogDll - Win32 UnicodeDebug"
# Name "AFileDialogDll - Win32 UnicodeRelease"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "RenderItem"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\AFileDialogDll\ECMRenderItem.cpp
# End Source File
# Begin Source File

SOURCE=..\AFileDialogDll\GfxRenderItem.cpp
# End Source File
# Begin Source File

SOURCE=..\AFileDialogDll\SMDRenderItem.cpp
# End Source File
# Begin Source File

SOURCE=..\AFileDialogDll\UMDRenderItem.cpp
# End Source File
# Begin Source File

SOURCE=..\AFileDialogDll\UMDSRenderItem.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=..\AFileDialogDll\AFileDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\AFileDialogDll\AFileDialogDll.cpp
# End Source File
# Begin Source File

SOURCE=..\AFileDialogDll\AFileDialogDll.rc
# End Source File
# Begin Source File

SOURCE=..\AFileDialogDll\AFilePreview.cpp
# End Source File
# Begin Source File

SOURCE=..\AFileDialogDll\AFilePreviewWnd.cpp
# End Source File
# Begin Source File

SOURCE=..\AFileDialogDll\AFTaskPlaySound.cpp
# End Source File
# Begin Source File

SOURCE=..\AFileDialogDll\AImagePreviewImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\AFileDialogDll\AListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=..\AFileDialogDll\ASoundPlayerImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\AFileDialogDll\ASoundPlayerWrapper.cpp
# End Source File
# Begin Source File

SOURCE=..\AFileDialogDll\base64.cpp

!IF  "$(CFG)" == "AFileDialogDll - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "AFileDialogDll - Win32 Debug"

!ELSEIF  "$(CFG)" == "AFileDialogDll - Win32 UnicodeDebug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "AFileDialogDll - Win32 UnicodeRelease"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\AFileDialogDll\DynSizeCtrl.cpp
# End Source File
# Begin Source File

SOURCE=..\AFileDialogDll\LuaBind.cpp

!IF  "$(CFG)" == "AFileDialogDll - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "AFileDialogDll - Win32 Debug"

!ELSEIF  "$(CFG)" == "AFileDialogDll - Win32 UnicodeDebug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "AFileDialogDll - Win32 UnicodeRelease"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\AFileDialogDll\Render.cpp
# End Source File
# Begin Source File

SOURCE=..\AFileDialogDll\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "RenderItemH"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\AFileDialogDll\ECMRenderItem.h
# End Source File
# Begin Source File

SOURCE=..\AFileDialogDll\GfxRenderItem.h
# End Source File
# Begin Source File

SOURCE=..\AFileDialogDll\SMDRenderItem.h
# End Source File
# Begin Source File

SOURCE=..\AFileDialogDll\UMDRenderItem.h
# End Source File
# Begin Source File

SOURCE=..\AFileDialogDll\UMDSRenderItem.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\AFileDialogDll\AFDRequireHeaders.h
# End Source File
# Begin Source File

SOURCE=..\AFileDialogDll\AFileDialog.h
# End Source File
# Begin Source File

SOURCE=..\AFileDialogDll\AFileDialogDll.h
# End Source File
# Begin Source File

SOURCE=..\AFileDialogDll\AFilePreview.h
# End Source File
# Begin Source File

SOURCE=..\AFileDialogDll\AFilePreviewWnd.h
# End Source File
# Begin Source File

SOURCE=..\AFileDialogDll\AFTaskPlaySound.h
# End Source File
# Begin Source File

SOURCE=..\AFileDialogDll\AImagePreviewImpl.h
# End Source File
# Begin Source File

SOURCE=..\AFileDialogDll\AListCtrl.h
# End Source File
# Begin Source File

SOURCE=..\AFileDialogDll\ARenderItem.h
# End Source File
# Begin Source File

SOURCE=..\AFileDialogDll\ASoundPlayerImpl.h
# End Source File
# Begin Source File

SOURCE=..\AFileDialogDll\base64.h
# End Source File
# Begin Source File

SOURCE=..\AFileDialogDll\DynSizeCtrl.h
# End Source File
# Begin Source File

SOURCE=..\AFileDialogDll\LuaBind.h
# End Source File
# Begin Source File

SOURCE=..\AFileDialogDll\Render.h
# End Source File
# Begin Source File

SOURCE=..\AFileDialogDll\resource.h
# End Source File
# Begin Source File

SOURCE=..\AFileDialogDll\StdAfx.h
# End Source File
# Begin Source File

SOURCE=..\AFileDialogDll\SyncWrapper.h
# End Source File
# Begin Source File

SOURCE=..\AFileDialogDll\TypeDefs.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\AFileDialogDll\res\AFileDialogDll.rc2
# End Source File
# Begin Source File

SOURCE=..\AFileDialogDll\res\arrow_back.bmp
# End Source File
# Begin Source File

SOURCE=..\AFileDialogDll\res\arrow_up.bmp
# End Source File
# Begin Source File

SOURCE=..\AFileDialogDll\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=..\AFileDialogDll\res\bitmap3.bmp
# End Source File
# Begin Source File

SOURCE=..\AFileDialogDll\res\icon_directory.bmp
# End Source File
# Begin Source File

SOURCE=..\AFileDialogDll\res\icon_other.bmp
# End Source File
# Begin Source File

SOURCE=..\AFileDialogDll\res\icon_picture.bmp
# End Source File
# Begin Source File

SOURCE=..\AFileDialogDll\res\icon_sound.bmp
# End Source File
# Begin Source File

SOURCE=..\AFileDialogDll\res\icon_text.bmp
# End Source File
# End Group
# Begin Group "UIControls"

# PROP Default_Filter ""
# End Group
# Begin Group "Wrappers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\AFileDialogDll\AFileDialogWrapper.cpp
# End Source File
# Begin Source File

SOURCE=..\AFileDialogDll\AFileDialogWrapper.h
# End Source File
# Begin Source File

SOURCE=..\AFileDialogDll\AFileImageWrapper.cpp
# End Source File
# Begin Source File

SOURCE=..\AFileDialogDll\AFileImageWrapper.h
# End Source File
# Begin Source File

SOURCE=..\AFileDialogDll\AImagePreviewWrapper.cpp
# End Source File
# Begin Source File

SOURCE=..\AFileDialogDll\AImagePreviewWrapper.h
# End Source File
# Begin Source File

SOURCE=..\AFileDialogDll\ASoundPlayerWrapper.h
# End Source File
# End Group
# Begin Group "AFileDialog2"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\AFileDialogDll\AFileDialog2.cpp
# End Source File
# Begin Source File

SOURCE=..\AFileDialogDll\AFileDialog2.h
# End Source File
# Begin Source File

SOURCE=..\AFileDialogDll\AFileItem.cpp
# End Source File
# Begin Source File

SOURCE=..\AFileDialogDll\AFileItem.h
# End Source File
# Begin Source File

SOURCE=..\AFileDialogDll\AFileItemType.cpp
# End Source File
# Begin Source File

SOURCE=..\AFileDialogDll\AFileItemType.h
# End Source File
# Begin Source File

SOURCE=..\AFileDialogDll\AListCtrl2.cpp
# End Source File
# Begin Source File

SOURCE=..\AFileDialogDll\AListCtrl2.h
# End Source File
# End Group
# End Target
# End Project
