# Build Instructions
## GGP
```
git clone --recursive git@github.com:chaoticbob/BigWheels.git
cd BigWheels
mkdir build-ggp
cd build-ggp
cmake
```
Open `BigWheels.sln` and build

## Linux
```
git clone --recursive git@github.com:chaoticbob/BigWheels.git
cd BigWheels
mkdir build-make
cd build-make
cmake ..
make -j <# CPUs to use>
```
## Windows 

```
git clone --recursive git@github.com:chaoticbob/BigWheels.git
cd BigWheels
mkdir build-vs2019
cd build-vs2019
cmake -G "Visual Studio 16 2019" -A x64 ..
```
Open `BigWheels.sln` and build

# Shader Compilation
## DXC
The build system will look for `dxc.exe` or `dxc` in the Vulkan SDK bin directory. 

To use a custom DXC executable use the following:
```
cmake <platform specific params> -DDXC_PATH=<path to DXC executable>
```
The build system will generate an error if the DXC executable is not present.

## FXC
The build system will look for `fxc.exe` in the Windows SDK version that CMake selects. 
