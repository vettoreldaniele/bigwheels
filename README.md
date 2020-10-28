# Requirements
 * GGP
   * Visual Studio 2019 Professional
 * Linux
   * Visual Studio Code or CLI
 * Windows
   * Visual Studio 2019 Professaion or Visual Studio Code
 * Latest Vulkan SDK
 * Recent version of Windows SDK

# Build Instructions
## GGP
```
git clone --recursive git@github.com:chaoticbob/BigWheels.git
cd BigWheels
mkdir build-ggp
cd build-ggp
cmake -G "Visual Studio 16 2019" -DCMAKE_TOOLCHAIN_FILE="C:\\Program Files\\GGP SDK\\cmake\\ggp.cmake" ..
```
Open `BigWheels.sln` and build

Built binaries are written to `BigWheels\bin`.

**NOTE:** GGP supplied Vulkan headers and libraries are used for building *but* the build system will look for the DXC executable in the Vulkan SDK directory.  

## Linux
```
git clone --recursive git@github.com:chaoticbob/BigWheels.git
cd BigWheels
mkdir build-make
cd build-make
cmake ..
make -j <# CPUs to use>
```

Built binaries are written to `BigWheels\bin`.

## Windows 
```
git clone --recursive git@github.com:chaoticbob/BigWheels.git
cd BigWheels
mkdir build-vs2019
cd build-vs2019
cmake -G "Visual Studio 16 2019" -A x64 ..
```
Open `BigWheels.sln` and build

Built binaries are written to `BigWheels\bin`.

# Shader Compilation
Shader binaries are generated during project build. For GGP and Linux only SPIR-V binaries are generated. For Windows DXBC, DXIL, and SPIR-V binaries are generated.

## DXC
The build system will look for `dxc.exe` or `dxc` in the Vulkan SDK bin directory. 

To use a custom DXC executable use the following:
```
cmake <platform specific params> -DDXC_PATH=<path to DXC executable>
```
The build system will generate an error if the DXC executable is not present.

## FXC
The build system will look for `fxc.exe` in the Windows SDK version that CMake selects. 
