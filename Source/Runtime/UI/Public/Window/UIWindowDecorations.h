#pragma once

#include "Runtime/Core/Public/Math/Vector.h"
#include "Runtime/Core/Public/Widget/UIWidget.h"
#include <string>

namespace VGE {
namespace UI {

// Forward declarations
class UIWindow;

/**
 * @brief Style settings for window decorations
 */
struct DecorationStyle
{
    float titleBarHeight = 30.0f;      ///< Height of the title bar
    float borderWidth = 4.0f;          ///< Width of resizable borders
    float buttonSize = 24.0f;          ///< Size of window control buttons
    float iconSize = 16.0f;            ///< Size of window icon
    float buttonSpacing = 5.0f;        ///< Spacing between window control buttons
    float titlePadding = 8.0f;         ///< Padding around window title text
    float iconPadding = 7.0f;          ///< Padding around window icon

    // Colors for different states
    glm::vec4 activeColor = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);             ///< Color when window is active
    glm::vec4 inactiveColor = glm::vec4(0.3f, 0.3f, 0.3f, 1.0f);          ///< Color when window is inactive
    glm::vec4 buttonHoverColor = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);       ///< Button hover state color
    glm::vec4 buttonActiveColor = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);      ///< Button pressed state color
    glm::vec4 textColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);              ///< Title text color
    glm::vec4 closeButtonHoverColor = glm::vec4(0.8f, 0.2f, 0.2f, 1.0f);  ///< Close button hover color
    glm::vec4 closeButtonActiveColor = glm::vec4(0.6f, 0.15f, 0.15f, 1.0f); ///< Close button pressed color
};

/**
 * @brief Manages window decorations including title bar, borders, and control buttons
 */
class UIWindowDecorations : public UIWidget
{
public:
    explicit UIWindowDecorations(UIWindow* window);
    virtual ~UIWindowDecorations() = default;

    // UIWidget interface
    void Render() override;
    void Update(float deltaTime) override;

    // Style management
    void SetStyle(const DecorationStyle& style);
    const DecorationStyle& GetStyle() const { return m_Style; }

    // Hit testing
    bool HitTest(const glm::vec2& position, bool& outResizable) const;
    bool HitTestTitleBar(const glm::vec2& position) const;
    bool HitTestBorders(const glm::vec2& position, bool& outResizable) const;
    bool HitTestButtons(const glm::vec2& position) const;

    // Mouse event handlers
    void OnMouseDown(const glm::vec2& position);
    void OnMouseUp(const glm::vec2& position);
    void OnMouseMove(const glm::vec2& position);
    void OnDoubleClick(const glm::vec2& position);

    // Rendering components
    void RenderTitleBar();
    void RenderBorders();
    void RenderButtons();
    void RenderIcon();

    // Visibility control
    void ShowTitleBar(bool show) { m_ShowTitleBar = show; }
    void ShowBorders(bool show) { m_ShowBorders = show; }
    void ShowIcon(bool show) { m_ShowIcon = show; }
    void ShowButtons(bool show) { m_ShowButtons = show; }

    bool IsTitleBarVisible() const { return m_ShowTitleBar; }
    bool AreBordersVisible() const { return m_ShowBorders; }
    bool IsIconVisible() const { return m_ShowIcon; }
    bool AreButtonsVisible() const { return m_ShowButtons; }

    // State management
    bool IsDragging() const { return m_IsDragging; }
    bool IsResizing() const { return m_IsResizing; }
    void SetActive(bool active) { m_IsActive = active; }
    bool IsActive() const { return m_IsActive; }

protected:
    // UIWidget event handlers
    void OnMousePressed(const glm::vec2& position) override { OnMouseDown(position); }
    void OnMouseReleased(const glm::vec2& position) override { OnMouseUp(position); }
    void OnMouseMoved(const glm::vec2& position) override { OnMouseMove(position); }
    void OnMouseDoubleClicked(const glm::vec2& position) override { OnDoubleClick(position); }

private:
    // Helper functions
    int GetButtonAtPosition(const glm::vec2& position) const;
    void HandleButtonClick(int buttonIndex);
    void HandleResize(const glm::vec2& position);
    void RenderButtonIcon(int buttonIndex, const glm::vec2& position);

private:
    UIWindow* m_Window;                ///< Owner window
    DecorationStyle m_Style;           ///< Decoration style settings

    // Visibility flags
    bool m_ShowTitleBar;               ///< Show title bar flag
    bool m_ShowBorders;                ///< Show borders flag
    bool m_ShowIcon;                   ///< Show window icon flag
    bool m_ShowButtons;                ///< Show window buttons flag

    // State flags
    bool m_IsActive;                   ///< Window active state
    bool m_IsResizing;                 ///< Window resizing state
    bool m_IsDragging;                 ///< Window dragging state
    glm::vec2 m_DragOffset;           ///< Drag operation offset
    glm::vec2 m_ResizeStart;          ///< Resize operation start position
    int m_HoveredButton;              ///< Currently hovered button index
    int m_ActiveButton;               ///< Currently pressed button index
};

}} // namespace VGE::UI