#include "Runtime/UI/Public/Controls/UIWindow.h"
#include "Runtime/Core/Public/Core/UICore.h"
#include "Runtime/Core/Public/Renderer/UIRenderer.h"
#include <algorithm>

namespace VGE {
namespace UI {

UIWindow::UIWindow(const std::string& title)
    : m_Title(title)
    , m_State(WindowState::Normal)
    , m_Position(0.0f, 0.0f)
    , m_Size(400.0f, 300.0f)
    , m_MinSize(100.0f, 100.0f)
    , m_MaxSize(10000.0f, 10000.0f)
    , m_Padding(5.0f, 5.0f, 5.0f, 5.0f)
    , m_Visible(true)
    , m_Modal(false)
    , m_Dockable(true)
    , m_Focused(false)
    , m_Dragging(false)
    , m_Resizing(false)
    , m_ResizeEdge(0)
{
}

void UIWindow::Initialize() {
    UIWidget::Initialize();
    for (auto& widget : m_Widgets) {
        widget->Initialize();
    }
}

void UIWindow::Update(float deltaTime) {
    if (!m_Visible) return;

    UIWidget::Update(deltaTime);
    for (auto& widget : m_Widgets) {
        widget->Update(deltaTime);
    }
}

void UIWindow::Render(UIRenderer* renderer) {
    if (!m_Visible) return;

    // Render window background
    renderer->DrawRect(m_Position, m_Size, glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));

