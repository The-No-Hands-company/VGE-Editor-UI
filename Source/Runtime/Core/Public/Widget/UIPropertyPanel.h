#pragma once

#include "Runtime/Core/Public/Widget/UIWidget.h"
#include "Runtime/Core/Public/Theme/UIAnimatable.h"
#include "Runtime/Core/Public/Core/UITypes.h"
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>
#include <variant>
#include <any>

namespace VGE {
namespace UI {

// Forward declarations
class UIPropertyEditor;
class UIPropertyValidator;
class UIPropertyUndo;

// Property types supported by the panel
using PropertyValue = std::variant<
    bool,               // Boolean
    int32_t,           // Integer
    float,             // Float
    std::string,       // String
    glm::vec2,         // 2D Vector
    glm::vec3,         // 3D Vector
    glm::vec4,         // 4D Vector (also used for colors)
    std::vector<std::any>  // Collection/Array
>;

// Property metadata for reflection
struct PropertyMetadata {
    std::string name;           // Display name
    std::string description;    // Tooltip description
    std::string category;       // Property category for grouping
    bool isReadOnly = false;    // Read-only flag
    bool isHidden = false;      // Hidden flag
    std::string units;         // Units (e.g., "meters", "degrees")
    PropertyValue defaultValue; // Default value
    std::vector<PropertyValue> enumValues;  // Possible values for enums
    std::function<bool(const PropertyValue&)> validator;  // Custom validation
    std::vector<PropertyValue> presets;     // Value presets
};

// Property change event data
struct PropertyChangeEvent {
    std::string propertyName;
    PropertyValue oldValue;
    PropertyValue newValue;
    bool isUndoable = true;
};

class UI_API UIPropertyPanel : public UIWidget, public UIAnimatable {
public:
    UIPropertyPanel();
    virtual ~UIPropertyPanel() = default;

    // UIWidget interface
    void OnLayoutUpdate() override;
    glm::vec2 CalculateDesiredSize() override;
    void ArrangeChildren() override;
    void OnPaint(UIRenderer* renderer) override;

    // UIAnimatable interface
    void OnAnimationUpdate(float deltaTime) override;
    void OnAnimationStateChanged(const std::string& newState) override;

    // Property management
    void SetTarget(void* target, const std::string& typeName);
    void ClearTarget();
    void RefreshProperties();
    
    // Property registration
    void RegisterProperty(const std::string& name, const PropertyMetadata& metadata);
    void UnregisterProperty(const std::string& name);
    
    // Property access
    PropertyValue GetPropertyValue(const std::string& name) const;
    bool SetPropertyValue(const std::string& name, const PropertyValue& value);
    
    // Custom editors
    void RegisterEditor(const std::string& typeName, std::shared_ptr<UIPropertyEditor> editor);
    void UnregisterEditor(const std::string& typeName);
    
    // Validation
    void RegisterValidator(const std::string& propertyName, std::shared_ptr<UIPropertyValidator> validator);
    void UnregisterValidator(const std::string& propertyName);
    
    // Undo/Redo
    void Undo();
    void Redo();
    void BeginBatchEdit();
    void EndBatchEdit();
    void ClearUndoHistory();
    
    // Presets
    void AddPreset(const std::string& propertyName, const PropertyValue& value);
    void RemovePreset(const std::string& propertyName, const PropertyValue& value);
    void ClearPresets(const std::string& propertyName);
    
    // Event callbacks
    using PropertyChangeCallback = std::function<void(const PropertyChangeEvent&)>;
    void SetOnPropertyChanged(PropertyChangeCallback callback) { m_OnPropertyChanged = callback; }

protected:
    // Helper methods
    void UpdateLayout();
    void DrawProperties(UIRenderer* renderer);
    void DrawProperty(UIRenderer* renderer, const std::string& name, const PropertyMetadata& metadata);
    void HandlePropertyEdit(const std::string& name, const PropertyValue& value);
    
    // Property reflection helpers
    void ReflectProperties();
    void ClearProperties();
    PropertyValue GetPropertyValueFromTarget(const std::string& name) const;
    bool SetPropertyValueToTarget(const std::string& name, const PropertyValue& value);

private:
    // Target object
    void* m_Target = nullptr;
    std::string m_TargetTypeName;
    
    // Property data
    std::unordered_map<std::string, PropertyMetadata> m_Properties;
    std::unordered_map<std::string, std::shared_ptr<UIPropertyEditor>> m_Editors;
    std::unordered_map<std::string, std::shared_ptr<UIPropertyValidator>> m_Validators;
    
    // Undo/Redo system
    std::unique_ptr<UIPropertyUndo> m_UndoSystem;
    bool m_IsBatchEdit = false;
    
    // Layout data
    float m_ScrollOffset = 0.0f;
    float m_ContentHeight = 0.0f;
    
    // Callbacks
    PropertyChangeCallback m_OnPropertyChanged;
    
    // Style
    struct Style {
        float rowHeight = 24.0f;
        float labelWidth = 150.0f;
        float spacing = 4.0f;
        glm::vec4 backgroundColor = glm::vec4(0.15f, 0.15f, 0.15f, 1.0f);
        glm::vec4 labelColor = glm::vec4(0.9f, 0.9f, 0.9f, 1.0f);
        glm::vec4 valueColor = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
        glm::vec4 editingColor = glm::vec4(0.2f, 0.4f, 0.8f, 1.0f);
        glm::vec4 invalidColor = glm::vec4(0.8f, 0.2f, 0.2f, 1.0f);
        float fontSize = 12.0f;
    } m_Style;
};

}} // namespace VGE::UI 