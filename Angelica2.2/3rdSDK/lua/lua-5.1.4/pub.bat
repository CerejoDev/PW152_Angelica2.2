del pub /F /S /Q 
if not exist pub md pub
if not exist pub\include md pub\include
if not exist pub\include\3rdsdk md pub\include\3rdsdk
if not exist pub\lib md pub\lib
if not exist pub\lib\3rdsdk md pub\lib\3rdsdk
if not exist pub\lib_x64\3rdsdk md pub\lib_x64\3rdsdk
xcopy etc\lua.hpp pub\Include\3rdSDK /Y /D
xcopy src\lua.h pub\Include\3rdSDK /Y /D
xcopy src\lauxlib.h pub\Include\3rdSDK /Y /D
xcopy src\lualib.h pub\Include\3rdSDK /Y /D
xcopy src\luaconf.h pub\Include\3rdSDK /Y /D
xcopy build\win32\debug\lua5.1.mtd.lib pub\lib\3rdSDK /y /d
xcopy build\win32\release\lua5.1.mt.lib pub\lib\3rdSDK /y /d
xcopy build\x64\debug\lua5.1.mtd.lib pub\lib_x64\3rdSDK /y /d
xcopy build\x64\release\lua5.1.mt.lib pub\lib_x64\3rdSDK /y /d
pause