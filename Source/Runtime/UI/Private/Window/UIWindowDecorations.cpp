#include "Runtime/UI/Public/Window/UIWindowDecorations.h"
#include "Runtime/UI/Public/Window/UIWindow.h"
#include "Runtime/Core/Public/Renderer/UIRenderer.h"

namespace VGE {
namespace UI {

UIWindowDecorations::UIWindowDecorations(UIWindow* window)
    : UIWidget()
    , m_Window(window)
    , m_ShowTitleBar(true)
    , m_ShowBorders(true)
    , m_ShowIcon(true)
    , m_ShowButtons(true)
    , m_IsActive(true)
    , m_IsResizing(false)
    , m_IsDragging(false)
    , m_DragOffset(0.0f)
    , m_ResizeStart(0.0f)
    , m_HoveredButton(-1)
    , m_ActiveButton(-1)
{
    // Set default style
    m_Style = DecorationStyle();
}

void UIWindowDecorations::Render()
{
    if (!m_Window) return;

    RenderBorders();
    RenderTitleBar();
    RenderButtons();
    RenderIcon();
}

void UIWindowDecorations::Update(float deltaTime)
{
    // Base class handles input events through virtual functions
    UIWidget::Update(deltaTime);
}

void UIWindowDecorations::SetStyle(const DecorationStyle& style)
{
    m_Style = style;
}

bool UIWindowDecorations::HitTest(const glm::vec2& position, bool& outResizable) const
{
    outResizable = false;

    // Check title bar first
    if (m_ShowTitleBar && HitTestTitleBar(position)) {
        return true;
    }

    // Check borders
    if (m_ShowBorders && HitTestBorders(position, outResizable)) {
        return true;
    }

    // Check buttons
    if (m_ShowButtons && HitTestButtons(position)) {
        return true;
    }

    return false;
}

bool UIWindowDecorations::HitTestTitleBar(const glm::vec2& position) const
{
    if (!m_ShowTitleBar) return false;

    glm::vec2 windowPos = m_Window->GetPosition();
    glm::vec2 windowSize = m_Window->GetSize();
    
    // Title bar rect
    float titleBarLeft = windowPos.x;
    float titleBarRight = windowPos.x + windowSize.x;
    float titleBarTop = windowPos.y;
    float titleBarBottom = windowPos.y + m_Style.titleBarHeight;

    return position.x >= titleBarLeft && position.x <= titleBarRight &&
           position.y >= titleBarTop && position.y <= titleBarBottom;
}

bool UIWindowDecorations::HitTestBorders(const glm::vec2& position, bool& outResizable) const
{
    if (!m_ShowBorders) return false;

    glm::vec2 windowPos = m_Window->GetPosition();
    glm::vec2 windowSize = m_Window->GetSize();
    float borderWidth = m_Style.borderWidth;

    // Border rects
    bool hitLeft = position.x >= windowPos.x && position.x <= windowPos.x + borderWidth;
    bool hitRight = position.x >= windowPos.x + windowSize.x - borderWidth && position.x <= windowPos.x + windowSize.x;
    bool hitTop = position.y >= windowPos.y && position.y <= windowPos.y + borderWidth;
    bool hitBottom = position.y >= windowPos.y + windowSize.y - borderWidth && position.y <= windowPos.y + windowSize.y;

    outResizable = (hitLeft || hitRight || hitTop || hitBottom) && m_Window->IsResizable();
    return hitLeft || hitRight || hitTop || hitBottom;
}

bool UIWindowDecorations::HitTestButtons(const glm::vec2& position) const
{
    if (!m_ShowButtons) return false;

    glm::vec2 windowPos = m_Window->GetPosition();
    glm::vec2 buttonPos = windowPos;
    buttonPos.x += m_Window->GetSize().x - m_Style.buttonSize * 3 - 10.0f;
    buttonPos.y += 5.0f;

    // Test each button (minimize, maximize, close)
    for (int i = 0; i < 3; ++i) {
        glm::vec2 currentButtonPos = buttonPos;
        currentButtonPos.x += i * (m_Style.buttonSize + 5.0f);

        if (position.x >= currentButtonPos.x && position.x <= currentButtonPos.x + m_Style.buttonSize &&
            position.y >= currentButtonPos.y && position.y <= currentButtonPos.y + m_Style.buttonSize) {
            return true;
        }
    }

    return false;
}

void UIWindowDecorations::OnMouseDown(const glm::vec2& position)
{
    bool resizable;
    if (HitTestTitleBar(position)) {
        m_IsDragging = true;
        m_DragOffset = position - m_Window->GetPosition();
    }
    else if (HitTestBorders(position, resizable) && resizable) {
        m_IsResizing = true;
        m_ResizeStart = position;
    }
    else if (HitTestButtons(position)) {
        m_ActiveButton = GetButtonAtPosition(position);
    }
}

void UIWindowDecorations::OnMouseUp(const glm::vec2& position)
{
    if (m_IsDragging) {
        m_IsDragging = false;
    }
    
    if (m_IsResizing) {
        m_IsResizing = false;
    }

    if (m_ActiveButton >= 0) {
        if (HitTestButtons(position) && m_ActiveButton == GetButtonAtPosition(position)) {
            HandleButtonClick(m_ActiveButton);
        }
        m_ActiveButton = -1;
    }
}

void UIWindowDecorations::OnMouseMove(const glm::vec2& position)
{
    if (m_IsDragging) {
        m_Window->SetPosition(position - m_DragOffset);
    }
    else if (m_IsResizing) {
        HandleResize(position);
    }
    else {
        // Update button hover state
        m_HoveredButton = GetButtonAtPosition(position);
    }
}

void UIWindowDecorations::OnDoubleClick(const glm::vec2& position)
{
    if (HitTestTitleBar(position)) {
        if (m_Window->IsMaximized()) {
            m_Window->Restore();
        } else {
            m_Window->Maximize();
        }
    }
}

void UIWindowDecorations::RenderTitleBar()
{
    if (!m_ShowTitleBar) return;

    glm::vec2 windowPos = m_Window->GetPosition();
    glm::vec2 windowSize = m_Window->GetSize();
    
    // Title bar background
    UIRenderer::DrawRect(
        windowPos,
        glm::vec2(windowSize.x, m_Style.titleBarHeight),
        m_Window->IsFocused() ? m_Style.activeColor : m_Style.inactiveColor
    );

    // Title text
    glm::vec2 textPos = windowPos;
    textPos.x += m_Style.iconSize + 10.0f;
    textPos.y += (m_Style.titleBarHeight - 14.0f) * 0.5f;
    UIRenderer::DrawText(m_Window->GetTitle(), textPos, m_Style.textColor);
}

void UIWindowDecorations::RenderBorders()
{
    if (!m_ShowBorders) return;

    glm::vec2 windowPos = m_Window->GetPosition();
    glm::vec2 windowSize = m_Window->GetSize();
    glm::vec4 borderColor = m_Window->IsFocused() ? m_Style.activeColor : m_Style.inactiveColor;

    // Draw borders
    UIRenderer::DrawRectOutline(
        windowPos,
        windowSize,
        borderColor,
        m_Style.borderWidth
    );
}

void UIWindowDecorations::RenderButtons()
{
    if (!m_ShowButtons) return;

    glm::vec2 windowPos = m_Window->GetPosition();
    glm::vec2 buttonPos = windowPos;
    buttonPos.x += m_Window->GetSize().x - m_Style.buttonSize * 3 - 10.0f;
    buttonPos.y += 5.0f;

    // Render minimize, maximize, and close buttons
    for (int i = 0; i < 3; ++i) {
        glm::vec2 currentButtonPos = buttonPos;
        currentButtonPos.x += i * (m_Style.buttonSize + 5.0f);

        bool isHovered = m_HoveredButton == i;
        bool isActive = m_ActiveButton == i;
        glm::vec4 buttonColor = isActive ? m_Style.buttonActiveColor :
                               isHovered ? m_Style.buttonHoverColor :
                               m_Style.inactiveColor;

        UIRenderer::DrawRect(
            currentButtonPos,
            glm::vec2(m_Style.buttonSize),
            buttonColor
        );

        // Draw button icon
        RenderButtonIcon(i, currentButtonPos);
    }
}

void UIWindowDecorations::RenderIcon()
{
    if (!m_ShowIcon) return;

    glm::vec2 windowPos = m_Window->GetPosition();
    glm::vec2 iconPos = windowPos;
    iconPos.x += 5.0f;
    iconPos.y += (m_Style.titleBarHeight - m_Style.iconSize) * 0.5f;

    // TODO: Implement icon rendering
}

// Private helpers
int UIWindowDecorations::GetButtonAtPosition(const glm::vec2& position) const
{
    glm::vec2 windowPos = m_Window->GetPosition();
    glm::vec2 buttonPos = windowPos;
    buttonPos.x += m_Window->GetSize().x - m_Style.buttonSize * 3 - 10.0f;
    buttonPos.y += 5.0f;

    for (int i = 0; i < 3; ++i) {
        glm::vec2 currentButtonPos = buttonPos;
        currentButtonPos.x += i * (m_Style.buttonSize + 5.0f);

        if (position.x >= currentButtonPos.x && position.x <= currentButtonPos.x + m_Style.buttonSize &&
            position.y >= currentButtonPos.y && position.y <= currentButtonPos.y + m_Style.buttonSize) {
            return i;
        }
    }

    return -1;
}

void UIWindowDecorations::HandleButtonClick(int buttonIndex)
{
    switch (buttonIndex) {
        case 0: // Minimize
            m_Window->Minimize();
            break;
        case 1: // Maximize/Restore
            if (m_Window->IsMaximized()) {
                m_Window->Restore();
            } else {
                m_Window->Maximize();
            }
            break;
        case 2: // Close
            m_Window->Close();
            break;
    }
}

void UIWindowDecorations::HandleResize(const glm::vec2& position)
{
    glm::vec2 windowPos = m_Window->GetPosition();
    glm::vec2 windowSize = m_Window->GetSize();
    glm::vec2 delta = position - m_ResizeStart;

    // Determine resize direction
    bool resizeLeft = position.x < windowPos.x + m_Style.borderWidth;
    bool resizeRight = position.x > windowPos.x + windowSize.x - m_Style.borderWidth;
    bool resizeTop = position.y < windowPos.y + m_Style.borderWidth;
    bool resizeBottom = position.y > windowPos.y + windowSize.y - m_Style.borderWidth;

    // Apply resize
    if (resizeLeft) {
        float newWidth = windowSize.x - delta.x;
        if (newWidth >= m_Window->GetMinSize().x) {
            windowPos.x += delta.x;
            windowSize.x = newWidth;
        }
    }
    else if (resizeRight) {
        windowSize.x = std::max(m_Window->GetMinSize().x, windowSize.x + delta.x);
    }

    if (resizeTop) {
        float newHeight = windowSize.y - delta.y;
        if (newHeight >= m_Window->GetMinSize().y) {
            windowPos.y += delta.y;
            windowSize.y = newHeight;
        }
    }
    else if (resizeBottom) {
        windowSize.y = std::max(m_Window->GetMinSize().y, windowSize.y + delta.y);
    }

    m_Window->SetPosition(windowPos);
    m_Window->SetSize(windowSize);
    m_ResizeStart = position;
}

void UIWindowDecorations::RenderButtonIcon(int buttonIndex, const glm::vec2& position)
{
    glm::vec2 iconPos = position;
    iconPos.x += m_Style.buttonSize * 0.25f;
    iconPos.y += m_Style.buttonSize * 0.25f;
    float iconSize = m_Style.buttonSize * 0.5f;

    switch (buttonIndex) {
        case 0: // Minimize
            UIRenderer::DrawRect(
                iconPos + glm::vec2(0.0f, iconSize * 0.75f),
                glm::vec2(iconSize, 1.0f),
                m_Style.textColor
            );
            break;

        case 1: // Maximize/Restore
            if (m_Window->IsMaximized()) {
                // Restore icon
                UIRenderer::DrawRectOutline(
                    iconPos + glm::vec2(2.0f, -2.0f),
                    glm::vec2(iconSize - 4.0f),
                    m_Style.textColor,
                    1.0f
                );
                UIRenderer::DrawRectOutline(
                    iconPos + glm::vec2(-2.0f, 2.0f),
                    glm::vec2(iconSize - 4.0f),
                    m_Style.textColor,
                    1.0f
                );
            } else {
                // Maximize icon
                UIRenderer::DrawRectOutline(
                    iconPos,
                    glm::vec2(iconSize),
                    m_Style.textColor,
                    1.0f
                );
            }
            break;

        case 2: // Close
            float crossSize = iconSize * 0.7f;
            float offset = (m_Style.buttonSize - crossSize) * 0.5f;
            UIRenderer::DrawLine(
                position + glm::vec2(offset),
                position + glm::vec2(offset + crossSize),
                m_Style.textColor,
                1.0f
            );
            UIRenderer::DrawLine(
                position + glm::vec2(offset + crossSize, offset),
                position + glm::vec2(offset, offset + crossSize),
                m_Style.textColor,
                1.0f
            );
            break;
    }
}

}} // namespace VGE::UI 