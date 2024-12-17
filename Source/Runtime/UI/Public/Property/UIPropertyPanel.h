#pragma once

#include "Runtime/Core/Public/Core/UICore.h"
#include "Property/UIPropertyReflection.h"
#include "Property/UIPropertyEditor.h"
#include "Property/UIPropertyValidator.h"
#include "Property/UIPropertyUndo.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace VGE {
namespace UI {

/**
 * @brief Property panel widget for editing object properties
 */
class UI_API UIPropertyPanel : public UIWidget {
public:
    UIPropertyPanel();
    virtual ~UIPropertyPanel();

    // Target object management
    void SetTarget(void* target, const std::string& typeName);
    void ClearTarget();
    void* GetTarget() const { return m_Target; }
    
    // Property access
    std::any GetPropertyValue(const std::string& name) const;
    bool SetPropertyValue(const std::string& name, const std::any& value);
    
    // Property customization
    void SetPropertyAttributes(const std::string& name, const PropertyAttributes& attributes);
    void SetCustomEditor(const std::string& name, std::shared_ptr<UIPropertyEditor> editor);
    void SetValidator(const std::string& name, std::shared_ptr<UIPropertyValidator> validator);
    
    // Categories
    void SetCategory(const std::string& name, const std::string& category);
    void SetCategoryExpanded(const std::string& category, bool expanded);
    bool IsCategoryExpanded(const std::string& category) const;
    
    // Search/Filter
    void SetSearchText(const std::string& text);
    void SetFilter(std::function<bool(const std::string&, const PropertyDescriptor&)> filter);
    
    // Undo/Redo
    void Undo();
    void Redo();
    void BeginBatchEdit();
    void EndBatchEdit();
    
    // Widget interface
    void Update(float deltaTime) override;
    void Render() override;

private:
    void RefreshProperties();
    void RenderProperty(const std::string& name, const PropertyDescriptor& property);
    void RenderCategory(const std::string& category);
    bool ShouldShowProperty(const std::string& name, const PropertyDescriptor& property) const;
    
    struct CategoryInfo {
        bool expanded = true;
        std::vector<std::string> properties;
    };

private:
    void* m_Target;
    std::shared_ptr<TypeDescriptor> m_TypeDescriptor;
    std::unordered_map<std::string, PropertyAttributes> m_CustomAttributes;
    std::unordered_map<std::string, std::shared_ptr<UIPropertyEditor>> m_CustomEditors;
    std::unordered_map<std::string, std::shared_ptr<UIPropertyValidator>> m_Validators;
    std::unordered_map<std::string, CategoryInfo> m_Categories;
    std::string m_SearchText;
    std::function<bool(const std::string&, const PropertyDescriptor&)> m_Filter;
    std::unique_ptr<UIPropertyUndoSystem> m_UndoSystem;
    bool m_BatchEditInProgress = false;
};

}} // namespace VGE::UI 