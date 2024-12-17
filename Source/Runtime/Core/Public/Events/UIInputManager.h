#pragma once

#include "Runtime/Core/Public/Core/UICore.h"

#include <functional>
#include <unordered_map>
#include <vector>
#include <memory>
#include "Base/Widget.h"

namespace VGE::Editor::UI {

struct MouseState {
    float x, y;
    float prevX, prevY;
    float scrollX, scrollY;
    bool buttons[5];  // Left, Right, Middle, X1, X2
};

struct KeyState {
    bool isPressed;
    bool wasPressed;
    float pressTime;
};

class InputManager {
public:
    void Initialize();
    void Shutdown();
    void Update(float deltaTime);
    
    // Mouse input
    void OnMouseMove(float x, float y);
    void OnMouseButton(int button, bool pressed);
    void OnMouseScroll(float xOffset, float yOffset);
    
    // Keyboard input
    void OnKeyPressed(int key, bool pressed);
    void OnChar(unsigned int codepoint);
    
    // Input state
    const MouseState& GetMouseState() const { return m_MouseState; }
    bool IsKeyPressed(int key) const;
    bool WasKeyPressed(int key) const;
    float GetKeyPressTime(int key) const;
    
    // Widget focus
    void SetFocusedWidget(Widget* widget);
    Widget* GetFocusedWidget() const { return m_FocusedWidget; }
    
    // Input routing
    void RouteMouseInput(Widget* root);
    void RouteKeyboardInput(Widget* root);
    
    // Hotkeys and shortcuts
    using ShortcutCallback = std::function<void()>;
    void RegisterShortcut(const std::string& name, const std::vector<int>& keys, ShortcutCallback callback);
    void UnregisterShortcut(const std::string& name);
    
    // Input recording/playback (for macros)
    void StartRecording();
    void StopRecording();
    void PlayRecordedInput();

private:
    MouseState m_MouseState;
    std::unordered_map<int, KeyState> m_KeyStates;
    Widget* m_FocusedWidget;
    Widget* m_HoveredWidget;
    
    // Shortcuts
    struct Shortcut {
        std::vector<int> keys;
        ShortcutCallback callback;
        bool active;
    };
    std::unordered_map<std::string, Shortcut> m_Shortcuts;
    
    // Input recording
    struct InputEvent {
        enum Type { Mouse, Keyboard } type;
        float timestamp;
        union {
            struct { float x, y; int button; bool pressed; } mouse;
            struct { int key; bool pressed; } keyboard;
        } data;
    };
    std::vector<InputEvent> m_RecordedInput;
    bool m_IsRecording;
    
    // Helper methods
    void UpdateShortcuts();
    void ProcessInputEvent(const InputEvent& event);
    bool IsShortcutActive(const Shortcut& shortcut) const;
};

} // namespace VGE::Editor::UI



