#include "Runtime/Core/Public/Renderer/UIRenderPrimitives.h"
#include "Runtime/Core/Public/Core/Logger.h"
#include <glad/gl.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>

namespace VGE {
namespace UI {

UIRenderPrimitives::UIRenderPrimitives()
    : m_Shader(nullptr)
    , m_ColorUniformName("color")
    , m_TransformUniformName("transform")
    , m_RectVBO(0)
    , m_RectVAO(0)
    , m_LineVBO(0)
    , m_LineVAO(0)
    , m_CircleVBO(0)
    , m_CircleVAO(0)
    , m_GradientTexture(0)
{
}

UIRenderPrimitives::~UIRenderPrimitives() {
    Cleanup();
}

bool UIRenderPrimitives::Initialize() {
    if (!CreateShaders()) {
        Logger::Log(LogLevel::Error, "Failed to create shaders for render primitives");
        return false;
    }

    if (!CreateBuffers()) {
        Logger::Log(LogLevel::Error, "Failed to create buffers for render primitives");
        return false;
    }

    return true;
}

void UIRenderPrimitives::Cleanup() {
    if (m_RectVBO) glDeleteBuffers(1, &m_RectVBO);
    if (m_RectVAO) glDeleteVertexArrays(1, &m_RectVAO);
    if (m_LineVBO) glDeleteBuffers(1, &m_LineVBO);
    if (m_LineVAO) glDeleteVertexArrays(1, &m_LineVAO);
    if (m_CircleVBO) glDeleteBuffers(1, &m_CircleVBO);
    if (m_CircleVAO) glDeleteVertexArrays(1, &m_CircleVAO);

    m_RectVBO = m_RectVAO = 0;
    m_LineVBO = m_LineVAO = 0;
    m_CircleVBO = m_CircleVAO = 0;

    m_Shader.reset();
    m_GradientShader.reset();
}

bool UIRenderPrimitives::CreateShaders() {
    const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        uniform mat4 transform;
        void main() {
            gl_Position = transform * vec4(aPos, 0.0, 1.0);
        }
    )";

    const char* fragmentShaderSource = R"(
        #version 330 core
        uniform vec4 color;
        out vec4 FragColor;
        void main() {
            FragColor = color;
        }
    )";

    m_Shader = std::make_shared<UIShader>();
    if (!m_Shader->Initialize(vertexShaderSource, fragmentShaderSource)) {
        Logger::Log(LogLevel::Error, "Failed to initialize basic render primitives shader");
        return false;
    }

