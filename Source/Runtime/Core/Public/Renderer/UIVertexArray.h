#pragma once

#include "Runtime/Core/Public/Core/UICore.h"
#include "Runtime/Core/Public/Renderer/UIVertexBuffer.h"
#include "Runtime/Core/Public/Renderer/UIIndexBuffer.h"
#include <memory>
#include <vector>

namespace VGE {
namespace UI {

class UI_API UIVertexArray {
public:
    UIVertexArray();
    ~UIVertexArray();

    void Bind() const;
    void Unbind() const;

    void AddVertexBuffer(const std::shared_ptr<UIVertexBuffer>& vertexBuffer);
    void SetIndexBuffer(const std::shared_ptr<UIIndexBuffer>& indexBuffer);

    const std::vector<std::shared_ptr<UIVertexBuffer>>& GetVertexBuffers() const { return m_VertexBuffers; }
    const std::shared_ptr<UIIndexBuffer>& GetIndexBuffer() const { return m_IndexBuffer; }

    uint32_t GetRendererID() const { return m_RendererID; }

private:
    uint32_t m_RendererID;
    uint32_t m_VertexBufferIndex = 0;
    std::vector<std::shared_ptr<UIVertexBuffer>> m_VertexBuffers;
    std::shared_ptr<UIIndexBuffer> m_IndexBuffer;
};

}} // namespace VGE::UI 

