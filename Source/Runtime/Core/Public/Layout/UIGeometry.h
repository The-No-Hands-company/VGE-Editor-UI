#pragma once

#include "Runtime/Core/Public/Core/UICore.h"

#include <glm/glm.hpp>

namespace VGE {
namespace UI {

struct UIGeometry {
    glm::vec2 Position;     // Position relative to parent
    glm::vec2 Size;         // Size of the geometry
    glm::vec2 LocalPosition;// Position in local space
    float Scale;            // Uniform scale factor

    UIGeometry()
        : Position(0.0f)
        , Size(0.0f)
        , LocalPosition(0.0f)
        , Scale(1.0f)
    {}

    // Check if a point is within this geometry
    bool ContainsPoint(const glm::vec2& point) const {
        glm::vec2 localPoint = (point - Position) / Scale;
        return localPoint.x >= 0.0f && localPoint.x <= Size.x &&
               localPoint.y >= 0.0f && localPoint.y <= Size.y;
    }

    // Get absolute bounds
    glm::vec4 GetAbsoluteBounds() const {
        return glm::vec4(Position.x, Position.y, Position.x + Size.x * Scale, Position.y + Size.y * Scale);
    }

    // Get local bounds
    glm::vec4 GetLocalBounds() const {
        return glm::vec4(LocalPosition.x, LocalPosition.y, LocalPosition.x + Size.x, LocalPosition.y + Size.y);
    }

    // Create a child geometry with offset
    UIGeometry CreateChildGeometry(const glm::vec2& offset, const glm::vec2& childSize) const {
        UIGeometry child;
        child.Position = Position + offset * Scale;
        child.Size = childSize;
        child.LocalPosition = LocalPosition + offset;
        child.Scale = Scale;
        return child;
    }

    // Shrink geometry by padding
    UIGeometry ShrinkByPadding(float left, float top, float right, float bottom) const {
        UIGeometry padded = *this;
        padded.Position += glm::vec2(left, top) * Scale;
        padded.Size -= glm::vec2(left + right, top + bottom);
        padded.LocalPosition += glm::vec2(left, top);
        return padded;
    }

    // Expand geometry by margin
    UIGeometry ExpandByMargin(float left, float top, float right, float bottom) const {
        UIGeometry expanded = *this;
        expanded.Position -= glm::vec2(left, top) * Scale;
        expanded.Size += glm::vec2(left + right, top + bottom);
        expanded.LocalPosition -= glm::vec2(left, top);
        return expanded;
    }
};

}} // namespace VGE::UI 


