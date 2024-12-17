#include "Runtime/Core/Public/Widget/UIPropertyEditor.h"
#include "Runtime/Core/Public/Widget/UIPropertyPanel.h"
#include "Runtime/Core/Public/Core/Logger.h"

namespace VGE {
namespace UI {

// Base UIPropertyEditor implementation
UIPropertyEditor::UIPropertyEditor() {}

bool UIPropertyEditor::Validate(const PropertyValue& value) const {
    return true;
}

std::string UIPropertyEditor::GetValidationMessage() const {
    return m_ValidationMessage;
}

void UIPropertyEditor::SetReadOnly(bool readOnly) {
    m_IsReadOnly = readOnly;
}

void UIPropertyEditor::SetUnits(const std::string& units) {
    m_Units = units;
}

void UIPropertyEditor::SetPresets(const std::vector<PropertyValue>& presets) {
    m_Presets = presets;
}

void UIPropertyEditor::NotifyValueChanged(const PropertyValue& newValue) {
    if (m_OnValueChanged) {
        m_OnValueChanged(newValue);
    }
}

bool UIPropertyEditor::ValidateValue(const PropertyValue& value) const {
    return Validate(value);
}

UIPropertyPanel* UIPropertyEditor::GetPropertyPanel() const {
    return dynamic_cast<UIPropertyPanel*>(GetParent());
}

// Boolean Editor Implementation
void UIBooleanEditor::SetValue(const PropertyValue& value) {
    if (auto* boolVal = std::get_if<bool>(&value)) {
        m_Value = *boolVal;
    }
}

PropertyValue UIBooleanEditor::GetValue() const {
    return PropertyValue(m_Value);
}

bool UIBooleanEditor::IsEditing() const {
    return m_IsEditing;
}

bool UIBooleanEditor::SupportsType(const std::type_info& type) const {
    return type == typeid(bool);
}

void UIBooleanEditor::BeginEdit() {
    if (m_IsReadOnly) return;
    m_IsEditing = true;
}

void UIBooleanEditor::EndEdit(bool accept) {
    if (!m_IsEditing) return;
    m_IsEditing = false;
    
    if (accept) {
        NotifyValueChanged(GetValue());
    }
}

void UIBooleanEditor::Reset() {
    m_IsEditing = false;
}

void UIBooleanEditor::OnPaint(UIRenderer* renderer) {
    glm::vec2 checkboxSize(16.0f, 16.0f);
    glm::vec2 checkboxPos = GetPosition() + glm::vec2(4.0f, (GetSize().y - checkboxSize.y) * 0.5f);
    
    // Draw checkbox background
    glm::vec4 bgColor = m_IsReadOnly ? glm::vec4(0.3f) : glm::vec4(0.8f);
    renderer->DrawRect(checkboxPos, checkboxSize, bgColor);
    
    // Draw check mark if checked
    if (m_Value) {
        glm::vec4 checkColor(0.2f, 0.6f, 1.0f, 1.0f);
        renderer->DrawCheckMark(checkboxPos, checkboxSize, checkColor);
    }
}

// Number Editor Implementation
void UINumberEditor::SetValue(const PropertyValue& value) {
    if (auto* intVal = std::get_if<int32_t>(&value)) {
        m_Value = static_cast<double>(*intVal);
    } else if (auto* floatVal = std::get_if<float>(&value)) {
        m_Value = static_cast<double>(*floatVal);
    }
    m_EditText = FormatValue();
}

PropertyValue UINumberEditor::GetValue() const {
    return PropertyValue(static_cast<float>(m_Value));
}

bool UINumberEditor::IsEditing() const {
    return m_IsEditing;
}

bool UINumberEditor::SupportsType(const std::type_info& type) const {
    return type == typeid(int32_t) || type == typeid(float);
}

void UINumberEditor::BeginEdit() {
    if (m_IsReadOnly) return;
    m_IsEditing = true;
    m_EditText = FormatValue();
}

void UINumberEditor::EndEdit(bool accept) {
    if (!m_IsEditing) return;
    m_IsEditing = false;
    
    if (accept) {
        // Parse edit text
        try {
            m_Value = std::stod(m_EditText);
            m_Value = std::clamp(m_Value, m_MinValue, m_MaxValue);
            NotifyValueChanged(GetValue());
        } catch (...) {
            m_EditText = FormatValue();
        }
    } else {
        m_EditText = FormatValue();
    }
}

void UINumberEditor::Reset() {
    m_IsEditing = false;
    m_EditText = FormatValue();
}

void UINumberEditor::SetRange(double min, double max) {
    m_MinValue = min;
    m_MaxValue = max;
    m_Value = std::clamp(m_Value, min, max);
}

void UINumberEditor::SetStep(double step) {
    m_Step = step;
}

void UINumberEditor::SetPrecision(int precision) {
    m_Precision = precision;
    m_EditText = FormatValue();
}

void UINumberEditor::SetDisplayFormat(const std::string& format) {
    m_Format = format;
    m_EditText = FormatValue();
}

std::string UINumberEditor::FormatValue() const {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), m_Format.c_str(), m_Value);
    return std::string(buffer);
}

