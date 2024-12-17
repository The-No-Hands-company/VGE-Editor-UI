#pragma once

#include "Runtime/Core/Public/Core/UICore.h"
#include "Runtime/Core/Public/Core/UITypes.h"
#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace VGE {
namespace UI {

class UIRenderer;
class UISlot;

class UI_API UIWidget : public std::enable_shared_from_this<UIWidget> {
public:
    UIWidget();
    virtual ~UIWidget() = default;

    // Layout methods
    virtual void OnLayoutUpdate() = 0;
    virtual glm::vec2 CalculateDesiredSize() = 0;
    virtual void ArrangeChildren() = 0;

    // Paint methods
    virtual void OnPaint(UIRenderer* renderer) = 0;

    // Child management
    virtual void OnChildAdded(std::shared_ptr<UIWidget> child) = 0;
    virtual void OnChildRemoved(std::shared_ptr<UIWidget> child) = 0;

    // Position and size
    void SetPosition(const glm::vec2& position) { m_Position = position; }
    void SetSize(const glm::vec2& size) { m_Size = size; }
    const glm::vec2& GetPosition() const { return m_Position; }
    const glm::vec2& GetSize() const { return m_Size; }
    glm::vec4 GetBounds() const { return glm::vec4(m_Position.x, m_Position.y, m_Size.x, m_Size.y); }

    // Getters
    const UIGeometry& GetGeometry() const { return m_Geometry; }
    UIGeometry& GetGeometry() { return m_Geometry; }
    
    const Style& GetStyle() const { return m_Style; }
    Style& GetStyle() { return m_Style; }

    bool IsVisible() const { return m_IsVisible; }
    void SetVisible(bool visible) { m_IsVisible = visible; InvalidateLayout(); }

    // Layout invalidation
    void InvalidateLayout() {
        m_NeedsLayout = true;
        if (auto parent = m_Parent.lock()) {
            parent->InvalidateLayout();
        }
    }

    // Parent-child relationship
    void SetParent(std::shared_ptr<UIWidget> parent) { m_Parent = parent; }
    std::shared_ptr<UIWidget> GetParent() const { return m_Parent.lock(); }
    const std::vector<std::shared_ptr<UIWidget>>& GetChildren() const { return m_Children; }

protected:
    glm::vec2 m_Position;
    glm::vec2 m_Size;
    UIGeometry m_Geometry;
    Style m_Style;
    bool m_IsVisible{true};
    bool m_NeedsLayout{true};
    std::weak_ptr<UIWidget> m_Parent;
    std::vector<std::shared_ptr<UIWidget>> m_Children;
};

}} // namespace VGE::UI
