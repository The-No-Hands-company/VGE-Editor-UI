#include "U

#include "Runtime/Core/Public/Core/UICore.h"IScrollView.h"
#include "Graphics/RenderCommand.h"
#include "Runtime/Core/Public/Time.h"

namespace VGE {

UIScrollView::UIScrollView() {
    m_Name = "ScrollView";
    SetClipping(true);
}

void UIScrollView::Initialize() {
    UIWidget::Initialize();
}

void UIScrollView::Update(float deltaTime) {
    if (!m_IsVisible)
        return;

    // Update content widget if exists
    if (m_Content) {
        m_Content->Update(deltaTime);
    }

    // Update momentum scrolling
    if (m_Properties.EnableMomentum && !m_IsDragging) {
        UpdateMomentum(deltaTime);
    }

    // Update smooth scrolling
    if (m_Properties.EnableSmoothScrolling) {
        UpdateSmoothScrolling(deltaTime);
    }

    // Update scrollbar positions
    UpdateScrollbars();

    UIWidget::Update(deltaTime);
}

void UIScrollView::Render() {
    if (!m_IsVisible)
        return;

    // Render content
    if (m_Content) {
        // Set content position based on scroll
        m_Content->SetPosition(m_Position - m_ScrollPosition);
        m_Content->Render();
    }

    // Render scrollbars
    if (m_Properties.ShowHorizontalScrollbar && m_Properties.EnableHorizontal) {
        // Render horizontal scrollbar background
        RenderCommand::DrawRect(
            m_Position + glm::vec2(0.0f, m_Size.y - m_Properties.ScrollbarWidth),
            glm::vec2(m_Size.x - (m_Properties.ShowVerticalScrollbar ? m_Properties.ScrollbarWidth : 0.0f), m_Properties.ScrollbarWidth),
            m_IsHorizontalScrollbarHovered ? glm::vec4(0.3f, 0.3f, 0.3f, 1.0f) : glm::vec4(0.2f, 0.2f, 0.2f, 1.0f)
        );

        // Render horizontal scrollbar handle
        RenderCommand::DrawRect(
            m_Position + glm::vec2(m_HorizontalScrollbarPosition, m_Size.y - m_Properties.ScrollbarWidth),
            glm::vec2(m_HorizontalScrollbarSize, m_Properties.ScrollbarWidth),
            m_IsDraggingHorizontalScrollbar ? glm::vec4(0.6f, 0.6f, 0.6f, 1.0f) : glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)
        );
    }

    if (m_Properties.ShowVerticalScrollbar && m_Properties.EnableVertical) {
        // Render vertical scrollbar background
        RenderCommand::DrawRect(
            m_Position + glm::vec2(m_Size.x - m_Properties.ScrollbarWidth, 0.0f),
            glm::vec2(m_Properties.ScrollbarWidth, m_Size.y - (m_Properties.ShowHorizontalScrollbar ? m_Properties.ScrollbarWidth : 0.0f)),
            m_IsVerticalScrollbarHovered ? glm::vec4(0.3f, 0.3f, 0.3f, 1.0f) : glm::vec4(0.2f, 0.2f, 0.2f, 1.0f)
        );

        // Render vertical scrollbar handle
        RenderCommand::DrawRect(
            m_Position + glm::vec2(m_Size.x - m_Properties.ScrollbarWidth, m_VerticalScrollbarPosition),
            glm::vec2(m_Properties.ScrollbarWidth, m_VerticalScrollbarSize),
            m_IsDraggingVerticalScrollbar ? glm::vec4(0.6f, 0.6f, 0.6f, 1.0f) : glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)
        );
    }
}

void UIScrollView::SetContent(const Ref<UIWidget>& content) {
    m_Content = content;
    if (m_Content) {
        m_Content->SetParent(this);
        CalculateContentSize();
    }
}

void UIScrollView::ClearContent() {
    m_Content.reset();
    m_ScrollPosition = { 0.0f, 0.0f };
    m_TargetScrollPosition = { 0.0f, 0.0f };
    m_ScrollVelocity = { 0.0f, 0.0f };
}

