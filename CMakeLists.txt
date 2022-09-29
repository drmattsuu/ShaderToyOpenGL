cmake_minimum_required (VERSION 3.14)
project (GLShaderToy VERSION 0.0.1)

find_package(OpenGL REQUIRED)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

message("CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
message("CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")

if(CMAKE_SYSTEM MATCHES "Windows")
    add_definitions(-DGLFW_EXPOSE_NATIVE_WIN32)
    add_definitions(-DGLFW_EXPOSE_NATIVE_WGL)
    set(EXTRA_LIBS ${EXTRA_LIBS} winmm)
endif(CMAKE_SYSTEM MATCHES "Windows")

if( CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR )
    message( FATAL_ERROR "Please select another Build Directory!" )
endif()
if( CMAKE_SOURCE_DIR MATCHES " " )
    message( "Your Source Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()
if( CMAKE_BINARY_DIR MATCHES " " )
    message( "Your Build Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    message("This software does not currently offically support linux!")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    message(FATAL_ERROR "This software does not support MacOS")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/thirdparty/cmake-modules")

include(CreateLaunchers)
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    include(MSVCMultipleProcessCompile) # /MP
    add_definitions(
        -DTW_STATIC
        -DTW_NO_LIB_PRAGMA
        -DTW_NO_DIRECT3D
        -DGLEW_STATIC
    )
endif()


if (${CMAKE_BUILD_TYPE} MATCHES "Debug")
    add_definitions(-DTOY_WORKING_DIR=\"${CMAKE_SOURCE_DIR}\")
endif ()

if(EXISTS "${CMAKE_SOURCE_DIR}/thirdparty/boost_1_70_0")
    if(NOT DEFINED FETCHCONTENT_SOURCE_DIR_BOOST)
        set(FETCHCONTENT_SOURCE_DIR_BOOST "${CMAKE_SOURCE_DIR}/thirdparty/boost_1_70_0")
    endif()
endif()

# Compile thirdparty dependencies 
add_subdirectory (thirdparty)

set(ALL_LIBS
  ${OPENGL_LIBRARY}
  glfw
  glew
  glm
  imgui
  assimp
)

add_subdirectory (src)
