#pragma once

#include "Runtime/Core/Public/Core/UICore.h"
#include "Runtime/Core/Public/Widget/UIWidget.h"
#include <string>
#include <vector>
#include <functional>

namespace VGE {
namespace UI {

/**
 * @brief Menu item types
 */
enum class MenuItemType {
    Command,    ///< Regular command item
    Checkbox,   ///< Toggleable checkbox item
    Radio,      ///< Radio button item
    Separator,  ///< Separator line
    SubMenu     ///< Sub-menu container
};

/**
 * @brief Menu item data
 */
struct MenuItem {
    std::string text;           ///< Display text
    std::string shortcut;       ///< Keyboard shortcut
    std::string icon;           ///< Icon path
    MenuItemType type;          ///< Item type
    bool isEnabled = true;      ///< Enabled state
    bool isChecked = false;     ///< Checked state
    std::function<void()> command; ///< Command callback
    std::vector<MenuItem> subItems; ///< Sub-menu items
};

/**
 * @brief Menu widget for displaying hierarchical commands
 */
class UIMenu : public UIWidget {
public:
    UIMenu();
    virtual ~UIMenu() = default;

    // Item management
    void AddItem(const MenuItem& item);
    void RemoveItem(const std::string& text);
    void ClearItems();
    MenuItem* FindItem(const std::string& text);
    const std::vector<MenuItem>& GetItems() const { return m_Items; }

    // Item state
    void EnableItem(const std::string& text, bool enable);
    void CheckItem(const std::string& text, bool check);
    bool IsItemEnabled(const std::string& text) const;
    bool IsItemChecked(const std::string& text) const;

    // Menu state
    void Show(const glm::vec2& position);
    void Hide();
    bool IsVisible() const { return m_IsVisible; }
    void SetAutoClose(bool autoClose) { m_AutoClose = autoClose; }
    bool GetAutoClose() const { return m_AutoClose; }

    // Widget interface
    void Render() override;
    void Update(float deltaTime) override;

protected:
    // Event handlers
    virtual void OnItemAdded(MenuItem* item);
    virtual void OnItemRemoved(MenuItem* item);
    virtual void OnItemClicked(MenuItem* item);
    virtual void OnMenuShown();
    virtual void OnMenuHidden();

private:
    // Helper functions
    void RenderItem(const MenuItem& item);
    void HandleItemClick(MenuItem& item);
    MenuItem* FindItemRecursive(const std::string& text, std::vector<MenuItem>& items);
    void CloseSubMenus();

private:
    std::vector<MenuItem> m_Items;
    bool m_IsVisible;
    bool m_AutoClose;
    MenuItem* m_HoveredItem;
    UIMenu* m_ActiveSubMenu;
    glm::vec2 m_Position;
};

}} // namespace VGE::UI



