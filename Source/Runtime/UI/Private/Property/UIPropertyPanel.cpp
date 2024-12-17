#include "Property/UIPropertyPanel.h"
#include "Property/UIPropertyReflection.h"
#include "Theme/UIStyle.h"
#include <algorithm>

namespace VGE {
namespace UI {

UIPropertyPanel::UIPropertyPanel()
    : m_Target(nullptr)
    , m_UndoSystem(std::make_unique<UIPropertyUndoSystem>())
{
}

UIPropertyPanel::~UIPropertyPanel() = default;

void UIPropertyPanel::SetTarget(void* target, const std::string& typeName) {
    m_Target = target;
    m_TypeDescriptor = TypeRegistry::Get().GetType(std::type_index(typeid(*static_cast<const void*>(target))));
    RefreshProperties();
}

void UIPropertyPanel::ClearTarget() {
    m_Target = nullptr;
    m_TypeDescriptor.reset();
    m_Categories.clear();
}

std::any UIPropertyPanel::GetPropertyValue(const std::string& name) const {
    if (!m_Target || !m_TypeDescriptor) return std::any();
    
    auto property = m_TypeDescriptor->GetProperty(name);
    if (!property) return std::any();
    
    return property->GetValue(m_Target);
}

bool UIPropertyPanel::SetPropertyValue(const std::string& name, const std::any& value) {
    if (!m_Target || !m_TypeDescriptor) return false;
    
    auto property = m_TypeDescriptor->GetProperty(name);
    if (!property) return false;
    
    // Check custom validator
    auto validatorIt = m_Validators.find(name);
    if (validatorIt != m_Validators.end() && !validatorIt->second->Validate(value)) {
        return false;
    }
    
    // Store old value for undo
    auto oldValue = property->GetValue(m_Target);
    
    // Set new value
    property->SetValue(m_Target, value);
    
    // Add to undo system
    if (!m_BatchEditInProgress) {
        m_UndoSystem->AddOperation(std::make_unique<PropertyChangeOperation>(
            m_Target,
            property,
            oldValue,
            value
        ));
    }
    
    return true;
}

void UIPropertyPanel::SetPropertyAttributes(const std::string& name, const PropertyAttributes& attributes) {
    m_CustomAttributes[name] = attributes;
}

void UIPropertyPanel::SetCustomEditor(const std::string& name, std::shared_ptr<UIPropertyEditor> editor) {
    m_CustomEditors[name] = editor;
}

void UIPropertyPanel::SetValidator(const std::string& name, std::shared_ptr<UIPropertyValidator> validator) {
    m_Validators[name] = validator;
}

void UIPropertyPanel::SetCategory(const std::string& name, const std::string& category) {
    if (auto property = m_TypeDescriptor->GetProperty(name)) {
        m_Categories[category].properties.push_back(name);
    }
}

void UIPropertyPanel::SetCategoryExpanded(const std::string& category, bool expanded) {
    auto it = m_Categories.find(category);
    if (it != m_Categories.end()) {
        it->second.expanded = expanded;
    }
}

bool UIPropertyPanel::IsCategoryExpanded(const std::string& category) const {
    auto it = m_Categories.find(category);
    return it != m_Categories.end() ? it->second.expanded : true;
}

void UIPropertyPanel::SetSearchText(const std::string& text) {
    m_SearchText = text;
}

void UIPropertyPanel::SetFilter(std::function<bool(const std::string&, const PropertyDescriptor&)> filter) {
    m_Filter = filter;
}

void UIPropertyPanel::Undo() {
    m_UndoSystem->Undo();
}

void UIPropertyPanel::Redo() {
    m_UndoSystem->Redo();
}

void UIPropertyPanel::BeginBatchEdit() {
    m_BatchEditInProgress = true;
    m_UndoSystem->BeginBatch();
}

void UIPropertyPanel::EndBatchEdit() {
    m_BatchEditInProgress = false;
    m_UndoSystem->EndBatch();
}

void UIPropertyPanel::Update(float deltaTime) {
    // Handle keyboard shortcuts
    if (IsKeyPressed(Key::Z, ModifierKeys::Control)) {
        Undo();
    }
    else if (IsKeyPressed(Key::Y, ModifierKeys::Control)) {
        Redo();
    }
}

void UIPropertyPanel::Render() {
    if (!m_Target || !m_TypeDescriptor) return;
    
    // Render search bar
    RenderSearchBar();
    
    // Render categories
    for (const auto& [category, info] : m_Categories) {
        RenderCategory(category);
    }
}

void UIPropertyPanel::RefreshProperties() {
    if (!m_Target || !m_TypeDescriptor) return;
    
    m_Categories.clear();
    
    // Group properties by category
    for (const auto& [name, property] : m_TypeDescriptor->GetProperties()) {
        const auto& attributes = property->GetAttributes();
        std::string category = attributes.category.empty() ? "Default" : attributes.category;
        m_Categories[category].properties.push_back(name);
    }
    
    // Sort properties within categories
    for (auto& [category, info] : m_Categories) {
        std::sort(info.properties.begin(), info.properties.end());
    }
}

void UIPropertyPanel::RenderProperty(const std::string& name, const PropertyDescriptor& property) {
    if (!ShouldShowProperty(name, property)) return;
    
    // Get custom attributes if any
    const auto& attributes = property.GetAttributes();
    auto customAttribsIt = m_CustomAttributes.find(name);
    const auto& customAttribs = customAttribsIt != m_CustomAttributes.end() ? customAttribsIt->second : PropertyAttributes{};
    
    // Get current value
    auto value = property.GetValue(m_Target);
    
    // Find custom editor if any
    auto editorIt = m_CustomEditors.find(name);
    if (editorIt != m_CustomEditors.end()) {
        if (editorIt->second->OnGUI(name, value)) {
            SetPropertyValue(name, value);
        }
        return;
    }
    
    // Use default editor based on type
    auto type = property.GetType();
    if (type == typeid(bool)) {
        bool boolValue = std::any_cast<bool>(value);
        if (Checkbox(name.c_str(), &boolValue)) {
            SetPropertyValue(name, boolValue);
        }
    }
    else if (type == typeid(int)) {
        int intValue = std::any_cast<int>(value);
        if (DragInt(name.c_str(), &intValue)) {
            SetPropertyValue(name, intValue);
        }
    }
    else if (type == typeid(float)) {
        float floatValue = std::any_cast<float>(value);
        if (DragFloat(name.c_str(), &floatValue)) {
            SetPropertyValue(name, floatValue);
        }
    }
    else if (type == typeid(std::string)) {
        std::string stringValue = std::any_cast<std::string>(value);
        if (InputText(name.c_str(), &stringValue)) {
            SetPropertyValue(name, stringValue);
        }
    }
    else if (type == typeid(glm::vec2)) {
        glm::vec2 vec2Value = std::any_cast<glm::vec2>(value);
        if (DragFloat2(name.c_str(), &vec2Value.x)) {
            SetPropertyValue(name, vec2Value);
        }
    }
    else if (type == typeid(glm::vec3)) {
        glm::vec3 vec3Value = std::any_cast<glm::vec3>(value);
        if (DragFloat3(name.c_str(), &vec3Value.x)) {
            SetPropertyValue(name, vec3Value);
        }
    }
    else if (type == typeid(glm::vec4)) {
        glm::vec4 vec4Value = std::any_cast<glm::vec4>(value);
        if (DragFloat4(name.c_str(), &vec4Value.x)) {
            SetPropertyValue(name, vec4Value);
        }
    }
}

void UIPropertyPanel::RenderCategory(const std::string& category) {
    auto it = m_Categories.find(category);
    if (it == m_Categories.end()) return;
    
    if (CollapsingHeader(category.c_str(), &it->second.expanded)) {
        for (const auto& name : it->second.properties) {
            if (auto property = m_TypeDescriptor->GetProperty(name)) {
                RenderProperty(name, *property);
            }
        }
    }
}

bool UIPropertyPanel::ShouldShowProperty(const std::string& name, const PropertyDescriptor& property) const {
    // Check custom filter
    if (m_Filter && !m_Filter(name, property)) return false;
    
    // Check search text
    if (!m_SearchText.empty()) {
        std::string lowerName = name;
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
        std::string lowerSearch = m_SearchText;
        std::transform(lowerSearch.begin(), lowerSearch.end(), lowerSearch.begin(), ::tolower);
        if (lowerName.find(lowerSearch) == std::string::npos) return false;
    }
    
    // Check hidden flag
    const auto& attributes = property.GetAttributes();
    if (attributes.hidden) return false;
    
    return true;
}

}} // namespace VGE::UI 