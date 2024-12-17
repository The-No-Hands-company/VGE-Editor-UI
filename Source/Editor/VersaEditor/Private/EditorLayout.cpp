#include "E

#include "Runtime/Core/Public/Core/UICore.h"ditorLayout.h"
#include <fstream>
#include <queue>
#include <nlohmann/json.hpp>

namespace VGE {
namespace Editor {

void EditorLayout::SaveToFile(const std::string& filepath) {
    nlohmann::json j;
    
    if (m_Root)
        SerializeNode(m_Root.get(), j);
        
    std::ofstream file(filepath);
    file << j.dump(4);
}

void EditorLayout::LoadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open())
        return;
        
    nlohmann::json j;
    file >> j;
    
    m_Root = DeserializeNode(j);
    UpdateParentPointers(m_Root.get());
}

void EditorLayout::Reset() {
    m_Root = CreateNode("Root", { 1280.0f, 720.0f });
    
    // Create default layout
    auto* viewportNode = Split(m_Root.get(), DockDirection::Right, 0.75f);
    viewportNode->Name = "Viewport";
    
    auto* rightPane = Split(viewportNode, DockDirection::Right, 0.8f);
    rightPane->Name = "Properties";
    
    auto* leftPane = Split(m_Root.get(), DockDirection::Down, 0.7f);
    leftPane->Name = "Scene Hierarchy";
    
    auto* bottomPane = Split(leftPane, DockDirection::Tab);
    bottomPane->Name = "Console";
    
    auto* assetPane = CreateNode("Asset Browser");
    bottomPane->Children.push_back(std::shared_ptr<DockNode>(assetPane));
}

DockNode* EditorLayout::Split(DockNode* node, DockDirection direction, float ratio) {
    if (!node)
        return nullptr;
        
    auto newNode = std::make_shared<DockNode>();
    newNode->Parent = node;
    newNode->SplitDirection = direction;
    newNode->SplitRatio = ratio;
    
    node->Children.push_back(newNode);
    
    return newNode.get();
}

DockNode* EditorLayout::CreateNode(const std::string& name, const glm::vec2& size) {
    auto node = std::make_shared<DockNode>();
    node->Name = name;
    node->Size = size;
    
    if (!m_Root)
        m_Root = node;
        
    return node.get();
}

void EditorLayout::RemoveNode(DockNode* node) {
    if (!node || !node->Parent)
        return;
        
    auto& siblings = node->Parent->Children;
    siblings.erase(
        std::remove_if(siblings.begin(), siblings.end(),
            [node](const std::shared_ptr<DockNode>& child) {
                return child.get() == node;
            }),
        siblings.end()
    );
}

DockNode* EditorLayout::FindNode(const std::string& name) {
    if (!m_Root)
        return nullptr;
        
    std::queue<DockNode*> queue;
    queue.push(m_Root.get());
    
    while (!queue.empty()) {
        auto* current = queue.front();
        queue.pop();
        
        if (current->Name == name)
            return current;
            
        for (auto& child : current->Children)
            queue.push(child.get());
    }
    
    return nullptr;
}

std::vector<DockNode*> EditorLayout::GetAllNodes() {
    std::vector<DockNode*> result;
    
    if (!m_Root)
        return result;
        
    std::queue<DockNode*> queue;
    queue.push(m_Root.get());
    
    while (!queue.empty()) {
        auto* current = queue.front();
        queue.pop();
        
        result.push_back(current);
        
        for (auto& child : current->Children)
            queue.push(child.get());
    }
    
    return result;
}

void EditorLayout::SerializeNode(const DockNode* node, nlohmann::json& j) {
    if (!node)
        return;
        
    j["name"] = node->Name;
    j["size"] = { node->Size.x, node->Size.y };
    j["position"] = { node->Position.x, node->Position.y };
    j["splitRatio"] = node->SplitRatio;
    j["splitDirection"] = static_cast<int>(node->SplitDirection);
    j["visible"] = node->IsVisible;
    
    j["children"] = nlohmann::json::array();
    for (const auto& child : node->Children) {
        nlohmann::json childJson;
        SerializeNode(child.get(), childJson);
        j["children"].push_back(childJson);
    }
}

std::shared_ptr<DockNode> EditorLayout::DeserializeNode(const nlohmann::json& j) {
    auto node = std::make_shared<DockNode>();
    
    node->Name = j["name"].get<std::string>();
    node->Size = { j["size"][0], j["size"][1] };
    node->Position = { j["position"][0], j["position"][1] };
    node->SplitRatio = j["splitRatio"];
    node->SplitDirection = static_cast<DockDirection>(j["splitDirection"].get<int>());
    node->IsVisible = j["visible"];
    
    for (const auto& childJson : j["children"]) {
        auto child = DeserializeNode(childJson);
        child->Parent = node.get();
        node->Children.push_back(child);
    }
    
    return node;
}

void EditorLayout::UpdateParentPointers(DockNode* node, DockNode* parent) {
    if (!node)
        return;
        
    node->Parent = parent;
    
    for (auto& child : node->Children)
        UpdateParentPointers(child.get(), node);
}

} // namespace Editor
} // namespace VGE



