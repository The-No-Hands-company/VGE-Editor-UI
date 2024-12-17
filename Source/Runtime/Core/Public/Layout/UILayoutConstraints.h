#pragma once

#include "Runtime/Core/Public/Core/UICore.h"

#include <glm/glm.hpp>

namespace VGE {
namespace UI {

enum class LayoutAlignment {
    Start,      // Align to start of container
    Center,     // Center in container
    End,        // Align to end of container
    Stretch     // Stretch to fill container
};

enum class DockPosition {
    None,
    Left,
    Top,
    Right,
    Bottom,
    Fill
};

struct UILayoutConstraints {
    // Size constraints
    glm::vec2 minSize = glm::vec2(0.0f);
    glm::vec2 maxSize = glm::vec2(FLT_MAX);
    glm::vec2 preferredSize = glm::vec2(0.0f);
    
    // Flex properties
    float flexGrow = 0.0f;      // How much the element can grow
    float flexShrink = 1.0f;    // How much the element can shrink
    float flexBasis = 0.0f;     // Base size before growing/shrinking
    
    // Alignment
    LayoutAlignment horizontalAlignment = LayoutAlignment::Start;
    LayoutAlignment verticalAlignment = LayoutAlignment::Start;
    
    // Margins and padding
    glm::vec4 margin = glm::vec4(0.0f);  // Left, Top, Right, Bottom
    glm::vec4 padding = glm::vec4(0.0f); // Left, Top, Right, Bottom
    
    // Docking
    DockPosition dockPosition = DockPosition::None;
    
    // Aspect ratio
    float aspectRatio = 0.0f;   // 0 means no aspect ratio constraint
    bool maintainAspectRatio = false;
    
    // Visibility and interaction
    bool isVisible = true;
    bool isInteractive = true;
    
    // Z-index for layering
    int zIndex = 0;
    
    // Anchoring (0 to 1 for each axis)
    glm::vec2 anchorMin = glm::vec2(0.0f);
    glm::vec2 anchorMax = glm::vec2(1.0f);
    
    // Pivot point (0 to 1 for each axis)
    glm::vec2 pivot = glm::vec2(0.5f);
    
    // Convenience methods
    static UILayoutConstraints CreateHorizontalStretch() {
        UILayoutConstraints constraints;
        constraints.horizontalAlignment = LayoutAlignment::Stretch;
        return constraints;
    }
    
    static UILayoutConstraints CreateVerticalStretch() {
        UILayoutConstraints constraints;
        constraints.verticalAlignment = LayoutAlignment::Stretch;
        return constraints;
    }
    
    static UILayoutConstraints CreateFullStretch() {
        UILayoutConstraints constraints;
        constraints.horizontalAlignment = LayoutAlignment::Stretch;
        constraints.verticalAlignment = LayoutAlignment::Stretch;
        return constraints;
    }
    
    static UILayoutConstraints CreateDocked(DockPosition position) {
        UILayoutConstraints constraints;
        constraints.dockPosition = position;
        return constraints;
    }
    
    static UILayoutConstraints CreateCentered() {
        UILayoutConstraints constraints;
        constraints.horizontalAlignment = LayoutAlignment::Center;
        constraints.verticalAlignment = LayoutAlignment::Center;
        return constraints;
    }
};

}} // namespace VGE::UI 


