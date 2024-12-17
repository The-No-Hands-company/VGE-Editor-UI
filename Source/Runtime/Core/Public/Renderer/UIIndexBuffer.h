#pragma once

#include "Runtime/Core/Public/Core/UICore.h"
#include "Runtime/Core/Public/Renderer/UIBuffer.h"

namespace VGE {
namespace UI {

// Index buffer
class UI_API UIIndexBuffer : public UIBuffer {
public:
    UIIndexBuffer(const uint32_t* indices, uint32_t count);
    virtual ~UIIndexBuffer();

    virtual void Bind() const override;
    virtual void Unbind() const override;
    void SetData(const uint32_t* indices, uint32_t count);

    uint32_t GetCount() const { return m_Count; }

private:
    uint32_t m_Count = 0;
};

}} // namespace VGE::UI
