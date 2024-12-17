#pragma once

#include "Core/UICore.h"
#include "Widget/UIWidget.h"
#include "Widget/UIWindow.h"
#include <memory>
#include <vector>
#include <unordered_map>

namespace VGE {
namespace UI {

enum class WindowState {
    Normal,
    Minimized,
    Maximized,
    Hidden
};

struct WindowDesc {
    std::string title;
    glm::vec2 position;
    glm::vec2 size;
    bool resizable;
    bool movable;
    bool closable;
    bool minimizable;
    bool maximizable;

    WindowDesc()
        : position(0.0f)
        , size(400.0f, 300.0f)
        , resizable(true)
        , movable(true)
        , closable(true)
        , minimizable(true)
        , maximizable(true)
    {}
};

class UI_API UIWindowManager : public UIWidget {
public:
    UIWindowManager();
    virtual ~UIWindowManager();

    UIWindow* CreateWindow(const WindowDesc& desc);
    void DestroyWindow(UIWindow* window);
    void DestroyAllWindows();
    UIWindow* GetWindow(const std::string& title) const;

    void AddWindow(std::shared_ptr<UIWindow> window);
    void RemoveWindow(std::shared_ptr<UIWindow> window);
    void BringToFront(std::shared_ptr<UIWindow> window);
    void Clear();

    void ShowWindow(UIWindow* window);
    void HideWindow(UIWindow* window);
    void FocusWindow(UIWindow* window);
    void MaximizeWindow(UIWindow* window);
    void MinimizeWindow(UIWindow* window);
    void RestoreWindow(UIWindow* window);
    void CloseWindow(UIWindow* window);

    void SetWindowState(UIWindow* window, WindowState state);
    WindowState GetWindowState(UIWindow* window) const;

    void SaveWindowStates();
    void RestoreWindowStates();
    void ArrangeWindows();

    virtual void OnRender() override;
    virtual void OnUpdate(float deltaTime) override;
    virtual bool OnMouseMove(const glm::vec2& position) override;
    virtual bool OnMouseButton(int button, bool pressed, const glm::vec2& position) override;
    virtual bool OnMouseScroll(float xoffset, float yoffset) override;
    virtual bool OnKeyboard(int key, bool pressed) override;
    virtual bool OnChar(unsigned int codepoint) override;

    const std::vector<std::shared_ptr<UIWindow>>& GetWindows() const { return m_Windows; }
    std::shared_ptr<UIWindow> GetActiveWindow() const { return m_ActiveWindow; }
    std::shared_ptr<UIWindow> GetWindowAtPosition(const glm::vec2& position) const;

private:
    struct WindowState {
        glm::vec2 position;
        glm::vec2 size;
        bool visible;
        bool minimized;
        bool maximized;
    };

    void OnWindowCreated(UIWindow* window);
    void OnWindowDestroyed(UIWindow* window);
    void OnWindowStateChanged(UIWindow* window);
    void OnWindowActivated(UIWindow* window);
    void OnWindowDeactivated(UIWindow* window);
    void UpdateWindowOrder();
    WindowState& GetOrCreateState(UIWindow* window);

private:
    std::vector<std::shared_ptr<UIWindow>> m_Windows;
    std::shared_ptr<UIWindow> m_ActiveWindow;
    std::shared_ptr<UIWindow> m_DraggedWindow;
    std::unordered_map<UIWindow*, WindowState> m_WindowStates;
    glm::vec2 m_DragOffset;
    bool m_IsDragging;
    bool m_ArrangingWindows;
};

}} // namespace VGE::UI 