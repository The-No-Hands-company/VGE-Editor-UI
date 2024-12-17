#pragma once

#include "Runtime/Core/Public/Core/UICore.h"

#include "Runtime/Core/Public/Platform/Window.h"
#include <memory>
#include <string>

namespace VGE {
namespace UI {

class Application {
public:
    struct CreateInfo {
        std::string title = "VersaUI";
        int width = 1280;
        int height = 720;
        bool vsync = true;
    };

    Application(const CreateInfo& createInfo = CreateInfo());
    virtual ~Application();

    void Run();
    void Stop();

    Window* GetWindow() const { return m_Window.get(); }
    GLFWwindow* GetGLFWWindow() const { return m_Window ? m_Window->GetGLFWWindow() : nullptr; }

protected:
    virtual void OnUpdate(float deltaTime) {}
    virtual void OnRender() {}
    virtual void OnResize(int width, int height) {}
    virtual void OnMouseMove(double xpos, double ypos) {}
    virtual void OnMouseDown(double xpos, double ypos, int button) {}
    virtual void OnMouseUp(double xpos, double ypos, int button) {}
    virtual void OnKeyDown(int key) {}
    virtual void OnKeyUp(int key) {}

private:
    std::unique_ptr<Window> m_Window;
    bool m_Running;
};

} // namespace UI
} // namespace VGE



