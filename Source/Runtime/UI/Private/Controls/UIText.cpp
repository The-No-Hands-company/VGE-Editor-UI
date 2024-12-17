#include "Runtime/UI/Public/Controls/UIText.h"
#include "Runtime/Core/Public/UIRenderer.h"

namespace VGE {
namespace UI {

UIText::UIText(const std::string& text)
    : UIWidget(text)
    , m_Text(text)
    , m_Color(1.0f, 1.0f, 1.0f, 1.0f)
    , m_FontSize(14.0f)
    , m_HorizontalAlignment(TextAlignment::Left)
    , m_VerticalAlignment(TextAlignment::Top)
{
}

void UIText::Draw(UIRenderer& renderer) {
    if (!IsVisible()) return;
    
    OnDraw(renderer);
    
    for (const auto& child : GetChildren()) {
        child->Draw(renderer);
    }
}

void UIText::Update(float deltaTime) {
    if (!IsEnabled()) return;
    
    OnUpdate(deltaTime);
    
    for (const auto& child : GetChildren()) {
        child->Update(deltaTime);
    }
}

void UIText::OnDraw(UIRenderer& renderer) {
    if (!IsVisible()) return;

    renderer.DrawText(m_Text, GetPosition(), m_Color, m_HorizontalAlignment, m_VerticalAlignment, m_FontSize);
}

void UIText::OnUpdate(float deltaTime) {
    // Nothing to do
}

void UIText::OnPositionChanged() {
    // Nothing to do
}

void UIText::OnSizeChanged() {
    // Nothing to do
}

void UIText::OnChildAdded(std::shared_ptr<UIElement> child) {
    // Nothing to do
}

void UIText::OnChildRemoved(std::shared_ptr<UIElement> child) {
    // Nothing to do
}

void UIText::OnParentPositionChanged() {
    // Nothing to do
}

void UIText::SetText(const std::string& text) {
    m_Text = text;
    // TODO: Update size based on text measurement
}

}} // namespace VGE::UI 


