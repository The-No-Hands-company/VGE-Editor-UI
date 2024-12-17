#include "Runtime/UI/Public/Controls/UIPanel.h"
#include "Runtime/Core/Public/UIRenderer.h"
#include "Runtime/Core/Public/UIEventManager.h"
#include "Runtime/Core/Public/Core/UITypes.h"

namespace VGE {
namespace UI {

UIPanel::UIPanel(const std::string& name)
    : UIWidget(name)
    , m_Dockable(true)
    , m_Collapsed(false)
    , m_IsDragging(false)
    , m_PanelPadding(5.0f, 5.0f)
    , m_DragOffset(0.0f, 0.0f)
{
    SetSize(glm::vec2(300.0f, 400.0f));
}

void UIPanel::OnDraw(UIRenderer& renderer) {
    if (!IsVisible()) return;

    // Draw panel background
    renderer.DrawRect(GetPosition(), GetSize(), m_Style.backgroundColor);

    // Draw border if enabled
    if (m_Style.showBorder) {
        renderer.DrawRectOutline(GetPosition(), GetSize(), m_Style.borderColor, m_Style.borderThickness);
    }

    // Draw title bar if enabled
    if (m_Style.showTitleBar) {
        RenderTitleBar(renderer);
    }

    // Draw content if not collapsed
    if (!m_Collapsed) {
        RenderContent(renderer);
    }

    // Draw resize handle if enabled
    if (m_Style.showResizeHandle && !m_Collapsed) {
        const glm::vec2 handlePos = GetPosition() + GetSize() - glm::vec2(m_Style.resizeHandleSize);
        renderer.DrawRect(handlePos, glm::vec2(m_Style.resizeHandleSize), m_Style.borderColor);
    }
}

void UIPanel::RenderTitleBar(UIRenderer& renderer) {
    const glm::vec2 titleBarSize(GetSize().x, m_Style.titleBarHeight);
    
    // Draw title bar background
    renderer.DrawRect(GetPosition(), titleBarSize, m_Style.titleBarColor);
    
    // Draw title text
    const glm::vec2 textPos = GetPosition() + glm::vec2(m_Style.padding);
    renderer.DrawText(GetName(), textPos, m_Style.titleBarTextColor);
}

void UIPanel::SetCollapsed(bool collapsed) {
    if (m_Collapsed == collapsed) return;
    m_Collapsed = collapsed;

    // Adjust size when collapsing/expanding
    if (m_Collapsed) {
        m_ExpandedSize = GetSize();
        SetSize(glm::vec2(GetSize().x, m_Style.showTitleBar ? m_Style.titleBarHeight : 0.0f));
    } else {
        SetSize(m_ExpandedSize);
    }
}

bool UIPanel::IsMouseOverRect(const glm::vec2& pos, const glm::vec2& size) const {
    const glm::vec2& mousePos = GetAbsolutePosition();
    return mousePos.x >= pos.x && mousePos.x <= (pos.x + size.x) &&
           mousePos.y >= pos.y && mousePos.y <= (pos.y + size.y);
}

void UIPanel::OnUpdate(float deltaTime) {
    // Handle any panel-specific updates here
}

void UIPanel::OnPositionChanged() {
    // Handle position changes if needed
}

void UIPanel::OnSizeChanged() {
    // Update expanded size tracking if needed
    if (!m_Collapsed) {
        m_ExpandedSize = GetSize();
    }
}

void UIPanel::OnChildAdded(std::shared_ptr<UIElement> child) {
    // Handle child addition if needed
}

void UIPanel::OnChildRemoved(std::shared_ptr<UIElement> child) {
    // Handle child removal if needed
}

void UIPanel::OnParentPositionChanged() {
    // Handle parent position changes if needed
    // This might affect absolute positioning of the panel
}

void UIPanel::RenderContent(UIRenderer& renderer) {
    // Render child elements
    for (const auto& child : GetChildren()) {
        child->Draw(renderer);
    }
}

} // namespace UI
} // namespace VGE



