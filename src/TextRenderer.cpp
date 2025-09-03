#include "TextRenderer.h"

#include "Font.h"
#include "GLFWLib.h"

namespace
{
    int s_PositionNumComponents = 2;

    constexpr GLuint s_AttributePosition = 0;
    constexpr GLuint s_AttributeTextureIndex = 1;

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
}

void TextRenderer::AddString(const std::string& text, float size, float x, float y)
{
    const float scale = size / static_cast<float>(m_Font.m_Size);
    for (const char c : text)
    {
        const float xAdvance = AddCharacter(c, size, x, y);
        x += scale * xAdvance;
    }
}

void TextRenderer::RemoveAllStrings()
{
    m_Positions.clear();
    m_TextureUV.clear();
    m_Indices.clear();
    m_BuffersDirty = true;
}

float TextRenderer::AddCharacter(
    char c,
    float size,
    float x,
    float y)
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
     * TODO: figure out where the text baseline is (doesn't seem to be 'base')
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

    m_Positions.clear();
    m_Positions.push_back(0.f);
    m_Positions.push_back(0.f);
    m_Positions.push_back(1.f);
    m_Positions.push_back(0.f);
    m_Positions.push_back(0.f);
    m_Positions.push_back(1.f);
    m_Positions.push_back(1.f);
    m_Positions.push_back(1.f);

    m_TextureUV.reserve(m_TextureUV.size() + 8);
    m_TextureUV.push_back(t0);
    m_TextureUV.push_back(s1);
    m_TextureUV.push_back(s0);
    m_TextureUV.push_back(t0);
    m_TextureUV.push_back(s0);
    m_TextureUV.push_back(t1);
    m_TextureUV.push_back(s1);
    m_TextureUV.push_back(t1);

    m_Indices.reserve(m_Indices.size() + 6);
    m_Indices.push_back(i+0);
    m_Indices.push_back(i+1);
    m_Indices.push_back(i+2);
    m_Indices.push_back(i+1);
    m_Indices.push_back(i+2);
    m_Indices.push_back(i+3);

    m_BuffersDirty = true;

    return shape.m_XAdvance;
}

void TextRenderer::Draw()
{
    if (m_Positions.empty())
    {
        return;
    }

    glUseProgram(m_Program);
    if (m_BuffersDirty)
    {
        m_PositionsBuffer = TryCreateAndBindBuffer(GL_ARRAY_BUFFER, m_Positions, m_PositionsBuffer);
        glVertexAttribPointer(s_AttributePosition, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

        m_TextureUVBuffer = TryCreateAndBindBuffer(GL_ARRAY_BUFFER, m_TextureUV, m_TextureUVBuffer);
        glVertexAttribPointer(s_AttributeTextureIndex, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

        m_IndicesBuffer = TryCreateAndBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Indices, m_IndicesBuffer);

        m_BuffersDirty = false;
    }

    glBindTexture(GL_TEXTURE_2D, m_Font.m_GLTexture);

    // GLint transformUniform = glGetUniformLocation(m_Program, "transform");
    // glm::vec2 transformVec(m_Offset.x, m_Offset.y);
    // glUniform2f(transformUniform, transformVec.x, transformVec.y);
    // const GLint fontSizeUniform = glGetUniformLocation(m_Program, "u_fontSize");
    // glUniform1f(fontSizeUniform, m_FontSize);
    // GLint colorUniform = glGetUniformLocation(m_Program, "u_color");
    // glUniform3f(colorUniform, m_Color.x, m_Color.y, m_Color.z);

    glEnableVertexAttribArray(s_AttributePosition);
    glEnableVertexAttribArray(s_AttributeTextureIndex);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndicesBuffer);
    glDrawElements(GL_TRIANGLES, 0, 6, nullptr);
}
