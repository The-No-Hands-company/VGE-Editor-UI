#include "Window/UIWindow.h"
#include "Core/Logger.h"
#include "Platform/GLFW/GLFWPlatformWindow.h"

namespace VGE {
namespace UI {

UIWindow::UIWindow(const std::string& title, WindowType type)
    : m_Title(title)
    , m_Type(type)
{
    m_Decorations = std::make_unique<UIWindowDecorations>(this);
    m_RenderContext = std::make_unique<UIRenderContext>();
    m_RenderPrimitives = std::make_unique<UIRenderPrimitives>();
    CreatePlatformWindow();
    UpdateDecorations();
    InitializeRenderContext();
}

UIWindow::~UIWindow() {
    m_RenderPrimitives.reset();
    m_RenderContext.reset();
    DestroyPlatformWindow();
}

bool UIWindow::InitializeRenderContext() {
    if (!m_RenderContext) {
        Logger::Log(LogLevel::Error, "No render context created");
        return false;
    }

#ifdef UI_PLATFORM_WINDOWS
    if (!m_RenderContext->Initialize(m_PlatformHandle.hwnd)) {
        return false;
    }

    // Initialize rendering primitives
    m_RenderContext->MakeCurrent();
    if (!m_RenderPrimitives->Initialize()) {
        Logger::Log(LogLevel::Error, "Failed to initialize rendering primitives");
        return false;
    }
    m_RenderContext->ReleaseCurrent();

    return true;
#else
    return false;
#endif
}

void UIWindow::BeginRender() {
    if (m_RenderContext && m_RenderContext->IsValid()) {
        m_RenderContext->MakeCurrent();
        
        // Set up viewport
        glViewport(0, 0, static_cast<GLsizei>(m_State.size.x), static_cast<GLsizei>(m_State.size.y));
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Enable blending for transparency
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
}

void UIWindow::EndRender() {
    if (m_RenderContext && m_RenderContext->IsValid()) {
        // Disable blending
        glDisable(GL_BLEND);

        m_RenderContext->Present(true); // Enable vsync by default
        m_RenderContext->ReleaseCurrent();
    }
}

bool UIWindow::HasValidContext() const {
    return m_RenderContext && m_RenderContext->IsValid();
}

void UIWindow::Render() {
    if (!m_PlatformWindow) return;

    // Make this window's context current
    m_PlatformWindow->MakeCurrent();

    // Render base widget
    UIWidget::Render();

    // Swap buffers
    m_PlatformWindow->SwapBuffers();
}

void UIWindow::Update(float deltaTime) {
    if (m_PlatformWindow) {
        m_PlatformWindow->PollEvents();
    }
    
    // Update base widget
    UIWidget::Update(deltaTime);
}

void UIWindow::Show() {
    m_State.isVisible = true;
#ifdef UI_PLATFORM_WINDOWS
    ShowWindow(m_PlatformHandle.hwnd, SW_SHOW);
#endif
}

void UIWindow::Hide() {
    m_State.isVisible = false;
#ifdef UI_PLATFORM_WINDOWS
    ShowWindow(m_PlatformHandle.hwnd, SW_HIDE);
#endif
}

void UIWindow::Minimize() {
    m_State.isMinimized = true;
    m_State.isMaximized = false;
#ifdef UI_PLATFORM_WINDOWS
    ShowWindow(m_PlatformHandle.hwnd, SW_MINIMIZE);
#endif
}

void UIWindow::Maximize() {
    m_State.isMaximized = true;
    m_State.isMinimized = false;
#ifdef UI_PLATFORM_WINDOWS
    ShowWindow(m_PlatformHandle.hwnd, SW_MAXIMIZE);
#endif
}

void UIWindow::Restore() {
    m_State.isMaximized = false;
    m_State.isMinimized = false;
#ifdef UI_PLATFORM_WINDOWS
    ShowWindow(m_PlatformHandle.hwnd, SW_RESTORE);
#endif
}

void UIWindow::SetTitle(const std::string& title) {
    m_Title = title;
#ifdef UI_PLATFORM_WINDOWS
    SetWindowTextW(m_PlatformHandle.hwnd, std::wstring(title.begin(), title.end()).c_str());
#endif
}

void UIWindow::SetPosition(const glm::vec2& position) {
    m_State.position = position;
#ifdef UI_PLATFORM_WINDOWS
    SetWindowPos(m_PlatformHandle.hwnd, nullptr,
        static_cast<int>(position.x),
        static_cast<int>(position.y),
        0, 0, SWP_NOSIZE | SWP_NOZORDER);
#endif
}

void UIWindow::SetSize(const glm::vec2& size) {
    m_State.size = size;
#ifdef UI_PLATFORM_WINDOWS
    SetWindowPos(m_PlatformHandle.hwnd, nullptr,
        0, 0,
        static_cast<int>(size.x),
        static_cast<int>(size.y),
        SWP_NOMOVE | SWP_NOZORDER);
#endif
}

void UIWindow::BringToFront() {
#ifdef UI_PLATFORM_WINDOWS
    SetForegroundWindow(m_PlatformHandle.hwnd);
#endif
}

void UIWindow::SetTopMost(bool topMost) {
    m_IsTopMost = topMost;
#ifdef UI_PLATFORM_WINDOWS
    SetWindowPos(m_PlatformHandle.hwnd,
        topMost ? HWND_TOPMOST : HWND_NOTOPMOST,
        0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE);
#endif
}

void UIWindow::AddEventCallback(WindowEventCallback callback) {
    m_EventCallbacks.push_back(callback);
}

void UIWindow::RemoveEventCallback(WindowEventCallback callback) {
    m_EventCallbacks.erase(
        std::remove_if(m_EventCallbacks.begin(), m_EventCallbacks.end(),
            [&](const WindowEventCallback& cb) { return cb.target_type() == callback.target_type(); }),
        m_EventCallbacks.end());
}

void UIWindow::OnEvent(const WindowEvent& event) {
    for (const auto& callback : m_EventCallbacks) {
        callback(event);
        if (event.handled) break;
    }

    // Handle resize events for the render context
    if (event.type == WindowEventType::Resized && m_RenderContext && m_RenderContext->IsValid()) {
        m_RenderContext->MakeCurrent();
        glViewport(0, 0, static_cast<GLsizei>(event.size.x), static_cast<GLsizei>(event.size.y));
        m_RenderContext->ReleaseCurrent();
    }
}

void UIWindow::CreatePlatformWindow() {
#ifdef UI_PLATFORM_WINDOWS
    auto& windowManager = UIWindowManager::Get();
    HWND parent = nullptr;
    DWORD style = WS_OVERLAPPEDWINDOW;
    DWORD exStyle = WS_EX_APPWINDOW;

    // Adjust style based on window type
    switch (m_Type) {
        case WindowType::ToolWindow:
            style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX;
            exStyle = WS_EX_TOOLWINDOW;
            break;
        case WindowType::ModalDialog:
            style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
            exStyle = WS_EX_DLGMODALFRAME;
            if (auto mainWindow = windowManager.GetMainWindow()) {
                parent = mainWindow->GetPlatformHandle().hwnd;
            }
            break;
        case WindowType::PopupWindow:
            style = WS_POPUP | WS_BORDER;
            exStyle = WS_EX_TOPMOST | WS_EX_TOOLWINDOW;
            break;
    }

    // Create the window
    m_PlatformHandle.hwnd = CreateWindowExW(
        exStyle,
        L"VersaUIWindow",
        std::wstring(m_Title.begin(), m_Title.end()).c_str(),
        style,
        CW_USEDEFAULT, CW_USEDEFAULT,
        static_cast<int>(m_State.size.x),
        static_cast<int>(m_State.size.y),
        parent,
        nullptr,
        GetModuleHandle(nullptr),
        nullptr
    );

    if (m_PlatformHandle.hwnd) {
        // Store this pointer with the window
        SetWindowLongPtr(m_PlatformHandle.hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
        
        // Get the device context
        m_PlatformHandle.hdc = GetDC(m_PlatformHandle.hwnd);

        // Enable DWM transitions
        BOOL value = TRUE;
        DwmSetWindowAttribute(m_PlatformHandle.hwnd, DWMWA_TRANSITIONS_FORCEDISABLED, &value, sizeof(value));
    }
#endif
}

void UIWindow::DestroyPlatformWindow() {
#ifdef UI_PLATFORM_WINDOWS
    if (m_PlatformHandle.hdc) {
        ReleaseDC(m_PlatformHandle.hwnd, m_PlatformHandle.hdc);
        m_PlatformHandle.hdc = nullptr;
    }
    if (m_PlatformHandle.hwnd) {
        DestroyWindow(m_PlatformHandle.hwnd);
        m_PlatformHandle.hwnd = nullptr;
    }
#endif
}

void UIWindow::UpdateDecorations() {
#ifdef UI_PLATFORM_WINDOWS
    LONG style = GetWindowLong(m_PlatformHandle.hwnd, GWL_STYLE);
    LONG exStyle = GetWindowLong(m_PlatformHandle.hwnd, GWL_EXSTYLE);

    if (m_HasDecorations) {
        // Remove Windows decorations
        style &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
        exStyle &= ~(WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);
    }
    else {
        // Restore Windows decorations based on window type
        switch (m_Type) {
            case WindowType::MainFrame:
                style |= WS_OVERLAPPEDWINDOW;
                break;
            case WindowType::ToolWindow:
                style |= WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX;
                exStyle |= WS_EX_TOOLWINDOW;
                break;
            case WindowType::ModalDialog:
                style |= WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
                exStyle |= WS_EX_DLGMODALFRAME;
                break;
            case WindowType::PopupWindow:
                style |= WS_POPUP | WS_BORDER;
                exStyle |= WS_EX_TOPMOST | WS_EX_TOOLWINDOW;
                break;
        }
    }

    SetWindowLong(m_PlatformHandle.hwnd, GWL_STYLE, style);
    SetWindowLong(m_PlatformHandle.hwnd, GWL_EXSTYLE, exStyle);

    // Force window to update its frame
    SetWindowPos(m_PlatformHandle.hwnd, nullptr, 0, 0, 0, 0,
        SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);
#endif
}

void UIWindow::OnClosing() {
    if (m_OnClose) {
        m_OnClose();
    }
}

void UIWindow::OnFocused() {
    if (m_OnFocus) {
        m_OnFocus();
    }
}

void UIWindow::OnBlurred() {
    if (m_OnBlur) {
        m_OnBlur();
    }
}

void UIWindow::OnMinimized() {
    if (m_OnMinimize) {
        m_OnMinimize();
    }
}

void UIWindow::OnMaximized() {
    if (m_OnMaximize) {
        m_OnMaximize();
    }
}

void UIWindow::OnRestored() {
    if (m_OnRestore) {
        m_OnRestore();
    }
}

}} // namespace VGE::UI



