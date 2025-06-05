@echo off
set PROJ_DIR=%CD%

set PATH=%PATH%;%PROJ_DIR%\vcpkg_installed\x64-windows\debug\bin;^
%PROJ_DIR%\vcpkg_installed\x64-windows\debug\lib;^
%PROJ_DIR%\vcpkg_installed\x64-windows\debug\lib\manual-link;^
%PROJ_DIR%\out\src\Debug\DatabaseAdapters;^
%PROJ_DIR%\out\src\Debug\Entities;^
%PROJ_DIR%\out\src\Debug\EntitiesModule;^
%PROJ_DIR%\out\src\Debug\Events;^
%PROJ_DIR%\out\src\Debug\FilesystemAdapters;^
%PROJ_DIR%\out\src\Debug\Interprocess;^
%PROJ_DIR%\out\src\Debug\Intraprocess;^
%PROJ_DIR%\out\src\Debug\Resources;^
%PROJ_DIR%\out\src\Debug\ResourcesModule

echo "Running test_database_adapters"
.\out\src\test_database_adapters\Debug\test_database_adapters
echo
echo "Running test_entities"
.\out\src\test_entities\Debug\test_entities
echo
echo "Running test_events"
.\out\src\test_events\Debug\test_events
echo
echo "Running test_filesystem_adapters"
.\out\src\test_filesystem_adapters\Debug\test_filesystem_adapters
echo
echo "Running test_resources"
.\out\src\test_resources\Debug\test_resources
echo
echo "Running test_interprocess"
.\out\src\test_interprocess\Debug\test_interprocess
echo
echo "Running test_intraprocess"
.\out\src\test_intraprocess\Debug\test_intraprocess