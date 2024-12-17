#include "Layout/UILayoutManager.h"
#include <fstream>
#include <filesystem>

namespace VGE {
namespace UI {

UILayoutManager::UILayoutManager() {
    Initialize();
}

void UILayoutManager::Initialize() {
    // Load built-in and user presets
    LoadBuiltInPresets();
    LoadUserPresets();
}

bool UILayoutManager::SaveLayout(const std::string& filepath) {
    try {
        nlohmann::json layout;

        // Serialize all components
        layout["windows"] = SerializeWindowStates();
        layout["dockLayout"] = SerializeDockLayout();
        layout["tabArrangements"] = SerializeTabArrangements();
        layout["version"] = 1;  // For future compatibility

        // Write to file
        std::ofstream file(filepath);
        file << layout.dump(4);
        return true;
    }
    catch (const std::exception& e) {
        // Log error
        return false;
    }
}

bool UILayoutManager::LoadLayout(const std::string& filepath) {
    try {
        // Read file
        std::ifstream file(filepath);
        nlohmann::json layout;
        file >> layout;

        // Check version
        int version = layout["version"].get<int>();
        if (version > 1) {
            // Log warning about newer version
        }

        // Deserialize all components
        DeserializeWindowStates(layout["windows"]);
        DeserializeDockLayout(layout["dockLayout"]);
        DeserializeTabArrangements(layout["tabArrangements"]);

        return true;
    }
    catch (const std::exception& e) {
        // Log error
        return false;
    }
}

bool UILayoutManager::SavePreset(const std::string& name, const std::string& description,
                               const std::string& category) {
    try {
        // Create preset
        LayoutPreset preset;
        preset.name = name;
        preset.description = description;
        preset.category = category;
        preset.isBuiltIn = false;
        
        // Export current layout
        nlohmann::json layout;
        layout["windows"] = SerializeWindowStates();
        layout["dockLayout"] = SerializeDockLayout();
        layout["tabArrangements"] = SerializeTabArrangements();
        preset.data = layout;

        // Store preset
        m_Presets[name] = preset;
        SavePresets();

        return true;
    }
    catch (const std::exception& e) {
        // Log error
        return false;
    }
}

bool UILayoutManager::LoadPreset(const std::string& name) {
    auto it = m_Presets.find(name);
    if (it == m_Presets.end()) {
        return false;
    }

    try {
        const auto& preset = it->second;
        
        // Apply layout from preset
        DeserializeWindowStates(preset.data["windows"]);
        DeserializeDockLayout(preset.data["dockLayout"]);
        DeserializeTabArrangements(preset.data["tabArrangements"]);

        m_LastLoadedPreset = name;
        return true;
    }
    catch (const std::exception& e) {
        // Log error
        return false;
    }
}

std::vector<UILayoutManager::LayoutPreset> UILayoutManager::GetPresets() const {
    std::vector<LayoutPreset> presets;
    presets.reserve(m_Presets.size());
    
    for (const auto& [name, preset] : m_Presets) {
        presets.push_back(preset);
    }
    
    return presets;
}

bool UILayoutManager::DeletePreset(const std::string& name) {
    auto it = m_Presets.find(name);
    if (it == m_Presets.end() || it->second.isBuiltIn) {
        return false;
    }

    m_Presets.erase(it);
    SavePresets();
    return true;
}

std::string UILayoutManager::ExportLayout() const {
    nlohmann::json layout;
    layout["windows"] = SerializeWindowStates();
    layout["dockLayout"] = SerializeDockLayout();
    layout["tabArrangements"] = SerializeTabArrangements();
    layout["version"] = 1;
    return layout.dump(4);
}

bool UILayoutManager::ImportLayout(const std::string& data) {
    try {
        nlohmann::json layout = nlohmann::json::parse(data);
        
        // Check version
        int version = layout["version"].get<int>();
        if (version > 1) {
            // Log warning about newer version
        }

        // Apply layout
        DeserializeWindowStates(layout["windows"]);
        DeserializeDockLayout(layout["dockLayout"]);
        DeserializeTabArrangements(layout["tabArrangements"]);

        return true;
    }
    catch (const std::exception& e) {
        // Log error
        return false;
    }
}

void UILayoutManager::LoadBuiltInPresets() {
    // Default layout preset
    {
        LayoutPreset defaultPreset;
        defaultPreset.name = "Default";
        defaultPreset.description = "Default editor layout with property grid and scene view";
        defaultPreset.category = "Built-in";
        defaultPreset.isBuiltIn = true;
        
        // Create default layout
        nlohmann::json layout;
        
        // Main window
        layout["windows"] = {
            {"MainWindow", {
                {"name", "MainWindow"},
                {"title", "Editor"},
                {"isVisible", true},
                {"isMinimized", false},
                {"isMaximized", false},
                {"position", {100, 100}},
                {"size", {1280, 720}},
                {"type", 0},
                {"flags", 0}
            }}
        };
        
        // Dock layout with scene view and property grid
        layout["dockLayout"] = {
            {"dockSpaces", {
                {
                    {"id", "MainDockSpace"},
                    {"name", "MainDockSpace"},
                    {"position", {0, 0}},
                    {"size", {1280, 720}},
                    {"isSplit", true},
                    {"isVertical", false},
                    {"splitRatio", 0.7f},
                    {"children", {"SceneView", "PropertyGrid"}},
                    {"windows", nlohmann::json::array()}
                },
                {
                    {"id", "SceneView"},
                    {"name", "SceneView"},
                    {"position", {0, 0}},
                    {"size", {896, 720}},
                    {"isSplit", false},
                    {"windows", {"SceneViewWindow"}}
                },
                {
                    {"id", "PropertyGrid"},
                    {"name", "PropertyGrid"},
                    {"position", {896, 0}},
                    {"size", {384, 720}},
                    {"isSplit", false},
                    {"windows", {"PropertyGridWindow"}}
                }
            }},
            {"relationships", {
                {
                    {"source", "SceneView"},
                    {"target", "PropertyGrid"},
                    {"zone", 2} // DockZone::Right
                }
            }}
        };
        
        defaultPreset.data = layout;
        m_Presets["Default"] = defaultPreset;
    }
    
    // Dual view layout preset
    {
        LayoutPreset dualViewPreset;
        dualViewPreset.name = "Dual View";
        dualViewPreset.description = "Split screen with two scene views";
        dualViewPreset.category = "Built-in";
        dualViewPreset.isBuiltIn = true;
        
        nlohmann::json layout;
        
        // Main window setup
        layout["windows"] = {
            {"MainWindow", {
                {"name", "MainWindow"},
                {"title", "Editor - Dual View"},
                {"isVisible", true},
                {"isMinimized", false},
                {"isMaximized", false},
                {"position", {100, 100}},
                {"size", {1280, 720}},
                {"type", 0},
                {"flags", 0}
            }}
        };
        
        // Dock layout with two scene views
        layout["dockLayout"] = {
            {"dockSpaces", {
                {
                    {"id", "MainDockSpace"},
                    {"name", "MainDockSpace"},
                    {"position", {0, 0}},
                    {"size", {1280, 720}},
                    {"isSplit", true},
                    {"isVertical", true},
                    {"splitRatio", 0.5f},
                    {"children", {"TopView", "BottomView"}},
                    {"windows", nlohmann::json::array()}
                },
                {
                    {"id", "TopView"},
                    {"name", "TopView"},
                    {"position", {0, 0}},
                    {"size", {1280, 360}},
                    {"isSplit", false},
                    {"windows", {"SceneViewWindow1"}}
                },
                {
                    {"id", "BottomView"},
                    {"name", "BottomView"},
                    {"position", {0, 360}},
                    {"size", {1280, 360}},
                    {"isSplit", false},
                    {"windows", {"SceneViewWindow2"}}
                }
            }},
            {"relationships", {
                {
                    {"source", "TopView"},
                    {"target", "BottomView"},
                    {"zone", 1} // DockZone::Bottom
                }
            }}
        };
        
        dualViewPreset.data = layout;
        m_Presets["DualView"] = dualViewPreset;
    }
    
    // Compact layout preset
    {
        LayoutPreset compactPreset;
        compactPreset.name = "Compact";
        compactPreset.description = "Space-efficient layout with tabbed views";
        compactPreset.category = "Built-in";
        compactPreset.isBuiltIn = true;
        
        nlohmann::json layout;
        
        // Main window setup
        layout["windows"] = {
            {"MainWindow", {
                {"name", "MainWindow"},
                {"title", "Editor - Compact"},
                {"isVisible", true},
                {"isMinimized", false},
                {"isMaximized", false},
                {"position", {100, 100}},
                {"size", {1024, 768}},
                {"type", 0},
                {"flags", 0}
            }}
        };
        
        // Tab arrangement for compact view
        layout["tabArrangements"] = {
            {"containers", {
                {
                    {"id", "MainTabs"},
                    {"name", "MainTabs"},
                    {"position", {0, 0}},
                    {"size", {1024, 768}},
                    {"activeTab", "SceneTab"},
                    {"tabs", {
                        {
                            {"name", "SceneTab"},
                            {"title", "Scene"},
                            {"isVisible", true},
                            {"canClose", false},
                            {"order", 0},
                            {"content", "SceneViewWindow"},
                            {"contentType", "SceneView"}
                        },
                        {
                            {"name", "PropertiesTab"},
                            {"title", "Properties"},
                            {"isVisible", true},
                            {"canClose", false},
                            {"order", 1},
                            {"content", "PropertyGridWindow"},
                            {"contentType", "PropertyGrid"}
                        }
                    }}
                }
            }}
        };
        
        compactPreset.data = layout;
        m_Presets["Compact"] = compactPreset;
    }
}

void UILayoutManager::LoadUserPresets() {
    try {
        std::filesystem::path presetsPath = "layouts/presets";
        if (!std::filesystem::exists(presetsPath)) {
            return;
        }

        for (const auto& entry : std::filesystem::directory_iterator(presetsPath)) {
            if (entry.path().extension() == ".json") {
                std::ifstream file(entry.path());
                nlohmann::json presetJson;
                file >> presetJson;

                LayoutPreset preset;
                preset.name = presetJson["name"].get<std::string>();
                preset.description = presetJson["description"].get<std::string>();
                preset.category = presetJson["category"].get<std::string>();
                preset.isBuiltIn = false;
                preset.data = presetJson["data"];

                m_Presets[preset.name] = preset;
            }
        }
    }
    catch (const std::exception& e) {
        // Log error
    }
}

void UILayoutManager::SavePresets() {
    try {
        std::filesystem::path presetsPath = "layouts/presets";
        std::filesystem::create_directories(presetsPath);

        for (const auto& [name, preset] : m_Presets) {
            if (preset.isBuiltIn) {
                continue;
            }

            nlohmann::json presetJson;
            presetJson["name"] = preset.name;
            presetJson["description"] = preset.description;
            presetJson["category"] = preset.category;
            presetJson["data"] = preset.data;

            std::ofstream file(presetsPath / (name + ".json"));
            file << presetJson.dump(4);
        }
    }
    catch (const std::exception& e) {
        // Log error
    }
}

nlohmann::json UILayoutManager::SerializeWindowStates() const {
    nlohmann::json json;
    
    // Get window manager instance
    auto& windowManager = UIWindowManager::Get();
    
    // Serialize each window
    for (const auto& window : windowManager.GetWindows()) {
        nlohmann::json windowJson;
        
        // Basic window properties
        windowJson["name"] = window->GetName();
        windowJson["title"] = window->GetTitle();
        windowJson["isVisible"] = window->IsVisible();
        windowJson["isMinimized"] = window->IsMinimized();
        windowJson["isMaximized"] = window->IsMaximized();
        
        // Window position and size
        auto pos = window->GetPosition();
        auto size = window->GetSize();
        windowJson["position"] = {pos.x, pos.y};
        windowJson["size"] = {size.x, size.y};
        
        // Window type and flags
        windowJson["type"] = static_cast<int>(window->GetType());
        windowJson["flags"] = window->GetFlags();
        
        // Monitor info
        if (auto monitor = window->GetMonitor()) {
            windowJson["monitor"] = monitor->GetName();
        }
        
        json[window->GetName()] = windowJson;
    }
    
    return json;
}

void UILayoutManager::DeserializeWindowStates(const nlohmann::json& json) {
    auto& windowManager = UIWindowManager::Get();
    
    // Process each window
    for (auto it = json.begin(); it != json.end(); ++it) {
        const std::string& name = it.key();
        const auto& windowJson = it.value();
        
        // Find or create window
        auto window = windowManager.FindWindow(name);
        if (!window) {
            // Skip windows that don't exist anymore
            continue;
        }
        
        // Restore window properties
        window->SetTitle(windowJson["title"].get<std::string>());
        window->SetVisible(windowJson["isVisible"].get<bool>());
        
        // Restore window state
        if (windowJson["isMinimized"].get<bool>()) {
            window->Minimize();
        }
        else if (windowJson["isMaximized"].get<bool>()) {
            window->Maximize();
        }
        else {
            window->Restore();
        }
        
        // Restore position and size
        auto pos = windowJson["position"];
        auto size = windowJson["size"];
        window->SetPosition({pos[0].get<float>(), pos[1].get<float>()});
        window->SetSize({size[0].get<float>(), size[1].get<float>()});
        
        // Restore window flags
        window->SetFlags(windowJson["flags"].get<uint32_t>());
        
        // Restore monitor
        if (windowJson.contains("monitor")) {
            std::string monitorName = windowJson["monitor"].get<std::string>();
            if (auto monitor = windowManager.FindMonitor(monitorName)) {
                window->SetMonitor(monitor);
            }
        }
    }
}

nlohmann::json UILayoutManager::SerializeDockLayout() const {
    nlohmann::json json;
    
    // Get dock manager instance
    auto& dockManager = UIDockManager::Get();
    
    // Serialize dock spaces
    json["dockSpaces"] = nlohmann::json::array();
    for (const auto& dockSpace : dockManager.GetDockSpaces()) {
        nlohmann::json dockJson;
        
        // Basic properties
        dockJson["id"] = dockSpace->GetId();
        dockJson["name"] = dockSpace->GetName();
        
        // Position and size
        auto pos = dockSpace->GetPosition();
        auto size = dockSpace->GetSize();
        dockJson["position"] = {pos.x, pos.y};
        dockJson["size"] = {size.x, size.y};
        
        // Split configuration
        if (dockSpace->IsSplit()) {
            dockJson["isSplit"] = true;
            dockJson["isVertical"] = dockSpace->IsVerticalSplit();
            dockJson["splitRatio"] = dockSpace->GetSplitRatio();
            
            // Serialize child dock spaces
            dockJson["children"] = nlohmann::json::array();
            for (const auto& child : dockSpace->GetChildren()) {
                dockJson["children"].push_back(child->GetId());
            }
        }
        else {
            dockJson["isSplit"] = false;
        }
        
        // Docked windows
        dockJson["windows"] = nlohmann::json::array();
        for (const auto& window : dockSpace->GetDockedWindows()) {
            dockJson["windows"].push_back(window.name);
        }
        
        json["dockSpaces"].push_back(dockJson);
    }
    
    // Serialize dock relationships
    json["relationships"] = nlohmann::json::array();
    for (const auto& rel : dockManager.GetDockRelationships()) {
        nlohmann::json relJson;
        relJson["source"] = rel.source;
        relJson["target"] = rel.target;
        relJson["zone"] = static_cast<int>(rel.zone);
        json["relationships"].push_back(relJson);
    }
    
    return json;
}

void UILayoutManager::DeserializeDockLayout(const nlohmann::json& json) {
    auto& dockManager = UIDockManager::Get();
    
    // Clear existing layout
    dockManager.Clear();
    
    // First pass: Create dock spaces
    std::unordered_map<std::string, std::shared_ptr<UIDockSpace>> dockSpaces;
    for (const auto& dockJson : json["dockSpaces"]) {
        std::string id = dockJson["id"].get<std::string>();
        std::string name = dockJson["name"].get<std::string>();
        
        auto dockSpace = dockManager.CreateDockSpace(id, name);
        dockSpaces[id] = dockSpace;
        
        // Restore position and size
        auto pos = dockJson["position"];
        auto size = dockJson["size"];
        dockSpace->SetPosition({pos[0].get<float>(), pos[1].get<float>()});
        dockSpace->SetSize({size[0].get<float>(), size[1].get<float>()});
    }
    
    // Second pass: Setup splits and dock windows
    for (const auto& dockJson : json["dockSpaces"]) {
        std::string id = dockJson["id"].get<std::string>();
        auto dockSpace = dockSpaces[id];
        
        // Setup split if needed
        if (dockJson["isSplit"].get<bool>()) {
            bool isVertical = dockJson["isVertical"].get<bool>();
            float ratio = dockJson["splitRatio"].get<float>();
            
            // Get child dock spaces
            std::vector<std::shared_ptr<UIDockSpace>> children;
            for (const auto& childId : dockJson["children"]) {
                if (auto child = dockSpaces[childId.get<std::string>()]) {
                    children.push_back(child);
                }
            }
            
            if (children.size() == 2) {
                dockSpace->Split(isVertical, ratio);
                dockSpace->SetChildren(children[0], children[1]);
            }
        }
        
        // Dock windows
        for (const auto& windowName : dockJson["windows"]) {
            if (auto window = UIWindowManager::Get().FindWindow(windowName.get<std::string>())) {
                dockSpace->DockWindow(window, windowName.get<std::string>());
            }
        }
    }
    
    // Finally, restore relationships
    for (const auto& relJson : json["relationships"]) {
        std::string source = relJson["source"].get<std::string>();
        std::string target = relJson["target"].get<std::string>();
        DockZone zone = static_cast<DockZone>(relJson["zone"].get<int>());
        
        if (auto sourceDock = dockSpaces[source]) {
            if (auto targetDock = dockSpaces[target]) {
                dockManager.AddDockRelationship(sourceDock, targetDock, zone);
            }
        }
    }
}

nlohmann::json UILayoutManager::SerializeTabArrangements() const {
    nlohmann::json json;
    
    // Get tab system instance
    auto& tabSystem = UITabSystem::Get();
    
    // Serialize tab containers
    json["containers"] = nlohmann::json::array();
    for (const auto& container : tabSystem.GetTabContainers()) {
        nlohmann::json containerJson;
        
        // Basic properties
        containerJson["id"] = container->GetId();
        containerJson["name"] = container->GetName();
        
        // Position and size
        auto pos = container->GetPosition();
        auto size = container->GetSize();
        containerJson["position"] = {pos.x, pos.y};
        containerJson["size"] = {size.x, size.y};
        
        // Active tab
        if (auto activeTab = container->GetActiveTab()) {
            containerJson["activeTab"] = activeTab->GetName();
        }
        
        // Tabs
        containerJson["tabs"] = nlohmann::json::array();
        for (const auto& tab : container->GetTabs()) {
            nlohmann::json tabJson;
            tabJson["name"] = tab->GetName();
            tabJson["title"] = tab->GetTitle();
            tabJson["isVisible"] = tab->IsVisible();
            tabJson["canClose"] = tab->CanClose();
            tabJson["order"] = tab->GetOrder();
            
            // Tab content
            if (auto content = tab->GetContent()) {
                tabJson["content"] = content->GetName();
                tabJson["contentType"] = content->GetTypeName();
            }
            
            containerJson["tabs"].push_back(tabJson);
        }
        
        json["containers"].push_back(containerJson);
    }
    
    // Serialize tab groups
    json["groups"] = nlohmann::json::array();
    for (const auto& group : tabSystem.GetTabGroups()) {
        nlohmann::json groupJson;
        groupJson["id"] = group->GetId();
        groupJson["name"] = group->GetName();
        
        // Container references
        groupJson["containers"] = nlohmann::json::array();
        for (const auto& container : group->GetContainers()) {
            groupJson["containers"].push_back(container->GetId());
        }
        
        json["groups"].push_back(groupJson);
    }
    
    return json;
}

void UILayoutManager::DeserializeTabArrangements(const nlohmann::json& json) {
    auto& tabSystem = UITabSystem::Get();
    
    // Clear existing arrangements
    tabSystem.Clear();
    
    // First pass: Create tab containers
    std::unordered_map<std::string, std::shared_ptr<UITabContainer>> containers;
    for (const auto& containerJson : json["containers"]) {
        std::string id = containerJson["id"].get<std::string>();
        std::string name = containerJson["name"].get<std::string>();
        
        auto container = tabSystem.CreateTabContainer(id, name);
        containers[id] = container;
        
        // Restore position and size
        auto pos = containerJson["position"];
        auto size = containerJson["size"];
        container->SetPosition({pos[0].get<float>(), pos[1].get<float>()});
        container->SetSize({size[0].get<float>(), size[1].get<float>()});
    }
    
    // Second pass: Create tabs and restore content
    for (const auto& containerJson : json["containers"]) {
        std::string id = containerJson["id"].get<std::string>();
        auto container = containers[id];
        
        // Create tabs
        for (const auto& tabJson : containerJson["tabs"]) {
            std::string name = tabJson["name"].get<std::string>();
            std::string title = tabJson["title"].get<std::string>();
            
            auto tab = container->AddTab(name, title);
            tab->SetVisible(tabJson["isVisible"].get<bool>());
            tab->SetCanClose(tabJson["canClose"].get<bool>());
            tab->SetOrder(tabJson["order"].get<int>());
            
            // Restore content
            if (tabJson.contains("content") && tabJson.contains("contentType")) {
                std::string contentName = tabJson["content"].get<std::string>();
                std::string contentType = tabJson["contentType"].get<std::string>();
                
                if (auto content = UIWindowManager::Get().FindWindow(contentName)) {
                    tab->SetContent(content);
                }
            }
        }
        
        // Restore active tab
        if (containerJson.contains("activeTab")) {
            std::string activeTabName = containerJson["activeTab"].get<std::string>();
            if (auto tab = container->FindTab(activeTabName)) {
                container->SetActiveTab(tab);
            }
        }
    }
    
    // Finally, create tab groups
    for (const auto& groupJson : json["groups"]) {
        std::string id = groupJson["id"].get<std::string>();
        std::string name = groupJson["name"].get<std::string>();
        
        auto group = tabSystem.CreateTabGroup(id, name);
        
        // Add containers to group
        for (const auto& containerId : groupJson["containers"]) {
            if (auto container = containers[containerId.get<std::string>()]) {
                group->AddContainer(container);
            }
        }
    }
}

}} // namespace VGE::UI 