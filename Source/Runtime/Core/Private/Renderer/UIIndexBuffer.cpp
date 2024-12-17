#include "Runtime/Core/Public/Renderer/UIIndexBuffer.h"
#include <glad/gl.h>

namespace VGE {
namespace UI {

UIIndexBuffer::UIIndexBuffer(const uint32_t* indices, uint32_t count)
    : UIBuffer(indices, count * sizeof(uint32_t)), m_Count(count)
{
    // Buffer is already created by base class, just bind it as an element buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
}

UIIndexBuffer::~UIIndexBuffer() = default;

void UIIndexBuffer::Bind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
}

void UIIndexBuffer::Unbind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void UIIndexBuffer::SetData(const uint32_t* indices, uint32_t count) {
    m_Count = count;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
}

}} // namespace VGE::UI
