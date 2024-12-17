#include "Runtime/Core/Public/Theme/UIAnimation.h"
#include <cmath>

namespace VGE {
namespace Core {

std::shared_ptr<UIAnimationClip> UIAnimationTemplates::CreateFadeInOut(float duration) {
    auto clip = std::make_shared<UIAnimationClip>("FadeInOut");
    auto track = std::make_shared<UIAnimationTrack>();
    
    track->AddKeyframe(0.0f, 0.0f);                  // Start: fully transparent
    track->AddKeyframe(duration * 0.5f, 1.0f);       // Middle: fully opaque
    track->AddKeyframe(duration, 0.0f);              // End: fully transparent
    
    track->SetEasing(UIEaseType::SineInOut);
    track->SetDuration(duration);
    track->SetLoop(true);
    
    clip->AddTrack("opacity", track);
    clip->SetDuration(duration);
    
    return clip;
}

std::shared_ptr<UIAnimationClip> UIAnimationTemplates::CreatePulse(float duration) {
    auto clip = std::make_shared<UIAnimationClip>("Pulse");
    auto scaleTrack = std::make_shared<UIAnimationTrack>();
    
    scaleTrack->AddKeyframe(0.0f, 1.0f);            // Start: normal size
    scaleTrack->AddKeyframe(duration * 0.5f, 1.2f); // Middle: 20% larger
    scaleTrack->AddKeyframe(duration, 1.0f);        // End: normal size
    
    scaleTrack->SetEasing(UIEaseType::QuadInOut);
    scaleTrack->SetDuration(duration);
    scaleTrack->SetLoop(true);
    
    clip->AddTrack("scaleX", scaleTrack);
    clip->AddTrack("scaleY", scaleTrack);
    clip->SetDuration(duration);
    
    return clip;
}

std::shared_ptr<UIAnimationClip> UIAnimationTemplates::CreateBounce(float duration) {
    auto clip = std::make_shared<UIAnimationClip>("Bounce");
    auto track = std::make_shared<UIAnimationTrack>();
    
    track->AddKeyframe(0.0f, 0.0f);                 // Start position
    track->AddKeyframe(duration * 0.4f, -20.0f);    // First bounce up
    track->AddKeyframe(duration * 0.5f, 0.0f);      // Back to ground
    track->AddKeyframe(duration * 0.7f, -10.0f);    // Second bounce (smaller)
    track->AddKeyframe(duration * 0.8f, 0.0f);      // Back to ground
    track->AddKeyframe(duration * 0.9f, -5.0f);     // Third bounce (smallest)
    track->AddKeyframe(duration, 0.0f);             // Final position
    
    track->SetEasing(UIEaseType::QuadOut);
    track->SetDuration(duration);
    
    clip->AddTrack("positionY", track);
    clip->SetDuration(duration);
    
    return clip;
}

std::shared_ptr<UIAnimationClip> UIAnimationTemplates::CreateShake(float duration) {
    auto clip = std::make_shared<UIAnimationClip>("Shake");
    auto track = std::make_shared<UIAnimationTrack>();
    
    const float intensity = 5.0f;
    const int shakes = 6;
    const float timePerShake = duration / shakes;
    
    for (int i = 0; i <= shakes; ++i) {
        float t = i * timePerShake;
        float value = (i % 2 == 0) ? intensity : -intensity;
        value *= (1.0f - (float)i / shakes); // Decrease intensity over time
        track->AddKeyframe(t, value);
    }
    
    track->SetEasing(UIEaseType::Linear);
    track->SetDuration(duration);
    
    clip->AddTrack("positionX", track);
    clip->SetDuration(duration);
    
    return clip;
}

std::shared_ptr<UIAnimationClip> UIAnimationTemplates::CreateRotate(float duration) {
    auto clip = std::make_shared<UIAnimationClip>("Rotate");
    auto track = std::make_shared<UIAnimationTrack>();
    
    track->AddKeyframe(0.0f, 0.0f);        // Start: 0 degrees
    track->AddKeyframe(duration, 360.0f);   // End: 360 degrees
    
    track->SetEasing(UIEaseType::Linear);
    track->SetDuration(duration);
    track->SetLoop(true);
    
    clip->AddTrack("rotation", track);
    clip->SetDuration(duration);
    
    return clip;
}

std::shared_ptr<UIAnimationClip> UIAnimationTemplates::CreateSlide(
    const glm::vec2& from, const glm::vec2& to, float duration) {
    
    auto clip = std::make_shared<UIAnimationClip>("Slide");
    
    auto trackX = std::make_shared<UIAnimationTrack>();
    trackX->AddKeyframe(0.0f, from.x);
    trackX->AddKeyframe(duration, to.x);
    trackX->SetEasing(UIEaseType::QuadOut);
    trackX->SetDuration(duration);
    
    auto trackY = std::make_shared<UIAnimationTrack>();
    trackY->AddKeyframe(0.0f, from.y);
    trackY->AddKeyframe(duration, to.y);
    trackY->SetEasing(UIEaseType::QuadOut);
    trackY->SetDuration(duration);
    
    clip->AddTrack("positionX", trackX);
    clip->AddTrack("positionY", trackY);
    clip->SetDuration(duration);
    
    return clip;
}

std::shared_ptr<UIAnimationClip> UIAnimationTemplates::CreateElasticEntry(float duration) {
    auto clip = std::make_shared<UIAnimationClip>("ElasticEntry");
    
    // Scale animation with elastic effect
    auto scaleTrack = std::make_shared<UIAnimationTrack>();
    scaleTrack->AddKeyframe(0.0f, 0.3f);      // Start small
    scaleTrack->AddKeyframe(0.7f, 1.2f);      // Overshoot
    scaleTrack->AddKeyframe(0.85f, 0.9f);     // Bounce back
    scaleTrack->AddKeyframe(0.95f, 1.05f);    // Small overshoot
    scaleTrack->AddKeyframe(1.0f, 1.0f);      // Settle
    
    scaleTrack->SetEasing(UIEaseType::ElasticOut);
    scaleTrack->SetDuration(duration);
    
    clip->AddTrack("scaleX", scaleTrack);
    clip->AddTrack("scaleY", scaleTrack);
    
    // Fade in
    auto opacityTrack = std::make_shared<UIAnimationTrack>();
    opacityTrack->AddKeyframe(0.0f, 0.0f);
    opacityTrack->AddKeyframe(0.5f, 1.0f);
    opacityTrack->SetEasing(UIEaseType::QuadOut);
    opacityTrack->SetDuration(duration * 0.5f);
    
    clip->AddTrack("opacity", opacityTrack);
    clip->SetDuration(duration);
    
    return clip;
}

std::shared_ptr<UIAnimationClip> UIAnimationTemplates::CreatePopup(float duration) {
    auto clip = std::make_shared<UIAnimationClip>("Popup");
    
    // Scale up with bounce
    auto scaleTrack = std::make_shared<UIAnimationTrack>();
    scaleTrack->AddKeyframe(0.0f, 0.0f);      // Start invisible
    scaleTrack->AddKeyframe(0.6f, 1.1f);      // Overshoot
    scaleTrack->AddKeyframe(0.8f, 0.95f);     // Undershoot
    scaleTrack->AddKeyframe(1.0f, 1.0f);      // Final size
    
    scaleTrack->SetEasing(UIEaseType::BounceOut);
    scaleTrack->SetDuration(duration);
    
    clip->AddTrack("scaleX", scaleTrack);
    clip->AddTrack("scaleY", scaleTrack);
    
    // Fade in
    auto opacityTrack = std::make_shared<UIAnimationTrack>();
    opacityTrack->AddKeyframe(0.0f, 0.0f);
    opacityTrack->AddKeyframe(0.4f, 1.0f);
    opacityTrack->SetEasing(UIEaseType::QuadOut);
    opacityTrack->SetDuration(duration * 0.4f);
    
    clip->AddTrack("opacity", opacityTrack);
    clip->SetDuration(duration);
    
    return clip;
}

std::shared_ptr<UIAnimationClip> UIAnimationTemplates::CreateAttentionGrab(float duration) {
    auto clip = std::make_shared<UIAnimationClip>("AttentionGrab");
    
    // Pulsing scale
    auto scaleTrack = std::make_shared<UIAnimationTrack>();
    scaleTrack->AddKeyframe(0.0f, 1.0f);
    scaleTrack->AddKeyframe(0.2f, 1.15f);
    scaleTrack->AddKeyframe(0.4f, 1.0f);
    scaleTrack->AddKeyframe(0.6f, 1.15f);
    scaleTrack->AddKeyframe(0.8f, 1.0f);
    
    scaleTrack->SetEasing(UIEaseType::SineInOut);
    scaleTrack->SetDuration(duration);
    scaleTrack->SetLoop(true);
    
    clip->AddTrack("scaleX", scaleTrack);
    clip->AddTrack("scaleY", scaleTrack);
    
    // Glow effect (opacity of glow overlay)
    auto glowTrack = std::make_shared<UIAnimationTrack>();
    glowTrack->AddKeyframe(0.0f, 0.0f);
    glowTrack->AddKeyframe(0.2f, 0.4f);
    glowTrack->AddKeyframe(0.4f, 0.0f);
    glowTrack->AddKeyframe(0.6f, 0.4f);
    glowTrack->AddKeyframe(0.8f, 0.0f);
    
    glowTrack->SetEasing(UIEaseType::SineInOut);
    glowTrack->SetDuration(duration);
    glowTrack->SetLoop(true);
    
    clip->AddTrack("glowIntensity", glowTrack);
    clip->SetDuration(duration);
    
    return clip;
}

std::shared_ptr<UIAnimationClip> UIAnimationTemplates::CreateSwipeReveal(
    const glm::vec2& from, const glm::vec2& to, float duration) {
    
    auto clip = std::make_shared<UIAnimationClip>("SwipeReveal");
    
    // Position animation
    auto posXTrack = std::make_shared<UIAnimationTrack>();
    posXTrack->AddKeyframe(0.0f, from.x);
    posXTrack->AddKeyframe(duration, to.x);
    posXTrack->SetEasing(UIEaseType::ExpoOut);
    posXTrack->SetDuration(duration);
    
    auto posYTrack = std::make_shared<UIAnimationTrack>();
    posYTrack->AddKeyframe(0.0f, from.y);
    posYTrack->AddKeyframe(duration, to.y);
    posYTrack->SetEasing(UIEaseType::ExpoOut);
    posYTrack->SetDuration(duration);
    
    // Scale animation for stretchy effect
    auto scaleXTrack = std::make_shared<UIAnimationTrack>();
    scaleXTrack->AddKeyframe(0.0f, 0.8f);
    scaleXTrack->AddKeyframe(0.6f, 1.1f);
    scaleXTrack->AddKeyframe(1.0f, 1.0f);
    scaleXTrack->SetEasing(UIEaseType::BackOut);
    scaleXTrack->SetDuration(duration);
    
    auto scaleYTrack = std::make_shared<UIAnimationTrack>();
    scaleYTrack->AddKeyframe(0.0f, 0.8f);
    scaleYTrack->AddKeyframe(0.6f, 1.1f);
    scaleYTrack->AddKeyframe(1.0f, 1.0f);
    scaleYTrack->SetEasing(UIEaseType::BackOut);
    scaleYTrack->SetDuration(duration);
    
    clip->AddTrack("positionX", posXTrack);
    clip->AddTrack("positionY", posYTrack);
    clip->AddTrack("scaleX", scaleXTrack);
    clip->AddTrack("scaleY", scaleYTrack);
    clip->SetDuration(duration);
    
    return clip;
}

std::shared_ptr<UIAnimationClip> UIAnimationTemplates::CreateTypewriter(float duration, int characterCount) {
    auto clip = std::make_shared<UIAnimationClip>("Typewriter");
    
    auto charTrack = std::make_shared<UIAnimationTrack>();
    float timePerChar = duration / characterCount;
    
    for (int i = 0; i <= characterCount; ++i) {
        charTrack->AddKeyframe(i * timePerChar, static_cast<float>(i));
    }
    
    charTrack->SetEasing(UIEaseType::Linear);
    charTrack->SetDuration(duration);
    
    clip->AddTrack("visibleCharacters", charTrack);
    clip->SetDuration(duration);
    
    return clip;
}

} // namespace Core
} // namespace VGE 