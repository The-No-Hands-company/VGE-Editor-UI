#pragma once

#include "Runtime/Core/Public/Core/UICore.h"
#include <string>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>
#include "Runtime/Core/Public/Base/VGE.h"

namespace VGE {
namespace UI {

struct UIPanelLayout {
    std::string name;
    std::string type;
    bool isVisible = true;
    bool isOpen = true;
    glm::vec2 position = {0, 0};
    glm::vec2 size = {300, 200};
    std::string dockSpace;
    int dockId = -1;
};

struct UIDockSpaceLayout {
    std::string name;
    glm::vec2 position = {0, 0};
    glm::vec2 size = {800, 600};
    bool isVisible = true;
    std::vector<std::string> panels;
};

/**
 * @class UILayout
 * @brief Manages UI layout configuration including window positions, sizes, and docking
 */
class UILayout {
public:
    static UILayout& Instance() {
        static UILayout instance;
        return instance;
    }

    // Layout management
    void SaveLayout(const std::string& name);
    void LoadLayout(const std::string& name);
    void ResetToDefault();

    // Panel management
    void AddPanel(const UIPanelLayout& panel);
    void RemovePanel(const std::string& name);
    UIPanelLayout* GetPanel(const std::string& name);
    const std::vector<UIPanelLayout>& GetPanels() const { return m_Panels; }

    // DockSpace management
    void AddDockSpace(const UIDockSpaceLayout& dockSpace);
    void RemoveDockSpace(const std::string& name);
    UIDockSpaceLayout* GetDockSpace(const std::string& name);
    const std::vector<UIDockSpaceLayout>& GetDockSpaces() const { return m_DockSpaces; }

    // Default layout configuration
    void SetDefaultLayout();

private:
    UILayout() = default;
    ~UILayout() = default;
    UILayout(const UILayout&) = delete;
    UILayout& operator=(const UILayout&) = delete;

    // Serialization helpers
    nlohmann::json SerializeLayout() const;
    void DeserializeLayout(const nlohmann::json& json);
    void SaveToFile(const std::string& name, const nlohmann::json& json);
    nlohmann::json LoadFromFile(const std::string& name);

    // Layout configuration
    std::vector<UIPanelLayout> m_Panels;
    std::vector<UIDockSpaceLayout> m_DockSpaces;
    std::string m_CurrentLayout;
};

}} // namespace VGE::UI



