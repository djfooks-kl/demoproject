#pragma once

#include <glm/ext/matrix_float4x4.hpp>
#include <glm/vec3.hpp>
#include <string>
#include <vector>

typedef unsigned int GLuint;

namespace xc
{
    struct Font;

    class ShaderProgram;
}

struct TextRenderer
{
    TextRenderer(const xc::Font& font, const xc::ShaderProgram& program);
    ~TextRenderer();

    void Draw(const glm::mat4& viewProjection);

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

    const xc::Font& m_Font;
    const xc::ShaderProgram& m_Program;

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
