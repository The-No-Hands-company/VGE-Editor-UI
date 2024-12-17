#include "Runtime/UI/Public/Layout/UILayoutSerializer.h"
#include "Runtime/Core/Public/Core/Logger.h"
#include "Runtime/UI/Public/Controls/UITabManager.h"
#include <fstream>

namespace VGE {
namespace Editor {

bool UILayoutSerializer::SaveLayout(const std::string& filepath, std::shared_ptr<UIDockSpace> dockSpace) {
    if (!dockSpace) {
        Logger::Log(LogLevel::Error, "Cannot save layout: null dock space");
        return false;
    }

    try {
        nlohmann::json json;
        json["version"] = CURRENT_VERSION;
        json["dockSpace"] = SerializeDockSpace(*dockSpace);

        // Write to file
        std::ofstream file(filepath);
        if (!file.is_open()) {
            Logger::Log(LogLevel::Error, "Failed to open file for writing: {}", filepath);
            return false;
        }

        file << json.dump(4);
        return true;
    }
    catch (const std::exception& e) {
        Logger::Log(LogLevel::Error, "Failed to save layout: {}", e.what());
        return false;
    }
}

bool UILayoutSerializer::LoadLayout(const std::string& filepath, std::shared_ptr<UIDockSpace> dockSpace) {
    if (!dockSpace) {
        Logger::Log(LogLevel::Error, "Cannot load layout: null dock space");
        return false;
    }

    try {
        // Read file
        std::ifstream file(filepath);
        if (!file.is_open()) {
            Logger::Log(LogLevel::Error, "Failed to open file for reading: {}", filepath);
            return false;
        }

        nlohmann::json json = nlohmann::json::parse(file);

        // Validate version
        if (!ValidateVersion(json)) {
            return false;
        }

        // Deserialize dock space
        return DeserializeDockSpace(json["dockSpace"], *dockSpace);
    }
    catch (const std::exception& e) {
        Logger::Log(LogLevel::Error, "Failed to load layout: {}", e.what());
        return false;
    }
}

nlohmann::json UILayoutSerializer::SerializeDockSpace(const UIDockSpace& dockSpace) {
    nlohmann::json json;
    json["name"] = dockSpace.GetName();

    // Serialize splits
    auto& splits = json["splits"] = nlohmann::json::array();
    for (const auto& split : dockSpace.GetSplits()) {
        splits.push_back(SerializeSplit(split));
    }

    // Serialize docked windows
    auto& windows = json["windows"] = nlohmann::json::array();
    for (const auto& [name, window] : dockSpace.GetDockedWindows()) {
        auto windowJson = SerializeDockedWindow(window);
        windowJson["name"] = name;
        windows.push_back(windowJson);
    }

    // Serialize root tabs
    if (auto rootTabs = dockSpace.GetRootTabs()) {
        json["rootTabs"] = SerializeTabManager(*rootTabs);
    }

    return json;
}

nlohmann::json UILayoutSerializer::SerializeSplit(const DockSplit& split) {
    nlohmann::json json;
    json["isVertical"] = split.isVertical;
    json["ratio"] = split.ratio;

    if (split.leftTabs) {
        json["leftTabs"] = SerializeTabManager(*split.leftTabs);
    }
    if (split.rightTabs) {
        json["rightTabs"] = SerializeTabManager(*split.rightTabs);
    }

    return json;
}

nlohmann::json UILayoutSerializer::SerializeDockedWindow(const DockedWindow& window) {
    nlohmann::json json;
    json["title"] = window.title;
    json["isVisible"] = window.isVisible;
    json["isActive"] = window.isActive;
    // Note: Widget pointer is not serialized, must be restored by the application
    return json;
}

nlohmann::json UILayoutSerializer::SerializeTabManager(const UITabManager& tabManager) {
    nlohmann::json json;
    auto& tabs = json["tabs"] = nlohmann::json::array();

    for (const auto& tab : tabManager.GetTabs()) {
        nlohmann::json tabJson;
        tabJson["id"] = tab.id;
        tabJson["label"] = tab.label;
        tabJson["canClose"] = tab.canClose;
        tabJson["isActive"] = tab.isActive;
        tabs.push_back(tabJson);
    }

    return json;
}

bool UILayoutSerializer::DeserializeDockSpace(const nlohmann::json& json, UIDockSpace& dockSpace) {
    // Clear existing state
    dockSpace.Clear();  // Note: Need to add Clear() method to UIDockSpace

    // Deserialize splits
    if (json.contains("splits")) {
        for (const auto& splitJson : json["splits"]) {
            DockSplit split;
            if (DeserializeSplit(splitJson, split)) {
                dockSpace.AddSplit(split);  // Note: Need to add AddSplit() method to UIDockSpace
            }
        }
    }

    // Deserialize windows
    if (json.contains("windows")) {
        for (const auto& windowJson : json["windows"]) {
            DockedWindow window;
            if (DeserializeDockedWindow(windowJson, window)) {
                dockSpace.AddDockedWindow(window);  // Note: Need to add AddDockedWindow() method to UIDockSpace
            }
        }
    }

    // Deserialize root tabs
    if (json.contains("rootTabs") && dockSpace.GetRootTabs()) {
        DeserializeTabManager(json["rootTabs"], *dockSpace.GetRootTabs());
    }

    return true;
}

bool UILayoutSerializer::DeserializeSplit(const nlohmann::json& json, DockSplit& split) {
    split.isVertical = json.value("isVertical", true);
    split.ratio = json.value("ratio", 0.5f);

    // Tabs will be restored by the application
    return true;
}

bool UILayoutSerializer::DeserializeDockedWindow(const nlohmann::json& json, DockedWindow& window) {
    window.title = json.value("title", "");
    window.isVisible = json.value("isVisible", true);
    window.isActive = json.value("isActive", false);
    // Widget pointer must be restored by the application
    return true;
}

bool UILayoutSerializer::DeserializeTabManager(const nlohmann::json& json, UITabManager& tabManager) {
    if (!json.contains("tabs")) {
        return false;
    }

    for (const auto& tabJson : json["tabs"]) {
        std::string id = tabJson.value("id", "");
        std::string label = tabJson.value("label", "");
        bool canClose = tabJson.value("canClose", true);
        bool isActive = tabJson.value("isActive", false);

        // Note: Content must be restored by the application
        tabManager.AddTab(id, label, nullptr, canClose);
    }

    return true;
}

bool UILayoutSerializer::ValidateVersion(const nlohmann::json& json) {
    if (!json.contains("version")) {
        Logger::Log(LogLevel::Error, "Layout file missing version information");
        return false;
    }

    int version = json["version"];
    if (version > CURRENT_VERSION) {
        Logger::Log(LogLevel::Error, "Layout file version {} is newer than current version {}", 
                   version, CURRENT_VERSION);
        return false;
    }

    return true;
}

}} // namespace VGE::Editor 