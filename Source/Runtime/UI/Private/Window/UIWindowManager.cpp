#include "Window/UIWindowManager.h"
#include "Core/Logger.h"
#include <algorithm>

namespace VGE {
namespace UI {

UIWindowManager& UIWindowManager::Get() {
    static UIWindowManager instance;
    return instance;
}

std::shared_ptr<UIWindow> UIWindowManager::CreateWindow(const UIWindowDesc& desc) {
    auto window = std::make_shared<UIWindow>(desc);
    m_Windows.push_back(window);
    m_WindowStates[window] = WindowState();

    OnWindowCreated(window);
    return window;
}

void UIWindowManager::DestroyWindow(std::shared_ptr<UIWindow> window) {
    if (!window) return;
    
    OnWindowDestroyed(window);
    RemoveWindow(window);
}

void UIWindowManager::DestroyAllWindows() {
    while (!m_Windows.empty()) {
        DestroyWindow(m_Windows.back());
    }
}

std::shared_ptr<UIWindow> UIWindowManager::GetWindow(const std::string& title) const {
    auto it = std::find_if(m_Windows.begin(), m_Windows.end(),
        [&title](const std::shared_ptr<UIWindow>& window) {
            return window->GetTitle() == title;
        });
    return it != m_Windows.end() ? *it : nullptr;
}

void UIWindowManager::ShowWindow(std::shared_ptr<UIWindow> window) {
    if (!window) return;
    window->SetOpen(true);
    auto& state = GetOrCreateState(window);
    state.isVisible = true;
}

void UIWindowManager::HideWindow(std::shared_ptr<UIWindow> window) {
    if (!window) return;
    window->SetOpen(false);
    auto& state = GetOrCreateState(window);
    state.isVisible = false;
}

void UIWindowManager::FocusWindow(std::shared_ptr<UIWindow> window) {
    if (!window) return;
    if (m_ActiveWindow == window) return;

    if (m_ActiveWindow) {
        OnWindowDeactivated(m_ActiveWindow);
    }

    m_ActiveWindow = window;
    window->SetFocused(true);
    OnWindowActivated(window);
}

void UIWindowManager::MaximizeWindow(std::shared_ptr<UIWindow> window) {
    if (!window) return;
    window->SetMaximized(true);
    auto& state = GetOrCreateState(window);
    state.isMaximized = true;
    state.isMinimized = false;
    OnWindowStateChanged(window);
}

void UIWindowManager::MinimizeWindow(std::shared_ptr<UIWindow> window) {
    if (!window) return;
    window->SetMinimized(true);
    auto& state = GetOrCreateState(window);
    state.isMinimized = true;
    state.isMaximized = false;
    OnWindowStateChanged(window);
}

void UIWindowManager::RestoreWindow(std::shared_ptr<UIWindow> window) {
    if (!window) return;
    window->SetMaximized(false);
    window->SetMinimized(false);
    auto& state = GetOrCreateState(window);
    state.isMinimized = false;
    state.isMaximized = false;
    OnWindowStateChanged(window);
}

void UIWindowManager::CloseWindow(std::shared_ptr<UIWindow> window) {
    if (!window) return;
    window->SetOpen(false);
}

void UIWindowManager::SaveWindowStates() {
    for (const auto& window : m_Windows) {
        if (!window || !window->GetPlatformWindow()) continue;
        
        auto& state = GetOrCreateState(window);
        state.position = window->GetPlatformWindow()->GetPosition();
        state.size = window->GetPlatformWindow()->GetSize();
        state.isVisible = window->GetPlatformWindow()->IsVisible();
        state.isMinimized = window->GetPlatformWindow()->IsMinimized();
        state.isMaximized = window->GetPlatformWindow()->IsMaximized();
    }
}

void UIWindowManager::RestoreWindowStates() {
    for (const auto& window : m_Windows) {
        if (!window || !window->GetPlatformWindow()) continue;
        
        const auto& state = GetWindowState(window);
        window->GetPlatformWindow()->SetPosition(state.position);
        window->GetPlatformWindow()->SetSize(state.size);
        
        if (state.isVisible) {
            window->GetPlatformWindow()->Show();
        } else {
            window->GetPlatformWindow()->Hide();
        }
        
        if (state.isMinimized) {
            window->GetPlatformWindow()->Minimize();
        } else if (state.isMaximized) {
            window->GetPlatformWindow()->Maximize();
        } else {
            window->GetPlatformWindow()->Restore();
        }
    }
}

void UIWindowManager::SetWindowState(std::shared_ptr<UIWindow> window, const WindowState& state) {
    if (!window || !window->GetPlatformWindow()) return;
    
    m_WindowStates[window] = state;
    
    window->GetPlatformWindow()->SetPosition(state.position);
    window->GetPlatformWindow()->SetSize(state.size);
    
    if (state.isVisible) {
        window->GetPlatformWindow()->Show();
    } else {
        window->GetPlatformWindow()->Hide();
    }
    
    if (state.isMinimized) {
        window->GetPlatformWindow()->Minimize();
    } else if (state.isMaximized) {
        window->GetPlatformWindow()->Maximize();
    } else {
        window->GetPlatformWindow()->Restore();
    }
    
    OnWindowStateChanged(window);
}

const WindowState& UIWindowManager::GetWindowState(std::shared_ptr<UIWindow> window) const {
    static WindowState defaultState;
    auto it = m_WindowStates.find(window);
    return it != m_WindowStates.end() ? it->second : defaultState;
}

void UIWindowManager::ArrangeWindows() {
    if (m_ArrangingWindows || m_Windows.empty()) return;
    m_ArrangingWindows = true;

    // Default to cascade arrangement
    CascadeWindows();

    m_ArrangingWindows = false;
}

void UIWindowManager::CascadeWindows() {
    if (m_Windows.empty()) return;

    const float offsetX = 30.0f;
    const float offsetY = 30.0f;
    glm::vec2 position(0.0f);

    for (const auto& window : m_Windows) {
        if (!window || !window->GetPlatformWindow()) continue;
        if (!window->GetPlatformWindow()->IsVisible() || window->GetPlatformWindow()->IsMinimized()) continue;

        window->GetPlatformWindow()->SetPosition(position);
        position.x += offsetX;
        position.y += offsetY;
    }
}

void UIWindowManager::TileWindows(bool horizontal) {
    if (m_Windows.empty()) return;

    std::vector<std::shared_ptr<UIWindow>> visibleWindows;
    for (const auto& window : m_Windows) {
        if (window && window->GetPlatformWindow() && 
            window->GetPlatformWindow()->IsVisible() && 
            !window->GetPlatformWindow()->IsMinimized()) {
            visibleWindows.push_back(window);
        }
    }

    if (visibleWindows.empty()) return;

    // TODO: Implement tiling logic based on available screen space
}

void UIWindowManager::MinimizeAll() {
    for (const auto& window : m_Windows) {
        MinimizeWindow(window);
    }
}

void UIWindowManager::RestoreAll() {
    for (const auto& window : m_Windows) {
        RestoreWindow(window);
    }
}

// Protected event handlers
void UIWindowManager::OnWindowCreated(std::shared_ptr<UIWindow> window) {}
void UIWindowManager::OnWindowDestroyed(std::shared_ptr<UIWindow> window) {}
void UIWindowManager::OnWindowActivated(std::shared_ptr<UIWindow> window) {}
void UIWindowManager::OnWindowDeactivated(std::shared_ptr<UIWindow> window) {}
void UIWindowManager::OnWindowMoved(std::shared_ptr<UIWindow> window, const glm::vec2& position) {}
void UIWindowManager::OnWindowResized(std::shared_ptr<UIWindow> window, const glm::vec2& size) {}
void UIWindowManager::OnWindowStateChanged(std::shared_ptr<UIWindow> window) {}

// Private helper functions
void UIWindowManager::UpdateWindowOrder() {
    // Sort windows by z-order
    std::sort(m_Windows.begin(), m_Windows.end(),
        [](const std::shared_ptr<UIWindow>& a, const std::shared_ptr<UIWindow>& b) {
            const auto& stateA = UIWindowManager::Get().GetWindowState(a);
            const auto& stateB = UIWindowManager::Get().GetWindowState(b);
            return stateA.zOrder < stateB.zOrder;
        });
}

void UIWindowManager::UpdateWindowStates() {
    for (const auto& window : m_Windows) {
        if (!window || !window->GetPlatformWindow()) continue;
        
        auto& state = GetOrCreateState(window);
        state.position = window->GetPlatformWindow()->GetPosition();
        state.size = window->GetPlatformWindow()->GetSize();
        state.isVisible = window->GetPlatformWindow()->IsVisible();
        state.isMinimized = window->GetPlatformWindow()->IsMinimized();
        state.isMaximized = window->GetPlatformWindow()->IsMaximized();
    }
}

WindowState& UIWindowManager::GetOrCreateState(std::shared_ptr<UIWindow> window) {
    auto it = m_WindowStates.find(window);
    if (it == m_WindowStates.end()) {
        return m_WindowStates[window] = WindowState();
    }
    return it->second;
}

void UIWindowManager::RemoveWindow(std::shared_ptr<UIWindow> window) {
    m_WindowStates.erase(window);
    m_Windows.erase(
        std::remove(m_Windows.begin(), m_Windows.end(), window),
        m_Windows.end()
    );

    if (m_ActiveWindow == window) {
        m_ActiveWindow = nullptr;
        if (!m_Windows.empty()) {
            FocusWindow(m_Windows.back());
        }
    }
}

}} // namespace VGE::UI
