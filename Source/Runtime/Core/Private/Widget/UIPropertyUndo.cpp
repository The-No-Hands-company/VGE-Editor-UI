#include "Runtime/Core/Public/Widget/UIPropertyUndo.h"
#include "Runtime/Core/Public/Widget/UIPropertyPanel.h"
#include "Runtime/Core/Public/Core/Logger.h"

namespace VGE {
namespace UI {

// Set Property Command Implementation
UISetPropertyCommand::UISetPropertyCommand(const std::string& propertyName, 
                                         const PropertyValue& oldValue, 
                                         const PropertyValue& newValue)
    : m_PropertyName(propertyName)
    , m_OldValue(oldValue)
    , m_NewValue(newValue) {}

void UISetPropertyCommand::Execute() {
    if (m_Panel) {
        m_Panel->SetPropertyValue(m_PropertyName, m_NewValue);
    }
}

void UISetPropertyCommand::Undo() {
    if (m_Panel) {
        m_Panel->SetPropertyValue(m_PropertyName, m_OldValue);
    }
}

void UISetPropertyCommand::Redo() {
    Execute();
}

std::string UISetPropertyCommand::GetDescription() const {
    return "Set " + m_PropertyName;
}

// Batch Property Command Implementation
void UIBatchPropertyCommand::AddCommand(std::shared_ptr<UIPropertyCommand> command) {
    if (command) {
        command->m_Panel = m_Panel;
        m_Commands.push_back(command);
    }
}

void UIBatchPropertyCommand::ClearCommands() {
    m_Commands.clear();
}

void UIBatchPropertyCommand::Execute() {
    for (auto& command : m_Commands) {
        command->Execute();
    }
}

void UIBatchPropertyCommand::Undo() {
    for (auto it = m_Commands.rbegin(); it != m_Commands.rend(); ++it) {
        (*it)->Undo();
    }
}

void UIBatchPropertyCommand::Redo() {
    for (auto& command : m_Commands) {
        command->Redo();
    }
}

std::string UIBatchPropertyCommand::GetDescription() const {
    return "Batch Edit (" + std::to_string(m_Commands.size()) + " properties)";
}

// Property Undo System Implementation
UIPropertyUndo::UIPropertyUndo(UIPropertyPanel* panel) : m_Panel(panel) {}

void UIPropertyUndo::Execute(std::shared_ptr<UIPropertyCommand> command) {
    if (!command) return;
    
    command->m_Panel = m_Panel;
    
    if (m_IsBatchActive) {
        if (!m_CurrentBatch) {
            m_CurrentBatch = std::make_shared<UIBatchPropertyCommand>();
            m_CurrentBatch->m_Panel = m_Panel;
        }
        m_CurrentBatch->AddCommand(command);
    } else {
        command->Execute();
        PushCommand(command);
    }
}

void UIPropertyUndo::BeginBatch() {
    if (m_IsBatchActive) return;
    
    m_IsBatchActive = true;
    m_CurrentBatch = std::make_shared<UIBatchPropertyCommand>();
    m_CurrentBatch->m_Panel = m_Panel;
}

void UIPropertyUndo::EndBatch() {
    if (!m_IsBatchActive) return;
    
    m_IsBatchActive = false;
    
    if (m_CurrentBatch && !m_CurrentBatch->m_Commands.empty()) {
        m_CurrentBatch->Execute();
        PushCommand(m_CurrentBatch);
    }
    
    m_CurrentBatch.reset();
}

void UIPropertyUndo::Clear() {
    m_UndoStack.clear();
    m_RedoStack.clear();
    m_CurrentBatch.reset();
    m_IsBatchActive = false;
}

bool UIPropertyUndo::CanUndo() const {
    return !m_UndoStack.empty();
}

bool UIPropertyUndo::CanRedo() const {
    return !m_RedoStack.empty();
}

void UIPropertyUndo::Undo() {
    if (!CanUndo()) return;
    
    auto command = m_UndoStack.back();
    m_UndoStack.pop_back();
    
    command->Undo();
    m_RedoStack.push_back(command);
    
    TrimHistory();
}

void UIPropertyUndo::Redo() {
    if (!CanRedo()) return;
    
    auto command = m_RedoStack.back();
    m_RedoStack.pop_back();
    
    command->Redo();
    m_UndoStack.push_back(command);
    
    TrimHistory();
}

std::string UIPropertyUndo::GetUndoDescription() const {
    return CanUndo() ? m_UndoStack.back()->GetDescription() : "";
}

std::string UIPropertyUndo::GetRedoDescription() const {
    return CanRedo() ? m_RedoStack.back()->GetDescription() : "";
}

size_t UIPropertyUndo::GetUndoCount() const {
    return m_UndoStack.size();
}

size_t UIPropertyUndo::GetRedoCount() const {
    return m_RedoStack.size();
}

void UIPropertyUndo::SetMaxUndoLevels(size_t levels) {
    m_MaxUndoLevels = levels;
    TrimHistory();
}

size_t UIPropertyUndo::GetMaxUndoLevels() const {
    return m_MaxUndoLevels;
}

void UIPropertyUndo::PushCommand(std::shared_ptr<UIPropertyCommand> command) {
    m_UndoStack.push_back(command);
    m_RedoStack.clear();
    TrimHistory();
}

void UIPropertyUndo::TrimHistory() {
    while (m_UndoStack.size() > m_MaxUndoLevels) {
        m_UndoStack.pop_front();
    }
}

}} // namespace VGE::UI 