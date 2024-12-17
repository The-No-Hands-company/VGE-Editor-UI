#include "Runtime/Core/Public/Widget/UIWindowManager.h"
#include <algorithm>

namespace VGE {
namespace UI {

UIWindowManager::UIWindowManager()
    : m_ActiveWindow(nullptr)
    , m_ArrangingWindows(false)
{
}

UIWindow* UIWindowManager::CreateWindow(const UIWindowDesc& desc)
{
    auto window = new UIWindow(desc);
    m_Windows.push_back(window);
    m_WindowStates[window] = WindowState();
    OnWindowCreated(window);
    return window;
}

void UIWindowManager::DestroyWindow(UIWindow* window)
{
    if (!window) return;
    
    OnWindowDestroyed(window);
    RemoveWindow(window);
    delete window;
}

void UIWindowManager::DestroyAllWindows()
{
    while (!m_Windows.empty()) {
        DestroyWindow(m_Windows.back());
    }
}

UIWindow* UIWindowManager::GetWindow(const std::string& id) const
{
    auto it = std::find_if(m_Windows.begin(), m_Windows.end(),
        [&id](const UIWindow* window) {
            return window->GetTitle() == id;
        });
    return it != m_Windows.end() ? *it : nullptr;
}

void UIWindowManager::ShowWindow(UIWindow* window)
{
    if (!window) return;
    window->Show();
    auto& state = GetOrCreateState(window);
    state.isVisible = true;
    OnWindowStateChanged(window);
}

void UIWindowManager::HideWindow(UIWindow* window)
{
    if (!window) return;
    window->Hide();
    auto& state = GetOrCreateState(window);
    state.isVisible = false;
    OnWindowStateChanged(window);
}

void UIWindowManager::FocusWindow(UIWindow* window)
{
    if (!window || window == m_ActiveWindow) return;

    if (m_ActiveWindow) {
        OnWindowDeactivated(m_ActiveWindow);
    }

    m_ActiveWindow = window;
    window->Focus();
    UpdateWindowOrder();
    OnWindowActivated(window);
}

void UIWindowManager::MaximizeWindow(UIWindow* window)
{
    if (!window) return;
    window->Maximize();
    OnWindowStateChanged(window);
}

void UIWindowManager::MinimizeWindow(UIWindow* window)
{
    if (!window) return;
    window->Minimize();
    OnWindowStateChanged(window);
}

void UIWindowManager::RestoreWindow(UIWindow* window)
{
    if (!window) return;
    window->Restore();
    OnWindowStateChanged(window);
}

void UIWindowManager::CloseWindow(UIWindow* window)
{
    if (!window) return;
    window->Close();
}

void UIWindowManager::SetWindowState(UIWindow* window, const WindowState& state)
{
    if (!window) return;
    m_WindowStates[window] = state;
    
    if (state.isVisible) {
        window->Show();
    } else {
        window->Hide();
    }

    window->SetPosition(state.position);
    window->SetSize(state.size);
    OnWindowStateChanged(window);
}

const UIWindowManager::WindowState& UIWindowManager::GetWindowState(UIWindow* window) const
{
    static WindowState defaultState;
    auto it = m_WindowStates.find(window);
    return it != m_WindowStates.end() ? it->second : defaultState;
}

void UIWindowManager::SaveWindowStates()
{
    for (auto* window : m_Windows) {
        auto& state = GetOrCreateState(window);
        state.position = window->GetPosition();
        state.size = window->GetSize();
        state.isVisible = window->IsVisible();
    }
}

void UIWindowManager::RestoreWindowStates()
{
    for (auto* window : m_Windows) {
        const auto& state = GetWindowState(window);
        SetWindowState(window, state);
    }
}

void UIWindowManager::ArrangeWindows()
{
    if (m_ArrangingWindows || m_Windows.empty()) return;
    m_ArrangingWindows = true;

    // Get available space
    glm::vec2 pos(0.0f);
    glm::vec2 spacing(20.0f);
    
    for (auto* window : m_Windows) {
        if (!window->IsVisible()) continue;

        window->SetPosition(pos);
        pos.x += spacing.x;
        pos.y += spacing.y;
    }

    m_ArrangingWindows = false;
}

void UIWindowManager::CascadeWindows()
{
    if (m_ArrangingWindows || m_Windows.empty()) return;
    m_ArrangingWindows = true;

    glm::vec2 pos(0.0f);
    glm::vec2 offset(30.0f);
    
    for (auto* window : m_Windows) {
        if (!window->IsVisible()) continue;

        window->SetPosition(pos);
        pos += offset;
    }

    m_ArrangingWindows = false;
}

void UIWindowManager::TileWindows(bool horizontal)
{
    if (m_ArrangingWindows || m_Windows.empty()) return;
    m_ArrangingWindows = true;

    // Count visible windows
    size_t visibleCount = std::count_if(m_Windows.begin(), m_Windows.end(),
        [](const UIWindow* window) { return window->IsVisible(); });

    if (visibleCount == 0) {
        m_ArrangingWindows = false;
        return;
    }

    // Calculate grid dimensions
    int cols = horizontal ? static_cast<int>(visibleCount) : 1;
    int rows = horizontal ? 1 : static_cast<int>(visibleCount);

    // Calculate window size
    glm::vec2 totalSize(1920.0f, 1080.0f); // TODO: Get actual screen size
    glm::vec2 windowSize = totalSize / glm::vec2(cols, rows);

    // Position windows
    glm::vec2 pos(0.0f);
    int currentCol = 0;
    int currentRow = 0;

    for (auto* window : m_Windows) {
        if (!window->IsVisible()) continue;

        window->SetPosition(pos);
        window->SetSize(windowSize);

        if (horizontal) {
            pos.x += windowSize.x;
            currentCol++;
            if (currentCol >= cols) {
                pos.x = 0.0f;
                pos.y += windowSize.y;
                currentCol = 0;
            }
        } else {
            pos.y += windowSize.y;
            currentRow++;
            if (currentRow >= rows) {
                pos.y = 0.0f;
                pos.x += windowSize.x;
                currentRow = 0;
            }
        }
    }

    m_ArrangingWindows = false;
}

void UIWindowManager::MinimizeAll()
{
    for (auto* window : m_Windows) {
        MinimizeWindow(window);
    }
}

void UIWindowManager::RestoreAll()
{
    for (auto* window : m_Windows) {
        RestoreWindow(window);
    }
}

// Protected event handlers
void UIWindowManager::OnWindowCreated(UIWindow* window)
{
    NotifyWindowEvent(window, WindowEventType::Created);
}

void UIWindowManager::OnWindowDestroyed(UIWindow* window)
{
    NotifyWindowEvent(window, WindowEventType::Destroyed);
}

void UIWindowManager::OnWindowActivated(UIWindow* window)
{
    NotifyWindowEvent(window, WindowEventType::FocusGained);
}

void UIWindowManager::OnWindowDeactivated(UIWindow* window)
{
    NotifyWindowEvent(window, WindowEventType::FocusLost);
}

void UIWindowManager::OnWindowMoved(UIWindow* window, const glm::vec2& position)
{
    WindowEvent event;
    event.type = WindowEventType::Moved;
    event.position = position;
    window->DispatchEvent(event);
}

void UIWindowManager::OnWindowResized(UIWindow* window, const glm::vec2& size)
{
    WindowEvent event;
    event.type = WindowEventType::Resized;
    event.size = size;
    window->DispatchEvent(event);
}

void UIWindowManager::OnWindowStateChanged(UIWindow* window)
{
    SaveWindowStates();
}

// Private helpers
void UIWindowManager::UpdateWindowOrder()
{
    if (m_ActiveWindow) {
        // Move active window to end of list (top-most)
        auto it = std::find(m_Windows.begin(), m_Windows.end(), m_ActiveWindow);
        if (it != m_Windows.end()) {
            m_Windows.erase(it);
            m_Windows.push_back(m_ActiveWindow);
        }
    }
}

void UIWindowManager::UpdateWindowStates()
{
    for (auto* window : m_Windows) {
        auto& state = GetOrCreateState(window);
        state.position = window->GetPosition();
        state.size = window->GetSize();
        state.isVisible = window->IsVisible();
    }
}

UIWindowManager::WindowState& UIWindowManager::GetOrCreateState(UIWindow* window)
{
    auto it = m_WindowStates.find(window);
    if (it == m_WindowStates.end()) {
        return m_WindowStates[window] = WindowState();
    }
    return it->second;
}

void UIWindowManager::RemoveWindow(UIWindow* window)
{
    if (m_ActiveWindow == window) {
        m_ActiveWindow = nullptr;
    }

    auto it = std::find(m_Windows.begin(), m_Windows.end(), window);
    if (it != m_Windows.end()) {
        m_Windows.erase(it);
    }

    m_WindowStates.erase(window);
}

void UIWindowManager::NotifyWindowEvent(UIWindow* window, WindowEventType type)
{
    WindowEvent event;
    event.type = type;
    window->DispatchEvent(event);
}

}} // namespace VGE::UI                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               