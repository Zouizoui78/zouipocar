# zouipocar server

This is the server programs of zouipocar that receives the data sent by the tracking device, stores it and displays it on a map.

## Build
### Debug

    cmake -B build
    cmake --build build

In this configuration code coverage data is generated. The data can be viewed with e.g. `gcovr` if installed :

    gcovr -r src build

## Release

    cmake -B build -DCMAKE_BUILD_TYPE=release -DZOUIPOCAR_TESTS=OFF
    cmake --build build

## Packaging

    ./scripts/package.sh