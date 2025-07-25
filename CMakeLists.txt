cmake_minimum_required(VERSION 3.15)

project(EmImEntGL VERSION 1.0)

add_library(project_compiler_flags INTERFACE)
target_compile_features(project_compiler_flags INTERFACE cxx_std_20)

set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU,LCC,Emscripten>")
set(msvc_cxx "$<COMPILE_LANG_AND_ID:CXX,MSVC>")

target_compile_options(project_compiler_flags INTERFACE
    "$<${gcc_like_cxx}:$<BUILD_INTERFACE:-Wall;-Wextra;-Wshadow;-Wformat=2;-Wunused>>"
    "$<${msvc_cxx}:$<BUILD_INTERFACE:-W4>>"
)

set(cpp_files
    src/main.cpp
    src/BaseApp.cpp)

if (EMSCRIPTEN)
    message(STATUS "This is an Emscripten build.")
    target_link_options(project_compiler_flags INTERFACE
        -sUSE_WEBGL2=1
        -sUSE_GLFW=3
        -sALLOW_MEMORY_GROWTH=1)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    configure_file(scripts/server.sh.in server.sh)
    list(APPEND cpp_files src/WebApp.cpp)
else()
    list(APPEND cpp_files src/WindowApp.cpp)
    list(APPEND cpp_files 3rdParty/glad/src/gles2.c)
    include_directories(3rdParty/glad/include)
    add_subdirectory(3rdParty/glfw-3.4)
endif()

include_directories(src)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT EmImEntGL)

add_executable(EmImEntGL ${cpp_files})

if (NOT EMSCRIPTEN)
    target_link_libraries(EmImEntGL PRIVATE glfw ${GLFW_LIBRARIES})
endif()

target_compile_definitions(EmImEntGL PRIVATE
    "$<$<COMPILE_LANG_AND_ID:CXX,Emscripten>:EMSCRIPTEN_BUILD>"
)

target_link_libraries(EmImEntGL PUBLIC project_compiler_flags)

install(TARGETS EmImEntGL DESTINATION bin)
