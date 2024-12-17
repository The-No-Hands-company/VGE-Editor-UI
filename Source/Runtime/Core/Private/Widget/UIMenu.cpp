#include "Runtime/Core/Public/Widget/UIMenu.h"
#include "Runtime/Core/Public/Renderer/UIRenderer.h"

namespace VGE {
namespace UI {

// UIMenuItem implementation
UIMenuItem::UIMenuItem(const std::string& text, MenuItemType type)
    : m_Text(text)
    , m_Type(type)
{
}

void UIMenuItem::SetChecked(bool checked) {
    if (m_Type == MenuItemType::Radio || m_Type == MenuItemType::Checkbox) {
        m_IsChecked = checked;
    }
}

void UIMenuItem::SetShortcut(const std::string& shortcutStr) {
    m_Shortcut = MenuShortcut::FromString(shortcutStr);
}

void UIMenuItem::SetSubMenu(std::unique_ptr<UIMenu> menu) {
    m_SubMenu = std::move(menu);
    m_Type = MenuItemType::SubMenu;
}

// MenuShortcut implementation
std::string MenuShortcut::ToString() const {
    std::string result;
    if (Ctrl) result += "Ctrl+";
    if (Shift) result += "Shift+";
    if (Alt) result += "Alt+";
    if (Key) result += static_cast<char>(Key);
    return result;
}

MenuShortcut MenuShortcut::FromString(const std::string& str) {
    MenuShortcut shortcut;
    std::string s = str;
    size_t pos;
    
    while ((pos = s.find('+')) != std::string::npos) {
        std::string modifier = s.substr(0, pos);
        if (modifier == "Ctrl") shortcut.Ctrl = true;
        else if (modifier == "Shift") shortcut.Shift = true;
        else if (modifier == "Alt") shortcut.Alt = true;
        s = s.substr(pos + 1);
    }
    
    if (!s.empty()) {
        shortcut.Key = static_cast<int>(s[0]);
    }
    
    return shortcut;
}

// UIMenu implementation
UIMenu::UIMenu(const std::string& label)
    : UIWidget()
{
    SetName(label);
}

void UIMenu::Update(float deltaTime) {
    if (m_ActiveSubmenu) {
        m_ActiveSubmenu->Update(deltaTime);
    }
}

void UIMenu::Draw(UIRenderer& renderer) {
    if (!IsVisible()) return;

    // Draw menu items
    float y = GetPosition().y;
    for (const auto& item : m_Items) {
        bool isHovered = item.get() == m_HoveredItem;
        
        // Item background
        if (isHovered) {
            renderer.DrawRect(
                glm::vec2(GetPosition().x, y),
                glm::vec2(GetSize().x, GetItemHeight()),
                glm::vec4(0.3f, 0.3f, 0.3f, 1.0f)
            );
        }
        
        // Item text
        glm::vec4 textColor = item->IsEnabled() ? 
            glm::vec4(0.9f, 0.9f, 0.9f, 1.0f) : 
            glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
            
        float textY = y + (GetItemHeight() - renderer.GetTextSize(item->GetText()).y) * 0.5f;
        renderer.DrawText(item->GetText(), glm::vec2(GetPosition().x + 8, textY), textColor);
        
        y += GetItemHeight();
    }

    // Draw active submenu
    if (m_ActiveSubmenu) {
        m_ActiveSubmenu->Draw(renderer);
    }
}

UIMenuItem* UIMenu::AddItem(const std::string& text, std::function<void()> action) {
    auto item = std::make_unique<UIMenuItem>(text);
    if (action) {
        item->SetAction(action);
    }
    UIMenuItem* result = item.get();
    m_Items.push_back(std::move(item));
    return result;
}

UIMenuItem* UIMenu::AddCheckItem(const std::string& text, bool checked) {
    auto item = std::make_unique<UIMenuItem>(text, MenuItemType::Checkbox);
    item->SetChecked(checked);
    UIMenuItem* result = item.get();
    m_Items.push_back(std::move(item));
    return result;
}

UIMenuItem* UIMenu::AddRadioItem(const std::string& text, const std::string& group) {
    auto item = std::make_unique<UIMenuItem>(text, MenuItemType::Radio);
    UIMenuItem* result = item.get();
    m_Items.push_back(std::move(item));
    m_RadioGroups[group].push_back(result);
    return result;
}

UIMenu* UIMenu::AddSubMenu(const std::string& text) {
    auto submenu = std::make_unique<UIMenu>();
    UIMenu* result = submenu.get();
    auto item = std::make_unique<UIMenuItem>(text, MenuItemType::SubMenu);
    item->SetSubMenu(std::move(submenu));
    m_Items.push_back(std::move(item));
    return result;
}

void UIMenu::AddSeparator() {
    m_Items.push_back(std::make_unique<UIMenuItem>("", MenuItemType::Separator));
}

void UIMenu::RemoveItem(UIMenuItem* item) {
    auto it = std::find_if(m_Items.begin(), m_Items.end(),
        [item](const auto& ptr) { return ptr.get() == item; });
    if (it != m_Items.end()) {
        m_Items.erase(it);
    }
}

void UIMenu::Clear() {
    m_Items.clear();
    m_RadioGroups.clear();
    m_HoveredItem = nullptr;
    m_ActiveSubmenu = nullptr;
}

void UIMenu::Show(const glm::vec2& position) {
    SetPosition(position);
    SetVisible(true);
}

void UIMenu::Hide() {
    SetVisible(false);
    if (m_ActiveSubmenu) {
        m_ActiveSubmenu->Hide();
    }
}

UIMenuItem* UIMenu::GetItem(const std::string& text) {
    auto it = std::find_if(m_Items.begin(), m_Items.end(),
        [&text](const auto& item) { return item->GetText() == text; });
    return it != m_Items.end() ? it->get() : nullptr;
}

void UIMenu::SetRadioGroup(const std::string& group, UIMenuItem* checkedItem) {
    auto& items = m_RadioGroups[group];
    for (auto item : items) {
        item->SetChecked(item == checkedItem);
    }
}

UIMenuItem* UIMenu::GetCheckedRadioItem(const std::string& group) const {
    auto it = m_RadioGroups.find(group);
    if (it != m_RadioGroups.end()) {
        for (auto item : it->second) {
            if (item->IsChecked()) {
                return item;
            }
        }
    }
    return nullptr;
}

}} // namespace VGE::UI 