void UINumberEditor::OnPaint(UIRenderer* renderer) {
    glm::vec4 textColor = m_IsReadOnly ? glm::vec4(0.5f) : glm::vec4(0.9f);
    
    // Draw text field
    renderer->DrawRect(GetPosition(), GetSize(), glm::vec4(0.2f));
    renderer->DrawText(m_EditText, GetPosition() + glm::vec2(4.0f, 2.0f), textColor);
    
    // Draw units if any
    if (!m_Units.empty()) {
        float unitsWidth = renderer->GetTextWidth(m_Units);
        glm::vec2 unitsPos = GetPosition() + glm::vec2(GetSize().x - unitsWidth - 4.0f, 2.0f);
        renderer->DrawText(m_Units, unitsPos, glm::vec4(0.6f));
    }
}

// String Editor Implementation
void UIStringEditor::SetValue(const PropertyValue& value) {
    if (auto* strVal = std::get_if<std::string>(&value)) {
        m_Value = *strVal;
        m_EditText = m_Value;
    }
}

PropertyValue UIStringEditor::GetValue() const {
    return PropertyValue(m_Value);
}

bool UIStringEditor::IsEditing() const {
    return m_IsEditing;
}

bool UIStringEditor::SupportsType(const std::type_info& type) const {
    return type == typeid(std::string);
}

void UIStringEditor::BeginEdit() {
    if (m_IsReadOnly) return;
    m_IsEditing = true;
    m_EditText = m_Value;
}

void UIStringEditor::EndEdit(bool accept) {
    if (!m_IsEditing) return;
    m_IsEditing = false;
    
    if (accept) {
        m_Value = m_EditText;
        NotifyValueChanged(GetValue());
    } else {
        m_EditText = m_Value;
    }
}

void UIStringEditor::Reset() {
    m_IsEditing = false;
    m_EditText = m_Value;
}

void UIStringEditor::SetMaxLength(size_t maxLength) {
    m_MaxLength = maxLength;
    if (m_Value.length() > maxLength) {
        m_Value = m_Value.substr(0, maxLength);
        m_EditText = m_Value;
    }
}

void UIStringEditor::SetPasswordMode(bool enabled) {
    m_IsPassword = enabled;
}

void UIStringEditor::SetMultiline(bool enabled) {
    m_IsMultiline = enabled;
}

void UIStringEditor::OnPaint(UIRenderer* renderer) {
    glm::vec4 textColor = m_IsReadOnly ? glm::vec4(0.5f) : glm::vec4(0.9f);
    
    // Draw text field
    renderer->DrawRect(GetPosition(), GetSize(), glm::vec4(0.2f));
    
    // Draw text or password dots
    std::string displayText = m_IsPassword ? 
        std::string(m_EditText.length(), '*') : m_EditText;
    
    renderer->DrawText(displayText, GetPosition() + glm::vec2(4.0f, 2.0f), textColor);
}

// Vector Editor Implementation
void UIVectorEditor::SetValue(const PropertyValue& value) {
    if (auto* vec2Val = std::get_if<glm::vec2>(&value)) {
        m_Value = glm::vec4(*vec2Val, 0.0f, 0.0f);
        m_Components = 2;
    } else if (auto* vec3Val = std::get_if<glm::vec3>(&value)) {
        m_Value = glm::vec4(*vec3Val, 0.0f);
        m_Components = 3;
    } else if (auto* vec4Val = std::get_if<glm::vec4>(&value)) {
        m_Value = *vec4Val;
        m_Components = 4;
    }
}

PropertyValue UIVectorEditor::GetValue() const {
    switch (m_Components) {
        case 2: return PropertyValue(glm::vec2(m_Value));
        case 3: return PropertyValue(glm::vec3(m_Value));
        case 4: return PropertyValue(m_Value);
        default: return PropertyValue(glm::vec4(0.0f));
    }
}

bool UIVectorEditor::IsEditing() const {
    return m_IsEditing;
}

bool UIVectorEditor::SupportsType(const std::type_info& type) const {
    return type == typeid(glm::vec2) || 
           type == typeid(glm::vec3) || 
           type == typeid(glm::vec4);
}

void UIVectorEditor::BeginEdit() {
    if (m_IsReadOnly) return;
    m_IsEditing = true;
}

void UIVectorEditor::EndEdit(bool accept) {
    if (!m_IsEditing) return;
    m_IsEditing = false;
    
    if (accept) {
        NotifyValueChanged(GetValue());
    }
}

void UIVectorEditor::Reset() {
    m_IsEditing = false;
    m_EditingComponent = -1;
}

