#pragma once

#include "Runtime/Core/Public/Core/UICore.h"

#include "Runtime/Core/Public/UIWidget.h"
#include "Runtime/Core/Public/Core/UITypes.h"
#include "Runtime/Core/Public/UIRenderer.h"
#include <string>

namespace VGE {
namespace UI {

class UI_API UIText : public UIWidget {
public:
    UIText(const std::string& text = "");
    virtual ~UIText() = default;

    // Core functionality
    virtual void Draw(UIRenderer& renderer) override;
    virtual void Update(float deltaTime) override;

protected:
    // UIElement interface implementation
    virtual void OnDraw(UIRenderer& renderer) override;
    virtual void OnUpdate(float deltaTime) override;
    virtual void OnPositionChanged() override;
    virtual void OnSizeChanged() override;
    virtual void OnChildAdded(std::shared_ptr<UIElement> child) override;
    virtual void OnChildRemoved(std::shared_ptr<UIElement> child) override;
    virtual void OnParentPositionChanged() override;

public:
    // Text properties
    void SetText(const std::string& text);
    const std::string& GetText() const { return m_Text; }
    void SetColor(const Color& color) { m_Color = color; }
    const Color& GetColor() const { return m_Color; }
    void SetFontSize(float size) { m_FontSize = size; }
    float GetFontSize() const { return m_FontSize; }
    void SetAlignment(TextAlignment hAlign, TextAlignment vAlign) {
        m_HorizontalAlignment = hAlign;
        m_VerticalAlignment = vAlign;
    }
    TextAlignment GetHorizontalAlignment() const { return m_HorizontalAlignment; }
    TextAlignment GetVerticalAlignment() const { return m_VerticalAlignment; }

protected:
    std::string m_Text;
    Color m_Color{1.0f, 1.0f, 1.0f, 1.0f};
    float m_FontSize{14.0f};
    TextAlignment m_HorizontalAlignment{TextAlignment::Left};
    TextAlignment m_VerticalAlignment{TextAlignment::Top};
};

}} // namespace VGE::UI



