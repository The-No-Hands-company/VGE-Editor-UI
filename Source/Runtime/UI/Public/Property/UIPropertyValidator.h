#pragma once

#include "Runtime/Core/Public/Core/UICore.h"
#include "Runtime/UI/Public/Controls/UIPropertyGrid.h"
#include <string>
#include <functional>
#include <optional>

namespace VGE {
namespace Editor {

/**
 * @brief Result of a property validation
 */
struct ValidationResult {
    bool isValid = true;
    std::string message;
};

/**
 * @brief Base class for property validators
 */
class UIPropertyValidator {
public:
    virtual ~UIPropertyValidator() = default;
    virtual ValidationResult Validate(const PropertyValue& value) const = 0;
};

/**
 * @brief Validates numeric ranges
 */
template<typename T>
class UIRangeValidator : public UIPropertyValidator {
public:
    UIRangeValidator(T min, T max) : m_Min(min), m_Max(max) {}

    ValidationResult Validate(const PropertyValue& value) const override {
        try {
            T val = std::get<T>(value);
            if (val < m_Min || val > m_Max) {
                return { false, "Value must be between " + std::to_string(m_Min) + 
                               " and " + std::to_string(m_Max) };
            }
        }
        catch (const std::bad_variant_access&) {
            return { false, "Invalid value type" };
        }
        return { true, "" };
    }

private:
    T m_Min;
    T m_Max;
};

/**
 * @brief Validates strings using a custom function
 */
class UIStringValidator : public UIPropertyValidator {
public:
    using ValidatorFunc = std::function<ValidationResult(const std::string&)>;

    explicit UIStringValidator(ValidatorFunc validator) : m_Validator(std::move(validator)) {}

    ValidationResult Validate(const PropertyValue& value) const override {
        try {
            const std::string& str = std::get<std::string>(value);
            return m_Validator(str);
        }
        catch (const std::bad_variant_access&) {
            return { false, "Invalid value type" };
        }
    }

private:
    ValidatorFunc m_Validator;
};

/**
 * @brief Factory for creating common validators
 */
class UIPropertyValidatorFactory {
public:
    static std::unique_ptr<UIPropertyValidator> CreateRangeValidator(float min, float max) {
        return std::make_unique<UIRangeValidator<float>>(min, max);
    }

    static std::unique_ptr<UIPropertyValidator> CreateRangeValidator(int min, int max) {
        return std::make_unique<UIRangeValidator<int>>(min, max);
    }

    static std::unique_ptr<UIPropertyValidator> CreateStringLengthValidator(size_t minLength, size_t maxLength) {
        return std::make_unique<UIStringValidator>(
            [=](const std::string& str) -> ValidationResult {
                if (str.length() < minLength || str.length() > maxLength) {
                    return { false, "String length must be between " + std::to_string(minLength) +
                                  " and " + std::to_string(maxLength) };
                }
                return { true, "" };
            });
    }

    static std::unique_ptr<UIPropertyValidator> CreateRegexValidator(const std::string& pattern,
                                                                   const std::string& errorMessage) {
        return std::make_unique<UIStringValidator>(
            [=](const std::string& str) -> ValidationResult {
                try {
                    std::regex regex(pattern);
                    if (!std::regex_match(str, regex)) {
                        return { false, errorMessage };
                    }
                }
                catch (const std::regex_error&) {
                    return { false, "Invalid regex pattern" };
                }
                return { true, "" };
            });
    }
};

}} // namespace VGE::Editor 