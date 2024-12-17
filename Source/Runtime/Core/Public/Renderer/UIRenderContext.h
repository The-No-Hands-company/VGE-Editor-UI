#pragma once

#include "Core/UICore.h"
#include "Platform/GLFW/GLFWPlatformWindow.h"
#include <memory>

namespace VGE {
namespace UI {

struct WindowDesc {
    std::string title = "VersaUI Window";
    uint32_t width = 800;
    uint32_t height = 600;
    bool resizable = true;
    bool decorated = true;
    bool floating = false;
    bool maximized = false;
    bool visible = true;
    bool focused = true;
    bool focusOnShow = true;
    bool scaleToMonitor = true;
    bool centerCursor = true;
    bool transparentFramebuffer = false;
};

class UI_API UIRenderContext {
public:
    UIRenderContext();
    ~UIRenderContext();

    bool Initialize(const WindowDesc& desc);
    void MakeCurrent();
    void SwapBuffers();
    void PollEvents();
    bool ShouldClose() const;

    void SetVSync(bool enabled);
    void SetTitle(const std::string& title);
    void SetSize(uint32_t width, uint32_t height);
    void SetPosition(int x, int y);
    void SetResizable(bool resizable);
    void SetDecorated(bool decorated);
    void SetFloating(bool floating);
    void SetMaximized(bool maximized);
    void SetVisible(bool visible);
    void SetFocused(bool focused);
    void SetShouldClose(bool shouldClose);

    GLFWPlatformWindow* GetWindow() { return m_Window.get(); }
    const GLFWPlatformWindow* GetWindow() const { return m_Window.get(); }

private:
    std::unique_ptr<GLFWPlatformWindow> m_Window;
    bool m_IsInitialized;
};

}} // namespace VGE::UI