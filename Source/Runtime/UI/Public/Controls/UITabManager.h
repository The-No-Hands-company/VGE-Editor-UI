#pragma once

#include "Runtime/Core/Public/Core/UICore.h"
#include "Runtime/Core/Public/Widget/UIWidget.h"
#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace VGE {
namespace UI {

/**
 * @brief Information about a tab
 */
struct UITabInfo {
    std::string id;                      ///< Unique identifier for the tab
    std::string label;                   ///< Display label
    std::shared_ptr<UIWidget> content;   ///< Tab content widget
    bool canClose = true;                ///< Whether the tab can be closed
    bool isActive = false;               ///< Whether the tab is currently active
};

/**
 * @brief Manages a collection of tabs and their content
 */
class UITabManager {
public:
    UITabManager() = default;
    virtual ~UITabManager() = default;

    /**
     * @brief Add a new tab
     * @param id Unique identifier for the tab
     * @param label Display label for the tab
     * @param content Widget to display in the tab
     * @param canClose Whether the tab can be closed
     * @return True if tab was added successfully
     */
    bool AddTab(const std::string& id, const std::string& label,
               std::shared_ptr<UIWidget> content, bool canClose = true);

    /**
     * @brief Remove a tab by its ID
     * @param id ID of the tab to remove
     * @return True if tab was removed
     */
    bool RemoveTab(const std::string& id);

    /**
     * @brief Get all tabs
     * @return Vector of tab information
     */
    const std::vector<UITabInfo>& GetTabs() const { return m_Tabs; }

    /**
     * @brief Get the active tab
     * @return Pointer to active tab info, or nullptr if none active
     */
    const UITabInfo* GetActiveTab() const;

    /**
     * @brief Set callbacks for tab events
     */
    void SetOnTabActivated(std::function<void(const UITabInfo&)> callback) { m_OnTabActivated = callback; }
    void SetOnTabClosed(std::function<void(const UITabInfo&)> callback) { m_OnTabClosed = callback; }
    void SetOnTabDragStart(std::function<void(const UITabInfo&)> callback) { m_OnTabDragStart = callback; }
    void SetOnTabDragEnd(std::function<void(const UITabInfo&)> callback) { m_OnTabDragEnd = callback; }

private:
    std::vector<UITabInfo> m_Tabs;
    std::string m_ActiveTabId;

    // Event callbacks
    std::function<void(const UITabInfo&)> m_OnTabActivated;
    std::function<void(const UITabInfo&)> m_OnTabClosed;
    std::function<void(const UITabInfo&)> m_OnTabDragStart;
    std::function<void(const UITabInfo&)> m_OnTabDragEnd;
};

}} // namespace VGE::UI 