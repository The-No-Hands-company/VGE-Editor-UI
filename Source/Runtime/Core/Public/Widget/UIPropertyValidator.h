#pragma once

#include "Runtime/Core/Public/Core/UITypes.h"
#include <string>
#include <functional>
#include <vector>

namespace VGE {
namespace UI {

// Base class for property validators
class UI_API UIPropertyValidator {
public:
    virtual ~UIPropertyValidator() = default;
    
    // Validation interface
    virtual bool Validate(const PropertyValue& value) const = 0;
    virtual std::string GetErrorMessage() const = 0;
    
    // Helper methods
    static bool IsNumeric(const PropertyValue& value);
    static bool IsString(const PropertyValue& value);
    static bool IsVector(const PropertyValue& value);
    static bool IsCollection(const PropertyValue& value);
};

// Standard validators
class UI_API UIRangeValidator : public UIPropertyValidator {
public:
    UIRangeValidator(double min, double max);
    
    bool Validate(const PropertyValue& value) const override;
    std::string GetErrorMessage() const override;
    
private:
    double m_Min;
    double m_Max;
};

class UI_API UIStringLengthValidator : public UIPropertyValidator {
public:
    UIStringLengthValidator(size_t minLength, size_t maxLength);
    
    bool Validate(const PropertyValue& value) const override;
    std::string GetErrorMessage() const override;
    
private:
    size_t m_MinLength;
    size_t m_MaxLength;
};

class UI_API UIRegexValidator : public UIPropertyValidator {
public:
    UIRegexValidator(const std::string& pattern);
    
    bool Validate(const PropertyValue& value) const override;
    std::string GetErrorMessage() const override;
    
private:
    std::string m_Pattern;
};

class UI_API UICustomValidator : public UIPropertyValidator {
public:
    using ValidatorFunction = std::function<bool(const PropertyValue&)>;
    
    UICustomValidator(ValidatorFunction validator, const std::string& errorMessage);
    
    bool Validate(const PropertyValue& value) const override;
    std::string GetErrorMessage() const override;
    
private:
    ValidatorFunction m_Validator;
    std::string m_ErrorMessage;
};

class UI_API UICollectionValidator : public UIPropertyValidator {
public:
    UICollectionValidator(size_t minSize, size_t maxSize, std::shared_ptr<UIPropertyValidator> elementValidator = nullptr);
    
    bool Validate(const PropertyValue& value) const override;
    std::string GetErrorMessage() const override;
    
private:
    size_t m_MinSize;
    size_t m_MaxSize;
    std::shared_ptr<UIPropertyValidator> m_ElementValidator;
};

class UI_API UICompositeValidator : public UIPropertyValidator {
public:
    void AddValidator(std::shared_ptr<UIPropertyValidator> validator);
    void RemoveValidator(std::shared_ptr<UIPropertyValidator> validator);
    void ClearValidators();
    
    bool Validate(const PropertyValue& value) const override;
    std::string GetErrorMessage() const override;
    
private:
    std::vector<std::shared_ptr<UIPropertyValidator>> m_Validators;
    mutable std::string m_LastError;
};

}} // namespace VGE::UI 