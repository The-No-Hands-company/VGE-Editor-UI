#pragma once

#include "Runtime/Core/Public/Core/UICore.h"
#include "Runtime/Core/Public/Base/VGE.h"
#include <string>

namespace VGE {

struct ComboBoxGradient {
    glm::vec4 StartColor;
    glm::vec4 EndColor;
    float Angle = 0.0f; // Angle in degrees
};

struct ComboBoxShadow {
    glm::vec4 Color = glm::vec4(0.0f, 0.0f, 0.0f, 0.5f);
    glm::vec2 Offset = glm::vec2(0.0f, 2.0f);
    float Blur = 4.0f;
    float Spread = 0.0f;
};

struct ComboBoxBorder {
    glm::vec4 Color = glm::vec4(0.3f, 0.3f, 0.3f, 1.0f);
    float Width = 1.0f;
    float Radius = 4.0f;
};

struct ComboBoxVisualStyle {
    // Colors
    glm::vec4 TextColor = glm::vec4(1.0f);
    glm::vec4 DisabledTextColor = glm::vec4(0.5f);
    glm::vec4 PlaceholderColor = glm::vec4(0.7f);
    
    // Backgrounds
    ComboBoxGradient NormalGradient = {
        glm::vec4(0.2f, 0.2f, 0.2f, 1.0f),
        glm::vec4(0.25f, 0.25f, 0.25f, 1.0f),
        90.0f
    };
    ComboBoxGradient HoverGradient = {
        glm::vec4(0.25f, 0.25f, 0.25f, 1.0f),
        glm::vec4(0.3f, 0.3f, 0.3f, 1.0f),
        90.0f
    };
    ComboBoxGradient SelectedGradient = {
        glm::vec4(0.3f, 0.5f, 0.7f, 1.0f),
        glm::vec4(0.4f, 0.6f, 0.8f, 1.0f),
        90.0f
    };
    
    // Borders
    ComboBoxBorder Border;
    ComboBoxBorder FocusBorder = {
        glm::vec4(0.4f, 0.6f, 0.8f, 1.0f),
        1.0f,
        4.0f
    };
    
    // Shadows
    ComboBoxShadow DropShadow;
    ComboBoxShadow HoverShadow = {
        glm::vec4(0.0f, 0.0f, 0.0f, 0.6f),
        glm::vec2(0.0f, 3.0f),
        6.0f,
        0.0f
    };
    
    // Animation
    float AnimationSpeed = 10.0f;
    float BounceStrength = 1.2f;
    float ElasticStrength = 1.0f;
};

} // namespace VGE



