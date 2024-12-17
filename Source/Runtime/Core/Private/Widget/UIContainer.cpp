#include "Runtime/Core/Public/Widget/UIContainer.h"
#include "Runtime/Core/Public/Renderer/UIRenderer.h"

namespace VGE {
namespace UI {

UIContainer::UIContainer()
    : m_Padding(0.0f)
{
}

void UIContainer::OnLayoutUpdate()
{
    CalculateDesiredSize();
    ArrangeChildren();
}

glm::vec2 UIContainer::CalculateDesiredSize()
{
    glm::vec2 desiredSize(0.0f);
    for (const auto& slot : m_Slots)
    {
        auto widget = slot->GetWidget();
        if (widget)
        {
            auto childSize = widget->CalculateDesiredSize();
            desiredSize.x = std::max(desiredSize.x, childSize.x);
            desiredSize.y = std::max(desiredSize.y, childSize.y);
        }
    }

    // Add padding
    desiredSize.x += m_Padding.x + m_Padding.z;
    desiredSize.y += m_Padding.y + m_Padding.w;

    return desiredSize;
}

void UIContainer::ArrangeChildren()
{
    for (const auto& slot : m_Slots)
    {
        auto widget = slot->GetWidget();
        if (widget)
        {
            widget->OnLayoutUpdate();
        }
    }
}

void UIContainer::OnPaint(UIRenderer* renderer)
{
    if (!renderer) return;

    for (const auto& slot : m_Slots)
    {
        auto widget = slot->GetWidget();
        if (widget)
        {
            widget->OnPaint(renderer);
        }
    }
}

void UIContainer::OnChildAdded(std::shared_ptr<UIWidget> child)
{
    UIWidget::OnChildAdded(child);
}

void UIContainer::OnChildRemoved(std::shared_ptr<UIWidget> child)
{
    UIWidget::OnChildRemoved(child);
}

void UIContainer::AddChild(std::shared_ptr<UIWidget> child)
{
    if (!child) return;

    auto slot = std::make_shared<UISlot>();
    slot->SetWidget(child);
    m_Slots.push_back(slot);
    OnChildAdded(child);
}

void UIContainer::RemoveChild(std::shared_ptr<UIWidget> child)
{
    if (!child) return;

    auto it = std::find_if(m_Slots.begin(), m_Slots.end(),
        [&child](const std::shared_ptr<UISlot>& slot) {
            return slot->GetWidget() == child;
        });

    if (it != m_Slots.end())
    {
        OnChildRemoved(child);
        m_Slots.erase(it);
    }
}

void UIContainer::ClearChildren()
{
    for (const auto& slot : m_Slots)
    {
        auto widget = slot->GetWidget();
        if (widget)
        {
            OnChildRemoved(widget);
        }
    }
    m_Slots.clear();
}

}} // namespace VGE::UI
