#pragma once

#include "Runtime/Core/Public/Core/UICore.h"
#include "UIComboBoxBase.h"
#include "Runtime/Core/Public/Base/VGE.h"

namespace VGE {

struct ComboBoxGradient {
    glm::vec4 StartColor;
    glm::vec4 EndColor;
    float Angle = 0.0f;
};

struct ComboBoxShadow {
    glm::vec4 Color = glm::vec4(0.0f, 0.0f, 0.0f, 0.5f);
    glm::vec2 Offset = glm::vec2(0.0f, 2.0f);
    float Blur = 4.0f;
    float Spread = 0.0f;
};

struct ComboBoxBorder {
    glm::vec4 Color = glm::vec4(0.3f, 0.3f, 0.3f, 1.0f);
    float Width = 1.0f;
    float Radius = 4.0f;
};

struct ComboBoxVisualStyle {
    ComboBoxGradient NormalGradient;
    ComboBoxGradient HoverGradient;
    ComboBoxGradient SelectedGradient;
    ComboBoxBorder Border;
    ComboBoxBorder FocusBorder;
    ComboBoxShadow DropShadow;
    ComboBoxShadow HoverShadow;
    glm::vec4 TextColor = glm::vec4(1.0f);
    glm::vec4 DisabledTextColor = glm::vec4(0.5f);
};

class UIComboBoxRenderer {
public:
    UIComboBoxRenderer(UIComboBoxBase* owner);
    
    void SetStyle(const ComboBoxVisualStyle& style) { m_Style = style; }
    const ComboBoxVisualStyle& GetStyle() const { return m_Style; }
    
    void RenderBackground(const glm::vec2& position, const glm::vec2& size, float hoverAmount);
    void RenderBorder(const glm::vec2& position, const glm::vec2& size, bool focused);
    void RenderDropShadow(const glm::vec2& position, const glm::vec2& size, float hoverAmount);
    void RenderItem(const ComboBoxItem& item, const glm::vec2& position, bool selected, bool hovered, float hoverAmount);
    void RenderDropdownArrow(const glm::vec2& position, float size, float rotation);

private:
    void RenderGradient(const glm::vec2& position, const glm::vec2& size, const ComboBoxGradient& gradient);
    
    UIComboBoxBase* m_Owner;
    ComboBoxVisualStyle m_Style;
};

} // namespace VGE



