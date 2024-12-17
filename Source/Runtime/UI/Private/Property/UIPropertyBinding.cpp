#include "Property/UIPropertyBinding.h"
#include "Property/UIPropertyReflection.h"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <glm/glm.hpp>

namespace VGE {
namespace UI {

void UIPropertyBindingManager::CreateBinding(
    void* source,
    const std::string& sourcePath,
    void* target,
    const std::string& targetPath,
    BindingMode mode) {
    
    BindingInstance instance;
    instance.binding.sourcePath = sourcePath;
    instance.binding.targetPath = targetPath;
    instance.binding.mode = mode;
    instance.source = source;
    instance.target = target;
    instance.dirty = true;
    
    m_Bindings.push_back(instance);
    
    // Initial update
    if (mode != BindingMode::OneWayToSource) {
        UpdateBinding(instance);
    }
}

void UIPropertyBindingManager::CreateExpressionBinding(
    void* target,
    const std::string& targetPath,
    std::shared_ptr<UIPropertyExpression> expression) {
    
    BindingInstance instance;
    instance.binding.targetPath = targetPath;
    instance.binding.mode = BindingMode::OneWay;
    instance.binding.converter = expression;
    instance.source = nullptr;
    instance.target = target;
    instance.dirty = true;
    
    m_Bindings.push_back(instance);
    
    // Initial update
    UpdateBinding(instance);
}

void UIPropertyBindingManager::RemoveBindings(void* object) {
    m_Bindings.erase(
        std::remove_if(m_Bindings.begin(), m_Bindings.end(),
            [object](const BindingInstance& binding) {
                return binding.source == object || binding.target == object;
            }),
        m_Bindings.end()
    );
}

void UIPropertyBindingManager::RemoveBinding(
    void* source,
    const std::string& sourcePath,
    void* target,
    const std::string& targetPath) {
    
    m_Bindings.erase(
        std::remove_if(m_Bindings.begin(), m_Bindings.end(),
            [&](const BindingInstance& binding) {
                return binding.source == source &&
                       binding.target == target &&
                       binding.binding.sourcePath == sourcePath &&
                       binding.binding.targetPath == targetPath;
            }),
        m_Bindings.end()
    );
}

void UIPropertyBindingManager::ClearAllBindings() {
    m_Bindings.clear();
}

void UIPropertyBindingManager::RegisterConverter(
    const std::type_index& sourceType,
    const std::type_index& targetType,
    std::shared_ptr<UIPropertyExpression> converter) {
    
    TypePair pair{sourceType, targetType};
    m_Converters[pair] = converter;
}

void UIPropertyBindingManager::RegisterValidator(
    const std::type_index& type,
    std::shared_ptr<UIPropertyExpression> validator) {
    
    m_Validators[type] = validator;
}

void UIPropertyBindingManager::NotifyPropertyChanged(void* source, const std::string& path) {
    for (auto& binding : m_Bindings) {
        if (binding.source == source && binding.binding.sourcePath == path) {
            binding.dirty = true;
        }
        else if (binding.target == source && binding.binding.targetPath == path &&
                 binding.binding.mode == BindingMode::TwoWay) {
            // For two-way bindings, propagate changes back to source
            auto targetType = TypeRegistry::Get().GetType(std::type_index(typeid(*static_cast<void*>(binding.target))));
            auto sourceType = TypeRegistry::Get().GetType(std::type_index(typeid(*static_cast<void*>(binding.source))));
            
            if (targetType && sourceType) {
                auto targetProperty = targetType->GetProperty(binding.binding.targetPath);
                auto sourceProperty = sourceType->GetProperty(binding.binding.sourcePath);
                
                if (targetProperty && sourceProperty) {
                    auto value = targetProperty->GetValue(binding.target);
                    value = ConvertValue(value, targetProperty->GetType(), sourceProperty->GetType());
                    
                    if (ValidateValue(value, sourceProperty->GetType())) {
                        sourceProperty->SetValue(binding.source, value);
                    }
                }
            }
        }
    }
}

void UIPropertyBindingManager::UpdateBindings() {
    for (auto& binding : m_Bindings) {
        if (binding.dirty) {
            UpdateBinding(binding);
            binding.dirty = false;
        }
    }
}

void UIPropertyBindingManager::UpdateBinding(BindingInstance& binding) {
    // Handle expression bindings
    if (!binding.source && binding.binding.converter) {
        auto targetType = TypeRegistry::Get().GetType(std::type_index(typeid(*static_cast<void*>(binding.target))));
        if (targetType) {
            auto targetProperty = targetType->GetProperty(binding.binding.targetPath);
            if (targetProperty) {
                std::unordered_map<std::string, std::any> variables;
                auto value = binding.binding.converter->Evaluate(variables);
                
                if (ValidateValue(value, targetProperty->GetType())) {
                    targetProperty->SetValue(binding.target, value);
                }
            }
        }
        return;
    }
    
    // Handle regular bindings
    if (!binding.source || !binding.target) return;
    
    auto sourceType = TypeRegistry::Get().GetType(std::type_index(typeid(*static_cast<void*>(binding.source))));
    auto targetType = TypeRegistry::Get().GetType(std::type_index(typeid(*static_cast<void*>(binding.target))));
    
    if (!sourceType || !targetType) return;
    
    auto sourceProperty = sourceType->GetProperty(binding.binding.sourcePath);
    auto targetProperty = targetType->GetProperty(binding.binding.targetPath);
    
    if (!sourceProperty || !targetProperty) return;
    
    // Get source value
    auto value = sourceProperty->GetValue(binding.source);
    
    // Apply converter if any
    if (binding.binding.converter) {
        std::unordered_map<std::string, std::any> variables;
        variables["value"] = value;
        value = binding.binding.converter->Evaluate(variables);
    }
    else {
        value = ConvertValue(value, sourceProperty->GetType(), targetProperty->GetType());
    }
    
    // Validate value
    if (!ValidateValue(value, targetProperty->GetType())) return;
    
    // Apply custom validator if any
    if (binding.binding.validator) {
        std::unordered_map<std::string, std::any> variables;
        variables["value"] = value;
        if (!std::any_cast<bool>(binding.binding.validator->Evaluate(variables))) {
            return;
        }
    }
    
    // Set target value
    targetProperty->SetValue(binding.target, value);
}

std::any UIPropertyBindingManager::ConvertValue(
    const std::any& value,
    const std::type_index& sourceType,
    const std::type_index& targetType) {
    
    // If types match, no conversion needed
    if (sourceType == targetType) return value;
    
    // Look for registered converter
    TypePair pair{sourceType, targetType};
    auto it = m_Converters.find(pair);
    if (it != m_Converters.end()) {
        std::unordered_map<std::string, std::any> variables;
        variables["value"] = value;
        return it->second->Evaluate(variables);
    }
    
    // Built-in numeric conversions
    try {
        // Integer conversions
        if (sourceType == typeid(int)) {
            int intVal = std::any_cast<int>(value);
            if (targetType == typeid(float)) return static_cast<float>(intVal);
            if (targetType == typeid(double)) return static_cast<double>(intVal);
            if (targetType == typeid(bool)) return intVal != 0;
            if (targetType == typeid(std::string)) return std::to_string(intVal);
        }
        
        // Float conversions
        if (sourceType == typeid(float)) {
            float floatVal = std::any_cast<float>(value);
            if (targetType == typeid(int)) return static_cast<int>(floatVal);
            if (targetType == typeid(double)) return static_cast<double>(floatVal);
            if (targetType == typeid(bool)) return floatVal != 0.0f;
            if (targetType == typeid(std::string)) {
                std::stringstream ss;
                ss << std::fixed << std::setprecision(3) << floatVal;
                return ss.str();
            }
        }
        
        // Double conversions
        if (sourceType == typeid(double)) {
            double doubleVal = std::any_cast<double>(value);
            if (targetType == typeid(int)) return static_cast<int>(doubleVal);
            if (targetType == typeid(float)) return static_cast<float>(doubleVal);
            if (targetType == typeid(bool)) return doubleVal != 0.0;
            if (targetType == typeid(std::string)) {
                std::stringstream ss;
                ss << std::fixed << std::setprecision(3) << doubleVal;
                return ss.str();
            }
        }
        
        // Boolean conversions
        if (sourceType == typeid(bool)) {
            bool boolVal = std::any_cast<bool>(value);
            if (targetType == typeid(int)) return boolVal ? 1 : 0;
            if (targetType == typeid(float)) return boolVal ? 1.0f : 0.0f;
            if (targetType == typeid(double)) return boolVal ? 1.0 : 0.0;
            if (targetType == typeid(std::string)) return boolVal ? "true" : "false";
        }
        
        // String conversions
        if (sourceType == typeid(std::string)) {
            const std::string& strVal = std::any_cast<const std::string&>(value);
            if (targetType == typeid(int)) return std::stoi(strVal);
            if (targetType == typeid(float)) return std::stof(strVal);
            if (targetType == typeid(double)) return std::stod(strVal);
            if (targetType == typeid(bool)) return strVal == "true" || strVal == "1";
        }
        
        // Vector conversions
        if (sourceType == typeid(glm::vec2)) {
            const glm::vec2& vec2Val = std::any_cast<const glm::vec2&>(value);
            if (targetType == typeid(glm::vec3)) return glm::vec3(vec2Val, 0.0f);
            if (targetType == typeid(glm::vec4)) return glm::vec4(vec2Val, 0.0f, 1.0f);
            if (targetType == typeid(std::string)) {
                std::stringstream ss;
                ss << std::fixed << std::setprecision(3) << "(" << vec2Val.x << ", " << vec2Val.y << ")";
                return ss.str();
            }
        }
        
        if (sourceType == typeid(glm::vec3)) {
            const glm::vec3& vec3Val = std::any_cast<const glm::vec3&>(value);
            if (targetType == typeid(glm::vec2)) return glm::vec2(vec3Val);
            if (targetType == typeid(glm::vec4)) return glm::vec4(vec3Val, 1.0f);
            if (targetType == typeid(std::string)) {
                std::stringstream ss;
                ss << std::fixed << std::setprecision(3) 
                   << "(" << vec3Val.x << ", " << vec3Val.y << ", " << vec3Val.z << ")";
                return ss.str();
            }
        }
        
        if (sourceType == typeid(glm::vec4)) {
            const glm::vec4& vec4Val = std::any_cast<const glm::vec4&>(value);
            if (targetType == typeid(glm::vec2)) return glm::vec2(vec4Val);
            if (targetType == typeid(glm::vec3)) return glm::vec3(vec4Val);
            if (targetType == typeid(std::string)) {
                std::stringstream ss;
                ss << std::fixed << std::setprecision(3) 
                   << "(" << vec4Val.x << ", " << vec4Val.y << ", " 
                   << vec4Val.z << ", " << vec4Val.w << ")";
                return ss.str();
            }
        }
        
        // Color conversions
        if (sourceType == typeid(glm::vec3) && targetType == typeid(glm::vec4)) {
            const glm::vec3& colorRGB = std::any_cast<const glm::vec3&>(value);
            return glm::vec4(colorRGB, 1.0f);
        }
        
        if (sourceType == typeid(glm::vec4) && targetType == typeid(glm::vec3)) {
            const glm::vec4& colorRGBA = std::any_cast<const glm::vec4&>(value);
            return glm::vec3(colorRGBA);
        }
        
        // Enum conversions
        if (sourceType == typeid(int) && targetType.hash_code() > typeid(int).hash_code()) {
            // Assume target type is an enum if its hash is greater than int's hash
            return value; // Pass through the integer value
        }
        
        if (sourceType.hash_code() > typeid(int).hash_code() && targetType == typeid(int)) {
            // Convert enum to int
            return value; // Pass through the integer value
        }
    }
    catch (const std::exception&) {
        // Conversion failed, return original value
        return value;
    }
    
    return value;
}

bool UIPropertyBindingManager::ValidateValue(const std::any& value, const std::type_index& type) {
    // Look for registered validator
    auto it = m_Validators.find(type);
    if (it != m_Validators.end()) {
        std::unordered_map<std::string, std::any> variables;
        variables["value"] = value;
        return std::any_cast<bool>(it->second->Evaluate(variables));
    }
    
    return true;
}

void UIPropertyBindingManager::CreateCollectionBinding(
    void* source,
    const std::string& sourcePath,
    void* target,
    const std::string& targetPath,
    const CollectionBindingOptions& options) {
    
    CollectionBindingInstance instance;
    instance.binding.sourcePath = sourcePath;
    instance.binding.targetPath = targetPath;
    instance.binding.options = options;
    instance.source = source;
    instance.target = target;
    
    m_CollectionBindings.push_back(instance);
    
    // Initial sync
    if (options.mode != CollectionBindingMode::OneWayToSource) {
        CollectionChangeEvent event{CollectionChangeType::Reset};
        UpdateCollectionBinding(instance, event);
    }
}

void UIPropertyBindingManager::RemoveCollectionBinding(
    void* source,
    const std::string& sourcePath,
    void* target,
    const std::string& targetPath) {
    
    m_CollectionBindings.erase(
        std::remove_if(m_CollectionBindings.begin(), m_CollectionBindings.end(),
            [&](const CollectionBindingInstance& binding) {
                return binding.source == source &&
                       binding.target == target &&
                       binding.binding.sourcePath == sourcePath &&
                       binding.binding.targetPath == targetPath;
            }),
        m_CollectionBindings.end()
    );
}

void UIPropertyBindingManager::NotifyCollectionChanged(
    void* source,
    const std::string& path,
    const CollectionChangeEvent& event) {
    
    for (auto& binding : m_CollectionBindings) {
        if (binding.source == source && binding.binding.sourcePath == path) {
            if (binding.binding.options.mode != CollectionBindingMode::OneWayToSource) {
                UpdateCollectionBinding(binding, event);
            }
        }
        else if (binding.target == source && binding.binding.targetPath == path &&
                 binding.binding.options.mode == CollectionBindingMode::TwoWay) {
            PropagateCollectionChange(binding, event);
        }
    }
}

void UIPropertyBindingManager::RegisterCollectionConverter(
    const std::type_index& sourceType,
    const std::type_index& targetType,
    std::shared_ptr<UIPropertyExpression> converter) {
    
    TypePair pair{sourceType, targetType};
    m_CollectionConverters[pair] = converter;
}

void UIPropertyBindingManager::RegisterCollectionValidator(
    const std::type_index& type,
    std::shared_ptr<UIPropertyExpression> validator) {
    
    m_CollectionValidators[type] = validator;
}

void UIPropertyBindingManager::UpdateCollectionBinding(
    CollectionBindingInstance& binding,
    const CollectionChangeEvent& event) {
    
    auto sourceType = TypeRegistry::Get().GetType(std::type_index(typeid(*static_cast<void*>(binding.source))));
    auto targetType = TypeRegistry::Get().GetType(std::type_index(typeid(*static_cast<void*>(binding.target))));
    
    if (!sourceType || !targetType) return;
    
    auto sourceProperty = sourceType->GetProperty(binding.binding.sourcePath);
    auto targetProperty = targetType->GetProperty(binding.binding.targetPath);
    
    if (!sourceProperty || !targetProperty) return;
    
    // Handle different collection change types
    switch (event.type) {
        case CollectionChangeType::Add:
            if (binding.binding.options.updateTargetOnAdd) {
                auto convertedItem = ConvertCollectionItem(
                    event.item,
                    sourceProperty->GetType(),
                    targetProperty->GetType()
                );
                if (ValidateCollectionItem(convertedItem, targetProperty->GetType())) {
                    // Add item to target collection
                    auto targetCollection = targetProperty->GetValue(binding.target);
                    if (targetCollection.type() == typeid(std::vector<std::any>)) {
                        auto& vec = std::any_cast<std::vector<std::any>&>(targetCollection);
                        if (event.index <= vec.size()) {
                            vec.insert(vec.begin() + event.index, convertedItem);
                            targetProperty->SetValue(binding.target, targetCollection);
                        }
                    }
                }
            }
            break;
            
        case CollectionChangeType::Remove:
            if (binding.binding.options.updateTargetOnRemove) {
                // Remove item from target collection
                auto targetCollection = targetProperty->GetValue(binding.target);
                if (targetCollection.type() == typeid(std::vector<std::any>)) {
                    auto& vec = std::any_cast<std::vector<std::any>&>(targetCollection);
                    if (event.index < vec.size()) {
                        vec.erase(vec.begin() + event.index);
                        targetProperty->SetValue(binding.target, targetCollection);
                    }
                }
            }
            break;
            
        case CollectionChangeType::Replace:
            if (binding.binding.options.updateTargetOnReplace) {
                auto convertedItem = ConvertCollectionItem(
                    event.newItem,
                    sourceProperty->GetType(),
                    targetProperty->GetType()
                );
                if (ValidateCollectionItem(convertedItem, targetProperty->GetType())) {
                    // Replace item in target collection
                    auto targetCollection = targetProperty->GetValue(binding.target);
                    if (targetCollection.type() == typeid(std::vector<std::any>)) {
                        auto& vec = std::any_cast<std::vector<std::any>&>(targetCollection);
                        if (event.index < vec.size()) {
                            vec[event.index] = convertedItem;
                            targetProperty->SetValue(binding.target, targetCollection);
                        }
                    }
                }
            }
            break;
            
        case CollectionChangeType::Move:
            if (binding.binding.options.updateTargetOnMove) {
                // Move item in target collection
                auto targetCollection = targetProperty->GetValue(binding.target);
                if (targetCollection.type() == typeid(std::vector<std::any>)) {
                    auto& vec = std::any_cast<std::vector<std::any>&>(targetCollection);
                    if (event.index < vec.size() && event.newIndex < vec.size()) {
                        auto item = vec[event.index];
                        vec.erase(vec.begin() + event.index);
                        vec.insert(vec.begin() + event.newIndex, item);
                        targetProperty->SetValue(binding.target, targetCollection);
                    }
                }
            }
            break;
            
        case CollectionChangeType::Reset:
            if (binding.binding.options.updateTargetOnReset) {
                // Get source collection
                auto sourceCollection = sourceProperty->GetValue(binding.source);
                if (sourceCollection.type() == typeid(std::vector<std::any>)) {
                    const auto& sourceVec = std::any_cast<const std::vector<std::any>&>(sourceCollection);
                    
                    // Convert and validate all items
                    std::vector<std::any> targetVec;
                    targetVec.reserve(sourceVec.size());
                    
                    for (const auto& item : sourceVec) {
                        auto convertedItem = ConvertCollectionItem(
                            item,
                            sourceProperty->GetType(),
                            targetProperty->GetType()
                        );
                        if (ValidateCollectionItem(convertedItem, targetProperty->GetType())) {
                            targetVec.push_back(convertedItem);
                        }
                    }
                    
                    // Set target collection
                    targetProperty->SetValue(binding.target, targetVec);
                }
            }
            break;
    }
}

void UIPropertyBindingManager::PropagateCollectionChange(
    CollectionBindingInstance& binding,
    const CollectionChangeEvent& event) {
    
    auto sourceType = TypeRegistry::Get().GetType(std::type_index(typeid(*static_cast<void*>(binding.source))));
    auto targetType = TypeRegistry::Get().GetType(std::type_index(typeid(*static_cast<void*>(binding.target))));
    
    if (!sourceType || !targetType) return;
    
    auto sourceProperty = sourceType->GetProperty(binding.binding.sourcePath);
    auto targetProperty = targetType->GetProperty(binding.binding.targetPath);
    
    if (!sourceProperty || !targetProperty) return;
    
    // Create reverse event
    CollectionChangeEvent reverseEvent = event;
    
    // Convert items if needed
    if (event.type == CollectionChangeType::Add || event.type == CollectionChangeType::Replace) {
        reverseEvent.item = ConvertCollectionItem(
            event.item,
            targetProperty->GetType(),
            sourceProperty->GetType()
        );
        if (event.type == CollectionChangeType::Replace) {
            reverseEvent.newItem = ConvertCollectionItem(
                event.newItem,
                targetProperty->GetType(),
                sourceProperty->GetType()
            );
        }
    }
    
    // Update source collection
    switch (event.type) {
        case CollectionChangeType::Add:
            if (binding.binding.options.updateSourceOnAdd) {
                auto sourceCollection = sourceProperty->GetValue(binding.source);
                if (sourceCollection.type() == typeid(std::vector<std::any>)) {
                    auto& vec = std::any_cast<std::vector<std::any>&>(sourceCollection);
                    if (event.index <= vec.size() && 
                        ValidateCollectionItem(reverseEvent.item, sourceProperty->GetType())) {
                        vec.insert(vec.begin() + event.index, reverseEvent.item);
                        sourceProperty->SetValue(binding.source, sourceCollection);
                    }
                }
            }
            break;
            
        case CollectionChangeType::Remove:
            if (binding.binding.options.updateSourceOnRemove) {
                auto sourceCollection = sourceProperty->GetValue(binding.source);
                if (sourceCollection.type() == typeid(std::vector<std::any>)) {
                    auto& vec = std::any_cast<std::vector<std::any>&>(sourceCollection);
                    if (event.index < vec.size()) {
                        vec.erase(vec.begin() + event.index);
                        sourceProperty->SetValue(binding.source, sourceCollection);
                    }
                }
            }
            break;
            
        case CollectionChangeType::Replace:
            if (binding.binding.options.updateSourceOnReplace) {
                auto sourceCollection = sourceProperty->GetValue(binding.source);
                if (sourceCollection.type() == typeid(std::vector<std::any>)) {
                    auto& vec = std::any_cast<std::vector<std::any>&>(sourceCollection);
                    if (event.index < vec.size() && 
                        ValidateCollectionItem(reverseEvent.newItem, sourceProperty->GetType())) {
                        vec[event.index] = reverseEvent.newItem;
                        sourceProperty->SetValue(binding.source, sourceCollection);
                    }
                }
            }
            break;
            
        case CollectionChangeType::Move:
            if (binding.binding.options.updateSourceOnMove) {
                auto sourceCollection = sourceProperty->GetValue(binding.source);
                if (sourceCollection.type() == typeid(std::vector<std::any>)) {
                    auto& vec = std::any_cast<std::vector<std::any>&>(sourceCollection);
                    if (event.index < vec.size() && event.newIndex < vec.size()) {
                        auto item = vec[event.index];
                        vec.erase(vec.begin() + event.index);
                        vec.insert(vec.begin() + event.newIndex, item);
                        sourceProperty->SetValue(binding.source, sourceCollection);
                    }
                }
            }
            break;
            
        case CollectionChangeType::Reset:
            if (binding.binding.options.updateSourceOnReset) {
                auto targetCollection = targetProperty->GetValue(binding.target);
                if (targetCollection.type() == typeid(std::vector<std::any>)) {
                    const auto& targetVec = std::any_cast<const std::vector<std::any>&>(targetCollection);
                    
                    std::vector<std::any> sourceVec;
                    sourceVec.reserve(targetVec.size());
                    
                    for (const auto& item : targetVec) {
                        auto convertedItem = ConvertCollectionItem(
                            item,
                            targetProperty->GetType(),
                            sourceProperty->GetType()
                        );
                        if (ValidateCollectionItem(convertedItem, sourceProperty->GetType())) {
                            sourceVec.push_back(convertedItem);
                        }
                    }
                    
                    sourceProperty->SetValue(binding.source, sourceVec);
                }
            }
            break;
    }
}

std::any UIPropertyBindingManager::ConvertCollectionItem(
    const std::any& item,
    const std::type_index& sourceType,
    const std::type_index& targetType) {
    
    // If types match, no conversion needed
    if (sourceType == targetType) return item;
    
    // Look for registered converter
    TypePair pair{sourceType, targetType};
    auto it = m_CollectionConverters.find(pair);
    if (it != m_CollectionConverters.end()) {
        std::unordered_map<std::string, std::any> variables;
        variables["value"] = item;
        return it->second->Evaluate(variables);
    }
    
    // Use regular value converter as fallback
    return ConvertValue(item, sourceType, targetType);
}

bool UIPropertyBindingManager::ValidateCollectionItem(
    const std::any& item,
    const std::type_index& type) {
    
    // Look for registered validator
    auto it = m_CollectionValidators.find(type);
    if (it != m_CollectionValidators.end()) {
        std::unordered_map<std::string, std::any> variables;
        variables["value"] = item;
        return std::any_cast<bool>(it->second->Evaluate(variables));
    }
    
    // Use regular value validator as fallback
    return ValidateValue(item, type);
}

}} // namespace VGE::UI 