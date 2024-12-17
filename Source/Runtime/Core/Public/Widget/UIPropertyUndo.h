#pragma once

#include "Runtime/Core/Public/Core/UITypes.h"
#include <string>
#include <vector>
#include <memory>
#include <deque>

namespace VGE {
namespace UI {

// Forward declarations
class UIPropertyPanel;

// Base class for property commands
class UI_API UIPropertyCommand {
public:
    virtual ~UIPropertyCommand() = default;
    
    virtual void Execute() = 0;
    virtual void Undo() = 0;
    virtual void Redo() = 0;
    virtual std::string GetDescription() const = 0;
    
protected:
    friend class UIPropertyUndo;
    UIPropertyPanel* m_Panel = nullptr;
};

// Standard property commands
class UI_API UISetPropertyCommand : public UIPropertyCommand {
public:
    UISetPropertyCommand(const std::string& propertyName, const PropertyValue& oldValue, const PropertyValue& newValue);
    
    void Execute() override;
    void Undo() override;
    void Redo() override;
    std::string GetDescription() const override;
    
private:
    std::string m_PropertyName;
    PropertyValue m_OldValue;
    PropertyValue m_NewValue;
};

class UI_API UIBatchPropertyCommand : public UIPropertyCommand {
public:
    void AddCommand(std::shared_ptr<UIPropertyCommand> command);
    void ClearCommands();
    
    void Execute() override;
    void Undo() override;
    void Redo() override;
    std::string GetDescription() const override;
    
private:
    std::vector<std::shared_ptr<UIPropertyCommand>> m_Commands;
};

// Undo/Redo system
class UI_API UIPropertyUndo {
public:
    UIPropertyUndo(UIPropertyPanel* panel);
    
    // Command management
    void Execute(std::shared_ptr<UIPropertyCommand> command);
    void BeginBatch();
    void EndBatch();
    void Clear();
    
    // Undo/Redo operations
    bool CanUndo() const;
    bool CanRedo() const;
    void Undo();
    void Redo();
    
    // State access
    std::string GetUndoDescription() const;
    std::string GetRedoDescription() const;
    size_t GetUndoCount() const;
    size_t GetRedoCount() const;
    
    // Settings
    void SetMaxUndoLevels(size_t levels);
    size_t GetMaxUndoLevels() const;
    
private:
    void PushCommand(std::shared_ptr<UIPropertyCommand> command);
    void TrimHistory();
    
private:
    UIPropertyPanel* m_Panel;
    std::deque<std::shared_ptr<UIPropertyCommand>> m_UndoStack;
    std::deque<std::shared_ptr<UIPropertyCommand>> m_RedoStack;
    std::shared_ptr<UIBatchPropertyCommand> m_CurrentBatch;
    size_t m_MaxUndoLevels = 100;
    bool m_IsBatchActive = false;
};

}} // namespace VGE::UI 