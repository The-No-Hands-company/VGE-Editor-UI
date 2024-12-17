#pragma once

#include "Runtime/Core/Public/Core/UICore.h"
#include "Runtime/Core/Public/Widget/UIWidget.h"
#include "Runtime/Core/Public/Layout/UILayoutableBase.h"
#include <glm/glm.hpp>
#include <memory>

namespace VGE {
namespace UI {

class UI_API UIHorizontalLayout : public UIWidget {
public:
    UIHorizontalLayout(const std::string& title = "");
    virtual ~UIHorizontalLayout() = default;

    // Element management
    void AddElement(std::shared_ptr<UIWidget> element);
    void RemoveElement(std::shared_ptr<UIWidget> element);
    void Clear();

    // Layout properties
    void SetSpacing(float spacing) { m_Spacing = spacing; InvalidateLayout(); }
    float GetSpacing() const { return m_Spacing; }

    void SetPadding(const glm::vec4& padding) { m_Padding = padding; InvalidateLayout(); }
    const glm::vec4& GetPadding() const { return m_Padding; }

    void SetMinWidth(float width) { m_MinWidth = width; InvalidateLayout(); }
    float GetMinWidth() const { return m_MinWidth; }

    void SetMinHeight(float height) { m_MinHeight = height; InvalidateLayout(); }
    float GetMinHeight() const { return m_MinHeight; }

    // UIWidget interface
    virtual void OnLayoutUpdate() override;
    virtual glm::vec2 CalculateDesiredSize() override;
    virtual void ArrangeChildren() override;
    virtual void OnPaint(UIRenderer* renderer) override;
    virtual void OnChildAdded(std::shared_ptr<UIWidget> child) override;
    virtual void OnChildRemoved(std::shared_ptr<UIWidget> child) override;

private:
    float m_Spacing = 5.0f;
    glm::vec4 m_Padding = glm::vec4(5.0f);
    float m_MinWidth = 100.0f;
    float m_MinHeight = 30.0f;
};

}} // namespace VGE::UI 