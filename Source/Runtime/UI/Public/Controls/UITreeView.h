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
 * @brief Tree node data
 */
struct TreeNode {
    std::string text;           ///< Node display text
    std::string tooltip;        ///< Node tooltip
    std::string icon;           ///< Node icon path
    bool isExpanded = false;    ///< Node expansion state
    bool isSelected = false;    ///< Node selection state
    bool isDraggable = true;    ///< Whether node can be dragged
    bool isDropTarget = true;   ///< Whether node can accept drops
    void* userData = nullptr;   ///< User-defined data pointer
    std::vector<TreeNode> children; ///< Child nodes
};

/**
 * @brief Tree view appearance settings
 */
struct TreeViewStyle {
    float rowHeight = 24.0f;        ///< Height of each tree row
    float indentWidth = 20.0f;      ///< Indentation width per level
    float iconSize = 16.0f;         ///< Size of node icons
    float toggleSize = 12.0f;       ///< Size of expand/collapse toggle
    float togglePadding = 4.0f;     ///< Padding around toggle
    float iconSpacing = 4.0f;       ///< Spacing between icon and text
    float selectionPadding = 2.0f;  ///< Padding around selection highlight
    bool showGridLines = true;      ///< Whether to show grid lines
    bool showRootNode = false;      ///< Whether to show root node
};

/**
 * @brief Tree view widget for displaying hierarchical data
 */
class UITreeView : public UIWidget {
public:
    UITreeView();
    virtual ~UITreeView() = default;

    // Node management
    void SetRootNode(const TreeNode& node);
    TreeNode* AddNode(TreeNode* parent, const TreeNode& node);
    void RemoveNode(TreeNode* node);
    void ClearNodes();
    TreeNode* GetRootNode() { return &m_RootNode; }
    const TreeNode* GetRootNode() const { return &m_RootNode; }

    // Node operations
    void ExpandNode(TreeNode* node, bool recursive = false);
    void CollapseNode(TreeNode* node, bool recursive = false);
    void SelectNode(TreeNode* node, bool clearOthers = true);
    void DeselectNode(TreeNode* node);
    void EnsureNodeVisible(TreeNode* node);
    TreeNode* FindNode(const std::string& text);
    TreeNode* GetNodeAtPosition(const glm::vec2& position);

    // Selection
    const std::vector<TreeNode*>& GetSelectedNodes() const { return m_SelectedNodes; }
    void ClearSelection();
    bool IsNodeSelected(const TreeNode* node) const;

    // Style
    void SetStyle(const TreeViewStyle& style) { m_Style = style; }
    const TreeViewStyle& GetStyle() const { return m_Style; }

    // Event callbacks
    using NodeCallback = std::function<void(TreeNode*)>;
    using NodeDropCallback = std::function<bool(TreeNode*, TreeNode*)>;
    void SetNodeSelectedCallback(NodeCallback callback) { m_OnNodeSelected = callback; }
    void SetNodeExpandedCallback(NodeCallback callback) { m_OnNodeExpanded = callback; }
    void SetNodeCollapsedCallback(NodeCallback callback) { m_OnNodeCollapsed = callback; }
    void SetNodeDoubleClickedCallback(NodeCallback callback) { m_OnNodeDoubleClicked = callback; }
    void SetNodeDragStartCallback(NodeCallback callback) { m_OnNodeDragStart = callback; }
    void SetNodeDropCallback(NodeDropCallback callback) { m_OnNodeDrop = callback; }

    // Widget interface
    void Render() override;
    void Update(float deltaTime) override;

protected:
    // Event handlers
    virtual void OnNodeAdded(TreeNode* node);
    virtual void OnNodeRemoved(TreeNode* node);
    virtual void OnNodeSelected(TreeNode* node);
    virtual void OnNodeExpanded(TreeNode* node);
    virtual void OnNodeCollapsed(TreeNode* node);
    virtual void OnNodeDoubleClicked(TreeNode* node);
    virtual void OnNodeDragStart(TreeNode* node);
    virtual bool OnNodeDrop(TreeNode* source, TreeNode* target);

private:
    // Helper functions
    void RenderNode(TreeNode* node, int level, float& yOffset);
    void RenderNodeContent(TreeNode* node, const glm::vec2& position, float width);
    void RenderToggle(TreeNode* node, const glm::vec2& position);
    void RenderIcon(TreeNode* node, const glm::vec2& position);
    void RenderDragHighlight(const glm::vec2& position, float width);
    void HandleNodeClick(TreeNode* node, bool isDoubleClick);
    void HandleNodeDrag(TreeNode* node, const glm::vec2& position);
    void UpdateNodeStates();
    float CalculateContentWidth() const;
    bool IsNodeVisible(const TreeNode* node) const;
    bool IsNodeDropAllowed(TreeNode* source, TreeNode* target) const;

private:
    TreeNode m_RootNode;                        ///< Root tree node
    TreeViewStyle m_Style;                      ///< Tree view style settings
    std::vector<TreeNode*> m_SelectedNodes;     ///< Currently selected nodes
    TreeNode* m_HoveredNode;                    ///< Currently hovered node
    TreeNode* m_DraggedNode;                    ///< Currently dragged node
    TreeNode* m_DropTargetNode;                 ///< Current drop target node
    float m_ScrollOffset;                       ///< Vertical scroll offset
    bool m_IsDragging;                         ///< Drag operation state
    glm::vec2 m_DragStartPos;                  ///< Drag start position
    
    // Callbacks
    NodeCallback m_OnNodeSelected;
    NodeCallback m_OnNodeExpanded;
    NodeCallback m_OnNodeCollapsed;
    NodeCallback m_OnNodeDoubleClicked;
    NodeCallback m_OnNodeDragStart;
    NodeDropCallback m_OnNodeDrop;
};

}} // namespace VGE::UI



