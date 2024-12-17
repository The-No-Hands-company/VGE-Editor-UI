#include "Runtime/Core/Public/Effects/UIGlowEffect.h"
#include "Runtime/Core/Public/Core/Logger.h"
#include <glad/gl.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>

namespace VGE {
namespace UI {

UIGlowEffect::UIGlowEffect()
    : m_GlowFBO(0)
    , m_GlowTexture(0)
    , m_ColorUniform("glowColor")
    , m_SizeUniform("glowSize")
    , m_IntensityUniform("glowIntensity")
    , m_TimeUniform("time")
    , m_Initialized(false)
    , m_Time(0.0f)
{
}

UIGlowEffect::~UIGlowEffect() {
    Cleanup();
}

bool UIGlowEffect::Initialize() {
    if (m_Initialized) return true;

    if (!CreateShaders()) {
        Logger::Log(LogLevel::Error, "Failed to create glow effect shaders");
        return false;
    }

    if (!CreateFramebuffers()) {
        Logger::Log(LogLevel::Error, "Failed to create glow effect framebuffers");
        return false;
    }

    m_Initialized = true;
    return true;
}

void UIGlowEffect::Cleanup() {
    if (m_GlowFBO) {
        glDeleteFramebuffers(1, &m_GlowFBO);
        m_GlowFBO = 0;
    }

    if (m_GlowTexture) {
        glDeleteTextures(1, &m_GlowTexture);
        m_GlowTexture = 0;
    }

    m_GlowShader.reset();
    m_Initialized = false;
}

bool UIGlowEffect::CreateShaders() {
    const char* glowVertexShader = R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        layout (location = 1) in vec2 aTexCoord;
        out vec2 TexCoord;
        out vec2 FragPos;
        
        void main() {
            gl_Position = vec4(aPos, 0.0, 1.0);
            TexCoord = aTexCoord;
            FragPos = aPos;
        }
    )";

    const char* glowFragmentShader = R"(
        #version 330 core
        in vec2 TexCoord;
        in vec2 FragPos;
        out vec4 FragColor;
        
        uniform vec4 glowColor;
        uniform float glowSize;
        uniform float glowIntensity;
        uniform float time;
        uniform bool innerGlow;
        uniform vec2 size;
        
        float gaussian(float x, float sigma) {
            return exp(-(x * x) / (2.0 * sigma * sigma)) / (sqrt(2.0 * 3.14159) * sigma);
        }
        
        void main() {
            vec2 uv = FragPos;
            float alpha = 0.0;
            
            // Sample multiple points for glow effect
            const int SAMPLES = 12;
            float sigma = glowSize * 0.5;
            
            for (int x = -SAMPLES; x <= SAMPLES; x++) {
                for (int y = -SAMPLES; y <= SAMPLES; y++) {
                    vec2 offset = vec2(x, y) * (glowSize / float(SAMPLES));
                    vec2 samplePos = (uv + offset) / size;
                    
                    if (samplePos.x >= 0.0 && samplePos.x <= 1.0 &&
                        samplePos.y >= 0.0 && samplePos.y <= 1.0) {
                        float weight = gaussian(length(offset), sigma);
                        
                        if (innerGlow) {
                            // For inner glow, invert the weight
                            weight = 1.0 - weight;
                        }
                        
                        alpha += weight;
                    }
                }
            }
            
            // Apply pulsing effect if enabled
            float pulseIntensity = glowIntensity;
            if (time > 0.0) {
                float pulse = (sin(time) + 1.0) * 0.5; // Oscillate between 0 and 1
                pulseIntensity *= mix(0.7, 1.0, pulse);
            }
            
            alpha = smoothstep(0.0, 1.0, alpha) * pulseIntensity;
            if (innerGlow) {
                alpha = 1.0 - alpha;
            }
            
            FragColor = vec4(glowColor.rgb, glowColor.a * alpha);
        }
    )";

    m_GlowShader = std::make_shared<UIShader>();
    if (!m_GlowShader->Initialize(glowVertexShader, glowFragmentShader)) {
        return false;
    }

    return true;
}

bool UIGlowEffect::CreateFramebuffers() {
    // Create glow framebuffer
    glGenFramebuffers(1, &m_GlowFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_GlowFBO);

    // Create glow texture
    glGenTextures(1, &m_GlowTexture);
    glBindTexture(GL_TEXTURE_2D, m_GlowTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 1920, 1080, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_GlowTexture, 0);

    // Check framebuffer status
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        Logger::Log(LogLevel::Error, "Glow framebuffer is not complete");
        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}

void UIGlowEffect::Apply(const glm::vec2& position, const glm::vec2& size) {
    if (!m_Initialized) return;

    // Update time for pulsing effect
    if (m_Settings.pulseSpeed > 0.0f) {
        m_Time += 0.016f * m_Settings.pulseSpeed; // Assuming 60 FPS, should use actual delta time
    }

    // Render outer glow if enabled
    if (m_Settings.enableOuterGlow) {
        RenderOuterGlow();
    }

    // Render inner glow if enabled
    if (m_Settings.enableInnerGlow) {
        RenderInnerGlow();
    }
}

void UIGlowEffect::UpdateSettings(const GlowEffectSettings& settings) {
    m_Settings = settings;
    UpdateUniforms();
}

void UIGlowEffect::EnableOuterGlow(bool enable) {
    m_Settings.enableOuterGlow = enable;
    UpdateUniforms();
}

void UIGlowEffect::EnableInnerGlow(bool enable) {
    m_Settings.enableInnerGlow = enable;
    UpdateUniforms();
}

void UIGlowEffect::EnablePulsing(float speed) {
    m_Settings.pulseSpeed = speed;
    m_Time = 0.0f; // Reset time when enabling/disabling pulsing
    UpdateUniforms();
}

void UIGlowEffect::UpdateUniforms() {
    if (!m_Initialized) return;

    m_GlowShader->Bind();
    m_GlowShader->SetVec4(m_ColorUniform, m_Settings.glowColor);
    m_GlowShader->SetFloat(m_SizeUniform, m_Settings.glowSize);
    m_GlowShader->SetFloat(m_IntensityUniform, m_Settings.glowIntensity);
    m_GlowShader->SetFloat(m_TimeUniform, m_Settings.pulseSpeed > 0.0f ? m_Time : 0.0f);
}

void UIGlowEffect::RenderOuterGlow() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_GlowFBO);
    m_GlowShader->Bind();
    m_GlowShader->SetBool("innerGlow", false);
    // Render full-screen quad with outer glow shader
    // ... (implement quad rendering)
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void UIGlowEffect::RenderInnerGlow() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_GlowFBO);
    m_GlowShader->Bind();
    m_GlowShader->SetBool("innerGlow", true);
    // Render full-screen quad with inner glow shader
    // ... (implement quad rendering)
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

}} // namespace VGE::UI 