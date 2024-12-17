#include "Runtime/UI/Public/Controls/UIToolbar.h"
#include "Runtime/Core/Public/Theme/UITheme.h"
#include "Runtime/Core/Public/Widget/UIWidget.h"
#include "Runtime/Core/Public/Renderer/UIRenderer.h"
#include "Runtime/Core/Public/Input/Input.h"
#include "Runtime/Core/Public/Base/VGE.h"
#include <glm/gtc/constants.hpp>
#include <memory>

namespace VGE {
namespace Editor {

using namespace VGE::UI;

// UIToolbarItem implementation
UIToolbarItem::UIToolbarItem(const std::string& text, ToolbarItemType type)
    : m_Text(text), m_Type(type) {}

void UIToolbarItem::SetToggled(bool toggled) {
    if (m_Type != ToolbarItemType::ToggleButton) return;
    m_IsToggled = toggled;
}

void UIToolbarItem::SetCustomWidget(std::unique_ptr<UIWidget> widget) {
    m_CustomWidget = std::move(widget);
    m_Type = ToolbarItemType::Custom;
}

void UIToolbarItem::SetDropdownMenu(std::unique_ptr<UIMenu> menu) {
    m_DropdownMenu = std::move(menu);
    m_Type = ToolbarItemType::Dropdown;
}

// UIToolbar implementation
UIToolbar::UIToolbar(ToolbarOrientation orientation)
    : m_Orientation(orientation) {}

void UIToolbar::Update() {
    // Update hover animation
    float targetHoverAlpha = m_HoveredItem ? 1.0f : 0.0f;
    m_HoverAlpha = glm::mix(m_HoverAlpha, targetHoverAlpha, GetDeltaTime() * 10.0f);

    // Update custom widgets
    for (auto& item : m_Items) {
        if (item->GetType() == ToolbarItemType::Custom && item->GetCustomWidget()) {
            item->GetCustomWidget()->Update();
        }
    }
    
    UpdateLayout();
    UpdateStyle();
}

void UIToolbar::Draw() {
    auto* renderer = Renderer::Get();
    const auto& theme = UITheme::Get();
    
    // Draw toolbar background
    renderer->DrawRect(GetPosition(), GetSize(), theme.ToolbarBackground);
    
    // Draw items
    size_t visibleCount = 0;
    for (size_t i = 0; i < m_Items.size(); ++i) {
        if (!IsItemVisible(i)) continue;
        
        auto* item = m_Items[i].get();
        glm::vec2 itemPos = CalculateItemPosition(i);
        bool isHovered = item == m_HoveredItem;
        
        switch (item->GetType()) {
            case ToolbarItemType::Button:
                DrawButton(item, itemPos, isHovered);
                break;
                
            case ToolbarItemType::ToggleButton:
                DrawToggleButton(item, itemPos, isHovered);
                break;
                
            case ToolbarItemType::Dropdown:
                DrawDropdown(item, itemPos, isHovered);
                break;
                
            case ToolbarItemType::Separator:
                DrawSeparator(itemPos);
                break;
                
            case ToolbarItemType::Custom:
                if (auto* widget = item->GetCustomWidget()) {
                    widget->SetPosition(itemPos);
                    widget->Draw();
                }
                break;
        }
        
        visibleCount++;
    }
    
    // Draw overflow button if needed
    if (m_OverflowEnabled && visibleCount < m_Items.size()) {
        glm::vec2 overflowPos = CalculateItemPosition(visibleCount);
        DrawOverflowButton(overflowPos, m_HoveredItem == nullptr);
    }
}

void UIToolbar::UpdateStyle() {
    const auto& theme = UITheme::Get();
    
    // Update base properties
    m_ItemSpacing = theme.ToolbarItemSpacing;
    m_IconSize = theme.ToolbarIconSize;
    m_MinButtonWidth = theme.ToolbarMinButtonWidth;
    
    // Update animations
    float deltaTime = GetDeltaTime();
    
    // Update hover animation
    float targetHoverAlpha = m_HoveredItem ? 1.0f : 0.0f;
    m_HoverAlpha = glm::mix(m_HoverAlpha, targetHoverAlpha, deltaTime * theme.ToolbarHoverAnimationSpeed);
    
    // Update press animation
    float targetPressAlpha = m_PressedItem ? 1.0f : 0.0f;
    m_PressAlpha = glm::mix(m_PressAlpha, targetPressAlpha, deltaTime * theme.ToolbarPressAnimationSpeed);
}

void UIToolbar::ApplyItemStyle(UIToolbarItem* item, const glm::vec2& position, bool isHovered, bool isPressed) {
    if (!item) return;
    
    auto* renderer = Renderer::Get();
    const auto& theme = UITheme::Get();
    
    float width = CalculateItemWidth(item);
    float height = CalculateItemHeight(item);
    glm::vec2 size(width, height);
    
    // Calculate background color with hover and press effects
    glm::vec4 bgColor = theme.ToolbarItemBackground;
    if (!item->IsEnabled()) {
        bgColor = theme.ToolbarItemBackgroundDisabled;
    } else {
        if (isHovered) {
            bgColor = glm::mix(bgColor, theme.ToolbarItemHoverBackground, m_HoverAlpha);
        }
        if (isPressed) {
            bgColor = glm::mix(bgColor, theme.ToolbarItemPressBackground, m_PressAlpha);
        }
        if (item->GetType() == ToolbarItemType::ToggleButton && item->IsToggled()) {
            bgColor = glm::mix(bgColor, theme.ToolbarItemToggleBackground, 0.8f);
        }
    }
    
    // Draw background with rounded corners and shadow
    if (theme.ToolbarItemShadowEnabled) {
        renderer->DrawRectShadow(
            position + theme.ToolbarItemShadowOffset,
            size,
            theme.ToolbarItemShadowColor,
            theme.ToolbarItemShadowBlur
        );
    }
    
    renderer->DrawRectRounded(
        position,
        size,
        bgColor,
        theme.ToolbarItemCornerRadius
    );
    
    // Draw hover glow effect
    if (isHovered && item->IsEnabled()) {
        renderer->DrawRectGlow(
            position,
            size,
            theme.ToolbarItemHoverGlow,
            m_HoverAlpha * theme.ToolbarItemGlowIntensity,
            theme.ToolbarItemGlowRadius
        );
    }
    
    // Draw pressed effect
    if (isPressed && item->IsEnabled()) {
        renderer->DrawRectInnerShadow(
            position,
            size,
            theme.ToolbarItemPressShadow,
            theme.ToolbarItemPressShadowSize
        );
    }
    
    // Draw focus outline
    if (item->HasFocus()) {
        renderer->DrawRectOutlineRounded(
            position - glm::vec2(2.0f),
            size + glm::vec2(4.0f),
            theme.ToolbarItemFocusOutline,
            theme.ToolbarItemCornerRadius + 2.0f,
            theme.ToolbarItemFocusOutlineWidth
        );
    }
    
    // Draw content
    float contentX = position.x + theme.ToolbarItemPadding.x;
    float contentY = position.y + height * 0.5f;
    
    // Draw icon
    if (!item->GetIcon().empty()) {
        glm::vec4 iconColor = item->IsEnabled() ?
            (item->IsToggled() ? theme.ToolbarItemIconToggled : theme.ToolbarItemIcon) :
            theme.ToolbarItemIconDisabled;
            
        if (isHovered) {
            iconColor = glm::mix(iconColor, theme.ToolbarItemIconHover, m_HoverAlpha);
        }
        
        renderer->DrawIcon(
            item->GetIcon(),
            glm::vec2(contentX, contentY - m_IconSize * 0.5f),
            glm::vec2(m_IconSize),
            iconColor
        );
        contentX += m_IconSize + theme.ToolbarItemIconTextSpacing;
    }
    
    // Draw text
    if (!item->GetText().empty()) {
        glm::vec4 textColor = item->IsEnabled() ?
            (item->IsToggled() ? theme.ToolbarItemTextToggled :
             (isHovered ? theme.ToolbarItemTextHover : theme.ToolbarItemText)) :
            theme.ToolbarItemTextDisabled;
            
        renderer->DrawText(
            item->GetText(),
            glm::vec2(contentX, contentY),
            textColor,
            TextAlignment::Left | TextAlignment::VCenter,
            theme.ToolbarItemFont
        );
    }
}

void UIToolbar::DrawButton(UIToolbarItem* item, const glm::vec2& position, bool isHovered) {
    ApplyItemStyle(item, position, isHovered, m_PressedItem == item);
}

void UIToolbar::DrawToggleButton(UIToolbarItem* item, const glm::vec2& position, bool isHovered) {
    ApplyItemStyle(item, position, isHovered, m_PressedItem == item);
    
    // Draw toggle indicator
    if (item->IsToggled()) {
        auto* renderer = Renderer::Get();
        const auto& theme = UITheme::Get();
        
        float width = CalculateItemWidth(item);
        float height = CalculateItemHeight(item);
        
        switch (theme.ToolbarToggleIndicatorStyle) {
            case ToggleIndicatorStyle::Bottom:
                renderer->DrawRect(
                    glm::vec2(position.x, position.y + height - theme.ToolbarToggleIndicatorSize),
                    glm::vec2(width, theme.ToolbarToggleIndicatorSize),
                    theme.ToolbarItemToggleIndicator
                );
                break;
                
            case ToggleIndicatorStyle::Left:
                renderer->DrawRect(
                    glm::vec2(position.x, position.y),
                    glm::vec2(theme.ToolbarToggleIndicatorSize, height),
                    theme.ToolbarItemToggleIndicator
                );
                break;
                
            case ToggleIndicatorStyle::Background:
                // Already handled in ApplyItemStyle
                break;
        }
    }
}

void UIToolbar::DrawDropdown(UIToolbarItem* item, const glm::vec2& position, bool isHovered) {
    ApplyItemStyle(item, position, isHovered, m_PressedItem == item);
    
    // Draw dropdown arrow
    auto* renderer = Renderer::Get();
    const auto& theme = UITheme::Get();
    
    float width = CalculateItemWidth(item);
    float arrowSize = theme.ToolbarDropdownArrowSize;
    float arrowX = position.x + width - arrowSize - theme.ToolbarItemPadding.x;
    float arrowY = position.y + CalculateItemHeight(item) * 0.5f;
    
    glm::vec4 arrowColor = item->IsEnabled() ?
        (isHovered ? theme.ToolbarItemIconHover : theme.ToolbarItemIcon) :
        theme.ToolbarItemIconDisabled;
        
    renderer->DrawIcon(
        theme.ToolbarDropdownArrowIcon,
        glm::vec2(arrowX, arrowY - arrowSize * 0.5f),
        glm::vec2(arrowSize),
        arrowColor
    );
}

void UIToolbar::DrawSeparator(const glm::vec2& position) {
    auto* renderer = Renderer::Get();
    const auto& theme = UITheme::Get();
    
    if (m_Orientation == ToolbarOrientation::Horizontal) {
        float height = CalculateItemHeight(nullptr);
        float lineHeight = height * theme.ToolbarSeparatorSizeRatio;
        float y = position.y + (height - lineHeight) * 0.5f;
        
        renderer->DrawLine(
            glm::vec2(position.x + theme.ToolbarItemPadding.x, y),
            glm::vec2(position.x + theme.ToolbarItemPadding.x, y + lineHeight),
            theme.ToolbarSeparator
        );
    } else {
        float width = CalculateItemWidth(nullptr);
        float lineWidth = width * theme.ToolbarSeparatorSizeRatio;
        float x = position.x + (width - lineWidth) * 0.5f;
        
        renderer->DrawLine(
            glm::vec2(x, position.y + theme.ToolbarItemPadding.y),
            glm::vec2(x + lineWidth, position.y + theme.ToolbarItemPadding.y),
            theme.ToolbarSeparator
        );
    }
}

void UIToolbar::DrawOverflowButton(const glm::vec2& position, bool isHovered) {
    auto* renderer = Renderer::Get();
    const auto& theme = UITheme::Get();
    
    float width = m_MinButtonWidth;
    float height = CalculateItemHeight(nullptr);
    glm::vec2 size(width, height);
    
    // Draw button background
    glm::vec4 bgColor = isHovered ?
        glm::mix(theme.ToolbarItemBackground, theme.ToolbarItemHoverBackground, m_HoverAlpha) :
        theme.ToolbarItemBackground;
        
    renderer->DrawRectRounded(position, size, bgColor, 4.0f);
    
    if (isHovered) {
        renderer->DrawRectGlow(
            position,
            size,
            theme.ToolbarItemHoverGlow,
            m_HoverAlpha * 0.5f,
            4.0f
        );
    }
    
    // Draw overflow dots
    float dotSize = 3.0f;
    float dotSpacing = 2.0f;
    float totalWidth = dotSize * 3 + dotSpacing * 2;
    float x = position.x + (width - totalWidth) * 0.5f;
    float y = position.y + height * 0.5f;
    
    glm::vec4 dotColor = isHovered ?
        theme.ToolbarItemTextHover :
        theme.ToolbarItemText;
        
    for (int i = 0; i < 3; ++i) {
        renderer->DrawRect(
            glm::vec2(x + (dotSize + dotSpacing) * i, y - dotSize * 0.5f),
            glm::vec2(dotSize),
            dotColor
        );
    }
}

bool UIToolbar::HandleEvent(Event& event) {
    // First let active dropdown menus handle the event
    for (auto& item : m_Items) {
        if (item->GetType() == ToolbarItemType::Dropdown && 
            item->GetDropdownMenu() && 
            item->GetDropdownMenu()->HandleEvent(event)) {
            return true;
        }
    }
    
    // Handle overflow menu events
    if (m_OverflowMenu && m_OverflowMenu->HandleEvent(event)) {
        return true;
    }
    
    switch (event.Type) {
        case EventType::MouseMove:
            return HandleMouseMove(event);
            
        case EventType::MouseButton:
            return HandleMouseButton(event);
            
        case EventType::MouseScroll:
            return HandleMouseScroll(event);
            
        case EventType::KeyPress:
            return HandleKeyPress(event);
            
        case EventType::KeyRelease:
            return HandleKeyRelease(event);
            
        case EventType::Focus:
            return HandleFocus(event);
            
        case EventType::FocusLost:
            return HandleFocusLost(event);
            
        default:
            break;
    }
    
    return false;
}

bool UIToolbar::HandleMouseMove(const Event& event) {
    UIToolbarItem* oldHovered = m_HoveredItem;
    m_HoveredItem = HitTest(event.MousePosition);
    
    // Show tooltip if hovering over an item
    if (m_HoveredItem != oldHovered) {
        if (m_HoveredItem && !m_HoveredItem->GetTooltip().empty()) {
            UITooltip::Show(m_HoveredItem->GetTooltip(), event.MousePosition);
        } else {
            UITooltip::Hide();
        }
    }
    
    // Handle hover animations
    if (m_HoveredItem != oldHovered) {
        m_HoverAlpha = 0.0f;  // Reset hover animation
        
        // Trigger accessibility events
        if (oldHovered) {
            TriggerEvent(EventType::AccessibilityHoverEnd, oldHovered);
        }
        if (m_HoveredItem) {
            TriggerEvent(EventType::AccessibilityHoverStart, m_HoveredItem);
        }
    }
    
    return m_HoveredItem != nullptr;
}

bool UIToolbar::HandleMouseButton(const Event& event) {
    if (event.MouseButton == MouseButton::Left) {
        if (event.ButtonState == ButtonState::Pressed) {
            if (auto* item = HitTest(event.MousePosition)) {
                if (item->IsEnabled()) {
                    m_PressedItem = item;
                    m_PressAlpha = 1.0f;
                    
                    // Handle different item types
                    switch (item->GetType()) {
                        case ToolbarItemType::Button:
                            item->Execute();
                            TriggerEvent(EventType::AccessibilityAction, item);
                            return true;
                            
                        case ToolbarItemType::ToggleButton:
                            item->SetToggled(!item->IsToggled());
                            item->Execute();
                            TriggerEvent(EventType::AccessibilityToggle, item);
                            return true;
                            
                        case ToolbarItemType::Dropdown:
                            ShowDropdownMenu(item);
                            TriggerEvent(EventType::AccessibilityExpand, item);
                            return true;
                            
                        case ToolbarItemType::Custom:
                            if (auto* widget = item->GetCustomWidget()) {
                                Event customEvent = event;
                                customEvent.MousePosition -= widget->GetPosition();
                                return widget->HandleEvent(customEvent);
                            }
                            break;
                            
                        default:
                            break;
                    }
                } else {
                    // Play error sound for disabled items
                    Audio::PlaySound("error");
                }
            } else if (m_OverflowEnabled && IsOverflowButtonHovered(event.MousePosition)) {
                ShowOverflowMenu();
                return true;
            }
        } else if (event.ButtonState == ButtonState::Released) {
            m_PressedItem = nullptr;
            m_PressAlpha = 0.0f;
        }
    }
    
    return false;
}

bool UIToolbar::HandleMouseScroll(const Event& event) {
    if (!m_OverflowEnabled || m_Items.empty()) return false;
    
    // Calculate scroll amount
    int scrollAmount = static_cast<int>(event.ScrollDelta.y);
    if (scrollAmount == 0) return false;
    
    // Calculate new first visible item
    int newFirst = static_cast<int>(m_FirstVisibleItem) - scrollAmount;
    newFirst = glm::clamp(newFirst, 0, static_cast<int>(m_Items.size() - 1));
    
    if (newFirst != static_cast<int>(m_FirstVisibleItem)) {
        m_FirstVisibleItem = static_cast<size_t>(newFirst);
        UpdateLayout();
        
        // Announce scroll for screen readers
        std::string announcement = "Scrolled to item " + 
            std::to_string(m_FirstVisibleItem + 1) + " of " + 
            std::to_string(m_Items.size());
        TriggerEvent(EventType::AccessibilityAnnounce, announcement);
        
        return true;
    }
    
    return false;
}

bool UIToolbar::HandleKeyPress(const Event& event) {
    // Handle keyboard navigation
    switch (event.Key) {
        case Key::Tab:
            if (event.Modifiers & KeyModifier::Shift) {
                FocusPreviousItem();
            } else {
                FocusNextItem();
            }
            return true;
            
        case Key::Left:
            if (m_Orientation == ToolbarOrientation::Horizontal) {
                FocusPreviousItem();
                return true;
            }
            break;
            
        case Key::Right:
            if (m_Orientation == ToolbarOrientation::Horizontal) {
                FocusNextItem();
                return true;
            }
            break;
            
        case Key::Up:
            if (m_Orientation == ToolbarOrientation::Vertical) {
                FocusPreviousItem();
                return true;
            }
            break;
            
        case Key::Down:
            if (m_Orientation == ToolbarOrientation::Vertical) {
                FocusNextItem();
                return true;
            }
            break;
            
        case Key::Space:
        case Key::Enter:
            if (auto* item = GetFocusedItem()) {
                if (item->IsEnabled()) {
                    switch (item->GetType()) {
                        case ToolbarItemType::Button:
                            item->Execute();
                            TriggerEvent(EventType::AccessibilityAction, item);
                            return true;
                            
                        case ToolbarItemType::ToggleButton:
                            item->SetToggled(!item->IsToggled());
                            item->Execute();
                            TriggerEvent(EventType::AccessibilityToggle, item);
                            return true;
                            
                        case ToolbarItemType::Dropdown:
                            ShowDropdownMenu(item);
                            TriggerEvent(EventType::AccessibilityExpand, item);
                            return true;
                            
                        default:
                            break;
                    }
                } else {
                    Audio::PlaySound("error");
                }
            }
            break;
            
        case Key::Escape:
            if (m_OverflowMenu && m_OverflowMenu->IsShown()) {
                m_OverflowMenu->Hide();
                return true;
            }
            break;
            
        default:
            // Handle access keys (Alt + letter)
            if (event.Modifiers & KeyModifier::Alt) {
                char key = static_cast<char>(event.Key);
                for (auto& item : m_Items) {
                    if (!item->GetText().empty() && 
                        std::toupper(item->GetText()[0]) == std::toupper(key)) {
                        if (item->IsEnabled()) {
                            item->Execute();
                            TriggerEvent(EventType::AccessibilityAction, item);
                        } else {
                            Audio::PlaySound("error");
                        }
                        return true;
                    }
                }
            }
            break;
    }
    
    return false;
}

bool UIToolbar::HandleKeyRelease(const Event& event) {
    // Handle any key release specific logic
    return false;
}

bool UIToolbar::HandleFocus(const Event& event) {
    // Focus the first item if none is focused
    if (!GetFocusedItem() && !m_Items.empty()) {
        FocusItem(m_Items[m_FirstVisibleItem].get());
    }
    return true;
}

bool UIToolbar::HandleFocusLost(const Event& event) {
    // Clear focus state
    if (auto* focused = GetFocusedItem()) {
        focused->SetFocus(false);
        TriggerEvent(EventType::AccessibilityFocusLost, focused);
    }
    return true;
}

void UIToolbar::FocusNextItem() {
    size_t currentIndex = GetFocusedItemIndex();
    size_t nextIndex = currentIndex;
    
    do {
        nextIndex = (nextIndex + 1) % m_Items.size();
    } while (nextIndex != currentIndex && 
             (!m_Items[nextIndex]->IsEnabled() || 
              m_Items[nextIndex]->GetType() == ToolbarItemType::Separator));
    
    if (nextIndex != currentIndex) {
        FocusItem(m_Items[nextIndex].get());
        ScrollToItem(nextIndex);
    }
}

void UIToolbar::FocusPreviousItem() {
    size_t currentIndex = GetFocusedItemIndex();
    size_t prevIndex = currentIndex;
    
    do {
        prevIndex = (prevIndex + m_Items.size() - 1) % m_Items.size();
    } while (prevIndex != currentIndex && 
             (!m_Items[prevIndex]->IsEnabled() || 
              m_Items[prevIndex]->GetType() == ToolbarItemType::Separator));
    
    if (prevIndex != currentIndex) {
        FocusItem(m_Items[prevIndex].get());
        ScrollToItem(prevIndex);
    }
}

void UIToolbar::FocusItem(UIToolbarItem* item) {
    if (auto* focused = GetFocusedItem()) {
        focused->SetFocus(false);
        TriggerEvent(EventType::AccessibilityFocusLost, focused);
    }
    
    if (item) {
        item->SetFocus(true);
        TriggerEvent(EventType::AccessibilityFocus, item);
        
        // Announce item for screen readers
        std::string announcement = item->GetText();
        if (!item->GetTooltip().empty()) {
            announcement += ". " + item->GetTooltip();
        }
        if (!item->IsEnabled()) {
            announcement += " (Disabled)";
        }
        TriggerEvent(EventType::AccessibilityAnnounce, announcement);
    }
}

size_t UIToolbar::GetFocusedItemIndex() const {
    for (size_t i = 0; i < m_Items.size(); ++i) {
        if (m_Items[i]->HasFocus()) {
            return i;
        }
    }
    return 0;
}

UIToolbarItem* UIToolbar::GetFocusedItem() const {
    for (const auto& item : m_Items) {
        if (item->HasFocus()) {
            return item.get();
        }
    }
    return nullptr;
}

void UIToolbar::TriggerEvent(EventType type, UIToolbarItem* item) {
    Event event;
    event.Type = type;
    event.Sender = this;
    event.Data = item;
    EventSystem::Trigger(event);
}

void UIToolbar::TriggerEvent(EventType type, const std::string& message) {
    Event event;
    event.Type = type;
    event.Sender = this;
    event.Data = message;
    EventSystem::Trigger(event);
}

UIToolbarItem* UIToolbar::AddButton(const std::string& text, const std::string& icon) {
    auto item = std::make_unique<UIToolbarItem>(text, ToolbarItemType::Button);
    item->SetIcon(icon);
    UIToolbarItem* ptr = item.get();
    m_Items.push_back(std::move(item));
    return ptr;
}

UIToolbarItem* UIToolbar::AddToggleButton(const std::string& text, const std::string& icon) {
    auto item = std::make_unique<UIToolbarItem>(text, ToolbarItemType::ToggleButton);
    item->SetIcon(icon);
    UIToolbarItem* ptr = item.get();
    m_Items.push_back(std::move(item));
    return ptr;
}

UIToolbarItem* UIToolbar::AddDropdown(const std::string& text, const std::string& icon) {
    auto item = std::make_unique<UIToolbarItem>(text, ToolbarItemType::Dropdown);
    item->SetIcon(icon);
    UIToolbarItem* ptr = item.get();
    m_Items.push_back(std::move(item));
    return ptr;
}

UIToolbarItem* UIToolbar::AddCustomWidget(std::unique_ptr<UIWidget> widget) {
    auto item = std::make_unique<UIToolbarItem>("", ToolbarItemType::Custom);
    item->SetCustomWidget(std::move(widget));
    UIToolbarItem* ptr = item.get();
    m_Items.push_back(std::move(item));
    return ptr;
}

void UIToolbar::AddSeparator() {
    m_Items.push_back(std::make_unique<UIToolbarItem>("", ToolbarItemType::Separator));
}

void UIToolbar::RemoveItem(UIToolbarItem* item) {
    auto it = std::find_if(m_Items.begin(), m_Items.end(),
        [item](const auto& ptr) { return ptr.get() == item; });
    if (it != m_Items.end()) {
        m_Items.erase(it);
    }
}

void UIToolbar::Clear() {
    m_Items.clear();
    m_HoveredItem = nullptr;
}

void UIToolbar::SetOrientation(ToolbarOrientation orientation) {
    if (m_Orientation != orientation) {
        m_Orientation = orientation;
        // Recalculate layout
    }
}

float UIToolbar::CalculateItemWidth(UIToolbarItem* item) const {
    if (!item) return m_MinButtonWidth;
    
    // Handle fixed width items
    if (item->GetFixedWidth() > 0) {
        return item->GetFixedWidth();
    }
    
    float width = 0;
    
    // Add icon width
    if (!item->GetIcon().empty()) {
        width += m_IconSize + m_ItemSpacing;
    }
    
    // Add text width
    if (!item->GetText().empty()) {
        auto* renderer = Renderer::Get();
        width += renderer->GetTextWidth(item->GetText());
        width += m_ItemSpacing * 2;  // Padding on both sides
    }
    
    // Add dropdown arrow width
    if (item->GetType() == ToolbarItemType::Dropdown) {
        width += m_IconSize + m_ItemSpacing;
    }
    
    // Handle custom widgets
    if (item->GetType() == ToolbarItemType::Custom && item->GetCustomWidget()) {
        width = std::max(width, item->GetCustomWidget()->GetWidth());
    }
    
    // Ensure minimum width
    return std::max(width, m_MinButtonWidth);
}

float UIToolbar::CalculateItemHeight(UIToolbarItem* item) const {
    if (!item) return m_IconSize + m_ItemSpacing * 2;
    
    // Handle fixed height items
    if (item->GetFixedHeight() > 0) {
        return item->GetFixedHeight();
    }
    
    float height = m_IconSize + m_ItemSpacing * 2;  // Default height
    
    // Handle custom widgets
    if (item->GetType() == ToolbarItemType::Custom && item->GetCustomWidget()) {
        height = std::max(height, item->GetCustomWidget()->GetHeight());
    }
    
    return height;
}

glm::vec2 UIToolbar::CalculateItemPosition(size_t index) const {
    if (index >= m_Items.size()) return GetPosition();
    
    glm::vec2 pos = GetPosition();
    
    if (m_Orientation == ToolbarOrientation::Horizontal) {
        float x = pos.x;
        for (size_t i = m_FirstVisibleItem; i < index; ++i) {
            if (i >= m_Items.size()) break;
            x += CalculateItemWidth(m_Items[i].get()) + m_ItemSpacing;
        }
        return glm::vec2(x, pos.y);
    } else {
        float y = pos.y;
        for (size_t i = m_FirstVisibleItem; i < index; ++i) {
            if (i >= m_Items.size()) break;
            y += CalculateItemHeight(m_Items[i].get()) + m_ItemSpacing;
        }
        return glm::vec2(pos.x, y);
    }
}

void UIToolbar::UpdateLayout() {
    if (m_Items.empty()) {
        m_FirstVisibleItem = 0;
        m_LastVisibleItem = 0;
        return;
    }
    
    float availableSpace = m_Orientation == ToolbarOrientation::Horizontal ?
        GetWidth() : GetHeight();
        
    // Reserve space for overflow button if enabled
    if (m_OverflowEnabled) {
        availableSpace -= m_MinButtonWidth + m_ItemSpacing;
    }
    
    // Calculate visible items
    float usedSpace = 0;
    size_t lastVisible = m_FirstVisibleItem;
    
    for (size_t i = m_FirstVisibleItem; i < m_Items.size(); ++i) {
        float itemSize = m_Orientation == ToolbarOrientation::Horizontal ?
            CalculateItemWidth(m_Items[i].get()) :
            CalculateItemHeight(m_Items[i].get());
            
        if (usedSpace + itemSize + m_ItemSpacing > availableSpace) {
            break;
        }
        
        usedSpace += itemSize + m_ItemSpacing;
        lastVisible = i;
    }
    
    m_LastVisibleItem = lastVisible;
    
    // Create or update overflow menu if needed
    if (m_OverflowEnabled && m_LastVisibleItem < m_Items.size() - 1) {
        if (!m_OverflowMenu) {
            m_OverflowMenu = std::make_unique<UIMenu>();
        }
        UpdateOverflowMenu();
    } else {
        m_OverflowMenu.reset();
    }
}

void UIToolbar::UpdateOverflowMenu() {
    if (!m_OverflowMenu) return;
    
    m_OverflowMenu->Clear();
    
    // Add overflow items to menu
    for (size_t i = m_LastVisibleItem + 1; i < m_Items.size(); ++i) {
        auto* item = m_Items[i].get();
        
        switch (item->GetType()) {
            case ToolbarItemType::Button:
            case ToolbarItemType::ToggleButton: {
                auto* menuItem = m_OverflowMenu->AddItem(item->GetText());
                menuItem->SetIcon(item->GetIcon());
                menuItem->SetEnabled(item->IsEnabled());
                if (item->GetType() == ToolbarItemType::ToggleButton) {
                    menuItem = m_OverflowMenu->AddCheckItem(item->GetText());
                    menuItem->SetChecked(item->IsToggled());
                }
                menuItem->SetAction([item]() { item->Execute(); });
                break;
            }
            
            case ToolbarItemType::Dropdown:
                if (auto* dropdownMenu = item->GetDropdownMenu()) {
                    auto* submenu = m_OverflowMenu->AddSubMenu(item->GetText());
                    // Clone the dropdown menu items
                    // This could be enhanced to share the menu instead of cloning
                    submenu->SetIcon(item->GetIcon());
                    submenu->SetEnabled(item->IsEnabled());
                }
                break;
                
            case ToolbarItemType::Separator:
                m_OverflowMenu->AddSeparator();
                break;
                
            default:
                break;
        }
    }
}

bool UIToolbar::IsItemVisible(size_t index) const {
    return index >= m_FirstVisibleItem && index <= m_LastVisibleItem;
}

void UIToolbar::ShowOverflowMenu() {
    if (!m_OverflowMenu) return;
    
    // Calculate menu position
    glm::vec2 buttonPos = CalculateItemPosition(m_LastVisibleItem + 1);
    glm::vec2 menuPos;
    
    if (m_Orientation == ToolbarOrientation::Horizontal) {
        menuPos = glm::vec2(buttonPos.x, buttonPos.y + CalculateItemHeight(nullptr));
    } else {
        menuPos = glm::vec2(buttonPos.x + CalculateItemWidth(nullptr), buttonPos.y);
    }
    
    m_OverflowMenu->Show(menuPos);
}

void UIToolbar::ScrollToItem(size_t index) {
    if (index >= m_Items.size()) return;
    
    // If item is already visible, do nothing
    if (IsItemVisible(index)) return;
    
    // If item is before first visible, scroll backward
    if (index < m_FirstVisibleItem) {
        m_FirstVisibleItem = index;
        UpdateLayout();
    }
    // If item is after last visible, scroll forward
    else if (index > m_LastVisibleItem) {
        // Calculate how many items we need to skip to make the target item visible
        size_t itemsToSkip = index - m_LastVisibleItem;
        m_FirstVisibleItem += itemsToSkip;
        UpdateLayout();
    }
}

} // namespace Editor
} // namespace VGE



