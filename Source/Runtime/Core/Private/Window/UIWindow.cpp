#include "Runtime/Core/Public/Window/UIWindow.h"
#include "Runtime/Core/Public/Core/Logger.h"
#include "Runtime/Core/Public/Renderer/UIRenderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

namespace VGE {
namespace UI {

void UIWindow::SetOpen(bool open) {
    if (m_PlatformWindow) {
        if (!open && m_EffectSettings.enableAnimation) {
            // Start closing animation
            m_StartPosition = GetPosition();
            m_TargetPosition = GetPosition() + glm::vec2(0.0f, 20.0f);
            m_StartSize = GetSize();
            m_TargetSize = GetSize();
            m_AnimationProgress = 0.0f;
            m_IsAnimating = true;
        }
        m_PlatformWindow->SetShouldClose(!open);
    }
}

void UIWindow::SetFocused(bool focused) {
    if (m_PlatformWindow) {
        if (focused) {
            m_PlatformWindow->Focus();
            OnFocused();
        } else {
            OnBlurred();
        }
    }
}

void UIWindow::SetMinimized(bool minimized) {
    if (m_PlatformWindow) {
        if (minimized) {
            if (m_EffectSettings.enableAnimation) {
                // Start minimize animation
                m_StartPosition = GetPosition();
                m_TargetPosition = GetPosition() + glm::vec2(0.0f, GetSize().y);
                m_StartSize = GetSize();
                m_TargetSize = glm::vec2(GetSize().x, 0.0f);
                m_AnimationProgress = 0.0f;
                m_IsAnimating = true;
            }
            m_PlatformWindow->Minimize();
            OnMinimized();
        } else {
            m_PlatformWindow->Restore();
            OnRestored();
        }
    }
}

void UIWindow::SetMaximized(bool maximized) {
    if (m_PlatformWindow) {
        if (maximized) {
            if (m_EffectSettings.enableAnimation) {
                // Start maximize animation
                m_StartPosition = GetPosition();
                m_TargetPosition = glm::vec2(0.0f);
                m_StartSize = GetSize();
                m_TargetSize = m_PlatformWindow->GetScreenSize();
                m_AnimationProgress = 0.0f;
                m_IsAnimating = true;
            }
            m_PlatformWindow->Maximize();
            OnMaximized();
        } else {
            m_PlatformWindow->Restore();
            OnRestored();
        }
    }
}

void UIWindow::SetTitle(const std::string& title) {
    m_Title = title;
    if (m_PlatformWindow) {
        m_PlatformWindow->SetTitle(title);
    }
}

void UIWindow::SetResizable(bool resizable) {
    if (m_PlatformWindow) {
        m_PlatformWindow->SetResizable(resizable);
    }
}

void UIWindow::EnableGlassMorphism(bool enable) {
    m_EffectSettings.enableGlassMorphism = enable;
    if (enable && !m_GlassEffect->IsInitialized()) {
        m_GlassEffect->Initialize();
    }
}

void UIWindow::UpdateGlassSettings(const GlassEffectSettings& settings) {
    m_EffectSettings.glassSettings = settings;
    if (m_EffectSettings.enableGlassMorphism) {
        m_GlassEffect->UpdateSettings(settings);
    }
}

void UIWindow::EnableGlow(bool enable) {
    m_EffectSettings.enableGlow = enable;
    if (enable && !m_GlowEffect->IsInitialized()) {
        m_GlowEffect->Initialize();
    }
}

void UIWindow::UpdateGlowSettings(const GlowEffectSettings& settings) {
    m_EffectSettings.glowSettings = settings;
    if (m_EffectSettings.enableGlow) {
        m_GlowEffect->UpdateSettings(settings);
    }
}

void UIWindow::EnableSnapping(bool enable) {
    m_EffectSettings.snapSettings.enableSnapping = enable;
    if (m_WindowSnapping) {
        m_WindowSnapping->EnableSnapping(enable);
    }
}

void UIWindow::UpdateSnapSettings(const SnapSettings& settings) {
    m_EffectSettings.snapSettings = settings;
    if (m_WindowSnapping) {
        m_WindowSnapping->SetSettings(settings);
    }
}

void UIWindow::SetEffectSettings(const WindowEffectSettings& settings) {
    m_EffectSettings = settings;
    if (m_EffectSettings.enableGlassMorphism) {
        m_GlassEffect->UpdateSettings(settings.glassSettings);
    }
    if (m_EffectSettings.enableGlow) {
        m_GlowEffect->UpdateSettings(settings.glowSettings);
    }
    if (m_WindowSnapping) {
        m_WindowSnapping->SetSettings(settings.snapSettings);
    }
}

void UIWindow::Update(float deltaTime) {
    UIWidget::Update(deltaTime);
    
    if (m_IsAnimating) {
        UpdateAnimation(deltaTime);
    }

    // Update window snapping
    if (m_WindowSnapping) {
        m_WindowSnapping->Update(deltaTime);
    }
}

void UIWindow::Render() {
    if (!IsVisible()) return;

    // Render shadow if enabled
    if (m_EffectSettings.enableShadow && !IsMinimized()) {
        RenderShadow();
    }

    // Render background with glass effect if enabled
    if (m_EffectSettings.enableGlassMorphism) {
        RenderGlassEffect();
    } else {
        RenderBackground();
    }

    // Render glow effect if enabled
    if (m_EffectSettings.enableGlow) {
        RenderGlowEffect();
    }

    // Render snap guides if enabled
    if (m_WindowSnapping && m_EffectSettings.snapSettings.showGuides) {
        m_WindowSnapping->RenderGuides();
    }

    // Render window decorations
    RenderDecorations();

    // Render child widgets
    UIWidget::Render();
}

void UIWindow::RenderBackground() {
    auto renderer = UIRenderer::Get();
    if (!renderer) return;

    const glm::vec4 backgroundColor(0.2f, 0.2f, 0.2f, 1.0f);
    renderer->DrawRect(GetPosition(), GetSize(), backgroundColor);
}

void UIWindow::RenderDecorations() {
    auto renderer = UIRenderer::Get();
    if (!renderer) return;

    // Title bar
    const float titleBarHeight = 25.0f;
    const glm::vec4 titleBarColor(0.3f, 0.3f, 0.3f, 1.0f);
    renderer->DrawRect(
        GetPosition(),
        glm::vec2(GetSize().x, titleBarHeight),
        titleBarColor
    );

    // Title text
    const glm::vec4 textColor(1.0f);
    renderer->DrawText(
        m_Title,
        GetPosition() + glm::vec2(5.0f, 5.0f),
        textColor
    );

    // Window buttons
    const float buttonSize = 16.0f;
    const float buttonSpacing = 5.0f;
    const float buttonsStartX = GetPosition().x + GetSize().x - (buttonSize + buttonSpacing) * 3;
    const float buttonY = GetPosition().y + (titleBarHeight - buttonSize) * 0.5f;

    // Close button
    renderer->DrawRect(
        glm::vec2(buttonsStartX + (buttonSize + buttonSpacing) * 2, buttonY),
        glm::vec2(buttonSize),
        glm::vec4(0.8f, 0.2f, 0.2f, 1.0f)
    );

    // Maximize button
    renderer->DrawRect(
        glm::vec2(buttonsStartX + (buttonSize + buttonSpacing), buttonY),
        glm::vec2(buttonSize),
        glm::vec4(0.4f, 0.4f, 0.4f, 1.0f)
    );

    // Minimize button
    renderer->DrawRect(
        glm::vec2(buttonsStartX, buttonY),
        glm::vec2(buttonSize),
        glm::vec4(0.4f, 0.4f, 0.4f, 1.0f)
    );
}

void UIWindow::RenderShadow() {
    auto renderer = UIRenderer::Get();
    if (!renderer) return;

    // Render outer shadow
    const int shadowSamples = 20;
    const float maxOffset = m_EffectSettings.shadowBlur;
    
    for (int i = 0; i < shadowSamples; ++i) {
        float t = static_cast<float>(i) / shadowSamples;
        float offset = maxOffset * t;
        glm::vec4 shadowColor = m_EffectSettings.shadowColor;
        shadowColor.a *= (1.0f - t);

        renderer->DrawRect(
            GetPosition() + m_EffectSettings.shadowOffset + glm::vec2(-offset),
            GetSize() + glm::vec2(offset * 2.0f),
            shadowColor
        );
    }
}

void UIWindow::RenderGlassEffect() {
    if (!m_GlassEffect || !m_GlassEffect->IsInitialized()) return;

    // Apply glass effect
    m_GlassEffect->Apply(GetPosition(), GetSize());
}

void UIWindow::RenderGlowEffect() {
    if (!m_GlowEffect || !m_GlowEffect->IsInitialized()) return;

    // Apply glow effect
    m_GlowEffect->Apply(GetPosition(), GetSize());
}

void UIWindow::UpdateAnimation(float deltaTime) {
    if (!m_IsAnimating) return;

    // Update animation progress
    float animationSpeed = 1.0f / m_EffectSettings.animationDuration;
    m_AnimationProgress = std::min(m_AnimationProgress + deltaTime * animationSpeed, 1.0f);

    // Apply easing function (cubic ease-out)
    float t = 1.0f - std::pow(1.0f - m_AnimationProgress, 3.0f);

    // Update position and size
    SetPosition(glm::mix(m_StartPosition, m_TargetPosition, t));
    SetSize(glm::mix(m_StartSize, m_TargetSize, t));

    // Check if animation is complete
    if (m_AnimationProgress >= 1.0f) {
        m_IsAnimating = false;
    }
}

void UIWindow::OnClosing() {
    if (m_OnClose) m_OnClose();
}

void UIWindow::OnFocused() {
    if (m_OnFocus) m_OnFocus();
}

void UIWindow::OnBlurred() {
    if (m_OnBlur) m_OnBlur();
}

void UIWindow::OnMinimized() {
    if (m_OnMinimize) m_OnMinimize();
}

void UIWindow::OnMaximized() {
    if (m_OnMaximize) m_OnMaximize();
}

void UIWindow::OnRestored() {
    if (m_OnRestore) m_OnRestore();
}

void UIWindow::InitializeEffects() {
    if (m_EffectSettings.enableGlassMorphism) {
        m_GlassEffect->Initialize();
    }
    if (m_EffectSettings.enableGlow) {
        m_GlowEffect->Initialize();
    }
}

Platform::WindowDesc UIWindow::CreatePlatformDesc(const std::string& title, int width, int height) const {
    Platform::WindowDesc desc;
    desc.title = title;
    desc.size = glm::vec2(width, height);
    return desc;
}

Platform::WindowDesc UIWindow::CreatePlatformDesc(const UIWindowDesc& desc) const {
    Platform::WindowDesc platformDesc;
    platformDesc.title = desc.title;
    platformDesc.size = glm::vec2(desc.width, desc.height);
    platformDesc.resizable = desc.resizable;
    platformDesc.decorated = desc.decorated;
    platformDesc.floating = desc.floating;
    platformDesc.maximized = desc.maximized;
    platformDesc.minimized = desc.minimized;
    platformDesc.visible = desc.visible;
    platformDesc.focusOnShow = desc.focusOnShow;
    platformDesc.scaleToMonitor = desc.scaleToMonitor;
    platformDesc.centerCursor = desc.centerCursor;
    platformDesc.transparentFramebuffer = desc.transparentFramebuffer;
    platformDesc.fullscreen = desc.fullscreen;
    return platformDesc;
}

}} // namespace VGE::UI 