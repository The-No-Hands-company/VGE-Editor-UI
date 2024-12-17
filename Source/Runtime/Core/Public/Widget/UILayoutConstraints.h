#pragma once

#include "Runtime/Core/Public/Core/UICore.h"
#include <glm/glm.hpp>
#include <functional>

namespace VGE {
namespace UI {

// Forward declarations
class UIWidget;

// Size constraint types
enum class UISizeConstraint {
    None,       // No constraint
    Fixed,      // Fixed size
    Range,      // Min/max range
    Aspect,     // Aspect ratio
    Custom      // Custom constraint function
};

// Size constraint function type
using UISizeConstraintFunc = std::function<glm::vec2(const glm::vec2& desiredSize, const glm::vec2& parentSize)>;

// Layout constraint class
class UI_API UILayoutConstraints {
public:
    UILayoutConstraints();
    virtual ~UILayoutConstraints();

    // Size constraints
    void SetFixedSize(const glm::vec2& size);
    void SetMinSize(const glm::vec2& size);
    void SetMaxSize(const glm::vec2& size);
    void SetSizeRange(const glm::vec2& min, const glm::vec2& max);
    void SetAspectRatio(float ratio);
    void SetCustomConstraint(UISizeConstraintFunc func);

    // Position constraints
    void SetFixedPosition(const glm::vec2& position);
    void SetRelativePosition(const glm::vec2& position); // 0-1 range
    void SetAlignmentPosition(const glm::vec2& position); // -1 to 1 range
    void SetCustomPosition(std::function<glm::vec2(const UIWidget* widget)> func);

    // Constraint application
    glm::vec2 ApplySizeConstraints(const glm::vec2& desiredSize, const glm::vec2& parentSize) const;
    glm::vec2 ApplyPositionConstraints(const UIWidget* widget) const;

    // Constraint types
    UISizeConstraint GetSizeConstraintType() const { return m_SizeConstraintType; }
    bool HasPositionConstraint() const { return m_HasPositionConstraint; }

    // Getters
    const glm::vec2& GetFixedSize() const { return m_FixedSize; }
    const glm::vec2& GetMinSize() const { return m_MinSize; }
    const glm::vec2& GetMaxSize() const { return m_MaxSize; }
    float GetAspectRatio() const { return m_AspectRatio; }
    const glm::vec2& GetFixedPosition() const { return m_FixedPosition; }
    const glm::vec2& GetRelativePosition() const { return m_RelativePosition; }
    const glm::vec2& GetAlignmentPosition() const { return m_AlignmentPosition; }

private:
    // Size constraints
    UISizeConstraint m_SizeConstraintType;
    glm::vec2 m_FixedSize;
    glm::vec2 m_MinSize;
    glm::vec2 m_MaxSize;
    float m_AspectRatio;
    UISizeConstraintFunc m_CustomSizeConstraint;

    // Position constraints
    bool m_HasPositionConstraint;
    glm::vec2 m_FixedPosition;
    glm::vec2 m_RelativePosition;
    glm::vec2 m_AlignmentPosition;
    std::function<glm::vec2(const UIWidget* widget)> m_CustomPositionConstraint;

    // Helper functions
    glm::vec2 ApplyAspectRatio(const glm::vec2& size) const;
    glm::vec2 ClampSize(const glm::vec2& size) const;
};

}} // namespace VGE::UI 


