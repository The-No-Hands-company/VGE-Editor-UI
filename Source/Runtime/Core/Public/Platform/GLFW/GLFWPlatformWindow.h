#pragma once

#include "Core/UICore.h"
#include "Platform/PlatformWindow.h"
#include "Platform/UIWindowEvent.h"
#include <functional>
#include <string>
#include <vector>

struct GLFWwindow;

namespace VGE {
namespace UI {

class GLFWPlatformWindow : public Platform::PlatformWindow {
public:
    explicit GLFWPlatformWindow(const Platform::WindowDesc& desc);
    ~GLFWPlatformWindow() override;

    void PollEvents();
    void SwapBuffers();
    bool ShouldClose() const;

    void SetVSync(bool enabled);
    void SetResizable(bool resizable);
    void SetDecorated(bool decorated);
    void SetFloating(bool floating);
    void SetMaximized(bool maximized);
    void SetVisible(bool visible);
    void SetFocused(bool focused);
    void SetShouldClose(bool shouldClose);

    void AddEventCallback(WindowEventCallback callback);
    void RemoveEventCallback(WindowEventCallback callback);

    GLFWwindow* GetNativeHandle() const { return m_Window; }

protected:
    // Platform::PlatformWindow interface
    void PlatformShow() override;
    void PlatformHide() override;
    void PlatformSetPosition(const glm::vec2& position) override;
    void PlatformSetSize(const glm::vec2& size) override;
    void PlatformSetTitle(const std::string& title) override;
    void PlatformFocus() override;
    void PlatformMaximize() override;
    void PlatformMinimize() override;
    void PlatformRestore() override;
    void PlatformClose() override;

private:
    void DispatchEvent(const WindowEvent& event);
    static void GLFWWindowSizeCallback(GLFWwindow* window, int width, int height);
    static void GLFWWindowPosCallback(GLFWwindow* window, int x, int y);
    static void GLFWWindowCloseCallback(GLFWwindow* window);
    static void GLFWWindowFocusCallback(GLFWwindow* window, int focused);
    static void GLFWCursorEnterCallback(GLFWwindow* window, int entered);
    static void GLFWCursorPosCallback(GLFWwindow* window, double x, double y);
    static void GLFWMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void GLFWScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    static void GLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void GLFWCharCallback(GLFWwindow* window, unsigned int codepoint);

private:
    GLFWwindow* m_Window;
    std::vector<WindowEventCallback> m_EventCallbacks;
};

}} // namespace VGE::UI 