    const char* gradientVertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        layout (location = 1) in vec2 aTexCoord;
        uniform mat4 transform;
        out vec2 TexCoord;
        void main() {
            gl_Position = transform * vec4(aPos, 0.0, 1.0);
            TexCoord = aTexCoord;
        }
    )";

    const char* gradientFragmentShaderSource = R"(
        #version 330 core
        in vec2 TexCoord;
        uniform sampler2D gradientTex;
        uniform int gradientType;
        uniform float gradientAngle;
        out vec4 FragColor;

        void main() {
            float coord;
            if (gradientType == 0) { // Linear
                float angle = radians(gradientAngle);
                vec2 dir = vec2(cos(angle), sin(angle));
                coord = dot(TexCoord - 0.5, dir) + 0.5;
            }
            else if (gradientType == 1) { // Radial
                coord = length(TexCoord - 0.5) * 2.0;
            }
            else { // Angular
                coord = (atan(TexCoord.y - 0.5, TexCoord.x - 0.5) + 3.14159) / 6.28318;
            }
            FragColor = texture(gradientTex, vec2(coord, 0.5));
        }
    )";

    m_GradientShader = std::make_shared<UIShader>();
    if (!m_GradientShader->Initialize(gradientVertexShaderSource, gradientFragmentShaderSource)) {
        Logger::Log(LogLevel::Error, "Failed to initialize gradient shader");
        return false;
    }

    // Effect shader for shadows and glows
    const char* effectVertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        uniform mat4 transform;
        out vec2 FragPos;
        void main() {
            gl_Position = transform * vec4(aPos, 0.0, 1.0);
            FragPos = aPos;
        }
    )";

    const char* effectFragmentShaderSource = R"(
        #version 330 core
        in vec2 FragPos;
        uniform vec4 color;
        uniform vec2 size;
        uniform vec2 shadowOffset;
        uniform float blurRadius;
        out vec4 FragColor;

        float gaussian(float x, float sigma) {
            return exp(-(x * x) / (2.0 * sigma * sigma)) / (sqrt(2.0 * 3.14159) * sigma);
        }

        void main() {
            vec2 uv = FragPos;
            float alpha = 0.0;
            
            // Sample multiple points for blur effect
            const int SAMPLES = 9;
            float sigma = blurRadius * 0.5;
            
            for (int x = -SAMPLES; x <= SAMPLES; x++) {
                for (int y = -SAMPLES; y <= SAMPLES; y++) {
                    vec2 offset = vec2(x, y) * (blurRadius / float(SAMPLES));
                    vec2 samplePos = (uv + offset - shadowOffset) / size;
                    
                    if (samplePos.x >= 0.0 && samplePos.x <= 1.0 &&
                        samplePos.y >= 0.0 && samplePos.y <= 1.0) {
                        float weight = gaussian(length(offset), sigma);
                        alpha += weight;
                    }
                }
            }
            
            alpha = smoothstep(0.0, 1.0, alpha);
            FragColor = vec4(color.rgb, color.a * alpha);
        }
    )";

    m_EffectShader = std::make_shared<UIShader>();
    if (!m_EffectShader->Initialize(effectVertexShaderSource, effectFragmentShaderSource)) {
        Logger::Log(LogLevel::Error, "Failed to initialize effect shader");
        return false;
    }

    // Pattern shader
    const char* patternVertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        layout (location = 1) in vec2 aTexCoord;
        uniform mat4 transform;
        out vec2 TexCoord;
        void main() {
            gl_Position = transform * vec4(aPos, 0.0, 1.0);
            TexCoord = aTexCoord;
        }
    )";

    const char* patternFragmentShaderSource = R"(
        #version 330 core
        in vec2 TexCoord;
        uniform int patternType;
        uniform vec4 color1;
        uniform vec4 color2;
        uniform float scale;
        uniform float rotation;
        uniform float density;
        out vec4 FragColor;

        vec2 rotateUV(vec2 uv, float angle) {
            float s = sin(angle);
            float c = cos(angle);
            mat2 rot = mat2(c, -s, s, c);
            return (rot * (uv - 0.5)) + 0.5;
        }

        void main() {
            vec2 uv = rotateUV(TexCoord, rotation);
            uv *= scale;
            
            if (patternType == 0) { // Checkerboard
                vec2 grid = floor(uv * 2.0);
                float pattern = mod(grid.x + grid.y, 2.0);
                FragColor = mix(color1, color2, pattern);
            }
            else if (patternType == 1) { // Dots
                vec2 grid = fract(uv * density);
                float dist = length(grid - 0.5);
                float circle = smoothstep(0.5, 0.45, dist);
                FragColor = mix(color2, color1, circle);
            }
            else if (patternType == 2) { // Lines
                float line = mod(floor(uv.x * density), 2.0);
                FragColor = mix(color1, color2, line);
            }
            else if (patternType == 3) { // CrossHatch
                float line1 = mod(floor(uv.x * density), 2.0);
                float line2 = mod(floor(uv.y * density), 2.0);
                float pattern = max(line1, line2);
                FragColor = mix(color1, color2, pattern);
            }
            else { // Custom
                FragColor = color1;
            }
        }
    )";

    m_PatternShader = std::make_shared<UIShader>();
    if (!m_PatternShader->Initialize(patternVertexShaderSource, patternFragmentShaderSource)) {
        Logger::Log(LogLevel::Error, "Failed to initialize pattern shader");
        return false;
    }

    // Texture shader with blending
    const char* textureVertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        layout (location = 1) in vec2 aTexCoord;
        uniform mat4 transform;
        out vec2 TexCoord;
        void main() {
            gl_Position = transform * vec4(aPos, 0.0, 1.0);
            TexCoord = aTexCoord;
        }
    )";

    const char* textureFragmentShaderSource = R"(
        #version 330 core
        in vec2 TexCoord;
        uniform sampler2D texSampler;
        uniform vec4 tint;
        uniform int blendMode;
        out vec4 FragColor;

        vec4 blend(vec4 src, vec4 dst) {
            if (blendMode == 0) return src; // Normal
            else if (blendMode == 1) return src * dst; // Multiply
            else if (blendMode == 2) return vec4(1.0) - ((vec4(1.0) - src) * (vec4(1.0) - dst)); // Screen
            else if (blendMode == 3) { // Overlay
                vec4 result;
                for(int i = 0; i < 3; i++) {
                    if(dst[i] < 0.5)
                        result[i] = 2.0 * src[i] * dst[i];
                    else
                        result[i] = 1.0 - 2.0 * (1.0 - src[i]) * (1.0 - dst[i]);
                }
                result.a = src.a;
                return result;
            }
            // Add more blend modes as needed
            return src;
        }

        void main() {
            vec4 texColor = texture(texSampler, TexCoord);
            FragColor = blend(texColor * tint, vec4(1.0));
        }
    )";

    m_TextureShader = std::make_shared<UIShader>();
    if (!m_TextureShader->Initialize(textureVertexShaderSource, textureFragmentShaderSource)) {
        Logger::Log(LogLevel::Error, "Failed to initialize texture shader");
        return false;
    }

    return true;
}

