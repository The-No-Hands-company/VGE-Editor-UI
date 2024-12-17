#include "Runtime/UI/Public/Docking/UIDockingManager.h"
#include "Runtime/Core/Public/Core/Logger.h"

namespace VGE {
namespace Editor {

UIDockingManager& UIDockingManager::Get() {
    static UIDockingManager instance;
    return instance;
}

std::shared_ptr<UIDockSpace> UIDockingManager::CreateDockSpace(const std::string& name) {
    // Check if dock space already exists
    auto it = m_DockSpaces.find(name);
    if (it != m_DockSpaces.end()) {
        Logger::Log(LogLevel::Warning, "Dock space '{}' already exists", name);
        return it->second;
    }

    // Create new dock space
    auto dockSpace = std::make_shared<UIDockSpace>(name);
    m_DockSpaces[name] = dockSpace;

    // If no root dock space set, use this one
    if (!m_RootDockSpace) {
        m_RootDockSpace = dockSpace;
    }

    return dockSpace;
}

std::shared_ptr<UIDockSpace> UIDockingManager::GetDockSpace(const std::string& name) const {
    auto it = m_DockSpaces.find(name);
    return it != m_DockSpaces.end() ? it->second : nullptr;
}

void UIDockingManager::RemoveDockSpace(const std::string& name) {
    auto it = m_DockSpaces.find(name);
    if (it != m_DockSpaces.end()) {
        if (it->second == m_RootDockSpace) {
            m_RootDockSpace.reset();
        }
        m_DockSpaces.erase(it);
    }
}

void UIDockingManager::SetRootDockSpace(std::shared_ptr<UIDockSpace> dockSpace) {
    if (!dockSpace) {
        Logger::Log(LogLevel::Warning, "Attempting to set null root dock space");
        return;
    }

    m_RootDockSpace = dockSpace;

    // Add to dock spaces if not already present
    auto name = dockSpace->GetName();
    if (m_DockSpaces.find(name) == m_DockSpaces.end()) {
        m_DockSpaces[name] = dockSpace;
    }
}

void UIDockingManager::Update(float deltaTime) {
    // Update all dock spaces
    for (const auto& pair : m_DockSpaces) {
        if (auto dockSpace = pair.second) {
            dockSpace->Update(deltaTime);
        }
    }
}

void UIDockingManager::Render() {
    // Render all dock spaces
    for (const auto& pair : m_DockSpaces) {
        if (auto dockSpace = pair.second) {
            dockSpace->Render();
        }
    }
}

}} // namespace VGE::Editor 