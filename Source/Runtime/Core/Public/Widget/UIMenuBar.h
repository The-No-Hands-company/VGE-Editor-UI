#pragma once

#include "Core/UICore.h"
#include "Widget/UIWidget.h"
#include "Widget/UIMenu.h"
#include <string>
#include <vector>
#include <memory>

namespace VGE {
namespace UI {

class UI_API UIMenuBar : public UIWidget {
public:
    UIMenuBar();
    virtual ~UIMenuBar();

    std::shared_ptr<UIMenu> AddMenu(const std::string& text);
    void RemoveMenu(const std::string& text);
    void Clear();

    virtual void OnRender() override;
    virtual void OnUpdate(float deltaTime) override;
    virtual bool OnMouseMove(const glm::vec2& position) override;
    virtual bool OnMouseButton(int button, bool pressed, const glm::vec2& position) override;

private:
    struct MenuEntry {
        std::string text;
        std::shared_ptr<UIMenu> menu;
        bool isOpen;
    };

    std::vector<MenuEntry> m_Menus;
    int m_HoveredMenu;
    bool m_IsActive;
};

}} // namespace VGE::UI 