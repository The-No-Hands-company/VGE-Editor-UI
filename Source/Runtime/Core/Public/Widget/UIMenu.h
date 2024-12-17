#pragma once

#include "Core/UICore.h"
#include "Widget/UIWidget.h"
#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <unordered_map>

namespace VGE {
namespace UI {

class UI_API UIMenu : public UIWidget {
public:
    struct MenuItem {
        std::string text;
        std::function<void()> callback;
        bool separator;
        bool enabled;
        bool checked;
        int radioGroup;
        std::vector<MenuItem> subItems;

        MenuItem(const std::string& text = "", const std::function<void()>& callback = nullptr)
            : text(text), callback(callback), separator(false), enabled(true), checked(false), radioGroup(-1) {}

        static MenuItem Separator() {
            MenuItem item;
            item.separator = true;
            return item;
        }
    };

    UIMenu();
    virtual ~UIMenu();

    void AddItem(const MenuItem& item);
    void RemoveItem(const MenuItem& item);
    void AddSeparator();
    void Clear();

    void Show(const glm::vec2& position);
    void Hide();

    virtual void OnRender() override;
    virtual void OnUpdate(float deltaTime) override;
    virtual bool OnMouseMove(const glm::vec2& position) override;
    virtual bool OnMouseButton(int button, bool pressed, const glm::vec2& position) override;

    void SetPosition(const glm::vec2& position) { m_Position = position; }
    void SetVisible(bool visible) { m_IsVisible = visible; }
    bool IsVisible() const { return m_IsVisible; }

private:
    void RenderMenuItem(const MenuItem& item, const glm::vec2& position);
    bool HandleMenuItemClick(const MenuItem& item, const glm::vec2& position);
    void UpdateLayout();

private:
    std::vector<MenuItem> m_Items;
    std::unordered_map<int, std::vector<MenuItem*>> m_RadioGroups;
    std::shared_ptr<UIMenu> m_ActiveSubmenu;
    glm::vec2 m_Position;
    bool m_IsVisible;
    int m_HoveredItem;
    float m_ItemHeight;
    float m_ItemSpacing;
    float m_SubmenuArrowWidth;
};

}} // namespace VGE::UI 