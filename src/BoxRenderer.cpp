#include "BoxRenderer.h"

#include <glm/gtc/type_ptr.hpp>

#include "GLFWLib.h"

namespace
{
    int s_PositionNumComponents = 2;

    constexpr GLuint s_AttributePosition = 0;
    constexpr GLuint s_AttributeTextureUV = 1;
    constexpr GLuint s_AttributeColor = 2;

    template<typename TDATA>
    GLuint TryCreateAndBindBuffer(const GLenum target, const std::vector<TDATA>& data, GLuint buffer)
    {
        if (!buffer)
        {
            glGenBuffers(1, &buffer);
        }
        glBindBuffer(target, buffer);
        glBufferData(target, data.size() * sizeof(TDATA), &data.front(), GL_STATIC_DRAW);
        return buffer;
    }
}

BoxRenderer::BoxRenderer(GLuint program)
    : m_Program(program)
{
}

BoxRenderer::~BoxRenderer()
{
    glDeleteVertexArrays(1, &m_VBO);
    glDeleteBuffers(1, &m_PositionsBuffer);
    glDeleteBuffers(1, &m_TextureUVBuffer);
    glDeleteBuffers(1, &m_IndicesBuffer);
}

void BoxRenderer::AddBox(float size, const glm::vec2& center, const glm::vec3& color)
{
    const unsigned int i = static_cast<unsigned int>(m_Positions.size()) / s_PositionNumComponents;

    const float halfSize = size * 0.5f;
    const float x0 = center.x - halfSize;
    const float y0 = center.y - halfSize;
    const float x1 = center.x + halfSize;
    const float y1 = center.y + halfSize;

    const float s0 = 0.f;
    const float t0 = 0.f;
    const float s1 = 1.f;
    const float t1 = 1.f;

    m_Positions.reserve(m_Positions.size() + 8);
    m_Positions.push_back(x0);
    m_Positions.push_back(y0);
    m_Positions.push_back(x1);
    m_Positions.push_back(y0);
    m_Positions.push_back(x0);
    m_Positions.push_back(y1);
    m_Positions.push_back(x1);
    m_Positions.push_back(y1);

    m_TextureUV.reserve(m_TextureUV.size() + 8);
    m_TextureUV.push_back(s0);
    m_TextureUV.push_back(t0);
    m_TextureUV.push_back(s1);
    m_TextureUV.push_back(t0);
    m_TextureUV.push_back(s0);
    m_TextureUV.push_back(t1);
    m_TextureUV.push_back(s1);
    m_TextureUV.push_back(t1);

    m_Colors.reserve(m_Colors.size() + 12);
    m_Colors.push_back(color.r);
    m_Colors.push_back(color.g);
    m_Colors.push_back(color.b);
    m_Colors.push_back(color.r);
    m_Colors.push_back(color.g);
    m_Colors.push_back(color.b);
    m_Colors.push_back(color.r);
    m_Colors.push_back(color.g);
    m_Colors.push_back(color.b);
    m_Colors.push_back(color.r);
    m_Colors.push_back(color.g);
    m_Colors.push_back(color.b);

    //   2---3
    //   | \ |
    //   0---1
    // anti-clockwise winding
    m_Indices.reserve(m_Indices.size() + 6);
    m_Indices.push_back(i+0);
    m_Indices.push_back(i+1);
    m_Indices.push_back(i+2);
    m_Indices.push_back(i+1);
    m_Indices.push_back(i+2);
    m_Indices.push_back(i+3);

    m_BuffersDirty = true;
}

void BoxRenderer::RemoveAllBoxes()
{
    m_Positions.clear();
    m_TextureUV.clear();
    m_Colors.clear();
    m_Indices.clear();
    m_BuffersDirty = true;
}

void BoxRenderer::Draw(const glm::mat4& viewProjection)
{
    if (m_Positions.empty())
    {
        return;
    }

    if (m_BuffersDirty)
    {
        if (m_VBO == 0)
        {
            glGenVertexArrays(1, &m_VBO);
        }

        glBindVertexArray(m_VBO);
        m_PositionsBuffer = TryCreateAndBindBuffer(GL_ARRAY_BUFFER, m_Positions, m_PositionsBuffer);
        m_TextureUVBuffer = TryCreateAndBindBuffer(GL_ARRAY_BUFFER, m_TextureUV, m_TextureUVBuffer);
        m_ColorBuffer = TryCreateAndBindBuffer(GL_ARRAY_BUFFER, m_Colors, m_ColorBuffer);
        m_IndicesBuffer = TryCreateAndBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Indices, m_IndicesBuffer);

        glBindBuffer(GL_ARRAY_BUFFER, m_PositionsBuffer);
        glVertexAttribPointer(s_AttributePosition, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(s_AttributePosition);

        glBindBuffer(GL_ARRAY_BUFFER, m_TextureUVBuffer);
        glVertexAttribPointer(s_AttributeTextureUV, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(s_AttributeTextureUV);

        glBindBuffer(GL_ARRAY_BUFFER, m_ColorBuffer);
        glVertexAttribPointer(s_AttributeColor, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(s_AttributeColor);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndicesBuffer);

        m_BuffersDirty = false;
    }

    glUseProgram(m_Program);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glBindVertexArray(m_VBO);

    GLint viewProjectionUniform = glGetUniformLocation(m_Program, "viewProjection");
    glUniformMatrix4fv(viewProjectionUniform, 1, GL_FALSE, glm::value_ptr(viewProjection));

    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_Indices.size()), GL_UNSIGNED_INT, nullptr);
}
