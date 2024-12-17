#include "Runtime/Core/Public/Renderer/UIBuffer.h"

namespace VGE {
namespace UI {

UIBuffer::UIBuffer()
    : m_RendererID(0)
{
}

UIBuffer::UIBuffer(uint32_t size)
    : m_RendererID(0)
{
    glGenBuffers(1, &m_RendererID);
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
}

UIBuffer::UIBuffer(const void* data, uint32_t size)
    : m_RendererID(0)
{
    glGenBuffers(1, &m_RendererID);
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

UIBuffer::~UIBuffer() {
    if (m_RendererID) {
        glDeleteBuffers(1, &m_RendererID);
    }
}

}} // namespace VGE::UI
