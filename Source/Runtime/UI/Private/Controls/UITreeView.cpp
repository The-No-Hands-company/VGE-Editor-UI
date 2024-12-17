#include "Runtime/UI/Public/Controls/UITreeView.h"
#include "Runtime/Core/Public/Core/UICore.h"
#include <algorithm>

namespace VGE {
namespace UI {

UITreeView::UITreeView()
    : m_HoveredNode(nullptr)
    , m_DraggedNode(nullptr)
    , m_DropTargetNode(nullptr)
    , m_ScrollOffset(0.0f)
    , m_IsDragging(false)
    , m_DragStartPos(0.0f)
{
    // Initialize default style
    m_Style.rowHeight = 24.0f;
    m_Style.indentWidth = 20.0f;
    m_Style.iconSize = 16.0f;
    m_Style.toggleSize = 12.0f;
    m_Style.togglePadding = 4.0f;
    m_Style.iconSpacing = 4.0f;
    m_Style.selectionPadding = 2.0f;
    m_Style.showGridLines = true;
    m_Style.showRootNode = false;
}

void UITreeView::SetRootNode(const TreeNode& node) {
    m_RootNode = node;
    m_SelectedNodes.clear();
    OnNodeAdded(&m_RootNode);
}

TreeNode* UITreeView::AddNode(TreeNode* parent, const TreeNode& node) {
    if (!parent) {
        parent = &m_RootNode;
    }
    
    parent->children.push_back(node);
    TreeNode* newNode = &parent->children.back();
    OnNodeAdded(newNode);
    return newNode;
}

void UITreeView::RemoveNode(TreeNode* node) {
    if (!node || node == &m_RootNode) return;

    // Find parent node
    std::function<bool(TreeNode&, TreeNode*)> removeFromParent = 
        [&](TreeNode& parent, TreeNode* target) -> bool {
        auto it = std::find_if(parent.children.begin(), parent.children.end(),
            [target](const TreeNode& child) { return &child == target; });
        
        if (it != parent.children.end()) {
            OnNodeRemoved(&(*it));
            parent.children.erase(it);
            return true;
        }

        for (auto& child : parent.children) {
            if (removeFromParent(child, target)) {
                return true;
            }
        }
        return false;
    };

    removeFromParent(m_RootNode, node);
}

void UITreeView::ClearNodes() {
    m_RootNode.children.clear();
    m_SelectedNodes.clear();
}

void UITreeView::ExpandNode(TreeNode* node, bool recursive) {
    if (!node) return;
    
    node->isExpanded = true;
    OnNodeExpanded(node);
    
    if (recursive) {
        for (auto& child : node->children) {
            ExpandNode(&child, true);
        }
    }
}

void UITreeView::CollapseNode(TreeNode* node, bool recursive) {
    if (!node) return;
    
    node->isExpanded = false;
    OnNodeCollapsed(node);
    
    if (recursive) {
        for (auto& child : node->children) {
            CollapseNode(&child, true);
        }
    }
}

void UITreeView::SelectNode(TreeNode* node, bool clearOthers) {
    if (!node) return;
    
    if (clearOthers) {
        ClearSelection();
    }
    
    if (!IsNodeSelected(node)) {
        node->isSelected = true;
        m_SelectedNodes.push_back(node);
        OnNodeSelected(node);
    }
}

void UITreeView::DeselectNode(TreeNode* node) {
    if (!node) return;
    
    node->isSelected = false;
    auto it = std::find(m_SelectedNodes.begin(), m_SelectedNodes.end(), node);
    if (it != m_SelectedNodes.end()) {
        m_SelectedNodes.erase(it);
    }
}

void UITreeView::EnsureNodeVisible(TreeNode* node) {
    if (!node) return;
    
    // Expand all parent nodes
    std::function<bool(TreeNode&, TreeNode*)> expandParents = 
        [&](TreeNode& root, TreeNode* target) -> bool {
        if (&root == target) return true;
        
        for (auto& child : root.children) {
            if (&child == target || expandParents(child, target)) {
                root.isExpanded = true;
                return true;
            }
        }
        return false;
    };
    
    expandParents(m_RootNode, node);
}

TreeNode* UITreeView::FindNode(const std::string& text) {
    std::function<TreeNode*(TreeNode&)> findInNode = 
        [&](TreeNode& node) -> TreeNode* {
        if (node.text == text) return &node;
        
        for (auto& child : node.children) {
            if (TreeNode* found = findInNode(child)) {
                return found;
            }
        }
        return nullptr;
    };
    
    return findInNode(m_RootNode);
}

TreeNode* UITreeView::GetNodeAtPosition(const glm::vec2& position) {
    float yOffset = -m_ScrollOffset;
    
    std::function<TreeNode*(TreeNode&, float&)> findAtPosition = 
        [&](TreeNode& node, float& currentY) -> TreeNode* {
        if (!IsNodeVisible(&node)) return nullptr;
        
        float nodeY = currentY;
        currentY += m_Style.rowHeight;
        
        if (position.y >= nodeY && position.y < nodeY + m_Style.rowHeight) {
            return &node;
        }
        
        if (node.isExpanded) {
            for (auto& child : node.children) {
                if (TreeNode* found = findAtPosition(child, currentY)) {
                    return found;
                }
            }
        }
        return nullptr;
    };
    
    if (!m_Style.showRootNode) {
        for (auto& child : m_RootNode.children) {
            if (TreeNode* found = findAtPosition(child, yOffset)) {
                return found;
            }
        }
        return nullptr;
    }
    
    return findAtPosition(m_RootNode, yOffset);
}

void UITreeView::ClearSelection() {
    for (auto* node : m_SelectedNodes) {
        node->isSelected = false;
    }
    m_SelectedNodes.clear();
}

bool UITreeView::IsNodeSelected(const TreeNode* node) const {
    return std::find(m_SelectedNodes.begin(), m_SelectedNodes.end(), node) != m_SelectedNodes.end();
}

void UITreeView::Render() {
    float yOffset = 0.0f;
    
    if (m_Style.showRootNode) {
        RenderNode(&m_RootNode, 0, yOffset);
    } else {
        for (auto& child : m_RootNode.children) {
            RenderNode(&child, 0, yOffset);
        }
    }
}

void UITreeView::Update(float deltaTime) {
    UpdateNodeStates();
    
    // Handle drag and drop
    if (m_IsDragging) {
        glm::vec2 mousePos = GetMousePosition(); // Implement this based on your input system
        HandleNodeDrag(m_DraggedNode, mousePos);
    }
}

void UITreeView::RenderNode(TreeNode* node, int level, float& yOffset) {
    if (!node || !IsNodeVisible(node)) return;
    
    float xOffset = level * m_Style.indentWidth;
    glm::vec2 nodePos(xOffset, yOffset);
    float width = CalculateContentWidth();
    
    // Render node content
    RenderNodeContent(node, nodePos, width);
    yOffset += m_Style.rowHeight;
    
    // Render children if expanded
    if (node->isExpanded) {
        for (auto& child : node->children) {
            RenderNode(&child, level + 1, yOffset);
        }
    }
}

void UITreeView::RenderNodeContent(TreeNode* node, const glm::vec2& position, float width) {
    // Implement based on your rendering system
    // This should render:
    // 1. Background (selected/hovered state)
    // 2. Expand/collapse toggle
    // 3. Icon
    // 4. Text
    // 5. Grid lines if enabled
}

void UITreeView::RenderToggle(TreeNode* node, const glm::vec2& position) {
    // Implement based on your rendering system
    // This should render the expand/collapse toggle button
}

void UITreeView::RenderIcon(TreeNode* node, const glm::vec2& position) {
    // Implement based on your rendering system
    // This should render the node icon if one is specified
}

void UITreeView::RenderDragHighlight(const glm::vec2& position, float width) {
    // Implement based on your rendering system
    // This should render the drag target highlight
}

void UITreeView::HandleNodeClick(TreeNode* node, bool isDoubleClick) {
    if (!node) return;
    
    if (isDoubleClick) {
        if (m_OnNodeDoubleClicked) {
            m_OnNodeDoubleClicked(node);
        }
    } else {
        SelectNode(node, !IsKeyDown(Key::Control)); // Implement IsKeyDown based on your input system
        if (m_OnNodeSelected) {
            m_OnNodeSelected(node);
        }
    }
}

void UITreeView::HandleNodeDrag(TreeNode* node, const glm::vec2& position) {
    if (!node || !node->isDraggable) return;
    
    TreeNode* dropTarget = GetNodeAtPosition(position);
    if (dropTarget != m_DropTargetNode) {
        m_DropTargetNode = dropTarget;
    }
    
    if (m_DropTargetNode && IsNodeDropAllowed(node, m_DropTargetNode)) {
        RenderDragHighlight(position, CalculateContentWidth());
    }
}

void UITreeView::UpdateNodeStates() {
    // Update hover state
    glm::vec2 mousePos = GetMousePosition(); // Implement this based on your input system
    m_HoveredNode = GetNodeAtPosition(mousePos);
    
    // Update drag state
    if (IsMouseButtonPressed(MouseButton::Left) && m_HoveredNode) { // Implement these based on your input system
        m_IsDragging = true;
        m_DraggedNode = m_HoveredNode;
        m_DragStartPos = mousePos;
        if (m_OnNodeDragStart) {
            m_OnNodeDragStart(m_DraggedNode);
        }
    } else if (IsMouseButtonReleased(MouseButton::Left) && m_IsDragging) {
        if (m_DropTargetNode && m_DraggedNode && 
            IsNodeDropAllowed(m_DraggedNode, m_DropTargetNode)) {
            if (m_OnNodeDrop) {
                m_OnNodeDrop(m_DraggedNode, m_DropTargetNode);
            }
        }
        m_IsDragging = false;
        m_DraggedNode = nullptr;
        m_DropTargetNode = nullptr;
    }
}

float UITreeView::CalculateContentWidth() const {
    // Implement based on your layout system
    // Should return the available width for node content
    return GetSize().x; // Assuming GetSize() returns the widget's size
}

bool UITreeView::IsNodeVisible(const TreeNode* node) const {
    if (!node) return false;
    if (node == &m_RootNode) return m_Style.showRootNode;
    
    // Check if all parent nodes are expanded
    std::function<bool(const TreeNode&, const TreeNode*)> isParentExpanded = 
        [&](const TreeNode& root, const TreeNode* target) -> bool {
        for (const auto& child : root.children) {
            if (&child == target) return true;
            if (child.isExpanded && isParentExpanded(child, target)) {
                return true;
            }
        }
        return false;
    };
    
    return isParentExpanded(m_RootNode, node);
}

bool UITreeView::IsNodeDropAllowed(TreeNode* source, TreeNode* target) const {
    if (!source || !target) return false;
    if (!source->isDraggable || !target->isDropTarget) return false;
    
    // Prevent dropping on self or children
    std::function<bool(const TreeNode&, const TreeNode*)> isAncestor = 
        [&](const TreeNode& node, const TreeNode* target) -> bool {
        if (&node == target) return true;
        for (const auto& child : node.children) {
            if (isAncestor(child, target)) return true;
        }
        return false;
    };
    
    return !isAncestor(*source, target);
}

// Protected event handlers with default empty implementations
void UITreeView::OnNodeAdded(TreeNode* node) {}
void UITreeView::OnNodeRemoved(TreeNode* node) {}
void UITreeView::OnNodeSelected(TreeNode* node) {}
void UITreeView::OnNodeExpanded(TreeNode* node) {}
void UITreeView::OnNodeCollapsed(TreeNode* node) {}
void UITreeView::OnNodeDoubleClicked(TreeNode* node) {}
void UITreeView::OnNodeDragStart(TreeNode* node) {}
bool UITreeView::OnNodeDrop(TreeNode* source, TreeNode* target) { return false; }

}} // namespace VGE::UI



