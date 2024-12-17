#pragma once

#include "Runtime/Core/Public/Core/UICore.h"
#include "Runtime/Core/Public/Platform/UIWindow.h"
#include "Runtime/Core/Public/Platform/UIWindowDesc.h"
#include <memory>
#include <string>
#include <vector>
#include <functional>

namespace VGE {
namespace UI {

// Forward declarations
class UIWindow;
class UIRenderer;

// Platform events
struct UIPlatformEvent {
    enum class Type {
        None,
        WindowClose,
        WindowResize,
        WindowFocus,
        WindowLostFocus,
        WindowMoved
    };

    Type EventType = Type::None;
    UIWindow* Window = nullptr;
    union {
        struct {
            int Width;
            int Height;
        } Resize;
        struct {
            int X;
            int Y;
        } Move;
    };
};

// Platform event callback
using UIPlatformEventCallback = std::function<void(const UIPlatformEvent&)>;

class UI_API UIPlatform {
public:
    static UIPlatform& Get();

    // Initialization
    bool Initialize();
    void Shutdown();

    // Window management
    std::shared_ptr<UIWindow> CreateWindow(const UIWindowDesc& desc);
    void DestroyWindow(std::shared_ptr<UIWindow> window);
    
    // Event handling
    void PollEvents();
    void AddEventCallback(UIPlatformEventCallback callback);
    void RemoveEventCallback(UIPlatformEventCallback callback);

    // OpenGL context
    bool MakeContextCurrent(UIWindow* window);
    void SwapBuffers(UIWindow* window);
    void SetVSync(bool enabled);

    // Platform information
    bool IsInitialized() const { return m_Initialized; }
    const std::vector<std::shared_ptr<UIWindow>>& GetWindows() const { return m_Windows; }
    std::shared_ptr<UIWindow> GetMainWindow() const { return m_MainWindow; }
    
    // Error handling
    const std::string& GetLastError() const { return m_LastError; }

private:
    UIPlatform() = default;
    ~UIPlatform() = default;
    
    // Prevent copying
    UIPlatform(const UIPlatform&) = delete;
    UIPlatform& operator=(const UIPlatform&) = delete;

    // Internal initialization
    bool InitializeGLFW();
    void SetupErrorCallback();
    
    // Internal event handling
    void DispatchEvent(const UIPlatformEvent& event);
    static void GLFWErrorCallback(int error, const char* description);

    // State
    bool m_Initialized = false;
    std::string m_LastError;
    std::vector<std::shared_ptr<UIWindow>> m_Windows;
    std::shared_ptr<UIWindow> m_MainWindow;
    std::vector<UIPlatformEventCallback> m_EventCallbacks;
};

}} // namespace VGE::UI
