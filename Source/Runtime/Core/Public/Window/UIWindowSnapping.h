#pragma once

#include "Core/UICore.h"
#include <glm/glm.hpp>
#include <vector>
#include <memory>

namespace VGE {
namespace UI {

class UIWindow;

struct SnapGuide {
    glm::vec2 position;
    glm::vec2 direction;  // Normalized direction vector
    float strength;       // Snap strength (0-1)
    bool isEdge;         // True if this is a screen edge
};

struct SnapSettings {
    bool enableSnapping = true;
    float snapDistance = 10.0f;      // Distance at which snapping activates
    float snapStrength = 0.5f;       // How strong the snapping force is (0-1)
    bool snapToEdges = true;         // Snap to screen edges
    bool snapToOtherWindows = true;  // Snap to other window edges
    bool snapToGrid = false;         // Snap to a virtual grid
    float gridSize = 20.0f;          // Size of the virtual grid
    bool showGuides = true;          // Show visual guides when snapping
    glm::vec4 guideColor = glm::vec4(0.2f, 0.6f, 1.0f, 0.5f);  // Color of snap guides
};

class UI_API UIWindowSnapping {
public:
    UIWindowSnapping(UIWindow* window);
    ~UIWindowSnapping() = default;

    // Update snapping behavior
    void Update(float deltaTime);
    
    // Calculate snap position
    glm::vec2 CalculateSnapPosition(const glm::vec2& targetPos);
    
    // Settings
    void SetSettings(const SnapSettings& settings) { m_Settings = settings; }
    const SnapSettings& GetSettings() const { return m_Settings; }
    void EnableSnapping(bool enable) { m_Settings.enableSnapping = enable; }
    
    // Guides
    void RenderGuides();
    void ClearGuides() { m_ActiveGuides.clear(); }
    
    // Grid
    void SetGridSize(float size) { m_Settings.gridSize = size; }
    float GetGridSize() const { return m_Settings.gridSize; }
    
    // Magnetic strength
    void SetSnapStrength(float strength) { m_Settings.snapStrength = glm::clamp(strength, 0.0f, 1.0f); }
    float GetSnapStrength() const { return m_Settings.snapStrength; }

private:
    // Helper functions
    void UpdateGuides();
    void CollectScreenEdges();
    void CollectWindowEdges();
    void CollectGridLines();
    glm::vec2 ApplySnapping(const glm::vec2& position);
    float CalculateSnapForce(float distance) const;
    bool IsWithinSnapDistance(const glm::vec2& pos1, const glm::vec2& pos2) const;

private:
    UIWindow* m_Window;
    SnapSettings m_Settings;
    std::vector<SnapGuide> m_ActiveGuides;
    
    // Cached data
    glm::vec2 m_ScreenSize;
    std::vector<UIWindow*> m_NearbyWindows;
    bool m_IsSnapping;
    glm::vec2 m_LastSnapPosition;
    float m_SnapTransition;  // For smooth snapping animation
};

}} // namespace VGE::UI 