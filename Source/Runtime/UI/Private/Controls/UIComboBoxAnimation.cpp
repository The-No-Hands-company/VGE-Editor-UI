#include "U

#include "Runtime/Core/Public/Core/UICore.h"IComboBoxAnimation.h"
#include <algorithm>
#include <cmath>

namespace VGE {

float UIComboBoxAnimation::Ease(float t, EaseType type) {
    t = std::clamp(t, 0.0f, 1.0f);
    
    switch (type) {
        case EaseType::Linear:
            return t;
        case EaseType::EaseInQuad:
            return EaseInQuad(t);
        case EaseType::EaseOutQuad:
            return EaseOutQuad(t);
        case EaseType::EaseInOutQuad:
            return EaseInOutQuad(t);
        case EaseType::EaseOutBack:
            return EaseOutBack(t);
        case EaseType::EaseOutElastic:
            return EaseOutElastic(t);
        case EaseType::EaseOutBounce:
            return EaseOutBounce(t);
        default:
            return t;
    }
}

float UIComboBoxAnimation::EaseOutBack(float t) {
    const float c1 = 1.70158f;
    const float c3 = c1 + 1.0f;
    return 1.0f + c3 * std::pow(t - 1.0f, 3.0f) + c1 * std::pow(t - 1.0f, 2.0f);
}

float UIComboBoxAnimation::EaseOutElastic(float t) {
    const float c4 = (2.0f * 3.14159f) / 3.0f;
    return t == 0.0f ? 0.0f
         : t == 1.0f ? 1.0f
         : std::pow(2.0f, -10.0f * t) * std::sin((t * 10.0f - 0.75f) * c4) + 1.0f;
}

float UIComboBoxAnimation::EaseOutBounce(float t) {
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

void UIComboBoxAnimation::Update(float deltaTime, float targetValue, float speed, EaseType easeType) {
    m_TargetValue = targetValue;
    
    if (m_CurrentValue != m_TargetValue) {
        float diff = m_TargetValue - m_CurrentValue;
        float step = diff * std::min(1.0f, deltaTime * speed);
        
        m_CurrentValue += step;
        
        if (std::abs(m_CurrentValue - m_TargetValue) < 0.001f) {
            m_CurrentValue = m_TargetValue;
        }
    }
}

} // namespace VGE



