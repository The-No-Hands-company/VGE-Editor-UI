#pragma once

#include "Runtime/Core/Public/Core/UICore.h"
#include "Runtime/Core/Public/Renderer/FontRenderer.h"
#include <glm/glm.hpp>
#include <memory>
#include <string>

namespace VGE {
namespace UI {

class UIRenderer;
class UIFont;

class UI_API UIFontRenderer {
public:
    UIFontRenderer(UIRenderer* renderer);
    ~UIFontRenderer();

    bool Initialize();
    void Shutdown();

    void SetFont(std::shared_ptr<UIFont> font);
    std::shared_ptr<UIFont> GetFont() const { return m_Font; }

    void DrawText(const std::string& text, const glm::vec2& position, const glm::vec4& color);
    glm::vec2 MeasureText(const std::string& text) const;

private:
    UIRenderer* m_Renderer;
    std::shared_ptr<UIFont> m_Font;
    float m_FontSize;
};

} // namespace UI
} // namespace VGE