void UIScrollView::ScrollTo(const glm::vec2& position, bool animate) {
    if (animate && m_Properties.EnableSmoothScrolling) {
        m_TargetScrollPosition = position;
    } else {
        m_ScrollPosition = position;
        m_TargetScrollPosition = position;
        ClampScrollPosition();
        NotifyScroll();
    }
}

void UIScrollView::ScrollToTop(bool animate) {
    ScrollTo({ m_ScrollPosition.x, 0.0f }, animate);
}

void UIScrollView::ScrollToBottom(bool animate) {
    ScrollTo({ m_ScrollPosition.x, CalculateMaxScroll().y }, animate);
}

void UIScrollView::ScrollToLeft(bool animate) {
    ScrollTo({ 0.0f, m_ScrollPosition.y }, animate);
}

void UIScrollView::ScrollToRight(bool animate) {
    ScrollTo({ CalculateMaxScroll().x, m_ScrollPosition.y }, animate);
}

void UIScrollView::SetProperties(const ScrollViewProperties& properties) {
    m_Properties = properties;
    CalculateContentSize();
}

float UIScrollView::GetScrollPercentageX() const {
    glm::vec2 maxScroll = CalculateMaxScroll();
    return maxScroll.x > 0.0f ? m_ScrollPosition.x / maxScroll.x : 0.0f;
}

float UIScrollView::GetScrollPercentageY() const {
    glm::vec2 maxScroll = CalculateMaxScroll();
    return maxScroll.y > 0.0f ? m_ScrollPosition.y / maxScroll.y : 0.0f;
}

void UIScrollView::OnMouseDown(const glm::vec2& position) {
    if (!m_IsVisible)
        return;

    m_IsDragging = true;
    m_LastMousePosition = position;
    m_DragStartPosition = position;
    m_LastDragTime = Time::GetTime();
    m_ScrollVelocity = { 0.0f, 0.0f };

    // Check if clicking on scrollbars
    if (m_Properties.ShowHorizontalScrollbar && IsScrollbarHovered(true)) {
        m_IsDraggingHorizontalScrollbar = true;
    }
    if (m_Properties.ShowVerticalScrollbar && IsScrollbarHovered(false)) {
        m_IsDraggingVerticalScrollbar = true;
    }
}

void UIScrollView::OnMouseUp(const glm::vec2& position) {
    if (!m_IsVisible)
        return;

    if (m_IsDragging) {
        float dragTime = Time::GetTime() - m_LastDragTime;
        if (dragTime > 0.0f && m_Properties.EnableMomentum) {
            m_ScrollVelocity = (position - m_DragStartPosition) / dragTime;
        }
    }

    m_IsDragging = false;
    m_IsDraggingHorizontalScrollbar = false;
    m_IsDraggingVerticalScrollbar = false;
}

void UIScrollView::OnMouseMove(const glm::vec2& position) {
    if (!m_IsVisible)
        return;

    // Update scrollbar hover states
    m_IsHorizontalScrollbarHovered = IsScrollbarHovered(true);
    m_IsVerticalScrollbarHovered = IsScrollbarHovered(false);

    if (m_IsDragging) {
        glm::vec2 delta = position - m_LastMousePosition;
        
        if (m_IsDraggingHorizontalScrollbar) {
            float scrollableWidth = m_Size.x - m_HorizontalScrollbarSize;
            float scrollPercentage = delta.x / scrollableWidth;
            m_ScrollPosition.x += scrollPercentage * m_ContentSize.x;
        }
        else if (m_IsDraggingVerticalScrollbar) {
            float scrollableHeight = m_Size.y - m_VerticalScrollbarSize;
            float scrollPercentage = delta.y / scrollableHeight;
            m_ScrollPosition.y += scrollPercentage * m_ContentSize.y;
        }
        else {
            if (m_Properties.EnableHorizontal)
                m_ScrollPosition.x -= delta.x;
            if (m_Properties.EnableVertical)
                m_ScrollPosition.y -= delta.y;
        }

        ClampScrollPosition();
        NotifyScroll();
    }

    m_LastMousePosition = position;
}

