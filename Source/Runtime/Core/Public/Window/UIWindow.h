#pragma once

#include "Core/UICore.h"
#include "Runtime/Core/Public/Widget/UIWidget.h"
#include "Runtime/Core/Public/Effects/UIGlassEffect.h"
#include "Runtime/Core/Public/Effects/UIGlowEffect.h"
#include "Runtime/Core/Public/Window/UIWindowSnapping.h"
#include "Platform/UIWindow.h"
#include "Platform/UIWindowDesc.h"
#include <string>
#include <memory>
#include <functional>

namespace VGE {
namespace UI {

struct WindowEffectSettings {
    // Glass morphism settings
    bool enableGlassMorphism = false;
    GlassEffectSettings glassSettings;
    
    // Glow settings
    bool enableGlow = false;
    GlowEffectSettings glowSettings;
    
    // Shadow settings
    bool enableShadow = true;
    glm::vec4 shadowColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.3f);
    glm::vec2 shadowOffset = glm::vec2(0.0f, 5.0f);
    float shadowBlur = 15.0f;
    
    // Animation settings
    bool enableAnimation = true;
    float animationDuration = 0.3f;

    // Snapping settings
    SnapSettings snapSettings;
};

class UI_API UIWindow : public UIWidget {
public:
    explicit UIWindow(const std::string& title = "")
        : m_Title(title)
        , m_PlatformWindow(std::make_unique<Platform::PlatformWindow>(CreatePlatformDesc(title)))
        , m_GlassEffect(std::make_unique<UIGlassEffect>())
        , m_GlowEffect(std::make_unique<UIGlowEffect>())
        , m_WindowSnapping(std::make_unique<UIWindowSnapping>(this))
        , m_AnimationProgress(1.0f) {}
    
    UIWindow(const std::string& title, int width, int height)
        : m_Title(title)
        , m_PlatformWindow(std::make_unique<Platform::PlatformWindow>(CreatePlatformDesc(title, width, height)))
        , m_GlassEffect(std::make_unique<UIGlassEffect>())
        , m_GlowEffect(std::make_unique<UIGlowEffect>())
        , m_WindowSnapping(std::make_unique<UIWindowSnapping>(this))
        , m_AnimationProgress(1.0f) {}
        
    explicit UIWindow(const UIWindowDesc& desc)
        : m_Title(desc.title)
        , m_PlatformWindow(std::make_unique<Platform::PlatformWindow>(CreatePlatformDesc(desc)))
        , m_GlassEffect(std::make_unique<UIGlassEffect>())
        , m_GlowEffect(std::make_unique<UIGlowEffect>())
        , m_WindowSnapping(std::make_unique<UIWindowSnapping>(this))
        , m_AnimationProgress(1.0f) {}
        
    virtual ~UIWindow() = default;

    // Window state
    bool IsOpen() const { return m_PlatformWindow && !m_PlatformWindow->ShouldClose(); }
    void SetOpen(bool open);
    bool IsFocused() const { return m_PlatformWindow && m_PlatformWindow->IsFocused(); }
    void SetFocused(bool focused);
    bool IsMinimized() const { return m_PlatformWindow && m_PlatformWindow->IsMinimized(); }
    void SetMinimized(bool minimized);
    bool IsMaximized() const { return m_PlatformWindow && m_PlatformWindow->IsMaximized(); }
    void SetMaximized(bool maximized);

    // Window properties
    const std::string& GetTitle() const { return m_Title; }
    void SetTitle(const std::string& title);
    bool IsResizable() const { return m_PlatformWindow && m_PlatformWindow->IsResizable(); }
    void SetResizable(bool resizable);

    // Position and size with snapping
    void SetPosition(const glm::vec2& position) override {
        if (m_WindowSnapping) {
            UIWidget::SetPosition(m_WindowSnapping->CalculateSnapPosition(position));
        } else {
            UIWidget::SetPosition(position);
        }
    }

    // Modern effects
    void EnableGlassMorphism(bool enable);
    void UpdateGlassSettings(const GlassEffectSettings& settings);
    void EnableGlow(bool enable);
    void UpdateGlowSettings(const GlowEffectSettings& settings);
    void EnableSnapping(bool enable);
    void UpdateSnapSettings(const SnapSettings& settings);
    void SetEffectSettings(const WindowEffectSettings& settings);
    const WindowEffectSettings& GetEffectSettings() const { return m_EffectSettings; }

    // Window events
    using WindowCallback = std::function<void()>;
    void SetOnClose(WindowCallback callback) { m_OnClose = callback; }
    void SetOnFocus(WindowCallback callback) { m_OnFocus = callback; }
    void SetOnBlur(WindowCallback callback) { m_OnBlur = callback; }
    void SetOnMinimize(WindowCallback callback) { m_OnMinimize = callback; }
    void SetOnMaximize(WindowCallback callback) { m_OnMaximize = callback; }
    void SetOnRestore(WindowCallback callback) { m_OnRestore = callback; }

    // Widget interface
    void Update(float deltaTime) override;
    void Render() override;

    // Platform window access
    Platform::PlatformWindow* GetPlatformWindow() { return m_PlatformWindow.get(); }
    const Platform::PlatformWindow* GetPlatformWindow() const { return m_PlatformWindow.get(); }

protected:
    // Event handlers
    virtual void OnClosing();
    virtual void OnFocused();
    virtual void OnBlurred();
    virtual void OnMinimized();
    virtual void OnMaximized();
    virtual void OnRestored();

    // Rendering helpers
    virtual void RenderBackground();
    virtual void RenderDecorations();
    virtual void RenderShadow();
    virtual void RenderGlassEffect();
    virtual void RenderGlowEffect();
    virtual void UpdateAnimation(float deltaTime);

private:
    Platform::WindowDesc CreatePlatformDesc(const std::string& title, int width = 1280, int height = 720) const;
    Platform::WindowDesc CreatePlatformDesc(const UIWindowDesc& desc) const;
    void InitializeEffects();

private:
    std::string m_Title;
    std::unique_ptr<Platform::PlatformWindow> m_PlatformWindow;
    std::unique_ptr<UIGlassEffect> m_GlassEffect;
    std::unique_ptr<UIGlowEffect> m_GlowEffect;
    std::unique_ptr<UIWindowSnapping> m_WindowSnapping;
    WindowEffectSettings m_EffectSettings;
    float m_AnimationProgress;

    // Window state for animations
    glm::vec2 m_StartPosition;
    glm::vec2 m_TargetPosition;
    glm::vec2 m_StartSize;
    glm::vec2 m_TargetSize;
    bool m_IsAnimating;

    // Event callbacks
    WindowCallback m_OnClose;
    WindowCallback m_OnFocus;
    WindowCallback m_OnBlur;
    WindowCallback m_OnMinimize;
    WindowCallback m_OnMaximize;
    WindowCallback m_OnRestore;
};

}} // namespace VGE::UI 