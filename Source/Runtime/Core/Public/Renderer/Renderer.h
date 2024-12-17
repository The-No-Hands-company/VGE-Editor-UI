#pragma once

#include "Runtime/Core/Public/Core/UICore.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>

namespace VGE {
namespace UI {

class Renderer {
public:
    Renderer();
    ~Renderer();

    bool Initialize();
    void SetViewport(int width, int height);
    
    // Shape rendering
    void DrawRect(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
    void DrawRoundedRect(const glm::vec2& position, const glm::vec2& size, const glm::vec2& topRadius, const glm::vec2& bottomRadius, const glm::vec4& color);
    void DrawOutline(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color, float width = 1.0f);

    // Frame management
    void BeginFrame();
    void EndFrame();

private:
    // OpenGL resources
    GLuint m_ShapeShader;
    GLuint m_RectVAO;
    GLuint m_RectVBO;

    // Viewport dimensions
    int m_ViewportWidth;
    int m_ViewportHeight;

    // Helper functions
    bool InitializeShaders();
    void CreateRoundedRectMesh();
};

} // namespace UI
} // namespace VGE



