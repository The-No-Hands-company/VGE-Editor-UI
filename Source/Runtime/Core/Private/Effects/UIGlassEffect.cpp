#include "Runtime/Core/Public/Effects/UIGlassEffect.h"
#include "Runtime/Core/Public/Core/Logger.h"
#include <glad/gl.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>

namespace VGE {
namespace UI {

UIGlassEffect::UIGlassEffect()
    : m_BlurFBO(0)
    , m_BlurTexture(0)
    , m_NoiseTexture(0)
    , m_BlurRadiusUniform("blurRadius")
    , m_TransparencyUniform("transparency")
    , m_RefractionUniform("refraction")
    , m_ChromaticAberrationUniform("chromaticAberration")
    , m_DistortionUniform("distortion")
    , m_TintColorUniform("tintColor")
    , m_NoiseIntensityUniform("noiseIntensity")
    , m_TimeUniform("time")
    , m_Initialized(false)
    , m_Time(0.0f)
{
}

UIGlassEffect::~UIGlassEffect() {
    Cleanup();
}

bool UIGlassEffect::Initialize() {
    if (m_Initialized) return true;

    if (!CreateShaders()) {
        Logger::Log(LogLevel::Error, "Failed to create glass effect shaders");
        return false;
    }

    if (!CreateFramebuffers()) {
        Logger::Log(LogLevel::Error, "Failed to create glass effect framebuffers");
        return false;
    }

    m_Initialized = true;
    return true;
}

void UIGlassEffect::Cleanup() {
    if (m_BlurFBO) {
        glDeleteFramebuffers(1, &m_BlurFBO);
        m_BlurFBO = 0;
    }

    if (m_BlurTexture) {
        glDeleteTextures(1, &m_BlurTexture);
        m_BlurTexture = 0;
    }

    if (m_NoiseTexture) {
        glDeleteTextures(1, &m_NoiseTexture);
        m_NoiseTexture = 0;
    }

    m_BlurShader.reset();
    m_GlassShader.reset();
    m_Initialized = false;
}

bool UIGlassEffect::CreateShaders() {
    // Blur shader
    const char* blurVertexShader = R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        layout (location = 1) in vec2 aTexCoord;
        out vec2 TexCoord;
        
        void main() {
            gl_Position = vec4(aPos, 0.0, 1.0);
            TexCoord = aTexCoord;
        }
    )";

    const char* blurFragmentShader = R"(
        #version 330 core
        in vec2 TexCoord;
        out vec4 FragColor;
        
        uniform sampler2D screenTexture;
        uniform float blurRadius;
        
        void main() {
            vec2 texelSize = 1.0 / textureSize(screenTexture, 0);
            vec4 result = vec4(0.0);
            float total = 0.0;
            
            for(float x = -blurRadius; x <= blurRadius; x++) {
                for(float y = -blurRadius; y <= blurRadius; y++) {
                    vec2 offset = vec2(x, y) * texelSize;
                    float weight = exp(-(x*x + y*y) / (2.0 * blurRadius * blurRadius));
                    result += texture(screenTexture, TexCoord + offset) * weight;
                    total += weight;
                }
            }
            
            FragColor = result / total;
        }
    )";

    m_BlurShader = std::make_shared<UIShader>();
    if (!m_BlurShader->Initialize(blurVertexShader, blurFragmentShader)) {
        return false;
    }

    // Glass shader
    const char* glassVertexShader = R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        layout (location = 1) in vec2 aTexCoord;
        out vec2 TexCoord;
        out vec2 ScreenPos;
        
        void main() {
            gl_Position = vec4(aPos, 0.0, 1.0);
            TexCoord = aTexCoord;
            ScreenPos = aPos * 0.5 + 0.5;
        }
    )";

    const char* glassFragmentShader = R"(
        #version 330 core
        in vec2 TexCoord;
        in vec2 ScreenPos;
        out vec4 FragColor;
        
        uniform sampler2D blurTexture;
        uniform sampler2D noiseTexture;
        uniform float transparency;
        uniform float refraction;
        uniform float chromaticAberration;
        uniform float distortion;
        uniform vec4 tintColor;
        uniform float noiseIntensity;
        uniform float time;
        
        void main() {
            // Sample noise texture with time-based animation
            vec2 noiseCoord = TexCoord + time * 0.1;
            vec2 noise = (texture(noiseTexture, noiseCoord).rg * 2.0 - 1.0) * noiseIntensity;
            
            // Apply distortion
            vec2 distortedCoord = TexCoord + noise * distortion;
            
            // Chromatic aberration
            vec4 blur;
            blur.r = texture(blurTexture, distortedCoord + vec2(chromaticAberration, 0.0)).r;
            blur.g = texture(blurTexture, distortedCoord).g;
            blur.b = texture(blurTexture, distortedCoord - vec2(chromaticAberration, 0.0)).b;
            blur.a = texture(blurTexture, distortedCoord).a;
            
            // Refraction
            vec2 refractionOffset = noise * refraction;
            vec4 refractedColor = texture(blurTexture, distortedCoord + refractionOffset);
            
            // Combine effects
            vec4 finalColor = mix(blur, refractedColor, 0.5);
            finalColor = mix(finalColor, tintColor, tintColor.a);
            finalColor.a = transparency;
            
            FragColor = finalColor;
        }
    )";

    m_GlassShader = std::make_shared<UIShader>();
    if (!m_GlassShader->Initialize(glassVertexShader, glassFragmentShader)) {
        return false;
    }

    return true;
}

