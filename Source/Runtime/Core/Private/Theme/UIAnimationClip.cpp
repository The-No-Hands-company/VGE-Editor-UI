#include "Runtime/Core/Public/Theme/UIAnimation.h"

namespace VGE {
namespace Core {

UIAnimationClip::UIAnimationClip(const std::string& name)
    : m_Name(name), m_Duration(0.0f) {}

UIAnimationClip::~UIAnimationClip() {}

void UIAnimationClip::AddTrack(const std::string& property,
                              std::shared_ptr<UIAnimationTrack> track) {
    m_Tracks[property] = track;
    // Update clip duration if this track is longer
    if (track->GetDuration() > m_Duration) {
        m_Duration = track->GetDuration();
    }
}

void UIAnimationClip::RemoveTrack(const std::string& property) {
    m_Tracks.erase(property);
    // Recalculate duration
    m_Duration = 0.0f;
    for (const auto& [prop, track] : m_Tracks) {
        if (track->GetDuration() > m_Duration) {
            m_Duration = track->GetDuration();
        }
    }
}

std::shared_ptr<UIAnimationTrack> UIAnimationClip::GetTrack(
    const std::string& property) const {
    auto it = m_Tracks.find(property);
    return (it != m_Tracks.end()) ? it->second : nullptr;
}

void UIAnimationClip::SetDuration(float duration) {
    m_Duration = duration;
    // Update all tracks to match the new duration
    for (auto& [property, track] : m_Tracks) {
        track->SetDuration(duration);
    }
}

float UIAnimationClip::GetDuration() const {
    return m_Duration;
}

void UIAnimationClip::Evaluate(float time,
                             std::unordered_map<std::string, float>& values) const {
    for (const auto& [property, track] : m_Tracks) {
        values[property] = track->Evaluate(time);
    }
}

} // namespace Core
} // namespace VGE 