#include "U

#include "Runtime/Core/Public/Core/UICore.h"I/Editor/Panels/PropertiesPanel.h"
#include "UI/Framework/Widgets/UIInputWidgets.h"
#include "UI/Framework/Layout/UILayout.h"
#include "UI/Framework/Widgets/UIPropertyGrid.h"
#include "Scene/SceneNode.h"
#include <algorithm>

namespace VGE {
namespace Editor {

PropertiesPanel::PropertiesPanel()
    : EditorPanel("Properties")
    , m_ShowReadOnly(false)
{
    SetSize(glm::vec2(300.0f, 600.0f));
}

void PropertiesPanel::Initialize()
{
    EditorPanel::Initialize();
}

void PropertiesPanel::Update(float deltaTime)
{
    EditorPanel::Update(deltaTime);
    
    // Update property values if target is valid
    if (m_Target)
    {
        for (auto& [name, prop] : m_Properties)
        {
            if (prop.getter)
            {
                prop.value = prop.getter();
            }
        }
    }
}

void PropertiesPanel::SetTarget(std::shared_ptr<Scene::SceneNode> target)
{
    if (m_Target == target)
        return;
        
    m_Target = target;
    ClearProperties();
    
    if (m_Target)
    {
        // Register default properties
        RegisterDefaultProperties();
    }
}

void PropertiesPanel::RegisterProperty(const Property& property)
{
    m_Properties[property.name] = property;
}

void PropertiesPanel::UnregisterProperty(const std::string& name)
{
    m_Properties.erase(name);
}

void PropertiesPanel::ClearProperties()
{
    m_Properties.clear();
}

void PropertiesPanel::SetExpandedCategory(const std::string& category, bool expanded)
{
    m_ExpandedCategories[category] = expanded;
}

bool PropertiesPanel::IsCategoryExpanded(const std::string& category) const
{
    auto it = m_ExpandedCategories.find(category);
    return it != m_ExpandedCategories.end() ? it->second : true;
}

void PropertiesPanel::RenderPanelContent()
{
    auto layout = CreateVerticalLayout();
    
    // No target message
    if (!m_Target)
    {
        CreateText("No object selected")->SetStyle(TextStyle::Disabled);
        return;
    }
    
    // Filter input
    if (auto input = CreateInputField("##Filter", m_Filter))
    {
        // Filter updated automatically
    }
    AddSeparator();
    
    // Show read-only toggle
    if (auto checkbox = CreateCheckbox("Show Read-Only", m_ShowReadOnly))
    {
        m_ShowReadOnly = checkbox->IsChecked();
    }
    AddSeparator();
    
    // Group properties by category
    std::map<std::string, std::vector<std::reference_wrapper<const Property>>> categorizedProps;
    for (const auto& [name, prop] : m_Properties)
    {
        if (!PropertyMatchesFilter(prop))
            continue;
            
        if (!m_ShowReadOnly && prop.readOnly)
            continue;
            
        categorizedProps[prop.category].push_back(std::cref(prop));
    }
    
    // Render properties by category
    for (const auto& [category, props] : categorizedProps)
    {
        bool isExpanded = IsCategoryExpanded(category);
        if (auto header = CreateCollapsibleHeader(category, isExpanded))
        {
            SetExpandedCategory(category, header->IsExpanded());
            
            if (header->IsExpanded())
            {
                auto grid = CreatePropertyGrid();
                
                for (const auto& propRef : props)
                {
                    const auto& prop = propRef.get();
                    RenderProperty(prop);
                }
            }
        }
    }
}

void PropertiesPanel::RenderProperty(const Property& prop)
{
    // Create property row with label
    auto row = CreatePropertyRow(prop.name);
    
    // Add tooltip if available
    if (!prop.tooltip.empty())
    {
        row->SetTooltip(prop.tooltip);
    }
    
    // Disable editing if read-only
    if (prop.readOnly)
    {
        row->SetEnabled(false);
    }
    
    // Render property based on type
    switch (prop.type)
    {
        case PropertyType::Int:
            RenderIntProperty(prop);
            break;
        case PropertyType::Float:
            RenderFloatProperty(prop);
            break;
        case PropertyType::Bool:
            RenderBoolProperty(prop);
            break;
        case PropertyType::String:
            RenderStringProperty(prop);
            break;
        case PropertyType::Vector2:
            RenderVector2Property(prop);
            break;
        case PropertyType::Vector3:
            RenderVector3Property(prop);
            break;
        case PropertyType::Vector4:
            RenderVector4Property(prop);
            break;
        case PropertyType::Color:
            RenderColorProperty(prop);
            break;
        case PropertyType::Enum:
            RenderEnumProperty(prop);
            break;
        case PropertyType::Asset:
            RenderAssetProperty(prop);
            break;
        case PropertyType::Custom:
            if (prop.customRenderer)
            {
                prop.customRenderer();
            }
            break;
    }
}

void PropertiesPanel::RenderIntProperty(const Property& prop)
{
    int value = std::any_cast<int>(prop.value);
    if (auto input = CreateNumberInput(prop.name, value))
    {
        if (prop.setter)
        {
            prop.setter(input->GetValue());
        }
    }
}

void PropertiesPanel::RenderFloatProperty(const Property& prop)
{
    float value = std::any_cast<float>(prop.value);
    if (auto input = CreateNumberInput(prop.name, value, 0.1f))
    {
        if (prop.setter)
        {
            prop.setter(input->GetValue());
        }
    }
}

void PropertiesPanel::RenderBoolProperty(const Property& prop)
{
    bool value = std::any_cast<bool>(prop.value);
    if (auto checkbox = CreateCheckbox(prop.name, value))
    {
        if (prop.setter)
        {
            prop.setter(checkbox->IsChecked());
        }
    }
}

void PropertiesPanel::RenderStringProperty(const Property& prop)
{
    std::string value = std::any_cast<std::string>(prop.value);
    if (auto input = CreateInputField(prop.name, value))
    {
        if (prop.setter)
        {
            prop.setter(input->GetValue());
        }
    }
}

void PropertiesPanel::RenderVector2Property(const Property& prop)
{
    glm::vec2 value = std::any_cast<glm::vec2>(prop.value);
    if (auto input = CreateVectorInput(prop.name, value, 2))
    {
        if (prop.setter)
        {
            prop.setter(input->GetValue());
        }
    }
}

void PropertiesPanel::RenderVector3Property(const Property& prop)
{
    glm::vec3 value = std::any_cast<glm::vec3>(prop.value);
    if (auto input = CreateVectorInput(prop.name, value, 3))
    {
        if (prop.setter)
        {
            prop.setter(input->GetValue());
        }
    }
}

void PropertiesPanel::RenderVector4Property(const Property& prop)
{
    glm::vec4 value = std::any_cast<glm::vec4>(prop.value);
    if (auto input = CreateVectorInput(prop.name, value, 4))
    {
        if (prop.setter)
        {
            prop.setter(input->GetValue());
        }
    }
}

void PropertiesPanel::RenderColorProperty(const Property& prop)
{
    glm::vec4 value = std::any_cast<glm::vec4>(prop.value);
    if (auto picker = CreateColorPicker(prop.name, value))
    {
        if (prop.setter)
        {
            prop.setter(picker->GetColor());
        }
    }
}

void PropertiesPanel::RenderEnumProperty(const Property& prop)
{
    int value = std::any_cast<int>(prop.value);
    if (auto combo = CreateComboBox(prop.name, prop.enumValues, value))
    {
        if (prop.setter)
        {
            prop.setter(combo->GetSelectedIndex());
        }
    }
}

void PropertiesPanel::RenderAssetProperty(const Property& prop)
{
    std::string value = std::any_cast<std::string>(prop.value);
    if (auto asset = CreateAssetField(prop.name, value))
    {
        if (prop.setter)
        {
            prop.setter(asset->GetPath());
        }
    }
}

bool PropertiesPanel::PropertyMatchesFilter(const Property& prop) const
{
    if (m_Filter.empty())
        return true;
        
    // Case-insensitive search in name and category
    std::string lowerFilter = m_Filter;
    std::transform(lowerFilter.begin(), lowerFilter.end(), lowerFilter.begin(), ::tolower);
    
    std::string lowerName = prop.name;
    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
    
    std::string lowerCategory = prop.category;
    std::transform(lowerCategory.begin(), lowerCategory.end(), lowerCategory.begin(), ::tolower);
    
    return lowerName.find(lowerFilter) != std::string::npos ||
           lowerCategory.find(lowerFilter) != std::string::npos;
}

} // namespace Editor
} // namespace VGE 
} // namespace VGE 


