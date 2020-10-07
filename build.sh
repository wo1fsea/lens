rm -r ./build 
mkdir ./build
cd ./build/ 
cmake -DCMAKE_TOOLCHAIN_FILE="~/vcpkg/scripts/buildsystems/vcpkg.cmake" ../ 
cmake --build .
./lens/lens 0 &
./lens/lens 1 &
./lens/lens 2 &
./lens/lens 3 &