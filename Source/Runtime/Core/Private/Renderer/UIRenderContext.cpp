#include "Renderer/UIRenderContext.h"
#include "Core/Logger.h"
#include <glad/gl.h>
#include <GLFW/glfw3.h>

namespace VGE {
namespace UI {

UIRenderContext::UIRenderContext()
    : m_IsInitialized(false)
{
}

UIRenderContext::~UIRenderContext()
{
    m_Window.reset();
    m_IsInitialized = false;
}

bool UIRenderContext::Initialize(const WindowDesc& desc)
{
    if (m_IsInitialized)
    {
        Logger::Warning("Render context already initialized");
        return false;
    }

    if (!glfwInit())
    {
        Logger::Error("Failed to initialize GLFW");
        return false;
    }

    m_Window = std::make_unique<GLFWPlatformWindow>(desc);
    if (!m_Window)
    {
        Logger::Error("Failed to create GLFW window");
        return false;
    }

    m_IsInitialized = true;
    return true;
}

void UIRenderContext::MakeCurrent()
{
    if (m_IsInitialized && m_Window)
    {
        // GLFWPlatformWindow handles making the context current
    }
}

void UIRenderContext::SwapBuffers()
{
    if (m_IsInitialized && m_Window)
    {
        m_Window->SwapBuffers();
    }
}

void UIRenderContext::PollEvents()
{
    if (m_IsInitialized)
    {
        m_Window->PollEvents();
    }
}

bool UIRenderContext::ShouldClose() const
{
    return !m_IsInitialized || (m_Window && m_Window->ShouldClose());
}

void UIRenderContext::SetVSync(bool enabled)
{
    if (m_IsInitialized && m_Window)
    {
        m_Window->SetVSync(enabled);
    }
}

void UIRenderContext::SetTitle(const std::string& title)
{
    if (m_IsInitialized && m_Window)
    {
        m_Window->SetTitle(title);
    }
}

void UIRenderContext::SetSize(uint32_t width, uint32_t height)
{
    if (m_IsInitialized && m_Window)
    {
        m_Window->SetSize(width, height);
    }
}

void UIRenderContext::SetPosition(int x, int y)
{
    if (m_IsInitialized && m_Window)
    {
        m_Window->SetPosition(x, y);
    }
}

void UIRenderContext::SetResizable(bool resizable)
{
    if (m_IsInitialized && m_Window)
    {
        m_Window->SetResizable(resizable);
    }
}

void UIRenderContext::SetDecorated(bool decorated)
{
    if (m_IsInitialized && m_Window)
    {
        m_Window->SetDecorated(decorated);
    }
}

void UIRenderContext::SetFloating(bool floating)
{
    if (m_IsInitialized && m_Window)
    {
        m_Window->SetFloating(floating);
    }
}

void UIRenderContext::SetMaximized(bool maximized)
{
    if (m_IsInitialized && m_Window)
    {
        m_Window->SetMaximized(maximized);
    }
}

void UIRenderContext::SetVisible(bool visible)
{
    if (m_IsInitialized && m_Window)
    {
        m_Window->SetVisible(visible);
    }
}

void UIRenderContext::SetFocused(bool focused)
{
    if (m_IsInitialized && m_Window)
    {
        m_Window->SetFocused(focused);
    }
}

void UIRenderContext::SetShouldClose(bool shouldClose)
{
    if (m_IsInitialized && m_Window)
    {
        m_Window->SetShouldClose(shouldClose);
    }
}

}} // namespace VGE::UI