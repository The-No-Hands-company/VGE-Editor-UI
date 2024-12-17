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
    void Render(UI