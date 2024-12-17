#include "Runtime/Core/Public/Core/Application.h"
#include "Runtime/Core/Public/Core/UICore.h"
#include <chrono>
#include <thread>

namespace VGE {
namespace UI {

Application::Application(const CreateInfo& createInfo)
    : m_Running(false)
{
    m_Window = std::make_unique<Window>(createInfo.title, createInfo.width, createInfo.height);
    if (!m_Window->Initialize()) {
        throw std::runtime_error("Failed to initialize window");
    }

    // Set up callbacks
    m_Window->SetResizeCallback([this](int width, int height) {
        OnResize(width, height);
    });

    m_Window->SetMouseMoveCallback([this](double xpos, double ypos) {
        OnMouseMove(xpos, ypos);
    });

    m_Window->SetMouseDownCallback([this](double xpos, double ypos, int button) {
        OnMouseDown(xpos, ypos, button);
    });

    m_Window->SetMouseUpCallback([this](double xpos, double ypos, int button) {
        OnMouseUp(xpos, ypos, button);
    });

    m_Window->SetKeyDownCallback([this](int key) {
        OnKeyDown(key);
    });

    m_Window->SetKeyUpCallback([this](int key) {
        OnKeyUp(key);
    });
}

Application::~Application() {
    Stop();
}

void Application::Run() {
    m_Running = true;

    auto lastTime = std::chrono::high_resolution_clock::now();

    while (m_Running && !m_Window->ShouldClose()) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;

        OnUpdate(deltaTime);
        OnRender();

        m_Window->SwapBuffers();
        m_Window->PollEvents();
    }
}

void Application::Stop() {
    m_Running = false;
    m_Window.reset();
}

} // namespace UI
} // namespace VGE



