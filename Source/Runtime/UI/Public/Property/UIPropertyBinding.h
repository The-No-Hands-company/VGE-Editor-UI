#pragma once

#include "Runtime/Core/Public/Core/UICore.h"
#include "Property/UIPropertyReflection.h"
#include <memory>
#include <string>
#include <functional>
#include <unordered_map>
#include <vector>

namespace VGE {
namespace UI {

/**
 * @brief Expression evaluator for property bindings
 */
class UIPropertyExpression {
public:
    using EvaluateFn = std::function<std::any(const std::unordered_map<std::string, std::any>&)>;
    
    UIPropertyExpression(const std::string& expression, EvaluateFn evaluator)
        : m_Expression(expression)
        , m_Evaluator(evaluator)
    {}
    
    std::any Evaluate(const std::unordered_map<std::string, std::any>& variables) const {
        return m_Evaluator(variables);
    }
    
    const std::string& GetExpression() const { return m_Expression; }

private:
    std::string m_Expression;
    EvaluateFn m_Evaluator;
};

/**
 * @brief Property binding mode
 */
enum class BindingMode {
    OneWay,         // Source -> Target
    TwoWay,         // Source <-> Target
    OneWayToSource, // Target -> Source
    OneTime         // Source -> Target (once)
};

/**
 * @brief Property binding definition
 */
struct PropertyBinding {
    std::string sourcePath;
    std::string targetPath;
    BindingMode mode = BindingMode::OneWay;
    std::shared_ptr<UIPropertyExpression> converter;
    std::shared_ptr<UIPropertyExpression> validator;
};

/**
 * @brief Collection change type
 */
enum class CollectionChangeType {
    Add,
    Remove,
    Replace,
    Move,
    Reset
};

/**
 * @brief Collection change event
 */
struct CollectionChangeEvent {
    CollectionChangeType type;
    size_t index;
    size_t newIndex;  // Used for Move
    std::any item;
    std::any newItem; // Used for Replace
};

/**
 * @brief Collection binding mode
 */
enum class CollectionBindingMode {
    OneWay,         // Source -> Target
    TwoWay,         // Source <-> Target
    OneWayToSource  // Target -> Source
};

/**
 * @brief Collection binding options
 */
struct CollectionBindingOptions {
    CollectionBindingMode mode = CollectionBindingMode::OneWay;
    bool updateTargetOnAdd = true;
    bool updateTargetOnRemove = true;
    bool updateTargetOnReplace = true;
    bool updateTargetOnMove = true;
    bool updateTargetOnReset = true;
    bool updateSourceOnAdd = true;
    bool updateSourceOnRemove = true;
    bool updateSourceOnReplace = true;
    bool updateSourceOnMove = true;
    bool updateSourceOnReset = true;
};

/**
 * @brief Collection binding definition
 */
struct CollectionBinding {
    std::string sourcePath;
    std::string targetPath;
    CollectionBindingOptions options;
    std::shared_ptr<UIPropertyExpression> itemConverter;
    std::shared_ptr<UIPropertyExpression> itemValidator;
};

/**
 * @brief Property binding manager
 */
class UI_API UIPropertyBindingManager {
public:
    static UIPropertyBindingManager& Get() {
        static UIPropertyBindingManager instance;
        return instance;
    }
    
    // Binding creation
    void CreateBinding(
        void* source,
        const std::string& sourcePath,
        void* target,
        const std::string& targetPath,
        BindingMode mode = BindingMode::OneWay);
    
    void CreateExpressionBinding(
        void* target,
        const std::string& targetPath,
        std::shared_ptr<UIPropertyExpression> expression);
    
    // Collection binding creation
    void CreateCollectionBinding(
        void* source,
        const std::string& sourcePath,
        void* target,
        const std::string& targetPath,
        const CollectionBindingOptions& options = CollectionBindingOptions());
    
    // Binding management
    void RemoveBindings(void* object);
    void RemoveBinding(void* source, const std::string& sourcePath, void* target, const std::string& targetPath);
    void ClearAllBindings();
    
