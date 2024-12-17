#pragma once

#include "Runtime/Core/Public/Core/UICore.h"
#include <glm/glm.hpp>

namespace VGE {
namespace UI {

/**
 * @brief Enumeration of possible dock zones
 */
enum class DockZone {
    None,       ///< No valid dock zone
    Left,       ///< Left side of dock space
    Right,      ///< Right side of dock space
    Top,        ///< Top of dock space
    Bottom,     ///< Bottom of dock space
    Center,     ///< Center of dock space
    TopLeft,    ///< Top-left corner
    TopRight,   ///< Top-right corner
    BottomLeft, ///< Bottom-left corner
    BottomRight ///< Bottom-right corner
};

/**
 * @brief Constants for dock zone detection
 */
constexpr float ZONE_EDGE_THRESHOLD = 0.25f;    ///< Threshold for edge zones
constexpr float ZONE_CORNER_THRESHOLD = 0.25f;  ///< Threshold for corner zones

class UIDockSpace;

/**
 * @brief Utility class for detecting dock zones
 */
class UIDockZoneDetector {
public:
    /**
     * @brief Detects the dock zone based on mouse position
     * @param mousePos Current mouse position
     * @param dockSpace Reference to the dock space
     * @return The detected dock zone
     */
    static DockZone DetectZone(const glm::vec2& mousePos, const UIDockSpace& dockSpace);
};

}} // namespace VGE::UI 