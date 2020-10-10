rmdir /s /q .\build
mkdir build
cd .\build
cmake -DCMAKE_TOOLCHAIN_FILE="g:/vcpkg/scripts/buildsystems/vcpkg.cmake" ..\
cmake --build .
cd .\lens
.\Debug\lens.exe
cd ..\..\
pause