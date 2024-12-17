#pragma once

#include "Runtime/Core/Public/Widget/UIWidget.h"
#include "Runtime/Core/Public/Theme/UIAnimatable.h"
#include "Runtime/Core/Public/Core/UITypes.h"
#include <string>
#include <vector>
#include <functional>
#include <memory>

namespace VGE {
namespace UI {

struct CommandPaletteItem {
    std::string name;
    std::string description;
    std::string category;
    std::string shortcut;
    std::function<void()> action;
    float relevanceScore = 0.0f;
};

struct CommandPaletteStyle {
    // Colors
    glm::vec4 backgroundColor = glm::vec4(0.15f, 0.15f, 0.15f, 0.95f);
    glm::vec4 searchBarColor = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
    glm::vec4 textColor = glm::vec4(1.0f);
    glm::vec4 highlightColor = glm::vec4(0.3f, 0.6f, 1.0f, 1.0f);
    glm::vec4 categoryColor = glm::vec4(0.7f, 0.7f, 0.7f, 1.0f);
    glm::vec4 shortcutColor = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
    
    // Dimensions
    float width = 600.0f;
    float maxHeight = 400.0f;
    float searchBarHeight = 40.0f;
    float itemHeight = 32.0f;
    float padding = 8.0f;
    float borderRadius = 8.0f;
    
    // Font settings
    float searchFontSize = 16.0f;
    float itemFontSize = 14.0f;
    float categoryFontSize = 12.0f;
};

class UI_API UICommandPalette : public UIWidget, public UIAnimatable {
public:
    UICommandPalette();
    virtual ~UICommandPalette() = default;

    // UIWidget interface
    void OnLayoutUpdate() override;
    glm::vec2 CalculateDesiredSize() override;
    void ArrangeChildren() override;
    void OnPaint(UIRenderer* renderer) override;
    void OnChildAdded(std::shared_ptr<UIWidget> child) override;
    void OnChildRemoved(std::shared_ptr<UIWidget> child) override;

    // UIAnimatable interface
    void OnAnimationUpdate(float deltaTime) override;
    void OnAnimationStateChanged(const std::string& newState) override;

    // Command palette specific methods
    void Show();
    void Hide();
    bool IsShown() const { return m_IsShown; }
    
    void AddCommand(const CommandPaletteItem& item);
    void RemoveCommand(const std::string& name);
    void ClearCommands();
    
    void SetSearchText(const std::string& text);
    const std::string& GetSearchText() const { return m_SearchText; }
    
    void SetStyle(const CommandPaletteStyle& style);
    const CommandPaletteStyle& GetStyle() const { return m_Style; }

    // Event callbacks
    using CommandSelectedCallback = std::function<void(const CommandPaletteItem&)>;
    void SetOnCommandSelected(CommandSelectedCallback callback) { m_OnCommandSelected = callback; }

private:
    // Helper methods
    void UpdateSearch();
    void UpdateLayout();
    float CalculateRelevanceScore(const CommandPaletteItem& item, const std::string& query);
    void SelectNextItem();
    void SelectPreviousItem();
    void ExecuteSelectedCommand();
    
    // Input handling
    void HandleKeyPress(const KeyEvent& event);
    void HandleTextInput(const TextInputEvent& event);
    
    // Drawing helpers
    void DrawBackground(UIRenderer* renderer);
    void DrawSearchBar(UIRenderer* renderer);
    void DrawResults(UIRenderer* renderer);
    void DrawItem(UIRenderer* renderer, const CommandPaletteItem& item, bool isSelected, float y);

private:
    bool m_IsShown;
    std::string m_SearchText;
    std::vector<CommandPaletteItem> m_Commands;
    std::vector<CommandPaletteItem> m_FilteredCommands;
    int m_SelectedIndex;
    CommandPaletteStyle m_Style;
    CommandSelectedCallback m_OnCommandSelected;
    
    // Animation state
    float m_ShowProgress;
    float m_SearchBarFocus;
    std::vector<float> m_ItemAnimProgress;
};

}} // namespace VGE::UI 