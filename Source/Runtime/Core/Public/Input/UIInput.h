#pragma once

#include "Runtime/Core/Public/Core/UICore.h"
#include <glm/glm.hpp>
#include <functional>
#include <vector>
#include <unordered_map>

namespace VGE {
namespace UI {

// Input event types
enum class UIEventType {
    None = 0,
    KeyPress,
    KeyRelease,
    KeyRepeat,
    MouseMove,
    MouseButton,
    MouseScroll,
    CharInput
};

// Mouse buttons
enum class UIMouseButton {
    Left,
    Right,
    Middle,
    Button4,
    Button5
};

// Mouse button states
enum class UIMouseButtonState {
    Released,
    Pressed,
    Repeat
};

// Keyboard keys (matching GLFW for simplicity)
enum class UIKey {
    Unknown = -1,
    Space = 32,
    Apostrophe = 39,
    Comma = 44,
    Minus = 45,
    Period = 46,
    Slash = 47,
    Alpha0 = 48,
    Alpha1 = 49,
    Alpha2 = 50,
    Alpha3 = 51,
    Alpha4 = 52,
    Alpha5 = 53,
    Alpha6 = 54,
    Alpha7 = 55,
    Alpha8 = 56,
    Alpha9 = 57,
    Semicolon = 59,
    Equal = 61,
    A = 65,
    B = 66,
    C = 67,
    D = 68,
    E = 69,
    F = 70,
    G = 71,
    H = 72,
    I = 73,
    J = 74,
    K = 75,
    L = 76,
    M = 77,
    N = 78,
    O = 79,
    P = 80,
    Q = 81,
    R = 82,
    S = 83,
    T = 84,
    U = 85,
    V = 86,
    W = 87,
    X = 88,
    Y = 89,
    Z = 90,
    Escape = 256,
    Enter = 257,
    Tab = 258,
    Backspace = 259,
    Insert = 260,
    Delete = 261,
    Right = 262,
    Left = 263,
    Down = 264,
    Up = 265,
    PageUp = 266,
    PageDown = 267,
    Home = 268,
    End = 269,
    CapsLock = 280,
    ScrollLock = 281,
    NumLock = 282,
    PrintScreen = 283,
    Pause = 284,
    F1 = 290,
    F2 = 291,
    F3 = 292,
    F4 = 293,
    F5 = 294,
    F6 = 295,
    F7 = 296,
    F8 = 297,
    F9 = 298,
    F10 = 299,
    F11 = 300,
    F12 = 301
};

// Key states
enum class UIKeyState {
    Released,
    Pressed,
    Repeat
};

// Modifier flags
enum class UIModifier : uint32_t {
    None     = 0,
    Shift    = 1 << 0,
    Control  = 1 << 1,
    Alt      = 1 << 2,
    Super    = 1 << 3,
    CapsLock = 1 << 4,
    NumLock  = 1 << 5
};

// Base event struct
struct UI_API UIEvent {
    UIEventType Type;
    uint32_t Modifiers;

    UIEvent(UIEventType type) : Type(type), Modifiers(0) {}
    virtual ~UIEvent() = default;
};

// Mouse event struct
struct UI_API UIMouseEvent : public UIEvent {
    glm::vec2 Position;
    UIMouseButton Button;
    UIMouseButtonState ButtonState;

    UIMouseEvent(UIEventType type, const glm::vec2& position)
        : UIEvent(type), Position(position), Button(UIMouseButton::Left), ButtonState(UIMouseButtonState::Released)
    {};
};

// Key event struct
struct UI_API UIKeyEvent : public UIEvent {
    UIKey Key;
    UIKeyState State;
    int ScanCode;

    UIKeyEvent(UIEventType type, UIKey key, UIKeyState state, int scanCode)
        : UIEvent(type), Key(key), State(state), ScanCode(scanCode)
    {};
};

// Character input event struct
struct UI_API UICharEvent : public UIEvent {
    uint32_t CodePoint;

    UICharEvent(uint32_t codePoint)
        : UIEvent(UIEventType::CharInput), CodePoint(codePoint)
    {};
};

// Event listener type
using UIEventListener = std::function<bool(UIEvent&)>;

class UI_API UIInputProcessor {
public:
    UIInputProcessor();
    ~UIInputProcessor();

    void ProcessEvent(UIEvent& event);
    void Update();

    void AddEventListener(UIEventType type, UIEventListener listener);
    void RemoveEventListener(UIEventType type, UIEventListener listener);

    const glm::vec2& GetMousePosition() const { return m_MousePosition; }
    bool IsMouseButtonPressed(UIMouseButton button) const;
    bool IsKeyPressed(UIKey key) const;
    bool HasModifier(UIModifier modifier) const;

private:
    struct InputState {
        bool IsPressed = false;
        bool WasPressed = false;
    };

    glm::vec2 m_MousePosition;
    std::unordered_map<UIMouseButton, InputState> m_MouseButtonStates;
    std::unordered_map<UIKey, InputState> m_KeyStates;
    uint32_t m_Modifiers;

    std::unordered_map<UIEventType, std::vector<UIEventListener>> m_EventListeners;
};

}} // namespace VGE::UI


