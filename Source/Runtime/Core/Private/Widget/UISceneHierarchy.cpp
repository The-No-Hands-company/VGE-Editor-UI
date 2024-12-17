#include "Runtime/Core/Public/Widget/UISceneHierarchy.h"
#include "Runtime/Core/Public/Theme/UIAnimationPresets.h"
#include "Runtime/Core/Public/Theme/UIAnimationTemplates.h"
#include "Runtime/Core/Public/Render/UIRenderer.h"
#include "Runtime/Core/Public/Widget/UIPropertyPanel.h"
#include <algorithm>

namespace VGE {
namespace UI {

UISceneHierarchy::UISceneHierarchy()
    : m_IsDragging(false)
    , m_ScrollOffset(0.0f)
    , m_ContentHeight(0.0f)
{
    // Set up animation state machine
    auto stateMachine = std::make_shared<UIAnimationStateMachine>();
    
    // Add states
    stateMachine->AddState("Normal", std::make_shared<UIAnimationState>("Normal"));
    stateMachine->AddState("DragActive", CreateDragActiveState());
    
    // Add transitions
    stateMachine->AddTransition(UIAnimationPresets::CreateFadeTransition("Normal", "DragActive", 0.2f));
    stateMachine->AddTransition(UIAnimationPresets::CreateFadeTransition("DragActive", "Normal", 0.2f));
    
    stateMachine->SetDefaultState("Normal");
    SetAnimationStateMachine(stateMachine);
    
    m_PropertyPanel = std::make_shared<UIPropertyPanel>();
    AddChild(m_PropertyPanel);
}

void UISceneHierarchy::OnLayoutUpdate() {
    UpdateLayout();
    UpdateVisibleItems();
}

glm::vec2 UISceneHierarchy::CalculateDesiredSize() {
    return glm::vec2(200.0f, m_ContentHeight); // Default width, height based on content
}

void UISceneHierarchy::ArrangeChildren() {
    // Layout tree view on left side
    float treeWidth = GetSize().x * 0.4f;
    SetTreeViewSize(glm::vec2(treeWidth, GetSize().y));
    
    // Layout property panel on right side
    if (m_PropertyPanel) {
        float propertyPanelWidth = GetSize().x - treeWidth - 4.0f;
        m_PropertyPanel->SetPosition(glm::vec2(treeWidth + 4.0f, 0.0f));
        m_PropertyPanel->SetSize(glm::vec2(propertyPanelWidth, GetSize().y));
    }
    
    // Update base class
    UITreeView::ArrangeChildren();
}

void UISceneHierarchy::OnPaint(UIRenderer* renderer) {
    DrawBackground(renderer);
    DrawSearchBar(renderer);
    DrawColumnHeaders(renderer);
    DrawColumns(renderer);
    DrawItems(renderer);
    
    if (m_IsDragging) {
        DrawDragIndicator(renderer);
    }
    
    // Draw splitter
    float treeWidth = GetSize().x * 0.4f;
    glm::vec2 splitterPos(treeWidth, 0.0f);
    glm::vec2 splitterSize(4.0f, GetSize().y);
    renderer->DrawRect(GetPosition() + splitterPos, splitterSize, glm::vec4(0.3f));
}

void UISceneHierarchy::OnChildAdded(std::shared_ptr<UIWidget> child) {
    // Scene hierarchy doesn't support child widgets
}

void UISceneHierarchy::OnChildRemoved(std::shared_ptr<UIWidget> child) {
    // Scene hierarchy doesn't support child widgets
}

void UISceneHierarchy::OnAnimationUpdate(float deltaTime) {
    // Update animation states
}

void UISceneHierarchy::OnAnimationStateChanged(const std::string& newState) {
    // Handle state changes
}

void UISceneHierarchy::AddItem(const std::shared_ptr<SceneHierarchyItem>& item, const std::shared_ptr<SceneHierarchyItem>& parent) {
    if (!item) return;
    
    if (parent) {
        item->parent = parent;
        parent->children.push_back(item);
    } else {
        m_RootItems.push_back(item);
    }
    
    UpdateLayout();
}

void UISceneHierarchy::RemoveItem(const std::shared_ptr<SceneHierarchyItem>& item) {
    if (!item) return;
    
    // Remove from selection
    auto selIt = std::find(m_SelectedItems.begin(), m_SelectedItems.end(), item);
    if (selIt != m_SelectedItems.end()) {
        m_SelectedItems.erase(selIt);
        NotifySelectionChanged();
    }
    
    // Remove from parent
    if (auto parent = item->parent.lock()) {
        auto it = std::find(parent->children.begin(), parent->children.end(), item);
        if (it != parent->children.end()) {
            parent->children.erase(it);
        }
    } else {
        // Remove from root items
        auto it = std::find(m_RootItems.begin(), m_RootItems.end(), item);
        if (it != m_RootItems.end()) {
            m_RootItems.erase(it);
        }
    }
    
    UpdateLayout();
}

void UISceneHierarchy::Clear() {
    m_RootItems.clear();
    m_SelectedItems.clear();
    m_VisibleItems.clear();
    m_DraggedItem = nullptr;
    m_DropTarget = nullptr;
    m_IsDragging = false;
    
    UpdateLayout();
    NotifySelectionChanged();
}

void UISceneHierarchy::SelectItem(const std::shared_ptr<SceneHierarchyItem>& item, bool addToSelection) {
    if (!item) return;
    
    UpdateSelection(item, addToSelection);
    NotifySelectionChanged();
}

void UISceneHierarchy::DeselectItem(const std::shared_ptr<SceneHierarchyItem>& item) {
    if (!item) return;
    
    auto it = std::find(m_SelectedItems.begin(), m_SelectedItems.end(), item);
    if (it != m_SelectedItems.end()) {
        item->isSelected = false;
        m_SelectedItems.erase(it);
        NotifySelectionChanged();
    }
}

void UISceneHierarchy::ClearSelection() {
    for (auto& item : m_SelectedItems) {
        item->isSelected = false;
    }
    m_SelectedItems.clear();
    NotifySelectionChanged();
}

void UISceneHierarchy::ExpandItem(const std::shared_ptr<SceneHierarchyItem>& item) {
    if (!item || item->isExpanded) return;
    
    item->isExpanded = true;
    UpdateLayout();
    
    if (m_OnItemExpanded) {
        m_OnItemExpanded(item);
    }
}

void UISceneHierarchy::CollapseItem(const std::shared_ptr<SceneHierarchyItem>& item) {
    if (!item || !item->isExpanded) return;
    
    item->isExpanded = false;
    UpdateLayout();
    
    if (m_OnItemCollapsed) {
        m_OnItemCollapsed(item);
    }
}

void UISceneHierarchy::ExpandAll() {
    std::function<void(const std::shared_ptr<SceneHierarchyItem>&)> expand = 
        [&](const std::shared_ptr<SceneHierarchyItem>& item) {
            item->isExpanded = true;
            for (const auto& child : item->children) {
                expand(child);
            }
        };
    
    for (const auto& item : m_RootItems) {
        expand(item);
    }
    
    UpdateLayout();
}

void UISceneHierarchy::CollapseAll() {
    std::function<void(const std::shared_ptr<SceneHierarchyItem>&)> collapse = 
        [&](const std::shared_ptr<SceneHierarchyItem>& item) {
            item->isExpanded = false;
            for (const auto& child : item->children) {
                collapse(child);
            }
        };
    
    for (const auto& item : m_RootItems) {
        collapse(item);
    }
    
    UpdateLayout();
}

bool UISceneHierarchy::CanDrop(const std::shared_ptr<SceneHierarchyItem>& source, const std::shared_ptr<SceneHierarchyItem>& target) const {
    if (!source || !target) return false;
    
    // Don't allow dropping on itself or its children
    std::function<bool(const std::shared_ptr<SceneHierarchyItem>&)> isChild = 
        [&](const std::shared_ptr<SceneHierarchyItem>& item) -> bool {
            if (item == source) return true;
            for (const auto& child : item->children) {
                if (isChild(child)) return true;
            }
            return false;
        };
    
    if (isChild(target)) return false;
    
    // Allow custom validation
    if (m_OnCanDrop) {
        return m_OnCanDrop(source, target);
    }
    
    return true;
}

bool UISceneHierarchy::DropItem(const std::shared_ptr<SceneHierarchyItem>& source, const std::shared_ptr<SceneHierarchyItem>& target) {
    if (!CanDrop(source, target)) return false;
    
    // Remove from old parent
    if (auto parent = source->parent.lock()) {
        auto it = std::find(parent->children.begin(), parent->children.end(), source);
        if (it != parent->children.end()) {
            parent->children.erase(it);
        }
    } else {
        auto it = std::find(m_RootItems.begin(), m_RootItems.end(), source);
        if (it != m_RootItems.end()) {
            m_RootItems.erase(it);
        }
    }
    
    // Add to new parent
    source->parent = target;
    target->children.push_back(source);
    
    // Notify custom handler
    if (m_OnDrop) {
        m_OnDrop(source, target);
    }
    
    UpdateLayout();
    return true;
}

void UISceneHierarchy::SetStyle(const SceneHierarchyStyle& style) {
    m_Style = style;
    UpdateLayout();
}

void UISceneHierarchy::UpdateLayout() {
    // Calculate content height
    float height = 0.0f;
    
    std::function<void(const std::shared_ptr<SceneHierarchyItem>&, int)> calculateHeight = 
        [&](const std::shared_ptr<SceneHierarchyItem>& item, int level) {
            if (!item->isVisible) return;
            
            height += m_Style.itemHeight;
            
            if (item->isExpanded) {
                for (const auto& child : item->children) {
                    calculateHeight(child, level + 1);
                }
            }
        };
    
    for (const auto& item : m_RootItems) {
        calculateHeight(item, 0);
    }
    
    m_ContentHeight = height;
}

void UISceneHierarchy::UpdateVisibleItems() {
    m_VisibleItems.clear();
    
    std::function<void(const std::shared_ptr<SceneHierarchyItem>&)> collectVisible = 
        [&](const std::shared_ptr<SceneHierarchyItem>& item) {
            if (!item->isVisible) return;
            
            m_VisibleItems.push_back(item);
            
            if (item->isExpanded) {
                for (const auto& child : item->children) {
                    collectVisible(child);
                }
            }
        };
    
    for (const auto& item : m_RootItems) {
        collectVisible(item);
    }
}

void UISceneHierarchy::HandleMouseDown(const MouseEvent& event) {
    auto item = FindItemAtPosition(event.position);
    if (!item) {
        ClearSelection();
        return;
    }
    
    // Start drag operation
    if (event.button == MouseButton::Left) {
        m_DragStartPos = event.position;
        m_DraggedItem = item;
    }
    
    // Handle selection
    UpdateSelection(item, event.modifiers & ModifierKeys::Control);
    NotifySelectionChanged();
}

void UISceneHierarchy::HandleMouseMove(const MouseEvent& event) {
    if (m_DraggedItem && !m_IsDragging) {
        // Check if drag should start
        const float dragThreshold = 5.0f;
        if (glm::length(event.position - m_DragStartPos) > dragThreshold) {
            m_IsDragging = true;
            GetAnimationStateMachine()->TransitionTo("DragActive");
            
            if (m_OnBeginDrag) {
                m_OnBeginDrag(m_DraggedItem);
            }
        }
    }
    
    if (m_IsDragging) {
        // Update drop target
        auto target = FindItemAtPosition(event.position);
        if (target != m_DropTarget) {
            m_DropTarget = target;
        }
    }
}

void UISceneHierarchy::HandleMouseUp(const MouseEvent& event) {
    if (m_IsDragging) {
        if (m_DropTarget && CanDrop(m_DraggedItem, m_DropTarget)) {
            DropItem(m_DraggedItem, m_DropTarget);
        }
        
        m_IsDragging = false;
        GetAnimationStateMachine()->TransitionTo("Normal");
        
        if (m_OnEndDrag) {
            m_OnEndDrag(m_DraggedItem);
        }
    }
    
    m_DraggedItem = nullptr;
    m_DropTarget = nullptr;
}

void UISceneHierarchy::HandleKeyPress(const KeyEvent& event) {
    if (m_SelectedItems.empty()) return;
    
    auto currentItem = m_SelectedItems.back();
    
    switch (event.key) {
        case Key::Up: {
            // Select previous visible item
            auto it = std::find(m_VisibleItems.begin(), m_VisibleItems.end(), currentItem);
            if (it != m_VisibleItems.begin()) {
                SelectItem(*--it, event.modifiers & ModifierKeys::Shift);
            }
            break;
        }
        
        case Key::Down: {
            // Select next visible item
            auto it = std::find(m_VisibleItems.begin(), m_VisibleItems.end(), currentItem);
            if (it != m_VisibleItems.end() && ++it != m_VisibleItems.end()) {
                SelectItem(*it, event.modifiers & ModifierKeys::Shift);
            }
            break;
        }
        
        case Key::Left:
            if (currentItem->isExpanded) {
                CollapseItem(currentItem);
            }
            break;
            
        case Key::Right:
            if (!currentItem->isExpanded) {
                ExpandItem(currentItem);
            }
            break;
    }
}

void UISceneHierarchy::DrawBackground(UIRenderer* renderer) {
    renderer->DrawRect(GetBounds(), m_Style.backgroundColor);
}

void UISceneHierarchy::DrawItems(UIRenderer* renderer) {
    float y = GetBounds().y - m_ScrollOffset;
    
    std::function<void(const std::shared_ptr<SceneHierarchyItem>&, int)> drawItem = 
        [&](const std::shared_ptr<SceneHierarchyItem>& item, int level) {
            if (!item->isVisible) return;
            
            DrawItem(renderer, item, y, level);
            y += m_Style.itemHeight;
            
            if (item->isExpanded) {
                for (const auto& child : item->children) {
                    drawItem(child, level + 1);
                }
            }
        };
    
    for (const auto& item : m_RootItems) {
        drawItem(item, 0);
    }
}

void UISceneHierarchy::DrawItem(UIRenderer* renderer, const std::shared_ptr<SceneHierarchyItem>& item, float y, int level) {
    const auto bounds = GetBounds();
    const float x = bounds.x + GetItemIndentation(level);
    const float width = bounds.z - x;
    const glm::vec4 itemBounds(bounds.x, y, bounds.z, y + m_Style.itemHeight);
    
    // Draw custom background if item is highlighted from search
    if (item->isHighlighted) {
        renderer->DrawRect(itemBounds, m_Style.searchHighlightColor);
    }
    
    // Draw selection background
    if (item->isSelected) {
        renderer->DrawRect(itemBounds, m_Style.selectedColor);
    }
    
    // Draw expand/collapse icon
    if (!item->children.empty()) {
        const float iconX = x - m_Style.iconSize - m_Style.iconPadding;
        const float iconY = y + (m_Style.itemHeight - m_Style.iconSize) * 0.5f;
        
        // TODO: Draw proper expand/collapse icon
        renderer->DrawRect(
            glm::vec4(iconX, iconY, m_Style.iconSize, m_Style.iconSize),
            item->isExpanded ? m_Style.itemColor : m_Style.itemColor * 0.8f
        );
    }
    
    // Draw item icon
    if (!item->icon.empty()) {
        const float iconX = x;
        const float iconY = y + (m_Style.itemHeight - m_Style.iconSize) * 0.5f;
        
        // TODO: Draw actual icon from texture
        renderer->DrawRect(
            glm::vec4(iconX, iconY, m_Style.iconSize, m_Style.iconSize),
            m_Style.itemColor
        );
    }
    
    // Draw item name or renaming text input
    const float textX = x + m_Style.iconSize + m_Style.textPadding;
    const float textY = y + m_Style.itemHeight * 0.5f;
    
    if (item->isBeingRenamed) {
        renderer->DrawRect(
            glm::vec4(textX - 2.0f, y + 2.0f, width - textX + 2.0f, m_Style.itemHeight - 4.0f),
            m_Style.renamingBackgroundColor
        );
        
        renderer->DrawText(
            m_RenamingText + "|",
            glm::vec2(textX, textY),
            m_Style.fontSize,
            m_Style.itemColor,
            TextAlignment::Left,
            TextBaseline::Middle
        );
    } else {
        renderer->DrawText(
            item->name,
            glm::vec2(textX, textY),
            m_Style.fontSize,
            m_Style.itemColor,
            TextAlignment::Left,
            TextBaseline::Middle
        );
    }
    
    // Draw custom properties in columns
    DrawItemProperties(renderer, item, itemBounds);
    
    // Apply custom item renderer if available
    if (m_ItemRenderer) {
        m_ItemRenderer(renderer, item, itemBounds);
    }
    
    // Apply custom item decorator if available
    if (m_ItemDecorator) {
        m_ItemDecorator(renderer, item, itemBounds);
    }
}

void UISceneHierarchy::DrawDragIndicator(UIRenderer* renderer) {
    if (!m_DropTarget || !m_DraggedItem) return;
    
    // Draw drop target indicator
    const auto bounds = GetBounds();
    const float y = bounds.y + m_VisibleItems.size() * m_Style.itemHeight;
    
    renderer->DrawRect(
        glm::vec4(bounds.x, y - 2.0f, bounds.z, 4.0f),
        m_Style.dropTargetColor
    );
}

std::shared_ptr<SceneHierarchyItem> UISceneHierarchy::FindItemAtPosition(const glm::vec2& position) const {
    const float y = position.y + m_ScrollOffset - GetBounds().y;
    const int index = static_cast<int>(y / m_Style.itemHeight);
    
    if (index >= 0 && index < m_VisibleItems.size()) {
        return m_VisibleItems[index];
    }
    
    return nullptr;
}

bool UISceneHierarchy::IsItemVisible(const std::shared_ptr<SceneHierarchyItem>& item) const {
    if (!item->isVisible) return false;
    
    auto parent = item->parent.lock();
    while (parent) {
        if (!parent->isVisible || !parent->isExpanded) return false;
        parent = parent->parent.lock();
    }
    
    return true;
}

float UISceneHierarchy::GetItemIndentation(int level) const {
    return m_Style.indentWidth * level + m_Style.padding;
}

void UISceneHierarchy::UpdateSelection(const std::shared_ptr<SceneHierarchyItem>& item, bool addToSelection) {
    if (!addToSelection) {
        ClearSelection();
    }
    
    item->isSelected = true;
    m_SelectedItems.push_back(item);
}

void UISceneHierarchy::NotifySelectionChanged() {
    if (m_OnSelectionChanged) {
        m_OnSelectionChanged(m_SelectedItems);
    }
}

std::shared_ptr<UIAnimationState> UISceneHierarchy::CreateDragActiveState() {
    auto state = std::make_shared<UIAnimationState>("DragActive");
    auto sequence = std::make_shared<UIAnimationSequence>();
    
    // Add drag state animations
    auto fadeIn = UIAnimationTemplates::CreateFade(0.0f, 1.0f, 0.2f);
    auto scaleUp = UIAnimationTemplates::CreateScale(
        glm::vec2(1.0f), glm::vec2(1.02f), 0.2f);
    
    sequence->AddClip(fadeIn, 0.0f);
    sequence->AddClip(scaleUp, 0.0f);
    
    state->AddSequence(sequence);
    return state;
}

void UISceneHierarchy::SetSearchText(const std::string& text) {
    if (m_SearchText != text) {
        m_SearchText = text;
        UpdateSearch();
    }
}

void UISceneHierarchy::SetFilter(std::function<bool(const std::shared_ptr<SceneHierarchyItem>&)> filter) {
    m_Filter = filter;
    UpdateSearch();
}

void UISceneHierarchy::AddColumn(const SceneHierarchyColumn& column) {
    m_Columns.push_back(column);
    UpdateLayout();
}

void UISceneHierarchy::RemoveColumn(const std::string& columnName) {
    auto it = std::find_if(m_Columns.begin(), m_Columns.end(),
        [&](const SceneHierarchyColumn& col) { return col.name == columnName; });
    if (it != m_Columns.end()) {
        m_Columns.erase(it);
        UpdateLayout();
    }
}

void UISceneHierarchy::SetColumnWidth(const std::string& columnName, float width) {
    auto it = std::find_if(m_Columns.begin(), m_Columns.end(),
        [&](const SceneHierarchyColumn& col) { return col.name == columnName; });
    if (it != m_Columns.end()) {
        it->width = std::max(width, m_Style.columnMinWidth);
        UpdateLayout();
    }
}

void UISceneHierarchy::SetContextMenuCallback(std::function<void(const std::vector<std::shared_ptr<SceneHierarchyItem>>&, const glm::vec2&)> callback) {
    m_ContextMenuCallback = callback;
}

void UISceneHierarchy::Cut() {
    Copy();
    Delete();
}

void UISceneHierarchy::Copy() {
    m_ClipboardItems = m_SelectedItems;
}

void UISceneHierarchy::Paste() {
    if (m_ClipboardItems.empty()) return;
    
    BatchBegin();
    for (const auto& item : m_ClipboardItems) {
        // Create a deep copy of the item and its children
        auto copy = std::make_shared<SceneHierarchyItem>(*item);
        copy->name += " (Copy)";
        AddItem(copy, m_SelectedItems.empty() ? nullptr : m_SelectedItems.back());
    }
    BatchEnd();
}

void UISceneHierarchy::Duplicate() {
    Copy();
    Paste();
}

void UISceneHierarchy::Delete() {
    if (m_SelectedItems.empty()) return;
    
    BatchBegin();
    for (const auto& item : m_SelectedItems) {
        RemoveItem(item);
    }
    BatchEnd();
}

void UISceneHierarchy::StartRenaming(const std::shared_ptr<SceneHierarchyItem>& item) {
    if (!item) return;
    
    m_RenamingItem = item;
    m_RenamingText = item->name;
    item->isBeingRenamed = true;
}

void UISceneHierarchy::EndRenaming(bool accept) {
    if (!m_RenamingItem) return;
    
    if (accept && !m_RenamingText.empty()) {
        m_RenamingItem->name = m_RenamingText;
    }
    
    m_RenamingItem->isBeingRenamed = false;
    m_RenamingItem = nullptr;
    m_RenamingText.clear();
}

void UISceneHierarchy::SetItemProperty(const std::shared_ptr<SceneHierarchyItem>& item, const std::string& property, const std::string& value) {
    if (!item) return;
    item->properties[property] = value;
}

void UISceneHierarchy::BatchBegin() {
    m_IsBatchOperationActive = true;
    m_BatchOperations.clear();
}

void UISceneHierarchy::BatchEnd() {
    if (!m_IsBatchOperationActive) return;
    
    ApplyBatchOperations();
    m_IsBatchOperationActive = false;
    m_BatchOperations.clear();
}

void UISceneHierarchy::BatchSetProperty(const std::string& property, const std::string& value) {
    if (!m_IsBatchOperationActive) return;
    m_BatchOperations[property] = value;
}

void UISceneHierarchy::SetItemRenderer(std::function<void(UIRenderer*, const std::shared_ptr<SceneHierarchyItem>&, const glm::vec4&)> renderer) {
    m_ItemRenderer = renderer;
}

void UISceneHierarchy::SetItemDecorator(std::function<void(UIRenderer*, const std::shared_ptr<SceneHierarchyItem>&, const glm::vec4&)> decorator) {
    m_ItemDecorator = decorator;
}

void UISceneHierarchy::HandleContextMenu(const glm::vec2& position) {
    if (m_ContextMenuCallback) {
        m_ContextMenuCallback(m_SelectedItems, position);
    }
}

void UISceneHierarchy::HandleSearchInput(const std::string& text) {
    SetSearchText(text);
}

void UISceneHierarchy::HandleColumnResize(const std::string& columnName, float delta) {
    auto it = std::find_if(m_Columns.begin(), m_Columns.end(),
        [&](const SceneHierarchyColumn& col) { return col.name == columnName; });
    if (it != m_Columns.end() && it->resizable) {
        SetColumnWidth(columnName, it->width + delta);
    }
}

void UISceneHierarchy::HandleColumnSort(const std::string& columnName) {
    if (m_SortColumn == columnName) {
        m_SortAscending = !m_SortAscending;
    } else {
        m_SortColumn = columnName;
        m_SortAscending = true;
    }
    
    UpdateSort();
}

void UISceneHierarchy::HandleRenaming(const KeyEvent& event) {
    if (!m_RenamingItem) return;
    
    switch (event.key) {
        case Key::Enter:
            EndRenaming(true);
            break;
            
        case Key::Escape:
            EndRenaming(false);
            break;
            
        case Key::Backspace:
            if (!m_RenamingText.empty()) {
                m_RenamingText.pop_back();
            }
            break;
            
        default:
            if (event.character >= 32 && event.character < 127) {
                m_RenamingText += static_cast<char>(event.character);
            }
            break;
    }
}

void UISceneHierarchy::UpdateSearch() {
    m_FilteredItems.clear();
    
    std::function<void(const std::shared_ptr<SceneHierarchyItem>&)> searchItem =
        [&](const std::shared_ptr<SceneHierarchyItem>& item) {
            bool matches = true;
            
            // Apply search filter
            if (!m_SearchText.empty()) {
                matches = item->name.find(m_SearchText) != std::string::npos;
            }
            
            // Apply custom filter
            if (matches && m_Filter) {
                matches = m_Filter(item);
            }
            
            item->isHighlighted = matches && !m_SearchText.empty();
            
            if (matches) {
                m_FilteredItems.push_back(item);
            }
            
            // Always search children to maintain hierarchy
            for (const auto& child : item->children) {
                searchItem(child);
            }
        };
    
    for (const auto& item : m_RootItems) {
        searchItem(item);
    }
    
    UpdateLayout();
}

void UISceneHierarchy::UpdateSort() {
    if (m_SortColumn.empty()) return;
    
    auto comparator = [&](const std::shared_ptr<SceneHierarchyItem>& a, const std::shared_ptr<SceneHierarchyItem>& b) {
        auto it = std::find_if(m_Columns.begin(), m_Columns.end(),
            [&](const SceneHierarchyColumn& col) { return col.name == m_SortColumn; });
        
        if (it != m_Columns.end() && it->getter) {
            std::string valueA = it->getter(a);
            std::string valueB = it->getter(b);
            return m_SortAscending ? valueA < valueB : valueA > valueB;
        }
        
        return false;
    };
    
    std::function<void(std::vector<std::shared_ptr<SceneHierarchyItem>>&)> sortItems =
        [&](std::vector<std::shared_ptr<SceneHierarchyItem>>& items) {
            std::sort(items.begin(), items.end(), comparator);
            for (auto& item : items) {
                sortItems(item->children);
            }
        };
    
    sortItems(m_RootItems);
    UpdateLayout();
}

void UISceneHierarchy::UpdateAnimations(float deltaTime) {
    const float animationSpeed = 4.0f;
    
    std::function<void(const std::shared_ptr<SceneHierarchyItem>&)> updateItem =
        [&](const std::shared_ptr<SceneHierarchyItem>& item) {
            // Update expansion animation
            if (item->isExpanded && item->animationProgress < 1.0f) {
                item->animationProgress = std::min(1.0f, item->animationProgress + deltaTime * animationSpeed);
            } else if (!item->isExpanded && item->animationProgress > 0.0f) {
                item->animationProgress = std::max(0.0f, item->animationProgress - deltaTime * animationSpeed);
            }
            
            for (const auto& child : item->children) {
                updateItem(child);
            }
        };
    
    for (const auto& item : m_RootItems) {
        updateItem(item);
    }
}

void UISceneHierarchy::ApplyBatchOperations() {
    for (const auto& item : m_SelectedItems) {
        for (const auto& [property, value] : m_BatchOperations) {
            SetItemProperty(item, property, value);
        }
    }
}

void UISceneHierarchy::DrawColumns(UIRenderer* renderer) {
    const auto bounds = GetBounds();
    float x = bounds.x;
    
    for (const auto& column : m_Columns) {
        glm::vec4 columnBounds(x, bounds.y, x + column.width, bounds.w);
        
        // Draw column separator
        renderer->DrawLine(
            glm::vec2(x, bounds.y),
            glm::vec2(x, bounds.w),
            m_Style.columnSeparatorColor
        );
        
        x += column.width;
    }
}

void UISceneHierarchy::DrawSearchBar(UIRenderer* renderer) {
    const auto bounds = GetBounds();
    const glm::vec4 searchBarBounds(
        bounds.x,
        bounds.y - m_Style.searchBarHeight,
        bounds.z,
        bounds.y
    );
    
    // Draw search bar background
    renderer->DrawRect(searchBarBounds, m_Style.backgroundColor * 1.1f);
    
    // Draw search text
    renderer->DrawText(
        m_SearchText.empty() ? "Search..." : m_SearchText,
        glm::vec2(searchBarBounds.x + m_Style.padding, searchBarBounds.y + m_Style.searchBarHeight * 0.5f),
        m_Style.fontSize,
        m_SearchText.empty() ? m_Style.itemColor * 0.5f : m_Style.itemColor,
        TextAlignment::Left,
        TextBaseline::Middle
    );
}

void UISceneHierarchy::DrawColumnHeaders(UIRenderer* renderer) {
    const auto bounds = GetBounds();
    float x = bounds.x;
    
    for (const auto& column : m_Columns) {
        glm::vec4 headerBounds(
            x,
            bounds.y - m_Style.columnHeaderHeight,
            x + column.width,
            bounds.y
        );
        
        // Draw header background
        renderer->DrawRect(
            headerBounds,
            column.name == m_SortColumn ? m_Style.columnHeaderHoverColor : m_Style.columnHeaderColor
        );
        
        // Draw header text
        renderer->DrawText(
            column.name,
            glm::vec2(headerBounds.x + m_Style.padding, headerBounds.y + m_Style.columnHeaderHeight * 0.5f),
            m_Style.fontSize,
            m_Style.itemColor,
            TextAlignment::Left,
            TextBaseline::Middle
        );
        
        // Draw sort indicator
        if (column.name == m_SortColumn) {
            // TODO: Draw proper sort indicator
            const float indicatorSize = 8.0f;
            const float indicatorX = headerBounds.z - indicatorSize - m_Style.padding;
            const float indicatorY = headerBounds.y + (m_Style.columnHeaderHeight - indicatorSize) * 0.5f;
            
            renderer->DrawRect(
                glm::vec4(indicatorX, indicatorY, indicatorSize, indicatorSize),
                m_Style.itemColor
            );
        }
        
        x += column.width;
    }
}

void UISceneHierarchy::DrawItemProperties(UIRenderer* renderer, const std::shared_ptr<SceneHierarchyItem>& item, const glm::vec4& bounds) {
    float x = bounds.x;
    
    for (const auto& column : m_Columns) {
        if (column.getter) {
            std::string value = column.getter(item);
            
            renderer->DrawText(
                value,
                glm::vec2(x + m_Style.padding, bounds.y + m_Style.itemHeight * 0.5f),
                m_Style.fontSize,
                m_Style.itemColor,
                TextAlignment::Left,
                TextBaseline::Middle
            );
        }
        
        x += column.width;
    }
}

void UISceneHierarchy::OnSelectionChanged(const std::vector<SceneNode*>& selection) {
    // Update base class
    UITreeView::OnSelectionChanged(selection);
    
    // Update property panel
    if (selection.size() == 1) {
        SceneNode* node = selection[0];
        m_PropertyPanel->SetTarget(node, "SceneNode");
        
        // Register common properties
        PropertyMetadata nameMeta{
            "Name",
            "Node name",
            "General",
            false,
            false,
            "",
            PropertyValue(std::string())
        };
        m_PropertyPanel->RegisterProperty("name", nameMeta);
        
        PropertyMetadata positionMeta{
            "Position",
            "Node position in world space",
            "Transform",
            false,
            false,
            "units",
            PropertyValue(glm::vec3(0.0f))
        };
        m_PropertyPanel->RegisterProperty("position", positionMeta);
        
        PropertyMetadata rotationMeta{
            "Rotation",
            "Node rotation in degrees",
            "Transform",
            false,
            false,
            "degrees",
            PropertyValue(glm::vec3(0.0f))
        };
        m_PropertyPanel->RegisterProperty("rotation", rotationMeta);
        
        PropertyMetadata scaleMeta{
            "Scale",
            "Node scale",
            "Transform",
            false,
            false,
            "",
            PropertyValue(glm::vec3(1.0f))
        };
        m_PropertyPanel->RegisterProperty("scale", scaleMeta);
        
        // Register node-specific properties
        node->RegisterProperties(m_PropertyPanel.get());
        
        // Add validators
        auto nameValidator = std::make_shared<UIStringLengthValidator>(1, 64);
        m_PropertyPanel->RegisterValidator("name", nameValidator);
        
        auto transformValidator = std::make_shared<UIRangeValidator>(-10000.0, 10000.0);
        m_PropertyPanel->RegisterValidator("position", transformValidator);
        m_PropertyPanel->RegisterValidator("rotation", transformValidator);
        m_PropertyPanel->RegisterValidator("scale", transformValidator);
    } else {
        m_PropertyPanel->ClearTarget();
    }
}

void UISceneHierarchy::OnPropertyChanged(const PropertyChangeEvent& event) {
    if (GetSelectedNodes().size() != 1) return;
    
    SceneNode* node = GetSelectedNodes()[0];
    if (!node) return;
    
    if (event.propertyName == "name") {
        if (auto* newName = std::get_if<std::string>(&event.newValue)) {
            node->SetName(*newName);
            RefreshNode(node);
        }
    }
    else if (event.propertyName == "position") {
        if (auto* newPos = std::get_if<glm::vec3>(&event.newValue)) {
            node->SetPosition(*newPos);
        }
    }
    else if (event.propertyName == "rotation") {
        if (auto* newRot = std::get_if<glm::vec3>(&event.newValue)) {
            node->SetRotation(*newRot);
        }
    }
    else if (event.propertyName == "scale") {
        if (auto* newScale = std::get_if<glm::vec3>(&event.newValue)) {
            node->SetScale(*newScale);
        }
    }
    else {
        // Handle custom properties
        node->OnPropertyChanged(event);
    }
}

}} // namespace VGE::UI 