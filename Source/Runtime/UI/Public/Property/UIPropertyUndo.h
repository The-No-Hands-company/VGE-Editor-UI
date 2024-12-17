#pragma once

#include "Runtime/Core/Public/Core/UICore.h"
#include "Runtime/UI/Public/Controls/UIPropertyGrid.h"
#include <string>
#include <vector>
#include <memory>

namespace VGE {
namespace Editor {

/**
 * @brief Represents a property change that can be undone/redone
 */
struct PropertyChange {
    std::string propertyName;
    PropertyValue oldValue;
    PropertyValue newValue;
    std::string displayName;
};

/**
 * @brief Manages undo/redo operations for property changes
 */
class UIPropertyUndoSystem {
public:
    /**
     * @brief Record a property change
     * @param propertyName Name of the changed property
     * @param oldValue Previous value
     * @param newValue New value
     * @param displayName Display name for the undo operation
     */
    void RecordChange(const std::string& propertyName,
                     const PropertyValue& oldValue,
                     const PropertyValue& newValue,
                     const std::string& displayName);

    /**
     * @brief Begin a compound change operation
     * @param name Name of the compound operation
     */
    void BeginCompoundChange(const std::string& name);

    /**
     * @brief End the current compound change operation
     */
    void EndCompoundChange();

    /**
     * @brief Undo the last change
     * @return True if a change was undone
     */
    bool Undo();

    /**
     * @brief Redo the last undone change
     * @return True if a change was redone
     */
    bool Redo();

    /**
     * @brief Check if undo is available
     */
    bool CanUndo() const { return !m_UndoStack.empty(); }

    /**
     * @brief Check if redo is available
     */
    bool CanRedo() const { return !m_RedoStack.empty(); }

    /**
     * @brief Get the name of the next undo operation
     */
    std::string GetUndoName() const;

    /**
     * @brief Get the name of the next redo operation
     */
    std::string GetRedoName() const;

private:
    struct CompoundChange {
        std::string name;
        std::vector<PropertyChange> changes;
    };

    std::vector<CompoundChange> m_UndoStack;
    std::vector<CompoundChange> m_RedoStack;
    std::unique_ptr<CompoundChange> m_CurrentCompound;
};

}} // namespace VGE::Editor 