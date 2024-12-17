#include "Layout/UILayoutPresetManager.h"
#include "Core/UICore.h"
#include <imgui.h>

namespace VGE {
namespace UI {

UILayoutPresetManager::UILayoutPresetManager()
    : UIWindow("Layout Presets", "Layout Presets")
    , m_StatusMessageTimer(0.0f)
{
    Initialize();
}

void UILayoutPresetManager::Initialize() {
    UIWindow::Initialize();
    m_LayoutManager = std::make_shared<UILayoutManager>();
    m_LayoutManager->Initialize();
    
    SetSize({500, 600});
    SetFlags(WindowFlags::Modal | WindowFlags::Resizable);
}

void UILayoutPresetManager::Render() {
    if (!IsVisible()) return;
    
    UpdateStatusMessage();
    
    ImGui::SetNextWindowSize(ImVec2(GetSize().x, GetSize().y), ImGuiCond_FirstUseEver);
    if (ImGui::Begin(GetTitle().c_str(), nullptr, ImGuiWindowFlags_NoCollapse)) {
        // Status message
        if (!m_StatusMessage.empty() && m_StatusMessageTimer > 0.0f) {
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", m_StatusMessage.c_str());
            ImGui::Separator();
        }
        
        // Main sections
        if (ImGui::BeginTabBar("PresetManagerTabs")) {
            if (ImGui::BeginTabItem("Presets")) {
                RenderPresetList();
                ImGui::EndTabItem();
            }
            
            if (ImGui::BeginTabItem("New Preset")) {
                RenderNewPresetSection();
                ImGui::EndTabItem();
            }
            
            if (ImGui::BeginTabItem("Import/Export")) {
                RenderImportExportSection();
                ImGui::EndTabItem();
            }
            
            ImGui::EndTabBar();
        }
    }
    ImGui::End();
}

void UILayoutPresetManager::Show() {
    SetVisible(true);
}

void UILayoutPresetManager::Hide() {
    SetVisible(false);
}

void UILayoutPresetManager::RenderPresetList() {
    ImGui::BeginChild("PresetList", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()));
    
    // Get presets grouped by category
    auto presets = m_LayoutManager->GetPresets();
    std::map<std::string, std::vector<UILayoutManager::LayoutPreset>> presetsByCategory;
    for (const auto& preset : presets) {
        presetsByCategory[preset.category].push_back(preset);
    }
    
    // Render presets by category
    for (const auto& [category, categoryPresets] : presetsByCategory) {
        if (ImGui::CollapsingHeader(category.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
            for (const auto& preset : categoryPresets) {
                ImGui::PushID(preset.name.c_str());
                
                // Preset name and description
                ImGui::BeginGroup();
                ImGui::Text("%s", preset.name.c_str());
                ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "%s", preset.description.c_str());
                ImGui::EndGroup();
                
                ImGui::SameLine(ImGui::GetWindowWidth() - 160);
                
                // Load button
                if (ImGui::Button("Load")) {
                    if (m_LayoutManager->LoadPreset(preset.name)) {
                        ShowStatusMessage("Layout loaded: " + preset.name);
                    }
                    else {
                        ShowStatusMessage("Failed to load layout: " + preset.name, true);
                    }
                }
                
                // Delete button (only for non-built-in presets)
                if (!preset.isBuiltIn) {
                    ImGui::SameLine();
                    if (ImGui::Button("Delete")) {
                        if (m_LayoutManager->DeletePreset(preset.name)) {
                            ShowStatusMessage("Preset deleted: " + preset.name);
                        }
                        else {
                            ShowStatusMessage("Failed to delete preset: " + preset.name, true);
                        }
                    }
                }
                
                ImGui::PopID();
            }
        }
    }
    
    ImGui::EndChild();
}

void UILayoutPresetManager::RenderNewPresetSection() {
    ImGui::BeginChild("NewPreset");
    
    // Input fields
    ImGui::Text("Create New Preset");
    ImGui::Separator();
    
    ImGui::Text("Name:");
    ImGui::InputText("##Name", &m_NewPresetName);
    
    ImGui::Text("Description:");
    ImGui::InputTextMultiline("##Description", &m_NewPresetDescription);
    
    ImGui::Text("Category:");
    ImGui::InputText("##Category", &m_NewPresetCategory);
    
    ImGui::Separator();
    
    // Create button
    if (ImGui::Button("Create Preset")) {
        if (m_NewPresetName.empty()) {
            ShowStatusMessage("Please enter a preset name", true);
        }
        else if (m_LayoutManager->SavePreset(m_NewPresetName, m_NewPresetDescription, m_NewPresetCategory)) {
            ShowStatusMessage("Preset created: " + m_NewPresetName);
            m_NewPresetName.clear();
            m_NewPresetDescription.clear();
            m_NewPresetCategory = "Custom";
        }
        else {
            ShowStatusMessage("Failed to create preset: " + m_NewPresetName, true);
        }
    }
    
    ImGui::EndChild();
}

void UILayoutPresetManager::RenderImportExportSection() {
    ImGui::BeginChild("ImportExport");
    
    // Export current layout
    if (ImGui::Button("Export Current Layout")) {
        m_ImportBuffer = m_LayoutManager->ExportLayout();
        ShowStatusMessage("Layout exported to buffer");
    }
    
    ImGui::Separator();
    
    // Import layout
    ImGui::Text("Import Layout:");
    ImGui::InputTextMultiline("##Import", &m_ImportBuffer, ImVec2(-1, 200));
    
    if (ImGui::Button("Import")) {
        if (m_ImportBuffer.empty()) {
            ShowStatusMessage("Import buffer is empty", true);
        }
        else if (m_LayoutManager->ImportLayout(m_ImportBuffer)) {
            ShowStatusMessage("Layout imported successfully");
        }
        else {
            ShowStatusMessage("Failed to import layout", true);
        }
    }
    
    ImGui::EndChild();
}

void UILayoutPresetManager::ShowStatusMessage(const std::string& message, bool isError) {
    m_StatusMessage = message;
    m_StatusMessageTimer = 3.0f; // Show for 3 seconds
}

void UILayoutPresetManager::UpdateStatusMessage() {
    if (m_StatusMessageTimer > 0.0f) {
        m_StatusMessageTimer -= ImGui::GetIO().DeltaTime;
        if (m_StatusMessageTimer <= 0.0f) {
            m_StatusMessage.clear();
        }
    }
}

}} // namespace VGE::UI 