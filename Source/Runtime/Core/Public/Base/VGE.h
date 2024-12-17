#pragma once

#include <string>
#include <memory>
#include <functional>
#include <glm/glm.hpp>

namespace Versa {

// Forward declarations
class UIElement;
class UIWidget;
class UIEvent;

// Common types
using UIElementPtr = std::shared_ptr<UIElement>;
using UIWidgetPtr = std::shared_ptr<UIWidget>;
using EventCallback = std::function<void(const UIEvent&)>;

// Common constants
constexpr float UI_DEFAULT_PADDING = 5.0f;
constexpr float UI_DEFAULT_MARGIN = 5.0f;
constexpr float UI_DEFAULT_SPACING = 5.0f;

// Common utility functions
inline glm::vec2 GetScreenPosition(const glm::vec2& position, const glm::vec2& parentPosition) {
    return position + parentPosition;
}

} // namespace Versa



