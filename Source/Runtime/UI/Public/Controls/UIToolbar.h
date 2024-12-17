#pragma once

#include "Runtime/Core/Public/Core/UICore.h"
#include "Runtime/Core/Public/Base/VGE.h"
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>

namespace VGE {
namespace UI {

struct UIToolbarItem {
    std::string name;
    std::string icon;
    std::string tooltip;
    bool isEnabled = true;
    bool isToggled = false;
    bool isToggleable = false;
    glm::vec2 size = {24, 24};
    std::function<void()> callback;
};

/**
 * @class UIToolbar
 * @brief Manages toolbars in the UI
 *
 * Handles:
 * - Toolbar creation and management
 * - Toolbar item management
 * - Icons and tooltips
 * - Item callbacks
 */
class UIToolbar {
public:
    UIToolbar();
    ~UIToolbar();

    // Toolbar management
    void AddToolbar(const std::string& name);
    void RemoveToolbar(const std::string& name);
    bool HasToolbar(const std::string& name) const;

    // Item management
    void AddItem(const std::string& toolbar, const UIToolbarItem& item);
    void RemoveItem(const std::string& toolbar, const std::string& item);
    void EnableItem(const std::string& toolbar, const std::string& item, bool enable = true);
    void ToggleItem(const std::string& toolbar, const std::string& item, bool toggle = true);
    bool IsItemEnabled(const std::string& toolbar, const std::string& item) const;
    bool IsItemToggled(const std::string& toolbar, const std::string& item) const;

    // Icon management
    void SetItemIcon(const std::string& toolbar, const std::string& item, const std::string& icon);
    std::string GetItemIcon(const std::string& toolbar, const std::string& item) const;

    // Tooltip management
    void SetItemTooltip(const std::string& toolbar, const std::string& item, const std::string& tooltip);
    std::string GetItemTooltip(const std::string& toolbar, const std::string& item) const;

    // Size management
    void SetItemSize(const std::string& toolbar, const std::string& item, const glm::vec2& size);
    glm::vec2 GetItemSize(const std::string& toolbar, const std::string& item) const;

    // Rendering
    void Render();

private:
    // Helper functions
    UIToolbarItem* FindItem(const std::string& toolbar, const std::string& item);
    const UIToolbarItem* FindItem(const std::string& toolbar, const std::string& item) const;
    void RenderItem(const UIToolbarItem& item);

    // Toolbar storage
    std::unordered_map<std::string, std::vector<UIToolbarItem>> m_Toolbars;
    bool m_IsDirty = false;
};

}} // namespace VGE::UI



