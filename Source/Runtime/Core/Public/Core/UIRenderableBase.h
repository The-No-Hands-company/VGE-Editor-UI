#pragma once

#include "Runtime/Core/Public/IUIRenderable.h"
#include "Runtime/Core/Public/Core/UICore.h"

namespace VGE {
namespace UI {

class UI_API UIRenderableBase : public IUIRenderable {
public:
    virtual ~UIRenderableBase() = default;

    // IUIRenderable implementation
    virtual void Render() override {
        PreRender();
        OnRender();
        PostRender();
    }

    virtual void PreRender() override {
        // Default implementation
    }

    virtual void PostRender() override {
        // Default implementation
    }

protected:
    virtual void OnRender() = 0;
};

}} // namespace VGE::UI



