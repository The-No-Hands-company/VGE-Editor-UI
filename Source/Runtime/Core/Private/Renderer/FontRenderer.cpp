#include "Renderer/FontRenderer.h"
#include "Core/Logger.h"
#include "Core/UITypes.h"
#include <glad/gl.h>
#include <ft2build.h>
#include FT_FREETYPE_H

namespace VGE {
namespace UI {

namespace {
    const char* textVertexShader = R"(
        #version 330 core
        layout (location = 0) in vec4 vertex;
        out vec2 TexCoords;
        uniform mat4 projection;
        void main() {
            gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
            TexCoords = vertex.zw;
        }
    )";

    const char* textFragmentShader = R"(
        #version 330 core
        in vec2 TexCoords;
        out vec4 color;
        uniform sampler2D text;
        uniform vec4 textColor;
        void main() {
            vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
            color = textColor * sampled;
        }
    )";
}

FontRenderer::FontRenderer(UIRenderer* renderer)
    : m_Renderer(renderer)
    , m_VAO(0)
    , m_VBO(0)
    , m_FreeType(nullptr)
    , m_Face(nullptr)
{
}

FontRenderer::~FontRenderer() {
    Shutdown();
}

bool FontRenderer::Initialize() {
    // Initialize shader
    m_TextShader = std::make_shared<UIShader>();
    if (!m_TextShader->Initialize(textVertexShader, textFragmentShader)) {
        return false;
    }

    // Initialize vertex buffers
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return InitializeFreetype();
}

void FontRenderer::Shutdown() {
    ShutdownFreetype();

    if (m_VAO) {
        glDeleteVertexArrays(1, &m_VAO);
        m_VAO = 0;
    }
    if (m_VBO) {
        glDeleteBuffers(1, &m_VBO);
        m_VBO = 0;
    }

    m_TextShader.reset();
    m_Characters.clear();
}

bool FontRenderer::InitializeFreetype() {
    if (FT_Init_FreeType(&m_FreeType)) {
        return false;
    }
    return true;
}

void FontRenderer::ShutdownFreetype() {
    if (m_Face) {
        FT_Done_Face(m_Face);
        m_Face = nullptr;
    }
    if (m_FreeType) {
        FT_Done_FreeType(m_FreeType);
        m_FreeType = nullptr;
    }
}

bool FontRenderer::LoadFont(const std::string& path, uint32_t size) {
    if (m_Face) {
        FT_Done_Face(m_Face);
        m_Face = nullptr;
    }

    if (FT_New_Face(m_FreeType, path.c_str(), 0, &m_Face)) {
        return false;
    }

    FT_Set_Pixel_Sizes(m_Face, 0, size);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    m_Characters.clear();

    // Pre-load ASCII characters
    for (unsigned char c = 0; c < 128; c++) {
        if (!LoadCharacter(c)) {
            return false;
        }
    }

    return true;
}

bool FontRenderer::LoadCharacter(char c) {
    if (FT_Load_Char(m_Face, c, FT_LOAD_RENDER)) {
        return false;
    }

    TextureSpecification spec;
    spec.Width = m_Face->glyph->bitmap.width;
    spec.Height = m_Face->glyph->bitmap.rows;
    spec.Format = TextureFormat::RGBA;
    spec.MinFilter = TextureFilter::Linear;
    spec.MagFilter = TextureFilter::Linear;
    spec.WrapS = TextureWrap::ClampToEdge;
    spec.WrapT = TextureWrap::ClampToEdge;
    spec.GenerateMips = false;

    UITexture texture(spec);
    texture.SetData(m_Face->glyph->bitmap.buffer, m_Face->glyph->bitmap.width * m_Face->glyph->bitmap.rows);

    Character character = {
        texture,
        glm::ivec2(m_Face->glyph->bitmap.width, m_Face->glyph->bitmap.rows),
        glm::ivec2(m_Face->glyph->bitmap_left, m_Face->glyph->bitmap_top),
        static_cast<uint32_t>(m_Face->glyph->advance.x)
    };

    m_Characters.insert(std::pair<char, Character>(c, character));
    return true;
}

void FontRenderer::DrawText(const std::string& text, const glm::vec2& position, const glm::vec4& color) {
    m_TextShader->Bind();
    m_TextShader->SetVec4("textColor", color);
    m_TextShader->SetMat4("projection", m_Renderer->GetProjectionMatrix());

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(m_VAO);

    float x = position.x;
    float y = position.y;

    for (char c : text) {
        auto it = m_Characters.find(c);
        if (it == m_Characters.end()) {
            if (!LoadCharacter(c)) {
                continue;
            }
            it = m_Characters.find(c);
        }

        const Character& ch = it->second;

        float xpos = x + ch.bearing.x;
        float ypos = y - (ch.size.y - ch.bearing.y);

        float w = ch.size.x;
        float h = ch.size.y;

        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };

        ch.texture->Bind(0);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        x += (ch.advance >> 6);
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    m_TextShader->Unbind();
}

void FontRenderer::SetFont(const std::shared_ptr<UIFont>& font) {
    m_Font = font;
    if (m_Font) {
        LoadFont(m_Font->GetPath(), m_Font->GetSize());
    }
}

FontRenderer& FontRenderer::GetFontRenderer() {
    static FontRenderer instance(nullptr);
    return instance;
}

}} // namespace VGE::UI