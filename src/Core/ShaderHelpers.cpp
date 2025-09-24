#include "Core/ShaderHelpers.h"

#include <array>
#include <format>
#include <fstream>
#include <sstream>

#include "Core/GLFWLib.h"
#include "Core/Result.h"

xc::Result<GLuint> xc::CompileShader(const GLenum type, const std::string& path)
{
    std::ifstream fileStream(path);
    if (fileStream.fail())
    {
        return xc::ResultError{ std::format("Error could not read file \"{}\"", path) };
    }
    std::stringstream buffer;
    buffer << fileStream.rdbuf();
    std::string string = buffer.str();
    const char* cstring = string.c_str();
    const GLint cstrLength = static_cast<GLint>(string.length());

    const GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &cstring, &cstrLength);
    glCompileShader(shader);
    GLint compiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (compiled)
    {
        return shader;
    }

    std::array<char, 4096> infoBuffer;
    GLsizei length;
    glGetShaderInfoLog(shader, static_cast<GLsizei>(infoBuffer.size()), &length, infoBuffer.data());
    return xc::ResultError{ std::format("Error compiling shader \"{}\": {}", path, infoBuffer.data()) };
}