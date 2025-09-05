#include "TextRenderer.h"

#include "Font.h"
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

TextRenderer::TextRenderer(const Font& font, GLuint program)
    : m_Font(font)
    , m_Program(program)
{
}

TextRenderer::~TextRenderer()
{
    if (m_PositionsBuffer)
    {
        glDeleteTextures(1, &m_PositionsBuffer);
    }
    if (m_TextureUVBuffer)
    {
        glDeleteTextures(1, &m_TextureUVBuffer);
    }
    if (m_IndicesBuffer)
    {
        glDeleteTextures(1, &m_IndicesBuffer);
    }
    // TODO VBO
}

void TextRenderer::AddString(const std::string& text, float size, float x, float y, const glm::vec3& color)
{
    for (const char c : text)
    {
        const float xAdvance = AddCharacter(c, size, x, y, color);
        x += xAdvance;
    }
}

void TextRenderer::RemoveAllStrings()
{
    m_Positions.clear();
    m_TextureUV.clear();
    m_Colors.clear();
    m_Indices.clear();
    m_BuffersDirty = true;
}

float TextRenderer::AddCharacter(
    char c,
    float size,
    float x,
    float y,
    const glm::vec3& color)
{
    // https://www.redblobgames.com/x/2403-distance-field-fonts/
    const auto shapeItr = m_Font.m_GlyphLayout.find(c);
    if (shapeItr == m_Font.m_GlyphLayout.end())
    {
        return 0.f;
    }
    const auto& shape = shapeItr->second;

    const float s0 = shape.m_X / m_Font.m_ScaleW;
    const float s1 = (shape.m_X + shape.m_Width) / m_Font.m_ScaleW;
    const float t1 = shape.m_Y / m_Font.m_ScaleH;
    const float t0 = (shape.m_Y + shape.m_Height) / m_Font.m_ScaleH;

    /* Placement of a character within the box:
     *
     *     +---------+  -- top of box @ lineHeight
     *     |         |
     *     +---------+  -- top of glyph @ lineHeight - yoffset
     *     |         |
     *     |         |
     *     |         |
     *     |         |
     *     |         |
     *     +---------+  -- bottom of glyph @ lineHeight - yoffset - height
     *     |         |
     *     +---------+  -- bottom of box @ 0
     *
     *
     */
    const float scale = size / m_Font.m_Size;
    const float x0 = x + scale * shape.m_XOffset; // left
    const float x1 = x + scale * (shape.m_XOffset + shape.m_Width); // right
    const float y0 = y + scale * (m_Font.m_LineHeight - shape.m_YOffset - shape.m_Height); // bottom
    const float y1 = y + scale * (m_Font.m_LineHeight - shape.m_YOffset); // top


    const unsigned int i = static_cast<unsigned int>(m_Positions.size()) / s_PositionNumComponents;

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

    return shape.m_XAdvance * scale;
}

void TextRenderer::Draw()
{
    if (m_Positions.empty())
    {
        return;
    }

    if (m_BuffersDirty)
    {
        m_PositionsBuffer = TryCreateAndBindBuffer(GL_ARRAY_BUFFER, m_Positions, m_PositionsBuffer);
        m_TextureUVBuffer = TryCreateAndBindBuffer(GL_ARRAY_BUFFER, m_TextureUV, m_TextureUVBuffer);
        m_ColorBuffer = TryCreateAndBindBuffer(GL_ARRAY_BUFFER, m_Colors, m_ColorBuffer);
        m_IndicesBuffer = TryCreateAndBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Indices, m_IndicesBuffer);

        if (m_VBO == 0)
        {
            glGenVertexArrays(1, &m_VBO);
            glBindVertexArray(m_VBO);

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

            glBindVertexArray(0);
        }

        m_BuffersDirty = false;
    }

    glUseProgram(m_Program);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glBindTexture(GL_TEXTURE_2D, m_Font.m_GLTexture);
    glBindVertexArray(m_VBO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_Indices.size()), GL_UNSIGNED_INT, nullptr);
}
