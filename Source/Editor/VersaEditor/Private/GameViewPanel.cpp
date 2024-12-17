#include "U

#include "Runtime/Core/Public/Core/UICore.h"I/Editor/Panels/GameViewPanel.h"
#include "UI/Framework/Widgets/UIInputWidgets.h"
#include "UI/Framework/Layout/UILayout.h"
#include "Graphics/RenderTarget.h"
#include "Graphics/Texture.h"

namespace VGE {
namespace Editor {

GameViewPanel::GameViewPanel()
    : EditorPanel("Game View")
    , m_ViewWidth(1280)
    , m_ViewHeight(720)
    , m_Scale(1.0f)
    , m_AspectRatio(16.0f / 9.0f)
    , m_ShowStats(true)
    , m_MaintainAspectRatio(true)
    , m_IsPlaying(false)
    , m_IsPaused(false)
    , m_GameTime(0.0f)
    , m_FrameTime(0.0f)
    , m_FPS(0)
    , m_IsHovered(false)
    , m_IsFocused(false)
    , m_IsResizing(false)
{
    SetSize(glm::vec2(800.0f, 600.0f));
}

void GameViewPanel::Initialize()
{
    EditorPanel::Initialize();
    
    // Create render target
    m_RenderTarget = std::make_shared<Graphics::RenderTarget>(m_ViewWidth, m_ViewHeight);
    m_GameTexture = m_RenderTarget->GetColorTexture();
}

void GameViewPanel::Update(float deltaTime)
{
    EditorPanel::Update(deltaTime);
    
    if (m_IsPlaying && !m_IsPaused)
    {
        UpdateGameView(deltaTime);
    }
    
    HandleInput();
}

void GameViewPanel::SetResolution(int width, int height)
{
    if (width == m_ViewWidth && height == m_ViewHeight)
        return;
        
    m_ViewWidth = width;
    m_ViewHeight = height;
    m_AspectRatio = static_cast<float>(width) / static_cast<float>(height);
    
    ResizeRenderTarget();
}

void GameViewPanel::SetScale(float scale)
{
    m_Scale = glm::clamp(scale, 0.1f, 10.0f);
    UpdateViewportSize();
}

void GameViewPanel::SetAspectRatio(float ratio)
{
    m_AspectRatio = ratio;
    if (m_MaintainAspectRatio)
    {
        m_ViewHeight = static_cast<int>(m_ViewWidth / ratio);
        ResizeRenderTarget();
    }
}

void GameViewPanel::Play()
{
    m_IsPlaying = true;
    m_IsPaused = false;
    m_GameTime = 0.0f;
}

void GameViewPanel::Pause()
{
    m_IsPaused = !m_IsPaused;
}

void GameViewPanel::Stop()
{
    m_IsPlaying = false;
    m_IsPaused = false;
    m_GameTime = 0.0f;
}

void GameViewPanel::RenderPanelContent()
{
    auto layout = CreateVerticalLayout();
    
    // Toolbar
    RenderToolbar();
    AddSeparator();
    
    // Game view
    RenderGameView();
    
    // Stats overlay
    if (m_ShowStats)
    {
        RenderStats();
    }
}

void GameViewPanel::RenderToolbar()
{
    auto toolbar = CreateHorizontalLayout();
    
    if (auto button = CreateButton(m_IsPlaying ? "Stop" : "Play"))
    {
        if (m_IsPlaying)
            Stop();
        else
            Play();
    }
    
    if (auto button = CreateButton("Pause"))
    {
        Pause();
    }
    
    // Resolution controls
    if (auto input = CreateNumberInput("Width", m_ViewWidth))
    {
        if (input->GetValue() != m_ViewWidth)
        {
            SetResolution(static_cast<int>(input->GetValue()), m_ViewHeight);
        }
    }
    
    if (auto input = CreateNumberInput("Height", m_ViewHeight))
    {
        if (input->GetValue() != m_ViewHeight)
        {
            SetResolution(m_ViewWidth, static_cast<int>(input->GetValue()));
        }
    }
    
    // Scale control
    if (auto slider = CreateSlider("Scale", m_Scale, 0.1f, 10.0f))
    {
        SetScale(slider->GetValue());
    }
    
    // Aspect ratio lock
    if (auto checkbox = CreateCheckbox("Lock Aspect", m_MaintainAspectRatio))
    {
        m_MaintainAspectRatio = checkbox->IsChecked();
        if (m_MaintainAspectRatio)
        {
            SetAspectRatio(m_AspectRatio);
        }
    }
    
    // Stats toggle
    if (auto checkbox = CreateCheckbox("Stats", m_ShowStats))
    {
        m_ShowStats = checkbox->IsChecked();
    }
}

void GameViewPanel::RenderGameView()
{
    auto viewportSize = GetContentSize();
    float contentWidth = viewportSize.x;
    float contentHeight = viewportSize.y;
    
    // Calculate scaled dimensions
    float scaledWidth = m_ViewWidth * m_Scale;
    float scaledHeight = m_ViewHeight * m_Scale;
    
    // Center the game view
    float offsetX = (contentWidth - scaledWidth) * 0.5f;
    float offsetY = (contentHeight - scaledHeight) * 0.5f;
    
    auto gameView = CreateImageView(m_GameTexture);
    gameView->SetPosition(glm::vec2(offsetX, offsetY));
    gameView->SetSize(glm::vec2(scaledWidth, scaledHeight));
    
    m_IsHovered = gameView->IsHovered();
    m_IsFocused = gameView->IsFocused();
}

void GameViewPanel::RenderStats()
{
    auto statsPanel = CreatePanel();
    statsPanel->SetPosition(glm::vec2(10.0f, 30.0f));
    statsPanel->SetSize(glm::vec2(200.0f, 100.0f));
    
    auto layout = CreateVerticalLayout();
    CreateText(std::string("FPS: ") + std::to_string(m_FPS));
    CreateText(std::string("Frame Time: ") + std::to_string(m_FrameTime * 1000.0f) + " ms");
    CreateText(std::string("Game Time: ") + std::to_string(m_GameTime) + " s");
    CreateText(std::string("Resolution: ") + std::to_string(m_ViewWidth) + "x" + std::to_string(m_ViewHeight));
    CreateText(std::string("Scale: ") + std::to_string(m_Scale) + "x");
}

void GameViewPanel::HandleInput()
{
    if (!m_IsHovered)
        return;
        
    // Pan with right mouse button
    if (IsMouseButtonDown(MouseButton::Right))
    {
        auto mouseDelta = GetMouseDelta();
        // Handle panning
    }
    
    // Zoom with mouse wheel
    float mouseWheel = GetMouseWheelDelta();
    if (mouseWheel != 0.0f)
    {
        float zoom = mouseWheel * 0.1f;
        SetScale(m_Scale + zoom);
    }
}

void GameViewPanel::UpdateGameView(float deltaTime)
{
    m_GameTime += deltaTime;
    m_FrameTime = deltaTime;
    m_FPS = static_cast<int>(1.0f / deltaTime);
    
    // Update game simulation here
}

void GameViewPanel::ResizeRenderTarget()
{
    if (m_RenderTarget)
    {
        m_RenderTarget->Resize(m_ViewWidth, m_ViewHeight);
        m_GameTexture = m_RenderTarget->GetColorTexture();
    }
}

void GameViewPanel::UpdateViewportSize()
{
    auto size = GetContentSize();
    // Update viewport calculations if needed
}

} // namespace Editor
} // namespace VGE 


