#pragma once

#include "Runtime/Core/Public/Core/UICore.h"
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

class UIComboBoxAnimation {
public:
    static float Ease(float t, EaseType type = EaseType::Linear);
    static float EaseInQuad(float t) { return t * t; }
    static float EaseOutQuad(float t) { return t * (2.0f - t); }
    static float EaseInOutQuad(float t) { return t < 0.5f ? 2.0f * t * t : -1.0f + (4.0f - 2.0f * t) * t; }
    static float EaseOutBack(float t);
    static float EaseOutElastic(float t);
    static float EaseOutBounce(float t);

    void Update(float deltaTime, float targetValue, float speed = 10.0f, EaseType easeType = EaseType::Linear);
    float GetValue() const { return m_CurrentValue; }
    void SetValue(float value) { m_CurrentValue = value; }
    bool IsAnimating() const { return std::abs(m_CurrentValue - m_TargetValue) > 0.001f; }

private:
    float m_CurrentValue = 0.0f;
    float m_TargetValue = 0.0f;
};

} // namespace VGE



