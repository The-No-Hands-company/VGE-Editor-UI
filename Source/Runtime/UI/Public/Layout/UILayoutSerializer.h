#pragma once

#include "Runtime/Core/Public/Core/UICore.h"
#include "Runtime/UI/Public/Docking/UIDockSpace.h"
#include <memory>
#include <string>
#include <nlohmann/json.hpp>

namespace VGE {
namespace Editor {

/**
 * @brief Serializes and deserializes UI layout configurations
 */
class UILayoutSerializer {
public:
    /**
     * @brief Save the current layout to a file
     * @param filepath Path to save the layout file
     * @param dockSpace Root dock space to serialize
     * @return True if save was successful
     */
    static bool SaveLayout(const std::string& filepath, std::shared_ptr<UIDockSpace> dockSpace);

    /**
     * @brief Load a layout from a file
     * @param filepath Path to the layout file
     * @param dockSpace Root dock space to load into
     * @return True if load was successful
     */
    static bool LoadLayout(const std::string& filepath, std::shared_ptr<UIDockSpace> dockSpace);

private:
    // Serialization helpers
    static nlohmann::json SerializeDockSpace(const UIDockSpace& dockSpace);
    static nlohmann::json SerializeSplit(const DockSplit& split);
    static nlohmann::json SerializeDockedWindow(const DockedWindow& window);
    static nlohmann::json SerializeTabManager(const UITabManager& tabManager);

    // Deserialization helpers
    static bool DeserializeDockSpace(const nlohmann::json& json, UIDockSpace& dockSpace);
    static bool DeserializeSplit(const nlohmann::json& json, DockSplit& split);
    static bool DeserializeDockedWindow(const nlohmann::json& json, DockedWindow& window);
    static bool DeserializeTabManager(const nlohmann::json& json, UITabManager& tabManager);

    // Version handling
    static constexpr int CURRENT_VERSION = 1;
    static bool ValidateVersion(const nlohmann::json& json);
};

}} // namespace VGE::Editor 