#include "P

#include "Runtime/Core/Public/Core/UICore.h"ublic/UI/Editor/Panels/SceneHierarchyPanel.h"
#include "Public/UI/Framework/Core/UISystem.h"
#include "Public/Scene/Scene.h"
#include "Public/Scene/SceneNode.h"

namespace VGE {
namespace Editor {

SceneHierarchyPanel::SceneHierarchyPanel()
    : EditorPanel("Scene Hierarchy")
{
    SetSize(glm::vec2(300.0f, 600.0f));
}

void SceneHierarchyPanel::Initialize() {
    EditorPanel::Initialize();
}

void SceneHierarchyPanel::Update(float deltaTime) {
    EditorPanel::Update(deltaTime);
}

void SceneHierarchyPanel::SetScene(std::shared_ptr<Scene::Scene> scene) {
    m_Scene = scene;
    m_SelectedNode = nullptr;
}

void SceneHierarchyPanel::SetSelectedNode(std::shared_ptr<Scene::SceneNode> node) {
    if (m_SelectedNode == node) return;
    
    m_SelectedNode = node;
    // TODO: Emit selection changed event
}

void SceneHierarchyPanel::RenderPanelContent() {
    auto& renderer = UI::UISystem::Get().GetRenderer();
    
    // Render toolbar
    RenderToolbar();
    
    // Render scene hierarchy
    if (m_Scene) {
        auto rootNode = m_Scene->GetRootNode();
        if (rootNode) {
            RenderNode(rootNode);
        }
    } else {
        // Draw "No Scene Loaded" message
        glm::vec2 textPos = GetPosition() + glm::vec2(10.0f, 30.0f);
        // TODO: Add text rendering
        renderer.DrawRect(textPos, glm::vec2(100.0f, 20.0f), glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
    }
}

void SceneHierarchyPanel::RenderToolbar() {
    auto& renderer = UI::UISystem::Get().GetRenderer();
    
    // Draw toolbar background
    float toolbarHeight = 30.0f;
    glm::vec2 toolbarPos = GetPosition() + glm::vec2(0.0f, GetSize().y - toolbarHeight);
    glm::vec2 toolbarSize = glm::vec2(GetSize().x, toolbarHeight);
    
    renderer.DrawRect(toolbarPos, toolbarSize, glm::vec4(0.25f, 0.25f, 0.25f, 1.0f));
    
    // TODO: Add toolbar buttons and filter input
}

void SceneHierarchyPanel::RenderNode(std::shared_ptr<Scene::SceneNode> node, int depth) {
    if (!node) return;
    
    // Skip if doesn't match filter
    if (!m_Filter.empty() && !NodeMatchesFilter(node)) {
        return;
    }
    
    auto& renderer = UI::UISystem::Get().GetRenderer();
    
    // Calculate node position
    float nodeHeight = 20.0f;
    float indentWidth = 20.0f;
    glm::vec2 nodePos = GetPosition() + glm::vec2(depth * indentWidth, nodeHeight * depth);
    
    // Draw node background
    glm::vec4 bgColor = (node == m_SelectedNode) ?
        glm::vec4(0.2f, 0.4f, 0.8f, 0.5f) :
        glm::vec4(0.2f, 0.2f, 0.2f, 0.5f);
    
    renderer.DrawRect(nodePos, glm::vec2(GetSize().x - depth * indentWidth, nodeHeight), bgColor);
    
    // TODO: Add expand/collapse arrow
    // TODO: Add node icon
    // TODO: Add node name text
    
    // Draw children if expanded
    for (const auto& child : node->GetChildren()) {
        RenderNode(child, depth + 1);
    }
}

void SceneHierarchyPanel::RenderNodeContextMenu(std::shared_ptr<Scene::SceneNode> node) {
    if (!node) return;
    
    // TODO: Implement context menu
    // - Add Child
    // - Delete Node
    // - Duplicate Node
    // - Rename Node
    // - etc.
}

bool SceneHierarchyPanel::NodeMatchesFilter(std::shared_ptr<Scene::SceneNode> node) const {
    if (!node) return false;
    if (m_Filter.empty()) return true;
    
    // Check node name
    if (node->GetName().find(m_Filter) != std::string::npos) {
        return true;
    }
    
    // Check components if enabled
    if (m_ShowComponents) {
        // TODO: Check component names
    }
    
    return false;
}

} // namespace Editor
} // namespace VGE 


