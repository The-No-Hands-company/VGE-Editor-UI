#pragma once

#include "Runtime/Core/Public/Core/UICore.h"
#include "Runtime/Core/Public/Widget/UIWidget.h"
#include "Runtime/UI/Public/Docking/UIDockZone.h"
#include "Runtime/UI/Public/Docking/UIDockPreview.h"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

namespace VGE {
namespace Editor {

class UITabManager;
class UITabInfo;

/**
 * @brief Structure representing a docked window
 */
struct DockedWindow {
    std::shared_ptr<UIWidget> widget;
    std::string name;
    std::string title;
    bool isVisible = true;
    bool isActive = false;
};

/**
 * @brief Structure representing a split in the dock space
 */
struct DockSplit {
    bool isVertical = true;
    float ratio = 0.5f;
    std::shared_ptr<UITabManager> leftTabs;
    std::shared_ptr<UITabManager> rightTabs;
};

/**
 * @brief A dock space that manages dockable windows and splits
 */
class UIDockSpace : public UIWidget {
public:
    explicit UIDockSpace(const std::string& name);
    virtual ~UIDockSpace() = default;

    // Initialization
    void Initialize() override;

    // Widget interface
    void Update(float deltaTime) override;
    void Render() override;

    // Window management
    bool DockWindow(std::shared_ptr<UIWidget> window, const std::string& name,
                   const std::string& title, DockZone zone);
    bool UndockWindow(const std::string& name);

    // Split management
    bool Split(bool isVertical, float ratio = 0.5f);
    void AddSplit(const DockSplit& split) { m_Splits.push_back(split); }

    // Drag operations
    void BeginDrag(const std::string& windowName);
    bool EndDrag();
    void UpdateDragPreview(const glm::vec2& mousePos);

    // Layout management
    void Clear();
    void AddDockedWindow(const DockedWindow& window);

    // Getters
    const std::string& GetName() const { return m_Name; }
    const std::vector<DockSplit>& GetSplits() const { return m_Splits; }
    const std::unordered_map<std::string, DockedWindow>& GetDockedWindows() const { return m_DockedWindows; }
    std::shared_ptr<UITabManager> GetRootTabs() const { return m_RootTabs; }

protected:
    // Tab event handlers
    void OnTabActivated(const UITabInfo& tab);
    void OnTabClosed(const UITabInfo& tab);
    void OnTabDragStart(const UITabInfo& tab);
    void OnTabDragEnd(const UITabInfo& tab);

private:
    std::shared_ptr<UITabManager> CreateTabManager();

private:
    std::string m_Name;
    bool m_FirstFrame;
    std::shared_ptr<UITabManager> m_RootTabs;
    std::vector<DockSplit> m_Splits;
    std::unordered_map<std::string, DockedWindow> m_DockedWindows;
    std::string m_DraggingWindow;
    UIDockPreview m_DragPreview;
};

}} // namespace VGE::Editor 