#pragma once

#include "Runtime/Core/Public/Core/UICore.h"
#include "Property/UIPropertyEditor.h"
#include "Theme/UIStyle.h"
#include <memory>
#include <vector>
#include <string>
#include <functional>

namespace VGE {
namespace UI {

/**
 * @brief Base class for array property editors
 */
class UIArrayEditor : public UIPropertyEditor {
public:
    using CreateElementFn = std::function<std::any()>;
    using CustomizeElementFn = std::function<void(const std::string&, std::any&)>;
    
    UIArrayEditor(
        std::shared_ptr<UIPropertyEditor> elementEditor,
        CreateElementFn createFn = nullptr,
        CustomizeElementFn customizeFn = nullptr)
        : m_ElementEditor(elementEditor)
        , m_CreateElement(createFn)
        , m_CustomizeElement(customizeFn)
    {}
    
    bool OnGUI(const std::string& name, std::any& value) override;

protected:
    std::shared_ptr<UIPropertyEditor> m_ElementEditor;
    CreateElementFn m_CreateElement;
    CustomizeElementFn m_CustomizeElement;
};

/**
 * @brief Editor for enum types
 */
class UIEnumEditor : public UIPropertyEditor {
public:
    using GetNamesFn = std::function<std::vector<std::string>()>;
    using GetValueFn = std::function<int(const std::string&)>;
    
    UIEnumEditor(GetNamesFn getNames, GetValueFn getValue)
        : m_GetNames(getNames)
        , m_GetValue(getValue)
    {}
    
    bool OnGUI(const std::string& name, std::any& value) override;

private:
    GetNamesFn m_GetNames;
    GetValueFn m_GetValue;
};

/**
 * @brief Editor for flags (bit field enums)
 */
class UIFlagsEditor : public UIPropertyEditor {
public:
    using GetFlagsFn = std::function<std::vector<std::pair<std::string, int>>()>;
    
    UIFlagsEditor(GetFlagsFn getFlags)
        : m_GetFlags(getFlags)
    {}
    
    bool OnGUI(const std::string& name, std::any& value) override;

private:
    GetFlagsFn m_GetFlags;
};

/**
 * @brief Editor for color values
 */
class UIColorEditor : public UIPropertyEditor {
public:
    bool OnGUI(const std::string& name, std::any& value) override;
};

/**
 * @brief Editor for curve editing
 */
class UICurveEditor : public UIPropertyEditor {
public:
    bool OnGUI(const std::string& name, std::any& value) override;
};

/**
 * @brief Editor for gradients
 */
class UIGradientEditor : public UIPropertyEditor {
public:
    bool OnGUI(const std::string& name, std::any& value) override;
};

/**
 * @brief Editor for object references
 */
class UIObjectReferenceEditor : public UIPropertyEditor {
public:
    using FilterFn = std::function<bool(const std::string&)>;
    
    UIObjectReferenceEditor(const std::string& typeName, FilterFn filter = nullptr)
        : m_TypeName(typeName)
        , m_Filter(filter)
    {}
    
    bool OnGUI(const std::string& name, std::any& value) override;

private:
    std::string m_TypeName;
    FilterFn m_Filter;
};

/**
 * @brief Editor for asset references
 */
class UIAssetReferenceEditor : public UIPropertyEditor {
public:
    using FilterFn = std::function<bool(const std::string&)>;
    
    UIAssetReferenceEditor(const std::string& assetType, FilterFn filter = nullptr)
        : m_AssetType(assetType)
        , m_Filter(filter)
    {}
    
    bool OnGUI(const std::string& name, std::any& value) override;

private:
    std::string m_AssetType;
    FilterFn m_Filter;
};

/**
 * @brief Editor for path selection
 */
class UIPathEditor : public UIPropertyEditor {
public:
    enum class PathType {
        File,
        Directory,
        Both
    };
    
    UIPathEditor(PathType type = PathType::File, const std::string& filter = "")
        : m_PathType(type)
        , m_Filter(filter)
    {}
    
    bool OnGUI(const std::string& name, std::any& value) override;

private:
    PathType m_PathType;
    std::string m_Filter;
};

/**
 * @brief Editor for key bindings
 */
class UIKeyBindingEditor : public UIPropertyEditor {
public:
    bool OnGUI(const std::string& name, std::any& value) override;
};

/**
 * @brief Editor for text with syntax highlighting
 */
class UICodeEditor : public UIPropertyEditor {
public:
    UICodeEditor(const std::string& language = "")
        : m_Language(language)
    {}
    
    bool OnGUI(const std::string& name, std::any& value) override;

private:
    std::string m_Language;
};

}} // namespace VGE::UI 