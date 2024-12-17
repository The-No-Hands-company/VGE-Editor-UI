#pragma once

#include "Runtime/Core/Public/Core/UICore.h"
#include "Runtime/Core/Public/Core/UITypes.h"
#include <glm/glm.hpp>
#include <string>
#include <memory>
#include <vector>
#include <stack>

namespace VGE {
namespace UI {

// Forward declarations
class UIElement;
class UIShader;
class UITexture;
class UIFontRenderer;
class UIVertexArray;
class UIVertexBuffer;
class UIIndexBuffer;
class UIFont;

class UI_API UIRenderer {
public:
    static UIRenderer& Get() {
        static UIRenderer instance;
        return instance;
    }

    void Initialize();
    void Shutdown();

    void BeginFrame();
    void EndFrame();

    // Basic shape rendering
    void DrawRect(const Rect& rect, const Color& color);
    void DrawRect(const glm::vec2& position, const glm::vec2& size, const Color& color);
    void DrawRectOutline(const Rect& rect, const Color& color, float thickness = 1.0f);
    void DrawRectOutline(const glm::vec2& position, const glm::vec2& size, const Color& color, float thickness = 1.0f);
    void DrawRoundedRect(const Rect& rect, const Color& color, float radius);
    void DrawRoundedRect(const glm::vec2& position, const glm::vec2& size, const Color& color, float radius);
    void DrawRoundedRectOutline(const Rect& rect, const Color& color, float radius, float thickness = 1.0f);
    void DrawRoundedRectOutline(const glm::vec2& position, const glm::vec2& size, const Color& color, float radius, float thickness = 1.0f);
    void DrawLine(const glm::vec2& start, const glm::vec2& end, const Color& color, float thickness = 1.0f);
    void DrawCircle(const glm::vec2& center, float radius, const Color& color, int segments = 32);
    
    // Text rendering
    glm::vec2 MeasureText(const std::string& text, float fontSize = 14.0f) const;
    void DrawText(const std::string& text, const glm::vec2& position, const Color& color, TextAlignment hAlign = TextAlignment::Left, TextAlignment vAlign = TextAlignment::Top, float fontSize = 14.0f);
    void DrawTextCentered(const std::string& text, const glm::vec2& position, const glm::vec2& size, const Color& color, float fontSize = 14.0f);
    void SetFont(const std::shared_ptr<UIFont>& font);

    // Texture rendering
    void DrawTexture(const std::shared_ptr<UITexture>& texture, const glm::vec2& position, const glm::vec2& size);
    void DrawTextureColored(const std::shared_ptr<UITexture>& texture, const glm::vec2& position, 
                           const glm::vec2& size, const Color& color);
    void DrawIcon(const std::string& iconName, const glm::vec2& position, const glm::vec2& size, const Color& color);

    // Clipping and viewport
    void PushClipRect(const glm::vec2& position, const glm::vec2& size);
    void PopClipRect();
    void SetViewport(int width, int height);

    // Shader management
    void SetShader(const std::shared_ptr<UIShader>& shader);
    void ResetShader();

    // Matrix access
    const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }

private:
    UIRenderer();
    ~UIRenderer();
    UIRenderer(const UIRenderer&) = delete;
    UIRenderer& operator=(const UIRenderer&) = delete;

    void UpdateProjectionMatrix();

    // OpenGL resources
    unsigned int m_ShaderProgram{0};
    unsigned int m_VAO{0};
    unsigned int m_VBO{0};

    // Viewport
    int m_ViewportWidth{0};
    int m_ViewportHeight{0};
    glm::mat4 m_ProjectionMatrix{1.0f};

    // Shaders
    std::shared_ptr<UIShader> m_ColorShader;
    std::shared_ptr<UIShader> m_TextureShader;
    std::shared_ptr<UIShader> m_CurrentShader;

    // Buffers
    std::shared_ptr<UIVertexArray> m_QuadVA;
    std::shared_ptr<UIVertexBuffer> m_QuadVB;
    std::shared_ptr<UIIndexBuffer> m_QuadIB;

    // Font rendering
    std::shared_ptr<UIFontRenderer> m_FontRenderer;
    std::shared_ptr<UIFont> m_Font;

    // Clipping
    struct ClipRect {
        glm::vec2 position;
        glm::vec2 size;
    };
    std::stack<ClipRect> m_ClipStack;
};

}} // namespace VGE::UI
