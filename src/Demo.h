#pragma once

#include <flecs/flecs.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <memory>
#include <string>

namespace xc
{
    struct Font;

    class ShaderProgram;
}

typedef unsigned int GLuint;

struct BoxRenderer;
struct GLFWwindow;
struct TextRenderer;

class Demo
{
public:
    Demo();
    ~Demo();

    void Init(GLFWwindow* window);
    void Update(GLFWwindow* window, double time, float deltaTime);

    void Render(double time, float deltaTime);
    void DrawImGui();

    void ProcessKeyInput(GLFWwindow* window, int key, int scancode, int action, int mods);
    void ProcessCursorInput(GLFWwindow* window, double xpos, double ypos);

private:
    std::unique_ptr<xc::Font> m_Font;
    std::unique_ptr<TextRenderer> m_TextRenderer;
    std::unique_ptr<BoxRenderer> m_BoxRenderer;
    flecs::world m_World;

    std::unique_ptr<xc::ShaderProgram> m_TextProgram;
    std::unique_ptr<xc::ShaderProgram> m_BoxProgram;

    std::unique_ptr<xc::ShaderProgram> m_DemoProgram;
    GLuint m_PositionsBuffer;
    GLuint m_TextureUBuffer;
    GLuint m_IndicesBuffer;
    GLuint m_DemoVBO;
    GLuint m_Texture;

    bool m_SettingsOpen = false;

    float m_FontSize = 0.2f;
    glm::vec2 m_Position = glm::vec2(-0.9f, -0.9f);
    glm::vec3 m_Color = glm::vec3(1.f, 1.f, 1.f);
    std::string m_Text = "Hello world";
};