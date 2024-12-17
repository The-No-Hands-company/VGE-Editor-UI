#include "Layout/UIHorizontalLayout.h"
#include "Core/UIRenderer.h"
#include <algorithm>

namespace VGE {
namespace UI {

UIHorizontalLayout::UIHorizontalLayout(const std::string& title)
    : UIWidget(title)
{
}

void UIHorizontalLayout::AddElement(std::shared_ptr<UIWidget> element) {
    if (!element) return;
    
    AddChild(element);
    InvalidateLayout();
}

void UIHorizontalLayout::RemoveElement(std::shared_ptr<UIWidget> element) {
    if (!element) return;
    
    RemoveChild(element);
    InvalidateLayout();
}

void UIHorizontalLayout::Clear() {
    ClearChildren();
    InvalidateLayout();
}

void UIHorizontalLayout::OnUpdate(float deltaTime) {
    UIWidget::OnUpdate(deltaTime);
}

void UIHorizontalLayout::OnDraw(UIRenderer& renderer) {
    UIWidget::OnDraw(renderer);
}

bool UIHorizontalLayout::OnMouseMove(const glm::vec2& position) {
    return UIWidget::OnMouseMove(position);
}

bool UIHorizontalLayout::OnMouseDown(const glm::vec2& position) {
    return UIWidget::OnMouseDown(position);
}

bool UIHorizontalLayout::OnMouseUp(const glm::vec2& position) {
    return UIWidget::OnMouseUp(position);
}

void UIHorizontalLayout::OnLayoutUpdate() {
    float currentX = m_Padding.x;
    float contentHeight = GetSize().y - (m_Padding.y + m_Padding.w);
    
    for (auto& child : GetChildren()) {
        if (!child->IsVisible()) continue;
        
        // Calculate child position and size
        glm::vec2 childSize = child->GetDesiredSize();
        childSize.y = std::max(childSize.y, contentHeight);
        
        child->SetSize(childSize);
        child->SetPosition(glm::vec2(currentX, m_Padding.y));
        
        currentX += childSize.x + m_Spacing;
    }
}

glm::vec2 UIHorizontalLayout::CalculateDesiredSize() const {
    glm::vec2 size(m_Padding.x + m_Padding.z, m_Padding.y + m_Padding.w);
    bool first = true;
    
    for (const auto& child : GetChildren()) {
        if (!child->IsVisible()) continue;
        
        glm::vec2 childSize = child->GetDesiredSize();
        size.x += childSize.x + (first ? 0.0f : m_Spacing);
        size.y = std::max(size.y, childSize.y + m_Padding.y + m_Padding.w);
        first = false;
    }
    
    size.x = std::max(size.x, m_MinWidth);
    size.y = std::max(size.y, m_MinHeight);
    
    return size;
}

}} // namespace VGE::UI 