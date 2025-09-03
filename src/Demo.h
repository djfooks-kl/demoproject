#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <string>
#include <memory>

typedef unsigned int GLuint;
struct Font;
struct TextRenderer;

class Demo
{
public:
    Demo();
    ~Demo();

    void Init();
    void Update(double time, float deltaTime);

private:
    std::unique_ptr<Font> m_Font;
    std::unique_ptr<TextRenderer> m_TextRenderer;

    GLuint m_Program;
    GLuint m_VBO;
    GLuint m_Texture;

    bool m_SettingsOpen = false;

    float m_FontSize = 0.5f;
    glm::vec2 m_Offset = glm::vec2(0.f, 0.f);
    glm::vec3 m_Color = glm::vec3(1.f, 1.f, 1.f);
    std::string m_Text = "Hello world";
};