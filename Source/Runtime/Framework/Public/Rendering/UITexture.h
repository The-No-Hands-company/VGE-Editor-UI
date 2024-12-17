#pragma once

#include "Runtime/Core/Public/Core/UICore.h"
#include "Runtime/Core/Public/Widget/UIWidget.h"
#include "Runtime/Core/Public/Input/UITextInput.h"
#include "Runtime/Core/Public/Widget/UIScrollView.h"
#include <functional>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>

namespace VGE {
namespace UI {

// Forward declarations
class UIComboBoxRenderer;
class UIComboBoxAnimator;
class UIComboBoxKeyboardHandler;

struct ComboBoxItem {
    std::string Text;
    std::string SearchText;
    std::string Description;
    std::string Category;
    uint64_t Id = 0;
    void* UserData = nullptr;
    bool IsEnabled = true;
};

class UI_API UIComboBoxBase : public UIWidget {
public:
    UIComboBoxBase();
    virtual ~UIComboBoxBase() = default;

    virtual void Initialize() override;
    virtual void Update(float deltaTime) override;
    virtual void Render() override;

    // Item management
    void AddItem(const std::string& text, uint64_t id = 0, void* userData = nullptr);
    void AddItem(const ComboBoxItem& item);
    void RemoveItem(size_t index);
    void ClearItems();

    // Selection
    void SetSelectedIndex(size_t index);
    size_