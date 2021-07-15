# Overview
## Sample Prefixes
 * **dx11** - D3D11 with SM 5.0
   * Shaders are compiled with FXC
 * **dx12** - D3D12 with SM 5.1
   * Shaders are compiled with FXC
 * **dxil** - D3D12 with SM 6.0+ 
   * Shaders are compiled with DXC/DXIL
 * **dxvk** - D3D11 using DXVK with SM 5.0
   * Shaders are compiled with FXC
 * **vk** - Vulkan
   * Shaders are compiled with DXC/SPIR-V

# Requirements
 * Software
   * GGP
     * Visual Studio 2019 Professional
     * GGP SDK
     * Recent Vulkan SDK
       * 1.2.154.1 or later
   * Linux
     * Visual Studio Code or CLI
     * Recent Vulkan SDK
       * 1.2.154.1 or later
   * Windows
     * Visual Studio 2019 Professional or Visual Studio Code
     * Recent Vulkan SDK
       * 1.2.154.1 or later
     * Recent version of Windows SDK
 * Hardware
   * AMD
     * Vega GPUs
       * Vega 56
       * Vega 64
     * Navi GPUs
        * Radeon 5700
        * Radeon 5700 XT
   * NVIDIA
     * 20x0 GPUs
     * 16x0 GPus
     * 10x0 GPUs
       * *This is iffy - some of the later more recent DX12 and Vulkan features many not work.*
    * Intel
       * None tested

# Build Instructions
## GGP (on Windows)
```
git clone --recursive git@github.com:googlestadia/BigWheels.git
cd BigWheels
mkdir build-ggp
cd build-ggp
cmake -G "Visual Studio 16 2019" -DCMAKE_TOOLCHAIN_FILE="C:\\Program Files\\GGP SDK\\cmake\\ggp.cmake" ..
```
Open `BigWheels.sln` and build

Built binaries are written to `BigWheels\bin`.

**NOTE:** GGP supplied Vulkan headers and libraries are used for building *but* the build system will look for the DXC executable in the Vulkan SDK directory.  

## GGP using DXVK (libporto) (on Windows)
NOTE: There are two repos for the steps in this section.
```
git clone --recursive git@github.com:googlestadia/BigWheels.git
cd BigWheels\third_party
git clone <URL to libporto repo>
cd ..
mkdir build-ggp
cd build-ggp
cmake -G "Visual Studio 16 2019" -DCMAKE_TOOLCHAIN_FILE="C:\\Program Files\\GGP SDK\\cmake\\ggp.cmake" .. -DPPX_DXVK=1
```
Open `BigWheels.sln` and build

Built binaries are written to `BigWheels\bin`.

**NOTE:**
* **libporto** must be manually cloned into BigWheels\third_party.
* GGP supplied Vulkan headers and libraries are used for building *but* the build system will look for the DXC executable in the Vulkan SDK directory.  


## GGP (on Linux)
```
git clone --recursive git@github.com:googlestadia/BigWheels.git
cd BigWheels
mkdir build-ggp
cd build-ggp
cmake -DCMAKE_TOOLCHAIN_FILE=$PATH_TO_GGP_SDK/cmake/ggp.cmake -DDXC_PATH=$PATH_TO_VULKAN_SDK/x86_64/bin/dxc -DPPX_GGP=true ..
make -j <# CPUs to use>
```

Built binaries are written to `BigWheels/bin`.


### Running on GGP
Push the `assets` folder up to the instance before running. Since the shaders are compiled per project they must be built and pushed *before* running.
```
ggp ssh put -r assets
```


## Linux
```
git clone --recursive git@github.com:googlestadia/BigWheels.git
cd BigWheels
mkdir build-make
cd build-make
cmake ..
make -j <# CPUs to use>
```

Built binaries are written to `BigWheels/bin`.

## Windows 
```
git clone --recursive git@github.com:googlestadia/BigWheels.git
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
