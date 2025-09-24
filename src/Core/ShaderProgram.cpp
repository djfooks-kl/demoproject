#include "Core/ShaderProgram.h"

#include <array>
#include <format>
#include <fstream>

#include "Core/GLFWLib.h"
#include "Core/ShaderHelpers.h"

xc::ShaderProgram::ShaderProgram(xc::ShaderProgramOptions options)
    : m_Options(std::move(options))
{
}

xc::Result<void> xc::ShaderProgram::TryLoad()
{
    xc::Result<GLuint> vertexResult = xc::CompileShader(GL_VERTEX_SHADER, std::format("{}{}", DATA_DIR, m_Options.m_VertexPath));
    if (!vertexResult)
    {
        return xc::ResultError{ vertexResult.GetError() };
    }
    xc::Result<GLuint> fragmentResult = xc::CompileShader(GL_FRAGMENT_SHADER, std::format("{}{}", DATA_DIR, m_Options.m_FragmentPath));
    if (!fragmentResult)
    {
        return xc::ResultError{ fragmentResult.GetError() };
    }
    m_Program = glCreateProgram();
    glAttachShader(m_Program, vertexResult.Get());
    glAttachShader(m_Program, fragmentResult.Get());
    glLinkProgram(m_Program);
    GLint linked;
    glGetProgramiv(m_Program, GL_LINK_STATUS, &linked);
    if (linked)
    {
        return {};
    }

    std::array<char, 4096> buffer;
    GLsizei length;
    glGetProgramInfoLog(m_Program, static_cast<GLsizei>(buffer.size()), &length, buffer.data());
    return xc::ResultError{ buffer.data() };
}

void xc::ShaderProgram::TryLoadAndOutputError()
{
    xc::Result<void> result = TryLoad();
    if (!result)
    {
        printf("%s\n", result.GetError().c_str());
    }
}