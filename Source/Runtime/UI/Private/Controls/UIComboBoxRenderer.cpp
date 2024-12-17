#include "Runtime/Core/Public/Core/UICore.h"
#include "Runtime/UI/Public/Controls/UIComboBoxRenderer.h"
#include "Runtime/Core/Public/Graphics/RenderCommand.h"
#include <glm/gtx/transform.hpp>

namespace VGE {
namespace UI {

UIComboBoxRenderer::UIComboBoxRenderer(UIComboBoxBase* owner)
    : m_Owner(owner)
{
    InitializeDefaultStyle();
}

void UIComboBoxRenderer::InitializeDefaultStyle() {
    // Initialize default visual style
    m_Style.NormalGradient = {
        glm::vec4(0.2f, 0.2f, 0.2f, 1.0f),
        glm::vec4(0.25f, 0.25f, 0.25f, 1.0f),
        90.0f
    };
    
    m_Style.HoverGradient = {
        glm::vec4(0.25f, 0.25f, 0.25f, 1.0f),
        glm::vec4(0.3f, 0.3f, 0.3f, 1.0f),
        90.0f
    };
    
    m_Style.SelectedGradient = {
        glm::vec4(0.3f, 0.5f, 0.7f, 1.0f),
        glm::vec4(0.4f, 0.6f, 0.8f, 1.0f),
        90.0f
    };
    
    m_Style.TextColor = glm::vec4(1.0f);
    m_Style.DisabledTextColor = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
    m_Style.TagBackground = glm::vec4(0.3f, 0.3f, 0.3f, 1.0f);
    m_Style.TagText = glm::vec4(1.0f);
}

void UIComboBoxRenderer::RenderText(const std::string& text, const glm::vec2& position, const glm::vec4& color) {
    RenderCommand::DrawText(text, position, color);
}

float UIComboBoxRenderer::GetTextWidth(const std::string& text) {
    return RenderCommand::GetTextWidth(text);
}

void UIComboBoxRenderer::RenderTag(const std::string& text, const glm::vec2& position, bool isCount) {
    const float tagHeight = m_Owner->GetSize().y - 10.0f;
    const float tagWidth = GetTextWidth(text) + 20.0f;
    const float rounding = 3.0f;
    
    // Background
    glm::vec4 bgColor = isCount ? 
        glm::vec4(0.4f, 0.4f, 0.4f, 1.0f) : m_Style.TagBackground;
    
    RenderCommand::DrawRoundedRect(
        position + glm::vec2(0.0f, 5.0f),
        glm::vec2(tagWidth, tagHeight),
        bgColor,
        rounding
    );
    
    // Text
    RenderCommand::DrawText(
        text,
        position + glm::vec2(10.0f, 7.0f),
        m_Style.TagText
    );
}

void UIComboBoxRenderer::RenderBackground(const glm::vec2& position, const glm::vec2& size, float hoverAmount) {
    // Interpolate between normal and hover gradients
    ComboBoxGradient gradient;
    gradient.StartColor = glm::mix(m_Style.NormalGradient.StartColor, m_Style.HoverGradient.StartColor, hoverAmount);
    gradient.EndColor = glm::mix(m_Style.NormalGradient.EndColor, m_Style.HoverGradient.EndColor, hoverAmount);
    gradient.Angle = m_Style.NormalGradient.Angle;
    
    RenderGradient(position, size, gradient);
}

void UIComboBoxRenderer::RenderBorder(const glm::vec2& position, const glm::vec2& size, bool focused) {
    const auto& border = focused ? m_Style.FocusBorder : m_Style.Border;
    
    RenderCommand::DrawRoundedRectOutline(
        position,
        size,
        border.Color,
        border.Width,
        border.Radius
    );
}

void UIComboBoxRenderer::RenderDropShadow(const glm::vec2& position, const glm::vec2& size, float hoverAmount) {
    // Interpolate between normal and hover shadows
    ComboBoxShadow shadow;
    shadow.Color = glm::mix(m_Style.DropShadow.Color, m_Style.HoverShadow.Color, hoverAmount);
    shadow.Offset = glm::mix(m_Style.DropShadow.Offset, m_Style.HoverShadow.Offset, hoverAmount);
    shadow.Blur = glm::mix(m_Style.DropShadow.Blur, m_Style.HoverShadow.Blur, hoverAmount);
    shadow.Spread = glm::mix(m_Style.DropShadow.Spread, m_Style.HoverShadow.Spread, hoverAmount);
    
    RenderCommand::DrawShadow(
        position + shadow.Offset,
        size,
        shadow.Color,
        shadow.Blur,
        shadow.Spread
    );
}

void UIComboBoxRenderer::RenderItem(const ComboBoxItem& item, const glm::vec2& position, bool selected, bool hovered, float hoverAmount) {
    // Calculate background gradient
    ComboBoxGradient gradient = m_Style.NormalGradient;
    
    if (selected) {
        gradient = m_Style.SelectedGradient;
    } else if (hoverAmount > 0.01f) {
        gradient.StartColor = glm::mix(m_Style.NormalGradient.StartColor, m_Style.HoverGradient.StartColor, hoverAmount);
        gradient.EndColor = glm::mix(m_Style.NormalGradient.EndColor, m_Style.HoverGradient.EndColor, hoverAmount);
    }
    
    // Render background
    RenderGradient(position, glm::vec2(m_Owner->GetSize().x, 24.0f), gradient);
    
    // Render text
    glm::vec4 textColor = item.IsEnabled ? m_Style.TextColor : m_Style.DisabledTextColor;
    RenderCommand::DrawText(
        item.Text,
        position + glm::vec2(5.0f, 4.0f),
        textColor
    );
    
    // Render description if present
    if (!item.Description.empty()) {
        float descriptionOffset = RenderCommand::GetTextWidth(item.Text) + 10.0f;
        glm::vec4 descColor = glm::vec4(textColor.r, textColor.g, textColor.b, textColor.a * 0.7f);
        RenderCommand::DrawText(
            item.Description,
            position + glm::vec2(5.0f + descriptionOffset, 4.0f),
            descColor,
            0.8f
        );
    }
}

void UIComboBoxRenderer::RenderDropdownArrow(const glm::vec2& position, float size, float rotation) {
    // Create rotation matrix
    glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(position, 0.0f));
    transform = glm::rotate(transform, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
    
    // Draw triangle
    RenderCommand::DrawTriangle(
        glm::vec2(0.0f),
        size,
        m_Style.TextColor,
        transform
    );
}

void UIComboBoxRenderer::RenderGradient(const glm::vec2& position, const glm::vec2& size, const ComboBoxGradient& gradient) {
    // Convert angle to radians and calculate direction
    float angleRad = glm::radians(gradient.Angle);
    glm::vec2 direction(std::cos(angleRad), std::sin(angleRad));
    
    // Calculate gradient vector based on size and direction
    float length = std::abs(size.x * direction.x) + std::abs(size.y * direction.y);
    glm::vec2 gradientVector = direction * length;
    
    RenderCommand::DrawGradientRect(
        position,
        size,
        gradient.StartColor,
        gradient.EndColor,
        gradientVector
    );
}

} // namespace VGE::UI



