#pragma once

namespace VGE {
namespace UI {

class UIEvent;

class IUIEventHandler {
public:
    virtual ~IUIEventHandler() = default;
    virtual bool HandleEvent(const UIEvent& event) = 0;
    virtual void OnEvent(const UIEvent& event) = 0;
    virtual bool CanHandle(const UIEvent& event) const = 0;
};

}} // namespace VGE::UI



