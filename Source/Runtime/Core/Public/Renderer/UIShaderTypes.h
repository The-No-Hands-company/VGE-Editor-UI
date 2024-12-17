#pragma once

#include "Runtime/Core/Public/Core/UICore.h"

namespace VGE {
namespace UI {

enum class UIShaderDataType {
    None = 0,
    Float,
    Float2,
    Float3,
    Float4,
    Int,
    Int2,
    Int3,
    Int4,
    Bool,
    Mat3,
    Mat4
};

static uint32_t GetShaderDataTypeSize(UIShaderDataType type) {
    switch (type) {
        case UIShaderDataType::Float:    return 4;
        case UIShaderDataType::Float2:   return 4 * 2;
        case UIShaderDataType::Float3:   return 4 * 3;
        case UIShaderDataType::Float4:   return 4 * 4;
        case UIShaderDataType::Int:      return 4;
        case UIShaderDataType::Int2:     return 4 * 2;
        case UIShaderDataType::Int3:     return 4 * 3;
        case UIShaderDataType::Int4:     return 4 * 4;
        case UIShaderDataType::Bool:     return 1;
        case UIShaderDataType::Mat3:     return 4 * 3 * 3;
        case UIShaderDataType::Mat4:     return 4 * 4 * 4;
        default:                         return 0;
    }
}

struct BufferElement {
    std::string Name;
    UIShaderDataType Type;
    uint32_t Size;
    uint32_t Offset;
    bool Normalized;

    BufferElement() = default;

    BufferElement(UIShaderDataType type, const std::string& name, bool normalized = false)
        : Name(name), Type(type), Size(GetShaderDataTypeSize(type)), Offset(0), Normalized(normalized)
    {
    }

    uint32_t GetComponentCount() const {
        switch (Type) {
            case UIShaderDataType::Float:   return 1;
            case UIShaderDataType::Float2:  return 2;
            case UIShaderDataType::Float3:  return 3;
            case UIShaderDataType::Float4:  return 4;
            case UIShaderDataType::Int:     return 1;
            case UIShaderDataType::Int2:    return 2;
            case UIShaderDataType::Int3:    return 3;
            case UIShaderDataType::Int4:    return 4;
            case UIShaderDataType::Bool:    return 1;
            case UIShaderDataType::Mat3:    return 3 * 3;
            case UIShaderDataType::Mat4:    return 4 * 4;
            default:                        return 0;
        }
    }
};

class BufferLayout {
public:
    BufferLayout() {}

    BufferLayout(const std::initializer_list<BufferElement>& elements)
        : m_Elements(elements)
    {
        CalculateOffsetsAndStride();
    }

    uint32_t GetStride() const { return m_Stride; }
    const std::vector<BufferElement>& GetElements() const { return m_Elements; }

    std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
    std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
    std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
    std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }

private:
    void CalculateOffsetsAndStride() {
        uint32_t offset = 0;
        m_Stride = 0;
        for (auto& element : m_Elements) {
            element.Offset = offset;
            offset += element.Size;
            m_Stride += element.Size;
        }
    }

private:
    std::vector<BufferElement> m_Elements;
    uint32_t m_Stride = 0;
};

}} // namespace VGE::UI 