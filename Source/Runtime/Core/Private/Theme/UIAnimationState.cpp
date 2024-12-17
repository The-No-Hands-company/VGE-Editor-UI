#include "Runtime/Core/Public/Theme/UIAnimationState.h"
#include <algorithm>
#include <cmath>

namespace VGE {
namespace Core {

// UIAnimationStateTransition Implementation
UIAnimationStateTransition::UIAnimationStateTransition(const std::string& from, const std::string& to)
    : m_FromState(from), m_ToState(to) {}

// UIAnimationState Implementation
UIAnimationState::UIAnimationState(const std::string& name)
    : m_Name(name) {}

void UIAnimationState::AddClip(std::shared_ptr<UIAnimationClip> clip) {
    m_Clips.push_back(clip);
}

void UIAnimationState::AddSequence(std::shared_ptr<UIAnimationSequence> sequence) {
    m_Sequence = sequence;
}

void UIAnimationState::SetBlendTree(std::shared_ptr<UIAnimationBlendTree> blendTree) {
    m_BlendTree = blendTree;
}

// UIAnimationBlendTree Implementation
UIAnimationBlendTree::UIAnimationBlendTree() {}

void UIAnimationBlendTree::AddAnimation(const std::string& name,
                                      std::shared_ptr<UIAnimationClip> clip,
                                      const std::vector<float>& parameters) {
    BlendNode node;
    node.name = name;
    node.clip = clip;
    node.parameters = parameters;
    m_Nodes.push_back(node);

    // Ensure parameters vector has enough space
    if (parameters.size() > m_Parameters.size()) {
        m_Parameters.resize(parameters.size(), 0.0f);
    }
}

void UIAnimationBlendTree::SetParameter(size_t index, float value) {
    if (index < m_Parameters.size()) {
        m_Parameters[index] = value;
        UpdateWeights();
    }
}

void UIAnimationBlendTree::Update(float deltaTime) {
    UpdateWeights();
}

std::unordered_map<std::string, float> UIAnimationBlendTree::GetBlendedValues() const {
    std::unordered_map<std::string, float> result;
    
    // Evaluate each animation and blend the results
    for (const auto& node : m_Nodes) {
        if (node.weight > 0.0f) {
            std::unordered_map<std::string, float> values;
            node.clip->Evaluate(0.0f, values); // Get current frame values
            
            // Blend values
            for (const auto& [property, value] : values) {
                if (result.find(property) == result.end()) {
                    result[property] = 0.0f;
                }
                result[property] += value * node.weight;
            }
        }
    }
    
    return result;
}

void UIAnimationBlendTree::UpdateWeights() {
    float totalDistance = 0.0f;
    
    // Calculate distances and weights
    for (auto& node : m_Nodes) {
        float distance = 0.0f;
        for (size_t i = 0; i < node.parameters.size(); ++i) {
            float diff = node.parameters[i] - m_Parameters[i];
            distance += diff * diff;
        }
        distance = std::sqrt(distance);
        
        // Convert distance to weight (inverse relationship)
        node.weight = distance > 0.0f ? 1.0f / distance : 1.0f;
        totalDistance += node.weight;
    }
    
    // Normalize weights
    if (totalDistance > 0.0f) {
        for (auto& node : m_Nodes) {
            node.weight /= totalDistance;
        }
    }
}

// UIAnimationStateMachine Implementation
UIAnimationStateMachine::UIAnimationStateMachine() {}

void UIAnimationStateMachine::AddState(const std::string& name,
                                     std::shared_ptr<UIAnimationState> state) {
    m_States[name] = state;
    if (m_DefaultState.empty()) {
        m_DefaultState = name;
    }
}

void UIAnimationStateMachine::SetDefaultState(const std::string& name) {
    if (m_States.find(name) != m_States.end()) {
        m_DefaultState = name;
    }
}

void UIAnimationStateMachine::AddTransition(std::shared_ptr<UIAnimationStateTransition> transition) {
    m_Transitions.push_back(transition);
}

void UIAnimationStateMachine::Start() {
    m_CurrentState = m_DefaultState;
    m_InTransition = false;
    m_TransitionTime = 0.0f;
}

void UIAnimationStateMachine::Stop() {
    m_InTransition = false;
    m_TransitionTime = 0.0f;
}

void UIAnimationStateMachine::Update(float deltaTime) {
    if (m_CurrentState.empty()) return;
    
    // Update current state
    auto currentState = m_States[m_CurrentState];
    if (currentState->m_BlendTree) {
        currentState->m_BlendTree->Update(deltaTime);
    }
    
    // Update transitions
    UpdateTransitions(deltaTime);
}

void UIAnimationStateMachine::TriggerTransition(const std::string& toState) {
    if (m_CurrentState == toState) return;
    
    auto transition = FindTransition(m_CurrentState, toState);
    if (transition) {
        m_InTransition = true;
        m_TransitionTime = 0.0f;
        m_CurrentTransition = transition;
    }
}

std::string UIAnimationStateMachine::GetCurrentState() const {
    return m_CurrentState;
}

bool UIAnimationStateMachine::IsInTransition() const {
    return m_InTransition;
}

float UIAnimationStateMachine::GetTransitionProgress() const {
    if (!m_InTransition || !m_CurrentTransition) return 0.0f;
    return m_TransitionTime / m_CurrentTransition->m_Duration;
}

void UIAnimationStateMachine::UpdateTransitions(float deltaTime) {
    if (!m_InTransition || !m_CurrentTransition) return;
    
    m_TransitionTime += deltaTime;
    float blend = m_TransitionTime / m_CurrentTransition->m_Duration;
    
    if (blend >= 1.0f) {
        // Complete transition
        m_CurrentState = m_CurrentTransition->m_ToState;
        m_InTransition = false;
        m_CurrentTransition = nullptr;
    } else {
        // Blend states
        BlendStates(m_CurrentTransition->m_FromState,
                   m_CurrentTransition->m_ToState,
                   blend);
    }
}

void UIAnimationStateMachine::BlendStates(const std::string& fromState,
                                        const std::string& toState,
                                        float blend) {
    auto from = m_States[fromState];
    auto to = m_States[toState];
    
    if (from->m_BlendTree && to->m_BlendTree) {
        auto fromValues = from->m_BlendTree->GetBlendedValues();
        auto toValues = to->m_BlendTree->GetBlendedValues();
        
        // Blend between states
        std::unordered_map<std::string, float> blendedValues;
        for (const auto& [property, fromValue] : fromValues) {
            float toValue = toValues[property];
            blendedValues[property] = fromValue * (1.0f - blend) + toValue * blend;
        }
        
        // Apply blended values
        // Note: Implementation depends on how you want to apply the values
        // to your UI components
    }
}

std::shared_ptr<UIAnimationStateTransition> UIAnimationStateMachine::FindTransition(
    const std::string& from, const std::string& to) const {
    for (const auto& transition : m_Transitions) {
        if (transition->m_FromState == from && transition->m_ToState == to) {
            if (!transition->m_Condition || transition->m_Condition()) {
                return transition;
            }
        }
    }
    return nullptr;
}

} // namespace Core
} // namespace VGE 