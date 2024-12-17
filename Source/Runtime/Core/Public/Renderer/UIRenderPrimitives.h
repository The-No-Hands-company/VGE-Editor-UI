#pragma once

#include "Core/UICore.h"
#include "Runtime/Core/Public/Renderer/UIShader.h"
#include <glm/glm.hpp>
#include <string>
#include <memory>
#include <vector>

namespace VGE {
namespace UI {

struct GradientStop {
    glm::vec4 color;
    float position;  // 0.0 to 1.0
};

enum class GradientType {
    Linear,
    Radial,
    Angular
};

enum class PatternType {
    Checkerboard,
    Dots,
    Lines,
    CrossHatch,
    Custom
};

enum class BlendMode {
    Normal,
    Multiply,
    Screen,
    Overlay,
    SoftLight,
    HardLight,
    ColorDodge,
    ColorBurn
};

struct PatternSettings {
    PatternType type = PatternType::Checkerboard;
    glm::vec4 color1 = glm::vec4(1.0f);
    glm::vec4 color2 = glm::vec4(0.0f);
    float scale = 1.0f;
    float rotation = 0.0f;
    float density = 1.0f;  // For dots and lines
};

class UI_API UIRenderPrimitives {
public:
    UIRenderPrimitives();
    ~UIRenderPrimitives();

    bool Initialize();
    void Cleanup();

    // Basic primitives
    void DrawRect(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
    void DrawRectOutline(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color, float thickness = 1.0f);
    void DrawLine(const glm::vec2& start, const glm::vec2& end, const glm::vec4& color, float thickness = 1.0f);
    void DrawDashedLine(const glm::vec2& start, const glm::vec2& end, const glm::vec4& color, float thickness = 1.0f, 
                       float dashLength = 5.0f, float gapLength = 5.0f);
    void DrawCircle(const glm::vec2& center, float radius, const glm::vec4& color, int segments = 32);
    void DrawCircleOutline(const glm::vec2& center, float radius, const glm::vec4& color, float thickness = 1.0f, int segments = 32);
    void DrawTriangle(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3, const glm::vec4& color);

    // Advanced shapes
    void DrawRoundedRect(const glm::vec2& position, const glm::vec2& size, float radius, const glm::vec4& color);
    void DrawRoundedRectOutline(const glm::vec2& position, const glm::vec2& size, float radius, const glm::vec4& color, float thickness = 1.0f);
    void DrawEllipse(const glm::vec2& center, const glm::vec2& radii, const glm::vec4& color, int segments = 32);
    void DrawRegularPolygon(const glm::vec2& center, float radius, int sides, const glm::vec4& color);
    void DrawStar(const glm::vec2& center, float outerRadius, float innerRadius, int points, const glm::vec4& color);

    // Curves and arcs
    void DrawArc(const glm::vec2& center, float radius, float startAngle, float endAngle, const glm::vec4& color, int segments = 32);
    void DrawPieSegment(const glm::vec2& center, float radius, float startAngle, float endAngle, const glm::vec4& color, int segments = 32);
    void DrawBezier(const glm::vec2& start, const glm::vec2& control1, const glm::vec2& control2, const glm::vec2& end, const glm::vec4& color, int segments = 32);
    void DrawQuadraticBezier(const glm::vec2& start, const glm::vec2& control, const glm::vec2& end, const glm::vec4& color, int segments = 32);
    void DrawPolygon(const std::vector<glm::vec2>& points, const glm::vec4& color);

    // Gradient fills
    void DrawRectWithGradient(const glm::vec2& position, const glm::vec2& size, const std::vector<GradientStop>& stops, 
                             GradientType type = GradientType::Linear, float angle = 0.0f);
    void DrawCircleWithGradient(const glm::vec2& center, float radius, const std::vector<GradientStop>& stops, 
                               GradientType type = GradientType::Radial);

    // Effects
    void DrawRectWithShadow(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color,
                           const glm::vec4& shadowColor, const glm::vec2& shadowOffset, float shadowBlur);
    void DrawGlowingRect(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color,
                        const glm::vec4& glowColor, float glowSize);

    // Pattern fills
    void DrawRectWithPattern(const glm::vec2& position, const glm::vec2& size, 
                           const PatternSettings& pattern);
    void DrawCircleWithPattern(const glm::vec2& center, float radius,
                             const PatternSettings& pattern, int segments = 32);

    // Advanced effects
    void DrawInnerShadow(const glm::vec2& position, const glm::vec2& size,
                        const glm::vec4& shadowColor, float blur);
    void DrawBevel(const glm::vec2& position, const glm::vec2& size,
                  const glm::vec4& lightColor, const glm::vec4& shadowColor,
                  float size, bool raised = true);
    void DrawBlur(const glm::vec2& position, const glm::vec2& size,
                 float radius, bool gaussian = true);

    // Texture support
    void DrawTexturedRect(const glm::vec2& position, const glm::vec2& size,
                         uint32_t textureId, const glm::vec4& tint = glm::vec4(1.0f));
    void DrawTexturedRectWithBlend(const glm::vec2& position, const glm::vec2& size,
                                 uint32_t textureId, BlendMode blendMode,
                                 const glm::vec4& tint = glm::vec4(1.0f));

    // Path operations
    void BeginPath();
    void MoveTo(const glm::vec2& point);
    void LineTo(const glm::vec2& point);
    void BezierCurveTo(const glm::vec2& control1, const glm::vec2& control2,
                      const glm::vec2& end);
    void QuadraticCurveTo(const glm::vec2& control, const glm::vec2& end);
    void ClosePath();
    void StrokePath(const glm::vec4& color, float thickness = 1.0f,
                   bool roundCaps = true, bool roundJoins = true);
    void FillPath(const glm::vec4& color);

private:
    bool CreateShaders();
    bool CreateBuffers();
    void UpdateCircleBuffer(int segments);
    void DrawRoundedCorner(const glm::vec2& center, float radius, float startAngle, float endAngle, const glm::vec4& color, int segments = 8);
    void CreateGradientTexture(const std::vector<GradientStop>& stops);
    void CreatePatternTexture(const PatternSettings& pattern);
    void UpdateBlendShader(BlendMode mode);

private:
    std::shared_ptr<UIShader> m_Shader;
    std::shared_ptr<UIShader> m_GradientShader;
    std::shared_ptr<UIShader> m_EffectShader;
    std::shared_ptr<UIShader> m_PatternShader;
    std::shared_ptr<UIShader> m_TextureShader;
    std::shared_ptr<UIShader> m_BlendShader;
    
    std::string m_ColorUniformName;
    std::string m_TransformUniformName;
    std::string m_GradientTypeUniformName;
    std::string m_GradientAngleUniformName;
    std::string m_ShadowParamsUniformName;
    std::string m_PatternTypeUniformName;
    std::string m_PatternColor1UniformName;
    std::string m_PatternColor2UniformName;
    std::string m_PatternScaleUniformName;
    std::string m_PatternRotationUniformName;
    std::string m_PatternDensityUniformName;
    std::string m_BlendModeUniformName;

    uint32_t m_RectVBO;
    uint32_t m_RectVAO;
    uint32_t m_LineVBO;
    uint32_t m_LineVAO;
    uint32_t m_CircleVBO;
    uint32_t m_CircleVAO;
    uint32_t m_GradientTexture;
    uint32_t m_PatternTexture;
    std::vector<glm::vec2> m_PathPoints;
    bool m_PathStarted;
};

}} // namespace VGE::UI