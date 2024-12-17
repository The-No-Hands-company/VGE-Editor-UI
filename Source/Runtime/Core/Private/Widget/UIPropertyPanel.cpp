#include "Runtime/Core/Public/Widget/UIPropertyPanel.h"
#include "Runtime/Core/Public/Widget/UIPropertyEditor.h"
#include "Runtime/Core/Public/Widget/UIPropertyValidator.h"
#include "Runtime/Core/Public/Widget/UIPropertyUndo.h"
#include "Runtime/Core/Public/Core/Logger.h"

namespace VGE {
namespace UI {

UIPropertyPanel::UIPropertyPanel() {
    m_UndoSystem = std::make_unique<UIPropertyUndo>(this);
}

void UIPropertyPanel::OnLayoutUpdate() {
    UpdateLayout();
}

glm::vec2 UIPropertyPanel::CalculateDesiredSize() {
    return glm::vec2(300.0f, m_ContentHeight);
}

void UIPropertyPanel::ArrangeChildren() {
    // Properties are arranged vertically in the panel
    float yOffset = -m_ScrollOffset;
    for (const auto& [name, metadata] : m_Properties) {
        if (metadata.isHidden) continue;
        
        // Each property gets a row
        glm::vec2 rowPos(0.0f, yOffset);
        glm::vec2 rowSize(GetSize().x, m_Style.rowHeight);
        
        // Update editor position if exists
        if (auto it = m_Editors.find(name); it != m_Editors.end()) {
            glm::vec2 editorPos(m_Style.labelWidth + m_Style.spacing, yOffset);
            glm::vec2 editorSize(rowSize.x - m_Style.labelWidth - m_Style.spacing, m_Style.rowHeight);
            it->second->SetPosition(editorPos);
            it->second->SetSize(editorSize);
        }
        
        yOffset += m_Style.rowHeight + m_Style.spacing;
    }
}

void UIPropertyPanel::OnPaint(UIRenderer* renderer) {
    // Draw background
    renderer->DrawRect(GetPosition(), GetSize(), m_Style.backgroundColor);
    
    // Draw properties
    DrawProperties(renderer);
}

void UIPropertyPanel::OnAnimationUpdate(float deltaTime) {
    // Update animations for property transitions
    for (auto& [name, editor] : m_Editors) {
        if (auto* animatable = dynamic_cast<UIAnimatable*>(editor.get())) {
            animatable->OnAnimationUpdate(deltaTime);
        }
    }
}

void UIPropertyPanel::OnAnimationStateChanged(const std::string& newState) {
    // Propagate animation state changes to editors
    for (auto& [name, editor] : m_Editors) {
        if (auto* animatable = dynamic_cast<UIAnimatable*>(editor.get())) {
            animatable->OnAnimationStateChanged(newState);
        }
    }
}

void UIPropertyPanel::SetTarget(void* target, const std::string& typeName) {
    if (m_Target == target && m_TargetTypeName == typeName) return;
    
    m_Target = target;
    m_TargetTypeName = typeName;
    
    ReflectProperties();
    RefreshProperties();
}

void UIPropertyPanel::ClearTarget() {
    m_Target = nullptr;
    m_TargetTypeName.clear();
    ClearProperties();
}

void UIPropertyPanel::RefreshProperties() {
    for (const auto& [name, metadata] : m_Properties) {
        if (auto it = m_Editors.find(name); it != m_Editors.end()) {
            PropertyValue value = GetPropertyValueFromTarget(name);
            it->second->SetValue(value);
        }
    }
}

void UIPropertyPanel::RegisterProperty(const std::string& name, const PropertyMetadata& metadata) {
    m_Properties[name] = metadata;
    
    // Create appropriate editor
    auto editor = CreateEditorForType(metadata.defaultValue.index());
    if (editor) {
        editor->SetReadOnly(metadata.isReadOnly);
        editor->SetUnits(metadata.units);
        editor->SetPresets(metadata.presets);
        editor->SetOnValueChanged([this, name](const PropertyValue& value) {
            HandlePropertyEdit(name, value);
        });
        m_Editors[name] = editor;
    }
}

void UIPropertyPanel::UnregisterProperty(const std::string& name) {
    m_Properties.erase(name);
    m_Editors.erase(name);
    m_Validators.erase(name);
}

PropertyValue UIPropertyPanel::GetPropertyValue(const std::string& name) const {
    if (auto it = m_Properties.find(name); it != m_Properties.end()) {
        return GetPropertyValueFromTarget(name);
    }
    return PropertyValue();
}

bool UIPropertyPanel::SetPropertyValue(const std::string& name, const PropertyValue& value) {
    if (auto it = m_Properties.find(name); it != m_Properties.end()) {
        if (it->second.isReadOnly) return false;
        
        // Validate value
        if (auto validatorIt = m_Validators.find(name); validatorIt != m_Validators.end()) {
            if (!validatorIt->second->Validate(value)) return false;
        }
        
        // Update target and editor
        if (SetPropertyValueToTarget(name, value)) {
            if (auto editorIt = m_Editors.find(name); editorIt != m_Editors.end()) {
                editorIt->second->SetValue(value);
            }
            return true;
        }
    }
    return false;
}

void UIPropertyPanel::RegisterEditor(const std::string& typeName, std::shared_ptr<UIPropertyEditor> editor) {
    // Store custom editor for type
    m_CustomEditors[typeName] = editor;
}

void UIPropertyPanel::UnregisterEditor(const std::string& typeName) {
    m_CustomEditors.erase(typeName);
}

void UIPropertyPanel::RegisterValidator(const std::string& propertyName, std::shared_ptr<UIPropertyValidator> validator) {
    m_Validators[propertyName] = validator;
}

void UIPropertyPanel::UnregisterValidator(const std::string& propertyName) {
    m_Validators.erase(propertyName);
}

void UIPropertyPanel::Undo() {
    if (m_UndoSystem->CanUndo()) {
        m_UndoSystem->Undo();
        RefreshProperties();
    }
}

void UIPropertyPanel::Redo() {
    if (m_UndoSystem->CanRedo()) {
        m_UndoSystem->Redo();
        RefreshProperties();
    }
}

void UIPropertyPanel::BeginBatchEdit() {
    m_IsBatchEdit = true;
    m_UndoSystem->BeginBatch();
}

void UIPropertyPanel::EndBatchEdit() {
    m_IsBatchEdit = false;
    m_UndoSystem->EndBatch();
}

void UIPropertyPanel::ClearUndoHistory() {
    m_UndoSystem->Clear();
}

void UIPropertyPanel::UpdateLayout() {
    m_ContentHeight = 0.0f;
    for (const auto& [name, metadata] : m_Properties) {
        if (!metadata.isHidden) {
            m_ContentHeight += m_Style.rowHeight + m_Style.spacing;
        }
    }
}

void UIPropertyPanel::DrawProperties(UIRenderer* renderer) {
    float yOffset = -m_ScrollOffset;
    
    for (const auto& [name, metadata] : m_Properties) {
        if (metadata.isHidden) continue;
        
        DrawProperty(renderer, name, metadata);
        yOffset += m_Style.rowHeight + m_Style.spacing;
    }
}

void UIPropertyPanel::DrawProperty(UIRenderer* renderer, const std::string& name, const PropertyMetadata& metadata) {
    glm::vec2 labelPos = GetPosition() + glm::vec2(0.0f, -m_ScrollOffset);
    glm::vec2 labelSize(m_Style.labelWidth, m_Style.rowHeight);
    
    // Draw label
    renderer->DrawText(metadata.name, labelPos, m_Style.labelColor, m_Style.fontSize);
    
    // Draw editor
    if (auto it = m_Editors.find(name); it != m_Editors.end()) {
        it->second->OnPaint(renderer);
    }
}

void UIPropertyPanel::HandlePropertyEdit(const std::string& name, const PropertyValue& value) {
    if (auto it = m_Properties.find(name); it != m_Properties.end()) {
        PropertyValue oldValue = GetPropertyValueFromTarget(name);
        
        if (!m_IsBatchEdit) {
            auto command = std::make_shared<UISetPropertyCommand>(name, oldValue, value);
            m_UndoSystem->Execute(command);
        }
        
        if (m_OnPropertyChanged) {
            PropertyChangeEvent event{
                name,
                oldValue,
                value,
                !it->second.isReadOnly
            };
            m_OnPropertyChanged(event);
        }
    }
}

std::shared_ptr<UIPropertyEditor> UIPropertyPanel::CreateEditorForType(size_t typeIndex) {
    switch (typeIndex) {
        case 0: return std::make_shared<UIBooleanEditor>();
        case 1: return std::make_shared<UINumberEditor>();
        case 2: return std::make_shared<UINumberEditor>();
        case 3: return std::make_shared<UIStringEditor>();
        case 4: return std::make_shared<UIVectorEditor>();
        case 5: return std::make_shared<UIVectorEditor>();
        case 6: return std::make_shared<UIVectorEditor>();
        case 7: return std::make_shared<UICollectionEditor>();
        default: return nullptr;
    }
}

}} // namespace VGE::UI 