    // Collection binding management
    void RemoveCollectionBinding(void* source, const std::string& sourcePath, void* target, const std::string& targetPath);
    
    // Value conversion
    void RegisterConverter(
        const std::type_index& sourceType,
        const std::type_index& targetType,
        std::shared_ptr<UIPropertyExpression> converter);
    
    // Validation
    void RegisterValidator(
        const std::type_index& type,
        std::shared_ptr<UIPropertyExpression> validator);
    
    // Collection item conversion
    void RegisterCollectionConverter(
        const std::type_index& sourceType,
        const std::type_index& targetType,
        std::shared_ptr<UIPropertyExpression> converter);
    
    // Collection item validation
    void RegisterCollectionValidator(
        const std::type_index& type,
        std::shared_ptr<UIPropertyExpression> validator);
    
    // Update handling
    void NotifyPropertyChanged(void* source, const std::string& path);
    void UpdateBindings();
    
    // Collection change notification
    void NotifyCollectionChanged(void* source, const std::string& path, const CollectionChangeEvent& event);

private:
    struct BindingInstance {
        PropertyBinding binding;
        void* source;
        void* target;
        bool dirty = false;
    };
    
    struct CollectionBindingInstance {
        CollectionBinding binding;
        void* source;
        void* target;
    };
    
    struct TypePair {
        std::type_index sourceType;
        std::type_index targetType;
        
        bool operator==(const TypePair& other) const {
            return sourceType == other.sourceType && targetType == other.targetType;
        }
    };
    
    struct TypePairHash {
        size_t operator()(const TypePair& pair) const {
            return std::hash<std::type_index>()(pair.sourceType) ^ std::hash<std::type_index>()(pair.targetType);
        }
    };

private:
    UIPropertyBindingManager() = default;
    void UpdateBinding(BindingInstance& binding);
    std::any ConvertValue(const std::any& value, const std::type_index& sourceType, const std::type_index& targetType);
    bool ValidateValue(const std::any& value, const std::type_index& type);
    
    // Collection binding helpers
    void UpdateCollectionBinding(CollectionBindingInstance& binding, const CollectionChangeEvent& event);
    void PropagateCollectionChange(CollectionBindingInstance& binding, const CollectionChangeEvent& event);
    std::any ConvertCollectionItem(const std::any& item, const std::type_index& sourceType, const std::type_index& targetType);
    bool ValidateCollectionItem(const std::any& item, const std::type_index& type);

private:
    std::vector<BindingInstance> m_Bindings;
    std::unordered_map<TypePair, std::shared_ptr<UIPropertyExpression>, TypePairHash> m_Converters;
    std::unordered_map<std::type_index, std::shared_ptr<UIPropertyExpression>> m_Validators;
    std::vector<CollectionBindingInstance> m_CollectionBindings;
    std::unordered_map<TypePair, std::shared_ptr<UIPropertyExpression>, TypePairHash> m_CollectionConverters;
    std::unordered_map<std::type_index, std::shared_ptr<UIPropertyExpression>> m_CollectionValidators;
};

// Helper functions
inline UIPropertyBindingManager& GetBindingManager() {
    return UIPropertyBindingManager::Get();
}

template<typename Source, typename Target>
void Bind(
    Source* source,
    const std::string& sourcePath,
    Target* target,
    const std::string& targetPath,
    BindingMode mode = BindingMode::OneWay) {
    GetBindingManager().CreateBinding(source, sourcePath, target, targetPath, mode);
}

template<typename Target>
void BindExpression(
    Target* target,
    const std::string& targetPath,
    std::shared_ptr<UIPropertyExpression> expression) {
    GetBindingManager().CreateExpressionBinding(target, targetPath, expression);
}

template<typename Source, typename Target>
void BindCollection(
    Source* source,
    const std::string& sourcePath,
    Target* target,
    const std::string& targetPath,
    const CollectionBindingOptions& options = CollectionBindingOptions()) {
    GetBindingManager().CreateCollectionBinding(source, sourcePath, target, targetPath, options);
}

}} // namespace VGE::UI 