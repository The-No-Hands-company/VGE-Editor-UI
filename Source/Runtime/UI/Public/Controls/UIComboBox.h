#pragma once

#include "Runtime/Core/Public/Core/UICore.h"

#include "Runtime/Core/Public/UIWidget.h"
#include "ComboBox/Header Files/UIComboBoxBase.h"
#include "ComboBox/Header Files/UIComboBoxRenderer.h"
#include "ComboBox/Header Files/UIComboBoxAnimator.h"
#include "ComboBox/Header Files/UIComboBoxKeyboard.h"
#include <memory>

namespace VGE {

struct ComboBoxItem {
    std::string Text;
    std::string SearchText;  // Lowercase version for searching
    std::string Description; // Optional description/subtitle
    std::string Category;    // Optional category for grouping
    std::string IconName;    // Optional icon identifier
    glm::vec4 IconTint = glm::vec4(1.0f);
    uint64_t Id = 0;        // Optional unique identifier
    void* UserData = nullptr;
    bool IsEnabled = true;
};

struct ComboBoxProperties {
    bool EnableSearch = true;
    bool EnableFiltering = true;
    bool EnableCategories = true;
    bool EnableMultiSelect = false;
    bool ShowDescription = true;
    bool AutoClose = true;
    bool ClearSearchOnClose = true;
    bool EnableKeyboardNavigation = true;
    float MaxDropdownHeight = 300.0f;
    float ItemHeight = 24.0f;
    float CategoryHeaderHeight = 28.0f;
    float IconSize = 16.0f;
    float IconPadding = 4.0f;
    size_t MaxVisibleItems = 8;
    std::string Placeholder = "Select an item...";
    std::string SearchPlaceholder = "Type to search...";
    std::string NoResultsText = "No items found";
    ComboBoxVisualStyle VisualStyle;
};

class UIComboBox : public UIWidget, public UIComboBoxBase {
public:
    UIComboBox();
    virtual ~UIComboBox() = default;

    // UIWidget overrides
    virtual void Initialize() override;
    virtual void Update(float deltaTime) override;
    virtual void Render() override;
    virtual void OnMouseMove(const glm::vec2& position) override;
    virtual void OnMouseDown(const glm::vec2& position, MouseButton button) override;
    virtual void OnMouseUp(const glm::vec2& position, MouseButton button) override;
    virtual void OnKeyDown(int key, bool ctrl, bool shift, bool alt) override;
    virtual void OnKeyUp(int key, bool ctrl, bool shift, bool alt) override;
    virtual void OnFocusGained() override;
    virtual void OnFocusLost() override;

    // Item management
    void AddItem(const std::string& text, uint64_t id = 0, void* userData = nullptr);
    void AddItem(const ComboBoxItem& item);
    void RemoveItem(size_t index);
    void RemoveItem(const std::string& text);
    void RemoveItem(uint64_t id);
    void ClearItems();

    // Selection
    void SetSelectedIndex(size_t index);
    void SetSelectedId(uint64_t id);
    size_t GetSelectedIndex() const { return m_SelectedIndex; }
    uint64_t GetSelectedId() const;
    const ComboBoxItem* GetSelectedItem() const;
    
    // Multi-select support
    void SetSelected(size_t index, bool selected);
    void SetSelectedIds(const std::vector<uint64_t>& ids);
    std::vector<size_t> GetSelectedIndices() const;
    std::vector<uint64_t> GetSelectedIds() const;
    std::vector<const ComboBoxItem*> GetSelectedItems() const;

    // Search and filter
    void SetSearchText(const std::string& text);
    const std::string& GetSearchText() const { return m_SearchText; }
    void ClearSearch();

    // Properties and style
    void SetProperties(const ComboBoxProperties& properties);
    const ComboBoxProperties& GetProperties() const { return m_Properties; }
    void SetVisualStyle(const ComboBoxVisualStyle& style);
    const ComboBoxVisualStyle& GetVisualStyle() const { return m_Properties.VisualStyle; }

    // State
    bool IsDropdownOpen() const { return m_IsDropdownOpen; }
    void OpenDropdown();
    void CloseDropdown();
    void ToggleDropdown();

    // Component access
    UIComboBoxRenderer* GetRenderer() { return m_Renderer.get(); }
    UIComboBoxAnimator* GetAnimator() { return m_Animator.get(); }
    UIComboBoxKeyboard* GetKeyboard() { return m_Keyboard.get(); }

private:
    std::unique_ptr<UIComboBoxRenderer> m_Renderer;
    std::unique_ptr<UIComboBoxAnimator> m_Animator;
    std::unique_ptr<UIComboBoxKeyboard> m_Keyboard;

    // Internal event handlers
    void HandleDropdownStateChange(bool isOpen);
    void HandleSelectionChange(size_t index);
    void HandleSearchTextChange(const std::string& searchText);

    // Core data
    ComboBoxProperties m_Properties;
    std::vector<ComboBoxItem> m_Items;
    std::vector<bool> m_SelectedItems;
    size_t m_SelectedIndex = (size_t)-1;
    size_t m_HoveredItemIndex = (size_t)-1;
    
    // State
    bool m_IsDropdownOpen = false;
    std::string m_SearchText;
    
    // Callbacks
    SelectionCallback m_OnSelectionChanged;
    MultiSelectionCallback m_OnMultiSelectionChanged;
    SearchCallback m_OnSearchChanged;
    
    // Categories
    std::vector<std::string> m_Categories;
    std::unordered_map<std::string, std::vector<ComboBoxItem*>> m_ItemsByCategory;
};

} // namespace VGE



