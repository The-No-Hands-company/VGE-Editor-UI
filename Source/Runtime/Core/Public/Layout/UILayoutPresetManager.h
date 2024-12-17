#pragma once

#include "Runtime/Core/Public/Core/UICore.h"
#include "Runtime/Core/Public/Layout/UILayoutManager.h"
#include "Runtime/Core/Public/Widget/UIWindow.h"
#include <string>
#include <memory>
#include <vector>

namespace VGE {
namespace UI {

/**
 * @brief UI for managing layout presets
 * 
 * Features:
 * - List available presets
 * - Load/save presets
 * - Create/delete presets
 * - Import/export presets
 */
class UILayoutPresetManager : public UIWindow {
public:
    /**
     * @brief Constructor
     */
    UILayoutPresetManager();

    /**
     * @brief Destructor
     */
    virtual ~UILayoutPresetManager() = default;

    /**
     * @brief Initialize the preset manager
     */
    void Initialize() override;

    /**
     * @brief Render the preset manager UI
     */
    void Render() override;

    /**
     * @brief Show the preset manager window
     */
    void Show();

    /**
     * @brief Hide the preset manager window
     */
    void Hide();

    /**
     * @brief Get the window type name
     * @return Type name string
     */
    std::string GetTypeName() const override { return "LayoutPresetManager"; }

private:
    std::shared_ptr<UILayoutManager> m_LayoutManager;  ///< Layout manager instance
    std::string m_NewPresetName;                      ///< Name for new preset
    std::string m_NewPresetDescription;               ///< Description for new preset
    std::string m_NewPresetCategory;                  ///< Category for new preset
    std::string m_ImportBuffer;                       ///< Buffer for importing layout data
    std::string m_StatusMessage;                      ///< Status/error message
    float m_StatusMessageTimer;                       ///< Timer for status message display

    /**
     * @brief Render the preset list section
     */
    void RenderPresetList();

    /**
     * @brief Render the new preset section
     */
    void RenderNewPresetSection();

    /**
     * @brief Render the import/export section
     */
    void RenderImportExportSection();

    /**
     * @brief Show a status message
     * @param message Message to display
     * @param isError Whether this is an error message
     */
    void ShowStatusMessage(const std::string& message, bool isError = false);

    /**
     * @brief Update status message timer
     */
    void UpdateStatusMessage();
};

}} // namespace VGE::UI 