#pragma once

#include "Runtime/Core/Public/Core/UICore.h"
#include "UIDockSpace.h"
#include "UIWindowManager.h"
#include <memory>
#include <string>
#include <nlohmann/json.hpp>

namespace VGE {
namespace Editor {

/**
 * @class UILayoutSerializer
 * @brief Handles serialization and deserialization of editor layouts
 * 
 * Manages:
 * - Layout saving and loading
 * - Window state persistence
 * - Dock space configuration serialization
 * - Layout versioning
 */
class UILayoutSerializer {
public:
    /**
     * @brief Constructor
     */
    UILayoutSerializer() = default;

    /**
     * @brief Destructor
     */
    ~UILayoutSerializer() = default;

    /**
     * @brief Saves the current layout to a file
     * @param filepath Path to save the layout
     * @param rootDockSpace Root dock space to serialize
     * @param windowManager Window manager to serialize window states from
     * @return True if save was successful
     */
    bool SaveLayout(const std::string& filepath,
                   std::shared_ptr<UIDockSpace> rootDockSpace,
                   const UIWindowManager& windowManager);

    /**
     * @brief Loads a layout from a file
     * @param filepath Path to load the layout from
     * @param rootDockSpace Root dock space to deserialize into
     * @param windowManager Window manager to deserialize window states into
     * @return True if load was successful
     */
    bool LoadLayout(const std::string& filepath,
                   std::shared_ptr<UIDockSpace>& rootDockSpace,
                   UIWindowManager& windowManager);

    /**
     * @brief Gets the version of the last loaded/saved layout
     * @return Layout version string
     */
    const std::string& GetLayoutVersion() const { return m_LayoutVersion; }

private:
    /**
     * @brief Serializes a dock space to JSON
     * @param dockSpace Dock space to serialize
     * @return JSON object
     */
    nlohmann::json SerializeDockSpace(std::shared_ptr<UIDockSpace> dockSpace);

    /**
     * @brief Deserializes a dock space from JSON
     * @param json JSON object to deserialize from
     * @return Deserialized dock space
     */
    std::shared_ptr<UIDockSpace> DeserializeDockSpace(const nlohmann::json& json);

    /**
     * @brief Serializes window states to JSON
     * @param windowManager Window manager to serialize states from
     * @return JSON object
     */
    nlohmann::json SerializeWindowStates(const UIWindowManager& windowManager);

    /**
     * @brief Deserializes window states from JSON
     * @param json JSON object to deserialize from
     * @param windowManager Window manager to deserialize states into
     */
    void DeserializeWindowStates(const nlohmann::json& json, UIWindowManager& windowManager);

    /**
     * @brief Validates a layout file
     * @param json JSON object to validate
     * @return True if layout is valid
     */
    bool ValidateLayout(const nlohmann::json& json);

    std::string m_LayoutVersion; ///< Version of the current layout
    static constexpr const char* CURRENT_VERSION = "1.0"; ///< Current layout format version
};

}} // namespace VGE::Editor



