file(GLOB imgui_sources
	imgui/*.cpp
	imgui/*.h
	imgui/misc/cpp/imgui_stdlib.*)

list(APPEND imgui_sources
	imgui/backends/imgui_impl_glfw.cpp
	imgui/backends/imgui_impl_glfw.h
	imgui/backends/imgui_impl_opengl3.cpp
	imgui/backends/imgui_impl_opengl3.h
	imgui/misc/debuggers/imgui.natvis
	imgui/misc/debuggers/imgui.natstepfilter)

add_library(ImGui STATIC ${imgui_sources})
target_include_directories(ImGui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
target_include_directories(ImGui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends)
target_include_directories(ImGui PUBLIC ${CMAKE_SOURCE_DIR}/${glfw_include_dir})
