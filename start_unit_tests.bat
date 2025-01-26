@echo off
set PROJ_DIR=%CD%

set PATH=%PATH%;%PROJ_DIR%\vcpkg_installed\x64-windows\debug\bin;^
%PROJ_DIR%\vcpkg_installed\x64-windows\debug\lib;^
%PROJ_DIR%\vcpkg_installed\x64-windows\debug\lib\manual-link;^
%PROJ_DIR%\out\build\x64-Debug\DatabaseAdapters;^
%PROJ_DIR%\out\build\x64-Debug\Entities;^
%PROJ_DIR%\out\build\x64-Debug\EntitiesModule;^
%PROJ_DIR%\out\build\x64-Debug\Events;^
%PROJ_DIR%\out\build\x64-Debug\FilesystemAdapters;^
%PROJ_DIR%\out\build\x64-Debug\Interprocess;^
%PROJ_DIR%\out\build\x64-Debug\Intraprocess;^
%PROJ_DIR%\out\build\x64-Debug\Resources;^
%PROJ_DIR%\out\build\x64-Debug\ResourcesModule

echo "Running test_database_adapters"
.\out\build\x64-Debug\test_database_adapters\test_database_adapters
echo
echo "Running test_entities"
.\out\build\x64-Debug\test_entities\test_entities
echo
echo "Running test_events"
.\out\build\x64-Debug\test_events\test_events
echo
echo "Running test_filesystem_adapters"
.\out\build\x64-Debug\test_filesystem_adapters\test_filesystem_adapters
echo
echo "Running test_resources"
.\out\build\x64-Debug\test_resources\test_resources
echo
echo "Running test_interprocess"
.\out\build\x64-Debug\test_interprocess\test_interprocess
echo
echo "Running test_intraprocess"
.\out\build\x64-Debug\test_intraprocess\test_intraprocess