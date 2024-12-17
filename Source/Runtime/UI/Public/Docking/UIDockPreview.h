#pragma once

#include "Runtime/Core/Public/Core/UICore.h"
#include "Runtime/UI/Public/Docking/UIDockZone.h"
#include <glm/glm.hpp>

namespace VGE {
namespace UI {

class UIDockSpace;

/**
 * @brief Preview visualization for dock operations
 */
class UIDockPreview {
public:
    UIDockPreview() = default;

    /**
     * @brief Updates the preview based on mouse position
     * @param mousePos Current mouse position
     * @param dockSpace Reference to the dock space
     */
    void Update(const glm::vec2& mousePos, const UIDockSpace& dockSpace);

    /**
     * @brief Renders the preview visualization
     */
    void Render();

    /**
     * @brief Sets the active state of the preview
     * @param active Whether the preview should be active
     */
    void SetActive(bool active) { m_IsActive = active; }

    /**
     * @brief Gets the current dock zone
     * @return The current dock zone
     */
    DockZone GetZone() const { return m_CurrentZone; }

private:
    /**
     * @brief Calculates preview geometry based on dock zone
     * @param zone The dock zone
     * @param dockSpace Reference to the dock space
     * @return Pair of position and size for the preview
     */
    std::pair<glm::vec2, glm::vec2> CalculatePreviewGeometry(
        DockZone zone, const UIDockSpace& dockSpace) const;

private:
    bool m_IsActive = false;
    DockZone m_CurrentZone = DockZone::None;
    glm::vec2 m_Position;
    glm::vec2 m_Size;
    float m_Opacity = 0.3f;
};
}
} 