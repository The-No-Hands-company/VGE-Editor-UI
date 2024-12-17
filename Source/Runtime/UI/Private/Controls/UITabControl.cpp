#include "Runtime/UI/Public/Controls/UITabControl.h"
#include "Runtime/Core/Public/Renderer/UIRenderer.h"
#include "Runtime/Core/Public/Theme/UITheme.h"
#include "Runtime/Core/Public/Widget/UIWidget.h"
#include "Runtime/Core/Public/Events/Event.h"
#include "Runtime/Core/Public/Base/VGE.h"
#include <glm/gtc/constants.hpp>
#include <algorithm>

namespace VGE {
namespace Editor {

UITabControl::UITabControl(const std::string& label)
    : UIWidget(label)
{
    SetSize(glm::vec2(400, 300)); // Default size
    m_ScrollOffset = 0.0f;
    m_ScrollVelocity = 0.0f;
}

void UITabControl::Update()
{
    if (!m_IsVisible) return;

    // Update scroll animation
    if (std::abs(m_ScrollVelocity) > 0.01f) {
        m_ScrollOffset += m_ScrollVelocity;
        m_ScrollVelocity *= 0.9f; // Damping

        // Clamp scroll offset
        float maxScroll = GetMaxScrollOffset();
        m_ScrollOffset = std::clamp(m_ScrollOffset, 0.0f, maxScroll);
    }

    // Update active tab content
    if (m_ActiveTab && m_ActiveTab->Content) {
        m_ActiveTab->Content->Update();
    }
}

void UITabControl::Draw()
{
    if (!m_IsVisible) return;

    // Draw tab bar background
    DrawTabBar();

    // Draw active tab content
    if (m_ActiveTab) {
        DrawTabContent(*m_ActiveTab);
    }

    // Draw scroll indicators if needed
    DrawScrollIndicators();
}

void UITabControl::DrawTabBar()
{
    // Draw tab bar background
    const auto& theme = UITheme::Get();
    Renderer::DrawRect(
        m_Position,
        glm::vec2(m_Size.x, m_TabHeight),
        theme.TabBarBackground
    );

    // Calculate visible tab range
    float visibleWidth = m_Size.x - (m_ShowAddButton ? m_TabHeight : 0.0f);
    float currentX = m_Position.x - m_ScrollOffset;
    
    // Draw tabs
    for (auto& tab : m_Tabs) {
        float tabWidth = CalculateTabWidth(tab);
        
        // Skip tabs that are completely outside the visible area
        if (currentX + tabWidth < m_Position.x || currentX > m_Position.x + visibleWidth) {
            currentX += tabWidth;
            continue;
        }

        // Draw tab
        DrawTab(tab, glm::vec2(currentX, m_Position.y), tabWidth);
        currentX += tabWidth;
    }

    // Draw dragged tab if any
    if (m_IsDragging && m_DraggedTab) {
        glm::vec2 dragPos = m_LastMousePos - m_DragOffset;
        DrawTab(*m_DraggedTab, dragPos, m_DraggedTabWidth);
    }

    // Draw add button if enabled
    if (m_ShowAddButton) {
        DrawAddButton();
    }

    // Draw scroll buttons if needed
    if (NeedsScrolling()) {
        DrawScrollButtons();
    }
}

void UITabControl::DrawTab(TabItem& tab, const glm::vec2& position, float width)
{
    const auto& theme = UITheme::Get();
    
    // Draw tab background
    glm::vec4 bgColor;
    if (&tab == m_DraggedTab) {
        bgColor = theme.TabDragging;
    } else if (tab.IsActive) {
        bgColor = theme.TabActive;
    } else if (tab.IsHovered) {
        bgColor = theme.TabHovered;
    } else {
        bgColor = theme.TabNormal;
    }

    Renderer::DrawRect(
        position,
        glm::vec2(width, m_TabHeight),
        bgColor
    );

    float contentX = position.x + m_TabPadding;
    float centerY = position.y + m_TabHeight * 0.5f;

    // Draw icon if present
    if (!tab.Icon.empty()) {
        float iconSize = m_TabHeight * 0.6f;
        Renderer::DrawIcon(
            tab.Icon,
            glm::vec2(contentX, centerY - iconSize * 0.5f),
            glm::vec2(iconSize)
        );
        contentX += iconSize + m_TabPadding;
    }

    // Draw tab name
    Renderer::DrawText(
        tab.Name,
        glm::vec2(contentX, centerY),
        tab.IsActive ? theme.TextActive : theme.TextNormal,
        12.0f
    );

    // Draw close button if enabled
    if (m_TabsCloseable && tab.CanClose) {
        float closeX = position.x + width - m_TabPadding - m_CloseButtonSize;
        float closeY = centerY - m_CloseButtonSize * 0.5f;
        DrawCloseButton(glm::vec2(closeX, closeY), tab.IsHovered);
    }
}

void UITabControl::DrawScrollIndicators()
{
    if (!NeedsScrolling()) return;

    const auto& theme = UITheme::Get();
    float maxScroll = GetMaxScrollOffset();

    // Left indicator
    if (m_ScrollOffset > 0.0f) {
        Renderer::DrawRect(
            m_Position,
            glm::vec2(10.0f, m_TabHeight),
            theme.ScrollIndicator
        );
    }

    // Right indicator
    if (m_ScrollOffset < maxScroll) {
        Renderer::DrawRect(
            m_Position + glm::vec2(m_Size.x - 10.0f, 0.0f),
            glm::vec2(10.0f, m_TabHeight),
            theme.ScrollIndicator
        );
    }
}

void UITabControl::DrawScrollButtons()
{
    const auto& theme = UITheme::Get();
    float buttonWidth = 20.0f;

    // Left scroll button
    if (m_ScrollOffset > 0.0f) {
        bool leftHovered = IsPointInRect(m_LastMousePos, m_Position, glm::vec2(buttonWidth, m_TabHeight));
        Renderer::DrawRect(
            m_Position,
            glm::vec2(buttonWidth, m_TabHeight),
            leftHovered ? theme.ButtonHovered : theme.ButtonNormal
        );
        Renderer::DrawText("◄", m_Position + glm::vec2(buttonWidth * 0.5f, m_TabHeight * 0.5f), theme.TextNormal, 12.0f);
    }

    // Right scroll button
    if (m_ScrollOffset < GetMaxScrollOffset()) {
        glm::vec2 rightPos = m_Position + glm::vec2(m_Size.x - buttonWidth, 0.0f);
        bool rightHovered = IsPointInRect(m_LastMousePos, rightPos, glm::vec2(buttonWidth, m_TabHeight));
        Renderer::DrawRect(
            rightPos,
            glm::vec2(buttonWidth, m_TabHeight),
            rightHovered ? theme.ButtonHovered : theme.ButtonNormal
        );
        Renderer::DrawText("►", rightPos + glm::vec2(buttonWidth * 0.5f, m_TabHeight * 0.5f), theme.TextNormal, 12.0f);
    }
}

bool UITabControl::HandleEvent(Event& event)
{
    if (!m_IsVisible) return false;

    m_LastMousePos = event.GetMousePosition();

    // Handle scrolling
    if (HandleScrolling(event))
        return true;

    // Handle tab bar events first
    if (HandleTabBarEvents(event))
        return true;

    // Handle drag and drop
    if (HandleDragAndDrop(event))
        return true;

    // Handle active tab content events
    if (m_ActiveTab && m_ActiveTab->Content) {
        return m_ActiveTab->Content->HandleEvent(event);
    }

    return false;
}

bool UITabControl::HandleScrolling(Event& event)
{
    if (!NeedsScrolling()) return false;

    switch (event.Type) {
        case EventType::MouseWheel: {
            if (IsPointInRect(m_LastMousePos, m_Position, glm::vec2(m_Size.x, m_TabHeight))) {
                m_ScrollOffset = std::clamp(
                    m_ScrollOffset - event.MouseWheel.YOffset * 30.0f,
                    0.0f,
                    GetMaxScrollOffset()
                );
                return true;
            }
            return false;
        }
        case EventType::MouseButton: {
            if (event.MouseButton.Button == MouseButton::Left &&
                event.MouseButton.Action == ButtonAction::Press) {
                // Check scroll buttons
                float buttonWidth = 20.0f;
                if (IsPointInRect(m_LastMousePos, m_Position, glm::vec2(buttonWidth, m_TabHeight))) {
                    m_ScrollVelocity = -200.0f; // Scroll left
                    return true;
                }
                if (IsPointInRect(m_LastMousePos, 
                    m_Position + glm::vec2(m_Size.x - buttonWidth, 0.0f),
                    glm::vec2(buttonWidth, m_TabHeight))) {
                    m_ScrollVelocity = 200.0f; // Scroll right
                    return true;
                }
            }
            return false;
        }
        default:
            return false;
    }
}

bool UITabControl::HandleDragAndDrop(Event& event)
{
    switch (event.Type) {
        case EventType::MouseButton: {
            if (event.MouseButton.Button == MouseButton::Left) {
                if (event.MouseButton.Action == ButtonAction::Press) {
                    // Start dragging
                    if (!m_IsDragging && m_HoveredTab) {
                        m_IsDragging = true;
                        m_DraggedTab = m_HoveredTab;
                        m_DraggedTabWidth = CalculateTabWidth(*m_DraggedTab);
                        m_DragStartPos = m_LastMousePos;
                        m_DragOffset = m_LastMousePos - glm::vec2(
                            GetTabXPosition(m_DraggedTab),
                            m_Position.y
                        );
                        return true;
                    }
                } else if (event.MouseButton.Action == ButtonAction::Release) {
                    // End dragging
                    if (m_IsDragging) {
                        FinalizeDragAndDrop();
                        return true;
                    }
                }
            }
            return false;
        }
        case EventType::MouseMove: {
            if (m_IsDragging) {
                // Update drag position and check for reordering
                float dragX = m_LastMousePos.x - m_DragOffset.x;
                int newIndex = GetTabIndexAtPosition(dragX + m_DraggedTabWidth * 0.5f);
                if (newIndex >= 0 && newIndex < static_cast<int>(m_Tabs.size())) {
                    int oldIndex = GetTabIndex(m_DraggedTab);
                    if (oldIndex != newIndex) {
                        MoveTab(oldIndex, newIndex);
                    }
                }
                return true;
            }
            return false;
        }
        default:
            return false;
    }
}

void UITabControl::FinalizeDragAndDrop()
{
    m_IsDragging = false;
    m_DraggedTab = nullptr;
}

float UITabControl::GetTabXPosition(const TabItem* tab) const
{
    float x = m_Position.x - m_ScrollOffset;
    for (const auto& t : m_Tabs) {
        if (&t == tab) break;
        x += CalculateTabWidth(t);
    }
    return x;
}

int UITabControl::GetTabIndexAtPosition(float x) const
{
    float currentX = m_Position.x - m_ScrollOffset;
    for (size_t i = 0; i < m_Tabs.size(); ++i) {
        float width = CalculateTabWidth(m_Tabs[i]);
        if (x >= currentX && x < currentX + width) {
            return static_cast<int>(i);
        }
        currentX += width;
    }
    return -1;
}

float UITabControl::GetMaxScrollOffset() const
{
    float totalWidth = 0.0f;
    for (const auto& tab : m_Tabs) {
        totalWidth += CalculateTabWidth(tab);
    }
    
    float visibleWidth = m_Size.x - (m_ShowAddButton ? m_TabHeight : 0.0f);
    return std::max(0.0f, totalWidth - visibleWidth);
}

bool UITabControl::NeedsScrolling() const
{
    return GetMaxScrollOffset() > 0.0f;
}

void UITabControl::MoveTab(int fromIndex, int toIndex)
{
    if (fromIndex == toIndex) return;
    if (fromIndex < 0 || fromIndex >= static_cast<int>(m_Tabs.size())) return;
    if (toIndex < 0 || toIndex >= static_cast<int>(m_Tabs.size())) return;

    TabItem tab = std::move(m_Tabs[fromIndex]);
    m_Tabs.erase(m_Tabs.begin() + fromIndex);
    m_Tabs.insert(m_Tabs.begin() + toIndex, std::move(tab));
}

} // namespace Editor
} // namespace VGE



