#pragma once

#include "Runtime/Core/Public/Core/UICore.h"
#include "Runtime/UI/Public/Docking/UIDockSpace.h"
#include <memory>
#include <string>
#include <unordered_map>

namespace VGE {
namespace Editor {

/**
 * @brief Manages the docking system and coordinates between dock spaces
 */
class UIDockingManager {
public:
    static UIDockingManager& Get();

    /**
     * @brief Create a new dock space
     * @param name Unique name for the dock space
     * @return Created dock space
     */
    std::shared_ptr<UIDockSpace> CreateDockSpace(const std::string& name);

    /**
     * @brief Get a dock space by name
     * @param name Name of the dock space to find
     * @return Found dock space or nullptr
     */
    std::shared_ptr<UIDockSpace> GetDockSpace(const std::string& name) const;

    /**
     * @brief Remove a dock space
     * @param name Name of the dock space to remove
     */
    void RemoveDockSpace(const std::string& name);

    /**
     * @brief Get the root/main dock space
     * @return Root dock space
     */
    std::shared_ptr<UIDockSpace> GetRootDockSpace() const { return m_RootDockSpace; }

    /**
     * @brief Set the root dock space
     * @param dockSpace Dock space to set as root
     */
    void SetRootDockSpace(std::shared_ptr<UIDockSpace> dockSpace);

    /**
     * @brief Update all dock spaces
     * @param deltaTime Time since last update
     */
    void Update(float deltaTime);

    /**
     * @brief Render all dock spaces
     */
    void Render();

private:
    UIDockingManager() = default;
    ~UIDockingManager() = default;

    UIDockingManager(const UIDockingManager&) = delete;
    UIDockingManager& operator=(const UIDockingManager&) = delete;

private:
    std::shared_ptr<UIDockSpace> m_RootDockSpace;
    std::unordered_map<std::string, std::shared_ptr<UIDockSpace>> m_DockSpaces;
};

}} // namespace VGE::Editor