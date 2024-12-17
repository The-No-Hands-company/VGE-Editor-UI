#pragma once

#include "Runtime/Core/Public/Core/UICore.h"
#include "UIDockSpace.h"
#include <memory>
#include <string>
#include <unordered_map>

namespace VGE {
namespace Editor {

/**
 * @class UIDockingSystem
 * @brief Manages multiple dock spaces and docking operations
 * 
 * Handles:
 * - Dock space creation and management
 * - Window docking/undocking
 * - Layout persistence
 * - Dock space synchronization
 */
class UIDockingSystem {
public:
    /**
     * @brief Constructor
     */
    UIDockingSystem();

    /**
     * @brief Destructor
     */
    ~UIDockingSystem() = default;

    /**
     * @brief Initializes the docking system
     */
    void Initialize();

    /**
     * @brief Updates the docking system
     * @param deltaTime Time elapsed since last update
     */
    void Update(float deltaTime);

    /**
     * @brief Renders all dock spaces
     */
    void Render();

    /**
     * @brief Creates a new dock space
     * @param name Dock space name
     * @return True if dock space was created successfully
     */
    bool CreateDockSpace(const std::string& name);

    /**
     * @brief Destroys a dock space
     * @param name Dock space name
     * @return True if dock space was destroyed successfully
     */
    bool DestroyDockSpace(const std::string& name);

    /**
     * @brief Gets a dock space by name
     * @param name Dock space name
     * @return Dock space pointer, or nullptr if not found
     */
    std::shared_ptr<UIDockSpace> GetDockSpace(const std::string& name);

    /**
     * @brief Docks a window in a specific dock space
     * @param dockSpaceName Dock space name
     * @param window Window to dock
     * @param windowName Window name
     * @param windowTitle Window title (optional)
     * @return True if window was docked successfully
     */
    bool DockWindow(const std::string& dockSpaceName, std::shared_ptr<UIWidget> window,
                   const std::string& windowName, const std::string& windowTitle = "");

    /**
     * @brief Undocks a window from any dock space
     * @param windowName Window name
     * @return True if window was undocked successfully
     */
    bool UndockWindow(const std::string& windowName);

    /**
     * @brief Splits a dock space
     * @param name Dock space name
     * @param isVertical True for vertical split, false for horizontal
     * @param ratio Split ratio (0.0 - 1.0)
     * @return True if split was successful
     */
    bool SplitDockSpace(const std::string& name, bool isVertical, float ratio = 0.5f);

    /**
     * @brief Saves the current docking layout to a JSON file
     * @param filepath Path to save the file
     * @return True if successful
     */
    bool SaveLayout(const std::string& filepath) const;

    /**
     * @brief Loads a docking layout from a JSON file
     * @param filepath Path to load the file from
     * @return True if successful
     */
    bool LoadLayout(const std::string& filepath);

private:
    std::unordered_map<std::string, std::shared_ptr<UIDockSpace>> m_DockSpaces; ///< Dock spaces
    std::unordered_map<std::string, std::string> m_WindowToDockSpace; ///< Maps windows to their dock spaces
};



