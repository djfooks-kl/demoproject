add_library(flecs STATIC
	flecs/flecs.c
	flecs/flecs.h)
target_include_directories(flecs PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_options(flecs PRIVATE
	"$<${msvc_cxx}:/wd4127>")
