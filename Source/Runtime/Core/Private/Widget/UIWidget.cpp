#include "Runtime/Core/Public/Widget/UIWidget.h"

namespace VGE {
namespace UI {

UIWidget::UIWidget()
    : m_Position(0.0f)
    , m_Size(0.0f)
{
}

void UIWidget::OnLayoutUpdate()
{
}

glm::vec2 UIWidget::CalculateDesiredSize()
{
    return m_Size;
}

void UIWidget::ArrangeChildren()
{
    for (auto& child : m_Children)
    {
        child->OnLayoutUpdate();
    }
}

void UIWidget::OnPaint(UIRenderer* renderer)
{
}

void UIWidget::OnChildAdded(std::shared_ptr<UIWidget> child)
{
    m_Children.push_back(child);
    child->SetParent(shared_from_this());
}

void UIWidget::OnChildRemoved(std::shared_ptr<UIWidget> child)
{
    auto it = std::find(m_Children.begin(), m_Children.end(), child);
    if (it != m_Children.end())
    {
        (*it)->SetParent(nullptr);
        m_Children.erase(it);
    }
}

}} // namespace VGE::UI
