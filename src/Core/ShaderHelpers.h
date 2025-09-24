#pragma once

#include <string>

#include "Core/Result.h"

typedef unsigned int GLuint;
typedef unsigned int GLenum;

namespace xc
{
	xc::Result<GLuint> CompileShader(const GLenum type, const std::string& path);
}