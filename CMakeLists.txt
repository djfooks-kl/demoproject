cmake_minimum_required(VERSION 3.15)

project(DemoProject VERSION 1.0)

add_library(project_compiler_flags INTERFACE)
target_compile_features(project_compiler_flags INTERFACE cxx_std_20)

set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU,LCC,Emscripten>")
set(msvc_cxx "$<COMPILE_LANG_AND_ID:CXX,MSVC>")

target_compile_options(project_compiler_flags INTERFACE
    "$<${gcc_like_cxx}:$<BUILD_INTERFACE:-Wall;-Wextra;-Wshadow;-Wformat=2;-Wunused>>"
    "$<${msvc_cxx}:$<BUILD_INTERFACE:-W4>>"
)

set(source_files
    src/BaseApp.cpp
    src/BaseApp.h
    src/Demo.cpp
    src/Demo.h
    src/Font.cpp
    src/Font.h
    src/TextRenderer.cpp
    src/TextRenderer.h
    src/GLFWLib.h
    src/STBImage.cpp)

if (EMSCRIPTEN)
    message(STATUS "This is an Emscripten build.")
    list(APPEND source_files
        src/WebApp.cpp
        src/WebApp.h)

    target_link_options(project_compiler_flags INTERFACE
        -sUSE_WEBGL2=1
        -sUSE_GLFW=3
        -sSTACK_SIZE=1mb
        -sEXPORTED_RUNTIME_METHODS=cwrap
        -sALLOW_MEMORY_GROWTH=1
        --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/data@./data)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    configure_file(scripts/server.sh.in server.sh)
else()
    list(APPEND source_files
        src/WindowApp.cpp
        src/WindowApp.h
        3rdParty/glad/src/gles2.c)
    set(glfw_dir 3rdParty/glfw-3.4)
    set(glfw_include_dir ${glfw_dir}/include)
    add_subdirectory(${glfw_dir})
endif()

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT DemoProject)

add_library(DemoProjectLib STATIC ${source_files})
target_include_directories(DemoProjectLib PUBLIC src)

if (EMSCRIPTEN)
    target_compile_definitions(DemoProjectLib PRIVATE DATA_DIR="./data/")
else()
    target_compile_definitions(DemoProjectLib PRIVATE DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data/")
endif()

add_executable(DemoProject src/main.cpp)
target_link_libraries(DemoProject PUBLIC DemoProjectLib)

if (NOT EMSCRIPTEN)
    target_include_directories(DemoProjectLib PUBLIC 3rdParty/glad/include)
    target_include_directories(DemoProjectLib PUBLIC ${glfw_include_dir})
    target_link_libraries(DemoProjectLib PUBLIC glfw ${GLFW_LIBRARIES})
endif()

add_subdirectory(3rdParty/glm-1.0.1)
target_link_libraries(DemoProjectLib PUBLIC glm::glm-header-only)

target_include_directories(DemoProjectLib PUBLIC 3rdParty/stb)
target_include_directories(DemoProjectLib PUBLIC 3rdParty/nlohmann-json)

add_subdirectory(3rdParty/imgui-1.92.1)
target_include_directories(DemoProjectLib PUBLIC 3rdParty/imgui-1.92.1)
target_link_libraries(DemoProjectLib PUBLIC ImGui)

add_subdirectory(3rdParty/flecs-4.1.0)
target_link_libraries(DemoProjectLib PUBLIC flecs)

target_compile_definitions(DemoProjectLib PRIVATE
    "$<$<COMPILE_LANG_AND_ID:CXX,Emscripten>:EMSCRIPTEN_BUILD>"
)

target_link_libraries(DemoProjectLib PUBLIC project_compiler_flags)

if (NOT EMSCRIPTEN)
    add_subdirectory(3rdParty/Catch2)
    add_executable(tests
        src/ut/test.cpp)
    target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
    target_link_libraries(tests PRIVATE DemoProjectLib)

    include(Catch)
    catch_discover_tests(tests)
endif()
