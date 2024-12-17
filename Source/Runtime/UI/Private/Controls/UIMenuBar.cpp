#include "Runtime/UI/Public/Controls/UIMenuBar.h"
#include "Runtime/UI/Public/Controls/UIMenu.h"
#include "Runtime/Core/Public/Renderer/UIRenderer.h"

namespace VGE {
namespace Editor {

std::shared_ptr<UIMenu> UIMenuBar::AddMenu(const std::string& name) {
    auto menu = std::make_shared<UIMenu>(name);
    m_Menus.push_back(menu);
    return menu;
}

void UIMenuBar::OnMouseMove(const glm::vec2& position) {
    // Update hovered menu
    m_HoveredMenu = nullptr;
    float x = GetPosition().x;
    
    for (const auto& menu : m_Menus) {
        glm::vec2 labelSize = UIRenderer::Get().GetTextSize(menu->GetName());
        glm::vec2 itemSize(labelSize.x + m_MenuSpacing * 2, m_MenuHeight);
        
        if (position.x >= x && position.x < x + itemSize.x &&
            position.y >= GetPosition().y && position.y < GetPosition().y + m_MenuHeight) {
            m_HoveredMenu = menu.get();
            break;
        }
        x += itemSize.x;
    }
    
    // Update active menu mouse position
    if (m_ActiveMenu) {
        m_ActiveMenu->OnMouseMove(position);
    }
}

void UIMenuBar::OnMouseDown(const glm::vec2& position) {
    if (m_HoveredMenu) {
        m_ActiveMenu = m_HoveredMenu;
        m_ActiveMenu->OnMouseDown(position);
    } else if (m_ActiveMenu) {
        m_ActiveMenu->OnMouseDown(position);
    }
}

void UIMenuBar::OnMouseUp(const glm::vec2& position) {
    if (m_ActiveMenu) {
        m_ActiveMenu->OnMouseUp(position);
        if (!m_ActiveMenu->HasActiveSubmenu()) {
            m_ActiveMenu = nullptr;
        }
    }
}

void UIMenuBar::OnUpdate(float deltaTime) {
    if (m_ActiveMenu) {
        m_ActiveMenu->OnUpdate(deltaTime);
    }
}

void UIMenuBar::OnPaint(UIRenderer& renderer) {
    // Draw menubar background
    renderer.DrawRect(GetPosition(), GetSize(), glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));
    renderer.DrawRectOutline(GetPosition(), GetSize(), glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));

    // Draw menu items
    float x = GetPosition().x;
    for (const auto& menu : m_Menus) {
        bool isHovered = menu.get() == m_HoveredMenu;
        bool isActive = menu.get() == m_ActiveMenu;
        
        // Calculate item size
        glm::vec2 labelSize = renderer.GetTextSize(menu->GetName());
        glm::vec2 itemSize(labelSize.x + m_MenuSpacing * 2, m_MenuHeight);
        
        // Draw item background
        if (isHovered || isActive) {
            renderer.DrawRect(
                glm::vec2(x, GetPosition().y),
                itemSize,
                glm::vec4(0.3f, 0.3f, 0.3f, 1.0f)
            );
        }
        
        // Draw item text
        glm::vec4 textColor = isHovered || isActive ? 
            glm::vec4(1.0f) : glm::vec4(0.9f, 0.9f, 0.9f, 1.0f);
            
        renderer.DrawText(
            menu->GetName(),
            glm::vec2(x + m_MenuSpacing, GetPosition().y + (m_MenuHeight - labelSize.y) * 0.5f),
            textColor
        );
        
        x += itemSize.x;
    }

    // Draw active menu
    if (m_ActiveMenu) {
        m_ActiveMenu->OnPaint(renderer);
    }
}

}} // namespace VGE::Editor




