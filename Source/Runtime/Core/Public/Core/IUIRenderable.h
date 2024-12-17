#pragma once

#include "Runtime/Core/Public/Core/UICore.h"

namespace VGE {
namespace UI {

class UI_API IUIRenderable {
public:
    virtual ~IUIRenderable() = default;

    virtual void Render() = 0;
    virtual void PreRender() = 0;
    virtual void OnRender() = 0;
    virtual void PostRender() = 0;
};

}} // namespace VGE::UI



