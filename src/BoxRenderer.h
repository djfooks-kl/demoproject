#pragma once

#include <glm/ext/matrix_float4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>

typedef unsigned int GLuint;

struct BoxRenderer
{
    BoxRenderer(GLuint program);
    ~BoxRenderer();

    void Draw(const glm::mat4& viewProjection);

    void AddBox(
        float size,
        const glm::vec2& center,
        const glm::vec3& color);
    void RemoveAllBoxes();

private:
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
