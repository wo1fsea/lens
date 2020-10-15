# lens 
A Path Tracing Example with GUI

## Build
1. Install [vcpkg](https://github.com/microsoft/vcpkg)

```
> git clone https://github.com/Microsoft/vcpkg.git
> cd vcpkg

PS> .\bootstrap-vcpkg.bat
Linux:~/$ ./bootstrap-vcpkg.sh
```

2. Use Vcpkg to Install Dependencies 

```
PS> .\vcpkg install SDL2
Linux:~/$ ./vcpkg install SDL2
```

3. Use CMake to Build

```
> cd lens
> mkdir ./build
> cd ./build 
> cmake -DCMAKE_TOOLCHAIN_FILE="[your_vcpkg_root]/scripts/buildsystems/vcpkg.cmake" ../ 
> cmake --build ./
```