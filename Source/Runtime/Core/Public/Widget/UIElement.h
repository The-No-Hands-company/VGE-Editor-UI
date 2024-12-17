#pragma once

#include "Runtime/Core/Public/Core/UICore.h"

#include "Runtime/Core/Public/Core/UIForward.h"
#include "Runtime/Core/Public/Core/UITypes.h"
#include "UISlot.h"
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>
#include <functional>

namespace VGE {
namespace UI {

// Forward declarations
class UIRenderer;
struct UIGeometry;
class UIStyle;

// Delegate types
template<typename... Args>
using UIDelegate = std::function<void(Args...)>;

using UIMouseEvent = UIDelegate<const glm::vec2&>;
using UIClickEvent = UIDelegate<>;

class UI_API UIElement : public std::enable_shared_from_this<UIElement> {
public:
    UIElement();
    virtual ~UIElement() = default;

    // Core widget interface
    virtual void OnPaint(UIRenderer& renderer, const UIGeometry& geometry);
    virtual glm::vec2 ComputeDesiredSize() const;
    virtual void ArrangeChildren(const UIGeometry& allottedGeometry);

    // Input handling
    virtual bool OnMouseMove(const glm::vec2& position);
    virtual bool OnMouseEnter();
    virtual bool OnMouseLeave();
    virtual bool OnMouseButtonDown(const glm::vec2& position);
    virtual bool OnMouseButtonUp(const glm::vec2& position);
    virtual bool OnMouseDown(const glm::vec2& position) { return OnMouseButtonDown(position); }
    virtual bool OnMouseUp(const glm::vec2& position) { return OnMouseButtonUp(position); }
    virtual bool OnMouseScroll(float xOffset, float yOffset) { return false; }
    virtual bool OnKeyDown(int key) { return false; }
    virtual bool OnKeyUp(int key) { return false; }
    virtual bool OnChar(unsigned int codepoint) { return false; }
    virtual bool OnFocusGained() { return false; }
    virtual bool OnFocusLost() { return false; }

    // Event binding
    template<typename Func>
    void OnClicked(Func&& handler) {
        m_OnClicked = std::forward<Func>(handler);
    }

    template<typename Func>
    void OnHovered(Func&& handler) {
        m_OnHovered = std::forward<Func>(handler);
    }

    // Slot management
    void AssignSlot(const UISlot& slot);
    const UISlot& GetSlot() const { return m_Slot; }

    // Child management (now uses slots)
    virtual void AddChild(const std::shared_ptr<UIElement>& child, const UISlot& slot);
    virtual void RemoveChild(const std::shared_ptr<UIElement>& child);
    const std::vector<UISlot>& GetChildSlots() const { return m_ChildSlots; }

    // Visibility
    void SetVisibility(bool visible) { m_IsVisible = visible; }
    bool IsVisible() const { return m_IsVisible; }

    // Enable/Disable
    void SetEnabled(bool enabled) { m_IsEnabled = enabled; }
    bool IsEnabled() const { return m_IsEnabled; }

    // Style
    void SetStyle(const std::shared_ptr<UIStyle>& style) { m_Style = style; }
    const std::shared_ptr<UIStyle>& GetStyle() const { return m_Style; }

    // Geometry
    const UIGeometry& GetCachedGeometry() const { return m_CachedGeometry; }

protected:
    // Event handlers
    UIClickEvent m_OnClicked;
    UIMouseEvent m_OnHovered;

    // State
    bool m_IsVisible;
    bool m_IsEnabled;
    bool m_IsHovered;

    // Layout
    UISlot m_Slot;
    std::vector<UISlot> m_ChildSlots;
    UIGeometry m_CachedGeometry;

    // Style
    std::shared_ptr<UIStyle> m_Style;

    // Parent reference (weak to avoid cycles)
    std::weak_ptr<UIElement> m_Parent;

    // Helper methods
    virtual void InvalidateLayout();
    virtual void CacheGeometry(const UIGeometry& geometry);
};

// Helper for creating widgets with fluent interface
template<typename WidgetType>
class UIWidgetBuilder {
public:
    explicit UIWidgetBuilder(std::shared_ptr<WidgetType> widget) : m_Widget(widget) {}

    template<typename Func>
    UIWidgetBuilder& OnClicked(Func&& handler) {
        m_Widget->OnClicked(std::forward<Func>(handler));
        return *this;
    }

    template<typename Func>
    UIWidgetBuilder& OnHovered(Func&& handler) {
        m_Widget->OnHovered(std::forward<Func>(handler));
        return *this;
    }

    UIWidgetBuilder& Visibility(bool visible) {
        m_Widget->SetVisibility(visible);
        return *this;
    }

    UIWidgetBuilder& Enable(bool enabled) {
        m_Widget->SetEnabled(enabled);
        return *this;
    }

    UIWidgetBuilder& Style(const std::shared_ptr<UIStyle>& style) {
        m_Widget->SetStyle(style);
        return *this;
    }

    std::shared_ptr<WidgetType> Build() {
        return m_Widget;
    }

private:
    std::shared_ptr<WidgetType> m_Widget;
};

}} // namespace VGE::UI



