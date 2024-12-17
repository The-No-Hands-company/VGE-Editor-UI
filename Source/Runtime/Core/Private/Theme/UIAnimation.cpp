#include "Runtime/Core/Public/Theme/UIAnimation.h"
#include <cmath>

namespace VGE {
namespace Core {

// UIEasing Implementation
float UIEasing::Linear(float t) { return t; }

float UIEasing::QuadIn(float t) { return t * t; }
float UIEasing::QuadOut(float t) { return 1 - (1 - t) * (1 - t); }
float UIEasing::QuadInOut(float t) {
    return t < 0.5f ? 2 * t * t : 1 - pow(-2 * t + 2, 2) / 2;
}

float UIEasing::Interpolate(float t, UIEaseType type) {
    switch (type) {
        case UIEaseType::Linear: return Linear(t);
        case UIEaseType::QuadIn: return QuadIn(t);
        case UIEaseType::QuadOut: return QuadOut(t);
        case UIEaseType::QuadInOut: return QuadInOut(t);
        default: return Linear(t);
    }
}

std::function<float(float)> UIEasing::GetEasingFunction(UIEaseType type) {
    switch (type) {
        case UIEaseType::Linear: return Linear;
        case UIEaseType::QuadIn: return QuadIn;
        case UIEaseType::QuadOut: return QuadOut;
        case UIEaseType::QuadInOut: return QuadInOut;
        default: return Linear;
    }
}

// UIAnimationTrack Implementation
UIAnimationTrack::UIAnimationTrack()
    : m_EaseType(UIEaseType::Linear), m_Loop(false), m_Duration(0.0f) {}

UIAnimationTrack::~UIAnimationTrack() {}

void UIAnimationTrack::AddKeyframe(float time, float value) {
    m_Keyframes.push_back({time, value});
    // Sort keyframes by time
    std::sort(m_Keyframes.begin(), m_Keyframes.end(),
              [](const Keyframe& a, const Keyframe& b) { return a.time < b.time; });
}

void UIAnimationTrack::SetEasing(UIEaseType type) {
    m_EaseType = type;
}

void UIAnimationTrack::SetLoop(bool loop) {
    m_Loop = loop;
}

void UIAnimationTrack::SetDuration(float duration) {
    m_Duration = duration;
}

float UIAnimationTrack::Evaluate(float time) const {
    if (m_Keyframes.empty()) return 0.0f;
    if (m_Keyframes.size() == 1) return m_Keyframes[0].value;

    // Handle looping
    if (m_Loop && m_Duration > 0.0f) {
        time = fmod(time, m_Duration);
    }

    // Find surrounding keyframes
    for (size_t i = 0; i < m_Keyframes.size() - 1; ++i) {
        if (time >= m_Keyframes[i].time && time <= m_Keyframes[i + 1].time) {
            float t = (time - m_Keyframes[i].time) /
                     (m_Keyframes[i + 1].time - m_Keyframes[i].time);
            t = UIEasing::Interpolate(t, m_EaseType);
            return m_Keyframes[i].value * (1 - t) + m_Keyframes[i + 1].value * t;
        }
    }

    return m_Keyframes.back().value;
}

// UIAnimator Implementation
UIAnimator::UIAnimator() {}
UIAnimator::~UIAnimator() {}

void UIAnimator::Animate(const std::string& property, float startValue,
                        float endValue, float duration, UIEaseType easeType) {
    UIAnimationState& state = m_AnimationStates[property];
    state.active = true;
    state.startValue = startValue;
    state.endValue = endValue;
    state.currentValue = startValue;
    state.duration = duration;
    state.elapsed = 0.0f;
    state.easeType = easeType;
}

void UIAnimator::AnimateTo(const std::string& property, float endValue,
                          float duration, UIEaseType easeType) {
    auto it = m_AnimationStates.find(property);
    float startValue = (it != m_AnimationStates.end()) ? 
                      it->second.currentValue : endValue;
    Animate(property, startValue, endValue, duration, easeType);
}

void UIAnimator::Update(float deltaTime) {
    for (auto& [property, state] : m_AnimationStates) {
        if (state.active) {
            UpdateAnimation(property, state, deltaTime);
        }
    }
}

void UIAnimator::UpdateAnimation(const std::string& property,
                               UIAnimationState& state, float deltaTime) {
    state.elapsed += deltaTime;
    float t = std::min(state.elapsed / state.duration, 1.0f);
    t = UIEasing::Interpolate(t, state.easeType);
    
    state.currentValue = state.startValue * (1 - t) + state.endValue * t;
    
    if (state.updateCallback) {
        state.updateCallback(state.currentValue);
    }
    
    if (state.elapsed >= state.duration) {
        CompleteAnimation(property, state);
    }
}

void UIAnimator::CompleteAnimation(const std::string& property,
                                 UIAnimationState& state) {
    state.active = false;
    state.currentValue = state.endValue;
    
    if (state.completeCallback) {
        state.completeCallback();
    }
}

void UIAnimator::Stop(const std::string& property) {
    auto it = m_AnimationStates.find(property);
    if (it != m_AnimationStates.end()) {
        it->second.active = false;
    }
}

void UIAnimator::StopAll() {
    for (auto& [property, state] : m_AnimationStates) {
        state.active = false;
    }
}

bool UIAnimator::IsAnimating(const std::string& property) const {
    auto it = m_AnimationStates.find(property);
    return it != m_AnimationStates.end() && it->second.active;
}

void UIAnimator::SetUpdateCallback(const std::string& property,
                                 const std::function<void(float)>& callback) {
    auto it = m_AnimationStates.find(property);
    if (it != m_AnimationStates.end()) {
        it->second.updateCallback = callback;
    }
}

void UIAnimator::SetCompleteCallback(const std::string& property,
                                   const std::function<void()>& callback) {
    auto it = m_AnimationStates.find(property);
    if (it != m_AnimationStates.end()) {
        it->second.completeCallback = callback;
    }
}

// Transition Effects Implementation
void UIAnimator::FadeIn(float duration) {
    Animate("opacity", 0.0f, 1.0f, duration, UIEaseType::QuadOut);
}

void UIAnimator::FadeOut(float duration) {
    Animate("opacity", 1.0f, 0.0f, duration, UIEaseType::QuadOut);
}

void UIAnimator::SlideIn(const glm::vec2& from, float duration) {
    Animate("positionX", from.x, 0.0f, duration, UIEaseType::QuadOut);
    Animate("positionY", from.y, 0.0f, duration, UIEaseType::QuadOut);
}

void UIAnimator::SlideOut(const glm::vec2& to, float duration) {
    Animate("positionX", 0.0f, to.x, duration, UIEaseType::QuadOut);
    Animate("positionY", 0.0f, to.y, duration, UIEaseType::QuadOut);
}

void UIAnimator::Scale(const glm::vec2& from, const glm::vec2& to, float duration) {
    Animate("scaleX", from.x, to.x, duration, UIEaseType::QuadOut);
    Animate("scaleY", from.y, to.y, duration, UIEaseType::QuadOut);
}

void UIAnimator::Rotate(float fromAngle, float toAngle, float duration) {
    Animate("rotation", fromAngle, toAngle, duration, UIEaseType::QuadOut);
}

} // namespace Core
} // namespace VGE 