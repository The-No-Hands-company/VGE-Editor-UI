#include "Runtime/UI/Public/Controls/UIComboBoxBase.h"
#include "Runtime/Core/Public/Core/UICore.h"
#include "Runtime/UI/Public/Controls/UIComboBoxRenderer.h"
#include "Runtime/UI/Public/Controls/UIComboBoxAnimator.h"
#include "Runtime/UI/Public/Controls/UIComboBoxKeyboard.h"
#include "Runtime/Core/Public/Graphics/RenderCommand.h"
#include "Runtime/Core/Public/Core/Application.h"

namespace VGE {
namespace UI {

UIComboBoxBase::UIComboBoxBase(const std::string& label) 
    : UIWidget(label)
    , m_Style(ComboBoxStyle::Default)
    , m_MaxTags(5)
    , m_ShowIcons(true)
    , m_ShowDescriptions(true)
    , m_UseVirtualization(true)
{
    m_Name = label.empty() ? "ComboBox" : label;
    m_Size = { 200.0f, 30.0f }; // Default size
    m_AnimationDuration = 0.2f;
    m_AnimationProgress = 0.0f;
    m_MaxHeight = 300.0f;
    m_ItemHeight = 24.0f;
    m_EnableDragDrop = false;
    
    // Initialize default visual style
    InitializeDefaultStyle();
}

void UIComboBoxBase::InitializeDefaultStyle() {
    m_Style = ComboBoxStyle::Default;
    m_Theme = std::make_shared<UITheme>();
    
    // Set up default colors and styles
    auto& colors = m_Theme->GetColors();
    colors.Button = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
    colors.ButtonHovered = glm::vec4(0.25f, 0.25f, 0.25f, 1.0f);
    colors.ButtonActive = glm::vec4(0.3f, 0.3f, 0.3f, 1.0f);
    colors.PopupBackground = glm::vec4(0.2f, 0.2f, 0.2f, 0.95f);
    colors.Text = glm::vec4(1.0f);
    colors.TextDisabled = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
    colors.ItemSelected = glm::vec4(0.3f, 0.5f, 0.7f, 1.0f);
    colors.ItemHovered = glm::vec4(0.3f, 0.3f, 0.3f, 1.0f);
    
    auto& style = m_Theme->GetStyle();
    style.ButtonRounding = 3.0f;
    style.PopupRounding = 3.0f;
    style.ButtonPadding = 5.0f;
    style.ItemPadding = 4.0f;
    style.TagRounding = 2.0f;
}

void UIComboBoxBase::Initialize() {
    UIWidget::Initialize();
    
    // Create search input
    m_SearchInput = CreateRef<UITextInput>();
    m_SearchInput->Initialize();
    m_SearchInput->SetPlaceholder("Search...");
    
    // Create scroll view for dropdown
    m_ScrollView = CreateRef<UIScrollView>();
    m_ScrollView->Initialize();
    
    // Create renderer
    m_Renderer = CreateRef<UIComboBoxRenderer>(this);
    
    // Create animator
    m_Animator = CreateRef<UIComboBoxAnimator>(this);
    
    // Create keyboard handler
    m_KeyboardHandler = CreateRef<UIComboBoxKeyboard>(this);
}

void UIComboBoxBase::Update(float deltaTime) {
    if (!m_IsVisible)
        return;

    UIWidget::Update(deltaTime);

    UpdateAnimation(deltaTime);

    if (m_IsDropdownOpen) {
        m_SearchInput->Update(deltaTime);
        m_ScrollView->Update(deltaTime);
        UpdateLayout();
        
        // Update filtering if search text changed
        if (m_SearchInput->GetText() != m_SearchText) {
            m_SearchText = m_SearchInput->GetText();
            UpdateFiltering();
        }
    }
}

void UIComboBoxBase::UpdateAnimation(float deltaTime) {
    float targetProgress = m_IsDropdownOpen ? 1.0f : 0.0f;
    
    if (m_AnimationProgress != targetProgress) {
        float speed = 1.0f / m_AnimationDuration;
        m_AnimationProgress = glm::mix(m_AnimationProgress, targetProgress, deltaTime * speed);
        
        // Snap to target when very close
        if (std::abs(m_AnimationProgress - targetProgress) < 0.01f) {
            m_AnimationProgress = targetProgress;
        }
    }
}

void UIComboBoxBase::UpdateLayout() {
    // Update popup position and size
    m_PopupPosition = m_Position + glm::vec2(0.0f, m_Size.y);
    m_PopupSize = { m_Size.x, std::min(m_MaxHeight, m_FilteredItems.size() * m_ItemHeight) };
    
    // Update search input position and size
    if (m_SearchInput && (m_Style == ComboBoxStyle::SearchFocused || !m_SearchText.empty())) {
        m_SearchInput->SetPosition(m_PopupPosition);
        m_SearchInput->SetSize({ m_PopupSize.x, m_ItemHeight });
        m_PopupPosition.y += m_ItemHeight;
        m_PopupSize.y -= m_ItemHeight;
    }
    
    // Update scroll view
    if (m_ScrollView) {
        m_ScrollView->SetPosition(m_PopupPosition);
        m_ScrollView->SetSize(m_PopupSize);
        m_ScrollView->SetContentSize({ m_PopupSize.x, m_FilteredItems.size() * m_ItemHeight });
    }
}

bool UIComboBoxBase::HandleEvent(Event& event) {
    if (!m_Enabled) return false;

    // Handle search input events
    if (m_IsDropdownOpen && m_SearchInput) {
        if (m_SearchInput->HandleEvent(event)) {
            return true;
        }
    }
    
    // Handle scroll view events
    if (m_IsDropdownOpen && m_ScrollView) {
        if (m_ScrollView->HandleEvent(event)) {
            return true;
        }
    }
    
    // Handle dropdown button events
    EventDispatcher dispatcher(event);
    
    dispatcher.Dispatch<MouseButtonPressedEvent>([this](MouseButtonPressedEvent& e) {
        if (e.GetMouseButton() == MouseButton::Left) {
            if (IsPointInside(e.GetPosition())) {
                ToggleDropdown();
                return true;
            } else if (m_IsDropdownOpen && !IsPointInsidePopup(e.GetPosition())) {
                CloseDropdown();
                return true;
            }
        }
        return false;
    });
    
    // Handle item interaction
    if (m_IsDropdownOpen) {
        if (HandleItemInteraction(event)) {
            return true;
        }
    }
    
    // Handle drag-drop if enabled
    if (m_EnableDragDrop) {
        if (HandleDragDrop(event)) {
            return true;
        }
    }
    
    return false;
}

bool UIComboBoxBase::HandleDragDrop(Event& event) {
    EventDispatcher dispatcher(event);
    
    bool handled = false;
    dispatcher.Dispatch<MouseDragEvent>([this, &handled](MouseDragEvent& e) {
        if (m_SelectedIndex != (size_t)-1 && m_Items[m_SelectedIndex].IsDraggable) {
            BeginDrag(m_Items[m_SelectedIndex]);
            handled = true;
        }
        return true;
    });
    
    dispatcher.Dispatch<MouseDropEvent>([this, &handled](MouseDropEvent& e) {
        if (IsPointInside(e.GetPosition())) {
            HandleDrop(e.GetPayload());
            handled = true;
        }
        return true;
    });
    
    return handled;
}

void UIComboBoxBase::AddItem(const std::string& text, uint64_t id, void* userData) {
    ComboBoxItem item;
    item.Text = text;
    item.SearchText = ToLowerCase(text);
    item.Id = id;
    item.UserData = userData;
    AddItem(item);
}

void UIComboBoxBase::AddItem(const ComboBoxItem& item) {
    m_Items.push_back(item);
    m_SelectedItems.push_back(false);
    UpdateFiltering();
}

void UIComboBoxBase::RemoveItem(size_t index) {
    if (index < m_Items.size()) {
        m_Items.erase(m_Items.begin() + index);
        m_SelectedItems.erase(m_SelectedItems.begin() + index);
        UpdateFiltering();
    }
}

void UIComboBoxBase::ClearItems() {
    m_Items.clear();
    m_SelectedItems.clear();
    m_SelectedIndex = (size_t)-1;
    m_HoveredItemIndex = (size_t)-1;
    m_SearchText.clear();
}

void UIComboBoxBase::SetSelectedIndex(size_t index) {
    if (index >= m_Items.size())
        return;

    m_SelectedIndex = index;
    
    if (m_OnSelectionChanged) {
        m_OnSelectionChanged(m_Items[index]);
    }
}

void UIComboBoxBase::SetSelected(size_t index, bool selected) {
    if (index >= m_Items.size())
        return;

    m_SelectedItems[index] = selected;
    
    if (m_OnMultiSelectionChanged) {
        std::vector<ComboBoxItem> selectedItems;
        for (size_t i = 0; i < m_Items.size(); i++) {
            if (m_SelectedItems[i]) {
                selectedItems.push_back(m_Items[i]);
            }
        }
        m_OnMultiSelectionChanged(selectedItems);
    }
}

std::vector<size_t> UIComboBoxBase::GetSelectedIndices() const {
    std::vector<size_t> indices;
    for (size_t i = 0; i < m_SelectedItems.size(); i++) {
        if (m_SelectedItems[i]) {
            indices.push_back(i);
        }
    }
    return indices;
}

std::vector<const ComboBoxItem*> UIComboBoxBase::GetSelectedItems() const {
    std::vector<const ComboBoxItem*> items;
    for (size_t i = 0; i < m_Items.size(); i++) {
        if (m_SelectedItems[i]) {
            items.push_back(&m_Items[i]);
        }
    }
    return items;
}

void UIComboBoxBase::SetSearchText(const std::string& text) {
    if (m_SearchText != text) {
        m_SearchText = text;
        UpdateFiltering();
        
        if (m_OnSearchChanged) {
            m_OnSearchChanged(text);
        }
    }
}

void UIComboBoxBase::ClearSearch() {
    SetSearchText("");
    if (m_SearchInput) {
        m_SearchInput->SetText("");
    }
}

void UIComboBoxBase::OpenDropdown() {
    if (!m_IsDropdownOpen) {
        m_IsDropdownOpen = true;
        if (m_SearchInput) {
            m_SearchInput->Focus();
        }
        UpdateFiltering();
    }
}

void UIComboBoxBase::CloseDropdown() {
    if (m_IsDropdownOpen) {
        m_IsDropdownOpen = false;
        ClearSearch();
    }
}

void UIComboBoxBase::ToggleDropdown() {
    if (m_IsDropdownOpen) {
        CloseDropdown();
    } else {
        OpenDropdown();
    }
}

void UIComboBoxBase::UpdateFiltering() {
    m_FilteredItems.clear();
    m_FilteredIndices.clear();
    
    std::string searchText = ToLowerCase(m_SearchText);
    
    // If search text is empty, show all items
    if (searchText.empty()) {
        m_FilteredItems = m_Items;
        m_FilteredIndices.resize(m_Items.size());
        for (size_t i = 0; i < m_Items.size(); ++i) {
            m_FilteredIndices[i] = i;
        }
        return;
    }
    
    // Split search text into words for multi-word search
    std::vector<std::string> searchWords = SplitString(searchText, ' ');
    
    // Filter items
    for (size_t i = 0; i < m_Items.size(); ++i) {
        const auto& item = m_Items[i];
        bool matches = true;
        
        // Item must match all search words
        for (const auto& word : searchWords) {
            if (item.SearchText.find(word) == std::string::npos) {
                matches = false;
                break;
            }
        }
        
        if (matches) {
            m_FilteredItems.push_back(item);
            m_FilteredIndices.push_back(i);
        }
    }
    
    // Sort filtered items by relevance
    std::sort(m_FilteredItems.begin(), m_FilteredItems.end(),
        [&searchText](const ComboBoxItem& a, const ComboBoxItem& b) {
            // Primary sort: exact match
            bool aExact = (a.SearchText == searchText);
            bool bExact = (b.SearchText == searchText);
            if (aExact != bExact) return aExact > bExact;
            
            // Secondary sort: starts with search text
            bool aStarts = a.SearchText.find(searchText) == 0;
            bool bStarts = b.SearchText.find(searchText) == 0;
            if (aStarts != bStarts) return aStarts > bStarts;
            
            // Tertiary sort: length (shorter items first)
            if (a.SearchText.length() != b.SearchText.length())
                return a.SearchText.length() < b.SearchText.length();
            
            // Final sort: alphabetical
            return a.SearchText < b.SearchText;
        });
    
    // Update scroll view content height
    if (m_ScrollView) {
        float itemHeight = 24.0f;
        m_ScrollView->SetContentHeight(m_FilteredItems.size() * itemHeight);
    }
    
    // Update selection if the selected item is filtered out
    if (m_SelectedIndex != (size_t)-1) {
        bool selectedItemVisible = false;
        for (size_t i = 0; i < m_FilteredIndices.size(); ++i) {
            if (m_FilteredIndices[i] == m_SelectedIndex) {
                selectedItemVisible = true;
                break;
            }
        }
        
        if (!selectedItemVisible) {
            m_HoveredItemIndex = (size_t)-1;
        }
    }
}

bool UIComboBoxBase::IsItemVisible(const ComboBoxItem& item) const {
    if (m_SearchText.empty())
        return true;
    
    std::string searchText = ToLowerCase(m_SearchText);
    std::vector<std::string> searchWords = SplitString(searchText, ' ');
    
    // Item must match all search words
    for (const auto& word : searchWords) {
        if (item.SearchText.find(word) == std::string::npos) {
            return false;
        }
    }
    
    return true;
}

std::string UIComboBoxBase::ToLowerCase(const std::string& text) {
    std::string result = text;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::vector<std::string> UIComboBoxBase::SplitString(const std::string& text, char delimiter) {
    std::vector<std::string> result;
    std::stringstream ss(text);
    std::string item;
    
    while (std::getline(ss, item, delimiter)) {
        if (!item.empty()) {
            result.push_back(item);
        }
    }
    
    return result;
}

void UIComboBoxBase::Render() {
    if (!m_IsVisible)
        return;

    UIWidget::Render();

    // Render selected item
    if (m_SelectedIndex != (size_t)-1 && m_SelectedIndex < m_Items.size()) {
        const auto& item = m_Items[m_SelectedIndex];
        RenderItem(item, m_Position, m_Size, true);
    }

    // Render dropdown
    if (m_IsDropdownOpen) {
        // Render search input
        if (m_SearchInput) {
            m_SearchInput->Render();
        }
        
        // Render scroll view with filtered items
        if (m_ScrollView) {
            m_ScrollView->Render();
            
            float itemHeight = 24.0f;
            float scrollY = m_ScrollView->GetScrollPosition().y;
            
            // Calculate visible range
            size_t startIndex = static_cast<size_t>(scrollY / itemHeight);
            size_t endIndex = startIndex + static_cast<size_t>(m_ScrollView->GetSize().y / itemHeight) + 1;
            endIndex = std::min(endIndex, m_FilteredItems.size());
            
            // Render visible items
            for (size_t i = startIndex; i < endIndex; ++i) {
                const auto& item = m_FilteredItems[i];
                size_t originalIndex = m_FilteredIndices[i];
                
                glm::vec2 itemPos = m_ScrollView->GetPosition() + glm::vec2(0.0f, i * itemHeight - scrollY);
                glm::vec2 itemSize(m_ScrollView->GetSize().x, itemHeight);
                
                bool isSelected = (originalIndex == m_SelectedIndex);
                bool isHovered = (i == m_HoveredItemIndex);
                
                RenderItem(item, itemPos, itemSize, isSelected, isHovered);
            }
        }
    }
}

void UIComboBoxBase::OnMouseDown(const glm::vec2& position) {
    UIWidget::OnMouseDown(position);
    
    if (!m_IsDropdownOpen) {
        OpenDropdown();
    }
}

void UIComboBoxBase::OnMouseUp(const glm::vec2& position) {
    UIWidget::OnMouseUp(position);
}

void UIComboBoxBase::OnMouseMove(const glm::vec2& position) {
    UIWidget::OnMouseMove(position);
}

void UIComboBoxBase::OnKeyDown(int keyCode) {
    UIWidget::OnKeyDown(keyCode);
    
    if (m_IsDropdownOpen) {
        m_SearchInput->OnKeyDown(keyCode);
    }
}

void UIComboBoxBase::OnFocusLost() {
    UIWidget::OnFocusLost();
    CloseDropdown();
}

void UIComboBoxBase::SelectNextItem() {
    if (m_Items.empty())
        return;
        
    size_t nextIndex = (m_SelectedIndex + 1) % m_Items.size();
    SetSelectedIndex(nextIndex);
}

void UIComboBoxBase::SelectPreviousItem() {
    if (m_Items.empty())
        return;
        
    size_t prevIndex = m_SelectedIndex == 0 || m_SelectedIndex == (size_t)-1 
        ? m_Items.size() - 1 
        : m_SelectedIndex - 1;
    SetSelectedIndex(prevIndex);
}

void UIComboBoxBase::SelectFirstItem() {
    if (!m_Items.empty()) {
        SetSelectedIndex(0);
    }
}

void UIComboBoxBase::SelectLastItem() {
    if (!m_Items.empty()) {
        SetSelectedIndex(m_Items.size() - 1);
    }
}

void UIComboBoxBase::SelectNextPage() {
    if (m_Items.empty())
        return;
        
    size_t itemsPerPage = GetItemsPerPage();
    size_t nextIndex = m_SelectedIndex + itemsPerPage;
    if (nextIndex >= m_Items.size()) {
        nextIndex = m_Items.size() - 1;
    }
    SetSelectedIndex(nextIndex);
}

void UIComboBoxBase::SelectPreviousPage() {
    if (m_Items.empty())
        return;
        
    size_t itemsPerPage = GetItemsPerPage();
    if (m_SelectedIndex == (size_t)-1 || m_SelectedIndex < itemsPerPage) {
        SetSelectedIndex(0);
    } else {
        SetSelectedIndex(m_SelectedIndex - itemsPerPage);
    }
}

size_t UIComboBoxBase::GetItemsPerPage() const {
    if (!m_ScrollView)
        return 1;
        
    float itemHeight = 24.0f; // TODO: Get from properties
    float viewportHeight = m_ScrollView->GetViewportSize().y;
    return static_cast<size_t>(viewportHeight / itemHeight);
}

void UIComboBoxBase::RenderDropdownButton() {
    if (!m_Renderer) return;
    
    // Render button background
    m_Renderer->RenderBackground(m_Position, m_Size, m_HoverAmount);
    
    // Render selected items
    if (m_Style == ComboBoxStyle::MultiSelect) {
        RenderSelectedTags();
    } else {
        RenderSelectedItem();
    }
    
    // Render dropdown arrow
    float arrowRotation = m_IsDropdownOpen ? 180.0f : 0.0f;
    glm::vec2 arrowPos = m_Position + glm::vec2(m_Size.x - 20.0f, m_Size.y * 0.5f);
    m_Renderer->RenderDropdownArrow(arrowPos, 8.0f, arrowRotation);
}

void UIComboBoxBase::RenderSelectedTags() {
    if (!m_Renderer) return;
    
    auto selectedIndices = GetSelectedIndices();
    if (selectedIndices.empty()) {
        // Render placeholder
        m_Renderer->RenderText(m_Name, m_Position + glm::vec2(5.0f), m_Theme->GetColors().TextDisabled);
        return;
    }
    
    float xOffset = 5.0f;
    size_t displayedTags = 0;
    
    for (size_t idx : selectedIndices) {
        if (idx >= m_Items.size() || displayedTags >= m_MaxTags) break;
        
        const auto& item = m_Items[idx];
        float tagWidth = m_Renderer->GetTextWidth(item.Text) + 20.0f;
        
        if (xOffset + tagWidth > m_Size.x - 40.0f) {
            // Draw remaining count
            std::string remaining = "+" + std::to_string(selectedIndices.size() - displayedTags);
            m_Renderer->RenderTag(remaining, m_Position + glm::vec2(xOffset, 0.0f), true);
            break;
        }
        
        m_Renderer->RenderTag(item.Text, m_Position + glm::vec2(xOffset, 0.0f), false);
        xOffset += tagWidth + 5.0f;
        displayedTags++;
    }
}

void UIComboBoxBase::RenderSelectedItem() {
    if (!m_Renderer || m_SelectedIndex >= m_Items.size()) {
        // Render placeholder
        m_Renderer->RenderText(m_Name, m_Position + glm::vec2(5.0f), m_Theme->GetColors().TextDisabled);
        return;
    }
    
    const auto& item = m_Items[m_SelectedIndex];
    m_Renderer->RenderText(item.Text, m_Position + glm::vec2(5.0f), m_Theme->GetColors().Text);
    
    if (m_ShowDescriptions && !item.Description.empty()) {
        float textWidth = m_Renderer->GetTextWidth(item.Text);
        m_Renderer->RenderText(item.Description, 
            m_Position + glm::vec2(textWidth + 10.0f, 0.0f), 
            m_Theme->GetColors().TextDisabled);
    }
}

} // namespace VGE::UI



