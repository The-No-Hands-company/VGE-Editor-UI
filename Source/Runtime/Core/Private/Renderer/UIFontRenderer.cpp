#include "Renderer/UIFontRenderer.h"
#include "Core/Logger.h"
#include "Renderer/FontRenderer.h"
#include <glad/gl.h>

namespace VGE {
namespace UI {

UIFontRenderer::UIFontRenderer(UIRenderer* renderer)
    : m_Renderer(renderer)
    , m_Font(nullptr)
    , m_FontSize(16.0f)
{
}

UIFontRenderer::~UIFontRenderer() {
    Shutdown();
}

bool UIFontRenderer::Initialize() {
    return FontRenderer::Get().Initialize();
}

void UIFontRenderer::Shutdown() {
    m_Font.reset();
}

void UIFontRenderer::SetFont(std::shared_ptr<UIFont> font) {
    m_Font = font;
    if (m_Font) {
        m_FontSize = m_Font->GetSize();
    }
}

void UIFontRenderer::DrawText(const std::string& text, const glm::vec2& position, const glm::vec4& color) {
    if (!m_Font) return;
    FontRenderer::Get().DrawText(text, position, color, m_FontSize);
}

glm::vec2 UIFontRenderer::MeasureText(const std::string& text) const {
    if (!m_Font) return glm::vec2(0.0f);
    return FontRenderer::Get().GetTextDimensions(text, m_FontSize);
}

} // namespace UI
} // namespace VGE
