#include "Runtime/UI/Public/Docking/UIDockSpace.h"
#include "Runtime/Core/Public/Renderer/UIRenderer.h"
#include "Runtime/Core/Public/Core/Logger.h"

namespace VGE {
namespace Editor {

UIDockSpace::UIDockSpace(const std::string& name)
    : m_Name(name)
    , m_FirstFrame(true)
{
    m_RootTabs = CreateTabManager();
}

void UIDockSpace::Initialize() {
    UIWidget::Initialize();
    
    // Initialize the root tab manager
    if (m_RootTabs) {
        m_RootTabs->SetOnTabActivated([this](const UITabInfo& tab) { OnTabActivated(tab); });
        m_RootTabs->SetOnTabClosed([this](const UITabInfo& tab) { OnTabClosed(tab); });
        m_RootTabs->SetOnTabDragStart([this](const UITabInfo& tab) { OnTabDragStart(tab); });
        m_RootTabs->SetOnTabDragEnd([this](const UITabInfo& tab) { OnTabDragEnd(tab); });
    }
}

void UIDockSpace::Update(float deltaTime) {
    UIWidget::Update(deltaTime);

    // Update drag preview if active
    if (!m_DraggingWindow.empty()) {
        auto mousePos = GetInput()->GetMousePosition();
        UpdateDragPreview(mousePos);
    }

    // Update all tab managers
    if (m_RootTabs) {
        // If not split, just update root tabs
        if (m_Splits.empty()) {
            // Update tab content
            if (auto activeTab = m_RootTabs->GetActiveTab()) {
                if (activeTab->content) {
                    activeTab->content->Update(deltaTime);
                }
            }
        }
        // If split, update both sides
        else {
            for (const auto& split : m_Splits) {
                if (split.leftTabs) {
                    if (auto activeTab = split.leftTabs->GetActiveTab()) {
                        if (activeTab->content) {
                            activeTab->content->Update(deltaTime);
                        }
                    }
                }
                if (split.rightTabs) {
                    if (auto activeTab = split.rightTabs->GetActiveTab()) {
                        if (activeTab->content) {
                            activeTab->content->Update(deltaTime);
                        }
                    }
                }
            }
        }
    }
}

void UIDockSpace::Render() {
    UIWidget::Render();

    // Render the dock space background
    auto renderer = UIRenderer::Get();
    if (renderer) {
        renderer->DrawRect(m_Position, m_Size, glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));
    }

    // Render content based on split configuration
    if (m_Splits.empty()) {
        // No splits, render root tabs
        if (m_RootTabs) {
            if (auto activeTab = m_RootTabs->GetActiveTab()) {
                if (activeTab->content) {
                    activeTab->content->Render();
                }
            }
        }
    }
    else {
        // Render split content
        for (const auto& split : m_Splits) {
            // Calculate split positions
            glm::vec2 leftPos = m_Position;
            glm::vec2 rightPos = m_Position;
            glm::vec2 leftSize = m_Size;
            glm::vec2 rightSize = m_Size;

            if (split.isVertical) {
                leftSize.x *= split.ratio;
                rightSize.x *= (1.0f - split.ratio);
                rightPos.x += leftSize.x;
            }
            else {
                leftSize.y *= split.ratio;
                rightSize.y *= (1.0f - split.ratio);
                rightPos.y += leftSize.y;
            }

            // Render left/top content
            if (split.leftTabs) {
                if (auto activeTab = split.leftTabs->GetActiveTab()) {
                    if (activeTab->content) {
                        activeTab->content->SetPosition(leftPos);
                        activeTab->content->SetSize(leftSize);
                        activeTab->content->Render();
                    }
                }
            }

            // Render right/bottom content
            if (split.rightTabs) {
                if (auto activeTab = split.rightTabs->GetActiveTab()) {
                    if (activeTab->content) {
                        activeTab->content->SetPosition(rightPos);
                        activeTab->content->SetSize(rightSize);
                        activeTab->content->Render();
                    }
                }
            }

            // Draw split line
            if (renderer) {
                glm::vec4 splitColor(0.3f, 0.3f, 0.3f, 1.0f);
                if (split.isVertical) {
                    glm::vec2 lineStart(leftPos.x + leftSize.x, leftPos.y);
                    glm::vec2 lineEnd(lineStart.x, lineStart.y + m_Size.y);
                    renderer->DrawLine(lineStart, lineEnd, splitColor, 2.0f);
                }
                else {
                    glm::vec2 lineStart(leftPos.x, leftPos.y + leftSize.y);
                    glm::vec2 lineEnd(lineStart.x + m_Size.x, lineStart.y);
                    renderer->DrawLine(lineStart, lineEnd, splitColor, 2.0f);
                }
            }
        }
    }

