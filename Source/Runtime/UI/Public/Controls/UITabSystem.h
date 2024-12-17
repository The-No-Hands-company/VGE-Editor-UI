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
 * @brief Tab item data
 */
struct TabItem {
    std::string title;          ///< Tab title
    std::string tooltip;        ///< Tab tooltip
    std::string icon;           ///< Tab icon
    bool isCloseable = true;    ///< Whether tab can be closed
    bool isDraggable = true;    ///< Whether tab can be dragged
    UIWidget* content = nullptr; ///< Tab content widget
};

/**
 * @brief Tab bar appearance settings
 */
struct TabBarStyle {
    float height = 30.0f;       ///< Tab bar height
    float tabMinWidth = 100.0f; ///< Minimum tab width
    float tabMaxWidth = 200.0f; ///< Maximum tab width
    float tabPadding = 8.0f;    ///< Tab padding
    float iconSize = 16.0f;     ///< Tab icon size
    float closeButtonSize = 12.0f; ///< Close button size
    float scrollButtonWidth = 20.0f; ///< Scroll button width
};

/**
 * @brief Tab system widget for managing tabbed content
 */
class UITabSystem : public UIWidget {
public:
    UITabSystem();
    virtual ~UITabSystem() = default;

    // Tab management
    int AddTab(const TabItem& item);
    void RemoveTab(int index);
    void RemoveAllTabs();
    void SelectTab(int index);
    int GetSelectedTab() const { return m_SelectedTab; }
    int GetTabCount() const { return static_cast<int>(m_Tabs.size()); }
    const TabItem* GetTab(int index) const;
    TabItem* GetTab(int index);

    // Tab operations
    void MoveTab(int fromIndex, int toIndex);
    void SwapTabs(int index1, int index2);
    bool IsTabVisible(int index) const;
    void EnsureTabVisible(int index);

    // Style
    void SetStyle(const TabBarStyle& style) { m_Style = style; }
    const TabBarStyle& GetStyle() const { return m_Style; }

    // Event callbacks
    using TabCallback = std::function<void(int)>;
    void SetTabSelectedCallback(TabCallback callback) { m_OnTabSelected = callback; }
    void SetTabClosedCallback(TabCallback callback) { m_OnTabClosed = callback; }
    void SetTabMovedCallback(std::function<void(int, int)> callback) { m_OnTabMoved = callback; }

    // Widget interface
    void Render() override;
    void Update(float deltaTime) override;

protected:
    // Event handlers
    virtual void OnTabAdded(int index);
    virtual void OnTabRemoved(int index);
    virtual void OnTabSelected(int index);
    virtual void OnTabMoved(int fromIndex, int toIndex);
    virtual void OnTabStartDrag(int index);
    virtual void OnTabEndDrag(int index, bool dropped);

private:
    // Helper functions
    void RenderTabBar();
    void RenderTab(int index, const glm::vec2& position);
    void RenderContent();
    void UpdateTabPositions();
    void HandleTabClick(int index);
    void HandleTabClose(int index);
    bool IsTabDraggable(int index) const;
    bool IsTabCloseable(int index) const;
    float GetTabWidth(int index) const;

private:
    std::vector<TabItem> m_Tabs;
    TabBarStyle m_Style;
    int m_SelectedTab;
    int m_HoveredTab;
    int m_DraggedTab;
    float m_ScrollOffset;
    bool m_IsDragging;
    glm::vec2 m_DragStartPos;
    TabCallback m_OnTabSelected;
    TabCallback m_OnTabClosed;
    std::function<void(int, int)> m_OnTabMoved;
};

}} // namespace VGE::UI 