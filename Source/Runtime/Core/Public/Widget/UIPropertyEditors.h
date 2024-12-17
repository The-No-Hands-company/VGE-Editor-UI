#pragma once

#include "Core/UICore.h"
#include "Base/UIWidget.h"
#include "UIPropertyGrid.h"
#include <functional>

namespace VGE {
namespace UI {

/**
 * @brief Base class for property editors
 */
class UIPropertyEditor {
public:
    virtual ~UIPropertyEditor() = default;

    /**
     * @brief Renders the editor
     * @param position Editor position
     * @param size Editor size
     * @param value Current value
     * @param metadata Property metadata
     * @return True if value was modified
     */
    virtual bool Render(const glm::vec2& position, const glm::vec2& size,
                       PropertyValue& value, const PropertyMetadata& metadata) = 0;
};

/**
 * @brief Editor for boolean properties
 */
class UIBoolEditor : public UIPropertyEditor {
public:
    bool Render(const glm::vec2& position, const glm::vec2& size,
                PropertyValue& value, const PropertyMetadata& metadata) override;
};

/**
 * @brief Editor for numeric properties (int/float)
 */
class UINumericEditor : public UIPropertyEditor {
public:
    bool Render(const glm::vec2& position, const glm::vec2& size,
                PropertyValue& value, const PropertyMetadata& metadata) override;
};

/**
 * @brief Editor for string properties
 */
class UIStringEditor : public UIPropertyEditor {
public:
    bool Render(const glm::vec2& position, const glm::vec2& size,
                PropertyValue& value, const PropertyMetadata& metadata) override;
};

/**
 * @brief Editor for vector properties
 */
class UIVectorEditor : public UIPropertyEditor {
public:
    bool Render(const glm::vec2& position, const glm::vec2& size,
                PropertyValue& value, const PropertyMetadata& metadata) override;
};

/**
 * @brief Editor for color properties
 */
class UIColorEditor : public UIPropertyEditor {
public:
    bool Render(const glm::vec2& position, const glm::vec2& size,
                PropertyValue& value, const PropertyMetadata& metadata) override;
};

/**
 * @brief Editor for enum properties
 */
class UIEnumEditor : public UIPropertyEditor {
public:
    bool Render(const glm::vec2& position, const glm::vec2& size,
                PropertyValue& value, const PropertyMetadata& metadata) override;
};

/**
 * @brief Editor for asset properties
 */
class UIAssetEditor : public UIPropertyEditor {
public:
    bool Render(const glm::vec2& position, const glm::vec2& size,
                PropertyValue& value, const PropertyMetadata& metadata) override;
};

/**
 * @brief Factory for creating property editors
 */
class UIPropertyEditorFactory {
public:
    /**
     * @brief Gets the editor for a property type
     * @param type Property type
     * @return Editor instance
     */
    static std::shared_ptr<UIPropertyEditor> GetEditor(PropertyType type);

private:
    static std::unordered_map<PropertyType, std::shared_ptr<UIPropertyEditor>> s_Editors;
};

}} // namespace VGE::UI 