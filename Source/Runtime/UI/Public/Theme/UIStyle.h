#pragma once

#include "Runtime/Core/Public/Core/UICore.h"
#include <glm/glm.hpp>
#include <string>

namespace VGE {
namespace UI {

/**
 * @brief Common style settings for UI elements
 */
struct CommonStyle {
    float padding = 4.0f;           ///< Default padding
    float spacing = 4.0f;           ///< Default spacing
    float borderWidth = 1.0f;       ///< Default border width
    float cornerRadius = 2.0f;      ///< Default corner radius
    float fontSize = 14.0f;         ///< Default font size
    std::string fontFamily = "Arial"; ///< Default font family
};

/**
 * @brief Colors for different UI states
 */
struct StateColors {
    glm::vec4 normal = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);      ///< Normal state
    glm::vec4 hover = glm::vec4(0.3f, 0.3f, 0.3f, 1.0f);       ///< Hover state
    glm::vec4 active = glm::vec4(0.15f, 0.15f, 0.15f, 1.0f);   ///< Active/pressed state
    glm::vec4 disabled = glm::vec4(0.5f, 0.5f, 0.5f, 0.5f);    ///< Disabled state
    glm::vec4 focused = glm::vec4(0.25f, 0.25f, 0.25f, 1.0f);  ///< Focused state
};

/**
 * @brief Button-specific style settings
 */
struct ButtonStyle {
    StateColors colors;             ///< Button state colors
    float height = 24.0f;          ///< Button height
    float minWidth = 80.0f;        ///< Minimum button width
    float iconSize = 16.0f;        ///< Icon size
    float iconSpacing = 4.0f;      ///< Spacing between icon and text
};

/**
 * @brief Input field style settings
 */
struct InputStyle {
    StateColors colors;             ///< Input state colors
    float height = 24.0f;          ///< Input field height
    float caretWidth = 2.0f;       ///< Text caret width
    float caretBlinkRate = 0.5f;   ///< Text caret blink rate
    glm::vec4 selectionColor = glm::vec4(0.2f, 0.4f, 0.8f, 0.5f); ///< Text selection color
};

/**
 * @brief Scrollbar style settings
 */
struct ScrollbarStyle {
    StateColors trackColors;        ///< Track state colors
    StateColors thumbColors;        ///< Thumb state colors
    float width = 12.0f;           ///< Scrollbar width
    float minThumbSize = 20.0f;    ///< Minimum thumb size
};

/**
 * @brief Complete UI style settings
 */
class UIStyle {
public:
    UIStyle() = default;
    virtual ~UIStyle() = default;

    // Common settings
    CommonStyle common;

    // Widget-specific styles
    ButtonStyle button;
    InputStyle input;
    ScrollbarStyle scrollbar;

    // Global colors
    glm::vec4 textColor = glm::vec4(1.0f);
    glm::vec4 backgroundColor = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
    glm::vec4 borderColor = glm::vec4(0.3f, 0.3f, 0.3f, 1.0f);
    glm::vec4 accentColor = glm::vec4(0.2f, 0.4f, 0.8f, 1.0f);

    // Utility functions
    void ScaleAll(float scale);
    void Reset();

protected:
    virtual void OnStyleChanged();
};

}} // namespace VGE::UI 