bool UIGlassEffect::CreateFramebuffers() {
    // Create blur framebuffer
    glGenFramebuffers(1, &m_BlurFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_BlurFBO);

    // Create blur texture
    glGenTextures(1, &m_BlurTexture);
    glBindTexture(GL_TEXTURE_2D, m_BlurTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 1920, 1080, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_BlurTexture, 0);

    // Create noise texture
    glGenTextures(1, &m_NoiseTexture);
    glBindTexture(GL_TEXTURE_2D, m_NoiseTexture);
    
    // Generate perlin noise texture
    const int noiseSize = 256;
    std::vector<float> noiseData(noiseSize * noiseSize * 4);
    for (int y = 0; y < noiseSize; y++) {
        for (int x = 0; x < noiseSize; x++) {
            float noise = static_cast<float>(rand()) / RAND_MAX;
            int index = (y * noiseSize + x) * 4;
            noiseData[index] = noise;
            noiseData[index + 1] = noise;
            noiseData[index + 2] = noise;
            noiseData[index + 3] = 1.0f;
        }
    }
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, noiseSize, noiseSize, 0, GL_RGBA, GL_FLOAT, noiseData.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Check framebuffer status
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        Logger::Log(LogLevel::Error, "Framebuffer is not complete");
        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}

void UIGlassEffect::Apply(const glm::vec2& position, const glm::vec2& size) {
    if (!m_Initialized) return;

    // Update time for animations
    m_Time += 0.016f; // Assuming 60 FPS, should use actual delta time

    // First pass: Blur
    RenderBlurPass();

    // Second pass: Glass effect
    RenderGlassPass();
}

void UIGlassEffect::UpdateSettings(const GlassEffectSettings& settings) {
    m_Settings = settings;
    UpdateUniforms();
}

void UIGlassEffect::EnableBlur(bool enable) {
    m_Settings.blurRadius = enable ? 10.0f : 0.0f;
    UpdateUniforms();
}

void UIGlassEffect::EnableRefraction(bool enable) {
    m_Settings.refraction = enable ? 0.1f : 0.0f;
    UpdateUniforms();
}

void UIGlassEffect::EnableChromaticAberration(bool enable) {
    m_Settings.chromaticAberration = enable ? 0.02f : 0.0f;
    UpdateUniforms();
}

void UIGlassEffect::EnableDistortion(bool enable) {
    m_Settings.distortion = enable ? 0.05f : 0.0f;
    UpdateUniforms();
}

void UIGlassEffect::EnableNoise(bool enable) {
    m_Settings.enableNoise = enable;
    m_Settings.noiseIntensity = enable ? 0.03f : 0.0f;
    UpdateUniforms();
}

void UIGlassEffect::UpdateUniforms() {
    if (!m_Initialized) return;

    m_BlurShader->Bind();
    m_BlurShader->SetFloat(m_BlurRadiusUniform, m_Settings.blurRadius);

    m_GlassShader->Bind();
    m_GlassShader->SetFloat(m_TransparencyUniform, m_Settings.transparency);
    m_GlassShader->SetFloat(m_RefractionUniform, m_Settings.refraction);
    m_GlassShader->SetFloat(m_ChromaticAberrationUniform, m_Settings.chromaticAberration);
    m_GlassShader->SetFloat(m_DistortionUniform, m_Settings.distortion);
    m_GlassShader->SetVec4(m_TintColorUniform, m_Settings.tintColor);
    m_GlassShader->SetFloat(m_NoiseIntensityUniform, m_Settings.noiseIntensity);
    m_GlassShader->SetFloat(m_TimeUniform, m_Time);
}

void UIGlassEffect::RenderBlurPass() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_BlurFBO);
    m_BlurShader->Bind();
    // Render full-screen quad with blur shader
    // ... (implement quad rendering)
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void UIGlassEffect::RenderGlassPass() {
    m_GlassShader->Bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_BlurTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_NoiseTexture);
    // Render full-screen quad with glass shader
    // ... (implement quad rendering)
}

}} // namespace VGE::UI 