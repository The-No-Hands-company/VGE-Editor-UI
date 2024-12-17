#include "Runtime/UI/Public/Property/UIPropertyGrid.h"
#include "Runtime/Core/Public/Renderer/UIRenderer.h"
#include "Runtime/Core/Public/Core/Logger.h"

namespace VGE {
namespace UI {

UIPropertyGrid::UIPropertyGrid() {
    // Set default size
    SetSize(glm::vec2(300.0f, 500.0f));
}

void UIPropertyGrid::Initialize() {
    UIWidget::Initialize();
}

void UIPropertyGrid::Update(float deltaTime) {
    UIWidget::Update(deltaTime);
}

void UIPropertyGrid::Render() {
    UIWidget::Render();

    auto renderer = UIRenderer::Get();
    if (!renderer) return;

    // Draw background
    renderer->DrawRect(m_Position, m_Size, glm::vec4(0.15f, 0.15f, 0.15f, 1.0f));

    // Draw search bar if enabled
    float yOffset = 0.0f;
    if (m_ShowSearch) {
        const float searchHeight = 25.0f;
        glm::vec2 searchPos = m_Position;
        glm::vec2 searchSize(m_Size.x, searchHeight);
        
        // Draw search background
        renderer->DrawRect(searchPos, searchSize, glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));
        
        // TODO: Implement actual search input
        yOffset += searchHeight;
    }

    // Calculate content area
    glm::vec2 contentPos = m_Position + glm::vec2(0.0f, yOffset);
    glm::vec2 contentSize = m_Size - glm::vec2(0.0f, yOffset);

    // Render categories or flat list
    if (m_ShowCategories) {
        float categoryY = contentPos.y;
        for (auto& category : m_Categories) {
            if (!category.properties.empty()) {
                RenderCategory(category);
            }
        }
    }
    else {
        // Render all properties in a flat list
        float propertyY = contentPos.y;
        for (const auto& category : m_Categories) {
            for (const auto& property : category.properties) {
                if (MatchesSearch(property)) {
                    RenderProperty(property);
                }
            }
        }
    }
}

void UIPropertyGrid::AddProperty(const Property& property) {
    auto& category = GetOrCreateCategory(property.category);
    category.properties.push_back(property);
}

void UIPropertyGrid::RemoveProperty(const std::string& name) {
    for (auto& category : m_Categories) {
        auto it = std::find_if(category.properties.begin(), category.properties.end(),
            [&name](const Property& prop) { return prop.name == name; });
        
        if (it != category.properties.end()) {
            category.properties.erase(it);
            return;
        }
    }
}

const PropertyValue* UIPropertyGrid::GetValue(const std::string& name) const {
    for (const auto& category : m_Categories) {
        for (const auto& property : category.properties) {
            if (property.name == name) {
                return &property.value;
            }
        }
    }
    return nullptr;
}

bool UIPropertyGrid::SetValue(const std::string& name, const PropertyValue& value) {
    for (auto& category : m_Categories) {
        for (auto& property : category.properties) {
            if (property.name == name) {
                property.value = value;
                if (property.metadata.onChange) {
                    property.metadata.onChange(value);
                }
                return true;
            }
        }
    }
    return false;
}

void UIPropertyGrid::Clear() {
    m_Categories.clear();
}

void UIPropertyGrid::RenderProperty(const Property& property) {
    auto renderer = UIRenderer::Get();
    if (!renderer) return;

    const float propertyHeight = 25.0f;
    const float labelWidth = m_Size.x * 0.4f;
    const float padding = 5.0f;

    // Draw property background
    glm::vec2 propertyPos = m_Position;
    glm::vec2 propertySize(m_Size.x, propertyHeight);
    renderer->DrawRect(propertyPos, propertySize, glm::vec4(0.18f, 0.18f, 0.18f, 1.0f));

    // Draw property name
    glm::vec2 labelPos = propertyPos + glm::vec2(padding, 0.0f);
    renderer->DrawText(property.displayName.empty() ? property.name : property.displayName,
                      labelPos, glm::vec4(1.0f));

    // Draw property value using appropriate editor
    glm::vec2 valuePos = propertyPos + glm::vec2(labelWidth + padding, 0.0f);
    glm::vec2 valueSize(m_Size.x - labelWidth - padding * 2, propertyHeight);

    auto editor = UIPropertyEditorFactory::GetEditor(property.type);
    if (editor) {
        PropertyValue value = property.value; // Make a copy
        if (editor->Render(valuePos, valueSize, value, property.metadata)) {
            // Value was modified, update it
            const_cast<Property&>(property).value = value;
            if (property.metadata.onChange) {
                property.metadata.onChange(value);
            }
        }
    }
    else if (property.type == PropertyType::Custom && property.metadata.customRenderer) {
        // Use custom renderer
        property.metadata.customRenderer(std::get<void*>(property.value));
    }
}

void UIPropertyGrid::RenderCategory(PropertyCategory& category) {
    auto renderer = UIRenderer::Get();
    if (!renderer) return;

    const float categoryHeaderHeight = 25.0f;
    const float padding = 5.0f;

    // Draw category header
    glm::vec2 headerPos = m_Position;
    glm::vec2 headerSize(m_Size.x, categoryHeaderHeight);
    renderer->DrawRect(headerPos, headerSize, glm::vec4(0.25f, 0.25f, 0.25f, 1.0f));

    // Draw category name
    glm::vec2 namePos = headerPos + glm::vec2(padding, 0.0f);
    renderer->DrawText(category.name, namePos, glm::vec4(1.0f));

    // Draw expand/collapse arrow
    // TODO: Implement expand/collapse UI

    // If expanded, render properties
    if (category.isExpanded) {
        float propertyY = headerPos.y + categoryHeaderHeight;
        for (const auto& property : category.properties) {
            if (MatchesSearch(property)) {
                RenderProperty(property);
                propertyY += 25.0f; // Property height
            }
        }
    }
}

PropertyCategory& UIPropertyGrid::GetOrCreateCategory(const std::string& name) {
    auto it = std::find_if(m_Categories.begin(), m_Categories.end(),
        [&name](const PropertyCategory& cat) { return cat.name == name; });

    if (it != m_Categories.end()) {
        return *it;
    }

    PropertyCategory category;
    category.name = name;
    m_Categories.push_back(category);
    return m_Categories.back();
}

bool UIPropertyGrid::MatchesSearch(const Property& property) const {
    if (m_SearchText.empty()) return true;

    // Case-insensitive search in name and display name
    auto searchLower = m_SearchText;
    std::transform(searchLower.begin(), searchLower.end(), searchLower.begin(), ::tolower);

    auto nameLower = property.name;
    std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);

    auto displayNameLower = property.displayName;
    std::transform(displayNameLower.begin(), displayNameLower.end(), displayNameLower.begin(), ::tolower);

    return nameLower.find(searchLower) != std::string::npos ||
           displayNameLower.find(searchLower) != std::string::npos;
}

}} // namespace VGE::UI 