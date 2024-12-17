#include "Runtime/UI/Public/Controls/UIButton.h"
#include "Runtime/Core/Public/UIRenderer.h"
#include "Runtime/Core/Public/UITheme.h"

namespace VGE {
namespace UI {

UIButton::UIButton(const std::string& text, ButtonStyle style)
    : UIWidget(text)
    , m_Text(text)
    , m_ButtonStyle(style)
    , m_Toggleable(false)
    , m_Toggled(false)
    , m_PressAnimation(0.0f)
    , m_HoverAnimation(0.0f)
{
}

void UIButton::Draw(UIRenderer& renderer) {
    if (!IsVisible()) return;
    
    OnDraw(renderer);
    
    for (const auto& child : GetChildren()) {
        child->Draw(renderer);
    }
}

void UIButton::Update(float deltaTime) {
    if (!IsEnabled()) return;
    
    OnUpdate(deltaTime);
    
    for (const auto& child : GetChildren()) {
        child->Update(deltaTime);
    }
}

void UIButton::OnDraw(UIRenderer& renderer) {
    if (!IsVisible() || !m_Theme) return;

    const auto& colors = m_Theme->GetColors();
    const auto& style = m_Theme->GetStyle();
    
    // Determine button color based on state
    Color bgColor;
    switch (m_ButtonStyle) {
        case ButtonStyle::Primary:
            bgColor = colors.primary;
            break;
        case ButtonStyle::Secondary:
            bgColor = colors.secondary;
            break;
        case ButtonStyle::Success:
            bgColor = colors.success;
            break;
        case ButtonStyle::Danger:
            bgColor = colors.error;
            break;
        case ButtonStyle::Warning:
            bgColor = colors.warning;
            break;
        case ButtonStyle::Info:
            bgColor = colors.info;
            break;
        default:
            bgColor = colors.surface;
            break;
    }

    // Apply state modifications
    if (!IsEnabled()) {
        bgColor.a *= 0.5f;
    }
    else if (m_State == UIWidgetState::Pressed) {
        bgColor = Color::Lerp(bgColor, Color::Black, m_PressAnimation);
    }
    else if (m_State == UIWidgetState::Hovered) {
        bgColor = Color::Lerp(bgColor, Color::White, m_HoverAnimation);
    }

    // Draw button background
    renderer.DrawRoundedRect(GetRect(), bgColor, style.cornerRadius);

    // Draw text if present
    if (!m_Text.empty()) {
        renderer.DrawTextCentered(m_Text, GetPosition(), GetSize(), colors.onSurface);
    }
}

void UIButton::OnUpdate(float deltaTime) {
    // Update animations
    const float ANIMATION_SPEED = 10.0f;
    
    if (m_State == UIWidgetState::Pressed) {
        m_PressAnimation = std::min(1.0f, m_PressAnimation + deltaTime * ANIMATION_SPEED);
    }
    else {
        m_PressAnimation = std::max(0.0f, m_PressAnimation - deltaTime * ANIMATION_SPEED);
    }
    
    if (m_State == UIWidgetState::Hovered) {
        m_HoverAnimation = std::min(1.0f, m_HoverAnimation + deltaTime * ANIMATION_SPEED);
    }
    else {
        m_HoverAnimation = std::max(0.0f, m_HoverAnimation - deltaTime * ANIMATION_SPEED);
    }
}

void UIButton::OnPositionChanged() {
    // Nothing to do
}

void UIButton::OnSizeChanged() {
    // Nothing to do
}

void UIButton::OnChildAdded(std::shared_ptr<UIElement> child) {
    // Nothing to do
}

void UIButton::OnChildRemoved(std::shared_ptr<UIElement> child) {
    // Nothing to do
}

void UIButton::OnParentPositionChanged() {
    // Nothing to do
}

bool UIButton::OnMouseMove(const glm::vec2& mousePos) {
    return UIWidget::OnMouseMove(mousePos);
}

bool UIButton::OnMouseDown(const glm::vec2& mousePos) {
    if (!IsEnabled() || !IsVisible()) return false;
    
    if (IsPointInside(mousePos)) {
        m_State = UIWidgetState::Pressed;
        OnPressed();
        return true;
    }
    
    return false;
}

bool UIButton::OnMouseUp(const glm::vec2& mousePos) {
    if (!IsEnabled() || !IsVisible()) return false;
    
    if (m_State == UIWidgetState::Pressed) {
        m_State = IsPointInside(mousePos) ? UIWidgetState::Hovered : UIWidgetState::Normal;
        OnReleased();
        
        if (IsPointInside(mousePos)) {
            if (m_Toggleable) {
                m_Toggled = !m_Toggled;
            }
            if (m_ClickCallback) {
                m_ClickCallback();
            }
        }
        
        return true;
    }
    
    return false;
}

void UIButton::OnMouseEnter() {
    // Base implementation is sufficient
}

void UIButton::OnMouseLeave() {
    // Base implementation is sufficient
}

void UIButton::OnPressed() {
    // Base implementation is sufficient
}

void UIButton::OnReleased() {
    // Base implementation is sufficient
}

}} // namespace VGE::UI



