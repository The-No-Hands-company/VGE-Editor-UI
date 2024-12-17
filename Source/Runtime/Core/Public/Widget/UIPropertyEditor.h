#pragma once

#include "Runtime/Core/Public/Widget/UIWidget.h"
#include "Runtime/Core/Public/Core/UITypes.h"
#include <string>
#include <functional>
#include <memory>

namespace VGE {
namespace UI {

class UIPropertyPanel;

// Base class for all property editors
class UI_API UIPropertyEditor : public UIWidget {
public:
    UIPropertyEditor();
    virtual ~UIPropertyEditor() = default;

    // Editor interface
    virtual void SetValue(const PropertyValue& value) = 0;
    virtual PropertyValue GetValue() const = 0;
    virtual bool IsEditing() const = 0;
    virtual bool SupportsType(const std::type_info& type) const = 0;
    
    // Editor state
    virtual void BeginEdit() = 0;
    virtual void EndEdit(bool accept) = 0;
    virtual void Reset() = 0;
    
    // Validation
    virtual bool Validate(const PropertyValue& value) const;
    virtual std::string GetValidationMessage() const;
    
    // Customization
    virtual void SetReadOnly(bool readOnly);
    virtual void SetUnits(const std::string& units);
    virtual void SetPresets(const std::vector<PropertyValue>& presets);
    
    // Event callbacks
    using ValueChangeCallback = std::function<void(const PropertyValue&)>;
    void SetOnValueChanged(ValueChangeCallback callback) { m_OnValueChanged = callback; }

protected:
    // Helper methods
    void NotifyValueChanged(const PropertyValue& newValue);
    bool ValidateValue(const PropertyValue& value) const;
    
    // Access to parent panel
    UIPropertyPanel* GetPropertyPanel() const;

protected:
    bool m_IsReadOnly = false;
    std::string m_Units;
    std::vector<PropertyValue> m_Presets;
    ValueChangeCallback m_OnValueChanged;
    std::string m_ValidationMessage;
};

// Standard editor implementations
class UI_API UIBooleanEditor : public UIPropertyEditor {
public:
    void SetValue(const PropertyValue& value) override;
    PropertyValue GetValue() const override;
    bool IsEditing() const override;
    bool SupportsType(const std::type_info& type) const override;
    
    void BeginEdit() override;
    void EndEdit(bool accept) override;
    void Reset() override;
    
    void OnPaint(UIRenderer* renderer) override;
    
private:
    bool m_Value = false;
    bool m_IsEditing = false;
};

class UI_API UINumberEditor : public UIPropertyEditor {
public:
    void SetValue(const PropertyValue& value) override;
    PropertyValue GetValue() const override;
    bool IsEditing() const override;
    bool SupportsType(const std::type_info& type) const override;
    
    void BeginEdit() override;
    void EndEdit(bool accept) override;
    void Reset() override;
    
    void OnPaint(UIRenderer* renderer) override;
    
    // Number-specific settings
    void SetRange(double min, double max);
    void SetStep(double step);
    void SetPrecision(int precision);
    void SetDisplayFormat(const std::string& format);
    
private:
    double m_Value = 0.0;
    double m_MinValue = -std::numeric_limits<double>::max();
    double m_MaxValue = std::numeric_limits<double>::max();
    double m_Step = 1.0;
    int m_Precision = 3;
    std::string m_Format = "%.3f";
    bool m_IsEditing = false;
    std::string m_EditText;
};

class UI_API UIStringEditor : public UIPropertyEditor {
public:
    void SetValue(const PropertyValue& value) override;
    PropertyValue GetValue() const override;
    bool IsEditing() const override;
    bool SupportsType(const std::type_info& type) const override;
    
    void BeginEdit() override;
    void EndEdit(bool accept) override;
    void Reset() override;
    
    void OnPaint(UIRenderer* renderer) override;
    
    // String-specific settings
    void SetMaxLength(size_t maxLength);
    void SetPasswordMode(bool enabled);
    void SetMultiline(bool enabled);
    
private:
    std::string m_Value;
    std::string m_EditText;
    size_t m_MaxLength = std::numeric_limits<size_t>::max();
    bool m_IsPassword = false;
    bool m_IsMultiline = false;
    bool m_IsEditing = false;
};

class UI_API UIVectorEditor : public UIPropertyEditor {
public:
    void SetValue(const PropertyValue& value) override;
    PropertyValue GetValue() const override;
    bool IsEditing() const override;
    bool SupportsType(const std::type_info& type) const override;
    
    void BeginEdit() override;
    void EndEdit(bool accept) override;
    void Reset() override;
    
    void OnPaint(UIRenderer* renderer) override;
    
    // Vector-specific settings
    void SetComponentLabels(const std::vector<std::string>& labels);
    void SetComponentRanges(const std::vector<std::pair<double, double>>& ranges);
    
private:
    glm::vec4 m_Value;
    int m_Components = 4;
    std::vector<std::string> m_Labels;
    std::vector<std::pair<double, double>> m_Ranges;
    bool m_IsEditing = false;
    int m_EditingComponent = -1;
};

class UI_API UIEnumEditor : public UIPropertyEditor {
public:
    void SetValue(const PropertyValue& value) override;
    PropertyValue GetValue() const override;
    bool IsEditing() const override;
    bool SupportsType(const std::type_info& type) const override;
    
    void BeginEdit() override;
    void EndEdit(bool accept) override;
    void Reset() override;
    
    void OnPaint(UIRenderer* renderer) override;
    
    // Enum-specific settings
    void SetEnumValues(const std::vector<std::pair<std::string, PropertyValue>>& values);
    
private:
    PropertyValue m_Value;
    std::vector<std::pair<std::string, PropertyValue>> m_EnumValues;
    bool m_IsEditing = false;
    int m_SelectedIndex = -1;
};

class UI_API UICollectionEditor : public UIPropertyEditor {
public:
    void SetValue(const PropertyValue& value) override;
    PropertyValue GetValue() const override;
    bool IsEditing() const override;
    bool SupportsType(const std::type_info& type) const override;
    
    void BeginEdit() override;
    void EndEdit(bool accept) override;
    void Reset() override;
    
    void OnPaint(UIRenderer* renderer) override;
    
    // Collection-specific settings
    void SetElementEditor(std::shared_ptr<UIPropertyEditor> editor);
    void SetMaxElements(size_t maxElements);
    
private:
    std::vector<std::any> m_Value;
    std::shared_ptr<UIPropertyEditor> m_ElementEditor;
    size_t m_MaxElements = std::numeric_limits<size_t>::max();
    bool m_IsEditing = false;
    int m_EditingIndex = -1;
};

}} // namespace VGE::UI 