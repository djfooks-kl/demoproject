#pragma once

#include <flecs/flecs.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <memory>
#include <string>

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
    flecs::world m_World;

    GLuint m_TextProgram;

    GLuint m_Program;
    GLuint m_PositionsBuffer;
    GLuint m_TextureUBuffer;
    GLuint m_IndicesBuffer;
    GLuint m_DemoVBO;
    GLuint m_Texture;

    bool m_SettingsOpen = false;

    float m_FontSize = 0.5f;
    glm::vec2 m_Offset = glm::vec2(0.f, 0.f);
    glm::vec3 m_Color = glm::vec3(1.f, 1.f, 1.f);
    std::string m_Text = "Hello world";
};