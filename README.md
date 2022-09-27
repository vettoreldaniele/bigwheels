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
       * 1.3.216.0 or later
   * Linux
     * Visual Studio Code or CLI
     * Recent Vulkan SDK
       * 1.3.216.0 or later
   * Windows
     * Visual Studio 2019 Professional or Visual Studio Code
     * Recent Vulkan SDK
       * 1.3.216.0 or later
     * Recent version of Windows SDK
       * 10.0.22621.0 or later
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
The recommended build system is CMake. This repo supports both in-tree, and out-of-tree builds.
For Windows, the recommended generator is `Visual Studio 2019`, for linux, `Ninja`.

Binaries are written to `<build-dir>/bin` and libraries to `<build-dir>/lib`.

Shaders are also written to `<build-dir>`, but prefixed with their path and the format:

- For SPIR-V `$REPO/assets/shaders/my_shader.hlsl` is compiled to `$BUILD_DIR/assets/shaders/spv/my_shader.spv`
- For DXBC51 `$REPO/assets/shaders/my_shader.hlsl` is compiled to `$BUILD_DIR/assets/shaders/dxbc51/my_shader.dxbc51`

## GGP (on Windows)
```
git clone --recursive https://github.com/googlestadia/BigWheels
cd BigWheels
cmake -B build-ggp -G "Visual Studio 16 2019" -DCMAKE_TOOLCHAIN_FILE="C:\\Program Files\\GGP SDK\\cmake\\ggp.cmake"
```
Open `build-ggp/BigWheels.sln` and build

Built binaries are written to `build-ggp\bin\vk_*`.

**NOTE:** GGP supplied Vulkan headers and libraries are used for building *but* the build system will look for the DXC
executable in the Vulkan SDK directory.

## GGP (on Linux)
```
git clone --recursive https://github.com/googlestadia/BigWheels
cd BigWheels
cmake . -GNinja \
    -DCMAKE_TOOLCHAIN_FILE=$PATH_TO_GGP_SDK/cmake/ggp.cmake \
    -DDXC_PATH=$PATH_TO_VULKAN_SDK/x86_64/bin/dxc \
    -DPPX_GGP=true
ninja
```

Built binaries are written to `bin/vk_*`.

## GGP using dxil-spirv (on Linux)
```
git clone --recursive https://github.com/googlestadia/BigWheels
cd BigWheels
cmake . -GNinja \
    -DCMAKE_TOOLCHAIN_FILE=$PATH_TO_GGP_SDK/cmake/ggp.cmake \
    -DDXC_PATH=$PATH_TO_VULKAN_SDK/x86_64/bin/dxc \
    -DDXIL_SPIRV_PATH=$PATH_TO_DXIL_SPIRV_SRC/build/dxil-spirv \
    -DPPX_DXIL_SPV=true
ninja
```

Built binaries are written to `bin/dxil_spv_*`.

### Running on GGP
Push the `assets` folder up to the instance before running. Since the shaders are compiled per project they must be built and pushed *before* running.
```
ggp ssh put -r assets
```

## Linux
```
git clone --recursive https://github.com/googlestadia/BigWheels
cmake . -GNinja
ninja
```

Built binaries are written to `bin/`.

## Windows
```
git clone --recursive https://github.com/googlestadia/BigWheels
cd BigWheels
cmake -B build -G "Visual Studio 16 2019" -A x64
```

Open `build\BigWheels.sln` and build

Built binaries are written to `build\bin`.

# Shader Compilation
Shader binaries are generated during project build. Because this project supports multiple APIs, we build them
for each one depending on the need. API support depends on the system nature and configuration:
    - For GGP and Linux, only SPIR-V is generated.
    - For Windows DXBC, DXIL, and SPIR-V is generated.

To request a specific API, flags can be passed to Cmake:
 - PPX_D3D12 : DirectX12 support.
 - PPX_D3D11 : DirectX11 support.
 - PPX_VULKAN : Vulkan support with SPIR-V.
 - PPX_DXIL_SPV : Vulkan support with SPIR-V compiled from DXIL.

All targets require DXC.
DirectX11 and DirectX12 require FXC.

## DXC
The build system will look for `dxc.exe` or `dxc` in the Vulkan SDK bin directory.
The DXC path can also be provided using `-DDXC_PATH=<path to DXC executable>`.
**DXC is REQUIRED for this project.**

## FXC
The build system will look for `fxc.exe` in the Windows SDK version that CMake selects.
The FXC path can also be provided using `-DFXC_PATH=<path to FXC executable>`.

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
