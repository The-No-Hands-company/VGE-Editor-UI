#pragma once

#include "Runtime/Core/Public/Core/UICore.h"
#include "Runtime/Core/Public/Widget/UIWidget.h"
#include <functional>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace VGE {
namespace UI {

/**
 * @class UICommandPalette
 * @brief Quick command search and execution interface
 * 
 * Features:
 * - Fuzzy search for commands
 * - Keyboard shortcut support
 * - Command categories and grouping
 * - Command history and favorites
 */
class UI_API UICommandPalette : public UIWidget {
public:
    /**
     * @brief Command information structure
     */
    struct Command {
        std::string name;           ///< Display name of the command
        std::string description;    ///< Command description
        std::string category;       ///< Category for grouping
        std::string shortcut;       ///< Keyboard shortcut
        std::function<void()> action; ///< Command action to execute
        bool isFavorite;           ///< Whether command is marked as favorite
        
        Command(const std::string& n, const std::string& d, const std::string& c, 
               const std::string& s, std::function<void()> a)
            : name(n), description(d), category(c), shortcut(s), action(a), isFavorite(false) {}
    };

    UICommandPalette();
    virtual ~UICommandPalette() = default;

    void Initialize() override;
    void Update(float deltaTime) override;
    void Render(UIRenderer* renderer) override;

    /**
     * @brief Register a new command
     */
    void RegisterCommand(const std::string& name, const std::string& description,
                        const std::string& category, const std::string& shortcut,
                        std::function<void()> action);

    /**
     * @brief Remove a registered command
     */
    void UnregisterCommand(const std::string& name);

    /**
     * @brief Toggle command palette visibility
     */
    void ToggleVisibility();

    /**
     * @brief Set command as favorite
     */
    void SetCommandFavorite(const std::string& name, bool favorite);

    /**
     * @brief Handle keyboard input
     */
    bool OnKeyPress(int key, int mods) override;
    bool OnChar(unsigned int codepoint) override;

private:
    std::unordered_map<std::string, std::shared_ptr<Command>> m_Commands;
    std::vector<std::shared_ptr<Command>> m_FilteredCommands;
    std::vector<std::shared_ptr<Command>> m_CommandHistory;
    std::string m_SearchQuery;
    int m_SelectedIndex;
    bool m_IsVisible;
    
    /**
     * @brief Update filtered commands based on search
     */
    void UpdateFilteredCommands();

    /**
     * @brief Execute selected command
     */
    void ExecuteSelected();

    /**
     * @brief Add command to history
     */
    void AddToHistory(const std::shared_ptr<Command>& command);

    /**
     * @brief Calculate fuzzy match score
     */
    float CalculateFuzzyScore(const std::string& str, const std::string& query);
};

}} // namespace VGE::UI



