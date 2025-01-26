PROJ_DIR=`pwd`
"PATH": "${env.PATH};${projectDir}/out/build/x64-Debug/vcpkg_installed/x64-windows/debug/bin;${projectDir}/out/build/x64-Debug/vcpkg_installed/x64-windows/debug/lib;${projectDir}/out/build/x64-Debug/vcpkg_installed/x64-windows/debug/lib/manual-link;${projectDir}/out/build/x64-Debug/DatabaseAdapters;${projectDir}/out/build/x64-Debug/Entities;${projectDir}/out/build/x64-Debug/EntitiesModule;${projectDir}/out/build/x64-Debug/Events;${projectDir}/out/build/x64-Debug/FilesystemAdapters;${projectDir}/out/build/x64-Debug/Interprocess;${projectDir}/out/build/x64-Debug/Intraprocess;${projectDir}/out/build/x64-Debug/Resources;${projectDir}/out/build/x64-Debug/ResourcesModule",

echo "Running test_database_adapters"
./out/test_database_adapters/test_database_adapters
echo
echo "Running test_entities"
./out/test_entities/test_entities
echo
echo "Running test_events"
./out/test_events/test_events
echo
echo "Running test_filesystem_adapters"
./out/test_filesystem_adapters/test_filesystem_adapters
echo
echo "Running test_resources"
./out/test_resources/test_resources
# TODO: cpprest sdk is not portable between windows and mac - replace with Drogon
#echo "Running test_interprocess"
#./out/test_interprocess/test_interprocess
echo
echo "Running test_intraprocess"
./out/test_intraprocess/test_intraprocess