#pragma once

#include "Runtime/Core/Public/Core/UICore.h"
#include <vector>
#include "Runtime/Core/Public/Base/VGE.h"

namespace VGE {

enum class EaseType {
    Linear,
    EaseInQuad,
    EaseOutQuad,
    EaseInOutQuad,
    EaseOutBack,
    EaseOutElastic,
    EaseOutBounce
};

class UIComboBoxAnimator {
public:
    struct AnimationState {
        float CurrentValue = 0.0f;
        float TargetValue = 0.0f;
        EaseType EaseType = EaseType::Linear;
        bool IsAnimating() const { return std::abs(CurrentValue - TargetValue) > 0.001f; }
    };

    UIComboBoxAnimator();
    
    void Update(float deltaTime);
    
    // Animation controls
    void SetDropdownOpen(bool open);
    void SetHovered(bool hovered);
    void SetItemHovered(size_t index, bool hovered);
    void AddItem();
    void RemoveItem(size_t index);
    void ClearItems();
    
    // Animation properties
    void SetAnimationSpeed(float speed) { m_AnimationSpeed = speed; }
    float GetAnimationSpeed() const { return m_AnimationSpeed; }
    
    // Animation state
    float GetDropdownAnimation() const { return m_DropdownAnimation.CurrentValue; }
    float GetHoverAnimation() const { return m_HoverAnimation.CurrentValue; }
    float GetItemAnimation(size_t index) const;

private:
    void UpdateAnimation(AnimationState& state, float deltaTime);
    float CalculateEase(float t, EaseType type);
    
    AnimationState m_DropdownAnimation;
    AnimationState m_HoverAnimation;
    std::vector<AnimationState> m_ItemAnimations;
    
    float m_AnimationSpeed = 10.0f;
};

} // namespace VGE



