#include "Core/NLPSystem.h"
#include "Core/IntentClassifier.h"
#include "Core/EntityExtractor.h"
#include "Core/ContextAnalyzer.h"
#include "Core/ResponseGenerator.h"
#include "Core/Time.h"
#include <algorithm>

NLPSystem::NLPSystem() {
    // Initialize components
    m_IntentClassifier = std::make_unique<IntentClassifier>();
    m_EntityExtractor = std::make_unique<EntityExtractor>();
    m_ContextAnalyzer = std::make_unique<ContextAnalyzer>();
    m_ResponseGenerator = std::make_unique<ResponseGenerator>();
    
    // Load models and patterns
    LoadModels();
}

NLPSystem::~NLPSystem() = default;

NLPResponse NLPSystem::ProcessQuery(const std::string& query, const NLPContext& context) {
    // Update context
    UpdateContext(context);
    
    // Classify intent
    Intent intent = ClassifyIntent(query);
    if (!ValidateIntent(intent)) {
        return CreateErrorResponse("Could not understand intent");
    }
    
    // Extract entities
    std::vector<Entity> entities = ExtractEntities(query);
    if (!ValidateEntities(entities)) {
        return CreateErrorResponse("Could not extract required information");
    }
    
    // Generate response
    NLPResponse response = GenerateResponse(intent, entities, context);
    
    // Log interaction
    LogInteraction(query, intent, response);
    
    return response;
}

Intent NLPSystem::ClassifyIntent(const std::string& query) {
    return m_IntentClassifier->ClassifyQuery(query);
}

std::vector<Entity> NLPSystem::ExtractEntities(const std::string& query) {
    return m_EntityExtractor->ExtractEntities(query);
}

void NLPSystem::UpdateContext(const NLPContext& context) {
    m_CurrentContext = context;
    m_ContextAnalyzer->UpdateContext(context);
}

void NLPSystem::ClearContext() {
    m_CurrentContext = NLPContext();
    m_ContextAnalyzer->ClearContext();
    m_QueryHistory.clear();
}

void NLPSystem::LoadModels() {
    // Load all component models
    m_IntentClassifier->LoadPatterns();
    m_EntityExtractor->LoadPatterns();
    m_ResponseGenerator->LoadTemplates();
}

void NLPSystem::UpdateModels() {
    // Update component models based on recent interactions
    m_IntentClassifier->UpdatePatterns();
    m_EntityExtractor->LoadPatterns();
    m_ResponseGenerator->OptimizeTemplates();
}

void NLPSystem::TrainFromInteraction(
    const std::string& query,
    const NLPResponse& response,
    bool wasSuccessful) {
    
    // Train intent classifier
    auto intent = ClassifyIntent(query);
    m_IntentClassifier->TrainFromExample(query, intent.name);
    
    // Train entity extractor
    auto entities = ExtractEntities(query);
    m_EntityExtractor->TrainFromExample(query, entities);
    
    // Update response generator
    float effectiveness = wasSuccessful ? 1.0f : 0.0f;
    m_ResponseGenerator->LearnFromFeedback(response, effectiveness, "");
}

bool NLPSystem::ValidateIntent(const Intent& intent) {
    // Check confidence threshold
    const float MIN_CONFIDENCE = 0.6f;
    if (intent.confidence < MIN_CONFIDENCE) {
        return false;
    }
    
    // Validate required parameters
    return ValidateIntentParameters(intent);
}

bool NLPSystem::ValidateEntities(const std::vector<Entity>& entities) {
    // Check each entity's confidence
    const float MIN_ENTITY_CONFIDENCE = 0.7f;
    
    return std::all_of(entities.begin(), entities.end(),
        [MIN_ENTITY_CONFIDENCE](const Entity& entity) {
            return entity.confidence >= MIN_ENTITY_CONFIDENCE;
        });
}

NLPResponse NLPSystem::GenerateResponse(
    const Intent& intent,
    const std::vector<Entity>& entities,
    const NLPContext& context) {
    
    // Prepare response context
    ResponseContext responseContext{
        intent,
        entities,
        context,
        {}  // No constraints by default
    };
    
    // Generate response using response generator
    return m_ResponseGenerator->GenerateResponse(responseContext);
}

void NLPSystem::LogInteraction(
    const std::string& query,
    const Intent& intent,
    const NLPResponse& response) {
    
    // Add to query history
    m_QueryHistory.push_back({query, intent});
    
    // Limit history size
    const size_t MAX_HISTORY = 100;
    if (m_QueryHistory.size() > MAX_HISTORY) {
        m_QueryHistory.erase(m_QueryHistory.begin());
    }
    
    // TODO: Implement more detailed logging
}

// P