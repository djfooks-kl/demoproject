#pragma once

#include <string>

#include "Core/Result.h"

typedef unsigned int GLuint;

namespace xc
{
    struct ShaderProgramOptions
    {
        std::string m_VertexPath;
        std::string m_FragmentPath;
    };

    class ShaderProgram
    {
    public:
        ShaderProgram(ShaderProgramOptions options);

        xc::Result<void> TryLoad();
        void TryLoadAndOutputError();

        GLuint GetProgramId() const { return m_Program; }

    private:
        ShaderProgramOptions m_Options;
        GLuint m_Program = 0;
    };
}