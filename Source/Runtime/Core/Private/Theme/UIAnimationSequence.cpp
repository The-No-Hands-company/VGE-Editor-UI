#include "Runtime/Core/Public/Theme/UIAnimationSequence.h"

namespace VGE {
namespace Core {

UIAnimationSequence::UIAnimationSequence(const std::string& name)
    : m_Name(name)
    , m_CurrentStep(0)
    , m_CurrentTime(0.0f)
    , m_TimeScale(1.0f)
    , m_IsPlaying(false)
    , m_Loop(false) {}

UIAnimationSequence::~UIAnimationSequence() {
    Stop();
}

UIAnimationSequence& UIAnimationSequence::Then(std::shared_ptr<UIAnimationClip> clip, float delay) {
    UIAnimationStep step;
    step.clip = clip;
    step.delay = delay;
    step.parallel = false;
    m_Steps.push_back(step);
    return *this;
}

UIAnimationSequence& UIAnimationSequence::With(std::shared_ptr<UIAnimationClip> clip, float delay) {
    if (!m_Steps.empty()) {
        m_Steps.back().parallel = true;
    }
    UIAnimationStep step;
    step.clip = clip;
    step.delay = delay;
    step.parallel = false;
    m_Steps.push_back(step);
    return *this;
}

UIAnimationSequence& UIAnimationSequence::OnStepComplete(std::function<void()> callback) {
    if (!m_Steps.empty()) {
        m_Steps.back().onComplete = callback;
    }
    return *this;
}

void UIAnimationSequence::Play() {
    m_CurrentStep = 0;
    m_CurrentTime = 0.0f;
    m_IsPlaying = true;
    m_ActiveSteps.clear();
    
    // Start first step
    if (!m_Steps.empty()) {
        m_ActiveSteps.push_back(0);
    }
}

void UIAnimationSequence::Pause() {
    m_IsPlaying = false;
}

void UIAnimationSequence::Resume() {
    m_IsPlaying = true;
}

void UIAnimationSequence::Stop() {
    m_IsPlaying = false;
    m_CurrentStep = 0;
    m_CurrentTime = 0.0f;
    m_ActiveSteps.clear();
}

void UIAnimationSequence::Reset() {
    Stop();
    Play();
}

void UIAnimationSequence::SetLoop(bool loop) {
    m_Loop = loop;
}

void UIAnimationSequence::SetTimeScale(float scale) {
    m_TimeScale = scale;
}

bool UIAnimationSequence::IsPlaying() const {
    return m_IsPlaying;
}

float UIAnimationSequence::GetProgress() const {
    if (m_Steps.empty()) return 0.0f;
    
    float totalDuration = 0.0f;
    float currentProgress = 0.0f;
    
    // Calculate total duration of non-parallel animations
    for (size_t i = 0; i < m_Steps.size(); ++i) {
        if (!m_Steps[i].parallel) {
            totalDuration += m_Steps[i].clip->GetDuration() + m_Steps[i].delay;
        }
    }
    
    // Calculate current progress
    for (size_t i = 0; i < m_CurrentStep; ++i) {
        if (!m_Steps[i].parallel) {
            currentProgress += m_Steps[i].clip->GetDuration() + m_Steps[i].delay;
        }
    }
    currentProgress += m_CurrentTime;
    
    return totalDuration > 0.0f ? currentProgress / totalDuration : 1.0f;
}

void UIAnimationSequence::Update(float deltaTime) {
    if (!m_IsPlaying) return;
    
    deltaTime *= m_TimeScale;
    m_CurrentTime += deltaTime;
    
    UpdateActiveSteps(deltaTime);
}

void UIAnimationSequence::UpdateActiveSteps(float deltaTime) {
    for (auto it = m_ActiveSteps.begin(); it != m_ActiveSteps.end();) {
        size_t stepIndex = *it;
        const auto& step = m_Steps[stepIndex];
        
        // Check if we need to wait for delay
        if (m_CurrentTime < step.delay) {
            ++it;
            continue;
        }
        
        // Update the clip
        std::unordered_map<std::string, float> values;
        step.clip->Evaluate(m_CurrentTime - step.delay, values);
        
        // Check if this step is complete
        if (m_CurrentTime - step.delay >= step.clip->GetDuration()) {
            CompleteStep(stepIndex);
            it = m_ActiveSteps.erase(it);
            
            // Start next step if this one was parallel
            if (step.parallel && stepIndex + 1 < m_Steps.size()) {
                m_ActiveSteps.push_back(stepIndex + 1);
            }
        } else {
            ++it;
        }
    }
    
    // Check if we need to advance to next step
    if (m_ActiveSteps.empty() && m_CurrentStep < m_Steps.size()) {
        AdvanceToNextStep();
    }
    
    // Handle sequence completion
    if (m_ActiveSteps.empty() && m_CurrentStep >= m_Steps.size()) {
        if (m_Loop) {
            Reset();
        } else {
            Stop();
        }
    }
}

void UIAnimationSequence::AdvanceToNextStep() {
    while (m_CurrentStep < m_Steps.size()) {
        m_ActiveSteps.push_back(m_CurrentStep++);
        if (!m_Steps[m_CurrentStep - 1].parallel) {
            break;
        }
    }
    m_CurrentTime = 0.0f;
}

void UIAnimationSequence::CompleteStep(size_t stepIndex) {
    const auto& step = m_Steps[stepIndex];
    if (step.onComplete) {
        step.onComplete();
    }
}

// UIAnimationSequencer Implementation
UIAnimationSequencer& UIAnimationSequencer::Get() {
    static UIAnimationSequencer instance;
    return instance;
}

std::shared_ptr<UIAnimationSequence> UIAnimationSequencer::CreateSequence(const std::string& name) {
    auto sequence = std::make_shared<UIAnimationSequence>(name);
    m_Sequences[name] = sequence;
    return sequence;
}

void UIAnimationSequencer::PlaySequence(const std::string& name) {
    auto it = m_Sequences.find(name);
    if (it != m_Sequences.end()) {
        it->second->Play();
    }
}

void UIAnimationSequencer::StopSequence(const std::string& name) {
    auto it = m_Sequences.find(name);
    if (it != m_Sequences.end()) {
        it->second->Stop();
    }
}

void UIAnimationSequencer::StopAllSequences() {
    for (auto& [name, sequence] : m_Sequences) {
        sequence->Stop();
    }
}

void UIAnimationSequencer::Update(float deltaTime) {
    for (auto& [name, sequence] : m_Sequences) {
        sequence->Update(deltaTime);
    }
}

} // namespace Core
} // namespace VGE 