void UIVectorEditor::SetComponentLabels(const std::vector<std::string>& labels) {
    m_Labels = labels;
}

void UIVectorEditor::SetComponentRanges(const std::vector<std::pair<double, double>>& ranges) {
    m_Ranges = ranges;
}

void UIVectorEditor::OnPaint(UIRenderer* renderer) {
    float componentWidth = (GetSize().x - (m_Components - 1) * 4.0f) / m_Components;
    
    for (int i = 0; i < m_Components; ++i) {
        glm::vec2 compPos = GetPosition() + glm::vec2(i * (componentWidth + 4.0f), 0.0f);
        glm::vec2 compSize(componentWidth, GetSize().y);
        
        // Draw component background
        glm::vec4 bgColor = (i == m_EditingComponent) ? glm::vec4(0.3f) : glm::vec4(0.2f);
        renderer->DrawRect(compPos, compSize, bgColor);
        
        // Draw label if available
        if (i < m_Labels.size()) {
            renderer->DrawText(m_Labels[i], compPos + glm::vec2(2.0f, 2.0f), glm::vec4(0.6f));
        }
        
        // Draw value
        char buffer[32];
        snprintf(buffer, sizeof(buffer), "%.2f", m_Value[i]);
        renderer->DrawText(buffer, compPos + glm::vec2(4.0f, 20.0f), glm::vec4(0.9f));
    }
}

// Collection Editor Implementation
void UICollectionEditor::SetValue(const PropertyValue& value) {
    if (auto* collection = std::get_if<std::vector<std::any>>(&value)) {
        m_Value = *collection;
    }
}

PropertyValue UICollectionEditor::GetValue() const {
    return PropertyValue(m_Value);
}

bool UICollectionEditor::IsEditing() const {
    return m_IsEditing;
}

bool UICollectionEditor::SupportsType(const std::type_info& type) const {
    return type == typeid(std::vector<std::any>);
}

void UICollectionEditor::BeginEdit() {
    if (m_IsReadOnly) return;
    m_IsEditing = true;
}

void UICollectionEditor::EndEdit(bool accept) {
    if (!m_IsEditing) return;
    m_IsEditing = false;
    
    if (accept) {
        NotifyValueChanged(GetValue());
    }
}

void UICollectionEditor::Reset() {
    m_IsEditing = false;
    m_EditingIndex = -1;
}

void UICollectionEditor::SetElementEditor(std::shared_ptr<UIPropertyEditor> editor) {
    m_ElementEditor = editor;
}

void UICollectionEditor::SetMaxElements(size_t maxElements) {
    m_MaxElements = maxElements;
    if (m_Value.size() > maxElements) {
        m_Value.resize(maxElements);
    }
}

void UICollectionEditor::OnPaint(UIRenderer* renderer) {
    // Draw collection header
    renderer->DrawRect(GetPosition(), glm::vec2(GetSize().x, 24.0f), glm::vec4(0.25f));
    renderer->DrawText(std::to_string(m_Value.size()) + " items", 
                      GetPosition() + glm::vec2(4.0f, 4.0f), 
                      glm::vec4(0.9f));
    
    // Draw add/remove buttons if not read-only
    if (!m_IsReadOnly) {
        glm::vec2 buttonSize(20.0f, 20.0f);
        glm::vec2 addPos = GetPosition() + glm::vec2(GetSize().x - 48.0f, 2.0f);
        glm::vec2 removePos = GetPosition() + glm::vec2(GetSize().x - 24.0f, 2.0f);
        
        renderer->DrawRect(addPos, buttonSize, glm::vec4(0.3f, 0.6f, 0.3f, 1.0f));
        renderer->DrawRect(removePos, buttonSize, glm::vec4(0.6f, 0.3f, 0.3f, 1.0f));
        renderer->DrawText("+", addPos + glm::vec2(6.0f, 2.0f), glm::vec4(1.0f));
        renderer->DrawText("-", removePos + glm::vec2(7.0f, 2.0f), glm::vec4(1.0f));
    }
    
    // Draw elements if editor is available
    if (m_ElementEditor) {
        float yOffset = 28.0f;
        for (size_t i = 0; i < m_Value.size(); ++i) {
            glm::vec2 elementPos = GetPosition() + glm::vec2(0.0f, yOffset);
            glm::vec2 elementSize(GetSize().x, 24.0f);
            
            // Draw element background
            glm::vec4 bgColor = (i == m_EditingIndex) ? glm::vec4(0.3f) : glm::vec4(0.2f);
            renderer->DrawRect(elementPos, elementSize, bgColor);
            
            // Update element editor
            m_ElementEditor->SetPosition(elementPos + glm::vec2(4.0f, 0.0f));
            m_ElementEditor->SetSize(elementSize - glm::vec2(8.0f, 0.0f));
            m_ElementEditor->OnPaint(renderer);
            
            yOffset += 28.0f;
        }
    }
}

}} // namespace VGE::UI