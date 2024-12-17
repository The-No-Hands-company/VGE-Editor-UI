#pragma once

#include "Runtime/Core/Public/Core/UICore.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <functional>
#include <string>

namespace VGE {
namespace UI {

class Window {
public:
    using ResizeCallback = std::function<void(int, int)>;
    using MouseMoveCallback = std::function<void(double, double)>;
    using MouseButtonCallback = std::function<void(double, double, int)>;
    using KeyCallback = std::function<void(int)>;

    Window(const std::string& title = "VersaUI", int width = 1280, int height = 720);
    ~Window();

    bool Initialize();
    void Shutdown();

    void PollEvents();
    void SwapBuffers();

    bool ShouldClose() const;
    void SetShouldClose(bool shouldClose);

    int GetWidth() const { return m_Width; }
    int GetHeight() const { return m_Height; }
    GLFWwindow* GetGLFWWindow() const { return m_Window; }

    void SetResizeCallback(const ResizeCallback& callback) { m_ResizeCallback = callback; }
    void SetMouseMoveCallback(const MouseMoveCallback& callback) { m_MouseMoveCallback = callback; }
    void SetMouseDownCallback(const MouseButtonCallback& callback) { m_MouseDownCallback = callback; }
    void SetMouseUpCallback(const MouseButtonCallback& callback) { m_MouseUpCallback = callback; }
    void SetKeyDownCallback(const KeyCallback& callback) { m_KeyDownCallback = callback; }
    void SetKeyUpCallback(const KeyCallback& callback) { m_KeyUpCallback = callback; }

private:
    static void GLFWWindowResizeCallback(GLFWwindow* window, int width, int height);
    static void GLFWWindowMaximizeCallback(GLFWwindow* window, int maximized);
    static void GLFWCursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    static void GLFWMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void GLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

    GLFWwindow* m_Window;
    std::string m_Title;
    int m_Width;
    int m_Height;

    ResizeCallback m_ResizeCallback;
    MouseMoveCallback m_MouseMoveCallback;
    MouseButtonCallback m_MouseDownCallback;
    MouseButtonCallback m_MouseUpCallback;
    KeyCallback m_KeyDownCallback;
    KeyCallback m_KeyUpCallback;
};

} // namespace UI
} // namespace VGE