bool UIRenderPrimitives::CreateBuffers() {
    // Create rectangle buffers
    glGenVertexArrays(1, &m_RectVAO);
    glGenBuffers(1, &m_RectVBO);

    glBindVertexArray(m_RectVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_RectVBO);

    const float rectVertices[] = {
        0.0f, 0.0f,  // Bottom-left
        1.0f, 0.0f,  // Bottom-right
        1.0f, 1.0f,  // Top-right
        0.0f, 1.0f   // Top-left
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(rectVertices), rectVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    // Create line buffers
    glGenVertexArrays(1, &m_LineVAO);
    glGenBuffers(1, &m_LineVBO);

    glBindVertexArray(m_LineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_LineVBO);

    const float lineVertices[] = {
        0.0f, 0.0f,  // Start
        1.0f, 1.0f   // End
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    // Create circle buffers (initial size, will be updated as needed)
    glGenVertexArrays(1, &m_CircleVAO);
    glGenBuffers(1, &m_CircleVBO);

    glBindVertexArray(m_CircleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_CircleVBO);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    // Reset state
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return true;
}

void UIRenderPrimitives::DrawRect(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color) {
    m_Shader->Bind();
    m_Shader->SetVec4(m_ColorUniformName, color);

    // Create transform matrix
    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, glm::vec3(position, 0.0f));
    transform = glm::scale(transform, glm::vec3(size, 1.0f));
    m_Shader->SetMat4(m_TransformUniformName, transform);

    glBindVertexArray(m_RectVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);
}

void UIRenderPrimitives::DrawRectOutline(const glm::vec2& position, const glm::vec2& size,
                                       const glm::vec4& color, float thickness) {
    // Draw four lines for the outline
    DrawLine(position, position + glm::vec2(size.x, 0.0f), color, thickness);
    DrawLine(position + glm::vec2(size.x, 0.0f), position + size, color, thickness);
    DrawLine(position + size, position + glm::vec2(0.0f, size.y), color, thickness);
    DrawLine(position + glm::vec2(0.0f, size.y), position, color, thickness);
}

void UIRenderPrimitives::DrawLine(const glm::vec2& start, const glm::vec2& end,
                                const glm::vec4& color, float thickness) {
    m_Shader->Bind();
    m_Shader->SetVec4(m_ColorUniformName, color);

    // Calculate line direction and perpendicular
    glm::vec2 dir = end - start;
    float length = glm::length(dir);
    if (length < 0.0001f) return;

    dir /= length;
    glm::vec2 perp(-dir.y, dir.x);
    perp *= thickness * 0.5f;

    // Update line vertices
    float vertices[] = {
        start.x + perp.x, start.y + perp.y,
        start.x - perp.x, start.y - perp.y,
        end.x + perp.x, end.y + perp.y,
        end.x - perp.x, end.y - perp.y
    };

    glBindVertexArray(m_LineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_LineVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

    glm::mat4 transform = glm::mat4(1.0f);
    m_Shader->SetMat4(m_TransformUniformName, transform);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void UIRenderPrimitives::UpdateCircleBuffer(int segments) {
    std::vector<float> vertices;
    vertices.reserve((segments + 2) * 2);

    // Center vertex
    vertices.push_back(0.0f);
    vertices.push_back(0.0f);

    // Circle vertices
    for (int i = 0; i <= segments; ++i) {
        float angle = (2.0f * glm::pi<float>() * i) / segments;
        vertices.push_back(std::cos(angle));
        vertices.push_back(std::sin(angle));
    }

    glBindVertexArray(m_CircleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_CircleVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
}

void UIRenderPrimitives::DrawCircle(const glm::vec2& center, float radius,
                                  const glm::vec4& color, int segments) {
    m_Shader->Bind();
    m_Shader->SetVec4(m_ColorUniformName, color);

    UpdateCircleBuffer(segments);

    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, glm::vec3(center, 0.0f));
    transform = glm::scale(transform, glm::vec3(radius, radius, 1.0f));
    m_Shader->SetMat4(m_TransformUniformName, transform);

    glBindVertexArray(m_CircleVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, segments + 2);
    glBindVertexArray(0);
}

void UIRenderPrimitives::DrawCircleOutline(const glm::vec2& center, float radius,
                                         const glm::vec4& color, float thickness,
                                         int segments) {
    // Draw inner and outer circles
    DrawCircle(center, radius - thickness * 0.5f, color, segments);
    DrawCircle(center, radius + thickness * 0.5f, color, segments);
}

void UIRenderPrimitives::DrawTriangle(const glm::vec2& p1, const glm::vec2& p2,
                                    const glm::vec2& p3, const glm::vec4& color) {
    m_Shader->Bind();
    m_Shader->SetVec4(m_ColorUniformName, color);

    float vertices[] = {
        p1.x, p1.y,
        p2.x, p2.y,
        p3.x, p3.y
    };

    glBindVertexArray(m_LineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_LineVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

    glm::mat4 transform = glm::mat4(1.0f);
    m_Shader->SetMat4(m_TransformUniformName, transform);

    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}

void UIRenderPrimitives::DrawRoundedRect(const glm::vec2& position, const glm::vec2& size, float radius, const glm::vec4& color) {
    // Clamp radius to half of the smallest dimension
    radius = std::min(radius, std::min(size.x * 0.5f, size.y * 0.5f));

    // Draw the center rectangle
    DrawRect(position + glm::vec2(radius, 0.0f), 
            glm::vec2(size.x - 2.0f * radius, size.y), color);
    
    // Draw the side rectangles
    DrawRect(position + glm::vec2(0.0f, radius),
            glm::vec2(radius, size.y - 2.0f * radius), color);
    DrawRect(position + glm::vec2(size.x - radius, radius),
            glm::vec2(radius, size.y - 2.0f * radius), color);

    // Draw the four corners
    DrawRoundedCorner(position + glm::vec2(radius, radius), radius, glm::pi<float>(), 1.5f * glm::pi<float>(), color);
    DrawRoundedCorner(position + glm::vec2(size.x - radius, radius), radius, 1.5f * glm::pi<float>(), 2.0f * glm::pi<float>(), color);
    DrawRoundedCorner(position + glm::vec2(size.x - radius, size.y - radius), radius, 0.0f, 0.5f * glm::pi<float>(), color);
    DrawRoundedCorner(position + glm::vec2(radius, size.y - radius), radius, 0.5f * glm::pi<float>(), glm::pi<float>(), color);
}

void UIRenderPrimitives::DrawRoundedRectOutline(const glm::vec2& position, const glm::vec2& size, float radius, const glm::vec4& color, float thickness) {
    // Clamp radius to half of the smallest dimension
    radius = std::min(radius, std::min(size.x * 0.5f, size.y * 0.5f));

    // Draw horizontal lines
    DrawLine(position + glm::vec2(radius, 0.0f), 
            position + glm::vec2(size.x - radius, 0.0f), color, thickness);
    DrawLine(position + glm::vec2(radius, size.y), 
            position + glm::vec2(size.x - radius, size.y), color, thickness);

    // Draw vertical lines
    DrawLine(position + glm::vec2(0.0f, radius), 
            position + glm::vec2(0.0f, size.y - radius), color, thickness);
    DrawLine(position + glm::vec2(size.x, radius), 
            position + glm::vec2(size.x, size.y - radius), color, thickness);

    // Draw the four corners as arcs
    DrawArc(position + glm::vec2(radius, radius), radius, glm::pi<float>(), 1.5f * glm::pi<float>(), color);
    DrawArc(position + glm::vec2(size.x - radius, radius), radius, 1.5f * glm::pi<float>(), 2.0f * glm::pi<float>(), color);
    DrawArc(position + glm::vec2(size.x - radius, size.y - radius), radius, 0.0f, 0.5f * glm::pi<float>(), color);
    DrawArc(position + glm::vec2(radius, size.y - radius), radius, 0.5f * glm::pi<float>(), glm::pi<float>(), color);
}

void UIRenderPrimitives::DrawRoundedCorner(const glm::vec2& center, float radius, float startAngle, float endAngle, const glm::vec4& color, int segments) {
    m_Shader->Bind();
    m_Shader->SetVec4(m_ColorUniformName, color);

    std::vector<float> vertices;
    vertices.reserve((segments + 2) * 2);

    // Center vertex
    vertices.push_back(center.x);
    vertices.push_back(center.y);

    // Arc vertices
    float angleStep = (endAngle - startAngle) / segments;
    for (int i = 0; i <= segments; ++i) {
        float angle = startAngle + angleStep * i;
        vertices.push_back(center.x + radius * std::cos(angle));
        vertices.push_back(center.y + radius * std::sin(angle));
    }

    glBindVertexArray(m_CircleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_CircleVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

    glm::mat4 transform = glm::mat4(1.0f);
    m_Shader->SetMat4(m_TransformUniformName, transform);

    glDrawArrays(GL_TRIANGLE_FAN, 0, segments + 2);
    glBindVertexArray(0);
}

void UIRenderPrimitives::DrawArc(const glm::vec2& center, float radius, float startAngle, float endAngle, const glm::vec4& color, int segments) {
    m_Shader->Bind();
    m_Shader->SetVec4(m_ColorUniformName, color);

    std::vector<float> vertices;
    vertices.reserve((segments + 1) * 2);

    float angleStep = (endAngle - startAngle) / segments;
    for (int i = 0; i <= segments; ++i) {
        float angle = startAngle + angleStep * i;
        vertices.push_back(center.x + radius * std::cos(angle));
        vertices.push_back(center.y + radius * std::sin(angle));
    }

    glBindVertexArray(m_LineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_LineVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

    glm::mat4 transform = glm::mat4(1.0f);
    m_Shader->SetMat4(m_TransformUniformName, transform);

    glDrawArrays(GL_LINE_STRIP, 0, segments + 1);
    glBindVertexArray(0);
}

void UIRenderPrimitives::DrawBezier(const glm::vec2& start, const glm::vec2& control1, const glm::vec2& control2, const glm::vec2& end, const glm::vec4& color, int segments) {
    m_Shader->Bind();
    m_Shader->SetVec4(m_ColorUniformName, color);

    std::vector<float> vertices;
    vertices.reserve((segments + 1) * 2);

    for (int i = 0; i <= segments; ++i) {
        float t = static_cast<float>(i) / segments;
        float u = 1.0f - t;
        float tt = t * t;
        float uu = u * u;
        float uuu = uu * u;
        float ttt = tt * t;

        float x = uuu * start.x + 3 * uu * t * control1.x + 3 * u * tt * control2.x + ttt * end.x;
        float y = uuu * start.y + 3 * uu * t * control1.y + 3 * u * tt * control2.y + ttt * end.y;

        vertices.push_back(x);
        vertices.push_back(y);
    }

    glBindVertexArray(m_LineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_LineVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

    glm::mat4 transform = glm::mat4(1.0f);
    m_Shader->SetMat4(m_TransformUniformName, transform);

    glDrawArrays(GL_LINE_STRIP, 0, segments + 1);
    glBindVertexArray(0);
}

void UIRenderPrimitives::DrawPolygon(const std::vector<glm::vec2>& points, const glm::vec4& color) {
    if (points.size() < 3) return;

    m_Shader->Bind();
    m_Shader->SetVec4(m_ColorUniformName, color);

    std::vector<float> vertices;
    vertices.reserve(points.size() * 2);

    for (const auto& point : points) {
        vertices.push_back(point.x);
        vertices.push_back(point.y);
    }

    glBindVertexArray(m_LineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_LineVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

    glm::mat4 transform = glm::mat4(1.0f);
    m_Shader->SetMat4(m_TransformUniformName, transform);

    glDrawArrays(GL_TRIANGLE_FAN, 0, points.size());
    glBindVertexArray(0);
}

void UIRenderPrimitives::CreateGradientTexture(const std::vector<GradientStop>& stops) {
    if (stops.empty()) return;

    const int TEXTURE_WIDTH = 256;
    std::vector<glm::vec4> textureData(TEXTURE_WIDTH);

    // Sort stops by position
    std::vector<GradientStop> sortedStops = stops;
    std::sort(sortedStops.begin(), sortedStops.end(),
              [](const GradientStop& a, const GradientStop& b) { return a.position < b.position; });

    // Ensure we have stops at 0 and 1
    if (sortedStops.front().position > 0.0f) {
        sortedStops.insert(sortedStops.begin(), {sortedStops.front().color, 0.0f});
    }
    if (sortedStops.back().position < 1.0f) {
        sortedStops.push_back({sortedStops.back().color, 1.0f});
    }

    // Generate texture data
    for (int i = 0; i < TEXTURE_WIDTH; ++i) {
        float pos = static_cast<float>(i) / (TEXTURE_WIDTH - 1);
        
        // Find surrounding stops
        size_t nextStop = 0;
        while (nextStop < sortedStops.size() && sortedStops[nextStop].position < pos) {
            ++nextStop;
        }
        
        if (nextStop == 0) {
            textureData[i] = sortedStops[0].color;
        }
        else if (nextStop >= sortedStops.size()) {
            textureData[i] = sortedStops.back().color;
        }
        else {
            const auto& stop1 = sortedStops[nextStop - 1];
            const auto& stop2 = sortedStops[nextStop];
            float t = (pos - stop1.position) / (stop2.position - stop1.position);
            textureData[i] = glm::mix(stop1.color, stop2.color, t);
        }
    }

    // Create and update texture
    if (m_GradientTexture == 0) {
        glGenTextures(1, &m_GradientTexture);
    }
    
    glBindTexture(GL_TEXTURE_1D, m_GradientTexture);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA32F, TEXTURE_WIDTH, 0, GL_RGBA, GL_FLOAT, textureData.data());
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
}

void UIRenderPrimitives::DrawRectWithGradient(const glm::vec2& position, const glm::vec2& size,
                                            const std::vector<GradientStop>& stops,
                                            GradientType type, float angle) {
    CreateGradientTexture(stops);
    
    m_GradientShader->Bind();
    m_GradientShader->SetInt("gradientType", static_cast<int>(type));
    m_GradientShader->SetFloat("gradientAngle", angle);
    
    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, glm::vec3(position, 0.0f));
    transform = glm::scale(transform, glm::vec3(size, 1.0f));
    m_GradientShader->SetMat4(m_TransformUniformName, transform);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_1D, m_GradientTexture);
    
    glBindVertexArray(m_RectVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);
}

void UIRenderPrimitives::DrawDashedLine(const glm::vec2& start, const glm::vec2& end,
                                      const glm::vec4& color, float thickness,
                                      float dashLength, float gapLength) {
    glm::vec2 dir = end - start;
    float length = glm::length(dir);
    if (length < 0.0001f) return;

    dir /= length;
    float totalLength = dashLength + gapLength;
    int numSegments = static_cast<int>(length / totalLength);

    glm::vec2 currentStart = start;
    for (int i = 0; i < numSegments; ++i) {
        glm::vec2 dashEnd = currentStart + dir * dashLength;
        DrawLine(currentStart, dashEnd, color, thickness);
        currentStart += dir * totalLength;
    }

    // Draw remaining dash if any
    float remainingLength = length - (numSegments * totalLength);
    if (remainingLength > 0.0f) {
        float finalDashLength = std::min(remainingLength, dashLength);
        DrawLine(currentStart, currentStart + dir * finalDashLength, color, thickness);
    }
}

void UIRenderPrimitives::DrawEllipse(const glm::vec2& center, const glm::vec2& radii,
                                   const glm::vec4& color, int segments) {
    m_Shader->Bind();
    m_Shader->SetVec4(m_ColorUniformName, color);

    std::vector<float> vertices;
    vertices.reserve((segments + 2) * 2);

    // Center vertex
    vertices.push_back(center.x);
    vertices.push_back(center.y);

    // Ellipse vertices
    for (int i = 0; i <= segments; ++i) {
        float angle = (2.0f * glm::pi<float>() * i) / segments;
        vertices.push_back(center.x + radii.x * std::cos(angle));
        vertices.push_back(center.y + radii.y * std::sin(angle));
    }

    glBindVertexArray(m_CircleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_CircleVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

    glm::mat4 transform = glm::mat4(1.0f);
    m_Shader->SetMat4(m_TransformUniformName, transform);

    glDrawArrays(GL_TRIANGLE_FAN, 0, segments + 2);
    glBindVertexArray(0);
}

void UIRenderPrimitives::DrawRegularPolygon(const glm::vec2& center, float radius,
                                          int sides, const glm::vec4& color) {
    if (sides < 3) return;

    m_Shader->Bind();
    m_Shader->SetVec4(m_ColorUniformName, color);

    std::vector<float> vertices;
    vertices.reserve((sides + 2) * 2);

    // Center vertex
    vertices.push_back(center.x);
    vertices.push_back(center.y);

    // Polygon vertices
    for (int i = 0; i <= sides; ++i) {
        float angle = (2.0f * glm::pi<float>() * i) / sides;
        vertices.push_back(center.x + radius * std::cos(angle));
        vertices.push_back(center.y + radius * std::sin(angle));
    }

    glBindVertexArray(m_CircleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_CircleVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

    glm::mat4 transform = glm::mat4(1.0f);
    m_Shader->SetMat4(m_TransformUniformName, transform);

    glDrawArrays(GL_TRIANGLE_FAN, 0, sides + 2);
    glBindVertexArray(0);
}

void UIRenderPrimitives::DrawStar(const glm::vec2& center, float outerRadius,
                                float innerRadius, int points, const glm::vec4& color) {
    if (points < 3) return;

    m_Shader->Bind();
    m_Shader->SetVec4(m_ColorUniformName, color);

    std::vector<float> vertices;
    vertices.reserve((points * 2 + 2) * 2);

    // Center vertex
    vertices.push_back(center.x);
    vertices.push_back(center.y);

    // Star vertices
    for (int i = 0; i <= points * 2; ++i) {
        float angle = (2.0f * glm::pi<float>() * i) / (points * 2);
        float radius = (i % 2 == 0) ? outerRadius : innerRadius;
        vertices.push_back(center.x + radius * std::cos(angle));
        vertices.push_back(center.y + radius * std::sin(angle));
    }

    glBindVertexArray(m_CircleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_CircleVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

    glm::mat4 transform = glm::mat4(1.0f);
    m_Shader->SetMat4(m_TransformUniformName, transform);

    glDrawArrays(GL_TRIANGLE_FAN, 0, points * 2 + 2);
    glBindVertexArray(0);
}

void UIRenderPrimitives::DrawPieSegment(const glm::vec2& center, float radius,
                                      float startAngle, float endAngle,
                                      const glm::vec4& color, int segments) {
    m_Shader->Bind();
    m_Shader->SetVec4(m_ColorUniformName, color);

    std::vector<float> vertices;
    vertices.reserve((segments + 2) * 2);

    // Center vertex
    vertices.push_back(center.x);
    vertices.push_back(center.y);

    // Arc vertices
    float angleStep = (endAngle - startAngle) / segments;
    for (int i = 0; i <= segments; ++i) {
        float angle = startAngle + angleStep * i;
        vertices.push_back(center.x + radius * std::cos(angle));
        vertices.push_back(center.y + radius * std::sin(angle));
    }

    glBindVertexArray(m_CircleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_CircleVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

    glm::mat4 transform = glm::mat4(1.0f);
    m_Shader->SetMat4(m_TransformUniformName, transform);

    glDrawArrays(GL_TRIANGLE_FAN, 0, segments + 2);
    glBindVertexArray(0);
}

void UIRenderPrimitives::DrawRectWithShadow(const glm::vec2& position, const glm::vec2& size,
                                          const glm::vec4& color, const glm::vec4& shadowColor,
                                          const glm::vec2& shadowOffset, float shadowBlur) {
    // Draw shadow first
    m_EffectShader->Bind();
    m_EffectShader->SetVec4("color", shadowColor);
    m_EffectShader->SetVec2("size", size);
    m_EffectShader->SetVec2("shadowOffset", shadowOffset);
    m_EffectShader->SetFloat("blurRadius", shadowBlur);

    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, glm::vec3(position + shadowOffset, 0.0f));
    transform = glm::scale(transform, glm::vec3(size, 1.0f));
    m_EffectShader->SetMat4(m_TransformUniformName, transform);

    glBindVertexArray(m_RectVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    // Draw main rectangle
    DrawRect(position, size, color);
}

void UIRenderPrimitives::DrawGlowingRect(const glm::vec2& position, const glm::vec2& size,
                                       const glm::vec4& color, const glm::vec4& glowColor,
                                       float glowSize) {
    // Draw glow effect
    m_EffectShader->Bind();
    m_EffectShader->SetVec4("color", glowColor);
    m_EffectShader->SetVec2("size", size + glm::vec2(glowSize * 2.0f));
    m_EffectShader->SetVec2("shadowOffset", glm::vec2(0.0f));
    m_EffectShader->SetFloat("blurRadius", glowSize);

    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, glm::vec3(position - glm::vec2(glowSize), 0.0f));
    transform = glm::scale(transform, glm::vec3(size + glm::vec2(glowSize * 2.0f), 1.0f));
    m_EffectShader->SetMat4(m_TransformUniformName, transform);

    glBindVertexArray(m_RectVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    // Draw main rectangle
    DrawRect(position, size, color);
}

void UIRenderPrimitives::DrawRectWithPattern(const glm::vec2& position, const glm::vec2& size,
                                           const PatternSettings& pattern) {
    m_PatternShader->Bind();
    m_PatternShader->SetInt("patternType", static_cast<int>(pattern.type));
    m_PatternShader->SetVec4("color1", pattern.color1);
    m_PatternShader->SetVec4("color2", pattern.color2);
    m_PatternShader->SetFloat("scale", pattern.scale);
    m_PatternShader->SetFloat("rotation", pattern.rotation);
    m_PatternShader->SetFloat("density", pattern.density);

    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, glm::vec3(position, 0.0f));
    transform = glm::scale(transform, glm::vec3(size, 1.0f));
    m_PatternShader->SetMat4(m_TransformUniformName, transform);

    glBindVertexArray(m_RectVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);
}

void UIRenderPrimitives::DrawCircleWithPattern(const glm::vec2& center, float radius,
                                             const PatternSettings& pattern, int segments) {
    m_PatternShader->Bind();
    m_PatternShader->SetInt("patternType", static_cast<int>(pattern.type));
    m_PatternShader->SetVec4("color1", pattern.color1);
    m_PatternShader->SetVec4("color2", pattern.color2);
    m_PatternShader->SetFloat("scale", pattern.scale);
    m_PatternShader->SetFloat("rotation", pattern.rotation);
    m_PatternShader->SetFloat("density", pattern.density);

    UpdateCircleBuffer(segments);

    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, glm::vec3(center, 0.0f));
    transform = glm::scale(transform, glm::vec3(radius, radius, 1.0f));
    m_PatternShader->SetMat4(m_TransformUniformName, transform);

    glBindVertexArray(m_CircleVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, segments + 2);
    glBindVertexArray(0);
}

void UIRenderPrimitives::DrawTexturedRect(const glm::vec2& position, const glm::vec2& size,
                                        uint32_t textureId, const glm::vec4& tint) {
    m_TextureShader->Bind();
    m_TextureShader->SetInt("blendMode", 0); // Normal blend
    m_TextureShader->SetVec4("tint", tint);

    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, glm::vec3(position, 0.0f));
    transform = glm::scale(transform, glm::vec3(size, 1.0f));
    m_TextureShader->SetMat4(m_TransformUniformName, transform);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId);

    glBindVertexArray(m_RectVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);
}

void UIRenderPrimitives::DrawTexturedRectWithBlend(const glm::vec2& position, const glm::vec2& size,
                                                 uint32_t textureId, BlendMode blendMode,
                                                 const glm::vec4& tint) {
    m_TextureShader->Bind();
    m_TextureShader->SetInt("blendMode", static_cast<int>(blendMode));
    m_TextureShader->SetVec4("tint", tint);

    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, glm::vec3(position, 0.0f));
    transform = glm::scale(transform, glm::vec3(size, 1.0f));
    m_TextureShader->SetMat4(m_TransformUniformName, transform);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId);

    glBindVertexArray(m_RectVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);
}

void UIRenderPrimitives::BeginPath() {
    m_PathPoints.clear();
    m_PathStarted = true;
}

void UIRenderPrimitives::MoveTo(const glm::vec2& point) {
    if (!m_PathStarted) return;
    if (!m_PathPoints.empty()) {
        m_PathPoints.push_back(m_PathPoints.back()); // Disconnect from previous segment
    }
    m_PathPoints.push_back(point);
}

void UIRenderPrimitives::LineTo(const glm::vec2& point) {
    if (!m_PathStarted) return;
    m_PathPoints.push_back(point);
}

void UIRenderPrimitives::BezierCurveTo(const glm::vec2& control1, const glm::vec2& control2,
                                     const glm::vec2& end) {
    if (!m_PathStarted || m_PathPoints.empty()) return;

    const int segments = 30;
    const glm::vec2 start = m_PathPoints.back();

    for (int i = 1; i <= segments; ++i) {
        float t = static_cast<float>(i) / segments;
        float u = 1.0f - t;
        float tt = t * t;
        float uu = u * u;
        float uuu = uu * u;
        float ttt = tt * t;

        glm::vec2 point = uuu * start +
                         3.0f * uu * t * control1 +
                         3.0f * u * tt * control2 +
                         ttt * end;
        m_PathPoints.push_back(point);
    }
}

void UIRenderPrimitives::QuadraticCurveTo(const glm::vec2& control, const glm::vec2& end) {
    if (!m_PathStarted || m_PathPoints.empty()) return;

    const int segments = 30;
    const glm::vec2 start = m_PathPoints.back();

    for (int i = 1; i <= segments; ++i) {
        float t = static_cast<float>(i) / segments;
        float u = 1.0f - t;
        float tt = t * t;
        float uu = u * u;

        glm::vec2 point = uu * start +
                         2.0f * u * t * control +
                         tt * end;
        m_PathPoints.push_back(point);
    }
}

void UIRenderPrimitives::ClosePath() {
    if (!m_PathStarted || m_PathPoints.size() < 2) return;
    m_PathPoints.push_back(m_PathPoints.front());
    m_PathStarted = false;
}

void UIRenderPrimitives::StrokePath(const glm::vec4& color, float thickness,
                                  bool roundCaps, bool roundJoins) {
    if (m_PathPoints.size() < 2) return;

    for (size_t i = 1; i < m_PathPoints.size(); ++i) {
        DrawLine(m_PathPoints[i - 1], m_PathPoints[i], color, thickness);

        // Draw rounded joins if enabled
        if (roundJoins && i > 1 && i < m_PathPoints.size() - 1) {
            DrawCircle(m_PathPoints[i], thickness * 0.5f, color);
        }
    }

    // Draw rounded caps if enabled
    if (roundCaps && !m_PathStarted) {
        DrawCircle(m_PathPoints.front(), thickness * 0.5f, color);
        DrawCircle(m_PathPoints.back(), thickness * 0.5f, color);
    }
}

void UIRenderPrimitives::FillPath(const glm::vec4& color) {
    if (m_PathPoints.size() < 3) return;

    m_Shader->Bind();
    m_Shader->SetVec4(m_ColorUniformName, color);

    std::vector<float> vertices;
    vertices.reserve(m_PathPoints.size() * 2);

    for (const auto& point : m_PathPoints) {
        vertices.push_back(point.x);
        vertices.push_back(point.y);
    }

    glBindVertexArray(m_LineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_LineVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

    glm::mat4 transform = glm::mat4(1.0f);
    m_Shader->SetMat4(m_TransformUniformName, transform);

    glDrawArrays(GL_TRIANGLE_FAN, 0, m_PathPoints.size());
    glBindVertexArray(0);
}

void UIRenderPrimitives::DrawInnerShadow(const glm::vec2& position, const glm::vec2& size,
                                       const glm::vec4& shadowColor, float blur) {
    // Draw four inner shadow edges
    const float maxAlpha = shadowColor.a;
    const int samples = 20;

    // Top shadow
    for (int i = 0; i < samples; ++i) {
        float t = static_cast<float>(i) / samples;
        float alpha = maxAlpha * (1.0f - t);
        float y = blur * t;
        DrawRect(position + glm::vec2(0, y),
                glm::vec2(size.x, 1),
                glm::vec4(shadowColor.r, shadowColor.g, shadowColor.b, alpha));
    }

    // Bottom shadow
    for (int i = 0; i < samples; ++i) {
        float t = static_cast<float>(i) / samples;
        float alpha = maxAlpha * (1.0f - t);
        float y = size.y - blur * t;
        DrawRect(position + glm::vec2(0, y),
                glm::vec2(size.x, 1),
                glm::vec4(shadowColor.r, shadowColor.g, shadowColor.b, alpha));
    }

    // Left shadow
    for (int i = 0; i < samples; ++i) {
        float t = static_cast<float>(i) / samples;
        float alpha = maxAlpha * (1.0f - t);
        float x = blur * t;
        DrawRect(position + glm::vec2(x, 0),
                glm::vec2(1, size.y),
                glm::vec4(shadowColor.r, shadowColor.g, shadowColor.b, alpha));
    }

    // Right shadow
    for (int i = 0; i < samples; ++i) {
        float t = static_cast<float>(i) / samples;
        float alpha = maxAlpha * (1.0f - t);
        float x = size.x - blur * t;
        DrawRect(position + glm::vec2(x, 0),
                glm::vec2(1, size.y),
                glm::vec4(shadowColor.r, shadowColor.g, shadowColor.b, alpha));
    }
}

void UIRenderPrimitives::DrawBevel(const glm::vec2& position, const glm::vec2& size,
                                 const glm::vec4& lightColor, const glm::vec4& shadowColor,
                                 float bevelSize, bool raised) {
    const glm::vec4& topColor = raised ? lightColor : shadowColor;
    const glm::vec4& bottomColor = raised ? shadowColor : lightColor;

    // Top edge
    DrawRect(position,
            glm::vec2(size.x, bevelSize),
            topColor);

    // Bottom edge
    DrawRect(position + glm::vec2(0, size.y - bevelSize),
            glm::vec2(size.x, bevelSize),
            bottomColor);

    // Left edge
    DrawRect(position + glm::vec2(0, bevelSize),
            glm::vec2(bevelSize, size.y - 2 * bevelSize),
            topColor);

    // Right edge
    DrawRect(position + glm::vec2(size.x - bevelSize, bevelSize),
            glm::vec2(bevelSize, size.y - 2 * bevelSize),
            bottomColor);

    // Corners
    DrawRect(position,
            glm::vec2(bevelSize),
            topColor);
    DrawRect(position + glm::vec2(size.x - bevelSize, 0),
            glm::vec2(bevelSize),
            raised ? glm::mix(topColor, bottomColor, 0.5f) : bottomColor);
    DrawRect(position + glm::vec2(0, size.y - bevelSize),
            glm::vec2(bevelSize),
            raised ? glm::mix(topColor, bottomColor, 0.5f) : bottomColor);
    DrawRect(position + size - glm::vec2(bevelSize),
            glm::vec2(bevelSize),
            bottomColor);
}

void UIRenderPrimitives::DrawBlur(const glm::vec2& position, const glm::vec2& size,
                                float radius, bool gaussian) {
    // Implementation would require framebuffer support and multiple passes
    // This is a placeholder that could be implemented with proper render target support
    Logger::Log(LogLevel::Warning, "Blur effect requires render target support");
}

}} // namespace VGE::UI 