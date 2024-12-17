#include "Runtime/Core/Public/Input/UIInput.h"

namespace VGE {
namespace UI {

UIInputProcessor::UIInputProcessor()
    : m_MousePosition(0.0f)
    , m_Modifiers(0)
{
}

UIInputProcessor::~UIInputProcessor() = default;

void UIInputProcessor::ProcessEvent(UIEvent& event)
{
    // Update internal state based on event type
    switch (event.Type)
    {
        case UIEventType::MouseMove:
        {
            auto& mouseEvent = static_cast<UIMouseEvent&>(event);
            m_MousePosition = mouseEvent.Position;
            break;
        }
        case UIEventType::MouseButton:
        {
            auto& mouseEvent = static_cast<UIMouseEvent&>(event);
            auto& state = m_MouseButtonStates[mouseEvent.Button];
            state.WasPressed = state.IsPressed;
            state.IsPressed = mouseEvent.ButtonState == UIMouseButtonState::Pressed;
            break;
        }
        case UIEventType::KeyPress:
        case UIEventType::KeyRelease:
        {
            auto& keyEvent = static_cast<UIKeyEvent&>(event);
            auto& state = m_KeyStates[keyEvent.Key];
            state.WasPressed = state.IsPressed;
            state.IsPressed = keyEvent.State == UIKeyState::Pressed;
            m_Modifiers = event.Modifiers;
            break;
        }
    }

    // Notify event listeners
    auto it = m_EventListeners.find(event.Type);
    if (it != m_EventListeners.end())
    {
        for (const auto& listener : it->second)
        {
            if (listener(event))
                break;
        }
    }
}

void UIInputProcessor::Update()
{
    // Update was-pressed states
    for (auto& [button, state] : m_MouseButtonStates)
    {
        state.WasPressed = state.IsPressed;
    }

    for (auto& [key, state] : m_KeyStates)
    {
        state.WasPressed = state.IsPressed;
    }
}

void UIInputProcessor::AddEventListener(UIEventType type, UIEventListener listener)
{
    if (listener)
    {
        m_EventListeners[type].push_back(std::move(listener));
    }
}

void UIInputProcessor::RemoveEventListener(UIEventType type, UIEventListener listener)
{
    auto it = m_EventListeners.find(type);
    if (it != m_EventListeners.end())
    {
        auto& listeners = it->second;
        listeners.erase(
            std::remove_if(listeners.begin(), listeners.end(),
                [&](const UIEventListener& l) {
                    return l.target_type() == listener.target_type();
                }),
            listeners.end()
        );
    }
}

bool UIInputProcessor::IsMouseButtonPressed(UIMouseButton button) const
{
    auto it = m_MouseButtonStates.find(button);
    return it != m_MouseButtonStates.end() && it->second.IsPressed;
}

bool UIInputProcessor::IsKeyPressed(UIKey key) const
{
    auto it = m_KeyStates.find(key);
    return it != m_KeyStates.end() && it->second.IsPressed;
}

bool UIInputProcessor::HasModifier(UIModifier modifier) const
{
    return (m_Modifiers & static_cast<uint32_t>(modifier)) != 0;
}

}} // namespace VGE::UI 


