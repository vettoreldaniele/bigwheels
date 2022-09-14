# Overview
BigWheels is a cross-platform, API agnostic framework to build graphics applications.

## Sample Prefixes
 * **dx11** - D3D11 with SM 5.0
   * Shaders are compiled with FXC
 * **dx12** - D3D12 with SM 5.1
   * Shaders are compiled with FXC
 * **dxil** - D3D12 with SM 6.0+
   * Shaders are compiled with DXC/DXIL
 * **dxil_spv** - Vulkan
   * Shaders are compiled with DXC/DXIL followed by dxil-spirv
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
       * 10.0.22000 or later
 * Hardware
   * AMD
     * Vega GPUs
       * Vega 56
       * Vega 64
     * Navi GPUs
        * Radeon 5700
        * Radeon 5700 XT
        * Radeon 6700 XT
   * NVIDIA
     * 20x0 GPUs
     * 16x0 GPus
     * 10x0 GPUs
       * *This is iffy - some of the later more recent DX12 and Vulkan features many not work.*
    * Intel
       * None tested

# Build Instructions

By default, built binaries are written to the source directory, under `BigWheels\bin`.
For fully out of tree builds, pass `-DPPX_OUTPUT_DIR=<your-build-dir>` to CMake.
This will make CMake copy both binaries and assets to `<your-build-dir>`.

## GGP (on Windows)
```
git clone --recursive https://github.com/googlestadia/BigWheels
cd BigWheels
mkdir build-ggp
cd build-ggp
cmake -G "Visual Studio 16 2019" -DCMAKE_TOOLCHAIN_FILE="C:\\Program Files\\GGP SDK\\cmake\\ggp.cmake" ..
```
Open `BigWheels.sln` and build

Built binaries are written to `PPX_OUTPUT_DIR\bin\vk_*`.

**NOTE:** GGP supplied Vulkan headers and libraries are used for building *but* the build system will look for the DXC executable in the Vulkan SDK directory.

## GGP (on Linux)
```
git clone --recursive https://github.com/googlestadia/BigWheels
cd BigWheels
mkdir build-ggp
cd build-ggp
cmake -DCMAKE_TOOLCHAIN_FILE=$PATH_TO_GGP_SDK/cmake/ggp.cmake -DDXC_PATH=$PATH_TO_VULKAN_SDK/x86_64/bin/dxc -DPPX_GGP=true ..
make -j <# CPUs to use>
```

Built binaries are written to `PPX_OUTPUT_DIR/bin/vk_*`.

## GGP using dxil-spirv (on Linux)

```
git clone --recursive https://github.com/googlestadia/BigWheels
cd BigWheels
mkdir build-dxil-spirv
cd build-dxil-spirv
cmake -DCMAKE_TOOLCHAIN_FILE=$PATH_TO_GGP_SDK/cmake/ggp.cmake -DDXC_PATH=$PATH_TO_VULKAN_SDK/x86_64/bin/dxc -DDXIL_SPIRV_PATH=$PATH_TO_DXIL_SPIRV_SRC/build/dxil-spirv -DPPX_DXIL_SPV=true ..
make -j <# CPUs to use>
```

Note: The dxil-spirv should be built from the [experimental/dxiivk](https://porto-internal.googlesource.com/third_party/dxil-spirv/) branch.

Built binaries are written to `PPX_OUTPUT_DIR/bin/dxil_spv_*`.

### Running on GGP
Push the `assets` folder up to the instance before running. Since the shaders are compiled per project they must be built and pushed *before* running.
```
ggp ssh put -r assets
```

## Linux
```
git clone --recursive https://github.com/googlestadia/BigWheels
cd BigWheels
mkdir build-make
cd build-make
cmake ..
make -j <# CPUs to use>
```

Built binaries are written to `PPX_OUTPUT_DIR/bin`.

## Windows
```
git clone --recursive https://github.com/googlestadia/BigWheels
cd BigWheels
mkdir build-vs2019
cd build-vs2019
cmake -G "Visual Studio 16 2019" -A x64 ..
```
Open `BigWheels.sln` and build

Built binaries are written to `PPX_OUTPUT_DIR\bin`.

# Shader Compilation
Shader binaries are generated during project build. For GGP and Linux only SPIR-V binaries are generated. For Windows DXBC, DXIL, and SPIR-V binaries are generated.

## HLSL Binding Semantics support
Build with `-DENABLE_HLSL_BINDING_SEMANTICS=ON` to enable HLSL binding semantics in DXC during shader compilation. Note that this requires `DXC_PATH` to be set to a DXC that supports this flag.

## DXC
The build system will look for `dxc.exe` or `dxc` in the Vulkan SDK bin directory.

To use a custom DXC executable use the following:
```
cmake <platform specific params> -DDXC_PATH=<path to DXC executable>
```
The build system will generate an error if the DXC executable is not present.

## FXC
The build system will look for `fxc.exe` in the Windows SDK version that CMake selects.

# Unit Tests
Unit tests are supported both in CMake and Bazel. For the former, they can be optionally disabled with `-DBUILD_TESTS=OFF`.

To add a new test:
1) Add and write a new test file under `src/test/`, e.g. `src/test/new_test.cpp`.
2) The new test will automatically be picked up by the build system.

