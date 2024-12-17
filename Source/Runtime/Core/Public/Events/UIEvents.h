#pragma once

#include "Runtime/Core/Public/Core/UICore.h"

#include <glm/glm.hpp>
#include <functional>
#include <vector>

namespace VGE {
namespace UI {

// Forward declaration
class UIElement;

enum class EventType {
    None,
    MouseEnter,
    MouseLeave,
    MouseMove,
    MouseDown,
    MouseUp,
    Click
};

struct UIEvent {
    EventType type = EventType::None;
    UIElement* target = nullptr;
    bool handled = false;
    
    UIEvent(EventType t) : type(t) {}
};

struct MouseEvent : UIEvent {
    glm::vec2 position;
    glm::vec2 delta;
    int button = 0;
    
    MouseEvent(EventType t, const glm::vec2& pos) 
        : UIEvent(t), position(pos) {}
};

// Simple event handler system
template<typename T>
class Signal {
public:
    using Handler = std::function<void(const T&)>;
    
    void Connect(Handler handler) {
        handlers.push_back(handler);
    }
    
    void Emit(const T& event) {
        for (auto& handler : handlers) {
            handler(event);
        }
    }
    
private:
    std::vector<Handler> handlers;
};

}} // namespace VGE::UI



