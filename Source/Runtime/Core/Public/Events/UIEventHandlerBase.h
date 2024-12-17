#pragma once

#include "Runtime/Core/Public/IUIEventHandler.h"
#include "Runtime/Core/Public/Core/UICore.h"
#include <glm/glm.hpp>

namespace VGE {
namespace UI {

class UI_API UIEventHandlerBase : public IUIEventHandler {
public:
    virtual ~UIEventHandlerBase() = default;

    // IUIEventHandler implementation
    virtual bool HandleMouseMove(const glm::vec2& position) override {
        return OnMouseMove(position);
    }

    virtual bool HandleMouseButton(int button, bool pressed, const glm::vec2& position) override {
        return OnMouseButton(button, pressed, position);
    }

    virtual bool HandleMouseScroll(float xoffset, float yoffset) override {
        return OnMouseScroll(xoffset, yoffset);
    }

    virtual bool HandleKeyInput(int key, int scancode, int action, int mods) override {
        return OnKeyInput(key, scancode, action, mods);
    }

    virtual bool HandleCharInput(unsigned int codepoint) override {
        return OnCharInput(codepoint);
    }

protected:
    virtual bool OnMouseMove(const glm::vec2& position) { return false; }
    virtual bool OnMouseButton(int button, bool pressed, const glm::vec2& position) { return false; }
    virtual bool OnMouseScroll(float xoffset, float yoffset) { return false; }
    virtual bool OnKeyInput(int key, int scancode, int action, int mods) { return false; }
    virtual bool OnCharInput(unsigned int codepoint) { return false; }
};

}} // namespace VGE::UI



