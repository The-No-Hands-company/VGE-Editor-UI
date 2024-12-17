#include "Runtime/UI/Public/Controls/UITabManager.h"
#include "Runtime/Core/Public/Core/Logger.h"

namespace VGE {
namespace UI {

bool UITabManager::AddTab(const std::string& id, const std::string& label,
                         std::shared_ptr<UIWidget> content, bool canClose) {
    // Check for duplicate ID
    auto it = std::find_if(m_Tabs.begin(), m_Tabs.end(),
        [&id](const UITabInfo& tab) { return tab.id == id; });
    
    if (it != m_Tabs.end()) {
        Logger::Log(LogLevel::Warning, "Tab with ID '{}' already exists", id);
        return false;
    }

    // Create new tab
    UITabInfo tab;
    tab.id = id;
    tab.label = label;
    tab.content = content;
    tab.canClose = canClose;
    tab.isActive = m_Tabs.empty(); // First tab is active by default

    // Add tab
    m_Tabs.push_back(tab);

    // Set as active if first tab
    if (tab.isActive) {
        m_ActiveTabId = id;
        if (m_OnTabActivated) {
            m_OnTabActivated(tab);
        }
    }

    return true;
}

bool UITabManager::RemoveTab(const std::string& id) {
    auto it = std::find_if(m_Tabs.begin(), m_Tabs.end(),
        [&id](const UITabInfo& tab) { return tab.id == id; });
    
    if (it == m_Tabs.end()) {
        return false;
    }

    // Call callback before removing
    if (m_OnTabClosed) {
        m_OnTabClosed(*it);
    }

    // If removing active tab, activate the next one
    if (it->id == m_ActiveTabId) {
        if (m_Tabs.size() > 1) {
            // Try to activate the next tab, or the previous if at the end
            auto nextIt = std::next(it);
            if (nextIt == m_Tabs.end()) {
                nextIt = std::prev(it);
            }
            m_ActiveTabId = nextIt->id;
            nextIt->isActive = true;
            if (m_OnTabActivated) {
                m_OnTabActivated(*nextIt);
            }
        } else {
            m_ActiveTabId.clear();
        }
    }

    // Remove the tab
    m_Tabs.erase(it);
    return true;
}

const UITabInfo* UITabManager::GetActiveTab() const {
    if (m_ActiveTabId.empty()) {
        return nullptr;
    }

    auto it = std::find_if(m_Tabs.begin(), m_Tabs.end(),
        [this](const UITabInfo& tab) { return tab.id == m_ActiveTabId; });
    
    return it != m_Tabs.end() ? &(*it) : nullptr;
}

}} // namespace VGE::UI 