#pragma once

#include "Runtime/Core/Public/Widget/UIWidget.h"
#include "Runtime/Core/Public/Widget/UISlot.h"
#include <memory>
#include <vector>
#include <glm/glm.hpp>

namespace VGE {
namespace UI {

enum class ContainerLayout {
    None,       // No automatic layout
    Vertical,   // Stack children vertically
    Horizontal, // Stack children horizontally
    Grid        // Arrange children in a grid
};

class UI_API UIContainer : public UIWidget {
public:
    UIContainer();
    virtual ~UIContainer() = default;

    // UIWidget interface
    void OnLayoutUpdate() override;
    glm::vec2 CalculateDesiredSize() override;
    void ArrangeChildren() override;
    void OnPaint(UIRenderer* renderer) override;
    void OnChildAdded(std::shared_ptr<UIWidget> child) override;
    void OnChildRemoved(std::shared_ptr<UIWidget> child) override;

    // Container properties
    void SetLayout(ContainerLayout layout);
    ContainerLayout GetLayout() const { return m_Layout; }

    void SetSpacing(float spacing) { m_Spacing = spacing; InvalidateLayout(); }
    float GetSpacing() const { return m_Spacing; }

    void SetPadding(float left, float top, float right, float bottom) { m_Padding = glm::vec4(left, top, right, bottom); InvalidateLayout(); }
    const glm::vec4& GetPadding() const { return m_Padding; }

    void SetGridColumns(int columns) { m_GridColumns = columns; InvalidateLayout(); }
    int GetGridColumns() const { return m_GridColumns; }

    // Container-specific methods
    void AddChild(std::shared_ptr<UIWidget> child);
    void RemoveChild(std::shared_ptr<UIWidget> child);
    void ClearChildren();

protected:
    std::vector<std::shared_ptr<UISlot>> m_Slots;

private:
    ContainerLayout m_Layout = ContainerLayout::None;
    float m_Spacing = 5.0f;
    glm::vec4 m_Padding = glm::vec4(5.0f);
    int m_GridColumns = 2;

    // Layout calculation helpers
    glm::vec2 CalculateVerticalLayout() const;
    glm::vec2 CalculateHorizontalLayout() const;
    glm::vec2 CalculateGridLayout() const;

    void ArrangeVertical(const glm::vec4& bounds);
    void ArrangeHorizontal(const glm::vec4& bounds);
    void ArrangeGrid(const glm::vec4& bounds);
};

}} // namespace VGE::UI
