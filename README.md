This is a basic demo-project with all the parts needed to start making a web game with c++.
Try it out [here](https://djfooks-kl.github.io/demoproject/pages/DemoProject.html)

This demo project can be either
 - Built with Visual Studio 2022 and run as an executable
 - Built with Emscripten and run in a web page

This means you can still test and debug the game in visual studio but can also at any time run it in a website.

Includes some libraries to get the ball rolling:
- GLFW (including glad with OpenGL2.0 bindings)
- glm
- ImGUI
- flecs
- catch2

### To build with MSVC

```
# Start up "Developer Command Prompt for VS 2022"
mkdir build
cd build
cmake ..
cmake --build .
```

### To build with Emscripten (for web)

```
emsdk_env
mkdir build_web
cd build_web
emcmake cmake ..
cmake --build .
```
