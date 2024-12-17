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
    size_t GetSelectedIndex() const { return m_SelectedIndex; }
    const ComboBoxItem* GetSelectedItem() const;
    
    // Multi-select support
    void SetSelected(size_t index, bool selected);
    std::vector<size_t> GetSelectedIndices() const;
    std::vector<const ComboBoxItem*> GetSelectedItems() const;

    // Search
    void SetSearchText(const std::string& text);
    const std::string& GetSearchText() const { return m_SearchText; }
    void ClearSearch();

    // State
    bool IsDropdownOpen() const { return m_IsDropdownOpen; }
    void OpenDropdown();
    void CloseDropdown();
    void ToggleDropdown();

    // Callbacks
    using SelectionCallback = std::function<void(const ComboBoxItem&)>;
    using MultiSelectionCallback = std::function<void(const std::vector<ComboBoxItem>&)>;
    using SearchCallback = std::function<void(const std::string&)>;

    void SetOnSelectionChanged(const SelectionCallback& callback) { m_OnSelectionChanged = callback; }
    void SetOnMultiSelectionChanged(const MultiSelectionCallback& callback) { m_OnMultiSelectionChanged = callback; }
    void SetOnSearchChanged(const SearchCallback& callback) { m_OnSearchChanged = callback; }

protected:
    virtual void OnMouseDown(const glm::vec2& position) override;
    virtual void OnMouseUp(const glm::vec2& position) override;
    virtual void OnMouseMove(const glm::vec2& position) override;
    virtual void OnKeyDown(int keyCode) override;
    virtual void OnFocusLost() override;

    // Navigation methods
    void SelectNextItem();
    void SelectPreviousItem();
    void SelectFirstItem();
    void SelectLastItem();
    void SelectNextPage();
    void SelectPreviousPage();
    size_t GetItemsPerPage() const;

private:
    void UpdateLayout();
    void UpdateFiltering();
    bool IsItemVisible(const ComboBoxItem& item) const;
    size_t GetItemIndex(const ComboBoxItem* item) const;

    // Helper functions
    std::string ToLowerCase(const std::string& text);
    std::vector<std::string> SplitString(const std::string& text, char delimiter);

    // Items and Selection
    std::vector<ComboBoxItem> m_Items;
    std::vector<bool> m_SelectedItems;
    std::vector<ComboBoxItem> m_FilteredItems;
    std::vector<size_t> m_FilteredIndices;
    size_t m_SelectedIndex = (size_t)-1;
    size_t m_HoveredItemIndex = (size_t)-1;

    // UI Components
    Ref<UITextInput> m_SearchInput;
    Ref<UIScrollView> m_ScrollView;
    Ref<UIComboBoxRenderer> m_Renderer;
    Ref<UIComboBoxAnimator> m_Animator;
    Ref<UIComboBoxKeyboardHandler> m_KeyboardHandler;

    // State
    bool m_IsDropdownOpen = false;
    std::string m_SearchText;

    // Callbacks
    SelectionCallback m_OnSelectionChanged;
    MultiSelectionCallback m_OnMultiSelectionChanged;
    SearchCallback m_OnSearchChanged;
};

}} // namespace VGE::UI



