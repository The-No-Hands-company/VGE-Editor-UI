#pragma once

#include "Runtime/Core/Public/Core/UICore.h"
#include "Runtime/Core/Public/Widget/UIWindow.h"
#include "Runtime/Core/Public/Widget/UIDockSpace.h"
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <nlohmann/json.hpp>

namespace VGE {
namespace UI {

/**
 * @brief Manages layout serialization and persistence
 * 
 * The UILayoutManager class provides:
 * - Layout saving and loading
 * - Layout presets
 * - Window state persistence
 * - Dock layout serialization
 * - Tab arrangement persistence
 */
class UILayoutManager {
public:
    /**
     * @brief Layout preset information
     */
    struct LayoutPreset {
        std::string name;           ///< Preset name
        std::string description;    ///< Preset description
        std::string category;       ///< Preset category
        bool isBuiltIn;            ///< Whether this is a built-in preset
        nlohmann::json data;       ///< Layout data
    };

    /**
     * @brief Constructor
     */
    UILayoutManager();

    /**
     * @brief Destructor
     */
    virtual ~UILayoutManager() = default;

    /**
     * @brief Initialize the layout manager
     */
    void Initialize();

    /**
     * @brief Save current layout to file
     * @param filepath Path to save file
     * @return True if successful
     */
    bool SaveLayout(const std::string& filepath);

    /**
     * @brief Load layout from file
     * @param filepath Path to load file
     * @return True if successful
     */
    bool LoadLayout(const std::string& filepath);

    /**
     * @brief Save current layout as preset
     * @param name Preset name
     * @param description Preset description
     * @param category Preset category
     * @return True if successful
     */
    bool SavePreset(const std::string& name, const std::string& description = "",
                   const std::string& category = "Custom");

    /**
     * @brief Load layout preset
     * @param name Preset name
     * @return True if successful
     */
    bool LoadPreset(const std::string& name);

    /**
     * @brief Get all available presets
     * @return Vector of preset information
     */
    std::vector<LayoutPreset> GetPresets() const;

    /**
     * @brief Delete a preset
     * @param name Preset name
     * @return True if successful
     */
    bool DeletePreset(const std::string& name);

    /**
     * @brief Export layout to string
     * @return JSON string containing layout data
     */
    std::string ExportLayout() const;

    /**
     * @brief Import layout from string
     * @param data JSON string containing layout data
     * @return True if successful
     */
    bool ImportLayout(const std::string& data);

private:
    std::unordered_map<std::string, LayoutPreset> m_Presets;  ///< Available presets
    std::string m_LastLoadedPreset;                          ///< Name of last loaded preset

    /**
     * @brief Load built-in presets
     */
    void LoadBuiltInPresets();

    /**
     * @brief Load user presets
     */
    void LoadUserPresets();

    /**
     * @brief Save presets to disk
     */
    void SavePresets();

    /**
     * @brief Serialize window states
     * @return JSON object containing window states
     */
    nlohmann::json SerializeWindowStates() const;

    /**
     * @brief Deserialize window states
     * @param json JSON object containing window states
     */
    void DeserializeWindowStates(const nlohmann::json& json);

    /**
     * @brief Serialize dock layout
     * @return JSON object containing dock layout
     */
    nlohmann::json SerializeDockLayout() const;

    /**
     * @brief Deserialize dock layout
     * @param json JSON object containing dock layout
     */
    void DeserializeDockLayout(const nlohmann::json& json);

    /**
     * @brief Serialize tab arrangements
     * @return JSON object containing tab arrangements
     */
    nlohmann::json SerializeTabArrangements() const;

    /**
     * @brief Deserialize tab arrangements
     * @param json JSON object containing tab arrangements
     */
    void DeserializeTabArrangements(const nlohmann::json& json);
};

}} // namespace VGE::UI 