#pragma once

#include "Runtime/Core/Public/Core/UICore.h"
#include "Runtime/Core/Public/Widget/UIWidget.h"
#include "Platform/UIWindow.h"
#include "Platform/UIWindowDesc.h"
#include <string>
#include <memory>
#include <functional>

namespace VGE {
namespace UI {

/**
 * @brief Base class for window widgets
 */
class UI_API UIWindow : public UIWidget {
public:
    explicit UIWindow(const std::string& title = "")
        : m_Title(title)
        , m_PlatformWindow(std::make_unique<Platform::PlatformWindow>(CreatePlatformDesc(title))) {}
    
    UIWindow(const std::string& title, int width, int height)
        : m_Title(title)
        , m_PlatformWindow(std::make_unique<Platform::PlatformWindow>(CreatePlatformDesc(title, width, height))) {}
        
    explicit UIWindow(const UIWindowDesc& desc)
        : m_Title(desc.title)
        , m_PlatformWindow(std::make_unique<Platform::PlatformWindow>(CreatePlatformDesc(desc))) {}
        
    virtual ~UIWindow() = default;

    // Window state
    bool IsOpen() const { return m_PlatformWindow && !m_PlatformWindow->ShouldClose(); }
    void SetOpen(bool open) { if (m_PlatformWindow) m_PlatformWindow->SetShouldClose(!open); }
    bool IsFocused() const { return m_PlatformWindow && m_PlatformWindow->IsFocused(); }
    void SetFocused(bool focused) { if (m_PlatformWindow) m_PlatformWindow->Focus(); }
    bool IsMinimized() const { return m_PlatformWindow && m_PlatformWindow->IsMinimized(); }
    void SetMinimized(bool minimized) { if (m_PlatformWindow) minimized ? m_PlatformWindow->Minimize() : m_PlatformWindow->Restore(); }
    bool IsMaximized() const { return m_PlatformWindow && m_PlatformWindow->IsMaximized(); }
    void SetMaximized(bool maximized) { if (m_PlatformWindow) maximized ? m_PlatformWindow->Maximize() : m_PlatformWindow->Restore(); }

    // Window properties
    const std::string& GetTitle() const { return m_Title; }
    void SetTitle(const std::string& title) { 
        m_Title = title;
        if (m_PlatformWindow) m_PlatformWindow->SetTitle(title);
    }
    bool IsResizable() const { return m_PlatformWindow && m_PlatformWindow->IsResizable(); }
    void SetResizable(bool resizable) { if (m_PlatformWindow) m_PlatformWindow->SetResizable(resizable); }

    // Window events
    using WindowCallback = std::function<void()>;
    void SetOnClose(WindowCallback callback) { m_OnClose = callback; }
    void SetOnFocus(WindowCallback callback) { m_OnFocus = callback; }
    void SetOnBlur(WindowCallback callback) { m_OnBlur = callback; }
    void SetOnMinimize(WindowCallback callback) { m_OnMinimize = callback; }
    void SetOnMaximize(WindowCallback callback) { m_OnMaximize = callback; }
    void SetOnRestore(WindowCallback callback) { m_OnRestore = callback; }

    // Widget interface
    void Update(float deltaTime) override;
    void Render() override;

    // Platform window access
    Platform::PlatformWindow* GetPlatformWindow() { return m_PlatformWindow.get(); }
    const Platform::PlatformWindow* GetPlatformWindow() const { return m_PlatformWindow.get(); }

protected:
    // Event handlers
    virtual void OnClosing();
    virtual void OnFocused();
    virtual void OnBlurred();
    virtual void OnMinimized();
    virtual void OnMaximized();
    virtual void OnRestored();

private:
    Platform::WindowDesc CreatePlatformDesc(const std::string& title, int width = 1280, int height = 720) const {
        Platform::WindowDesc desc;
        desc.title = title;
        desc.size = glm::vec2(width, height);
        return desc;
    }

    Platform::WindowDesc CreatePlatformDesc(const UIWindowDesc& desc) const {
        Platform::WindowDesc platformDesc;
        platformDesc.title = desc.title;
        platformDesc.size = glm::vec2(desc.width, desc.height);
        platformDesc.resizable = desc.resizable;
        platformDesc.decorated = desc.decorated;
        platformDesc.floating = desc.floating;
        platformDesc.maximized = desc.maximized;
        platformDesc.minimized = desc.minimized;
        platformDesc.visible = desc.visible;
        platformDesc.focusOnShow = desc.focusOnShow;
        platformDesc.scaleToMonitor = desc.scaleToMonitor;
        platformDesc.centerCursor = desc.centerCursor;
        platformDesc.transparentFramebuffer = desc.transparentFramebuffer;
        platformDesc.fullscreen = desc.fullscreen;
        return platformDesc;
    }

private:
    std::string m_Title;
    std::unique_ptr<Platform::PlatformWindow> m_PlatformWindow;

    // Event callbacks
    WindowCallback m_OnClose;
    WindowCallback m_OnFocus;
    WindowCallback m_OnBlur;
    WindowCallback m_OnMinimize;
    WindowCallback m_OnMaximize;
    WindowCallback m_OnRestore;
};

}} // namespace VGE::UI