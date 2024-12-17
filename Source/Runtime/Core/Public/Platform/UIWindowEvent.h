#pragma once

#include "Core/UICore.h"
#include <glm/glm.hpp>
#include <string>
#include <functional>

namespace VGE {
namespace UI {

enum class WindowEventType {
    None,
    Created,
    Destroyed,
    Moved,
    Resized,
    Minimized,
    Maximized,
    Restored,
    FocusGained,
    FocusLost,
    Close,
    MouseEnter,
    MouseLeave,
    MouseMove,
    MouseButton,
    MouseScroll,
    KeyPress,
    KeyRelease,
    Char
};

struct WindowEvent {
    WindowEventType type = WindowEventType::None;
    std::string windowTitle;
    glm::vec2 position = glm::vec2(0.0f);
    glm::vec2 size = glm::vec2(0.0f);
    glm::vec2 mousePosition = glm::vec2(0.0f);
    int mouseButton = 0;
    int keyCode = 0;
    int scanCode = 0;
    int modifiers = 0;
    unsigned int charCode = 0;
    float scrollX = 0.0f;
    float scrollY = 0.0f;
    bool handled = false;
};

using WindowEventCallback = std::function<void(const WindowEvent&)>;

}} // namespace VGE::UI 