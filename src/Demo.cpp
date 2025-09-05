#include "Demo.h"

#include <array>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ImGui/imgui.h>
#include <ImGui/misc/cpp/imgui_stdlib.h>
#include <glm/vec2.hpp>

#include "DemoColorAnimationComponent.h"
#include "DemoSystem.h"
#include "Font.h"
#include "GLFWLib.h"
#include "TextRenderer.h"

namespace
{
    constexpr int s_TextureWidth = 2;
    constexpr GLuint s_AttributePosition = 0;
    constexpr GLuint s_AttributeTextureIndex = 1;

    GLuint CompileShader(GLenum type, const char* name, const char* path)
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

    GLuint BuildShaderProgram(const char* name, const char* vertexPath, const char* fragmentPath)
    {
        const GLuint vs = CompileShader(GL_VERTEX_SHADER, "vertex", vertexPath);
        const GLuint fs = CompileShader(GL_FRAGMENT_SHADER, "fragment", fragmentPath);
        const GLuint program = glCreateProgram();
        glAttachShader(program, vs);
        glAttachShader(program, fs);
        glLinkProgram(program);
        GLint linked;
        glGetProgramiv(program, GL_LINK_STATUS, &linked);
        if (!linked)
        {
            std::array<char, 4096> buffer;
            GLsizei length;
            glGetProgramInfoLog(program, static_cast<GLsizei>(buffer.size()), &length, buffer.data());
            printf("Error linking program \"%s\":\n%s\n", name, buffer.data());
            return 0;
        }
        return program;
    }

    void SetTextureData(const flecs::world& world)
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

Demo::Demo()
{
}

Demo::~Demo()
{
    // TODO clean up all of the GL objects!!!!!
}

void Demo::Update(const double time, const float deltaTime)
{
    demo_system::Update(m_World, time, deltaTime);

    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(m_Program);
    glBindVertexArray(m_DemoVBO);
    glBindTexture(GL_TEXTURE_2D, m_Texture);
    SetTextureData(m_World);

    const float animation = static_cast<float>(std::abs(static_cast<uint8_t>(time * 100.f) - 128)) / 200.f;
    GLint transformUniform = glGetUniformLocation(m_Program, "transform");
    glm::vec2 transformVec(0.f, animation);
    glUniform2f(transformUniform, transformVec.x, transformVec.y);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    m_TextRenderer->Draw();

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("ImGui menu"))
        {
            if (ImGui::MenuItem("Settings"))
            {
                m_SettingsOpen = true;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    if (ImGui::Begin("Settings", &m_SettingsOpen))
    {
        bool textDirty = false;
        textDirty |= ImGui::SliderFloat("Font Size", &m_FontSize, 0.f, 15.f);
        textDirty |= ImGui::SliderFloat2("Text Position", &m_Position.x, -10.f * m_FontSize, 10.f * m_FontSize);
        textDirty |= ImGui::SliderFloat3("Color", &m_Color.x, 0.f, 1.f);
        textDirty |= ImGui::InputText("Text", &m_Text);

        if (textDirty)
        {
            m_TextRenderer->RemoveAllStrings();
            m_TextRenderer->AddString(m_Text, m_FontSize, m_Position.x, m_Position.y, m_Color);
        }
        ImGui::End();
    }
}

void Demo::Init()
{
    m_Program = BuildShaderProgram("Demo", DATA_DIR "/demovertex.glsl", DATA_DIR "/demofragment.glsl");
    m_TextProgram = BuildShaderProgram("Text", DATA_DIR "/vertex.glsl", DATA_DIR "/fragment.glsl");

    m_Font = std::make_unique<Font>();
    m_Font->Load(DATA_DIR "/sourcecodepro-medium.png", DATA_DIR "/sourcecodepro-medium.json");

    m_TextRenderer = std::make_unique<TextRenderer>(*m_Font, m_TextProgram);
    m_TextRenderer->AddString(m_Text, m_FontSize, m_Position.x, m_Position.y, m_Color);

    glGenTextures(1, &m_Texture);
    glBindTexture(GL_TEXTURE_2D, m_Texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    SetTextureData(m_World);
    glGenerateMipmap(GL_TEXTURE_2D);

    const float size = 0.5f;
    float positions[] = {
       -size, -size,
        size, -size,
       -size,  size,
        size,  size,
    };

    const float fWidth = s_TextureWidth;
    float textureU[] = {
        1.f / fWidth,
        0.f / fWidth,
        2.f / fWidth,
        1.f / fWidth
    };

    //   2---3
    //   | \ |
    //   0---1
    // anti-clockwise winding
    unsigned int indices[] = {
        0,1,2,2,1,3
    };

    glGenBuffers(1, &m_PositionsBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_PositionsBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);

    glGenBuffers(1, &m_TextureUBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_TextureUBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(textureU), textureU, GL_STATIC_DRAW);

    glGenBuffers(1, &m_IndicesBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndicesBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &m_DemoVBO);
    {
        glBindVertexArray(m_DemoVBO);

        glBindBuffer(GL_ARRAY_BUFFER, m_PositionsBuffer);
        glVertexAttribPointer(s_AttributePosition, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(s_AttributePosition);

        glBindBuffer(GL_ARRAY_BUFFER, m_TextureUBuffer);
        glVertexAttribPointer(s_AttributeTextureIndex, 1, GL_FLOAT, GL_FALSE, 1 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(s_AttributeTextureIndex);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndicesBuffer);

        glBindVertexArray(0);
    }
}
