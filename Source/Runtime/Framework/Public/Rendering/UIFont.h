#pragma once

#include "NLPSystem.h"
#include "ContextAnalyzer.h"
#include "IntentClassifier.h"
#include "EntityExtractor.h"
#include <memory>
#include <vector>
#include <string>

namespace VGE::AI {

class AICore {
public:
    AICore();
    ~AICore();

    // Core functionality
    void Initialize();
    void Update(float deltaTime);
    void ProcessInput(const std::string& input);
    void HandleEvent(const AIEvent& event);

    // Domain-specific assistance
    void AssistGameDevelopment(const GameDevContext& context);
    void AssistCADModeling(const CADContext& context);
    void AssistAnimation(const AnimationContext& context);
    void AdaptUserInterface(const UIContext& context);

    // Learning and adaptation
    void LearnFromInteraction(const UserInteraction& interaction);
    void UpdateModels();
    void OptimizeWorkflow();

    // System management
    void SaveState();
    void LoadState();
    void ConfigureSystem(const AIConfig& config);

private:
    // Core components
    std::unique_ptr<NLPSystem> m_NLPSystem;
    std::unique_ptr<ContextAnalyzer> m_ContextAnalyzer;
    std::unique_ptr<IntentClassifier> m_IntentClassifier;
    std::unique_ptr<EntityExtractor> m_EntityExtractor;

    // Domain managers
    std::unique_ptr<GameDevAI> m_GameDevManager;
    std::unique_ptr<CADAssistant> m_CADManager;
    std::unique_ptr<AnimationAI> m_AnimationManager;
    std::unique_ptr<AdaptiveUI> m_UIManager;

    // Learning system
    std::unique_ptr<LearningSystem> m_LearningSystem;
    std::unique_ptr<Patter