Install vcpkg. Then run:

0. cd X
1. vcpkg install --x-manifest-root=. --feature-flags=manifests

Build and install the X project:

0. Run start_win_devenv.bat
1. Click 'continue without code'
2. Make the CMakeLists.txt the startup item
3. Set configuration to be 'x64-Debug'
4. File > Open > CMake
5. If the 'out' directory is not created automatically (Cmake configuration step) then run: Project > Generate Cache for GLAsteroids
6. Verify that boost and python variables in CMakeLists.txt are consistent with your installation
7. Build > Build All
8. Build > Install X
