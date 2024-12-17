#include "Runtime/UI/Public/Controls/UITextInput.h"
#include "Runtime/Core/Public/Core/Application.h"
#include "Runtime/Core/Public/Events/Event.h"
#include "Runtime/Core/Public/Events/KeyEvent.h"
#include "Runtime/Core/Public/Events/MouseEvent.h"
#include "Runtime/Core/Public/Renderer/UIRenderer.h"
#include "Runtime/Core/Public/Theme/UITheme.h"
#include "Runtime/Core/Public/Widget/UIWidget.h"
#include "Runtime/Core/Public/Base/VGE.h"
#include <glm/gtc/constants.hpp>
#include <algorithm>

namespace VGE {
namespace Editor {

using namespace VGE::UI;

UITextInput::UITextInput(const std::string& placeholder)
    : UIWidget("")
    , m_Placeholder(placeholder) {
    m_Size = { 200.0f, 30.0f }; // Default size
}

void UITextInput::Update() {
    auto& input = Application::Get().GetInput();
    glm::vec2 mousePos = input.GetMousePosition();

    // Update hover state
    bool isHovered = IsPointInside(mousePos);
    float targetHover = isHovered ? 1.0f : 0.0f;
    m_HoverAnimation = glm::mix(m_HoverAnimation, targetHover, 0.1f);

    // Update cursor blink
    if (m_State == UIWidgetState::Focused) {
        UpdateCursorBlink();
    }
}

void UITextInput::Draw() {
    if (!m_Visible || !m_Theme)
        return;

    auto& colors = m_Theme->GetColors();
    auto& style = m_Theme->GetStyle();

    // Calculate colors based on state
    glm::vec4 bgColor = colors.InputBackground;
    glm::vec4 borderColor = colors.InputBorder;
    glm::vec4 textColor = colors.Text;

    if (m_State == UIWidgetState::Focused) {
        borderColor = colors.Accent;
    }
    else if (m_State == UIWidgetState::Disabled) {
        bgColor.a *= 0.5f;
        borderColor.a *= 0.5f;
        textColor.a *= 0.5f;
    }
    else {
        borderColor = glm::mix(borderColor, colors.InputBorderHovered, m_HoverAnimation);
    }

    // Draw background and border
    RenderCommand::SetColor(bgColor);
    RenderCommand::DrawRoundedRect(
        m_Position,
        m_Size,
        style.InputRounding
    );

    RenderCommand::SetColor(borderColor);
    RenderCommand::DrawRoundedRectOutline(
        m_Position,
        m_Size,
        style.InputRounding,
        1.0f
    );

    // Calculate text area
    float textPadding = style.InputPadding;
    glm::vec2 textPos = m_Position + glm::vec2(textPadding);
    glm::vec2 textSize = m_Size - glm::vec2(textPadding * 2.0f);

    // Draw text or placeholder
    std::string displayText = GetVisibleText();
    if (displayText.empty() && !m_Placeholder.empty() && m_State != UIWidgetState::Focused) {
        RenderCommand::SetColor(colors.TextDisabled);
        RenderCommand::DrawText(
            m_Placeholder,
            textPos,
            textSize,
            UIAlignment::Left
        );
    }
    else {
        RenderCommand::SetColor(textColor);
        RenderCommand::DrawText(
            displayText,
            textPos - glm::vec2(m_ScrollOffset, 0.0f),
            textSize,
            UIAlignment::Left
        );

        // Draw selection if any
        if (m_SelectionStart != m_SelectionEnd && m_State == UIWidgetState::Focused) {
            size_t start = std::min(m_SelectionStart, m_SelectionEnd);
            size_t end = std::max(m_SelectionStart, m_SelectionEnd);
            std::string preSelection = m_Text.substr(0, start);
            std::string selection = m_Text.substr(start, end - start);
            
            glm::vec2 selStartPos = textPos + glm::vec2(GetTextDimensions(preSelection).x - m_ScrollOffset, 0.0f);
            glm::vec2 selSize = glm::vec2(GetTextDimensions(selection).x, textSize.y);

            RenderCommand::SetColor(glm::vec4(colors.Accent.r, colors.Accent.g, colors.Accent.b, 0.3f));
            RenderCommand::DrawRect(selStartPos, selSize);
        }

        // Draw cursor
        if (m_State == UIWidgetState::Focused && m_CursorVisible) {
            std::string textBeforeCursor = m_Text.substr(0, m_CursorPosition);
            float cursorX = textPos.x + GetTextDimensions(textBeforeCursor).x - m_ScrollOffset;
            
            RenderCommand::SetColor(colors.Text);
            RenderCommand::DrawRect(
                glm::vec2(cursorX, textPos.y),
                glm::vec2(1.0f, textSize.y)
            );
        }
    }
}

bool UITextInput::HandleEvent(Event& event) {
    if (!m_Enabled || m_IsReadOnly)
        return false;

    EventDispatcher dispatcher(event);
    
    dispatcher.Dispatch<MouseButtonPressedEvent>([this](MouseButtonPressedEvent& e) {
        if (e.GetMouseButton() == MouseButton::Left) {
            bool wasInside = IsPointInside(e.GetPosition());
            if (wasInside && m_State != UIWidgetState::Focused) {
                m_State = UIWidgetState::Focused;
                m_CursorPosition = m_Text.length(); // Move cursor to end
                m_SelectionStart = m_SelectionEnd = m_CursorPosition;
                return true;
            }
            else if (!wasInside && m_State == UIWidgetState::Focused) {
                m_State = UIWidgetState::Normal;
                return true;
            }
        }
        return false;
    });

    dispatcher.Dispatch<KeyPressedEvent>([this](KeyPressedEvent& e) {
        if (m_State == UIWidgetState::Focused) {
            HandleKeyPress(e.GetKeyCode(), e.IsShiftPressed(), e.IsControlPressed());
            return true;
        }
        return false;
    });

    dispatcher.Dispatch<TextInputEvent>([this](TextInputEvent& e) {
        if (m_State == UIWidgetState::Focused) {
            HandleTextInput(e.GetText());
            return true;
        }
        return false;
    });

    return false;
}

void UITextInput::SetText(const std::string& text) {
    if (m_Text != text) {
        m_Text = text;
        m_CursorPosition = m_Text.length();
        m_SelectionStart = m_SelectionEnd = m_CursorPosition;
        if (m_TextChangedCallback) {
            m_TextChangedCallback(m_Text);
        }
    }
}

void UITextInput::SetCursorPosition(size_t position) {
    m_CursorPosition = std::min(position, m_Text.length());
    m_SelectionStart = m_SelectionEnd = m_CursorPosition;
    EnsureCursorVisible();
}

void UITextInput::SetSelection(size_t start, size_t end) {
    m_SelectionStart = std::min(start, m_Text.length());
    m_SelectionEnd = std::min(end, m_Text.length());
    m_CursorPosition = m_SelectionEnd;
    EnsureCursorVisible();
}

void UITextInput::HandleTextInput(const std::string& text) {
    if (m_MaxLength != std::string::npos && m_Text.length() + text.length() > m_MaxLength)
        return;

    // Delete selected text if any
    if (m_SelectionStart != m_SelectionEnd) {
        size_t start = std::min(m_SelectionStart, m_SelectionEnd);
        size_t end = std::max(m_SelectionStart, m_SelectionEnd);
        m_Text.erase(start, end - start);
        m_CursorPosition = start;
    }

    // Insert new text
    m_Text.insert(m_CursorPosition, text);
    m_CursorPosition += text.length();
    m_SelectionStart = m_SelectionEnd = m_CursorPosition;

    if (m_TextChangedCallback) {
        m_TextChangedCallback(m_Text);
    }

    EnsureCursorVisible();
}

void UITextInput::HandleKeyPress(KeyCode key, bool shift, bool ctrl) {
    switch (key) {
        case KeyCode::Left: {
            if (ctrl) {
                // Move to previous word
                size_t pos = m_CursorPosition;
                while (pos > 0 && std::isspace(m_Text[pos - 1])) pos--;
                while (pos > 0 && !std::isspace(m_Text[pos - 1])) pos--;
                m_CursorPosition = pos;
            }
            else if (m_CursorPosition > 0) {
                m_CursorPosition--;
            }
            if (!shift) {
                m_SelectionStart = m_SelectionEnd = m_CursorPosition;
            }
            else {
                m_SelectionEnd = m_CursorPosition;
            }
            break;
        }
        case KeyCode::Right: {
            if (ctrl) {
                // Move to next word
                size_t pos = m_CursorPosition;
                while (pos < m_Text.length() && !std::isspace(m_Text[pos])) pos++;
                while (pos < m_Text.length() && std::isspace(m_Text[pos])) pos++;
                m_CursorPosition = pos;
            }
            else if (m_CursorPosition < m_Text.length()) {
                m_CursorPosition++;
            }
            if (!shift) {
                m_SelectionStart = m_SelectionEnd = m_CursorPosition;
            }
            else {
                m_SelectionEnd = m_CursorPosition;
            }
            break;
        }
        case KeyCode::Home: {
            m_CursorPosition = 0;
            if (!shift) {
                m_SelectionStart = m_SelectionEnd = m_CursorPosition;
            }
            else {
                m_SelectionEnd = m_CursorPosition;
            }
            break;
        }
        case KeyCode::End: {
            m_CursorPosition = m_Text.length();
            if (!shift) {
                m_SelectionStart = m_SelectionEnd = m_CursorPosition;
            }
            else {
                m_SelectionEnd = m_CursorPosition;
            }
            break;
        }
        case KeyCode::Backspace: {
            if (m_SelectionStart != m_SelectionEnd) {
                size_t start = std::min(m_SelectionStart, m_SelectionEnd);
                size_t end = std::max(m_SelectionStart, m_SelectionEnd);
                m_Text.erase(start, end - start);
                m_CursorPosition = start;
                m_SelectionStart = m_SelectionEnd = m_CursorPosition;
                if (m_TextChangedCallback) {
                    m_TextChangedCallback(m_Text);
                }
            }
            else if (m_CursorPosition > 0) {
                m_Text.erase(m_CursorPosition - 1, 1);
                m_CursorPosition--;
                m_SelectionStart = m_SelectionEnd = m_CursorPosition;
                if (m_TextChangedCallback) {
                    m_TextChangedCallback(m_Text);
                }
            }
            break;
        }
        case KeyCode::Delete: {
            if (m_SelectionStart != m_SelectionEnd) {
                size_t start = std::min(m_SelectionStart, m_SelectionEnd);
                size_t end = std::max(m_SelectionStart, m_SelectionEnd);
                m_Text.erase(start, end - start);
                m_CursorPosition = start;
                m_SelectionStart = m_SelectionEnd = m_CursorPosition;
                if (m_TextChangedCallback) {
                    m_TextChangedCallback(m_Text);
                }
            }
            else if (m_CursorPosition < m_Text.length()) {
                m_Text.erase(m_CursorPosition, 1);
                if (m_TextChangedCallback) {
                    m_TextChangedCallback(m_Text);
                }
            }
            break;
        }
        case KeyCode::Enter: {
            if (m_SubmitCallback) {
                m_SubmitCallback(m_Text);
            }
            break;
        }
        case KeyCode::A: {
            if (ctrl) {
                m_SelectionStart = 0;
                m_SelectionEnd = m_Text.length();
                m_CursorPosition = m_SelectionEnd;
            }
            break;
        }
        case KeyCode::C: {
            if (ctrl && m_SelectionStart != m_SelectionEnd) {
                size_t start = std::min(m_SelectionStart, m_SelectionEnd);
                size_t end = std::max(m_SelectionStart, m_SelectionEnd);
                std::string selectedText = m_Text.substr(start, end - start);
                Application::Get().SetClipboardText(selectedText);
            }
            break;
        }
        case KeyCode::X: {
            if (ctrl && m_SelectionStart != m_SelectionEnd) {
                size_t start = std::min(m_SelectionStart, m_SelectionEnd);
                size_t end = std::max(m_SelectionStart, m_SelectionEnd);
                std::string selectedText = m_Text.substr(start, end - start);
                Application::Get().SetClipboardText(selectedText);
                m_Text.erase(start, end - start);
                m_CursorPosition = start;
                m_SelectionStart = m_SelectionEnd = m_CursorPosition;
                if (m_TextChangedCallback) {
                    m_TextChangedCallback(m_Text);
                }
            }
            break;
        }
        case KeyCode::V: {
            if (ctrl) {
                std::string clipboardText = Application::Get().GetClipboardText();
                HandleTextInput(clipboardText);
            }
            break;
        }
    }

    EnsureCursorVisible();
}

void UITextInput::UpdateCursorBlink() {
    static const float BLINK_RATE = 0.53f; // Seconds
    m_CursorBlinkTime += Application::Get().GetDeltaTime();
    if (m_CursorBlinkTime >= BLINK_RATE) {
        m_CursorBlinkTime = 0.0f;
        m_CursorVisible = !m_CursorVisible;
    }
}

void UITextInput::EnsureCursorVisible() {
    if (!m_Theme)
        return;

    std::string textBeforeCursor = m_Text.substr(0, m_CursorPosition);
    float cursorX = GetTextDimensions(textBeforeCursor).x;
    float visibleWidth = m_Size.x - (m_Theme->GetStyle().InputPadding * 2.0f);

    // Adjust scroll offset if cursor is out of view
    if (cursorX - m_ScrollOffset > visibleWidth) {
        m_ScrollOffset = cursorX - visibleWidth + 10.0f; // Add small padding
    }
    else if (cursorX - m_ScrollOffset < 0) {
        m_ScrollOffset = cursorX;
    }
}

std::string UITextInput::GetVisibleText() const {
    if (m_IsPassword && !m_Text.empty()) {
        return std::string(m_Text.length(), '*');
    }
    return m_Text;
}

glm::vec2 UITextInput::GetTextDimensions(const std::string& text) const {
    if (!m_Theme)
        return glm::vec2(0.0f);

    return RenderCommand::GetTextDimensions(text, m_Theme->GetStyle().FontSize);
}

} // namespace Editor
} // namespace VGE



