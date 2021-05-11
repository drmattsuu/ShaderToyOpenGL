set (_src_root_path "${CMAKE_SOURCE_DIR}/src")

add_executable(GLShaderToy
    "main.cpp"
    "Drawing/GLRenderWindow.cpp")

target_link_libraries(GLShaderToy
    PUBLIC Boost::filesystem
    PUBLIC Boost::boost
    PUBLIC ${ALL_LIBS}
)

target_include_directories(GLShaderToy
    PRIVATE
    SYSTEM PUBLIC
        ${CMAKE_SOURCE_DIR}/thirdparty/glfw/include
        ${CMAKE_SOURCE_DIR}/thirdparty/glew-2.1.0/include
        ${CMAKE_SOURCE_DIR}/thirdparty/imgui
)

install(TARGETS GLShaderToy
    CONFIGURATIONS Debug
    RUNTIME DESTINATION GLShaderToy)

install(TARGETS GLShaderToy
    CONFIGURATIONS Release
    RUNTIME DESTINATION GLShaderToy)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/resources DESTINATION GLShaderToy)