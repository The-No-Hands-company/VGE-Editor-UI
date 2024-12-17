#include "Platform/UIWindow.h"
#include "Core/Logger.h"
#include "Core/UITypes.h"
#include "Platform/GLFW/GLFWPlatformWindow.h"
#include <glad/gl.h>
#include <GLFW/glfw3.h>

namespace VGE {
namespace UI {

UIWindow::UIWindow(const UIWindowDesc& desc)
    : m_Width(static_cast<int>(desc.width))
    , m_Height(static_cast<int>(desc.height))
{
    Platform::WindowDesc platformDesc;
    platformDesc.title = desc.title;
    platformDesc.size = glm::vec2(desc.width, desc.height);
    platformDesc.resizable = desc.resizable;
    platformDesc.decorated = desc.decorated;
    platformDesc.floating = desc.floating;
    platformDesc.maximized = desc.maximized;
    platformDesc.minimized = desc.minimized;
    platformDesc.visible = desc.visible;
    platformDesc.fullscreen = desc.fullscreen;

    m_PlatformWindow = std::make_unique<GLFWPlatformWindow>(platformDesc);
    if (!m_PlatformWindow)
    {
        Logger::Error("Failed to create platform window");
        return;
    }

    // Set up callbacks
    m_PlatformWindow->SetOnResize([this](int width, int height) {
        OnResize(width, height);
    });
}

UIWindow::UIWindow(const std::string& title, int width, int height)
    : m_Width(width)
    , m_Height(height)
{
    Platform::WindowDesc desc;
    desc.title = title;
    desc.size = glm::vec2(width, height);
    desc.resizable = true;
    desc.decorated = true;
    desc.visible = true;

    m_PlatformWindow = std::make_unique<GLFWPlatformWindow>(desc);
    if (!m_PlatformWindow)
    {
        Logger::Error("Failed to create platform window");
        return;
    }

    // Set up callbacks
    m_PlatformWindow->SetOnResize([this](int width, int height) {
        OnResize(width, height);
    });
}

UIWindow::~UIWindow()
{
    m_PlatformWindow.reset();
}

void UIWindow::Update()
{
    if (m_PlatformWindow)
    {
        m_PlatformWindow->PollEvents();
        m_PlatformWindow->SwapBuffers();
    }
}

void UIWindow::Clear()
{
    if (m_PlatformWindow)
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
}

void UIWindow::SetClearColor(float r, float g, float b, float a)
{
    if (m_PlatformWindow)
    {
        glClearColor(r, g, b, a);
    }
}

bool UIWindow::ShouldClose() const
{
    return !m_PlatformWindow || m_PlatformWindow->ShouldClose();
}

void UIWindow::OnResize(int width, int height)
{
    m_Width = width;
    m_Height = height;
    glViewport(0, 0, width, height);
}

}  // namespace UI
}  // namespace VGE
