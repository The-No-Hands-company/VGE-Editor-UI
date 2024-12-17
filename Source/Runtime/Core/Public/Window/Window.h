#pragma once

#include "Runtime/Core/Public/Core/UICore.h"
#include "Runtime/Core/Public/Platform/PlatformWindow.h"
#include "Runtime/Core/Public/Renderer/UIRenderContext.h"
#include <memory>
#include <string>
#include <functional>

namespace VGE {
namespace Core {

// Forward declarations
class RenderContext;
class InputManager;

class Window : public Platform::PlatformWindow {
public:
    Window(const Platform::WindowDesc& desc);
    virtual ~Window() = default;

    // Rendering
    virtual void BeginFrame();
    virtual void EndFrame();
    virtual void Present();
    RenderContext* GetRenderContext() { return m_RenderContext.get(); }

    // Input handling
    InputManager* GetInputManager() { return m_InputManager.get(); }
    virtual void ProcessInput();

    // Event handling
    using RenderCallback = std::function<void()>;
    using UpdateCallback = std::function<void(float)>;
    void SetRenderCallback(RenderCallback callback) { m_RenderCallback = callback; }
    void SetUpdateCallback(UpdateCallback callback) { m_UpdateCallback = callback; }

protected:
    // Platform event handlers
    virtual void OnWindowResized(const glm::vec2& size);
    virtual void OnWindowMoved(const glm::vec2& position);
    virtual void OnWindowFocusChanged(bool focused);
    virtual void OnWindowMinimized(bool minimized);
    virtual void OnWindowMaximized(bool maximized);
    virtual void OnWindowClosed();

    // Internal helpers
    virtual void CreateRenderContext();
    virtual void DestroyRenderContext();
    virtual void UpdateRenderContext();

private:
    std::unique_ptr<RenderContext> m_RenderContext;
    std::unique_ptr<InputManager> m_InputManager;
    RenderCallback m_RenderCallback;
    UpdateCallback m_UpdateCallback;
    bool m_ShouldClose = false;
};

}} // namespace VGE::Core 