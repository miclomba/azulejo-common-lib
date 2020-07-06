export REPOS="$HOME/source/repos"
#export X_DIR="$REPOS/Backend/X"

export X_LINK_DIR="/usr/local/lib"
#export GTEST_LINK_DIR="$REPOS/googletest/out/xcode/lib"
#export GMOCK_LINK_DIR="$REPOS/googletest/out/xcode/lib"
export BOOST_LINK_DIR="$REPOS/boost/boost_1_69_0/stage/lib/Debug"
export OMP_LINK_DIR="/usr/local/Cellar/libomp/10.0.0/lib"
export DYLD_LIBRARY_PATH="$BOOST_LINK_DIR"

export PATH="$X_LINK_DIR:$BOOST_LINK_DIR:$OMP_LINK_DIR:$PATH"

echo "Running test_database_adapters"
./out/xcode/test_database_adapters/Debug/test_database_adapters | grep -i failed
echo "Running test_entities"
./out/xcode/test_entities/Debug/test_entities | grep -i failed
echo "Running test_events"
./out/xcode/test_events/Debug/test_events | grep -i failed
echo "Running test_filesystem_adapters"
./out/xcode/test_filesystem_adapters/Debug/test_filesystem_adapters | grep -i failed
echo "Running test_resources"
./out/xcode/test_resources/Debug/test_resources | grep -i failed
echo "Running test_interprocess"
./out/xcode/test_interprocess/Debug/test_interprocess | grep -i failed
echo "Running test_intraprocess"
./out/xcode/test_intraprocess/Debug/test_intraprocess | grep -i failed
