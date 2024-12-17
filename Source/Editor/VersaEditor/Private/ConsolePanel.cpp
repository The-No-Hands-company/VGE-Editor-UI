#include "P

#include "Runtime/Core/Public/Core/UICore.h"ublic/UI/Editor/Panels/ConsolePanel.h"
#include "Public/UI/Framework/Core/UISystem.h"
#include <chrono>

namespace VGE {
namespace Editor {

ConsolePanel::ConsolePanel()
    : EditorPanel("Console")
{
    SetSize(glm::vec2(800.0f, 200.0f));
}

void ConsolePanel::Initialize() {
    EditorPanel::Initialize();
    Log("Console initialized", LogLevel::Info);
}

void ConsolePanel::Update(float deltaTime) {
    EditorPanel::Update(deltaTime);
    
    // Handle auto-scroll
    if (m_ScrollToBottom) {
        // TODO: Implement actual scrolling once we have the scrolling system
        m_ScrollToBottom = false;
    }
}

void ConsolePanel::Clear() {
    m_LogMessages.clear();
}

void ConsolePanel::Log(const std::string& message, LogLevel level) {
    // Create new log message
    LogMessage logMsg(message, level);
    
    // Set timestamp
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    logMsg.timestamp = std::chrono::duration_cast<std::chrono::duration<double>>(duration).count();
    
    // Add to queue
    m_LogMessages.push_back(std::move(logMsg));
    
    // Prune if necessary
    PruneOldMessages();
    
    // Trigger scroll to bottom if auto-scroll is enabled
    if (m_AutoScroll) {
        m_ScrollToBottom = true;
    }
}

void ConsolePanel::RenderPanelContent() {
    auto& renderer = UI::UISystem::Get().GetRenderer();
    
    // Render toolbar
    float toolbarHeight = 30.0f;
    glm::vec2 toolbarPos = GetPosition() + glm::vec2(0.0f, GetSize().y - toolbarHeight);
    glm::vec2 toolbarSize = glm::vec2(GetSize().x, toolbarHeight);
    
    renderer.DrawRect(toolbarPos, toolbarSize, glm::vec4(0.25f, 0.25f, 0.25f, 1.0f));
    
    // TODO: Add toolbar buttons for clear, filters, etc.
    
    // Render log messages
    float messageHeight = 20.0f;
    float contentHeight = GetSize().y - toolbarHeight;
    int maxVisibleMessages = static_cast<int>(contentHeight / messageHeight);
    
    glm::vec2 messagePos = GetPosition();
    
    // TODO: Implement proper scrolling
    int startIdx = std::max(0, static_cast<int>(m_LogMessages.size()) - maxVisibleMessages);
    
    for (size_t i = startIdx; i < m_LogMessages.size(); ++i) {
        const auto& msg = m_LogMessages[i];
        
        // Skip filtered messages
        if ((msg.level == LogLevel::Info && !m_ShowInfo) ||
            (msg.level == LogLevel::Warning && !m_ShowWarnings) ||
            (msg.level == LogLevel::Error && !m_ShowErrors) ||
            (msg.level == LogLevel::Debug && !m_ShowDebug)) {
            continue;
        }
        
        // Skip if doesn't match search
        if (!m_SearchFilter.empty() && 
            msg.message.find(m_SearchFilter) == std::string::npos) {
            continue;
        }
        
        // Draw message background
        glm::vec4 bgColor = (i % 2 == 0) ? 
            glm::vec4(0.2f, 0.2f, 0.2f, 0.5f) : 
            glm::vec4(0.22f, 0.22f, 0.22f, 0.5f);
        
        renderer.DrawRect(messagePos, glm::vec2(GetSize().x, messageHeight), bgColor);
        
        // Draw message text
        // TODO: Add proper text rendering
        // For now just draw a colored rectangle to represent the log level
        glm::vec4 levelColor = GetColorForLogLevel(msg.level);
        renderer.DrawRect(messagePos, glm::vec2(4.0f, messageHeight), levelColor);
        
        messagePos.y += messageHeight;
    }
}

void ConsolePanel::PruneOldMessages() {
    while (m_LogMessages.size() > m_MaxLogEntries) {
        m_LogMessages.pop_front();
    }
}

glm::vec4 ConsolePanel::GetColorForLogLevel(LogLevel level) const {
    switch (level) {
        case LogLevel::Info:
            return glm::vec4(0.0f, 0.8f, 0.0f, 1.0f);
        case LogLevel::Warning:
            return glm::vec4(1.0f, 0.8f, 0.0f, 1.0f);
        case LogLevel::Error:
            return glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
        case LogLevel::Debug:
            return glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
        default:
            return glm::vec4(1.0f);
    }
}

} // namespace Editor
} // namespace VGE 


