#include "E

#include "Runtime/Core/Public/Core/UICore.h"ditorLayer.h"
#include "Runtime/Core/Public/Core/Application.h"
#include "UI/UISystem.h"
#include "UI/UITheme.h"

namespace VGE {
namespace Editor {

EditorLayer::EditorLayer()
    : m_UISystem(std::make_unique<UISystem>()) {
}

EditorLayer::~EditorLayer() = default;

void EditorLayer::OnAttach() {
    // Initialize UI System
    m_UISystem->Initialize();

    // Load editor theme
    UITheme editorTheme;
    editorTheme.LoadFromFile("Assets/Themes/DarkTheme.json");
    m_UISystem->SetTheme(editorTheme);

    // Create panels
    m_SceneHierarchyPanel = std::make_unique<SceneHierarchyPanel>();
    m_PropertiesPanel = std::make_unique<PropertiesPanel>();
    m_ViewportPanel = std::make_unique<ViewportPanel>();
    m_AssetBrowserPanel = std::make_unique<AssetBrowserPanel>();
    m_ConsolePanel = std::make_unique<ConsolePanel>();
    m_ToolbarPanel = std::make_unique<ToolbarPanel>();

    // Load saved layout if exists
    LoadEditorLayout();
}

void EditorLayer::OnDetach() {
    // Save layout before detaching
    SaveEditorLayout();

    // Cleanup panels
    m_SceneHierarchyPanel.reset();
    m_PropertiesPanel.reset();
    m_ViewportPanel.reset();
    m_AssetBrowserPanel.reset();
    m_ConsolePanel.reset();
    m_ToolbarPanel.reset();

    // Shutdown UI System
    m_UISystem->Shutdown();
}

void EditorLayer::OnUpdate() {
    // Update UI System
    m_UISystem->Update(Application::Get().GetDeltaTime());

    // Update panels
    m_SceneHierarchyPanel->OnUpdate();
    m_PropertiesPanel->OnUpdate();
    m_ViewportPanel->OnUpdate();
    m_AssetBrowserPanel->OnUpdate();
    m_ConsolePanel->OnUpdate();
    m_ToolbarPanel->OnUpdate();
}

void EditorLayer::OnRender() {
    m_UISystem->BeginFrame();

    // Begin dockspace
    m_UISystem->BeginDockspace();

    // Show menu bar
    ShowMenuBar();

    // Render panels
    m_SceneHierarchyPanel->OnRender();
    m_PropertiesPanel->OnRender();
    m_ViewportPanel->OnRender();
    m_AssetBrowserPanel->OnRender();
    m_ConsolePanel->OnRender();
    m_ToolbarPanel->OnRender();

    m_UISystem->EndDockspace();
    m_UISystem->EndFrame();
}

void EditorLayer::OnEvent(Event& event) {
    // Handle events in panels
    m_SceneHierarchyPanel->OnEvent(event);
    m_PropertiesPanel->OnEvent(event);
    m_ViewportPanel->OnEvent(event);
    m_AssetBrowserPanel->OnEvent(event);
    m_ConsolePanel->OnEvent(event);
    m_ToolbarPanel->OnEvent(event);
}

void EditorLayer::SetupLayout() {
    // Set up default layout if no saved layout exists
    m_UISystem->CreateWindow("Scene Hierarchy", {0, 0}, {300, 400});
    m_UISystem->CreateWindow("Properties", {0, 400}, {300, 400});
    m_UISystem->CreateWindow("Viewport", {300, 0}, {800, 600});
    m_UISystem->CreateWindow("Asset Browser", {300, 600}, {800, 200});
    m_UISystem->CreateWindow("Console", {0, 800}, {1100, 200});
    m_UISystem->CreateWindow("Toolbar", {1100, 0}, {200, 1000});
}

void EditorLayer::ShowMenuBar() {
    m_UISystem->BeginPanel("MenuBar");

    if (m_UISystem->BeginWindow("File")) {
        if (m_UISystem->Button("New Scene")) {
            // Handle new scene
        }
        if (m_UISystem->Button("Open Scene...")) {
            // Handle open scene
        }
        if (m_UISystem->Button("Save Scene")) {
            // Handle save scene
        }
        if (m_UISystem->Button("Save Scene As...")) {
            // Handle save scene as
        }
        if (m_UISystem->Button("Exit")) {
            Application::Get().Close();
        }
        m_UISystem->EndWindow();
    }

    if (m_UISystem->BeginWindow("Edit")) {
        if (m_UISystem->Button("Undo")) {
            // Handle undo
        }
        if (m_UISystem->Button("Redo")) {
            // Handle redo
        }
        m_UISystem->EndWindow();
    }

    if (m_UISystem->BeginWindow("View")) {
        if (m_UISystem->Button("Reset Layout")) {
            SetupLayout();
        }
        m_UISystem->EndWindow();
    }

    m_UISystem->EndPanel();
}

void EditorLayer::SaveEditorLayout() {
    // TODO: Implement layout saving
}

void EditorLayer::LoadEditorLayout() {
    // TODO: Implement layout loading
    // For now, just set up default layout
    SetupLayout();
}

} // namespace Editor
} // namespace VGE



