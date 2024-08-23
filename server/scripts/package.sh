rm -r build
cmake -B build -DCMAKE_BUILD_TYPE=release -DZOUIPOCAR_TESTS=OFF
cmake --build build -j
rm zouipocar.zip
zip -r zouipocar.zip www
zip -j zouipocar.zip build/zouipocar zouipocar.service
