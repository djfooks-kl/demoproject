#pragma once

#ifdef EMSCRIPTEN
# include <GLES3/gl3.h>
#else
# include <glad/gles2.h>
# define GLFW_INCLUDE_NONE
#endif

#include <GLFW/glfw3.h>