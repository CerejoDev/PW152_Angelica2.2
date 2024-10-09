REM USE ONLY WITH VISUAL C++ 6.0 Command Prompt
REM TO MAKE SHORTCUT
REM C:\Windows\System32\cmd.exe /k "C:\Microsoft Visual Studio\VC98\Bin\vcvars32.bat"
link.exe /LIB AAssist.obj ACounter.obj AList.obj ALog.obj AMemory.obj AMemoryDbg.obj APerlinNoise1D.obj APerlinNoise2D.obj APerlinNoise3D.obj APerlinNoiseBase.obj ARandomGen.obj AString.obj ATempMemMan.obj ATime.obj  AWString.obj jexception.obj /OUT:AngelicaCommon_dr.lib
@pause