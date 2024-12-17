#include "Layout/UIVerticalLayout.h"
#include "Core/UIRenderer.h"
#include <algorithm>

namespace VGE {
namespace UI {

UIVerticalLayout::UIVerticalLayout(const std::string& title)
    : UIWidget(title)
{
}

void UIVerticalLayout::AddElement(std::shared_ptr<UIWidget> element) {
    if (!element) return;
    
    AddChild(element);
    InvalidateLayout();
}

void UIVerticalLayout::RemoveElement(std::shared_ptr<UIWidget> element) {
    if (!element) return;
    
    RemoveChild(element);
    InvalidateLayout();
}

void UIVerticalLayout::Clear() {
    ClearChildren();
    InvalidateLayout();
}

void UIVerticalLayout::OnUpdate(float deltaTime) {
    UIWidget::OnUpdate(deltaTime);
}

void UIVerticalLayout::OnDraw(UIRenderer& renderer) {
    UIWidget::OnDraw(renderer);
}

bool UIVerticalLayout::OnMouseMove(const glm::vec2& position) {
    return UIWidget::OnMouseMove(position);
}

bool UIVerticalLayout::OnMouseDown(const glm::vec2& position) {
    return UIWidget::OnMouseDown(position);
}

bool UIVerticalLayout::OnMouseUp(const glm::vec2& position) {
    return UIWidget::OnMouseUp(position);
}

void UIVerticalLayout::OnLayoutUpdate() {
    float currentY = m_LayoutOptions.padding;
    float contentWidth = GetSize().x - (m_LayoutOptions.padding * 2.0f);
    
    for (auto& child : GetChildren()) {
        if (!child->IsVisible()) continue;
        
        // Calculate child position and size
        glm::vec2 childSize = child->GetDesiredSize();
        float childX = m_LayoutOptions.padding;
        
        switch (m_LayoutOptions.alignment) {
            case HorizontalAlignment::Center:
                childX = m_LayoutOptions.padding + (contentWidth - childSize.x) * 0.5f;
                break;
            case HorizontalAlignment::Right:
                childX = GetSize().x - m_LayoutOptions.padding - childSize.x;
                break;
            case HorizontalAlignment::Stretch:
                childSize.x = contentWidth;
                break;
            default: // Left alignment
                break;
        }
        
        child->SetSize(childSize);
        child->SetPosition(glm::vec2(childX, currentY));
        
        currentY += childSize.y + m_LayoutOptions.spacing;
    }
}

glm::vec2 UIVerticalLayout::CalculateDesiredSize() const {
    glm::vec2 size(m_LayoutOptions.padding * 2.0f);
    bool first = true;
    
    for (const auto& child : GetChildren()) {
        if (!child->IsVisible()) continue;
        
        glm::vec2 childSize = child->GetDesiredSize();
        size.x = std::max(size.x, childSize.x + m_LayoutOptions.padding * 2.0f);
        size.y += childSize.y + (first ? 0.0f : m_LayoutOptions.spacing);
        first = false;
    }
    
    if (!first) { // If we have any visible children
        size.y += m_LayoutOptions.padding * 2.0f;
    }
    
    return size;
}

}} // namespace VGE::UI 