#pragma once

#include "Runtime/Core/Public/Core/UITypes.h"
#include "Runtime/Core/Public/Widget/UIWidget.h"
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <memory>

namespace VGE {
namespace Editor {

class UIMenu;

class UIMenuBar : public UIWidget {
public:
    UIMenuBar() {
        m_MenuHeight = 25.0f;
        m_MenuSpacing = 5.0f;
    }

    virtual ~UIMenuBar() = default;

    // Menu management
    std::shared_ptr<UIMenu> AddMenu(const std::string& name);
    const std::vector<std::shared_ptr<UIMenu>>& GetMenus() const { return m_Menus; }
    
    // UI-specific customization
    void SetMenuHeight(float height) { m_MenuHeight = height; }
    void SetMenuSpacing(float spacing) { m_MenuSpacing = spacing; }
    
    // Mouse interaction
    void OnMouseMove(const glm::vec2& position);
    void OnMouseDown(const glm::vec2& position);
    void OnMouseUp(const glm::vec2& position);
    
    // Update and render
    void OnUpdate(float deltaTime);
    void OnPaint(class UIRenderer& renderer);

protected:
    // UI-specific properties
    float m_MenuHeight;
    float m_MenuSpacing;
    
    // Menu state
    std::vector<std::shared_ptr<UIMenu>> m_Menus;
    UIMenu* m_HoveredMenu = nullptr;
    UIMenu* m_ActiveMenu = nullptr;
    
    // Helper methods
    UIMenu* GetHoveredMenu() const { return m_HoveredMenu; }
    UIMenu* GetActiveMenu() const { return m_ActiveMenu; }
};

}} // namespace VGE::Editor



