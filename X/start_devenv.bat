@echo off
SET ORIG_PATH=%PATH%
SET TEST_EXPLORER_LINK_DIRS=C:/Users/miclomba/source/repos/Backend/X/x64/Debug;C:/Users/miclomba/source/repos/Backend/X/out/build/x64-Debug
SET PATH=%PATH%;%TEST_EXPLORER_LINK_DIRS%

"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\IDE\devenv.exe"

SET PATH=%ORIG_PATH%