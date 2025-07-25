#pragma once

#ifdef EMSCRIPTEN
#include <GLES2/gl2.h>
#else
#include <glad/gles2.h>
#define GLFW_INCLUDE_NONE
#endif
#include <GLFW/glfw3.h>