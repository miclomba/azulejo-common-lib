PROJ_DIR=`pwd`
PATH="$PATH:$PROJ_DIR/vcpkg_installed/x64-osx/debug/bin:$PROJ_DIR/vcpkg_installed/x64-osx/debug/lib:$PROJ_DIR/vcpkg_installed/x64-osx/debug/lib/manual-link:$PROJ_DIR/out/DatabaseAdapters:$PROJ_DIR/out/Entities:$PROJ_DIR/out/EntitiesModule:$PROJ_DIR/out/Events:$PROJ_DIR/out/FilesystemAdapters:$PROJ_DIR/out/Interprocess:$PROJ_DIR/out/Intraprocess:$PROJ_DIR/out/Resources:$PROJ_DIR/out/ResourcesModule"


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