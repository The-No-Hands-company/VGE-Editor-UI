#pragma once

#include "Runtime/Core/Public/Core/UICore.h"
#include <string>
#include <memory>
#include <unordered_map>
#include <functional>
#include <any>
#include <typeindex>

namespace VGE {
namespace UI {

/**
 * @brief Property attributes for customizing property behavior and display
 */
struct PropertyAttributes {
    bool readOnly = false;
    bool hidden = false;
    std::string category;
    std::string tooltip;
    std::string displayName;
    std::any defaultValue;
    std::any minValue;
    std::any maxValue;
    std::vector<std::any> enumValues;
    std::function<bool(const std::any&)> validator;
};

/**
 * @brief Property descriptor containing type information and access methods
 */
class PropertyDescriptor {
public:
    using GetterFn = std::function<std::any(const void*)>;
    using SetterFn = std::function<void(void*, const std::any&)>;

    PropertyDescriptor(
        std::type_index type,
        GetterFn getter,
        SetterFn setter,
        PropertyAttributes attributes = {})
        : m_Type(type)
        , m_Getter(getter)
        , m_Setter(setter)
        , m_Attributes(attributes)
    {}

    std::type_index GetType() const { return m_Type; }
    const PropertyAttributes& GetAttributes() const { return m_Attributes; }
    
    std::any GetValue(const void* instance) const {
        return m_Getter(instance);
    }
    
    void SetValue(void* instance, const std::any& value) const {
        if (!m_Attributes.readOnly && (!m_Attributes.validator || m_Attributes.validator(value))) {
            m_Setter(instance, value);
        }
    }

private:
    std::type_index m_Type;
    GetterFn m_Getter;
    SetterFn m_Setter;
    PropertyAttributes m_Attributes;
};

/**
 * @brief Type descriptor containing property information for a type
 */
class TypeDescriptor {
public:
    void AddProperty(const std::string& name, std::shared_ptr<PropertyDescriptor> property) {
        m_Properties[name] = property;
    }

    const std::unordered_map<std::string, std::shared_ptr<PropertyDescriptor>>& GetProperties() const {
        return m_Properties;
    }

    std::shared_ptr<PropertyDescriptor> GetProperty(const std::string& name) const {
        auto it = m_Properties.find(name);
        return it != m_Properties.end() ? it->second : nullptr;
    }

private:
    std::unordered_map<std::string, std::shared_ptr<PropertyDescriptor>> m_Properties;
};

/**
 * @brief Registry for type reflection information
 */
class TypeRegistry {
public:
    static TypeRegistry& Get() {
        static TypeRegistry instance;
        return instance;
    }

    template<typename T>
    void RegisterType(const std::string& name) {
        m_Types[std::type_index(typeid(T))] = std::make_shared<TypeDescriptor>();
        m_TypeNames[std::type_index(typeid(T))] = name;
    }

    template<typename T, typename P>
    void RegisterProperty(
        const std::string& name,
        P T::* member,
        PropertyAttributes attributes = {}) {
        auto type = std::type_index(typeid(T));
        auto it = m_Types.find(type);
        if (it == m_Types.end()) {
            RegisterType<T>(typeid(T).name());
            it = m_Types.find(type);
        }

        auto property = std::make_shared<PropertyDescriptor>(
            std::type_index(typeid(P)),
            [member](const void* instance) -> std::any {
                return static_cast<const T*>(instance)->*member;
            },
            [member](void* instance, const std::any& value) {
                static_cast<T*>(instance)->*member = std::any_cast<P>(value);
            },
            attributes
        );

        it->second->AddProperty(name, property);
    }

    std::shared_ptr<TypeDescriptor> GetType(std::type_index type) const {
        auto it = m_Types.find(type);
        return it != m_Types.end() ? it->second : nullptr;
    }

    std::string GetTypeName(std::type_index type) const {
        auto it = m_TypeNames.find(type);
        return it != m_TypeNames.end() ? it->second : "";
    }

private:
    TypeRegistry() = default;
    std::unordered_map<std::type_index, std::shared_ptr<TypeDescriptor>> m_Types;
    std::unordered_map<std::type_index, std::string> m_TypeNames;
};

// Helper macros for property registration
#define REGISTER_TYPE(Type) \
    VGE::UI::TypeRegistry::Get().RegisterType<Type>(#Type)

#define REGISTER_PROPERTY(Type, Property, ...) \
    VGE::UI::TypeRegistry::Get().RegisterProperty<Type>( \
        #Property, \
        &Type::Property, \
        VGE::UI::PropertyAttributes{__VA_ARGS__})

}}} // namespace VGE::UI 