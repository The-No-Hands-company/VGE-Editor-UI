#include "Runtime/Core/Public/Widget/UIPropertyValidator.h"
#include "Runtime/Core/Public/Core/Logger.h"
#include <regex>

namespace VGE {
namespace UI {

// Helper methods implementation
bool UIPropertyValidator::IsNumeric(const PropertyValue& value) {
    return std::holds_alternative<int32_t>(value) || 
           std::holds_alternative<float>(value);
}

bool UIPropertyValidator::IsString(const PropertyValue& value) {
    return std::holds_alternative<std::string>(value);
}

bool UIPropertyValidator::IsVector(const PropertyValue& value) {
    return std::holds_alternative<glm::vec2>(value) || 
           std::holds_alternative<glm::vec3>(value) || 
           std::holds_alternative<glm::vec4>(value);
}

bool UIPropertyValidator::IsCollection(const PropertyValue& value) {
    return std::holds_alternative<std::vector<std::any>>(value);
}

// Range Validator Implementation
UIRangeValidator::UIRangeValidator(double min, double max) 
    : m_Min(min), m_Max(max) {}

bool UIRangeValidator::Validate(const PropertyValue& value) const {
    if (!IsNumeric(value)) return false;
    
    double numValue;
    if (auto* intVal = std::get_if<int32_t>(&value)) {
        numValue = static_cast<double>(*intVal);
    } else if (auto* floatVal = std::get_if<float>(&value)) {
        numValue = static_cast<double>(*floatVal);
    } else {
        return false;
    }
    
    return numValue >= m_Min && numValue <= m_Max;
}

std::string UIRangeValidator::GetErrorMessage() const {
    return "Value must be between " + std::to_string(m_Min) + 
           " and " + std::to_string(m_Max);
}

// String Length Validator Implementation
UIStringLengthValidator::UIStringLengthValidator(size_t minLength, size_t maxLength)
    : m_MinLength(minLength), m_MaxLength(maxLength) {}

bool UIStringLengthValidator::Validate(const PropertyValue& value) const {
    if (!IsString(value)) return false;
    
    const auto& str = std::get<std::string>(value);
    return str.length() >= m_MinLength && str.length() <= m_MaxLength;
}

std::string UIStringLengthValidator::GetErrorMessage() const {
    return "String length must be between " + std::to_string(m_MinLength) + 
           " and " + std::to_string(m_MaxLength) + " characters";
}

// Regex Validator Implementation
UIRegexValidator::UIRegexValidator(const std::string& pattern)
    : m_Pattern(pattern) {}

bool UIRegexValidator::Validate(const PropertyValue& value) const {
    if (!IsString(value)) return false;
    
    const auto& str = std::get<std::string>(value);
    try {
        std::regex regex(m_Pattern);
        return std::regex_match(str, regex);
    } catch (const std::regex_error&) {
        return false;
    }
}

std::string UIRegexValidator::GetErrorMessage() const {
    return "Value must match pattern: " + m_Pattern;
}

// Custom Validator Implementation
UICustomValidator::UICustomValidator(ValidatorFunction validator, const std::string& errorMessage)
    : m_Validator(validator), m_ErrorMessage(errorMessage) {}

bool UICustomValidator::Validate(const PropertyValue& value) const {
    return m_Validator ? m_Validator(value) : true;
}

std::string UICustomValidator::GetErrorMessage() const {
    return m_ErrorMessage;
}

// Collection Validator Implementation
UICollectionValidator::UICollectionValidator(size_t minSize, size_t maxSize, 
                                           std::shared_ptr<UIPropertyValidator> elementValidator)
    : m_MinSize(minSize), m_MaxSize(maxSize), m_ElementValidator(elementValidator) {}

bool UICollectionValidator::Validate(const PropertyValue& value) const {
    if (!IsCollection(value)) return false;
    
    const auto& collection = std::get<std::vector<std::any>>(value);
    if (collection.size() < m_MinSize || collection.size() > m_MaxSize) {
        return false;
    }
    
    if (m_ElementValidator) {
        for (const auto& element : collection) {
            if (!m_ElementValidator->Validate(PropertyValue(element))) {
                return false;
            }
        }
    }
    
    return true;
}

std::string UICollectionValidator::GetErrorMessage() const {
    return "Collection size must be between " + std::to_string(m_MinSize) + 
           " and " + std::to_string(m_MaxSize);
}

// Composite Validator Implementation
void UICompositeValidator::AddValidator(std::shared_ptr<UIPropertyValidator> validator) {
    if (validator) {
        m_Validators.push_back(validator);
    }
}

void UICompositeValidator::RemoveValidator(std::shared_ptr<UIPropertyValidator> validator) {
    auto it = std::find(m_Validators.begin(), m_Validators.end(), validator);
    if (it != m_Validators.end()) {
        m_Validators.erase(it);
    }
}

void UICompositeValidator::ClearValidators() {
    m_Validators.clear();
}

bool UICompositeValidator::Validate(const PropertyValue& value) const {
    for (const auto& validator : m_Validators) {
        if (!validator->Validate(value)) {
            m_LastError = validator->GetErrorMessage();
            return false;
        }
    }
    return true;
}

std::string UICompositeValidator::GetErrorMessage() const {
    return m_LastError;
}

}} // namespace VGE::UI 