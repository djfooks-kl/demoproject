#include "Demo.h"

#include <array>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ImGui/imgui.h>
#include <glm/vec2.hpp>
#include <stb/stb_image.h>

#include "GLFWLib.h"

namespace
{
    constexpr int s_TextureWidth = 2;
    constexpr int s_TextureHeight = 2;
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

    void setTextureData()
    {
        int width, height, channels;
        unsigned char *data = stbi_load(DATA_DIR "/sourcecodepro-medium.png", &width, &height, &channels, 0);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);
    }
}

void Demo::Update(const double time, const float /*deltaTime*/)
{
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(m_Program);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glVertexAttribPointer(s_AttributePosition, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(s_AttributePosition);

    glVertexAttribPointer(s_AttributeTextureIndex, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(s_AttributeTextureIndex);

    glBindTexture(GL_TEXTURE_2D, m_Texture);

    const float animation = static_cast<float>(std::abs(static_cast<uint8_t>(time * 100.f) - 128)) / 500.f;
    GLint transformUniform = glGetUniformLocation(m_Program, "transform");
    glm::vec2 transformVec(0.f, animation);
    glUniform2f(transformUniform, transformVec.x, transformVec.y);

    const GLint pxrangeUniform = glGetUniformLocation(m_Program, "u_pxrange");
    glUniform1f(pxrangeUniform, m_Pxrange);
    const GLint fontSizeUniform = glGetUniformLocation(m_Program, "u_fontSize");
    glUniform1f(fontSizeUniform, m_FontSize);
    const GLint weightUniform = glGetUniformLocation(m_Program, "u_weight");
    glUniform1f(weightUniform, m_Weight);
    GLint colorUniform = glGetUniformLocation(m_Program, "u_color");
    glUniform3f(colorUniform, m_Color.x, m_Color.y, m_Color.z);

    glDrawArrays(GL_TRIANGLES, 0, 6);

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
        ImGui::SliderFloat("PX Range", &m_Pxrange, 0.f, 1.f);
        ImGui::SliderFloat("Font Size", &m_FontSize, 0.f, 100.f);
        ImGui::SliderFloat("Weight", &m_Weight, 0.f, 1.f);
        ImGui::SliderFloat3("Color", &m_Color.x, 0.f, 1.f);
        ImGui::End();
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    setTextureData();
    glGenerateMipmap(GL_TEXTURE_2D);

    float size = 0.8f;
    float vertices[] = {
        // positions    texture UV
        -size,  size,     0.f, 0.f,
        -size,  -size,    0.f, 1.f,
        size,   -size,    1.f, 1.f,

        -size,  size,     0.f, 0.f,
        size,   size,     1.f, 0.f,
        size,   -size,    1.f, 1.f
    };

    glGenBuffers(1, &m_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}
