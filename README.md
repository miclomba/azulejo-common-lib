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

### Install VCPKG Dependencies

Install [vcpkg](https://github.com/microsoft/vcpkg) adjacent to your project directory.

### On Windows

0. Download [Visual Studio Community](https://visualstudio.microsoft.com/vs/community/)
1. Install `VSCode`
2. Install VSCode extensions:

- CMake Tools (by Microsoft)
- C/C++ (by Microsoft)

3. Copy settings `cp .vscode/settings.win.json .vscode/settings.json`

### On MacOS

0. Install `XCode`
1. Install `clang` by running: `xcode-select --install`
2. Install build tools: `brew install cmake ninja autoconf automake autoconf-archive libtool pkg-config`
3. FreeGLUT depends on `X11` so install `XQuartz` by running: `brew install --cask xquartz`
4. Install `libomp` by running: `brew install libomp llvm`
5. Istall dependencies: `vcpkg install --x-manifest-root=. --feature-flags=manifests`
6. Install `VSCode`
7. Install VSCode extensions:

- CMake Tools (by Microsoft)
- C/C++ (by Microsoft)

8. Copy settings `cp .vscode/settings.mac.json .vscode/settings.json`

### On Ubuntu

0. Install dependencies:

```
sudo apt install -y \
  build-essential autoconf autoconf-archive automake libtool pkg-config cmake git unzip ninja-build curl \
  libssl-dev zlib1g-dev libbz2-dev libreadline-dev libsqlite3-dev libncursesw5-dev libffi-dev \
  libgdbm-dev liblzma-dev uuid-dev tk-dev xz-utils wget libgomp1
  libgl1-mesa-dev libglu1-mesa-dev libx11-dev libxrandr-dev libxi-dev libxmu-dev libxxf86vm-dev
```

1. Istall dependencies: `vcpkg install --x-manifest-root=. --feature-flags=manifests`
2. Install `VSCode`
3. Install VSCode extensions:

- CMake Tools (by Microsoft)
- C/C++ (by Microsoft)

4. Copy settings `cp .vscode/settings.linux.json .vscode/settings.json`

## Build

0. Run `CMake: Configure` command and use the appropriate compiler:

- Clang (on macOS)
- GCC (on Linux)
- Visual Studio (on Windows)

1. Run `CMake: Build`

## Install

Run command: `CMake: Install`

## Test

Run command: `./start_unit_tests.sh`
