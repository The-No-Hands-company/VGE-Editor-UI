/**
 * @file UIAnimation.h
 * @brief Defines the animation system for the Versa Engine UI framework.
 * 
 * The animation system provides a flexible and powerful way to create smooth
 * transitions and effects for UI elements. Features include:
 * - Property-based animations
 * - Easing functions
 * - Animation tracks and clips
 * - Predefined animation templates
 * - Custom animation callbacks
 */

#pragma once

#include "Runtime/Core/Public/Core/UICore.h"

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "Runtime/Core/Public/Base/VGE.h"

namespace VGE {
namespace Core {

/**
 * @enum UIEaseType
 * @brief Defines various easing functions for animations.
 */
enum class UIEaseType {
    Linear,     ///< Linear interpolation
    QuadIn,     ///< Quadratic ease in
    QuadOut,    ///< Quadratic ease out
    QuadInOut,  ///< Quadratic ease in and out
    CubicIn,    ///< Cubic ease in
    CubicOut,   ///< Cubic ease out
    CubicInOut, ///< Cubic ease in and out
    QuartIn,    ///< Quartic ease in
    QuartOut,   ///< Quartic ease out
    QuartInOut, ///< Quartic ease in and out
    QuintIn,    ///< Quintic ease in
    QuintOut,   ///< Quintic ease out
    QuintInOut, ///< Quintic ease in and out
    SineIn,     ///< Sine ease in
    SineOut,    ///< Sine ease out
    SineInOut,  ///< Sine ease in and out
    ExpoIn,     ///< Exponential ease in
    ExpoOut,    ///< Exponential ease out
    ExpoInOut,  ///< Exponential ease in and out
    CircIn,     ///< Circular ease in
    CircOut,    ///< Circular ease out
    CircInOut,  ///< Circular ease in and out
    ElasticIn,  ///< Elastic ease in
    ElasticOut, ///< Elastic ease out
    ElasticInOut, ///< Elastic ease in and out
    BackIn,     ///< Back ease in
    BackOut,    ///< Back ease out
    BackInOut,  ///< Back ease in and out
    BounceIn,   ///< Bounce ease in
    BounceOut,  ///< Bounce ease out
    BounceInOut ///< Bounce ease in and out
};

/**
 * @class UIEasing
 * @brief Provides easing function implementations and utilities.
 */
class UIEasing {
public:
    static float Interpolate(float t, UIEaseType type);
    static std::function<float(float)> GetEasingFunction(UIEaseType type);

private:
    static float Linear(float t);
    static float QuadIn(float t);
    static float QuadOut(float t);
    static float QuadInOut(float t);
    // ... (other easing functions)
};

/**
 * @struct UIAnimationState
 * @brief Holds the current state of an animation.
 * 
 * Tracks progress, values, and callbacks for a single property animation.
 */
struct UIAnimationState {
    bool active = false;
    float startValue = 0.0f;
    float endValue = 0.0f;
    float currentValue = 0.0f;
    float duration = 0.0f;
    float elapsed = 0.0f;
    UIEaseType easeType = UIEaseType::Linear;
    std::function<void(float)> updateCallback;
    std::function<void()> completeCallback;
};

/**
 * @class UIAnimationTrack
 * @brief Represents a sequence of keyframes for a single property.
 * 
 * Features:
 * - Multiple keyframes with values and times
 * - Easing between keyframes
 * - Loop control
 * - Duration management
 */
class UIAnimationTrack {
public:
    UIAnimationTrack();
    ~UIAnimationTrack();

    void AddKeyframe(float time, float value);
    void SetEasing(UIEaseType type);
    void SetLoop(bool loop);
    void SetDuration(float duration);
    float Evaluate(float time) const;

private:
    struct Keyframe {
        float time;
        float value;
    };
    std::vector<Keyframe> m_Keyframes;
    UIEaseType m_EaseType;
    bool m_Loop;
    float m_Duration;
};

/**
 * @class UIAnimationClip
 * @brief A collection of animation tracks that can be played together.
 * 
 * Allows grouping multiple property animations into a single playable unit.
 * Each track in the clip can animate a different property.
 */
class UIAnimationClip {
public:
    UIAnimationClip(const std::string& name);
    ~UIAnimationClip();

