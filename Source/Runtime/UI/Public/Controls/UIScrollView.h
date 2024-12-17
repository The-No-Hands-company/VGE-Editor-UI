#pragma once

#include "Runtime/Core/Public/Core/UICore.h"
#include "Runtime/Core/Public/Widget/UIWidget.h"
#include "Runtime/Core/Public/Math/Math.h"

#include <functional>

namespace VGE {
namespace UI {

struct ScrollViewProperties {
    bool EnableHorizontal = true;
    bool EnableVertical = true;
    bool ShowHorizontalScrollbar = true;
    bool ShowVerticalScrollbar = true;
    bool EnableMomentum = true;
    bool EnableSmoothScrolling = true;
    float ScrollSpeed = 1.0f;
    float MomentumDecay = 0.95f;
    float SmoothScrollingSpeed = 10.0f;
    float ScrollbarWidth = 12.0f;
    float MinimumScrollbarLength = 30.0f;
};

class UI_API UIScrollView : public UIWidget {
public:
    UIScrollView();
    virtual ~UIScrollView() = default;

    virtual void Initialize() override;
    virtual void Update(float deltaTime) override;
    virtual void Render() override;

    // Content management
    void SetContent(const Ref<UIWidget>& content);
    Ref<UIWidget> GetContent() const { return m_Content; }
    void ClearContent();

    // Scrolling control
    void ScrollTo(const glm::vec2& position, bool animate = true);
    void ScrollToTop(bool animate = true);
    void ScrollToBottom(bool animate = true);
    void ScrollToLeft(bool animate = true);
    void ScrollToRight(bool animate = true);

    // Properties
    void SetProperties(const ScrollViewProperties& properties);
    const ScrollViewProperties& GetProperties() const { return m_Properties; }

    // Scroll position
    const glm::vec2& GetScrollPosition() const { return m_ScrollPosition; }
    const glm::vec2& GetTargetScrollPosition() const { return m_TargetScrollPosition; }
    float GetScrollPercentageX() const;
    float GetScrollPercentageY() const;

    // Event handlers
    void SetOnScrollCallback(const std::function<void(const glm::vec2&)>& callback) { m_OnScrollCallback = callback; }

protected:
    virtual void OnMouseDown(const glm::vec2& position) override;
    virtual void OnMouseUp(const glm::vec2& position) override;
    virtual void OnMouseMove(const glm::vec2& position) override;
    virtual void OnMouseWheel(float delta) override;
    virtual void OnResize(const glm::vec2& size) override;

private:
    void UpdateScrollbars();
    void UpdateMomentum(float deltaTime);
    void UpdateSmoothScrolling(float deltaTime);
    bool IsScrollbarHovered(bool horizontal) const;
    void ClampScrollPosition();
    void CalculateContentSize();
    glm::vec2 CalculateMaxScroll() const;
    void NotifyScroll();

private:
    ScrollViewProperties m_Properties;
    Ref<UIWidget> m_Content;
    
    // Scroll state
    glm::vec2 m_ScrollPosition = { 0.0f, 0.0f };
    glm::vec2 m_TargetScrollPosition = { 0.0f, 0.0f };
    glm::vec2 m_ScrollVelocity = { 0.0f, 0.0f };
    glm::vec2 m_ContentSize = { 0.0f, 0.0f };
    
    // Interaction state
    bool m_IsDragging = false;
    glm::vec2 m_LastMousePosition = { 0.0f, 0.0f };
    glm::vec2 m_DragStartPosition = { 0.0f, 0.0f };
    float m_LastDragTime = 0.0f;
    
    // Scrollbar state
    bool m_IsHorizontalScrollbarHovered = false;
    bool m_IsVerticalScrollbarHovered = false;
    bool m_IsDraggingHorizontalScrollbar = false;
    bool m_IsDraggingVerticalScrollbar = false;
    float m_HorizontalScrollbarPosition = 0.0f;
    float m_VerticalScrollbarPosition = 0.0f;
    float m_HorizontalScrollbarSize = 0.0f;
    float m_VerticalScrollbarSize = 0.0f;
    
    // Callbacks
    std::function<void(const glm::vec2&)> m_OnScrollCallback;
};

} // namespace UI
} // namespace VGE



