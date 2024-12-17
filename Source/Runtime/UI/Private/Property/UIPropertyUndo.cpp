#include "Runtime/UI/Public/Property/UIPropertyUndo.h"
#include "Runtime/Core/Public/Core/Logger.h"

namespace VGE {
namespace Editor {

void UIPropertyUndoSystem::RecordChange(const std::string& propertyName,
                                      const PropertyValue& oldValue,
                                      const PropertyValue& newValue,
                                      const std::string& displayName) {
    PropertyChange change{propertyName, oldValue, newValue, displayName};

    if (m_CurrentCompound) {
        m_CurrentCompound->changes.push_back(change);
    }
    else {
        CompoundChange singleChange;
        singleChange.name = displayName;
        singleChange.changes.push_back(change);
        m_UndoStack.push_back(std::move(singleChange));
    }

    // Clear redo stack when new changes are made
    m_RedoStack.clear();
}

void UIPropertyUndoSystem::BeginCompoundChange(const std::string& name) {
    if (m_CurrentCompound) {
        Logger::Log(LogLevel::Warning, "Beginning compound change while another is active");
        EndCompoundChange();
    }

    m_CurrentCompound = std::make_unique<CompoundChange>();
    m_CurrentCompound->name = name;
}

void UIPropertyUndoSystem::EndCompoundChange() {
    if (!m_CurrentCompound) {
        Logger::Log(LogLevel::Warning, "Ending compound change when none is active");
        return;
    }

    if (!m_CurrentCompound->changes.empty()) {
        m_UndoStack.push_back(std::move(*m_CurrentCompound));
    }

    m_CurrentCompound.reset();
}

bool UIPropertyUndoSystem::Undo() {
    if (!CanUndo()) {
        return false;
    }

    // Get the last change
    CompoundChange& change = m_UndoStack.back();

    // Apply changes in reverse order
    for (auto it = change.changes.rbegin(); it != change.changes.rend(); ++it) {
        // TODO: Apply the change through the property system
        // This needs to be implemented in the property system
    }

    // Move to redo stack
    m_RedoStack.push_back(std::move(change));
    m_UndoStack.pop_back();

    return true;
}

bool UIPropertyUndoSystem::Redo() {
    if (!CanRedo()) {
        return false;
    }

    // Get the last undone change
    CompoundChange& change = m_RedoStack.back();

    // Apply changes in forward order
    for (const auto& propertyChange : change.changes) {
        // TODO: Apply the change through the property system
        // This needs to be implemented in the property system
    }

    // Move to undo stack
    m_UndoStack.push_back(std::move(change));
    m_RedoStack.pop_back();

    return true;
}

std::string UIPropertyUndoSystem::GetUndoName() const {
    if (!CanUndo()) {
        return "";
    }
    return m_UndoStack.back().name;
}

std::string UIPropertyUndoSystem::GetRedoName() const {
    if (!CanRedo()) {
        return "";
    }
    return m_RedoStack.back().name;
}

}} // namespace VGE::Editor 