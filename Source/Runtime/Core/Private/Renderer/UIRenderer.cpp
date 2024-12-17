#include "Runtime/Core/Public/Renderer/UIRenderer.h"
#include "Runtime/Core/Public/Renderer/UIShader.h"
#include "Runtime/Core/Public/Renderer/UIFontRenderer.h"
#include "Runtime/Core/Public/Renderer/UITexture.h"
#include "Runtime/Core/Public/Renderer/ShaderSources.h"
#include <glad/gl.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace VGE {
namespace UI {

UIRenderer::UIRenderer()
    : m_ShaderProgram(0)
    , m_VAO(0)
    , m_VBO(0)
    , m_ViewportWidth(0)
    , m_ViewportHeight(0)
    , m_ProjectionMatrix(1.0f)
{
}

UIRenderer::~UIRenderer() {
    Shutdown();
}

void UIRenderer::Initialize() {
    // Initialize shaders
    m_ColorShader = std::make_shared<UIShader>();
    const auto& coloredRectShader = ShaderSources::Get().GetShaderSource(ShaderSources::Type::UI_ColoredRect);
    m_ColorShader->Initialize(coloredRectShader.vertexSource, coloredRectShader.fragmentSource);

    m_TextureShader = std::make_shared<UIShader>();
    const auto& textureShader = ShaderSources::Get().GetShaderSource(ShaderSources::Type::UI_Texture);
    m_TextureShader->Initialize(textureShader.vertexSource, textureShader.fragmentSource);

    // Initialize font renderer
    m_FontRenderer = std::make_shared<UIFontRenderer>(this);
    m_FontRenderer->Initialize();

    // Create vertex buffers
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    // Setup vertex buffer
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void UIRenderer::Shutdown() {
    if (m_VAO) {
        glDeleteVertexArrays(1, &m_VAO);
        m_VAO = 0;
    }
    if (m_VBO) {
        glDeleteBuffers(1, &m_VBO);
        m_VBO = 0;
    }
    m_FontRenderer.reset();
    m_ColorShader.reset();
    m_TextureShader.reset();
}

void UIRenderer::BeginFrame() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void UIRenderer::EndFrame() {
    glDisable(GL_BLEND);
    while (!m_ClipStack.empty()) {
        PopClipRect();
    }
}

void UIRenderer::DrawRect(const Rect& rect, const Color& color) {
    DrawRect(rect.min, rect.max - rect.min, color);
}

void UIRenderer::DrawRect(const glm::vec2& position, const glm::vec2& size, const Color& color) {
    m_ColorShader->Bind();
    m_ColorShader->SetVec4("color", color);
    m_ColorShader->SetMat4("model", glm::translate(glm::mat4(1.0f), glm::vec3(position, 0.0f)) * 
                                   glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f)));
    m_ColorShader->SetMat4("projection", m_ProjectionMatrix);

    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    m_ColorShader->Unbind();
}

void UIRenderer::DrawLine(const glm::vec2& start, const glm::vec2& end, const Color& color, float thickness) {
    glm::vec2 direction = end - start;
    float length = glm::length(direction);
    if (length < 0.0001f) return;

    direction /= length;
    glm::vec2 perpendicular(-direction.y, direction.x);
    glm::vec2 offset = perpendicular * (thickness * 0.5f);

    float vertices[6][2] = {
        { start.x + offset.x, start.y + offset.y },
        { start.x - offset.x, start.y - offset.y },
        { end.x - offset.x, end.y - offset.y },
        { start.x + offset.x, start.y + offset.y },
        { end.x - offset.x, end.y - offset.y },
        { end.x + offset.x, end.y + offset.y }
    };

    m_ColorShader->Bind();
    m_ColorShader->SetVec4("color", color);
    m_ColorShader->SetMat4("model", glm::mat4(1.0f));
    m_ColorShader->SetMat4("projection", m_ProjectionMatrix);

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    m_ColorShader->Unbind();
}

