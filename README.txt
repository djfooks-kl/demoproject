How to build MSVC:

Start up "Developer Command Prompt for VS 2022"
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
