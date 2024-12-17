#pragma once

#include "Core/UICore.h"
#include "Platform/UIWindowEvent.h"
#include <glm/glm.hpp>
#include <string>
#include <functional>
#include <vector>

namespace VGE {
namespace Platform {

struct WindowDesc {
    std::string title = "VersaUI Window";
    glm::vec2 size = glm::vec2(800.0f, 600.0f);
    bool resizable = true;
    bool decorated = true;
    bool floating = false;
    bool maximized = false;
    bool minimized = false;
    bool visible = true;
    bool focused = true;
    bool focusOnShow = true;
    bool scaleToMonitor = true;
    bool centerCursor = true;
    bool transparentFramebuffer = false;
    bool fullscreen = false;
    int samples = 0; // MSAA samples (0 = disabled)
    int zOrder = 0;  // Initial z-order (higher values are on top)
};

class PlatformWindow {
public:
    explicit PlatformWindow(const WindowDesc& desc)
        : m_Desc(desc)
        , m_Title(desc.title)
        , m_Position(0.0f)
        , m_Size(desc.size)
        , m_Visible(desc.visible)
        , m_Minimized(desc.minimized)
        , m_Maximized(desc.maximized)
        , m_Focused(desc.focused)
        , m_Resizable(desc.resizable)
        , m_Decorated(desc.decorated)
        , m_Floating(desc.floating)
        , m_ZOrder(desc.zOrder)
    {}

    virtual ~PlatformWindow() = default;

    // Window state
    bool IsVisible() const { return m_Visible; }
    bool IsMinimized() const { return m_Minimized; }
    bool IsMaximized() const { return m_Maximized; }
    bool IsFocused() const { return m_Focused; }
    bool IsResizable() const { return m_Resizable; }
    bool IsDecorated() const { return m_Decorated; }
    bool IsFloating() const { return m_Floating; }

    // Window properties
    const std::string& GetTitle() const { return m_Title; }
    const glm::vec2& GetPosition() const { return m_Position; }
    const glm::vec2& GetSize() const { return m_Size; }
    int GetZOrder() const { return m_ZOrder; }

    // Window operations
    void Show() { PlatformShow(); }
    void Hide() { PlatformHide(); }
    void SetPosition(const glm::vec2& position) { PlatformSetPosition(position); }
    void SetSize(const glm::vec2& size) { PlatformSetSize(size); }
    void SetTitle(const std::string& title) { PlatformSetTitle(title); }
    void Focus() { PlatformFocus(); }
    void Maximize() { PlatformMaximize(); }
    void Minimize() { PlatformMinimize(); }
    void Restore() { PlatformRestore(); }
    void Close() { PlatformClose(); }
    void BringToFront() { PlatformBringToFront(); }
    void SendToBack() { PlatformSendToBack(); }
    void SetZOrder(int zOrder) { 
        m_ZOrder = zOrder;
        PlatformSetZOrder(zOrder);
    }

    // Event handling
    void AddEventCallback(UI::WindowEventCallback callback) { m_EventCallbacks.push_back(callback); }
    void RemoveEventCallback(UI::WindowEventCallback callback) {
        m_EventCallbacks.erase(
            std::remove_if(m_EventCallbacks.begin(), m_EventCallbacks.end(),
                [&](const UI::WindowEventCallback& cb) { return cb.target_type() == callback.target_type(); }),
            m_EventCallbacks.end()
        );
    }

protected:
    // Platform-specific implementation should override these
    virtual void PlatformShow() = 0;
    virtual void PlatformHide() = 0;
    virtual void PlatformSetPosition(const glm::vec2& position) = 0;
    virtual void PlatformSetSize(const glm::vec2& size) = 0;
    virtual void PlatformSetTitle(const std::string& title) = 0;
    virtual void PlatformFocus() = 0;
    virtual void PlatformMaximize() = 0;
    virtual void PlatformMinimize() = 0;
    virtual void PlatformRestore() = 0;
    virtual void PlatformClose() = 0;
    virtual void PlatformBringToFront() = 0;
    virtual void PlatformSendToBack() = 0;
    virtual void PlatformSetZOrder(int zOrder) = 0;

    // Event dispatch helper
    void DispatchEvent(const UI::WindowEvent& event) {
        for (const auto& callback : m_EventCallbacks) {
            callback(event);
            if (event.handled) break;
        }
    }

protected:
    WindowDesc m_Desc;
    std::string m_Title;
    glm::vec2 m_Position;
    glm::vec2 m_Size;
    bool m_Visible;
    bool m_Minimized;
    bool m_Maximized;
    bool m_Focused;
    bool m_Resizable;
    bool m_Decorated;
    bool m_Floating;
    int m_ZOrder;
    std::vector<UI::WindowEventCallback> m_EventCallbacks;
};

}} // namespace VGE::Platform