void UIRenderer::DrawCircle(const glm::vec2& center, float radius, const Color& color, int segments) {
    std::vector<float> vertices;
    vertices.reserve((segments + 2) * 2);

    // Center vertex
    vertices.push_back(center.x);
    vertices.push_back(center.y);

    // Circle vertices
    for (int i = 0; i <= segments; ++i) {
        float angle = (2.0f * glm::pi<float>() * i) / segments;
        vertices.push_back(center.x + radius * cos(angle));
        vertices.push_back(center.y + radius * sin(angle));
    }

    m_ColorShader->Bind();
    m_ColorShader->SetVec4("color", color);
    m_ColorShader->SetMat4("model", glm::mat4(1.0f));
    m_ColorShader->SetMat4("projection", m_ProjectionMatrix);

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLE_FAN, 0, segments + 2);
    glBindVertexArray(0);

    m_ColorShader->Unbind();
}

void UIRenderer::DrawTexture(const std::shared_ptr<UITexture>& texture, const glm::vec2& position, const glm::vec2& size) {
    if (!texture) return;

    m_TextureShader->Bind();
    m_TextureShader->SetMat4("model", glm::translate(glm::mat4(1.0f), glm::vec3(position, 0.0f)) * 
                                     glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f)));
    m_TextureShader->SetMat4("projection", m_ProjectionMatrix);
    m_TextureShader->SetVec4("color", glm::vec4(1.0f));
    
    texture->Bind(0);
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    texture->Unbind();

    m_TextureShader->Unbind();
}

void UIRenderer::DrawTextureColored(const std::shared_ptr<UITexture>& texture, const glm::vec2& position, 
                                  const glm::vec2& size, const Color& color) {
    if (!texture) return;

    m_TextureShader->Bind();
    m_TextureShader->SetMat4("model", glm::translate(glm::mat4(1.0f), glm::vec3(position, 0.0f)) * 
                                     glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f)));
    m_TextureShader->SetMat4("projection", m_ProjectionMatrix);
    m_TextureShader->SetVec4("color", color);
    
    texture->Bind(0);
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    texture->Unbind();

    m_TextureShader->Unbind();
}

void UIRenderer::DrawText(const std::string& text, const glm::vec2& position, const Color& color, 
                         TextAlignment hAlign, TextAlignment vAlign, float fontSize) {
    if (!m_FontRenderer) return;
    m_FontRenderer->DrawText(text, position, color);
}

void UIRenderer::SetFont(const std::shared_ptr<UIFont>& font) {
    m_Font = font;
    if (m_FontRenderer) {
        m_FontRenderer->SetFont(font);
    }
}

void UIRenderer::PushClipRect(const glm::vec2& position, const glm::vec2& size) {
    ClipRect clip{position, size};
    m_ClipStack.push(clip);

    glEnable(GL_SCISSOR_TEST);
    glScissor(static_cast<GLint>(position.x), 
              static_cast<GLint>(m_ViewportHeight - (position.y + size.y)),
              static_cast<GLsizei>(size.x), 
              static_cast<GLsizei>(size.y));
}

void UIRenderer::PopClipRect() {
    if (m_ClipStack.empty()) return;

    m_ClipStack.pop();
    if (m_ClipStack.empty()) {
        glDisable(GL_SCISSOR_TEST);
    } else {
        const auto& clip = m_ClipStack.top();
        glScissor(static_cast<GLint>(clip.position.x), 
                  static_cast<GLint>(m_ViewportHeight - (clip.position.y + clip.size.y)),
                  static_cast<GLsizei>(clip.size.x), 
                  static_cast<GLsizei>(clip.size.y));
    }
}

void UIRenderer::SetViewport(int width, int height) {
    m_ViewportWidth = width;
    m_ViewportHeight = height;
    UpdateProjectionMatrix();
}

void UIRenderer::SetShader(const std::shared_ptr<UIShader>& shader) {
    if (shader) {
        shader->Bind();
        m_CurrentShader = shader;
    }
}

void UIRenderer::ResetShader() {
    if (m_CurrentShader) {
        m_CurrentShader->Unbind();
        m_CurrentShader = nullptr;
    }
}

void UIRenderer::UpdateProjectionMatrix() {
    m_ProjectionMatrix = glm::ortho(0.0f, static_cast<float>(m_ViewportWidth),
                                   static_cast<float>(m_ViewportHeight), 0.0f,
                                   -1.0f, 1.0f);
}

}} // namespace VGE::UI
