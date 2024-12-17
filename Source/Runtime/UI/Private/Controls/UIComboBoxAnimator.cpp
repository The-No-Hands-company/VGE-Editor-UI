#include "U

#include "Runtime/Core/Public/Core/UICore.h"IComboBoxAnimator.h"
#include <algorithm>
#include <cmath>

namespace VGE {

UIComboBoxAnimator::UIComboBoxAnimator() {
    m_DropdownAnimation.EaseType = EaseType::EaseOutBack;
    m_HoverAnimation.EaseType = EaseType::EaseOutQuad;
}

void UIComboBoxAnimator::Update(float deltaTime) {
    UpdateAnimation(m_DropdownAnimation, deltaTime);
    UpdateAnimation(m_HoverAnimation, deltaTime);
    
    for (auto& animation : m_ItemAnimations) {
        UpdateAnimation(animation, deltaTime);
    }
}

void UIComboBoxAnimator::SetDropdownOpen(bool open) {
    m_DropdownAnimation.TargetValue = open ? 1.0f : 0.0f;
}

void UIComboBoxAnimator::SetHovered(bool hovered) {
    m_HoverAnimation.TargetValue = hovered ? 1.0f : 0.0f;
}

void UIComboBoxAnimator::SetItemHovered(size_t index, bool hovered) {
    if (index < m_ItemAnimations.size()) {
        m_ItemAnimations[index].TargetValue = hovered ? 1.0f : 0.0f;
    }
}

void UIComboBoxAnimator::AddItem() {
    AnimationState state;
    state.EaseType = EaseType::EaseOutQuad;
    m_ItemAnimations.push_back(state);
}

void UIComboBoxAnimator::RemoveItem(size_t index) {
    if (index < m_ItemAnimations.size()) {
        m_ItemAnimations.erase(m_ItemAnimations.begin() + index);
    }
}

void UIComboBoxAnimator::ClearItems() {
    m_ItemAnimations.clear();
}

float UIComboBoxAnimator::GetItemAnimation(size_t index) const {
    if (index < m_ItemAnimations.size()) {
        return m_ItemAnimations[index].CurrentValue;
    }
    return 0.0f;
}

void UIComboBoxAnimator::UpdateAnimation(AnimationState& state, float deltaTime) {
    if (!state.IsAnimating())
        return;
        
    float diff = state.TargetValue - state.CurrentValue;
    float step = diff * std::min(1.0f, deltaTime * m_AnimationSpeed);
    
    state.CurrentValue += step;
    
    // Apply easing
    if (state.CurrentValue != state.TargetValue) {
        float t = std::abs(state.CurrentValue - state.TargetValue) / std::abs(diff);
        t = CalculateEase(1.0f - t, state.EaseType);
        state.CurrentValue = state.TargetValue - (diff * (1.0f - t));
    }
    
    // Clamp to target if close enough
    if (std::abs(state.CurrentValue - state.TargetValue) < 0.001f) {
        state.CurrentValue = state.TargetValue;
    }
}

float UIComboBoxAnimator::CalculateEase(float t, EaseType type) {
    t = std::clamp(t, 0.0f, 1.0f);
    
    switch (type) {
        case EaseType::Linear:
            return t;
            
        case EaseType::EaseInQuad:
            return t * t;
            
        case EaseType::EaseOutQuad:
            return t * (2.0f - t);
            
        case EaseType::EaseInOutQuad:
            return t < 0.5f ? 2.0f * t * t : -1.0f + (4.0f - 2.0f * t) * t;
            
        case EaseType::EaseOutBack: {
            const float c1 = 1.70158f;
            const float c3 = c1 + 1.0f;
            return 1.0f + c3 * std::pow(t - 1.0f, 3.0f) + c1 * std::pow(t - 1.0f, 2.0f);
        }
            
        case EaseType::EaseOutElastic: {
            const float c4 = (2.0f * 3.14159f) / 3.0f;
            return t == 0.0f ? 0.0f
                 : t == 1.0f ? 1.0f
                 : std::pow(2.0f, -10.0f * t) * std::sin((t * 10.0f - 0.75f) * c4) + 1.0f;
        }
            
        case EaseType::EaseOutBounce: {
            const float n1 = 7.5625f;
            const float d1 = 2.75f;
            
            if (t < 1.0f / d1) {
                return n1 * t * t;
            } else if (t < 2.0f / d1) {
                t -= 1.5f / d1;
                return n1 * t * t + 0.75f;
            } else if (t < 2.5f / d1) {
                t -= 2.25f / d1;
                return n1 * t * t + 0.9375f;
            } else {
                t -= 2.625f / d1;
                return n1 * t * t + 0.984375f;
            }
        }
            
        default:
            return t;
    }
}

} // namespace VGE



