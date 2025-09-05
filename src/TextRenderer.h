#pragma once

#include <string>
#include <vector>
#include <glm/vec3.hpp>

typedef unsigned int GLuint;

struct Font;

struct TextRenderer
{
    TextRenderer(const Font& font, GLuint program);
    ~TextRenderer();

    void Draw();

    void AddString(
        const std::string& text,
        float size,
        float x,
        float y,
        const glm::vec3& color);
    void RemoveAllStrings();

private:
    float AddCharacter(
        char c,
        float size,
        float x,
        float y,
        const glm::vec3& color);

    const Font& m_Font;
    GLuint m_Program = 0;

    std::vector<float> m_Positions;
    std::vector<float> m_TextureUV;
    std::vector<float> m_Colors;
    std::vector<unsigned int> m_Indices;

    GLuint m_PositionsBuffer = 0;
    GLuint m_TextureUVBuffer = 0;
    GLuint m_ColorBuffer = 0;
    GLuint m_IndicesBuffer = 0;
    GLuint m_VBO = 0;
    bool m_BuffersDirty = false;
};
