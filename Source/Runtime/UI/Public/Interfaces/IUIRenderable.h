#pragma once

namespace VGE {
namespace UI {

class IUIRenderable {
public:
    virtual ~IUIRenderable() = default;
    virtual void Render() = 0;
    virtual void PreRender() = 0;
    virtual void PostRender() = 0;
};

}} // namespace VGE::UI



