#pragma once

#include "Core/Component.h"
#include <string>
#include <vector>
#include <memory>

struct EditorContext {
    std::string currentFile;
    std::string currentFunction;
    std::vector<std::string> openFiles;
    std::string projectType;
    std::string activeTask;
};

struct DocumentationContext {
    std::vector<std::string> relevantDocs;
    std::vector<std::string> suggestedTopics;
    std::vector<std::string> recentlyViewedDocs;
};

class ContextManagerComponent : public Component {
public:
    explicit ContextManagerComponent(GameObject* owner);
    
    void Initialize() override;
    void Update() override;
    
    // Context management
    void UpdateEditorContext(const EditorContext& context);
    void UpdateDocumentationContext();
    void RecordUserQuery(const std::string& query);
    
    // Getters
    const EditorContext& GetEditorContext() const { return m_EditorContext; }
    const DocumentationContext& GetDocumentationContext() const { return m_DocContext; }
    std::string GetCurrentQuery() const { return m_CurrentQuery; }
    void* GetCurrentContext() const; // Returns combined context
    
    // Context analysis
    float GetContextRelevance(const std::string& topic) const;
    std::vector<std::string> GetSuggestedResources() const;
    
private:
    EditorContext m_EditorContext;
    DocumentationContext m_DocContext;
    std::string m_CurrentQuery;
    std::vector<std::string> m_QueryHistory;
    
    // Helper functions
    void UpdateContextualSuggestions();
    void CleanupQueryHistory();
};
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             