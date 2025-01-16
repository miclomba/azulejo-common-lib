@echo off
SET ORIG_PATH=%PATH%

SET REPOS=C:/Users/miclomba/source/repos
SET BASE_LINK_DIR=%REPOS%/Backend/X/out/build/x64-Debug
SET DEPENDENCY_LINK_BIN=%BASE_LINK_DIR%/vcpkg_installed/x64-windows/debug/bin
SET DEPENDENCY_LINK_LIB=%BASE_LINK_DIR%/vcpkg_installed/x64-windows/debug/lib
SET DEPENDENCY_MAN_LINK_BIN=%BASE_LINK_DIR%/vcpkg_installed/x64-windows/debug/bin
SET DEPENDENCY_MAN_LINK_DIR=%BASE_LINK_DIR%/vcpkg_installed/x64-windows/debug/lib/manual-link
SET DATABASE_ADAPTERS=%BASE_LINK_DIR%/DatabaseAdapters
SET ENTITIES=%BASE_LINK_DIR%/Entities
SET ENTITIESMODULE=%BASE_LINK_DIR%/EntitiesModule
SET EVENTS=%BASE_LINK_DIR%/Events
SET FILESYSTEM_ADAPTERS=%BASE_LINK_DIR%/FilesystemAdapters
SET INTERPROCESS=%BASE_LINK_DIR%/Interprocess
SET INTRAPROCESS=%BASE_LINK_DIR%/Intraprocess
SET RESOURCES=%BASE_LINK_DIR%/Resources
SET RESOURCESMODULE=%BASE_LINK_DIR%/ResourcesModule

REM Setting the path so that test_*.exe can find the appropriate dlls during test discovery
SET PATH=%PATH%;%DEPENDENCY_MAN_LINK_BIN%;%DEPENDENCY_MAN_LINK_DIR%;%DEPENDENCY_LINK_BIN%;%DEPENDENCY_LINK_LIB%;%DATABASE_ADAPTERS%;%ENTITIES%;%ENTITIESMODULE%;%EVENTS%;%FILESYSTEM_ADAPTERS%;%INTERPROCESS%;%INTRAPROCESS%;%RESOURCES%;%RESOURCESMODULE%

if %1.==. (
"C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\devenv.exe"
) else (
"C:/Users/miclomba/source/repos/Backend/X/out/install/X/rest_server.exe" %1
)

SET PATH=%ORIG_PATH%