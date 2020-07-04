export REPOS="$HOME/source/repos"
export X_DIR="$REPOS/Backend/X"

export INSTALL_PATH="$X_DIR/out/xcode/install"
export X_LINK_DIR="$X_DIR/out/xcode"
export X_INCL_DIR="$X_DIR"

# google tests
export GTEST_LINK_DIR="$REPOS/googletest/out/xcode/lib"
export GTEST_INCL_DIR="$REPOS/googletest/googletest/include"
export GMOCK_LINK_DIR="$REPOS/googletest/out/xcode/lib"
export GMOCK_INCL_DIR="$REPOS/googletest/googlemock/include"

# compile boost yourself
export BOOST_INCL_DIR="$REPOS/boost/boost_1_69_0"
export BOOST_LINK_DIR="$REPOS/boost/boost_1_69_0/stage/lib"

# brew install libomp
export OMP_INCL_DIR="/usr/local/Cellar/libomp/10.0.0/include"
export OMP_LINK_DIR="/usr/local/Cellar/libomp/10.0.0/lib"

# brew install python???
export PYTHON3_LINK_DIR="/Library/Frameworks/Python.framework/Versions/2.7/lib"
export PYTHON3_INCL_DIR="/Library/Frameworks/Python.framework/Versions/2.7/include"

export PATH="$PATH:$X_LINK_DIR:$BOOST_LINK_DIR:$GTEST_LINK_DIR:$GMOCK_LINK_DIR:$OMP_LINK_DIR:$PYTHON3_LINK_DIR"

cmake -G Xcode ../../