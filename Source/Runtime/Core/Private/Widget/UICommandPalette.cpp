#include "Runtime/Core/Public/Widget/UICommandPalette.h"
#include "Runtime/Core/Public/Theme/UIAnimationPresets.h"
#include "Runtime/Core/Public/Theme/UIAnimationTemplates.h"
#include "Runtime/Core/Public/Render/UIRenderer.h"
#include <algorithm>

namespace VGE {
namespace UI {

UICommandPalette::UICommandPalette()
    : m_IsShown(false)
    , m_SelectedIndex(-1)
    , m_ShowProgress(0.0f)
    , m_SearchBarFocus(0.0f)
{
    // Set up animation state machine
    auto stateMachine = std::make_shared<UIAnimationStateMachine>();
    
    // Add states
    stateMachine->AddState("Hidden", std::make_shared<UIAnimationState>("Hidden"));
    stateMachine->AddState("Showing", CreateShowingState());
    stateMachine->AddState("Visible", std::make_shared<UIAnimationState>("Visible"));
    stateMachine->AddState("Hiding", CreateHidingState());
    
    // Add transitions
    stateMachine->AddTransition(UIAnimationPresets::CreateFadeTransition("Hidden", "Showing", 0.2f));
    stateMachine->AddTransition(UIAnimationPresets::CreateFadeTransition("Showing", "Visible", 0.1f));
    stateMachine->AddTransition(UIAnimationPresets::CreateFadeTransition("Visible", "Hiding", 0.2f));
    stateMachine->AddTransition(UIAnimationPresets::CreateFadeTransition("Hiding", "Hidden", 0.1f));
    
    stateMachine->SetDefaultState("Hidden");
    SetAnimationStateMachine(stateMachine);
}

void UICommandPalette::Show() {
    if (!m_IsShown) {
        m_IsShown = true;
        m_SearchText.clear();
        m_SelectedIndex = -1;
        UpdateSearch();
        GetAnimationStateMachine()->TransitionTo("Showing");
    }
}

void UICommandPalette::Hide() {
    if (m_IsShown) {
        m_IsShown = false;
        GetAnimationStateMachine()->TransitionTo("Hiding");
    }
}

void UICommandPalette::AddCommand(const CommandPaletteItem& item) {
    m_Commands.push_back(item);
    if (m_IsShown) {
        UpdateSearch();
    }
}

void UICommandPalette::RemoveCommand(const std::string& name) {
    m_Commands.erase(
        std::remove_if(m_Commands.begin(), m_Commands.end(),
            [&name](const CommandPaletteItem& item) { return item.name == name; }),
        m_Commands.end());
    
    if (m_IsShown) {
        UpdateSearch();
    }
}

void UICommandPalette::ClearCommands() {
    m_Commands.clear();
    m_FilteredCommands.clear();
    m_SelectedIndex = -1;
}

void UICommandPalette::SetSearchText(const std::string& text) {
    if (m_SearchText != text) {
        m_SearchText = text;
        UpdateSearch();
    }
}

void UICommandPalette::SetStyle(const CommandPaletteStyle& style) {
    m_Style = style;
    InvalidateLayout();
}

void UICommandPalette::OnLayoutUpdate() {
    // Center the command palette in the parent widget
    if (auto parent = GetParent().lock()) {
        const auto parentSize = parent->GetSize();
        const auto desiredSize = CalculateDesiredSize();
        
        SetPosition(glm::vec2(
            (parentSize.x - desiredSize.x) * 0.5f,
            (parentSize.y - desiredSize.y) * 0.5f
        ));
        SetSize(desiredSize);
    }
}

glm::vec2 UICommandPalette::CalculateDesiredSize() {
    const float height = std::min(
        m_Style.maxHeight,
        m_Style.searchBarHeight + m_Style.itemHeight * m_FilteredCommands.size() + m_Style.padding * 2
    );
    
    return glm::vec2(m_Style.width, height);
}

void UICommandPalette::ArrangeChildren() {
    // Command palette doesn't have child widgets
}

void UICommandPalette::OnPaint(UIRenderer* renderer) {
    if (!m_IsShown && m_ShowProgress <= 0.0f) return;
    
    DrawBackground(renderer);
    DrawSearchBar(renderer);
    DrawResults(renderer);
}

void UICommandPalette::OnChildAdded(std::shared_ptr<UIWidget> child) {
    // Command palette doesn't support child widgets
}

void UICommandPalette::OnChildRemoved(std::shared_ptr<UIWidget> child) {
    // Command palette doesn't support child widgets
}

void UICommandPalette::OnAnimationUpdate(float deltaTime) {
    // Update animation progress values based on current state
    const auto& currentState = GetAnimationStateMachine()->GetCurrentState();
    
    if (currentState == "Showing") {
        m_ShowProgress = std::min(1.0f, m_ShowProgress + deltaTime * 5.0f);
        m_SearchBarFocus = std::min(1.0f, m_SearchBarFocus + deltaTime * 4.0f);
    }
    else if (currentState == "Hiding") {
        m_ShowProgress = std::max(0.0f, m_ShowProgress - deltaTime * 5.0f);
        m_SearchBarFocus = std::max(0.0f, m_SearchBarFocus - deltaTime * 4.0f);
    }
    
    // Update item animations
    const size_t numItems = m_FilteredCommands.size();
    m_ItemAnimProgress.resize(numItems);
    
    for (size_t i = 0; i < numItems; ++i) {
        const float targetProgress = (currentState == "Showing" || currentState == "Visible") ? 1.0f : 0.0f;
        const float delay = float(i) * 0.05f;
        
        if (m_ShowProgress > delay) {
            const float progressDelta = deltaTime * 5.0f;
            if (targetProgress > m_ItemAnimProgress[i]) {
                m_ItemAnimProgress[i] = std::min(targetProgress, m_ItemAnimProgress[i] + progressDelta);
            } else {
                m_ItemAnimProgress[i] = std::max(targetProgress, m_ItemAnimProgress[i] - progressDelta);
            }
        }
    }
}

void UICommandPalette::OnAnimationStateChanged(const std::string& newState) {
    if (newState == "Hidden") {
        m_ShowProgress = 0.0f;
        m_SearchBarFocus = 0.0f;
        m_ItemAnimProgress.clear();
    }
}

void UICommandPalette::UpdateSearch() {
    m_FilteredCommands = m_Commands;
    
    if (!m_SearchText.empty()) {
        // Calculate relevance scores
        for (auto& command : m_FilteredCommands) {
            command.relevanceScore = CalculateRelevanceScore(command, m_SearchText);
        }
        
        // Remove items with zero relevance
        m_FilteredCommands.erase(
            std::remove_if(m_FilteredCommands.begin(), m_FilteredCommands.end(),
                [](const CommandPaletteItem& item) { return item.relevanceScore <= 0.0f; }),
            m_FilteredCommands.end());
        
        // Sort by relevance
        std::sort(m_FilteredCommands.begin(), m_FilteredCommands.end(),
            [](const CommandPaletteItem& a, const CommandPaletteItem& b) {
                return a.relevanceScore > b.relevanceScore;
            });
    }
    
    // Reset selection
    m_SelectedIndex = m_FilteredCommands.empty() ? -1 : 0;
    
    // Update layout
    InvalidateLayout();
}

float UICommandPalette::CalculateRelevanceScore(const CommandPaletteItem& item, const std::string& query) {
    // Simple fuzzy matching algorithm
    float score = 0.0f;
    size_t lastMatchPos = 0;
    size_t consecutiveMatches = 0;
    
    for (char c : query) {
        // Convert to lowercase for case-insensitive comparison
        c = std::tolower(c);
        
        // Search in name
        size_t pos = item.name.find(c, lastMatchPos);
        if (pos != std::string::npos) {
            // Award points based on position and consecutive matches
            score += 1.0f + (pos == lastMatchPos ? consecutiveMatches * 0.5f : 0.0f);
            lastMatchPos = pos + 1;
            consecutiveMatches++;
        } else {
            // Search in description if not found in name
            pos = item.description.find(c);
            if (pos != std::string::npos) {
                score += 0.5f; // Lower score for description matches
            } else {
                return 0.0f; // No match found
            }
            consecutiveMatches = 0;
        }
    }
    
    return score;
}

void UICommandPalette::HandleKeyPress(const KeyEvent& event) {
    if (!m_IsShown) return;
    
    switch (event.key) {
        case Key::Up:
            SelectPreviousItem();
            break;
            
        case Key::Down:
            SelectNextItem();
            break;
            
        case Key::Enter:
            ExecuteSelectedCommand();
            break;
            
        case Key::Escape:
            Hide();
            break;
    }
}

void UICommandPalette::HandleTextInput(const TextInputEvent& event) {
    if (!m_IsShown) return;
    
    SetSearchText(m_SearchText + event.text);
}

void UICommandPalette::SelectNextItem() {
    if (!m_FilteredCommands.empty()) {
        m_SelectedIndex = (m_SelectedIndex + 1) % m_FilteredCommands.size();
    }
}

void UICommandPalette::SelectPreviousItem() {
    if (!m_FilteredCommands.empty()) {
        m_SelectedIndex = (m_SelectedIndex - 1 + m_FilteredCommands.size()) % m_FilteredCommands.size();
    }
}

void UICommandPalette::ExecuteSelectedCommand() {
    if (m_SelectedIndex >= 0 && m_SelectedIndex < m_FilteredCommands.size()) {
        const auto& selectedCommand = m_FilteredCommands[m_SelectedIndex];
        Hide();
        if (selectedCommand.action) {
            selectedCommand.action();
        }
        if (m_OnCommandSelected) {
            m_OnCommandSelected(selectedCommand);
        }
    }
}

void UICommandPalette::DrawBackground(UIRenderer* renderer) {
    const auto bounds = GetBounds();
    const float alpha = m_ShowProgress * m_Style.backgroundColor.a;
    const auto color = glm::vec4(
        m_Style.backgroundColor.r,
        m_Style.backgroundColor.g,
        m_Style.backgroundColor.b,
        alpha
    );
    
    renderer->DrawRoundedRect(bounds, m_Style.borderRadius, color);
}

void UICommandPalette::DrawSearchBar(UIRenderer* renderer) {
    const auto bounds = GetBounds();
    const float searchBarY = bounds.y + m_Style.padding;
    const glm::vec4 searchBarBounds(
        bounds.x + m_Style.padding,
        searchBarY,
        bounds.z - m_Style.padding * 2,
        m_Style.searchBarHeight
    );
    
    // Draw search bar background
    renderer->DrawRoundedRect(
        searchBarBounds,
        m_Style.borderRadius * 0.75f,
        m_Style.searchBarColor
    );
    
    // Draw search text
    const float textPadding = m_Style.padding * 1.5f;
    renderer->DrawText(
        m_SearchText.empty() ? "Type to search..." : m_SearchText,
        glm::vec2(searchBarBounds.x + textPadding, searchBarY + m_Style.searchBarHeight * 0.5f),
        m_Style.searchFontSize,
        m_SearchText.empty() ? glm::vec4(m_Style.textColor.r, m_Style.textColor.g, m_Style.textColor.b, 0.5f) : m_Style.textColor,
        TextAlignment::Left,
        TextBaseline::Middle
    );
    
    // Draw search bar focus animation
    if (m_SearchBarFocus > 0.0f) {
        const float focusAlpha = m_SearchBarFocus * 0.5f;
        const float focusPadding = 2.0f;
        renderer->DrawRoundedRectOutline(
            glm::vec4(
                searchBarBounds.x - focusPadding,
                searchBarBounds.y - focusPadding,
                searchBarBounds.z + focusPadding * 2,
                searchBarBounds.w + focusPadding * 2
            ),
            m_Style.borderRadius * 0.75f,
            glm::vec4(m_Style.highlightColor.r, m_Style.highlightColor.g, m_Style.highlightColor.b, focusAlpha),
            2.0f
        );
    }
}

void UICommandPalette::DrawResults(UIRenderer* renderer) {
    const auto bounds = GetBounds();
    const float resultsY = bounds.y + m_Style.searchBarHeight + m_Style.padding * 2;
    
    for (size_t i = 0; i < m_FilteredCommands.size(); ++i) {
        const float itemY = resultsY + i * m_Style.itemHeight;
        const bool isSelected = (int)i == m_SelectedIndex;
        const float itemProgress = m_ItemAnimProgress[i];
        
        if (itemProgress > 0.0f) {
            DrawItem(renderer, m_FilteredCommands[i], isSelected, itemY);
        }
    }
}

void UICommandPalette::DrawItem(UIRenderer* renderer, const CommandPaletteItem& item, bool isSelected, float y) {
    const auto bounds = GetBounds();
    const float itemX = bounds.x + m_Style.padding;
    const float itemWidth = bounds.z - m_Style.padding * 2;
    
    // Draw selection background
    if (isSelected) {
        renderer->DrawRoundedRect(
            glm::vec4(itemX, y, itemWidth, m_Style.itemHeight),
            m_Style.borderRadius * 0.5f,
            glm::vec4(m_Style.highlightColor.r, m_Style.highlightColor.g, m_Style.highlightColor.b, 0.2f)
        );
    }
    
    const float textY = y + m_Style.itemHeight * 0.5f;
    const float categoryWidth = 80.0f;
    const float shortcutWidth = 100.0f;
    const float nameWidth = itemWidth - categoryWidth - shortcutWidth - m_Style.padding * 2;
    
    // Draw category
    if (!item.category.empty()) {
        renderer->DrawText(
            item.category,
            glm::vec2(itemX, textY),
            m_Style.categoryFontSize,
            m_Style.categoryColor,
            TextAlignment::Left,
            TextBaseline::Middle
        );
    }
    
    // Draw name
    renderer->DrawText(
        item.name,
        glm::vec2(itemX + categoryWidth + m_Style.padding, textY),
        m_Style.itemFontSize,
        m_Style.textColor,
        TextAlignment::Left,
        TextBaseline::Middle
    );
    
    // Draw shortcut
    if (!item.shortcut.empty()) {
        renderer->DrawText(
            item.shortcut,
            glm::vec2(itemX + itemWidth - m_Style.padding, textY),
            m_Style.categoryFontSize,
            m_Style.shortcutColor,
            TextAlignment::Right,
            TextBaseline::Middle
        );
    }
}

std::shared_ptr<UIAnimationState> UICommandPalette::CreateShowingState() {
    auto state = std::make_shared<UIAnimationState>("Showing");
    auto sequence = std::make_shared<UIAnimationSequence>();
    
    // Add show animations
    auto fadeIn = UIAnimationTemplates::CreateFade(0.0f, 1.0f, 0.2f);
    auto scaleUp = UIAnimationTemplates::CreateScale(
        glm::vec2(0.98f), glm::vec2(1.0f), 0.2f);
    auto slideDown = UIAnimationTemplates::CreateSlide(
        glm::vec2(0.0f, -10.0f), glm::vec2(0.0f), 0.2f);
    
    sequence->AddClip(fadeIn, 0.0f);
    sequence->AddClip(scaleUp, 0.0f);
    sequence->AddClip(slideDown, 0.0f);
    
    state->AddSequence(sequence);
    return state;
}

std::shared_ptr<UIAnimationState> UICommandPalette::CreateHidingState() {
    auto state = std::make_shared<UIAnimationState>("Hiding");
    auto sequence = std::make_shared<UIAnimationSequence>();
    
    // Add hide animations
    auto fadeOut = UIAnimationTemplates::CreateFade(1.0f, 0.0f, 0.2f);
    auto scaleDown = UIAnimationTemplates::CreateScale(
        glm::vec2(1.0f), glm::vec2(0.98f), 0.2f);
    auto slideUp = UIAnimationTemplates::CreateSlide(
        glm::vec2(0.0f), glm::vec2(0.0f, -10.0f), 0.2f);
    
    sequence->AddClip(fadeOut, 0.0f);
    sequence->AddClip(scaleDown, 0.0f);
    sequence->AddClip(slideUp, 0.0f);
    
    state->AddSequence(sequence);
    return state;
}

}} // namespace VGE::UI 