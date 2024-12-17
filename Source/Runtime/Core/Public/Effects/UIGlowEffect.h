#pragma once

#include "Core/UICore.h"
#include "Runtime/Core/Public/Renderer/UIShader.h"
#include <glm/glm.hpp>
#include <memory>

namespace VGE {
namespace UI {

struct GlowEffectSettings {
    glm::vec4 glowColor = glm::vec4(1.0f, 1.0f, 1.0f, 0.5f);
    float glowSize = 15.0f;
    float glowIntensity = 1.0f;
    float pulseSpeed = 0.0f;  // 0 for no pulsing
    float pulseMinIntensity = 0.7f;
    float pulseMaxIntensity = 1.0f;
    bool enableOuterGlow = true;
    bool enableInnerGlow = false;
    glm::vec4 innerGlowColor = glm::vec4(1.0f, 1.0f, 1.0f, 0.3f);
    float innerGlowSize = 5.0f;
};

class UI_API UIGlowEffect {
public:
    UIGlowEffect();
    ~UIGlowEffect();

    bool Initialize();
    void Cleanup();

    // Apply glow effect to a region
    void Apply(const glm::vec2& position, const glm::vec2& size);
    
    // Update effect settings
    void UpdateSettings(const GlowEffectSettings& settings);
    
    // Enable/disable features
    void EnableOuterGlow(bool enable);
    void EnableInnerGlow(bool enable);
    void EnablePulsing(float speed);

    // Get current settings
    const GlowEffectSettings& GetSettings() const { return m_Settings; }
    
    // Check if initialized
    bool IsInitialized() const { return m_Initialized; }

private:
    bool CreateShaders();
    bool CreateFramebuffers();
    void UpdateUniforms();
    void RenderOuterGlow();
    void RenderInnerGlow();

private:
    GlowEffectSettings m_Settings;
    std::shared_ptr<UIShader> m_GlowShader;
    
    uint32_t m_GlowFBO;
    uint32_t m_GlowTexture;
    
    // Shader uniforms
    std::string m_ColorUniform;
    std::string m_SizeUniform;
    std::string m_IntensityUniform;
    std::string m_TimeUniform;
    
    bool m_Initialized;
    float m_Time;
};

}} // namespace VGE::UI 