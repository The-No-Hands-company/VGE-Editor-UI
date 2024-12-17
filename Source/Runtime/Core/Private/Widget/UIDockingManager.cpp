#include "Widget/UIDockingManager.h"
#include "Core/UICore.h"
#include <imgui.h>

namespace VGE {
namespace UI {

UIDockingManager& UIDockingManager::Get() {
    static UIDockingManager instance;
    return instance;
}

void UIDockingManager::Initialize() {
    m_DockableWindows.clear();
    m_DockRelationships.clear();
    m_DraggingWindow.clear();
    m_IsDragging = false;
}

void UIDockingManager::Update(float deltaTime) {
    // Update all dockable windows
    for (auto& [name, dockable] : m_DockableWindows) {
        if (dockable.window) {
            dockable.window->Update(deltaTime);
        }
        if (dockable.dockSpace) {
            dockable.dockSpace->Update(deltaTime);
        }
    }
    
    // Update drag preview if dragging
    if (m_IsDragging) {
        ImVec2 mousePos = ImGui::GetMousePos();
        UpdateWindowDragPreview({mousePos.x, mousePos.y});
        
        // End drag on mouse release
        if (!ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
            EndWindowDrag();
        }
    }
}

std::shared_ptr<UIWindow> UIDockingManager::CreateDockableWindow(
    const std::string& name, const std::string& title) {
    // Create window and dock space
    auto window = std::make_shared<UIWindow>(name, title);
    auto dockSpace = std::make_shared<UIDockSpace>(name + "_DockSpace");
    
    // Initialize both
    window->Initialize();
    dockSpace->Initialize();
    
    // Setup dockable window
    DockableWindow dockable;
    dockable.window = window;
    dockable.dockSpace = dockSpace;
    dockable.isDetached = false;
    dockable.detachedPosition = {100, 100};
    dockable.detachedSize = {800, 600};
    
    m_DockableWindows[name] = dockable;
    return window;
}

std::shared_ptr<UIWindow> UIDockingManager::GetDockableWindow(const std::string& name) const {
    auto it = m_DockableWindows.find(name);
    if (it != m_DockableWindows.end()) {
        return it->second.window;
    }
    return nullptr;
}

bool UIDockingManager::DockWindowToWindow(const std::string& sourceWindow, const std::string& targetWindow,
                                       DockZone zone, float ratio) {
    // Find source and target
    auto sourceIt = m_DockableWindows.find(sourceWindow);
    auto targetIt = m_DockableWindows.find(targetWindow);
    if (sourceIt == m_DockableWindows.end() || targetIt == m_DockableWindows.end()) {
        return false;
    }
    
    auto& source = sourceIt->second;
    auto& target = targetIt->second;
    
    // Can't dock to self
    if (sourceWindow == targetWindow) {
        return false;
    }
    
    // Detach source if needed
    if (!source.isDetached) {
        DetachWindow(sourceWindow);
    }
    
    // Create dock relationship
    UpdateDockRelationships(sourceWindow, targetWindow, zone, ratio);
    
    // Update window states
    source.isDetached = false;
    source.window->SetVisible(true);
    
    // Dock the window
    if (target.dockSpace) {
        return target.dockSpace->DockWindow(source.window, sourceWindow, source.window->GetTitle(), zone);
    }
    
    return false;
}

bool UIDockingManager::DetachWindow(const std::string& windowName) {
    auto it = m_DockableWindows.find(windowName);
    if (it == m_DockableWindows.end()) {
        return false;
    }
    
    auto& dockable = it->second;
    
    // Already detached
    if (dockable.isDetached) {
        return true;
    }
    
    // Remove from any existing dock relationships
    RemoveDockRelationships(windowName);
    
    // Update window state
    dockable.isDetached = true;
    dockable.window->SetPosition(dockable.detachedPosition);
    dockable.window->SetSize(dockable.detachedSize);
    
    return true;
}

void UIDockingManager::BeginWindowDrag(const std::string& windowName) {
    auto it = m_DockableWindows.find(windowName);
    if (it == m_DockableWindows.end()) {
        return;
    }
    
    m_DraggingWindow = windowName;
    m_IsDragging = true;
    
    // Store current position/size for detached state
    auto& dockable = it->second;
    dockable.detachedPosition = dockable.window->GetPosition();
    dockable.detachedSize = dockable.window->GetSize();
}

void UIDockingManager::UpdateWindowDragPreview(const glm::vec2& mousePos) {
    if (!m_IsDragging) {
        return;
    }
    
    // Find potential dock target
    auto [targetWindow, zone] = FindDockTarget(mousePos);
    
    // Update preview
    if (!targetWindow.empty()) {
        auto targetIt = m_DockableWindows.find(targetWindow);
        if (targetIt != m_DockableWindows.end() && targetIt->second.dockSpace) {
            targetIt->second.dockSpace->UpdateDragPreview(mousePos);
        }
    }
}

bool UIDockingManager::EndWindowDrag() {
    if (!m_IsDragging) {
        return false;
    }
    
    bool success = false;
    
    // Find dock target at current mouse position
    ImVec2 mousePos = ImGui::GetMousePos();
    auto [targetWindow, zone] = FindDockTarget({mousePos.x, mousePos.y});
    
    // Attempt to dock
    if (!targetWindow.empty()) {
        success = DockWindowToWindow(m_DraggingWindow, targetWindow, zone);
    }
    else {
        // No target found, detach the window
        success = DetachWindow(m_DraggingWindow);
        
        // Position at mouse
        if (success) {
            auto it = m_DockableWindows.find(m_DraggingWindow);
            if (it != m_DockableWindows.end()) {
                it->second.window->SetPosition({mousePos.x, mousePos.y});
            }
        }
    }
    
    // Clear drag state
    m_DraggingWindow.clear();
    m_IsDragging = false;
    
    return success;
}

std::pair<std::string, DockZone> UIDockingManager::FindDockTarget(const glm::vec2& mousePos) const {
    // Check each window for potential dock zones
    for (const auto& [name, dockable] : m_DockableWindows) {
        // Skip dragged window
        if (name == m_DraggingWindow) {
            continue;
        }
        
        // Skip detached windows that are minimized
        if (dockable.isDetached && dockable.window->IsMinimized()) {
            continue;
        }
        
        // Check if mouse is over window
        auto windowPos = dockable.window->GetPosition();
        auto windowSize = dockable.window->GetSize();
        
        if (mousePos.x >= windowPos.x && mousePos.x <= windowPos.x + windowSize.x &&
            mousePos.y >= windowPos.y && mousePos.y <= windowPos.y + windowSize.y) {
            
            // Determine dock zone based on position within window
            float zoneSize = 50.0f; // Size of edge zones
            
            // Left zone
            if (mousePos.x < windowPos.x + zoneSize) {
                return {name, DockZone::Left};
            }
            // Right zone
            else if (mousePos.x > windowPos.x + windowSize.x - zoneSize) {
                return {name, DockZone::Right};
            }
            // Top zone
            else if (mousePos.y < windowPos.y + zoneSize) {
                return {name, DockZone::Top};
            }
            // Bottom zone
            else if (mousePos.y > windowPos.y + windowSize.y - zoneSize) {
                return {name, DockZone::Bottom};
            }
            // Center zone
            else {
                return {name, DockZone::Center};
            }
        }
    }
    
    return {"", DockZone::None};
}

void UIDockingManager::UpdateDockRelationships(const std::string& sourceWindow, const std::string& targetWindow,
                                            DockZone zone, float ratio) {
    // Remove any existing relationships for source window
    RemoveDockRelationships(sourceWindow);
    
    // Add new relationship
    DockRelationship rel;
    rel.sourceWindow = sourceWindow;
    rel.targetWindow = targetWindow;
    rel.zone = zone;
    rel.ratio = ratio;
    m_DockRelationships.push_back(rel);
}

void UIDockingManager::RemoveDockRelationships(const std::string& windowName) {
    // Remove relationships where window is source or target
    m_DockRelationships.erase(
        std::remove_if(m_DockRelationships.begin(), m_DockRelationships.end(),
            [&](const DockRelationship& rel) {
                return rel.sourceWindow == windowName || rel.targetWindow == windowName;
            }),
        m_DockRelationships.end()
    );
}

}} // namespace VGE::UI 