#pragma once

#include "Runtime/Core/Public/Core/UICore.h"
#include <glad/gl.h>

namespace VGE {
namespace UI {

class UI_API UIBuffer {
public:
    UIBuffer();
    UIBuffer(uint32_t size);
    UIBuffer(const void* data, uint32_t size);
    virtual ~UIBuffer();

    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;

protected:
    uint32_t m_RendererID;
};

}} // namespace VGE::UI
