#include "Demo.h"

#include <array>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ImGui/imgui.h>
#include <glm/vec2.hpp>

#include "DemoSystem.h"
#include "DemoColorAnimationComponent.h"
#include "GLFWLib.h"

namespace
{
    constexpr int s_TextureWidth = 2;
    constexpr GLuint s_AttributePosition = 0;
    constexpr GLuint s_AttributeTextureIndex = 1;

    GLuint compile_shader(GLenum type, const char* name, const char* path)
    {
        std::ifstream fileStream(path);
        if (fileStream.fail())
        {
            printf("Error could not read file \"%s\"\n", path);
            return 0;
        }
        std::stringstream buffer;
        buffer << fileStream.rdbuf();
        std::string string = buffer.str();
        const char* cstring = string.c_str();
        const GLint cstrLength = static_cast<GLint>(string.length());

        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &cstring, &cstrLength);
        glCompileShader(shader);
        GLint compiled;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled)
        {
            std::array<char, 4096> infoBuffer;
            GLsizei length;
            glGetShaderInfoLog(shader, static_cast<GLsizei>(infoBuffer.size()), &length, infoBuffer.data());
            printf("Error compiling shader \"%s\": %s\n", name, infoBuffer.data());
            printf("%s\n", cstring);
        }
        return shader;
    }

    void setTextureData(const flecs::world& world)
    {
        std::array<uint8_t, 6> data = {
            0, 0, 255,
            0, 255, 0 };

        auto query = world.query_builder<const demo::ColorAnimationComponent>();
        query.each([&](const demo::ColorAnimationComponent& color) {
            data[0] = color.m_R;
            data[1] = color.m_G;
            data[2] = color.m_B;
        });

        const GLsizei height = 1;
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, s_TextureWidth, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data.data());
    }
}

void Demo::Update(const double time, const float deltaTime)
{
    demo_system::Update(m_World, time, deltaTime);

    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(m_Program);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glVertexAttribPointer(s_AttributePosition, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(s_AttributePosition);

    glVertexAttribPointer(s_AttributeTextureIndex, 1, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(s_AttributeTextureIndex);

    glBindTexture(GL_TEXTURE_2D, m_Texture);
    setTextureData(m_World);

    const float animation = static_cast<float>(std::abs(static_cast<uint8_t>(time * 100.f) - 128)) / 200.f;
    GLint transformUniform = glGetUniformLocation(m_Program, "transform");
    glm::vec2 transformVec(0.f, animation);
    glUniform2f(transformUniform, transformVec.x, transformVec.y);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("ImGui menu"))
        {
            ImGui::MenuItem("Hello");
            ImGui::MenuItem("World");
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    const bool clickWindow = ImGui::IsMouseReleased(ImGuiMouseButton_Left) && !ImGui::GetIO().WantCaptureMouse;
    if (clickWindow || m_ContextMenuOpen)
    {
        m_ContextMenuOpen = ImGui::BeginPopupContextVoid(nullptr, ImGuiPopupFlags_MouseButtonLeft);
        if (m_ContextMenuOpen)
        {
            ImGui::MenuItem("Hello");
            ImGui::MenuItem("Popup");
            ImGui::EndPopup();
        }
    }
}

void Demo::Init()
{
    GLuint vs = compile_shader(GL_VERTEX_SHADER, "vertex", DATA_DIR "/vertex.glsl");
    GLuint fs = compile_shader(GL_FRAGMENT_SHADER, "fragment", DATA_DIR "/fragment.glsl");
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
    setTextureData(m_World);
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
