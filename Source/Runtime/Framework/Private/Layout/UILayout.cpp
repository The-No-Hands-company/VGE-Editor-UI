#include "Core/AIEventSystem.h"
#include <algorithm>
#include <fstream>
#include <chrono>

namespace VGE::AI {

AIEventSystem::AIEventSystem()
    : m_MaxQueueSize(1000)
    , m_ProcessingInterval(0.016f) // 60fps
    , m_EventLoggingEnabled(false)
    , m_LastProcessTime(0.0f)
{
}

AIEventSystem::~AIEventSystem() = default;

void AIEventSystem::PushEvent(const AIEvent& event) {
    if (!ValidateEvent(event)) {
        return;
    }
    
    // Add to queue if not full
    if (m_EventQueue.size() < m_MaxQueueSize) {
        m_EventQueue.push(event);
        UpdateEventStatistics(event);
    }
}

void AIEventSystem::ProcessEvents() {
    auto currentTime = static_cast<float>(
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count()
    ) / 1000.0f;
    
    if (currentTime - m_LastProcessTime < m_ProcessingInterval) {
        return;
    }
    
    while (!m_EventQueue.empty()) {
        const auto& event = m_EventQueue.top();
        ProcessEvent(event);
        
        // Add to history if logging is enabled
        if (m_EventLoggingEnabled) {
            m_EventHistory.push_back(event);
        }
        
        m_EventQueue.pop();
    }
    
    m_LastProcessTime = currentTime;
    PruneEventHistory();
}

void AIEventSystem::RegisterHandler(AIEventType type, EventHandler handler) {
    // Add handler with default priority
    m_Handlers[type].push_back({1.0f, handler});
    
    // Sort handlers by priority
    std::sort(m_Handlers[type].begin(), m_Handlers[type].end(),
        [](const auto& a, const auto& b) {
            return a.first > b.first;
        });
}

void AIEventSystem::SetHandlerPriority(AIEventType type, float priority) {
    if (m_Handlers.count(type) > 0) {
        for (auto& handler : m_Handlers[type]) {
            handler.first = priority;
        }
        
        // Re-sort handlers
        std::sort(m_Handlers[type].begin(), m_Handlers[type].end(),
            [](const auto& a, const auto& b) {
                return a.first > b.first;
            });
    }
}

std::vector<AIEvent> AIEventSystem::GetEvents(AIEventType type) const {
    std::vector<AIEvent> events;
    for (const auto& event : m_EventHistory) {
        if (event.type == type) {
            events.push_back(event);
        }
    }
    return events;
}

float AIEventSystem::CalculateEventFrequency(AIEventType type) const {
    if (m_EventHistory.empty()) {
        return 0.0f;
    }
    
    size_t count = m_EventCounts.count(type) ? m_EventCounts.at(type) : 0;
    float timeSpan = m_EventHistory.back().timestamp - m_EventHistory.front().timestamp;
    
    return timeSpan > 0.0f ? static_cast<float>(count) / timeSpan : 0.0f;
}

void AIEventSystem::GenerateEventReport(const std::string& filepath) const {
    std::ofstream report(filepath);
    if (!report.is_open()) {
        return;
    }
    
    // Write header
    report << "AI Event System Report\n";
    report << "=====================\n\n";
    
    // Event type frequencies
    report << "Event Frequencies:\n";
    for (const auto& [type, count] : m_EventCounts) {
        report << "Type " << static_cast<int>(type) << ": " 
               << CalculateEventFrequency(type) << " events/sec\n";
    }
    
    // Handler statistics
    report << "\nHandler Statistics:\n";
    for (const auto& [type, handlers] : m_Handlers) {
        report << "Type " << static_cast<int>(type) << ": " 
               << handlers.size() << " handlers\n";
    }
    
    report.close();
}

void AIEventSystem::ProcessEvent(const AIEvent& event) {
    // Find handlers for this event type
    auto it = m_Handlers.find(event.type);
    if (it != m_Handlers.end()) {
        // Execute handlers in priority order
        for (const auto& [priority, handler] : it->second) {
            handler(event);
        }
    }
}

void AIEventSystem::UpdateEventStatistics(const AIEvent& event) {
    m_EventCounts[event.type]++;
}

void AIEventSystem::PruneEventHistory() {
    const size_t MAX_HISTORY = 10000;
    if (m_EventHistory.size() > MAX_HISTORY) {
        m_EventHistory.erase(
            m_EventHistory.begin(),
            m_EventHistory.begin() + (m_EventHistory.size() - MAX_HISTORY)
        );
    }
}

bool AIEventSystem::ValidateEvent(const AIEvent& event) const {
    // Basic validation
    if (event.id.empty()) return false;
    if (event.timestamp < 0.0f) return false;
    if (event.priority < 0.0f || event.priority > 1.0f) return false;
    
    return true;
}

} // namespace VGE::AI
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      