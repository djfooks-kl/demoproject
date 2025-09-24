#include <string>
#include <unordered_map>

typedef unsigned int GLuint;

namespace xc
{
    struct FontAtlasGlyphLayout
    {
        int m_Id = 0;
        int m_Index = 0;
        float m_X = 0.f;
        float m_Y = 0.f;
        float m_Width = 0.f;
        float m_Height = 0.f;
        float m_XOffset = 0.f;
        float m_YOffset = 0.f;
        float m_XAdvance = 0.f;
    };

    struct Font
    {
        ~Font();

        bool Load(const std::string& texturePath, const std::string& atlasPath);

        std::string m_TexturePath;
        std::string m_AtlasPath;

        GLuint m_GLTexture = 0;

        int m_Size = 0;
        int m_TextureWidth = 0;
        int m_TextureHeight = 0;

        float m_LineHeight = 0;
        float m_ScaleW = 0;
        float m_ScaleH = 0;

        std::unordered_map<char, FontAtlasGlyphLayout> m_GlyphLayout;

    private:
        bool LoadTexture();
        bool LoadAtlas();
    };
}