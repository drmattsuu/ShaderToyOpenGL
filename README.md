# ShaderToyOpenGL

## Basic Checkout and Build Process
### Requirements
- CMake v3.15 or newer (https://cmake.org/download/)
- A compiler that supports C++17
- Boost 17.0 commpied for msvc141 in `/thirdparty/boost_1_70_0`

### Third-party Libraries
From the root of this repo run the following commands to fetch third-party dependencies;
  ```
  $ git submodule init
  $ git submodule update --recursive
  ``` 

### Building - Windows
- Open the root of this repo in Visual Studio 2019 (community edition should work fine)
- VS should detect the `CMakeLists.txt` file at root and offer to generate the cmake cache.
  - If not, right click this file and select "Generate Cache For ..."
- Build using Ctrl+Shift+B or use Build > Build All via the menu.
