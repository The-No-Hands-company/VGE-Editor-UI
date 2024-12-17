#include "Runtime/Core/Public/Widget/UIDialog.h"
#include "Runtime/Core/Public/Renderer/UIRenderer.h"

namespace VGE {
namespace Editor {

UIDialog::UIDialog(const std::string& title)
    : m_Title(title) {
    SetSize(glm::vec2(400.0f, 300.0f));
}

void UIDialog::OnUpdate(float deltaTime) {
    if (m_IsDragging) {
        glm::vec2 mousePos = GetMousePosition();
        SetPosition(mousePos - m_DragOffset);
    }
    
    if (m_IsResizing) {
        glm::vec2 mousePos = GetMousePosition();
        glm::vec2 newSize = mousePos - GetPosition();
        SetSize(glm::max(newSize, glm::vec2(100.0f, 100.0f)));
    }
}

void UIDialog::OnPaint(UIRenderer& renderer) {
    // Draw dialog background
    renderer.DrawRect(GetPosition(), GetSize(), glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));
    renderer.DrawRectOutline(GetPosition(), GetSize(), glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
    
    // Draw components
    RenderTitleBar(renderer);
    RenderContent(renderer);
    
    if (m_Resizable) {
        RenderResizeHandle(renderer);
    }
}

void UIDialog::RenderTitleBar(UIRenderer& renderer) {
    glm::vec2 titleBarPos = GetPosition();
    glm::vec2 titleBarSize(GetSize().x, m_TitleBarHeight);
    
    // Draw title bar background
    renderer.DrawRect(titleBarPos, titleBarSize, glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
    
    // Draw title text
    glm::vec2 textSize = renderer.GetTextSize(m_Title);
    glm::vec2 textPos = titleBarPos + glm::vec2(10.0f, (m_TitleBarHeight - textSize.y) * 0.5f);
    renderer.DrawText(m_Title, textPos, glm::vec4(1.0f));
}

void UIDialog::RenderContent(UIRenderer& renderer) {
    // Content area starts below title bar
    glm::vec2 contentPos = GetPosition() + glm::vec2(0.0f, m_TitleBarHeight);
    glm::vec2 contentSize = GetSize() - glm::vec2(0.0f, m_TitleBarHeight);
    
    // Draw content background
    renderer.DrawRect(contentPos, contentSize, glm::vec4(0.15f, 0.15f, 0.15f, 1.0f));
}

void UIDialog::RenderResizeHandle(UIRenderer& renderer) {
    glm::vec2 handlePos = GetPosition() + GetSize() - glm::vec2(m_ResizeHandleSize);
    glm::vec2 handleSize(m_ResizeHandleSize);
    
    renderer.DrawRect(handlePos, handleSize, glm::vec4(0.4f, 0.4f, 0.4f, 1.0f));
}

void UIDialog::CenterInParent() {
    if (auto parent = GetParent()) {
        glm::vec2 parentSize = parent->GetSize();
        glm::vec2 dialogSize = GetSize();
        glm::vec2 newPos = (parentSize - dialogSize) * 0.5f;
        SetPosition(newPos);
    }
}

bool UIDialog::OnMouseMove(const glm::vec2& mousePos) {
    if (m_IsDragging) {
        SetPosition(mousePos - m_DragOffset);
        return true;
    }
    
    if (m_IsResizing) {
        glm::vec2 newSize = mousePos - GetPosition();
        SetSize(glm::max(newSize, glm::vec2(100.0f, 100.0f)));
        return true;
    }
    
    return false;
}

bool UIDialog::OnMouseDown(const glm::vec2& mousePos) {
    // Check if clicking in title bar
    if (m_Draggable) {
        glm::vec2 localPos = mousePos - GetPosition();
        if (localPos.y < m_TitleBarHeight) {
            m_IsDragging = true;
            m_DragOffset = localPos;
            return true;
        }
    }
    
    // Check if clicking resize handle
    if (m_Resizable) {
        glm::vec2 handlePos = GetPosition() + GetSize() - glm::vec2(m_ResizeHandleSize);
        if (mousePos.x >= handlePos.x && mousePos.y >= handlePos.y) {
            m_IsResizing = true;
            return true;
        }
    }
    
    return false;
}

bool UIDialog::OnMouseUp(const glm::vec2& mousePos) {
    bool wasHandled = m_IsDragging || m_IsResizing;
    m_IsDragging = false;
    m_IsResizing = false;
    return wasHandled;
}

void UIDialog::OnMouseEnter() {
    // Handle mouse enter if needed
}

void UIDialog::OnMouseLeave() {
    // Handle mouse leave if needed
}

void UIDialog::OnPressed() {
    // Handle pressed if needed
}

void UIDialog::OnReleased() {
    // Handle released if needed
}

void UIDialog::OnPositionChanged() {
    // Handle position changed if needed
}

void UIDialog::OnSizeChanged() {
    // Handle size changed if needed
}

void UIDialog::OnChildAdded(std::shared_ptr<UIElement> child) {
    // Handle child added if needed
}

void UIDialog::OnChildRemoved(std::shared_ptr<UIElement> child) {
    // Handle child removed if needed
}

void UIDialog::OnParentPositionChanged() {
    // Handle parent position changed if needed
}

}} // namespace VGE::Editor 