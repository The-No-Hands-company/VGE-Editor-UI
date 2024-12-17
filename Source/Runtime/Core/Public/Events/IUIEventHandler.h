#pragma once

#include "Runtime/Core/Public/Core/UICore.h"
#include <glm/glm.hpp>

namespace VGE {
namespace UI {

class UI_API IUIEventHandler {
public:
    virtual ~IUIEventHandler() = default;

    virtual bool HandleMouseMove(const glm::vec2& position) = 0;
    virtual bool HandleMouseButton(int button, bool pressed, const glm::vec2& position) = 0;
    virtual bool HandleMouseScroll(float xoffset, float yoffset) = 0;
    virtual bool HandleKeyInput(int key, int scancode, int action, int mods) = 0;
    virtual bool HandleCharInput(unsigned int codepoint) = 0;
};

}} // namespace VGE::UI