    void AddTrack(const std::string& property, std::shared_ptr<UIAnimationTrack> track);
    void RemoveTrack(const std::string& property);
    std::shared_ptr<UIAnimationTrack> GetTrack(const std::string& property) const;
    void SetDuration(float duration);
    float GetDuration() const;
    void Evaluate(float time, std::unordered_map<std::string, float>& values) const;

private:
    std::string m_Name;
    float m_Duration;
    std::unordered_map<std::string, std::shared_ptr<UIAnimationTrack>> m_Tracks;
};

/**
 * @class UIAnimator
 * @brief Main class for managing and playing animations.
 * 
 * Features:
 * - Single property animations
 * - Animation clip playback
 * - Transition effects (fade, slide, scale, rotate)
 * - Animation state management
 * - Update callbacks
 */
class UIAnimator {
public:
    UIAnimator();
    ~UIAnimator();

    // Single Property Animation
    void Animate(const std::string& property, float startValue, float endValue,
                float duration, UIEaseType easeType = UIEaseType::Linear);
    void AnimateTo(const std::string& property, float endValue,
                  float duration, UIEaseType easeType = UIEaseType::Linear);
    void Stop(const std::string& property);
    void StopAll();
    bool IsAnimating(const std::string& property) const;

    // Animation Clips
    void AddClip(std::shared_ptr<UIAnimationClip> clip);
    void RemoveClip(const std::string& name);
    void PlayClip(const std::string& name, bool loop = false);
    void StopClip(const std::string& name);
    bool IsPlayingClip(const std::string& name) const;

    // Transition Effects
    void FadeIn(float duration);
    void FadeOut(float duration);
    void SlideIn(const glm::vec2& from, float duration);
    void SlideOut(const glm::vec2& to, float duration);
    void Scale(const glm::vec2& from, const glm::vec2& to, float duration);
    void Rotate(float fromAngle, float toAngle, float duration);

    // Update
    void Update(float deltaTime);
    void SetUpdateCallback(const std::string& property,
                          const std::function<void(float)>& callback);
    void SetCompleteCallback(const std::string& property,
                           const std::function<void()>& callback);

private:
    std::unordered_map<std::string, UIAnimationState> m_AnimationStates;
    std::unordered_map<std::string, std::shared_ptr<UIAnimationClip>> m_Clips;
    std::unordered_map<std::string, bool> m_PlayingClips;

    void UpdateAnimation(const std::string& property, UIAnimationState& state,
                        float deltaTime);
    void CompleteAnimation(const std::string& property, UIAnimationState& state);
};

/**
 * @class UIAnimationTemplates
 * @brief Provides predefined animation templates for common effects.
 * 
 * Available templates:
 * - Fade in/out
 * - Pulse
 * - Bounce
 * - Shake
 * - Rotate
 * - Slide
 */
class UIAnimationTemplates {
public:
    static std::shared_ptr<UIAnimationClip> CreateFadeInOut(float duration);
    static std::shared_ptr<UIAnimationClip> CreatePulse(float duration);
    static std::shared_ptr<UIAnimationClip> CreateBounce(float duration);
    static std::shared_ptr<UIAnimationClip> CreateShake(float duration);
    static std::shared_ptr<UIAnimationClip> CreateRotate(float duration);
    static std::shared_ptr<UIAnimationClip> CreateSlide(const glm::vec2& from,
                                                       const glm::vec2& to,
                                                       float duration);

    // New modern animation templates
    static std::shared_ptr<UIAnimationClip> CreateElasticEntry(float duration);
    static std::shared_ptr<UIAnimationClip> CreatePopup(float duration);
    static std::shared_ptr<UIAnimationClip> CreateAttentionGrab(float duration);
    static std::shared_ptr<UIAnimationClip> CreateSwipeReveal(const glm::vec2& from,
                                                             const glm::vec2& to,
                                                             float duration);
    static std::shared_ptr<UIAnimationClip> CreateTypewriter(float duration,
                                                            int characterCount);
};

} // namespace Core
} // namespace VGE



