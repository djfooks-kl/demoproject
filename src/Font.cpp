#include "Font.h"

#include "GLFWLib.h"

#include <fstream>
#include <iostream>
#include <nlohmann-json/json.hpp>
#include <stb/stb_image.h>

using json = nlohmann::json;

Font::~Font()
{
    glDeleteTextures(1, &m_GLTexture);
}

bool Font::Load(const std::string& texturePath, const std::string& atlasPath)
{
    m_TexturePath = texturePath;
    m_AtlasPath = atlasPath;
    return LoadTexture() && LoadAtlas();
}

bool Font::LoadTexture()
{
    glGenTextures(1, &m_GLTexture);
    glBindTexture(GL_TEXTURE_2D, m_GLTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    int channels;
    unsigned char *data = stbi_load(m_TexturePath.c_str(), &m_TextureWidth, &m_TextureHeight, &channels, 0);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_TextureWidth, m_TextureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
    return true;
}

bool Font::LoadAtlas()
{
    std::ifstream fileStream(m_AtlasPath);
    if (fileStream.fail())
    {
        printf("Error could not read file \"%s\"\n", m_AtlasPath.c_str());
        return false;
    }
    json data = json::parse(fileStream);

    m_LineHeight = data["common"]["lineHeight"].get<float>();
    m_ScaleW = data["common"]["scaleW"].get<float>();
    m_ScaleH = data["common"]["scaleH"].get<float>();

    m_Size = data["info"]["size"].get<int>();

    for (auto& charJson : data["chars"])
    {
        char c;
        if (charJson["char"].is_number())
        {
            c = '0' + charJson["char"].get<char>();
        }
        else
        {
            const std::string cStr = charJson["char"].get<std::string>();
            c = cStr[0];
        }

        FontAtlasGlyphLayout glyph;

        glyph.m_Id = charJson["id"].get<int>();
        glyph.m_Index = charJson["index"].get<int>();

        glyph.m_X = charJson["x"].get<float>();
        glyph.m_Y = charJson["y"].get<float>();
        glyph.m_Width = charJson["width"].get<float>();
        glyph.m_Height = charJson["height"].get<float>();
        glyph.m_XOffset = charJson["xoffset"].get<float>();
        glyph.m_YOffset = charJson["yoffset"].get<float>();
        glyph.m_XAdvance = charJson["xadvance"].get<float>();

        m_GlyphLayout[c] = std::move(glyph);
    }

    return true;
}
