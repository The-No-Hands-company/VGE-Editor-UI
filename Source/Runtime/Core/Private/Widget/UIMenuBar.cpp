#include "Runtime/Core/Public/Widget/UIMenuBar.h"
#include "Runtime/Core/Public/Renderer/UIRenderer.h"

namespace VGE {
namespace UI {

UIMenuBar::UIMenuBar()
    : UIWidget()
{
}

void UIMenuBar::Update(float deltaTime) {
    if (m_ActiveMenu) {
        m_ActiveMenu->Update(deltaTime);
    }
}

void UIMenuBar::Draw(UIRenderer& renderer) {
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
        m_ActiveMenu->Draw(renderer);
    }
}

UIMenu* UIMenuBar::AddMenu(const std::string& text) {
    auto menu = std::make_unique<UIMenu>(text);
    UIMenu* result = menu.get();
    m_Menus.push_back(std::move(menu));
    UpdateLayout();
    return result;
}

void UIMenuBar::RemoveMenu(UIMenu* menu) {
    auto it = std::find_if(m_Menus.begin(), m_Menus.end(),
        [menu](const auto& ptr) { return ptr.get() == menu; });
    if (it != m_Menus.end()) {
        m_Menus.erase(it);
        UpdateLayout();
    }
}

void UIMenuBar::ClearMenus() {
    m_Menus.clear();
    m_ActiveMenu = nullptr;
    m_HoveredMenu = nullptr;
    UpdateLayout();
}

bool UIMenuBar::OnMouseMove(const glm::vec2& mousePos) {
    UIMenu* hitMenu = HitTest(mousePos);
    
    if (hitMenu != m_HoveredMenu) {
        m_HoveredMenu = hitMenu;
        if (m_ActiveMenu && hitMenu) {
            m_ActiveMenu = hitMenu;
            m_ActiveMenu->Show(glm::vec2(
                hitMenu->GetPosition().x,
                GetPosition().y + m_MenuHeight
            ));
        }
    }
    
    if (m_ActiveMenu) {
        return m_ActiveMenu->OnMouseMove(mousePos);
    }
    
    return hitMenu != nullptr;
}

bool UIMenuBar::OnMouseDown(const glm::vec2& mousePos) {
    if (UIMenu* hitMenu = HitTest(mousePos)) {
        if (m_ActiveMenu != hitMenu) {
            if (m_ActiveMenu) {
                m_ActiveMenu->Hide();
            }
            m_ActiveMenu = hitMenu;
            m_ActiveMenu->Show(glm::vec2(
                hitMenu->GetPosition().x,
                GetPosition().y + m_MenuHeight
            ));
        }
        return true;
    }
    else if (m_ActiveMenu) {
        return m_ActiveMenu->OnMouseDown(mousePos);
    }
    return false;
}

bool UIMenuBar::OnMouseUp(const glm::vec2& mousePos) {
    if (m_ActiveMenu) {
        bool handled = m_ActiveMenu->OnMouseUp(mousePos);
        if (!handled && !HitTest(mousePos)) {
            CloseActiveMenu();
        }
        return handled;
    }
    return false;
}

void UIMenuBar::CloseActiveMenu() {
    if (m_ActiveMenu) {
        m_ActiveMenu->Hide();
        m_ActiveMenu = nullptr;
    }
}

UIMenu* UIMenuBar::HitTest(const glm::vec2& position) const {
    float x = GetPosition().x;
    for (const auto& menu : m_Menus) {
        glm::vec2 labelSize = GetRenderer()->GetTextSize(menu->GetName());
        glm::vec2 itemSize(labelSize.x + m_MenuSpacing * 2, m_MenuHeight);
        
        if (position.x >= x && position.x < x + itemSize.x &&
            position.y >= GetPosition().y && position.y < GetPosition().y + m_MenuHeight) {
            return menu.get();
        }
        
        x += itemSize.x;
    }
    return nullptr;
}

void UIMenuBar::UpdateLayout() {
    float totalWidth = 0;
    for (const auto& menu : m_Menus) {
        glm::vec2 labelSize = GetRenderer()->GetTextSize(menu->GetName());
        totalWidth += labelSize.x + m_MenuSpacing * 2;
    }
    SetSize(glm::vec2(totalWidth, m_MenuHeight));
}

}} // namespace VGE::UI 