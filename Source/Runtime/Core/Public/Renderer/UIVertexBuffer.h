#pragma once

#include "Runtime/Core/Public/Core/UICore.h"
#include "Runtime/Core/Public/Renderer/UIShaderTypes.h"
#include "Runtime/Core/Public/Renderer/UIBuffer.h"

namespace VGE {
namespace UI {

// Forward declarations
class UIVertexArray;

// Vertex buffer
class UI_API UIVertexBuffer : public UIBuffer {
public:
    UIVertexBuffer(uint32_t size);
    UIVertexBuffer(const void* vertices, uint32_t size);
    virtual ~UIVertexBuffer() = default;

    virtual void Bind() const override;
    virtual void Unbind() const override;

    void SetData(const void* data, uint32_t size);
    void SetLayout(const BufferLayout& layout) { m_Layout = layout; }
    const BufferLayout& GetLayout() const { return m_Layout; }

private:
    BufferLayout m_Layout;
};

}} // namespace VGE::UI