    // Render drag preview if active
    if (!m_DraggingWindow.empty()) {
        m_DragPreview.Render();
    }
}

bool UIDockSpace::DockWindow(std::shared_ptr<UIWidget> window, const std::string& name,
                           const std::string& title, DockZone zone) {
    if (!window) {
        Logger::Log(LogLevel::Warning, "Attempting to dock null window");
        return false;
    }

    // Create window info
    DockedWindow dockedWindow;
    dockedWindow.widget = window;
    dockedWindow.name = name;
    dockedWindow.title = title.empty() ? name : title;
    dockedWindow.isVisible = true;
    dockedWindow.isActive = true;

    // Store window
    m_DockedWindows[name] = dockedWindow;

    // Add to appropriate tab manager based on zone
    if (m_Splits.empty()) {
        return m_RootTabs->AddTab(name, dockedWindow.title, window);
    }
    else {
        // Handle docking in split spaces based on zone
        // This is a simplified version - you might want to make it more sophisticated
        auto& split = m_Splits.back();
        switch (zone) {
            case DockZone::Left:
            case DockZone::TopLeft:
            case DockZone::BottomLeft:
                return split.leftTabs->AddTab(name, dockedWindow.title, window);
            
            case DockZone::Right:
            case DockZone::TopRight:
            case DockZone::BottomRight:
                return split.rightTabs->AddTab(name, dockedWindow.title, window);
            
            default:
                return m_RootTabs->AddTab(name, dockedWindow.title, window);
        }
    }
}

bool UIDockSpace::UndockWindow(const std::string& name) {
    auto it = m_DockedWindows.find(name);
    if (it == m_DockedWindows.end()) {
        return false;
    }

    // Remove from tab managers
    if (m_RootTabs) {
        m_RootTabs->RemoveTab(name);
    }
    
    for (auto& split : m_Splits) {
        if (split.leftTabs) split.leftTabs->RemoveTab(name);
        if (split.rightTabs) split.rightTabs->RemoveTab(name);
    }

    // Remove from docked windows
    m_DockedWindows.erase(it);
    return true;
}

bool UIDockSpace::Split(bool isVertical, float ratio) {
    DockSplit split;
    split.isVertical = isVertical;
    split.ratio = ratio;
    split.leftTabs = CreateTabManager();
    split.rightTabs = CreateTabManager();

    // Move existing tabs to left side if this is our first split
    if (m_Splits.empty() && m_RootTabs) {
        auto tabs = m_RootTabs->GetTabs();
        for (const auto& tab : tabs) {
            split.leftTabs->AddTab(tab.id, tab.label, tab.content, tab.canClose);
        }
    }

    m_Splits.push_back(split);
    return true;
}

void UIDockSpace::BeginDrag(const std::string& windowName) {
    m_DraggingWindow = windowName;
    m_DragPreview.SetActive(true);
}

bool UIDockSpace::EndDrag() {
    if (m_DraggingWindow.empty()) {
        return false;
    }

    auto zone = m_DragPreview.GetZone();
    if (zone != DockZone::None) {
        // Get the window being dragged
        auto it = m_DockedWindows.find(m_DraggingWindow);
        if (it != m_DockedWindows.end()) {
            // Undock from current position and redock at new position
            auto window = it->second.widget;
            auto title = it->second.title;
            UndockWindow(m_DraggingWindow);
            DockWindow(window, m_DraggingWindow, title, zone);
        }
    }

    m_DraggingWindow.clear();
    m_DragPreview.SetActive(false);
    return true;
}

void UIDockSpace::UpdateDragPreview(const glm::vec2& mousePos) {
    m_DragPreview.Update(mousePos, *this);
}

std::shared_ptr<UITabManager> UIDockSpace::CreateTabManager() {
    auto tabManager = std::make_shared<UITabManager>();
    tabManager->SetOnTabActivated([this](const UITabInfo& tab) { OnTabActivated(tab); });
    tabManager->SetOnTabClosed([this](const UITabInfo& tab) { OnTabClosed(tab); });
    tabManager->SetOnTabDragStart([this](const UITabInfo& tab) { OnTabDragStart(tab); });
    tabManager->SetOnTabDragEnd([this](const UITabInfo& tab) { OnTabDragEnd(tab); });
    return tabManager;
}

void UIDockSpace::OnTabActivated(const UITabInfo& tab) {
    auto it = m_DockedWindows.find(tab.id);
    if (it != m_DockedWindows.end()) {
        it->second.isActive = true;
    }
}

void UIDockSpace::OnTabClosed(const UITabInfo& tab) {
    UndockWindow(tab.id);
}

void UIDockSpace::OnTabDragStart(const UITabInfo& tab) {
    BeginDrag(tab.id);
}

void UIDockSpace::OnTabDragEnd(const UITabInfo& tab) {
    EndDrag();
}

void UIDockSpace::Clear() {
    m_Splits.clear();
    m_DockedWindows.clear();
    m_DraggingWindow.clear();
    m_DragPreview.SetActive(false);
    
    // Reset root tabs
    m_RootTabs = CreateTabManager();
}

void UIDockSpace::AddDockedWindow(const DockedWindow& window) {
    // Add to docked windows map
    m_DockedWindows[window.name] = window;

    // Add to root tabs if no splits exist
    if (m_Splits.empty() && m_RootTabs) {
        m_RootTabs->AddTab(window.name, window.title, window.widget);
    }
}

}} // namespace VGE::Editor



