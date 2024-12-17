#pragma once

#include "Runtime/Core/Public/Core/UICore.h"

#include "Runtime/Core/Public/Widget/UIWidget.h"
#include <functional>

namespace VGE {
namespace Editor {

class UITextInput : public UIWidget {
public:
    UITextInput(const std::string& placeholder = "");
    virtual ~UITextInput() = default;

    virtual void Update() override;
    virtual void Draw() override;
    virtual bool HandleEvent(Event& event) override;

    // Getters
    const std::string& GetText() const { return m_Text; }
    const std::string& GetPlaceholder() const { return m_Placeholder; }
    bool IsPassword() const { return m_IsPassword; }
    bool IsMultiline() const { return m_IsMultiline; }
    bool IsReadOnly() const { return m_IsReadOnly; }
    size_t GetMaxLength() const { return m_MaxLength; }
    size_t GetCursorPosition() const { return m_CursorPosition; }
    size_t GetSelectionStart() const { return m_SelectionStart; }
    size_t GetSelectionEnd() const { return m_SelectionEnd; }

    // Setters
    void SetText(const std::string& text);
    void SetPlaceholder(const std::string& placeholder) { m_Placeholder = placeholder; }
    void SetPassword(bool isPassword) { m_IsPassword = isPassword; }
    void SetMultiline(bool isMultiline) { m_IsMultiline = isMultiline; }
    void SetReadOnly(bool isReadOnly) { m_IsReadOnly = isReadOnly; }
    void SetMaxLength(size_t maxLength) { m_MaxLength = maxLength; }
    void SetCursorPosition(size_t position);
    void SetSelection(size_t start, size_t end);

    // Event callbacks
    void SetTextChangedCallback(std::function<void(const std::string&)> callback) {
        m_TextChangedCallback = callback;
    }
    void SetSubmitCallback(std::function<void(const std::string&)> callback) {
        m_SubmitCallback = callback;
    }

private:
    void HandleTextInput(const std::string& text);
    void HandleKeyPress(KeyCode key, bool shift, bool ctrl);
    void UpdateCursorBlink();
    void EnsureCursorVisible();
    std::string GetVisibleText() const;
    glm::vec2 GetTextDimensions(const std::string& text) const;

    std::string m_Text;
    std::string m_Placeholder;
    bool m_IsPassword = false;
    bool m_IsMultiline = false;
    bool m_IsReadOnly = false;
    size_t m_MaxLength = std::string::npos;
    size_t m_CursorPosition = 0;
    size_t m_SelectionStart = 0;
    size_t m_SelectionEnd = 0;
    float m_ScrollOffset = 0.0f;

    // Visual state
    bool m_CursorVisible = true;
    float m_CursorBlinkTime = 0.0f;
    float m_HoverAnimation = 0.0f;

    // Callbacks
    std::function<void(const std::string&)> m_TextChangedCallback;
    std::function<void(const std::string&)> m_SubmitCallback;
};

} // namespace Editor
} // namespace VGE



