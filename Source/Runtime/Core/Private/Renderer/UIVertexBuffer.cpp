#include "Runtime/Core/Public/Renderer/UIVertexBuffer.h"
#include <glad/gl.h>

namespace VGE {
namespace UI {

UIVertexBuffer::UIVertexBuffer(uint32_t size)
    : UIBuffer(size)
{
    // Buffer is already created by base class, just bind it as a vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
}

UIVertexBuffer::UIVertexBuffer(const void* vertices, uint32_t size)
    : UIBuffer(vertices, size)
{
    // Buffer is already created by base class, just bind it as a vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
}

void UIVertexBuffer::Bind() const {
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
}

void UIVertexBuffer::Unbind() const {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void UIVertexBuffer::SetData(const void* data, uint32_t size) {
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
}

}} // namespace VGE::UI 