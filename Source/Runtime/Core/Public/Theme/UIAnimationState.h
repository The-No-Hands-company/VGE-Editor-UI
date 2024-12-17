#pragma once

#include "Runtime/Core/Public/Theme/UIAnimation.h"
#include "Runtime/Core/Public/Theme/UIAnimationSequence.h"
#include <string>
#include <unordered_map>
#include <memory>

namespace VGE {
namespace Core {

/**
 * @class UIAnimationStateTransition
 * @brief Defines a transition between two animation states.
 */
class UIAnimationStateTransition {
public:
    UIAnimationStateTransition(const std::string& from, const std::string& to);

    void SetDuration(float duration) { m_Duration = duration; }
    void SetBlendTime(float time) { m_BlendTime = time; }
    void SetExitTime(float time) { m_ExitTime = time; }
    void SetCondition(std::function<bool()> condition) { m_Condition = condition; }
    void SetInterruptible(bool interruptible) { m_Interruptible = interruptible; }

private:
    std::string m_FromState;
    std::string m_ToState;
    float m_Duration = 0.3f;
    float m_BlendTime = 0.1f;
    float m_ExitTime = 0.0f;
    bool m_Interruptible = true;
    std::function<bool()> m_Condition;

    friend class UIAnimationStateMachine;
};

/**
 * @class UIAnimationState
 * @brief Represents a single state in the animation state machine.
 */
class UIAnimationState {
public:
    UIAnimationState(const std::string& name);

    void AddClip(std::shared_ptr<UIAnimationClip> clip);
    void AddSequence(std::shared_ptr<UIAnimationSequence> sequence);
    void SetLooping(bool loop) { m_Loop = loop; }
    void SetSpeed(float speed) { m_Speed = speed; }
    void SetBlendTree(std::shared_ptr<UIAnimationBlendTree> blendTree);

private:
    std::string m_Name;
    std::vector<std::shared_ptr<UIAnimationClip>> m_Clips;
    std::shared_ptr<UIAnimationSequence> m_Sequence;
    std::shared_ptr<UIAnimationBlendTree> m_BlendTree;
    bool m_Loop = false;
    float m_Speed = 1.0f;

    friend class UIAnimationStateMachine;
};

/**
 * @class UIAnimationBlendTree
 * @brief Manages blending between multiple animations based on parameters.
 */
class UIAnimationBlendTree {
public:
    UIAnimationBlendTree();

    void AddAnimation(const std::string& name, std::shared_ptr<UIAnimationClip> clip,
                     const std::vector<float>& parameters);
    void SetParameter(size_t index, float value);
    void Update(float deltaTime);
    std::unordered_map<std::string, float> GetBlendedValues() const;

private:
    struct BlendNode {
        std::string name;
        std::shared_ptr<UIAnimationClip> clip;
        std::vector<float> parameters;
        float weight = 0.0f;
    };

    std::vector<BlendNode> m_Nodes;
    std::vector<float> m_Parameters;
    void UpdateWeights();
};

/**
 * @class UIAnimationStateMachine
 * @brief Manages state-based animations with transitions and blending.
 */
class UIAnimationStateMachine {
public:
    UIAnimationStateMachine();

    // State Management
    void AddState(const std::string& name, std::shared_ptr<UIAnimationState> state);
    void SetDefaultState(const std::string& name);
    void AddTransition(std::shared_ptr<UIAnimationStateTransition> transition);

    // Runtime Control
    void Start();
    void Stop();
    void Update(float deltaTime);
    void TriggerTransition(const std::string& toState);

    // State Query
    std::string GetCurrentState() const;
    bool IsInTransition() const;
    float GetTransitionProgress() const;

private:
    std::unordered_map<std::string, std::shared_ptr<UIAnimationState>> m_States;
    std::vector<std::shared_ptr<UIAnimationStateTransition>> m_Transitions;
    std::string m_CurrentState;
    std::string m_DefaultState;
    bool m_InTransition = false;
    float m_TransitionTime = 0.0f;
    std::shared_ptr<UIAnimationStateTransition> m_CurrentTransition;

    void UpdateTransitions(float deltaTime);
    void BlendStates(const std::string& fromState, const std::string& toState, float blend);
    std::shared_ptr<UIAnimationStateTransition> FindTransition(const std::string& from,
                                                             const std::string& to) const;
};

} // namespace Core
} // namespace VGE 