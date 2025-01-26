# azulejo-common-lib

A library for composing C++ class systems that use aggregation as a basis for class member
association. Aggregated members benefit from recursive functions that automate serialization
and storage in relational databases. In addition, the library provides channels for event
driven communication along with concurrency, parallelism, and interprocess utilities (TCP,
HTTP, shared memory).

Sample implementations:

- [azulejo-gl-asteroids](https://github.com/miclomba/azulejo-gl-asteroids) - a classic Asteroids game using OpenGL
- [azulejo-gl-3dmodeler](https://github.com/miclomba/azulejo-gl-3dmodeler) - a 3D modeling app starter using OpenGL

## Prerequisites

### On Windows

Download [Visual Studio Community](https://visualstudio.microsoft.com/vs/community/)

### On MacOS

0. Install `XCode`
1. Install `clang` by running: `xcode-select --install`
2. Install build tools: `brew install cmake ninja autoconf automake autoconf-archive libtool pkg-config`
3. FreeGLUT depends on `X11` so install `XQuartz` by running: `brew install --cask xquartz`
4. Install `libomp` by running: `brew install libomp llvm`
5. Install `VSCode`
6. Install VSCode extensions:

- CMake Tools (by Microsoft)
- C/C++ (by Microsoft)

### Install VCPKG Dependencies

Install [vcpkg](https://github.com/microsoft/vcpkg) adjacent to your project directory. Then run:

```
cd azulejo-common-lib
vcpkg install --x-manifest-root=. --feature-flags=manifests
```

## Build

### On Windows (Visual Studio)

0. Start Visual Studio
1. Click `continue without code`
2. Set configuration to be `x64-Debug`
3. `File > Open > CMake`
4. If the 'out' directory is not created automatically (Cmake configuration step) then run: `Project > Configure Cache`
5. Verify boost and python ENVs in CMakeSettings.json are consistent with your installations
6. `Build > Build All`

### On MacOS (VSCode)

0. Run `CMake: Configure` command and use the appropriate compiler:

- Clang (on macOS)
- GCC (on Linux)
- Visual Studio (on Windows)

1. Run `CMake: Build`

## Install

### On Windows (Visual Studio)

0. `Build > Install X`

### On MacOS (VSCode)

0. Run command: `CMake: Install`

## Test

### On Windows (Visual Studio)

In Visual Studio: `Test > Run CTests for X`

### On MacOS (VSCode)

Run command: `CMake: Run Tests`
