#include "U

#include "Runtime/Core/Public/Core/UICore.h"IComboBox.h"
#include "Graphics/RenderCommand.h"
#include "Runtime/Core/Public/Input.h"
#include "Runtime/Core/Public/Time.h"

namespace VGE {

UIComboBox::UIComboBox() : UIComboBoxBase() {
    m_Name = "ComboBox";
    m_Renderer = std::make_unique<UIComboBoxRenderer>(this);
    m_Animator = std::make_unique<UIComboBoxAnimator>(this);
    m_Keyboard = std::make_unique<UIComboBoxKeyboard>(this);
}

void UIComboBox::Initialize() {
    UIWidget::Initialize();
    
    // Initialize components
    m_Renderer->Initialize();
    m_Animator->Initialize();

    // Create search input
    m_SearchInput = CreateRef<UITextInput>();
    m_SearchInput->Initialize();
    m_SearchInput->SetPlaceholder(m_Properties.SearchPlaceholder);
    
    // Create scroll view for dropdown
    m_ScrollView = CreateRef<UIScrollView>();
    m_ScrollView->Initialize();

    // Initialize animations
    m_DropdownAnimation = 0.0f;
    m_DropdownTargetAnimation = 0.0f;
    m_HoverAnimation = 0.0f;
    m_ItemHoverAnimations.resize(m_Items.size(), 0.0f);
}

void UIComboBox::Update(float deltaTime) {
    if (!m_IsVisible)
        return;

    UIWidget::Update(deltaTime);
    
    // Update components
    m_Animator->Update(deltaTime);
    
    // Update search filtering if needed
    if (IsDropdownOpen() && HasSearchTextChanged()) {
        UpdateFiltering();
    }
}

void UIComboBox::Render() {
    if (!m_IsVisible)
        return;

    m_Renderer->Render();
}

void UIComboBox::UpdateAnimations(float deltaTime) {
    // Update dropdown animation
    if (m_DropdownAnimation != m_DropdownTargetAnimation) {
        m_DropdownAnimation = glm::mix(m_DropdownAnimation, m_DropdownTargetAnimation, 
            deltaTime * m_Properties.AnimationSpeed);
        
        if (std::abs(m_DropdownAnimation - m_DropdownTargetAnimation) < 0.001f) {
            m_DropdownAnimation = m_DropdownTargetAnimation;
        }
    }

    // Update hover animations
    if (IsHovered()) {
        m_HoverAnimation = glm::min(1.0f, m_HoverAnimation + deltaTime * m_Properties.AnimationSpeed);
    } else {
        m_HoverAnimation = glm::max(0.0f, m_HoverAnimation - deltaTime * m_Properties.AnimationSpeed);
    }

    // Update item hover animations
    for (size_t i = 0; i < m_ItemHoverAnimations.size(); i++) {
        bool isHovered = m_HoveredItemIndex == i;
        float& animation = m_ItemHoverAnimations[i];
        
        if (isHovered) {
            animation = glm::min(1.0f, animation + deltaTime * m_Properties.AnimationSpeed);
        } else {
            animation = glm::max(0.0f, animation - deltaTime * m_Properties.AnimationSpeed);
        }
    }
}

void UIComboBox::RenderBackground() {
    const auto& style = m_Properties.VisualStyle;
    
    // Determine which gradient to use based on state
    const ComboBoxGradient* gradient = &style.NormalGradient;
    if (m_HoverAnimation > 0.01f) {
        gradient = &style.HoverGradient;
    }
    
    RenderGradient(m_Position, m_Size, *gradient);
}

void UIComboBox::RenderBorder() {
    const auto& style = m_Properties.VisualStyle;
    const auto& border = IsFocused() ? style.FocusBorder : style.Border;
    
    // Render rounded rectangle border
    RenderCommand::DrawRoundedRectOutline(
        m_Position, 
        m_Size, 
        border.Color,
        border.Width,
        border.Radius
    );
}

void UIComboBox::RenderDropShadow() {
    const auto& style = m_Properties.VisualStyle;
    const auto& shadow = m_HoverAnimation > 0.01f ? style.HoverShadow : style.DropShadow;
    
    // Render drop shadow with blur
    RenderCommand::DrawShadow(
        m_Position + shadow.Offset,
        m_Size,
        shadow.Color,
        shadow.Blur,
        shadow.Spread
    );
}

void UIComboBox::RenderGradient(const glm::vec2& position, const glm::vec2& size, const ComboBoxGradient& gradient) {
    // Convert angle to radians
    float angleRad = glm::radians(gradient.Angle);
    glm::vec2 direction(std::cos(angleRad), std::sin(angleRad));
    
    // Calculate gradient vector
    float length = std::abs(size.x * direction.x) + std::abs(size.y * direction.y);
    glm::vec2 gradientVector = direction * length;
    
    // Render gradient
    RenderCommand::DrawGradientRect(
        position,
        size,
        gradient.StartColor,
        gradient.EndColor,
        gradientVector
    );
}

void UIComboBox::RenderSelectedItems() {
    if (m_Properties.EnableMultiSelect) {
        // Render multiple selected items as tags
        float xOffset = 5.0f;
        float yOffset = (m_Size.y - 20.0f) * 0.5f;
        
        for (size_t i = 0; i < m_Items.size(); i++) {
            if (!m_SelectedItems[i]) continue;
            
            const auto& item = m_Items[i];
            float tagWidth = RenderCommand::GetTextWidth(item.Text) + 20.0f;
            
            if (xOffset + tagWidth > m_Size.x - 30.0f) {
                // Show overflow indicator
                RenderCommand::DrawText("...", m_Position + glm::vec2(m_Size.x - 40.0f, yOffset), glm::vec4(0.7f));
                break;
            }

            // Render tag background
            glm::vec2 tagPos = m_Position + glm::vec2(xOffset, yOffset);
            RenderCommand::DrawRect(tagPos, glm::vec2(tagWidth, 20.0f), glm::vec4(0.3f, 0.5f, 0.7f, 1.0f));
            
            // Render tag text
            RenderCommand::DrawText(item.Text, tagPos + glm::vec2(5.0f, 2.0f), glm::vec4(1.0f));
            
            xOffset += tagWidth + 5.0f;
        }
    } else {
        // Render single selected item or placeholder
        const ComboBoxItem* selectedItem = GetSelectedItem();
        std::string displayText = selectedItem ? selectedItem->Text : m_Properties.Placeholder;
        glm::vec4 textColor = selectedItem ? glm::vec4(1.0f) : glm::vec4(0.7f);
        
        if (selectedItem && !selectedItem->Description.empty()) {
            // Render main text
            RenderCommand::DrawText(displayText, m_Position + glm::vec2(5.0f, 5.0f), textColor);
            
            // Render description
            float descriptionY = m_Position.y + m_Size.y - 20.0f;
            RenderCommand::DrawText(selectedItem->Description, 
                m_Position + glm::vec2(5.0f, descriptionY),
                glm::vec4(0.7f),
                0.8f); // Smaller font size for description
        } else {
            // Center text vertically if no description
            float textY = (m_Size.y - RenderCommand::GetTextHeight()) * 0.5f;
            RenderCommand::DrawText(displayText, m_Position + glm::vec2(5.0f, textY), textColor);
        }
    }
}

void UIComboBox::RenderDropdown() {
    glm::vec2 dropdownPos = m_Position + glm::vec2(0.0f, m_Size.y);
    float dropdownHeight = CalculateDropdownHeight() * m_DropdownAnimation;
    
    // Render dropdown background with shadow
    RenderCommand::DrawShadowRect(dropdownPos, glm::vec2(m_Size.x, dropdownHeight), 
        glm::vec4(0.15f, 0.15f, 0.15f, 1.0f), 10.0f);
    
    // Early return if animation is still starting
    if (m_DropdownAnimation < 0.1f) return;

    // Render search box if enabled
    float contentStartY = dropdownPos.y;
    if (m_Properties.EnableSearch) {
        m_SearchInput->SetPosition(dropdownPos + glm::vec2(5.0f, 5.0f));
        m_SearchInput->SetSize(glm::vec2(m_Size.x - 10.0f, 20.0f));
        m_SearchInput->Render();
        contentStartY += 30.0f;
    }

    // Setup scroll view
    m_ScrollView->SetPosition(glm::vec2(dropdownPos.x, contentStartY));
    m_ScrollView->SetSize(glm::vec2(m_Size.x, dropdownHeight - (contentStartY - dropdownPos.y)));

    // Render items within scroll view
    if (m_Properties.EnableCategories) {
        RenderCategorizedItems();
    } else {
        RenderFlatItems();
    }
}

void UIComboBox::RenderCategorizedItems() {
    float yOffset = 0.0f;
    
    for (const auto& category : m_Categories) {
        // Skip empty categories
        if (m_ItemsByCategory[category].empty()) continue;

        // Render category header
        RenderCategoryHeader(category, glm::vec2(0.0f, yOffset));
        yOffset += m_Properties.CategoryHeaderHeight;

        // Render items in category
        for (const ComboBoxItem* item : m_ItemsByCategory[category]) {
            if (!IsItemVisible(*item)) continue;

            size_t itemIndex = GetItemIndex(item);
            bool isSelected = IsItemSelected(itemIndex);
            bool isHovered = itemIndex == m_HoveredIndex;
            
            RenderItem(*item, glm::vec2(0.0f, yOffset), isSelected, isHovered);
            yOffset += m_Properties.ItemHeight;
        }
    }
}

void UIComboBox::RenderFlatItems() {
    float yOffset = 0.0f;
    
    for (size_t i = 0; i < m_FilteredItems.size(); i++) {
        const ComboBoxItem* item = m_FilteredItems[i];
        bool isSelected = IsItemSelected(GetItemIndex(item));
        bool isHovered = i == m_HoveredIndex;
        
        RenderItem(*item, glm::vec2(0.0f, yOffset), isSelected, isHovered);
        yOffset += m_Properties.ItemHeight;
    }
}

void UIComboBox::RenderCategoryHeader(const std::string& category, const glm::vec2& position) {
    // Render category background
    RenderCommand::DrawRect(position, 
        glm::vec2(m_Size.x, m_Properties.CategoryHeaderHeight),
        glm::vec4(0.25f, 0.25f, 0.25f, 1.0f));
    
    // Render category text
    float textY = position.y + (m_Properties.CategoryHeaderHeight - RenderCommand::GetTextHeight()) * 0.5f;
    RenderCommand::DrawText(category, 
        glm::vec2(position.x + 5.0f, textY),
        glm::vec4(0.8f),
        1.0f,
        true); // Bold text for categories
}

void UIComboBox::RenderItem(const ComboBoxItem& item, const glm::vec2& position, bool isSelected, bool isHovered) {
    float itemHeight = m_Properties.ItemHeight;
    size_t itemIndex = GetItemIndex(&item);
    
    // Get animation value for this item
    float hoverValue = m_ItemAnimations[itemIndex].GetValue();
    
    // Calculate background color with gradient
    const auto& style = m_Properties.VisualStyle;
    ComboBoxGradient itemGradient = style.NormalGradient;
    
    if (hoverValue > 0.01f) {
        itemGradient = style.HoverGradient;
    }
    if (isSelected) {
        itemGradient = style.SelectedGradient;
    }
    
    // Render item background with gradient
    RenderGradient(
        position,
        glm::vec2(m_Size.x, itemHeight),
        itemGradient
    );

    float xOffset = 5.0f;

    // Render checkbox for multi-select
    if (m_Properties.EnableMultiSelect) {
        RenderCheckbox(
            position + glm::vec2(xOffset, (itemHeight - 16.0f) * 0.5f),
            isSelected,
            isHovered
        );
        xOffset += 20.0f;
    }

    // Render icon if present
    if (!item.IconName.empty()) {
        glm::vec2 iconPos = position + glm::vec2(xOffset, (itemHeight - m_Properties.IconSize) * 0.5f);
        RenderCommand::DrawIcon(
            item.IconName,
            iconPos,
            m_Properties.IconSize,
            item.IconTint
        );
        xOffset += m_Properties.IconSize + m_Properties.IconPadding;
    }

    // Text color based on enabled state
    glm::vec4 textColor = item.IsEnabled ? style.TextColor : style.DisabledTextColor;
    
    // Render main text
    RenderCommand::DrawText(
        item.Text,
        position + glm::vec2(xOffset, (itemHeight - 16.0f) * 0.5f),
        textColor
    );

    // Render description if enabled
    if (m_Properties.ShowDescription && !item.Description.empty()) {
        float descriptionOffset = RenderCommand::GetTextWidth(item.Text) + 10.0f;
        glm::vec4 descColor = glm::vec4(textColor.r, textColor.g, textColor.b, textColor.a * 0.7f);
        RenderCommand::DrawText(
            item.Description,
            position + glm::vec2(xOffset + descriptionOffset, (itemHeight - 16.0f) * 0.5f),
            descColor,
            0.8f
        );
    }
}

void UIComboBox::RenderCheckbox(const glm::vec2& position, bool checked, bool hovered) {
    // Render checkbox background
    glm::vec4 boxColor = hovered ? glm::vec4(0.3f, 0.3f, 0.3f, 1.0f) : glm::vec4(0.25f, 0.25f, 0.25f, 1.0f);
    RenderCommand::DrawRect(position, glm::vec2(16.0f), boxColor);
    
    // Render check mark if selected
    if (checked) {
        RenderCommand::DrawCheckMark(position + glm::vec2(3.0f), 10.0f, glm::vec4(1.0f));
    }
}

void UIComboBox::AddItem(const std::string& text, uint64_t id, void* userData) {
    ComboBoxItem item;
    item.Text = text;
    item.SearchText = text;
    std::transform(item.SearchText.begin(), item.SearchText.end(), item.SearchText.begin(), ::tolower);
    item.Id = id;
    item.UserData = userData;
    
    m_Items.push_back(item);
    m_SelectedItems.push_back(false);
    m_ItemAnimations.emplace_back();
    UpdateFiltering();
    UpdateCategories();
}

void UIComboBox::AddItem(const ComboBoxItem& item) {
    m_Items.push_back(item);
    m_SelectedItems.push_back(false);
    m_ItemAnimations.emplace_back();
    UpdateFiltering();
    UpdateCategories();
}

void UIComboBox::RemoveItem(size_t index) {
    if (index < m_Items.size()) {
        m_Items.erase(m_Items.begin() + index);
        m_SelectedItems.erase(m_SelectedItems.begin() + index);
        m_ItemAnimations.erase(m_ItemAnimations.begin() + index);
        UpdateFiltering();
        UpdateCategories();
    }
}

void UIComboBox::ClearItems() {
    m_Items.clear();
    m_SelectedItems.clear();
    m_ItemAnimations.clear();
    m_Categories.clear();
    m_ItemsByCategory.clear();
    m_SelectedIndex = (size_t)-1;
    m_HoveredItemIndex = (size_t)-1;
}

void UIComboBox::SetSelected(size_t index, bool selected) {
    if (index >= m_Items.size())
        return;

    if (m_Properties.EnableMultiSelect) {
        m_SelectedItems[index] = selected;
        NotifyMultiSelectionChanged();
    } else {
        m_SelectedIndex = selected ? index : (size_t)-1;
        NotifySelectionChanged();
    }
}

void UIComboBox::SetSelectedIds(const std::vector<uint64_t>& ids) {
    if (!m_Properties.EnableMultiSelect)
        return;

    // Clear all selections
    std::fill(m_SelectedItems.begin(), m_SelectedItems.end(), false);

    // Set new selections
    for (uint64_t id : ids) {
        for (size_t i = 0; i < m_Items.size(); i++) {
            if (m_Items[i].Id == id) {
                m_SelectedItems[i] = true;
                break;
            }
        }
    }

    NotifyMultiSelectionChanged();
}

std::vector<size_t> UIComboBox::GetSelectedIndices() const {
    std::vector<size_t> indices;
    if (m_Properties.EnableMultiSelect) {
        for (size_t i = 0; i < m_SelectedItems.size(); i++) {
            if (m_SelectedItems[i]) {
                indices.push_back(i);
            }
        }
    } else if (m_SelectedIndex != (size_t)-1) {
        indices.push_back(m_SelectedIndex);
    }
    return indices;
}

std::vector<uint64_t> UIComboBox::GetSelectedIds() const {
    std::vector<uint64_t> ids;
    if (m_Properties.EnableMultiSelect) {
        for (size_t i = 0; i < m_SelectedItems.size(); i++) {
            if (m_SelectedItems[i]) {
                ids.push_back(m_Items[i].Id);
            }
        }
    } else if (m_SelectedIndex != (size_t)-1) {
        ids.push_back(m_Items[m_SelectedIndex].Id);
    }
    return ids;
}

void UIComboBox::UpdateCategories() {
    if (!m_Properties.EnableCategories)
        return;

    m_Categories.clear();
    m_ItemsByCategory.clear();

    // Collect unique categories
    std::set<std::string> uniqueCategories;
    for (const auto& item : m_Items) {
        if (!item.Category.empty()) {
            uniqueCategories.insert(item.Category);
        }
    }

    // Sort categories
    m_Categories.assign(uniqueCategories.begin(), uniqueCategories.end());
    std::sort(m_Categories.begin(), m_Categories.end());

    // Group items by category
    for (const auto& item : m_Items) {
        std::string category = item.Category.empty() ? "Uncategorized" : item.Category;
        m_ItemsByCategory[category].push_back(&item);
    }
}

void UIComboBox::UpdateFiltering() {
    m_FilteredItems.clear();
    
    std::string searchLower = m_SearchText;
    std::transform(searchLower.begin(), searchLower.end(), searchLower.begin(), ::tolower);
    
    for (const auto& item : m_Items) {
        if (IsItemVisible(item)) {
            m_FilteredItems.push_back(&item);
        }
    }
}

bool UIComboBox::IsItemVisible(const ComboBoxItem& item) const {
    if (!item.IsEnabled)
        return false;

    if (m_SearchText.empty())
        return true;

    // Convert search text to lowercase for case-insensitive comparison
    std::string searchLower = m_SearchText;
    std::transform(searchLower.begin(), searchLower.end(), searchLower.begin(), ::tolower);

    // Perform fuzzy search
    return FuzzyMatch(item.SearchText, searchLower);
}

bool UIComboBox::FuzzyMatch(const std::string& text, const std::string& pattern) const {
    size_t textLen = text.length();
    size_t patternLen = pattern.length();
    
    if (patternLen == 0) return true;
    if (textLen == 0) return false;

    // Simple substring search for exact matches
    if (text.find(pattern) != std::string::npos)
        return true;

    // Fuzzy search
    size_t textIndex = 0;
    size_t patternIndex = 0;
    
    while (textIndex < textLen && patternIndex < patternLen) {
        if (text[textIndex] == pattern[patternIndex]) {
            patternIndex++;
        }
        textIndex++;
    }
    
    return patternIndex == patternLen;
}

size_t UIComboBox::GetItemIndex(const ComboBoxItem* item) const {
    for (size_t i = 0; i < m_Items.size(); i++) {
        if (&m_Items[i] == item) {
            return i;
        }
    }
    return (size_t)-1;
}

bool UIComboBox::IsItemSelected(size_t index) const {
    if (index >= m_Items.size())
        return false;

    return m_Properties.EnableMultiSelect ? m_SelectedItems[index] : (index == m_SelectedIndex);
}

void UIComboBox::NotifySelectionChanged() {
    if (m_OnSelectionChanged && m_SelectedIndex < m_Items.size()) {
        m_OnSelectionChanged(m_Items[m_SelectedIndex]);
    }
}

void UIComboBox::NotifyMultiSelectionChanged() {
    if (!m_OnMultiSelectionChanged)
        return;

    std::vector<ComboBoxItem> selectedItems;
    for (size_t i = 0; i < m_SelectedItems.size(); i++) {
        if (m_SelectedItems[i]) {
            selectedItems.push_back(m_Items[i]);
        }
    }
    m_OnMultiSelectionChanged(selectedItems);
}

void UIComboBox::OnMouseDown(const glm::vec2& position) {
    if (!m_IsVisible)
        return;

    if (IsPointInside(position)) {
        ToggleDropdown();
    } else if (m_IsDropdownOpen) {
        // Check if clicked inside dropdown
        glm::vec2 dropdownPos = m_Position + glm::vec2(0.0f, m_Size.y);
        glm::vec2 dropdownSize = glm::vec2(m_Size.x, CalculateDropdownHeight());
        
        if (IsPointInRect(position, dropdownPos, dropdownSize)) {
            // Handle item selection
            float yOffset = m_Properties.EnableSearch ? 30.0f : 0.0f;
            float relativeY = position.y - (dropdownPos.y + yOffset);
            size_t clickedIndex = (size_t)(relativeY / m_Properties.ItemHeight);
            
            if (clickedIndex < m_FilteredItems.size()) {
                SetSelectedIndex(clickedIndex);
                if (m_Properties.AutoClose) {
                    CloseDropdown();
                }
            }
        } else {
            CloseDropdown();
        }
    }
}

void UIComboBox::OnMouseMove(const glm::vec2& position) {
    UIWidget::OnMouseMove(position);
    
    if (IsDropdownOpen()) {
        // Update hovered item index based on mouse position
        m_HoveredItemIndex = m_Renderer->GetItemIndexAtPosition(position);
    }
}

void UIComboBox::OnMouseDown(const glm::vec2& position, MouseButton button) {
    UIWidget::OnMouseDown(position, button);
    
    if (button != MouseButton::Left)
        return;
        
    if (!IsDropdownOpen()) {
        OpenDropdown();
    } else {
        if (m_HoveredItemIndex != (size_t)-1) {
            SetSelected(m_HoveredItemIndex, !IsSelected(m_HoveredItemIndex));
            if (!GetProperties().EnableMultiSelect) {
                CloseDropdown();
            }
        }
    }
}

void UIComboBox::OnMouseUp(const glm::vec2& position, MouseButton button) {
    UIWidget::OnMouseUp(position, button);
}

void UIComboBox::OnKeyDown(int key, bool ctrl, bool shift, bool alt) {
    UIWidget::OnKeyDown(key, ctrl, shift, alt);
    m_Keyboard->HandleKeyDown(key, ctrl, shift, alt);
}

void UIComboBox::OnKeyUp(int key, bool ctrl, bool shift, bool alt) {
    UIWidget::OnKeyUp(key, ctrl, shift, alt);
}

void UIComboBox::OnFocusGained() {
    UIWidget::OnFocusGained();
}

void UIComboBox::OnFocusLost() {
    UIWidget::OnFocusLost();
    if (GetProperties().AutoClose) {
        CloseDropdown();
    }
}

void UIComboBox::HandleDropdownStateChange(bool isOpen) {
    if (isOpen) {
        m_Animator->StartDropdownOpenAnimation();
    } else {
        m_Animator->StartDropdownCloseAnimation();
        if (GetProperties().ClearSearchOnClose) {
            ClearSearch();
        }
    }
}

void UIComboBox::HandleSelectionChange(size_t index) {
    NotifySelectionChanged();
}

void UIComboBox::HandleSearchTextChange(const std::string& searchText) {
    UpdateFiltering();
}

void UIComboBox::ToggleDropdown() {
    m_IsDropdownOpen = !m_IsDropdownOpen;
    if (m_IsDropdownOpen) {
        if (m_Properties.EnableSearch) {
            m_SearchInput->SetText("");
            m_SearchInput->Focus();
        }
        UpdateFiltering();
        m_DropdownAnimation.Update(0.0f, 1.0f, m_Properties.AnimationSpeed, EaseType::EaseOutBack);
    } else {
        m_DropdownAnimation.Update(1.0f, 0.0f, m_Properties.AnimationSpeed, EaseType::EaseOutBack);
    }
}

float UIComboBox::CalculateDropdownHeight() const {
    float height = m_Properties.EnableSearch ? 30.0f : 0.0f;
    height += m_FilteredItems.size() * m_Properties.ItemHeight;
    return std::min(height, m_Properties.MaxDropdownHeight);
}

void UIComboBox::HandleKeyboardNavigation(int keyCode) {
    if (!m_Properties.EnableKeyboardNavigation)
        return;

    switch (keyCode) {
        case VK_UP:
            SelectPreviousItem();
            break;
        case VK_DOWN:
            SelectNextItem();
            break;
        case VK_HOME:
            SelectFirstItem();
            break;
        case VK_END:
            SelectLastItem();
            break;
        case VK_RETURN:
            if (!m_IsDropdownOpen) {
                OpenDropdown();
            } else if (m_HoveredItemIndex < m_Items.size()) {
                SetSelected(m_HoveredItemIndex, !IsSelected(m_HoveredItemIndex));
                if (!m_Properties.EnableMultiSelect) {
                    CloseDropdown();
                }
            }
            break;
        case VK_ESCAPE:
            if (m_IsDropdownOpen) {
                CloseDropdown();
            }
            break;
    }
}

void UIComboBox::SelectNextItem() {
    if (m_Items.empty()) return;
    size_t nextIndex = GetNextSelectableIndex(m_HoveredItemIndex);
    if (nextIndex != m_HoveredItemIndex) {
        m_HoveredItemIndex = nextIndex;
        m_ScrollView->ScrollToItem(nextIndex);
    }
}

void UIComboBox::SelectPreviousItem() {
    if (m_Items.empty()) return;
    size_t prevIndex = GetNextSelectableIndex(m_HoveredItemIndex, false);
    if (prevIndex != m_HoveredItemIndex) {
        m_HoveredItemIndex = prevIndex;
        m_ScrollView->ScrollToItem(prevIndex);
    }
}

void UIComboBox::SelectFirstItem() {
    if (m_Items.empty()) return;
    m_HoveredItemIndex = GetNextSelectableIndex(0);
    m_ScrollView->ScrollToItem(m_HoveredItemIndex);
}

void UIComboBox::SelectLastItem() {
    if (m_Items.empty()) return;
    m_HoveredItemIndex = GetNextSelectableIndex(m_Items.size() - 1, false);
    m_ScrollView->ScrollToItem(m_HoveredItemIndex);
}

size_t UIComboBox::GetNextSelectableIndex(size_t currentIndex, bool forward) {
    if (m_Items.empty()) return currentIndex;
    
    size_t index = currentIndex;
    size_t count = m_Items.size();
    
    for (size_t i = 0; i < count; i++) {
        if (forward) {
            index = (index + 1) % count;
        } else {
            index = (index + count - 1) % count;
        }
        
        if (m_Items[index].IsEnabled) {
            return index;
        }
    }
    
    return currentIndex;
}

} // namespace VGE



