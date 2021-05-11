
## Quick vars to remove some extra guff
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLEW_BUILD_DOCS FALSE)
set(GLFW_INSTALL FALSE)
set(GLM_TEST_ENABLE FALSE)
set(BOOST_DISABLE_TESTS TRUE)

### GLFW ###

add_subdirectory (glfw)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
set(OPENGL_LIBRARY
    ${OPENGL_LIBRARY}
    -lGL -lGLU -lXrandr -lXext -lX11 -lrt
    ${CMAKE_DL_LIBS}
    ${GLFW_LIBRARIES}
)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

### GLEW ###

add_library(glew STATIC
    glew-2.1.0/src/glew.c
)

target_link_libraries(glew
    ${OPENGL_LIBRARY}
    ${EXTRA_LIBS}
)

target_include_directories(glew 
    PUBLIC
    glew-2.1.0/include
)

### GLM ###

add_subdirectory(glm)

## Boost ##

add_subdirectory(boost-cmake)

## ImGui ##

add_library(imgui
	imgui/imgui.cpp
	imgui/imgui_demo.cpp
	imgui/imgui_draw.cpp
	imgui/imgui_tables.cpp
	imgui/imgui_widgets.cpp
)

target_include_directories(imgui
    PUBLIC
    imgui
)

