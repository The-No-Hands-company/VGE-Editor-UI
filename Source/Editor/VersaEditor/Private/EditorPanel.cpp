#include "P

#include "Runtime/Core/Public/Core/UICore.h"ublic/UI/Editor/Panels/EditorPanel.h"
#include "Public/UI/Framework/Core/UISystem.h"

namespace VGE {
namespace Editor {

EditorPanel::EditorPanel(const std::string& name)
    : UIElement(name)
{
}

void EditorPanel::Initialize() {
    UIElement::Initialize();
    
    // Register with UI system
    UI::UISystem::Get().RegisterElement(std::shared_ptr<EditorPanel>(this));
}

void EditorPanel::Update(float deltaTime) {
    if (!m_IsOpen || !IsEnabled()) return;
    
    UIElement::Update(deltaTime);
}

void EditorPanel::Render() {
    if (!m_IsOpen || !IsVisible()) return;
    
    // Begin panel rendering
    auto& renderer = UI::UISystem::Get().GetRenderer();
    
    // Draw panel background
    renderer.DrawRect(GetPosition(), GetSize(), glm::vec4(0.2f, 0.2f, 0.2f, 0.95f));
    
    // Draw panel content
    RenderPanelContent();
    
    // Draw children
    UIElement::Render();
}

void EditorPanel::UpdateLayout() {
    if (!m_IsOpen) return;
    
    UIElement::UpdateLayout();
}

} // namespace Editor
} // namespace VGE 