To build tests (without running them):
* For Bazel, use `bazel build :ppx_tests`.
* For CMake, use `make build-tests`.

Test binaries are output in `test/`.

To run tests (they will be built if needed):
* For Bazel, use `bazel test :all`.
* For CMake, use `make run-tests`.

# Benchmarks

BigWheels includes a variety of benchmarks that test graphics fundamentals under the `benchmarks` folder. Benchmarks use special assets found in `assets/benchmarks`.

To build and run benchmarks, build BigWheels for your platform of choice. Then, you can run the benchmarks manually or use the provided helper script to run a group of benchmarks specified in a CSV file on a GGP instance.

All benchmarks support the `--stats-file path/to/stats.csv` option that controls where the results in CSV format are written to. Refer to a specific benchmark's code to determine which additional options they support.

## Running benchmarks manually
Once a benchmark is built, its binary will be in `bin/`. Simply run the binary along with any options you want.

Example:
```
bin/vk_texture_sample --stats-file results.csv --num-images 1 --force-mip-level 0 --filter-type linear
```

## Running a group of benchmarks on GGP
You can use the `tools/run-benchmarks.py` script to run a group of benchmarks on a GGP instance and automatically retrieve the results.

You can control which benchmarks are run, and with which options, by modifying `tools/benchmark_testcases.csv` or providing your own. The format of the testcase CSV file is:
```
short_name, descriptive_name, benchmark_binary_name, benchmark_binary_options
```

An example would be:
```
texture_load_1, Texture load (1 image), vk_texture_load, --num-images 1
```

Once you have a list of benchmarks you'd like to run, use the `run-benchmarks.py` script to dispatch their execution. You can control how many frames each benchmark runs for, and the directory where the results will be stored:
```
tools/run-benchmarks.py tools/benchmark_testcases.csv --instance=${USER}-1 --num_frames=20 --out=benchmark_results
```

## Analyzing benchmark results
Each benchmark is different, but all of the GPU benchmarks output a CSV file that contains per-frame performance results. The CSV format differs depending on each benchmark, but all contain at least the following information in the first three columns: frame number, GPU pipeline execution time in milliseconds, CPU frame time in milliseconds. You can refer to a specific benchmark's code to determine what other information is included.

You can use the `tools/compare-benchmark-results.py` script to compare a group of benchmarks across different platforms/settings.  This script accepts a list of directories, each containing benchmark results
from benchmark runs. The first results directory specified on the command line
is used as a baseline, against which all other results are compared against.

Example use:
```
tools/compare-benchmarks-results.py results_dir_1 results_dir_2 results_dir_3
```

# Using SwiftShader

BigWheels can use SwiftShader's Vulkan ICD in place of a GPU's ICD using the `VK_ICD_FILENAMES` environment variable. No special build mode required, just make sure the build for the desired platform has Vulkan enabled. If `VK_ICD_FILENAMES` is present, the Vulkan loader will use its value to load the ICD instead of going through discovery. Using BigWheels with SwiftShader does mean building SwiftShader. To build SwiftShader use the instructions that follow.

## Building SwiftShader on Windows
```
git clone https://swiftshader.googlesource.com/SwiftShader
cd SwiftShader
mkdir build-vs2019
cmake .. -G "Visual Studio 16 2019" -A x64 -Thost=x64 -DSWIFTSHADER_BUILD_EGL=FALSE -DSWIFTSHADER_BUILD_GLESv2=FALSE -DSWIFTSHADER_BUILD_PVR=FALSE -DSWIFTSHADER_BUILD_TESTS=FALSE -DSWIFTSHADER_WARNINGS_AS_ERRORS=FALSE -DREACTOR_ENABLE_MEMORY_SANITIZER_INSTRUMENTATION=FALSE -DSWIFTSHADER_ENABLE_ASTC=FALSE -DSPIRV_SKIP_EXECUTABLES=TRUE
```
Open `SwiftShader.sln` and build.

The DLL and JSON for the ICD are located in `build-vs2019/Windows`.

## Building SwiftShader on Linux
```
git clone https://swiftshader.googlesource.com/SwiftShader
cd SwiftShader
mkdir build-make
cd build-make
cmake .. -DSWIFTSHADER_BUILD_EGL=FALSE -DSWIFTSHADER_BUILD_GLESv2=FALSE -DSWIFTSHADER_BUILD_PVR=FALSE -DSWIFTSHADER_BUILD_TESTS=FALSE -DSWIFTSHADER_WARNINGS_AS_ERRORS=FALSE -DREACTOR_ENABLE_MEMORY_SANITIZER_INSTRUMENTATION=FALSE -DSWIFTSHADER_ENABLE_ASTC=FALSE -DSPIRV_SKIP_EXECUTABLES=TRUE
make -j <# CPUs to use>
```
The .so and JSON for the ICD are located in `build-make/Linux`.

## Using the SwiftShader ICD

The DLL/.so library and JSON manifest can be copied to a different location. Remember to set/export the `VK_ICD_FILENAMES` environment variable before running any of the BigWheels samples.

### Windows
Add `VK_ICD_FILENAMES=C:\absolute\path\to\vk_swiftshader_icd.json` as an entry to `Configuration Properties -> Debugging -> Environment` in the project's property pages.

### Linux
```
export VK_ICD_FILENAMES=/absolute/path/to/vk_swiftshader_icd.json
```