void UIScrollView::OnMouseWheel(float delta) {
    if (!m_IsVisible || !m_Properties.EnableVertical)
        return;

    float scrollAmount = delta * m_Properties.ScrollSpeed;
    if (m_Properties.EnableSmoothScrolling) {
        m_TargetScrollPosition.y -= scrollAmount;
    } else {
        m_ScrollPosition.y -= scrollAmount;
        ClampScrollPosition();
        NotifyScroll();
    }
}

void UIScrollView::OnResize(const glm::vec2& size) {
    UIWidget::OnResize(size);
    CalculateContentSize();
}

void UIScrollView::UpdateScrollbars() {
    if (!m_Content)
        return;

    // Calculate scrollbar sizes and positions
    glm::vec2 maxScroll = CalculateMaxScroll();
    
    if (m_Properties.ShowHorizontalScrollbar && maxScroll.x > 0.0f) {
        float viewportRatio = m_Size.x / m_ContentSize.x;
        m_HorizontalScrollbarSize = std::max(viewportRatio * m_Size.x, m_Properties.MinimumScrollbarLength);
        float scrollableWidth = m_Size.x - m_HorizontalScrollbarSize;
        m_HorizontalScrollbarPosition = (m_ScrollPosition.x / maxScroll.x) * scrollableWidth;
    }

    if (m_Properties.ShowVerticalScrollbar && maxScroll.y > 0.0f) {
        float viewportRatio = m_Size.y / m_ContentSize.y;
        m_VerticalScrollbarSize = std::max(viewportRatio * m_Size.y, m_Properties.MinimumScrollbarLength);
        float scrollableHeight = m_Size.y - m_VerticalScrollbarSize;
        m_VerticalScrollbarPosition = (m_ScrollPosition.y / maxScroll.y) * scrollableHeight;
    }
}

void UIScrollView::UpdateMomentum(float deltaTime) {
    if (glm::length2(m_ScrollVelocity) > 0.0f) {
        m_ScrollPosition += m_ScrollVelocity * deltaTime;
        m_ScrollVelocity *= m_Properties.MomentumDecay;

        if (glm::length2(m_ScrollVelocity) < 0.01f) {
            m_ScrollVelocity = { 0.0f, 0.0f };
        }

        ClampScrollPosition();
        NotifyScroll();
    }
}

void UIScrollView::UpdateSmoothScrolling(float deltaTime) {
    glm::vec2 delta = m_TargetScrollPosition - m_ScrollPosition;
    if (glm::length2(delta) > 0.01f) {
        m_ScrollPosition += delta * (deltaTime * m_Properties.SmoothScrollingSpeed);
        ClampScrollPosition();
        NotifyScroll();
    }
}

bool UIScrollView::IsScrollbarHovered(bool horizontal) const {
    glm::vec2 localMousePos = m_LastMousePosition - m_Position;

    if (horizontal) {
        return localMousePos.y >= m_Size.y - m_Properties.ScrollbarWidth &&
               localMousePos.y <= m_Size.y &&
               localMousePos.x >= m_HorizontalScrollbarPosition &&
               localMousePos.x <= m_HorizontalScrollbarPosition + m_HorizontalScrollbarSize;
    } else {
        return localMousePos.x >= m_Size.x - m_Properties.ScrollbarWidth &&
               localMousePos.x <= m_Size.x &&
               localMousePos.y >= m_VerticalScrollbarPosition &&
               localMousePos.y <= m_VerticalScrollbarPosition + m_VerticalScrollbarSize;
    }
}

void UIScrollView::ClampScrollPosition() {
    glm::vec2 maxScroll = CalculateMaxScroll();
    m_ScrollPosition = glm::clamp(m_ScrollPosition, glm::vec2(0.0f), maxScroll);
    m_TargetScrollPosition = glm::clamp(m_TargetScrollPosition, glm::vec2(0.0f), maxScroll);
}

void UIScrollView::CalculateContentSize() {
    if (m_Content) {
        m_ContentSize = m_Content->GetSize();
    } else {
        m_ContentSize = { 0.0f, 0.0f };
    }
}

glm::vec2 UIScrollView::CalculateMaxScroll() const {
    return glm::max(m_ContentSize - m_Size, glm::vec2(0.0f));
}

void UIScrollView::NotifyScroll() {
    if (m_OnScrollCallback) {
        m_OnScrollCallback(m_ScrollPosition);
    }
}

} // namespace VGE



