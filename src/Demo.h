#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <string>

typedef unsigned int GLuint;

class Demo
{
public:
    void Init();
    void Update(double time, float deltaTime);

private:
    GLuint m_Program;
    GLuint m_VBO;
    GLuint m_Texture;

    bool m_SettingsOpen = false;

    float m_Pxrange = 0.5f;
    float m_FontSize = 0.5f;
    float m_Weight = 0.5f;
    glm::vec2 m_Offset = glm::vec2(0.f, 0.f);
    glm::vec3 m_Color = glm::vec3(1.f, 1.f, 1.f);
    std::string m_Text = "Hello world";
};