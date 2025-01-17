
# azulejo-common-lib

A library for composing C++ class systems that use aggregation as a basis for class member 
association. Aggregated members benefit from recursive functions that automate serialization 
and storage in relational databases. In addition, the library provides channels for event 
driven communication along with concurrency, parallelism, and interprocess utilities (TCP, 
HTTP, shared memory). 

Sample implementations:
+ [azulejo-gl-asteroids](https://github.com/miclomba/azulejo-gl-asteroids) - a classic Asteroids game using OpenGL
+ [azulejo-gl-3dmodeler](https://github.com/miclomba/azulejo-gl-3dmodeler) - a 3D modeling app starter using OpenGL

## Prerequisites

Download [Visual Studio Community](https://visualstudio.microsoft.com/vs/community/)

## Install Dependencies

Install [vcpkg](https://github.com/microsoft/vcpkg). Then run:

```
cd azulejo-common-lib
vcpkg install --x-manifest-root=. --feature-flags=manifests
```

## Build

0. Start Visual Studio
1. Click `continue without code`
2. Set configuration to be `x64-Debug`
3. `File > Open > CMake`
4. If the 'out' directory is not created automatically (Cmake configuration step) then run: `Project > Configure Cache`
5. Verify boost and python ENVs in CMakeSettings.json are consistent with your installations 
6. `Build > Build All`
7. `Build > Install X`

### Test

`Test > Run CTests for X`

