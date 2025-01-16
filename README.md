
# azulejo-common-lib

A set of common components for building C++ apps. Current offerrings:

0. Event consumer and emitter base classes 
1. Serializable base "Entity" to streamline light-object serialization 
2. Serializable base "Resource" to streamline heavy-object (e.g. array) serialization 
3. Interprocess constructs that streamline communication over TCP, REST, Shared Memory, Async IO
4. Intraprocess constructos (Thread Pool, Runnable)

See example usage in [azulejo-gl-asteroids](https://github.com/miclomba/azulejo-gl-asteroids)

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

