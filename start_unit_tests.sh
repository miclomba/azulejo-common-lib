PROJ_DIR=`pwd`
PATH="$PATH:$PROJ_DIR/vcpkg_installed/x64-osx/debug/bin:$PROJ_DIR/vcpkg_installed/x64-osx/debug/lib:$PROJ_DIR/vcpkg_installed/x64-osx/debug/lib/manual-link:$PROJ_DIR/out/src/DatabaseAdapters:$PROJ_DIR/out/src/Entities:$PROJ_DIR/out/src/EntitiesModule:$PROJ_DIR/out/src/Events:$PROJ_DIR/out/src/FilesystemAdapters:$PROJ_DIR/out/src/Interprocess:$PROJ_DIR/out/src/Intraprocess:$PROJ_DIR/out/src/Resources:$PROJ_DIR/out/src/ResourcesModule"


echo "Running test_database_adapters"
./out/src/test_database_adapters/test_database_adapters
echo
echo "Running test_entities"
./out/src/test_entities/test_entities
echo
echo "Running test_events"
./out/src/test_events/test_events
echo
echo "Running test_filesystem_adapters"
./out/src/test_filesystem_adapters/test_filesystem_adapters
echo
echo "Running test_resources"
./out/src/test_resources/test_resources
# TODO: cpprest sdk is not portable between windows and mac - replace with Drogon
#echo "Running test_interprocess"
#./out/src/test_interprocess/test_interprocess
echo
echo "Running test_intraprocess"
./out/src/test_intraprocess/test_intraprocess