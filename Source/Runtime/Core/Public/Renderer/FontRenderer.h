#pragma once

#include "Runtime/Core/Public/Core/UICore.h"
#include "Runtime/Core/Public/Core/UITypes.h"
#include "Runtime/Core/Public/Renderer/UIShader.h"
#include "Runtime/Core/Public/Renderer/UITexture.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include <string>
#include <unordered_map>
#include <memory>

namespace VGE {
namespace UI {

class UIRenderer;
class UIFont;

class UI_API FontRenderer {
public:
    explicit FontRenderer(UIRenderer* renderer = nullptr);
    ~FontRenderer();

    bool Initialize();
    void Shutdown();

    bool LoadFont(const std::string& filename, uint32_t size);
    void DrawText(const std::string& text, const glm::vec2& position, const glm::vec4& color);
    void SetFont(const std::shared_ptr<UIFont>& font);

    static FontRenderer& GetFontRenderer();

private:
    bool InitializeFreetype();
    void ShutdownFreetype();
    bool LoadCharacter(char c);

private:
    UIRenderer* m_Renderer;
    std::shared_ptr<UIShader> m_TextShader;
    std::shared_ptr<UIFont> m_Font;
    
    uint32_t m_VAO;
    uint32_t m_VBO;
    
    FT_Library m_FreeType;
    FT_Face m_Face;
    std::unordered_map<char, Character> m_Characters;
};

}} // namespace VGE::UI


