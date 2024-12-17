#include "U

#include "Runtime/Core/Public/Core/UICore.h"I/Editor/Panels/ViewportPanel.h"
#include "UI/Framework/Widgets/UIInputWidgets.h"
#include "UI/Framework/Layout/UILayout.h"
#include "Graphics/Camera.h"
#include "Graphics/RenderTarget.h"

namespace VGE {
namespace Editor {

ViewportPanel::ViewportPanel()
    : EditorPanel("Viewport")
    , m_Camera(std::make_shared<Graphics::Camera>())
    , m_RenderTarget(std::make_shared<Graphics::RenderTarget>())
    , m_ShowCameraControls(false)
{
    SetSize(glm::vec2(800.0f, 600.0f));
}

void ViewportPanel::Initialize()
{
    EditorPanel::Initialize();
    
    // Initialize camera
    m_Camera->SetPosition(glm::vec3(0.0f, 5.0f, 10.0f));
    m_Camera->LookAt(glm::vec3(0.0f));
    
    // Create render target
    m_RenderTarget->Create(800, 600);
}

void ViewportPanel::Update(float deltaTime)
{
    EditorPanel::Update(deltaTime);
    
    // Update camera
    if (m_Camera)
    {
        HandleCameraInput(deltaTime);
    }
}

void ViewportPanel::RenderPanelContent()
{
    auto layout = CreateVerticalLayout();
    
    // Toolbar
    RenderToolbar();
    
    // Viewport
    RenderViewport();
    
    // Camera controls
    if (m_ShowCameraControls)
    {
        RenderCameraControls();
    }
}

void ViewportPanel::RenderToolbar()
{
    auto toolbar = CreateHorizontalLayout();
    
    // Camera controls toggle
    if (auto button = CreateButton("Camera Controls"))
    {
        m_ShowCameraControls = !m_ShowCameraControls;
    }
    
    // View options
    if (auto button = CreateButton("Reset Camera"))
    {
        ResetCamera();
    }
    
    // Grid toggle
    if (auto checkbox = CreateCheckbox("Show Grid", m_ShowGrid))
    {
        m_ShowGrid = checkbox->IsChecked();
    }
    
    // Gizmo options
    if (auto combo = CreateComboBox("Gizmo Mode", {"Translate", "Rotate", "Scale"}, static_cast<int>(m_GizmoMode)))
    {
        m_GizmoMode = static_cast<GizmoMode>(combo->GetSelectedIndex());
    }
}

void ViewportPanel::RenderViewport()
{
    // Get viewport size
    auto viewportSize = GetContentSize();
    
    // Update render target if needed
    if (m_RenderTarget->GetWidth() != static_cast<uint32_t>(viewportSize.x) ||
        m_RenderTarget->GetHeight() != static_cast<uint32_t>(viewportSize.y))
    {
        m_RenderTarget->Resize(static_cast<uint32_t>(viewportSize.x), 
                             static_cast<uint32_t>(viewportSize.y));
    }
    
    // Create viewport view
    auto viewport = CreateViewport();
    viewport->SetSize(viewportSize);
    viewport->SetRenderTarget(m_RenderTarget);
    viewport->SetCamera(m_Camera);
    
    // Handle viewport interaction
    if (viewport->IsHovered())
    {
        HandleViewportInput();
    }
}

void ViewportPanel::RenderCameraControls()
{
    auto controls = CreatePanel("Camera Controls");
    controls->SetFlags(UIPanelFlags::NoMove | UIPanelFlags::NoResize);
    controls->SetPosition(glm::vec2(10.0f, GetSize().y - 200.0f));
    controls->SetSize(glm::vec2(200.0f, 180.0f));
    
    auto layout = CreateVerticalLayout();
    
    // Camera position
    auto pos = m_Camera->GetPosition();
    if (auto input = CreateVectorInput("Position", pos, 3))
    {
        m_Camera->SetPosition(input->GetValue());
    }
    
    // Camera rotation
    auto rot = m_Camera->GetRotation();
    if (auto input = CreateVectorInput("Rotation", rot, 3))
    {
        m_Camera->SetRotation(input->GetValue());
    }
    
    // Camera FOV
    float fov = m_Camera->GetFOV();
    if (auto input = CreateNumberInput("FOV", fov, 1.0f))
    {
        m_Camera->SetFOV(input->GetValue());
    }
    
    // Camera speed
    if (auto input = CreateNumberInput("Move Speed", m_CameraMoveSpeed, 0.1f))
    {
        m_CameraMoveSpeed = input->GetValue();
    }
    
    if (auto input = CreateNumberInput("Rotate Speed", m_CameraRotateSpeed, 0.1f))
    {
        m_CameraRotateSpeed = input->GetValue();
    }
}

void ViewportPanel::HandleCameraInput(float deltaTime)
{
    if (!IsHovered())
        return;
        
    // Camera movement
    if (IsMouseButtonDown(MouseButton::Right))
    {
        auto mouseDelta = GetMouseDelta();
        
        // Rotation
        if (mouseDelta.x != 0.0f || mouseDelta.y != 0.0f)
        {
            auto rotation = m_Camera->GetRotation();
            rotation.y += mouseDelta.x * m_CameraRotateSpeed;
            rotation.x += mouseDelta.y * m_CameraRotateSpeed;
            m_Camera->SetRotation(rotation);
        }
        
        // Movement
        glm::vec3 moveDir(0.0f);
        if (IsKeyDown(Key::W)) moveDir.z -= 1.0f;
        if (IsKeyDown(Key::S)) moveDir.z += 1.0f;
        if (IsKeyDown(Key::A)) moveDir.x -= 1.0f;
        if (IsKeyDown(Key::D)) moveDir.x += 1.0f;
        if (IsKeyDown(Key::Q)) moveDir.y -= 1.0f;
        if (IsKeyDown(Key::E)) moveDir.y += 1.0f;
        
        if (glm::length(moveDir) > 0.0f)
        {
            moveDir = glm::normalize(moveDir);
            m_Camera->Move(moveDir * m_CameraMoveSpeed * deltaTime);
        }
    }
    
    // Camera zoom
    float mouseWheel = GetMouseWheelDelta();
    if (mouseWheel != 0.0f)
    {
        auto position = m_Camera->GetPosition();
        auto forward = m_Camera->GetForward();
        position += forward * mouseWheel * m_CameraZoomSpeed;
        m_Camera->SetPosition(position);
    }
}

void ViewportPanel::HandleViewportInput()
{
    // Handle gizmo interaction
    if (m_SelectedObject && m_GizmoMode != GizmoMode::None)
    {
        auto gizmo = CreateGizmo(m_GizmoMode);
        gizmo->SetTarget(m_SelectedObject);
        gizmo->Update();
    }
    
    // Handle object selection
    if (IsMouseButtonPressed(MouseButton::Left) && !IsKeyDown(Key::Alt))
    {
        auto mousePos = GetMousePosition();
        auto ray = m_Camera->ScreenPointToRay(mousePos);
        
        // TODO: Implement object picking
    }
}

void ViewportPanel::ResetCamera()
{
    m_Camera->SetPosition(glm::vec3(0.0f, 5.0f, 10.0f));
    m_Camera->SetRotation(glm::vec3(0.0f));
    m_Camera->LookAt(glm::vec3(0.0f));
}

} // namespace Editor
} // namespace VGE



