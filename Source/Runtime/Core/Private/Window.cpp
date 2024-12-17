#include <glad/gl.h>
#include "Runtime/Core/Public/Core/UICore.h"
#include "Runtime/Core/Public/Platform/Window.h"
#include <GLFW/glfw3.h>
#include <iostream>

namespace VGE {
namespace UI {

Window::Window(const std::string& title, int width, int height)
    : m_Window(nullptr)
    , m_Title(title)
    , m_Width(width)
    , m_Height(height)
{
}

Window::~Window() {
    Shutdown();
}

bool Window::Initialize() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }

    // Get primary monitor and its video mode
    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
    
    // Set window hints for maximized state
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);  // Start hidden

    // Create window with monitor's resolution
    m_Width = mode->width;
    m_Height = mode->height;
    m_Window = glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), nullptr, nullptr);
    if (!m_Window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(m_Window);
    glfwSetWindowUserPointer(m_Window, this);
    
    // Set up callbacks
    glfwSetFramebufferSizeCallback(m_Window, GLFWWindowResizeCallback);
    glfwSetCursorPosCallback(m_Window, GLFWCursorPosCallback);
    glfwSetMouseButtonCallback(m_Window, GLFWMouseButtonCallback);
    glfwSetKeyCallback(m_Window, GLFWKeyCallback);
    glfwSetWindowMaximizeCallback(m_Window, GLFWWindowMaximizeCallback);

    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwDestroyWindow(m_Window);
        glfwTerminate();
        return false;
    }

    // Show window after initialization
    glfwShowWindow(m_Window);

    // Force a resize event to update UI
    int fbWidth, fbHeight;
    glfwGetFramebufferSize(m_Window, &fbWidth, &fbHeight);
    GLFWWindowResizeCallback(m_Window, fbWidth, fbHeight);

    return true;
}

void Window::Shutdown() {
    if (m_Window) {
        glfwDestroyWindow(m_Window);
        m_Window = nullptr;
    }
    glfwTerminate();
}

void Window::PollEvents() {
    glfwPollEvents();
}

void Window::SwapBuffers() {
    glfwSwapBuffers(m_Window);
}

bool Window::ShouldClose() const {
    return glfwWindowShouldClose(m_Window);
}

void Window::SetShouldClose(bool shouldClose) {
    glfwSetWindowShouldClose(m_Window, shouldClose);
}

void Window::GLFWWindowResizeCallback(GLFWwindow* window, int width, int height) {
    Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (self) {
        self->m_Width = width;
        self->m_Height = height;
        if (self->m_ResizeCallback) {
            self->m_ResizeCallback(width, height);
        }
    }
}

void Window::GLFWWindowMaximizeCallback(GLFWwindow* window, int maximized) {
    Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (self) {
        // Get current framebuffer size
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        
        // Force a resize event
        if (self->m_ResizeCallback) {
            self->m_ResizeCallback(width, height);
        }
    }
}

void Window::GLFWCursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (self && self->m_MouseMoveCallback) {
        self->m_MouseMoveCallback(xpos, ypos);
    }
}

void Window::GLFWMouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (self) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        
        if (action == GLFW_PRESS && self->m_MouseDownCallback) {
            self->m_MouseDownCallback(xpos, ypos, button);
        }
        else if (action == GLFW_RELEASE && self->m_MouseUpCallback) {
            self->m_MouseUpCallback(xpos, ypos, button);
        }
    }
}

void Window::GLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (self) {
        if (action == GLFW_PRESS && self->m_KeyDownCallback) {
            self->m_KeyDownCallback(key);
        }
        else if (action == GLFW_RELEASE && self->m_KeyUpCallback) {
            self->m_KeyUpCallback(key);
        }
    }
}

} // namespace UI
} // namespace VGE



