#include "Demo.h"

#include <array>
#include <iostream>
#include <ImGui/imgui.h>

#include "GLFWLib.h"

namespace
{
    constexpr int s_TextureWidth = 2;
    constexpr GLuint s_AttributePosition = 0;
    constexpr GLuint s_AttributeTextureIndex = 1;
    const char* vertexShaderStr = R"(#version 300 es
        precision mediump float;

        layout (location = 0) in vec2 position;
        layout (location = 1) in float inTextureIndex;
        out float textureIndex;

        uniform vec2 transform;

        void main() {
            gl_Position = vec4(position + transform, 0.0, 1.0);
            textureIndex = inTextureIndex;
        }
    )";

    const char* fragmentShaderStr = R"(#version 300 es
        precision mediump float;

        in float textureIndex;
        out vec4 FragColor;

        uniform sampler2D ourTexture;

        void main() {
            vec4 color = texture(ourTexture, vec2(textureIndex, 0.0));
            FragColor = color;
        }
    )";

    GLuint compile_shader(GLenum type, const char* name, const char* src) {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &src, NULL);
        glCompileShader(shader);
        GLint compiled;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled)
        {
            std::array<char, 4096> buffer;
            GLsizei length;
            glGetShaderInfoLog(shader, static_cast<GLsizei>(buffer.size()), &length, buffer.data());
            printf("Error compiling shader \"%s\": %s\n", name, buffer.data());
        }
        return shader;
    }

    void setTextureData(const double time)
    {
        const uint8_t animation = static_cast<uint8_t>(std::abs(static_cast<uint8_t>(time * 100.f) - 128));
        std::array<uint8_t, 6> data = { animation, 0, 255, static_cast<uint8_t>(255 - animation), 0, 0 };

        const GLsizei height = 1;
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, s_TextureWidth, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data.data());
    }
}

void Demo::Update(const double time, const float /*deltaTime*/)
{
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(m_Program);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glVertexAttribPointer(s_AttributePosition, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(s_AttributePosition);

    glVertexAttribPointer(s_AttributeTextureIndex, 1, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(s_AttributeTextureIndex);

    glBindTexture(GL_TEXTURE_2D, m_Texture);
    setTextureData(time);

    const float animation = static_cast<float>(std::abs(static_cast<uint8_t>(time * 100.f) - 128)) / 200.f;
    GLint transformUniform = glGetUniformLocation(m_Program, "transform");
    glUniform2f(transformUniform, 0, animation);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("ImGui menu"))
        {
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void Demo::Init()
{
    GLuint vs = compile_shader(GL_VERTEX_SHADER, "vertex", vertexShaderStr);
    GLuint fs = compile_shader(GL_FRAGMENT_SHADER, "fragment", fragmentShaderStr);
    m_Program = glCreateProgram();
    glAttachShader(m_Program, vs);
    glAttachShader(m_Program, fs);
    glLinkProgram(m_Program);
    GLint linked;
    glGetProgramiv(m_Program, GL_LINK_STATUS, &linked);
    if (!linked)
    {
        std::array<char, 4096> buffer;
        GLsizei length;
        glGetProgramInfoLog(m_Program, static_cast<GLsizei>(buffer.size()), &length, buffer.data());
        printf("Error linking program %s\n", buffer.data());
    }

    glGenTextures(1, &m_Texture);
    glBindTexture(GL_TEXTURE_2D, m_Texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    setTextureData(0.0);
    glGenerateMipmap(GL_TEXTURE_2D);

    const float fWidth = s_TextureWidth;
    float vertices[] = {
        // positions    texture index
        0.0f,  0.5f,    0.f / fWidth,
       -0.5f, -0.5f,    1.f / fWidth,
        0.5f, -0.5f,    2.f / fWidth
    };

    glGenBuffers(1, &m_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}
