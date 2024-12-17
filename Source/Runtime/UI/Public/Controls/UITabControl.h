#pragma once

#include "Runtime/Core/Public/Core/UICore.h"
#include "Runtime/Core/Public/Widget/UIWidget.h"

#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace VGE {
namespace UI {

struct TabItem {
    std::string Name;
    std::string Icon;  // Path to icon or icon identifier
    bool IsActive = false;
    bool IsHovered = false;
    bool CanClose = true;
    std::shared_ptr<UIWidget> Content;
};

class UI_API UITabControl : public UIWidget {
public:
    UITabControl(const std::string& label = "");
    virtual ~UITabControl() = default;

    virtual void Update() override;
    virtual void Draw() override;
    virtual bool HandleEvent(Event& event) override;

    // Tab management
    int AddTab(const std::string& name, std::shared_ptr<UIWidget> content = nullptr);
    void RemoveTab(int index);
    void RemoveTab(const std::string& name);
    void ClearTabs();
    void MoveTab(int fromIndex, int toIndex);

    // Tab access
    TabItem* GetTab(int index);
    TabItem* GetTab(const std::string& name);
    TabItem* GetActiveTab() { return m_ActiveTab; }
    const std::vector<TabItem>& GetTabs() const { return m_Tabs; }
    
    // Tab operations
    void SetActiveTab(int index);
    void SetActiveTab(const std::string& name);
    
    // Appearance
    void SetTabHeight(float height) { m_TabHeight = height; }
    void SetTabPadding(float padding) { m_TabPadding = padding; }
    void SetCloseButtonSize(float size) { m_CloseButtonSize = size; }
    void SetShowAddButton(bool show) { m_ShowAddButton = show; }
    void SetTabsCloseable(bool closeable) { m_TabsCloseable = closeable; }
    void SetReorderable(bool reorderable) { m_Reorderable = reorderable; }
    
    // Events
    void SetTabChangedCallback(std::function<void(TabItem*)> callback) {
        m_TabChangedCallback = callback;
    }
    void SetTabClosingCallback(std::function<bool(TabItem*)> callback) {
        m_TabClosingCallback = callback;
    }
    void SetTabAddedCallback(std::function<void(TabItem*)> callback) {
        m_TabAddedCallback = callback;
    }

private:
    // Rendering helpers
    void DrawTabBar();
    void DrawTab(TabItem& tab, const glm::vec2& position, float width);
    void DrawTabContent(TabItem& tab);
    void DrawAddButton();
    void DrawCloseButton(const glm::vec2& position, bool isHovered);
    void DrawScrollIndicators();
    void DrawScrollButtons();
    
    // Layout calculations
    float CalculateTabWidth(const TabItem& tab) const;
    glm::vec2 GetContentAreaPosition() const;
    glm::vec2 GetContentAreaSize() const;
    float GetTabXPosition(const TabItem* tab) const;
    int GetTabIndexAtPosition(float x) const;
    float GetMaxScrollOffset() const;
    bool NeedsScrolling() const;
    
    // Event handling
    bool HandleTabBarEvents(Event& event);
    bool HandleTabContentEvents(Event& event);
    bool HandleDragAndDrop(Event& event);
    bool HandleScrolling(Event& event);
    TabItem* HitTestTab(const glm::vec2& position, float& outTabX);
    void FinalizeDragAndDrop();
    
    // Core properties
    std::vector<TabItem> m_Tabs;
    TabItem* m_ActiveTab = nullptr;
    TabItem* m_HoveredTab = nullptr;
    
    // Appearance
    float m_TabHeight = 24.0f;
    float m_TabPadding = 10.0f;
    float m_CloseButtonSize = 12.0f;
    bool m_ShowAddButton = true;
    bool m_TabsCloseable = true;
    bool m_Reorderable = true;
    
    // Scrolling state
    float m_ScrollOffset = 0.0f;
    float m_ScrollVelocity = 0.0f;
    glm::vec2 m_LastMousePos;
    
    // Drag and drop state
    bool m_IsDragging = false;
    TabItem* m_DraggedTab = nullptr;
    glm::vec2 m_DragStartPos;
    glm::vec2 m_DragOffset;
    float m_DraggedTabWidth = 0.0f;
    
    // Callbacks
    std::function<void(TabItem*)> m_TabChangedCallback;
    std::function<bool(TabItem*)> m_TabClosingCallback;
    std::function<void(TabItem*)> m_TabAddedCallback;
};

} // namespace UI
} // namespace VGE



