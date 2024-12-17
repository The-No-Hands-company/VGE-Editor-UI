#include "Runtime/Core/Public/Renderer/UIVertexArray.h"
#include "Runtime/Core/Public/Renderer/UIShaderTypes.h"
#include <glad/gl.h>

namespace VGE {
namespace UI {

static GLenum ShaderDataTypeToOpenGLBaseType(UIShaderDataType type) {
    switch (type) {
        case UIShaderDataType::Float:    return GL_FLOAT;
        case UIShaderDataType::Float2:   return GL_FLOAT;
        case UIShaderDataType::Float3:   return GL_FLOAT;
        case UIShaderDataType::Float4:   return GL_FLOAT;
        case UIShaderDataType::Int:      return GL_INT;
        case UIShaderDataType::Int2:     return GL_INT;
        case UIShaderDataType::Int3:     return GL_INT;
        case UIShaderDataType::Int4:     return GL_INT;
        case UIShaderDataType::Bool:     return GL_BOOL;
        case UIShaderDataType::Mat3:     return GL_FLOAT;
        case UIShaderDataType::Mat4:     return GL_FLOAT;
        default:                         return GL_NONE;
    }
}

UIVertexArray::UIVertexArray() {
    glGenVertexArrays(1, &m_RendererID);
}

UIVertexArray::~UIVertexArray() {
    glDeleteVertexArrays(1, &m_RendererID);
}

void UIVertexArray::Bind() const {
    glBindVertexArray(m_RendererID);
}

void UIVertexArray::Unbind() const {
    glBindVertexArray(0);
}

void UIVertexArray::AddVertexBuffer(const std::shared_ptr<UIVertexBuffer>& vertexBuffer) {
    glBindVertexArray(m_RendererID);
    vertexBuffer->Bind();

    const auto& layout = vertexBuffer->GetLayout();
    for (const auto& element : layout) {
        switch (element.Type) {
            case UIShaderDataType::Float:
            case UIShaderDataType::Float2:
            case UIShaderDataType::Float3:
            case UIShaderDataType::Float4: {
                glEnableVertexAttribArray(m_VertexBufferIndex);
                glVertexAttribPointer(m_VertexBufferIndex,
                    element.GetComponentCount(),
                    ShaderDataTypeToOpenGLBaseType(element.Type),
                    element.Normalized ? GL_TRUE : GL_FALSE,
                    layout.GetStride(),
                    (const void*)element.Offset);
                m_VertexBufferIndex++;
                break;
            }
            case UIShaderDataType::Int:
            case UIShaderDataType::Int2:
            case UIShaderDataType::Int3:
            case UIShaderDataType::Int4:
            case UIShaderDataType::Bool: {
                glEnableVertexAttribArray(m_VertexBufferIndex);
                glVertexAttribIPointer(m_VertexBufferIndex,
                    element.GetComponentCount(),
                    ShaderDataTypeToOpenGLBaseType(element.Type),
                    layout.GetStride(),
                    (const void*)element.Offset);
                m_VertexBufferIndex++;
                break;
            }
            case UIShaderDataType::Mat3:
            case UIShaderDataType::Mat4: {
                uint8_t count = element.GetComponentCount();
                for (uint8_t i = 0; i < count; i++) {
                    glEnableVertexAttribArray(m_VertexBufferIndex);
                    glVertexAttribPointer(m_VertexBufferIndex,
                        count,
                        ShaderDataTypeToOpenGLBaseType(element.Type),
                        element.Normalized ? GL_TRUE : GL_FALSE,
                        layout.GetStride(),
                        (const void*)(element.Offset + sizeof(float) * count * i));
                    glVertexAttribDivisor(m_VertexBufferIndex, 1);
                    m_VertexBufferIndex++;
                }
                break;
            }
            default:
                break;
        }
    }

    m_VertexBuffers.push_back(vertexBuffer);
}

void UIVertexArray::SetIndexBuffer(const std::shared_ptr<UIIndexBuffer>& indexBuffer) {
    glBindVertexArray(m_RendererID);
    indexBuffer->Bind();

    m_IndexBuffer = indexBuffer;
}

}} // namespace VGE::UI
