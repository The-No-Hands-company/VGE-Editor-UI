#include "U

#include "Runtime/Core/Public/Core/UICore.h"IComboBoxKeyboard.h"
#include <Windows.h>

namespace VGE {

UIComboBoxKeyboardHandler::UIComboBoxKeyboardHandler() {
    InitializeDefaultBindings();
}

void UIComboBoxKeyboardHandler::InitializeDefaultBindings() {
    m_KeyBindings = {
        // Navigation
        { VK_DOWN, false, false, false, ComboBoxKeyAction::SelectNext },
        { VK_UP, false, false, false, ComboBoxKeyAction::SelectPrevious },
        { VK_HOME, false, false, false, ComboBoxKeyAction::SelectFirst },
        { VK_END, false, false, false, ComboBoxKeyAction::SelectLast },
        { VK_PRIOR, false, false, false, ComboBoxKeyAction::SelectPreviousPage },
        { VK_NEXT, false, false, false, ComboBoxKeyAction::SelectNextPage },
        
        // Dropdown control
        { VK_RETURN, false, false, false, ComboBoxKeyAction::ToggleSelected },
        { VK_SPACE, false, false, false, ComboBoxKeyAction::ToggleSelected },
        { VK_ESCAPE, false, false, false, ComboBoxKeyAction::CloseDropdown },
        { VK_F4, false, false, false, ComboBoxKeyAction::OpenDropdown },
        
        // Multi-select operations
        { 'A', true, false, false, ComboBoxKeyAction::SelectAll },
        { VK_ESCAPE, true, false, false, ComboBoxKeyAction::DeselectAll },
        { VK_DELETE, false, false, false, ComboBoxKeyAction::DeleteSelected },
        
        // Search
        { 'F', true, false, false, ComboBoxKeyAction::FocusSearch },
        { VK_ESCAPE, false, false, true, ComboBoxKeyAction::ClearSearch }
    };
}

void UIComboBoxKeyboardHandler::HandleKeyDown(int keyCode, bool ctrl, bool shift, bool alt) {
    if (!m_ActionCallback) return;
    
    for (const auto& binding : m_KeyBindings) {
        if (MatchesBinding(binding, keyCode, ctrl, shift, alt)) {
            m_ActionCallback(binding.Action);
            return;
        }
    }
}

bool UIComboBoxKeyboardHandler::MatchesBinding(const ComboBoxKeyBinding& binding, int keyCode, bool ctrl, bool shift, bool alt) {
    return binding.KeyCode == keyCode &&
           binding.Ctrl == ctrl &&
           binding.Shift == shift &&
           binding.Alt == alt;
}

void UIComboBoxKeyboardHandler::AddKeyBinding(const ComboBoxKeyBinding& binding) {
    // Remove any existing binding with the same key combination
    RemoveKeyBinding(binding.KeyCode, binding.Ctrl, binding.Shift, binding.Alt);
    m_KeyBindings.push_back(binding);
}

void UIComboBoxKeyboardHandler::RemoveKeyBinding(int keyCode, bool ctrl, bool shift, bool alt) {
    m_KeyBindings.erase(
        std::remove_if(m_KeyBindings.begin(), m_KeyBindings.end(),
            [&](const ComboBoxKeyBinding& binding) {
                return MatchesBinding(binding, keyCode, ctrl, shift, alt);
            }),
        m_KeyBindings.end()
    );
}

void UIComboBoxKeyboardHandler::ResetToDefaultBindings() {
    InitializeDefaultBindings();
}

} // namespace VGE



