#include "Runtime/UI/Public/Controls/UIMenu.h"
#include "Runtime/Core/Public/Core/UIRenderer.h"

namespace VGE {
namespace UI {

void UIMenu::Draw(UIRenderer& renderer) {
    if (!IsVisible()) return;

    // Draw menu background with UI-specific styling
    renderer.DrawRect(GetPosition(), GetSize(), glm::vec4(0.2f, 0.2f, 0.2f, 0.95f));
    renderer.DrawRectOutline(GetPosition(), GetSize(), glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));

    // Draw menu items with UI-specific styling
    float y = GetPosition().y;
    for (const auto& item : GetItems()) {
        bool isHovered = item.get() == GetHoveredItem();
        
        // Item background
        if (isHovered) {
            renderer.DrawRect(
                glm::vec2(GetPosition().x, y),
                glm::vec2(GetSize().x, m_ItemHeight),
                glm::vec4(0.3f, 0.3f, 0.3f, 1.0f)
            );
        }
        
        // Item text
        glm::vec4 textColor = item->IsEnabled() ? 
            glm::vec4(0.9f, 0.9f, 0.9f, 1.0f) : 
            glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
            
        float textY = y + (m_ItemHeight - renderer.GetTextSize(item->GetText()).y) * 0.5f;
        
        // Draw icon if present
        float textX = GetPosition().x + m_IconSize + m_ItemHeight * 0.5f;
        if (!item->GetIcon().empty()) {
            renderer.DrawIcon(
                item->GetIcon(),
                glm::vec2(GetPosition().x + m_ItemHeight * 0.25f, y + (m_ItemHeight - m_IconSize) * 0.5f),
                glm::vec2(m_IconSize),
                textColor
            );
        }
        
        // Draw checkbox/radio state
        if (item->GetType() == MenuItemType::Checkbox || item->GetType() == MenuItemType::Radio) {
            if (item->IsChecked()) {
                renderer.DrawIcon(
                    item->GetType() == MenuItemType::Checkbox ? "check" : "radio",
                    glm::vec2(GetPosition().x + 4, y + (m_ItemHeight - 16) * 0.5f),
                    glm::vec2(16),
                    textColor
                );
            }
        }
        
        // Draw text
        renderer.DrawText(item->GetText(), glm::vec2(textX, textY), textColor);
        
        // Draw shortcut if present
        const auto& shortcut = item->GetShortcut();
        std::string shortcutText = shortcut.ToString();
        if (!shortcutText.empty()) {
            glm::vec2 shortcutSize = renderer.GetTextSize(shortcutText);
            renderer.DrawText(
                shortcutText,
                glm::vec2(
                    GetPosition().x + GetSize().x - shortcutSize.x - m_ItemHeight * 0.5f,
                    textY
                ),
                glm::vec4(0.6f, 0.6f, 0.6f, 1.0f)
            );
        }
        
        // Draw submenu arrow
        if (item->GetType() == MenuItemType::SubMenu) {
            renderer.DrawIcon(
                "arrow_right",
                glm::vec2(
                    GetPosition().x + GetSize().x - 16 - 4,
                    y + (m_ItemHeight - 16) * 0.5f
                ),
                glm::vec2(16),
                textColor
            );
        }
        
        // Draw separator
        if (item->GetType() == MenuItemType::Separator) {
            renderer.DrawLine(
                glm::vec2(GetPosition().x + 4, y + m_ItemHeight * 0.5f),
                glm::vec2(GetPosition().x + GetSize().x - 4, y + m_ItemHeight * 0.5f),
                glm::vec4(0.3f, 0.3f, 0.3f, 1.0f)
            );
        }
        
        y += m_ItemHeight;
    }

    // Draw scrollbar if needed
    float contentHeight = GetItems().size() * m_ItemHeight;
    if (contentHeight > m_MaxHeight) {
        float scrollbarHeight = (m_MaxHeight / contentHeight) * m_MaxHeight;
        float scrollbarY = GetPosition().y + (m_ScrollOffset / contentHeight) * m_MaxHeight;
        
        renderer.DrawRect(
            glm::vec2(GetPosition().x + GetSize().x - 8, GetPosition().y),
            glm::vec2(8, m_MaxHeight),
            glm::vec4(0.15f, 0.15f, 0.15f, 1.0f)
        );
        
        renderer.DrawRect(
            glm::vec2(GetPosition().x + GetSize().x - 7, scrollbarY),
            glm::vec2(6, scrollbarHeight),
            glm::vec4(0.4f, 0.4f, 0.4f, 1.0f)
        );
    }
}

}} // namespace VGE::UI



