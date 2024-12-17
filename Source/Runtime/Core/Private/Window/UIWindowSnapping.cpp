#include "Runtime/Core/Public/Window/UIWindowSnapping.h"
#include "Runtime/Core/Public/Window/UIWindow.h"
#include "Runtime/Core/Public/Core/Logger.h"
#include "Runtime/Core/Public/Renderer/UIRenderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

namespace VGE {
namespace UI {

UIWindowSnapping::UIWindowSnapping(UIWindow* window)
    : m_Window(window)
    , m_IsSnapping(false)
    , m_LastSnapPosition(0.0f)
    , m_SnapTransition(0.0f)
{
    // Initialize screen size
    if (m_Window && m_Window->GetPlatformWindow()) {
        m_ScreenSize = m_Window->GetPlatformWindow()->GetScreenSize();
    }
}

void UIWindowSnapping::Update(float deltaTime) {
    if (!m_Window || !m_Settings.enableSnapping) return;

    // Update screen size if needed
    if (m_Window->GetPlatformWindow()) {
        m_ScreenSize = m_Window->GetPlatformWindow()->GetScreenSize();
    }

    // Update guides
    UpdateGuides();

    // Update snap transition for smooth movement
    if (m_IsSnapping) {
        m_SnapTransition = std::min(m_SnapTransition + deltaTime * 10.0f, 1.0f);
        
        // Apply smooth transition to window position
        glm::vec2 currentPos = m_Window->GetPosition();
        glm::vec2 targetPos = glm::mix(currentPos, m_LastSnapPosition, m_SnapTransition);
        m_Window->SetPosition(targetPos);

        // Check if snapping is complete
        if (m_SnapTransition >= 1.0f) {
            m_IsSnapping = false;
            m_SnapTransition = 0.0f;
        }
    }
}

glm::vec2 UIWindowSnapping::CalculateSnapPosition(const glm::vec2& targetPos) {
    if (!m_Settings.enableSnapping) return targetPos;

    // Collect all potential snap points
    ClearGuides();
    if (m_Settings.snapToEdges) CollectScreenEdges();
    if (m_Settings.snapToOtherWindows) CollectWindowEdges();
    if (m_Settings.snapToGrid) CollectGridLines();

    // Apply snapping
    glm::vec2 snappedPos = ApplySnapping(targetPos);

    // If position changed, start snapping animation
    if (snappedPos != targetPos) {
        m_IsSnapping = true;
        m_LastSnapPosition = snappedPos;
        m_SnapTransition = 0.0f;
    }

    return snappedPos;
}

void UIWindowSnapping::RenderGuides() {
    if (!m_Settings.showGuides || m_ActiveGuides.empty()) return;

    auto renderer = UIRenderer::Get();
    if (!renderer) return;

    for (const auto& guide : m_ActiveGuides) {
        // Calculate guide line endpoints
        glm::vec2 start = guide.position;
        glm::vec2 end = guide.position + guide.direction * 1000.0f; // Long enough to cross screen

        // Draw guide line
        glm::vec4 color = m_Settings.guideColor;
        color.a *= guide.strength; // Fade based on snap strength
        renderer->DrawLine(start, end, color, 2.0f);

        // Draw snap point indicator
        if (guide.isEdge) {
            renderer->DrawRect(
                guide.position - glm::vec2(2.0f),
                glm::vec2(4.0f),
                m_Settings.guideColor
            );
        }
    }
}

void UIWindowSnapping::UpdateGuides() {
    ClearGuides();
    
    if (m_Settings.snapToEdges) CollectScreenEdges();
    if (m_Settings.snapToOtherWindows) CollectWindowEdges();
    if (m_Settings.snapToGrid) CollectGridLines();
}

void UIWindowSnapping::CollectScreenEdges() {
    // Screen edges
    m_ActiveGuides.push_back({
        glm::vec2(0.0f, 0.0f),           // Left edge
        glm::vec2(0.0f, 1.0f),
        1.0f,
        true
    });
    
    m_ActiveGuides.push_back({
        glm::vec2(m_ScreenSize.x, 0.0f),  // Right edge
        glm::vec2(0.0f, 1.0f),
        1.0f,
        true
    });
    
    m_ActiveGuides.push_back({
        glm::vec2(0.0f, 0.0f),           // Top edge
        glm::vec2(1.0f, 0.0f),
        1.0f,
        true
    });
    
    m_ActiveGuides.push_back({
        glm::vec2(0.0f, m_ScreenSize.y),  // Bottom edge
        glm::vec2(1.0f, 0.0f),
        1.0f,
        true
    });
}

void UIWindowSnapping::CollectWindowEdges() {
    // This would need access to window manager to get other windows
    // For now, we'll just use screen edges
    // TODO: Implement window edge collection when window manager is available
}

void UIWindowSnapping::CollectGridLines() {
    if (!m_Settings.snapToGrid) return;

    // Create vertical grid lines
    for (float x = 0.0f; x < m_ScreenSize.x; x += m_Settings.gridSize) {
        m_ActiveGuides.push_back({
            glm::vec2(x, 0.0f),
            glm::vec2(0.0f, 1.0f),
            0.5f,  // Grid lines have lower snap strength
            false
        });
    }

    // Create horizontal grid lines
    for (float y = 0.0f; y < m_ScreenSize.y; y += m_Settings.gridSize) {
        m_ActiveGuides.push_back({
            glm::vec2(0.0f, y),
            glm::vec2(1.0f, 0.0f),
            0.5f,  // Grid lines have lower snap strength
            false
        });
    }
}

glm::vec2 UIWindowSnapping::ApplySnapping(const glm::vec2& position) {
    glm::vec2 snappedPos = position;
    float bestSnapDistanceX = m_Settings.snapDistance;
    float bestSnapDistanceY = m_Settings.snapDistance;

    for (const auto& guide : m_ActiveGuides) {
        // Calculate distance to guide
        glm::vec2 toGuide = guide.position - position;
        
        // Project onto guide direction and perpendicular
        float alongGuide = glm::dot(toGuide, guide.direction);
        float perpGuide = glm::dot(toGuide, glm::vec2(-guide.direction.y, guide.direction.x));

        // Check if we're within snap distance
        if (std::abs(perpGuide) < m_Settings.snapDistance) {
            float snapForce = CalculateSnapForce(perpGuide) * guide.strength;
            
            // Apply snap force
            if (guide.direction.x == 0.0f && std::abs(perpGuide) < bestSnapDistanceX) {
                snappedPos.x = guide.position.x;
                bestSnapDistanceX = std::abs(perpGuide);
            }
            if (guide.direction.y == 0.0f && std::abs(perpGuide) < bestSnapDistanceY) {
                snappedPos.y = guide.position.y;
                bestSnapDistanceY = std::abs(perpGuide);
            }
        }
    }

    return snappedPos;
}

float UIWindowSnapping::CalculateSnapForce(float distance) const {
    // Use smooth step function for natural-feeling snap force
    float t = 1.0f - (std::abs(distance) / m_Settings.snapDistance);
    t = glm::clamp(t, 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t) * m_Settings.snapStrength;
}

bool UIWindowSnapping::IsWithinSnapDistance(const glm::vec2& pos1, const glm::vec2& pos2) const {
    return glm::distance(pos1, pos2) < m_Settings.snapDistance;
}

}} // namespace VGE::UI 