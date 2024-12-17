#pragma once

#include "Core/UICore.h"
#include "Runtime/Core/Public/Renderer/UIShader.h"
#include <glm/glm.hpp>
#include <memory>

namespace VGE {
namespace UI {

struct GlassEffectSettings {
    float blurRadius = 10.0f;
    float transparency = 0.5f;
    float refraction = 0.1f;
    float chromaticAberration = 0.02f;
    float distortion = 0.05f;
    glm::vec4 tintColor = glm::vec4(1.0f, 1.0f, 1.0f, 0.1f);
    bool enableNoise = true;
    float noiseIntensity = 0.03f;
};

class UI_API UIGlassEffect {
public:
    UIGlassEffect();
    ~UIGlassEffect();

    bool Initialize();
    void Cleanup();

    // Apply glass effect to a region
    void Apply(const glm::vec2& position, const glm::vec2& size);
    
    // Update effect settings
    void UpdateSettings(const GlassEffectSettings& settings);
    
    // Enable/disable features
    void EnableBlur(bool enable);
    void EnableRefraction(bool enable);
    void EnableChromaticAberration(bool enable);
    void EnableDistortion(bool enable);
    void EnableNoise(bool enable);

    // Get current settings
    const GlassEffectSettings& GetSettings() const { return m_Settings; }

private:
    bool CreateShaders();
    bool CreateFramebuffers();
    void UpdateUniforms();
    void RenderBlurPass();
    void RenderGlassPass();

private:
    GlassEffectSettings m_Settings;
    std::shared_ptr<UIShader> m_BlurShader;
    std::shared_ptr<UIShader> m_GlassShader;
    
    uint32_t m_BlurFBO;
    uint32_t m_BlurTexture;
    uint32_t m_NoiseTexture;
    
    // Shader uniforms
    std::string m_BlurRadiusUniform;
    std::string m_TransparencyUniform;
    std::string m_RefractionUniform;
    std::string m_ChromaticAberrationUniform;
    std::string m_DistortionUniform;
    std::string m_TintColorUniform;
    std::string m_NoiseIntensityUniform;
    std::string m_TimeUniform;

    bool m_Initialized;
    float m_Time;
};

}} // namespace VGE::UI 