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

### On MacOS

0. Install `XCode`
1. Install `clang` by running: `xcode-select --install`
2. Install build tools: `./scripts/macos_install_dependencies.sh`
3. FreeGLUT depends on `X11` so install `XQuartz` by running: `brew install --cask xquartz`
4. Install `libomp` by running: `brew install libomp llvm`
5. Istall dependencies: `./scripts/vcpkg_install.sh`
6. Install `VSCode`
7. Install VSCode extensions:

- CMake Tools (by Microsoft)
- C/C++ (by Microsoft)

8. Copy settings `cp .vscode/settings.mac.json .vscode/settings.json`

### On Ubuntu

0. Install dependencies: `./scripts/linux_install_dependencies.sh`
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

Run: `./scripts/install.sh`

## Uninstall

Run: `./scripts/uninstall.sh`

## Test

Run: `./scripts/start_unit_tests.sh`
