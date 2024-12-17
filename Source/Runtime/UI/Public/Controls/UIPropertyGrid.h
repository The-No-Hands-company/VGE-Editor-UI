#pragma once

#include "Runtime/Core/Public/Core/UICore.h"
#include "Runtime/Core/Public/Widget/UIWidget.h"
#include "Runtime/UI/Public/Controls/UIPropertyEditors.h"
#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace VGE {
namespace UI {

/**
 * @brief Property category for grouping properties
 */
struct PropertyCategory {
    std::string name;
    bool isExpanded;
    std::vector<class PropertyItem*> items;
};

/**
 * @brief Base class for property items
 */
class PropertyItem {
public:
    virtual ~PropertyItem() = default;
    virtual void Render() = 0;
    virtual bool IsModified() const = 0;
    virtual void Reset() = 0;
    virtual void Apply() = 0;

    const std::string& GetName() const { return m_Name; }
    const std::string& GetDescription() const { return m_Description; }
    bool IsEnabled() const { return m_Enabled; }
    void SetEnabled(bool enabled) { m_Enabled = enabled; }

protected:
    std::string m_Name;
    std::string m_Description;
    bool m_Enabled = true;
};

/**
 * @brief Property grid widget for editing object properties
 */
class UIPropertyGrid : public UIWidget {
public:
    UIPropertyGrid();
    virtual ~UIPropertyGrid() = default;

    // Property management
    void AddProperty(const std::string& category, PropertyItem* item);
    void RemoveProperty(PropertyItem* item);
    void ClearProperties();
    
    // Category management
    void ExpandCategory(const std::string& name);
    void CollapseCategory(const std::string& name);
    void ExpandAll();
    void CollapseAll();

    // Property access
    const std::vector<PropertyCategory>& GetCategories() const { return m_Categories; }
    PropertyCategory* FindCategory(const std::string& name);
    PropertyItem* FindProperty(const std::string& name);

    // Modification state
    bool HasModifiedProperties() const;
    void ApplyModifiedProperties();
    void ResetModifiedProperties();

    // Widget interface
    void Render() override;
    void Update(float deltaTime) override;

protected:
    // Event handlers
    virtual void OnPropertyAdded(PropertyItem* item);
    virtual void OnPropertyRemoved(PropertyItem* item);
    virtual void OnPropertyModified(PropertyItem* item);
    virtual void OnCategoryExpanded(PropertyCategory* category);
    virtual void OnCategoryCollapsed(PropertyCategory* category);

private:
    // Helper functions
    void RenderCategory(PropertyCategory& category);
    void RenderProperty(PropertyItem* item);
    PropertyCategory& GetOrCreateCategory(const std::string& name);
    void NotifyPropertyModified(PropertyItem* item);

private:
    std::vector<PropertyCategory> m_Categories;
    float m_LabelWidth;
    bool m_AutoExpand;
};

}} // namespace VGE::UI 