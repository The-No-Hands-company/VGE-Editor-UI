#pragma once

#include "Core/AIAgent.h"
#include "Core/GameObject.h"
#include "Core/BTStatus.h"
#include <memory>
#include <string>
#include <vector>

enum class AssistantState {
    Idle,
    ProcessingQuery,
    ProvidingAssistance,
    UpdatingUI,
    TrackingProgress
};

struct QueryResult {
    std::string intent;
    float confidence;
    std::vector<std::string> entities;
    std::string context;
};

struct AssistanceOpportunity {
    std::string type;
    float relevance;
    std::string suggestion;
    std::vector<std::string> resources;
};

class AIAssistant : public AIAgent {
public:
    explicit AIAssistant(GameObject* owner);
    
    void Update() override;
    
protected:
    void InitializeBehaviorTree();
    void InitializeComponents();
    
    // Core behavior handlers
    BTStatus HandleUserQuery();
    BTStatus ProvideProactiveAssistance();
    BTStatus UpdateLearningProgress();
    BTStatus AdaptUserInterface();
    BTStatus MonitorUserActivity();
    
    // Helper functions
    void UpdateContext();
    void UpdateUserProfile();
    QueryResult ProcessQuery(const std::string& query);
    void GenerateResponse(const QueryResult& result);
    void UpdateLearningFromInteraction(const QueryResult& result);
    AssistanceOpportunity AnalyzeContext();
    void ProvideSuggestion(const AssistanceOpportunity& opportunity);
    void CheckLearningMilestones();
    float CalculateOptimalUIComplexity();
    void UpdateActivityPatterns();
    void IdentifyLearningOpportunities();
    void UpdateContextualResources();
    void AdaptAssistanceStyle(const UserPreferences& preferences);
    
    // State checking
