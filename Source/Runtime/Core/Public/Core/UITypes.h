#pragma once

#include "Runtime/Core/Public/Core/UICore.h"
#include <glm/glm.hpp>

namespace VGE {
namespace UI {

// Forward declarations
class UIRenderer;

// Default values
constexpr float UI_DEFAULT_MARGIN = 5.0f;
constexpr float UI_DEFAULT_PADDING = 5.0f;

// Font rendering types
struct Character {
    unsigned int TextureID;  // ID handle of the glyph texture
    glm::ivec2   Size;      // Size of glyph
    glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
    unsigned int Advance;    // Offset to advance to next glyph
};

struct UI_API Style {
    // Colors
    Color backgroundColor{0.2f, 0.2f, 0.2f, 1.0f};
    Color borderColor{0.3f, 0.3f, 0.3f, 1.0f};
    Color textColor{1.0f, 1.0f, 1.0f, 1.0f};
    Color titleBarColor{0.25f, 0.25f, 0.25f, 1.0f};
    Color titleBarTextColor{1.0f, 1.0f, 1.0f, 1.0f};
    Color hoverColor{0.3f, 0.3f, 0.3f, 1.0f};
    Color activeColor{0.35f, 0.35f, 0.35f, 1.0f};
    Color disabledColor{0.15f, 0.15f, 0.15f, 0.5f};

    // Dimensions
    float cornerRadius{3.0f};
    float borderThickness{1.0f};
    float padding{5.0f};
    float titleBarHeight{25.0f};
    float minWidth{100.0f};
    float minHeight{50.0f};
    float resizeHandleSize{10.0f};
    float borderWidth{1.0f}; // Added for UIDialog
};

// Theme colors
struct UI_API ThemeColors {
    Color primary{0.2f, 0.6f, 1.0f, 1.0f};
    Color onPrimary{1.0f, 1.0f, 1.0f, 1.0f};
    Color secondary{0.5f, 0.5f, 0.5f, 1.0f};
    Color onSecondary{1.0f, 1.0f, 1.0f, 1.0f};
    Color background{0.15f, 0.15f, 0.15f, 1.0f};
    Color onBackground{1.0f, 1.0f, 1.0f, 1.0f};
    Color surface{0.2f, 0.2f, 0.2f, 1.0f};
    Color onSurface{1.0f, 1.0f, 1.0f, 1.0f};
    Color error{1.0f, 0.3f, 0.3f, 1.0f};
    Color onError{1.0f, 1.0f, 1.0f, 1.0f};
    Color success{0.3f, 0.8f, 0.3f, 1.0f};
    Color onSuccess{1.0f, 1.0f, 1.0f, 1.0f};
    Color warning{1.0f, 0.8f, 0.3f, 1.0f};
    Color onWarning{0.0f, 0.0f, 0.0f, 1.0f};
    Color info{0.3f, 0.8f, 1.0f, 1.0f};
    Color onInfo{1.0f, 1.0f, 1.0f, 1.0f};
    Color hover{1.0f, 1.0f, 1.0f, 0.1f};
    Color pressed{0.0f, 0.0f, 0.0f, 0.1f};
};

// Theme style
struct UI_API ThemeStyle {
    float fontSize = 14.0f;
    float cornerRadius = 4.0f;
    float borderWidth = 1.0f;
};

struct UI_API UIGeometry {
    Rect LocalRect;      // Local space rectangle
    Rect AbsoluteRect;   // Screen space rectangle
    glm::mat4 Transform; // Local to screen space transform

    UIGeometry() : Transform(1.0f) {}

    UIGeometry(const Rect& local, const Rect& absolute, const glm::mat4& transform = glm::mat4(1.0f))
        : LocalRect(local)
        , AbsoluteRect(absolute)
        , Transform(transform)
    {}

    bool ContainsPoint(const glm::vec2& point) const {
        return AbsoluteRect.Contains(point);
    }
};

}} // namespace VGE::UI
