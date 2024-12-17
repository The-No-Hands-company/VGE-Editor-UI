#pragma once

#include "Runtime/Core/Public/Core/UICore.h"
#include "Runtime/Core/Public/Widget/UIWidget.h"
#include "Runtime/Core/Public/Layout/UILayoutableBase.h"
#include <memory>
#include <vector>

namespace VGE {
namespace UI {

enum class HorizontalAlignment {
    Left,
    Center,
    Right,
    Stretch
};

class UI_API UIVerticalLayout : public UIWidget {
public:
    struct LayoutOptions {
        float spacing = 5.0f;
        float padding = 10.0f;
        HorizontalAlignment alignment = HorizontalAlignment::Center;
    };

    UIVerticalLayout(const std::string& title = "");
    virtual ~UIVerticalLayout() = default;

    void SetLayoutOptions(const LayoutOptions& options) { m_LayoutOptions = options; InvalidateLayout(); }
    const LayoutOptions& GetLayoutOptions() const { return m_LayoutOptions; }

    // Element management
    void AddElement(std::shared_ptr<UIWidget> element);
    void RemoveElement(std::shared_ptr<UIWidget> element);
    void Clear();

    // UIWidget interface
    virtual void OnUpdate(float deltaTime) override;
    virtual void OnDraw(UIRenderer& renderer) override;
    virtual bool OnMouseMove(const glm::vec2& position) override;
    virtual bool OnMouseDown(const glm::vec2& position) override;
    virtual bool OnMouseUp(const glm::vec2& position) override;

protected:
    virtual void OnLayoutUpdate() override;
    virtual glm::vec2 CalculateDesiredSize() const override;

private:
    LayoutOptions m_LayoutOptions;
    std::vector<std::shared_ptr<UIWidget>> m_Elements;
};

}} // namespace VGE::UI 