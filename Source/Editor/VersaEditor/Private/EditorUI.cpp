#include "U

#include "Runtime/Core/Public/Core/UICore.h"I/EditorUI.h"
#include "UI/Framework/Core/UIWindow.h"
#include "UI/Editor/Windows/EditorWindow.h"
#include "UI/Editor/Panels/EditorPanel.h"
#include "UI/Framework/Theme/UITheme.h"

namespace VGE {
namespace Editor {

EditorUI& EditorUI::Get()
{
    static EditorUI instance;
    return instance;
}

bool EditorUI::Initialize()
{
    // Initialize UI system
    InitializeUISystem();
    
    // Create main window
    m_MainWindow = std::make_shared<EditorWindow>("Versa Engine Editor");
    m_UISystem.RegisterElement(m_MainWindow);
    
    // Setup default layout
    SetupDefaultLayout();
    
    // Apply theme
    ApplyTheme();
    
    return true;
}

void EditorUI::Shutdown()
{
    m_MainWindow.reset();
    ShutdownUISystem();
}

void EditorUI::BeginFrame()
{
    m_UISystem.BeginFrame();
}

void EditorUI::EndFrame()
{
    m_UISystem.EndFrame();
}

void EditorUI::Update(float deltaTime)
{
    m_UISystem.Update(deltaTime);
}

std::shared_ptr<EditorWindow> EditorUI::CreateWindow(const std::string& title)
{
    auto window = std::make_shared<EditorWindow>(title);
    m_UISystem.RegisterElement(window);
    return window;
}

std::shared_ptr<EditorPanel> EditorUI::CreatePanel(const std::string& title)
{
    auto panel = std::make_shared<EditorPanel>(title);
    m_UISystem.RegisterElement(panel);
    return panel;
}

void EditorUI::InitializeUISystem()
{
    if (!m_UISystem.Initialize())
    {
        // Handle initialization failure
        throw std::runtime_error("Failed to initialize UI system");
    }
}

void EditorUI::ShutdownUISystem()
{
    m_UISystem.Shutdown();
}

void EditorUI::SetupDefaultLayout()
{
    if (!m_MainWindow)
        return;
        
    // Create dock space
    auto dockSpace = m_MainWindow->GetDockSpace();
    
    // Setup default docking layout
    std::vector<UI::DockSplit> splits = {
        { UI::DockSplit::Direction::Left, 0.2f },   // Scene hierarchy
        { UI::DockSplit::Direction::Right, 0.2f },  // Properties
        { UI::DockSplit::Direction::Down, 0.25f }   // Console
    };
    
    dockSpace->SetLayout(splits);
}

void EditorUI::ApplyTheme()
{
    auto& theme = UI::UITheme::Get();
    
    // Set up colors
    theme.SetColor(UI::ThemeColor::Text, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    theme.SetColor(UI::ThemeColor::TextDisabled, glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
    theme.SetColor(UI::ThemeColor::WindowBg, glm::vec4(0.06f, 0.06f, 0.06f, 1.0f));
    theme.SetColor(UI::ThemeColor::ChildBg, glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
    theme.SetColor(UI::ThemeColor::PopupBg, glm::vec4(0.08f, 0.08f, 0.08f, 0.94f));
    theme.SetColor(UI::ThemeColor::Border, glm::vec4(0.43f, 0.43f, 0.50f, 0.5f));
    theme.SetColor(UI::ThemeColor::BorderShadow, glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
    theme.SetColor(UI::ThemeColor::FrameBg, glm::vec4(0.16f, 0.29f, 0.48f, 0.54f));
    theme.SetColor(UI::ThemeColor::FrameBgHovered, glm::vec4(0.26f, 0.59f, 0.98f, 0.4f));
    theme.SetColor(UI::ThemeColor::FrameBgActive, glm::vec4(0.26f, 0.59f, 0.98f, 0.67f));
    theme.SetColor(UI::ThemeColor::TitleBg, glm::vec4(0.04f, 0.04f, 0.04f, 1.0f));
    theme.SetColor(UI::ThemeColor::TitleBgActive, glm::vec4(0.16f, 0.29f, 0.48f, 1.0f));
    theme.SetColor(UI::ThemeColor::TitleBgCollapsed, glm::vec4(0.0f, 0.0f, 0.0f, 0.51f));
    theme.SetColor(UI::ThemeColor::MenuBarBg, glm::vec4(0.14f, 0.14f, 0.14f, 1.0f));
    theme.SetColor(UI::ThemeColor::ScrollbarBg, glm::vec4(0.02f, 0.02f, 0.02f, 0.53f));
    theme.SetColor(UI::ThemeColor::ScrollbarGrab, glm::vec4(0.31f, 0.31f, 0.31f, 1.0f));
    theme.SetColor(UI::ThemeColor::ScrollbarGrabHovered, glm::vec4(0.41f, 0.41f, 0.41f, 1.0f));
    theme.SetColor(UI::ThemeColor::ScrollbarGrabActive, glm::vec4(0.51f, 0.51f, 0.51f, 1.0f));
    theme.SetColor(UI::ThemeColor::CheckMark, glm::vec4(0.26f, 0.59f, 0.98f, 1.0f));
    theme.SetColor(UI::ThemeColor::SliderGrab, glm::vec4(0.24f, 0.52f, 0.88f, 1.0f));
    theme.SetColor(UI::ThemeColor::SliderGrabActive, glm::vec4(0.26f, 0.59f, 0.98f, 1.0f));
    theme.SetColor(UI::ThemeColor::Button, glm::vec4(0.26f, 0.59f, 0.98f, 0.4f));
    theme.SetColor(UI::ThemeColor::ButtonHovered, glm::vec4(0.26f, 0.59f, 0.98f, 1.0f));
    theme.SetColor(UI::ThemeColor::ButtonActive, glm::vec4(0.06f, 0.53f, 0.98f, 1.0f));
    theme.SetColor(UI::ThemeColor::Header, glm::vec4(0.26f, 0.59f, 0.98f, 0.31f));
    theme.SetColor(UI::ThemeColor::HeaderHovered, glm::vec4(0.26f, 0.59f, 0.98f, 0.8f));
    theme.SetColor(UI::ThemeColor::HeaderActive, glm::vec4(0.26f, 0.59f, 0.98f, 1.0f));
    theme.SetColor(UI::ThemeColor::Separator, glm::vec4(0.43f, 0.43f, 0.50f, 0.5f));
    theme.SetColor(UI::ThemeColor::SeparatorHovered, glm::vec4(0.1f, 0.4f, 0.75f, 0.78f));
    theme.SetColor(UI::ThemeColor::SeparatorActive, glm::vec4(0.1f, 0.4f, 0.75f, 1.0f));
    theme.SetColor(UI::ThemeColor::Tab, glm::vec4(0.17f, 0.3f, 0.49f, 0.86f));
    theme.SetColor(UI::ThemeColor::TabHovered, glm::vec4(0.26f, 0.59f, 0.98f, 0.8f));
    theme.SetColor(UI::ThemeColor::TabActive, glm::vec4(0.2f, 0.41f, 0.68f, 1.0f));
    
    // Set up style variables
    theme.SetFloat(UI::ThemeVar::WindowRounding, 0.0f);
    theme.SetFloat(UI::ThemeVar::FrameRounding, 4.0f);
    theme.SetFloat(UI::ThemeVar::GrabRounding, 4.0f);
    theme.SetFloat(UI::ThemeVar::ScrollbarRounding, 9.0f);
    theme.SetFloat(UI::ThemeVar::WindowBorderSize, 1.0f);
    theme.SetFloat(UI::ThemeVar::ChildBorderSize, 1.0f);
    theme.SetFloat(UI::ThemeVar::PopupBorderSize, 1.0f);
    theme.SetFloat(UI::ThemeVar::FrameBorderSize, 0.0f);
    theme.SetFloat(UI::ThemeVar::TabBorderSize, 1.0f);
    
    // Apply the theme
    theme.Apply();
}

} // namespace Editor
} // namespace VGE



