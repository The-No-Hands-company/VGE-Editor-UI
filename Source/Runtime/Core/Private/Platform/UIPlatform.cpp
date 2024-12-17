#include "Platform/UIPlatform.h"
#include "Platform/UIWindow.h"
#include "Core/Logger.h"
#include <GLFW/glfw3.h>
#include <iostream>

namespace VGE {
namespace UI {

UIPlatform& UIPlatform::Get() {
    static UIPlatform instance;
    return instance;
}

bool UIPlatform::Initialize() {
    if (m_Initialized) {
        m_LastError = "Platform already initialized";
        return false;
    }

    if (!InitializeGLFW()) {
        return false;
    }

    SetupErrorCallback();
    m_Initialized = true;
    return true;
}

void UIPlatform::Shutdown() {
    if (!m_Initialized) return;

    // Destroy all windows
    m_Windows.clear();
    m_MainWindow.reset();

    // Terminate GLFW
    glfwTerminate();
    m_Initialized = false;
}

bool UIPlatform::InitializeGLFW() {
    // Set error callback first
    glfwSetErrorCallback(GLFWErrorCallback);

    if (!glfwInit()) {
        m_LastError = "Failed to initialize GLFW";
        return false;
    }

    // Set OpenGL hints
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif

    return true;
}

void UIPlatform::SetupErrorCallback() {
    // Already set in InitializeGLFW
}

std::shared_ptr<UIWindow> UIPlatform::CreateWindow(const UIWindowDesc& desc) {
    if (!m_Initialized) {
        m_LastError = "Platform not initialized";
        return nullptr;
    }

    // Create the window
    auto window = std::make_shared<UIWindow>(desc);
    if (!window->GetNativeHandle()) {
        m_LastError = "Failed to create window";
        return nullptr;
    }

    // Store the window
    m_Windows.push_back(window);

    // Set as main window if it's the first one
    if (!m_MainWindow) {
        m_MainWindow = window;
    }

    return window;
}

void UIPlatform::DestroyWindow(std::shared_ptr<UIWindow> window) {
    if (!window) return;

    // Remove from windows list
    auto it = std::find(m_Windows.begin(), m_Windows.end(), window);
    if (it != m_Windows.end()) {
        m_Windows.erase(it);
    }

    // Clear main window if this was it
    if (m_MainWindow == window) {
        m_MainWindow = m_Windows.empty() ? nullptr : m_Windows[0];
    }
}

void UIPlatform::PollEvents() {
    if (!m_Initialized) return;
    glfwPollEvents();
}

void UIPlatform::AddEventCallback(UIPlatformEventCallback callback) {
    if (callback) {
        m_EventCallbacks.push_back(std::move(callback));
    }
}

void UIPlatform::RemoveEventCallback(UIPlatformEventCallback callback) {
    // Note: This is a simple implementation. In production, you might want a better way to identify callbacks
    auto it = std::find_if(m_EventCallbacks.begin(), m_EventCallbacks.end(),
        [&callback](const UIPlatformEventCallback& cb) {
            return cb.target_type() == callback.target_type();
        });
    
    if (it != m_EventCallbacks.end()) {
        m_EventCallbacks.erase(it);
    }
}

bool UIPlatform::MakeContextCurrent(UIWindow* window) {
    if (!m_Initialized) return false;

    GLFWwindow* handle = window ? window->GetNativeHandle() : nullptr;
    glfwMakeContextCurrent(handle);

    // Initialize GLAD if this is the first time making a context current
    static bool gladInitialized = false;
    if (handle && !gladInitialized) {
        if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
            m_LastError = "Failed to initialize GLAD";
            return false;
        }
        gladInitialized = true;
    }

    return true;
}

void UIPlatform::SwapBuffers(UIWindow* window) {
    if (!window) return;
    glfwSwapBuffers(window->GetNativeHandle());
}

void UIPlatform::SetVSync(bool enabled) {
    glfwSwapInterval(enabled ? 1 : 0);
}

void UIPlatform::DispatchEvent(const UIPlatformEvent& event) {
    for (const auto& callback : m_EventCallbacks) {
        callback(event);
    }
}

void UIPlatform::GLFWErrorCallback(int error, const char* description) {
    std::string errorMsg = std::string("GLFW Error ") + std::to_string(error) + ": " + description;
    UIPlatform::Get().m_LastError = errorMsg;
    std::cerr << "GLFW Error [" << error << "]: " << description << std::endl;
}

}} // namespace VGE::UI 


