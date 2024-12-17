#pragma once

#include "Runtime/Core/Public/Core/UICore.h"
#include <functional>

namespace VGE {

enum class ComboBoxKeyAction {
    None,
    OpenDropdown,
    CloseDropdown,
    SelectNext,
    SelectPrevious,
    SelectFirst,
    SelectLast,
    SelectNextPage,
    SelectPreviousPage,
    ToggleSelected,
    SelectAll,
    DeselectAll,
    DeleteSelected,
    FocusSearch,
    ClearSearch
};

struct ComboBoxKeyBinding {
    int KeyCode;
    bool Ctrl;
    bool Shift;
    bool Alt;
    ComboBoxKeyAction Action;
};

class UIComboBoxKeyboardHandler {
public:
    UIComboBoxKeyboardHandler();
    
    void HandleKeyDown(int keyCode, bool ctrl, bool shift, bool alt);
    void SetActionCallback(const std::function<void(ComboBoxKeyAction)>& callback) { m_ActionCallback = callback; }
    
    // Add/Remove custom key bindings
    void AddKeyBinding(const ComboBoxKeyBinding& binding);
    void RemoveKeyBinding(int keyCode, bool ctrl = false, bool shift = false, bool alt = false);
    void ResetToDefaultBindings();

private:
    std::vector<ComboBoxKeyBinding> m_KeyBindings;
    std::function<void(ComboBoxKeyAction)> m_ActionCallback;
    
    void InitializeDefaultBindings();
    bool MatchesBinding(const ComboBoxKeyBinding& binding, int keyCode, bool ctrl, bool shift, bool alt);
};

} // namespace VGE



