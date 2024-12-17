#pragma once

#include "Runtime/Core/Public/Core/UICore.h"
#include <glad/gl.h>

namespace VGE {
namespace UI {

enum class UIShaderDataType {
    None = 0,
    Float,
    Float2,
    Float3,
    Float4,
    Mat3,
    Mat4,
    Int,
    Int2,
    Int3,
    Int4,
    Bool
};

static uint32_t UIShaderDataTypeSize(UIShaderDataType type) {
    switch (type) {
        case UIShaderDataType::Float:    return 4;
        case UIShaderDataType::Float2:   return 4 * 2;
        case UIShaderDataType::Float3:   return 4 * 3;
        case UIShaderDataType::Float4:   return 4 * 4;
        case UIShaderDataType::Mat3:     return 4 * 3 * 3;
        case UIShaderDataType::Mat4:     return 4 * 4 * 4;
        case UIShaderDataType::Int:      return 4;
        case UIShaderDataType::Int2:     return 4 * 2;
        case UIShaderDataType::Int3:     return 4 * 3;
        case UIShaderDataType::Int4:     return 4 * 4;
        case UIShaderDataType::Bool:     return 1;
        case UIShaderDataType::None:
        default:
            return 0;
    }
}

static uint32_t UIShaderDataTypeComponentCount(UIShaderDataType type) {
    switch (type) {
        case UIShaderDataType::Float:    return 1;
        case UIShaderDataType::Float2:   return 2;
        case UIShaderDataType::Float3:   return 3;
        case UIShaderDataType::Float4:   return 4;
        case UIShaderDataType::Mat3:     return 3 * 3;
        case UIShaderDataType::Mat4:     return 4 * 4;
        case UIShaderDataType::Int:      return 1;
        case UIShaderDataType::Int2:     return 2;
        case UIShaderDataType::Int3:     return 3;
        case UIShaderDataType::Int4:     return 4;
        case UIShaderDataType::Bool:     return 1;
        case UIShaderDataType::None:
        default:
            return 0;
    }
}

static uint32_t UIShaderDataTypeToOpenGLBaseType(UIShaderDataType type) {
    switch (type) {
        case UIShaderDataType::Float:
        case UIShaderDataType::Float2:
        case UIShaderDataType::Float3:
        case UIShaderDataType::Float4:
        case UIShaderDataType::Mat3:
        case UIShaderDataType::Mat4:
            return GL_FLOAT;
        case UIShaderDataType::Int:
        case UIShaderDataType::Int2:
        case UIShaderDataType::Int3:
        case UIShaderDataType::Int4:
            return GL_INT;
        case UIShaderDataType::Bool:
            return GL_BOOL;
        case UIShaderDataType::None:
        default:
            return 0;
    }
}

}} // namespace VGE::UI
