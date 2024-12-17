#pragma once

#include "Runtime/Core/Public/Core/UICore.h"
#include "Runtime/UI/Public/Window/UIWindow.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace VGE {
namespace UI {

struct WindowState {
    glm::vec2 position = glm::vec2(0.0f);
    glm::vec2 size = glm::vec2(800.0f, 600.0f);
    bool isVisible = true;
    bool isMinimized = false;
    bool isMaximized = false;
    bool isTopMost = false;
    int zOrder = 0;
};

/**
 * @brief Manages window creation, destruction, and state
 */
class UI_API UIWindowManager {
public:
    static UIWindowManager& Get();

    // Window management
    std::shared_ptr<UIWindow> CreateWindow(const UIWindowDesc& desc);
    void DestroyWindow(std::shared_ptr<UIWindow> window);
    void DestroyAllWindows();
    std::shared_ptr<UIWindow> GetWindow(const std::string& title) const;
    const std::vector<std::shared_ptr<UIWindow>>& GetWindows() const { return m_Windows; }

    // Window operations
    void ShowWindow(std::shared_ptr<UIWindow> window);
    void HideWindow(std::shared_ptr<UIWindow> window);
    void FocusWindow(std::shared_ptr<UIWindow> window);
    void MaximizeWindow(std::shared_ptr<UIWindow> window);
    void MinimizeWindow(std::shared_ptr<UIWindow> window);
    void RestoreWindow(std::shared_ptr<UIWindow> window);
    void CloseWindow(std::shared_ptr<UIWindow> window);

    // Window state
    void SaveWindowStates();
    void RestoreWindowStates();
    void SetWindowState(std::shared_ptr<UIWindow> window, const WindowState& state);
    const WindowState& GetWindowState(std::shared_ptr<UIWindow> window) const;

    // Window layout
    void ArrangeWindows();
    void CascadeWindows();
    void TileWindows(bool horizontal = false);
    void MinimizeAll();
    void RestoreAll();

    // Active window
    std::shared_ptr<UIWindow> GetActiveWindow() const { return m_ActiveWindow; }

protected:
    UIWindowManager() = default;
    ~UIWindowManager() = default;

    // Event handlers
    virtual void OnWindowCreated(std::shared_ptr<UIWindow> window);
    virtual void OnWindowDestroyed(std::shared_ptr<UIWindow> window);
    virtual void OnWindowActivated(std::shared_ptr<UIWindow> window);
    virtual void OnWindowDeactivated(std::shared_ptr<UIWindow> window);
    virtual void OnWindowMoved(std::shared_ptr<UIWindow> window, const glm::vec2& position);
    virtual void OnWindowResized(std::shared_ptr<UIWindow> window, const glm::vec2& size);
    virtual void OnWindowStateChanged(std::shared_ptr<UIWindow> window);

private:
    // Helper functions
    void UpdateWindowOrder();
    void UpdateWindowStates();
    WindowState& GetOrCreateState(std::shared_ptr<UIWindow> window);
    void RemoveWindow(std::shared_ptr<UIWindow> window);

private:
    std::vector<std::shared_ptr<UIWindow>> m_Windows;
    std::unordered_map<std::shared_ptr<UIWindow>, WindowState> m_WindowStates;
    std::shared_ptr<UIWindow> m_ActiveWindow;
    bool m_ArrangingWindows = false;
};

}} // namespace VGE::UI 