A basic demo-project with all the parts needed to start making a web game with c++.

Including:
- Emscripten builds
- GLFW
- ImGUI
- flecs
- catch2

How to build MSVC:

	# Start up "Developer Command Prompt for VS 2022"
	mkdir build
	cd build
	cmake ..
	cmake --build .

How to build Emscripten:

	emsdk_env
	mkdir build_web
	cd build_web
	emcmake cmake ..
	cmake --build .
