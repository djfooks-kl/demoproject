#include "Demo.h"

#include <array>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ImGui/imgui.h>
#include <ImGui/misc/cpp/imgui_stdlib.h>
#include <glm/vec2.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "BoxRenderer.h"
#include "CameraComponent.h"
#include "CameraInputComponent.h"
#include "CameraInputSystem.h"
#include "CameraSystem.h"
#include "Font.h"
#include "GLFWLib.h"
#include "GlobalComponent.h"
#include "InputComponent.h"
#include "InputSystem.h"
#include "MouseTrailSystem.h"
#include "MouseTrailComponent.h"
#include "TextRenderer.h"
#include "WindowSizeComponent.h"
#include "WindowSizeSystem.h"

namespace
{
    constexpr int s_TextureWidth = 2;
    constexpr GLuint s_AttributePosition = 0;
    constexpr GLuint s_AttributeTextureIndex = 1;
    constexpr float s_BlueSpeed = 100.f;
    constexpr float s_GreenSpeed = 150.f;

    // TODO move to BaseApp?
    void WindowSizeCallback(GLFWwindow* /*window*/, int width, int height)
    {
        glViewport(0, 0, width, height);
    }

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

    void SetTextureData(const double time)
    {
        std::array<uint8_t, 6> data = {
            255, 0, static_cast<uint8_t>(time * s_BlueSpeed),
            255, static_cast<uint8_t>(255.f - time * s_GreenSpeed), 0 };

        const GLsizei height = 1;
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, s_TextureWidth, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data.data());
    }
}

Demo::Demo()
{
}

Demo::~Demo()
{
    m_Font.reset();
    m_TextRenderer.reset();

    glDeleteProgram(m_Program);
    glDeleteProgram(m_TextProgram);
    glDeleteVertexArrays(1, &m_DemoVBO);
    glDeleteBuffers(1, &m_PositionsBuffer);
    glDeleteBuffers(1, &m_TextureUBuffer);
    glDeleteBuffers(1, &m_IndicesBuffer);
    glDeleteTextures(1, &m_Texture);
}

void Demo::ProcessKeyInput(GLFWwindow* /*window*/, int key, int scancode, int action, int mods)
{
    input_system::UpdateKeyInput(m_World, key, scancode, action, mods);
}

void Demo::ProcessCursorInput(GLFWwindow* /*window*/, double xpos, double ypos)
{
    input_system::UpdateCursorInput(m_World, xpos, ypos);
}

void Demo::DrawImGui()
{
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

    // scale window to fit contents
    ImGui::SetNextWindowSize(ImVec2{ 0.f, 0.f });
    if (ImGui::Begin("Settings", &m_SettingsOpen))
    {
        ImGui::Text("WASD - Move camera, EQ - Zoom in/out");
        m_World.query<const demo::CameraComponent>()
            .each([&](
                const demo::CameraComponent& camera)
        {
            ImGui::Text("Zoom %.3f", camera.m_Zoom);
            ImGui::Text("Pos %.3f %.3f", camera.m_Position.x, camera.m_Position.y);
        });

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

void Demo::Render(double time, float /*deltaTime*/)
{
    glClear(GL_COLOR_BUFFER_BIT);

    const demo::CameraComponent* camera = nullptr;
    m_World.query<const demo::CameraComponent>()
        .each([&](
            const demo::CameraComponent& cameraComponent)
    {
        camera = &cameraComponent;
    });

    if (!camera)
        return;

    glUseProgram(m_Program);
    glBindVertexArray(m_DemoVBO);
    glBindTexture(GL_TEXTURE_2D, m_Texture);
    SetTextureData(time);

    GLint viewProjectionUniform = glGetUniformLocation(m_Program, "viewProjection");
    glUniformMatrix4fv(viewProjectionUniform, 1, GL_FALSE, glm::value_ptr(camera->m_ViewProjection));

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    m_TextRenderer->Draw(camera->m_ViewProjection);

    m_BoxRenderer->RemoveAllBoxes();
    m_World.query<const demo::MouseTrailComponent>()
        .each([&](
            const demo::MouseTrailComponent& mouseTrail)
    {
        for (const demo::TrailPoint& p : mouseTrail.m_Positions)
        {
            m_BoxRenderer->AddBox(0.05f, p.m_Position, p.m_Color);
        }
    });
    //m_BoxRenderer->AddBox(0.05f, camera->m_WorldMouse, glm::vec3(0.f, 0.f, 1.f));
    m_BoxRenderer->Draw(camera->m_ViewProjection);
}

void Demo::Update(GLFWwindow* window, const double time, const float deltaTime)
{
    window_size_system::Update(m_World, window);
    camera_input_system::Update(m_World, time, deltaTime);
    camera_system::Update(m_World, time, deltaTime);
    mouse_trail_system::Update(m_World, time, deltaTime);

    Render(time, deltaTime);
    DrawImGui();
}

void Demo::Init(GLFWwindow* window)
{
    {
        flecs::entity globalEntity = m_World.entity();
        globalEntity.add<demo::InputComponent>();
        globalEntity.add<demo::WindowSizeComponent>();
        globalEntity.add<demo::CameraInputComponent>();
        globalEntity.add<demo::CameraComponent>();
        globalEntity.add<demo::MouseTrailComponent>();
    }

    glfwSetWindowSizeCallback(window, WindowSizeCallback);

    m_Program = BuildShaderProgram("Demo", DATA_DIR "/demovertex.glsl", DATA_DIR "/demofragment.glsl");
    m_TextProgram = BuildShaderProgram("Text", DATA_DIR "/boxvertex.glsl", DATA_DIR "/textfragment.glsl");
    m_BoxProgram = BuildShaderProgram("Box", DATA_DIR "/boxvertex.glsl", DATA_DIR "/boxfragment.glsl");

    m_Font = std::make_unique<Font>();
    m_Font->Load(DATA_DIR "/sourcecodepro-medium.png", DATA_DIR "/sourcecodepro-medium.json");

    m_TextRenderer = std::make_unique<TextRenderer>(*m_Font, m_TextProgram);
    m_TextRenderer->AddString(m_Text, m_FontSize, m_Position.x, m_Position.y, m_Color);

    m_BoxRenderer = std::make_unique<BoxRenderer>(m_BoxProgram);

    glGenTextures(1, &m_Texture);
    glBindTexture(GL_TEXTURE_2D, m_Texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    SetTextureData(0.0);
    glGenerateMipmap(GL_TEXTURE_2D);

    const float size = 0.2f;
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