    // Render title bar
    float titleBarHeight = 25.0f;
    glm::vec2 titleBarPos = m_Position;
    glm::vec2 titleBarSize(m_Size.x, titleBarHeight);
    renderer->DrawRect(titleBarPos, titleBarSize, glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
    
    // Render title text
    glm::vec2 textPos = titleBarPos + glm::vec2(5.0f, 5.0f);
    renderer->DrawText(m_Title, textPos, glm::vec4(1.0f));

    // Render window controls
    float controlSize = 16.0f;
    float padding = 4.0f;
    glm::vec2 controlPos(m_Position.x + m_Size.x - controlSize - padding, m_Position.y + padding);
    
    // Close button
    renderer->DrawRect(controlPos, glm::vec2(controlSize), glm::vec4(0.8f, 0.2f, 0.2f, 1.0f));
    controlPos.x -= controlSize + padding;
    
    // Maximize button
    renderer->DrawRect(controlPos, glm::vec2(controlSize), glm::vec4(0.2f, 0.8f, 0.2f, 1.0f));
    controlPos.x -= controlSize + padding;
    
    // Minimize button
    renderer->DrawRect(controlPos, glm::vec2(controlSize), glm::vec4(0.2f, 0.2f, 0.8f, 1.0f));

    // Render child widgets
    for (auto& widget : m_Widgets) {
        widget->Render(renderer);
    }
}

void UIWindow::SetState(WindowState state) {
    if (m_State != state) {
        WindowState oldState = m_State;
        m_State = state;
        OnStateChanged(oldState, state);
    }
}

void UIWindow::AddWidget(const std::shared_ptr<UIWidget>& widget) {
    if (widget) {
        m_Widgets.push_back(widget);
        if (IsInitialized()) {
            widget->Initialize();
        }
    }
}

void UIWindow::RemoveWidget(const std::shared_ptr<UIWidget>& widget) {
    auto it = std::find(m_Widgets.begin(), m_Widgets.end(), widget);
    if (it != m_Widgets.end()) {
        m_Widgets.erase(it);
    }
}

void UIWindow::ClearWidgets() {
    m_Widgets.clear();
}

void UIWindow::SetPosition(const glm::vec2& position) {
    if (position != m_Position) {
        glm::vec2 oldPos = m_Position;
        m_Position = position;
        OnPositionChanged(oldPos, m_Position);
    }
}

void UIWindow::SetSize(const glm::vec2& size) {
    glm::vec2 newSize = glm::clamp(size, m_MinSize, m_MaxSize);
    if (newSize != m_Size) {
        glm::vec2 oldSize = m_Size;
        m_Size = newSize;
        OnSizeChanged(oldSize, m_Size);
    }
}

bool UIWindow::OnMouseMove(const glm::vec2& position) {
    m_LastMousePos = position;

    if (m_Dragging) {
        UpdateDrag(position);
        return true;
    }
    
    if (m_Resizing) {
        UpdateResize(position);
        return true;
    }

    // Update resize edge highlight
    if (m_State == WindowState::Normal) {
        UpdateResizeEdge(position);
    }

    // Forward to child widgets if not interacting with window controls
    if (!IsPointInTitleBar(position)) {
        for (auto& widget : m_Widgets) {
            if (widget->OnMouseMove(position)) {
                return true;
            }
        }
    }

    return false;
}

bool UIWindow::OnMouseButton(int button, bool pressed, const glm::vec2& position) {
    if (button == 0) { // Left mouse button
        if (pressed) {
            // Check for title bar drag
            if (IsPointInTitleBar(position)) {
                StartDrag(position);
                return true;
            }
            
            // Check for resize
            int edge;
            if (m_State == WindowState::Normal && IsPointInResizeArea(position, edge)) {
                m_ResizeEdge = edge;
                StartResize(position);
                return true;
            }
        } else {
            if (m_Dragging) {
                EndDrag();
                return true;
            }
            if (m_Resizing) {
                EndResize();
                return true;
            }
        }
    }

    // Forward to child widgets
    for (auto& widget : m_Widgets) {
        if (widget->OnMouseButton(button, pressed, position)) {
            return true;
        }
    }

    return false;
}

void UIWindow::UpdateResizeEdge(const glm::vec2& mousePos) {
    int edge;
    m_ResizeEdge = IsPointInResizeArea(mousePos, edge) ? edge : 0;
}

void UIWindow::StartResize(const glm::vec2& mousePos) {
    m_Resizing = true;
    m_LastMousePos = mousePos;
}

void UIWindow::UpdateResize(const glm::vec2& mousePos) {
    glm::vec2 delta = mousePos - m_LastMousePos;
    glm::vec2 newSize = m_Size;
    glm::vec2 newPos = m_Position;

    if (m_ResizeEdge & 1) { // Left edge
        newPos.x += delta.x;
        newSize.x -= delta.x;
    }
    if (m_ResizeEdge & 2) { // Right edge
        newSize.x += delta.x;
    }
    if (m_ResizeEdge & 4) { // Top edge
        newPos.y += delta.y;
        newSize.y -= delta.y;
    }
    if (m_ResizeEdge & 8) { // Bottom edge
        newSize.y += delta.y;
    }

    // Apply changes while respecting min/max size
    SetSize(newSize);
    SetPosition(newPos);

    m_LastMousePos = mousePos;
}

void UIWindow::EndResize() {
    m_Resizing = false;
    m_ResizeEdge = 0;
}

void UIWindow::StartDrag(const glm::vec2& mousePos) {
    m_Dragging = true;
    m_DragOffset = m_Position - mousePos;
}

void UIWindow::UpdateDrag(const glm::vec2& mousePos) {
    SetPosition(mousePos + m_DragOffset);
}

void UIWindow::EndDrag() {
    m_Dragging = false;
}

bool UIWindow::IsPointInTitleBar(const glm::vec2& point) const {
    return point.x >= m_Position.x && 
           point.x <= m_Position.x + m_Size.x &&
           point.y >= m_Position.y &&
           point.y <= m_Position.y + 25.0f; // Title bar height
}

bool UIWindow::IsPointInResizeArea(const glm::vec2& point, int& edge) const {
    const float handleSize = 6.0f;
    
    bool left = point.x >= m_Position.x && point.x < m_Position.x + handleSize;
    bool right = point.x >= m_Position.x + m_Size.x - handleSize && point.x < m_Position.x + m_Size.x;
    bool top = point.y >= m_Position.y && point.y < m_Position.y + handleSize;
    bool bottom = point.y >= m_Position.y + m_Size.y - handleSize && point.y < m_Position.y + m_Size.y;

    edge = 0;
    if (left) edge |= 1;
    if (right) edge |= 2;
    if (top) edge |= 4;
    if (bottom) edge |= 8;

    return edge != 0;
}

// Virtual event handlers with default empty implementations
void UIWindow::OnStateChanged(WindowState oldState, WindowState newState) {}
void UIWindow::OnPositionChanged(const glm::vec2& oldPos, const glm::vec2& newPos) {}
void UIWindow::OnSizeChanged(const glm::vec2& oldSize, const glm::vec2& newSize) {}
void UIWindow::OnClose() {}
void UIWindow::OnFocus(bool focused) { m_Focused = focused; }

// Forward remaining input events to child widgets
bool UIWindow::OnMouseScroll(float xoffset, float yoffset) {
    for (auto& widget : m_Widgets) {
        if (widget->OnMouseScroll(xoffset, yoffset)) return true;
    }
    return false;
}

bool UIWindow::OnKeyPress(int key, int scancode, int mods) {
    for (auto& widget : m_Widgets) {
        if (widget->OnKeyPress(key, scancode, mods)) return true;
    }
    return false;
}

bool UIWindow::OnKeyRelease(int key, int scancode, int mods) {
    for (auto& widget : m_Widgets) {
        if (widget->OnKeyRelease(key, scancode, mods)) return true;
    }
    return false;
}

bool UIWindow::OnChar(unsigned int codepoint) {
    for (auto& widget : m_Widgets) {
        if (widget->OnChar(codepoint)) return true;
    }
    return false;
}

}} // namespace VGE::UI



