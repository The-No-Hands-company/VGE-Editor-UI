#include "Platform/GLFW/GLFWPlatformWindow.h"
#include "Core/Logger.h"
#include "Core/UITypes.h"
#include <glad/gl.h>
#include <GLFW/glfw3.h>

namespace VGE {
namespace UI {

GLFWPlatformWindow::GLFWPlatformWindow(const Platform::WindowDesc& desc)
    : Platform::PlatformWindow(desc)
    , m_Window(nullptr)
{
    if (!glfwInit())
    {
        Logger::Error("Failed to initialize GLFW");
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, desc.resizable ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_DECORATED, desc.decorated ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_FLOATING, desc.floating ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_MAXIMIZED, desc.maximized ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_VISIBLE, desc.visible ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_FOCUSED, true);
    glfwWindowHint(GLFW_FOCUS_ON_SHOW, true);
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, true);
    glfwWindowHint(GLFW_CENTER_CURSOR, true);

    m_Window = glfwCreateWindow(
        static_cast<int>(desc.size.x),
        static_cast<int>(desc.size.y),
        desc.title.c_str(),
        nullptr,
        nullptr
    );

    if (!m_Window)
    {
        Logger::Error("Failed to create GLFW window");
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(m_Window);
    glfwSetWindowUserPointer(m_Window, this);

    // Set up GLFW callbacks
    glfwSetWindowSizeCallback(m_Window, GLFWWindowSizeCallback);
    glfwSetWindowPosCallback(m_Window, GLFWWindowPosCallback);
    glfwSetWindowCloseCallback(m_Window, GLFWWindowCloseCallback);
    glfwSetWindowFocusCallback(m_Window, GLFWWindowFocusCallback);
    glfwSetCursorEnterCallback(m_Window, GLFWCursorEnterCallback);
    glfwSetCursorPosCallback(m_Window, GLFWCursorPosCallback);
    glfwSetMouseButtonCallback(m_Window, GLFWMouseButtonCallback);
    glfwSetScrollCallback(m_Window, GLFWScrollCallback);
    glfwSetKeyCallback(m_Window, GLFWKeyCallback);
    glfwSetCharCallback(m_Window, GLFWCharCallback);

    // Initialize window position and size
    int x, y;
    glfwGetWindowPos(m_Window, &x, &y);
    m_Position = glm::vec2(x, y);

    int width, height;
    glfwGetWindowSize(m_Window, &width, &height);
    m_Size = glm::vec2(width, height);

    // Dispatch created event
    WindowEvent event;
    event.type = WindowEventType::Created;
    event.windowTitle = desc.title;
    event.position = m_Position;
    event.size = m_Size;
    DispatchEvent(event);
}

GLFWPlatformWindow::~GLFWPlatformWindow()
{
    if (m_Window)
    {
        WindowEvent event;
        event.type = WindowEventType::Destroyed;
        event.windowTitle = m_Title;
        DispatchEvent(event);

        glfwDestroyWindow(m_Window);
        m_Window = nullptr;
    }
}

void GLFWPlatformWindow::PollEvents()
{
    glfwPollEvents();
}

void GLFWPlatformWindow::SwapBuffers()
{
    if (m_Window)
    {
        glfwSwapBuffers(m_Window);
    }
}

bool GLFWPlatformWindow::ShouldClose() const
{
    return m_Window ? glfwWindowShouldClose(m_Window) : true;
}

void GLFWPlatformWindow::SetVSync(bool enabled)
{
    if (m_Window)
    {
        glfwSwapInterval(enabled ? 1 : 0);
    }
}

void GLFWPlatformWindow::SetResizable(bool resizable)
{
    if (m_Window)
    {
        glfwSetWindowAttrib(m_Window, GLFW_RESIZABLE, resizable ? GLFW_TRUE : GLFW_FALSE);
        m_Resizable = resizable;
    }
}

void GLFWPlatformWindow::SetDecorated(bool decorated)
{
    if (m_Window)
    {
        glfwSetWindowAttrib(m_Window, GLFW_DECORATED, decorated ? GLFW_TRUE : GLFW_FALSE);
        m_Decorated = decorated;
    }
}

void GLFWPlatformWindow::SetFloating(bool floating)
{
    if (m_Window)
    {
        glfwSetWindowAttrib(m_Window, GLFW_FLOATING, floating ? GLFW_TRUE : GLFW_FALSE);
        m_Floating = floating;
    }
}

void GLFWPlatformWindow::SetMaximized(bool maximized)
{
    if (m_Window)
    {
        if (maximized)
            glfwMaximizeWindow(m_Window);
        else
            glfwRestoreWindow(m_Window);
        m_Maximized = maximized;
    }
}

void GLFWPlatformWindow::SetVisible(bool visible)
{
    if (m_Window)
    {
        if (visible)
            glfwShowWindow(m_Window);
        else
            glfwHideWindow(m_Window);
        m_Visible = visible;
    }
}

void GLFWPlatformWindow::SetFocused(bool focused)
{
    if (m_Window && focused)
    {
        glfwFocusWindow(m_Window);
        m_Focused = focused;
    }
}

void GLFWPlatformWindow::SetShouldClose(bool shouldClose)
{
    if (m_Window)
    {
        glfwSetWindowShouldClose(m_Window, shouldClose ? GLFW_TRUE : GLFW_FALSE);
    }
}

void GLFWPlatformWindow::AddEventCallback(WindowEventCallback callback)
{
    m_EventCallbacks.push_back(callback);
}

void GLFWPlatformWindow::RemoveEventCallback(WindowEventCallback callback)
{
    m_EventCallbacks.erase(
        std::remove_if(m_EventCallbacks.begin(), m_EventCallbacks.end(),
            [&](const WindowEventCallback& cb) { return cb.target_type() == callback.target_type(); }),
        m_EventCallbacks.end()
    );
}

// Protected Platform::PlatformWindow interface implementation
void GLFWPlatformWindow::PlatformShow()
{
    SetVisible(true);
}

void GLFWPlatformWindow::PlatformHide()
{
    SetVisible(false);
}

void GLFWPlatformWindow::PlatformSetPosition(const glm::vec2& position)
{
    if (m_Window)
    {
        glfwSetWindowPos(m_Window, static_cast<int>(position.x), static_cast<int>(position.y));
        m_Position = position;

        WindowEvent event;
        event.type = WindowEventType::Moved;
        event.windowTitle = m_Title;
        event.position = position;
        DispatchEvent(event);
    }
}

void GLFWPlatformWindow::PlatformSetSize(const glm::vec2& size)
{
    if (m_Window)
    {
        glfwSetWindowSize(m_Window, static_cast<int>(size.x), static_cast<int>(size.y));
        m_Size = size;

        WindowEvent event;
        event.type = WindowEventType::Resized;
        event.windowTitle = m_Title;
        event.size = size;
        DispatchEvent(event);
    }
}

void GLFWPlatformWindow::PlatformSetTitle(const std::string& title)
{
    if (m_Window)
    {
        glfwSetWindowTitle(m_Window, title.c_str());
        m_Title = title;
    }
}

void GLFWPlatformWindow::PlatformFocus()
{
    SetFocused(true);
}

void GLFWPlatformWindow::PlatformMaximize()
{
    SetMaximized(true);
}

void GLFWPlatformWindow::PlatformMinimize()
{
    if (m_Window)
    {
        glfwIconifyWindow(m_Window);
        m_Minimized = true;

        WindowEvent event;
        event.type = WindowEventType::Minimized;
        event.windowTitle = m_Title;
        DispatchEvent(event);
    }
}

void GLFWPlatformWindow::PlatformRestore()
{
    if (m_Window)
    {
        glfwRestoreWindow(m_Window);
        m_Minimized = false;
        m_Maximized = false;

        WindowEvent event;
        event.type = WindowEventType::Restored;
        event.windowTitle = m_Title;
        DispatchEvent(event);
    }
}

void GLFWPlatformWindow::PlatformClose()
{
    SetShouldClose(true);
}

// Private methods
void GLFWPlatformWindow::DispatchEvent(const WindowEvent& event)
{
    for (const auto& callback : m_EventCallbacks)
    {
        callback(event);
        if (event.handled) break;
    }
}

// Static GLFW callback implementations
void GLFWPlatformWindow::GLFWWindowSizeCallback(GLFWwindow* window, int width, int height)
{
    auto* platformWindow = static_cast<GLFWPlatformWindow*>(glfwGetWindowUserPointer(window));
    if (platformWindow)
    {
        WindowEvent event;
        event.type = WindowEventType::Resized;
        event.windowTitle = platformWindow->GetTitle();
        event.size = glm::vec2(width, height);
        platformWindow->DispatchEvent(event);
    }
}

void GLFWPlatformWindow::GLFWWindowPosCallback(GLFWwindow* window, int x, int y)
{
    auto* platformWindow = static_cast<GLFWPlatformWindow*>(glfwGetWindowUserPointer(window));
    if (platformWindow)
    {
        WindowEvent event;
        event.type = WindowEventType::Moved;
        event.windowTitle = platformWindow->GetTitle();
        event.position = glm::vec2(x, y);
        platformWindow->DispatchEvent(event);
    }
}

void GLFWPlatformWindow::GLFWWindowCloseCallback(GLFWwindow* window)
{
    auto* platformWindow = static_cast<GLFWPlatformWindow*>(glfwGetWindowUserPointer(window));
    if (platformWindow)
    {
        WindowEvent event;
        event.type = WindowEventType::Close;
        event.windowTitle = platformWindow->GetTitle();
        platformWindow->DispatchEvent(event);
    }
}

void GLFWPlatformWindow::GLFWWindowFocusCallback(GLFWwindow* window, int focused)
{
    auto* platformWindow = static_cast<GLFWPlatformWindow*>(glfwGetWindowUserPointer(window));
    if (platformWindow)
    {
        WindowEvent event;
        event.type = focused ? WindowEventType::FocusGained : WindowEventType::FocusLost;
        event.windowTitle = platformWindow->GetTitle();
        platformWindow->DispatchEvent(event);
    }
}

void GLFWPlatformWindow::GLFWCursorEnterCallback(GLFWwindow* window, int entered)
{
    auto* platformWindow = static_cast<GLFWPlatformWindow*>(glfwGetWindowUserPointer(window));
    if (platformWindow)
    {
        WindowEvent event;
        event.type = entered ? WindowEventType::MouseEnter : WindowEventType::MouseLeave;
        event.windowTitle = platformWindow->GetTitle();
        platformWindow->DispatchEvent(event);
    }
}

void GLFWPlatformWindow::GLFWCursorPosCallback(GLFWwindow* window, double x, double y)
{
    auto* platformWindow = static_cast<GLFWPlatformWindow*>(glfwGetWindowUserPointer(window));
    if (platformWindow)
    {
        WindowEvent event;
        event.type = WindowEventType::MouseMove;
        event.windowTitle = platformWindow->GetTitle();
        event.mousePosition = glm::vec2(x, y);
        platformWindow->DispatchEvent(event);
    }
}

void GLFWPlatformWindow::GLFWMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    auto* platformWindow = static_cast<GLFWPlatformWindow*>(glfwGetWindowUserPointer(window));
    if (platformWindow)
    {
        WindowEvent event;
        event.type = WindowEventType::MouseButton;
        event.windowTitle = platformWindow->GetTitle();
        event.mouseButton = button;
        event.modifiers = mods;
        
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        event.mousePosition = glm::vec2(x, y);
        
        platformWindow->DispatchEvent(event);
    }
}

void GLFWPlatformWindow::GLFWScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    auto* platformWindow = static_cast<GLFWPlatformWindow*>(glfwGetWindowUserPointer(window));
    if (platformWindow)
    {
        WindowEvent event;
        event.type = WindowEventType::MouseScroll;
        event.windowTitle = platformWindow->GetTitle();
        event.scrollX = static_cast<float>(xoffset);
        event.scrollY = static_cast<float>(yoffset);
        
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        event.mousePosition = glm::vec2(x, y);
        
        platformWindow->DispatchEvent(event);
    }
}

void GLFWPlatformWindow::GLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    auto* platformWindow = static_cast<GLFWPlatformWindow*>(glfwGetWindowUserPointer(window));
    if (platformWindow)
    {
        WindowEvent event;
        event.type = action == GLFW_PRESS ? WindowEventType::KeyPress : WindowEventType::KeyRelease;
        event.windowTitle = platformWindow->GetTitle();
        event.keyCode = key;
        event.scanCode = scancode;
        event.modifiers = mods;
        platformWindow->DispatchEvent(event);
    }
}

void GLFWPlatformWindow::GLFWCharCallback(GLFWwindow* window, unsigned int codepoint)
{
    auto* platformWindow = static_cast<GLFWPlatformWindow*>(glfwGetWindowUserPointer(window));
    if (platformWindow)
    {
        WindowEvent event;
        event.type = WindowEventType::Char;
        event.windowTitle = platformWindow->GetTitle();
        event.charCode = codepoint;
        platformWindow->DispatchEvent(event);
    }
}

}} // namespace VGE::UI