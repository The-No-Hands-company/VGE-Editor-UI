#pragma once

#include "Core/UICore.h"
#include "Platform/UIWindowDesc.h"
#include <memory>
#include <string>

struct GLFWwindow;

namespace VGE {
namespace UI {

class GLFWPlatformWindow;

class UI_API UIWindow {
public:
    explicit UIWindow(const UIWindowDesc& desc);
    UIWindow(const std::string& title, int width = 1280, int height = 720);
    ~UIWindow();

    void Update();
    void Clear();
    void SetClearColor(float r, float g, float b, float a = 1.0f);
    bool ShouldClose() const;

    int GetWidth() const { return m_Width; }
    int GetHeight() const { return m_Height; }
    GLFWPlatformWindow* GetPlatformWindow() { return m_PlatformWindow.get(); }
    const GLFWPlatformWindow* GetPlatformWindow() const { return m_PlatformWindow.get(); }

protected:
    virtual void OnResize(int width, int height);

private:
    std::unique_ptr<GLFWPlatformWindow> m_PlatformWindow;
    int m_Width;
    int m_Height;
};

}} // namespace VGE::UI 