#pragma once

#include "Runtime/Core/Public/UIWidget.h"
#include "Runtime/Core/Public/Core/UITypes.h"
#include "Runtime/Core/Public/Core/UICore.h"

namespace VGE {
namespace UI {

enum class ButtonStyle {
    Default,
    Primary,
    Secondary,
    Success,
    Danger,
    Warning,
    Info,
    Link
};

class UI_API UIButton : public UIWidget {
public:
    UIButton(const std::string& text = "", ButtonStyle style = ButtonStyle::Default);
    virtual ~UIButton() = default;

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

    // Mouse event handlers
    virtual bool OnMouseMove(const glm::vec2& mousePos) override;
    virtual bool OnMouseDown(const glm::vec2& mousePos) override;
    virtual bool OnMouseUp(const glm::vec2& mousePos) override;
    virtual void OnMouseEnter() override;
    virtual void OnMouseLeave() override;
    virtual void OnPressed() override;
    virtual void OnReleased() override;

public:
    // Getters
    const std::string& GetText() const { return m_Text; }
    ButtonStyle GetStyle() const { return m_ButtonStyle; }
    bool IsPressed() const { return m_State == UIWidgetState::Pressed; }
    bool IsToggled() const { return m_Toggled; }

    // Setters
    void SetText(const std::string& text) { m_Text = text; }
    void SetButtonStyle(ButtonStyle style) { m_ButtonStyle = style; }
    void SetToggled(bool toggled) { m_Toggled = toggled; }
    void SetToggleable(bool toggleable) { m_Toggleable = toggleable; }

    // Event handlers
    void SetClickCallback(std::function<void()> callback) { m_ClickCallback = callback; }

protected:
    std::string m_Text;
    ButtonStyle m_ButtonStyle;
    std::function<void()> m_ClickCallback;
    bool m_Toggleable = false;
    bool m_Toggled = false;
    float m_PressAnimation = 0.0f;
    float m_HoverAnimation = 0.0f;
};

} // namespace UI
} // namespace VGE


