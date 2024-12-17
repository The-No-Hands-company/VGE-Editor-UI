#include "Runtime/Core/Public/Widget/UISlot.h"
#include "Runtime/Core/Public/Widget/UIWidget.h"

namespace VGE {
namespace UI {

UISlot::UISlot()
    : m_Widget(nullptr)
    , m_Padding(0.0f)
    , m_Fill(1.0f)
    , m_MaxSize(0.0f)
    , m_HAlign(HAlign::Fill)
    , m_VAlign(VAlign::Fill)
{
}

void UISlot::SetWidget(std::shared_ptr<UIWidget> widget)
{
    m_Widget = widget;
}

UISlot& UISlot::Padding(float left, float top, float right, float bottom)
{
    m_Padding = glm::vec4(left, top, right, bottom);
    return *this;
}

UISlot& UISlot::SetHAlign(HAlign align)
{
    m_HAlign = align;
    return *this;
}

UISlot& UISlot::SetVAlign(VAlign align)
{
    m_VAlign = align;
    return *this;
}

UISlot& UISlot::Fill(float horizontal, float vertical)
{
    m_Fill = glm::vec2(horizontal, vertical);
    return *this;
}

UISlot& UISlot::MaxSize(float width, float height)
{
    m_MaxSize = glm::vec2(width, height);
    return *this;
}

}} // namespace VGE::UI
