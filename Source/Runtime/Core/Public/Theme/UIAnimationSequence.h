#pragma once

#include "Runtime/Core/Public/Theme/UIAnimation.h"
#include <vector>
#include <memory>
#include <functional>

namespace VGE {
namespace Core {

/**
 * @class UIAnimationStep
 * @brief Represents a single step in an animation sequence.
 */
struct UIAnimationStep {
    std::shared_ptr<UIAnimationClip> clip;
    float delay = 0.0f;
    std::function<void()> onComplete;
    bool parallel = false;  // If true, plays alongside next animation
};

/**
 * @class UIAnimationSequence
 * @brief Manages a sequence of animations that play in order or in parallel.
 * 
 * Features:
 * - Sequential and parallel animation playback
 * - Delayed animation starts
 * - Completion callbacks per step
 * - Sequence looping
 * - Time scaling
 */
class UIAnimationSequence {
public:
    UIAnimationSequence(const std::string& name);
    ~UIAnimationSequence();

    // Building the sequence
    UIAnimationSequence& Then(std::shared_ptr<UIAnimationClip> clip, float delay = 0.0f);
    UIAnimationSequence& With(std::shared_ptr<UIAnimationClip> clip, float delay = 0.0f);
    UIAnimationSequence& OnStepComplete(std::function<void()> callback);
    
    // Playback control
    void Play();
    void Pause();
    void Resume();
    void Stop();
    void Reset();
    
    // Properties
    void SetLoop(bool loop);
    void SetTimeScale(float scale);
    bool IsPlaying() const;
    float GetProgress() const;
    
    // Update
    void Update(float deltaTime);

private:
    std::string m_Name;
    std::vector<UIAnimationStep> m_Steps;
    size_t m_CurrentStep;
    float m_CurrentTime;
    float m_TimeScale;
    bool m_IsPlaying;
    bool m_Loop;
    std::vector<size_t> m_ActiveSteps;

    void AdvanceToNextStep();
    void UpdateActiveSteps(float deltaTime);
    void CompleteStep(size_t stepIndex);
};

/**
 * @class UIAnimationSequencer
 * @brief Global manager for animation sequences.
 */
class UIAnimationSequencer {
public:
    static UIAnimationSequencer& Get();

    std::shared_ptr<UIAnimationSequence> CreateSequence(const std::string& name);
    void PlaySequence(const std::string& name);
    void StopSequence(const std::string& name);
    void StopAllSequences();
    void Update(float deltaTime);

private:
    UIAnimationSequencer() = default;
    std::unordered_map<std::string, std::shared_ptr<UIAnimationSequence>> m_Sequences;
};

} // namespace Core
} // namespace VGE 